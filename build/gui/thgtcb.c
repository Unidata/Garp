/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	thgtcb.c
 *
 *	Callbacks and utilities for time/height cross section data.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 3/97	COMET	Added TAXIS parameter to FDF. Added scale factor
 *			textfield to option dialog.
 *	 6/97	COMET	Removed imgobj.h
 *	 7/97	COMET	Added model defined subdirectories.
 *	 9/97	COMET	Added model name to object, pass the actual time
 *			(for autoupdate)
 *	10/97	COMET	Added macro functionality.
 *	11/97	COMET	Fixed some small leaks.
 *	11/97	COMET	Added "titleIndex" to keep track of title line,
 *			"model_label" for model name and "field_label"
 *			for the model field (or label from FDF).
 *	12/97	COMET	Fixed small leaks (line_color, refveci, symbol_type,
 *			symbol_headsize), fixed	leaks related to builddirpath()
 *	12/97	COMET	Added a "select_list" object to track date/time
 *			selections when time matching is turned off.
 *	 1/98	COMET	Added creation of DataTimeObject
 *	 8/99	COMET	Redefined FDF model directory.
 *
 ***********************************************************************/

#include "geminc.h"
#include "gemprm.h"
#include "guimacros.h"
#include "genglobs.h"
#include "winobj.h"
#include "mainwin.h"
#include "thgtobj.h"
#include "fdfobj.h"
#include "utils.h"
#include "ctbcmn.h"
#include "command.h"
#include "_proto.h"


char * BuildGemParm ( char *, KeyListType * );

void THBuildScrolledLists ( char *model );
void BuildTHGrid();


void 
TimeHeightIconCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
/*
 *  This function is called when the user clicks on the time/height
 *  icon or menu item.
 */
{
	GuiTimeHeightDialogType	*tht;

	char			*model;
	int			verbose;

	XmPushButtonCallbackStruct *call_data = 
			(XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("TimeHeightIconCB\n");

/*
 *	Get dialog structure.
 */
	tht = GetGuiTimeHeightDialog();
	
/*
 *	Get model type.
 */
	model = GetModelTypeTH ( tht );

/*
 *	Dynamically build dialog scrolled lists.
 */
	THBuildScrolledLists ( model );

}


void
THBuildScrolledLists ( char *model )
/*
 *  This function is called when the user clicks on the time/height
 *  icon or menu item.
 */
{
	Widget			forecast_list, scalar_list, vector_list;

	GuiTimeHeightDialogType	*tht;
	WindowObjectType	*wo;

	char			*scalardir, *vectordir;
	char			*scalarchapter, *vectorchapter;
	int			verbose;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("THBuildScrolledLists\n");

	if ( model == NULL ) return;

/*
 *	Get dialog structure.
 */
	tht = GetGuiTimeHeightDialog();
	wo = GetActiveWindowObject();
	
/*
 *	Enguage busy cursor.
 */
	SetupAbortProcess( wo );

/*
 *	Get model type.
 */
	scalardir = GetConfigValue( "thscalarfdf" );
	vectordir = GetConfigValue( "thvectorfdf" );
	scalarchapter = strdup ( GetModelTHScalarDir ( tht ) );
	vectorchapter = strdup ( GetModelTHVectorDir ( tht ) );

/*
 *	Save parent widget id.
 */
	forecast_list = GetModelScrolledListTHW( tht );
	scalar_list = GetScalarListTHW( tht );
	vector_list = GetVectorListTHW( tht );

/*
 *	Unmanage widgets.
 */
	XtUnmanageChild ( forecast_list );
	XtUnmanageChild ( scalar_list );
	XtUnmanageChild ( vector_list );

/*
 *	Clear scrolled list.
 */
	RemoveProductsFromList ( forecast_list );

/*
 *	Get date/times and files.
 */
	BuildModelAnalysisList ( forecast_list,
			         model,
			         &(tht->time_list),
			         &(tht->select_list) );

	BuildFieldList ( scalar_list, scalardir, scalarchapter, model,
	                                               &(tht->scalar_list) );

	BuildFieldList ( vector_list, vectordir, vectorchapter, model,
	                                               &(tht->vector_list) );
/*
 *	Highlight the last item in the list.
 */
/*
	HighLightList ( forecast_list, TIMEHEIGHTOBJECT,
	                 &(tht->time_list), &(tht->match_list) );
*/

/*
 *	Manage widgets.
 */
	XtManageChild ( forecast_list );
	XtManageChild ( scalar_list );
	XtManageChild ( vector_list );

/*
 *	Cancel busy cursor.
 */
	CancelAbortProcess( wo, False );

/*
 *	Free.
 */
	Free ( scalardir );
	Free ( vectordir );
	Free ( scalarchapter );
	Free ( vectorchapter );
}


void
GetTHModelField ( char *file, char *subdir, char *directory )
{
/*
 * Read in a macro or FDF. Update graphical user interface.
 */
	GuiTimeHeightDialogType	*tht;
	KeyListType		*fdf;


	char			*model;
	int			err, verbose;

	
	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "GetTHModelField\n" );

	tht = GetGuiTimeHeightDialog();
	fdf = GetTHGuiFdfInfo();

	model = strdup ( GetModelTypeTH ( tht ) );

/*
 *	Sanity test.
 */
	if ( file == NULL ) {
	    printf("GetTHModelField - file is NULL\n" );
	    return;
	}
	if ( subdir == NULL ) {
	    printf("GetTHModelField - chapter is NULL\n" );
	    return;
	}
	if ( directory == NULL ) {
	    printf("GetTHModelField - directory is NULL\n" );
	    return;
	}

/*
 *	Get and save FDF.
 */
	err = GetFDF ( file, directory, subdir, model, fdf );

/*
 *	Check for a macro.
 */
	if ( !err ) {
	    strcpy ( fdf->fileName, file );
	    strcpy ( fdf->path, directory );
	    if ( CheckForMacro ( fdf ) )
		SetTHMacro ( fdf, file );
	    else {
/*
 *	Update GUI.
 */
	    	SetTHFdfFallbackValues ( fdf );
	    	SetTHGridWidgets ( fdf, file );
	    }
	}
/*
 *	Free.
 */
	Free ( model );
}


void
GetTHModelFieldCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	Widget			active_list;
	Widget			inactive_list;

	GuiTimeHeightDialogType	*tht;
	ListParentObjectType	*lpo;


	char			*file, *chapter, *directory;
	int			*index, count;
	int			verbose;
	int			type;

	XmListCallbackStruct *cbs = 
			(XmListCallbackStruct *) xt_call_data;

	type = (int) client_data;

	
	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "GetTHModelFieldCB\n" );

	tht = GetGuiTimeHeightDialog();

/*
 *	Get handles to widgets for this plot type.
 */
	if      ( type == SCALARGRIDOBJECT ) {
	    chapter = strdup ( GetModelTHScalarDir ( tht ) );
	    directory = GetConfigValue( "thscalarfdf" );
	    active_list   = GetScalarListTHW ( tht );
	    inactive_list = GetVectorListTHW ( tht );
	    lpo = &(tht->scalar_list);
	}
	else if ( type == VECTORGRIDOBJECT ) {
	    chapter = strdup ( GetModelTHVectorDir ( tht ) );
	    directory = GetConfigValue( "thvectorfdf" );
	    active_list   = GetVectorListTHW ( tht );
	    inactive_list = GetScalarListTHW ( tht );
	    lpo = &(tht->vector_list);
	}
/*
 *	Set plot type.
 */
	SetTHModelGridTypeCB ( w, (XtPointer) type, (XtPointer) NULL );

/*
 *	Deselect field item.
 */
	if ( XmListGetSelectedPos ( inactive_list, &index, &count ) ) {
	    XtUnmanageChild ( inactive_list );
	    XmListDeselectPos ( inactive_list, *index );
	    XtManageChild ( inactive_list );
	    Free ( index );
	}

/*
 *	Get index of chosen field.
 */
	XmListGetSelectedPos ( lpo->w, &index, &count );
	if ( count <= 0 ) {
	    printf ( "  GetTHModelFieldCB - nothing selected ... returning\n");
	    return;
	}

/*
 *	Append FDF file name to path.
 */
	file = strdup ( lpo->label[*index-1] );
	Free (index );

	if ( directory == NULL ) {
	    printf ( "*** GetTHModelFieldCB - this fdf key is undefined\n" );
	    return;
	}

	GetTHModelField ( file, chapter, directory );

/*
 *	Free.
 */
	Free ( file );
	Free ( directory );
	Free ( chapter );
}


void
TimeHeightCB (	Widget w, 
		XtPointer client_data,
		XtPointer call_data )
{
/*
 *  This function is called when the user clicks on "Display" from the
 *  time/height cross section dialog.
 */
	GuiTimeHeightDialogType	*tht;
	KeyListType		*fdf;

	char			*path, *directory;
	int			plot_type;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;


	if( GetVerboseLevel() > VERBOSE_0 )
	    printf ( "TimeHeightCB\n" );

	tht = GetGuiTimeHeightDialog();
	fdf = GetTHGuiFdfInfo();

/*
 *	Get directory information.
 */
	plot_type = GetTHGridType ( tht );

	switch ( plot_type ) {
	case SCALARGRIDOBJECT:
	  directory = GetConfigValue( "thscalarfdf" );
	  break;
	case VECTORGRIDOBJECT:
	  directory = GetConfigValue( "thvectorfdf" ); 
	  break;
	default:
	  break;
	}


/*
 *	If the current FDF is actually a macro, step thorough it.
 */
	if ( CheckForMacro ( fdf ) ) {

	    RunTHMacro ( directory );

/*
 *	Restore plot type and current directory setting.
 */
	    SetTHGridType ( tht, plot_type );

	}
	else {
/*
 *	Just a regular FDF.
 */
	    BuildTHGrid();

	}

	if ( directory != NULL ) Free ( directory );
}


void
BuildTHGrid()
/*
 * Build metObjects for time/height cross section dialog.
 */
{
	extern Widget		main_shell;

	MetObjectType		*mlist[MAXMETOBJS];
	GuiTimeHeightDialogType *tht;
	WindowObjectType	*wo;
	KeyListType		*fdf;
	GlobalDataObjectType	*gd;
	GeoRefObjectType	*geo;

	char 			*file       , *ftime        , *vcoord,
				*field      , *point        , *vector,
				*ptype      , *refvec;

	char			*wind_symbol , *symbol_size ,
				*symbol_width, *symbol_type ,
				*symbol_headsize;

	char			*line_color  ,
				*line_type   , *line_width  ,
				*label_freq;

	char			*cint       , *cmin         , *cmax ,
				*level1     , *level2       , *taxis,
				*scale      , *text	    , *model,
				*yaxis;
        char                    *model_label, *field_label, *buf;

	char			dattim[80];

	int			plot_type, viewType;
	int			i, j;
	int			sw_count, *sw_index, int_duration, nfiles=0;
	int			int_label;
	int			iret, titleIndex, verbose;

	
	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "BuildTHGrid\n" );

	tht = GetGuiTimeHeightDialog();
	gd  = GetGlobalDataObject();
	fdf = GetTHGuiFdfInfo();
	wo  = GetActiveWindowObject();

/*
 *	Get highlighted position indices from list widget.
 */
	XmListGetSelectedPos ( GetModelScrolledListTHW( tht ), 
			       &sw_index, &sw_count );
	if ( sw_count <= 0 ) {
	    InfoPop ( main_shell, INFORMATION,
	    "\nSorry boss, you need to choose\nat least one date/time stamp.\n",
	    FALSE, FALSE, NULL, NULL );

	    return;
	}

/*
 *	File Selection widgets count starting at 1.
 */
	AdjustIndexOffset ( sw_count, sw_index );
	file    =       strdup ( tht->time_list.string[sw_index[0]] );

/*
 *	Save the first time in the list for selection purposes.
 */
	if ( sw_count > 0 ) {
	    if ( tht->select_list.listcount > 0 )
                Free ( tht->select_list.intList );

	    tht->select_list.listcount = sw_count;
	    tht->select_list.intList =
                 malloc ( sizeof(int) * tht->select_list.listcount );

	    for ( i=0; i<tht->select_list.listcount; i++ )
                tht->select_list.intList[i] = sw_index[i];

	    PreviousSelectedTime ( "SET", TIMEHEIGHTOBJECT,
	                            tht->time_list.label[sw_index[0]] );
	}
/*
 *	Check on the view types for compatibility. Compare this object 
 *	with the current georeference view state and do a reset if
 *	they don't match.
 */
	viewType  = GetMetObjectView( TIMEHEIGHTOBJECT );


	if ( geo = GetGeoRefObject( wo) )
		if ( geo->viewType != viewType ) WindowObjectFullReset( wo );

/*
 *	Vertical coordinate.
 */
 	if ( buf = XmTextGetString ( GetModelVCoordTHW ( tht ) ) ) {
	    vcoord = strdup ( buf );
	    XtFree ( buf );
	    FreeModelVCoordTH ( tht );
	    SetModelVCoordTH ( tht, vcoord );
	}
	else
	    vcoord = strdup (" ");

/*
 *	Get field label.
 */
	field_label = GetFdfKeyValue ( fdf, "label" );

/*
 *	Get FDF from text widget.
 */
	plot_type = GetTHGridType ( tht );
	if      ( plot_type == SCALARGRIDOBJECT ) {
	    vector = strdup (" ");
	    if ( buf = XmTextGetString ( GetFunctionTHW ( tht ) ) ) {
		field = strdup ( buf );
	    	if ( field_label == NULL ) field_label = strdup ( field );
		XtFree ( buf );
	    }
	    else
		field = strdup (" ");
	}

	else if ( plot_type == VECTORGRIDOBJECT ) {
	    field = strdup (" ");
	    if ( buf = XmTextGetString ( GetFunctionTHW ( tht ) ) ) {
		vector = strdup ( buf );
	    	if ( field_label == NULL ) field_label = strdup ( vector );
		XtFree ( buf );
	    }
	    else
		vector = strdup (" ");
	}

/*
 *	Position of time height section.
 */
	if ( buf = XmTextGetString ( GetStationTHW ( tht ) ) ) {
	    point = strdup ( buf );
	    XtFree ( buf );
	}
	else
	    point = strdup (" ");
	    
/*
 *	Get contour interval, minimum and maximum.
 */
	if ( buf = XmTextGetString ( GetContourIntervalTHW ( tht ) ) ) {
	    cint = strdup ( buf );
	    XtFree ( buf );
	    FreeContourIntervalTH ( tht );
	    SetContourIntervalTH ( tht, cint );
	}
	else
	    cint = strdup ( "0" );
	
	if ( buf = XmTextGetString ( GetContourMinimumTHW ( tht ) ) ) {
	    cmin = strdup ( buf );
	    XtFree ( buf );
	    FreeContourMinimumTH ( tht );
	    SetContourMinimumTH ( tht, cmin );
	}
	else
	    cmin = strdup ( "0" );

	if ( buf = XmTextGetString ( GetContourMaximumTHW ( tht ) ) ) {
	    cmax = strdup ( buf );
	    XtFree ( buf );
	    FreeContourMaximumTH ( tht );
	    SetContourMaximumTH ( tht, cmax );
	}
	else
	    cmax = strdup ( "0" );

/*
 *	Get contour line characteristics.
 */
	line_color = strdup ( GetLineColorTH ( tht ) );

	if ( strcmp ( line_color, "NONE" ) == 0 ) {
	    GetNextObjectColor ( gd );
	    Free ( line_color );
	    line_color = strdup ( gd->colors);
	}
	else
	    SetColorAsUsed ( line_color );


	line_type  = strdup ( GetLineTypeTH ( tht ) );
	line_width = strdup ( GetLineWidthTH ( tht ) );
	label_freq = strdup ( GetLabelFrequencyTH ( tht ) );

/*
 *	Set text size, font, text width and hardware/software flag.
 */
	text = BuildGemParm ( "text", fdf );
	
/*
 *	Scale factor.
 */
	if ( buf = XmTextGetString ( GetScaleFactorTHW ( tht ) ) ) {
	    scale = strdup ( buf );
	    XtFree ( buf );
	}
	else
	    scale = strdup ( "9999" );

/* 
 *	Wind, Refvec.
 */
	wind_symbol = strdup ( GetVectorSymbolTH ( tht ) );
	symbol_size = GetFdfKeyValue ( fdf, "wind_symbol_size" );
	symbol_width = GetFdfKeyValue ( fdf, "wind_symbol_width" );
	symbol_type = GetFdfKeyValue ( fdf, "wind_symbol_type" );
	symbol_headsize = GetFdfKeyValue ( fdf, "wind_symbol_headsize" );

	refvec = BuildGemParm ( "refvec", fdf );

/*
 *	Yaxis, axis type.
 */
        if ( buf = XmTextGetString ( GetLevel1THW ( tht ) ) ) {
	    level1 = strdup ( buf );
	    XtFree ( buf );
	    FreeLevel1TH ( tht );
            SetLevel1TH ( tht, level1 );
	}
	else
	    level1 = strdup (" ");

	if ( buf = XmTextGetString ( GetLevel2THW ( tht ) ) ) {
	    level2 = strdup ( buf );
	    XtFree ( buf );
	    FreeLevel2TH ( tht );
	    SetLevel2TH ( tht, level2 );
	}
	else
	    level2 = strdup (" ");

	PutInFDF ( "yaxis_lower", level1, fdf, OVERWRITE );
	PutInFDF ( "yaxis_upper", level2, fdf, OVERWRITE );
	yaxis = BuildGemParm ( "yaxis", fdf );

	PutInFDF ( "ptype_axis", GetAxisTypeTH ( tht ), fdf, OVERWRITE );
	ptype = BuildGemParm ( "ptype", fdf );

/*
 *	Time axis.
 */
	PutInFDF ( "taxis_dir", GetTimeAxisTH ( tht ), fdf, OVERWRITE );
	taxis = BuildGemParm ( "taxis", fdf );

/*
 *	Model type and label.
 */
	model       = strdup ( GetModelTypeTH ( tht ) );
	model_label = strdup ( GetModelLabelTypeTH ( tht ) );

/*
 *	Save title line index.
 */
	titleIndex = GetNextTitleIndex ( wo );

/*
 *	Forecast time.
 */
	ftime = strdup ( tht->time_list.label[sw_index[0]] );

/*
 *	Loop over stations. Loop over number of files.
 */
sw_count=1;
	for ( i=0; i<sw_count; i++ ) {
	    mlist[i] = (MetObjectType *)
		MakeTimeHeightObject ( file,        ftime,
                                       vcoord,      field,
                                       point,       vector,
                                       ptype,       taxis,
                                       yaxis,       wind_symbol,
                                       symbol_size, symbol_width,
				       symbol_type, symbol_headsize,
                                       line_color,  line_type,
                                       line_width,  label_freq,
                                       cint,        cmin,
                                       cmax,        scale,
                                       text,        model,
	                               model_label, field_label,
                                       titleIndex );
	}
/*
 *	Free.
 */
	Free ( file );
	Free ( ftime );
	Free ( vcoord );
	Free ( field );
	Free ( point );
	Free ( vector );
	Free ( refvec );
	Free ( ptype );
	Free ( wind_symbol );
	Free ( symbol_size );
	Free ( symbol_width );
	Free ( symbol_type );
	Free ( symbol_headsize );
	Free ( line_color );
	Free ( line_type );
	Free ( line_width );
	Free ( label_freq );
	Free ( cint );
	Free ( cmin );
	Free ( cmax );
	Free ( scale );
	Free ( text );
	Free ( level1 );
	Free ( level2 );
	Free ( taxis );
	Free ( yaxis );
	Free ( model );
	Free ( model_label );
	Free ( field_label );
	Free ( sw_index );

	AddObjectListToDisplay( sw_count, mlist );

/*
 *      Create a DataTimeObject for time matching.
 */
        if ( wo && sw_count > 0 )
            CreateDataTimeObject ( wo, mlist );

	if( wo )
		ProcessDisplayList( wo );

	return;	
}

