//Display draw and initialise
#include "LCD_Test.h"
#include "LCD_2in.h"
#include "GUI_Paint.h"

//Interrupt
#include "hardware/irq.h"

//Standard libraries and Pico initialise
#include <stdio.h>
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdlib.h>
#include <string.h>
#include "pico/binary_info.h"

//Math
#include <math.h>

//Timer
#include "hardware/timer.h" 

//Multicore
#include "pico/multicore.h"

//ADC libraries
#include "hardware/dma.h"
#include "hardware/adc.h"
#include "hardware/uart.h"





//Defined variables
#define ADC_NUM 0
#define ADC_PIN (26 + ADC_NUM)
#define ADC_VREF 3.3
#define ADC_RANGE (1 << 12)
#define ADC_CONVERT (ADC_VREF / (ADC_RANGE - 1))
#define ALARM_NUM 0
#define ALARM_IRQ TIMER_IRQ_0
#define LENGTH 320

//Variables
static volatile bool interrupt_triggered;
int valuePassed =0; 
int coordinateYDisplay[LENGTH] ={ [0 ... LENGTH-1] = 120};
int potentiometerInt =0;
int coordinateY[LENGTH];
int intSignalRead;
uint adc_raw;
UWORD *BlackImage;


//Core 0 function activated by the interrupt
static void interrupt_irq(void) {

    //Clear the interrupt timer
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM);  

    //Value used to stored the 
    float floatSignalRead;

    //Select first ADC (connected to signal generator)
    adc_select_input(0);

    //Read raw data from the ADC and convert into a float
    adc_raw = adc_read();
    floatSignalRead = adc_raw *ADC_CONVERT;

    //Adjust the value of the read to be any number between 0 and 240
    //240 is the maximum height of the screen
    floatSignalRead = floatSignalRead*240/3;

    //Convert the float to int
    intSignalRead = (int) floatSignalRead;

    //Value used for the locking potentiometer
    float potentiometerValue;

    //Check if the value read is the first value
    //This is used to get the locking value
    if (valuePassed ==0)
    {
        //If it is the first value, switch ADC to the second one, locking potentiometer one
        adc_select_input(1);

        //Use the same technique as before to read the value from the ADC
        //store it into a variable and convert it to an int between 0 and 240
        uint adc_potentiometer;
        adc_potentiometer = adc_read();
        potentiometerValue = adc_potentiometer * ADC_CONVERT;
        potentiometerValue = potentiometerValue*240/3.3;
        potentiometerInt = (int) potentiometerValue;    
    }

    //Check if the value read is the first value
    //This is used in case the first value is not the locking value
    if(valuePassed==0){
        if(!(intSignalRead == potentiometerInt))
        {
            interrupt_triggered = true;
            return;
        }
    }

    //Check the first 7 values read. This is used to always set the wave to one specific pattern
    //e.g a sine wave locking value could be 120 (middle of the wave) but the next value could be either 119 or 121
    //as one cycle passes through that value at least twice. This method ensure that only one instance of that value is used
    if(valuePassed ==1 || valuePassed ==2 || valuePassed ==3 || 
       valuePassed ==4 || valuePassed ==5 || valuePassed ==6 || valuePassed ==7)
    {
        //Check if the value read is lower than the locking value
        //Only register values greater than the locking value, hence the wave is going upwards
        //The direction of the wave was chosen arbitrarily
        if(intSignalRead<potentiometerInt){

            //If it is then the wave is going down so ignore the values
            interrupt_triggered=true;
            return;
        }
    }

    //memmove is a standard library function used moving blocks of memory
    //Here it is used for array shifting, shifting the coordinateY array to the left by 1 position
    memmove(coordinateY, coordinateY+1,sizeof(coordinateY)-sizeof(coordinateY[0])); 

    //Make the last element in the array the same as the value read from the signal generator
    coordinateY[LENGTH-1]=intSignalRead;

    //Add one to the counter of how many values have been passed
    valuePassed++;

    //Check if values passed are as many as the array length (array is full)
    if(valuePassed == LENGTH){

        //If the array is full
        for (int z =0; z <LENGTH;z++)
        {
            //Use FIFO queue to push each value of the array to the other core
            multicore_fifo_push_blocking((uint32_t) coordinateY[z]);
        }

        //Reset the valuePassed to 0 for the next instance
        valuePassed=0;
    }
    
    //Set the interrupt to true as it has completed its job
    interrupt_triggered = true;
}








//Interrupt triggering handler function
//@params delay_us time entered by the user, used for length of each interrupt cycle
static void interrupts_in_us(uint32_t delay_us) {
    //Initial setting of the interrupt
    hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);

    //interrupt_irq is the function that is executed when the interrupt is called
    irq_set_exclusive_handler(ALARM_IRQ, interrupt_irq);

    //Set the remaining variables in order for the interrupt to work
    irq_set_enabled(ALARM_IRQ, true);
    uint64_t target = timer_hw->timerawl + delay_us;
    timer_hw->alarm[ALARM_NUM] = (uint32_t) target;
}


//Core 1 function, used for visualisation
void core1_interrupt_handler() {

    //Create the variables needed
    int coordinateYDisplay[LENGTH];
    int valueY;

    //While loop valid as long as there are items in the FIFO queue
    while (multicore_fifo_rvalid()){

        //Variables for storing  and converting the ADC raw data for timescaling
        uint adc_timescale;
        float timescaleFloatSignalRead;
        int timescaleIntSignalRead;

        //Variables for the stretch constant to extend the wave
        float stretchFloat;
        int stretch;

        //Select the ADC2 port (third ADC connected to the timescaling potentiometer)
        adc_select_input(2);

        //Read the ADC raw data and convert into a value betwen 0 and 320 (horizontal size of the screen)
        adc_timescale = adc_read(); // raw voltage from ADC
        timescaleFloatSignalRead = adc_timescale *ADC_CONVERT;
        timescaleFloatSignalRead = timescaleFloatSignalRead*320/3.3;
        timescaleIntSignalRead = (int) timescaleFloatSignalRead;

        //Calculate the stretch factor to used to fill the display according to the timescale
        stretchFloat = 1/(timescaleFloatSignalRead/320);
        stretch = (int) stretchFloat;

        //Clear the screen
        Paint_Clear(BLACK);

        //For loop to pop every item from the FIFO queue        
        for (int z =0; z <LENGTH;z++)
        {
            //Store the value into a variable
            valueY = (int) multicore_fifo_pop_blocking();

            //Add that value to an array
            coordinateYDisplay[z] = valueY;
        }

        //For loop to draw a line on display between each item of the array, hence the whole waveform
        for (size_t i = 0; i < timescaleIntSignalRead; i++)
        {          
            //Function from library to draw lines on display
            Paint_DrawLine(i*stretchFloat,coordinateYDisplay[i], (i+1)*stretchFloat, coordinateYDisplay[i+1],
            WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
        }
            //Send the image into the buffer to display it (from library)
            LCD_2IN_Display((uint8_t * )BlackImage);
    }

    //Clear the queue when all code is stopped being executed
    multicore_fifo_clear_irq(); 
    
}

// Core 1 Main Code
void core1_entry() {

    //Initiate the FIFO queue
    multicore_fifo_clear_irq();

    //Initiate the core 1 handler for all the necessary functions in core 1
    irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_interrupt_handler);
    irq_set_enabled(SIO_IRQ_PROC1, true);

    // Infinte While Loop to wait for interrupt
    while (1){
        tight_loop_contents();
    }
}

int main(void)
{
    //Initialise pins
    stdio_init_all();

    //Initialise the board
        if(DEV_Module_Init()!=0){
        return -1;
    }   

    //Initialiase core 1 for future workload
    multicore_launch_core1(core1_entry); 


    //Initialise the display, setting brightness, background colour and alignment
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

    //Initialise the adc reading algorithm
    adc_init();
    adc_gpio_init( ADC_PIN);
    adc_select_input(ADC_NUM);

    //While loop where the main body of the algorithm is executed
    while(1)
    {
        //Set the interrupt to false
        interrupt_triggered = false;

        //Start the interrupt
        interrupts_in_us(22);

        //Keep the ongoing operation until the interrupt finished a cylce
        while(!interrupt_triggered); 

        //Once the interrupt has finished a cycle it will restart      
    }    

    //Free up all the memory used as well as close the main function
    free(BlackImage);
    BlackImage = NULL;
    DEV_Module_Exit();
    return 0;
}





