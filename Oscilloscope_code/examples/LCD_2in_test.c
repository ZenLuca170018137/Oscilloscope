
/*****************************************************************************
* | File      	:   LCD_2in_test.c
* | Author      :   Waveshare team
* | Function    :   2inch LCD test demo
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2021-08-20
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "LCD_Test.h"
#include "LCD_2in.h"
#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/irq.h"


#define PI 3.14159265
#define SAMPLES 100
#define ALARM_NUM 0
#define ALARM_IRQ TIMER_IRQ_0

static volatile bool alarm_fired;

bool reserved_addr(uint8_t addr) {
return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

static uint64_t get_time(void) {
    // Reading low latches the high value
    uint32_t lo = timer_hw->timelr;
    uint32_t hi = timer_hw->timehr;
    return ((uint64_t) hi << 32u) | lo;
}
static void alarm_irq(void) {
    // Clear the alarm irq
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM);

    // Assume alarm 0 has fired
    printf("Alarm IRQ fired\n");
    alarm_fired = true;
}
static void alarm_in_us(uint32_t delay_us) {
    // Enable the interrupt for our alarm (the timer outputs 4 alarm irqs)
    hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);
    // Set irq handler for alarm irq
    irq_set_exclusive_handler(ALARM_IRQ, alarm_irq);
    // Enable the alarm irq
    irq_set_enabled(ALARM_IRQ, true);
    // Enable interrupt in block and at processor

    // Alarm is only 32 bits so if trying to delay more
    // than that need to be careful and keep track of the upper
    // bits
    uint64_t target = timer_hw->timerawl + delay_us;

    // Write the lower 32 bits of the target time to the alarm which
    // will arm it
    timer_hw->alarm[ALARM_NUM] = (uint32_t) target;
}

int LCD_2in_test(void)
{

    DEV_Delay_ms(100);
    printf("LCD_2in_test Demo\r\n");
    if(DEV_Module_Init()!=0){
        return -1;
    }
    DEV_SET_PWM(50);
    /* LCD Init */
    printf("2inch LCD demo...\r\n");
    LCD_2IN_Init(HORIZONTAL);
    LCD_2IN_Clear(BLACK);
    
    //LCD_SetBacklight(1023);
    UDOUBLE Imagesize = LCD_2IN_HEIGHT*LCD_2IN_WIDTH*2;
    UWORD *BlackImage;
    if((BlackImage = (UWORD *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for  black memory...\r\n");
        exit(0);
    }
    // /*1.Create a new image cache named IMAGE_RGB and fill it with white*/
    Paint_NewImage((UBYTE *)BlackImage,LCD_2IN.WIDTH,LCD_2IN.HEIGHT, 90, BLACK);
    Paint_SetScale(65);
    Paint_Clear(BLACK);
    Paint_SetRotate(ROTATE_270);
    // /* GUI */
    printf("drawing...\r\n");
    // /*2.Drawing on the image*/
    
// #if 1
//     Paint_DrawPoint(2,1, BLACK, DOT_PIXEL_1X1,  DOT_FILL_RIGHTUP);//240 240
//     Paint_DrawPoint(2,6, BLACK, DOT_PIXEL_2X2,  DOT_FILL_RIGHTUP);
//     Paint_DrawPoint(2,11, BLACK, DOT_PIXEL_3X3, DOT_FILL_RIGHTUP);
//     Paint_DrawPoint(2,16, BLACK, DOT_PIXEL_4X4, DOT_FILL_RIGHTUP);
//     Paint_DrawPoint(2,21, BLACK, DOT_PIXEL_5X5, DOT_FILL_RIGHTUP);
//     Paint_DrawLine( 10,  5, 40, 35, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
//     Paint_DrawLine( 10, 35, 40,  5, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

//     Paint_DrawLine( 80,  20, 110, 20, CYAN, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
//     Paint_DrawLine( 95,   5,  95, 35, CYAN, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

//     Paint_DrawRectangle(10, 5, 40, 35, RED, DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
//     Paint_DrawRectangle(45, 5, 75, 35, BLUE, DOT_PIXEL_2X2,DRAW_FILL_FULL);

//     Paint_DrawCircle(95, 20, 15, GREEN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
//     Paint_DrawCircle(130, 20, 15, GREEN, DOT_PIXEL_1X1, DRAW_FILL_FULL);


//     Paint_DrawNum (50, 40 ,9.87654321, &Font20,5,  WHITE,  BLACK);
//     Paint_DrawString_EN(1, 40, "ABC", &Font20, 0x000f, 0xfff0);
//     Paint_DrawString_CN(1,60, "��ӭʹ��",  &Font24CN, WHITE, BLUE);
//     Paint_DrawString_EN(1, 100, "WaveShare", &Font16, RED, WHITE); 

//     // /*3.Refresh the picture in RAM to LCD*/
//     LCD_2IN_Display((UBYTE *)BlackImage);
//     DEV_Delay_ms(1000);

// #endif
// #if 1
//      Paint_DrawImage1(gImage_2inch_1,0,0,320,240);
//      LCD_2IN_Display((UBYTE *)BlackImage);
//      DEV_Delay_ms(2000);
     
// #endif
	// 4.Test button
   int key0 = 15; 
   int key1 = 17; 
   int key2 = 2; 
   int key3 = 3; 

   int i;
   double value;
   double start;
   int k;

   double Xcoordinate [1000];
   double Ycoordinate[1000];
   
   SET_Infrared_PIN(key0);    
   SET_Infrared_PIN(key1);
   SET_Infrared_PIN(key2);
   SET_Infrared_PIN(key3);

   Paint_Clear(BLACK);
   LCD_2IN_Display((uint8_t * )BlackImage);
   stdio_init_all();
    printf("Timer lowlevel!\n");
    #ifndef PICO_DEFAULT_LED_PIN
    #warning blink example requires a board with a regular LED
    #else
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    #endif
   
   while(1){
    alarm_fired = false;
        alarm_in_us(100000);
        gpio_put(LED_PIN, 1);
        Paint_DrawRectangle(288, 208, 308, 228, RED, DOT_PIXEL_1X1,DRAW_FILL_FULL);
        // Wait for alarm to fire
        //printf("heeeeeee");
        Paint_DrawString_EN(70, 100, " Life is pain", &Font20, WHITE, BLACK);
        while (!alarm_fired);
        gpio_put(LED_PIN, 0);


   	// //Paint_DrawString_EN(70, 100, " Life is pain", &Font20, WHITE, BLACK);
    // //    if(DEV_Digital_Read(key0 ) == 0){
    //         Paint_NewImage((UBYTE *)BlackImage,LCD_2IN.WIDTH,LCD_2IN.HEIGHT, 90, WHITE);
    //         Paint_SetScale(65);
    //         Paint_Clear(BLACK);
    //         Paint_SetRotate(ROTATE_270);
           
    //         start = start +10;


    //             for (i = 0, value = 0; i < 1000; ++i, value += 0.9) {
    //                 Xcoordinate[i] = value;
    //             }
    //             for (int i = 0, value = start; i < 1000; ++i, value += 1) {
    //                 Ycoordinate[i] = 120-(sin(value*(PI/180))*100);
    //             }


    //             int lenght = sizeof(Xcoordinate)/sizeof(Xcoordinate[0]);
    //             for (size_t i = 0; i < lenght; i++)
    //             {
    //                 Paint_DrawLine(Xcoordinate[i],Ycoordinate[i], Xcoordinate[i+1], Ycoordinate[i+1],RED, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

    //             }

                
    //             Paint_DrawLine(0,120, 320,120,WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    //             for (size_t i = 0; i < 320; i += 40)
    //             {
    //                 Paint_DrawLine(i,120, i,100,WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    //                 Paint_DrawLine(i,120, i,140,WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
                    
    //             }
    //             //DEV_Delay_ms(1000);

            

 
      
        
                    
    // //    }else  {
    // //    	Paint_DrawRectangle(288, 208, 308, 228, RED, DOT_PIXEL_1X1,DRAW_FILL_FULL);
    // //    }
    //     if(DEV_Digital_Read(key1 ) == 0){

            
    //         Paint_NewImage((UBYTE *)BlackImage,LCD_2IN.WIDTH,LCD_2IN.HEIGHT, 90, WHITE);
    //         Paint_SetScale(65);
    //         Paint_Clear(BLACK);
    //         Paint_SetRotate(ROTATE_270);
            
    //         for (i = 0, value = 0; i < 1000; ++i, value += 0.9) {
    //             Xcoordinate[i] = value;
    //         }
    //         for (int i = 0, value = 0; i < 1000; ++i, value += 1) {
    //             Ycoordinate[i] = 120+(sin(value*(PI/180))*100);
    //         }


    //         int lenght = sizeof(Xcoordinate)/sizeof(Xcoordinate[0]);
    //         for (size_t i = 0; i < lenght; i++)
    //         {
    //             Paint_DrawLine(Xcoordinate[i],Ycoordinate[i], Xcoordinate[i+1], Ycoordinate[i+1],RED, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    //         }
    //         Paint_DrawLine(0,120, 320,120,WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    //         for (size_t i = 0; i < 320; i += 40)
    //         {
    //             Paint_DrawLine(i,120, i,100,WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    //             Paint_DrawLine(i,120, i,140,WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    //         }
        
                
    //    }
           

    //    if(DEV_Digital_Read(key2) == 0 || DEV_Digital_Read(key3) == 0){
    //         Paint_NewImage((UBYTE *)BlackImage,LCD_2IN.WIDTH,LCD_2IN.HEIGHT, 90, WHITE);
    //         Paint_SetScale(65);
    //         Paint_Clear(BLACK);
    //         Paint_SetRotate(ROTATE_270);

    //         Paint_DrawString_EN(70, 100, " Life is pain", &Font20, WHITE, BLACK);

    //    }
       
    // //    if(DEV_Digital_Read(key3 ) == 0){
       
           
    // //         Paint_DrawRectangle(0, 0, 308, 32, GREEN, DOT_PIXEL_1X1,DRAW_FILL_FULL);

    // //    }else{
           
    // //         Paint_DrawRectangle(288, 12, 308, 32, RED, DOT_PIXEL_1X1,DRAW_FILL_FULL);   
      
    // //    }		
		LCD_2IN_Display((uint8_t * )BlackImage);             
   }

    /* Module Exit */
    free(BlackImage);
    BlackImage = NULL;
    
    
    DEV_Module_Exit();
    return 0;
}