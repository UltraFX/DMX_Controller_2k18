#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#define BUTTON0			0x01
#define BUTTON1 		0x02
#define BUTTON2			0x04
#define BUTTON3			0x08
#define BUTTON4			0x10
#define BUTTON_ENTER	0x20

/**
*	@brief User Interface Task
*
*	Handles everything related to user interaction (Display, Fader, Buttons, LEDs)
*/
void vUITask( void *pvParameters );

/**< @brief UI Timer interrupt callback (should be called in a timer interrupt that is set to 1 ms) */
void uiIntCallback(void);

/**
 * 	@brief get fader values in form of RGB color
 *
 * 	@return RGB-Color from Fader
 */
rgb_t uiReadFader(void);

/**
*	@brief check if a button has been pressed  
*
*	@return		8-bit coded state of all available buttons (5 Lamp buttons + ENTER)
*/
uint8_t uiGetButtonStates(void);

/**
*	@brief get button state of a particular button
*
*	@param[in]	byButton	button to be checked
*	@return					button state
*/
uint8_t uiGetButton(uint8_t byButton);

/**
*	@brief check if particular button has been pressed long
*
*	@param[in]	byButton	button to be checked
*	@return					button state
*/
uint8_t uiGetButtonLong(uint8_t byButton);

/**
*	@brief read encoder state
*
*	@return			button state
*/
int8_t uiReadEncoder(void);

#endif
