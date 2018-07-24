/*
 * menu.c
 *
 *  Created on: 31.05.2018
 *      Author: ndamm
 */

#include "main.h"

/* local Function prototypes **************************************/
static void menu_refresh(uint8_t byPos);
static void menu_manual_refresh(uint8_t byPos);

static void menu_display(uint8_t byID);

static void showMan(void);
static void setColor(void);
static void setScene(void);
static void progAct(void);
static void setSpeed(void);

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


//MENU_SET

typedef struct
{
	uint8_t byID;
	char *pcName;
	uint8_t bAct;
	uint8_t bySettable;
	uint8_t byNext;
	void(*pFunc)(void);
} element_t;

typedef struct
{
	uint8_t byID;
	char *pName;
	uint8_t byNumElements;
	uint8_t byCurElement;
	element_t *psElement;
} menu_t;

static element_t sMainMenu[] =
{
	{ELEM_PROG,	"Programme", 		1, 0, MENU_PROG, NULL},
	{ELEM_COL, 	"Farbwahl", 		0, 0, MENU_COL1, NULL},
	{ELEM_USER, "Benutzerdefiniert",0, 1, MENU_USER, NULL},
	{ELEM_SCENE,"Szenen",           0, 1, MENU_SCENE, NULL},
	{ELEM_SET, 	"Einstellungen", 	0, 0, MENU_SET, NULL}
};//sProgramMenu

//{ELEM_MAN, 	"Manuell", 			0, MENU_NONE, showMan},

static element_t sProgramMenu[] =
{
	{ELEM_BLACKOUT, 	"Blackout",     1, 0, MENU_NONE, progAct},
    {ELEM_MOOD, 		"Moodlight", 	0, 0, MENU_NONE, progAct},
	{ELEM_MOOD_DELAY, 	"Moodl. Verz.", 0, 0, MENU_NONE, progAct},
	{ELEM_BACK, 		"Zurueck", 		0, 0, MENU_MAIN, NULL}
};

static element_t sColor1Menu[] =
{
	{ELEM_RED, 	 	"Rot", 			1, 0, MENU_NONE, setColor},
	{ELEM_GREEN, 	"Gruen", 		0, 0, MENU_NONE, setColor},
	{ELEM_BLUE, 	"Blau", 		0, 0, MENU_NONE, setColor},
	{ELEM_WHITE, 	"Weiss", 		0, 0, MENU_NONE, setColor},
	{ELEM_MORE, 	"Weiter", 		0, 0, MENU_COL2, NULL},
};

static element_t sColor2Menu[] =
{
	{ELEM_MAG, 		"Magenta", 		1, 0, MENU_NONE, setColor},
	{ELEM_CYAN, 	"Cyan", 		0, 0, MENU_NONE, setColor},
	{ELEM_YEL, 		"Gelb", 		0, 0, MENU_NONE, setColor},
	{ELEM_BACK, 	"Zurueck", 		0, 0, MENU_MAIN, NULL},
};

static element_t sUserCol[] =
{
	{ELEM_USER_1, "BENUTZER 1", 1, 1,  MENU_NONE, setColor},
	{ELEM_USER_2, "BENUTZER 2", 1, 1, MENU_NONE, setColor},
	{ELEM_USER_3, "BENUTZER 3", 1, 1, MENU_NONE, setColor},
	{ELEM_USER_4, "BENUTZER 4", 1, 1, MENU_NONE, setColor},
	{ELEM_BACK,   "Zurueck", 	0, 0, MENU_MAIN, NULL}
};

static element_t sScenes[] =
{
	{ELEM_SCENE_1, "SZENE 1", 1, 1, MENU_NONE, setScene},
	{ELEM_SCENE_2, "SZENE 2", 1, 1, MENU_NONE, setScene},
	{ELEM_SCENE_3, "SZENE 3", 1, 1, MENU_NONE, setScene},
	{ELEM_SCENE_4, "SZENE 4", 1, 1, MENU_NONE, setScene},
	{ELEM_BACK,   "Zurueck",  0, 0, MENU_MAIN, NULL}
};

static element_t sSetup[] =
{
	{ELEM_SPEED_PROG, 	"Geschw. Prog.",	0, 0, 	MENU_SPEED_PROG, 	NULL},
	{ELEM_SPEED_CHG, 	"Geschw. Wechs.",	0, 0,  	MENU_SPEED_CHG, 	NULL},
	{ELEM_DMX_ADDR, 	"DMX-Addr",			0, 0,	MENU_ADDR, 			NULL},
	{ELEM_DISP, 		"Display.", 		0, 0,	MENU_DISP, 			NULL}
};

static element_t sSpeed[] =
{
	{ELEM_SPEED_SLO, 	"Langsam", 	1, 0,	MENU_NONE, 	setSpeed},
	{ELEM_SPEED_MED, 	"Normal", 	1, 0,   MENU_NONE, 	setSpeed},
	{ELEM_SPEED_HI, 	"Schnell",  1, 0,	MENU_NONE, 	setSpeed},
	{ELEM_BACK, 		"Schnell",  1, 0,	MENU_NONE, 	setSpeed}
};

static menu_t sMenu[] =
{
	{MENU_MAIN, 		"Hauptmenue",		5, 0, sMainMenu},
	{MENU_PROG, 		"Programme", 		4, 0, sProgramMenu},
	{MENU_COL1, 		"Farben1", 			5, 0, sColor1Menu},
	{MENU_COL2, 		"Farben2", 			4, 0, sColor2Menu},
	{MENU_USER,			"Benutzerdefiniert",5, 0, sUserCol},
	{MENU_SCENE,    	"Szenen",			5, 0, sScenes},
	{MENU_SET,    		"Einstellungen",	4, 0, sSetup},
	{MENU_SPEED_PROG,	"Geschwindigkeit",	3, 0, sSpeed},
	{MENU_SPEED_CHG,	"Geschwindigkeit",	3, 0, sSpeed}
};

/* menu states *****************************************************/
typedef enum
{
	STATE_INIT = 0,
	STATE_IDLE,
	STATE_SETTABLE,
	STATE_MANUAL
}menu_state_t;

/* local variables *************************************************/
static menu_t *psCurrentMenu;

static menu_state_t eState = STATE_INIT;

font_t font;

/* Function definitions ********************************************/
void menu_init(void)
{
	psCurrentMenu = &sMenu[0];

	font = gdispOpenFont("DejaVuSans12");
}

void menu_handler(void)
{
	static int8_t iEncValue = 0;
	static uint8_t byMenu = 0, byPosition = 0;

	xDebugMessage xMessage;

	iEncValue += uiReadEncoder();

	switch(eState)
	{
	case STATE_INIT:
		//guiShowPage(MAIN_PAGE);
		menu_display(byMenu);
		menu_refresh(byPosition);
		eState = STATE_IDLE;
		break;
	case STATE_IDLE:
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
				menu_display(psCurrentMenu->psElement[byPosition].byNext);
				menu_refresh(0);
				byPosition = 0;
			}
		}

		/* check encoder */
		if(iEncValue <= -4)
		{
			if(byPosition > 0)
			{
				/* go up */
				menu_refresh(--byPosition);
			}
			iEncValue = 0;
		}

		if(iEncValue >= 4)
		{
			if(byPosition < (psCurrentMenu->byNumElements-1))
			{
				/* go down */
				menu_refresh(++byPosition);
			}
			iEncValue = 0;
		}
		break;
	case STATE_SETTABLE:
		/* Enter-button pressed long */
		if(uiGetButtonLong(BUTTON_ENTER) == 1)
		{
			xMessage.pcMessage = "Lang\n";
			xMessage.wLength = 5;
			xQueueSend(xDebugQueue, &xMessage, portMAX_DELAY);
		}
		/* Enter-button pressed short */
		else if(uiGetButton(BUTTON_ENTER) == 1)
		{
			xMessage.pcMessage = "Kurz\n";
			xMessage.wLength = 5;
			xQueueSend(xDebugQueue, &xMessage, portMAX_DELAY);

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
				menu_refresh(0);
				byPosition = 0;
			}
		}

		/* check encoder */
		if(iEncValue <= -4)
		{
			if(byPosition > 0)
			{
				/* go up */
				menu_refresh(--byPosition);
			}
			iEncValue = 0;
		}

		if(iEncValue >= 4)
		{
			if(byPosition < (psCurrentMenu->byNumElements-1))
			{
				/* go down */
				menu_refresh(++byPosition);
			}
			iEncValue = 0;
		}
		break;
	case STATE_MANUAL:
		if(iEncValue <= -4)
		{
			if(byPosition > 0)
			{
				menu_manual_refresh(0);
			}
			iEncValue = 0;
		}

		if(iEncValue >= 4)
		{
			if(byPosition < (psCurrentMenu->byNumElements-1))
			{
				menu_manual_refresh(1);
			}
			iEncValue = 0;
		}
		
		if(uiGetButton(BUTTON_ENTER) == 1)
		{
			menu_display(MENU_MAIN);
			menu_refresh(3);
			eState = STATE_IDLE;
		}
		break;
	default:
		break;
	}
}

rgb_t menuEepReadColor(uint8_t byAdr)
{
	rgb_t sRet = {0,0,0};
	uint8_t byaData[3] = {0};

	(void)i2c_eeprom_read(byAdr, byaData, 3);

	sRet.byR = byaData[0];
	sRet.byG = byaData[1];
	sRet.byB = byaData[2];

	return sRet;
}

/* local function definitions *********************************************/
static void menu_refresh(uint8_t byPos)
{
	static uint8_t byCurPos = 0;

	psCurrentMenu->byCurElement = byPos;

	gdispDrawBox(3, 3+(20*byCurPos), 122, 25, White);
	gdispDrawBox(3, 3+(20*byPos), 122, 25, Red);

	byCurPos = byPos;
}

static void menu_manual_refresh(uint8_t byPos)
{
	static uint8_t byCurPos = 0;
	
	if(byCurPos != byPos)
	{
		gdispDrawBox(3, 80+(20*byCurPos), 122, 25, White);
		gdispDrawBox(3, 80+(20*byPos), 122, 25, Red);
		byCurPos = byPos;
	}
}

static void menu_display(uint8_t byID)
{
	uint8_t byI;
	uint8_t byPosition = 10;

	psCurrentMenu = &sMenu[byID];

	guiShowPage(MAIN_PAGE);

	for(byI = 0; byI < psCurrentMenu->byNumElements; byI++)
	{
		gdispDrawString(5, byPosition, psCurrentMenu->psElement[byI].pcName, font, Black);
		byPosition += 20;
	}

	gdispFillArea(0, 107, 128, 20, Blue);
}

static void showMan(void)
{
	guiShowPage(DISPLAY_PAGE);
	eState = STATE_MANUAL;
}

static void progAct(void)
{
	prog_state_t eCurrState = progGet();
	uint8_t byProg = psCurrentMenu->psElement[psCurrentMenu->byCurElement].byID;

	switch(byProg)
	{
	case ELEM_BLACKOUT:
			progSet(PROG_BLACKOUT);
		break;
	case ELEM_MOOD:
		if(eCurrState == PROG_MOOD)
		{
			progSet(PROG_BLACKOUT);
		}
		else
		{
			progSet(PROG_MOOD);
		}
		break;
	case ELEM_MOOD_DELAY:
		if(eCurrState == PROG_MOOD_DELAY)
		{
			progSet(PROG_BLACKOUT);
		}
		else
		{
			progSet(PROG_MOOD_DELAY);
		}
		break;
	default:
		break;
	}
}

static void setColor(void)
{
	uint8_t byColor = psCurrentMenu->psElement[psCurrentMenu->byCurElement].byID;
	rgb_t byCol = {0,0,0};
	uint8_t byWhite = 0;

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
		byCol.byR = 255;
		byCol.byG = 255;
		byCol.byB = 255;
		break;
	case ELEM_USER_2:
		byCol.byR = 50;
		byCol.byG = 255;
		break;
	case ELEM_USER_3:
		break;
	case ELEM_USER_4:
		break;
	default:
		break;
	}

	progSetColor(byCol);
	progSetWhite(byWhite);
}

static void setScene(void)
{
	uint8_t byScene = psCurrentMenu->psElement[psCurrentMenu->byCurElement].byID;
	progSetScene(byScene);
}

static void setSpeed(void)
{

}
