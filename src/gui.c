#include "main.h"
#include "gui.h"
#include "rendering_functions.h"
#include "widgetstyles.h"
#include "resources_manager.h"

GHandle ghContainer;
GHandle ghMainContainer;

GHandle ghBarBlue;
GHandle ghBarRed;
GHandle ghBarGreen;

static void createMainMenu(void);

static bool_t _createPageDisplay_Page(void)
{
	GWidgetInit wi;

	gwinWidgetClearInit(&wi);

	// Page container
	wi.g.show = FALSE;
	wi.g.x = 0;
	wi.g.y = 0;
	wi.g.width = 128;
	wi.g.height = 80;
	wi.g.parent = 0;
	wi.text = "Container";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghContainer = gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);

	// ghBarRed
	wi.g.show = TRUE;
	wi.g.x = 5;
	wi.g.y = 5;
	wi.g.width = 100;
	wi.g.height = 20;
	wi.g.parent = ghContainer;
	wi.text = "Red";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghBarRed = gwinProgressbarCreate(0, &wi);
	gwinProgressbarSetRange(ghBarRed, 0, 255);
	gwinProgressbarSetPosition(ghBarRed, 0);

	// ghBarGreen
	wi.g.show = TRUE;
	wi.g.x = 5;
	wi.g.y = 30;
	wi.g.width = 100;
	wi.g.height = 20;
	wi.g.parent = ghContainer;
	wi.text = "Green";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghBarGreen = gwinProgressbarCreate(0, &wi);
	gwinProgressbarSetRange(ghBarGreen, 0, 255);
	gwinProgressbarSetPosition(ghBarGreen, 0);

	// ghBarBlue
	wi.g.show = TRUE;
	wi.g.x = 5;
	wi.g.y = 55;
	wi.g.width = 100;
	wi.g.height = 20;
	wi.g.parent = ghContainer;
	wi.text = "Blue";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghBarBlue = gwinProgressbarCreate(0, &wi);
	gwinProgressbarSetRange(ghBarBlue, 0, 255);
	gwinProgressbarSetPosition(ghBarBlue, 0);
	
	return TRUE;
}

static void createMainMenu(void)
{
	GWidgetInit wi;

	gwinWidgetClearInit(&wi);

	// Page container
	wi.g.show = FALSE;
	wi.g.x = 0;
	wi.g.y = 0;
	wi.g.width = 128;
	wi.g.height = 128;
	wi.g.parent = 0;
	wi.text = "Container";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghMainContainer = gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);

}

void guiShowPage(guiPage page)
{
	// Hide all pages
	gwinHide(ghContainer);
	gwinHide(ghMainContainer);

	// Show the selected page
	switch (page) {
		case DISPLAY_PAGE:
			gwinShow(ghContainer);
			break;
		case MAIN_PAGE:
			gwinShow(ghMainContainer);
			break;
		default:
			break;
	}
}

bool_t guiInit(void)
{
	// Initialize resources
	if (!guiResourcesManagerInit()) {
		return FALSE;
	}

	// Set GWIN default values
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);

	// Create display pages
	_createPageDisplay_Page();
	createMainMenu();

	return TRUE;
}

