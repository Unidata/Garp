/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	statusbar.c
 *
 *	This module contains routines used for displaying info on the
 *	status bar.
 *
 *	History:
 *
 *	 5/97	COMET	Original copy
 * 	 7/99	COMET	Removed time_format from GetClockString argument 
 *			list.
 *
 ***********************************************************************/


#include "winobj.h"
#include "_proto.h"
#include "genglobs.h"
#include "guimacros.h"

#define NUMVALIDT	3
#define NUMVALIDD	3
#define NUMVALIDSB	5

char *validtunits[] = { "K", "C", "F" };
char *validdunits[] = { "KM", "MI", "NM" };
char *validsbitems[] = { "CLOCK", "WINDOWCOORDS" , "PLOTCOORDS",
			 "PIXELVALUE", "DISTCALC" };

static char	tunits[] = "C";		/* default values */
static char	dunits[] = "KM";
static int 	statusBarItemList [NUMVALIDSB];
static int	statusBarItemCount;



int GetCurrentX ( WindowObjectType *wo );
int GetCurrentY ( WindowObjectType *wo );
int GetCurrentAnchorX ( WindowObjectType *wo );
int GetCurrentAnchorY ( WindowObjectType *wo );
Boolean GetAnchorFlag ( WindowObjectType *wo );



void PrintToStatusBar( char *string )
{

	XmString	label;
	extern Widget	statusbar_label;	
	Widget 		statusBarW;
	
	statusBarW = statusbar_label;

	
	label = XmStringLtoRCreate ( string,
	                                   XmSTRING_DEFAULT_CHARSET );

	XtVaSetValues ( statusBarW, XmNlabelString, label, NULL );

/*
 *	Force a redraw of the label widget to reduce annoying flicker.
 */
	RefreshWidgetNow ( statusBarW );	

	XmStringFree ( label );

}


void UpdateStatusBar( WindowObjectType *wo )

{
	char		string [256];
	char		tmpstr [96];
	int		x, y, ax, ay, i;
	

	x = GetCurrentX (wo);
	y = GetCurrentY (wo);
	ax = GetCurrentAnchorX (wo);
	ay = GetCurrentAnchorY (wo);
	

	strcpy (string, "");
	
	for ( i = 0; i < statusBarItemCount; i++ ) {

	    switch ( statusBarItemList [i] ) {

	    case CLOCK:
		GetClockString ( tmpstr );
		strcat  ( string, tmpstr );
		strcat  ( string, "  " );
		break;

	    case WINDOWCOORDS:
		sprintf ( tmpstr, "XY:%4d,%4d", x, y);
		strcat  ( string, tmpstr );
		strcat  ( string, "  " );
		break;

	    case PLOTCOORDS:
		GetCursorPosition ( wo, x, y, tmpstr );
		strcat  ( string, tmpstr );
		strcat  ( string, "  " );
		break;

	    case PIXELVALUE:
		GetDataAtCursor ( wo, tunits, x, y, tmpstr);
		strcat  ( string, tmpstr );
		strcat  ( string, "  " );
		break;

	    case DISTCALC:
		GetDistanceInfo ( wo, dunits, ax, ay, x, y, tmpstr );
		strcat  ( string, tmpstr );
		strcat  ( string, "  " );
		break;

	    }

	}
	
	PrintToStatusBar ( string );

}

/*
 *	Gets the default status bar item list
 */

void InitStatusBarDefaults ()
{
	char		*keylist;
	char		**itemlist;
	char		*tempunits;
	char		*distunits;
	int		numitems, count, verbose;
	int		n, i, j;
	BooleanType	found;

	verbose = GetVerboseLevel();

/*
 *	Extract the status bar item list and verify entries
 */
	keylist  = GetConfigValue ( "statusbaritems" );
	itemlist = SplitByDelimeter ( keylist, ",", &numitems );
	
	count = numitems;
	if ( count > NUMVALIDSB ) {
	    printf("Too many status bar items. Max allowed is %d.\n",NUMVALIDSB);
	    count = NUMVALIDSB ;
	}

	statusBarItemCount = 0;

	for ( i = 0; i < count; i++ )  {
	
	    ToUpper ( itemlist [i] );

	    for ( j = 0; j < NUMVALIDSB; j++ ) {
	    
		if ( strcmp ( itemlist [i], validsbitems[j] ) == 0 ) {

		    statusBarItemList [statusBarItemCount] = j+1;
		    statusBarItemCount++;

		    if ( verbose > VERBOSE_0 ) {
			printf("Status bar item %d: ", statusBarItemCount );
			printf("%s\n", itemlist [i]);
		    }
		    break;
		    
		}
	    }
	    if ( j == NUMVALIDSB ) 
	    	printf("Invalid status bar item name: %s\n",itemlist[i]);

	}

/*
 *	Get the temperature and distance units
 */
	tempunits = GetConfigValue ( "tempunits" );
	distunits = GetConfigValue ( "distunits" );

/*
 *	Verify temperature units
 */
	if ( tempunits ) {
	    ToUpper ( tempunits );
	    found = False;
	    for ( i = 0; i < NUMVALIDT; i++ ) {
		if ( strcmp ( validtunits[i], tempunits ) == 0 ) {
		    strcpy ( tunits, tempunits );
		    found = True;
		    break;
		}
	    }
	}	
	if ( ! found )
	    printf("Invalid or unspecified brightness temp units: %s\n",tempunits);	
	
/*
 *	Verify distance units
 */
	if ( distunits ) {
	    ToUpper ( distunits );
	    found = False;
	    for ( i = 0; i < NUMVALIDD; i++ ) {
		if ( strcmp ( validdunits[i], distunits ) == 0 ) {
		    strcpy ( dunits, distunits );
		    found = True;
		    break;
		}
	    }
	}
	if ( ! found )
	    printf("Invalid or unspecified distance units: %s\n",distunits);

/*
 *	Print verbose message
 */	
	if ( verbose > VERBOSE_0 ) {
	    printf("Status bar temperature units: %s\n",tunits);
	    printf("Status bar distance units: %s\n",dunits);
	}

/*
 *	Offer it up
 */
	if ( keylist ) Free ( keylist );
	if ( numitems ) StringListFree ( numitems, itemlist );
	if ( tempunits ) Free ( tempunits );
	if ( distunits ) Free ( distunits );

}


/*
 *	Sets the status bar preferences dialog
 */

void InitStatusBarPrefDialog ()
{

	extern Widget	sb_clock_button;
	extern Widget	sb_wincoords_button;
	extern Widget	sb_plotcoords_button;
	extern Widget	sb_pixval_button;
	extern Widget	sb_distcalc_button;
	extern Widget	sb_tempunits_om;
	extern Widget	sb_distunits_om;
	extern Widget	degC_labelW;
	extern Widget	degK_labelW;
	extern Widget	degF_labelW;
	extern Widget	km_labelW;
	extern Widget	mi_labelW;
	extern Widget	nmi_labelW;
	int		i;
	GlobalDataObjectType	*gd;

/*
 *	Initialize the image data saving flag to False. If the user
 *	wants pixel value data displayed, it will be set to true below.
 */
	gd = GetGlobalDataObject();
	SetSaveImageDataFlag ( gd, False );
		
/*
 *	Update the status bar preferences window buttons
 */
	for ( i = 0; i < statusBarItemCount; i++ ) {

	    switch ( statusBarItemList [i] ) {

	    case CLOCK:
		XtVaSetValues ( sb_clock_button, XmNset, True, NULL );
		break;

	    case WINDOWCOORDS:
		XtVaSetValues ( sb_wincoords_button, XmNset, True, NULL );
		break;

	    case PLOTCOORDS:
		XtVaSetValues ( sb_plotcoords_button, XmNset, True, NULL );
		break;

	    case PIXELVALUE:
		XtVaSetValues ( sb_pixval_button, XmNset, True, NULL );
		break;

	    case DISTCALC:
		XtVaSetValues ( sb_distcalc_button, XmNset, True, NULL );
		SetSaveImageDataFlag ( gd, True );
		break;

	    }
	}

/*
 *	Update the preferred temperature and distance
 */
	if ( strcmp ( tunits, "C" ) == 0 )
	    XtVaSetValues ( sb_tempunits_om, XmNmenuHistory, degC_labelW, NULL );
	else if ( strcmp ( tunits, "K" ) == 0 )
	    XtVaSetValues ( sb_tempunits_om, XmNmenuHistory, degK_labelW, NULL );
	else if ( strcmp ( tunits, "F" ) == 0 )
	    XtVaSetValues ( sb_tempunits_om, XmNmenuHistory, degF_labelW, NULL );


	if ( strcmp ( dunits, "KM" ) == 0 )
	    XtVaSetValues ( sb_distunits_om, XmNmenuHistory, km_labelW, NULL );
	else if ( strcmp ( dunits, "MI" ) == 0 )
	    XtVaSetValues ( sb_distunits_om, XmNmenuHistory, mi_labelW, NULL );
	else if ( strcmp ( dunits, "NM" ) == 0 )
	    XtVaSetValues ( sb_distunits_om, XmNmenuHistory, nmi_labelW, NULL );


}

void
StatusBarRemoveItem ( int item )
{
	int	i;

/*
 *	If the item is in the list, leave it there, but change the
 *	sign to indicate that it's inactive.
 */
	for ( i = 0; i < statusBarItemCount; i++ ) {

	    if ( item == statusBarItemList [i] ) 
	        
		statusBarItemList [i] = (-1) * item;
	}

}


void
StatusBarAddItem ( int item )
{

	int		i;
	BooleanType	found = False;

/*
 *	If the item is in the list but inactive, turn it back on.
 */
	for ( i = 0; i < statusBarItemCount; i++ )

	    if ( item == (-1) * statusBarItemList [i] ) {
	        
		statusBarItemList [i] = item;
		found = True;
	    }


/*
 *	If the item was not found in the list, add it on to the end.
 */
	if ( ! found ) {
	
		statusBarItemList [statusBarItemCount] = item;
		statusBarItemCount++;

	}

}



void
StatusBarItemToggleCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	XmToggleButtonCallbackStruct *cbs =
			(XmToggleButtonCallbackStruct *) xt_call_data;


	if ( cbs->set ) 
		StatusBarAddItem ( (int)client_data );
	else
		StatusBarRemoveItem ( (int)client_data );
	
/*
 *	If the item is PIXELVALUE, toggle the image data saving capability.
 */
	if ( (int)client_data == PIXELVALUE ) ToggleImageDataSaving ( cbs->set );


	UpdateStatusBar ( GetActiveWindowObject () );
}



void
SetStatusBarTempUnitsCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{


	XmToggleButtonCallbackStruct *cbs =
			(XmToggleButtonCallbackStruct *) xt_call_data;



	switch ( (int) client_data ) {

	    case CELCIUS:
		strcpy ( tunits, "C" );
		break;

	    case KELVIN:
		strcpy ( tunits, "K" );
		break;

	    case FAHRENHEIT:
		strcpy ( tunits, "F" );
		break;

	}

}

void
SetStatusBarDistanceUnitsCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{

	XmToggleButtonCallbackStruct *cbs =
			(XmToggleButtonCallbackStruct *) xt_call_data;



	switch ( (int) client_data ) {

	    case KILOMETERS:
		strcpy ( dunits, "KM" );
		break;

	    case MILES:
		strcpy ( dunits, "MI" );
		break;

	    case NAUTICAL_MILES:
		strcpy ( dunits, "NM" );
		break;

	}
	
}
