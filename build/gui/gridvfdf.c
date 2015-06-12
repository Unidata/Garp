/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gridvfdf.c
 *
 *	FDF related callbacks for vertical profile model data.	
 *
 *	History:
 *
 *	12/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 3/97	COMET	Added scale factor text field.
 *	11/97	COMET	Removed redundant checks on Free macros.
 *	11/97	COMET	Fixed small leaks
 *	12/97	COMET	Removed label from SetVPGridWidgets()
 *	12/97	COMET	Get default text settings from keys in "Garp_defaults".
 *	 4/99   COMET   Added keylist string to SetDefaultOptionButton.
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


void SetVPFdfFallbackValues ( KeyListType * );
void SetVPGridWidgets ( KeyListType *, char * );
void SetVPOptionMenuButtons ( KeyListType * );
void SetVPGridPlotType ( KeyListType * );


void
SetVPFdfFallbackValues ( KeyListType *fdf )
/*
 *	Set FDF default values if not explicitly defined in FDF.
 */
{
	GuiVertProfileObjectType	*vpt;
	GlobalDataObjectType		*gd;

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


	vpt = GetGuiVertProfileDialog();
	gd  = GetGlobalDataObject();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetVPFdfFallbackValues\n" );
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
	    vcoord = strdup ( GetModelVCoordVP ( vpt ) );
	    PutInFDF ( "vcoord", vcoord, fdf, OVERWRITE );
	    Free ( vcoord );
	}
	else {
	    FreeModelVCoordVP ( vpt );
	    SetModelVCoordVP ( vpt, vcoord );
	    Free ( vcoord );
	}

	if ( ptype_axis != NULL ) {
	    FreeAxisTypeVP ( vpt );
	    SetAxisTypeVP ( vpt, ptype_axis );
	    Free ( ptype_axis );
	}

/*
 *	Textfield widgets.
 */
	FreeScaleFactorVP ( vpt );
	if ( scale == NULL ) {
	    SetScaleFactorVP ( vpt, " " );
	}
	else {
	    SetScaleFactorVP ( vpt, scale );
	    Free ( scale );
	}

	FreeLineTypeVP ( vpt );
	if ( line_type == NULL ) {
	    SetLineTypeVP ( vpt, "1" );
	}
	else {
	    SetLineTypeVP ( vpt, line_type );
	    Free ( line_type );
	}

	FreeVectorSymbolVP ( vpt );
	if ( wind_symbol == NULL ) {
	    SetVectorSymbolVP ( vpt, "bk" );
	}
	else {
	    SetVectorSymbolVP ( vpt, wind_symbol );
	    Free ( wind_symbol );
	}

/*
 *	Set other fallback values.
 */
	FreeLineColorVP ( vpt );
	if ( line_color == NULL ) {
	    SetLineColorVP ( vpt, "NONE" );
	}
	else {
	    SetLineColorVP ( vpt, line_color );
	    Free ( line_color );
	}

	FreeLineWidthVP ( vpt );
	if ( line_width == NULL ) {
	    SetLineWidthVP ( vpt, "2" );
	}
	else {
	    SetLineWidthVP ( vpt, line_width );
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
SetVPGridWidgets ( KeyListType *fdf, char *file )
{
	Widget			w;
	XmString		xmstr;

	GuiVertProfileObjectType	*vpt;

	char			*function, *description;
	char			*level1,   *level2;
	int			verbose;

	vpt = GetGuiVertProfileDialog();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetVPGridWidgets\n" );

	function		= GetFdfKeyValue ( fdf, "function" );
	description		= GetFdfKeyValue ( fdf, "description" );
	level1			= GetFdfKeyValue ( fdf, "yaxis_lower" );
	level2			= GetFdfKeyValue ( fdf, "yaxis_upper" );

/*
 *	Use FDF keys to obtain values and set corresponding widget values.
 */
						/* function */
	FreeFunctionVP ( vpt );	
	SetFunctionVP ( vpt, function );
	XmTextSetString ( GetFunctionVPW ( vpt ), GetFunctionVP ( vpt ) );
	if ( function != NULL ) Free ( function );

						/* description */
	FreeFieldInfoVP ( vpt );
	SetFieldInfoVP ( vpt, description );
	XmTextSetString ( GetFieldInfoVPW ( vpt ), GetFieldInfoVP ( vpt ) );
	if ( description != NULL ) Free ( description );

/*
 *	Set level textfields.
 */
	if ( level1 != NULL ) {
	    FreeLevel1VP ( vpt );
	    SetLevel1VP ( vpt, level1 );
	    Free ( level1 );
	    XmTextSetString ( GetLevel1VPW ( vpt ), GetLevel1VP ( vpt ) );
	}

	if ( level2 != NULL ) {
	    FreeLevel2VP ( vpt );
	    SetLevel2VP ( vpt, level2 );
	    Free ( level2 );
	    XmTextSetString ( GetLevel2VPW ( vpt ), GetLevel2VP ( vpt ) );
	}

/*
 *	Set fdf name in main dialog.
 */
	FreeModelFdfVP ( vpt );
	SetModelFdfVP ( vpt, file );

        xmstr = XmStringLtoRCreate ( GetModelFdfVP ( vpt ),
					XmSTRING_DEFAULT_CHARSET );
        XtVaSetValues ( GetModelFdfVPW ( vpt ),
			XmNlabelString, xmstr,
			NULL);
        XmStringFree ( xmstr );

/*
 *	Set scale factor textfield.
 */
	XmTextSetString ( GetScaleFactorVPW ( vpt ), GetScaleFactorVP ( vpt ) );

/*
 *	Set contour line characteristics.
 */
/*	SetGridLineWidgets ( fdf ); */

/*
 *	Set option menu buttons.
 */
	SetVPOptionMenuButtons ( fdf );
}


void
SetVPOptionMenuButtons ( KeyListType *fdf )
{
	GuiVertProfileObjectType	*vpt;

	char			*keylist;
	char			**vcoords, **vectorsym,
				**linetype, **yaxis;
	int			nkeys;
	int			verbose;

	vpt = GetGuiVertProfileDialog();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SetVPOptionMenuButtons\n" );

/*
 *	Set option menu button and text field for vertical coordinate.
 */
	keylist   = GetConfigValue( "vcoordkeys" );
	vcoords  = SplitByDelimeter ( keylist, ",", &nkeys );
	XmTextSetString ( GetModelVCoordVPW ( vpt ), GetModelVCoordVP ( vpt ) );
	XtUnmanageChild ( GetModelVCoordOmVPW ( vpt ) );
	SetDefaultOptionButton ( GetModelVCoordOmVPW ( vpt ),
				 nkeys, GetModelVCoordVP ( vpt ), vcoords,
				 &(vpt->vcoordpm), "vcoordkeys" );
	XtManageChild ( GetModelVCoordOmVPW ( vpt ) );
	Free ( keylist );
	StringListFree ( nkeys, vcoords );

/*
 *	Set option menu button for yaxis type.
 */
	keylist   = GetConfigValue( "yaxiskeys" );
	yaxis = SplitByDelimeter ( keylist, ",", &nkeys );
	XtUnmanageChild ( GetAxisTypeOmVPW ( vpt ) );
	SetDefaultOptionButton ( GetAxisTypeOmVPW ( vpt ),
				nkeys, GetAxisTypeVP ( vpt ), yaxis,
				&(vpt->axis_type_pm), "yaxiskeys" );
	XtManageChild ( GetAxisTypeOmVPW ( vpt ) );
	Free ( keylist );
	StringListFree ( nkeys, yaxis );

/*
 *	Set option menu button for vector symbols.
 */
	keylist   = GetConfigValue( "vectorkeys" );
	vectorsym = SplitByDelimeter ( keylist, ",", &nkeys );
	XtUnmanageChild ( GetVectorSymbolOmVPW( vpt ) );
	SetDefaultOptionButton ( GetVectorSymbolOmVPW( vpt ),
				 nkeys, GetVectorSymbolVP ( vpt ), vectorsym,
				 &(vpt->vector_sym_pm), "vectorkeys" );
	XtManageChild ( GetVectorSymbolOmVPW( vpt ) );
	Free ( keylist );
	StringListFree ( nkeys, vectorsym );

/*
 *	Set option menu button for line type.
 */
	keylist   = GetConfigValue( "linetypekeys" );
	linetype = SplitByDelimeter ( keylist, ",", &nkeys );
	XtUnmanageChild ( GetLineTypeOmVPW ( vpt ) );
	SetDefaultOptionButton ( GetLineTypeOmVPW ( vpt ),
				 nkeys, GetLineTypeVP ( vpt ), linetype,
				 &(vpt->line_type_pm), "linetypekeys" );
	XtManageChild ( GetLineTypeOmVPW ( vpt ) );
	Free ( keylist );
	StringListFree ( nkeys, linetype );

}
