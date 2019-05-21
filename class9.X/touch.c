#include <xc.h>
#include "touch.h"
#include<math.h>
#include "ili9341.h"
#include<string.h>

void XPT2046_read(unsigned short *x, unsigned short *y, unsigned int *z) {
    unsigned short temp1, temp2, z1, z2;
    
    CS2 = 0; 
    //0b 1 A2A1A0 0001
    //get x position: A2 = 1, A1 = 0    , A0 = 1
    spi_io(0b11010001);
    temp1 = spi_io(0x00);
    temp2 = spi_io(0x00);
    *x = ((temp1  << 8) | temp2)>>3;  
    
    //get y position: A2 = 0, A1 = 0, A0 = 1    
    spi_io(0b10010001);
    temp1 = spi_io(0x00);
    temp2 = spi_io(0x00);
    *y = ((temp1  << 8) | temp2)>>3; 
    
    //get z1 pressure: A2 = 0, A1 = 1, A0 = 1
    spi_io(0b10110001); 
    temp1 = spi_io(0x00);
    temp2 = spi_io(0x00);
    z1 = ((temp1  << 8) | temp2)>>3;
    
    //get z2 pressure: A2 = 1, A1 = 0, A0 = 0
    spi_io(0b11000001); 
    temp1 = spi_io(0x00);
    temp2 = spi_io(0x00);
    z2 = ((temp1  << 8) | temp2)>>3;
    *z = z1 - z2 + 4095; 

    CS2 = 1; 
}

unsigned short xPixel(unsigned short x) {
    unsigned short xScaled = ((x-460.0)/15.0); //0->460, 240->4095
    return xScaled;
}

unsigned short yPixel(unsigned short y) {
    unsigned short yScaled = ((y-420.0)/10.5); //0->420, 310->3870
    return yScaled;
}

void touchinit(void) {
    char message[100], i, j;
    //button I++
    for (i=0;i<40;i++) {        //length
        for (j=0;j<20;j++) {    //height
            LCD_drawPixel(120+i, 160+j, ILI9341_WHITE);
        }
    }
    sprintf(message, "I++");
    LCD_print(message, 120+13, 160+8, ILI9341_RED, ILI9341_WHITE);
    
       //button I--
    for (i=0;i<40;i++) {        //length
        for (j=0;j<20;j++) {    //height
            LCD_drawPixel(120+i, 230+j, ILI9341_WHITE);
        }
    }
    sprintf(message, "I--");
    LCD_print(message, 120+13, 230+8, ILI9341_RED, ILI9341_WHITE);
}

signed int buttonsPush(unsigned short xPixel, unsigned short yPixel) {
    if ((xPixel>=117) && (xPixel <=152)) {
        if ((yPixel>=144) && (yPixel<=164)) {
            return 1;
        }
        else if ((yPixel>=71) && (yPixel<=88)) {
            return -1;
        }
    }
}