/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	upaprofcb.c
 *
 *	Callbacks and utilities for radiosonde data.
 *
 *	History:
 *
 *	 1/97	COMET	Original copy
 *	 3/97	COMET	Added wind position indicator.
 *	 4/97	COMET	Changed the names of some macros for consistency
 * 			with other data types. Added line type control.
 *	 8/97	COMET	Changed BuildUpperairList to BuildUpfList since
 *			this projection does not require time matching.
 *	 9/97	COMET	Added tfilter
 *	11/97	COMET	Added "titleIndex" to keep track of title line.
 *	12/97	COMET	Get default text settings from keys in "Garp_defaults".
 *	12/97	COMET	Added time matching.
 *	12/97	COMET	Added a "select_list" object to track date/time
 *			selections when time matching is turned off.
 *	 1/98	COMET	Use "select_list" for time matching. Removed strdup
 *			from call to GetNextWindPosition due to function
 *			change.
 *
 ***********************************************************************/


#include "guimacros.h"
#include "genglobs.h"
#include "winobj.h"
#include "upaprof.h"
#include "utils.h"
#include "menucb.h"
#include "_proto.h"

char * GetNextWindPostition ( GlobalDataObjectType * );

ListParentObjectType * 
ReturnListStruct ( Widget, ListParentObjectType,
                   ListIndexObjectType, ListIndexObjectType * );

void
UpaProfileIconCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
/*
 *	This callback is called when upper air icon is pushed.
 */
{
	GuiUpaProfileDialogType	*upf;
	WindowObjectType	*wo;

	Widget			scrolled_list, filter, station_list;
	char			*directory;
	int			nfiles=2;

        XmPushButtonCallbackStruct *cbs = 
	            (XmPushButtonCallbackStruct *) xt_call_data;

	wo = GetActiveWindowObject();

/*
 *	Initiate busy cursor.
 */
	SetupAbortProcess( wo );

/*
 *	Get dialog structure.
 */
	upf = GetGuiUpaProfileDialog();

/*
 *	Get configuration keys.
 */
	directory = GetConfigValue( "profiler" );

/*
 *	Get widget ids.
 */
	scrolled_list = GetScrolledListUFW ( upf );
	filter        = GetTimeFilterUFW ( upf );
	station_list  = GetStationListUFW ( upf );

/*
 *	Unmanage widgets.
 */
	XtUnmanageChild ( scrolled_list );
	XtUnmanageChild ( station_list );

/*
 *	Get list of date/time stamps. Get list of stations.
 */
	BuildUpfList (	scrolled_list,
			GetUpaProfTimeListLabelW(upf),
			filter,
			nfiles,
			&(upf->time_list),
			&(upf->match_list),
	                &(upf->select_list) );

	BuildStationList ( station_list, directory, 4, &(upf->station_list) );

/*
 *	Manage widgets.
 */
	XtManageChild ( scrolled_list );
	XtManageChild ( station_list );

/*
 *	Cancel busy cursor.
 */
	CancelAbortProcess( wo, False );

	if ( directory != NULL ) Free ( directory );
}


void
SetUpaProfileDiagramCB (Widget w, XtPointer client_data, XtPointer call_data )
{
/*
 * Set the thermodynamic diagram type to use. 
 */

	GuiUpaProfileDialogType	*upf;
	int			which, verbose;

	which = (int) client_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("SetUpaProfileDiagramCB\n" );

	upf = GetGuiUpaProfileDialog();

	FreeDiagramTypeUF ( upf );

	switch (which)
	{
	case 1:
	  SetDiagramTypeUF ( upf, "skewt" );
	  break;
	case 2:
	  SetDiagramTypeUF ( upf, "stuve" );
	  break;
	default:
	  printf ("SetUpaProfileDiagramCB - %d not supported.\n", which );
	  break;
	}
}


void
FilterUpaProfileTimes (Widget w, XtPointer client_data, XtPointer call_data )
{
	GuiUpaProfileDialogType	*upf;
	Widget			scrolled_list, filter;
	int			nfiles=2;
	int			verbose;

        XmToggleButtonCallbackStruct *cbs = 
			(XmToggleButtonCallbackStruct*) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("FilterUpaProfileTimes\n" );

	upf = GetGuiUpaProfileDialog();

/*
 *	Get widget ids.
 */
	scrolled_list = GetScrolledListUFW ( upf );
	filter        = GetTimeFilterUFW ( upf );

/*
 *	Unmanage scrolled list.
 */
	XtUnmanageChild ( scrolled_list );

/*
 *	Scrub the list if required.
 */
        BuildUpfList (  scrolled_list,
                        GetUpaProfTimeListLabelW(upf),
                        filter,
                        nfiles,
                        &(upf->time_list),
                        &(upf->match_list),
                        &(upf->select_list) );


/*
 *	Manage scrolled list.
 */
	XtManageChild ( scrolled_list );
}



void
UpperairProfileCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
/*
 *	This callback is called when the Apply button is pushed in the
 *	upperair vertical view dialog.
 */
	extern Widget		main_shell;

	MetObjectType		*mlist[MAXMETOBJS];
	WindowObjectType	*wo;
	GlobalDataObjectType	*gd;
	GeoRefObjectType	*geo;

	ListParentObjectType	*inList;
	char			**stns;
	char			*snfile, *dattim, *vcoord,
				*stndex, *stncol, *wind_symbol,
				*ptype,  *symbol_size, *symbol_width,
				*xaxis,  *yaxis,  *line_type,
				*winpos, *title;

	static char		colors[73], marker[73], filter[73],
				text[73]  ;

	int			count, i, j, k;

	int			sw_count;
	int			stn_count, *stn_index;
	int			titleIndex, viewType;


        XmPushButtonCallbackStruct *cbs = 
	                           (XmPushButtonCallbackStruct *) xt_call_data;

	GuiUpaProfileDialogType *upf =
			(GuiUpaProfileDialogType *) client_data;

	wo = GetActiveWindowObject();
	gd = GetGlobalDataObject();

/*
 *	Get highlighted position indices from list widgets.
 */

	XmListGetSelectedPos ( upf->station_list.w, &stn_index, &stn_count );
	if ( stn_count <= 0 ) {
	    InfoPop ( main_shell, INFORMATION,
	    "\nSorry boss, you need to choose\nat least one station.\n",
	    FALSE, FALSE, NULL, NULL );

	    return;
	}
/*
 *	File Selection widgets count starting at 1.
 */
	AdjustIndexOffset ( stn_count, stn_index );

	inList = ReturnListStruct ( GetScrolledListUFW( upf ),
	                            upf->time_list,
	                            upf->match_list,
	                            &(upf->select_list) );
	if ( inList == NULL ) return;
	sw_count = inList->listcount;

/*
 *	Save the first time in the list for selection purposes.
 */
	if ( upf->select_list.listcount > 0 )
	    PreviousSelectedTime ( "SET", UPPERAIRPROFILEOBJECT,
	                            inList->label[0] );

/*
 *	Check on the view types for compatibility. Compare this object 
 *	with the current georeference view state and do a reset if
 *	they don't match.
 */
	viewType  = GetMetObjectView( UPPERAIRPROFILEOBJECT );

	if ( geo = GetGeoRefObject( wo) )
		if ( geo->viewType != viewType ) WindowObjectFullReset( wo );

/*
 *	Extract values from upper air object.
 */
	stns = StringListAllocate ( stn_count );
	for ( i=0; i<stn_count; i++ ) {
	    stns[i] = (char *) malloc ( STIDSIZE * sizeof( char ) + 1 );
	    sscanf ( upf->station_list.label[stn_index[i]], "%s", stns[i] );
	}

/*
 *	Get the next color, set text.
 */
	GetNextObjectColor ( gd );
	strcpy ( colors, gd->colors);

	sprintf ( text, "%s/%s/%s/%s", gd->text_size, gd->text_font,
	                               gd->text_width, gd->text_hw_flag );

/*
 *	Plot type, thermodynamic indices.
 */
	ptype = strdup ( GetDiagramTypeUF ( upf ) );
	stndex = strdup ( GetIndicesUF ( upf ) );
	if ( strstr ( ptype, "skewt" ) ) {
	    xaxis = GetConfigValue( "SkewtXAxis" );
	    if ( xaxis == NULL ) xaxis = strdup (" ");
	}
        else if ( strstr ( ptype, "stuve" ) ) {
	    xaxis = GetConfigValue( "StuveXAxis" );
	    if ( xaxis == NULL ) xaxis = strdup (" ");
	}

/*
 *	Hard code the following for the moment....
 */
	strcpy ( marker, "0" );
	strcpy ( filter, "1" );

	wind_symbol = strdup ( "bk" );
	symbol_size = strdup ( "1.0" );
	symbol_width  = strdup ( "1.5" );
	vcoord = strdup ("pres");
	line_type = strdup ( GetLineTypeUF ( upf ) );
	winpos = GetNextWindPostition ( gd );
	yaxis =  strdup ("///;1");
	stncol = strdup( colors );

	titleIndex = GetNextTitleIndex ( wo );

/*
 *	Loop through creating met objects.
 */
	count = sw_count * stn_count;
	k = 0;
	for ( i=0; i<stn_count; i++ ) {
	    for ( j=0; j<sw_count; j++ ) {

		if ( inList->label[j] == NULL ) {
		    snfile = strdup ( "MISSING" );
		    dattim = strdup ( "MISSING" );
		}
		else {
		    snfile = strdup ( inList->string[j] );
		    dattim = strdup ( inList->label[j] );
		}

		title = strdup ( "Profile" );

	    	mlist[k] = (MetObjectType *)
			MakeUpaProfileObject( 
				snfile, stns[i], dattim, stndex, stncol, vcoord,
				colors, wind_symbol, symbol_size, symbol_width,
				line_type, winpos, ptype, xaxis, yaxis, marker,
				title, filter, text, titleIndex,
				XmToggleButtonGetState ( GetTimeFilterUFW (upf) ));

		k++;
		Free ( title );
		Free ( snfile );
		Free ( dattim );
	    }
	}

	StringListFree ( stn_count, stns );
	Free ( ptype );
	Free ( stndex );
	Free ( stncol );
	Free ( vcoord );
	Free ( wind_symbol );
	Free ( symbol_size );
	Free ( symbol_width );
	Free ( line_type );
	Free ( winpos );
	Free ( xaxis );
	Free ( yaxis );
	Free ( stn_index );

	if ( count > 0 )
	    AddObjectListToDisplay( count, mlist );

	wo = GetActiveWindowObject();

/*
 *	Create a DataTimeObject for time matching.
 */
	if ( wo && sw_count > 0 )
	    CreateDataTimeObject ( wo, mlist );


	if( wo )
		ProcessDisplayList( wo );

/*
 *	Refresh the time list if time matching
 */
	RefreshScrolledTimeLists ( True );

/*
 *	Free.
 */
	FreeListParentObject ( inList );

	return;
}
