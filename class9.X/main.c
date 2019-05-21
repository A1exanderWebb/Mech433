#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "ili9341.h"
#include<stdio.h>
#include<string.h>

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

#define x  28
#define y  32
#define c1 ILI9341_WHITE
#define c2 ILI9341_PURPLE
#define c3 ILI9341_DARKGREEN
#define c4 ILI9341_YELLOW
#define c5 ILI9341_RED
#define LENGTH 14

int main() {

    __builtin_disable_interrupts();
    
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;
    INTCONbits.MVEC = 0x1;
    DDPCONbits.JTAGEN = 0;
    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    LATAbits.LATA4 = 1;

    SPI1_init();
    LCD_init();
    LCD_clearScreen(c2);
    touchinit();     
    
    __builtin_enable_interrupts();             
    
    unsigned short Xpos, Ypos; 
    int z; 
    char m[100];
    int count = 0, buttonPush = 0, buttonPush_prev = 0, xPixel_prev, yPixel_prev;
    
    sprintf(m, "HW 9: Touchscreen");
    LCD_print(m, x, y, ILI9341_WHITE, ILI9341_PURPLE);
    while(1) {
        //Heartbeat
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 4800000) {
            LATAbits.LATA4 = 0;
        }
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 4800000) {
            LATAbits.LATA4 = 1;
        }
       
        //Raw touchscreen vals:
        XPT2046_read(&Xpos, &Ypos, &z);
        sprintf(m, "X data = %5d", Xpos);
        LCD_print(m, x, y+20, ILI9341_WHITE, ILI9341_PURPLE);
        sprintf(m, "Y data= %5d", Ypos);
        LCD_print(m, x, y+30, ILI9341_WHITE, ILI9341_PURPLE);
        sprintf(m, "Z data= %5d", z);
        LCD_print(m, x, y+40, ILI9341_WHITE, ILI9341_PURPLE);
        
        //Scaled vals:
        sprintf(m, "X Scaled = %5d", xPixel(Xpos));
        LCD_print(m, x, y+60, ILI9341_WHITE, ILI9341_PURPLE);
        sprintf(m, "Y Scaled = %5d", yPixel(Ypos));
        LCD_print(m, x, y+70, ILI9341_WHITE, ILI9341_PURPLE);
        
        //Press button:
        if (z > 100){
            buttonPush = 1;
            xPixel_prev = xPixel(Xpos);
            yPixel_prev = yPixel(Ypos);           
        }
        if (z < 100){
            buttonPush = 0;
        }
        if ((buttonPush == 0) && (buttonPush_prev == 1)){
            count = count + buttonsPush(xPixel_prev, yPixel_prev);
        }   
        buttonPush_prev = buttonPush;
        sprintf(m, "I = %d", count);
        LCD_print(m, 130, 200, ILI9341_WHITE, ILI9341_PURPLE);
    }
}