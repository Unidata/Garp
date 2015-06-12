/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gridvcb.c
 *
 *	Vertical profile callbacks for dynamically initialized widgets.	
 *
 *	History:
 *
 *	12/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 3/97	COMET	Desensitize FDF's when plotting SkewT or Stuve.
 *	 7/97	COMET	Added model defined subdirectories.
 *	10/97	COMET	Added macro functionality.
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
#include "vprofobj.h"
#include "model.h"
#include "fdfobj.h"
#include "genglobs.h"
#include "winobj.h"
#include "menucb.h"
#include "_proto.h"

char * GetStringArrayLabel ( int, char *, char **, char ** );


void
SetVPModelGridTypeCB ( Widget w, XtPointer clientData, XtPointer call_data )
/*
 * This function sets the plot type to SCALAROBJECT or VECTOROBJECT when
 * an FDF is chosen for model grids. It also activates the appropriate
 * radio button. This routine is used both as a callback and as a general
 * use routine so the widget id may not be that of either the scalar or
 * vector radio buttons.
 */
{
	Widget			scalar_b, vector_b;
	GuiVertProfileObjectType	*vpt;
	int			type;

	type = (int) clientData;

	vpt = GetGuiVertProfileDialog();

/*
 *	Set grid type to SCALAROBJECT or VECTOROBJECT.
 */
	SetVPGridType ( vpt, type );

/*
 *	Set radio button to either Scalar or Vector.
 */
	scalar_b = GetModelScalarButtonVPW ( vpt );
	vector_b = GetModelVectorButtonVPW ( vpt );

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
VPModelButtonCB (Widget    w,
                 XtPointer clientData,
                 XtPointer xt_call_data )
/*
 * Called when a model option button is picked. Set the new model
 * and update date/time scrolled list.
 */
{
	Widget				forecast_list;
	GuiVertProfileObjectType	*vpt;
	WindowObjectType		*wo;
	KeyListType			*fdf;

	char				*file, *model, *label;
	char				**models, **labels;
	char				*chapter, *directory;
	int				len, plot_type, numkeys;
	int				iret, err, verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "VPModelButtonCB\n" );

	vpt = GetGuiVertProfileDialog();
	wo = GetActiveWindowObject();
	fdf = GetVPGuiFdfInfo();

	SetupAbortProcess( wo );

/*
 *	Model.
 */
	model = strdup ( bo->string );
	FreeModelTypeVP ( vpt );
	SetModelTypeVP ( vpt, model );

/*
 *	Get model key and label list from configuration file.
 */
	iret = GetKeyList ( "modelkeys", "modellabels", ",",
                                   &numkeys, &models, &labels );

	label = GetStringArrayLabel ( numkeys, model,
                                          models, labels );
	FreeModelLabelTypeVP ( vpt );
	SetModelLabelTypeVP ( vpt, label );

/*
 *	Dynamically build dialog scrolled lists.
 */
	VPBuildScrolledLists ( model );

/*
 *	Update FDF.
 */
	plot_type = GetVPGridType ( vpt );
	file      = strdup ( GetModelFdfVP ( vpt ) );

	switch ( plot_type ) {
	case SCALARGRIDOBJECT:
	  chapter = strdup ( GetModelVPScalarDir ( vpt ) );
	  directory = GetConfigValue( "vpscalarfdf" );
	  break;
	case VECTORGRIDOBJECT:
	  chapter = strdup ( GetModelVPVectorDir ( vpt ) );
	  directory = GetConfigValue( "vpvectorfdf" );
	  break;
	default:
	  CancelAbortProcess( wo, False );
	  return;
	}
/*
 *	Reload FDF.
 */
	GetVPModelField ( file, chapter, directory );

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
VPVcoordButtonCB (Widget    w,
                  XtPointer clientData,
                  XtPointer xt_call_data )
/*
 * Called when a new vertical coordinate is chosen.
 */
{

	GuiVertProfileObjectType	*vpt;
	WindowObjectType		*wo;

	char				*vcoord;
	int				verbose;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "VPVcoordButtonCB\n" );

	vpt = GetGuiVertProfileDialog();
	wo = GetActiveWindowObject();

	SetupAbortProcess( wo );
/*
 *	Vertical coordinate.
 */
	vcoord = strdup ( bo->string );

	XmTextSetString ( GetModelVCoordVPW ( vpt ), vcoord );

	Free ( vcoord );

	CancelAbortProcess( wo, False );

	return;
}


void
VPScalarFdfDirCB (Widget    w,
                  XtPointer clientData,
                  XtPointer xt_call_data )
/*
 * Called when a new FDF chapter is chosen.
 */
{

	GuiVertProfileObjectType	*vpt;
	Widget			scalar_list;
	char			*model, *chapter, *directory;
	int			verbose;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "VPScalarFdfDirCB\n" );

	vpt = GetGuiVertProfileDialog();

	chapter = strdup ( bo->string );

/*
 *	Save FDF chapter.
 */
	FreeModelVPScalarDir ( vpt );
	SetModelVPScalarDir ( vpt, chapter );

/*
 *	Refill the scrolled list.
 */
	scalar_list = GetScalarListVPW( vpt );
	XtUnmanageChild ( scalar_list );
	RemoveProductsFromList ( scalar_list );

	model = strdup ( GetModelTypeVP ( vpt ) );
	directory = GetConfigValue( "vpscalarfdf" );
	BuildFieldList ( scalar_list, directory, chapter, model,
						&(vpt->scalar_list) );
	XtManageChild ( scalar_list );

	Free ( model );
	Free ( chapter );
	Free ( directory );

	return;
}


void
VPVectorFdfDirCB (Widget    w,
                  XtPointer clientData,
                  XtPointer xt_call_data )
/*
 * Called when a new FDF chapter is chosen.
 */
{

	GuiVertProfileObjectType	*vpt;
	Widget			vector_list;
	char			*model, *chapter, *directory;
	int			verbose;

	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "VPVectorFdfDirCB\n" );

	vpt = GetGuiVertProfileDialog();

	chapter = strdup ( bo->string );

/*
 *	Save FDF chapter.
 */
	FreeModelVPVectorDir ( vpt );
	SetModelVPVectorDir ( vpt, chapter );

/*
 *	Refill the scrolled list.
 */
	vector_list = GetVectorListVPW( vpt );
	XtUnmanageChild ( vector_list );
	RemoveProductsFromList ( vector_list );
	
	model = strdup ( GetModelTypeVP ( vpt ) );
	directory = GetConfigValue( "vpvectorfdf" );
	BuildFieldList ( vector_list, directory, chapter, model,
						&(vpt->vector_list) );
	XtManageChild ( vector_list );

	Free ( model );
	Free ( chapter );
	Free ( directory );

	return;
}


void
VPWindSymbolCB ( Widget    w,
                 XtPointer clientData,
                 XtPointer call_data )
/*
 * Called when a model wind symbol button is picked.
 */
{
	GuiVertProfileObjectType	*vpt;

	char			*symbol;
	int			verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "VPWindSymbolCB\n" );

	vpt = GetGuiVertProfileDialog();

	symbol = strdup ( bo->string );

	FreeVectorSymbolVP ( vpt );
	SetVectorSymbolVP ( vpt, symbol );

	Free ( symbol );
	return;
}


void
VPLineTypeCB ( Widget    w,
               XtPointer clientData,
               XtPointer call_data )
/*
 * Called when a model line type button is picked.
 */
{
	GuiVertProfileObjectType	*vpt;

	char			*linetype;
	int			verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "VPLineTypeCB\n" );

	vpt = GetGuiVertProfileDialog();

	linetype = strdup ( bo->string );

	FreeLineTypeVP ( vpt );
	SetLineTypeVP ( vpt, linetype );

	Free ( linetype );
	return;
}


void
CheckYaxisTypeVP ( GuiVertProfileObjectType *vpt, char *axis_type )
{
/*
 * Desensitize FDF lists for thermodynamic diagrams.
 */

	if ( axis_type == NULL ) return;

	if ( strcasecmp ( axis_type, "skewt" ) == 0 ||
	     strcasecmp ( axis_type, "stuve" ) == 0 ) {
	    XtSetSensitive ( GetScalarListVPW ( vpt ), FALSE );
	    XtSetSensitive ( GetVectorListVPW ( vpt ), FALSE );
	    XtSetSensitive ( GetFunctionVPW ( vpt ), FALSE );
	    XtSetSensitive ( GetModelFdfVPW ( vpt ), FALSE );
	}
	else {
	    XtSetSensitive ( GetScalarListVPW ( vpt ), TRUE );
	    XtSetSensitive ( GetVectorListVPW ( vpt ), TRUE );
	    XtSetSensitive ( GetFunctionVPW ( vpt ), TRUE );
	    XtSetSensitive ( GetModelFdfVPW ( vpt ), TRUE );
	}
}


void
VPVerticalAxisCB ( Widget    w,
                   XtPointer clientData,
                   XtPointer call_data )
/*
 * Called when a new vertical axis is chosen.
 */
{
	GuiVertProfileObjectType	*vpt;

	char			*axis_type;
	int			verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "VPVerticalAxisCB\n" );

	vpt = GetGuiVertProfileDialog();

	axis_type = strdup ( bo->string );

	FreeAxisTypeVP ( vpt );
	SetAxisTypeVP ( vpt, axis_type );

/*
 * 	Desensitize FDF lists for thermodynamic diagrams.
 */
	CheckYaxisTypeVP ( vpt, axis_type );

	Free ( axis_type );
	return;
}


void
SetVPMacro ( KeyListType *fdf, char *file )
{
/*
 * The current FDF is a macro so change GUI to reflect a different mode.
 */
	GuiVertProfileObjectType	*vpt;
	XmString			xmstr;
	char				*buf="MACRO";


	vpt = GetGuiVertProfileDialog();

/*
 *	Set function textfield widget.
 */
        FreeFunctionVP ( vpt );
	SetFunctionVP ( vpt, buf );
	XmTextSetString ( GetFunctionVPW ( vpt ), GetFunctionVP ( vpt ) );

/*
 *	Set label.
 */
	FreeModelFdfVP ( vpt );
	SetModelFdfVP ( vpt, file );

	xmstr = XmStringLtoRCreate ( buf, XmSTRING_DEFAULT_CHARSET );
	XtVaSetValues ( GetModelFdfVPW ( vpt ),
			XmNlabelString, xmstr,
			NULL);
	XmStringFree ( xmstr );

}


void
RunVPMacro ( char *directory )
{
	GuiVertProfileObjectType	*vpt;

	KeyListType			*fdf, *macro=NULL;
	char				*subdir, *model;
	char				*current_directory;
	char				*plot_type_key;
	char				chapter[81];
	char				keyName[1024], value[1024];
	int				i, err, plot_type;


	vpt = GetGuiVertProfileDialog();
	fdf = GetVPGuiFdfInfo();

/*
 *	Save default directory in case the directory is redefined in a
 *	macro.
 */
	current_directory = strdup ( directory );

/*
 *	Read macro into a key list which allows for duplicate 
 *	entries.
 */
	if ( GetVPGridType ( vpt ) == SCALARGRIDOBJECT )
	    subdir = strdup ( GetModelVPScalarDir ( vpt ) );
	else
	    subdir = strdup ( GetModelVPVectorDir ( vpt ) );

	model = strdup ( GetModelTypeVP ( vpt ) );
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
		SetVPFdfFallbackValues ( fdf );
	    	SetVPGridWidgets ( fdf, fdf->fileName );

		BuildVPGrid();

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
		GetVPModelField ( macro->keys[i].value, chapter,
		                  current_directory );
		fdf = GetVPGuiFdfInfo();
/*
 *	Save plot type.
 */
		plot_type_key = GetFdfKeyValue ( fdf, "type" );
		ToUpper ( plot_type_key );
		plot_type = SCALARGRIDOBJECT;
		if ( strcmp ( plot_type_key, "VECTOR" ) == 0 ) 
			plot_type = VECTORGRIDOBJECT;
		SetVPGridType ( vpt, plot_type );
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
