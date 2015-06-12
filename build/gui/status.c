/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	status.c
 *
 *	Callbacks and utilities for displaying program status information.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/04	Unidata/Chiz	Updated to use gprint with stdarg
 *
 ***********************************************************************/


#include <stdio.h>
#include <stdarg.h>
#include "gui.h"


void UnManageCB();
void PopDownCB ( Widget, XtPointer, XtPointer );
void PopUpCB ( Widget, XtPointer, XtPointer );


static Widget		text_w;	/* Scrolled text window */
Widget	This_dialog;


void
ManageWidgetCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
	Widget	thisWidget;

	XmAnyCallbackStruct *cbs =
			(XmAnyCallbackStruct *) call_data;

	thisWidget = ( Widget ) client_data;

	XtManageChild ( This_dialog );
}


void
PopUpCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
	Widget	shell;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

/*
 *	Shell widget is passed through as client data.
 */
	shell = ( Widget ) client_data;

/*
 *	Manage the shell for this dialog.
 */
/*	XtPopup ( shell, XtGrabNone ); */
/*	XtPopup ( XtParent (This_dialog), XtGrabNone ); */
	XtManageChild ( This_dialog );
}



void CreateErrorWindowShell ( Widget parent, XtAppContext app )
{
	Widget		shell, dialog, b, rowcol;
	Arg		al[64];
	register int	ac;

/*
 *	Dialog.
 */
	shell = XtVaCreatePopupShell( "Garp Error Statements",
			xmDialogShellWidgetClass,
			parent,
			XmNdialogType, XmDIALOG_TEMPLATE,
			XmNautoUnmanage, True,
			NULL);
/*
 *	Dialog.
 */
	ac = 0;
	XtSetArg(al[ac], XmNautoUnmanage, True ); ac++;
	XtSetArg(al[ac], XmNdialogType, XmDIALOG_TEMPLATE); ac++;
	dialog = XmCreateMessageBox ( shell,
			"dialog", al, ac );
	This_dialog = dialog;
/*
 *	Close button.
 */
	b = XtVaCreateManagedWidget( "Close",
			xmPushButtonWidgetClass, dialog,
			NULL);
	XtAddCallback ( b, XmNactivateCallback, PopDownCB, shell );

/*
 *	Rowcolumn to contain scrolled text window.
 */
	rowcol = XtVaCreateWidget("rowcol",
			xmRowColumnWidgetClass, dialog,
			NULL);
/*
 *	Scrolled text window.
 */
	XtSetArg(al[0],	XmNrows,		6);
	XtSetArg(al[1],	XmNcolumns,		80);
	XtSetArg(al[2],	XmNeditable,		False);
	XtSetArg(al[3],	XmNeditMode,		XmMULTI_LINE_EDIT);
	XtSetArg(al[4],	XmNwordWrap,		True);
	XtSetArg(al[5],	XmNscrollHorizontal,	False);
	XtSetArg(al[6],	XmNcursorPositionVisible, False);
	text_w = XmCreateScrolledText(rowcol, "text_w", al, 7);
	XtManageChild(text_w);

/*
 *	Capture Xt and Xlib errors.
 */
/*
	XtAppSetErrorHandler(app, xt_error);
	XtAppSetWarningHandler(app, xt_error);
	(void) XSetErrorHandler ( x_error );
*/

/*
 *	Manage.
 */
	XtManageChild ( rowcol );
/*	XtManageChild ( dialog ); */
}


