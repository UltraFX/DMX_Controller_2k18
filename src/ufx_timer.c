/*
 * ufx_timer.c
 *
 *  Created on: 05.05.2018
 *      Author: ndamm
 */

#include "main.h"

static void SysTickIntHandler(void);
static void MainTimHandler(void);

static volatile uint32_t dwTick = 0;


void timerSysTickInit(void)
{
	// Tick bei 1ms
	  SysTickPeriodSet(SysCtlClockGet() / 1000);

	  // Interrupt aktivieren und Handler SysTickIntHandler registrieren
	  SysTickIntRegister(SysTickIntHandler);
	  SysTickIntEnable();

	  // SysTick aktivieren
	  SysTickEnable();
}

static void SysTickIntHandler(void)
{

}

void timerMainInit(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	TimerConfigure(TIMER0_BASE, (TIMER_CFG_16_BIT | TIMER_CFG_A_PERIODIC));

	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 1000);

	TimerIntRegister(TIMER0_BASE, TIMER_A, MainTimHandler);

	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER0_BASE, TIMER_A);
}

void timerPWMInit(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

	TimerConfigure(TIMER1_BASE, TIMER_CFG_16_BIT_PAIR | TIMER_CFG_A_PWM);
	TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet()/2000);

	TimerMatchSet(TIMER1_BASE, TIMER_A, 20000);

	TimerEnable(TIMER1_BASE, TIMER_A);
}

void timerPWMSet(uint8_t byVal)
{
	uint16_t wVal = (uint16_t)byVal * 250;

	TimerMatchSet(TIMER1_BASE, TIMER_A, wVal);
}

void timerDelayMS(uint32_t dwDelay)
{
	uint32_t dwEnd = dwTick + dwDelay;

	while(dwTick < dwEnd);
}

uint16_t timerGetTick(void)
{
	return (uint16_t)dwTick;
}

uint16_t timerGetVal(void)
{
	return TimerValueGet(TIMER1_BASE, TIMER_A);
}

static void MainTimHandler(void)
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	dwTick++;

	uiIntCallback();
	progCallback();
}

/*systemticks_t gfxSystemTicks(void)
{
	return dwTick;
}

systemticks_t gfxMillisecondsToTicks(delaytime_t ms)
{
	return ms;
}*/
