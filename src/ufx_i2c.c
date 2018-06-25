/*
 * ufx_i2c.c
 *
 *  Created on: 21.06.2018
 *      Author: ndamm
 */

#include "main.h"

#define SLAVE_ADDR	0x50U

void i2cInit(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);

	I2CMasterInitExpClk(I2C1_MASTER_BASE, SysCtlClockGet(), false);

	I2CMasterEnable(I2C1_MASTER_BASE);

}

void i2c_wait_eeprom(void)
{
	uint8_t byState = 0;

	do
	{
		I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, SLAVE_ADDR, false);

		I2CMasterDataPut(I2C1_MASTER_BASE, (unsigned char)0);

		I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);

		while(I2CMasterBusy(I2C1_MASTER_BASE))
		{
			;
		}

		byState = (uint8_t)I2CMasterErr(I2C1_MASTER_BASE);
	}
	while(byState != 0);
}

uint8_t i2c_eeprom_write_byte(uint8_t byAddr, uint8_t byData)
{
	uint8_t byRet = 0;

	I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, SLAVE_ADDR, false);

	I2CMasterDataPut(I2C1_MASTER_BASE, (unsigned char)byAddr);

	I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	while(I2CMasterBusy(I2C1_MASTER_BASE))
	{
		;
	}

	if((byRet = (uint8_t)I2CMasterErr(I2C1_MASTER_BASE)))
	{
		I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
		return byRet;
	}

	I2CMasterDataPut(I2C1_MASTER_BASE, (unsigned char)byData);

	I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

	while(I2CMasterBusy(I2C1_MASTER_BASE))
	{
		;
	}

	if((byRet = (uint8_t)I2CMasterErr(I2C1_MASTER_BASE)))
	{
		return byRet;
	}

	return byRet;
}


uint8_t i2c_eeprom_read_byte(uint8_t byAddr, uint8_t *byData)
{
	uint8_t byRet = 0;

	I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, SLAVE_ADDR, false);

	I2CMasterDataPut(I2C1_MASTER_BASE, (unsigned char)byAddr);

	I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	while(I2CMasterBusy(I2C1_MASTER_BASE))
	{
	}

	if((byRet = (uint8_t)I2CMasterErr(I2C1_MASTER_BASE)))
	{
		return byRet;
	}

	I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, SLAVE_ADDR, true);


	I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

	while(I2CMasterBusy(I2C1_MASTER_BASE))
	{
	}

	if((byRet = (uint8_t)I2CMasterErr(I2C1_MASTER_BASE)))
	{
		return byRet;
	}

	*byData = I2CMasterDataGet(I2C1_MASTER_BASE);

	return byRet;
}

uint8_t i2c_eeprom_read(uint8_t byAddr, uint8_t *byData, uint8_t byLen)
{
	uint8_t byRet = 0;
	uint8_t byI;

	I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, SLAVE_ADDR, false);

	I2CMasterDataPut(I2C1_MASTER_BASE, (unsigned char)byAddr);

	I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	while(I2CMasterBusy(I2C1_MASTER_BASE))
	{
	}

	if((byRet = (uint8_t)I2CMasterErr(I2C1_MASTER_BASE)))
	{
		return byRet;
	}

	I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, SLAVE_ADDR, true);

	for(byI = 0; byI < byLen; byI++)
	{
		if(byI == 0)
		{
			I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
		}
		/*else if(byI == (byLen-1))
		{
			I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
		}*/
		else
		{
			I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
		}

		while(I2CMasterBusy(I2C1_MASTER_BASE))
		{
			;
		}

		byData[byI] = I2CMasterDataGet(I2C1_MASTER_BASE);

		if((byRet = (uint8_t)I2CMasterErr(I2C1_MASTER_BASE)))
		{
			I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
			return byRet;
		}
	}
	I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

	return byRet;
}

