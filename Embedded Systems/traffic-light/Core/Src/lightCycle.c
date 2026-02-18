#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "lightCycle.h"
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

uint32_t currentLights; //current state shown on the LEDs
uint32_t nextLights; //next state to be shown
uint32_t interruptLights; //updating light from interrupt source


int32_t extern currentWaitTime;

//NS start with green light
void init_Intersection(void){ // inital state
    set_TLight(NLight, Green); 
    set_TLight(SLight, Green);
    set_TLight(NSPed, Red); //NS ped is red and off
    blink_Ped_Indicator(NSPedIndicator, OFF, 0);

    set_TLight(ELight, Red);
    set_TLight(WLight, Red);//EW start with red
    set_TLight(EWPed, Green); // EW pedestrian start with green
    blink_Ped_Indicator(EWPedIndicator, OFF, 0);

    update_lights(0); //update hardware
}


//This function cycles the entire direction of the intersection
void cycle_Intersection(int direction){
    if (direction == 0){ //NS Green, warn EW lights
        set_TLight(ELight, Yellow);
        set_TLight(WLight, Yellow);
        update_lights(0);

        delay(orangeDelay);
        //stop EW traffic & prepare NS
        set_TLight(ELight, Red);
        set_TLight(WLight, Red);
        set_TLight(NSPed, Red);
        set_TLight(EWPed, Green);
        set_TLight(NLight, RedYellow);
        set_TLight(SLight, RedYellow);
        update_lights(0);

        delay(orangeDelay);
//NS traffic goes green
        set_TLight(NLight, Green);
        set_TLight(SLight, Green);
        blink_Ped_Indicator(EWPedIndicator, OFF, 0);
        update_lights(0);
        //done

    }else{ //EW switches to Green
        set_TLight(NLight, Yellow);
        set_TLight(SLight, Yellow);
        update_lights(0);

        delay(orangeDelay);
//stop NS traffic & prepare EW
        set_TLight(NLight, Red);
        set_TLight(SLight, Red);
        set_TLight(EWPed, Red);
        set_TLight(NSPed, Green);
        set_TLight(ELight, RedYellow);
        set_TLight(WLight, RedYellow);
        update_lights(0);

        delay(orangeDelay);
//EW light goes green

        set_TLight(ELight, Green);
        set_TLight(WLight, Green);
        blink_Ped_Indicator(NSPedIndicator, OFF, 0);
        update_lights(0);
        //done
    }
}

//This function turns the Ped indicator on or of for a certain light.
void blink_Ped_Indicator(int ILight, int state, int source){
    //state
    if(state == OFF){
        set_TLight(ILight, OFF);
    }else if(state == ON){
        set_TLight(ILight, ON);
    }//update immeditaly if called from interrupt
    if(source == 1){
        update_lights(source);
    }
}


//This function uses defined TrafficLights and colors to set the bitmask nextLihgts(and sometimes interruptLights)
//to the right values. TL is the light to be changed, color is what color the light should show.
void set_TLight(int TL, int color){
    if(ELight >= TL && TL >= WLight){
        //clear previous colour  bits
        nextLights = nextLights & ~(0b111 << TL*8);

        //set the bits
        nextLights = nextLights | (color << TL*8);
    }else if(TL == NLight){
        //clear the bits, special case for north.
        nextLights = nextLights & ~(0b111 << 19);

        //set the bits
        nextLights = nextLights | (color << 19);
    }else if( NSPed >= TL && TL >= EWPed){
        //clear the pedes bits
        nextLights = nextLights & ~(0b11 << ((TL-4)*8+3));

        //set the bits
        if(color == Green){
            nextLights = nextLights | (2 << ((TL-4)*8+3));
        }else{
            nextLights = nextLights | (1 << ((TL-4)*8+3));
        }
    }else if(NSPedIndicator >= TL && TL >= EWPedIndicator){ //ped indictation leds
        if(color == ON){
            nextLights = nextLights | (1 << ((TL-6)*8+5));
            interruptLights = currentLights | (1 << ((TL-6)*8+5));

        }else{//clear the bit
            nextLights = nextLights & ~(1 << ((TL-6)*8+5));
            interruptLights = currentLights & ~(1 << ((TL-6)*8+5));
        }

    }else if(TL == AllLights){
        if(color == ON){//set all bits
            nextLights = 0xFFFFFF;
        }else{//clear all bits
            nextLights = 0x0;
        }
    }else{
        //wrong input, error indic
        nextLights = 0x100000;
    }


}

//This functions sends the bitmask currentLights vairable to the shift registers (or interruptLights)
//is the update form the cycle, then source = 0, or interrupts then source = 1
void update_lights(int source) {
    if(source == 0){ //choose light buffer
        currentLights = nextLights;
    }else{
        currentLights = interruptLights;
    }

    uint8_t tx[3] = {//split 24 byte into 3 bytes
        (currentLights >> 16) & 0xFF,   // U3, MSB
        (currentLights >> 8)  & 0xFF,   // U2
        (currentLights >> 0)  & 0xFF    // U1, LSB
    };
//latch to low before shifting
    HAL_GPIO_WritePin(Shift_Latch_GPIO_Port, Shift_Latch_Pin, GPIO_PIN_RESET);

    // Sending 24-bit value (3 bytes) through SPI to shift registers
    HAL_SPI_Transmit(&hspi3, tx, 3, HAL_MAX_DELAY);

    HAL_GPIO_WritePin(Shift_Latch_GPIO_Port, Shift_Latch_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Shift_Latch_GPIO_Port, Shift_Latch_Pin, GPIO_PIN_RESET);

    // Prepare next cycle
    nextLights = currentLights;
}


//This function uses the global variable currentWaitTime to calculate delays.
void delay(int delaytime){
    int timestamp = currentWaitTime;//wait until required time has lapsed
    while ((int)currentWaitTime - timestamp < delaytime){
        //wait
    }
    return;
}

