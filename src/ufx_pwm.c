/*
 * ufx_pwm.c
 *
 *  Created on: 01.06.2018
 *      Author: ndamm
 */

#include "main.h"

void pwmInit(void)
{
	SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

	//SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

	PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, SysCtlClockGet()/2000);//25000);

	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, 250);//12500);

	PWMOutputState(PWM0_BASE, PWM_OUT_2, true);

	PWMGenEnable(PWM0_BASE, PWM_GEN_1);
}



