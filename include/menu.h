#ifndef _MENU_H
#define _MENU_H

#include "gfx.h"
#include "programs.h"

extern font_t font;

void menu_init(void);
void menu_handler(void);

rgb_t menuEepReadColor(uint8_t byAdr);

#endif
