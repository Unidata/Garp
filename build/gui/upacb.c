/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	upacb.c
 *
 *	Callbacks and utilities for radiosonde data.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 4/97	COMET	Changed the names of some macros for consistency
 *			with other data types.
 *	 8/97	COMET	Added time matching code.
 *	 9/97	COMET	Added tfilter
 *	 9/97	COMET	Read pressure levels in GetSoundingLevels from
 *			Garp_defaults.
 *	11/97	COMET	Add call to SetTimeListLabel()
 *	11/97	COMET	Added "titleIndex" to keep track of title line.
 *	12/97	COMET	Get default filter setting from "Garp_defaults".
 *	12/97	COMET	Remove SetTimeListLabel and pass label widget
 *			through BuildUpperairList(). Added code to refresh 
 *			the scrolled time list after display
 *	12/97	COMET	Added a "select_list" object to track date/time
 *			selections when time matching is turned off.
 *
 ***********************************************************************/


#include "guimacros.h"
#include "genglobs.h"
#include "winobj.h"
#include "upaobj.h"
#include "upaprof.h"
#include "utils.h"
#include "menucb.h"
#include "_proto.h"

int * MatchIndicesToList ( char **, int, int, int * );

void UpperairIconCB (Widget , XtPointer , XtPointer );

ListParentObjectType * 
ReturnListStruct ( Widget, ListParentObjectType,
                   ListIndexObjectType, ListIndexObjectType * );




void
SelectUpaIconCB (Widget w, XtPointer client_data, XtPointer call_data )
/*
 * This callback is used to decide which upper air dialog to popup.
 */
{
	GuiUpperairDialogType	*upa;
	GuiUpaProfileDialogType	*upf;
	Widget			option_menu, b;
	extern Widget		upa_dialog;
	extern Widget		upa_profile_dialog;

	int			which;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	which = (int) client_data;
	upa = GetGuiUpperairDialog();
	upf = GetGuiUpaProfileDialog();

/*
 *	which = 0 if icon was clicked.
 */
	if ( which == 0 )
	    which = GetUpaPlotType ( upa );

/*
 *	Set the projection button and call the appropriate dialog.
 */
	if      ( which == UPPERAIROBJECT ) {
	    option_menu = GetUpaPlotTypeOMW ( upa );
	    b           = GetUpaPlotTypeW ( upa );
	    XtUnmanageChild ( option_menu );
	    XtVaSetValues ( option_menu, XmNmenuHistory, b, NULL );
	    XtManageChild ( option_menu );
	    UpperairIconCB ( w, client_data, call_data );
	    XtManageChild ( upa_dialog );
	}
	else if ( which == UPPERAIRPROFILEOBJECT ) {
	    option_menu = GetPlotTypeOmUFW ( upf );
	    b           = GetPlotTypeUFW ( upf );
	    XtUnmanageChild ( option_menu );
	    XtVaSetValues ( option_menu, XmNmenuHistory, b, NULL );
	    XtManageChild ( option_menu );
	    UpaProfileIconCB ( w, client_data, call_data );
	    XtManageChild ( upa_profile_dialog );
	}
/*
 *	Save current setting.
 */
	SetUpaPlotType ( upa, which );
}


void
UpperairIconCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
/*
 *	This callback is called when upper air icon is pushed.
 */
{
	GuiUpperairDialogType	*upa;
	WindowObjectType	*wo;

	Widget			scrolled_list, filter;
	int	nfiles=2;

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
	upa = GetGuiUpperairDialog();

/*
 *	Get widget ids.
 */
	scrolled_list = GetUpaScrolledListW ( upa );
	filter        = GetUpaTimeFilterW ( upa );

/*
 *	Unmanage widgets.
 */
	XtUnmanageChild ( scrolled_list );

/*
 *	Build time list
 */
	BuildUpperairList ( scrolled_list,
			    GetUpaTimeListLabelW(upa),
			    filter,
	               	    nfiles,
	                    &(upa->time_list),
	                    &(upa->match_list),
	                    &(upa->select_list) );

/*
 *	Manage widgets.
 */
	XtManageChild ( scrolled_list );

/*
 *	Cancel busy cursor.
 */
	CancelAbortProcess( wo, False );
}


void
SetSoundingLevelCB( Widget    w,
	         XtPointer clientData,
	         XtPointer xt_call_data )
{
	Arg			al[64];
	register int		ac;
	XmString		xmstr;

	GuiUpperairDialogType	*upa;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
	            (XmPushButtonCallbackStruct *) xt_call_data;

	upa = GetGuiUpperairDialog();

/*
 *	Save button info.
 */
	upa->level_b.setbutton = bo;

/*
 *	Update label.
 */
	FreeUpaLevelLabel ( upa );
	SetUpaLevelLabel ( upa, upa->level_b.setbutton->label );

	ac = 0;
	xmstr = XmStringLtoRCreate ( GetUpaLevelLabel ( upa ),
                                     XmSTRING_DEFAULT_CHARSET );
	XtSetArg ( al[ac], XmNlabelString, xmstr ); ac++;
	XtSetValues ( GetUpaLevelLabelW ( upa ), al, ac );
	XmStringFree ( xmstr );
/*
 *	Destroy dialog.
 */
	XtDestroyWidget( upa->level_b.parent );
}


void
SelectSoundingLevelCB ( Widget w,
	                XtPointer client_data,
	                XtPointer xt_call_data )
{
	Widget			shell, rowcol, b;

	GuiUpperairDialogType	*upa;

	ButtonObject		*bo;
	ButtonParentObjectType	*bpo;
	char			*coordinate="pressure";
	char			**levels;
	int			i, num_levels;
	int			plot_type;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	plot_type = (int) client_data;

	upa = GetGuiUpperairDialog();

	bpo = &( upa->level_b );

/*
 *	Get number of vertical levels for this model in this coordinate system.
 */
	num_levels = GetSoundingLevels ( coordinate, &levels );
	bpo->button_cnt = num_levels;
	bpo->button_alloc_cnt = bpo->button_cnt;

/*
 *	Build dialog shell and widgets.
 */
	shell = XtVaCreatePopupShell( "Sounding Levels",
				xmDialogShellWidgetClass,
				( Widget ) GetTopShell(w),
				XmNdeleteResponse, XmDESTROY,
				NULL);

	rowcol = XtVaCreateWidget( "rowcolumn",
				xmRowColumnWidgetClass, shell,
				XmNnumColumns, 5,
				XmNpacking, XmPACK_COLUMN,
				XmNorientation, XmVERTICAL,
				NULL);

	bpo->parent = shell;
	bo = &(bpo->buttons[0]);

	for ( i=0; i<num_levels; i++, bo++ ) {
	    bo->w = XtVaCreateManagedWidget( levels[i],
				xmPushButtonWidgetClass, rowcol,
				XmNfillOnArm, TRUE,
				NULL);

	    XtAddCallback ( bo->w, XmNactivateCallback, SetSoundingLevelCB, bo );
	    bo->ismanaged = 1;
	    if ( bo->string != NULL ) Free (bo->string);
	    bo->string = strdup ( "" );
	    if ( bo->label != NULL ) Free (bo->label);
	    bo->label = strdup ( levels[i] );

	}

	XtManageChild ( rowcol );

/*
 *	Free.
 */
	FreeList ( num_levels, levels );

}

int
GetSoundingLevels ( char *coordinate, char  ***levels )
{
/*
 * Get a list of sounding levels from a key in the "Garp_defaults" file.
 */
	char	**ptr;
	char	*keylist;
	int	num_levels = 0;

	keylist = GetConfigValue( "upalevels" );
	if ( keylist == NULL )
	    printf (
	    "Error in GetSoundingLevels - 'upalevels' key is undefined\n" );

	ptr = SplitByDelimeter ( keylist, ",", &num_levels );
	*levels = ptr;

	if ( keylist != NULL ) Free ( keylist );

	return num_levels;
}


void
FilterUpaTimes (Widget w, XtPointer client_data, XtPointer call_data )
{
	GuiUpperairDialogType	*upa;
	Widget			scrolled_list, filter;
	int			nfiles=2;
	int			verbose;

        XmToggleButtonCallbackStruct *cbs = 
			(XmToggleButtonCallbackStruct*) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("FilterUpaTimes\n" );

	upa = GetGuiUpperairDialog();

/*
 *	Get widget ids.
 */
	scrolled_list = GetUpaScrolledListW ( upa );
	filter        = GetUpaTimeFilterW ( upa );

/*
 *	Unmanage scrolled list.
 */
	XtUnmanageChild ( scrolled_list );

/*
 *	Scrub the list if required.
 */
	BuildUpperairList ( scrolled_list, 
			    GetUpaTimeListLabelW(upa),
			    filter,
	                    nfiles,
	                    &(upa->time_list),
	                    &(upa->match_list),
	                    &(upa->select_list) );

/*
 *	Manage scrolled list.
 */
	XtManageChild ( scrolled_list );
}


void
UpperairCB  (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
/*
 *	This callback is called when the Apply button is pushed.
 */
	MetObjectType		*mlist[MAXMETOBJS];
	WindowObjectType	*wo;
	GlobalDataObjectType	*gd;
	GeoRefObjectType	*geo;

	char			*snfile, *dattim;
	char 			*levels, *vcoord, *filter;
	char			snparm[73];

	ListParentObjectType	*inList;
	static char		colors[73];
	static char		marker[73];
	static char		title[73];
	static char		text[73];
	static int		skpmis;

	char			*ptext;
	int			*indexList;
	int			i, count, matchSize, metObjectCount;
	int			titleIndex, viewType;

	struct {
	    BooleanType	temperature;
	    BooleanType	dewpt;
	    BooleanType	stid;
	    BooleanType	height;
	    BooleanType	height_ch;
	    BooleanType	wind;
	    BooleanType	color;
	    int		units;
	    int		symbol;
	} toggle;


        XmPushButtonCallbackStruct *cbs = 
	                           (XmPushButtonCallbackStruct *) xt_call_data;

	GuiUpperairDialogType *upa =
			(GuiUpperairDialogType *) client_data;

	gd = GetGlobalDataObject();
	wo = GetActiveWindowObject();

	inList = ReturnListStruct ( GetUpaScrolledListW ( upa ),
                                    upa->time_list,
                                    upa->match_list,
                                    &(upa->select_list) );
	if ( inList == NULL ) return;
	count = inList->listcount;

/*
 *	Save the first time in the list for selection purposes.
 */
	if ( upa->select_list.listcount > 0 )
	    PreviousSelectedTime ( "SET", UPPERAIROBJECT, inList->label[0] );

/*
 *	Check on the view types for compatibility. Compare this object 
 *	with the current georeference view state and do a reset if
 *	they don't match.
 */
	viewType  = GetMetObjectView( UPPERAIROBJECT );

	if ( geo = GetGeoRefObject( wo) )
		if ( geo->viewType != viewType ) WindowObjectFullReset( wo );

/*
 *	Get toggle button values.
 */
	toggle.temperature = XmToggleButtonGetState ( GetUpaTempW ( upa ) );
	toggle.dewpt       = XmToggleButtonGetState ( GetUpaDwptW ( upa ) );
	toggle.stid        = XmToggleButtonGetState ( GetUpaStidW ( upa ) );
	toggle.height      = XmToggleButtonGetState ( GetUpaHeightW ( upa ) );
	toggle.height_ch   = XmToggleButtonGetState ( GetUpaHeightChW ( upa ) );
	toggle.wind        = XmToggleButtonGetState ( GetUpaWindW ( upa ) );
	if ( XmToggleButtonGetState ( GetUpaWindUnitsMsW ( upa ) ) )
	    toggle.units = METERSPERSECOND;
	if ( XmToggleButtonGetState ( GetUpaWindUnitsKnotsW ( upa ) ) )
	    toggle.units = KNOTS;
	if ( XmToggleButtonGetState ( GetUpaWindSymbolBarbW ( upa ) ) )
	    toggle.symbol = BARB;
	if ( XmToggleButtonGetState ( GetUpaWindSymbolArrowW ( upa ) ) )
	    toggle.symbol = ARROW;
/*
 *	Set snparm string according to input.
 */
	upa_parms ( snparm,		toggle.temperature,
	            toggle.dewpt,	toggle.stid,
	            toggle.height,	toggle.height_ch,
	            toggle.wind,	toggle.units,
		    toggle.symbol );

/*
 *	Extract values from upper air object.
 */
	levels = strdup ( GetUpaLevelLabel ( upa ) );
	vcoord = strdup ("pres");
	if ( strcmp ( levels, "Surface" ) == 0 ) {
	    Free ( levels );
	    levels = strdup ( "0" );
	}
	if ( strcmp ( levels, "Top" ) == 0 ) {
	    Free ( levels );
	    levels = strdup ( "-1" );
	}

/*
 *	Get the next color
 */
	GetNextObjectColor ( gd );
	strcpy ( colors, gd->colors);

	sprintf ( text, "%s/%s/%s/%s", gd->text_size, gd->text_font,
	                               gd->text_width, gd->text_hw_flag );
/*
 *	Set filter.
 */
	filter = GetConfigValue( "upa_filter" );
	if ( filter == NULL ) filter = strdup ( "1" );
/*
 *	Hard code the following for the moment....
 */
	skpmis = TRUE;
	strcpy ( marker, "0" );
	strcpy ( title, "Upperair Obs" );

/*
 *	Create metObjects.
 */
	metObjectCount = 0;
	titleIndex = GetNextTitleIndex ( wo );

	for ( i=0; i<count; i++ ) {

/*
 *	Test for valid time match.
 */
	    if ( inList->label[i] == NULL ) {
	        snfile = strdup ( "MISSING" );
	        dattim = strdup ( "MISSING" );
	    }
	    else {
	        snfile = strdup ( inList->string[i] );
	        dattim = strdup ( inList->label[i] );
	    }
	    mlist[i] = (MetObjectType *) MakeUpperairObject( 
                           snfile, snparm, dattim, levels, vcoord,
                           colors, marker, title, filter, text, skpmis,
			   titleIndex,
			   XmToggleButtonGetState ( GetTimeFilterUFW (upa)));

	    metObjectCount++;

	    Free ( snfile );
	    Free ( dattim );
	}

	Free ( levels );
	Free ( vcoord );
	Free ( filter );

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
	if ( upa->match_list.listcount < 1 ) {
	    indexList = MatchIndicesToList (
			upa->time_list.label,
			upa->time_list.listcount,
			UPPERAIROBJECT, &matchSize );
	    upa->match_list.listcount = matchSize;
	    upa->match_list.intList = indexList;
	}

	if( wo )
		ProcessDisplayList( wo );

/*
 *	Refresh the item list if time matching
 */
	RefreshScrolledTimeLists ( True );

/*
 *	Free.
 */
	FreeListParentObject ( inList );

	return;
}
