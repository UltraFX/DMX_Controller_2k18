/*
 * ufx_gpio.c
 *
 *  Created on: 05.05.2018
 *      Author: ndamm
 */

#include "main.h"

#define LCD_CS    	  	GPIO_PIN_4
#define LCD_RS        	GPIO_PIN_6
#define LCD_Reset     	GPIO_PIN_7
#define LCD_Light     	GPIO_PIN_0

#define SSI_CLK   		GPIO_PIN_2
#define SSI_TX    		GPIO_PIN_5
#define SSI_RX    		GPIO_PIN_4

#define LED0_PORT		GPIO_PORTB_BASE
#define LED0_PIN		GPIO_PIN_5
#define	LED1_PORT		GPIO_PORTB_BASE
#define LED1_PIN		GPIO_PIN_6
#define	LED2_PORT		GPIO_PORTF_BASE
#define LED2_PIN		GPIO_PIN_0
#define	LED3_PORT		GPIO_PORTF_BASE
#define LED3_PIN		GPIO_PIN_1
#define	LED4_PORT		GPIO_PORTG_BASE
#define LED4_PIN		GPIO_PIN_1

#define LCD_LIGHT_PORT	GPIO_PORTB_BASE
#define LCD_LIGHT_PIN	GPIO_PIN_1

void gpioInit(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);

	/* LED's */
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, (GPIO_PIN_5 | GPIO_PIN_6));
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, (GPIO_PIN_0 | GPIO_PIN_1));
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_1);

	GPIOPadConfigSet(GPIO_PORTB_BASE, (GPIO_PIN_5 | GPIO_PIN_6),
						GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPD);
	GPIOPadConfigSet(GPIO_PORTF_BASE, (GPIO_PIN_0 | GPIO_PIN_1),
							GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPD);
	GPIOPadConfigSet(GPIO_PORTG_BASE, GPIO_PIN_1,
							GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPD);

	/* LED-Pins an GND, also invertiert! */
	gpioSet(GPIO_LED_0, 1);
	gpioSet(GPIO_LED_1, 1);
	gpioSet(GPIO_LED_2, 1);
	gpioSet(GPIO_LED_3, 1);
	gpioSet(GPIO_LED_4, 1);

	/* LCD Block */
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, LCD_Light);
	GPIOPadConfigSet(GPIO_PORTG_BASE, LCD_Light,
	                 GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);

	/* Alle Pins vom Register F konfigurieren */
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, LCD_Reset | LCD_RS | LCD_CS);
	GPIOPadConfigSet(GPIO_PORTC_BASE, LCD_Reset | LCD_RS | LCD_CS, GPIO_STRENGTH_8MA,
	                 GPIO_PIN_TYPE_STD_WPU);

	// CS vom LCD auf High setzen
    GPIOPinWrite(GPIO_PORTC_BASE, LCD_Reset, LCD_Reset);
	GPIOPinWrite(GPIO_PORTC_BASE, LCD_CS, LCD_CS);
	GPIOPinWrite(GPIO_PORTG_BASE, LCD_Light, 0);

	GPIOPinTypeGPIOOutput(LCD_LIGHT_PORT, LCD_LIGHT_PIN);
	GPIOPadConfigSet(LCD_LIGHT_PORT, LCD_LIGHT_PIN,
					 GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPD);

	GPIOPinWrite(LCD_LIGHT_PORT, LCD_LIGHT_PIN, 0);

	/* SPI Block */
	// Pin-zuweisungen fürs SSI-Interface
	GPIOPinTypeSSI(GPIO_PORTA_BASE, SSI_CLK | SSI_TX);
	/*GPIOPadConfigSet(GPIO_PORTA_BASE, SSI_CLK | SSI_TX,
					 GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);*/

	/* UART Block */
	GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_1);

	/* Drehencoder */

    //Pin 1 und 2 auf Eingang (Pin 1 = Hoch, Pin 2 = runter)
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2);
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

    /* Buttons */

    /* Enter */
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_3);
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    /* Buttons 0 - 4 */
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOPinTypeGPIOInput(GPIO_PORTG_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTG_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

}

void gpioSet(uint8_t byPin, uint8_t byState)
{
	switch(byPin)
	{
	case GPIO_LCD_CS:
		GPIOPinWrite(GPIO_PORTC_BASE, LCD_CS, (byState == 0)?0:LCD_CS);
		break;
	case GPIO_LCD_RS:
		GPIOPinWrite(GPIO_PORTC_BASE, LCD_RS, (byState == 0)?0:LCD_RS);
		break;
	case GPIO_LCD_RES:
		GPIOPinWrite(GPIO_PORTC_BASE, LCD_Reset, (byState == 0)?0:LCD_Reset);
		break;
	case GPIO_LCD_LGT:
		GPIOPinWrite(GPIO_PORTG_BASE, LCD_Light, (byState == 0)?0:LCD_Light);
		break;
	case GPIO_LED_0:
		GPIOPinWrite(LED0_PORT, LED0_PIN, (byState == 0)?0:LED0_PIN);
		break;
	case GPIO_LED_1:
		GPIOPinWrite(LED1_PORT, LED1_PIN, (byState == 0)?0:LED1_PIN);
		break;
	case GPIO_LED_2:
		GPIOPinWrite(LED2_PORT, LED2_PIN, (byState == 0)?0:LED2_PIN);
		break;
	case GPIO_LED_3:
		GPIOPinWrite(LED3_PORT, LED3_PIN, (byState == 0)?0:LED3_PIN);
		break;
	case GPIO_LED_4:
		GPIOPinWrite(LED4_PORT, LED4_PIN, (byState == 0)?0:LED4_PIN);
		break;
	default: break;
	}
}

uint8_t gpioGet(uint8_t byPin)
{
	uint8_t byState = 0;

	switch(byPin)
	{
	case GPIO_BUTTON_0:
		byState = (~GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0)) & GPIO_PIN_0;
		break;
	case GPIO_BUTTON_1:
		byState = (~GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_0)) & GPIO_PIN_0;
		break;
	case GPIO_BUTTON_2:
		byState = (~GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_2)) & GPIO_PIN_2;
		break;
	case GPIO_BUTTON_3:
		byState = (~GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_3)) & GPIO_PIN_3;
		break;
	case GPIO_BUTTON_4:
		byState = (~GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_4)) & GPIO_PIN_4;
		break;
	case GPIO_BUTTON_ENTER:
		byState = (~GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3)) & GPIO_PIN_3;
		break;
	default:
		break;
	}

	if(byState != 0)
	{
		byState = 1;
	}

	return byState;
}

void gpioLedTest(void)
{
	static uint8_t byLedState = 0;
	static uint16_t dwTime = 0;

	switch(byLedState)
	{
	case 0:
		gpioSet(GPIO_LED_0, 0);
		gpioSet(GPIO_LED_1, 1);
		gpioSet(GPIO_LED_2, 1);
		gpioSet(GPIO_LED_3, 1);
		gpioSet(GPIO_LED_4, 1);
		byaDmxData[21] = 255;
		byaDmxData[22] = 0;
		byaDmxData[23] = 0;
		break;
	case 1:
		gpioSet(GPIO_LED_0, 1);
		gpioSet(GPIO_LED_1, 0);
		byaDmxData[22] = 255;
		break;
	case 2:
		gpioSet(GPIO_LED_1, 1);
		gpioSet(GPIO_LED_2, 0);
		byaDmxData[21] = 0;
		break;
	case 3:
		gpioSet(GPIO_LED_2, 1);
		gpioSet(GPIO_LED_3, 0);
		byaDmxData[23] = 255;
		break;
	case 4:
		gpioSet(GPIO_LED_3, 1);
		gpioSet(GPIO_LED_4, 0);
		byaDmxData[22] = 0;
		break;
	case 5:
		byLedState = 0;
		break;
	default:
		byLedState = 0;
		break;
	}

	if(timerGetTick() > dwTime)
	{
		dwTime = timerGetTick() + 500;
		byLedState++;
	}
}
