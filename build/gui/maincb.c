/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	maincb.c
 *
 *	Callbacks used by main dialog.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	05/97	COMET	Moved cursor tracking and image data saving
 *			callbacks out of here.
 *	 6/97	COMET	Added SetImplicitImageResetCB function.
 *	 8/97	COMET	Moved UserDefinedGareaCB() into callbacks.c
 *	 3/98	COMET	Added TitleToggleCB to toggle title display.
 ***********************************************************************/


#include "guimacros.h"
#include "genglobs.h"
#include "mainwin.h"
#include "winobj.h"
#include "utils.h"
#include "wincb.h"


void
SetFrame ( Widget frame, int width, int height, Widget main_canvas,
	   int canvas_width, int canvas_height )
{
	Arg 			al[4];
	register int		ac = 0;
	WindowObjectType	*wo;


	wo = GetActiveWindowObject ();

/*
 *	Remove the resize callback while we change component sizes
 */	
	XtRemoveCallback ( wo->canvas, XmNresizeCallback, 
			CanvasResizeCB, (XtPointer) wo ); 

/*
 *	Set new frame height and width.
 */
	XtVaSetValues ( frame, XmNheight, height,
			XmNwidth, width, NULL );

	XtVaSetValues ( main_canvas, XmNheight, canvas_height, NULL );

/*
 *	Force a redraw
 */
	WindowObjectResize( wo, canvas_width, canvas_height );


/*
 *	Add the resize callback back
 */
	XtAddCallback ( wo->canvas, XmNresizeCallback, 
			CanvasResizeCB, (XtPointer) wo ); 


}


void
change_frame (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
extern Widget icon_frame;
extern Widget statusbar_frame;
extern Widget main_canvas;
int glob_fh;

	Widget		frame;
	int		this_frame;
	int		frame_width, frame_height;
	Dimension	canvas_width, canvas_height;

        XmToggleButtonCallbackStruct *cbs = 
	              (XmToggleButtonCallbackStruct *) xt_call_data;

	this_frame = (int) client_data;
/*
 *	Set frame from input.
 */
	switch ( this_frame ) {
	case ( ICONFRAME ):
	  frame = icon_frame;
	  glob_fh = 54;		/* Get default frame height. */
	  break;
	case ( STATUSBARFRAME ):
	  frame = statusbar_frame;
	  glob_fh = 22;		/* Get default frame height. */
	  break;
	default:
	  frame = icon_frame;
	  break;
	}

/*
 *	Get current drawing area dimensions.
 */
	XtVaGetValues( main_canvas,
	    XmNwidth,  &canvas_width,
	    XmNheight, &canvas_height,
	    NULL);

/*
 *	Set new frame height.
 */
	if ( cbs->set ) {
	    frame_height = glob_fh;
	    canvas_height -= glob_fh - 1;
	    frame_width = canvas_width;
	}
	else {
	    frame_height = 1;
	    canvas_height += glob_fh - 1;
	    frame_width = 1;
	}

/*
 *	Redraw frame and drawing area.
 */
	SetFrame ( frame, frame_width, frame_height, main_canvas, 
	           canvas_width, canvas_height );
}


/* 
 * Sets the graphics window so a graphic will either fill the window or
 * fill only that part of the window strictly defined by a graphics area.
 */

void
SetAreaDisplayModeCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	XmToggleButtonCallbackStruct *cbs =
			(XmToggleButtonCallbackStruct *) xt_call_data;

	ToggleAreaDisplaySetting ( cbs->set );
}



/* 
 * Sets the mode for whether GARP displays metobjects as we go, or 
 * when the whole frame is finished.
 */

void
SetPartialFrameRefreshCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	XmToggleButtonCallbackStruct *cbs =
			(XmToggleButtonCallbackStruct *) xt_call_data;

	SetPartialFrameRefreshFlag ( GetActiveWindowObject(), cbs->set );
}


/* 
 * Sets whether GARP performs a reset when images are loaded.
 */

void
SetImplicitImageResetCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
        GlobalDataObjectType    *gd;

	XmToggleButtonCallbackStruct *cbs =
			(XmToggleButtonCallbackStruct *) xt_call_data;

        gd = GetGlobalDataObject();

	SetImageResetFlag ( gd, cbs->set );
}


void TitleToggleCB (Widget w, XtPointer client_data, XtPointer xt_call_data)
{
/* 
 * Toggle title display.
 */
        GlobalDataObjectType    *gd;

        XmToggleButtonCallbackStruct *cbs = 
			(XmToggleButtonCallbackStruct *) xt_call_data ;

        gd = GetGlobalDataObject();

	SetTitleFlag ( gd, cbs->set );
}
