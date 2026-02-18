#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "lightCycle.h"
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

uint32_t currentLights;
uint32_t nextLights;

void init_Intersection(void){
    set_TLight(NLight, Green);
    set_TLight(SLight, Green);
    set_TLight(NSPed, Red);
    set_TLight(NSPedIndicator, OFF);

    set_TLight(ELight, Red);
    set_TLight(WLight, Red);
    set_TLight(EWPed, Green);
    set_TLight(EWPedIndicator, OFF);

    update_lights();
}

//NS = dir 0
void cycle_Intersection(int direction){
    if (direction == 0){ //NS Green
        set_TLight(ELight, Yellow);
        set_TLight(WLight, Yellow);
        update_lights();

        HAL_Delay(orangeDelay);
        
        set_TLight(ELight, Red);
        set_TLight(WLight, Red);
        set_TLight(NSPed, Red);
        set_TLight(NLight, RedYellow);
        set_TLight(SLight, RedYellow);
        update_lights();

        HAL_Delay(orangeDelay);

        set_TLight(NLight, Green);
        set_TLight(SLight, Green);
        set_TLight(EWPed, Green);
        set_TLight(EWPedIndicator, OFF);
        update_lights();
        //done

    }else{ //EW Green
        set_TLight(NLight, Yellow);
        set_TLight(SLight, Yellow);
        update_lights();

        HAL_Delay(orangeDelay);

        set_TLight(NLight, Red);
        set_TLight(SLight, Red);
        set_TLight(EWPed, Red);
        set_TLight(ELight, RedYellow);
        set_TLight(WLight, RedYellow);
        update_lights();

        HAL_Delay(orangeDelay);

        set_TLight(ELight, Green);
        set_TLight(WLight, Green);
        set_TLight(NSPed, Green);
        set_TLight(NSPedIndicator, OFF);
        update_lights();
        //done
    }

}

void set_TLight(int TL, int color){
    if(ELight <= TL && TL <= WLight){
        //clear the bits
        nextLights = nextLights & ~(111 << TL*8);

        //set the bits
        nextLights = nextLights | (color << TL*8);
    }else if(TL == NLight){
        //clear the bits
        nextLights = nextLights & ~(111 << 19);

        //set the bits
        nextLights = nextLights | (color << 19);
    }else if( NSPed <= TL && TL <= EWPed){
        //clear the bits
        nextLights = nextLights & ~(11 << ((4-TL)*8+3));

        //set the bits
        if(color == Green){
            nextLights = nextLights | (2 << ((TL-4)*8+3));
        }else{
            nextLights = nextLights | (1 << ((TL-4)*8+3));
        }
    }else if(NSPedIndicator <= TL && TL <= EWPedIndicator){
        if(color == ON){
            nextLights = nextLights | (1 << ((TL-6)*8+5));
        }else{
            nextLights = nextLights & ~(1 << ((TL-6)*8+5));
        }

    }else if(TL == AllLights){
        if(color == ON){
            nextLights = 0xFFFFFF;
        }else{
            nextLights = 0x0;
        }
    }else{
        //wrong input
        nextLights = 0x100000;
    }


}

void update_lights(void) {
    currentLights = nextLights;

    // Send 32-bit value (4 bytes) through SPI
    HAL_SPI_Transmit(&hspi2, (uint8_t*)&currentLights, sizeof(currentLights), HAL_MAX_DELAY);

    // Prepare next cycle
    nextLights = currentLights;
}

