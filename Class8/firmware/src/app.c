/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
#include<xc.h>         
#include<sys/attribs.h> 
#include<stdio.h>
#include<string.h>
#include "i2c_master_noint.h"
#include "ili9341.h"
#include "imu.h"
// *****************************************************************************
// *****************************************************************************

#include "app.h"


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
#define x  28
#define y  32
#define c1 ILI9341_WHITE
#define c2 ILI9341_PURPLE
#define c3 ILI9341_DARKGREEN
#define c4 ILI9341_YELLOW
#define c5 ILI9341_RED
#define LENGTH 14

// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
            char m[100];
            unsigned char data[LENGTH]; //14
            signed short aX, aY;
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
       
            
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
            ANSELBbits.ANSB2 = 0;
            ANSELBbits.ANSB3 = 0;
            __builtin_enable_interrupts();
    
            SPI1_init();
            LCD_init();
            imu_init();
            LCD_clearScreen(c5);
            

            
            if (appInitialized)
            {
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {               
        LATAbits.LATA4 = 0;
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT()<=1200000){;} //delay
        LATAbits.LATA4 = 1;
        signed char input = whoAmI();
        I2C_read_multiple(SLAVE_ADDR, OUT_TEMP_L, data, LENGTH);
        aX = (data[9]  << 8) | data[8];
        aX=-aX;
        aY = (data[11] << 8) | data[10];
        aY=-aY;
        sprintf(m, "aX = %d  ", aX);
        LCD_print(m,x,y,0x0000,0xFFFF);
        sprintf(m, "aY = %d  ", aY);
        LCD_print(m,x,y+16,0x0000,0xFFFF);
        int aXscaled = ((int)aX/170.0);
        int aYscaled = ((int)aY/170.0);
        sprintf(m, "aXscaled = %d  ", aXscaled);
        LCD_print(m,x,y+8,0x0000,0xFFFF);
        sprintf(m, "aYscaled = %d  ", aYscaled);
        LCD_print(m,x,y+24,0x0000,0xFFFF);

        //Let's draw bars:
        LCD_barX(aXscaled,125,y+135,0x0000,0xFFFF);
        LCD_barY(aYscaled,125,y+135,0x0000,0xFFFF);
        break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 

/*******************************************************************************
 End of File
 */
