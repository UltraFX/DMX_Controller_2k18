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

volatile uint8_t byaDmxData[DMX_CHANNELS] = {0}; /**< this local array contains all DMX data for one universe */
static uint8_t byaDeviceID[5] = {0};			 /**< start addresses of the 5 lamps */

/** 
*	@brief statemachine for DMX protocoll. Should be called in endless loop of main or task
*/
static void dmx_send(void);

void vDMXTask( void *pvParameters )
{
	while(1)
	{
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
	static uint8_t byDmxState = BREAK; /**< current DMX state */
	static uint16_t wDmxNum = 0;	   /**< counter for current DMX data */

    /* read DMX-Data from UART */
    switch(byDmxState)
	{
    case BREAK:									/**< generate BREAK */
    	/* change UART data rate to provoke Framing Error */
		uartBreakRate();
		/* send dummy byte */
		uartDmxSendByte(0);
		/* wait for end of transmission */
		while(uartBusy());
		/* go to STARTBIT state */
		byDmxState = STARTBIT;
		break;
	case STARTBIT:								/**< send Startbit */
		/* change UART data rate back to 250 kbit/s */
		uartDMXRate();
		/* send 0 */
		uartDmxSendByteNonBlocking(0);
		/* go to DMXDATA state */
		byDmxState = DMXDATA;
		break;
#warning OPTIMIZE THIS!
	case DMXDATA:								/** send DMX data */
	
		/* send until all 512 bytes are transmitted... */
		if(wDmxNum < DMX_CHANNELS)
		{
			for(wDmxNum = 0; wDmxNum < DMX_CHANNELS; wDmxNum++)
			{
				uartDmxSendByte(byaDmxData[wDmxNum]);
				while(uartBusy());
			}
		}
		/* ...then go back to BREAK state */
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


