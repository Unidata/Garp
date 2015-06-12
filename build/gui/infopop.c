/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	infopop.c
 *
 *	Convenience function call for information dialogs.
 *
 *	History:
 *
 *	 2/97	COMET	Original copy
 *
 ***********************************************************************/


#include <strings.h>
#include <stdlib.h>
#include <assert.h>

#include "gendef.h"
#include "genglobs.h"
#include "guimacros.h"


void
InfoPop ( Widget parent,	int type,
	  char *message,	Boolean help,
	  Boolean cancel,	void (*HelpCB)(),
	  void (*CancelCB)() )
{
	Widget		dialog;
	XmString	xmstr;
	char		buf[4096];

	if ( message == NULL ) return;
/*
 *  Create an information dialog of the type requested in function call.
 */
	switch ( type ) {
	case ERROR:
	  dialog = XmCreateErrorDialog ( parent, "ERROR", NULL, 0 );
	  strcpy ( buf, "ERROR:\n" );
	break;
	  
	case INFORMATION:
	  dialog = XmCreateInformationDialog ( parent, "INFORMATION", NULL, 0 );
	  strcpy ( buf, "INFORMATION:\n" );
	break;

	case QUESTION:
	  dialog = XmCreateQuestionDialog ( parent, "QUESTION", NULL, 0 );
	  strcpy ( buf, "QUESTION:\n" );
	break;

	case WARNING:
	  dialog = XmCreateWarningDialog ( parent, "WARNING", NULL, 0 );
	  strcpy ( buf, "WARNING:\n" );
	break;

	case WORKING:
	  dialog = XmCreateWorkingDialog ( parent, "WORKING", NULL, 0 );
	  strcpy ( buf, "WORKING:\n" );
	break;

	default:
	break;
	}

/*
 *	Write message to dialog.
 */
	strcat ( buf, message );
	xmstr = XmStringCreateLtoR ( buf, XmFONTLIST_DEFAULT_TAG);
	XtVaSetValues ( dialog,
		XmNmessageString,	xmstr,
		NULL );

	XmStringFree ( xmstr );

/*
 *	Add callbacks to buttons or, alternatively, delete the buttons.
 */
	if ( !help ) XtUnmanageChild ( 
	    XmMessageBoxGetChild ( dialog, XmDIALOG_HELP_BUTTON ) );
	else
	    XtAddCallback ( dialog, XmNhelpCallback, HelpCB, NULL );

	if ( !cancel ) XtUnmanageChild (
	    XmMessageBoxGetChild ( dialog, XmDIALOG_CANCEL_BUTTON ) );
	else
	    XtAddCallback ( dialog, XmNcancelCallback, CancelCB, NULL );

/*
 *	Manage popup.
 */
	XtManageChild ( dialog );
	XtPopup ( XtParent ( dialog ), XtGrabNone );
}
