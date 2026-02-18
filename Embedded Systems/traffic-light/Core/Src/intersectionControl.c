#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
// header files - project specific
#include "lightCycle.h"
#include "intersectionControl.h"
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

//0 is inactive, 1 is button/switch active
uint8_t NCar = 0;
uint8_t SCar = 0;
uint8_t ECar = 0;
uint8_t WCar = 0;
//pedestrian btn state and indic led state
uint8_t NSPed_btn = 0;
uint8_t EWPed_btn = 0;
uint8_t NSPed_InLight = 0;
uint8_t EWPed_InLight = 0;
//Timing variables used
int32_t currentWaitTime = 0; //ms, time since last intersection change
int32_t redWaitTimeStamp = max_int32; //To make the timestamp lie outside the possible interval
int32_t pedWaitTimeStamp = max_int32; //To make the timestamp lie outside the possible interval
int32_t ILightToggleTimeStamp = 0; //blink pedestiran  inidicator
uint8_t currentDir = NS; // direcetion of greenlight
uint8_t resetWait = 0; //reset timing

void initControler(void){
    init_Intersection(); // initial state, can be found in ightcycle.c
}

void intersection(void){
    while(1){
        //Walking delay, this delay stops the intersection from changing at all while pedestrians are walking
    	//2 orange delays are subtracted, as that is the time the light is green during intersection toggling
        while(currentWaitTime <= (walkingDelay - 2*orangeDelay)){
            //wait
        }

        //This is the minimum time that a car traffic light has to stay green, 
        //to avoid the light switching from red-yellow directly to yellow, confusing drivers.
        while(currentWaitTime <= minGreen){
            //wait
        }

        //This calculates waitTime (pedestrian and cars) )using timeStaps
        //orange delays are added, as that is the time the light stays red/yellow during intersection toggling
        int diff_Car = currentWaitTime - redWaitTimeStamp + 2*orangeDelay;
        int diff_Ped = currentWaitTime - pedWaitTimeStamp + orangeDelay;

        //checks if direction can be switched
        if ((ECar == 1 || WCar == 1) && (NCar == 0 && SCar == 0) && currentDir == NS){
            toggle_Intersection();
        }else if ((NCar == 1 || SCar == 1) && (ECar == 0 && WCar == 0) && currentDir == EW){
            toggle_Intersection();
        //force car to switch if cars have waited at red too long
        }else if(diff_Car >= redDelayMax){
            toggle_Intersection();    

        //force switch if pedestrian has waited too long
        }else if(diff_Ped >= pedestrianDelay){
            toggle_Intersection();    
        }
        //switch if no traffic is detected & greenDelay has been exeded
        else if(currentWaitTime + 2*orangeDelay >= greenDelay && !(ECar == 1 || WCar == 1 || NCar == 1 || SCar == 1)){
            toggle_Intersection();
        }
    }
}



void resetTiming(void){
    //Check for cars still coming from the direction that was just green
	//This is before the intersection toggles, thus it has too look for its own intersection direction
	if(currentDir == EW && (ECar == 1 || WCar == 1)){
		redWaitTimeStamp = 0;
	}else if(currentDir == NS && (NCar == 1 || SCar == 1)){
		redWaitTimeStamp = 0;
	}else{ //if there are no cars waiting, invalidate the timestamp
		redWaitTimeStamp = max_int32; //To make the timestamp lie outside the possible interval
	}
//reset the pedestrian waiting time
    pedWaitTimeStamp = max_int32; //To make the timestamp lie outside the possible interval
}




void toggle_PedIndicatorLight(int light){
	//north South ped indicators
	if(light == NSPedIndicator){
		if(NSPed_InLight == 0){
            NSPed_InLight = 1;
            blink_Ped_Indicator(NSPedIndicator, ON, 1);
        }else{
            NSPed_InLight = 0;
            blink_Ped_Indicator(NSPedIndicator, OFF, 1);
        }
		//east west ped indic
	}else if(light == EWPedIndicator){
        if(EWPed_InLight == 0){
            EWPed_InLight = 1;
            blink_Ped_Indicator(EWPedIndicator, ON, 1);
        }else{
            EWPed_InLight = 0;
            blink_Ped_Indicator(EWPedIndicator, OFF, 1);
        }
    }
}



void toggle_Intersection(void){
    resetTiming(); //resets timing before switch
    if(currentDir == NS){ //switch direciton
        currentDir = EW;
        cycle_Intersection(EW);
        NSPed_InLight = 0; //clear NS pedestrian state
        NSPed_btn = 0;
    }else{
        currentDir = NS;
        cycle_Intersection(NS);
        EWPed_InLight = 0; //clear EW pedestrian state
        EWPed_btn = 0;
    }
    //reset timer after switch
    ILightToggleTimeStamp = 0;
    currentWaitTime = 0;
    //restart pedestriant timer if someone pressed the wait button during the intersection cycle 
    if(pedWaitTimeStamp != max_int32){
        pedWaitTimeStamp = currentWaitTime;
    }
    return;
}




void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == TL4_Car_Pin){ // South car sensor
        if(currentDir == EW && redWaitTimeStamp > currentWaitTime){
            redWaitTimeStamp = currentWaitTime;
        }
        SCar = toggle_Car(SCar);
    }else if(GPIO_Pin == TL3_Car_Pin){// West car sensor
        if(currentDir == NS && redWaitTimeStamp > currentWaitTime){
            redWaitTimeStamp = currentWaitTime;
        }
        WCar = toggle_Car(WCar);
    }else if(GPIO_Pin == TL2_Car_Pin){ //North car sensor
        if(currentDir == EW && redWaitTimeStamp > currentWaitTime){
            redWaitTimeStamp = currentWaitTime;
        }
        NCar = toggle_Car(NCar);
    }else if(GPIO_Pin == TL1_Car_Pin){ //East car sensor
        if(currentDir == NS && redWaitTimeStamp > currentWaitTime){
            redWaitTimeStamp = currentWaitTime;
        }
        ECar = toggle_Car(ECar);
    }else if(GPIO_Pin == PL2_Switch_Pin){ //north south pedestrian
        if(currentDir == NS && pedWaitTimeStamp > currentWaitTime){
            NSPed_btn = 1;
            pedWaitTimeStamp = currentWaitTime;
            ILightToggleTimeStamp = currentWaitTime;
            toggle_PedIndicatorLight(NSPedIndicator);
        }
    }else if(GPIO_Pin == PL1_Switch_Pin){ //east west pedestrian button
        if(currentDir == EW && pedWaitTimeStamp > currentWaitTime){
            EWPed_btn = 1;
            pedWaitTimeStamp = currentWaitTime;
            ILightToggleTimeStamp = currentWaitTime;
            toggle_PedIndicatorLight(EWPedIndicator);
        }
    }

}


//this function is called by TIM3 interrupts every 10ms
void timer10ms_timeout(void){
    //increment the wait time
    currentWaitTime += 10;

    //time since last pedestrian indic toggle
    int diff_Toggle = currentWaitTime - ILightToggleTimeStamp;
    if(NSPed_btn == 1){ // blink NS indic pedes
        if(diff_Toggle >= toggleFreq){
            ILightToggleTimeStamp = currentWaitTime;
            toggle_PedIndicatorLight(NSPedIndicator);
        }
    }
    if(EWPed_btn == 1){ // blink EW indic pedestrian
        if(diff_Toggle >= toggleFreq){
            ILightToggleTimeStamp = currentWaitTime;
            toggle_PedIndicatorLight(EWPedIndicator);
        }
    }
}



// car state toggle
uint8_t toggle_Car(uint8_t car){
	uint8_t new_car;
	if(car == 1){
		new_car = 0;
	}else{
		new_car = 1;
	}
	return new_car;
}








