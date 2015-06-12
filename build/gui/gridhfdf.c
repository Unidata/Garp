/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gridhfdf.c
 *
 *	FDF related callbacks for plan view model data.	
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 3/97	COMET	Added ability to set contour parameters for any
 *			vertical coordinate system.
 *	11/97	COMET	Removed redundant checks on Free macros.
 *	11/97	COMET	Fixed some leaks
 *	12/97	COMET	Fixed small leak (cint_parse, fint_parse), remove
 *			label in SetGridWidgets()
 *	12/97	COMET	Get default text settings from keys in "Garp_defaults".
 *	12/97	COMET	Bug fixed multiple free's of cint_parse and fint_parse.
 *	 3/98	COMET	Added key to set Value toggle from an FDF.
 *	 4/99   COMET   Added keylist string to SetDefaultOptionButton.
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


char *
SetIntervalByLevel ( char *vcoord, 
                     char *vlevels,
                     char *level,
                     char *intervals )
/*
 * Set contour values according to FDF. This routine uses a list of vertical
 * levels, a list of contour values for layers between those levels and a
 * user defined level to determine the appropriate contour value.
 */
{
	char	*value;
	char	**levels, **values;
	int	i, numlev=0, numval=0;
	int	verbose;

	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetIntervalByLevel\n" );

/*
 *	Count number of levels and contour intervals in FDF.
 */
	levels = SplitByDelimeter ( vlevels, ";", &numlev );
	values = SplitByDelimeter ( intervals, ";", &numval );
	    
/*
 *	If only one interval is given in the FDF, return it's value.
 */
	if ( numval == 1 ) {
	    value = strdup ( values[0] );
	    StringListFree ( numlev, levels );
	    StringListFree ( numval, values );
	    return value;
	}

/*
 *	Special cases.
 */
	if ( strcasecmp ( level, "Surface" ) == 0 ) {
	    value = strdup ( values[0] );
	    StringListFree ( numlev, levels );
	    StringListFree ( numval, values );
	    return value;
	}

/*
 *	Sanity check. Make sure that:
 *		1) Number of values > 0.
 *		2) Number of values is one more than number of levels.
 */
	if ( numval < 1 ) {
	    printf ( "*** Error in SetIntervalByLevel\n - There are no contour levels defined in this FDF.\n" );
	    value = strdup ( "" );
	}
	else if ( numval - numlev != 1 ) {
	    printf ( "*** Error in SetIntervalByLevel\n - Number of levels defined in FDF should be one more than number of contour intervals.\n" );
	    value = strdup ( "" );
	}
	else {

/*
 *	Chosen level is below lowest level in list.
 */
	    if ( atoi (level) >= atoi (levels[0]) )
		value = strdup ( values[0] );

/*
 *	Chosen level is above highest level in list.
 */
	    else if ( atoi (level) < atoi (levels[numlev-1]) )
		value = strdup ( values[numval-1] );

/*
 *	Chosen level is between levels in list.
 */
	    else {
	    	for ( i=0; i<numlev-1; i++ ) {
		    if ( atoi (level) < atoi (levels[i]) &&
			 atoi (level) >=  atoi (levels[i+1]) ) {
		    	value = strdup ( values[i+1] );
		    	break;
		    }
	    	}
	    }
	}
/*
 *	Free.
 */
	StringListFree ( numlev, levels );
	StringListFree ( numval, values );

	return ( value );
}


void
SetContourLineInfo ( KeyListType *kl, char *vcoord, char *level )
/*
 * Set contour line parameters as defined for a given vertical coordinate
 * and vertical level.
 */
{
	GuiScalarGridDialogType	*grd;

	char			*vlevels;
	char			*cint, *cmin , *cmax;
	char			*fint, *fmin , *fmax;
	char			*cint_parse, *fint_parse;
	char			*value;
	char			key[GEMPAKSTRING];
	int			verbose;

	grd = GetGuiScalarGridDialog();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetContourLineInfo\n" );

/*
 *	Cannot calculate contour interval without both vertical coordinate
 *	and level list.
 */
	if ( vcoord == NULL || level == NULL ) return;

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
 *	Free some strings.
 */
	FreeContourIntervalHS ( grd );
	FreeContourMinimumHS ( grd );
	FreeContourMaximumHS ( grd );
	FreeFillIntervalHS ( grd );
	FreeFillMinimumHS ( grd );
	FreeFillMaximumHS ( grd );

/*
 *	Set contour interval and fill widgets.
 */
	if ( cint != NULL ) {
	    if ( cint_parse != NULL && strcmp ( cint_parse, "N" ) == 0 )
		value = strdup ( cint );
	    else
	        value = SetIntervalByLevel ( vcoord, vlevels, level, cint );

	    SetContourIntervalHS ( grd, value );
	    XmTextSetString ( GetContourIntervalHSW ( grd ),
	                      GetContourIntervalHS ( grd ) );
	    Free ( value );
	    Free ( cint );
	}
	else {
	    SetContourIntervalHS ( grd, " " );
	    XmTextSetString ( GetContourIntervalHSW ( grd ),
	                      GetContourIntervalHS ( grd ) );
	}

	if ( cmin != NULL ) {
	    value = SetIntervalByLevel ( vcoord, vlevels, level, cmin );
	    SetContourMinimumHS ( grd, value );
	    XmTextSetString ( GetContourMinimumHSW ( grd ),
	                      GetContourMinimumHS ( grd ) );
	    Free ( value );
	    Free ( cmin );
	}
	else {
	    SetContourMinimumHS ( grd, " " );
	    XmTextSetString ( GetContourMinimumHSW ( grd ),
	                      GetContourMinimumHS ( grd ) );
	}

	if ( cmax != NULL ) {
	    value = SetIntervalByLevel ( vcoord, vlevels, level, cmax );
	    SetContourMaximumHS ( grd, value );
	    XmTextSetString ( GetContourMaximumHSW ( grd ),
	                      GetContourMaximumHS ( grd ) );
	    Free ( value );
	    Free ( cmax );
	}
	else {
	    SetContourMaximumHS ( grd, " " );
	    XmTextSetString ( GetContourMaximumHSW ( grd ),
	                      GetContourMaximumHS ( grd ) );
	}

	if ( fint != NULL ) {
	    if ( fint_parse != NULL && strcmp ( fint_parse, "N" ) == 0 )
		value = strdup ( fint );
	    else
	        value = SetIntervalByLevel ( vcoord, vlevels, level, fint );

	    SetFillIntervalHS ( grd, value );
	    XmTextSetString ( GetFillIntervalHSW ( grd ),
	                      GetFillIntervalHS ( grd ) );
	    Free ( value );
	    Free ( fint );
	}
	else {
	    SetFillIntervalHS ( grd, " " );
	    XmTextSetString ( GetFillIntervalHSW ( grd ),
	                      GetFillIntervalHS ( grd ) );
	}

	if ( fmin != NULL ) {
	    value = SetIntervalByLevel ( vcoord, vlevels, level, fmin );
	    SetFillMinimumHS ( grd, value );
	    XmTextSetString ( GetFillMinimumHSW ( grd ),
	                      GetFillMinimumHS ( grd ) );
	    Free ( value );
	    Free ( fmin );
	}
	else {
	    SetFillMinimumHS ( grd, " " );
	    XmTextSetString ( GetFillMinimumHSW ( grd ),
	                      GetFillMinimumHS ( grd ) );
	}

	if ( fmax != NULL ) {
	    value = SetIntervalByLevel ( vcoord, vlevels, level, fmax );
	    SetFillMaximumHS ( grd, value );
	    XmTextSetString ( GetFillMaximumHSW ( grd ),
	                      GetFillMaximumHS ( grd ) );
	    Free ( value );
	    Free ( fmax );
	}
	else {
	    SetFillMaximumHS ( grd, " " );
	    XmTextSetString ( GetFillMaximumHSW ( grd ),
	                      GetFillMaximumHS ( grd ) );
	}

	if ( vlevels != NULL ) Free ( vlevels );
	if ( cint_parse != NULL ) Free ( cint_parse );
	if ( fint_parse != NULL ) Free ( fint_parse );
}


int
CountFdfFunctions ( KeyListType *fdf )
/*
 *	Count the number of functions in an FDF.
 */
{
	char		*function , *function2, *function3, *function4,
			*function5, *function6, *function7, *function8,
			*function9, *function10;

	int		nfunction = 0;

	function   = GetFdfKeyValue ( fdf, "function" );
	function2  = GetFdfKeyValue ( fdf, "function2" );
	function3  = GetFdfKeyValue ( fdf, "function3" );
	function4  = GetFdfKeyValue ( fdf, "function4" );
	function5  = GetFdfKeyValue ( fdf, "function5" );
	function6  = GetFdfKeyValue ( fdf, "function6" );
	function7  = GetFdfKeyValue ( fdf, "function7" );
	function8  = GetFdfKeyValue ( fdf, "function8" );
	function9  = GetFdfKeyValue ( fdf, "function9" );
	function10 = GetFdfKeyValue ( fdf, "function10" );

	if ( function   != NULL ) {
	    nfunction++;
	    Free ( function   );
	}
	if ( function2  != NULL ) {
	    nfunction++;
	    Free ( function2  );
	}
	if ( function3  != NULL ) {
	    nfunction++;
	    Free ( function3  );
	}
	if ( function4  != NULL ) {
	    nfunction++;
	    Free ( function4  );
	}
	if ( function5  != NULL ) {
	    nfunction++;
	    Free ( function5  );
	}
	if ( function6  != NULL ) {
	    nfunction++;
	    Free ( function6  );
	}
	if ( function7  != NULL ) {
	    nfunction++;
	    Free ( function7  );
	}
	if ( function8  != NULL ) {
	    nfunction++;
	    Free ( function8  );
	}
	if ( function9  != NULL ) {
	    nfunction++;
	    Free ( function9  );
	}
	if ( function10 != NULL ) {
	    nfunction++;
	    Free ( function10 );
	}

	return nfunction;
}


void
SetFdfFallbackValues ( KeyListType *fdf )
/*
 *	Set FDF default values if not explicitly defined in FDF.
 */
{
	ModelInfoType		*mdl;
	GuiScalarGridDialogType	*grd;
	GuiVectorGridDialogType	*vec;
	GlobalDataObjectType	*gd;

	char		*vcoord, *level1, *level2, *scale;

	char		*line_color,	*line_type;
	char		*line_width,	*line_label_frequency;

	char		*text_size,	*text_font,	*text_width,
			*text_hw_flag;

	char		*cint_parse,    *fint_parse;
	char		*skip_contour,	*skip_plot_x,	*skip_plot_y;

	char		*wind_symbol,
			*wind_symbol_size,	*wind_symbol_width,
			*wind_symbol_type,	*wind_symbol_headsize;

	int		nfunction, verbose;


	mdl = GetGuiModelInfo();
	grd = GetGuiScalarGridDialog();
	vec = GetGuiVectorGridDialog();
	gd  = GetGlobalDataObject();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetFdfFallbackValues\n" );
/*
 *	If the vertical coordinate and vertical levels are defined in
 *	the FDF, use them. Otherwise, get these values from gui
 *	widgets.
 */
	vcoord 		     = GetFdfKeyValue ( fdf, "vcoord" );
	level1 		     = GetFdfKeyValue ( fdf, "level1" );
	level2		     = GetFdfKeyValue ( fdf, "level2" );
	scale		     = GetFdfKeyValue ( fdf, "scale" );
	cint_parse           = GetFdfKeyValue ( fdf, "cint_parse" );
	fint_parse           = GetFdfKeyValue ( fdf, "fint_parse" );
	line_color           = GetFdfKeyValue ( fdf, "line_color" );
	line_type	     = GetFdfKeyValue ( fdf, "line_type" );
	line_width	     = GetFdfKeyValue ( fdf, "line_width" );
	line_label_frequency = GetFdfKeyValue ( fdf, "line_label_frequency" );
	text_size	     = GetFdfKeyValue ( fdf, "text_size" );
	text_font	     = GetFdfKeyValue ( fdf, "text_font" );
	text_width	     = GetFdfKeyValue ( fdf, "text_width" );
	text_hw_flag	     = GetFdfKeyValue ( fdf, "text_hw_flag" );
	skip_contour         = GetFdfKeyValue ( fdf, "skip_contour" );
	skip_plot_x          = GetFdfKeyValue ( fdf, "skip_plot_x" );
	skip_plot_y          = GetFdfKeyValue ( fdf, "skip_plot_y" );
	wind_symbol	     = GetFdfKeyValue ( fdf, "wind_symbol" );
	wind_symbol_size     = GetFdfKeyValue ( fdf, "wind_symbol_size" );
	wind_symbol_width    = GetFdfKeyValue ( fdf, "wind_symbol_width" );
	wind_symbol_type     = GetFdfKeyValue ( fdf, "wind_symbol_type" );
	wind_symbol_headsize = GetFdfKeyValue ( fdf, "wind_symbol_headsize" );


/*
 *	Parameters which have GUI components. Look at option menus first.
 */
	if ( vcoord != NULL ) {
	    ToLower ( vcoord );
	    strbrm ( vcoord );
	    FreeModelVCoord ( mdl );
	    SetModelVCoord ( mdl, vcoord );
	    Free ( vcoord );
	}

/*
 *	Textfield widgets.
 */
	if ( level1 != NULL ) {
	    FreeModelLevel1 ( mdl );
	    SetModelLevel1 ( mdl, level1 );
	    Free ( level1 );
	}

	if ( level2 != NULL ) {
	    FreeModelLevel2 ( mdl );
	    SetModelLevel2 ( mdl, level2 );
	    Free ( level2 );
	}


	FreeScaleFactorHS ( grd );
	if ( scale == NULL ) {
	    SetScaleFactorHS ( grd, " " );
	}
	else {
	    SetScaleFactorHS ( grd, scale );
	    Free ( scale );
	}

	FreeLineTypeHS ( grd );
	if ( line_type == NULL ) {
	    SetLineTypeHS ( grd, "1" );
	}
	else {
	    SetLineTypeHS ( grd, line_type );
	    Free ( line_type );
	}

	FreeLabelFrequencyHS ( grd );
	if ( line_label_frequency == NULL ) {
	    SetLabelFrequencyHS ( grd, "2" );
	}
	else {
	    SetLabelFrequencyHS ( grd, line_label_frequency );
	    Free ( line_label_frequency );
	}
/*
 *	Set other fallback values.
 */

	FreeLineColorHS ( grd );
	if ( line_color == NULL ) {
	    SetLineColorHS ( grd, "NONE" );
	}
	else {
	    SetLineColorHS ( grd, line_color );
	    Free ( line_color );
	}

	FreeLineWidthHS ( grd );
	if ( line_width == NULL ) {
	    SetLineWidthHS ( grd, "2" );
	}
	else {
	    SetLineWidthHS ( grd, line_width );
	    Free ( line_width );
	}

	if ( cint_parse == NULL ) {
	    PutInFDF ( "cint_parse", "Y", fdf, OVERWRITE );
	}
	else {
	    ToUpper ( cint_parse );
	    PutInFDF ( "cint_parse", cint_parse, fdf, OVERWRITE );
	    Free ( cint_parse );
	}

	if ( fint_parse == NULL ) {
	    PutInFDF ( "fint_parse", "Y", fdf, OVERWRITE );
	}
	else {
	    ToUpper ( fint_parse );
	    PutInFDF ( "fint_parse", fint_parse, fdf, OVERWRITE );
	    Free ( fint_parse );
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

	if ( skip_contour == NULL ) {
	    PutInFDF ( "skip_contour", "0", fdf, OVERWRITE );
	}
	else {
	    PutInFDF ( "skip_contour", skip_contour, fdf, OVERWRITE );
	    Free ( skip_contour );
	}

	if ( skip_plot_x == NULL ) {
	    PutInFDF ( "skip_plot_x", "1", fdf, OVERWRITE );
	}
	else {
	    PutInFDF ( "skip_plot_x", skip_plot_x, fdf, OVERWRITE );
	    Free ( skip_plot_x );
	}

	if ( skip_plot_y == NULL ) {
	    PutInFDF ( "skip_plot_y", "1", fdf, OVERWRITE );
	}
	else {
	    PutInFDF ( "skip_plot_y", skip_plot_y, fdf, OVERWRITE );
	    Free ( skip_plot_y );
	}

	FreeVectorSymbolHS ( grd );
	if ( wind_symbol == NULL ) {
	    SetVectorSymbolHS ( grd, "bk" );
	}
	else {
	    SetVectorSymbolHS ( grd, wind_symbol );
	    Free ( wind_symbol );
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

/*
 *	Count the number of functions contained in the FDF.
 */
	nfunction = CountFdfFunctions ( fdf );
	SetNumFunctionHS ( grd, nfunction );

	return;
}


void
SetGridPlotType ( KeyListType *kl )
{
	GuiScalarGridDialogType	*grd;

	char			*contour, *value, *fill, *hilo;
	int			verbose;

	grd = GetGuiScalarGridDialog();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetGridPlotType\n" );

/*
 *	Find out whether to plot contour line, fill, hilo symbols.
 */
	contour			= GetFdfKeyValue ( kl, "contour" );
	value			= GetFdfKeyValue ( kl, "value" );
	fill			= GetFdfKeyValue ( kl, "fill" );
	hilo			= GetFdfKeyValue ( kl, "hilo" );

/*
 *	Change toggle button states as given by FDF input.
 */
	if ( contour != NULL ) {
	    if ( *contour == 'y' || *contour == 'Y' )
		if ( ! XmToggleButtonGetState ( GetLineToggleHSW1 ( grd ) ) ) {
		    XmToggleButtonSetState (
		            GetLineToggleHSW1 ( grd ), TRUE, FALSE );
		    XmToggleButtonSetState (
		            GetLineToggleHSW2 ( grd ), TRUE, FALSE );
		}
		if ( XmToggleButtonGetState ( GetGridValueToggleHSW ( grd ) ) )
		    XmToggleButtonSetState (
			    GetGridValueToggleHSW ( grd ), FALSE, FALSE );

	    if ( *contour == 'n' || *contour == 'N' )
		if ( XmToggleButtonGetState ( GetLineToggleHSW1 ( grd ) ) ) {
		    XmToggleButtonSetState (
		            GetLineToggleHSW1 ( grd ), FALSE, FALSE );
		    XmToggleButtonSetState (
		            GetLineToggleHSW2 ( grd ), FALSE, FALSE );
		}
	    Free ( contour );
	}

/*
 *	Change toggle button states as given by FDF input.
 */
	if ( value != NULL ) {
	    if ( *value == 'y' || *value == 'Y' )
		if (!XmToggleButtonGetState (GetGridValueToggleHSW( grd ) ) )
		    XmToggleButtonSetState (
			    GetGridValueToggleHSW ( grd ), TRUE, FALSE );
		
		if ( XmToggleButtonGetState ( GetLineToggleHSW1 ( grd ) ) ) {
		    XmToggleButtonSetState (
			    GetLineToggleHSW1 ( grd ), FALSE, FALSE );
		    XmToggleButtonSetState (
			    GetLineToggleHSW2 ( grd ), FALSE, FALSE );
		}

	    if ( *value == 'n' || *value == 'N' )
		if ( XmToggleButtonGetState (GetGridValueToggleHSW( grd ) ) )
		    XmToggleButtonSetState (
			    GetGridValueToggleHSW ( grd ), FALSE, FALSE );

		if (! XmToggleButtonGetState ( GetGridValueToggleHSW ( grd ) ) ) {
		    XmToggleButtonSetState (
		            GetLineToggleHSW1 ( grd ), TRUE, FALSE );
		    XmToggleButtonSetState (
		            GetLineToggleHSW2 ( grd ), TRUE, FALSE );
		}
	    Free ( value );
	}

	if ( fill != NULL ) {
	    if ( *fill == 'y' || *fill == 'Y' )
		if ( ! XmToggleButtonGetState ( GetFillToggleHSW1 ( grd ) ) ) {
		    XmToggleButtonSetState (
		            GetFillToggleHSW1 ( grd ), TRUE, FALSE );
		    XmToggleButtonSetState (
		            GetFillToggleHSW2 ( grd ), TRUE, FALSE );
		}
	    if ( *fill == 'n' || *fill == 'N' )
		if ( XmToggleButtonGetState ( GetFillToggleHSW1 ( grd ) ) ) {
		    XmToggleButtonSetState (
		            GetFillToggleHSW1 ( grd ), FALSE, FALSE );
		    XmToggleButtonSetState (
		            GetFillToggleHSW2 ( grd ), FALSE, FALSE );
		}
	    Free ( fill );
	}

	if ( hilo != NULL ) {
	    if ( *hilo == 'y' || *hilo == 'Y' )
		if ( ! XmToggleButtonGetState ( GetSymbolToggleHSW1 ( grd ) ) ) {
		    XmToggleButtonSetState (
		            GetSymbolToggleHSW1 ( grd ), TRUE, FALSE );
		    XmToggleButtonSetState (
		            GetSymbolToggleHSW2 ( grd ), TRUE, FALSE );
		}
		if ( *hilo == 'n' || *hilo == 'N' )
		    if ( XmToggleButtonGetState ( GetSymbolToggleHSW1 ( grd ) ) ) {
			XmToggleButtonSetState (
			        GetSymbolToggleHSW1 ( grd ), FALSE, FALSE );
			XmToggleButtonSetState (
			        GetSymbolToggleHSW2 ( grd ), FALSE, FALSE );
		    }
	    Free ( hilo );
	}
}


void
SetGridOptionMenuButtons ( KeyListType *kl )
{
/*
 * Dynamically set option menus according to FDF parameter values.
 */
	GuiScalarGridDialogType	*grd;
	ModelInfoType		*mdl;

	char			*keylist;
	char			**vcoords, **vectorsym, **label_frequency,
				**linetype;
	int			nkeys;
	int			verbose;

	grd = GetGuiScalarGridDialog();
	mdl = GetGuiModelInfo();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetGridOptionMenuButtons\n" );

/*
 *	Set option menu button and text field for vertical coordinate.
 */
	keylist   = GetConfigValue( "vcoordkeys" );
	vcoords  = SplitByDelimeter ( keylist, ",", &nkeys );
	XmTextSetString ( GetModelVCoordHSW ( mdl ), GetModelVCoord ( mdl ) );
	XtUnmanageChild ( GetModelVCoordOmHSW ( grd ) );
	SetDefaultOptionButton ( GetModelVCoordOmHSW ( grd ),
				 nkeys, GetModelVCoord ( mdl ), vcoords,
				 &(grd->vcoordpm), "vcoordkeys" );
	XtManageChild ( GetModelVCoordOmHSW ( grd ) );
	Free ( keylist );
	StringListFree ( nkeys, vcoords );

/*
 *	Set option menu button for vector symbols.
 */
	keylist   = GetConfigValue( "vectorkeys" );
	vectorsym = SplitByDelimeter ( keylist, ",", &nkeys );
	XtUnmanageChild ( GetVectorSymbolOmHSW( grd ) );
	SetDefaultOptionButton ( GetVectorSymbolOmHSW( grd ),
				 nkeys, GetVectorSymbolHS ( grd ), vectorsym,
				 &(grd->vector_sym_pm), "vectorkeys" );
	XtManageChild ( GetVectorSymbolOmHSW( grd ) );
	Free ( keylist );
	StringListFree ( nkeys, vectorsym );

/*
 *	Set option menu button for line type.
 */
	keylist   = GetConfigValue( "linetypekeys" );
	linetype = SplitByDelimeter ( keylist, ",", &nkeys );
	XtUnmanageChild ( GetLineTypeOmHSW ( grd ) );
	SetDefaultOptionButton ( GetLineTypeOmHSW ( grd ),
				 nkeys, GetLineTypeHS ( grd ), linetype,
				 &(grd->line_type_pm), "linetypekeys" );
	XtManageChild ( GetLineTypeOmHSW ( grd ) );
	Free ( keylist );
	StringListFree ( nkeys, linetype );

/*
 *	Set option menu button for line label frequency.
 */
	keylist   = GetConfigValue( "labelfreqkeys" );
	label_frequency = SplitByDelimeter ( keylist, ",", &nkeys );
	XtUnmanageChild ( GetLabelFrequencyOmHSW ( grd ) );
	SetDefaultOptionButton ( GetLabelFrequencyOmHSW ( grd ),
				 nkeys, GetLabelFrequencyHS ( grd ),
                                 label_frequency, &(grd->label_freq_pm),
                                 "labelfreqkeys" );
	XtManageChild ( GetLabelFrequencyOmHSW ( grd ) );
	Free ( keylist );
	StringListFree ( nkeys, label_frequency );
}


void
SetGridWidgets ( KeyListType *fdf, char *file )
{
/*
 * Driver program for setting widget states according to FDF input.
 */
	Widget			w;
	XmString		xmstr;

	GuiScalarGridDialogType	*grd;
	ModelInfoType		*mdl;

	char			*function, *description;
	char			*layer,	*fline, *vcoord;
	int			verbose;

	grd = GetGuiScalarGridDialog();
	mdl = GetGuiModelInfo();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetGridWidgets\n" );

	function		= GetFdfKeyValue ( fdf, "function" );
	layer			= GetFdfKeyValue ( fdf, "layer" );
	description		= GetFdfKeyValue ( fdf, "description" );
	fline			= GetFdfKeyValue ( fdf, "fline" );
	vcoord			= strdup ( GetModelVCoord ( mdl ) );

/*
 *	Use FDF keys to obtain values and set corresponding widget values.
 */
						/* function */
	FreeFunctionHS ( grd );
	SetFunctionHS ( grd, function );
	XmTextSetString ( GetFunctionHSW ( grd ), GetFunctionHS ( grd ) );
	if ( function != NULL ) Free ( function );

						/* description */
	FreeFieldInfoHS ( grd );
	SetFieldInfoHS ( grd, description );
	XmTextSetString ( GetFieldInfoHSW ( grd ), GetFieldInfoHS ( grd ) );
	if ( description != NULL ) Free ( description );

/*
 *	Set level textfields.
 */
	XmTextSetString ( GetModelLevel1HSW ( mdl ), GetModelLevel1 ( mdl ) );
	XmTextSetString ( GetModelLevel2HSW ( mdl ), GetModelLevel2 ( mdl ) );

	if ( fline != NULL ) {			/* fline */
	    FreeFillColorsHS ( grd );
	    SetFillColorsHS ( grd, fline );
	    Free ( fline );
	}

/*
 *      Set level/layer radio button.
 */
	if ( layer != NULL ) {
            SetModelLevelTypeCB ( w, (XtPointer) layer,
                              	     (XtPointer) NULL );
	    Free ( layer );
	}

/*
 *	Set fdf name in main dialog.
 */
	FreeModelFdfHS ( grd );
	SetModelFdfHS ( grd, file );

        xmstr = XmStringLtoRCreate ( GetModelFdfHS ( grd ),
					XmSTRING_DEFAULT_CHARSET );
        XtVaSetValues ( GetModelFdfHSW ( grd ),
			XmNlabelString, xmstr,
			NULL);
        XmStringFree ( xmstr );

/*
 *	Set scale factor textfield.
 */
	XmTextSetString ( GetScaleFactorHSW ( grd ), GetScaleFactorHS ( grd ) );

/*
 *	Plot contour lines and/or color fill and/or symbols.
 */
	SetGridPlotType ( fdf );

/*
 *	Set line and fill intervals.
 */
	SetContourLineInfo ( fdf, vcoord, GetModelLevel1 ( mdl ) );

/*
 *	Set option menu buttons.
 */
	SetGridOptionMenuButtons ( fdf );

	Free ( vcoord );
}
