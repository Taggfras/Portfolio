#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "lightCycle.h"
#include "intersectionContol.h"
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

//0 is inactive, 1 is button/switch active
uint8_t NCar;
uint8_t SCar;
uint8_t ECar;
uint8_t WCar;
uint8_t NSPed_btn; 
uint8_t EWPed_btn;

uint32_t currentWaitTime = 0; //ms
uint32_t redWaitTimeStamp = 0;
uint8_t currentDir = NS;
uint8_t resetWait;

void intersection(void){
    init_Intersection();
    while(1){
        start:
        //checks if switch can be made now
        if(currentDir == NS){
            if ((ECar == 1 || WCar == 1) && (NCar == 0 && SCar == 0)){
                toggle_intersection(); 
            }
        }else if(currentDir == EW){
            if ((NCar == 1 || SCar == 1) && (ECar == 0 && WCar == 0)){
                toggle_intersection(); 
            }
        }else if(currentWaitTime - redWaitTimeStamp >= redDelayMax){
            toggle_intersection();    
        }
        else if(currentWaitTime >= greenDelay){
            toggle_intersection();
        }else{
            //Dont toggle, wait
            HAL_Delay(10);
            currentWaitTime += 10;
            goto start;
        }

        //Did toggle in some way
        redWaitTimeStamp = 1000 + greenDelay; //To make the timestamp lie outside the possible interval
        currentWaitTime = 0;
        resetWait = 0;
    }
}

void toggle_intersection(void){
    resetWait = 1;
    if(currentDir == NS){
        cycle_Intersection(EW);
    }else{
        cycle_Intersection(NS);
    }
    return;
}


//handle_exeption ---
void handle_exeption(int exeption){
    if(NSPed_btn == 1){
        set_TLight(NSPedIndicator, ON);
    }else if(EWPed_btn == 1){
        set_TLight(NSPedIndicator, ON);
    }
}


//---



