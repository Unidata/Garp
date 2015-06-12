/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	vgridcb.c
 *
 *	Plan view vector gridded data driver callback routines.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 4/97	COMET	Added "scale" parameter for vector size.
 *	 7/97	COMET	Added model defined subdirectories.
 *	 8/97	COMET	Added time matching code.
 *	 9/97	COMET	Added model to object (for autoupdate)
 *	11/97	COMET	Added "titleIndex" to keep track of title line,
 *			"model_label" for model name and "field_label"
 *			for the model field (or label from FDF).
 *	12/97	COMET	Fix leak associated with builddirpath()
 *	12/97	COMET	Added a "select_list" object to track date/time
 *			selections when time matching is turned off.
 *	 2/98	COMET	Added call to parse the gfunc and use in-line
 *			modifiers from it.
 *	 3/98	COMET	Moved the gfunc parsing code elsewhere.
 *	 8/99	COMET	Redefined FDF model directory.
 *
 ***********************************************************************/

#include "geminc.h"
#include "gemprm.h"
#include "utils.h"
#include "ctbcmn.h"
#include "guimacros.h"
#include "fdfobj.h"
#include "hsobj.h"
#include "hvobj.h"
#include "model.h"
#include "genglobs.h"
#include "winobj.h"
#include "menucb.h"
#include "_proto.h"
#include "underscore.h"


char * BuildGemParm ( char *, KeyListType * );
char * BuildGemFunction ( char *, char *, KeyListType * );

int * MatchIndicesToList ( char **, int, int, int * );

ListParentObjectType *
ReturnListStruct ( Widget, ListParentObjectType,
	           ListIndexObjectType, ListIndexObjectType * );


void
GetModelVector ( char *file, char *subdir )
/*
 * Set Field Description and GVECT text field using Field Description file
 * as input.
 */
{
	GuiScalarGridDialogType	*grd;
	GuiVectorGridDialogType	*vec;
	ModelInfoType		*mdl;
	KeyListType		*fdf;

	char			*directory, *model;
	int			err, verbose;

	
	grd = GetGuiScalarGridDialog();
	vec = GetGuiVectorGridDialog();
	mdl = GetGuiModelInfo();
	fdf = GetHSGuiFdfInfo();

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "GetModelVector\n" );


	model = strdup ( GetModelType ( mdl ) );

/*
 *	Sanity test.
 */
	if ( ! ( directory && file && subdir ) ) {
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
		SetHSMacro ( fdf, file );
	    else {

/*
 *	Update GUI.
 */
	    	SetFdfFallbackValues ( fdf );
	    	SetGridWidgets ( fdf, file );
	    }
	}

/*
 *	Free.
 */
	Free ( directory );
	Free ( model );
}


void
GetModelVectorCB ( Widget w, XtPointer client_data, XtPointer xt_call_data )
/*
 * Callback for vector scrolled list.
 * Set Field Description and GVECT text field using Field Description file
 * as input.
 */
{
	Widget			scalar_list;

	GuiScalarGridDialogType	*grd;
	GuiVectorGridDialogType	*vec;

	char			*file, *chapter, *directory;
	int			*index, count;
	int			verbose;

	ListParentObjectType *lpo = 
			(ListParentObjectType *) client_data;

	
	grd = GetGuiScalarGridDialog();
	vec = GetGuiVectorGridDialog();

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "GetModelVectorCB\n" );

/*
 *	Set plot type to vector.
 */
	SetModelGridTypeCB ( w, (XtPointer) VECTORGRIDOBJECT,
	                        (XtPointer) NULL );

/*
 *	Deselect vector field item.
 */
	scalar_list = GetScrolledListHSW ( grd );
	if ( XmListGetSelectedPos ( scalar_list, &index, &count ) ) {
	    XtUnmanageChild ( scalar_list );
	    XmListDeselectPos ( scalar_list, *index );
	    XtManageChild ( scalar_list );
	    Free ( index );
	}

/*
 *	Get index of chosen field.
 */
	XmListGetSelectedPos ( lpo->w, &index, &count );
        if ( count <= 0 ) {
	    printf ( "GetModelVectorCB - nothing selected ... returning\n");
	    return;
        }

/*
 *	Read in model field.
 */
	file = strdup ( lpo->label[*index-1] );
	chapter = strdup ( GetModelVectorDir ( grd ) );
	Free (index);

	directory = GetConfigValue( "vectorfdf" );
	if ( directory == NULL ) {
	    printf ( "*** GetModelVectorCB - vectorfdf key is undefined\n" );
	    return;
	}

        GetModelField ( file, chapter, directory );

/*
 *	Free.
 */
	Free ( file );
	Free ( chapter );
	Free ( directory );
}


void
BuildVectorGrid ( GuiVectorGridDialogType *vec )
{

	MetObjectType		*mlist[MAXMETOBJS];
	WindowObjectType	*wo;
	GuiScalarGridDialogType *grd;
	ModelInfoType		*mdl;
	KeyListType		*fdf;
	GlobalDataObjectType	*gd;
	GeoRefObjectType	*geo;

	ListParentObjectType	*inList;
	char			**levels;
	char			*model,		*file,		*ftime,
				*level,		*field,		*vcoord,
				*refvec,	*text,		*scale,
				*skip,		*skip_x,	*skip_y;
	char			*wind_symbol,		*wind_symbol_color,
				*wind_symbol_size,	*wind_symbol_width,
				*wind_symbol_type,	*wind_symbol_headsize;
        char                    *model_label,		*field_label;
	char			*buf, gridtype[80];

	int			*indexList, titleIndex;
	int			nfunction;
	int			count, matchSize, metObjectCount;
	int			i, j, nlevels;
	int			iret, viewType, verbose;

	grd = GetGuiScalarGridDialog();
	mdl = GetGuiModelInfo();
	gd  = GetGlobalDataObject();
	wo  = GetActiveWindowObject();
	fdf = GetHSGuiFdfInfo();

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "BuildVectorGrid\n" );

	inList = ReturnListStruct ( GetModelScrolledListHSW( grd ),
	                            grd->time_list,
	                            grd->match_list,
	                            &(grd->select_list) );
	count = inList->listcount;

/*
 *	Save the first time in the list for selection purposes.
 */
	if ( grd->select_list.listcount > 0 )
	    PreviousSelectedTime ( "SET", VECTORGRIDOBJECT, inList->label[0] );

/*
 *	Check on the view types for compatibility. Compare this object 
 *	with the current georeference view state and do a reset if
 *	they don't match.
 */
	viewType  = GetMetObjectView( VECTORGRIDOBJECT );

	if ( geo = GetGeoRefObject( wo) )
		if ( geo->viewType != viewType ) WindowObjectFullReset( wo );

/*
 *	Get FDF from text widget.
 */
	if ( GetFunctionHS ( grd ) != NULL ) { 		/* fdf */
	    field = (char *) malloc ( GEMPAKSTRING * sizeof ( char ) );
	    strcpy ( field, GetFunctionHS ( grd ) );
	    bpad ( field, GEMPAKSTRING, &iret );
	    strcpy ( gridtype, " v" );
	}
	else {
	    ("*** Error in BuildVectorGrid - field in undefined\n" );
	    return;
	}

/*
 *	For complicated fields which require several descriptors to build
 *	the field, format the definition to pass to FORTRAN.
 */
	nfunction = GetNumFunctionHS ( grd );
	if ( nfunction  > 1 ) {
	    Free ( field );
	    field = BuildGemFunction ( "function", gridtype, fdf );
	}

/*
 *	Get the vertical coordinate
 */
	if ( buf = XmTextGetString ( GetModelVCoordHSW ( mdl ) ) ) {
	    vcoord = strdup ( buf );
	    XtFree ( buf );
	    ToLower ( vcoord );
	    FreeModelVCoord ( mdl );
	    SetModelVCoord ( mdl, vcoord );
	}
	else
	    vcoord = strdup (" ");

/*
 *	Get the level
 */
	if ( level = XmTextGetString ( GetModelLevel1HSW ( mdl ) ) ) {
	    FreeModelLevel1 ( mdl );
	    SetModelLevel1 ( mdl, level );
	    XtFree ( level );
	}

	if ( level = XmTextGetString ( GetModelLevel2HSW ( mdl ) ) ) {
	    FreeModelLevel2 ( mdl );
	    SetModelLevel2 ( mdl, level );
	    XtFree ( level );
	}
	level = BuildGemParm ( "level", fdf );

/*
 *	Check if animating vertically through all model levels or layers.
 */
	if ( strcasecmp ( level, "all_level" ) == 0 )
	    nlevels = GetModelLevels ( 
			GetModelType(mdl), GetModelVCoord (mdl), &levels );
	else if ( strcasecmp ( level, "all_layer" ) == 0 )
	    nlevels = GetModelLayers (
			GetModelType(mdl), GetModelVCoord (mdl), &levels );
	else {
	    nlevels = 1;
	    levels = StringListAllocate ( nlevels );
	    *levels = strdup ( level );
	}
	Free ( level );

/*
 *	Set wind symbol and color.
 */
	wind_symbol = strdup ( GetVectorSymbolHS ( grd ) );
	wind_symbol_color = GetFdfKeyValue ( fdf, "wind_symbol_color" );
	if ( wind_symbol_color == NULL ) {
	    GetNextObjectColor ( gd );
	    wind_symbol_color = strdup ( gd->colors );
	}

	wind_symbol_size  = GetFdfKeyValue ( fdf, "wind_symbol_size" );
	wind_symbol_width = GetFdfKeyValue ( fdf, "wind_symbol_width" );
	wind_symbol_type = GetFdfKeyValue ( fdf, "wind_symbol_type" );
	wind_symbol_headsize = GetFdfKeyValue ( fdf, "wind_symbol_headsize" );

/*
 *	Set reference vector parameters.
 */
	refvec = BuildGemParm ( "refvec", fdf );

/*
 *	Scale, Skip factor.
 */
	if ( buf = XmTextGetString ( GetScaleFactorHSW ( grd ) ) ) {
	    scale = strdup ( buf );
	    XtFree ( buf );
	}
	else
	    scale = strdup ( "9999" );

	skip   = GetFdfKeyValue ( fdf, "skip_contour" );
	skip_x = GetFdfKeyValue ( fdf, "skip_plot_x" );
	skip_y = GetFdfKeyValue ( fdf, "skip_plot_y" );

/*
 *	Set text size, font, text width and hardware/software flag.
 */
	text = BuildGemParm ( "text", fdf );

/*
 *	Get model
 */
	model       = strdup ( GetModelType ( mdl ) );
	model_label = strdup ( GetModelLabelType ( mdl ) );

/*
 *	Get field label.
 */
	field_label = GetFdfKeyValue ( fdf, "label" );
	if ( field_label == NULL ) field_label = strdup ( field );

/*
 *	Save title line index.
 */
	titleIndex = GetNextTitleIndex ( wo );

/*
 *	Create metObjects.
 */
	metObjectCount = 0;
	for ( i=0; i<count; i++ ) {     /* loop over times */
	    if ( inList->label[i] == NULL ) {
		file  = strdup ( "MISSING" );
		ftime = strdup ( "MISSING" );
	    }
	    else {
		file  = strdup ( inList->string[i] );
		ftime = strdup ( inList->label[i] );
		strdelc ( ftime, ' ', &iret );
	    }

	    for ( j=0; j<nlevels; j++ ) {	/* loop over levels */

                mlist[metObjectCount] = (MetObjectType *)
				MakeHorizontalVectorGridObject(
					file,		  ftime,
					vcoord,		  levels[j],
					field,		  gridtype,
					text,
					wind_symbol,	  wind_symbol_color, 
					wind_symbol_size, wind_symbol_width,
					wind_symbol_type, wind_symbol_headsize,
					refvec,           scale,
					skip,
					skip_x,		  skip_y,
					model, 		  model_label,
					field_label,      nfunction,
                                        titleIndex );
		metObjectCount++;
	    }

	    Free ( ftime );
	    Free ( file );
	}

	Free ( field );
	Free ( vcoord );
	Free ( wind_symbol );
	Free ( wind_symbol_color );
	Free ( wind_symbol_size );
	Free ( wind_symbol_width );
	Free ( wind_symbol_type );
	Free ( wind_symbol_headsize );
	Free ( refvec );
	Free ( text );
	Free ( scale );
	Free ( skip );
	Free ( skip_x );
	Free ( skip_y );
	Free ( model );
	Free ( model_label );
	Free ( field_label );
	StringListFree ( nlevels, levels );

	count = count * nlevels;
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
	if ( grd->match_list.listcount < 1 ) {
	    indexList = MatchIndicesToList (
			grd->time_list.label,
			grd->time_list.listcount,
			VECTORGRIDOBJECT, &matchSize );
	    grd->match_list.listcount = matchSize;
	    grd->match_list.intList = indexList;
	}

        if( wo )
                ProcessDisplayList( wo );
/*
 *	Free.
 */
	FreeListParentObject ( inList );

	return;
}
