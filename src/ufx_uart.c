/*
 * ufx_uart.c
 *
 *  Created on: 05.05.2018
 *      Author: ndamm
 */

#include "main.h"

enum
{
	BREAK = 0,
	STARTBIT,
	DMXDATA
};

volatile uint8_t byaDmxData[DMX_CHANNELS] = {0};

static void uartDMXRate(void);
static void uartBreakRate(void);

//static void uartDMXHandler(void);

void uartInit(void)
{
	//UART1 für USB
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

	//UART0 für DMX
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	UARTClockSourceSet(UART1_BASE, UART_CLOCK_SYSTEM);
	UARTClockSourceSet(UART0_BASE, UART_CLOCK_SYSTEM);

	/* Einstellungen für Serial-USB (115200 Bits/s, 8-n-1) */
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,
			UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);

	//UARTFIFODisable(UART0_BASE);

	//UARTIntRegister(UART0_BASE, uartDMXHandler);

	uartDMXRate();

	//UARTIntEnable(UART0_BASE, UART_INT_TX);
	UARTEnable(UART0_BASE);

	//UARTCharPutNonBlocking(UART0_BASE, 0);
}

void uartSendByte(const char cByte)
{
        UARTCharPut(UART1_BASE, cByte);
}

void uartSendString(const char *pcBuffer, unsigned long ulCount)
{
    //
    // Loop while there are more characters to send.
    //
    while(ulCount--)
    {
        //
        // Write the next character to the UART.
        //
        UARTCharPut(UART1_BASE, *pcBuffer++);
    }
}

//DMX Sende-Routine
void uartDMXSend(void)
{
	static uint8_t byDmxState = BREAK;
	static uint16_t wDmxNum = 0;

    // DMX-Daten senden
    switch(byDmxState)
	{
    // BREAK erzeugen
    case BREAK:
    	uartBreakRate();
		UARTCharPut(UART0_BASE, 0);
		//while(UARTBusy(UART0_BASE));
		byDmxState = STARTBIT;
		break;

	// Startbit senden
	case STARTBIT:
		uartDMXRate();
		UARTCharPutNonBlocking(UART0_BASE, 0);
		byDmxState = DMXDATA;
		break;

	//DMX Daten senden
	case DMXDATA:
		if(wDmxNum < DMX_CHANNELS)
		{
			for(wDmxNum = 0; wDmxNum < DMX_CHANNELS; wDmxNum++)
			{
				UARTCharPut(UART0_BASE, byaDmxData[wDmxNum]);
				while(UARTBusy(UART0_BASE));
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

//Normale DMX-Rate
void uartDMXRate(void)
{
  // UART auf 250 KBaud, 8-n-1
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 250000,
						UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_TWO | UART_CONFIG_PAR_NONE);
}

//DMX Break-Rate
void uartBreakRate(void)
{
  // UART auf 250 KBaud, 8-n-1
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 90000,
						UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_TWO | UART_CONFIG_PAR_NONE);
}


