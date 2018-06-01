#ifndef UFX_TIMER
#define UFX_TIMER

/* SysTick functions */
void timerSysTickInit(void);

void timerMainInit(void);
void timerDelayMS(uint32_t dwDelay);
uint16_t timerGetTick(void);

#endif
