/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	file_browser.c
 *
 *	Callbacks and utilities to display grid information.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include <X11/Xos.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/FileSB.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/LabelG.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>


typedef struct {
	char	*pathName;
	Widget  parentWidget;
	Widget  fileSelectionWidget;
	Widget  textFieldWidget;
	Widget  scrolledTextWidget;
	Widget	mainWidget;
} UserDataType;
	
static UserDataType userData;

static void CancelCB(Widget w, XtPointer client, XtPointer cbs);

Widget CreateFileBrowser( Widget parent, char *pathName )
{


	Widget        main_w, menubar, menu, rc, text_w, file_w;
	XtAppContext  app;
	XmString      file, new, quit;
	extern void   read_file(), file_cb();
	Arg           args[4];


	if ( pathName == NULL ) return(NULL);

/*
 *	Keep track of certain variables associated with
 *	these dialogs.
 */
	userData.parentWidget = parent;
	userData.pathName     = strdup( pathName );

/* 
 *	MainWindow for the application -- contains menubar
 *	and ScrolledText/Prompt/TextField as WorkWindow.
 */
	userData.mainWidget = main_w = 
		XtVaCreateManagedWidget("GDInfoDialog", 
			xmMainWindowWidgetClass, parent,
			/* XmNscrollingPolicy,   XmVARIABLE, */
			NULL);

/*
 *	Create a simple MenuBar that contains one menu
 */
	file = XmStringCreateSimple("File");
	menubar = XmVaCreateSimpleMenuBar(main_w, "menubar",
			XmVaCASCADEBUTTON, file, 'F', NULL);
	XmStringFree(file);

/* 
 *	Menu is "File" -- callback is file_cb() 
 */
	new = XmStringCreateSimple("Open Grid File...");
	quit = XmStringCreateSimple("Close");

	menu = XmVaCreateSimplePulldownMenu(menubar, "file_menu", 
			0, file_cb, 
			XmVaPUSHBUTTON, new, 'N', NULL, NULL, 
			XmVaSEPARATOR, XmVaPUSHBUTTON, quit, 'Q', 
			NULL, NULL, NULL);
	XmStringFree(new);
	XmStringFree(quit);

/*
 *
 *	Menubar is done -- manage it 
 *
 */
	XtManageChild(menubar);

	rc = XtVaCreateWidget("WorkAreaRC", xmRowColumnWidgetClass, 
		main_w, NULL);

	XtVaCreateManagedWidget("Filename:", xmLabelGadgetClass, rc,
		XmNalignment, XmALIGNMENT_BEGINNING, NULL);

	userData.textFieldWidget = file_w = 
		XtVaCreateManagedWidget("TextField",
		xmTextFieldWidgetClass, rc, NULL);

/* 
 *	Create ScrolledText -- this is work area for the MainWindow
 */

/*	XtSetArg(args[0], XmNrows,      40);
	XtSetArg(args[1], XmNcolumns,   80);
*/

	XtSetArg(args[0], XmNeditable,  False);
	XtSetArg(args[1], XmNeditMode,  XmMULTI_LINE_EDIT);
	userData.scrolledTextWidget = text_w = 
		XmCreateScrolledText(rc, "ScrolledText", args, 2);

	XtManageChild(text_w);

/*
 *	store text_w as user data in "File" menu for file_cb() callback
 */
	XtAddCallback(file_w, XmNactivateCallback, read_file, NULL);

	XtManageChild(rc);

/* 
 *	Store the filename text widget to ScrolledText object
 */

	XmMainWindowSetAreas(main_w, menubar, NULL, NULL, NULL, rc);

	return(main_w);

}

/* 
 *	The "File" menu item was selected; popup a FileSelectionDialog. 
 */
void file_cb(Widget menu_item, int item_no)
{
	static Widget dialog;
	Widget text_w;
	XmString xmstr;
	extern void read_file();

/*
 *	User chose "close".
 */
	if (item_no == 1) {
		XtUnmanageChild( userData.parentWidget );
		return;  
	}

	if ( ! dialog ) {

		Widget menu = XtParent(menu_item);
		userData.fileSelectionWidget = dialog = 
			XmCreateFileSelectionDialog(menu, "file_sb", NULL, 0);

		XtAddCallback(dialog, XmNokCallback, read_file, NULL );

		XtAddCallback(dialog, XmNcancelCallback, CancelCB, NULL);

		xmstr = XmStringCreateSimple( userData.pathName );

		XtVaSetValues( dialog, XmNdirectory, xmstr, NULL );

		XmStringFree( xmstr );


	}

	XtManageChild( dialog );


/*
 *	If dialog is already popped up, XtPopup does nothing.
 *	Call XMapRaised() anyway to make sure it's visible.
 *
 */

	XtPopup(XtParent(dialog), XtGrabNone);
	XMapRaised(XtDisplay(dialog), XtWindow(XtParent(dialog)));
}

static void CancelCB(Widget w, XtPointer client, XtPointer cbs)
{

	XtUnmanageChild( userData.fileSelectionWidget );
}


/* 
 * Callback routine when the user selects Ok in the FileSelection
 * Dialog or presses Return in the single-line text widget.
 * The specified file must be a GEMPAK grid file and readable.
 *
 * If everything goes OK, "GDINFO" formatted text is displayed 
 * in the scrolled text window.
 *
 */
void
read_file(Widget w, XtPointer client, XmFileSelectionBoxCallbackStruct *cbs)
{
	char *filename, *text;
	struct stat statb;
	FILE *fp;
	Widget file_w;

	if (XtIsSubclass(w, xmTextFieldWidgetClass))
/*
 *		File was selected by typing in the text input field...
 */
		filename = XmTextFieldGetString( w );

	else
/*
 *		File was selected from FileSelectionDialog
 */
		XmStringGetLtoR(cbs->value, 
			XmSTRING_DEFAULT_CHARSET, &filename);

/*
 *	Check to see if anything was typed and the filename is
 *	valid.
 */
	if (! filename || ! *filename) {
		if (filename)
			XtFree(filename);
		return;
	}

	XtUnmanageChild( userData.fileSelectionWidget );

	text = (char *) GetGridInfoText( filename );
	if( ! text ) {
		XtFree(filename);
		return;
	}

/*
 *	Insert file contents in Text widget
 */
	XmTextSetString(userData.scrolledTextWidget, text);

/* 
 *	make sure text field is up to date -  only necessary 
 *	if activated from FileSelectionDialog 
 */
	if (userData.textFieldWidget != w) {

		XmTextFieldSetString(userData.textFieldWidget, filename);
		XmTextFieldSetCursorPosition(userData.textFieldWidget, 
				strlen(filename));
	}

/*
 *	Free all allocated space and we're outta here.
 */

	free( text );

	XtFree(filename);

}
