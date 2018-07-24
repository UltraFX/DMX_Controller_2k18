/*
 * programs.c
 *
 *  Created on: 01.06.2018
 *      Author: ndamm
 */

#include "main.h"

uint32_t dwTime = 0;

static prog_state_t eCurrState = PROG_IDLE;
static rgb_t sSoll[5]= {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

static void progMoodlight(void);
static void progDelMoodlight(void);
static uint8_t progTransition(void);
static void prog_manual(void);
static uint8_t progCalcTransition(uint8_t byDevice, rgb_t sSoll);

static hsv_t progRGB2HSV(rgb_t sVal);
static rgb_t progHSV2RGB(hsv_t sVal);
static void progSetColorAll(rgb_t sColor);

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
	uint8_t byI;
	static uint8_t byInit = 0;
	hsv_t sColorHSV = {0, 255, 255};

	switch(eCurrState)
	{
	case PROG_IDLE:
		byInit = 0;
		break;
	case PROG_BLACKOUT:
		for(byI = 0; byI < 5; byI++)
		{
			sSoll[byI] = (rgb_t){0,0,0};
		}
		progSet(PROG_TRANSITION);
		break;
	case PROG_MOOD:
		if(byInit == 0)
		{
			for(byI = 0; byI < 5; byI++)
			{
				sSoll[byI] = (rgb_t){255,0,0};
			}
			if(progTransition() == 0)
			{
				byInit = 1;
			}
		}
		else
		{
			progMoodlight();
		}
		break;
	case PROG_MOOD_DELAY:
		if(byInit == 0)
		{
			for(byI = 0; byI < 5; byI++)
			{
				sColorHSV.wHue = byI * COLOR_SHIFT;
				sSoll[byI] = progHSV2RGB(sColorHSV);
			}
			if(progTransition() == 0)
			{
				byInit = 1;
			}
		}
		else
		{
			progDelMoodlight();
		}
		break;
	case PROG_TRANSITION:
		if(progTransition() == 0)
		{
			eCurrState = PROG_IDLE;
		}
		break;
	case PROG_MANUAL:
		prog_manual();
		break;
	default:
		eCurrState = PROG_IDLE;
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

		sColor = progHSV2RGB(sMood);
		progSetColorAll(sColor);
	}
}

static void progDelMoodlight(void)
{
	static uint32_t dwLimit = 0;
	rgb_t sColor = {0,0,0};
	uint8_t byCnt = 0;
	static hsv_t sMood[5] =
		{
			{0, 255, 255},
			{COLOR_SHIFT, 255, 255},
			{2*COLOR_SHIFT, 255, 255},
			{3*COLOR_SHIFT, 255, 255},
			{4*COLOR_SHIFT, 255, 255}
		};

	if(dwTime > dwLimit)
	{
		dwLimit = dwTime + 100;

		for(byCnt = 0; byCnt < 5; byCnt++)
		{
			sMood[byCnt].wHue ++;

			if(sMood[byCnt].wHue >= 360)
			{
				sMood[byCnt].wHue = 0;
			}

			sColor = progHSV2RGB(sMood[byCnt]);
			dmxSetRGB(byCnt, sColor);
		}
	}
}

static uint8_t progTransition(void)
{
	static uint8_t byState = 0;
	static uint32_t dwLimit = 0;
	uint8_t byCnt = 0;
	uint8_t byButState = uiGetButtonStates();

	if(byState == 0)
	{
		byState = 0x1F;
	}

	if(dwTime > dwLimit)
	{
		dwLimit = dwTime + 2;

		for(byCnt = 0; byCnt < 5; byCnt++)
		{
			if(progCalcTransition(byCnt, sSoll[byCnt]) == 3)
			{
				byState &= ~(1 << byCnt);
			}

//			if((byButState & (1 << byCnt)) != 0)
//			{
//				if(progCalcTransition(byCnt, sSoll[byCnt]) == 3)
//				{
//					byState = 0;
//				}
//			}
		}
	}

	return byState;
}

static void prog_manual(void)
{

}

uint8_t progSetScene(uint8_t byScene)
{
    uint8_t byCnt;
    uint8_t byRet = 0;
    uint8_t byAddr;

    for(byCnt = 0; byCnt < 5; byCnt++)
    {
        byAddr = SCENES_AREA + ((byScene + byCnt) * 4);
        sSoll[byCnt] = menuEepReadColor(byAddr);

//        if(byRet != 0)
//        {
//            return 1;
//        }
    }

    eCurrState = PROG_TRANSITION;

    return 0;
}

static uint8_t progCalcTransition(uint8_t byDevice, rgb_t sSoll)
{
	//rgb_t sRetVal = {0, 0, 0};
	uint8_t byFinish = 0;

	rgb_t sIst = dmxGetRGB(byDevice);

	if((sSoll.byR < sIst.byR) && sIst.byR > 0)
	{
		sIst.byR = sIst.byR-1;
	}
	else if((sSoll.byR > sIst.byR) && sIst.byR < 360)
	{
		sIst.byR = sIst.byR+1;
	}
	else
	{
		byFinish++;
	}


	if((sSoll.byG < sIst.byG) && sIst.byG > 0)
	{
		sIst.byG = sIst.byG-1;
	}
	else if((sSoll.byG > sIst.byG) && sIst.byG < 255)
	{
		sIst.byG = sIst.byG+1;
	}
	else
	{
		byFinish++;
	}

	if((sSoll.byB < sIst.byB) && sIst.byB > 0)
	{
		sIst.byB = sIst.byB-1;
	}
	else if((sSoll.byB > sIst.byB) && sIst.byB < 255)
	{
		sIst.byB = sIst.byB+1;
	}
	else
	{
		byFinish++;
	}

	dmxSetRGB(byDevice, sIst);

	return byFinish;
}

void progCallback(void)
{
	dwTime++;
}


void progSetColor(rgb_t sCol)
{
	uint8_t byI = 0;
	uint8_t byBut = uiGetButtonStates();

	for(byI = 0; byI < 5; byI++)
	{
		if((byBut & (1 << byI)) != 0)
		{
			sSoll[byI] = sCol;
		}
	}

	eCurrState = PROG_TRANSITION;
}

void progSetWhite(uint8_t byVal)
{
	dmxSetWhite(2, byVal);
}


static rgb_t progHSV2RGB(hsv_t sVal)
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

static hsv_t progRGB2HSV(rgb_t sVal)
{
	hsv_t sRet;


	float fRed, fGreen, fBlue, h, s, v, max, min, del, rc, gc, bc;

	/* convert RGB to HSV */
	fRed = (float)sVal.byR / 255.0;            /* fRed,gd,bd range 0-1 instead of 0-255 */
	fGreen = (float)sVal.byG / 255.0;
	fBlue = (float)sVal.byB / 255.0;

	/* compute maximum of rd,gd,bd */
	if (fRed>=fGreen)
	{
		if (fRed>=fBlue)
		{
			max = fRed;
		}
		else
		{
			max = fBlue;
		}
	 }
	 else
	 {
		 if(fGreen>=fBlue)
		{
			 max = fGreen;
		}
		else
		{
			max = fBlue;
		}
	 }

	/* compute minimum of rd,gd,fBlue */
	if (fRed<=fGreen)
	{
		if(fRed<=fBlue)
		{
			min = fRed;
		}
		else
		{
			min = fBlue;
		}
	}
	else
	{
		if (fGreen<=fBlue)
		{
			min = fGreen;
		}
		else
		{
			min = fBlue;
		}
	}

	del = max - min;
	v = max;
	if (max != 0.0)
	{
		s = (del) / max;
	}
	else
	{
		s = 0.0;
	}

	h = 0.;
	if (s != 0.0)
	{
		rc = (max - fRed) / del;
		gc = (max - fGreen) / del;
		bc = (max - fBlue) / del;

		if(fRed==max)
		{
			h = bc - gc;
		}
		else if (fGreen==max)
		{
			h = 2 + rc - bc;
		}
		else if (fBlue==max)
		{
			h = 4 + gc - rc;
		}

		h = h * 60;

		if (h<0)
		{
			h += 360;
		}
	}

	sRet.wHue = (uint16_t)h;
	sRet.bySat = (uint8_t)(s * 255.0);
	sRet.byVal= (uint8_t)(v * 255.0);

return sRet;
}

static void progSetColorAll(rgb_t sColor)
{
	dmxSetRGB(0, sColor);
	dmxSetRGB(1, sColor);
	dmxSetRGB(2, sColor);
	dmxSetRGB(3, sColor);
	dmxSetRGB(4, sColor);
}


