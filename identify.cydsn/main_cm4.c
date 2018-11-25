
#include "project.h"

#include "stdlib.h"

#include <stdio.h>

int main(void)
{

    __enable_irq(); /* Enable global interrupts. */

    UART_Start();
    setvbuf( stdin, NULL, _IONBF, 0 );
    printf("Started\r\n");
    
    GraphicLCDIntf_1_Start();

    /* Reset - High, Low (reset), High */
    Cy_GPIO_Set(LCD_RESET_N_0_PORT, LCD_RESET_N_0_NUM);
    CyDelay(200);
    Cy_GPIO_Clr(LCD_RESET_N_0_PORT, LCD_RESET_N_0_NUM);
    CyDelay(200);
    Cy_GPIO_Set(LCD_RESET_N_0_PORT, LCD_RESET_N_0_NUM);
    
    
    
    uint8_t data[4];
    
    while(1)
    {
        char c;
        c=getchar();
        switch(c)
        
        {
            case 'R': // Reset Display
                GraphicLCDIntf_1_Write8_A0(0x01); // Soft Reset
                printf("Reset Display\r\n");
                CyDelay(10);
                break;
           
         case 'S': // Sleep Off
                GraphicLCDIntf_1_Write8_A0(0x11); // Soft Reset
                printf("Sleep Off Display\r\n");
                CyDelay(10);
                break;
        case 'o': // On
                GraphicLCDIntf_1_Write8_A0(0x29);
                printf("On\r\n");
                break;
        case 'O':
                GraphicLCDIntf_1_Write8_A0(0x28); // Soft Reset
                printf("Off\r\n");
                break;
        
                case 'B':
                GraphicLCDIntf_1_Write8_A0(0x51); // Brightness
                GraphicLCDIntf_1_Write8_A1(0x0); // brightness value
                
                printf("Brightness 0\r\n");
                break;
          case 'b':
                GraphicLCDIntf_1_Write8_A0(0x51); // Brightness
                CyDelay(1);
                GraphicLCDIntf_1_Write8_A1(0x23); // Brightness value
                
                printf("Brightness 0x23\r\n");
                break;
           case 'n': /// Read Brightness
                 GraphicLCDIntf_1_Write8_A0(0x52);
                CyDelay(1);
                data[0] = GraphicLCDIntf_1_Read8_A1();
                CyDelay(1);
                data[1] = GraphicLCDIntf_1_Read8_A1();
                printf("Brightness = %02X \r\n",data[1]);
                break;
                
            case 'r':
                
                GraphicLCDIntf_1_Write8_A0(0xd3);
                data[0] = GraphicLCDIntf_1_Read8_A1();
                CyDelay(1);
                data[1] = GraphicLCDIntf_1_Read8_A1();
                CyDelay(1);
                data[2] = GraphicLCDIntf_1_Read8_A1();
                CyDelay(1);
                data[3] = GraphicLCDIntf_1_Read8_A1();
      
                //GraphicLCDIntf_1_ReadM8_A1((uint8_t *)&deviceid,2);
                printf("Device ID = %02X %02X %02X %02X\r\n",data[0],data[1],data[2],data[3]);
            break;
                case 'i':
                  GraphicLCDIntf_1_Write8_A0(0xda);
                    data[0] = GraphicLCDIntf_1_Read8_A1();
                      data[1] = GraphicLCDIntf_1_Read8_A1();
                    printf("ID1 = %02X\r\n",data[1]);
                  
                    GraphicLCDIntf_1_Write8_A0(0xdb);
                    data[0] = GraphicLCDIntf_1_Read8_A1();
                      data[1] = GraphicLCDIntf_1_Read8_A1();
                    printf("ID2 = %02X\r\n",data[1]);
                  
                    GraphicLCDIntf_1_Write8_A0(0xdc);
                    data[0] = GraphicLCDIntf_1_Read8_A1();
                      data[1] = GraphicLCDIntf_1_Read8_A1();
                    printf("ID3 = %02X\r\n",data[1]);
                  
                break;
            case '?':
                printf("R\tReset Display 0x01\r\n");
                printf("S\tSleep Off 0x11\r\n");
                printf("o\tDisplay On 0x29\r\n");
                printf("O\tDisplay On 0x28\r\n");
                printf("?\tHelp\r\n");
                printf("r\tRead manufacturer ID\r\n");
                printf("b\tSet Brightness to 0x23\r\n");
                printf("n\tRead Brightness\r\n");
                
            break;
        }
    }
}