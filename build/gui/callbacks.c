/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	callbacks.c
 *
 *	General callback routines.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory 
 *			sanity.
 *	 6/97	COMET	Change BACKGROUNDOBJECT to BACKGROUNDIMAGEOBJECT
 *	 8/97	COMET	Callbacks to choose time match algorithm and 
 *			model forecast filter type.
 *	 9/97	COMET	Moved UserDefinedGareaCB into here, added checks
 *			for the number of pixmap's in each area-setting
 *			function.
 *	 9/97	COMET	Added code to ClearMainWindowCB to make it redraw
 *			any background map after clearing the window
 *	11/97	COMET	Added call to SetMapListChangeState. Fixed bug 
 *			where user presses DEFAULT area after RESET.
 *	12/97	COMET	Fixed leak  in MWContourMapButtonCB, added call to
 *			SetMapListChangeState
 *	12/97	COMET	Added call to RefreshScrolledTimeLists() in the
 *			time matching callbacks
 *	 1/98	COMET	Added BrowserCB().
 *	 1/98	COMET	Add special check for existing netscape. Add check
 *			for "&" in command string.
 ***********************************************************************/


#include "guimacros.h"
#include "genglobs.h"
#include "winobj.h"
#include "mainwin.h"
#include "utils.h"
#include "ctbcmn.h"
#include "command.h"
#include "_proto.h"
#include "version.h"
#include "maplist.h"


void QuitCB ( Widget    w, XtPointer clientData, XtPointer callData )
{
	printf ("%s exiting...\n",GARP_version);
	exit ( 0 );
}


void
ExitCB ( Widget dialog, XtPointer clientData, XtPointer callData )
{
	exit ( 0 );
}


void
Exit ( Widget parent, char *message )
{
/*
 * This function pops up a message dialog warning the user of an impending
 * program exit.
 */
	Widget		dialog, b;
	Arg		al[64];
	XmString	xmstr;

	xmstr = XmStringCreateSimple ( message );

/*
 *	Create a modal message box to eliminate interactivity with other
 *	widgets.
 */
	dialog = XmCreateMessageDialog ( parent, "message", al, 1 );
	XtVaSetValues ( dialog,
			XmNmessageString, xmstr,
			XmNdialogStyle,   XmDIALOG_FULL_APPLICATION_MODAL,
			NULL );
	XmStringFree ( xmstr );

	XtAddCallback ( dialog, XmNokCallback, ExitCB, NULL );

/*
 *	Do not use help or cancel buttons.
 */
	XtUnmanageChild (
	    XmMessageBoxGetChild ( dialog, XmDIALOG_HELP_BUTTON ) );

	XtUnmanageChild (
	    XmMessageBoxGetChild ( dialog, XmDIALOG_CANCEL_BUTTON ) );

	XtManageChild ( dialog );
	XtPopup ( XtParent ( dialog ), XtGrabNone );

}


void
CancelCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
	Widget		parent;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;
/*
 *	Get top shell for this widget.
 */
	parent = ( Widget ) GetTopShell(w);

/*
 *	Destroy the top shell.
 */
	DestroyShell ( w, parent );

}


void
PopDownCB ( Widget w, XtPointer client_data, XtPointer call_data )
{
	Widget		shell;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;
/*
 *	Shell widget is passed through as client data.
 */
	shell = ( Widget ) client_data;

/*
 *	Unmanage the shell for this dialog.
 */
	XtPopdown ( shell );

}


void 
ClearMainWindowCB  (Widget w, XtPointer client_data, XtPointer call_data )
{
	int			iret;
	
	WindowObjectType	*wo;
	PixmapObjectType	*po;
	GeoRefObjectType	*geo;

	XmPushButtonCallbackStruct *cbs = 
	            (XmPushButtonCallbackStruct *) call_data;

	if( !( wo = GetActiveWindowObject() ) )
		return;

/*
 *	Reset Most of the Window Object resources to their default state and
 *	clear the display window.
 */
	WindowObjectPartialReset( wo );

/*
 *	Force the background map to get redrawn. ( If there is a georef object
 *	AND if we're in plan view only)
 */
	geo = GetGeoRefObject( wo );

	if( ! geo ) return;
	
	if (geo->viewType != PLAN ) return;

/*
 *	Create a pixmap
 */
	if ( wo->numPixmapObjects == 0 ) 
	    po = GetNextPot (wo);


/*
 *	Redraw
 */
	RefreshDisplay();

}


void
ResetMainWindowCB (Widget w, XtPointer client_data, XtPointer call_data )
{
	int	iret;
	
	WindowObjectType *wo;

	XmPushButtonCallbackStruct *cbs = 
	            (XmPushButtonCallbackStruct *) call_data;

	if( !( wo = GetActiveWindowObject() ) )
		return;

/*
 *	Reset all Window Object resources to their default state and
 *	clear the display window.
 */

	WindowObjectFullReset( wo );

}


void
AnimationCB (Widget w, XtPointer client_data, XtPointer call_data )
{
        XmPushButtonCallbackStruct *cbs = 
	                           (XmPushButtonCallbackStruct *) call_data;
printf("AnimationCB\n" );
}


void
SetFontSizeCB (Widget w, XtPointer client_data, XtPointer call_data )
{
	int			size;

	XmToggleButtonCallbackStruct *cbs = 
	              (XmToggleButtonCallbackStruct *) call_data;


/*
 *	Set new size.
 */
	size = (int) client_data;
	SetDepictableValue ( "FONTSIZE", size );

	return;
}


void
SetSymbolSizeCB (Widget w, XtPointer client_data, XtPointer call_data )
{
	int			size;

	XmToggleButtonCallbackStruct *cbs = 
	              (XmToggleButtonCallbackStruct *) call_data;


/*
 *	Set new size.
 */
	size = (int) client_data;
	SetDepictableValue ( "SYMBOLSIZE", size );

	return;
}


void
SetSymbolPackingCB (Widget w, XtPointer client_data, XtPointer call_data )
{
	WindowObjectType	*wo;
	int			frequency;

	XmToggleButtonCallbackStruct *cbs = 
	              (XmToggleButtonCallbackStruct *) call_data;

	wo = GetActiveWindowObject();

/*
 *	Set new frequency.
 */
	frequency = (int) client_data;
	SetDepictableValue ( "SYMBOLPACKING", frequency );
	SetDepictableValue ( "SCALARPACKING", frequency );

	return;
}


void
SetSymbolWidthCB (Widget w, XtPointer client_data, XtPointer call_data )
{
	WindowObjectType	*wo;
	int			width;

	XmToggleButtonCallbackStruct *cbs = 
	              (XmToggleButtonCallbackStruct *) call_data;

	wo = GetActiveWindowObject();

/*
 *	Set new width.
 */
	width = (int) client_data;
	SetDepictableValue ( "SYMBOLWIDTH", width );

	return;
}


void
GeneralPreferencesCB (Widget w, XtPointer client_data, XtPointer call_data )
{
        XmPushButtonCallbackStruct *cbs = 
			(XmPushButtonCallbackStruct *) call_data;

/*
 *	Clear display and redraw pixmaps.
 */
	RefreshDisplay();

	return;
}



void
MWContourMapButtonCB (Widget    w,
                      XtPointer clientData,
                      XtPointer call_data )
/*
 * Called when a colored contour map option button is picked.
 */
{

	int			count = 1;
	MetObjectType		**mlist;
	WindowObjectType	*wo;
	char			**file;
	char			**time;
	char			*mapdir, *fullpath;
	int			verbose;
	int			viewType;
	GeoRefObjectType	*geo;
	GuiMapBackgroundDialogType *mbg;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	wo = GetActiveWindowObject();
	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_1 )
	    printf ( "MWContourMapButtonCB\n" );


/*
 *	Check on the view types for compatibility. Compare this object 
 *	with the current georeference view state and do a reset if
 *	they don't match.
 */
	viewType  = GetMetObjectView( BACKGROUNDIMAGEOBJECT );
	if ( geo = GetGeoRefObject( wo) )
		if ( geo->viewType != viewType ) WindowObjectFullReset( wo );

	file = StringListAllocate( count );
	time = StringListAllocate( count );

	mapdir = GetConfigValue( "colormapdir" );
	fullpath = builddirpath ( mapdir, bo->string);
	file[0] = (char *) malloc(strlen(fullpath) + 1);
        strcpy(file[0], fullpath );

	time[0] = (char *) malloc(strlen("MISSING") + 1);
        strcpy(time[0], "MISSING" );

/*
 *	Make metObject
 */
	mlist = MakeImageObjectList( BACKGROUNDIMAGEOBJECT, count, file, time );

/*
 *	Put the objects into the display list.
 */
	CopyObjectListToDisplay( wo, count, mlist );

/*
 *	Adding images into the list means any previous met objects
 *	should be invalidated (redrawn). 
 */
	ClearAllPots (wo);

	SetWindowObjectDrawnFlag ( wo, False );

/*
 *	Set the flag in the map backgrounds struct so that maps
 *	will get recreated. 
 */
	mbg = GetGuiMapBackgroundDialog();
  	SetMapListChangeState ( mbg, True);


	if( wo ) {
		ProcessDisplayList( wo );
	}

	StringListFree( count, file );
	StringListFree( count, time );
	Free (fullpath);
	Free ( mapdir );
	Free ( mlist );

	return;
}


/*
 *	Called when a garea option button is picked.
 */

void
MWGareaButtonCB ( Widget    w,
                  XtPointer clientData,
                  XtPointer call_data )
{
	GuiMainWinDialogType	*mwn;
	WindowObjectType	*wo;
	PixmapObjectType	*po;

	char			*label, *garea, *proj;
	char			**labels, **gareas, **projs;
	char			*keylist;
	int			i, num, geoReset;
	int			verbose;
	GuiMapBackgroundDialogType *mbg;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_1 )
	    printf ( "MWGareaButtonCB\n" );

	mwn = GetGuiMainWinDialog();
	wo = GetActiveWindowObject();

/*
 *	Get label describing geographic area.
 */
	label = strdup ( bo->label );

/*
 *	Get label, garea and projection key lists.
 */
	keylist = GetConfigValue( "MapLabels" );
	labels  = SplitByDelimeter ( keylist, ",", &num );
	Free ( keylist );

	keylist = GetConfigValue( "MapGarea" );
	gareas  = SplitByDelimeter ( keylist, ",", &num );
	Free ( keylist );

	keylist = GetConfigValue( "MapProjection" );
	projs   = SplitByDelimeter ( keylist, ",", &num );
	Free ( keylist );

		
/*
 *	Check the special case for the "default" area.
 */

	garea = NULL;
	proj = NULL;

	if ( strcmp ( label, "DEFAULT" ) == 0 ) {

/*
 *	    If no georef object exists as of yet, supply one. 
 */
	    if ( wo->geoRefObject == NULL ) {

		proj = GetConfigValue ( "default_proj" );
		if ( proj == NULL ) proj = strdup ( "lcc/30.0;-100;50.0//nm" );

		garea = GetConfigValue ( "default_garea" );
		if ( garea == NULL ) garea = strdup ( "23.0;-120.0;47.0;-65.0" );

		geoReset = 0;

	    }

/*
 *	    Otherwise, just release the current one, as it will get
 *	    re-instantiated later
 */
	    else {
		ReleaseGeoRefObject( wo );
	        geoReset = 1;
	    }
	}

/*
 *	If the area is not DEFAULT, find the entry in the list and set
 *	the garea and proj
 */
	else {
	    for ( i=0; i<num; i++ ) {
		if ( strcmp ( label, labels[i] ) == 0 ) {
	    	    garea = strdup ( gareas[i] );
	    	    proj  = strdup ( projs[i] );
		    if ( verbose ) {
			printf ( "MWGareaButtonCB - garea = %s\n", garea );
			printf ( "MWGareaButtonCB - projection = %s\n", proj );
		    }
		    geoReset = 0;
		    break;
		}
	    }
	}

/*
 *	Reset the mapping information or allow the system to
 *	back to it's default.
 */
	if( ! geoReset ) {
	    SetGeoRefObjectViewType( wo, PLAN );
	    SetGeoRefObjectGarea( wo, garea );
	    SetGeoRefObjectFilename( wo, NULL );
	    SetGeoRefObjectProjection( wo, proj );
	}

/*
 *	Make sure we have a pixmap to draw into
 */
	if ( wo->numPixmapObjects == 0 )
		po = GetNextPot (wo);

/*
 *	Set the flag in the map backgrounds struct so that maps
 *	will get recreated. 
 */
	mbg = GetGuiMapBackgroundDialog();
  	SetMapListChangeState ( mbg, True);

/*
 *	Clear display and redraw pixmaps.
 */
	RefreshDisplay();

/*
 *	Free.
 */
	Free ( label );
	if ( garea )
		Free ( garea );
	if ( proj )
		Free ( proj );
	StringListFree ( num, labels );
	StringListFree ( num, gareas );
	StringListFree ( num, projs );

	return;
}


/*
 *	Called when garea "Define" option button is picked.
 */

void
MWUserSetGareaCB ( Widget    w,
                   XtPointer clientData,
                   XtPointer call_data )
{
	extern Widget		garea_dialog;

	GuiMainWinDialogType	*mwn;
	WindowObjectType	*wo;

	int			verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_1 )
	    printf ( "MWUserSetGareaCB\n" );

	mwn = GetGuiMainWinDialog();
	wo = GetActiveWindowObject();

/*
 *	Manage dialog so user can set garea and projection.
 */
	XtManageChild ( garea_dialog );

}


/*
 *	Called when garea "Previous" option is selected.
 */

void
MWSetPreviousGareaCB ( Widget    w,
                   XtPointer clientData,
                   XtPointer call_data )
{

	WindowObjectType	*wo;
	GeoRefObjectType	*geo;
	PixmapObjectType	*po;
	int			verbose;
	int			itemp;
	char			garea[GEMPAKSTRING];
	char			proj[GEMPAKSTRING];
	GuiMapBackgroundDialogType *mbg;


	wo = GetActiveWindowObject();
	geo = GetGeoRefObject( wo );

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_1 )
		    printf ( "MWSetPreviousGareaCB\n" );

	if( ! geo ) return;

	strcpy ( garea, geo->prevGarea);
	strcpy ( proj, geo->prevProj);

	if ( strlen ( garea ) > 0 && strlen ( proj ) > 0 ) {

	    SetGeoRefObjectGarea( wo, garea );
	    SetGeoRefObjectProjection( wo, proj );

	    if ( wo->numPixmapObjects == 0 )
	    	po = GetNextPot(wo);

/*
 *	    Set the flag in the map backgrounds struct so that maps
 *	    will get recreated. 
 */
	    mbg = GetGuiMapBackgroundDialog();
  	    SetMapListChangeState ( mbg, True);

	    RefreshDisplay ();
	}
	
}

/*
 *	Called when the user presses apply on the user defined garea
 *	dialog.
 */

void
UserDefinedGareaCB  (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	GuiMainWinDialogType	*mwn;
	WindowObjectType	*wo;
	PixmapObjectType	*po;
	char			*text;
	char			garea[73];
	char			proj[24];
	BooleanType		flagg = False, flagp = False;
	int			verbose;
	GuiMapBackgroundDialogType *mbg;

        XmPushButtonCallbackStruct *cbs =
	                    (XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_1 )
	    printf ( "UserDefinedGareaCB\n" );

/*
 *	Get main dialog widget structure and active window.
 */
	mwn = GetGuiMainWinDialog();
	wo = GetActiveWindowObject();

/*
 *	Get garea text string.
 */
	if ( text = XmTextGetString ( GetGareaTextW ( mwn ) ) ) {
	    strcpy ( garea, text );
	    StringClean ( garea );
	    if (strlen (garea ) != 0 ) {
		flagg = True;
		SetGeoRefObjectGarea( wo, garea );
	    }
	    XtFree ( text );

	if ( text = XmTextGetString ( GetProjectionTextW ( mwn ) ) )
	    strcpy ( proj, text );
	    StringClean ( proj );
	    if (strlen ( proj ) != 0 ) {
		flagp = True;
		SetGeoRefObjectProjection( wo, proj);
	    }
	    XtFree ( text );
	}

	if ( verbose ) {
		printf ( "UserDefinedGareaCB - garea = %s\n", garea );
		printf ( "UserDefinedGareaCB - projection = %s\n", proj );
	}


/*
 *	Refresh only if garea or projection was updated.
 */
	if ( flagg && flagp ) {

	    SetGeoRefObjectViewType( wo, PLAN );
	    SetGeoRefObjectFilename( wo, NULL );

	    if ( wo->numPixmapObjects == 0 )
	    	po = GetNextPot(wo);

/*
 *	    Set the flag in the map backgrounds struct so that maps
 *	    will get recreated. 
 */
	    mbg = GetGuiMapBackgroundDialog();
  	    SetMapListChangeState ( mbg, True);

	    RefreshDisplay ();
	}
}


/*
 *	Called when Time Match radio button is clicked.
 */

void
SetTimeMatchTypeCB ( Widget w,
	             XtPointer clientData,
	             XtPointer xt_call_data )
{
	DataTimeObjectType	*dto;
	int			prevMatchType;

	XmToggleButtonCallbackStruct *call_data = 
	                  (XmToggleButtonCallbackStruct *) xt_call_data ;

	dto = GetDataTimeObject();

	prevMatchType = dto->matchType;
	dto->matchType = (int) clientData;

/*
 *	Update time lists if the time matching mode changed
 */
	if ( prevMatchType !=  dto->matchType )
		RefreshScrolledTimeLists ( False );


}


/*
 *	Called when Time Match Forecast Type button is clicked.
 */

void
SetModelForecastTypeCB ( Widget w,
	                 XtPointer clientData,
	                 XtPointer xt_call_data)
{
	DataTimeObjectType	*dto;
	int			prevForecastType;

	XmToggleButtonCallbackStruct *call_data = 
	                   (XmToggleButtonCallbackStruct *) xt_call_data ;

	dto = GetDataTimeObject();

	prevForecastType = dto->forecastType;
	dto->forecastType = (int) clientData;

/*
 *	Update time lists if the time matching mode changes
 */
	if ( prevForecastType != dto->forecastType ) 
		RefreshScrolledTimeLists ( False );


}


void
BrowserCB ( Widget w, XtPointer clientData, XtPointer xt_call_data )
{
	extern Widget	main_shell;
	char		*url, *command;
	char		*browser;
	char		*key1="UserGuide";
	char		*key2="CaseStudies";
	char		*key3="Browser";
	char		*nswin="netscape -remote 'openURL(%s, new-window)' > /dev/null 2>&1";
	int		button;
	int		iret, verbose;


	XmPushButtonCallbackStruct *call_data =
	                    (XmPushButtonCallbackStruct *) xt_call_data ;

	button = (int) clientData;

/*
 *	Get URL from Garp_defaults.
 */
	switch (button) {
	case 1:
	  url = GetConfigValue( key1 );
	  if ( url == NULL ) {
	    printf("  Warning: key '%s' is not defined in Garp_defaults\n",
	            key1);
	    return;
	  }
	  break;

	case 2:
	  url = GetConfigValue( key2 );
	  if ( url == NULL ) {
	    printf("  Warning: key '%s' is not defined in Garp_defaults\n",
	            key2);
	    return;
	  }
	  break;

	default:
	  return;
	  break;
	}

/*
 *	Get browser name from Garp_defaults.
 */
	browser = GetConfigValue( key3 );
	if ( browser == NULL ) {
	    printf("  Warning: key '%s' is not defined in Garp_defaults\n",
	            key3);
	    return;
	}

/*
 *	If the browser command is "netscape ", have it open the URL in a new
 *	window.
 */
	if ( strstr ( browser, "netscape " ) != 0 ) {

	    command = (char *) XtMalloc( strlen( nswin )
	                     + strlen( url ) + 1 );

	    sprintf( command, nswin, url );

/*
 *	    This should return fairly quickly, succeed or fail
 */
	    if ( system( command ) ) {

/*
 *		If we get here, then the command failed and we should
 *		allow it to continue on and start a new browser.
 */
		XtFree( (XtPointer) command );

	    }
	    else {
/*
 *		If we get here, the command succeeded, so free and return
 */
		Free ( url );
		Free ( browser );
		XtFree( (XtPointer) command );
		return;
	    }

	}

/*
 *	Build command line to start up browser. Need to make sure there is an
 *	"&" on the end of the command, because we have to launch it in the
 *	background, or it will block in the system function.
 */
	if ( strstr ( browser, "&" ) == NULL ) {
	    command = (char *) XtMalloc( strlen( browser )
	                     + strlen( url ) + 2 );
	    sprintf( command, browser, url );
	    strcat ( command, "&" );	
	}
	else {
	    command = (char *) XtMalloc( strlen( browser )
	                     + strlen( url ) + 1 );
	    sprintf( command, browser, url );
	}

/*
 *	Issue the command
 */
	if ( iret = system( command ) == -1 ) {
	    printf( "Could not start '%s' ", browser );
	    printf( "to send it to %s.\n", url ) ;
	}
	else
	    InfoPop ( main_shell, WORKING,
            "\nThe browser will be appearing shortly.\n",
            FALSE, FALSE, NULL, NULL );

/*
 *	Free.
 */
	Free ( url );
	Free ( browser );
	XtFree( (XtPointer) command ) ;
}
