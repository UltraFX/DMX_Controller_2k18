/*
 * user_interface.c
 *
 *  Created on: 21.05.2018
 *      Author: ndamm
 */

#include "main.h"

/* local Function variables **************************************/

volatile uint8_t byButtons = 0; /**< debounced physical button states */
volatile uint16_t wTime = 0;	/**< timing variable from timer callback (1 ms) for delays */
volatile uint32_t dwTick = 0;	/**< timing variable from timer callback (1 ms) for delays */

static int8_t iLastVal = 0;		/**< old encoder value */
static int8_t iEnc_delta = 0;	/**< value difference between the two encoder inputs */
static int8_t iEncValue = 0; 	/**< encoder value */

static uint8_t byButState = 0;	/**< variable for logical button state */
static uint8_t byLongFlag = 0;  /**< check if button has been pressed for a long time */

static const int16_t iaTable[16] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0}; /**< LUT for encoder values */

uint32_t dwADC[3];				/**< ADC values for the 3 faders */

/* Key definitions */
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

struct
{
	unsigned Key0:1;
	unsigned Key1:1;
	unsigned Key2:1;
	unsigned Key3:1;
	unsigned Key4:1;
	unsigned KeyEnter:1;
} sLong;

/* local Function prototypes **************************************/

/**
*	@brief get button states
*
*	@return button states, encoded in 8-bit value
*/
static uint8_t uiGetButtons(void);

/**< @brief read physical button states ad debounce */
static void uiCheckButtons(void);

/**
*	@brief read physical input stated and average them to avoid bouncing
*
*	@return	debounced input states
*/
static uint8_t uiDebounceButtons(void);

/**< brief read physical state of rotary encoder */
static void uiCheckEncoder(void);

void vUITask( void *pvParameters )
{
	(void)pvParameters;
	static uint8_t byLedAct[5] = {0};	/**< LEDs on or off */
	uint8_t byCnt = 0;					/**< counting variable for for-loops */
	const uint8_t byaLEDs[5] = 			/**< individual IDs for the LEDs */
	{
		GPIO_LED_0,
		GPIO_LED_1,
		GPIO_LED_2,
		GPIO_LED_3,
		GPIO_LED_4
	};

	/* init peripherals for UI */
	timerMainInit();
	timerPWMInit();
	adcInit();
	
	/* init pseudo random generator */
	srand((unsigned) timerGetVal());

#warning PWM setting for background light
	timerPWMSet(80);


	while(1)
	{
		/* read out voltaged of the 3 faders */
		adcRead(dwADC);
		
		/* poll encoder */
		iEncValue += uiReadEncoder();

		/* check if button was pressed and enable or disable LED (and channel activation) */
		for(byCnt = 0; byCnt < 5; byCnt++)
		{
			if(uiGetButton(1 << byCnt) == 1)
			{
				if(byLedAct[byCnt] == 0)
				{
					gpioSet(byaLEDs[byCnt], 0);
					byLedAct[byCnt] = 1;
					byButState |= (1 << byCnt);
				}
				else
				{
					gpioSet(byaLEDs[byCnt], 1);
					byLedAct[byCnt] = 0;
					byButState &= ~(1 << byCnt);

				}
			}
		}

		/* adc is 12 bit, but DMX-values are only 8-Bit -> divide by 4 */
		dwADC[0] = dwADC[0] >> 2;
		dwADC[1] = dwADC[1] >> 2;
		dwADC[2] = dwADC[2] >> 2;

		/* update progressbars for manual menu according to new fader positions */ 
		gwinProgressbarSetPosition(ghBarRed, dwADC[0]);
		gwinProgressbarSetPosition(ghBarGreen, dwADC[1]);
		gwinProgressbarSetPosition(ghBarBlue, dwADC[2]);

		vTaskDelay(10);
	}
}

/* Function definitions **************************************/
void uiIntCallback(void)
{
	wTime++;
	dwTick++;
	uiCheckEncoder();

	if(wTime > 5)
	{
		byButtons = uiDebounceButtons();
		uiCheckButtons();
		wTime = 0;
	}
}

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

int8_t uiGetEncoder(void)
{
	if(iEncValue >= 4)
	{
		iEncValue = 0;
		return 1;
	}
	else if(iEncValue <= -4)
	{
		iEncValue = 0;
		return -1;
	}
	
	return 0;
}

/* FADER *******************************************************************/
rgb_t uiReadFader(void)
{
	rgb_t sRet;

	sRet.byR = (uint8_t)(dwADC[0]);
	sRet.byG = (uint8_t)(dwADC[1]);
	sRet.byB = (uint8_t)(dwADC[2]);

	return sRet;
}

/* BUTTONS *******************************************************************/
uint8_t uiGetButtonStates(void)
{
	return byButState;
}

static uint8_t uiDebounceButtons(void)
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
			if(byLongFlag != 0)
			{
				byLongFlag = 0;
			}
			else
			{
				byRet = 1;
			}
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
			if(byLongFlag != 0)
			{
				byLongFlag = 0;
			}
			else
			{
				byRet = 1;
			}
			sKeys.KeyEnter = 0;
		}
		break;
	default:
		break;
	}

	return byRet;
}

uint8_t uiGetButtonLong(uint8_t byButton)
{
	uint8_t byRet = 0;

	switch(byButton)
	{
	case BUTTON0:
		if(sLong.Key0 == 1)
		{
			byRet = 1;
			sLong.Key0 = 0;
			byLongFlag = 1;
		}
		break;
	case BUTTON1:
		if(sLong.Key1 == 1)
		{
			byRet = 1;
			sLong.Key1 = 0;
			byLongFlag = 1;
		}
		break;
	case BUTTON2:
		if(sLong.Key2 == 1)
		{
			byRet = 1;
			sLong.Key2 = 0;
			byLongFlag = 1;
		}
		break;
	case BUTTON3:
		if(sLong.Key3 == 1)
		{
			byRet = 1;
			sLong.Key3 = 0;
			byLongFlag = 1;
		}
		break;
	case BUTTON4:
		if(sLong.Key4 == 1)
		{
			byRet = 1;
			sLong.Key4 = 0;
			byLongFlag = 1;
		}
		break;
	case BUTTON_ENTER:
		if(sLong.KeyEnter == 1)
		{
			byRet = 1;
			sLong.KeyEnter = 0;
			byLongFlag = 1;
		}
		break;
	default:
		break;
	}

	return byRet;
}

static void uiCheckButtons(void)
{
	static uint32_t wTimes[6] = {0};

	if(byButtons & 0x01)
	{
		if(sState.Key0 == 0)
		{
			wTimes[0] = dwTick + TIME_LONGPRESS;
		}
		sState.Key0 = 1;

		if(wTimes[0] <= dwTick)
		{
			sLong.Key0 = 1;
			wTimes[0] = dwTick + TIME_LONGPRESS;
		}
	}
	else
	{
		if(sState.Key0 == 1 && sKeys.Key0 == 0)
		{
			sKeys.Key0 = 1;
			sState.Key0 = 0;
		}

		sLong.Key0 = 0;
	}

	if(byButtons & 0x02)
	{

		if(sState.Key1 == 0)
		{
			wTimes[1] = dwTick + TIME_LONGPRESS;
		}
		sState.Key1 = 1;

		if(wTimes[1] <= dwTick)
		{
			sLong.Key1 = 1;
			wTimes[1] = dwTick + TIME_LONGPRESS;
		}
	}
	else
	{
		if(sState.Key1 == 1 && sKeys.Key1 == 0)
		{
			sKeys.Key1 = 1;
			sState.Key1 = 0;
		}

		sLong.Key1 = 0;
	}

	if(byButtons & 0x04)
	{

		if(sState.Key2 == 0)
		{
			wTimes[2] = dwTick + TIME_LONGPRESS;
		}
		sState.Key2 = 1;

		if(wTimes[2] <= dwTick)
		{
			sLong.Key2 = 1;
			wTimes[2] = dwTick + TIME_LONGPRESS;
		}
	}
	else
	{
		if(sState.Key2 == 1 && sKeys.Key2 == 0)
		{
			sKeys.Key2 = 1;
			sState.Key2 = 0;
		}

		sLong.Key2 = 0;
	}

	if(byButtons & 0x08)
	{
		if(sState.Key3 == 0)
		{
			wTimes[3] = dwTick + TIME_LONGPRESS;
		}
		sState.Key3 = 1;

		if(wTimes[3] <= dwTick)
		{
			sLong.Key3 = 1;
			wTimes[3] = dwTick + TIME_LONGPRESS;
		}
	}
	else
	{
		if(sState.Key3 == 1 && sKeys.Key3 == 0)
		{
			sKeys.Key3 = 1;
			sState.Key3 = 0;
		}

		sLong.Key3 = 0;
	}

	if(byButtons & 0x10)
	{
		if(sState.Key4 == 0)
		{
			wTimes[4] = dwTick + TIME_LONGPRESS;
		}
		sState.Key4 = 1;

		if(wTimes[4] <= dwTick)
		{
			sLong.Key4 = 1;
			wTimes[4] = dwTick + TIME_LONGPRESS;
		}
	}
	else
	{
		if(sState.Key4 == 1 && sKeys.Key4 == 0)
		{
			sKeys.Key4 = 1;
			sState.Key4 = 0;
		}

		sLong.Key4 = 0;
	}

	if(byButtons & 0x20)
	{
		if(sState.KeyEnter == 0)
		{
			wTimes[5] = dwTick + TIME_LONGPRESS;
		}
		sState.KeyEnter = 1;

		if(wTimes[5] <= dwTick)
		{
			sLong.KeyEnter = 1;
			wTimes[5] = dwTick + TIME_LONGPRESS;
		}
	}
	else
	{
		if(sState.KeyEnter == 1 && sKeys.KeyEnter == 0)
		{
			sKeys.KeyEnter = 1;
			sState.KeyEnter = 0;
		}

		sLong.KeyEnter = 0;
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

