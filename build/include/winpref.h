/***********************************************************************
 *
 *	Copyright 1997, University Corporation for Atmospheric Research.
 *
 *	winpref.h
 *
 *	Structure for saving logging widget info.
 *
 *	History:
 *
 *	 4/97	COMET	Original copy
 *
 ***********************************************************************/


#ifndef _WINPREF_H
#define _WINPREF_H

#include "children.h"


/*
 *	Dialogs.
 */

typedef struct {
	ToggleButtonObjectType	verbose_0;	/* Verbose level 0 button */
	ToggleButtonObjectType	verbose_1;	/* Verbose level 1 button */
	ToggleButtonObjectType	verbose_2;	/* Verbose level 2 button */
	ToggleButtonObjectType	verbose_3;	/* Verbose level 3 button */
	ToggleButtonObjectType	out;		/* Log to standard out */
	ToggleButtonObjectType	garplog;	/* Log to GARP log window */	
	ToggleButtonObjectType	file;		/* Log to a file */	
	ScrolledWObjectType	log_text;	/* Scrolled log text */
} GuiWinPrefDialogType;


GuiWinPrefDialogType *GetGuiWinPrefDialog();


#define SetVerboseLevel_0W(g,c)		((g)->verbose_0.w=(c))
#define GetVerboseLevel_0W(g)		((g)->verbose_0.w)

#define SetVerboseLevel_1W(g,c)		((g)->verbose_1.w=(c))
#define GetVerboseLevel_1W(g)		((g)->verbose_1.w)

#define SetVerboseLevel_2W(g,c)		((g)->verbose_2.w=(c))
#define GetVerboseLevel_2W(g)		((g)->verbose_2.w)

#define SetVerboseLevel_3W(g,c)		((g)->verbose_3.w=(c))
#define GetVerboseLevel_3W(g)		((g)->verbose_3.w)

#define SetGuiStdoutW(g,c)		((g)->out.w=(c))
#define GetGuiStdoutW(g)		((g)->out.w)

#define SetGuiGarpLogW(g,c)		((g)->garplog.w=(c))
#define GetGuiGarpLogW(g)		((g)->garplog.w)

#define SetGuiGarpFileW(g,c)		((g)->file.w=(c))
#define GetGuiGarpFileW(g)		((g)->file.w)

#define SetLogInfoW(g,c)		((g)->log_text.w=(c))
#define GetLogInfoW(g)			((g)->log_text.w)


#endif  /*   _WINPREF_H   */
