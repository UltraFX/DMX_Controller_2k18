/*
 * menu.c
 *
 *  Created on: 31.05.2018
 *      Author: ndamm
 */

#include "main.h"

/* local Function prototypes **************************************/
static void menu_refresh(uint8_t byPos);

static void menu_display(uint8_t byID);

static void showMan(void);
static void setColor(void);
static void progAct(void);

/* menu structures ************************************************/
enum
{
	ELEM_PROG,
	ELEM_COL,
	ELEM_USER,
	ELEM_MAN,
	ELEM_SET,
	ELEM_MOOD,
	ELEM_BACK,
	ELEM_MORE,

	ELEM_RED,
	ELEM_GREEN,
	ELEM_BLUE,
	ELEM_WHITE,
	ELEM_MAG,
	ELEM_CYAN,
	ELEM_YEL,

	ELEM_USER_0,
	ELEM_USER_1,
	ELEM_USER_2,
	ELEM_USER_3,
};

enum
{
	MENU_MAIN = 0,
	MENU_PROG,
	MENU_COL1,
	MENU_COL2,
	MENU_USER,
	MENU_SET,
	MENU_NONE = 0xff
};

typedef struct
{
	uint8_t byID;
	char *pcName;
	uint8_t bAct;
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
	{ELEM_PROG,	"Programme", 		1, MENU_PROG, NULL},
	{ELEM_COL, 	"Farbwahl", 		0, MENU_COL1, NULL},
	{ELEM_USER, "Benutzerdefiniert",0, MENU_USER, NULL},
	{ELEM_MAN, 	"Manuell", 			0, MENU_NONE, showMan},
	{ELEM_SET, 	"Einstellungen", 	0, MENU_SET, NULL}
};//sProgramMenu

static element_t sProgramMenu[] =
{
	{ELEM_MOOD, 	"Moodlight", 	1, MENU_NONE, progAct},
	{ELEM_BACK, 	"Zurueck", 		0, MENU_MAIN, NULL}
};

static element_t sColor1Menu[] =
{
	{ELEM_RED, 	 	"Rot", 			1, MENU_NONE, setColor},
	{ELEM_GREEN, 	"Gruen", 		0, MENU_NONE, setColor},
	{ELEM_BLUE, 	"Blau", 		0, MENU_NONE, setColor},
	{ELEM_WHITE, 	"Weiss", 		0, MENU_NONE, setColor},
	{ELEM_MORE, 	"Weiter", 		0, MENU_COL2, NULL},
};

static element_t sColor2Menu[] =
{
	{ELEM_MAG, 		"Magenta", 		1, MENU_NONE, setColor},
	{ELEM_CYAN, 	"Cyan", 		0, MENU_NONE, setColor},
	{ELEM_YEL, 		"Gelb", 		0, MENU_NONE, setColor},
	{ELEM_BACK, 	"Zurueck", 		0, MENU_MAIN, NULL},
};

static element_t sUserCol[] =
{
		{ELEM_USER_0, "BENUTZER 0", 1, MENU_NONE, setColor},
		{ELEM_USER_1, "BENUTZER 1", 1, MENU_NONE, setColor},
		{ELEM_USER_2, "BENUTZER 2", 1, MENU_NONE, setColor},
		{ELEM_USER_3, "BENUTZER 3", 1, MENU_NONE, setColor},
		{ELEM_BACK,   "Zurueck", 	0, MENU_MAIN, NULL}
};

static menu_t sMenu[] =
{
	{MENU_MAIN, 	"Hauptmenue",		5, 0, sMainMenu},
	{MENU_PROG, 	"Programme", 		2, 0, sProgramMenu},
	{MENU_COL1, 	"Farben1", 			5, 0, sColor1Menu},
	{MENU_COL2, 	"Farben2", 			4, 0, sColor2Menu},
	{MENU_USER,		"Benutzerdefiniert",5, 0, sUserCol}
};

/* menu states *****************************************************/
typedef enum
{
	STATE_INIT = 0,
	STATE_IDLE,
	STATE_MANUAL
}menu_state_t;

/* local variables *************************************************/
static menu_t *psCurrentMenu;

static menu_state_t eState = STATE_INIT;

static font_t font;

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
		if(uiGetButton(BUTTON_ENTER) == 1)
		{
			if(psCurrentMenu->psElement[byPosition].pFunc != NULL)
			{
				psCurrentMenu->psElement[byPosition].pFunc();
			}
			else
			{
				//psCurrentMenu = &sMenu[psCurrentMenu->psElement[byPosition].byNext];
				menu_display(psCurrentMenu->psElement[byPosition].byNext);
				menu_refresh(0);
				byPosition = 0;
			}
		}

		if(iEncValue <= -4)
		{
			if(byPosition > 0)
			{
				menu_refresh(--byPosition);
			}
			iEncValue = 0;
		}

		if(iEncValue >= 4)
		{
			if(byPosition < (psCurrentMenu->byNumElements-1))
			{
				menu_refresh(++byPosition);
			}
			iEncValue = 0;
		}
		break;
	case STATE_MANUAL:
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

/* local function definitions *********************************************/
static void menu_refresh(uint8_t byPos)
{
	static uint8_t byCurPos = 0;

	psCurrentMenu->byCurElement = byPos;

	gdispDrawBox(3, 3+(20*byCurPos), 122, 25, White);
	gdispDrawBox(3, 3+(20*byPos), 122, 25, Red);

	byCurPos = byPos;
	//}

	/*if(byPos > byCurPos)
	{
		byCurPos = byPos;
	}*/
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
	case ELEM_MOOD:
		if(eCurrState == PROG_MOOD)
		{
			progSet(PROG_IDLE);
		}
		else
		{
			progSet(PROG_MOOD);
		}
		break;
	default:
		break;
	}
}

static void setColor(void)
{
	uint8_t byColor = psCurrentMenu->psElement[psCurrentMenu->byCurElement].byID;
	static rgb_t byCol = {0,0,0};
	static uint8_t byWhite = 0;

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
		byCol.byR = 255;
		break;
	case ELEM_YEL:
		byCol.byR = 255;
		byCol.byG = 255;
		break;
	case ELEM_USER_0:
		byCol.byR = 255;
		byCol.byG = 255;
		byCol.byB = 255;
		break;
	case ELEM_USER_1:
		byCol.byR = 50;
		byCol.byG = 255;
		break;
	case ELEM_USER_2:
		break;
	case ELEM_USER_3:
		break;
	default:
		break;
	}

	dmxSetRGB(2, (rgb_t)byCol);
	dmxSetWhite(2, byWhite);
}
