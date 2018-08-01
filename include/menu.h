#ifndef _MENU_H
#define _MENU_H

#include "gfx.h"
#include "programs.h"

extern font_t font;

/**< @brief initialize menu (call once before endless loop in main or task) */
void menu_init(void);

/**< @brief cyclic menu handler (call in endless loop of main or task) */
void menu_handler(void);

/**
*	@brief get user defined color from EEPROM
*
*	@param[in]	byAdr	EEPROM address of user defined color
* 	@return				RGB value of user color
*/
rgb_t menuEepReadColor(uint8_t byAdr);

#endif
