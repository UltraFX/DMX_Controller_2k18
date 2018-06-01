//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
#include "main.h"
//#include "_write.h"


static void vDebugTask( void *pvParameters );
static void vDMXTask( void *pvParameters );

int main (void)
{
	/* Clock per PLL auf max (50 MHz) */
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_8MHZ | SYSCTL_OSC_MAIN);

	//Interrupts global aktivieren
	IntMasterEnable();

	//timerMainInit();
	gpioInit();
	uartInit();
	spiInit(10000000);


	/*gfxInit();
	guiInit();
	guiShowPage(DISPLAY_PAGE);*/

	xTaskCreate(vDebugTask, "DEBUG", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(vUITask, "UI", 256, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(vDMXTask, "DMX", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	/* Start the scheduler. */
	vTaskStartScheduler();

	return 0;
}

static void vDebugTask( void *pvParameters )
{

	while(1)
	{
			/*uartSendByte((uint8_t)(dwADC[0]>>8));
			uartSendByte((uint8_t)(dwADC[0]&0xff));*/
			uartSendString("Hello World!\n", 13);

			vTaskDelay(1000);
	}

	(void)pvParameters;
}

static void vDMXTask( void *pvParameters )
{
	while(1)
	{
		//progHandler();
		uartDMXSend();
		vTaskDelay(1);
	}

	(void)pvParameters;
}


// ----------------------------------------------------------------------------
