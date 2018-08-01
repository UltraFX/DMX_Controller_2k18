#ifndef _GFXCONF_H
#define _GFXCONF_H


///////////////////////////////////////////////////////////////////////////
// GOS                                                                   //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_OS_FREERTOS                      TRUE
#define GFX_OS_HEAP_SIZE                         256
#define GFX_OS_NO_INIT                           TRUE

#define GFX_COMPILER                             GFX_COMPILER_GCC
#define GFX_CPU                                  GFX_CPU_CORTEX_M3
#define GFX_CPU_ENDIAN                           GFX_CPU_ENDIAN_LITTLE


///////////////////////////////////////////////////////////////////////////
// GDISP                                                                 //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GDISP TRUE

#define GDISP_NEED_MULTITHREAD TRUE
#define GDISP_NEED_CONTROL TRUE
#define GDISP_NEED_MULTITHREAD                       TRUE

#define GDISP_NEED_STARTUP_LOGO FALSE
#define GDISP_STARTUP_COLOR HTML2COLOR(0x000000)
#define GDISP_DEFAULT_ORIENTATION                    GDISP_ROTATE_270    // If not defined the native hardware orientation is used.

#define GDISP_NEED_TEXT TRUE
#define GDISP_NEED_TEXT_WORDWRAP TRUE
#define GDISP_NEED_TEXT_UTF8
	#define GDISP_INCLUDE_FONT_DEJAVUSANS10          TRUE
	#define GDISP_INCLUDE_FONT_DEJAVUSANS12          TRUE
    #define GDISP_INCLUDE_FONT_DEJAVUSANS16          TRUE
    #define GDISP_INCLUDE_FONT_DEJAVUSANS20          FALSE
    #define GDISP_INCLUDE_FONT_DEJAVUSANS24          FALSE
    #define GDISP_INCLUDE_FONT_DEJAVUSANS32          FALSE
    #define GDISP_INCLUDE_FONT_DEJAVUSANSBOLD12      FALSE
	#define GDISP_INCLUDE_USER_FONTS TRUE



///////////////////////////////////////////////////////////////////////////
// GWIN                                                                  //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GWIN TRUE

#define GWIN_NEED_WINDOWMANAGER TRUE

#define GWIN_NEED_WIDGET TRUE

#define GWIN_NEED_CONTAINERS TRUE
	#define GWIN_NEED_CONTAINER TRUE

#define GWIN_NEED_PROGRESSBAR                    TRUE


///////////////////////////////////////////////////////////////////////////
// GEVENT                                                                //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GEVENT TRUE


///////////////////////////////////////////////////////////////////////////
// GTIMER                                                                //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GTIMER TRUE


///////////////////////////////////////////////////////////////////////////
// GQUEUE                                                                //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GQUEUE TRUE

#define GQUEUE_NEED_ASYNC TRUE


///////////////////////////////////////////////////////////////////////////
// GINPUT                                                                //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GINPUT TRUE

#define GINPUT_NEED_MOUSE TRUE


///////////////////////////////////////////////////////////////////////////
// GFILE                                                                 //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GFILE TRUE

#define GFILE_NEED_ROMFS TRUE
#define GFILE_MAX_GFILES 50

#endif // _GFXCONF_H
