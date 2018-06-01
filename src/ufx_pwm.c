/*
 * ufx_pwm.c
 *
 *  Created on: 01.06.2018
 *      Author: ndamm
 */

#include "main.h"

void pwmInit(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

	PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, 500);//25000);

	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, 250);//12500);

	PWMGenEnable(PWM0_BASE, PWM_GEN_1);

	PWMOutputState(PWM0_BASE, PWM_OUT_2, true);
}



