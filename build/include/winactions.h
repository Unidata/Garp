/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	winactions.h
 *
 *	Translation action declarations.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/




#ifndef _ACTIONS_H
#define _ACTIONS_H

#include <X11/X.h>
#include <X11/Intrinsic.h>

#include "winobj.h"

#define	DOUBLECLICKTIMEOUT  250
#define DOWNUPMAXTIME	    150

/*
 *	These declarations and macros would eventually be used to set and
 *	get the current functions for certain user interaction with the
 *	mouse. For example, clicking on something on the screen might
 *	do different things at different times, depending on what the
 *	SingleClickActionFunc is. Right now, the functions that are called
 *	are hard-coded into the Do*Task routines so these are not used.
 *	For each user interaction, (ie, a single mouse click) there are
 *	draw and action functions; the draw function allows something to
 *	appear on the screen (box, crosshair, etc...), and the action
 *	actually does something (zoom, update motion, etc).
 *	Depending on what the "mode" is, one of these might be NULL.
 *	Again though, they are not currently used. -jim
 */

#ifdef WINACTIONS_GLOBAL
void	(*SingleClickDrawFunc)();
void	(*SingleClickActionFunc)();
void	(*DoubleClickDrawFunc)();
void	(*DoubleClickActionFunc)();
void	(*ButtonDownMotionDrawFunc)();
void	(*ButtonDownMotionActionFunc)();
void	(*EndButtonDownMotionDrawFunc)();
void	(*EndButtonDownMotionActionFunc)();

#define SetSingleClickDrawFunc(x)		(SingleClickDrawFunc=x)
#define SetSingleClickActionFunc(x)		(SingleClickActionFunc=x)
#define SetDoubleClickDrawFunc(x)		(DoubleClickDrawFunc=x)
#define SetDoubleClickActionFunc(x)		(DoubleClickActionFunc=x)
#define SetButtonDownMotionDrawFunc(x)		(ButtonDownMotionDrawFunc=x)
#define SetButtonDownMotionActionFunc(x)	(ButtonDownMotionActionFunc=x)
#define SetEndButtonDownMotionDrawFunc(x)	(EndButtonDownMotionDrawFunc=x)
#define SetEndButtonDownMotionActionFunc(x)	(EndButtonDownMotionActionFunc=x)

#define GetSingleClickDrawFunc()		(SingleClickDrawFunc)
#define GetSingleClickActionFunc()		(SingleClickActionFunc)
#define GetDoubleClickDrawFunc()		(DoubleClickDrawFunc)
#define GetDoubleClickActionFunc()		(DoubleClickActionFunc)
#define GetButtonDownMotionDrawFunc()		(ButtonDownMotionDrawFunc)
#define GetButtonDownMotionActionFunc()		(ButtonDownMotionActionFunc)
#define GetEndButtonDownMotionDrawFunc()	(EndButtonDownMotionDrawFunc)
#define GetEndButtonDownMotionActionFunc()	(EndButtonDownMotionActionFunc)
#endif




void HandlePointerMotion(	Widget  w,
			XEvent *event, 
			String *parms,
			Cardinal *nparms );

void HandleBtn1Clicks(	Widget  w,
			XEvent *event, 
			String *parms,
			Cardinal *nparms );

void HandleCtrlBtn1Click(	Widget  w,
			XEvent *event, 
			String *parms,
			Cardinal *nparms );

void HandleBtn1DownMotion(	Widget  w,
			XEvent *event, 
			String *parms,
			Cardinal *nparms );

void GetInputFocus(	Widget w,
			XEvent *event,
			String *parms,
			Cardinal *nparms );

void AnimPlayToggle(	Widget w,
			XEvent *event,
			String *parms,
			Cardinal *nparms );
			
void AnimFwd(		Widget w,
			XEvent *event,
			String *parms,
			Cardinal *nparms );
			
void AnimBack(		Widget w,
			XEvent *event,
			String *parms,
			Cardinal *nparms );

void AnimRockToggle(	Widget w,
			XEvent *event,
			String *parms,
			Cardinal *nparms );

void HandleBtn3Clicks(	Widget  w,
			XEvent *event, 
			String *parms,
			Cardinal *nparms );



#endif /*  _ACTIONS_H  */
