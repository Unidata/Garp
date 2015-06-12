/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	dropdown.c
 *
 *	Callbacks and utilities to control dropdown labels.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	11/97	COMET	Added a timeout for the dropdown label so that
 *			it dissapears automatically after some time period.
 *	 1/98	COMET	Add some casts to remove compiler warnings
 *
 ***********************************************************************/


#include <X11/X.h>
#include <Xm/Xm.h>
#include <Xm/Label.h>

#include "winobj.h"
#include "mainwin.h"

/*
 *	Pulled out of gui.c.  Not the best way to do this...working
 *	on alternate method to get this widget id.
 */
static Widget	dropDownLabel = NULL;
static Widget	dropDownFrame = NULL;

static XtIntervalId xtIntervalID = (XtIntervalId) NULL;
int		timeout = 500;

static XtIntervalId dieIntervalID = (XtIntervalId) NULL;
int		dieTimeout = 5000;

void DropDownForceOff();
XtEventHandler DropDownEnterCB( Widget w, 
		XtPointer clientData, 
		XEvent *event, 
		Boolean *again );
XtEventHandler DropDownCancelCB( Widget w, 
		XtPointer clientData, 
		XEvent *event, 
		Boolean *again );

void DropDownRegisterButton( Widget w );
void DropDownRegisterAllButtons();

void DropDownRegisterAllButtons()
{

	GuiMainWinDialogType	*mwn;

	mwn = GetGuiMainWinDialog();

	DropDownRegisterButton( GetGuiSatelliteIconW ( mwn ) );
	DropDownRegisterButton( GetGuiSurfaceIconW   ( mwn ) );
	DropDownRegisterButton( GetGuiProfilerIconW  ( mwn ) );
	DropDownRegisterButton( GetGuiUpperairIconW  ( mwn ) );
/*	DropDownRegisterButton( GetGuiLightningIconW ( mwn ) ); */
/*    	DropDownRegisterButton( GetGuiAfosIconW      ( mwn ) ); */
    	DropDownRegisterButton( GetGuiGridhIconW     ( mwn ) );
	DropDownRegisterButton( GetGuiGridcIconW     ( mwn ) );
	DropDownRegisterButton( GetGuiGridzIconW     ( mwn ) );
	DropDownRegisterButton( GetGuiGridtIconW     ( mwn ) );
/*	DropDownRegisterButton( GetGuiTextIconW      ( mwn ) ); */
/*	DropDownRegisterButton( GetGui3dIconW        ( mwn ) ); */
	DropDownRegisterButton( GetGuiClearIconW     ( mwn ) );
	DropDownRegisterButton( GetGuiResetIconW     ( mwn ) );

}


void ManageTheLabel ( )
{

	XtManageChild( dropDownLabel);
/*	XtManageChild( dropDownFrame); */
	xtIntervalID = (XtIntervalId) NULL;

/*
 *	Set a timer so that the label automatically dissapears after
 *	dieTimeout milliseconds
 */
	dieIntervalID = XtAppAddTimeOut(
				GetWindowObjectAppContext(
					GetActiveWindowObject() ), 
				dieTimeout, 
				(XtTimerCallbackProc) DropDownForceOff, 
				(XtPointer) NULL );


}


void DropDownRegisterButton( Widget w )
{

    XtAddEventHandler ( w, EnterWindowMask, FALSE,
                (XtEventHandler)DropDownEnterCB, (XtPointer) NULL );

    XtAddEventHandler ( w, LeaveWindowMask | ButtonPressMask, FALSE,
                (XtEventHandler)DropDownCancelCB, (XtPointer) NULL );

}

XtEventHandler DropDownEnterCB( Widget w, 
		XtPointer clientData, 
		XEvent *event, 
		Boolean *again )
{

	XmString	labelXmstr    = NULL;
	Dimension 	width, height;
	Dimension 	xPositionButton, yPositionButton;
	Dimension  	xPositionLabel, yPositionLabel;

	GuiMainWinDialogType	*mwn;
	extern Widget main_form;

	mwn = GetGuiMainWinDialog();

	if( event->type != EnterNotify )
		return;

	if( dropDownLabel == NULL ) {
/*
		dropDownFrame = XtVaCreateWidget( "dropdownframe", 
				xmFrameWidgetClass,
				GetGuiMainAppShellW ( mwn ),
				XmNtopAttachment, XmATTACH_NONE,
				XmNbottomAttachment, XmATTACH_NONE,
				XmNleftAttachment, XmATTACH_NONE,
				XmNrightAttachment, XmATTACH_NONE,
				XmNshadowType, XmSHADOW_ETCHED_IN,
				NULL );
*/
		dropDownLabel = XtVaCreateWidget( "dropdownlabel", 
				xmLabelWidgetClass, 
				GetGuiMainAppShellW ( mwn ),
/*				dropDownFrame, */
				NULL );
	}


	DropDownForceOff();

	
	XtVaGetValues( w, XmNx, &xPositionButton,
			XmNy, &yPositionButton,
			XmNwidth, &width,
			XmNheight, &height,
			XmNlabelString, &labelXmstr,
			NULL );

	XtVaSetValues( dropDownLabel,
			XmNx, xPositionButton + (width/2),
			XmNy, yPositionButton + height + 40,
			XmNlabelString, labelXmstr,
			NULL );

/*
 *	Set a timer so that there is a delay before the label is
 *	actually visible
 */
	xtIntervalID = XtAppAddTimeOut(
				GetWindowObjectAppContext(
				GetActiveWindowObject() ), 
				timeout, 
				(XtTimerCallbackProc) ManageTheLabel, 
				(XtPointer) NULL );
	
	return;
}

/*
 *	It looks like (at least on HP's), that the button
 *	widgets can get a EnterNotify event without a cooresponding
 *	LeaveNotify event.  This occurrs when a pull down menu is
 *	obscuring the button, but the menu choice chosen from the
 *	drop down leaves the cursor in the middle of button underneath
 *	it.  Further if you immediately move the cursor outside of the
 *	button, then the leavenotify event is not received for the button.
 *
 */
void DropDownForceOff()
{

/*
 *	Remove any pending timer for label managing
 */


	if ( xtIntervalID != (XtIntervalId) NULL ) {
	    XtRemoveTimeOut (xtIntervalID);
	    xtIntervalID = (XtIntervalId) NULL;
	}

	if ( dieIntervalID !=  (XtIntervalId) NULL) {
	    XtRemoveTimeOut (dieIntervalID);
	    dieIntervalID = (XtIntervalId) NULL;
	}

	if( dropDownLabel && XtIsManaged( dropDownLabel) ) {
		XtUnmanageChild( dropDownLabel );
	}
}


XtEventHandler DropDownCancelCB( Widget w, 
		XtPointer clientData, 
		XEvent *event, 
		Boolean *again )

{

	if( event->type != LeaveNotify  && event->type != ButtonPress ) {
		printf("DropDownCancelCB: Got unexpected event, %d\n",
			event->type );
		return;
	}

	DropDownForceOff();

	return;

}
