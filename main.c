#include <msp430.h>
#include <header.h>

//SDA - port 7.4
//SCL - port 2.2

unsigned long int wynik=0;
float wynik2 = 0;

unsigned char *PTxData;
unsigned char TXByteCtr;
unsigned char TxData[] = {0x00, 0x00, 0x00, 0x00};


void Start(){
    // bit startu
    P7DIR |= SDA; //ustaw port na output
    P7OUT |= SDA; //ustaw wyjœcie na 1
    __delay_cycles(TIME);
    P2OUT |= SCL; //ustaw zegar na 1
    __delay_cycles(TIME);
    P7OUT &= ~SDA; //output = 0;
    __delay_cycles(TIME);
    P2OUT &= ~SCL; //ustaw zegar na 0
    __delay_cycles(TIME);
}


void SendComm(unsigned char comm){
    volatile unsigned int i;
    unsigned char temp;

    P7DIR |= SDA; //ustaw port na output

    for (i=0;i<8;++i) { //dla ka¿dego bitu w bajcie
        temp = comm<<i; //temp = comm przesuniêty o i bitów w lewo
        temp &= 0x80; //temp AND 1000 0000 = wyzeruj wszystkie bity oprócz najbardziej znaczacego. Dziêki temu temp = bit i zmiennej comm

        if (temp) //jeœli temp = 1
            P7OUT |= SDA; //output = 1
        else
            P7OUT &= ~SDA; //output = 0
        __delay_cycles(TIME);
        P2OUT |= SCL; //ustaw zegar na 1
        __delay_cycles(TIME*2);
        P2OUT &= ~SCL; //ustaw zegar na 0
        __delay_cycles(TIME);
        }

    //odbierz ACK
    P7DIR &= ~SDA; //ustaw na input
    __delay_cycles(TIME);
    P2OUT |= SCL; //ustaw zegar na 1
    __delay_cycles(TIME);
    //tutaj sprawdŸ ACK
    __delay_cycles(TIME);
    P2OUT &= ~SCL; //ustaw zegar na 0
    __delay_cycles(TIME);
}


unsigned int ReadTemp(void) { //czytaj temperaturê
    unsigned int result = 0;
    result = ReadByte(); //przeczytaj pierwsze 8 bitow temperatury i wyœlij ACK
    result |= (ReadByte() << 8); //przeczytaj drugie 8 bitow temperatury i wyœlij ACK
    ReadByte(); //przeczytaj PEC i wyœlij ACK
    return result;
}


unsigned int ReadByte(){
    volatile unsigned int i;
    unsigned int temp = 0;

    //odbierz 8 bitów
    P7DIR &= ~SDA; //ustaw port na input

    for (i=8;i>0;--i) { //dla ka¿dego z 8 odbieranych bitów
        __delay_cycles(TIME);
        P2OUT |= SCL; //ustaw zegar na 1
        __delay_cycles(TIME);

        temp <<= 1; //przesuñ temp o jeden bit w lewo
        if (P7IN&SDA) //jeœli odebrany bit to jeden
            temp |= 0x01; //ustaw najmniej znacz¹cy bit temp na 1

        __delay_cycles(TIME);
        P2OUT &= ~SCL; //ustaw zegar na 0
        __delay_cycles(TIME);
    }

    //wyœlij ACK
    P7OUT &= ~SDA; //ustaw output na 0
    P7DIR |= SDA; //ustaw na output
    __delay_cycles(TIME);
    P2OUT |= SCL; //ustaw zegar na 1
    __delay_cycles(TIME*2);
    P2OUT &= ~SCL; //ustaw zegar na 0
    __delay_cycles(TIME);

    return temp;

}

void Stop(){
    // bit stopu
    P7DIR |= SDA; //ustaw port na output
    P7OUT &= ~SDA; //ustaw output na 0
    __delay_cycles(TIME);
    P2OUT |= SCL; //ustaw zegar na 1
    __delay_cycles(TIME);
    P7OUT |= SDA; //output = 1;
    __delay_cycles(TIME);
}




void i2c_send_bytes(){
    P3SEL |= 0x03;
    UCB0CTL1 |= UCSWRST;
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;
    UCB0CTL1 = UCSSEL_2 + UCSWRST;
    UCB0BR0 = 12;
    UCB0BR1 = 0;
    UCB0I2CSA = SLAVE_ESP32_ADDR;
    UCB0CTL1 &= ~UCSWRST;
    UCB0IE |= UCTXIE;
    __delay_cycles(TIME);
    PTxData = (unsigned char *)TxData;
    TXByteCtr = sizeof TxData;
    UCB0CTL1 |= UCTR + UCTXSTT;
    __bis_SR_register(LPM0_bits + GIE);
    __no_operation();
    while (UCB0CTL1 & UCTXSTP);
}




int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    //init portów
    P2DIR |= SCL;
        Start();
        SendComm(0xB4); //slave adress + Write 0xB4
        SendComm(0x07); //komenda - czytaj temperature obiektu
        Start();
        SendComm(0xB5); //slave adress + Read 0xB5
        wynik = ReadTemp(); //Czytaj 3 bajty
        Stop();
        TxData[3] = (wynik);
        TxData[2] = (wynik)>>8;
        TxData[1] = (wynik>>16);
        TxData[0] = (wynik>>24);
        i2c_send_bytes();
        wynik2 = wynik*0.02-0.18-273.15;
        __delay_cycles(MEASURE_TIME);
}





#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCB0IV,12))
  {
  case  0: break;                           // Vector  0: No interrupts
  case  2: break;                           // Vector  2: ALIFG
  case  4: break;                           // Vector  4: NACKIFG
  case  6: break;                           // Vector  6: STTIFG
  case  8: break;                           // Vector  8: STPIFG
  case 10: break;                           // Vector 10: RXIFG
  case 12:                                  // Vector 12: TXIFG
    if (TXByteCtr)                          // Check TX byte counter
    {
      UCB0TXBUF = *PTxData++;               // Load TX buffer
      TXByteCtr--;                          // Decrement TX byte counter
    }
    else
    {
      UCB0CTL1 |= UCTXSTP;                  // I2C stop condition
      UCB0IFG &= ~UCTXIFG;                  // Clear USCI_B0 TX int flag
      __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
    }
  default: break;
  }
}
