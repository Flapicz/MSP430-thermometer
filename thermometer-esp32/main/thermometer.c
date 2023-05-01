#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_flash.h"
#include "esp_wifi.h"
#include <esp_wifi_types.h>
#include "nvs_flash.h"
#include "cJSON.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_system.h"


#define TAG "thermometer"
#define ESP_WIFI_SSID      "CCF7"
#define ESP_WIFI_PASS      "09876543211"


void post_data(double tempValue){
    cJSON * json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json,"tempValue", tempValue);
    char * mesage_payload = cJSON_Print(json);
    esp_http_client_config_t clientConfig = {
      .url = "http://frog01-20917.wykr.es",
      .method = HTTP_METHOD_POST
    };
    esp_http_client_handle_t client = esp_http_client_init(&clientConfig);
    esp_http_client_set_header(client,"CONTENT-TYPE","application/json");
    esp_http_client_set_post_field(client,mesage_payload, strlen((mesage_payload)));
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK){
        ESP_LOGE(TAG,"Successfully added record");
    }else{
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_close(client);
    cJSON_Delete(json);
    free(mesage_payload);
}

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
    switch (event_id){
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection ... \n");
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("WiFi got IP ... \n\n");
        break;
    default:
        break;
    }
}

#define I2C_SLAVE_SDA_IO GPIO_NUM_22
#define I2C_SLAVE_SCL_IO GPIO_NUM_21
#define I2C_SLAVE_MAX_SPEED 100000
#define I2C_SLAVE_NUM I2C_NUM_0
#define I2C_SLAVE_TX_BUF_LEN 256 
#define I2C_SLAVE_RX_BUF_LEN 256
#define ESP_SLAVE_ADDR 0x04


static esp_err_t i2c_slave_init() {
  i2c_config_t conf_slave;
  conf_slave.sda_io_num = I2C_SLAVE_SDA_IO;
  conf_slave.sda_pullup_en = GPIO_PULLUP_ENABLE;
  conf_slave.scl_io_num = I2C_SLAVE_SCL_IO;
  conf_slave.scl_pullup_en = GPIO_PULLUP_ENABLE;
  conf_slave.mode = I2C_MODE_SLAVE;
  conf_slave.slave.addr_10bit_en = 0;
  conf_slave.slave.slave_addr = ESP_SLAVE_ADDR;
  conf_slave.slave.maximum_speed = I2C_SLAVE_MAX_SPEED;
  i2c_param_config(I2C_SLAVE_NUM, &conf_slave);
  return i2c_driver_install(I2C_SLAVE_NUM, conf_slave.mode, I2C_SLAVE_RX_BUF_LEN, I2C_SLAVE_TX_BUF_LEN, 0);
}


void app_main(void){
    nvs_flash_init();

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); // 					                    
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            .scan_method = WIFI_ALL_CHANNEL_SCAN
        },
    };
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();
    ESP_LOGI(TAG, "SLAVE--------------------------------");
    ESP_ERROR_CHECK(i2c_slave_init());
    uint8_t  received_data[I2C_SLAVE_RX_BUF_LEN] = {0};
    while(1){
        int read = i2c_slave_read_buffer(I2C_SLAVE_NUM, received_data, I2C_SLAVE_RX_BUF_LEN, 1000 / portTICK_PERIOD_MS);
        if(read != 0){
            printf("---- Slave read: [%d] bytes ----\n", read);
            for (int i = 0; i < I2C_SLAVE_RX_BUF_LEN; i++) {
                printf("%d ", received_data[i]);
            }
            printf("\n");
            printf("--------------------------------\n");
            uint32_t end_result = received_data[0];
            for(int j=1 ; j < read; j++){
                end_result = (end_result<<8) | received_data[j];
            }
            printf("%ld in decimal \n ", end_result);
            post_data((int)end_result*0.02-0.18-273.15);
            i2c_reset_rx_fifo(I2C_SLAVE_NUM);
            memset(received_data, 0, I2C_SLAVE_RX_BUF_LEN);
        }
    }
}
