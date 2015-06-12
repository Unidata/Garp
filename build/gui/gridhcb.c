/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gridhcb.c
 *
 *	Callbacks for plan view model data.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 7/97	COMET	Added model defined subdirectories.
 *	 9/97	COMET	Added macro functionality.
 *	11/97	COMET	Fixed some leaks.
 *	11/97	COMET	Save the button label for a model. Deleted the 
 *			unused routine "SetModelCB".
 *	11/97	COMET	Fix leaks in SetModelLevels and SetModelLayers
 *	12/97	COMET	Fix leaks related to builddirpath
 *	12/97	COMET	Bug fixes in SetModelLevels and SetModelLayers.
 *	12/97	COMET	Added call to RefreshScrolledTimeLists
 *	 1/98	COMET	Took out printf statement for vert coord
 *	 2/98	COMET	Added code to allow for model dependent macros.
 *	 8/99	COMET	Redefined FDF model directory.
 *	 8/99	COMET	Check for null bo->string in CheckModelLevels().
 *	12/99	COMET	Added err=-1 test in RunHSMacro() to fix bug
 *			that occurred when you changed models but ran
 *			the same macro. Problem is because fdf->path is 
 *			overwritten.
 *
 ***********************************************************************/


#include "utils.h"
#include "guimacros.h"
#include "fdfobj.h"
#include "hsobj.h"
#include "hvobj.h"
#include "model.h"
#include "gdinfo.h"
#include "genglobs.h"
#include "winobj.h"
#include "menucb.h"
#include "_proto.h"


GridInfoObjectType *GetGridInfo();
char * GetStringArrayLabel ( int, char *, char **, char ** );


void
SetModelGridTypeCB ( Widget w, XtPointer clientData, XtPointer call_data )
/*
 * This function sets the plot type to SCALAROBJECT or VECTOROBJECT when
 * an FDF is chosen for model grids. It also activates the appropriate
 * radio button. This routine is used both as a callback and as a general
 * use routine so the widget id may not be that of either the scalar or
 * vector radio buttons.
 */
{
	Widget			scalar_b, vector_b;
	GuiScalarGridDialogType	*grd;
	ModelInfoType		*mdl;
	int			type;

	type = (int) clientData;

	grd = GetGuiScalarGridDialog();
	mdl = GetGuiModelInfo();

/*
 *	Set grid type to SCALAROBJECT or VECTOROBJECT.
 */
	SetGridType ( mdl, type );

/*
 *	Set radio button to either Scalar or Vector.
 */
	scalar_b = GetModelScalarButtonHSW ( grd );
	vector_b = GetModelVectorButtonHSW ( grd );

	if      ( type == SCALARGRIDOBJECT ) {
	    if ( ! XmToggleButtonGetState ( scalar_b ) )
	    	XmToggleButtonSetState ( scalar_b, TRUE, FALSE );
	    if ( XmToggleButtonGetState ( vector_b ) )
		XmToggleButtonSetState ( vector_b, FALSE, FALSE );
	}

	else if ( type == VECTORGRIDOBJECT ) {
	    if ( ! XmToggleButtonGetState ( vector_b ) )
	    	XmToggleButtonSetState ( vector_b, TRUE, FALSE );
	    if ( XmToggleButtonGetState ( scalar_b ) )
		XmToggleButtonSetState ( scalar_b, FALSE, FALSE );
	}

}


void
SetModelLevelTypeCB (Widget w, XtPointer clientData, XtPointer call_data )
{
/*
 * Sensitize level or layer radio button. Put this value into fdf.
 */
	Widget			level_b, layer_b;

	GuiScalarGridDialogType	*grd;
	ModelInfoType		*mdl;
	KeyListType		*fdf;

	char			*layer;

	XmToggleButtonCallbackStruct *cbs = 
			(XmToggleButtonCallbackStruct *) call_data;

	layer = strdup ( clientData );

	grd = GetGuiScalarGridDialog();
	mdl = GetGuiModelInfo();
	fdf = GetHSGuiFdfInfo();

/*
 *	Set level type to "level" or "layer".
 */
	PutInFDF ( "layer", layer, fdf, OVERWRITE );
/*	SetGridType ( mdl, type ); */

/*
 *	Set radio button to either Scalar or Vector.
 */
	level_b = GetModelLevelToggleHSW ( mdl );
	layer_b = GetModelLayerToggleHSW ( mdl );

	if      ( strcasecmp ( layer, "n" ) == 0 ) {
	    if ( ! XmToggleButtonGetState ( level_b ) )
	    	XmToggleButtonSetState ( level_b, TRUE, FALSE );
	    if ( XmToggleButtonGetState ( layer_b ) )
		XmToggleButtonSetState ( layer_b, FALSE, FALSE );
/*
 *	Desensitize Level 2 button and text field.
 */
	    XtSetSensitive ( GetModelLevel2BHSW ( grd ), FALSE );
	    XtSetSensitive ( GetModelLevel2HSW ( mdl ), FALSE );
	}

	else if ( strcasecmp ( layer, "y" ) == 0 ) {
	    if ( ! XmToggleButtonGetState ( layer_b ) )
	    	XmToggleButtonSetState ( layer_b, TRUE, FALSE );
	    if ( XmToggleButtonGetState ( level_b ) )
		XmToggleButtonSetState ( level_b, FALSE, FALSE );
/*
 *	Sensitize Level 2 button and text field.
 */	
	    XtSetSensitive ( GetModelLevel2BHSW ( grd ), TRUE );
	    XtSetSensitive ( GetModelLevel2HSW ( mdl ), TRUE );
	}

	Free (layer );
}


void
SetModelLevelCB( Widget    w,
	         XtPointer clientData,
	         XtPointer call_data )
{
/*
 * Set the active model level textfield widget.
 */
	Arg			al[64];
	register int		ac;
	XmString		xmstr;

	GuiScalarGridDialogType *grd;
	GuiVectorGridDialogType	*vec;
	ModelInfoType		*mdl;
	KeyListType		*fdf;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
	            (XmPushButtonCallbackStruct *) call_data;

	grd = GetGuiScalarGridDialog();
	vec = GetGuiVectorGridDialog();
	mdl = GetGuiModelInfo();
	fdf = GetHSGuiFdfInfo();

/*
 *	Save button info.
 */
	grd->level_b.setbutton = bo;

/*
 *	Update label.
 */
	if ( GetActiveModelLevel ( mdl ) == 1 ) {
	    FreeModelLevel1 ( mdl );
	    SetModelLevel1 ( mdl, grd->level_b.setbutton->label );
	    XmTextSetString ( GetModelLevel1HSW ( mdl ),
				 GetModelLevel1 ( mdl ) );
/*
 *	    Make sure the level radio button is set.
 */
/*	    SetModelLevelTypeCB ( bo->w, "n", NULL ); */

/*
 *	Reset contour information.
 */
	SetContourLineInfo ( fdf, GetModelVCoord ( mdl ),
	                          GetModelLevel1 ( mdl ) );

	}
	if ( GetActiveModelLevel ( mdl ) == 2 ) {
	    FreeModelLevel2 ( mdl );
	    SetModelLevel2 ( mdl, grd->level_b.setbutton->label );
	    XmTextSetString ( GetModelLevel2HSW ( mdl ),
				 GetModelLevel2 ( mdl ) );
	}

}


void
SetModelLayerCB( Widget    w,
	         XtPointer clientData,
	         XtPointer call_data )
{
/*
 * Set both model level textfield widgets.
 */
	Arg			al[64];
	register int		ac;
	XmString		xmstr;
	char			*fullstr, *firststr, *secondstr;

	GuiScalarGridDialogType *grd;
	GuiVectorGridDialogType	*vec;
	ModelInfoType		*mdl;
	KeyListType		*fdf;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
	            (XmPushButtonCallbackStruct *) call_data;

	grd = GetGuiScalarGridDialog();
	vec = GetGuiVectorGridDialog();
	mdl = GetGuiModelInfo();
	fdf = GetHSGuiFdfInfo();

/*
 *	Save button info.
 */
	grd->layer_b.setbutton = bo;

/*
 *	Parse the layer string into two level strings.
 */
	fullstr = strdup ( bo->label );
	firststr = strdelafter ( bo->label, ':' );
	secondstr = strdelupto ( bo->label, ':' );

/*
 *	Update label.
 */
	FreeModelLevel1 ( mdl );
	SetModelLevel1 ( mdl, firststr );
	XmTextSetString ( GetModelLevel1HSW ( mdl ),
				 GetModelLevel1 ( mdl ) );
	FreeModelLevel2 ( mdl );
	SetModelLevel2 ( mdl, secondstr );
	XmTextSetString ( GetModelLevel2HSW ( mdl ),
				 GetModelLevel2 ( mdl ) );
/*
 *	Make sure the layer radio button is set.
 */
/*	SetModelLevelTypeCB ( bo->w, "y", NULL ); */

/*
 *	Reset contour information.
 */
	SetContourLineInfo ( fdf, GetModelVCoord ( mdl ),
	                          GetModelLevel1 ( mdl ) );
/*
 *	Free.
 */
	Free ( fullstr );
	Free ( firststr );
}


void
RunHSMacro ( char *directory )
{
	GuiScalarGridDialogType	*grd;
	GuiVectorGridDialogType	*vec;
	ModelInfoType		*mdl;
	int			plot_type;

	KeyListType		*fdf, *macro=NULL;
	char			*subdir, *model;
	char			*current_directory;
	char			*plot_type_key;
	char			chapter[81];
	char			keyName[1024], value[1024];
	int			i, err;


	grd = GetGuiScalarGridDialog();
	vec = GetGuiVectorGridDialog();
	mdl = GetGuiModelInfo();
	fdf = GetHSGuiFdfInfo();

/*
 *	Save default directory in case the directory is redefined in a
 *	macro.
 */
	current_directory = strdup ( directory );

/*
 *	Read macro into a key list which allows for duplicate 
 *	entries.
 */
	if ( GetGridType ( mdl ) == SCALARGRIDOBJECT )
	    subdir = strdup ( GetModelScalarDir ( grd ) );
	else
	    subdir = strdup ( GetModelVectorDir ( grd ) );

	model = strdup ( GetModelType ( mdl ) );
	err = GetMacro ( fdf->fileName, fdf->path,
	           subdir, model, &macro );

/*
 * If err=-1, then the fdf->path was changed. Try the right path.
 */
	if (err==-1) {
	      err = GetMacro ( fdf->fileName, current_directory,
	                       subdir, model, &macro );
	}
        if (err) { 
	    Free (current_directory);
	    Free (subdir);
	    return;
	}
	
/*
 *	Put key/value pairs into FDF and then display it.
 */
	for ( i=0; i<macro->numKeys; i++ ) {

	    strcpy ( keyName, macro->keys[i].keyName );
	    strcpy ( value,   macro->keys[i].value );
	    ToUpper ( keyName );
	    ToUpper ( value );

/*
 *	Update GUI and build metObjects.
 */
	    if ( strcmp ( keyName, "OPERATION" ) == 0 &&
		     strcmp ( value, "RUN" ) == 0 ) {
		SetFdfFallbackValues ( fdf );
	    	SetGridWidgets ( fdf, fdf->fileName );

	    	switch ( plot_type ) {
	    	case SCALARGRIDOBJECT:
	      	  BuildScalarGrid ( grd );
	          break;
	        case VECTORGRIDOBJECT:
	          BuildVectorGrid ( vec );
	          break;
	        default:
	          break;
	        }

	    }
/*
 *	Save the directory holding the current FDF.
 */
	    else if ( strcmp ( keyName, "DIRECTORY" ) == 0 ) {
		Free ( current_directory );
		current_directory = GetConfigValue ( macro->keys[i].value );
	    }

/*
 *	Save the chapter holding the current FDF.
 */
	    else if ( strcmp ( keyName, "CHAPTER" ) == 0 ) {
		strcpy ( chapter, macro->keys[i].value );
	    }
/*
 *	Read in FDF. Save plot type.
 */
	    else if ( strcmp ( keyName, "FDF" ) == 0 ) {
		GetModelField ( macro->keys[i].value, chapter,
		                current_directory );
		fdf = GetHSGuiFdfInfo();
/*
 *	Save plot type.
 */
		plot_type_key = GetFdfKeyValue ( fdf, "type" );
		ToUpper ( plot_type_key );
		plot_type = SCALARGRIDOBJECT;
		if ( strcmp ( plot_type_key, "VECTOR" ) == 0 ) 
			plot_type = VECTORGRIDOBJECT;
		Free ( plot_type_key );
	    }
/*
 *	Put modifications into FDF. 
 */
	    else
	        PutInFDF ( macro->keys[i].keyName,
			       macro->keys[i].value, fdf, OVERWRITE );
	}

/*
 *	Restore current macro settings.
 */
	CopyFdf ( macro, fdf );
	   
	if ( macro != NULL ) DestroyFDF ( macro );

/* 
 *	Free.
 */
	Free ( model );
	Free ( subdir );
	Free ( current_directory );

}


void
GridDataCB ( Widget w, XtPointer client_data, XtPointer call_data )
/*
 * Callback for horizontal gridded data menu to determine which object
 * creation routine should be called.
 */
{
	GuiScalarGridDialogType	*grd;
	GuiVectorGridDialogType	*vec;

	KeyListType		*fdf;
	char			*path, *directory;
	int			plot_type;


	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	ModelInfoType *mdl =
			(ModelInfoType *) client_data;

	if( GetVerboseLevel() > VERBOSE_0 )
	    printf ("GridDataCB\n");

	grd = GetGuiScalarGridDialog();
	vec = GetGuiVectorGridDialog();
	fdf = GetHSGuiFdfInfo();

/*
 *	Get directory information.
 */
	plot_type = GetGridType ( mdl );

	switch ( plot_type ) {
	case SCALARGRIDOBJECT:
	  directory = GetConfigValue( "scalarfdf" );
	  break;
	case VECTORGRIDOBJECT:
	  directory = GetConfigValue( "vectorfdf" ); 
	  break;
	default:
	  break;
	}


/*
 *	If the current FDF is actually a macro, step thorough it.
 */
	if ( CheckForMacro ( fdf ) ) {

	    RunHSMacro ( directory );

	}
	else {
/*
 *	Just a regular FDF.
 */
	    switch ( plot_type ) {
	    case SCALARGRIDOBJECT:
	      BuildScalarGrid ( grd );
	      break;
	    case VECTORGRIDOBJECT:
	      BuildVectorGrid ( vec );
	      break;
	    default:
	      break;
	    }
	}

/*
 *	Refresh the time list if time matching
 */
	RefreshScrolledTimeLists ( True );

/*
 *	Free.
 */
	if ( directory != NULL ) Free ( directory );
}

void
CheckModelLevels ()
/*
 * Called when a new vertical coordinate is chosen to desensitize
 * level widgets if there are no valid vertical levels.
 */
{

	GuiScalarGridDialogType	*grd;
	ModelInfoType		*mdl;
	GridInfoObjectType	*gio;
	ButtonParentObjectType	*bpo;
	ButtonObject		*bo;

	char			**vcoords;
	int			i, j, numlevels, numvcoords;
	int			verbose;
	BooleanType		flag;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "CheckModelLevels\n" );

	grd = GetGuiScalarGridDialog();
	mdl = GetGuiModelInfo();
	gio = GetGridInfo();

/*
 *	If this vertical coordinate is valid at only one level then
 *	desensitize level buttons.
 */
	if ( GetNumberModelLevels ( mdl ) < 1 && 
	     GetNumberModelLayers ( mdl ) < 1 ) {
	    XtSetSensitive ( GetModelLevel1BHSW ( grd ), FALSE );
	    XtSetSensitive ( GetModelLevel1HSW ( mdl ), FALSE );
	    XtSetSensitive ( GetModelLevel2BHSW ( grd ), FALSE );
	    XtSetSensitive ( GetModelLevel2HSW ( mdl ), FALSE );
	}
/*
 *	Sensitize button 1. Sensitize button 2 if the current FDF is
 *	a layer field.
 */

	else {
	    XtSetSensitive ( GetModelLevel1BHSW ( grd ), TRUE );
	    XtSetSensitive ( GetModelLevel1HSW ( mdl ), TRUE );

	    if ( XmToggleButtonGetState ( GetModelLayerToggleHSW (mdl) ) ) {
	    	XtSetSensitive ( GetModelLevel2BHSW ( grd ), TRUE );
	    	XtSetSensitive ( GetModelLevel2HSW ( mdl ), TRUE );
	    }
	}

/*
 *	If a vertical coordinate button is invalid then desensitize it.
 */
	bpo = &(grd->vcoordpm);
	bo = &(bpo->buttons[0]);
	numlevels = GetNumberModelLevels ( mdl );
	vcoords = GetAllGridVerticalCoordinates ( gio, &numvcoords );

	for ( i=0; i<numlevels; i++ ) {

	    flag = False;
	    for ( j=0; j<numvcoords; j++ ) {
/*
 *	Make sure a vertical coordinate string is defined.
 */
	        if (bo->string == NULL) break;

		if ( strcasecmp ( bo->string, vcoords[j] ) == 0 ) {
		    flag = True;
	    	    if ( flag ) XtSetSensitive ( bo->w, TRUE );
	    	    else        XtSetSensitive ( bo->w, FALSE );
		    bo++;
		}
	    }
	}

	return;
}


int
SetModelLevels ()
/*
 * Save a list structure containing vertical level information.
 */
{
	ModelInfoType		*mdl;
	GridInfoObjectType	*gio;
	ListParentObjectType	*list_struct;
	char			**levels1, **levels2;
	char			**list1, **list2;
	int			numLevels, numElements;
	int			verbose;


        verbose = GetVerboseLevel();
        if( verbose > VERBOSE_0 )
            printf ( "SetModelLevels\n" );

	mdl = GetGuiModelInfo();
	gio = GetGridInfo();

/*
 *	Get a list of all levels for this model and vertical coordinate.
 */
	numLevels = GetVcoordGridLevels ( gio, &levels1, &levels2 );

/*
 *	For a level, remove duplicate entries from the first list.
 */
	numElements = FilterDuplicateLevels ( numLevels, levels1,
                                                   levels2, &list1, &list2 );

/*
 *	Save level lists in model structure. Free any existing stuff first.
 */
	list_struct = (&mdl->levlist);

	if ( list_struct->listcount > 0 ) {
	    StringListFree ( list_struct->listcount, list_struct->string);
	    StringListFree ( list_struct->listcount, list_struct->label);
	}

	list_struct->listcount = 0;
	list_struct->string    = NULL;
	list_struct->label     = NULL;

	if ( numElements > 0 ) {
	    list_struct->listcount = numElements;

	    list_struct->string = 
			StringListDup ( numElements, list1 );
	    list_struct->label = 
			StringListDup ( numElements, list1 );
	}
/*
 *	Save the number of model levels.
 */
	if( verbose > VERBOSE_1 )
	    printf ("  numLevels = %d\n", numElements );
	SetNumberModelLevels ( mdl, numElements );
	CheckModelLevels();

/*
 *	Free.
 */
	StringListFree ( numLevels, levels1 );
	StringListFree ( numLevels, levels2 );
	StringListFree ( numElements, list1 );
	StringListFree ( numElements, list2 );

	return NORMAL;
}


int
SetModelLayers ()
/*
 * Save a list structure containing vertical level information
 * for defined model layers.
 */
{
	ModelInfoType		*mdl;
	GridInfoObjectType	*gio;
	ListParentObjectType	*list_struct;
	char			**levels1, **levels2;
	char			**list1, **list2;
	char			**p, buf[80];
	int			numLayers, numElements;
	int			i, verbose;


        verbose = GetVerboseLevel();
        if( verbose > VERBOSE_0 )
            printf ( "SetModelLayers\n" );

	mdl = GetGuiModelInfo();
	gio = GetGridInfo();

/*
 *	Get a list of all levels for this model and vertical coordinate.
 */
	numLayers = GetVcoordGridLevels ( gio, &levels1, &levels2 );

/*
 *	For a layer, remove duplicate entries and entries with "-1" for
 *	level 2.
 */ 
	numElements = FilterDuplicateLayers ( numLayers, levels1,
						   levels2, &list1, &list2 );
/*
 *	Save level lists in model structure. Free any existing stuff first.
 */
	list_struct = (&mdl->layerlist);

	if ( list_struct->listcount > 0 ) {
	    StringListFree ( list_struct->listcount, list_struct->string);
	    StringListFree ( list_struct->listcount, list_struct->label);
	}

	list_struct->listcount = 0;
	list_struct->string    = NULL;
	list_struct->label     = NULL;

	if ( numElements > 0 ) {
	    list_struct->listcount = numElements;
	    list_struct->string = StringListAllocate ( numElements );
	    p = list_struct->string;

	    for ( i=0; i<numElements; i++, p++ ) {
	    	sprintf ( buf, "%s:%s", list1[i], list2[i] );
	    	*p = strdup ( buf );
	    }
	}
/*
 *	Save the number of model layers.
 */
	if( verbose > VERBOSE_1 )
	    printf ("  numLayers = %d\n", numElements );
	SetNumberModelLayers ( mdl, numElements );
	CheckModelLevels();

/*
 *	Free. (list1 and 2 were malloc'd with numLayers items).
 */
	StringListFree ( numLayers, levels1 );
	StringListFree ( numLayers, levels2 );
	StringListFree ( numElements, list1 );
	StringListFree ( numElements, list2 );

	return NORMAL;
}


void
HSModelButtonCB (Widget    w,
                 XtPointer clientData,
                 XtPointer call_data )
/*
 * Called when a model option button is picked. Set the new model
 * and update date/time scrolled list.
 */
{
	GuiScalarGridDialogType	*grd;
	ModelInfoType		*mdl;
	WindowObjectType	*wo;
	KeyListType		*fdf;

	char			*file, *model, *label;
	char			**models, **labels;
	char			*chapter, *directory;
	int			len, plot_type, numkeys;
	int			iret, err, verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "HSModelButtonCB\n" );

	grd = GetGuiScalarGridDialog();
	mdl = GetGuiModelInfo();
	wo  = GetActiveWindowObject();
	fdf = GetHSGuiFdfInfo();

	SetupAbortProcess( wo );

/*
 *	Set model.
 */
	model = strdup ( bo->string );
	FreeModelType( mdl );
	SetModelType ( mdl, model );

/*
 *	Get model key and label list from configuration file.
 */
	iret = GetKeyList ( "modelkeys", "modellabels", ",",
                                   &numkeys, &models, &labels );

	label = GetStringArrayLabel ( numkeys, model,
                                          models, labels );
	FreeModelLabelType ( mdl );
	SetModelLabelType ( mdl, label );

/*
 *	Dynamically build dialog scrolled lists.
 */
	HSBuildScrolledLists ( model );

/*
 *	If an FDF with the same name but different parameters is now 
 *	highlighted, we need to reload this FDF to update the GUI.
 *
 *	Get directory to build FDF full path name.
 */
	plot_type = GetGridType ( mdl );
	file      = strdup ( GetModelFdfHS ( grd ) );

	switch ( plot_type ) {
	case SCALARGRIDOBJECT:
	  chapter = strdup ( GetModelScalarDir ( grd ) );
	  directory = GetConfigValue( "scalarfdf" );
	  break;
	case VECTORGRIDOBJECT:
	  chapter = strdup ( GetModelVectorDir ( grd ) );
	  directory = GetConfigValue( "vectorfdf" );
	  break;
	default:
	  return;
	}

/*
 *	Reload FDF.
 */
	GetModelField ( file, chapter, directory );

/*
 *	Free.
 */
	Free ( model );
	Free ( label );
	Free ( file );
	Free ( chapter );
	Free ( directory );
        StringListFree ( numkeys, models );
        StringListFree ( numkeys, labels );

	CancelAbortProcess( wo, False );

	return;
}


void
HSVcoordButtonCB (Widget    w,
                  XtPointer clientData,
                  XtPointer call_data )
/*
 * Called when a new vertical coordinate is chosen.
 */
{

	GuiScalarGridDialogType	*grd;
	ModelInfoType		*mdl;
	WindowObjectType	*wo;

	char			*vcoord;
	int			verbose;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "HSVcoordButtonCB\n" );

	grd = GetGuiScalarGridDialog();
	mdl = GetGuiModelInfo();
	wo = GetActiveWindowObject();

	SetupAbortProcess( wo );

/*
 *	Vertical coordinate.
 */
	vcoord = strdup ( bo->string );

/*
 *	Save model type.
 */
	FreeModelVCoord ( mdl );
	SetModelVCoord ( mdl, vcoord );

/*
 *	Set vertical coordinate widgets.
 */
	XmTextSetString ( GetModelVCoordHSW ( mdl ), vcoord );

/*
 *	Reset model levels.
 */
	SetModelLevels ();
	SetModelLayers ();

	Free ( vcoord );

	CancelAbortProcess( wo, False );

	return;
}


void
HSGridFillTableCB ( Widget w, XtPointer clientData, XtPointer xt_call_data )
/*
 * Option menu callback for setting the color fill table.
 */
{
	GuiScalarGridDialogType	*grd;
	int			verbose;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "ScalarGridFillTable\n" );

	grd = GetGuiScalarGridDialog();

/*
 *	Set color table.
 */
	FreeFillColorsHS ( grd );
	SetFillColorsHS ( grd, bo->string );
}


void
HSScalarFdfDirCB (Widget    w,
                  XtPointer clientData,
                  XtPointer call_data )
/*
 * Called when a new FDF chapter is chosen.
 */
{

	GuiScalarGridDialogType	*grd;
	ModelInfoType		*mdl;
	Widget			scalar_list;
	char			*model, *directory, *chapter;
	int			verbose;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "HSScalarFdfDirCB\n" );

	grd = GetGuiScalarGridDialog();
	mdl = GetGuiModelInfo();

	chapter = strdup ( bo->string );

/*
 *	Save FDF chapter.
 */
	FreeModelScalarDir ( grd );
	SetModelScalarDir ( grd, chapter );

/*
 *	Refill the scrolled list.
 */
	scalar_list = GetScrolledListHSW( grd );
	XtUnmanageChild ( scalar_list );
	RemoveProductsFromList ( scalar_list );
	
	model = strdup ( GetModelType ( mdl ) );
	directory = GetConfigValue( "scalarfdf" );
	BuildFieldList ( scalar_list, directory, chapter, model,
                                                   &(grd->field_list) );
	XtManageChild ( scalar_list );

/*
 *	Free.
 */
	Free ( model );
	Free ( chapter );
	Free ( directory );

	return;
}


void
HSVectorFdfDirCB (Widget    w,
                  XtPointer clientData,
                  XtPointer call_data )
/*
 * Called when a new FDF chapter is chosen.
 */
{

	GuiScalarGridDialogType	*grd;
	GuiVectorGridDialogType	*vec;
	ModelInfoType		*mdl;
	Widget			vector_list;
	char			*model, *chapter, *directory;
	int			verbose;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "HSVectorFdfDirCB\n" );

	grd = GetGuiScalarGridDialog();
	vec = GetGuiVectorGridDialog();
	mdl = GetGuiModelInfo();

	chapter = strdup ( bo->string );

/*
 *	Save FDF chapter.
 */
	FreeModelVectorDir ( grd );
	SetModelVectorDir ( grd, chapter );

/*
 *	Refill the scrolled list.
 */
	vector_list = GetScrolledListHVW( vec );
	XtUnmanageChild ( vector_list );
	RemoveProductsFromList ( vector_list );
	
	model = strdup ( GetModelType ( mdl ) );
	directory = GetConfigValue( "vectorfdf" );
	BuildFieldList ( vector_list, directory, chapter, model,
                                                   &(vec->field_list) );
	XtManageChild ( vector_list );

	Free ( model );
	Free ( chapter );
	Free ( directory );

	return;
}


void
HSWindSymbolCB ( Widget    w,
                 XtPointer clientData,
                 XtPointer call_data )
/*
 * Called when a model wind symbol button is picked.
 */
{
	GuiScalarGridDialogType	*grd;

	char			*symbol;
	int			verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "HSWindSymbolCB\n" );

	grd = GetGuiScalarGridDialog();

	symbol = strdup ( bo->string );

	FreeVectorSymbolHS ( grd );
	SetVectorSymbolHS ( grd, symbol );

	Free ( symbol );
	return;
}


void
HSLineTypeCB ( Widget    w,
               XtPointer clientData,
               XtPointer call_data )
/*
 * Called when a model line type button is picked.
 */
{
	GuiScalarGridDialogType	*grd;

	char			*linetype;
	int			verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "HSLineTypeCB\n" );

	grd = GetGuiScalarGridDialog();

	linetype = strdup ( bo->string );

	FreeLineTypeHS ( grd );
	SetLineTypeHS ( grd, linetype );

	Free ( linetype );
	return;
}


void
HSLineLabelFrequencyCB ( Widget    w,
                         XtPointer clientData,
                         XtPointer call_data )
/*
 * Called when a model wind symbol button is picked.
 */
{
	GuiScalarGridDialogType	*grd;

	char			*labelfreq;
	int			verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "HSLineLabelFrequencyCB\n" );

	grd = GetGuiScalarGridDialog();

	labelfreq = strdup ( bo->string );

	FreeLabelFrequencyHS ( grd );
	SetLabelFrequencyHS ( grd, labelfreq );

	Free ( labelfreq );
	return;
}


void
SetHSMacro ( KeyListType *fdf, char *file )
{
/*
 * The current FDF is a macro so change GUI to reflect a different mode.
 */
	GuiScalarGridDialogType	*grd;
	XmString		xmstr;
	char			*buf="MACRO";


	grd = GetGuiScalarGridDialog();

/*
 *	Set function textfield widget.
 */
        FreeFunctionHS ( grd );
	SetFunctionHS ( grd, buf );
	XmTextSetString ( GetFunctionHSW ( grd ), GetFunctionHS ( grd ) );

/*
 *	Set label.
 */
	FreeModelFdfHS ( grd );
	SetModelFdfHS ( grd, file );

	xmstr = XmStringLtoRCreate ( buf, XmSTRING_DEFAULT_CHARSET );
	XtVaSetValues ( GetModelFdfHSW ( grd ),
			XmNlabelString, xmstr,
			NULL);
	XmStringFree ( xmstr );

}
