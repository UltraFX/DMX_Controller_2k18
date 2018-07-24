/*
 * dmx.c
 *
 *  Created on: 25.06.2018
 *      Author: ndamm
 */

#include <stdio.h>
#include <string.h>
#include "main.h"

enum
{
	BREAK = 0,
	STARTBIT,
	DMXDATA
};

volatile uint8_t byaDmxData[DMX_CHANNELS] = {0};
static uint8_t byaDeviceID[5] = {0};

static void dmx_send(void);

void vDMXTask( void *pvParameters )
{
	while(1)
	{
		//progHandler();
		dmx_send();
		vTaskDelay(1);
	}

	(void)pvParameters;
}

void setDeviceIDs(uint8_t *pbyIDs)
{
	memcpy(byaDeviceID, pbyIDs, 5*sizeof(uint8_t));
}

void dmxSetRGB(uint8_t byDevice, rgb_t sRGB)
{
	byaDmxData[byaDeviceID[byDevice]] = sRGB.byR;
	byaDmxData[byaDeviceID[byDevice]+1] = sRGB.byG;
	byaDmxData[byaDeviceID[byDevice]+2] = sRGB.byB;
}

rgb_t dmxGetRGB(uint8_t byDevice)
{
	rgb_t sRGB = {0, 0, 0};

	sRGB.byR = byaDmxData[byaDeviceID[byDevice]];
	sRGB.byG = byaDmxData[byaDeviceID[byDevice]+1];
	sRGB.byB = byaDmxData[byaDeviceID[byDevice]+2];

	return sRGB;
}

void dmxSetWhite(uint8_t byDevice, uint8_t byVal)
{
	byaDmxData[byaDeviceID[byDevice]+3] = byVal;
}

//DMX Sende-Routine
static void dmx_send(void)
{
	static uint8_t byDmxState = BREAK;
	static uint16_t wDmxNum = 0;

    // DMX-Daten senden
    switch(byDmxState)
	{
    // BREAK erzeugen
    case BREAK:
    	uartBreakRate();
		uartDmxSendByte(0);
		while(uartBusy());
		byDmxState = STARTBIT;
		break;

	// Startbit senden
	case STARTBIT:
		uartDMXRate();
		uartDmxSendByteNonBlocking(0);
		byDmxState = DMXDATA;
		break;

	//DMX Daten senden
	case DMXDATA:
		if(wDmxNum < DMX_CHANNELS)
		{
			for(wDmxNum = 0; wDmxNum < DMX_CHANNELS; wDmxNum++)
			{
				uartDmxSendByte(byaDmxData[wDmxNum]);
				while(uartBusy());
			}
		}
		else
	    {
			wDmxNum = 0;
			byDmxState = BREAK;
		}
		break;

	default:
		wDmxNum = BREAK;
        break;
	}

}


