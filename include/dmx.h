#ifndef _DMX_H
#define _DMX_H

#include "programs.h"

/**
* @brief DMX transmission Task
*/
extern void vDMXTask( void *pvParameters );

/**
*	@brief setup start addresses for all 5 lamps at once
*
* 	@param[in]	pbyData		pointer to an array that contains the 5 addresses
*/
extern void setDeviceIDs(uint8_t *pbyIDs);

/**
* 	@brief store RGB value for a lamp in DMX array for transmission 
* 
*	@param[in]	byDevice		DeviceID
*	@param[in]	sRGB			color data
*/
extern void dmxSetRGB(uint8_t byDevice, rgb_t sRGB);

/**
* 	@brief store white value for a lamp in DMX array for transmission 
* 
*	@param[in]	byDevice		DeviceID
*	@param[in]	byVal			white value (0 - 255)
*/
extern void dmxSetWhite(uint8_t byDevice, uint8_t byVal);

/**
*	@brief get color data from selected lamp
*
*	@param[in]	byDevice	DeviceID
*	@return					color value
*/
extern rgb_t dmxGetRGB(uint8_t byDevice);

#endif
