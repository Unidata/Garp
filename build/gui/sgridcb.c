/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	sgridcb.c
 *
 *	Plan view scalar gridded data driver callback routines.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/97	COMET	Fixed vertical level popup management.
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 2/97	COMET	Removed level=0 setting if only one vertical level.
 *	 2/97	COMET	Use qsort() to sort vertical levels and layers.
 *	 3/97	COMET	Added scale factor text field. Clear pixmaps when
 *			adding color fill objects. 
 *	 7/97	COMET	Added model defined subdirectories.
 *	 8/97	COMET	Added time matching code.
 *	 8/97	COMET	Added macro capability. Moved some functions.
 *	 9/97	COMET	Added model to object (for autoupdate)
 *	11/97	COMET	Add call to SetTimeListLabel(), fix small leaks
 *	11/97	COMET	Added "titleIndex" to keep track of title line,
 *			"model_label" for model name and "field_label"
 *			for the model field (or label from FDF).
 *	12/97	COMET	Fixed possible leak with XmListGetSelectedPos,
 *			and leaks related to builddirpath()
 *	12/97	COMET	Remove SetTimeListLabel and add args to BuildModel-
 *			ForecastList()
 *	12/97	COMET	Added a "select_list" object to track date/time
 *			selections when time matching is turned off.
 *	 2/98	COMET	Rewrote much of SelectModelLevelCB to fix a bug 
 *			that caused some model level buttons to be hidden.
 *	 2/98	COMET	Added call to parse the gfunc and use in-line
 *			modifiers from it.
 *	 3/98	COMET	Moved the gfunc parsing code elsewhere.
 *       2/99	UNIDATA Fixed bug limiting number of levels displayed.
 *	 5/99	COMET	Fixed bug limiting number of layers displayed.
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


void GetModelVectorCB();
void GridDataCB();
void SetModelLevelCB( Widget, XtPointer, XtPointer );
void SetModelLayerCB( Widget, XtPointer, XtPointer );
void HSBuildScrolledLists ( char * );
void SetHSMacro ( KeyListType *, char * );

BooleanType CheckForMacro ( KeyListType * );

char * BuildGemParm ( char *, KeyListType * );
char * BuildGemFunction ( char *, char *, KeyListType * );

int * MatchIndicesToList ( char **, int, int, int * );

ListParentObjectType * 
ReturnListStruct ( Widget, ListParentObjectType,
                   ListIndexObjectType, ListIndexObjectType * );



static int
AlphaSortUp( char **a, char **b)
{
/*
 * Sort a string list from small values to large lexigraphically.
 */
        return (  strcmp ( *a, *b ) );
}


static int
AlphaSortDown( char **a, char **b)
{
/*
 * Sort a string list from large values to small lexigraphically.
 */
        return (  strcmp ( *b, *a ) );
}


static int
IntSmallToLarge( char **A, char **B )
{
/*
 * Sort a string list from small values to large using integer comparison.
 */
	int	a, b;

	sscanf ( *A, "%d", &a );	
	sscanf ( *B, "%d", &b );	

	if ( a < b ) return -1;
	if ( a == b ) return 0;
	if ( a > b ) return 1;
}


static int
IntLargeToSmall( char **A, char **B )
{
/*
 * Sort a string list from large values to small using integer comparison.
 */
	int	a, b;

	sscanf ( *A, "%d", &a );	
	sscanf ( *B, "%d", &b );	

	if ( a > b ) return -1;
	if ( a == b ) return 0;
	if ( a < b ) return 1;
}


void
GridIconCB ( Widget w, XtPointer client_data, XtPointer xt_call_data )
{
/*
 * Callback from grid icon in main window.
 */
	ModelInfoType		*mdl;
	WindowObjectType	*wo;

	char			*model;
	int			verbose;

	XmPushButtonCallbackStruct *cbs =
                    (XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "GridIconCB\n" );

	wo = GetActiveWindowObject();
	SetupAbortProcess( wo );

/*
 *	Get dialog structure.
 */
	mdl = GetGuiModelInfo();

/*
 *	Get model type.
 */
	model = GetModelType ( mdl );

/*
 *	Dynamically build dialog scrolled lists.
 */
	HSBuildScrolledLists ( model );

	CancelAbortProcess( wo, False );

	return;
}


void
HSBuildScrolledLists ( char *model )
{
/*
 * Fill in date/time and FDF scrolled lists.
 */
	Widget			forecast_list, scalar_list, vector_list;

	GuiScalarGridDialogType	*grd;
	GuiVectorGridDialogType *vec;
	ModelInfoType		*mdl;

	char			*scalardir, *vectordir;
	char			*scalarchapter, *vectorchapter;
	int			verbose;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "BuildHSDialog\n" );

	if ( model == NULL ) return;

/*
 *	Get dialog structure.
 */
	grd = GetGuiScalarGridDialog();
	vec = GetGuiVectorGridDialog();
	mdl = GetGuiModelInfo();

/*
 *	Get info to determine full file name.
 */
	scalardir = GetConfigValue( "scalarfdf" );
	vectordir = GetConfigValue( "vectorfdf" );
	scalarchapter = strdup ( GetModelScalarDir ( grd ) );
	vectorchapter = strdup ( GetModelVectorDir ( grd ) );

/*
 *	Get scrolled list widget ids.
 */
	forecast_list = GetModelScrolledListHSW( grd );
	scalar_list = GetScrolledListHSW( grd );
	vector_list = GetScrolledListHVW( vec );

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
 *	Dynamically build dialog.
 */
	BuildModelForecastList ( SCALARGRIDOBJECT,
				 forecast_list,
				 GetModelTimeListLabelW(grd),
	                         model,
	                         &(grd->time_list),
	                         &(grd->match_list),
	                         &(grd->select_list) );

	BuildFieldList ( scalar_list, scalardir, scalarchapter, model,
	                                           &(grd->field_list) );

	BuildFieldList ( vector_list, vectordir, vectorchapter, model,
	                                           &(vec->field_list) );

/*
 *	Manage widgets.
 */
	XtManageChild ( forecast_list );
	XtManageChild ( scalar_list );
	XtManageChild ( vector_list );

/*
 *	Free.
 */
	Free ( scalardir );
	Free ( vectordir );
	Free ( scalarchapter );
	Free ( vectorchapter );

	return;
}


void
TimeButtonCB (Widget    w,
	      XtPointer clientData,
	      XtPointer xt_call_data )
{
/*
 * Not currently used.
 */
	GuiScalarGridDialogType *grd;
	int			verbose;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
	            (XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "TimeButtonCB\n" );

	grd = GetGuiScalarGridDialog();

/*
 *	Save button info.
 */
	/* grd->time_b.setbutton = bo; */
}


void
SetModelFieldCB( Widget    w,
	         XtPointer client_data,
	         XtPointer xt_call_data )
/*
 * Callback for grid Display button.
 * This callback gets the value of the Function text string in case it
 * was edited by the user and saves it.
 */
{
	char			*text;
	int			verbose;

	GuiScalarGridDialogType *grd =
	            (GuiScalarGridDialogType *) client_data;

	XmPushButtonCallbackStruct *cbs =
	            (XmPushButtonCallbackStruct *) xt_call_data;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "SetModelFieldCB\n" );

/*
 *	Get Text widget value.
 */
	if ( text = XmTextGetString ( grd->func_string.w ) ) {
/*
 *	    Free previous string.
 */
	    if ( grd->func_string.string && 
		                 strlen ( grd->func_string.string ) > 0 )
		Free ( grd->func_string.string );
/*
 *	    Set string.
 */
	    grd->func_string.string = strdup ( text );
	    XtFree ( text );
	}
}


void
GetModelField ( char *file, char *subdir, char *directory )
/*
 * Set Field Description and GFUNC text field using Field Description file
 * as input.
 */
{
	ModelInfoType		*mdl;
	KeyListType		*fdf;


	char			*model;
	int			err, verbose;

	
	mdl = GetGuiModelInfo();
	fdf = GetHSGuiFdfInfo();


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "GetModelField\n" );


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
	Free ( model );
}


void
GetModelFieldCB ( Widget w, XtPointer client_data, XtPointer xt_call_data )
/*
 * Callback for scalar grid list.
 * Set Field Description and GFUNC text field using Field Description file
 * as input.
 */
{
	Widget			vector_list;

	GuiScalarGridDialogType	*grd;
	GuiVectorGridDialogType	*vec;


	char			*file, *chapter, *directory;
	int			*index, count;
	int			verbose;

	ListParentObjectType *lpo = 
			(ListParentObjectType *) client_data;

	
	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "GetModelFieldCB\n" );

	grd = GetGuiScalarGridDialog();
	vec = GetGuiVectorGridDialog();

/*
 *	Set plot type to scalar.
 */
	SetModelGridTypeCB ( w, (XtPointer) SCALARGRIDOBJECT,
		                (XtPointer) NULL );

/*
 *	Deselect vector field item.
 */
	vector_list = GetScrolledListHVW ( vec );
	if ( XmListGetSelectedPos ( vector_list, &index, &count ) ) {
	    XtUnmanageChild ( vector_list );
	    XmListDeselectPos ( vector_list, *index );
	    XtManageChild ( vector_list );
	    Free ( index );
	}

/*
 *	Get index of chosen field.
 */
	XmListGetSelectedPos ( lpo->w, &index, &count );
	if ( count <= 0 ) {
	    printf ( "  GetModelFieldCB - nothing selected ... returning\n");
	    return;
	}

/*
 *	Read in model field.
 */
	file = strdup ( lpo->label[*index-1] );
	chapter = strdup ( GetModelScalarDir ( grd ) );
	Free ( index );

/*
 *	Append FDF file name to path.
 */
	directory = GetConfigValue( "scalarfdf" );
	if ( directory == NULL ) {
	    printf ( "*** GetModelFieldCB - scalarfdf key is undefined\n" );
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
SelectModelLevelCB ( Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	Widget			level_form, layer_form;
	static Widget		level_label, layer_label;
	static Widget		level_frame, layer_frame;
	static Widget		level_rowcol, layer_rowcol;
	GuiScalarGridDialogType	*grd;
	ModelInfoType		*mdl;

	ButtonObject		*bo;
	ButtonParentObjectType	*bpo1, *bpo2;
	char			*model, *coordinate;
	char			**levels, **layers;
	int			(*AlphaUpSortFunc)();
	int			(*AlphaDownSortFunc)();
	int			(*IntUpSortFunc)();
	int			(*IntDownSortFunc)();
	int			i, j, k, num_levels, num_layers;
	int			level, columns, verbose;

	static char		prevmdl[]="no         ";
	static char		prevvcoord[]="no         ";

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	level = (int) client_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "SelectModelLevelCB\n" );

	grd = GetGuiScalarGridDialog();
	mdl = GetGuiModelInfo();

/*
 *	Define sort algorithms.
 */
	AlphaUpSortFunc = AlphaSortUp;
	AlphaDownSortFunc = AlphaSortDown;
	IntUpSortFunc = IntSmallToLarge;
	IntDownSortFunc = IntLargeToSmall;

/*
 *	Set active level button.
 */
	SetActiveModelLevel ( mdl, level );

/*
 *	Get button parent objects.
 */
	bpo1 = &( grd->level_b );
	bpo2 = &( grd->layer_b );

/*
 *	Get currently selected model name.
 */
	model = GetModelType ( mdl );
	if ( model == NULL ) return;

	coordinate = strdup ( GetModelVCoord ( mdl ) );
	ToLower ( coordinate );

/*
 *	Bail out if we've already created this level list.
 */
	if ( strcmp ( model, prevmdl ) == 0 &&
	     strcmp ( prevvcoord, coordinate ) == 0 ) {
	    Free ( coordinate );
	    return;
	}
	strcpy ( prevmdl, model );
	strcpy ( prevvcoord, coordinate );

/*
 *	Get number of vertical levels for this model in this coordinate system.
 */
	num_levels = GetModelLevels ( model, coordinate, &levels );
	num_layers = GetModelLayers ( model, coordinate, &layers );


	if ( verbose > VERBOSE_1 ) {
	    printf("  num_levels = %d\n", num_levels );
	    printf("  num_layers = %d\n", num_layers );
	}

/*
 *	Check for overflow.
 */
	if ( num_levels > MAXBUTTONS ) {
	    printf ("*** Error in function SelectModelLevelCB\n" );
	    printf ("*** Can't make %d buttons, exiting ...\n", num_levels );
	    Exit ( w, "Fatal Error, see status box for message" );
	    return;
	}

	if ( num_layers > MAXBUTTONS ) {
	    printf ("*** Error in function SelectModelLevelCB\n" );
	    printf ("*** Can't make %d buttons, exiting ...\n", num_layers );
	    Exit ( w, "Fatal Error, see status box for message" );
	    return;
	}

/*
 *	Check for no buttons.
 */
	if ( num_levels <= 0 && num_layers <= 0 ) {
	    level_rowcol = NULL;
	    layer_rowcol = NULL;
	    return;
	}
/*
 *	Get widget id for frame widget.
 */
	level_form = GetModelLevelFormW ( grd ); 
	layer_form = GetModelLayerFormW ( grd );

/*
 *	Sort levels, layers. Use different algorithms depending on the
 *	vertical coordinate to attempt to list values from the ground up.
 */
	if ( num_levels > 0 ) {
	    if ( ( strcasecmp (  coordinate, "pres" ) == 0 ) || 
	         ( strcasecmp (  coordinate, "sgma" ) == 0 ) )
		qsort( levels, num_levels, sizeof(char *), IntDownSortFunc );
	    else
		qsort( levels, num_levels, sizeof(char *), IntUpSortFunc );
	}

	if ( num_layers > 0 ) {
	    if ( ( strcasecmp (  coordinate, "pres" ) == 0 ) || 
	         ( strcasecmp (  coordinate, "sgma" ) == 0 ) )
		qsort( layers, num_layers, sizeof(char *), AlphaDownSortFunc );
	    else
		qsort( layers, num_layers, sizeof(char *), AlphaUpSortFunc );
	}

/*
 *	Delete previously created widgets, free strings.
 */
	bo = &(bpo1->buttons[0]);
	for ( i=0; i<bpo1->button_cnt; i++, bo++ ) {
	    bo->ismanaged = 0;
	    Free ( bo->string );
	    Free ( bo->label );
	    XtDestroyWidget ( bo->w );
	}
	bpo1->button_cnt = 0;

	bo = &(bpo2->buttons[0]);
	for ( i=0; i<bpo2->button_cnt; i++, bo++ ) {
	    bo->ismanaged = 0;
	    Free ( bo->string );
	    Free ( bo->label );
	    XtDestroyWidget ( bo->w );
	}
	bpo2->button_cnt = 0;

	if ( level_label  != NULL ) XtDestroyWidget ( level_label );
	if ( level_frame  != NULL ) XtDestroyWidget ( level_frame );
	if ( level_rowcol != NULL ) XtDestroyWidget ( level_rowcol );
	if ( layer_label  != NULL ) XtDestroyWidget ( layer_label );
	if ( layer_frame  != NULL ) XtDestroyWidget ( layer_frame );
	if ( layer_rowcol != NULL ) XtDestroyWidget ( layer_rowcol );

/*
 *	Create rowcolumn for level buttons.
 */
	/*columns = ( num_levels + 4 ) / 5;*/
        /* original code failed to display all levels chiz/unidata 2/99 */
        columns = (int)sqrt((double)num_levels);
	if(columns*columns < num_levels) columns++;
	level_label  = NULL;
	level_frame  = NULL;
	level_rowcol = NULL;
	if ( columns > 0 ) {

	    level_label  = XtVaCreateManagedWidget( "Model Levels",
				xmLabelGadgetClass, level_form,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				NULL );

	    level_frame  = XtVaCreateManagedWidget( "frame1",
	                        xmFrameWidgetClass, level_form,
				XmNtopAttachment, XmATTACH_WIDGET,
				XmNtopWidget, level_label,
				XmNleftAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL );

            /* use fractionbase = columns rather than 4, chiz 2/99 */
            level_rowcol = XtVaCreateManagedWidget( "framerowcolumn1",
                                xmFormWidgetClass, level_frame,
                                XmNfractionBase, columns,
				NULL );

/*
 *	Create level buttons. Won't happen if num_levels < 1.
 */
	    bpo1->button_cnt = num_levels;
	    bpo1->button_alloc_cnt = num_levels;
	    bpo1->parent = level_form;
	    bo = &(bpo1->buttons[0]);

	    for ( k=0; k<num_levels; k++, bo++ ) {
                /* don't hardcode number of levels, chiz 2/99 */
	 	j = k/columns;
		i = k - (j * columns);
	        bo->w = XtVaCreateManagedWidget( levels[k],
				xmPushButtonWidgetClass, level_rowcol,
				XmNfillOnArm, TRUE,
				XmNtopAttachment, XmATTACH_POSITION,
				XmNtopPosition, j,
				XmNleftAttachment, XmATTACH_POSITION,
				XmNleftPosition, i,
				XmNrightAttachment, XmATTACH_POSITION,
				XmNrightPosition, i+1,
				XmNbottomAttachment, XmATTACH_POSITION,
				XmNbottomPosition, j+1,
				NULL);

	        XtAddCallback ( bo->w, XmNactivateCallback,
	                               SetModelLevelCB, bo );
	        bo->ismanaged = 1;
	        bo->string = strdup ( " " );
	        bo->label = strdup ( levels[k] );
	    }
	}

/*
 *	Create rowcolumn for layer buttons.
 */
        columns = (int)sqrt((double)num_layers);
	if(columns*columns < num_layers) columns++;
	layer_label  = NULL;
	layer_frame  = NULL;
	layer_rowcol = NULL;
	if ( columns > 0 ) {

	    layer_label  = XtVaCreateManagedWidget( "Model Layers",
				xmLabelGadgetClass, layer_form,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				NULL );

	    layer_frame  = XtVaCreateManagedWidget( "frame2",
				xmFrameWidgetClass, layer_form,
				XmNtopAttachment, XmATTACH_WIDGET,
				XmNtopWidget, layer_label,
				XmNleftAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL );

	    for ( i=0; i<3; i++ ) {
	    for ( j=0; j<columns; j++ ) {
            layer_rowcol = XtVaCreateManagedWidget( "framerowcolumn2",
                                xmFormWidgetClass, layer_frame,
                                XmNfractionBase, columns,
                                NULL);
	    }
	    }

/*
 *	Create layer buttons. Won't happen if num_layers < 1.
 */
	    bpo2->button_cnt = num_layers;
	    bpo2->button_alloc_cnt = num_layers;
	    bpo2->parent = layer_form;
	    bo = &(bpo2->buttons[0]);
	
	    for ( k=0; k<num_layers; k++, bo++ ) {
	 	j = k/columns;
		i = k - (j * columns);
	        bo->w = XtVaCreateManagedWidget( layers[k],
				xmPushButtonWidgetClass, layer_rowcol,
				XmNfillOnArm, TRUE,
				XmNtopAttachment, XmATTACH_POSITION,
				XmNtopPosition, j,
				XmNleftAttachment, XmATTACH_POSITION,
				XmNleftPosition, i,
				XmNrightAttachment, XmATTACH_POSITION,
				XmNrightPosition, i+1,
				XmNbottomAttachment, XmATTACH_POSITION,
				XmNbottomPosition, j+1,
				NULL);
	        XtAddCallback ( bo->w, XmNactivateCallback,
                                           SetModelLayerCB, bo );
	        bo->ismanaged = 1;
	        bo->string = strdup ( " " );
	        bo->label = strdup ( layers[k] );
	    }
	}

/*
 *	Free.
 */
	Free ( coordinate );
	StringListFree ( num_levels, levels );
	StringListFree ( num_layers, layers );

}


void
ScalarGridLineTypeCB ( Widget w, XtPointer client_data, XtPointer xt_call_data )
/*
 * This callback function sets the line type for scalar gridded data.
 */
{
	GuiScalarGridDialogType	*grd;
	char			buf[10];
	int			line_type;
	int			verbose;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	line_type = (int) client_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "ScalarGridLineTypeCB\n" );

	grd = GetGuiScalarGridDialog();
/*
 *	Set contour line type.
 */
	sprintf ( buf, "%d", line_type );

	FreeLineTypeHS ( grd );
	SetLineTypeHS ( grd, buf );
}


void
ScalarGridLabelFrequencyCB ( Widget w, XtPointer clientData, XtPointer xt_call_data )
/*
 * This callback function sets line labeling frequency for scalar gridded data.
 */
{
	GuiScalarGridDialogType *grd;
	char			buf[10];
	int			label_frequency;
	int			verbose;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	label_frequency = (int) clientData;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "ScalarGridLabelFrequencyCB\n" );

	grd = GetGuiScalarGridDialog();

/*
 *	Set line labeling frequency.
 */
	sprintf ( buf, "%d", label_frequency );

	FreeLabelFrequencyHS ( grd );
	SetLabelFrequencyHS ( grd, buf );
}


void
BuildScalarGrid ( GuiScalarGridDialogType *grd )
{

	MetObjectType		*mlist[MAXMETOBJS];
	WindowObjectType	*wo;
	ModelInfoType		*mdl;
	KeyListType		*fdf;
	GeoRefObjectType	*geo;
	GlobalDataObjectType	*gd;

	ListParentObjectType	*inList;
	char			**levels;

	char			*model,		*file,		*ftime,
				*level,		*level1,	*level2,
				*field;

	char			*cint,		*cmin,		*cmax,
				*fint,		*fmin,		*fmax;

	char			*line_color,	*line_type,
				*line_width,	*label_freq;

	char			*vcoord,	*fline,		*symbol,
				*hilo,		*contour,	*colorbar,
				*scale,		*text;

	char			*skip,		*skip_x,	*skip_y,
        			*model_label,   *field_label;
	char			*buf, gridtype[80];

	int			*indexList;
	int			nfunction;
	int			i,		j;
	int			count,		matchSize,     metObjectCount;
	int			verbose,	titleIndex,    iret;

	int			viewType, 	nlevels;
	BooleanType		do_contour, do_value, do_fill, do_symbol;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "BuildScalarGrid\n" );

	mdl = GetGuiModelInfo();
	fdf = GetHSGuiFdfInfo();
	gd = GetGlobalDataObject();
	wo = GetActiveWindowObject();

	inList = ReturnListStruct ( GetModelScrolledListHSW( grd ),
	                            grd->time_list,
	                            grd->match_list,
	                            &(grd->select_list) );
	if ( inList == NULL ) return;
	count = inList->listcount;

/*
 *	Save the first time in the list for selection purposes.
 */
	if ( grd->select_list.listcount > 0 )
	    PreviousSelectedTime ( "SET", SCALARGRIDOBJECT, inList->label[0] );

/*
 *	Check on the view types for compatibility. Compare this object 
 *	with the current georeference view state and do a reset if
 *	they don't match.
 */
	viewType  = GetMetObjectView( SCALARGRIDOBJECT );

	if ( geo = GetGeoRefObject( wo) )
		if ( geo->viewType != viewType ) WindowObjectFullReset( wo );

/*
 *	Get FDF from text widget.
 */
	if ( grd->func_string.string != NULL ) {		/* fdf */
	    field = (char *) malloc ( GEMPAKSTRING * sizeof ( char ) );
	    strcpy ( field, grd->func_string.string );
	    bpad ( field, GEMPAKSTRING, &iret );
	    strcpy ( gridtype, " s" );
	}
	else {
	    printf ("*** Error in BuildScalarGrid - field is undefined\n" );
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
 *	Get the vcoord
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
	if      ( strcasecmp ( level, "all_level" ) == 0 )
	    nlevels = GetModelLevels ( 
			GetModelType(mdl), GetModelVCoord (mdl), &levels );
	else if ( strcasecmp ( level, "all_layer" ) == 0 ) {
	    nlevels = GetModelLayers (
			GetModelType(mdl), GetModelVCoord (mdl), &levels );
	    PrintArgv( nlevels, levels );
	}
	else {
	    nlevels = 1;
	    levels = StringListAllocate ( nlevels );
	    *levels = strdup ( level );
	}
	Free ( level );

		 
/*
 *	Get contour interval, minimum and maximum.
 */
	if ( buf = XmTextGetString ( GetContourIntervalHSW ( grd ) ) ) {
	    cint = strdup ( buf );
	    XtFree ( buf );
	    FreeContourIntervalHS ( grd );
	    SetContourIntervalHS ( grd, cint );
	}
	else
	    cint = strdup ( "0" );

	if ( buf = XmTextGetString ( GetContourMinimumHSW ( grd ) ) ) {
	    cmin = strdup ( buf );
	    XtFree ( buf );
	    FreeContourMinimumHS ( grd );
	    SetContourMinimumHS ( grd, cmin );
	}
	else
	    cmin = strdup ( "0" );

	if ( buf = XmTextGetString ( GetContourMaximumHSW ( grd ) ) ) {
	    cmax = strdup ( buf );
	    XtFree ( buf );
	    FreeContourMaximumHS ( grd );
	    SetContourMaximumHS ( grd, cmax );
	}
	else
	    cmax = strdup ( "0" );

/*
 *	Get color fill interval, minimum and maximum.
 */
	if ( buf = XmTextGetString ( GetFillIntervalHSW ( grd ) ) ) {
	    fint = strdup ( buf );
	    XtFree ( buf );
	    FreeFillIntervalHS ( grd );
	    SetFillIntervalHS ( grd, fint );
	}
	else
	    fint = strdup ( "0" );

	if ( buf = XmTextGetString ( GetFillMinimumHSW ( grd ) ) ) {
	    fmin = strdup ( buf );
	    XtFree ( buf );
	    FreeFillMinimumHS ( grd );
	    SetFillMinimumHS ( grd, fmin );
	}
	else
	    fmin = strdup ( "0" );

	if ( buf = XmTextGetString ( GetFillMaximumHSW ( grd ) ) ) {
	    fmax = strdup ( buf );
	    XtFree ( buf );
	    FreeFillMaximumHS ( grd );
	    SetFillMaximumHS ( grd, fmax );
	}
	else
	    fmax = strdup ( "0" );

/*
 *	Check if contour lines, color fill or symbol is to be drawn.
 */
	do_contour = FALSE;
	do_value   = FALSE;
	do_fill    = FALSE;
	do_symbol  = FALSE;
	if ( XmToggleButtonGetState ( GetLineToggleHSW1 ( grd ) ) )
	    do_contour = TRUE;
	if ( XmToggleButtonGetState ( GetGridValueToggleHSW ( grd ) ) )
	    do_value   = TRUE;
	if ( XmToggleButtonGetState ( GetFillToggleHSW1 ( grd ) ) )
	    do_fill    = TRUE;
	if ( XmToggleButtonGetState ( GetSymbolToggleHSW1 ( grd ) ) )
	    do_symbol  = TRUE;
            
/*
 *	Get color fill colors.
 */
	fline = strdup ( GetFillColorsHS ( grd ) );

/*
 *	Get contour line characteristics.
 */
	line_color = strdup ( GetLineColorHS ( grd ) );

	if ( strcmp ( line_color, "NONE" ) == 0 ) {
	    GetNextObjectColor ( gd );
	    Free (line_color) ;
	    line_color = strdup ( gd->colors);
	}
	else
	    SetColorAsUsed ( line_color );

	line_type  = strdup ( GetLineTypeHS ( grd ) );
	line_width = strdup ( GetLineWidthHS ( grd ) );
	label_freq = strdup ( GetLabelFrequencyHS ( grd ) );

/*
 *	Information for plotting relative highs and lows.
 */
	hilo = BuildGemParm ( "hilo", fdf );

/*
 *	Define the characteristics for the HILO symbols.
 */
	symbol = BuildGemParm ( "symbol", fdf );

/*
 *	Specify the characteristics of a color bar.
 */
	colorbar = BuildGemParm ( "colorbar", fdf );
	         
/*
 *	Set attributes for the contour algorithms.
 */
	contour = BuildGemParm ( "contour", fdf );

/*
 *	Set text size, font, text width and hardware/software flag.
 */
	text = BuildGemParm ( "text", fdf );

/*
 *	Skip factor.
 */
	skip   = GetFdfKeyValue ( fdf, "skip_contour" );
	skip_x = GetFdfKeyValue ( fdf, "skip_plot_x" );
	skip_y = GetFdfKeyValue ( fdf, "skip_plot_y" );

/*
 *	Scale factor.
 */
	if ( buf = XmTextGetString ( GetScaleFactorHSW ( grd ) ) ) {
	    scale = strdup ( buf );
	    XtFree ( buf );
	}
	else
	    scale = strdup ( "9999" );

/*
 *	Get model type and model label.
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
        for ( i=0; i<count; i++ ) {	/* loop over times */
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
				MakeHorizontalScalarGridObject(
					file      , ftime     , vcoord    ,
					levels[j] , field     , gridtype  ,
					fline     , hilo      , symbol    , 
					colorbar  , skip      , skip_x    ,
					skip_y    , contour   ,
					text      , scale     ,
					line_color, line_type , line_width,
					label_freq, cint      , cmin      , 
					cmax      , fint      , fmin      , 
					fmax      , model     ,
					model_label, field_label,
					nfunction ,
					titleIndex, do_contour,
					do_value  , do_fill   , do_symbol );
		metObjectCount++;
            }

	    Free ( ftime );
	    Free ( file );
	}

	Free ( vcoord );
	Free ( field );
	Free ( fline );
	Free ( hilo );
	Free ( symbol );
	Free ( colorbar );
	Free ( skip );
	Free ( skip_x );
	Free ( skip_y );
	Free ( contour );
	Free ( text );
	Free ( scale );
	Free ( line_color );
	Free ( line_type );
	Free ( line_width );
	Free ( label_freq );
	Free ( cint );
	Free ( cmin );
	Free ( cmax );
	Free ( fint );
	Free ( fmin );
	Free ( fmax );
	Free ( model );
	Free ( model_label );
	Free ( field_label );
	StringListFree ( nlevels, levels );

	count = count * nlevels;
	if ( metObjectCount > 0 )
            AddObjectListToDisplay( count, mlist );

/*
 *	Adding color fill graphics into the list means any previous met
 *	objects should be invalidated (redrawn), so do that.
 */
	if ( do_fill ) {
		ClearAllPots (wo);
		SetWindowObjectDrawnFlag ( wo, False );
	}
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
			SCALARGRIDOBJECT, &matchSize );
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
