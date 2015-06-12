/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	widgetutil.c
 *
 *	Widget refresh utility.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 9/97	COMET	Added XFlush call
 *	 1/98	COMET	Added cast
 *
 ***********************************************************************/

#include "winobj.h"
#include <Xm/Xm.h>
#include <X11/IntrinsicP.h>

/*
 *  Widget utilities
 */


/*
 *  RefreshWidgetNow() forces a widget to be redrawn right away. Reduces
 *  flicker in label widgets. May have other applications.
 */

void RefreshWidgetNow ( Widget wid )
{

	XExposeEvent		xevent;
	int			h, w;

	xevent.type    = Expose;
	if ( (xevent.display = XtDisplay (wid)) == NULL ) return;
	if ( (xevent.window  = XtWindow (wid))  == (Window) NULL ) return;
	xevent.x = 0;
	xevent.y = 0;
	XtVaGetValues (wid, XmNwidth, &w, XmNheight, &h, NULL);
	xevent.width  = w;
	xevent.height = h;

	(XtClass (wid))->core_class.expose (wid, (XEvent *)&xevent, NULL);

	XFlush ( GetWindowObjectDisplay( GetActiveWindowObject() ));
}

