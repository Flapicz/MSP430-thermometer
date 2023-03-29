#include <msp430.h>
#include <header.h>
#include <stdio.h>

//SDA - port 7.4
//SCL - port 2.2


void Start(){
    // bit startu
    P7DIR |= SDA; //ustaw port na output
    P7OUT |= SDA; //ustaw wyj�cie na 1
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

    for (i=0;i<8;++i) { //dla ka�dego bitu w bajcie
        temp = comm<<i; //temp = comm przesuni�ty o i bit�w w lewo
        temp &= 0x80; //temp AND 1000 0000 = wyzeruj wszystkie bity opr�cz najbardziej znaczacego. Dzi�ki temu temp = bit i zmiennej comm

        if (temp) //je�li temp = 1
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
    if(!P7IN&SDA) //ACK
        printf("A");
    __delay_cycles(TIME);
    P2OUT &= ~SCL; //ustaw zegar na 0
    __delay_cycles(TIME);
}


unsigned int ReadTemp(void) { //czytaj temperatur�
    unsigned int result = 0;

    result = ReadByte(); //przeczytaj pierwsze 8 bitow temperatury i wy�lij ACK
    result |= (ReadByte() << 8); //przeczytaj drugie 8 bitow temperatury i wy�lij ACK
    ReadByte(); //przeczytaj PEC i wy�lij ACK

    return result;
}


unsigned int ReadByte(){
    volatile unsigned int i;
    unsigned int temp = 0;

    //odbierz 8 bit�w
    P7DIR &= ~SDA; //ustaw port na input

    for (i=8;i>0;--i) { //dla ka�dego z 8 odbieranych bit�w
        __delay_cycles(TIME);
        P2OUT |= SCL; //ustaw zegar na 1
        __delay_cycles(TIME);

        temp <<= 1; //przesu� temp o jeden bit w lewo
        if (P7IN&SDA) //je�li odebrany bit to jeden
            temp |= 0x01; //ustaw najmniej znacz�cy bit temp na 1

        __delay_cycles(TIME);
        P2OUT &= ~SCL; //ustaw zegar na 0
        __delay_cycles(TIME);
    }

    //wy�lij ACK
    P7DIR |= SDA; //ustaw na output
    P7OUT &= ~SDA; //ustaw output na 0
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


int main(void)
{
    unsigned int wynik = 0;

	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	//init port�w
	P2DIR |= SCL;

	for(;;){
	    Start();
	    SendComm(0x00); //slave adress + Write 0xB4
	    SendComm(0x06); //komenda - czytaj temperature obiektu
	    Start();
	    SendComm(0x01); //slave adress + Read 0xB5
	    wynik = ReadTemp(); //Czytaj 3 bajty
	    Stop();
	    __delay_cycles(MEASURE_TIME);
	}
}
