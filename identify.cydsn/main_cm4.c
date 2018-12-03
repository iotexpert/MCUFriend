
#include "project.h"

#include "stdlib.h"

#include <stdio.h>
#define LCD_COMMAND (0)
#define LCD_DATA (1)

void lcdReset()
{
       /* Reset - High, Low (reset), High */
    Cy_GPIO_Set(LCD_RESET_N_0_PORT, LCD_RESET_N_0_NUM);
    CyDelay(200);
    Cy_GPIO_Clr(LCD_RESET_N_0_PORT, LCD_RESET_N_0_NUM);
    CyDelay(200);
    Cy_GPIO_Set(LCD_RESET_N_0_PORT, LCD_RESET_N_0_NUM);
    CyDelay(200);
   
    GraphicLCDIntf_1_Write8(LCD_COMMAND,0x01);
 
}

void regReadPrint(uint8_t reg,uint8_t *buff,uint16_t num)
{
    GraphicLCDIntf_1_Write8(LCD_COMMAND,reg);
    GraphicLCDIntf_1_ReadM8(LCD_DATA,buff,num);
    printf("%02X ",reg);
    for(int i=0;i<num;i++)
    {
        printf("0x%02X,",buff[i]);
    }
    printf("\r\n");
    
}

int main(void)
{
    uint8_t buff[128];

    __enable_irq(); /* Enable global interrupts. */

    UART_Start();
    setvbuf( stdin, NULL, _IONBF, 0 );
    printf("Started\r\n");
    
    GraphicLCDIntf_1_Start();
    lcdReset();
    
    while(1)
    {
        char c;
        c=getchar();
        
        switch(c)
        {  
            case 'a':
                regReadPrint(0x04,buff,6);
                regReadPrint(0xA1,buff,6);
                regReadPrint(0xBF,buff,6);
                regReadPrint(0xDA,buff,1);
                regReadPrint(0xDB,buff,1);
                regReadPrint(0xDC,buff,1);
                regReadPrint(0xd3,buff,6);
            break;
            
            case '1':
                regReadPrint(0xDA,buff,2);
            break;
            case '2':
                regReadPrint(0xDB,buff,2);
            break;
            case '3':
                regReadPrint(0xDC,buff,2);
            break;
            
            case 'R': // Reset Display
                printf("LCD Reset\r\n");
                lcdReset();
                break;
           
            case '4':
                regReadPrint(0x04,buff,6);
                break;
               
            case 'b':
                regReadPrint(0xBF,buff,6);
                break;
                
            case 'd':
                regReadPrint(0xd3,buff,6);
                break;

            case '?':
                printf("R\tReset Display 0x01\r\n");
                printf("b\tRead Register 0xBF\r\n");
                printf("4\tRead Register 0x04\r\n");
                printf("d\tRead Register 0xd3\r\n");
                
                printf("?\tHelp\r\n");
            break;
        }
    }
}