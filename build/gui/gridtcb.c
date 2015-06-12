/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gridtcb.c
 *
 *	Cross section callbacks for dynamically initialized widgets.	
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 3/97	COMET	Added time axis callback.
 *	 7/97	COMET	Added model defined subdirectories.
 *	10/97	COMET	Added macro functionality.
 *	11/97	COMET	Save the button label for a model.
 *	12/97	COMET	Fix leaks related to builddirpath
 *	 2/98	COMET	Fix a bug that crashed GARP when changing model.
 *	 2/98	COMET	Added code to allow for model dependent macros.
 *	 8/99	COMET	Redefined FDF model directory.
 *
 ***********************************************************************/

#include "geminc.h"
#include "gemprm.h"
#include "utils.h"
#include "ctbcmn.h"
#include "guimacros.h"
#include "thgtobj.h"
#include "model.h"
#include "fdfobj.h"
#include "genglobs.h"
#include "winobj.h"
#include "menucb.h"
#include "_proto.h"

char * GetStringArrayLabel ( int, char *, char **, char ** );


void
SetTHModelGridTypeCB ( Widget w, XtPointer clientData, XtPointer call_data )
/*
 * This function sets the plot type to SCALAROBJECT or VECTOROBJECT when
 * an FDF is chosen for model grids. It also activates the appropriate
 * radio button. This routine is used both as a callback and as a general
 * use routine so the widget id may not be that of either the scalar or
 * vector radio buttons.
 */
{
	Widget			scalar_b, vector_b;
	GuiTimeHeightDialogType	*tht;
	int			type;

	type = (int) clientData;

	tht = GetGuiTimeHeightDialog();

/*
 *	Set grid type to SCALAROBJECT or VECTOROBJECT.
 */
	SetTHGridType ( tht, type );

/*
 *	Set radio button to either Scalar or Vector.
 */
	scalar_b = GetModelScalarButtonTHW ( tht );
	vector_b = GetModelVectorButtonTHW ( tht );

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
THModelButtonCB (Widget    w,
                 XtPointer clientData,
                 XtPointer xt_call_data )
/*
 * Called when a model option button is picked. Set the new model
 * and update date/time and FDF scrolled lists.
 */
{
	Widget			forecast_list;
	GuiTimeHeightDialogType	*tht;
	WindowObjectType	*wo;
	KeyListType		*fdf;

	char			*file, *model, *label;
	char			**models, **labels;
	char			*chapter, *directory;
	int			len, plot_type, numkeys;
	int			iret, err, verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "THModelButtonCB\n" );

	tht = GetGuiTimeHeightDialog();
	wo  = GetActiveWindowObject();
	fdf = GetTHGuiFdfInfo();

	SetupAbortProcess( wo );

/*
 *	Model.
 */
	model = strdup ( bo->string );
	FreeModelTypeTH ( tht );
	SetModelTypeTH ( tht, model );

/*
 *	Get model key and label list from configuration file.
 */
	iret = GetKeyList ( "modelkeys", "modellabels", ",",
                                   &numkeys, &models, &labels );

	label = GetStringArrayLabel ( numkeys, model,
                                          models, labels );
	FreeModelLabelTypeTH ( tht );
	SetModelLabelTypeTH ( tht, label );

/*
 *	Dynamically build dialog scrolled lists.
 */
	THBuildScrolledLists ( model );

/*
 *	Update FDF.
 */
	plot_type = GetTHGridType ( tht );
	file      = strdup ( GetModelFdfTH ( tht ) );

	switch ( plot_type ) {
	case SCALARGRIDOBJECT:
	  chapter = strdup ( GetModelTHScalarDir ( tht ) );
	  directory = GetConfigValue( "thscalarfdf" );
	  break;
	case VECTORGRIDOBJECT:
	  chapter = strdup ( GetModelTHVectorDir ( tht ) );
	  directory = GetConfigValue( "thvectorfdf" );
	  break;
	default:
	  CancelAbortProcess( wo, False );
	  return;
	}
/*
 *	Reload FDF.
 */
	GetTHModelField ( file, chapter, directory );

/*
 *	Free.
 */
	Free ( model );
	Free ( label );
	Free ( file );
	Free ( directory );
	Free ( chapter );
        StringListFree ( numkeys, models );
        StringListFree ( numkeys, labels );

	CancelAbortProcess( wo, False );

	return;
}


void
THVcoordButtonCB (Widget    w,
                  XtPointer clientData,
                  XtPointer xt_call_data )
/*
 * Called when a new vertical coordinate is chosen.
 */
{

	GuiTimeHeightDialogType	*tht;
	WindowObjectType	*wo;

	char			*vcoord;
	int			verbose;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "THVcoordButtonCB\n" );

	tht = GetGuiTimeHeightDialog();
	wo = GetActiveWindowObject();

	SetupAbortProcess( wo );
/*
 *	Vertical coordinate.
 */
	vcoord = strdup ( bo->string );

	XmTextSetString ( GetModelVCoordTHW ( tht ), vcoord );

	Free ( vcoord );

	CancelAbortProcess( wo, False );

	return;
}


void
THScalarFdfDirCB (Widget    w,
                  XtPointer clientData,
                  XtPointer xt_call_data )
/*
 * Called when a new FDF chapter is chosen.
 */
{

	GuiTimeHeightDialogType	*tht;
	Widget			scalar_list;
	char			*model, *chapter, *directory;
	int			verbose;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "THScalarFdfDirCB\n" );

	tht = GetGuiTimeHeightDialog();

	chapter = strdup ( bo->string );

/*
 *	Save FDF chapter.
 */
	FreeModelTHScalarDir ( tht );
	SetModelTHScalarDir ( tht, chapter );

/*
 *	Refill the scrolled list.
 */
	scalar_list = GetScalarListTHW( tht );
	XtUnmanageChild ( scalar_list );
	RemoveProductsFromList ( scalar_list );
	
	model = strdup ( GetModelTypeTH ( tht ) );
	directory = GetConfigValue( "thscalarfdf" );
	BuildFieldList ( scalar_list, directory, chapter, model,
						&(tht->scalar_list) );
	XtManageChild ( scalar_list );

	Free ( model );
	Free ( chapter );
	Free ( directory );

	return;
}


void
THVectorFdfDirCB (Widget    w,
                  XtPointer clientData,
                  XtPointer xt_call_data )
/*
 * Called when a new FDF chapter is chosen.
 */
{

	GuiTimeHeightDialogType	*tht;
	Widget			vector_list;
	char			*model, *chapter, *directory;
	int			verbose;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "THVectorFdfDirCB\n" );

	tht = GetGuiTimeHeightDialog();

	chapter = strdup ( bo->string );

/*
 *	Save FDF chapter.
 */
	FreeModelTHVectorDir ( tht );
	SetModelTHVectorDir ( tht, chapter );

/*
 *	Refill the scrolled list.
 */
	vector_list = GetVectorListTHW( tht );
	XtUnmanageChild ( vector_list );
	RemoveProductsFromList ( vector_list );
	
	model = strdup ( GetModelTypeTH ( tht ) );
	directory = GetConfigValue( "thvectorfdf" );
	BuildFieldList ( vector_list, directory, chapter, model,
						&(tht->vector_list) );
	XtManageChild ( vector_list );

	Free ( model );
	Free ( chapter );
	Free ( directory );

	return;
}


void
THWindSymbolCB ( Widget    w,
                 XtPointer clientData,
                 XtPointer call_data )
/*
 * Called when a model wind symbol button is picked.
 */
{
	GuiTimeHeightDialogType	*tht;

	char			*symbol;
	int			verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "THWindSymbolCB\n" );

	tht = GetGuiTimeHeightDialog();

	symbol = strdup ( bo->string );

	FreeVectorSymbolTH ( tht );
	SetVectorSymbolTH ( tht, symbol );

	Free ( symbol );
	return;
}


void
THLineTypeCB ( Widget    w,
               XtPointer clientData,
               XtPointer call_data )
/*
 * Called when a model line type button is picked.
 */
{
	GuiTimeHeightDialogType	*tht;

	char			*linetype;
	int			verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "THLineTypeCB\n" );

	tht = GetGuiTimeHeightDialog();

	linetype = strdup ( bo->string );

	FreeLineTypeTH ( tht );
	SetLineTypeTH ( tht, linetype );

	Free ( linetype );
	return;
}


void
THLineLabelFrequencyCB ( Widget    w,
                         XtPointer clientData,
                         XtPointer call_data )
/*
 * Called when a model wind symbol button is picked.
 */
{
	GuiTimeHeightDialogType	*tht;

	char			*labelfreq;
	int			verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "THLineLabelFrequencyCB\n" );

	tht = GetGuiTimeHeightDialog();

	labelfreq = strdup ( bo->string );

	FreeLabelFrequencyTH ( tht );
	SetLabelFrequencyTH ( tht, labelfreq );

	Free ( labelfreq );
	return;
}


void
THVerticalAxisCB ( Widget    w,
                   XtPointer clientData,
                   XtPointer call_data )
/*
 * Called when a model wind symbol button is picked.
 */
{
	GuiTimeHeightDialogType	*tht;

	char			*axis_type;
	int			verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "THVerticalAxisCB\n" );

	tht = GetGuiTimeHeightDialog();

	axis_type = strdup ( bo->string );

	FreeAxisTypeTH ( tht );
	SetAxisTypeTH ( tht, axis_type );

	Free ( axis_type );
	return;
}


void
THTimeAxisCB ( Widget    w,
               XtPointer clientData,
               XtPointer call_data )
{
/*
 * Called when a time axis direction is picked.
 */
	GuiTimeHeightDialogType	*tht;

	char			*direction;
	int	 		verbose;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if ( verbose > VERBOSE_0 )
	    printf ( "THTimeAxisCB\n" );

	tht = GetGuiTimeHeightDialog();

	direction = strdup ( bo->string );
	if ( verbose > VERBOSE_1 )
	    printf ( "  direction = %s\n", direction );

	FreeTimeAxisTH ( tht );
	SetTimeAxisTH ( tht, direction );

	Free ( direction );
	return;
} 


void
SetTHMacro ( KeyListType *fdf, char *file )
{
/*
 * The current FDF is a macro so change GUI to reflect a different mode.
 */
	GuiTimeHeightDialogType	*tht;
	XmString		xmstr;
	char			*buf="MACRO";


	tht = GetGuiTimeHeightDialog();

/*
 *	Set function textfield widget.
 */
        FreeFunctionTH ( tht );
	SetFunctionTH ( tht, buf );
	XmTextSetString ( GetFunctionTHW ( tht ), GetFunctionTH ( tht ) );

/*
 *	Set label.
 */
	FreeModelFdfTH ( tht );
	SetModelFdfTH ( tht, file );

	xmstr = XmStringLtoRCreate ( buf,
                                        XmSTRING_DEFAULT_CHARSET );
	XtVaSetValues ( GetModelFdfTHW ( tht ),
			XmNlabelString, xmstr,
			NULL);
	XmStringFree ( xmstr );

}


void
RunTHMacro ( char *directory )
{
	GuiTimeHeightDialogType	*tht;

	KeyListType		*fdf, *macro=NULL;
	char			*subdir, *model;
	char			*current_directory;
	char			*plot_type_key;
	char			chapter[81];
	char			keyName[1024], value[1024];
	int			i, err, plot_type;


	tht = GetGuiTimeHeightDialog();
	fdf = GetTHGuiFdfInfo();

/*
 *	Save default directory in case the directory is redefined in a
 *	macro.
 */
	current_directory = strdup ( directory );

/*
 *	Read macro into a key list which allows for duplicate 
 *	entries.
 */
	if ( GetTHGridType ( tht ) == SCALARGRIDOBJECT )
	    subdir = strdup ( GetModelTHScalarDir ( tht ) );
	else
	    subdir = strdup ( GetModelTHVectorDir ( tht ) );

	model = strdup ( GetModelTypeTH ( tht ) );
	if ( err = GetMacro ( fdf->fileName, fdf->path,
	           subdir, model, &macro ) ) return;

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
		SetTHFdfFallbackValues ( fdf );
	    	SetTHGridWidgets ( fdf, fdf->fileName );

		BuildTHGrid();

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
		GetTHModelField ( macro->keys[i].value, chapter,
		                  current_directory );
		fdf = GetTHGuiFdfInfo();
/*
 *	Save plot type.
 */
		plot_type_key = GetFdfKeyValue ( fdf, "type" );
		ToUpper ( plot_type_key );
		plot_type = SCALARGRIDOBJECT;
		if ( strcmp ( plot_type_key, "VECTOR" ) == 0 ) 
			plot_type = VECTORGRIDOBJECT;
		SetTHGridType ( tht, plot_type );
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
