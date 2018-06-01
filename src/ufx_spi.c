/*
 * ufx_spi.c
 *
 *  Created on: 05.05.2018
 *      Author: ndamm
 */

#include <stdio.h>

#include "inc/lm3s8938.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ssi.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"

static uint8_t  byWordLength = 8;
static uint32_t dwSpeed = 0;

void spiInit(uint32_t dwRate)
{
	dwSpeed = dwRate;

	// Aktiviere SSI1 und die GPIO-Register A (SSI) und C (LCD Control)
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

	  // SSI-Interface deaktivieren
	  SSIDisable(SSI0_BASE);

	  SSIClockSourceSet(SSI0_BASE, SSI_CLOCK_SYSTEM);

	  //SSI konfigurieren: SSI1, CPU-Takt, SPI Mode 0 (Clock-Polarität und Phase auf 0), SSI auf Master,
	  // Frequenz (übergeben per Funktionsparameter (Standard 300 kHz), 8-Bit-Übertragung
	  SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, dwRate, 8);

	  //SSI-Modul aktivieren
	  SSIEnable(SSI0_BASE);
}

void spiSetLength(uint8_t byLength)
{
	if(byWordLength == byLength)
	{
		return;
	}
	byWordLength = byLength;
	if(dwSpeed == 0)
	{
		return;
	}

	SSIDisable(SSI0_BASE);
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, dwSpeed, byLength);
	SSIEnable(SSI0_BASE);
}

void spiSend(uint16_t wData)
{
	SSIDataPut(SSI0_BASE, (uint32_t)wData);
	while(SSIBusy(SSI0_BASE));
}
