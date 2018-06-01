/*
 * ufx_adc.c
 *
 *  Created on: 15.05.2018
 *      Author: ndamm
 */

#include "main.h"


void adcInit(void)
{
	  /* ADC-Peripherie aktivieren */
	  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

	  SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS);

	  ADCSequenceDisable(ADC0_BASE, 0);

	  ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);

	  ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0);
	  ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH1);
	  ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH2 | ADC_CTL_IE | ADC_CTL_END);

	  //ADCHardwareOversampleConfigure(ADC0_BASE, 8);

	  ADCSequenceEnable(ADC0_BASE, 0);
	  ADCProcessorTrigger(ADC0_BASE, 0);
}


void adcRead(uint32_t *dwAdcVal)
{
	while(!ADCIntStatus(ADC0_BASE, 0, false))
	{
	}

	(void) ADCSequenceDataGet(ADC0_BASE, 0, dwAdcVal);
	ADCProcessorTrigger(ADC0_BASE, 0);

	//return (uint16_t)dwAdcVal;
}
