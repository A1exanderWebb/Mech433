#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h> 

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

int main(){
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 0;
    SPI1_init();
    LCD_init();
    LATAbits.LATA4 = 1;
    LCD_clearScreen(0x7800);
 
    char m[100];
    sprintf(m,"Hello World");
    unsigned short x=25;
    unsigned short y=15;
    //LCDdrawletter('d',150,100,0x0000,0xFC18);
    LCD_print(m,x,y,0x0000,0xFFFF);
    
    int i=0;
    while(i<=100){
        sprintf(m,"Hello World %i %%",i);
        LCD_print(m,x,y,0x0000,0xFFFF);
        
        LCD_barX(i,x,y+25,0x0000,0xFFFF);
        LCD_barX(i,x,y+26,0x0000,0xFFFF);
        LCD_barX(i,x,y+27,0x0000,0xFFFF);
        //LCD_barY(i,x,y+25,0x0000,0xFFFF);
        i++;
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT()<=1200000){;}
    }
    
    
    
    while(1) {
        _CP0_SET_COUNT(0);
        LATAbits.LATA4 = 1;
        while (_CP0_GET_COUNT()<=120000){;}
        LATAbits.LATA4 = 0;
        while (_CP0_GET_COUNT()<=120000){;}
        while (PORTBbits.RB4 == 0){;}   
    }
}
