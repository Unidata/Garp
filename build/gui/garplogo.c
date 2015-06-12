/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	garplogo.c
 *
 *	Sets the garp logo widget
 *
 *	History:
 *
 *	 8/97	COMET	Original copy
 *	 1/98	COMET	Cast some NULL values 
 *
 ***********************************************************************/

#include "winobj.h"
#include "mainwin.h"
#include "version.h"


/*
 *  "Default" colors for the logo widget
 */
static Pixel	armColor = (Pixel)NULL,  bgColor = (Pixel)NULL;


static void
SetGarpLogoW ( WindowObjectType *wo, BooleanType reset, char *string )
{

	Widget			logoWidget;
	extern Widget		icon_frame;
	XmString		label;
	GuiMainWinDialogType 	*mw;
	Dimension		height, width;
	Pixel			color;
	Display			*display;


/*
 *	Make sure we have something to work with. If the encompassing
 *	frame has been removed (height = 1) then bail out.
 */
	display = GetWindowObjectDisplay( wo );
	mw = GetGuiMainWinDialog();
	logoWidget = GetGuiGarpLogoW ( mw );
	if ( logoWidget == NULL ) return;

	XtVaGetValues ( icon_frame,
	    		XmNwidth,  &width,
	    		XmNheight, &height,
			NULL);
	if ( height == 1 ) return;
	
	if ( armColor == (Pixel)NULL || bgColor == (Pixel)NULL )
	
		XtVaGetValues ( logoWidget,
				XmNarmColor, &armColor, 
				XmNbackground, &bgColor,
				NULL);
	
	label = XmStringLtoRCreate ( string, XmSTRING_DEFAULT_CHARSET );

	if ( reset )
		color = bgColor;
	else
		color = armColor;

	XtVaSetValues (	logoWidget,
			XmNbackground, color,
			XmNlabelString, label, 
			NULL);

        RefreshWidgetNow ( logoWidget );
	XmStringFree ( label );
	
	XFlush( display );

}


/*
 *  SetGarpLogo(): User function to set the garp logo widget to some
 *  string.
 */

void SetGarpLogo ( WindowObjectType *wo, char *string )
{

	SetGarpLogoW ( wo, False, string );

}


/*
 *  ResetGarpLogo(): User function to set the garp logo widget back to the
 *  version string.
 */

void ResetGarpLogo ( WindowObjectType *wo )
{

	SetGarpLogoW ( wo, True, GARP_version );

}

