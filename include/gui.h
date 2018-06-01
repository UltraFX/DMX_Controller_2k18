#ifndef _GUI_H
#define _GUI_H

#include "gfx.h"

typedef enum guiPage {
	DISPLAY_PAGE,
	MAIN_PAGE
} guiPage;

// Widget handles
extern GHandle ghContainer;
extern GHandle ghBarBlue;
extern GHandle ghBarRed;
extern GHandle ghBarGreen;

#ifdef __cplusplus
extern "C" {
#endif

	bool_t guiInit(void);
	void guiShowPage(guiPage page);

#ifdef __cplusplus
}
#endif

#endif // _GUI_H
