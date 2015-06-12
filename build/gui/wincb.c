/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	wincb.c
 *
 *	Callbacks and utilities for rubber banding box and dropdown labels.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	12/97	COMET	Added event handler WindowStateChangeCB and
 *			calls to refresh image colors
 *
 ***********************************************************************/


#include "genglobs.h"
#include "winobj.h"
#include "wincb.h"
#include "mainwin.h"


void WindowObjectRegisterCallbacks( WindowObjectType *wo )
{

	GuiMainWinDialogType	*mw;
 
    
	mw = GetGuiMainWinDialog();

	XtAddCallback ( wo->canvas, XmNexposeCallback, 
		WindowObjectExposeCB , (XtPointer) wo );
	XtAddCallback ( wo->canvas, XmNresizeCallback, 
		CanvasResizeCB, (XtPointer) wo ); 

	XtAddEventHandler ( wo->canvas, EnterWindowMask, False,
    			GetInputFocus, (XtPointer) NULL );

	XtAddEventHandler ( GetGuiMainAppShellW(mw), StructureNotifyMask, False,
    			WindowStateChangeCB, (XtPointer) NULL );

	DropDownRegisterAllButtons();

}




void WindowObjectExposeCB ( Widget    w,
                 XtPointer clientData,
                 XtPointer callData )
{

	WindowObjectType	*wo;
	Pixmap			pixmap;
	XExposeEvent		*ev;
	XAnyEvent		*av;

	XmDrawingAreaCallbackStruct *data =
                (XmDrawingAreaCallbackStruct *) callData;



/*
 *	Get a better pointer to the event and check the event
 *	type.
 */
	av = &( data->event->xany );
	if( av->type != Expose ) {
	    if ( GetVerboseLevel() > VERBOSE_1 )
	    	printf("  WindowObjectExposeCB: Got event type %d\n", av->type);
	    return;
	}

/*
 *	Get a better handle on the X expose event structure.
 */
	ev = &( data->event->xexpose );


/*
 *	Get the active window.
 */
	wo = GetActiveWindowObject();

/*
 *	If we are in the process of loading items into the display
 *	list, avoid processing expose events until finished.
 */
	if( ! GetProcessExposeEventsFlag( wo ) ) {
	    if ( GetVerboseLevel() > VERBOSE_1 )
		printf("WindowObjectExposeCB: Not processing expose events\n");
	    return;
	}

/*
 *	Get the pixmap currently being displayed in the X window.
 */
	pixmap = GetActivePixmap( wo ); 

/*
 *	It's not an error for this pixmap to not exist, it doesn't 
 *	get created until needed. But only do the XCopyArea if there is
 *	pixmap data
 */
	if( pixmap != (Pixmap) NULL )

	    XCopyArea(	av->display,
			pixmap, 
			ev->window,
			GetWindowObjectGC( wo),
			ev->x,     ev->y, 
			ev->width, ev->height, 
			ev->x,     ev->y );

/*
 *	The newly exposed portion may now contain the pointer anchor point,
 *	so redraw just in case.
 */
	UpdateAnchorPoint ( wo );

	return;

}

void CanvasResizeCB( Widget    w,
                 XtPointer clientData,
                 XtPointer callData )

{

	WindowObjectType	*wo;
	Dimension		width, height;

	wo = GetWindowObjectByWidget( w );

/*
 *	Get the new window size.
 */
	XtVaGetValues(w,
		XmNwidth,  &width,
		XmNheight, &height,
		NULL);

	WindowObjectResize( wo, width, height );

	return;

}


void GetInputFocus(	Widget  w,
                	XtPointer clientData,
                	XEvent *event,
                	Boolean *flag)

{
	if ( GetVerboseLevel() > VERBOSE_1 )
		printf("GetInputFocus()\n");

/*
 *	Allows the keyboard looping controls to work again.
 */
	XmProcessTraversal ( w, XmTRAVERSE_CURRENT );

/*
 *	Since the window now has focus, refresh image colors.
 */
	RefreshImageColorMap ();

}


void WindowStateChangeCB( Widget  w,
			XtPointer clientData,
                	XEvent *event,
                	Boolean *flag)

{

	if (event->type == MapNotify) {

	    if ( GetVerboseLevel() > VERBOSE_1 )
		printf("WindowStateChangeCB(): Window mapped.\n");
/*
 *	    Refresh image colors.
 */
	    RefreshImageColorMap ();

	}

	else {
	    if ( GetVerboseLevel() > VERBOSE_1 )
		printf("WindowStateChangeCB(): state changed.\n");
	}
	
}
