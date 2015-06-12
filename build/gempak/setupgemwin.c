/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	setupgemwin.c
 *
 *	GEMPAK window startup functions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <Xm/Xm.h>

#include "underscore.h"

extern Pixel GetGemBackgroundPixel();

void
AttachGemwin ( toplevel, winname, canvas, iret )
	Widget		toplevel;
	char		*winname;
	Widget		canvas;
	int		*iret;

{

	GC		gemgc;
	Pixel		gemBackgroundColor;
	Window		windowid;
        Dimension       wdth, hght;
	XColor          cred;
	Display		*display;
	XColor		color, ignore;
	int		xwdth, xhght, xdpth;
	Cursor          curs;


	display   = XtDisplay ( canvas );
        windowid  = XtWindow ( canvas );
        gemgc     = XCreateGC ( display, windowid, 0, 0 );
        xdpth     = DefaultDepthOfScreen ( XtScreen ( canvas ) );

	XtVaGetValues ( canvas,
			XmNwidth,  &wdth,
			XmNheight, &hght,
			NULL);

/*
 *	Fill in window width and height.
 */
        xwdth = (int) wdth;
        xhght = (int) hght;


/*
 *      Create a red arrow for the cursor.
 */
        curs = XCreateFontCursor ( display, XC_top_left_arrow );
        XDefineCursor ( display, windowid, curs );
        cred.red   = 65535;
        cred.blue  =     0;
        cred.green =     0;
        cred.flags = DoRed | DoBlue | DoGreen;
        XRecolorCursor ( display, curs, &cred, &cred );

/*
 *      Set the fill rule.
 */
        XSetFillRule ( display, gemgc, WindingRule );

/*
 *	Start the device driver.
 */
        xmotifw ( windowid, winname, gemgc, xwdth, xhght, xdpth, iret );

	gg_motf ( winname, &iret, strlen (winname) );

	gemBackgroundColor = GetGemBackgroundPixel();

/*
 *	Set the drawing canvas background color to Gempak's
 *	background color.
 */
	XtVaSetValues( canvas, XmNbackground, gemBackgroundColor, NULL );

}


