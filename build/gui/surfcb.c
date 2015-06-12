/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	surfcb.c
 *
 *	Surface data callbacks.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 7/97	COMET	Dynamically initialize surface station parameters.
 *	 8/97	COMET	Added time matching code.
 *	11/97	COMET	Add call to SetTimeListLabel()
 *	11/97	COMET	Added "titleIndex" to keep track of title line.
 *	12/97	COMET	Get default filter setting from "Garp_defaults".
 *	12/97	COMET	Remove SetTimeListLabel and pass label widget 
 *			through BuildSfcObsList(). Added code to refresh 
 *			the scrolled time list after display
 *	 1/98	COMET	Removed MakeSurfaceObjTitle
 *	 2/98	COMET	Added a blank character to empty sfparm positions
 *			so it is parse properly by subsequent routines.
 *
 ***********************************************************************/


#include "guimacros.h"
#include "gendef.h"
#include "genglobs.h"
#include "winobj.h"
#include "sfcobj.h"
#include "utils.h"
#include "menucb.h"
#include "_proto.h"

#define FILLEDCIRCLE    0
#define TRIANGLE        3
#define BOX             4
#define DIAMOND         6
#define STAR            14

int * MatchIndicesToList ( char **, int, int, int * );

ListParentObjectType *
ReturnListStruct ( Widget, ListParentObjectType,
	           ListIndexObjectType, ListIndexObjectType * );



void
SurfaceObsIconCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
/*
 *	This callback is called when surface obs icon is pushed.
 */
{
	GuiSfcObsDialogType	*sfc;
	WindowObjectType	*wo;

	Widget			scrolled_list, sfcparm_list;
	int	nfiles=2;

        XmPushButtonCallbackStruct *cbs = 
	            (XmPushButtonCallbackStruct *) xt_call_data;

	sfc = GetGuiSfcObsDialog();
	wo = GetActiveWindowObject();

/*
 *	Initiate busy cursor.
 */
	SetupAbortProcess( wo );

/*
 *	Get widget ids.
 */
	scrolled_list = GetSfcScrolledListW ( sfc );
	sfcparm_list = GetSfcParameterListW ( sfc );

/*
 *	Unmanage widgets.
 */
	XtUnmanageChild ( scrolled_list );
	XtUnmanageChild ( sfcparm_list );

/*
 *	Clear scrolled list.
 */
	RemoveProductsFromList ( scrolled_list );
	RemoveProductsFromList ( sfcparm_list );

/*
 *	Dynamically build dialog.
 */

	BuildSfcObsList ( scrolled_list,
			  GetSfcTimeListLabelW(sfc),
	                  nfiles,
			  GetGuiSfcObsType ( sfc ),
	                  &(sfc->time_list),
	                  &(sfc->match_list),
	                  &(sfc->select_list) );

	BuildSurfaceParameterList ( sfcparm_list, &(sfc->parm_list) );

/*
 *	Manage widgets.
 */
	XtManageChild ( scrolled_list );
	XtManageChild ( sfcparm_list );

/*
 *	Cancel busy cursor.
 */
	CancelAbortProcess( wo, False );
}


void
SaveTimeMatchedList ( int num, int *index, char **labels, char ***picked_list )
/*
 * This function stores list items at given indices to to another list.
 */ 
{
	int	i;
	int	ind;
	char	**listp;

	
	*picked_list = (char **) malloc (MAX_LIST * sizeof( char * ) );

	listp = *picked_list;

	for ( i=0; i<num; i++ ) {
	    ind = index[i];
	    *listp = (char *) malloc ( strlen ( labels[ind]) + 1 );
	    strcpy ( *listp, labels[ind] );
	    listp++;
	}
/*
 *	Null terminate the array.
 */
	*listp = (char *) NULL;

}


void
DefaultSfcParameters ()
{
/*
 * Set the default surface station list for this ob type.
 */
	GuiSfcObsDialogType	*sfc;
	BooleanType		verbose;

	char			*keylist, *plot_type, type[80];
	char			**labels;
	int			num;

	char			*pos1,  *pos2,  *pos3,  *pos4,  *pos5,  *pos6;
	char			*pos7,  *pos8,  *pos9,  *pos10, *pos11;


	verbose = GetVerboseLevel();
	if ( verbose > VERBOSE_0 )
	    printf("DefaultSfcParameters\n");

	sfc = GetGuiSfcObsDialog();
/*
 *	Get station model parameter string.
 */
	plot_type =  GetGuiSfcObsType ( sfc );
	if ( plot_type == NULL ) return;
	sprintf ( type, "%s%s", plot_type, "_parms" );
	keylist = GetConfigValue( type );

/*
 *	Parse default parameter string.
 */
	ToUpper ( keylist );
	labels  = SplitByDelimeter ( keylist, ";", &num );

/*
 *	Create labels for each position. If the label is a "BLNK" or 
 *	a "SPAC", make it blank for readability.
 */
	if ( num > 0 && labels[0] ) {
	    if ( ( strcmp ( labels[0], "BLNK" ) == 0 ) ||
	         ( strcmp ( labels[0], "SPAC" ) == 0 ) )
		pos1  = strdup ( " " );
	    else
		pos1  = strdup ( labels[0] );
	}
	else                        pos1  = strdup ( " " ); 

	if ( num > 1  && labels[1] ) {
	    if ( ( strcmp ( labels[1], "BLNK" ) == 0 ) ||
                 ( strcmp ( labels[1], "SPAC" ) == 0 ) )
		pos2  = strdup ( " " );
	    else
		pos2  = strdup ( labels[1] );
	}
	else                        pos2  = strdup ( " " ); 

	if ( num > 2 && labels[2] ) {
	    if ( ( strcmp ( labels[2], "BLNK" ) == 0 ) ||
                 ( strcmp ( labels[2], "SPAC" ) == 0 ) )
		pos3  = strdup ( " " );
	    else
		pos3  = strdup ( labels[2] );
	}
	else                        pos3  = strdup ( " " ); 

	if ( num > 3 && labels[3] ) {
	    if ( ( strcmp ( labels[3], "BLNK" ) == 0 ) ||
                 ( strcmp ( labels[3], "SPAC" ) == 0 ) )
		pos4  = strdup ( " " );
	    else
		pos4  = strdup ( labels[3] );
	}
	else                        pos4  = strdup ( " " ); 

	if ( num > 4 && labels[4] ) {
	    if ( ( strcmp ( labels[4], "BLNK" ) == 0 ) ||
                 ( strcmp ( labels[4], "SPAC" ) == 0 ) )
		pos5  = strdup ( " " );
	    else
		pos5  = strdup ( labels[4] );
	}
	else                        pos5  = strdup ( " " ); 

	if ( num > 5 && labels[5] ) {
	    if ( ( strcmp ( labels[5], "BLNK" ) == 0 ) ||
                 ( strcmp ( labels[5], "SPAC" ) == 0 ) )
		pos6  = strdup ( " " );
	    else
		pos6  = strdup ( labels[5] );
	}
	else                        pos6  = strdup ( " " ); 

	if ( num > 6 && labels[6] ) {
	    if ( ( strcmp ( labels[6], "BLNK" ) == 0 ) ||
                 ( strcmp ( labels[6], "SPAC" ) == 0 ) )
		pos7  = strdup ( " " );
	    else
		pos7  = strdup ( labels[6] );
	}
	else                        pos7  = strdup ( " " ); 

	if ( num > 7  && labels[7] ) {
	    if ( ( strcmp ( labels[7], "BLNK" ) == 0 ) ||
                 ( strcmp ( labels[7], "SPAC" ) == 0 ) )
		pos8  = strdup ( " " );
	    else
		pos8  = strdup ( labels[7] );
	}
	else                        pos8  = strdup ( " " ); 

	if ( num > 8 && labels[8] ) {
	    if ( ( strcmp ( labels[8], "BLNK" ) == 0 ) ||
                 ( strcmp ( labels[8], "SPAC" ) == 0 ) )
		pos9  = strdup ( " " );
	    else
		pos9  = strdup ( labels[8] );
	}
	else                        pos9  = strdup ( " " ); 

	if ( num > 9 && labels[9] ) {
	    if ( ( strcmp ( labels[9], "BLNK" ) == 0 ) ||
                 ( strcmp ( labels[9], "SPAC" ) == 0 ) )
		pos10  = strdup ( " " );
	    else
		pos10  = strdup ( labels[9] );
	}
	else                        pos10  = strdup ( " " ); 

	if ( num > 10 && labels[10] ) {
	    if ( ( strcmp ( labels[10], "BLNK" ) == 0 ) ||
                 ( strcmp ( labels[10], "SPAC" ) == 0 ) )
		pos11  = strdup ( " " );
	    else
		pos11  = strdup ( labels[10] );
	}
	else                        pos11  = strdup ( " " ); 

/*
 *	Set textfield widgets.
 */
	XmTextFieldSetString ( GetSfcParm1  ( sfc ), pos1  );
	XmTextFieldSetString ( GetSfcParm2  ( sfc ), pos2  );
	XmTextFieldSetString ( GetSfcParm3  ( sfc ), pos3  );
	XmTextFieldSetString ( GetSfcParm4  ( sfc ), pos4  );
	XmTextFieldSetString ( GetSfcParm5  ( sfc ), pos5  );
	XmTextFieldSetString ( GetSfcParm6  ( sfc ), pos6  );
	XmTextFieldSetString ( GetSfcParm7  ( sfc ), pos7  );
	XmTextFieldSetString ( GetSfcParm8  ( sfc ), pos8  );
	XmTextFieldSetString ( GetSfcParm9  ( sfc ), pos9  );
	XmTextFieldSetString ( GetSfcParm10 ( sfc ), pos10 );
	XmTextFieldSetString ( GetSfcParm11 ( sfc ), pos11 );

/*
 *	Free.
 */
	Free ( keylist );
	Free ( pos1 );
	Free ( pos2 );
	Free ( pos3 );
	Free ( pos4 );
	Free ( pos5 );
	Free ( pos6 );
	Free ( pos7 );
	Free ( pos8 );
	Free ( pos9 );
	Free ( pos10 );
	Free ( pos11 );
	StringListFree ( num, labels );
}



void
DefaultSfcParametersCB (Widget w, XtPointer clientData, XtPointer call_data )
{
	BooleanType		verbose;


        XmPushButtonCallbackStruct *cbs = 
	            (XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if ( verbose > VERBOSE_0 )
	    printf("DefaultSfcParametersCB\n");

	DefaultSfcParameters();
}

void
ClearTextfieldCB(Widget w, XtPointer client_data, XtPointer xt_call_data)
{
	BooleanType		verbose;

	XmAnyCallbackStruct *call_data = 
			(XmAnyCallbackStruct *) xt_call_data ;

	verbose = GetVerboseLevel();
	if ( verbose > VERBOSE_0 )
	    printf("ClearTextfieldCB\n");

}


void
SFPlotTypeCB ( Widget w, XtPointer clientData, XtPointer call_data )
{
/*
 * This routine set the surface plot type and rebuilds the date/time
 * scrolled list.
 */
	GuiSfcObsDialogType	*sfc;
	BooleanType		verbose;

	char			*plot_type, *plot_label;


	ButtonObject *bo = (ButtonObject *) clientData;
	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if ( verbose > VERBOSE_0 )
	    printf("SFPlotTypeCB\n");

	sfc = GetGuiSfcObsDialog();
/*
 *	Set plot type.
 */
	plot_type  = strdup ( bo->string );
	plot_label = strdup ( bo->label );

	FreeGuiSfcObsType ( sfc );
	SetGuiSfcObsType ( sfc, plot_type );

	FreeGuiSfcObsLabel ( sfc );
	SetGuiSfcObsLabel ( sfc, plot_label );

/*
 *	Redraw station parameter textfield widgets.
 */
	DefaultSfcParameters ();

/*
 *	Rebuild date/time scrolled list.
 */
	SurfaceObsIconCB ( w, NULL, NULL );

/*
 *	Free.
 */
	Free ( plot_type );
	Free ( plot_label );
}


void
SFMarkerTypeCB ( Widget w, XtPointer clientData, XtPointer call_data )
{
/*
 * This routine sets the surface marker type.
 */
	GuiSfcObsDialogType	*sfc;
	BooleanType		verbose;

	char			*marker_type;


	ButtonObject *bo = (ButtonObject *) clientData;
	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if ( verbose > VERBOSE_0 )
	    printf("SFMarkerTypeCB\n");

	sfc = GetGuiSfcObsDialog();
/*
 *	Set plot type.
 */
	marker_type = strdup ( bo->string );
	FreeSfcMarkerType ( sfc );
	SetSfcMarkerType ( sfc, marker_type );

/*
 *	Free.
 */
	Free ( marker_type );
}



void
SurfaceObsCB  (Widget w, XtPointer client_data, XtPointer xt_call_data )
/*
 * This callback is called when the Apply button is pushed.
 */
{
	MetObjectType		*mlist[MAXMETOBJS];
	WindowObjectType	*wo;
	GlobalDataObjectType	*gd;
	GeoRefObjectType	*geo;

	ListParentObjectType	*inList;
	char			sffile[GEMPAKSTRING];
	char			sfparm[GEMPAKSTRING];
	char			dattim[GEMPAKSTRING];
	char			colors[GEMPAKSTRING];
	char			title[GEMPAKSTRING];
	char			text[GEMPAKSTRING];
	char			*filter, *buf;

	int			pmarker, marker, skpmis;
	int			*indexList, titleIndex;
	int			i, count, matchSize, metObjectCount;
	int			viewType;

	AbsTime			saveTime;
	BooleanType		verbose;
	BooleanType		color;

	struct {
	    char *pos1;
	    char *pos2;
	    char *pos3;
	    char *pos4;
	    char *pos5;
	    char *pos6;
	    char *pos7;
	    char *pos8;
	    char *pos9;
	    char *pos10;
	    char *pos11;
	} model;


        XmPushButtonCallbackStruct *cbs = 
	                           (XmPushButtonCallbackStruct *) xt_call_data;

	GuiSfcObsDialogType *sfc =
		            (GuiSfcObsDialogType *) client_data;

	gd = GetGlobalDataObject();
	wo = GetActiveWindowObject();
	verbose = GetVerboseLevel();

	if ( verbose > VERBOSE_0 ) printf("SurfaceObsCB\n");

	inList = ReturnListStruct ( GetSfcScrolledListW( sfc ),
	                            sfc->time_list,
	                            sfc->match_list,
	                            &(sfc->select_list) );
	if ( inList == NULL ) return;
	count = inList->listcount;
/*
 *	Save the first time in the list for selection purposes.
 */
	if ( sfc->select_list.listcount > 0 )
	    PreviousSelectedTime ( "SET", SURFACEOBJECT, inList->label[0] );

/*
 *	Check on the view types for compatibility. Compare this object 
 *	with the current georeference view state and do a reset if
 *	they don't match.
 */
	viewType  = GetMetObjectView( SURFACEOBJECT );

	if ( geo = GetGeoRefObject( wo) )
	    if ( geo->viewType != viewType ) WindowObjectFullReset( wo );

/*
 *	Get station model textfield values.
 */
	if ( buf = XmTextGetString ( GetSfcParm1 ( sfc ) ) ) {
	    if ( strlen ( buf ) < 1 ) {
	        model.pos1 = strdup ( " " );
	    }
	    else {
		model.pos1 = strdup ( buf );
	    }
	    XtFree ( buf );
	}
	else
	    model.pos1 = strdup ("BLNK");

	if ( buf = XmTextGetString ( GetSfcParm2 ( sfc ) ) ) {
	    if ( strlen ( buf ) < 1 ) {
		model.pos2 = strdup ( " " );
	    }
	    else {
	    	model.pos2 = strdup ( buf );
	    }
	    XtFree ( buf );
	}
	else
	    model.pos2 = strdup ("BLNK");

	if ( buf = XmTextGetString ( GetSfcParm3 ( sfc ) ) ) {
	    if ( strlen ( buf ) < 1 ) {
	        model.pos3 = strdup ( " " );
	    }
	    else {
		model.pos3 = strdup ( buf );
	    }
	    XtFree ( buf );
	}
	else
	    model.pos3 = strdup ("BLNK");

	if ( buf = XmTextGetString ( GetSfcParm4 ( sfc ) ) ) {
	    if ( strlen ( buf ) < 1 ) {
	        model.pos4 = strdup ( " " );
	    }
	    else {
		model.pos4 = strdup ( buf );
	    }
	    XtFree ( buf );
	}
	else
	    model.pos4 = strdup ("BLNK");

	if ( buf = XmTextGetString ( GetSfcParm5 ( sfc ) ) ) {
	    if ( strlen ( buf ) < 1 ) {
	        model.pos5 = strdup ( " " );
	    }
	    else {
		model.pos5 = strdup ( buf );
	    }
	    XtFree ( buf );
	}
	else
	    model.pos5 = strdup ("BLNK");

	if ( buf = XmTextGetString ( GetSfcParm6 ( sfc ) ) ) {
	    if ( strlen ( buf ) < 1 ) {
	        model.pos6 = strdup ( " " );
	    }
	    else {
		model.pos6 = strdup ( buf );
	    }
	    XtFree ( buf );
	}
	else
	    model.pos6 = strdup ("BLNK");

	if ( buf = XmTextGetString ( GetSfcParm7 ( sfc ) ) ) {
	    if ( strlen ( buf ) < 1 ) {
	        model.pos7 = strdup ( " " );
	    }
	    else {
		model.pos7 = strdup ( buf );
	    }
	    XtFree ( buf );
	}
	else
	    model.pos7 = strdup ("BLNK");

	if ( buf = XmTextGetString ( GetSfcParm8 ( sfc ) ) ) {
	    if ( strlen ( buf ) < 1 ) {
	        model.pos8 = strdup ( " " );
	    }
	    else {
		model.pos8 = strdup ( buf );
	    }
	    XtFree ( buf );
	}
	else
	    model.pos8 = strdup ("BLNK");

	if ( buf = XmTextGetString ( GetSfcParm9 ( sfc ) ) ) {
	    if ( strlen ( buf ) < 1 ) {
	        model.pos9 = strdup ( " " );
	    }
	    else {
		model.pos9 = strdup ( buf );
	    }
	    XtFree ( buf );
	}
	else
	    model.pos9 = strdup ("BLNK");

	if ( buf = XmTextGetString ( GetSfcParm10 ( sfc ) ) ) {
	    if ( strlen ( buf ) < 1 ) {
	        model.pos10 = strdup ( " " );
	    }
	    else {
		model.pos10 = strdup ( buf );
	    }
	    XtFree ( buf );
	}
	else
	    model.pos10 = strdup ("BLNK");

	if ( buf = XmTextGetString ( GetSfcParm11 ( sfc ) ) ) {
	    if ( strlen ( buf ) < 1 ) {
	        model.pos11 = strdup ( " " );
	    }
	    else {
		model.pos11 = strdup ( buf );
	    }
	    XtFree ( buf );
	}
	else
	    model.pos11 = strdup ("BLNK");

/*
 *	Set SFPARM string according to input.
 */
	sprintf ( sfparm, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s\0",
			model.pos1,  model.pos2,  model.pos3,  model.pos4,
			model.pos5,  model.pos6,  model.pos7,  model.pos8,
			model.pos9,  model.pos10, model.pos11 );
/*
 *	Set global data parameters.
 */
	color = XmToggleButtonGetState ( GetSfcColorW ( sfc ) );
	if ( color )
	    strcpy ( colors, "26;2;7;25;20;18;22;24;6" );
	else {
	    GetNextObjectColor ( gd );
	    strcpy ( colors, gd->colors);
	}

	sprintf ( text, "%s/%s/%s/%s", gd->text_size, gd->text_font,
	                               gd->text_width, gd->text_hw_flag );

/*
 *	Set filter.
 */
	filter = GetConfigValue( "sfc_filter" );
	if ( filter == NULL ) filter = strdup ( "1" );

/*
 *	Hard code the following for the moment....
 */
	skpmis = TRUE;
	pmarker = XmToggleButtonGetState ( GetSfcMarkerW ( sfc ) );
	marker = str2int ( GetSfcMarkerType ( sfc ) );

/*
 *	Create metObjects.
 */
	wo = GetActiveWindowObject();
	metObjectCount = 0;
	titleIndex = GetNextTitleIndex ( wo );

	for ( i=0; i<count; i++ ) {

/*
 *	Test for valid time match.
 */
	    if ( inList->label[i] == NULL ) {
	    	strcpy ( sffile, "MISSING" );
	    	strcpy ( dattim, "MISSING" );
	    }
	    else {
	    	strcpy ( sffile, inList->string[i] );
	    	strcpy ( dattim, inList->label[i] );
	    }

	    sprintf ( title, "%s %s", GetGuiSfcObsLabel ( sfc ), "Obs" );

	    mlist[i] = (MetObjectType *) MakeSurfaceObject( 
                        sfparm, sffile, dattim, colors, title,
	                filter, text, GetGuiSfcObsType(sfc), 
	                marker, pmarker, skpmis, titleIndex );

	    metObjectCount++;
	}

/*
 *	Make sure there is at least on non-NULL metObject in the list.
 */
	if ( metObjectCount > 0 )
	    AddObjectListToDisplay( count, mlist );

/*
 *	Create a DataTimeObject for time matching.
 */
	if ( wo && metObjectCount > 0 )
	    CreateDataTimeObject ( wo, mlist );

/*
 *	Save list indices in case dialog is not closed.
 */
	if ( sfc->match_list.listcount < 1 ) {
	    indexList = MatchIndicesToList (
			sfc->time_list.label,
			sfc->time_list.listcount,
			SURFACEOBJECT, &matchSize );
	    sfc->match_list.listcount = matchSize;
	    sfc->match_list.intList = indexList;
	}

	if( wo )
		ProcessDisplayList( wo );

/*
 *	Refresh the time list if time matching
 */
	RefreshScrolledTimeLists ( True );

/*
 *	Free.
 */
	Free ( model.pos1 );
	Free ( model.pos2 );
	Free ( model.pos3 );
	Free ( model.pos4 );
	Free ( model.pos5 );
	Free ( model.pos6 );
	Free ( model.pos7 );
	Free ( model.pos8 );
	Free ( model.pos9 );
	Free ( model.pos10 );
	Free ( model.pos11 );
	Free ( filter );
	FreeListParentObject ( inList );

	return;
}
