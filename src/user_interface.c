/*
 * user_interface.c
 *
 *  Created on: 21.05.2018
 *      Author: ndamm
 */

#include "main.h"

static uint8_t uiGetButtons(void);
static void uiCheckButtons(void);

static void uiCheckEncoder(void);

volatile uint8_t byButtons = 0;
volatile uint16_t wTime = 0;

static int8_t iLastVal = 0;
static int8_t iEnc_delta = 0;

static const int16_t iaTable[16] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};

struct
{
	unsigned Key0:1;
	unsigned Key1:1;
	unsigned Key2:1;
	unsigned Key3:1;
	unsigned Key4:1;
	unsigned KeyEnter:1;
} sKeys;

struct
{
	unsigned Key0:1;
	unsigned Key1:1;
	unsigned Key2:1;
	unsigned Key3:1;
	unsigned Key4:1;
	unsigned KeyEnter:1;
} sState;

void vUITask( void *pvParameters )
{
	(void)pvParameters;
	uint32_t dwADC[3];
	static uint8_t byLedAct[5] = {0};
	static uint8_t byEnter = 0;
	static uint8_t byBrightness = 0;

	timerMainInit();
	timerPWMInit();
	adcInit();
	//pwmInit();

	gfxInit();

	//gdispFillArea(0,0,128,128,Green);

	guiInit();
	menu_init();

	timerPWMSet(80);


	while(1)
	{
		adcRead(dwADC);
		menu_handler();
		progHandler();

		if(uiGetButton(BUTTON0) == 1)
		{
			if(byLedAct[0] == 0)
			{
				gpioSet(GPIO_LED_0, 0);
				byLedAct[0] = 1;
			}
			else
			{
				gpioSet(GPIO_LED_0, 1);
				byLedAct[0] = 0;

			}
		}

		if(uiGetButton(BUTTON1) == 1)
		{
			if(byLedAct[1] == 0)
			{
				gpioSet(GPIO_LED_1, 0);
				byLedAct[1] = 1;
			}
			else
			{
				gpioSet(GPIO_LED_1, 1);
				byLedAct[1] = 0;

			}
		}

		if(uiGetButton(BUTTON2) == 1)
		{
			if(byLedAct[2] == 0)
			{
				gpioSet(GPIO_LED_2, 0);
				byLedAct[2] = 1;
			}
			else
			{
				gpioSet(GPIO_LED_2, 1);
				byLedAct[2] = 0;

			}
		}

		if(uiGetButton(BUTTON3) == 1)
		{
			if(byLedAct[3] == 0)
			{
				gpioSet(GPIO_LED_3, 0);
				byLedAct[3] = 1;
			}
			else
			{
				gpioSet(GPIO_LED_3, 1);
				byLedAct[3] = 0;

			}
		}

		if(uiGetButton(BUTTON4) == 1)
		{
			if(byLedAct[4] == 0)
			{
				gpioSet(GPIO_LED_4, 0);
				byLedAct[4] = 1;
			}
			else
			{
				gpioSet(GPIO_LED_4, 1);
				byLedAct[4] = 0;

			}
		}

		dwADC[0] = dwADC[0] / 4;
		dwADC[1] = dwADC[1] / 4;
		dwADC[2] = dwADC[2] / 4;

		/*byaDmxData[21] = dwADC[0];
		byaDmxData[22] = dwADC[1];
		byaDmxData[23] = dwADC[2];*/

		gwinProgressbarSetPosition(ghBarRed, dwADC[0]);
		gwinProgressbarSetPosition(ghBarGreen, dwADC[1]);
		gwinProgressbarSetPosition(ghBarBlue, dwADC[2]);

		vTaskDelay(10);
	}
}

void uiIntCallback(void)
{

	/*byButtons = uiDebounceButtons();
	uiCheckButtons();*/

	wTime++;
	uiCheckEncoder();

	if(wTime > 5)
	{
		byButtons = uiDebounceButtons();
		uiCheckButtons();
		wTime = 0;
	}
}

/* DISPLAY *******************************************************************/
/*void uiCreateMainMenu(void)
{
	gdispDrawBox(3, 3, 122, 25, White);
	gdispDrawBox(3, 3, 122, 25, Red);

	gdispDrawString(5, 10, "Programme", font, Black);

	gdispDrawString(5, 30, "Farbwahl", font, Black);

	gdispDrawString(5, 50, "Manuell", font, Black);

	gdispDrawString(5, 70, "Einstellungen", font, Black);

	gdispFillArea(0, 107, 128, 20, Blue);
//	gdispDrawBox(3,3, 125, 30, Blue);
}*/

/* ENCODER *******************************************************************/
static void uiCheckEncoder(void)
{
	/* read encoder state every 1 ms (tick interrupt) */
	iLastVal = (iLastVal << 2) & 0x0F;
	if(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1) != 0)
	{
		iLastVal |= 2;
	}

	if(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2) != 0)
	{
		iLastVal |= 1;
	}
	iEnc_delta += iaTable[iLastVal];
}

int8_t uiReadEncoder(void)
{
	int8_t iRet;

	iRet = iEnc_delta;
	iEnc_delta = 0;

	return iRet;
}

/* BUTTONS *******************************************************************/
uint8_t uiDebounceButtons(void)
{
	static uint8_t byaBut[3] = {0};
	static uint8_t byCnt = 0;
	static uint8_t byRet;

	if(byCnt == 3)
	{
		byRet = (byaBut[0] & byaBut[1]) |
				(byaBut[1] & byaBut[2]) |
				(byaBut[2] & byaBut[0]);
		byCnt = 0;
	}
	else
	{
		byaBut[byCnt++] = uiGetButtons();
	}

	return byRet;
}

uint8_t uiGetButton(uint8_t byButton)
{
	uint8_t byRet = 0;

	switch(byButton)
	{
	case BUTTON0:
		if(sKeys.Key0 == 1)
		{
			byRet = 1;
			sKeys.Key0 = 0;
		}
		break;
	case BUTTON1:
		if(sKeys.Key1 == 1)
		{
			byRet = 1;
			sKeys.Key1 = 0;
		}
		break;
	case BUTTON2:
		if(sKeys.Key2 == 1)
		{
			byRet = 1;
			sKeys.Key2 = 0;
		}
		break;
	case BUTTON3:
		if(sKeys.Key3 == 1)
		{
			byRet = 1;
			sKeys.Key3 = 0;
		}
		break;
	case BUTTON4:
		if(sKeys.Key4 == 1)
		{
			byRet = 1;
			sKeys.Key4 = 0;
		}
		break;
	case BUTTON_ENTER:
		if(sKeys.KeyEnter == 1)
		{
			byRet = 1;
			sKeys.KeyEnter = 0;
		}
		break;
	default:
		break;
	}

	return byRet;
}

static void uiCheckButtons(void)
{
	if(byButtons & 0x01)
	{
		if(sState.Key0 == 0 && sKeys.Key0 == 0)
		{
			sKeys.Key0 = 1;
			sState.Key0 = 1;
		}
	}
	else
	{
		sState.Key0 = 0;
	}

	if(byButtons & 0x02)
		{
			if(sState.Key1 == 0 && sKeys.Key1 == 0)
			{
				sKeys.Key1 = 1;
				sState.Key1 = 1;
			}
		}
		else
		{
			sState.Key1 = 0;
		}

	if(byButtons & 0x04)
		{
			if(sState.Key2 == 0 && sKeys.Key2 == 0)
			{
				sKeys.Key2 = 1;
				sState.Key2 = 1;
			}
		}
		else
		{
			sState.Key2 = 0;
		}

	if(byButtons & 0x08)
		{
			if(sState.Key3 == 0 && sKeys.Key3 == 0)
			{
				sKeys.Key3 = 1;
				sState.Key3 = 1;
			}
		}
		else
		{
			sState.Key3 = 0;
		}

	if(byButtons & 0x10)
		{
			if(sState.Key4 == 0 && sKeys.Key4 == 0)
			{
				sKeys.Key4 = 1;
				sState.Key4 = 1;
			}
		}
		else
		{
			sState.Key4 = 0;
		}

	if(byButtons & 0x20)
		{
			if(sState.KeyEnter == 0 && sKeys.KeyEnter == 0)
			{
				sKeys.KeyEnter = 1;
				sState.KeyEnter = 1;
			}
		}
		else
		{
			sState.KeyEnter = 0;
		}
}


static uint8_t uiGetButtons(void)
{
	uint8_t byVal;

	byVal = gpioGet(GPIO_BUTTON_0);

	byVal |= gpioGet(GPIO_BUTTON_1) << 1;

	byVal |= gpioGet(GPIO_BUTTON_2) << 2;

	byVal |= gpioGet(GPIO_BUTTON_3) << 3;

	byVal |= gpioGet(GPIO_BUTTON_4) << 4;

	byVal |= gpioGet(GPIO_BUTTON_ENTER) << 5;

	return byVal;
}

