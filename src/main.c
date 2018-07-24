//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
#include "main.h"
//#include "_write.h"

QueueHandle_t xDebugQueue;

static void vDebugTask( void *pvParameters );
static void vStorageTask(void *pvParameters);

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

	xDebugQueue = xQueueCreate(3, sizeof(xDebugMessage));

	xTaskCreate(vStorageTask, "Storage", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
	xTaskCreate(vDebugTask, "DEBUG", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(vUITask, "UI", 265, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(vDMXTask, "DMX", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	/* Start the scheduler. */
	vTaskStartScheduler();

	return 0;
}

static void vDebugTask( void *pvParameters )
{
	uint8_t byState = 0;
	uint8_t byData[10] = {0};

	xDebugMessage xMessage;

	i2cInit();

	while(1)
	{
		while(xQueueReceive(xDebugQueue, &xMessage, portMAX_DELAY) != pdPASS)
		{
			;
		}
		uartSendString(xMessage.pcMessage, xMessage.wLength);
	}

	(void)pvParameters;
}

static void vStorageTask(void *pvParameters)
{
	uint8_t byCheck = 0;
	uint8_t byState = 0;
	uint8_t byDeviceIDs[5] = {0};

	i2cInit();

	byState = i2c_eeprom_read_byte(VERSION_AREA, &byCheck);

	if(byState == 0)
	{
		if(byCheck != VERSION) //no init
		{
			byState = i2c_eeprom_write_byte(0, 0x55);
			i2c_wait_eeprom();

			/* init device addresses */
			byState = i2c_eeprom_write_byte(DEVICE_AREA, DEVICE0_ADDR);
			i2c_wait_eeprom();
			byState = i2c_eeprom_write_byte(DEVICE_AREA+1, DEVICE1_ADDR);
			i2c_wait_eeprom();
			byState = i2c_eeprom_write_byte(DEVICE_AREA+2, DEVICE2_ADDR);
			i2c_wait_eeprom();
			byState = i2c_eeprom_write_byte(DEVICE_AREA+3, DEVICE3_ADDR);
			i2c_wait_eeprom();
			byState = i2c_eeprom_write_byte(DEVICE_AREA+4, DEVICE4_ADDR);
			i2c_wait_eeprom();
		}
	}

	byState = i2c_eeprom_read(DEVICE_AREA, byDeviceIDs, 5);
	setDeviceIDs(byDeviceIDs);

	while(1)
	{
		vTaskDelay(1000);
	}

	(void)pvParameters;
}


// ----------------------------------------------------------------------------
