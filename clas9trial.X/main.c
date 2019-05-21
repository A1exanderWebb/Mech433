#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "ili9341.h"
#include<stdio.h>
#include<string.h>
#include "touchscreen.h"

// DEVCFG0
#pragma config DEBUG = 0b11 // no debugging
#pragma config JTAGEN = 0b0 // no jtag
#pragma config ICESEL = 0b11 // use PGED1 and PGEC1
#pragma config PWP = 0x3f // no write protect
#pragma config BWP = 0b0 // no boot write protect
#pragma config CP = 0b1 // no code protect

// DEVCFG1
#pragma config FNOSC = 0b011 // use primary oscillator with pll
#pragma config FSOSCEN = 0b0 // turn off secondary oscillator
#pragma config IESO = 0b0 // no switching clocks
#pragma config POSCMOD = 0b10 // high speed crystal mode
#pragma config OSCIOFNC = 0b1 // disable secondary osc
#pragma config FPBDIV = 0b00 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = 0b11 // do not enable clock switch
#pragma config WDTPS = 0b10100 // use slowest wdt
#pragma config WINDIS = 0b1 // wdt no window mode
#pragma config FWDTEN = 0b0 // wdt disabled
#pragma config FWDTWINSZ = 0b11 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = 0b001 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = 0b111 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = 0b001 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = 0b001 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = 0b0 // USB clock on

// DEVCFG3
#pragma config USERID = 0b0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = 0b0 // allow multiple reconfigurations
#pragma config IOL1WAY = 0b0 // allow multiple reconfigurations
#pragma config FUSBIDIO = 0b1 // USB pins controlled by USB module
#pragma config FVBUSONIO = 0b1 // USB BUSON controlled by USB module

#define x  28
#define y  32
#define c1 ILI9341_WHITE
#define c2 ILI9341_PURPLE
#define c3 ILI9341_DARKGREEN
#define c4 ILI9341_YELLOW
#define c5 ILI9341_RED

#define LENGTH 14

#define x0 120
#define y0 160
#define xP 200
#define xM 20
#define yP 240
#define yM 80


//#define OUT_TEMP_L 0x20

int main() {

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
    TRISAbits.TRISA4 = 0;
    TRISBbits.TRISB4 = 1;
    LATAbits.LATA4 = 1;
    
    //ANSELBbits.ANSB2 = 0;
	//ANSELBbits.ANSB3 = 0;

    __builtin_enable_interrupts();
    

    SPI1_init();
    LCD_init();
    LCD_clearScreen(c2);
    buttons();                  //Buttons:
    //touchscreen
    unsigned short Xpos, Ypos; 
    int z; 
    char message[100];
    //for button press:
    int count = 0, buttonPush = 0, buttonPush_prev = 0, xPixel_prev, yPixel_prev;
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
        sprintf(message, "HW 9: Touchscreen");
        print2LCD(message, x, y, ILI9341_WHITE, ILI9341_PURPLE);
        
        //Raw touchscreen vals:
        XPT2046_read(&Xpos, &Ypos, &z);
        sprintf(message, "xRaw    = %5d", Xpos);
        print2LCD(message, x, y+20, ILI9341_WHITE, ILI9341_PURPLE);
        sprintf(message, "yRaw    = %5d", Ypos);
        print2LCD(message, x, y+30, ILI9341_WHITE, ILI9341_PURPLE);
        sprintf(message, "zRaw    = %5d", z);
        print2LCD(message, x, y+40, ILI9341_WHITE, ILI9341_PURPLE);
        
        //Scaled vals:
        sprintf(message, "xScaled = %5d", xPixel(Xpos));
        print2LCD(message, x, y+60, ILI9341_WHITE, ILI9341_PURPLE);
        sprintf(message, "xScaled = %5d", yPixel(Ypos));
        print2LCD(message, x, y+70, ILI9341_WHITE, ILI9341_PURPLE);
        
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
        sprintf(message, "I = %d", count);
        print2LCD(message, 130, 200, ILI9341_WHITE, ILI9341_PURPLE);
    }
}
