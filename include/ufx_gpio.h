#ifndef UFX_GPIO
#define UFX_GPIO

#include "inc/lm3s8938.h"

#define		GPIO_LCD_CS			0x00
#define		GPIO_LCD_RS			0x01
#define		GPIO_LCD_RES		0x02
#define 	GPIO_LCD_LGT		0x03

#define 	GPIO_LED_0			0x10
#define 	GPIO_LED_1			0x11
#define		GPIO_LED_2			0x12
#define		GPIO_LED_3			0x13
#define		GPIO_LED_4			0x14

#define 	GPIO_BUTTON_0		0x20
#define 	GPIO_BUTTON_1		0x21
#define		GPIO_BUTTON_2		0x22
#define		GPIO_BUTTON_3		0x23
#define		GPIO_BUTTON_4		0x24
#define		GPIO_BUTTON_ENTER	0x25

void gpioInit(void);

void gpioSet(uint8_t byPin, uint8_t byState);
uint8_t gpioGet(uint8_t byPin);

void gpioLedTest(void);

#endif
