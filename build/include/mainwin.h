/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	mainwin.h
 *
 *	Header file for main window.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/




#ifndef _MAINWIN_H
#define _MAINWIN_H

#include "children.h"


/*
 *	Dialogs.
 */

typedef struct {
	Widget			main_shell;	/* Main app shell       */
	Widget			satellite_icon; /* Image data icon	*/
	Widget			sfc_icon;	/* Surface obs icon	*/
	Widget			profiler_icon;	/* Profiler data icon	*/
	Widget			upperair_icon;	/* Sounding data icon	*/
	Widget			gridh_icon;	/* Horizontal grid icon	*/
	Widget			gridc_icon;	/* Cross-section icon	*/
	Widget			gridz_icon;     /* Vertical grid icon   */
	Widget			gridt_icon;	/* Time/height grid icon*/
	Widget			text_icon;	/* Text data icon	*/
	Widget			clear_icon;	/* Clear icon		*/
	Widget			reset_icon;	/* Reset icon		*/
	Widget			garp_logo;	/* Garp logo button	*/
	ButtonParentObjectType	gareapm;	/* Geographic area	*/
	ButtonParentObjectType	mappm;		/* Colored maps		*/
	TextFieldObjectType	garea;		/* Graphics area text	*/
	TextFieldObjectType	proj;		/* Map projection	*/
} GuiMainWinDialogType;



GuiMainWinDialogType *GetGuiMainWinDialog();

#define SetGuiMainAppShellW(m,c)		((m)->main_shell=(c))
#define GetGuiMainAppShellW(m)			((m)->main_shell)

#define SetGuiSatelliteIconW(m,c)		((m)->satellite_icon=(c))
#define GetGuiSatelliteIconW(m)			((m)->satellite_icon)

#define SetGuiSurfaceIconW(m,c)			((m)->sfc_icon=(c))
#define GetGuiSurfaceIconW(m)			((m)->sfc_icon)

#define SetGuiProfilerIconW(m,c)		((m)->profiler_icon=(c))
#define GetGuiProfilerIconW(m)			((m)->profiler_icon)

#define SetGuiUpperairIconW(m,c)		((m)->upperair_icon=(c))
#define GetGuiUpperairIconW(m)			((m)->upperair_icon)

#define SetGuiGridhIconW(m,c)			((m)->gridh_icon=(c))
#define GetGuiGridhIconW(m)			((m)->gridh_icon)

#define SetGuiGridcIconW(m,c)			((m)->gridc_icon=(c))
#define GetGuiGridcIconW(m)			((m)->gridc_icon)

#define SetGuiGridzIconW(m,c)			((m)->gridz_icon=(c))
#define GetGuiGridzIconW(m)			((m)->gridz_icon)

#define SetGuiGridtIconW(m,c)			((m)->gridt_icon=(c))
#define GetGuiGridtIconW(m)			((m)->gridt_icon)

#define SetGuiTextIconW(m,c)			((m)->text_icon=(c))
#define GetGuiTextIconW(m)			((m)->text_icon)

#define SetGuiClearIconW(m,c)			((m)->clear_icon=(c))
#define GetGuiClearIconW(m)			((m)->clear_icon)

#define SetGuiResetIconW(m,c)			((m)->reset_icon=(c))
#define GetGuiResetIconW(m)			((m)->reset_icon)

#define SetGuiGarpLogoW(m,c)			((m)->garp_logo=(c))
#define GetGuiGarpLogoW(m)			((m)->garp_logo)

#define SetGareaW(m,c)				((m)->gareapm.parent=(c))
#define GetGareaW(m)				((m)->gareapm.parent)

#define SetContourMapW(m,c)			((m)->mappm.parent=(c))
#define GetContourMapW(m)			((m)->mappm.parent)

#define SetGareaTextW(m,c)			((m)->garea.w=(c))
#define GetGareaTextW(m)			((m)->garea.w)

#define SetProjectionTextW(m,c)			((m)->proj.w=(c))
#define GetProjectionTextW(m)			((m)->proj.w)

#endif  /*   _MAINWIN_H   */
