void initControler(void);/*initialise of the intersection controller */
void intersection(void); /*intersection control loop, evaluate traffic situation, ped request & timing*/
void toggle_Intersection(void); /* handle toggle at the active intersec direction, switch between EW & NS */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin); /*car detection , pedest button inputs*/
uint8_t toggle_Car(uint8_t car); /*handles car switch*/
void toggle_PedIndicatorLight(int light); /*blink the ped indic LEDs whilst waiting, ex NSPedInidicator*/
void resetTiming(void);/*reset timing variable before switching intersection direction*/
void timer10ms_timeout(void);/*called by GPIO interrupt of TIM3, 10ms timout timer, ex handle ped wait light blink*/

#define max_int32 0x7FFFFFFF //make sure that the timestamp is out of bound
