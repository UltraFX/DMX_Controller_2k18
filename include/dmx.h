#ifndef _DMX_H
#define _DMX_H

#include "programs.h"

extern void vDMXTask( void *pvParameters );

extern void setDeviceIDs(uint8_t *pbyIDs);

extern void dmxSetRGB(uint8_t byDevice, rgb_t sRGB);
extern void dmxSetWhite(uint8_t byDevice, uint8_t byVal);

#endif
