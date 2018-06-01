/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#include "gdisp_lld_config.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "ufx_gpio.h"
#include "ufx_spi.h"

#define GDISP_SCREEN_HEIGHT		128
#define GDISP_SCREEN_WIDTH		128
#define ST7735_TYPE_R TRUE

static GFXINLINE void init_board(GDisplay *g) {
	gpioSet(GPIO_LCD_CS, 1);
	gpioSet(GPIO_LCD_RES, 1);
	gpioSet(GPIO_LCD_RS, 0);

	(void) g;
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	gpioSet(GPIO_LCD_RES, (state == false)?1:0);

	(void) g;
	(void) state;
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	gpioSet(GPIO_LCD_CS, 0);

	(void) g;
}

static GFXINLINE void release_bus(GDisplay *g) {
	gpioSet(GPIO_LCD_CS, 1);

	(void) g;
}

static GFXINLINE void write_data(GDisplay *g, uint16_t data) {

	gpioSet(GPIO_LCD_RS, 1);

	spiSetLength(16);
	spiSend(data);

	(void) g;
	(void) data;
}

static GFXINLINE void write_data_byte(GDisplay *g, uint8_t data) {

	gpioSet(GPIO_LCD_RS, 1);

	spiSetLength(8);
	spiSend(data);

	(void) g;
	(void) data;
}

static GFXINLINE void write_cmd(GDisplay *g, uint8_t cmd) {

	gpioSet(GPIO_LCD_RS, 0);

	spiSetLength(8);
	spiSend(cmd);
	(void) g;
	(void) cmd;
}

static GFXINLINE void set_backlight(GDisplay *g, uint16_t bl) {
	(void) g;
	(void) bl;
}


#endif /* _GDISP_LLD_BOARD_H */
