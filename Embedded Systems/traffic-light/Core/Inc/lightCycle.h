void cycle_Intersection(int direction); // handle full light cycle for given direction
void set_TLight(int TL, int color); // sets specific LED to a specific colour
void update_lights(int source); //sends the bitmask to the shift registers
void init_Intersection(void); // initial state of the intersection
void blink_Ped_Indicator(int ILight, int state, int source); /*
 * control for the pedestrian lights. used for ON/OFF and blinking of LEDs
 * ILight: pedestrian light indicator identifier */


void delay(int delaytime);

#define orangeDelay 2000 //ms red + yellow on
#define greenDelay 15000 //ms
#define redDelayMax 8000 //ms
#define pedestrianDelay 8000 //ms
#define walkingDelay 4000 //ms
#define toggleFreq 500 //ms, for the pedestrian indic light
#define minGreen 1000 //ms



#define NS 0 //North south interchange direction
#define EW 1 // east west interchange direction


#define NLight 3//TL 4, led 10-12
#define SLight 1 // TL 2, LED 4-6
#define ELight 2 //TL 3, LED 7-9
#define WLight 0 //TL 1, LED 1-3
#define NSPed 5 //NS ped light, LED 13-16
#define EWPed 4 //EW ped light, LED 19-22
#define NSPedIndicator 7 // used as ped waiting identifier
#define EWPedIndicator 6 // ^^

#define AllLights 8 // control's all lights

#define Red 1
#define Yellow 2
#define Green 4
#define RedYellow 3
#define OFF 0
#define ON 5


