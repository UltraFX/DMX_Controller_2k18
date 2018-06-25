/*
 * programs.c
 *
 *  Created on: 01.06.2018
 *      Author: ndamm
 */

#include "main.h"

uint32_t dwTime = 0;

static prog_state_t eCurrState = PROG_IDLE;

static void progMoodlight(void);
static rgb_t hsv2rgb(hsv_t sVal);

void progSet(prog_state_t eState)
{
	eCurrState = eState;
}

prog_state_t progGet(void)
{
	return eCurrState;
}

void progHandler(void)
{
	switch(eCurrState)
	{
	case PROG_IDLE:
		break;
	case PROG_MOOD:
		progMoodlight();
		break;
	}
}

static void progMoodlight(void)
{
	static uint32_t dwLimit = 0;
	static hsv_t sMood = {0, 255, 255};
	rgb_t sColor = {0,0,0};

	if(dwTime > dwLimit)
	{
		dwLimit = dwTime + 100;

		sMood.wHue++;

		if(sMood.wHue == 360)
		{
			sMood.wHue = 0;
		}

		sColor = hsv2rgb(sMood);
		dmxSetRGB(2, sColor);
	}
}

void progCallback(void)
{
	dwTime++;
}


static rgb_t hsv2rgb(hsv_t sVal)
{
	float fHue, fSat, fVal, fR, fG, fB;
	rgb_t sRet;

	fHue = (float)sVal.wHue;
	fSat = (float)sVal.bySat/255.0;
	fVal = (float)sVal.byVal/255.0;

	float fH = fHue / (float)60.0;

	/*uint8_t R,G,B;
	float r,g,b;*/

	uint8_t byH1 = floor(fH);
	float fF = fH - (float)byH1;
	float p = fVal * (1 - fSat);
	float q = fVal * (1 - fSat * fF);
	float t = fVal * (1 - fSat * (1 - fF));

	switch(byH1)
	{
	case 0: fR = fVal; 	fG = t; 	fB = p; break;
	case 1: fR = q; 	fG = fVal; 	fB = p; break;
	case 2: fR = p; 	fG = fVal; 	fB = t; break;
	case 3: fR = p; 	fG = q; 	fB = fVal; break;
	case 4: fR = t; 	fG = p; 	fB = fVal; break;
	default: fR = fVal; fG = p; 	fB = q; break;
	}

	sRet.byR = fR*255;
	sRet.byG = fG*255;
	sRet.byB = fB*255;

	//set_brightness(R,G,B);

	return sRet;
}

