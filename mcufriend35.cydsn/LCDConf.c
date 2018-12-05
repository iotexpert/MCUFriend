/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.46 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software  has been licensed to  Cypress Semiconductor Corporation,
whose registered  office is situated  at 198 Champion Ct. San Jose, CA 
95134 USA  solely for the  purposes of creating  libraries for Cypress
PSoC3 and  PSoC5 processor-based devices,  sublicensed and distributed
under  the  terms  and  conditions  of  the  Cypress  End User License
Agreement.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Microcontroller Systems LLC
Licensed to:              Cypress Semiconductor Corp, 198 Champion Ct., San Jose, CA 95134, USA
Licensed SEGGER software: emWin
License number:           GUI-00319
License model:            Services and License Agreement, signed June 10th, 2009
Licensed platform:        Any Cypress platform (Initial targets are: PSoC3, PSoC5)
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2009-06-12 - 2022-07-27
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/
#include <stdio.h>
#include "GUI.h"
#include "GUIDRV_FlexColor.h"

#include "project.h"
/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************
*/
//
// Physical display size
//   The display size should be adapted in order to match the size of
//   the target display.
//
#define XSIZE_PHYS 320
#define YSIZE_PHYS 480

//
// Color conversion
//   The color conversion functions should be selected according to
//   the color mode of the target display. Details can be found in
//   the chapter "Colors" in the emWin user manual.
//
#define COLOR_CONVERSION GUICC_M565

//
// Display driver
//
#define DISPLAY_DRIVER GUIDRV_FLEXCOLOR

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VXSIZE_PHYS
  #define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef   VYSIZE_PHYS
  #define VYSIZE_PHYS YSIZE_PHYS
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
  #error Color conversion not defined!
#endif
#ifndef   DISPLAY_DRIVER
  #error No display driver defined!
#endif


static const uint8_t ILI9341_regValues_2_4[]  = {        // BOE 2.4"                                                                                                                
0x1,0x0,                                  // Software Reset
0x28,0x0,                                 // Display Off
0x3A,0x1,0x55,                            // Pixel Format RGB=16-bits/pixel MCU=16-bits/Pixel
0xF6,0x3,0x1,0x1,0x0,                     // Interface control .. I have no idea
#if 0    
0xCF,0x3,0x0,0x81,0x30,                   // Not defined
0xED,0x4,0x64,0x3,0x12,0x81,              // Not defined
0xE8,0x3,0x85,0x10,0x78,                  // Not defined
0xCB,0x5,0x39,0x2C,0x0,0x34,0x2,          // Not defined
0xF7,0x1,0x20,                            // Not defined
0xEA,0x2,0x0,0x0,                         // Not defined  
#endif
0xB0,0x1,0x0,                             // RGB Interface Control
0xB1,0x2,0x0,0x1B,                        // Frame Rate Control
0xB4,0x1,0x0,                             // Display Inversion Control
0xC0,0x1,0x21,                            // Power Control 1
0xC1,0x1,0x11,                            // Power Control 2
0xC5,0x2,0x3F,0x3C,                       // VCOM Control 1
0xC7,0x1,0xB5,                            // VCOM Control 2
0x36,0x1,0x48,                            // Memory Access Control

#if 0
0xF2,0x1,0x0,                             // Not defined
#endif

0x26,0x1,0x1,                             // Gamma Set
0xE0,0xF,0xF,0x26,0x24,0xB,0xE,0x9,0x54,0xA8,0x46,0xC,0x17,0x9,0xF,0x7,0x0,    // Positive Gamma Correction
0xE1,0xF,0x0,0x19,0x1B,0x4,0x10,0x7,0x2A,0x47,0x39,0x3,0x6,0x6,0x30,0x38,0xF,  // Negative Gamme Correction
0x11,0x0,                                 // Sleep Out
0x29,0x0,                                 // Display On
0x36,0x1,0x48,                            // Memory Access Control
0x2A,0x4,0x0,0x0,0x0,0xEF,                // Column Address Set = 239
0x2B,0x4,0x0,0x0,0x1,0x3F,                // Row Address Set  = 319
0x33,0x6,0x0,0x0,0x1,0x40,0x0,0x0,        // Vertical Scrolling Definition
0x37,0x2,0x0,0x0,                         // Vertical Scrolling Start Address
0x13,0x0,                                 // Normal Display ON
0x20,0x0,                                 // Display Inversion OFF
};

static void lcd_start_reset()
{
    GraphicLCDIntf_1_Start();

    /* Reset - High, Low (reset), High */
    Cy_GPIO_Set(LCD_RESET_N_0_PORT, LCD_RESET_N_0_NUM);
    GUI_Delay(20);
    Cy_GPIO_Clr(LCD_RESET_N_0_PORT, LCD_RESET_N_0_NUM);
    GUI_Delay(100);
    Cy_GPIO_Set(LCD_RESET_N_0_PORT, LCD_RESET_N_0_NUM);
    GUI_Delay(100);
    GraphicLCDIntf_1_Write8(0,0x01);
    GUI_Delay(100);
}

static void sendStartSequence(const uint8_t *buff,uint32_t len)
{
    for(unsigned int i=0;i<len;i++)
    {
        if(buff[i] == 0xDD) // 
        {
            //printf("Delay %d\r\n",buff[i+1]);
            CyDelay(buff[i+1]);
            i=i+1;
        }
        else
        {
            GraphicLCDIntf_1_Write8_A0(buff[i]);
            //printf("C = %02X ",buff[i]);
            i=i+1;
            unsigned int count;
            count = buff[i];
            //printf("%02X ",count);
            for(unsigned int j=0;j<count;j++)
            {
                i=i+1;
                GraphicLCDIntf_1_Write8_A1(buff[i]);
            //    printf(" %02X",buff[i]);
            }
            //printf("\r\n");
        }
    }
}

static const uint8_t mcu35_init_sequence_kbv[]  = {
    0x1,0x0,                            // Software Reset
    0x28,0x0,                           // Display Off
    0x3A,0x1,0x55,                      // Pixel Format Set 565
    0x3A,0x1,0x55,                      // Pixel Format Set 565
    0x11,0x0,                           // Sleep Out
    0x29,0x0,                           // Display On
    0xB6,0x3,0x0,0x22,0x3B,             // Display Function Control
    0x36,0x1,0x8,                       // Memory Access Control
    0x2A,0x4,0x0,0x0,0x1,0x3F,          // Column Set Address 320
    0x2B,0x4,0x0,0x0,0x1,0xDF,          // Page Set Addres 480
    0x33,0x6,0x0,0x0,0x1,0xE0,0x0,0x0,  // Vertical Scrolling Definition
    0x37,0x2,0x0,0x0,                   // Vertical Scrolling Start Address
    0x13,0x0,                           // Normal Display On
    0x20,0x0,                           // Display Inversion Off
};

static const uint8_t mcu35_init_9090[]  = {
    0x1,0x0, 
    0x28,0x0, 
    0x3A,0x1,0x55, 
    0x11,0x0, 
    0x29,0x0, 
    0x36,0x1,0x48, 
    0x2A,0x4,0x0,0x0,0x1,0x3F, 
    0x2B,0x4,0x0,0x0,0x1,0xDF, 
    0x33,0x6,0x0,0x0,0x1,0xE0,0x0,0x0, 
    0x37,0x2,0x0,0x0, 
    0x13,0x0, 
    0x21,0x0, 
};



static const uint8_t mcu35_init_sequence_web[]  = {
    0xFF,0x00,          // ?
    0xFF,0x00,          // ?
    0xDD,5,             // Delay 5
    0xFF,0x00,          //
    0xFF,0x00,          //
    0xFF,0x00,          //
    0xFF,0x00,          // ?
    0xDD,10,            // delay 10
    //  
    0xB0,0x01,0x00,                                     // IF Mode control
    0xB3,0x04,0x02,0x00,0x00,0x10,                      // Frame Rate Control - only 2 paramters
    0xB4,0x01,0x11,                                     // Display inversion control 
    0xC0,0x08,0x13,0x3B,0x00,0x00,0x00,0x01,0x00,0x43,  // Power Control 1
    0xC1,0x04,0x08,0x15,0x08,0x08,                      // Power Control 2
    0xC4,0x04,0x15,0x03,0x03,0x01,                      // ?
    0xC6,0x01,0x02,                                     // ?
    // ??
    0xC8,0x15,0x0C,0x05,0x0A,0x6B,0x04,0x06,0x15,0x10,0x00,0x31,0x10,0x15,0x06,0x64,0x0D,0x0A,0x05,0x0C,0x31,0x00,
    0x35,0x01,0x00,                     // Tearing Effect 
    0x0C,0x01,0x66,                     // Read pixel format?
    0x3A,0x01,0x55,                     // Pixel Format Set
    
    0x44,0x02,0x00,0x01,                // Set Tear Scanline
    0xD0,0x04,0x07,0x07,0x14,0xA2,      // NVM Write
    0xD1,0x03,0x03,0x5A,0x10,           // NVM Protection Key
    0xD2,0x03,0x03,0x04,0x04,           // NVM Status Read

    0x11,0x00,                          // Sleep Out
    0xDD,150,                           // Delay 150ms
    0x2A,0x04,0x00,0x00,0x01,0x3F,      // Column Set Address 320
    0x2B,0x04,0x00,0x00,0x01,0xDF,      // Page Set Address   480
    0xDD,100,                           // Delay 100ms
    0x29,0x00,                          // Display On
    0xDD,30,                            // delay 30ms
    0x2C,0x00                           // Memory Write
};


static const uint8_t mcu35_init_sequence_web_edited[]  = {
//    0xFF,0x00,          // ?
//    0xFF,0x00,          // ?
//    0xDD,5,             // Delay 5
//    0xFF,0x00,          //
//    0xFF,0x00,          //
//    0xFF,0x00,          //
//    0xFF,0x00,          // ?
//    0xDD,10,            // delay 10
    //  
//    0xB0,0x01,0x00,                                     // IF Mode control
//    0xB3,0x04,0x02,0x00,0x00,0x10,                      // Frame Rate Control - only 2 paramters
//    0xB4,0x01,0x11,                                     // Display inversion control 
//    0xC0,0x08,0x13,0x3B,0x00,0x00,0x00,0x01,0x00,0x43,  // Power Control 1
//    0xC1,0x04,0x08,0x15,0x08,0x08,                      // Power Control 2
//    0xC4,0x04,0x15,0x03,0x03,0x01,                      // ?
//    0xC6,0x01,0x02,                                     // ?
    // ??
//    0xC8,0x15,0x0C,0x05,0x0A,0x6B,0x04,0x06,0x15,0x10,0x00,0x31,0x10,0x15,0x06,0x64,0x0D,0x0A,0x05,0x0C,0x31,0x00,
    0x35,0x01,0x00,                     // Tearing Effect 
 //   0x0C,0x01,0x66,                     // Read pixel format?
    0x3A,0x01,0x55,                     // Pixel Format Set
    
    0x44,0x02,0x00,0x01,                // Set Tear Scanline
//    0xD0,0x04,0x07,0x07,0x14,0xA2,      // NVM Write
//    0xD1,0x03,0x03,0x5A,0x10,           // NVM Protection Key
//   0xD2,0x03,0x03,0x04,0x04,           // NVM Status Read

    0x11,0x00,                          // Sleep Out
    0xDD,150,                           // Delay 150ms
    0x2A,0x04,0x00,0x00,0x01,0x3F,      // Column Set Address 320
    0x2B,0x04,0x00,0x00,0x01,0xDF,      // Page Set Address   480
    0xDD,100,                           // Delay 100ms
    0x29,0x00,                          // Display On
    0xDD,30,                            // delay 30ms
//    0x2C,0x00                           // Memory Write
};

static void _InitController35Web()
{
    GraphicLCDIntf_1_Write8_A0(0xFF);
    GraphicLCDIntf_1_Write8_A0(0xFF);
    CyDelay(5);
    GraphicLCDIntf_1_Write8_A0(0xFF);
    GraphicLCDIntf_1_Write8_A0(0xFF);
    GraphicLCDIntf_1_Write8_A0(0xFF);
    GraphicLCDIntf_1_Write8_A0(0xFF);
    CyDelay(10);
    //0xB0,0x01,0x00,
    GraphicLCDIntf_1_Write8_A0(0xB0); //0
    GraphicLCDIntf_1_Write8_A1(0x00);
    
    //0xB3,0x04,0x02,0x00,0x00,0x10,
    GraphicLCDIntf_1_Write8_A0(0xB3); //1
    GraphicLCDIntf_1_Write8_A1(0x02);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x10);
    
    //0xB4,0x01,0x11,
    GraphicLCDIntf_1_Write8_A0(0xB4); // 2
    GraphicLCDIntf_1_Write8_A1(0x11);//0X10

    // 0xC0,0x08,0x13,0x3B,0x00,0x00,0x00,0x01,0x00,0x43,
    GraphicLCDIntf_1_Write8_A0(0xC0);
    GraphicLCDIntf_1_Write8_A1(0x13);
    GraphicLCDIntf_1_Write8_A1(0x3B);//
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x01);
    GraphicLCDIntf_1_Write8_A1(0x00);//NW
    GraphicLCDIntf_1_Write8_A1(0x43);

    // 0xC1,0x04,0x08,0x15,0x08,0x08,
    GraphicLCDIntf_1_Write8_A0(0xC1);
    GraphicLCDIntf_1_Write8_A1(0x08);
    GraphicLCDIntf_1_Write8_A1(0x15);//CLOCK
    GraphicLCDIntf_1_Write8_A1(0x08);
    GraphicLCDIntf_1_Write8_A1(0x08);

    // 0xC4,0x04,0x15,0x03,0x03,0x01
    GraphicLCDIntf_1_Write8_A0(0xC4);
    GraphicLCDIntf_1_Write8_A1(0x15);
    GraphicLCDIntf_1_Write8_A1(0x03);
    GraphicLCDIntf_1_Write8_A1(0x03);
    GraphicLCDIntf_1_Write8_A1(0x01);

    // 0xC6,0x01,0x02
    GraphicLCDIntf_1_Write8_A0(0xC6);
    GraphicLCDIntf_1_Write8_A1(0x02);

    // 0xC8,0x15,0x0C,0x05,0x0A,0x6B,0x04,0x06,0x15,0x10,0x00,0x31,0x10,0x15,0x06,0x64,0x0D,0x0A,0x05,0x0C,0x31,0x00
    GraphicLCDIntf_1_Write8_A0(0xC8);
    GraphicLCDIntf_1_Write8_A1(0x0c);
    GraphicLCDIntf_1_Write8_A1(0x05);
    GraphicLCDIntf_1_Write8_A1(0x0A);//0X12
    GraphicLCDIntf_1_Write8_A1(0x6B);//0x7D
    GraphicLCDIntf_1_Write8_A1(0x04);
    GraphicLCDIntf_1_Write8_A1(0x06);//0x08
    GraphicLCDIntf_1_Write8_A1(0x15);//0x0A
    GraphicLCDIntf_1_Write8_A1(0x10);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x31);//0x23
    GraphicLCDIntf_1_Write8_A1(0x10);
    GraphicLCDIntf_1_Write8_A1(0x15);//0x0A
    GraphicLCDIntf_1_Write8_A1(0x06);//0x08
    GraphicLCDIntf_1_Write8_A1(0x64);//0x74
    GraphicLCDIntf_1_Write8_A1(0x0D);//0x0B
    GraphicLCDIntf_1_Write8_A1(0x0A);//0x12
    GraphicLCDIntf_1_Write8_A1(0x05);//0x08
    GraphicLCDIntf_1_Write8_A1(0x0C);//0x06
    GraphicLCDIntf_1_Write8_A1(0x31);//0x23
    GraphicLCDIntf_1_Write8_A1(0x00);

    // 0x35,0x01,0x00
    GraphicLCDIntf_1_Write8_A0(0x35);
    GraphicLCDIntf_1_Write8_A1(0x00);
    //GraphicLCDIntf_1_Write8_A0(0x36);
    //GraphicLCDIntf_1_Write8_A1(0x00);

    // 0x0C,0x01,x066
    GraphicLCDIntf_1_Write8_A0(0x0C);
    GraphicLCDIntf_1_Write8_A1(0x66);

    //0x3A,0x01,0x55
    GraphicLCDIntf_1_Write8_A0(0x3A);
    GraphicLCDIntf_1_Write8_A1(0x55); // ARH changed to 565
    //GraphicLCDIntf_1_Write8_A1(0x66);

    // 0x44,0x02,0x00,0x01
    GraphicLCDIntf_1_Write8_A0(0x44);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x01);

    // 0xD0,0x04,0x07,0x07,0x14,0xA2,
    GraphicLCDIntf_1_Write8_A0(0xD0);
    GraphicLCDIntf_1_Write8_A1(0x07);
    GraphicLCDIntf_1_Write8_A1(0x07);//VCI1
    GraphicLCDIntf_1_Write8_A1(0x14);//VRH 0x1D
    GraphicLCDIntf_1_Write8_A1(0xA2);//BT 0x06


    // 0xD1,0x03,0x03,0x5A,0x10 
    GraphicLCDIntf_1_Write8_A0(0xD1);
    GraphicLCDIntf_1_Write8_A1(0x03);
    GraphicLCDIntf_1_Write8_A1(0x5A);//VCM  0x5A
    GraphicLCDIntf_1_Write8_A1(0x10);//VDV

    // 0xD2,0x03,0x03,0x04,0x04,
    GraphicLCDIntf_1_Write8_A0(0xD2);
    GraphicLCDIntf_1_Write8_A1(0x03);
    GraphicLCDIntf_1_Write8_A1(0x04);//0x24
    GraphicLCDIntf_1_Write8_A1(0x04);

    // 0x11,0x00,
    GraphicLCDIntf_1_Write8_A0(0x11);
    CyDelay(150);


    // 0x2A,0x04,0x00,0x00,0x01,0x3F
    GraphicLCDIntf_1_Write8_A0(0x2A);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x01);
    GraphicLCDIntf_1_Write8_A1(0x3F);//320


    // 0x2B,0x04,0x00,0x00,0x01,0xDF
    GraphicLCDIntf_1_Write8_A0(0x2B);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x01);
    GraphicLCDIntf_1_Write8_A1(0xDF);//480
    //GraphicLCDIntf_1_Write8_A0(0xB4);
    //GraphicLCDIntf_1_Write8_A1(0x00);
    
    CyDelay(100);

    // 0x29,0x00
    GraphicLCDIntf_1_Write8_A0(0x29);
    CyDelay(30);


    // 0x2C,0x00
    GraphicLCDIntf_1_Write8_A0(0x2C);
}

#if 0
/********************************************************************
*
*       _InitController
*
* Purpose:
*   Initializes the LCD controller
*
*********************************************************************/
static void _InitController_st7789(void) {
    /* Set up the display controller and put it into operation. If the 
    *  display controller is not initialized by any external routine 
    *  this needs to be adapted by the customer.
    */
    
    /* Start the parallel interface */
    GraphicLCDIntf_1_Start();

    /* Reset - High, Low (reset), High */
    Cy_GPIO_Set(LCD_RESET_N_0_PORT, LCD_RESET_N_0_NUM);
    GUI_Delay(20);
    Cy_GPIO_Clr(LCD_RESET_N_0_PORT, LCD_RESET_N_0_NUM);
    GUI_Delay(100);
    Cy_GPIO_Set(LCD_RESET_N_0_PORT, LCD_RESET_N_0_NUM);
    GUI_Delay(100);

    GraphicLCDIntf_1_Write8_A0(0x28);
    GraphicLCDIntf_1_Write8_A0(0x11); //Exit Sleep mode
    GUI_Delay(100);
    GraphicLCDIntf_1_Write8_A0(0x36);
    GraphicLCDIntf_1_Write8_A1(0xA0);//MADCTL: memory data access control
    GraphicLCDIntf_1_Write8_A0(0x3A);
    GraphicLCDIntf_1_Write8_A1(0x65);//COLMOD: Interface Pixel format
    GraphicLCDIntf_1_Write8_A0(0xB2);
    GraphicLCDIntf_1_Write8_A1(0x0C);
    GraphicLCDIntf_1_Write8_A1(0x0C);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x33);
    GraphicLCDIntf_1_Write8_A1(0x33);//PORCTRK: Porch setting
    GraphicLCDIntf_1_Write8_A0(0xB7);
    GraphicLCDIntf_1_Write8_A1(0x35);//GCTRL: Gate Control
    GraphicLCDIntf_1_Write8_A0(0xBB);
    GraphicLCDIntf_1_Write8_A1(0x2B);//VCOMS: VCOM setting
    GraphicLCDIntf_1_Write8_A0(0xC0);
    GraphicLCDIntf_1_Write8_A1(0x2C);//LCMCTRL: LCM Control
    GraphicLCDIntf_1_Write8_A0(0xC2);
    GraphicLCDIntf_1_Write8_A1(0x01);
    GraphicLCDIntf_1_Write8_A1(0xFF);//VDVVRHEN: VDV and VRH Command Enable
    GraphicLCDIntf_1_Write8_A0(0xC3);
    GraphicLCDIntf_1_Write8_A1(0x11);//VRHS: VRH Set
    GraphicLCDIntf_1_Write8_A0(0xC4);
    GraphicLCDIntf_1_Write8_A1(0x20);//VDVS: VDV Set
    GraphicLCDIntf_1_Write8_A0(0xC6);
    GraphicLCDIntf_1_Write8_A1(0x0F);//FRCTRL2: Frame Rate control in normal mode
    GraphicLCDIntf_1_Write8_A0(0xD0);
    GraphicLCDIntf_1_Write8_A1(0xA4);
    GraphicLCDIntf_1_Write8_A1(0xA1);//PWCTRL1: Power Control 1
    GraphicLCDIntf_1_Write8_A0(0xE0);
    GraphicLCDIntf_1_Write8_A1(0xD0);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x05);
    GraphicLCDIntf_1_Write8_A1(0x0E);
    GraphicLCDIntf_1_Write8_A1(0x15);
    GraphicLCDIntf_1_Write8_A1(0x0D);
    GraphicLCDIntf_1_Write8_A1(0x37);
    GraphicLCDIntf_1_Write8_A1(0x43);
    GraphicLCDIntf_1_Write8_A1(0x47);
    GraphicLCDIntf_1_Write8_A1(0x09);
    GraphicLCDIntf_1_Write8_A1(0x15);
    GraphicLCDIntf_1_Write8_A1(0x12);
    GraphicLCDIntf_1_Write8_A1(0x16);
    GraphicLCDIntf_1_Write8_A1(0x19);//PVGAMCTRL: Positive Voltage Gamma control
    GraphicLCDIntf_1_Write8_A0(0xE1);
    GraphicLCDIntf_1_Write8_A1(0xD0);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x05);
    GraphicLCDIntf_1_Write8_A1(0x0D);
    GraphicLCDIntf_1_Write8_A1(0x0C);
    GraphicLCDIntf_1_Write8_A1(0x06);
    GraphicLCDIntf_1_Write8_A1(0x2D);
    GraphicLCDIntf_1_Write8_A1(0x44);
    GraphicLCDIntf_1_Write8_A1(0x40);
    GraphicLCDIntf_1_Write8_A1(0x0E);
    GraphicLCDIntf_1_Write8_A1(0x1C);
    GraphicLCDIntf_1_Write8_A1(0x18);
    GraphicLCDIntf_1_Write8_A1(0x16);
    GraphicLCDIntf_1_Write8_A1(0x19);//NVGAMCTRL: Negative Voltage Gamma control
    GraphicLCDIntf_1_Write8_A0(0x2B);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0xEF);//Y address set
    GraphicLCDIntf_1_Write8_A0(0x2A);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x00);
    GraphicLCDIntf_1_Write8_A1(0x01);
    GraphicLCDIntf_1_Write8_A1(0x3F);//X address set
    GUI_Delay(10);
    GraphicLCDIntf_1_Write8_A0(0x29);  
}
#endif
/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Function description
*   Called during the initialization process in order to set up the
*   display driver configuration.
*/
void LCD_X_Config(void) {
    GUI_DEVICE * pDevice;
    CONFIG_FLEXCOLOR Config = {0};
    GUI_PORT_API PortAPI = {0};
    //
    // Set the display driver and color conversion
    //
    // GUICC_565
    // GUICC_M565
    pDevice = GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, GUICC_565, 0, 0);
    //
    // Display driver configuration
    //
    LCD_SetSizeEx    (0, XSIZE_PHYS,   YSIZE_PHYS);
    LCD_SetVSizeEx   (0, VXSIZE_PHYS,  VYSIZE_PHYS);
    //
    // Orientation
    //
    Config.Orientation   =   GUI_SWAP_XY;
    // Sheild 1
    //Config.Orientation   = GUI_SWAP_XY | GUI_MIRROR_X | GUI_MIRROR_Y;
    GUIDRV_FlexColor_Config(pDevice, &Config);
    //
    // Set controller and operation mode
    //
    PortAPI.pfWrite8_A0  = GraphicLCDIntf_1_Write8_A0;
    PortAPI.pfWrite8_A1  = GraphicLCDIntf_1_Write8_A1;
    PortAPI.pfWriteM8_A1 = GraphicLCDIntf_1_WriteM8_A1;
    PortAPI.pfRead8_A1  = GraphicLCDIntf_1_Read8_A1;
    PortAPI.pfReadM8_A1  = GraphicLCDIntf_1_ReadM8_A1;
    GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66709, GUIDRV_FLEXCOLOR_M16C0B8);
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task, the routine needs to be adapted to
*   the display controller. Note that the commands marked
*   "optional" are not cogently required and should only be adapted if 
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Zero based layer index
*   Cmd        - Command to be executed
*   pData      - Pointer to a data structure.
* 
* Return Value:
*   < -1 - Error
*     -1 - The command is not handled.
*      0 - OK.
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
    int r;

    GUI_USE_PARA(LayerIndex);
    GUI_USE_PARA(pData);

    switch (Cmd) {
        case LCD_X_INITCONTROLLER: {
            //
            // Called during the initialization process in order to set up the
            // display controller and put it into operation. If the display
            // controller is not initialized by any external routine, this needs
            // to be adapted by the customer...
            //
            // ...
            
            lcd_start_reset(); 
            
            //sendStartSequence(ILI9341_regValues_2_4,sizeof(ILI9341_regValues_2_4));
            
            // Screen 1: Works!!!
            // Screen 2: upside down
            // Screen 3: Upside down and red
            
            //sendStartSequence(mcu35_init_sequence_kbv,sizeof(mcu35_init_sequence_kbv));
            
            // Screen 1:
            // Screen 2:
            // Screen 3:
            //sendStartSequence(mcu35_init_9090,sizeof(mcu35_init_9090));
            
            // Screen 1: Works
            // Screen 2: Colors, inverted text
            // Screen 3: Colors hosed...
            //sendStartSequence(mcu35_init_sequence_web,sizeof(mcu35_init_sequence_web));
            
            // Screen 1: Inverted with correct color
            // Screen 2: jack and colormap screwed
            // Screen 3: Colormap fucked
            //_InitController35Web();
      
            // Screen 1: 
            // Screen 2: 
            // Screen 3: 
            sendStartSequence(mcu35_init_sequence_web_edited,sizeof(mcu35_init_sequence_web_edited));
            
            return 0;
        }
        default:
            r = -1;
    }
    return r;
}

/*************************** End of file ****************************/
