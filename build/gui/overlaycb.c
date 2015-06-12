/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	overlaycb.c
 *
 *	Callbacks and utilities for overlay objects.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 3/97	COMET	Adapted station radio buttons to option menus.
 *
 ***********************************************************************/


#include "guimacros.h"
#include "genglobs.h"
#include "winobj.h"
#include "sfcobj.h"
#include "background.h"
#include "utils.h"
#include "menucb.h"


#define FILLEDCIRCLE    0
#define TRIANGLE        3
#define BOX             4
#define DIAMOND         6
#define STAR            14


void
StationOverlayCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{

	GuiBackgroundDialogType	*bak;
	MetObjectType		*mlist[MAXMETOBJS];
	WindowObjectType	*wo;

	extern Widget		mbg_circle;
	extern Widget		mbg_triangle;
	extern Widget		mbg_box;
	extern Widget		mbg_diamond;
	extern Widget		mbg_star;

	extern Widget		mbg_stid;
	extern Widget		mbg_marker;

	int	index, parm;
	int	count=1;
	int	plot_stid = 0;
	int	plot_marker = 0;
	int	marker;
	int	iret;

        XmPushButtonCallbackStruct *cbs = 
	            (XmPushButtonCallbackStruct *) xt_call_data;

	bak = GetGuiBackgroundDialog();
	wo = GetActiveWindowObject();

/*
 *	Get station file name and parameter to display.
 */
	index = atoi ( GetStationType ( bak ) );
	parm  = atoi ( GetStationParm ( bak ) );

/*
 *	Get values of toggle buttons.
 */
	if ( XmToggleButtonGetState ( mbg_circle ) ) marker = FILLEDCIRCLE;
	if ( XmToggleButtonGetState ( mbg_triangle ) ) marker = TRIANGLE;
	if ( XmToggleButtonGetState ( mbg_box ) ) marker = BOX;
	if ( XmToggleButtonGetState ( mbg_diamond ) ) marker = DIAMOND;
	if ( XmToggleButtonGetState ( mbg_star ) ) marker = STAR;

	if ( XmToggleButtonGetState ( mbg_stid ) )   plot_stid   = 1;
	if ( XmToggleButtonGetState ( mbg_marker ) ) plot_marker = 1;
	
/*
 *	Create met object for each station list.
 */
	mlist[0] = (MetObjectType *) MakeMarkerObject
	    ( index, parm, marker, plot_stid, plot_marker, &iret );

/*
 *	Test for valid input.
 */
	if ( iret == NORMAL ) {
	    CopyObjectListToDisplay( wo, count, mlist );

	    if( wo )
		ProcessDisplayList( wo );
	}

}
