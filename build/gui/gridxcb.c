/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gridxcb.c
 *
 *	Cross section callbacks for dynamically initialized widgets.	
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 7/97	COMET	Added model defined subdirectories.
 *	 9/97	COMET	Added macro functionality.
 *	11/97	COMET	Save the button label for a model.
 *	12/97	COMET	Fix leaks related to builddirpath()
 *	 2/98	COMET	Added code to allow for model dependent macros.
 *	 8/99	COMET	Redefined FDF model directory.
 *
 ***********************************************************************/

#include "geminc.h"
#include "gemprm.h"
#include "utils.h"
#include "ctbcmn.h"
#include "guimacros.h"
#include "xsecobj.h"
#include "model.h"
#include "fdfobj.h"
#include "genglobs.h"
#include "winobj.h"
#include "menucb.h"
#include "_proto.h"

char * GetStringArrayLabel ( int, char *, char **, char ** );


void
SetXSModelGridTypeCB ( Widget w, XtPointer clientData, XtPointer call_data )
/*
 * This function sets the plot type to SCALAROBJECT or VECTOROBJECT when
 * an FDF is chosen for model grids. It also activates the appropriate
 * radio button. This routine is used both as a callback and as a general
 * use routine so the widget id may not be that of either the scalar or
 * vector radio buttons.
 */
{
	Widget			scalar_b, vector_b;
	GuiXSectionDialogType	*vcs;
	int			type;

	type = (int) clientData;

	vcs = GetGuiXSectionDialog();

/*
 *	Set grid type to SCALAROBJECT or VECTOROBJECT.
 */
	SetXSGridType ( vcs, type );

/*
 *	Set radio button to either Scalar or Vector.
 */
	scalar_b = GetModelScalarButtonXSW ( vcs );
	vector_b = GetModelVectorButtonXSW ( vcs );

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
XSModelButtonCB (Widget    w,
                 XtPointer clientData,
                 XtPointer xt_call_data )
/*
 * Called when a model option button is picked. Set the new model
 * and update date/time scrolled list.
 */
{
	Widget			forecast_list;
	GuiXSectionDialogType	*vcs;
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
	    printf ( "XSModelButtonCB\n" );

	vcs = GetGuiXSectionDialog();
	wo  = GetActiveWindowObject();
	fdf = GetXSGuiFdfInfo();

	SetupAbortProcess( wo );

/*
 *	Set model.
 */
	model = strdup ( bo->string );
	FreeModelTypeXS ( vcs );
	SetModelTypeXS ( vcs, model );

/*
 *	Get model key and label list from configuration file.
 */
	iret = GetKeyList ( "modelkeys", "modellabels", ",",
                                   &numkeys, &models, &labels );

	label = GetStringArrayLabel ( numkeys, model,
                                          models, labels );
	FreeModelLabelTypeXS ( vcs );
	SetModelLabelTypeXS ( vcs, label );

/*
 *	Dynamically build dialog scrolled lists.
 */
	XSBuildScrolledLists ( model );

/*
 *	Update FDF.
 */
	plot_type = GetXSGridType ( vcs );
	file      = strdup ( GetModelFdfXS ( vcs ) );

	switch ( plot_type ) {
	case SCALARGRIDOBJECT:
	  chapter = strdup ( GetModelXSScalarDir ( vcs ) );
	  directory = GetConfigValue( "xsscalarfdf" );
	  break;
	case VECTORGRIDOBJECT:
	  chapter = strdup ( GetModelXSVectorDir ( vcs ) );
	  directory = GetConfigValue( "xsvectorfdf" );
	  break;
	default:
	  CancelAbortProcess( wo, False );
	  return;
	}
/*
 *	Reload FDF.
 */
	GetXSModelField ( file, chapter, directory );

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
XSVcoordButtonCB (Widget    w,
                  XtPointer clientData,
                  XtPointer xt_call_data )
/*
 * Called when a new vertical coordinate is chosen.
 */
{

	GuiXSectionDialogType	*vcs;
	WindowObjectType	*wo;

	char			*vcoord;
	int			verbose;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "XSVcoordButtonCB\n" );

	vcs = GetGuiXSectionDialog();
	wo = GetActiveWindowObject();

	SetupAbortProcess( wo );

/*
 *	Vertical coordinate.
 */
	vcoord = strdup ( bo->string );

	XmTextSetString ( GetModelVCoordXSW ( vcs ), vcoord );

	Free ( vcoord );

	CancelAbortProcess( wo, False );

	return;
}


void
XSGridFillTableCB ( Widget w, XtPointer clientData, XtPointer xt_call_data )
/*
 * Option menu callback for setting the color fill table.
 */
{
	GuiXSectionDialogType	*vcs;
	int			verbose;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "XSGridFillTableCB\n" );

	vcs = GetGuiXSectionDialog();

/*
 *	Set color table.
 */
	FreeFillColorsXS ( vcs );
	SetFillColorsXS ( vcs, bo->string );
}


void
XSScalarFdfDirCB (Widget    w,
                  XtPointer clientData,
                  XtPointer xt_call_data )
/*
 * Called when a new FDF chapter is chosen.
 */
{

	GuiXSectionDialogType	*vcs;
	Widget			scalar_list;
	char			*model, *chapter, *directory;
	int			verbose;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "XSScalarFdfDirCB\n" );

	vcs = GetGuiXSectionDialog();

	chapter = strdup ( bo->string );

/*
 *	Save FDF chapter.
 */
	FreeModelXSScalarDir ( vcs );
	SetModelXSScalarDir ( vcs, chapter );

/*
 *	Refill the scrolled list.
 */
	scalar_list = GetScalarListXSW( vcs );
	XtUnmanageChild ( scalar_list );
	RemoveProductsFromList ( scalar_list );
	
	model = strdup ( GetModelTypeXS ( vcs ) );
	directory = GetConfigValue( "xsscalarfdf" );
	BuildFieldList ( scalar_list, directory, chapter, model,
						&(vcs->scalar_list) );
	XtManageChild ( scalar_list );

	Free ( model );
	Free ( chapter );
	Free ( directory );

	return;
}


void
XSVectorFdfDirCB (Widget    w,
                  XtPointer clientData,
                  XtPointer xt_call_data )
/*
 * Called when a new FDF chapter is chosen.
 */
{

	GuiXSectionDialogType	*vcs;
	Widget			vector_list;
	char			*model, *chapter, *directory;
	int			verbose;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "XSVectorFdfDirCB\n" );

	vcs = GetGuiXSectionDialog();

	chapter = strdup ( bo->string );

/*
 *	Save FDF chapter.
 */
	FreeModelXSVectorDir ( vcs );
	SetModelXSVectorDir ( vcs, chapter );

/*
 *	Refill the scrolled list.
 */
	vector_list = GetVectorListXSW( vcs );
	XtUnmanageChild ( vector_list );
	RemoveProductsFromList ( vector_list );
	
	model = strdup ( GetModelTypeXS ( vcs ) );
	directory = GetConfigValue( "xsvectorfdf" );
	BuildFieldList ( vector_list, directory, chapter, model,
						&(vcs->vector_list) );
	XtManageChild ( vector_list );

	Free ( model );
	Free ( chapter );
	Free ( directory );

	return;
}


void
XSWindSymbolCB ( Widget    w,
                 XtPointer clientData,
                 XtPointer call_data )
/*
 * Called when a model wind symbol button is picked.
 */
{
	GuiXSectionDialogType	*vcs;

	char			*symbol;
	int			verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "XSWindSymbolCB\n" );

	vcs = GetGuiXSectionDialog();

	symbol = strdup ( bo->string );

	FreeVectorSymbolXS ( vcs );
	SetVectorSymbolXS ( vcs, symbol );

	Free ( symbol );
	return;
}


void
XSLineTypeCB ( Widget    w,
               XtPointer clientData,
               XtPointer call_data )
/*
 * Called when a model line type button is picked.
 */
{
	GuiXSectionDialogType	*vcs;

	char			*linetype;
	int			verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "XSLineTypeCB\n" );

	vcs = GetGuiXSectionDialog();

	linetype = strdup ( bo->string );

	FreeLineTypeXS ( vcs );
	SetLineTypeXS ( vcs, linetype );

	Free ( linetype );
	return;
}


void
XSLineLabelFrequencyCB ( Widget    w,
                         XtPointer clientData,
                         XtPointer call_data )
/*
 * Called when a model wind symbol button is picked.
 */
{
	GuiXSectionDialogType	*vcs;

	char			*labelfreq;
	int			verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "XSLineLabelFrequencyCB\n" );

	vcs = GetGuiXSectionDialog();

	labelfreq = strdup ( bo->string );

	FreeLabelFrequencyXS ( vcs );
	SetLabelFrequencyXS ( vcs, labelfreq );

	Free ( labelfreq );
	return;
}


void
XSVerticalAxisCB ( Widget    w,
                   XtPointer clientData,
                   XtPointer call_data )
/*
 * Called when a model wind symbol button is picked.
 */
{
	GuiXSectionDialogType	*vcs;

	char			*axis_type;
	int			verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "XSVerticalAxisCB\n" );

	vcs = GetGuiXSectionDialog();

	axis_type = strdup ( bo->string );

	FreeAxisTypeXS ( vcs );
	SetAxisTypeXS ( vcs, axis_type );

	Free ( axis_type );
	return;
}


void
SetXSMacro ( KeyListType *fdf, char *file )
{
/*
 * The current FDF is a macro so change GUI to reflect a different mode.
 */
	GuiXSectionDialogType	*vcs;
	XmString		xmstr;
	char			*buf="MACRO";


	vcs = GetGuiXSectionDialog();

/*
 *	Set function textfield widget.
 */
        FreeFunctionXS ( vcs );
	SetFunctionXS ( vcs, buf );
	XmTextSetString ( GetFunctionXSW ( vcs ), GetFunctionXS ( vcs ) );

/*
 *	Set label.
 */
	FreeModelFdfXS ( vcs );
	SetModelFdfXS ( vcs, file );

	xmstr = XmStringLtoRCreate ( buf, XmSTRING_DEFAULT_CHARSET );
	XtVaSetValues ( GetModelFdfXSW ( vcs ),
			XmNlabelString, xmstr,
			NULL);
	XmStringFree ( xmstr );

}


void
RunXSMacro ( char *directory )
{
	GuiXSectionDialogType	*vcs;

	KeyListType		*fdf, *macro=NULL;
	char			*subdir, *model;
	char			*current_directory;
	char			*plot_type_key;
	char			chapter[81];
	char			keyName[1024], value[1024];
	int			i, err, plot_type;


	vcs = GetGuiXSectionDialog();
	fdf = GetXSGuiFdfInfo();

/*
 *	Save default directory in case the directory is redefined in a
 *	macro.
 */
	current_directory = strdup ( directory );

/*
 *	Read macro into a key list which allows for duplicate 
 *	entries.
 */
	if ( GetXSGridType ( vcs ) == SCALARGRIDOBJECT )
	    subdir = strdup ( GetModelXSScalarDir ( vcs ) );
	else
	    subdir = strdup ( GetModelXSVectorDir ( vcs ) );

	model = strdup ( GetModelTypeXS ( vcs ) );
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
		SetXSFdfFallbackValues ( fdf );
	    	SetXSGridWidgets ( fdf, fdf->fileName );

		BuildXSGrid();

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
		GetXSModelField ( macro->keys[i].value, chapter,
		                  current_directory );
		fdf = GetXSGuiFdfInfo();
/*
 *	Save plot type.
 */
		plot_type_key = GetFdfKeyValue ( fdf, "type" );
		ToUpper ( plot_type_key );
		plot_type = SCALARGRIDOBJECT;
		if ( strcmp ( plot_type_key, "VECTOR" ) == 0 ) 
			plot_type = VECTORGRIDOBJECT;
		SetXSGridType ( vcs, plot_type );
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
