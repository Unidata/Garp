/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	garpinit.c
 *
 *	GARP initialization function.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 8/97	COMET	Initialize the auto-update subsystem.
 *	 8/97	COMET	Initialize data type templates for time matching.
 *	10/97	COMET	Added call to initialize line attribute dialog
 *	11/97	COMET	Removed call to InitDataTimeObject.
 *
 ***********************************************************************/


#define GLOBAL
#include "xwcmn.h"

#include "winobj.h"
#include "wincb.h"


WindowObjectType   mainWindow[5];


int GarpInitialize( Display *display, 
	XtAppContext app_context, 
	Widget main_shell, 
	Widget main_canvas )

{

	WindowObjectType        *mw;
	int			iret;


/*
 *	Initialize the WindowOjbect data structures - No X or gempak calls.
 */
	InitWindowObjects( mainWindow,  XtNumber( mainWindow ) );


/*
 *	Create a WindowObject - No X or gempak calls. 
 */
	mw = CreateWindowObject ( "Main" );


/*
 *	Make it the active WindowObject - No X or gempak calls.
 */
	SetActiveWindowObject ( mw );

/*
 *      Attach the widget drawing canvas to the WindowObject.
 */
	SetWindowObjectCanvas( mw, main_canvas );


/*
 *	Setup event handlers and callbacks for this window.  No Gempak 
 *	calls.
 */
	WindowObjectRegisterCallbacks( mw );

/*
 *	Initialize the auto-update subsystem.
 */
	InitAutoUpdate();

/*
 *	Do minimal GEMPAK and NWX initialization.
 */
	if( GempakInit ( main_shell ) ) {
		printf("GarpInitialize:  GempakInit failed\n");
		return( 1 );
	}

/*
 *	Fill in all the X11 Window parameters.   Have to wait until
 *      the window is realized before the size and window id is
 *      known.
 */
	SetWindowObjectXstuff( mw );


/*
 *	Attach the menubar-drawing canvas to GEMPAK and
 *	draw something into the graphics window. 
 */
	AttachGemwin ( main_shell, GetWindowObjectName(mw),
		GetWindowObjectCanvas(mw), &iret ); 

/*
 *	Initialize the line attributes dialog. (Must be done after gempak
 *	is initialized for the colors.)
 */
	InitLineAttributesPrefDialog ();

/*
 *	Initialize the lat/lon grid and maps preferences dialogs.
 */
	InitLatLonGridDialog ();

/*
 *	Initialize Map background objects
 */
	InitMapList();


/*
 *	Create the title object dialog window.
 */
	TitleObjectCreateList( mw, main_shell );

/*
 *	Put something in the statusbar...
 */
	UpdateFrameComponents ( mw );

/*
 *	Success, hopefully...
 */
	return( 0 );

}
