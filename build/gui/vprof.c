/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	vprof.c
 *
 *	Callbacks and utilities for vertical profile data.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 3/97	COMET	Added wind position indicator.
 *	 6/97	COMET	Removed imgobj.h
 *	 7/97	COMET	Added model defined subdirectories.
 *	 9/97	COMET	Added model to object (for autoupdate)
 *	10/97	COMET	Added macro functionality.
 *	11/97	COMET	Add call to SetTimeListLabel()
 *	11/97	COMET	Added "titleIndex" to keep track of title line,
 *			"model_label" for model name and "field_label"
 *			for the model field (or label from FDF).
 *	12/97	COMET	Fixed small leak, and leaks related to
 *			builddirpath()
 *	12/97	COMET	Remove SetTimeListLabel and add args to BuildModel-
 *			ForecastList(). Added creation of DataTimeObject
 *			for time matching.
 *	12/97	COMET	Added call to RefreshScrolledTimeList.
 *	12/97	COMET	Added a "select_list" object to track date/time
 *			selections when time matching is turned off.
 *	 1/98	COMET	Use "select_list" for time matching. Removed
 *			strdup from GetNextWindPosition due to function 
 *			change.
 *	 8/99	COMET	Redefined FDF model directory.
 *
 ***********************************************************************/

#include "geminc.h"
#include "gemprm.h"
#include "guimacros.h"
#include "genglobs.h"
#include "winobj.h"
#include "mainwin.h"
#include "vprofobj.h"
#include "fdfobj.h"
#include "utils.h"
#include "ctbcmn.h"
#include "command.h"
#include "_proto.h"


char * BuildGemParm ( char *, KeyListType * );
char * GetNextWindPostition ( GlobalDataObjectType * );

void VPBuildScrolledLists ( char *model );
void BuildVPGrid();

ListParentObjectType * 
ReturnListStruct ( Widget, ListParentObjectType,
                   ListIndexObjectType, ListIndexObjectType * );


void
VerticalProfileIconCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
/*
 *  This function is called when the user clicks on the vertical profile
 *  icon or menu item.
 */
{
	GuiVertProfileObjectType	*vpt;

	char				*model;
	int				verbose;

	XmPushButtonCallbackStruct *call_data = 
			(XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("VerticalProfileIconCB\n");

/*
 *	Get dialog structure.
 */
	vpt = GetGuiVertProfileDialog();
	
/*
 *	Get model type.
 */
	model = GetModelTypeVP ( vpt );

/*
 *	Dynamically build dialog.
 */
	VPBuildScrolledLists ( model );

}


void
VPBuildScrolledLists ( char *model )
/*
 * Fill in date/time and FDF scrolled lists.
 */
{
	Widget				forecast_list, scalar_list, vector_list;

	GuiVertProfileObjectType	*vpt;
	WindowObjectType		*wo;

	char				*scalardir, *vectordir;
	char				*scalarchapter, *vectorchapter;
	int				verbose;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("VPBuildScrolledLists\n");

	if ( model == NULL ) return;

/*
 *	Get dialog structure.
 */
	vpt = GetGuiVertProfileDialog();
	wo = GetActiveWindowObject();
	
/*
 *	Enguage busy cursor.
 */
	SetupAbortProcess( wo );

/*
 *	Get model type.
 */
	scalardir = GetConfigValue( "vpscalarfdf" );
	vectordir = GetConfigValue( "vpvectorfdf" );
	scalarchapter = strdup ( GetModelVPScalarDir ( vpt ) );
	vectorchapter = strdup ( GetModelVPVectorDir ( vpt ) );

/*
 *	Save parent widget id.
 */
	forecast_list = GetModelScrolledListVPW( vpt );
	scalar_list = GetScalarListVPW( vpt );
	vector_list = GetVectorListVPW( vpt );

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
	BuildModelForecastList ( VERTICALPROFILEOBJECT,
				 forecast_list,
				 GetModelVPTimeListLabelW(vpt),
			         model,
			         &(vpt->time_list),
				 &(vpt->match_list),
				 &(vpt->select_list) );


	BuildFieldList ( scalar_list, scalardir, scalarchapter, model,
	                                            &(vpt->scalar_list) );

	BuildFieldList ( vector_list, vectordir, vectorchapter, model,
	                                            &(vpt->vector_list) );


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
GetVPModelField ( char *file, char *subdir, char *directory )
{
	GuiVertProfileObjectType	*vpt;
	KeyListType			*dl, *kl, *fdf;


	char			*model;
	int			err, verbose;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "GetVPModelField\n" );

	vpt = GetGuiVertProfileDialog();
	fdf = GetVPGuiFdfInfo();

	model = strdup ( GetModelTypeVP ( vpt ) );

/*
 *	Sanity test.
 */
	if ( file == NULL ) {
	    printf("GetVPModelField - file is NULL\n" );
	    return;
	}
	if ( subdir == NULL ) {
	    printf("GetVPModelField - chapter is NULL\n" );
	    return;
	}
	if ( directory == NULL ) {
	    printf("GetVPModelField - directory is NULL\n" );
	    return;
	}

/*
 *	Get and save FDF.
 */
	err = GetFDF ( file, directory, subdir, model, fdf );

/*
 *	Check for macro.
 */
	if ( !err ) {
	    strcpy ( fdf->fileName, file );
	    strcpy ( fdf->path, directory );
	    if ( CheckForMacro ( fdf ) )
		SetVPMacro ( fdf, file );
	    else {
/*
 *	    Update GUI.
 */
	    	SetVPFdfFallbackValues ( fdf );
	    	SetVPGridWidgets ( fdf, file );
	    }
	}
/*
 *	Free.
 */
	Free ( model );
}


void
GetVPModelFieldCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	Widget			active_list;
	Widget			inactive_list;

	GuiVertProfileObjectType	*vpt;
	ListParentObjectType		*lpo;


	char			*file, *chapter, *directory;
	int			*index, count;
	int			verbose;
	int			type;

	XmListCallbackStruct *cbs = 
			(XmListCallbackStruct *) xt_call_data;

	type = (int) client_data;

	
	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "GetVPModelFieldCB\n" );

	vpt = GetGuiVertProfileDialog();

/*
 *	Get handles to widgets for this plot type.
 */
	if      ( type == SCALARGRIDOBJECT ) {
	    chapter = strdup ( GetModelVPScalarDir ( vpt ) );
	    directory = GetConfigValue( "vpscalarfdf" );
	    active_list   = GetScalarListVPW ( vpt );
	    inactive_list = GetVectorListVPW ( vpt );
	    lpo = &(vpt->scalar_list);
	}
	else if ( type == VECTORGRIDOBJECT ) {
	    chapter = strdup ( GetModelVPVectorDir ( vpt ) );
	    directory = GetConfigValue( "vpvectorfdf" );
	    active_list   = GetVectorListVPW ( vpt );
	    inactive_list = GetScalarListVPW ( vpt );
	    lpo = &(vpt->vector_list);
	}
/*
 *	Set plot type.
 */
	SetVPModelGridTypeCB ( w, (XtPointer) type, (XtPointer) NULL );

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
	    printf ( "  GetVPModelFieldCB - nothing selected ... returning\n");
	    return;
	}

/*
 *	Append FDF file name to path.
 */
	file = strdup ( lpo->label[*index-1] );
	Free (index );

	GetVPModelField ( file, chapter, directory );

/*
 *	Free.
 */
	Free ( file );
	Free ( directory );
	Free ( chapter );
}


void
VerticalProfileCB ( Widget w, 
		    XtPointer client_data,
		    XtPointer call_data )
{
/*
 *  This function is called when the user clicks on "Display" from the
 *  vertical profile dialog.
 */
	GuiVertProfileObjectType	*vpt;
	KeyListType			*fdf;

	char			*path, *directory;
	int			plot_type;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;


	if( GetVerboseLevel() > VERBOSE_0 )
	    printf ( "VerticalProfileCB\n" );

	vpt = GetGuiVertProfileDialog();
	fdf = GetVPGuiFdfInfo();

/*
 *	Get directory information.
 */
	plot_type = GetVPGridType ( vpt );

	switch ( plot_type ) {
	case SCALARGRIDOBJECT:
	  directory = GetConfigValue( "vpscalarfdf" );
	  break;
	case VECTORGRIDOBJECT:
	  directory = GetConfigValue( "vpvectorfdf" ); 
	  break;
	default:
	  break;
	}


/*
 *	If the current FDF is actually a macro, step thorough it.
 */
	if ( CheckForMacro ( fdf ) ) {

	    RunVPMacro ( directory );

/*
 *	Restore plot type and current directory setting.
 */
	    SetVPGridType ( vpt, plot_type );

	}
	else {
/*
 *	Just a regular FDF.
 */
	    BuildVPGrid();

	}

/*
 *	Refresh the time list if time matching
 */
	RefreshScrolledTimeLists ( True );

	if ( directory != NULL ) Free ( directory );
}


void
BuildVPGrid ()
/*
 * Build metObjects for vertical profile plot.
 */
{
	extern Widget			main_shell;

	MetObjectType			*mlist[MAXMETOBJS];
	GuiVertProfileObjectType	*vpt;
	WindowObjectType		*wo;
	KeyListType			*fdf;
	GlobalDataObjectType		*gd;
	GeoRefObjectType		*geo;

	ListParentObjectType	*inList;
	char 			*file       , *ftime        , *vcoord,
				*field      , *point        , *vector,
				*ptype      , *refvec       , *filter,
				*thtaln     , *thteln       , *mixrln;

	char			*winpos,       *wind_symbol , *symbol_size ,
				*symbol_width, *symbol_type ,
				*symbol_headsize;

	char			*line_color  , *model,
				*line_type   , *line_width;

	char			*level1,     *level2,     *xaxis,
				*yaxis,      *scale,      *text;

	char			dattim[80];
        char                    *model_label, *field_label, *buf;

	int			plot_type, viewType;
	int			i, j;
	int			count, int_duration, nfiles=0;
	int			int_label, titleIndex;
	int			iret, verbose;

	
	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "BuildVPGrid\n" );

	vpt = GetGuiVertProfileDialog();
	fdf = GetVPGuiFdfInfo();
	wo = GetActiveWindowObject();
	gd = GetGlobalDataObject();

/*
 *	Get highlighted position indices from list widget.
 */

	inList = ReturnListStruct ( GetModelScrolledListVPW( vpt ),
	                            vpt->time_list,
	                            vpt->match_list,
	                            &(vpt->select_list) );
	if ( inList == NULL ) return;
	count = inList->listcount;

/*
 *	Save the first time in the list for selection purposes.
 */
	if ( vpt->select_list.listcount > 0 )
	    PreviousSelectedTime ( "SET", VERTICALPROFILEOBJECT,
	                            inList->label[0] );

/*
 *	Check on the view types for compatibility. Compare this object 
 *	with the current georeference view state and do a reset if
 *	they don't match.
 */
	viewType  = GetMetObjectView( VERTICALPROFILEOBJECT );


	if ( geo = GetGeoRefObject( wo ) )
		if ( geo->viewType != viewType ) WindowObjectFullReset( wo );

/*
 *	Vertical coordinate.
 */
 	if ( buf = XmTextGetString ( GetModelVCoordVPW ( vpt ) ) ) {
	    vcoord = strdup ( buf );
	    XtFree ( buf );
	    FreeModelVCoordVP ( vpt );
	    SetModelVCoordVP ( vpt, vcoord );
	}
	else
	    vcoord = strdup (" ");


/*
 *	Get field label.
 */
	field_label = GetFdfKeyValue ( fdf, "label" );

/*
 *	Set as scalar or vector.
 */
	plot_type = GetVPGridType ( vpt );
	if      ( plot_type == SCALARGRIDOBJECT ) {
	    vector = strdup (" ");
	    if ( buf = XmTextGetString ( GetFunctionVPW ( vpt ) ) ) {
		field = strdup ( buf );
	    	if ( field_label == NULL ) field_label = strdup ( field );
		XtFree ( buf );
	    }
	    else {
		field = strdup (" ");
		field_label = strdup (" ");
	    }
	}
	else if ( plot_type == VECTORGRIDOBJECT ) {
	    field = strdup (" ");
	    if ( buf = XmTextGetString ( GetFunctionVPW ( vpt ) ) ) {
		vector = strdup ( buf );
	    	if ( field_label == NULL ) field_label = strdup ( vector );
		XtFree ( buf );
	    }
	    else {
		vector = strdup (" ");
		field_label = strdup (" ");
	    }
	}

/*
 *	Position of vertical profile.
 */
	if ( buf = XmTextGetString ( GetStationVPW ( vpt ) ) ) {
	    point = strdup ( buf );
	    XtFree ( buf );
	}
	else
	    point = strdup (" ");
	    
/*
 *	Get contour line characteristics.
 */
	line_color = strdup ( GetLineColorVP ( vpt ) );

	if ( strcmp ( line_color, "NONE" ) == 0 ) {
	    GetNextObjectColor ( gd );
	    Free ( line_color );
	    line_color = strdup ( gd->colors);
	}
	else
	    SetColorAsUsed ( line_color );

	line_type  = strdup ( GetLineTypeVP ( vpt ) );
	line_width = strdup ( GetLineWidthVP ( vpt ) );
/*
 *	Set text size, font, text width and hardware/software flag.
 */
	text = BuildGemParm ( "text", fdf );

/*
 *	Yaxis.
 */
        if ( buf = XmTextGetString ( GetLevel1VPW ( vpt ) ) ) {
	    level1 = strdup ( buf );
	    XtFree ( buf );
	    FreeLevel1VP ( vpt );
            SetLevel1VP ( vpt, level1 );
	}
	else
	    level1 = strdup (" ");

	if ( buf = XmTextGetString ( GetLevel2VPW ( vpt ) ) ) {
	    level2 = strdup ( buf );
	    XtFree ( buf );
	    FreeLevel2VP ( vpt );
	    SetLevel2VP ( vpt, level2 );
	}
	else
	    level2 = strdup (" ");

	PutInFDF ( "yaxis_lower", level1, fdf, OVERWRITE );
	PutInFDF ( "yaxis_upper", level2, fdf, OVERWRITE );
	yaxis = BuildGemParm ( "yaxis", fdf );

/*
 *	In lieu of macros, set sounding parameters in a function.
 */
	if ( strstr ( GetAxisTypeVP ( vpt ), "skewt" ) ||
	     strstr ( GetAxisTypeVP ( vpt ), "stuve"  ) ) {

	    Free ( field_label );
/*
 *	    Hardwire.
 */
	    scale = strdup (" ");
	    wind_symbol = strdup ("bk");
	    symbol_size = strdup ("1");
	    symbol_width = strdup ("1");
	    symbol_type = strdup ("111");
	    symbol_headsize = strdup ("1");
	    refvec = strdup (" ");

	    if ( strstr ( GetAxisTypeVP ( vpt ), "stuve" ) ) {
	        field_label = strdup ( "Stuve" );
	        ptype = strdup ("stuve");
	        xaxis = GetConfigValue( "StuveXAxis" );
	        if ( xaxis == NULL ) xaxis = strdup (" ");
	    }

	    else if ( strstr ( GetAxisTypeVP ( vpt ), "skewt" ) ) {
	        field_label = strdup ( "Skew-T" );
	        ptype = strdup ("skewt");
	        xaxis = GetConfigValue( "SkewtXAxis" );
	        if ( xaxis == NULL ) xaxis = strdup (" ");
	    }

	    else {
	        field_label = strdup ( "Skew-T" );
	        ptype = strdup ("skewt");
	        xaxis = GetConfigValue( "SkewtXAxis" );
	        if ( xaxis == NULL ) xaxis = strdup (" ");
	    }

	    thtaln=strdup("12/3/1");
	    thteln=strdup("15/1/1");
	    mixrln=strdup("5/10/2");
	    filter=strdup("0");

	}

	else {
/*
 *	    Scale factor.
 */
	    scale = BuildGemParm ( "scale", fdf );

/* 
 *	    Wind, Refvec.
 */
	    wind_symbol = strdup ( GetVectorSymbolVP ( vpt ) );
	    symbol_size = GetFdfKeyValue ( fdf, "wind_symbol_size" );
	    symbol_width = GetFdfKeyValue ( fdf, "wind_symbol_width" );
	    symbol_type = GetFdfKeyValue ( fdf, "wind_symbol_type" );
	    symbol_headsize = GetFdfKeyValue ( fdf, "wind_symbol_headsize" );

	    refvec = BuildGemParm ( "refvec", fdf );

/*
 *	    Xaxis, plot type.
 */
	    xaxis = BuildGemParm ( "xaxis", fdf );

	    PutInFDF ( "ptype_axis", GetAxisTypeVP ( vpt ), fdf, OVERWRITE );
	    ptype = BuildGemParm ( "ptype", fdf );

/*
 *	    Presently hardwired.
 */
	    thtaln=strdup(" ");
	    thteln=strdup(" ");
	    mixrln=strdup(" ");
	    filter=strdup("0");

	}
/*
 *	Wind position.
 */
	winpos = GetNextWindPostition ( gd );

	if ( verbose > 2 ) {
	    printf ("  xaxis = %s\n", xaxis );
	    printf ("  yaxis = %s\n", yaxis );
	}

/*
 *	Get model type and label.
 */
	model       = strdup ( GetModelTypeVP ( vpt ) );
	model_label = strdup ( GetModelLabelTypeVP ( vpt ) );

/*
 *	Save title line index.
 */
	titleIndex = GetNextTitleIndex ( wo );

/*
 *	Loop over stations. Loop over number of files.
 */
	for ( i=0; i<count; i++ ) {

	    if ( inList->label[i] == NULL ) {
		file  = strdup ( "MISSING" );
		ftime = strdup ( "MISSING" );
	    }
	    else {
	        file  = strdup ( inList->string[i] );
	        ftime = strdup ( inList->label[i] );
	        strdelc ( ftime, ' ', &iret );
	    }

	    mlist[i] = (MetObjectType *)
		MakeVerticalProfileObject ( file,        ftime,
                                            vcoord,      field,
                                            point,       vector,
                                            ptype,       xaxis,
                                            yaxis,       refvec,
                                            thtaln,      thteln,
                                            mixrln,      filter,
                                            winpos,      wind_symbol,
                                            symbol_size, symbol_width,
                                            symbol_type, symbol_headsize,
                                            line_color,  line_type,
                                            line_width,  
                                            scale,       text, 
                                            model,       model_label,
					    field_label, titleIndex );
	    Free ( file );
	    Free ( ftime );
	}
/*
 *	Free.
 */
	Free ( vcoord );
	Free ( field );
	Free ( point );
	Free ( vector );
	Free ( refvec );
	Free ( ptype );
	Free ( filter );
	Free ( thtaln );
	Free ( thteln );
	Free ( mixrln );
	Free ( winpos );
	Free ( wind_symbol );
	Free ( symbol_headsize );
	Free ( symbol_size );
	Free ( symbol_width );
	Free ( symbol_type );
	Free ( line_color );
	Free ( line_type );
	Free ( line_width );
	Free ( scale );
	Free ( xaxis );
	Free ( yaxis );
	Free ( text );
	Free ( level1 );
	Free ( level2 );
	Free ( model );
	Free ( model_label );
	Free ( field_label );

	AddObjectListToDisplay( count, mlist );

/*
 *      Create a DataTimeObject for time matching.
 */
        if ( wo && count > 0 )
            CreateDataTimeObject ( wo, mlist );

	if( wo )
		ProcessDisplayList( wo );
/*
 *	Free.
 */
	FreeListParentObject ( inList );

	return;	
}

