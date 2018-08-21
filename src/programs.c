/*
 * programs.c
 *
 *  Created on: 01.06.2018
 *      Author: ndamm
 */

#include "main.h"

/* local definitions ***********************************************/
#define ENABLE_HSV_RAND		1

/* local variables *************************************************/

uint32_t dwTime = 0; /**< this variable is incremented in the progCallback() and can be used for timing purposes (waiting) */

static prog_state_t eCurrState = PROG_IDLE; /**< current active state in the progHandler() */

static rgb_t sSoll[5]= {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}; /**< desired color values for each of the 5 lamps */

static uint16_t wSpeedProg = TIME_MOODLIGHT;
static uint16_t wSpeedWait = TIME_RANDOM_WAIT;

static uint16_t waSPeedProgTable[3] = {500, 100, 50};
static uint16_t waSPeedWaitTable[3] = {60000, 10000, 5000};

/* local Function prototypes **************************************/

/**  program for the regular moodlight */
static void progMoodlight(void);

/**  
*	@brief program for the delayed moodlight
*	
*   Hue of each lamp is shifted by 30°
*/
static void progDelMoodlight(void);

/**  
*	@brief program for color transition. Can be used in all other programs for initialization 
*
*	@return: 0, if all selected lamps have reached their desired colors 
*/
static uint8_t progTransition(void);

/**<  @brief program to set color manually via the 3 faders */
static void prog_manual(void);

/**< @brief generate randomized color and update all lamps */
static void prog_random_all(void);

/**< @brief generate 5 randomized colors and update lamps */
static void prog_random_each(void);

/**  
*	@brief calculations for color transition
*
*	@param[in]	byDevice	number of device (0-4)
*	@param[in]	sSoll		desired color 
*/
static uint8_t progCalcTransition(uint8_t byDevice, rgb_t sSoll);

/**  
*	@brief calculate HSV value from RGB value
*
*	@param[in] sVal	RGB color values
*	@return 		HSV values
*/
static hsv_t progRGB2HSV(rgb_t sVal);

/**  
*	@brief calculate RGB value from HSV value
*
*	@param[in] sVal	HSV color values
*	@return 		RGB values
*/
static rgb_t progHSV2RGB(hsv_t sVal);

/**  
*	@brief set color on all lamps at once
*
*	@param[in]	desired color value (RGB)
*/
static void progSetColorAll(rgb_t sColor);

/* Function definitions ********************************************/
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
	uint8_t byI; /**< counting variable for for-loops */
	static uint8_t byInit = 0; /**< init-flag, if something has to be done once before running a selected program */
	hsv_t sColorHSV = {0, 255, 255}; /**< HSV data for initialization of delayed moodlight program (saturation and volume predefined max) */

	switch(eCurrState)
	{
	case PROG_IDLE: /**< do nothing */
		byInit = 0;
		break;
	case PROG_BLACKOUT: /**< set all lamps to 0 (lights off) */
		for(byI = 0; byI < 5; byI++)
		{
			sSoll[byI] = (rgb_t){0,0,0};
		}
		progSet(PROG_TRANSITION);
		break;
	case PROG_MOOD: /**< run moodlight animation */
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
	case PROG_MOOD_DELAY: /**< run delayed moodlight animation */
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
	case PROG_RAND_ALL:	/**< generate random color and change all lamps to that */
		prog_random_all();
		break;
	case PROG_RAND_EACH: /**< generate individual random colors for each lamp and change them */
		prog_random_each();
		break;
	case PROG_TRANSITION: /**< smooth transition from current color to desired color */
		if(progTransition() == 0)
		{
			eCurrState = PROG_IDLE;
		}
		break;
	case PROG_MANUAL: /**< manual setup of the color of the selected devices via fader */
		prog_manual();
		break;
	default:
		eCurrState = PROG_IDLE;
		break;
	}
}

static void progMoodlight(void)
{
	static uint32_t dwLimit = 0; 		/**< time value, when the next step should be done */
	static hsv_t sMood = {0, 255, 255}; /**< currently set color */
	rgb_t sColor = {0,0,0}; 			/**< converted RGB value to send to lamps */

	/* wait until timeout (settable via TIME_MOODLIGHT in app_config.h */
	if(dwTime > dwLimit)
	{
		/* set new timeout */
		dwLimit = dwTime + wSpeedProg;

		/* increase hue value by 1 */
		sMood.wHue++;

		/* if we did a full turn (360°), get back to 0 */
		if(sMood.wHue == 360)
		{
			sMood.wHue = 0;
		}

		/* convert HSV value to RGB and send to DMX */
		sColor = progHSV2RGB(sMood);
		progSetColorAll(sColor);
	}
}

static void progDelMoodlight(void)
{
	static uint32_t dwLimit = 0; /**< time value, when the next step should be done */
	rgb_t sColor = {0,0,0};		 /**< converted RGB value to send to lamps */
	uint8_t byCnt = 0;	   	     /**< count value for for-loop */
	static hsv_t sMood[5] =      /**< currently set color (individually done for each lamp) */
		{
			{0, 255, 255},
			{COLOR_SHIFT, 255, 255},
			{2*COLOR_SHIFT, 255, 255},
			{3*COLOR_SHIFT, 255, 255},
			{4*COLOR_SHIFT, 255, 255}
		};

	/* wait until timeout (settable via TIME_MOODLIGHT in app_config.h */
	if(dwTime > dwLimit)
	{
		/* set new timeout */
		dwLimit = dwTime + wSpeedProg;

		/* do for each lamp individually */
		for(byCnt = 0; byCnt < 5; byCnt++)
		{
			/* increase hue value by 1 */
			sMood[byCnt].wHue ++;

			/* if we did a full turn (360°), get back to 0 */
			if(sMood[byCnt].wHue == 360)
			{
				sMood[byCnt].wHue = 0;
			}

			/* convert HSV value to RGB and send to DMX */
			sColor = progHSV2RGB(sMood[byCnt]);
			dmxSetRGB(byCnt, sColor);
		}
	}
}

static void prog_random_all(void)
{
	rgb_t sColor = {0, 0, 0};				/**< struct to store random color */
	static uint32_t dwLimit = 0;			/**< time value, when the next transition should be done */
	static uint8_t byEnableTransition = 0;	/**< keep transition going until desired colors have been reached. Then wait for next color change */
	
#ifdef ENABLE_HSV_RAND
	hsv_t sHSV = {0, 255, 255};		/**< hsv value initialized with full saturation and volume (brightness), only in HSV_RAND mode */
#endif
	
	if(dwTime > dwLimit)
	{
		dwLimit = dwTime + wSpeedWait;
				
		/* generate pseudo-random color in hue spectrum (360°) */
#ifdef ENABLE_HSV_RAND
		sHSV.wHue = rand() % 360;
		sColor = progHSV2RGB(sHSV);
#else
		sColor.byR = rand() % 255;
		sColor.byG = rand() % 255;
		sColor.byB = rand() % 255;
#endif	

		/* insert colors into DMX stream */
//		progSetColorAll(sColor);
		sSoll[0] = sSoll[1] = sSoll[2] = sSoll[3] = sSoll[4] = sColor;
		
		byEnableTransition = 1;
	}
	if(byEnableTransition != 0)
	{
		if(progTransition() == 0)
		{
			byEnableTransition = 0;
		}			
	}
}

static void prog_random_each(void)
{
	uint8_t byCnt = 0; 						/**< count value for for-loop */
	rgb_t sColor = {0,0,0};					/**< struct to store random color */
	static uint32_t dwLimit = 0;			/**< time value, when the next transition should be done */
	static uint8_t byEnableTransition = 0;  /**< keep transition going until desired colors have been reached. Then wait for next color change */
	
#ifdef ENABLE_HSV_RAND
	hsv_t sHSV = {0, 255, 255};				/**< hsv value initialized with full saturation and volume (brightness), only in HSV_RAND mode */
#endif

	if(dwTime > dwLimit)
	{
		dwLimit = dwTime + wSpeedWait;
		
		for(byCnt = 0; byCnt < 5; byCnt++)
		{
			/* generate pseudo-random color in hue spectrum (360°) */
#ifdef ENABLE_HSV_RAND
			sHSV.wHue = rand() % 360;
			sColor = progHSV2RGB(sHSV);
#else
			sColor.byR = rand() % 255;
			sColor.byG = rand() % 255;
			sColor.byB = rand() % 255;
#endif
			
			/* insert colors into DMX stream */
//			dmxSetRGB(byCnt, sColor);
			sSoll[byCnt] = sColor;
		}
		
		byEnableTransition = 1;
	}
	if(byEnableTransition != 0)
	{
		if(progTransition() == 0)
		{
			byEnableTransition = 0;
		}			
	}
}

static uint8_t progTransition(void)
{
	static uint8_t byState = 0;		/**< bitwise monitoring of transition state for each lamp */
	static uint32_t dwLimit = 0;	/**< time value, when the next step should be done */
	uint8_t byCnt = 0;				/**< count value for for-loop */
	uint8_t byButState = uiGetButtonStates(); /**< status of lamp-selection via buttons */

	/* when this function is entered the first time, set all lamp-states to 1 (0b00011111 -> 0x1F) */
	if(byState == 0)
	{
		byState = 0x1F;
	}

	/* wait until timeout (settable via TIME_TRANSITION in app_config.h */
	if(dwTime > dwLimit)
	{
		/* set new timeout */
		dwLimit = dwTime + TIME_TRANSITION;

		/* do for each lamp individually */
		for(byCnt = 0; byCnt < 5; byCnt++)
		{
			/* run transition, until lamp has reached desired color */
			if(progCalcTransition(byCnt, sSoll[byCnt]) == 3)
			{
				/* delete bit of lamp, if desired color has been reached */
				byState &= ~(1 << byCnt);
			}
		}
	}

	/* stop transition (0), when all 5 lamps have reached their desired color */
	return byState;
}

/* UNDER CONSTRUCTION ********************************************************************/
static void prog_manual(void)
{
	uint8_t byI = 0;						/**< Counter variable for for-loop */
	uint8_t byBut = uiGetButtonStates();	/**< get status of buttons */
	uint32_t dwADC[3] = {0};
	rgb_t sColor = {0,0,0};

	sColor = uiReadFader();

	/* do for each lamp individually */
	for(byI = 0; byI < 5; byI++)
	{
		/* check if lamp has been selected via button */
		if((byBut & (1 << byI)) != 0)
		{
			/* set new desired color */
			dmxSetRGB(byI, sColor);
		}
	}
}

uint8_t progSetScene(uint8_t byScene)
{
    uint8_t byCnt;
    uint8_t byAddr;

    for(byCnt = 0; byCnt < 5; byCnt++)
    {
        byAddr = SCENES_AREA + (byScene*20) + (byCnt * 4);
        sSoll[byCnt] = menuEepReadColor(byAddr);

//        if(byRet != 0)
//        {
//            return 1;
//        }
    }

    eCurrState = PROG_TRANSITION;

    return 0;
}

void progSetSpeed(uint8_t byType, uint8_t bySpeed)
{
	if(byType == 0)
	{
		wSpeedProg = waSPeedProgTable[bySpeed];
	}
	else
	{
		wSpeedWait = waSPeedWaitTable[bySpeed];
	}
}

static uint8_t progCalcTransition(uint8_t byDevice, rgb_t sSoll)
{
	uint8_t byFinish = 0; /**< status variable. Gets increased for each color that has reached desired value */

	/* get current color from dmx-data of lamp */
	rgb_t sIst = dmxGetRGB(byDevice);

	/* RED */
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

	/* GREEN */
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

	/* BLUE */
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

	/* write color to DMX */
	dmxSetRGB(byDevice, sIst);

	return byFinish;
}

void progCallback(void)
{
	dwTime++;
}


void progSetColor(rgb_t sCol)
{
	uint8_t byI = 0;						/**< Counter variable for for-loop */
	uint8_t byBut = uiGetButtonStates();	/**< get status of buttons */

	/* do for each lamp individually */
	for(byI = 0; byI < 5; byI++)
	{
		/* check if lamp has been selected via button */
		if((byBut & (1 << byI)) != 0)
		{
			/* set new desired color */
			sSoll[byI] = sCol;
		}
	}

	/* change current program state to transition program */
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


