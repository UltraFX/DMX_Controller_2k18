/*
 * ufx_uart.c
 *
 *  Created on: 05.05.2018
 *      Author: ndamm
 */

#include "main.h"

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

void uartDmxSendByte(uint8_t byData)
{
	UARTCharPut(UART0_BASE, (const char*)byData);
}

void uartDmxSendByteNonBlocking(uint8_t byData)
{
	UARTCharPutNonBlocking(UART0_BASE, (const char*)byData);
}

uint8_t uartBusy(void)
{
	return (uint8_t)UARTBusy(UART0_BASE);
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


