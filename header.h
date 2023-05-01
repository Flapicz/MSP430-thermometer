#ifndef TEMP_H_
#define TEMP_H_
#define SLAVE_ESP32_ADDR 0x04
#define SDA BIT4 //data - port 7.4
#define SCL BIT2 //clock - port 2.2

#define TIME 20 // Timeout_H podzielone przez 2
#define MEASURE_TIME 2000000 //przerwa pomiedzy pomiarami

void SendComm(unsigned char comm);
unsigned int ReadTemp(void);
void Start();
void Stop();
unsigned int ReadByte();


#endif /*TEMP_H_*/
