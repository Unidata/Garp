/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gemclear.c
 *
 *	Window clearing function.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include "winobj.h"

GemClear( WindowObjectType *wo )

{

	WindowObjectType	*orig_wo;

	orig_wo = GetActiveWindowObject();

	orig_wo = GetActiveWindowObject();
	if( wo != orig_wo ) {
		SetActiveWindowObject( wo );
	}

	
	/*
	 * Make X call to clear window.  The background color has been
	 * set as the current foreground color.
	 */
	XFillRectangle( GetWindowObjectDisplay( wo ), 
		GetWindowObjectWindowID( wo ),
		GetWindowObjectGC( wo ), 0, 0,
		GetWindowObjectWidth( wo ),
		GetWindowObjectHeight( wo ) );

	if( wo != orig_wo ) {
		SetActiveWindowObject( orig_wo );
	}

	return;

}

