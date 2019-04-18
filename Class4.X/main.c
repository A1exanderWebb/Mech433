#include<xc.h>           // processor SFR definitions
#include <math.h>
#include<sys/attribs.h>  // __ISR macro

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // use slowest wdt
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

#define M_PI  3.14159265358979323846
#define CS LATBbits.LATB7       // chip select pin

char SPI1_IO(char write){
    SPI1BUF=write;
    while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
    }
    return SPI1BUF;
}

void setVoltage(char channel, int voltage){
    unsigned char c1, c2;
    unsigned char BUFGASHD=0b111;
    c1=BUFGASHD<<4;
    c1=c1|channel<<7;
    c2=voltage & 0b11111111;
    voltage=voltage>>8;
    voltage=voltage & 0b1111;
    c1=c1|voltage;
    
    LATBbits.LATB7=0;
    SPI1_IO(c1);
    SPI1_IO(c2);
    LATBbits.LATB7=1;
}

void initSPI1(){
  SPI1CON = 0;              // turn off the spi module and reset it
  SPI1BUF;                  // clear the rx buffer by reading from it
  SPI1BRG = 0x1;            // baud=
  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on spi 1
  TRISBbits.TRISB7 = 0;
  TRISAbits.TRISA4 = 0;
  RPB8Rbits.RPB8R=0b0011;
}


int main() {
    unsigned int i = 0;
    unsigned int trivltg;
    unsigned int sinvltg;
    char rising = 1;
    
    __builtin_disable_interrupts();
    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;
    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;
    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    // do your TRIS and LAT commands here
    ANSELA=0;
    ANSELB=0;
    initSPI1();
    __builtin_enable_interrupts();
    
    while(1) {
        if (i==1000){
            i=0;
            rising=-rising;
        }
        sinvltg=2048+1300*sin(M_PI*2*i/999);
        
        if(rising==1){
            trivltg=i*4;
        }
        if(rising==-1){
            trivltg=4000-i*4;
        }
        
        setVoltage(0,sinvltg);
        setVoltage(1,trivltg);
        i++;
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT()<24000){;}
        
    }
}