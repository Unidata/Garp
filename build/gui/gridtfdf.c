/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gridtfdf.c
 *
 *	FDF related callbacks for cross section model data.	
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 3/97	COMET	Added time axis option menu.
 *	11/97	COMET	Removed redundant checks on Free macros.
 *	11/97	COMET	Fixed small leaks
 *	12/97	COMET	Removed label from SetTHGridWidgets()
 *	12/97	COMET	Get default text settings from keys in "Garp_defaults".
 *	 4/99   COMET   Added keylist string to SetDefaultOptionButton.
 *	 8/99	COMET	Use general vertical coordinate to define contour
 *			drawing parameters.
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


void SetTHFdfFallbackValues ( KeyListType * );
void SetTHGridWidgets ( KeyListType *, char * );
void SetTHOptionMenuButtons ( KeyListType * );
void SetTHContourLineInfo ( KeyListType * );
void SetTHGridPlotType ( KeyListType * );


void
SetTHFdfFallbackValues ( KeyListType *fdf )
/*
 *	Set FDF default values if not explicitly defined in FDF.
 */
{
	GuiTimeHeightDialogType	*tht;
	GlobalDataObjectType	*gd;

	char		*vcoord,
			*scale,		*line_color,	*line_type,
			*line_width,	*line_label_frequency;

	char		*text_size,	*text_font,	*text_width,
			*text_hw_flag;

	char		*ptype_axis,	*taxis_dir;

	char		*wind_symbol,
			*wind_symbol_size,	*wind_symbol_width,
			*wind_symbol_type,	*wind_symbol_headsize;

	int		verbose;


	tht = GetGuiTimeHeightDialog();
	gd  = GetGlobalDataObject();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetTHFdfFallbackValues\n" );
/*
 *	If the vertical coordinate and vertical levels are defined in
 *	the FDF, use them. Otherwise, get these values from gui
 *	widgets.
 */
	vcoord 		     = GetFdfKeyValue ( fdf, "vcoord" );
	scale		     = GetFdfKeyValue ( fdf, "scale" );
	line_color           = GetFdfKeyValue ( fdf, "line_color" );
	line_type	     = GetFdfKeyValue ( fdf, "line_type" );
	line_width	     = GetFdfKeyValue ( fdf, "line_width" );
	line_label_frequency = GetFdfKeyValue ( fdf, "line_label_frequency" );
	text_size	     = GetFdfKeyValue ( fdf, "text_size" );
	text_font	     = GetFdfKeyValue ( fdf, "text_font" );
	text_width	     = GetFdfKeyValue ( fdf, "text_width" );
	text_hw_flag	     = GetFdfKeyValue ( fdf, "text_hw_flag" );
	wind_symbol	     = GetFdfKeyValue ( fdf, "wind_symbol" );
	wind_symbol_size     = GetFdfKeyValue ( fdf, "wind_symbol_size" );
	wind_symbol_width    = GetFdfKeyValue ( fdf, "wind_symbol_width" );
	wind_symbol_type     = GetFdfKeyValue ( fdf, "wind_symbol_type" );
	wind_symbol_headsize = GetFdfKeyValue ( fdf, "wind_symbol_headsize" );
	ptype_axis	     = GetFdfKeyValue ( fdf, "ptype_axis" );
	taxis_dir	     = GetFdfKeyValue ( fdf, "taxis_dir" );

/*
 *	Parameters which have GUI components. Look at option menus first.
 */
	if ( vcoord == NULL ) {
	    ToLower ( vcoord );
	    vcoord = strdup ( GetModelVCoordTH ( tht ) );
	    PutInFDF ( "vcoord", vcoord, fdf, OVERWRITE );
	    Free ( vcoord );
	}
	else {
	    FreeModelVCoordTH ( tht );
	    SetModelVCoordTH ( tht, vcoord );
	    Free ( vcoord );
	}

	if ( ptype_axis != NULL ) {
	    FreeAxisTypeTH ( tht );
	    SetAxisTypeTH ( tht, ptype_axis );
	    Free ( ptype_axis );
	}

/*
 *	Textfield widgets.
 */
	FreeScaleFactorTH ( tht );
	if ( scale == NULL ) {
	    SetScaleFactorTH ( tht, " " );
	}
	else {
	    SetScaleFactorTH ( tht, scale );
	    Free ( scale );
	}
	    
	FreeTimeAxisTH ( tht );
	if ( taxis_dir == NULL ) {
	    SetTimeAxisTH ( tht, "R" );
	}
	else {
	    SetTimeAxisTH ( tht, taxis_dir );
	    Free ( taxis_dir );
	}

	FreeLineTypeTH ( tht );
	if ( line_type == NULL ) {
	    SetLineTypeTH ( tht, "1" );
	}
	else {
	    SetLineTypeTH ( tht, line_type );
	    Free ( line_type );
	}

	FreeLineWidthTH ( tht );
	if ( line_width == NULL ) {
	    SetLineWidthTH ( tht, "2" );
	}
	else {
	    SetLineWidthTH ( tht, line_width );
	    Free ( line_width );
	}

	FreeLabelFrequencyTH ( tht );
	if ( line_label_frequency == NULL ) {
	    SetLabelFrequencyTH ( tht, "2" );
	}
	else {
	    SetLabelFrequencyTH ( tht, line_label_frequency );
	    Free ( line_label_frequency );
	}

	FreeVectorSymbolTH ( tht );
	if ( wind_symbol == NULL ) {
	    SetVectorSymbolTH ( tht, "bk" );
	}
	else {
	    SetVectorSymbolTH ( tht, wind_symbol );
	    Free ( wind_symbol );
	}

/*
 *	Set other fallback values.
 */

	FreeLineColorTH ( tht );
	if ( line_color == NULL ) {
	    SetLineColorTH ( tht, "NONE" );
	}
	else {
	    SetLineColorTH ( tht, line_color );
	    Free ( line_color );
	}

	if ( text_size == NULL ) {
	    text_size = strdup ( gd->text_size );
	    PutInFDF ( "text_size", text_size, fdf, OVERWRITE );
	    Free ( text_size );
	}
	else {
	    PutInFDF ( "text_size", text_size, fdf, OVERWRITE );
	    Free ( text_size );
	}

	if ( text_font == NULL ) {
	    text_font = strdup ( gd->text_font );
	    PutInFDF ( "text_font", text_font, fdf, OVERWRITE );
	    Free ( text_font );
	}
	else {
	    PutInFDF ( "text_font", text_font, fdf, OVERWRITE );
	    Free ( text_font );
	}

	if ( text_width == NULL ) {
	    text_width = strdup ( gd->text_width );
	    PutInFDF ( "text_width", text_width, fdf, OVERWRITE );
	    Free ( text_width );
	}
	else {
	    PutInFDF ( "text_width", text_width, fdf, OVERWRITE );
	    Free ( text_width );
	}

	if ( text_hw_flag == NULL ) {
	    text_hw_flag = strdup ( gd->text_hw_flag );
	    PutInFDF ( "text_hw_flag", text_hw_flag, fdf, OVERWRITE );
	    Free ( text_hw_flag );
	}
	else {
	    PutInFDF ( "text_hw_flag", text_hw_flag, fdf, OVERWRITE );
	    Free ( text_hw_flag );
	}
	
	if ( wind_symbol_size == NULL ) {
	    PutInFDF ( "wind_symbol_size", "1", fdf, OVERWRITE );
	}
	else {
	    PutInFDF ( "wind_symbol_size", wind_symbol_size, fdf, OVERWRITE );
	    Free ( wind_symbol_size );
	}
	
	if ( wind_symbol_width == NULL ) {
	    PutInFDF ( "wind_symbol_width", "1", fdf, OVERWRITE );
	}
	else {
	    PutInFDF ( "wind_symbol_width", wind_symbol_width, fdf, OVERWRITE );
	    Free ( wind_symbol_width );
	}

	if ( wind_symbol_type== NULL ) {
	    PutInFDF ( "wind_symbol_type", "111", fdf, OVERWRITE );
	}
	else {
	    PutInFDF ( "wind_symbol_type", wind_symbol_type, fdf, OVERWRITE );
	    Free ( wind_symbol_type );
	}

	if ( wind_symbol_headsize == NULL ) {
	    PutInFDF ( "wind_symbol_headsize", "1", fdf, OVERWRITE );
	}
	else {
	    PutInFDF ( "wind_symbol_headsize", wind_symbol_headsize, fdf, OVERWRITE );
	    Free ( wind_symbol_headsize );
	}

	return;
}


void
SetTHGridWidgets ( KeyListType *fdf, char *file )
{
/*
 * Set time/height dialog widgets according to input from FDF.
 */

	Widget			w;
	XmString		xmstr;

	GuiTimeHeightDialogType	*tht;

	char			*function, *description;
	char			*level1, *level2;
	int			verbose;

	tht = GetGuiTimeHeightDialog();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetTHGridWidgets\n" );

	function		= GetFdfKeyValue ( fdf, "function" );
	description		= GetFdfKeyValue ( fdf, "description" );
	level1			= GetFdfKeyValue ( fdf, "yaxis_lower" );
	level2			= GetFdfKeyValue ( fdf, "yaxis_upper" );

/*
 *	Use FDF keys to obtain values and set corresponding widget values.
 */
						/* function */
	FreeFunctionTH ( tht );	
	SetFunctionTH ( tht, function );
	XmTextSetString ( GetFunctionTHW ( tht ), GetFunctionTH ( tht ) );
	if ( function != NULL ) Free ( function );

						/* description */
	FreeFieldInfoTH ( tht );
	SetFieldInfoTH ( tht, description );
	XmTextSetString ( GetFieldInfoTHW ( tht ), GetFieldInfoTH ( tht ) );
	if ( description != NULL ) Free ( description );

/*
 *	Set level textfields.
 */
	if ( level1 != NULL ) {
	    FreeLevel1TH ( tht );
	    SetLevel1TH ( tht, level1 );
	    Free ( level1 );
	    XmTextSetString ( GetLevel1THW ( tht ), GetLevel1TH ( tht ) );
	}
	if ( level2 != NULL ) {
	    FreeLevel2TH ( tht );
	    SetLevel2TH ( tht, level2 );
	    Free ( level2 );
	    XmTextSetString ( GetLevel2THW ( tht ), GetLevel2TH ( tht ) );
	}

/*
 *	Set fdf name in main dialog.
 */
	FreeModelFdfTH ( tht );
	SetModelFdfTH ( tht, file );

        xmstr = XmStringLtoRCreate ( GetModelFdfTH ( tht ),
					XmSTRING_DEFAULT_CHARSET );
        XtVaSetValues ( GetModelFdfTHW ( tht ),
			XmNlabelString, xmstr,
			NULL);
        XmStringFree ( xmstr );

/*
 *	Set scale factor textfield.
 */
	XmTextSetString ( GetScaleFactorTHW ( tht ), GetScaleFactorTH ( tht ) );

/*
 *	Set line intervals.
 */
	SetTHContourLineInfo ( fdf );

/*
 *	Set contour line characteristics.
 */
/*	SetGridLineWidgets ( fdf ); */

/*
 *	Set option menu buttons.
 */
	SetTHOptionMenuButtons ( fdf );
}


void
SetTHOptionMenuButtons ( KeyListType *fdf )
{
	GuiTimeHeightDialogType	*tht;

	char			*keylist;
	char			**vcoords, **vectorsym, **label_frequency,
				**linetype, **yaxis,    **taxis;
	int			nkeys;
	int			verbose;

	tht = GetGuiTimeHeightDialog();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetTHOptionMenuButtons\n" );

/*
 *	Set option menu button and text field for vertical coordinate.
 */
	keylist   = GetConfigValue( "vcoordkeys" );
	vcoords  = SplitByDelimeter ( keylist, ",", &nkeys );
	XmTextSetString ( GetModelVCoordTHW ( tht ), GetModelVCoordTH ( tht ) );
	XtUnmanageChild ( GetModelVCoordOmTHW ( tht ) );
	SetDefaultOptionButton ( GetModelVCoordOmTHW ( tht ),
				 nkeys, GetModelVCoordTH ( tht ), vcoords,
				 &(tht->vcoordpm), "vcoordkeys" );
	XtManageChild ( GetModelVCoordOmTHW ( tht ) );
	Free ( keylist );
	StringListFree ( nkeys, vcoords );

/*
 *	Set option menu button for yaxis type.
 */
	keylist   = GetConfigValue( "yaxiskeys" );
	yaxis = SplitByDelimeter ( keylist, ",", &nkeys );
	XtUnmanageChild ( GetAxisTypeOmTHW ( tht ) );
	SetDefaultOptionButton ( GetAxisTypeOmTHW ( tht ),
				nkeys, GetAxisTypeTH ( tht ), yaxis,
				&(tht->axis_type_pm), "yaxiskeys" );
	XtManageChild ( GetAxisTypeOmTHW ( tht ) );
	Free ( keylist );
	StringListFree ( nkeys, yaxis );

/*
 *	Set option menu button for vector symbols.
 */
	keylist   = GetConfigValue( "vectorkeys" );
	vectorsym = SplitByDelimeter ( keylist, ",", &nkeys );
	XtUnmanageChild ( GetVectorSymbolOmTHW( tht ) );
	SetDefaultOptionButton ( GetVectorSymbolOmTHW( tht ),
				 nkeys, GetVectorSymbolTH ( tht ), vectorsym,
				 &(tht->vector_sym_pm), "vectorkeys" );
	XtManageChild ( GetVectorSymbolOmTHW( tht ) );
	Free ( keylist );
	StringListFree ( nkeys, vectorsym );

/*
 *	Set option menu button for line type.
 */
	keylist   = GetConfigValue( "linetypekeys" );
	linetype = SplitByDelimeter ( keylist, ",", &nkeys );
	XtUnmanageChild ( GetLineTypeOmTHW ( tht ) );
	SetDefaultOptionButton ( GetLineTypeOmTHW ( tht ),
				 nkeys, GetLineTypeTH ( tht ), linetype,
				 &(tht->line_type_pm), "linetypekeys" );
	XtManageChild ( GetLineTypeOmTHW ( tht ) );
	Free ( keylist );
	StringListFree ( nkeys, linetype );

/*
 *	Set option menu button for line label frequency.
 */
	keylist   = GetConfigValue( "labelfreqkeys" );
	label_frequency = SplitByDelimeter ( keylist, ",", &nkeys );
	XtUnmanageChild ( GetLabelFrequencyOmTHW ( tht ) );
	SetDefaultOptionButton ( GetLabelFrequencyOmTHW ( tht ),
				 nkeys, GetLabelFrequencyTH ( tht ),
                                 label_frequency, &(tht->label_freq_pm),
                                 "labelfreqkeys" );
	XtManageChild ( GetLabelFrequencyOmTHW ( tht ) );
	Free ( keylist );
	StringListFree ( nkeys, label_frequency );

/*
 *	Set option menu button for time axis direction.
 */
	keylist   = GetConfigValue( "thtaxiskeys" );
	taxis = SplitByDelimeter ( keylist, ",", &nkeys );
	XtUnmanageChild ( GetTimeAxisOmTHW ( tht ) );
	SetDefaultOptionButton ( GetTimeAxisOmTHW ( tht ),
				 nkeys, GetTimeAxisTH ( tht ), taxis,
				 &(tht->taxis), "thtaxiskeys" );
	XtManageChild ( GetTimeAxisOmTHW ( tht ) );
	Free ( keylist );
	StringListFree ( nkeys, taxis );
}


void
SetTHContourLineInfo ( KeyListType *kl )
/*
 * Set contour line parameters as defined for a given vertical coordinate
 * and vertical level.
 */
{
	GuiTimeHeightDialogType	*tht;

	char			*vcoord, *vlevels;
	char			*cint, *cmin , *cmax;
	char			*cint_parse;
	char			*value;
	char			key[GEMPAKSTRING];
	int			verbose;

	tht = GetGuiTimeHeightDialog();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetTHContourLineInfo\n" );

	vcoord                  = GetFdfKeyValue ( kl, "vcoord" );
	if ( vcoord == NULL ) {
	    printf ("*** SetTHContourLineInfo - Could not find vcoord value ... returning\n" );
	    return;
	}

/*
 *	Get contour relevant keys for this vertical coordinate.
 */
	vlevels = GetFdfKeyValue ( kl, vcoord );
	sprintf ( key, "%s%s", vcoord, "_cint" );
	cint = GetFdfKeyValue ( kl, key );
	sprintf ( key, "%s%s", vcoord, "_cmin" );
	cmin = GetFdfKeyValue ( kl, key );
	sprintf ( key, "%s%s", vcoord, "_cmax" );
	cmax = GetFdfKeyValue ( kl, key );
	cint_parse = GetFdfKeyValue ( kl, "cint_parse" );

/*
 *	Set contour interval and fill widgets.
 */
	FreeContourIntervalTH ( tht );
	FreeContourMinimumTH ( tht );
	FreeContourMaximumTH ( tht );

	if ( cint != NULL ) {
	    value = strdup ( cint );
	    SetContourIntervalTH ( tht, value );
	    XmTextSetString ( GetContourIntervalTHW ( tht ),
	                      GetContourIntervalTH ( tht ) );
	    Free ( value );
	    Free ( cint );
	}
	else {
	    SetContourIntervalTH ( tht, " " );
	    XmTextSetString ( GetContourIntervalTHW ( tht ),
	                      GetContourIntervalTH ( tht ) );
	}

	if ( cmin != NULL ) {
	    value = strdup ( cmin );
	    SetContourMinimumTH ( tht, value );
	    XmTextSetString ( GetContourMinimumTHW ( tht ),
	                      GetContourMinimumTH ( tht ) );
	    Free ( value );
	    Free ( cmin );
	}
	else {
	    SetContourMinimumTH ( tht, " " );
	    XmTextSetString ( GetContourMinimumTHW ( tht ),
	                      GetContourMinimumTH ( tht ) );
	}

	if ( cmax != NULL ) {
	    value = strdup ( cmax );
	    SetContourMaximumTH ( tht, value );
	    XmTextSetString ( GetContourMaximumTHW ( tht ),
	                      GetContourMaximumTH ( tht ) );
	    Free ( value );
	    Free ( cmax );
	}
	else {
	    SetContourMaximumTH ( tht, " " );
	    XmTextSetString ( GetContourMaximumTHW ( tht ),
	                      GetContourMaximumTH ( tht ) );
	}


	Free ( vcoord );
	if ( vlevels != NULL ) Free ( vlevels );
	if ( cint_parse != NULL ) Free ( cint_parse );
}
