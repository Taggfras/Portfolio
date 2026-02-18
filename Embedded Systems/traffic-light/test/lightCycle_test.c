#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "lightCycle.h"

/*
#define N 3
#define S 1
#define E 0
#define W 2
#define NSPed 4
#define EWPed 5
#define NSPedIndicator 6
#define EWPedIndicator 7

#define AllLights 8

#define Red 1
#define Yellow 2
#define Green 4
#define RedYellow 3
#define OFF 0 
#define ON 5
*/
extern uint32_t currentLights;

int setLightsTest(){
    for(int i = 0; i<4; i++){
        for(int j = 1; j<5; j++){
            set_TLight(AllLights, OFF);
            set_TLight(i, j);
            update_lights(0);
            if(i < 3){
                if(currentLights == (j << i*8)){
                    printf("setLightsTest %d:%d passed\n", i, j);
                }else{
                    printf("setLightsTest %d:%d failed\n", i, j);
                }
            }else{
                if(currentLights == (j << 16+3)){
                    printf("setLightsTest %d:%d passed\n", i, j);
                }else{
                    printf("setLightsTest %d:%d failed\n", i, j);
                }
            }
            //printf("%d %x\n", i,currentLights);
        }
        printf("\n");
    }


}
//comment this in if test needs to be run
/*
int main(void){
    setLightsTest();
}*/
