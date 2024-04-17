#include "LCD_Test.h"
#include "LCD_2in.h"
#include "hardware/irq.h"
#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/timer.h" 
#include "GUI_Paint.h"
#include "hardware/gpio.h"
#include <stdlib.h>
#include "pico/multicore.h"
#include "hardware/dma.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "pico/binary_info.h"
#include <string.h>



#define ADC_NUM 0
#define ADC_PIN (26 + ADC_NUM)
#define ADC_VREF 3.3
#define ADC_RANGE (1 << 12)
#define ADC_CONVERT (ADC_VREF / (ADC_RANGE - 1))


#define ALARM_NUM 0
#define ALARM_IRQ TIMER_IRQ_0
#define PI 3.14159265
#define SAMPLES 100
#define ALARM_NUM 0
#define ALARM_IRQ TIMER_IRQ_0
#define LENGHT 320

#define CAPTURE_CHANNEL 0
#define CAPTURE_DEPTH 1000



uint8_t capture_buf[CAPTURE_DEPTH];


static volatile bool alarm_fired;
 int valuePassed =0;
   int key0 = 15; 
   int key1 = 17; 
   int key2 = 2; 
   int key3 = 3; 
    int YcoordinateTemp[LENGHT] ={ [0 ... LENGHT-1] = 120};
   int i;
   double value;
   double start =0;
   int xValue =0;
   int k;
   char rString[20];
   char startString[20];
   int Xcoordinate [LENGHT];
    int Ycoordinate[LENGHT];
    int intValue;
uint adc_raw;

   char testString[100];
   int colour[13] = {0xFFFF,0xFFE0,0x001F,0XF81F,0XFFE0,0X07FF,0xF800,0xF81F,0x07E0,0x7FFF,0XBC40,0XFC07,0X8430};
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;

UWORD *BlackImage;


static void fillXArray()
{
    for (i =0;i<LENGHT;i++)
    {
        Xcoordinate[i] = i;
    }
}







        //All the following code is for array shifting method
// static void alarm_irq(void) {
//     int valueToPush;
//     hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM);  
//     adc_raw = adc_read(); // raw voltage from ADC
//     valueToPush = adc_raw *ADC_CONVERT*10000;
//     multicore_fifo_push_blocking(valueToPush);
//     //printf("%d\n",valueToPush/1000*120);
//     //printf("%d\n",valueToPush);
//     //multicore_fifo_push_blocking((uint32_t) xValue);
//     alarm_fired = true;
//     //xValue++;

// }

// void core1_interrupt_handler() {
// int Yvalue;
// float valueToPop;
// int valueToPopInt;
//                 for (int i = 0; i < LENGHT; i++)
//         {
//             Paint_DrawLine(Xcoordinate[i],YcoordinateTemp[i], Xcoordinate[i+1], YcoordinateTemp[i+1],BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//         }
//     while (multicore_fifo_rvalid()){
//     //setPins();
//     //Paint_Clear(BLACK);

// //     for (int i = 0; i < LENGHT; i++)
// // {
// //     Paint_DrawLine(i,YcoordinateTemp[i]*2, i+1, YcoordinateTemp[i+1]*2,BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
// // }  
//     Yvalue = multicore_fifo_pop_blocking();
//     valueToPop = Yvalue;
//     valueToPop =(valueToPop/10000)*44;
//     valueToPopInt = (int)valueToPop;
//     printf("%d\n",valueToPopInt);
//     memmove(YcoordinateTemp, YcoordinateTemp+1, sizeof(YcoordinateTemp)-sizeof(YcoordinateTemp[0]));
//     YcoordinateTemp[LENGHT-1] =valueToPopInt;
//     //memmove(YcoordinateTemp, YcoordinateTemp+1, sizeof(YcoordinateTemp)-sizeof(YcoordinateTemp[0]));
//     // printf("%d\n",YcoordinateTemp[LENGHT-3]);
//     // printf("%d\n",YcoordinateTemp[LENGHT-2]);
//     // printf("%d\n",YcoordinateTemp[LENGHT-1]);
//         for (int i = 0; i < LENGHT-1; i++)
//         {  
//             //printf("%d\n",YcoordinateTemp[i]);
//             Paint_DrawLine(Xcoordinate[i],YcoordinateTemp[i], Xcoordinate[i+1], YcoordinateTemp[i+1],RED, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//             //Paint_DrawLine(i,YcoordinateTemp[i]*2, i+1, YcoordinateTemp[i+1]*2,YELLOW, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
//         }


//     //printf("End of the array");

//     }  
//     multicore_fifo_clear_irq();

//     // Clear interrupt
    
// }




// //works fine. too fast - pass full array
// static void alarm_irq(void) {
//     hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM);  
//     int Ycoordinate[LENGHT];
//     float floatValue;
//     int intValue;
//     for (i = 0; i < LENGHT; ++i) {
//         adc_raw = adc_read(); // raw voltage from ADC
//         floatValue = adc_raw *ADC_CONVERT;
//         floatValue = floatValue*240/3.3;
//         intValue = (int) floatValue;
//         Ycoordinate[i] = intValue;   
//     }
//     for (int z =0; z <LENGHT;z++)
//     {
//         printf("%d\n",Ycoordinate[z]);
//         multicore_fifo_push_blocking((uint32_t) Ycoordinate[z]);
//     }
//     alarm_fired = true;
    

// }

static void alarm_irq(void) {
    //printf("Hello");
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM);  
    float floatValue;

    adc_raw = adc_read(); // raw voltage from ADC
    floatValue = adc_raw *ADC_CONVERT;
    floatValue = floatValue*240/3.3;
    intValue = (int) floatValue;
    if(valuePassed==0){
        //printf("%d\n",intValue);
        if(!(intValue== 120))
        {
            //printf("0:%d",intValue);
            alarm_fired = true;
            return;
        }
    }
    if(valuePassed ==1 || valuePassed ==2 || valuePassed ==3 || valuePassed ==4 || valuePassed ==5 || valuePassed ==6 || valuePassed ==7)
    {
        if(intValue<120){
            alarm_fired=true;
            return;
        }
    }

        //printf("ADC: %d \n",intValue);
        memmove(Ycoordinate, Ycoordinate+1,sizeof(Ycoordinate)-sizeof(Ycoordinate[0])); 
        Ycoordinate[LENGHT-1]=intValue;
        
    
        //printf("Last: %d\n",Ycoordinate[LENGHT-1]);
        //printf("Second Last: %d\n",Ycoordinate[LENGHT-2]);
        //printf("Third Last: %d\n",Ycoordinate[LENGHT-3]);
        valuePassed++;
    
        if(valuePassed == LENGHT){
            for (int z =0; z <LENGHT;z++)
            {
                //printf("%d\n",Ycoordinate[z]);
                multicore_fifo_push_blocking((uint32_t) Ycoordinate[z]);
                //printf("%d/n",valuePassed);
                
            }
            //printf("HI ANTOINE");
            valuePassed=0;
        }

    
    alarm_fired = true;
    

}









static void alarm_in_us(uint32_t delay_us) {
    hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);
    irq_set_exclusive_handler(ALARM_IRQ, alarm_irq);
    irq_set_enabled(ALARM_IRQ, true);
    uint64_t target = timer_hw->timerawl + delay_us;
    timer_hw->alarm[ALARM_NUM] = (uint32_t) target;
}

void core1_interrupt_handler() {
int YcoordinateTemp[LENGHT];
int Yvalue;
int randomColour =0;
// for (size_t i = 0; i < LENGHT; i++)
// {
//     Paint_DrawLine(Xcoordinate[i],YcoordinateTemp[i], Xcoordinate[i+1], YcoordinateTemp[i+1],BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
// }
    
    while (multicore_fifo_rvalid()){
    Paint_Clear(BLACK);        
    for (int z =0; z <LENGHT;z++)
    {
        Yvalue = (int) multicore_fifo_pop_blocking();

        YcoordinateTemp[z] =Yvalue;
    
    }
                for (size_t i = 0; i < LENGHT; i++)
                {
                   
                    // if (randomColour ==14)
                    // {
                    //     randomColour =0;
                    // }
                    randomColour = rand() % 13;

                    //printf("%d\n",randomColour);
                    Paint_DrawLine(Xcoordinate[i],YcoordinateTemp[i], Xcoordinate[i+1], YcoordinateTemp[i+1],colour[randomColour], DOT_PIXEL_1X1, LINE_STYLE_SOLID);
                    //randomColour++;
                }
        LCD_2IN_Display((uint8_t * )BlackImage);
    }
    multicore_fifo_clear_irq(); // Clear interrupt
    
}
// Core 1 Main Code
void core1_entry() {
    // Configure Core 1 Interrupt

    multicore_fifo_clear_irq();
    irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_interrupt_handler);

    irq_set_enabled(SIO_IRQ_PROC1, true);

    // Infinte While Loop to wait for interrupt
    while (1){
        tight_loop_contents();
    }
}

int main(void)
{

    stdio_init_all();
    
    fillXArray();
    multicore_launch_core1(core1_entry); 
    if(DEV_Module_Init()!=0){
        return -1;
    }
    DEV_SET_PWM(50);
    LCD_2IN_Init(HORIZONTAL);
    LCD_2IN_Clear(BLACK);
    UDOUBLE Imagesize = LCD_2IN_HEIGHT*LCD_2IN_WIDTH*2;
    if((BlackImage = (UWORD *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for  black memory...\r\n");
        exit(0);
    }
    Paint_NewImage((UBYTE *)BlackImage,LCD_2IN.WIDTH,LCD_2IN.HEIGHT, 90, BLACK);
    Paint_SetScale(65);
    Paint_Clear(BLACK);
    Paint_SetRotate(ROTATE_270);
    adc_init();
    adc_gpio_init( ADC_PIN);
    adc_select_input( ADC_NUM);

    //     while (1) {
    //     adc_raw = adc_read(); // raw voltage from ADC
    //     printf("%.2f\n", adc_raw * ADC_CONVERT);
    //     sleep_ms(10);
    // }
    
   while(1){
        alarm_in_us(22);
        printf(" ");
        //sleep_ms(1);
        while(!alarm_fired);       
   }

    /* Module Exit */
    free(BlackImage);
    BlackImage = NULL;
    DEV_Module_Exit();
    return 0;
}





