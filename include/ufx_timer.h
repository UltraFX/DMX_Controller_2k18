#ifndef UFX_TIMER
#define UFX_TIMER

/* SysTick functions */
void timerSysTickInit(void);

void timerMainInit(void);
void timerPWMInit(void);
void timerPWMSet(uint8_t byVal);


void timerDelayMS(uint32_t dwDelay);
uint16_t timerGetTick(void);

#endif
