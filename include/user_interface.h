#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#define BUTTON0			0x01
#define BUTTON1 		0x02
#define BUTTON2			0x04
#define BUTTON3			0x08
#define BUTTON4			0x10
#define BUTTON_ENTER	0x20

void vUITask( void *pvParameters );

void uiIntCallback(void);
uint8_t uiDebounceButtons(void);
uint8_t uiGetButton(uint8_t byButton);

int8_t uiReadEncoder(void);

void uiCreateMainMenu(void);

#endif
