/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gridxfdf.c
 *
 *	FDF related callbacks for cross section model data.	
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 3/97	COMET	Added scale factor text field.
 *	11/97	COMET	Removed redundant checks on Free macros.
 *	11/97	COMET	Fixed small leaks
 *	12/97	COMET	Removed label from SetXSGridWidgets()
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
#include "xsecobj.h"
#include "model.h"
#include "fdfobj.h"
#include "genglobs.h"
#include "winobj.h"
#include "menucb.h"
#include "_proto.h"


void SetXSFdfFallbackValues ( KeyListType * );
void SetXSGridWidgets ( KeyListType *, char * );
void SetXSOptionMenuButtons ( KeyListType * );
void SetXSContourLineInfo ( KeyListType * );
void SetXSGridPlotType ( KeyListType * );


void
SetXSFdfFallbackValues ( KeyListType *fdf )
/*
 *	Set FDF default values if not explicitly defined in FDF.
 */
{
	GuiXSectionDialogType	*vcs;
	GlobalDataObjectType	*gd;

	char		*vcoord,
			*scale,		*line_color,	*line_type,
			*line_width,	*line_label_frequency;

	char		*text_size,	*text_font,	*text_width,
			*text_hw_flag;

	char		*wind_symbol,
			*wind_symbol_size,	*wind_symbol_width,
			*wind_symbol_type,	*wind_symbol_headsize;

	char		*ptype_axis;
	int		verbose;


	vcs = GetGuiXSectionDialog();
	gd  = GetGlobalDataObject();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetXSFdfFallbackValues\n" );
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


/*
 *	Parameters which have GUI components. Look at option menus first.
 */
	if ( vcoord == NULL ) {
	    ToLower ( vcoord );
	    vcoord = strdup ( GetModelVCoordXS ( vcs ) );
	    PutInFDF ( "vcoord", vcoord, fdf, OVERWRITE );
	    Free ( vcoord );
	}
	else {
	    FreeModelVCoordXS ( vcs );
	    SetModelVCoordXS ( vcs, vcoord );
	    Free ( vcoord );
	}

	if ( ptype_axis != NULL ) {
	    FreeAxisTypeXS ( vcs );
	    SetAxisTypeXS ( vcs, ptype_axis );
	    Free ( ptype_axis );
	}

/*
 *	Textfield widgets.
 */
	FreeScaleFactorXS ( vcs );
	if ( scale == NULL ) {
	    SetScaleFactorXS ( vcs, " " );
	}
	else {
	    SetScaleFactorXS ( vcs, scale );
	    Free ( scale );
	}

	FreeLineTypeXS ( vcs );
	if ( line_type == NULL ) {
	    SetLineTypeXS ( vcs, "1" );
	}
	else {
	    SetLineTypeXS ( vcs, line_type );
	    Free ( line_type );
	}

	FreeLabelFrequencyXS ( vcs );
	if ( line_label_frequency == NULL ) {
	    SetLabelFrequencyXS ( vcs, "2" );
	}
	else {
	    SetLabelFrequencyXS ( vcs, line_label_frequency );
	    Free ( line_label_frequency );
	}

	FreeVectorSymbolXS ( vcs );
	if ( wind_symbol == NULL ) {
	    SetVectorSymbolXS ( vcs, "bk" );
	}
	else {
	    SetVectorSymbolXS ( vcs, wind_symbol );
	    Free ( wind_symbol );
	}

/*
 *	Set other fallback values.
 */

	FreeLineColorXS ( vcs );
	if ( line_color == NULL ) {
	    SetLineColorXS ( vcs, "NONE" );
	}
	else {
	    SetLineColorXS ( vcs, line_color );
	    Free ( line_color );
	}

	FreeLineWidthXS ( vcs );
	if ( line_width == NULL ) {
	    SetLineWidthXS ( vcs, "2" );
	}
	else {
	    SetLineWidthXS ( vcs, line_width );
	    Free ( line_width );
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
SetXSGridWidgets ( KeyListType *fdf, char *file )
{
	Widget			w;
	XmString		xmstr;

	GuiXSectionDialogType	*vcs;

	char			*function, *description;
	char			*fline;
	char			*level1, *level2;
	int			verbose;

	vcs = GetGuiXSectionDialog();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetXSGridWidgets\n" );

	function		= GetFdfKeyValue ( fdf, "function" );
	description		= GetFdfKeyValue ( fdf, "description" );
	fline			= GetFdfKeyValue ( fdf, "fline" );
	level1			= GetFdfKeyValue ( fdf, "yaxis_lower" );
	level2			= GetFdfKeyValue ( fdf, "yaxis_upper" );

/*
 *	Use FDF keys to obtain values and set corresponding widget values.
 */
						/* function */
	FreeFunctionXS ( vcs );
	SetFunctionXS ( vcs, function );
	XmTextSetString ( GetFunctionXSW ( vcs ), GetFunctionXS ( vcs ) );
	if ( function != NULL ) Free ( function );

						/* description */
	FreeFieldInfoXS ( vcs );
	SetFieldInfoXS ( vcs, description );
	XmTextSetString ( GetFieldInfoXSW ( vcs ), GetFieldInfoXS ( vcs ) );
	if ( description != NULL ) Free ( description );

/*
 *	Set level textfields.
 */
	if ( level1 != NULL ) {
	    FreeLevel1XS ( vcs );
	    SetLevel1XS ( vcs, level1 );
	    Free ( level1 );
	    XmTextSetString ( GetLevel1XSW ( vcs ), GetLevel1XS ( vcs ) );
	}

	if ( level2 != NULL ) {
	    FreeLevel2XS ( vcs );
	    SetLevel2XS ( vcs, level2 );
	    Free ( level2 );
	    XmTextSetString ( GetLevel2XSW ( vcs ), GetLevel2XS ( vcs ) );
	}

	if ( fline != NULL ) {			/* fline */
	    FreeFillColorsXS ( vcs );
	    SetFillColorsXS ( vcs, fline );
	    Free ( fline );
	}

/*
 *	Set fdf name in main dialog.
 */
	FreeModelFdfXS ( vcs );
	SetModelFdfXS ( vcs, file );

        xmstr = XmStringLtoRCreate ( GetModelFdfXS ( vcs ),
					XmSTRING_DEFAULT_CHARSET );
        XtVaSetValues ( GetModelFdfXSW ( vcs ),
			XmNlabelString, xmstr,
			NULL);
        XmStringFree ( xmstr );

/*
 *	Set scale factor textfield.
 */
	XmTextSetString ( GetScaleFactorXSW ( vcs ), GetScaleFactorXS ( vcs ) );

/*
 *	Set vector information.
 */
/*	SetVectorPlotInfo ( fdf ); */

/*
 *	Plot contour lines and/or color fill and/or symbols.
 */
	SetXSGridPlotType ( fdf );

/*
 *	Set line and fill intervals.
 */
	SetXSContourLineInfo ( fdf );

/*
 *	Set contour line characteristics.
 */
/*	SetGridLineWidgets ( fdf ); */

/*
 *	Set option menu buttons.
 */
	SetXSOptionMenuButtons ( fdf );
}


void
SetXSGridPlotType ( KeyListType *kl )
{
	GuiXSectionDialogType	*vcs;

	char			*contour, *fill;
	int			verbose;

	vcs = GetGuiXSectionDialog();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetXSGridPlotType\n" );

/*
 *	Find out whether to plot contour line and fill symbols.
 */
	contour			= GetFdfKeyValue ( kl, "contour" );
	fill			= GetFdfKeyValue ( kl, "fill" );

/*
 *	Change toggle button states as given by FDF input.
 */
	if ( contour != NULL ) {
	    if ( *contour == 'y' || *contour == 'Y' )
		if ( ! XmToggleButtonGetState ( GetLineToggleXSW1 ( vcs ) ) ) {
		    XmToggleButtonSetState (
		            GetLineToggleXSW1 ( vcs ), TRUE, FALSE );
		    XmToggleButtonSetState (
		            GetLineToggleXSW2 ( vcs ), TRUE, FALSE );
		}
	    if ( *contour == 'n' || *contour == 'N' )
		if ( XmToggleButtonGetState ( GetLineToggleXSW1 ( vcs ) ) ) {
		    XmToggleButtonSetState (
		            GetLineToggleXSW1 ( vcs ), FALSE, FALSE );
		    XmToggleButtonSetState (
		            GetLineToggleXSW2 ( vcs ), FALSE, FALSE );
		}
	    Free ( contour );
	}

	if ( fill != NULL ) {
	    if ( *fill == 'y' || *fill == 'Y' )
		if ( ! XmToggleButtonGetState ( GetFillToggleXSW1 ( vcs ) ) ) {
		    XmToggleButtonSetState (
		            GetFillToggleXSW1 ( vcs ), TRUE, FALSE );
		    XmToggleButtonSetState (
		            GetFillToggleXSW2 ( vcs ), TRUE, FALSE );
		}
	    if ( *fill == 'n' || *fill == 'N' )
		if ( XmToggleButtonGetState ( GetFillToggleXSW1 ( vcs ) ) ) {
		    XmToggleButtonSetState (
		            GetFillToggleXSW1 ( vcs ), FALSE, FALSE );
		    XmToggleButtonSetState (
		            GetFillToggleXSW2 ( vcs ), FALSE, FALSE );
		}
	    Free ( fill );
	}
}


void
SetXSOptionMenuButtons ( KeyListType *fdf )
{
	GuiXSectionDialogType	*vcs;

	char			*keylist;
	char			**vcoords, **vectorsym, **label_frequency,
				**linetype, **yaxis;
	int			nkeys;
	int			verbose;

	vcs = GetGuiXSectionDialog();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetXSOptionMenuButtons\n" );

/*
 *	Set option menu button and text field for vertical coordinate.
 */
	keylist   = GetConfigValue( "vcoordkeys" );
	vcoords  = SplitByDelimeter ( keylist, ",", &nkeys );
	XmTextSetString ( GetModelVCoordXSW ( vcs ), GetModelVCoordXS ( vcs ) );
	XtUnmanageChild ( GetModelVCoordOmXSW ( vcs ) );
	SetDefaultOptionButton ( GetModelVCoordOmXSW ( vcs ),
				 nkeys, GetModelVCoordXS ( vcs ), vcoords,
				 &(vcs->vcoordpm), "vcoordkeys" );
	XtManageChild ( GetModelVCoordOmXSW ( vcs ) );
	Free ( keylist );
	StringListFree ( nkeys, vcoords );

/*
 *	Set option menu button for yaxis type.
 */
	keylist   = GetConfigValue( "yaxiskeys" );
	yaxis = SplitByDelimeter ( keylist, ",", &nkeys );
	XtUnmanageChild ( GetAxisTypeOmXSW ( vcs ) );
	SetDefaultOptionButton ( GetAxisTypeOmXSW ( vcs ),
				nkeys, GetAxisTypeXS ( vcs ), yaxis,
				&(vcs->axis_type_pm), "yaxiskeys" );
	XtManageChild ( GetAxisTypeOmXSW ( vcs ) );
	Free ( keylist );
	StringListFree ( nkeys, yaxis );

/*
 *	Set option menu button for vector symbols.
 */
	keylist   = GetConfigValue( "vectorkeys" );
	vectorsym = SplitByDelimeter ( keylist, ",", &nkeys );
	XtUnmanageChild ( GetVectorSymbolOmXSW( vcs ) );
	SetDefaultOptionButton ( GetVectorSymbolOmXSW( vcs ),
				 nkeys, GetVectorSymbolXS ( vcs ), vectorsym,
				 &(vcs->vector_sym_pm), "vectorkeys" );
	XtManageChild ( GetVectorSymbolOmXSW( vcs ) );
	Free ( keylist );
	StringListFree ( nkeys, vectorsym );

/*
 *	Set option menu button for line type.
 */
	keylist   = GetConfigValue( "linetypekeys" );
	linetype = SplitByDelimeter ( keylist, ",", &nkeys );
	XtUnmanageChild ( GetLineTypeOmXSW ( vcs ) );
	SetDefaultOptionButton ( GetLineTypeOmXSW ( vcs ),
				 nkeys, GetLineTypeXS ( vcs ), linetype,
				 &(vcs->line_type_pm), "linetypekeys" );
	XtManageChild ( GetLineTypeOmXSW ( vcs ) );
	Free ( keylist );
	StringListFree ( nkeys, linetype );

/*
 *	Set option menu button for line label frequency.
 */
	keylist   = GetConfigValue( "labelfreqkeys" );
	label_frequency = SplitByDelimeter ( keylist, ",", &nkeys );
	XtUnmanageChild ( GetLabelFrequencyOmXSW ( vcs ) );
	SetDefaultOptionButton ( GetLabelFrequencyOmXSW ( vcs ),
				 nkeys, GetLabelFrequencyXS ( vcs ),
                                 label_frequency, &(vcs->label_freq_pm),
                                 "labelfreqkeys" );
	XtManageChild ( GetLabelFrequencyOmXSW ( vcs ) );
	Free ( keylist );
	StringListFree ( nkeys, label_frequency );
}


void
SetXSContourLineInfo ( KeyListType *kl )
/*
 * Set contour line parameters as defined for a given vertical coordinate
 * and vertical level.
 */
{
	GuiXSectionDialogType	*vcs;

	char			*vcoord, *vlevels;
	char			*cint, *cmin , *cmax;
	char			*fint, *fmin , *fmax;
	char			*cint_parse, *fint_parse;
	char			*value;
	char			key[GEMPAKSTRING];
	int			verbose;

	vcs = GetGuiXSectionDialog();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetXSContourLineInfo\n" );

	vcoord                  = GetFdfKeyValue ( kl, "vcoord" );
	if ( vcoord == NULL ) {
	    printf ("*** SetXSContourLineInfo - Could not find vcoord value ... returning\n" );
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
	sprintf ( key, "%s%s", vcoord, "_fint" );
	fint = GetFdfKeyValue ( kl, key );
	sprintf ( key, "%s%s", vcoord, "_fmin" );
	fmin = GetFdfKeyValue ( kl, key );
	sprintf ( key, "%s%s", vcoord, "_fmax" );
	fmax = GetFdfKeyValue ( kl, key );
	cint_parse = GetFdfKeyValue ( kl, "cint_parse" );
	fint_parse = GetFdfKeyValue ( kl, "fint_parse" );

/*
 *	Set contour interval and fill widgets.
 */
	FreeContourIntervalXS ( vcs );
	FreeContourMinimumXS ( vcs );
	FreeContourMaximumXS ( vcs );
	FreeFillIntervalXS ( vcs );
	FreeFillMinimumXS ( vcs );
	FreeFillMaximumXS ( vcs );

	if ( cint != NULL ) {
	    value = strdup ( cint );
	    SetContourIntervalXS ( vcs, value );
	    XmTextSetString ( GetContourIntervalXSW ( vcs ),
	                      GetContourIntervalXS ( vcs ) );
	    Free ( value );
	    Free ( cint );
	}
	else {
	    SetContourIntervalXS ( vcs, " " );
	    XmTextSetString ( GetContourIntervalXSW ( vcs ),
	                      GetContourIntervalXS ( vcs ) );
	}

	if ( cmin != NULL ) {
	    value = strdup ( cmin );
	    SetContourMinimumXS ( vcs, value );
	    XmTextSetString ( GetContourMinimumXSW ( vcs ),
	                      GetContourMinimumXS ( vcs ) );
	    Free ( value );
	    Free ( cmin );
	}
	else {
	    SetContourMinimumXS ( vcs, " " );
	    XmTextSetString ( GetContourMinimumXSW ( vcs ),
	                      GetContourMinimumXS ( vcs ) );
	}

	if ( cmax != NULL ) {
	    value = strdup ( cmax );
	    SetContourMaximumXS ( vcs, value );
	    XmTextSetString ( GetContourMaximumXSW ( vcs ),
	                      GetContourMaximumXS ( vcs ) );
	    Free ( value );
	    Free ( cmax );
	}
	else {
	    SetContourMaximumXS ( vcs, " " );
	    XmTextSetString ( GetContourMaximumXSW ( vcs ),
	                      GetContourMaximumXS ( vcs ) );
	}

	if ( fint != NULL ) {
	    value = strdup ( fint );
	    SetFillIntervalXS ( vcs, value );
	    XmTextSetString ( GetFillIntervalXSW ( vcs ),
	                      GetFillIntervalXS ( vcs ) );
	    Free ( value );
	    Free ( fint );
	}
	else {
	    SetFillIntervalXS ( vcs, " " );
	    XmTextSetString ( GetFillIntervalXSW ( vcs ),
	                      GetFillIntervalXS ( vcs ) );
	}

	if ( fmin != NULL ) {
	    value = strdup ( fmin );
	    SetFillMinimumXS ( vcs, value );
	    XmTextSetString ( GetFillMinimumXSW ( vcs ),
	                      GetFillMinimumXS ( vcs ) );
	    Free ( value );
	    Free ( fmin );
	}
	else {
	    SetFillMinimumXS ( vcs, " " );
	    XmTextSetString ( GetFillMinimumXSW ( vcs ),
	                      GetFillMinimumXS ( vcs ) );
	}

	if ( fmax != NULL ) {
	    value = strdup ( fmax );
	    SetFillMaximumXS ( vcs, value );
	    XmTextSetString ( GetFillMaximumXSW ( vcs ),
	                      GetFillMaximumXS ( vcs ) );
	    Free ( value );
	    Free ( fmax );
	}
	else {
	    SetFillMaximumXS ( vcs, " " );
	    XmTextSetString ( GetFillMaximumXSW ( vcs ),
	                      GetFillMaximumXS ( vcs ) );
	}

	Free ( vcoord );
	if ( vlevels != NULL ) Free ( vlevels );
	if ( cint_parse != NULL ) Free ( cint_parse );
	if ( fint_parse != NULL ) Free ( fint_parse );
}
