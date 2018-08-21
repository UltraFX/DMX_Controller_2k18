/*
 * menu.c
 *
 *  Created on: 31.05.2018
 *      Author: ndamm
 */

#include "main.h"

/* local definitions **********************************************/
#define TYPE_NORMAL	1
#define TYPE_MAN	2
#define TYPE_PROMPT 3

/* local Function prototypes **************************************/

/**
*	@brief move the blue frame from one menu item to the next, when encoder changes position
*
*	@param[in]	byPos		new position for the blue frame (0-4)
*	@param[in]	byType 		menu type (normal dropdown or manual/prompt
*	@param[in]	byNewMenu	determine if new menu layer has been entered or not
*/
static void menu_refresh(uint8_t byPos, uint8_t byType, uint8_t byNewMenu);

// /**
// *	@brief same as menu_refresh(), but for the manual color menu (fader), because it has only 2 items below the 3 color progressbars
// *
// *	@param[in]	byPos	new position for the blue frame (0-1)
// */
// static void menu_manual_refresh(uint8_t byPos);

/**
 * 	@brief refresh selection between "Ja" and "Nein"
 *
 * 	@param[in]	byPos	new position for the blue frame
 */
static void menu_prompt_refresh(uint8_t byPos);

/**
*	@brief Display new menu (with 1-5 items and current red frame position)
*
*	@param[in]	byID	menu-ID from "Menu-Collections" enum
*/
static void menu_display(uint8_t byID);

/**< @brief show manual menu */
static void showMan(void);

/**
 *  @brief display prompt with yes and now
 *
 *  @parameter[in]	caText		text to be displayed on prompt
 */
static void showPrompt(char *caText);

/**< @brief fixed color menu */
static void setColor(void);

/**< @brief scene menu */
static void setScene(void);

/**< @brief start a selected program */
static void progAct(void);

/**< @brief set the speed for program-steps from the settings menu */
static void setSpeed(void);

/**< @brief get current encoder value */
//static void menu_update_encoder(void);

/**
*	@brief menu navigation routine (encoder movement)
*
*	@param[in]	byType	Normal or Manual/Prompt menu
*/
static void menu_nav_menu(uint8_t byType);

/* menu structures ************************************************/

/* Menu-Items */
enum
{
	ELEM_PROG,
	ELEM_COL,
	ELEM_USER,
	ELEM_SCENE,
	ELEM_MAN,
	ELEM_SET,
	ELEM_BLACKOUT,
	ELEM_MOOD,
	ELEM_MOOD_DELAY,
	ELEM_RAND_ALL,
	ELEM_RAND_EACH,
	ELEM_BACK,
	ELEM_MORE,

	ELEM_RED,
	ELEM_GREEN,
	ELEM_BLUE,
	ELEM_WHITE,
	ELEM_MAG,
	ELEM_CYAN,
	ELEM_YEL,

	ELEM_USER_1,
	ELEM_USER_2,
	ELEM_USER_3,
	ELEM_USER_4,
	ELEM_USER_5,

	ELEM_SCENE_1,
	ELEM_SCENE_2,
	ELEM_SCENE_3,
	ELEM_SCENE_4,

	ELEM_SPEED_PROG,
	ELEM_SPEED_CHG,
	ELEM_DMX_ADDR,
	ELEM_DISP,

	ELEM_SPEED_SLO,
	ELEM_SPEED_MED,
	ELEM_SPEED_HI
};

/* Menu-Collections */
enum
{
	MENU_MAIN = 0,
	MENU_PROG,
	MENU_COL1,
	MENU_COL2,
	MENU_USER,
	MENU_SCENE,
	MENU_SET,
	MENU_SPEED_PROG,
	MENU_SPEED_CHG,
	MENU_ADDR,
	MENU_DISP,
	MENU_NONE = 0xff
};

typedef struct
{
	uint8_t byID;			/**< Element ID */
	char *pcName;			/**< title of the element (will be shown on Display) */
	uint8_t bAct;			/**< active element, when entering the menu (red frame) */
	uint8_t bySettable;		/**< content can be changed (e.g. change color values or scene) with long press of Enter */
	uint8_t byNext;			/**< ID of the next menu, when pressed (MENU_NONE, when function should be called instead */
	void(*pFunc)(void);		/**< function to be called, when Enter is pressed */
} element_t;

typedef struct
{
	uint8_t byID;			/**< ID of menu */
	char *pName;			/**< title of menu */
	uint8_t byNumElements;	/**< number of menu elements (max: 5) */
	uint8_t byCurElement;	/**< currently selected menu element */
	element_t *psElement;	/**< pointer to the menu collection */
} menu_t;

static element_t sMainMenu[] =
{
	{ELEM_PROG,	"Programme", 		1, 0, MENU_PROG, NULL},
	{ELEM_COL, 	"Farbwahl", 		0, 0, MENU_COL1, NULL},
	{ELEM_USER, "Benutzerdefiniert",0, 1, MENU_USER, NULL},
	{ELEM_SCENE,"Szenen",           0, 1, MENU_SCENE, NULL},
	{ELEM_SET, 	"Einstellungen", 	0, 0, MENU_SET, NULL}
};

static element_t sProgramMenu[] =
{
	{ELEM_BLACKOUT, 	"Blackout",     	1, 0, MENU_NONE, progAct},
    {ELEM_MOOD, 		"Moodlight", 		0, 0, MENU_NONE, progAct},
	{ELEM_MOOD_DELAY, 	"Moodl. Verz.", 	0, 0, MENU_NONE, progAct},
	{ELEM_RAND_ALL, 	"Zufall alle",  	0, 0, MENU_NONE, progAct},
    {ELEM_RAND_EACH,    "Zufall single",  	0, 0, MENU_NONE, progAct},
	{ELEM_BACK, 		"Zurueck", 			0, 0, MENU_MAIN, NULL}
};

static element_t sColor1Menu[] =
{
	{ELEM_RED, 	 	"Rot", 			1, 0, MENU_NONE, setColor},
	{ELEM_GREEN, 	"Gruen", 		0, 0, MENU_NONE, setColor},
	{ELEM_BLUE, 	"Blau", 		0, 0, MENU_NONE, setColor},
	{ELEM_WHITE, 	"Weiss", 		0, 0, MENU_NONE, setColor},
	{ELEM_MORE, 	"Weiter", 		0, 0, MENU_COL2, NULL},
	{ELEM_BACK, 	"Zurueck", 		0, 0, MENU_MAIN, NULL}
};

static element_t sColor2Menu[] =
{
	{ELEM_MAG, 		"Magenta", 		1, 0, MENU_NONE, setColor},
	{ELEM_CYAN, 	"Cyan", 		0, 0, MENU_NONE, setColor},
	{ELEM_YEL, 		"Gelb", 		0, 0, MENU_NONE, setColor},
	{ELEM_BACK, 	"Zurueck", 		0, 0, MENU_MAIN, NULL}
};

static element_t sUserCol[] =
{
	{ELEM_USER_1, "BENUTZER 1", 1, 1,  MENU_NONE, setColor},
	{ELEM_USER_2, "BENUTZER 2", 1, 1, MENU_NONE, setColor},
	{ELEM_USER_3, "BENUTZER 3", 1, 1, MENU_NONE, setColor},
	{ELEM_USER_4, "BENUTZER 4", 1, 1, MENU_NONE, setColor},
	{ELEM_USER_5, "BENUTZER 5", 1, 1, MENU_NONE, setColor},
	{ELEM_BACK,   "Zurueck", 	0, 0, MENU_MAIN, NULL}
};

static element_t sScenes[] =
{
	{ELEM_SCENE_1, "SZENE 1", 	1, 1, MENU_NONE, setScene},
	{ELEM_SCENE_2, "SZENE 2", 	1, 1, MENU_NONE, setScene},
	{ELEM_SCENE_3, "SZENE 3", 	1, 1, MENU_NONE, setScene},
	{ELEM_SCENE_4, "SZENE 4", 	1, 1, MENU_NONE, setScene},
	{ELEM_BACK,   	"Zurueck",  0, 0, MENU_MAIN, NULL}
};

static element_t sSetup[] =
{
	{ELEM_SPEED_PROG, 	"Geschw. Prog.",	0, 0, 	MENU_SPEED_PROG, 	NULL},
	{ELEM_SPEED_CHG, 	"Geschw. Wechs.",	0, 0,  	MENU_SPEED_CHG, 	NULL},
	{ELEM_DMX_ADDR, 	"DMX-Addr",			0, 0,	MENU_ADDR, 			NULL},
	{ELEM_DISP, 		"Display.", 		0, 0,	MENU_DISP, 			NULL},
	{ELEM_BACK,   		"Zurueck",  		0, 0, 	MENU_MAIN, 			NULL}
};

static element_t sSpeed[] =
{
	{ELEM_SPEED_SLO, 	"Langsam", 	1, 0,	MENU_NONE, 	setSpeed},
	{ELEM_SPEED_MED, 	"Normal", 	1, 0,   MENU_NONE, 	setSpeed},
	{ELEM_SPEED_HI, 	"Schnell",  1, 0,	MENU_NONE, 	setSpeed},
	{ELEM_BACK,   		"Zurueck",  0, 0, 	MENU_SET, 	NULL}
};

static menu_t sMenu[] =
{
	{MENU_MAIN, 		"Hauptmenue",		5, 0, sMainMenu},
	{MENU_PROG, 		"Programme", 		6, 0, sProgramMenu},
	{MENU_COL1, 		"Farben1", 			6, 0, sColor1Menu},
	{MENU_COL2, 		"Farben2", 			4, 0, sColor2Menu},
	{MENU_USER,			"Benutzerdefiniert",6, 0, sUserCol},
	{MENU_SCENE,    	"Szenen",			5, 0, sScenes},
	{MENU_SET,    		"Einstellungen",	5, 0, sSetup},
	{MENU_SPEED_PROG,	"Geschwindigkeit",	4, 0, sSpeed},
	{MENU_SPEED_CHG,	"Geschwindigkeit",	4, 0, sSpeed}
};

/* menu states *****************************************************/
typedef enum
{
	STATE_INIT = 0,
	STATE_IDLE,
	STATE_SETTABLE,
	STATE_MANUAL,
	STATE_PROMPT,
	STATE_STORE_SCENE,
	STATE_STORE_COLOR
}menu_state_t;

/* local variables *************************************************/
static menu_t *psCurrentMenu; 				/**< currently active menu structure */

static menu_state_t eState = STATE_INIT; 	/**< current menu state */

static uint8_t byPosition = 0;				/**< current position in dropdown menu */
static uint8_t byPrePos = 0;

font_t font, bigfont;

/* Function definitions ********************************************/
void vMenuTask( void *pvParameters )
{
	(void)pvParameters;
	
	/* init display library */
	gfxInit();

	guiInit();

	menu_init();
	
	while(1)
	{
		/* handle menu state machine */
		menu_handler();
		
		/* update program state machine */
		progHandler();
		
		vTaskDelay(1);
	}
}

void menu_init(void)
{
	/* initial menu: Main Menu */
	psCurrentMenu = &sMenu[0];
	
	/* load font for menu */
	font = gdispOpenFont("DejaVuSans10");
	bigfont = gdispOpenFont("DejaVuSans16");
}

void menu_handler(void)
{
	static uint8_t byMenu = 0;					/**< current menu ID */
    static rgb_t   sUserColor = {0,0,0};
    static uint8_t byUserNr = 0;
    uint8_t byTemp;
	uint8_t byCnt;

#ifdef DEBUG_EN
	xDebugMessage xMessage;						/* Debug messages */
#endif
	
	/* get current encoder value */
//	menu_update_encoder();
	
	switch(eState)
	{
	case STATE_INIT:														/**< menu initialization */
		/* load main menu */
		menu_display(byMenu);
	    /* set red frame to first item */
		menu_refresh(byPosition, TYPE_NORMAL, 1);
		eState = STATE_IDLE;
		break;
	case STATE_IDLE:														/**< default state; wait for user input */
		/* Enter-Button pressed */
		if(uiGetButton(BUTTON_ENTER) == 1)
		{
			/* Enter-Button pressed long -> Switch to settable state */
			if(psCurrentMenu->psElement[byPosition].bySettable == 1)
			{
				eState = STATE_SETTABLE;
			}
			/* execute function */
			if(psCurrentMenu->psElement[byPosition].pFunc != NULL)
			{
				psCurrentMenu->psElement[byPosition].pFunc();
			}
			/* load next menu item */
			else
			{
				byPrePos = byPosition;
				menu_display(psCurrentMenu->psElement[byPosition].byNext);
				menu_refresh(0, TYPE_NORMAL, 1);
				byPosition = 0;
				//byPosition = byPrePos;
			}
		}

		/* check encoder */
		menu_nav_menu(TYPE_NORMAL);
		break;
	case STATE_SETTABLE:													/**< active, when a menu is entered that contains changeable items (e.g. user colors or scenes) */
		/* Enter-button pressed long */
		if(uiGetButtonLong(BUTTON_ENTER) == 1)
		{
			byUserNr = byPosition;
			
			/* If in user defined color menu and Enter pressed long */
			if(psCurrentMenu->byID == MENU_USER)
			{
				byPosition = 0;				
				eState = STATE_MANUAL;
				progSet(PROG_MANUAL);
				showMan();
			}
			
			/* If in scene menu and Enter pressed long */
			if(psCurrentMenu->byID == MENU_SCENE)
			{
				byPosition = 0;
				showPrompt("Szene speichern?");
				eState = STATE_STORE_SCENE;
			}
			
#ifdef DEBUG_EN
			xMessage.pcMessage = "Lang\n";
			xMessage.wLength = 5;
			xQueueSend(xDebugQueue, &xMessage, portMAX_DELAY);
#endif
		}
		/* Enter-button pressed short */
		else if(uiGetButton(BUTTON_ENTER) == 1)
		{				
#ifdef DEBUG_EN
			xMessage.pcMessage = "Kurz\n";
			xMessage.wLength = 5;
			xQueueSend(xDebugQueue, &xMessage, portMAX_DELAY);
#endif
			
			/* short press: go back to regular idle state */
			if(psCurrentMenu->psElement[byPosition].bySettable == 0)
			{
				eState = STATE_IDLE;
			}
			/* execute function */
			if(psCurrentMenu->psElement[byPosition].pFunc != NULL)
			{
				psCurrentMenu->psElement[byPosition].pFunc();
			}
			/* load next menu item */
			else
			{
				menu_display(psCurrentMenu->psElement[byPosition].byNext);
				menu_refresh(0, TYPE_NORMAL, 0);
				byPosition = 0;
			}
		}

		/* check encoder */
		menu_nav_menu(TYPE_NORMAL);
		break;
    /* UNDER CONSTRUCTION ****************************************/
	case STATE_MANUAL:		
		if(uiGetButton(BUTTON_ENTER) == 1)
		{
			if(byPosition == 0)
			{
				/* double check before storing the new value */
				sUserColor = uiReadFader();
				showPrompt("Sicher?");
				eState = STATE_STORE_COLOR;
			}
			else
			{
				/* return to user color menu */
				menu_display(MENU_USER);
				menu_refresh(byUserNr, TYPE_NORMAL, 0);
				byPosition = byUserNr;
				eState = STATE_SETTABLE;
			}
		}
		
		/* check encoder */
		menu_nav_menu(TYPE_MAN);
		break;
	case STATE_STORE_SCENE:
		/* yes */
		if(uiGetButton(BUTTON_ENTER) == 1)
		{
			/* do only if no program is running */
			if(byPosition == 0 && progGet() == PROG_IDLE)
			{
				/* get EEPROM address from currently selected menu item */
				byTemp = SCENES_AREA+(4*5*byUserNr);
				
				/* do for all colors */
				for(byCnt = 0; byCnt < 5; byCnt++)
				{
					/* get current color information from DMX-data */
					sUserColor = dmxGetRGB(byCnt);
					/* store in EEPROM */
					menuEepWriteColor(byTemp, sUserColor);		
					// (void)i2c_eeprom_write_byte(byTemp, sUserColor.byR);
					// i2c_wait_eeprom();
					byTemp += 4;
				}
			}
			/* return to scene menu */
			menu_display(MENU_SCENE);
			menu_refresh(byUserNr, TYPE_NORMAL, 0);
			byPosition = byUserNr;
			eState = STATE_IDLE;
		}
		menu_nav_menu(TYPE_PROMPT);
		break;
	case STATE_STORE_COLOR:
		/* yes */
		if(uiGetButton(BUTTON_ENTER) == 1)
		{
			if(byPosition == 0)
			{
				/* store value */
				byTemp = USER_AREA+(4*byUserNr);
				menuEepWriteColor(byTemp, sUserColor);
			}
			/* return to user color menu */
			menu_display(MENU_USER);
			menu_refresh(byUserNr, TYPE_NORMAL, 0);
			byPosition = byUserNr;
			eState = STATE_IDLE;
			progSet(PROG_IDLE);
		}
		menu_nav_menu(TYPE_PROMPT);
		break;
	case STATE_PROMPT:
		/* check encoder */
		menu_nav_menu(TYPE_MAN);
		break;
	default:
		break;
	}
}

rgb_t menuEepReadColor(uint8_t byAdr)
{
	rgb_t sRet = {0,0,0};						/**< return value from EEPROM */
	uint8_t byaData[3] = {0};					/**< single values for colors */

	/* readout 3 bytes from EEPROM */
	(void)i2c_eeprom_read(byAdr, byaData, 3);

	/* map bytes to colors in RGB datatype */
	sRet.byR = byaData[0];
	sRet.byG = byaData[1];
	sRet.byB = byaData[2];

	return sRet;
}

void menuEepWriteColor(uint8_t byAdr, rgb_t sColor)
{
	/* write 3 bytes into EEPROM */
	(void)i2c_eeprom_write_byte(byAdr, sColor.byR);
	i2c_wait_eeprom();
	(void)i2c_eeprom_write_byte(byAdr+1, sColor.byG);
	i2c_wait_eeprom();
	(void)i2c_eeprom_write_byte(byAdr+2, sColor.byB);
	i2c_wait_eeprom();
}

/* local function definitions *********************************************/
static void menu_refresh(uint8_t byPos, uint8_t byType, uint8_t byNewMenu)
{
	static uint8_t byCurPosNormal = 0; /**< last known position in menu (to remove red frame from old position) */
	static uint8_t byCurPosMan 	  = 0; /**< last known position in menu (to remove red frame from old position) */

	if(byNewMenu == 1)
	{
		byCurPosMan = 0;
		byCurPosNormal = 0;
	}

	if(byType == TYPE_NORMAL)
	{
		/* get position of new element */
		psCurrentMenu->byCurElement = byPos;

		/* remove red frame from old position and set red frame around new position */
		gdispFillArea(3, 2+(15*byCurPosNormal), 122, 15, White);
		gdispDrawString(5, 5+(15*byCurPosNormal), psCurrentMenu->psElement[byCurPosNormal].pcName, font, Black);
		gdispFillArea(3, 2+(15*byPos), 122, 15, HTML2COLOR(0x4674e9));
		gdispDrawString(5, 5+(15*byPos), psCurrentMenu->psElement[byPos].pcName, font, White);

		/* update last known position */
		byCurPosNormal = byPos;
	}
	else
	{
		/* see menu_refresh(), but fixed positions below color progressbars */
		if(byCurPosMan != byPos)
		{
			gdispFillArea(3, 80+(15*byCurPosMan), 122, 15, White);
			gdispDrawString(5, 83+(15*byCurPosMan), (byPos == 0)?"Zurueck":"Speichern", font, Black);
			gdispFillArea(3, 80+(15*byPos), 122, 15, HTML2COLOR(0x4674e9));
			gdispDrawString(5, 83+(15*byPos), (byPos == 0)?"Speichern":"Zurueck", font, White);
			byCurPosMan = byPos;
		}
	}
}

static void menu_nav_menu(uint8_t byType)
{
	uint8_t byMax;
	int8_t byEnc = 0;
	
	if(byType == TYPE_NORMAL)
	{
		byMax = (psCurrentMenu->byNumElements-1);
	}
	else
	{
		byMax = 1;
	}
	
	byEnc = uiGetEncoder();

	if(byEnc < 0)
	{
		if(byPosition > 0)
		{
			/* go up */
			if(byType == TYPE_PROMPT)
			{
				menu_prompt_refresh(--byPosition);
			}
			else
			{
				menu_refresh(--byPosition, byType, 0);
			}
		}	
	}
	
	if(byEnc > 0)
	{
		if(byPosition < byMax)
		{
			/* go down */
			if(byType == TYPE_PROMPT)
			{
				menu_prompt_refresh(++byPosition);
			}
			else
			{
				menu_refresh(++byPosition, byType, 0);
			}
		}
	}
}

// static void menu_manual_refresh(uint8_t byPos)
// {
	// static uint8_t byCurPos = 0;
	
	// /* see menu_refresh(), but fixed positions below color progressbars */
	// if(byCurPos != byPos)
	// {
		// gdispFillArea(3, 80+(15*byCurPos), 122, 15, White);
		// gdispDrawString(5, 83+(15*byCurPos), (byPos == 0)?"Zurueck":"Speichern", font, Black);
		// gdispFillArea(3, 80+(15*byPos), 122, 15, HTML2COLOR(0x4674e9));
		// gdispDrawString(5, 83+(15*byPos), (byPos == 0)?"Speichern":"Zurueck", font, White);
		// byCurPos = byPos;
	// }
// }

static void menu_prompt_refresh(uint8_t byPos)
{
	static uint8_t byCurPos = 0;

	/* see menu_refresh(), but fixed positions below color progressbars */
	if(byCurPos != byPos)
	{
		gdispFillArea(10+(60*byCurPos), 83, 45, 15, HTML2COLOR(0xcccccc));
		gdispFillArea(10+(60*byPos), 83, 45, 15, HTML2COLOR(0x4674e9));
		gdispDrawString(30, 85, "Ja", font, White);
		gdispDrawString(80, 85, "Nein", font, White);
		byCurPos = byPos;
	}
}

static void menu_display(uint8_t byID)
{
	uint8_t byI;						/**< counter variable for for-loop */
	uint8_t byPosition = 5;			/**< y-coordinate for positioning text on display (10 is an offset from the top) */

	/* get menu collection by ID */
	psCurrentMenu = &sMenu[byID];

	/* empty display */
	guiShowPage(MAIN_PAGE);

	/* display menu elements */
	for(byI = 0; byI < psCurrentMenu->byNumElements; byI++)
	{
		gdispDrawString(5, byPosition, psCurrentMenu->psElement[byI].pcName, font, Black);
		byPosition += 15;
	}

	/* draw task-bar on the bottom */
	gdispFillArea(0, 107, 128, 20, Blue);
}

/* UNDER CONSTRUCTION ************************************************************/
static void showMan(void)
{
	guiShowPage(DISPLAY_PAGE);

	gdispFillArea(3, 80, 122, 15, HTML2COLOR(0x4674e9));
	gdispDrawString(5, 83, "Speichern", font, White);
	gdispDrawString(5, 98, "Zurueck", font, Black);
}


static void showPrompt(char *caText)
{
	/* empty display */
	guiShowPage(MAIN_PAGE);

	/* Background color (grey) */
	gdispFillArea(0, 0, 127, 127, HTML2COLOR(0xcccccc));
	
	/* Red frame for Text */
	gdispFillArea(5, 20, 117, 30, HTML2COLOR(0xF5A9A9));
	gdispDrawBox(5, 20, 117, 30, HTML2COLOR(0xCC0000));
	
	/* Text */
	gdispDrawString(8, 28, caText, bigfont, HTML2COLOR(0xCC0000));

	/* Buttons */
	gdispFillArea(10, 83, 45, 15, HTML2COLOR(0x4674e9));
	gdispDrawString(30, 85, "Ja", font, White);
	gdispDrawString(80, 85, "Nein", font, White);
}

static void progAct(void)
{
	prog_state_t eCurrState = progGet();										 /**< get current program state */
	uint8_t byProg = psCurrentMenu->psElement[psCurrentMenu->byCurElement].byID; /**< get menu element ID to distinguish program to start */

	switch(byProg)
	{
	case ELEM_BLACKOUT:					/**< switch all lamps off */
			progSet(PROG_BLACKOUT);
		break;
	case ELEM_MOOD:						/**< enable regular moodlight (all lamps get the same color */
		if(eCurrState == PROG_MOOD)
		{
			progSet(PROG_BLACKOUT);
		}
		else
		{
			progSet(PROG_MOOD);
		}
		break;
	case ELEM_MOOD_DELAY:				/**< enable delayied moodlight (each lamp has a hue shifted by 30°) */
		if(eCurrState == PROG_MOOD_DELAY)
		{
			progSet(PROG_BLACKOUT);
		}
		else
		{
			progSet(PROG_MOOD_DELAY);
		}
		break;
	case ELEM_RAND_ALL:
		if(eCurrState == PROG_RAND_ALL)
		{
			progSet(PROG_BLACKOUT);
		}
		else
		{
			progSet(PROG_RAND_ALL);
		}
		break;
	case ELEM_RAND_EACH:
		if(eCurrState == PROG_RAND_EACH)
		{
			progSet(PROG_BLACKOUT);
		}
		else
		{
			progSet(PROG_RAND_EACH);
		}
		break;
	default:
		break;
	}
}

static void setColor(void)
{
	uint8_t byColor = psCurrentMenu->psElement[psCurrentMenu->byCurElement].byID; /**< get menu element ID to distinguish selected color */
	rgb_t byCol = {0,0,0};	/**< RGB data for selected color */
	uint8_t byWhite = 0;	/**< value of white lamp */

	switch(byColor)
	{
	case ELEM_RED:
		byCol.byR = 255;
		break;
	case ELEM_GREEN:
		byCol.byG = 255;
		break;
	case ELEM_BLUE:
		byCol.byB = 255;
		break;
	case ELEM_WHITE:
		byWhite = 255;
		break;
	case ELEM_MAG:
		byCol.byR = 255;
		byCol.byB = 255;
		break;
	case ELEM_CYAN:
		byCol.byB = 255;
		byCol.byG = 255;
		break;
	case ELEM_YEL:
		byCol.byR = 255;
		byCol.byG = 255;
		break;
	case ELEM_USER_1:
		byCol = menuEepReadColor(USER_AREA);
		break;
	case ELEM_USER_2:
		byCol = menuEepReadColor(USER_AREA+(1*4));
		break;
	case ELEM_USER_3:
		byCol = menuEepReadColor(USER_AREA+(2*4));
		break;
	case ELEM_USER_4:
		byCol = menuEepReadColor(USER_AREA+(3*4));
		break;
	case ELEM_USER_5:
		byCol = menuEepReadColor(USER_AREA+(4*4));
		break;
	default:
		break;
	}

	/* set color to desired value and start transition */
	progSetColor(byCol);
	progSetWhite(byWhite);
}

/* UNDER CONSTRUCTION **********************************************************/
static void setScene(void)
{
	uint8_t byScene = byPosition; //psCurrentMenu->psElement[psCurrentMenu->byCurElement].byID;
	progSetScene(byScene);
}

static void setSpeed(void)
{
	progSetSpeed(byPrePos, byPosition);
}

/* Menu navigation ************************************************************/
//static void menu_update_encoder(void)
//{
//	iEncValue += uiReadEncoder();
//}
