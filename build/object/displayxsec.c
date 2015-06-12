/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	displayxsec.c
 *
 *	Cross section scalar and vector grid display driver.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	03/97	J. Cowie/COMET	Changed to allow contour and fill info
 *				to be saved back into the metobject
 *	11/97	COMET	Added titleIndex to pxsec arg list.
 *
 ***********************************************************************/


#include "underscore.h"
#include "winobj.h"
#include "wincb.h"
#include "genglobs.h"
#include "_proto.h"


int DisplayXSection( WindowObjectType *wo, MetObjectType *mot )
{
/*
 *	Prepare a horizontal scalar grid Met object for plotting with a 
 *	GEMPAK driver subroutine.
 */

	char			**strarray;
	char			title[FILENAMESTRING], ctype[GEMPAKSTRING];
	char			cint[GEMPAKSTRING], fint[GEMPAKSTRING];
	char			line[GEMPAKSTRING], wind[GEMPAKSTRING];
	char			sizestr[GEMPAKSTRING], widthstr[GEMPAKSTRING];
	char			fontstr[GEMPAKSTRING], skipstr[GEMPAKSTRING];
	int			size, width, font_size, filter;
	int			i, verbose, frame, num, iret;

	XSectionGridObjectType	*xot;

	xot = (XSectionGridObjectType *) mot->metObjectContent;

	assert( xot );

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "DisplayXSection\n" );

	frame = GetActivePixmapObjectIndex (wo) + 1;

/*
 *	Save original FDF settings.
 */
	strcpy ( sizestr, xot->symbol_size );
	strcpy ( widthstr, xot->symbol_width );
	strcpy ( fontstr, xot->text );
	strcpy ( skipstr, xot->skip );

/*
 *	Set plotting parameters according to generalized values.
 */
	GetDepictableValue ( "SYMBOLSIZE", &size );
	GetDepictableValue ( "SYMBOLWIDTH", &width );
	GetDepictableValue ( "FONTSIZE", &font_size );
	GetDepictableValue ( "SYMBOLPACKING", &filter );

	SetGemText ( XSECTIONGRIDOBJECT, fontstr, "FONTSIZE", font_size );
	SetGemFloatVar ( XSECTIONGRIDOBJECT, sizestr, "SYMBOLSIZE", size );
	SetGemFloatVar ( XSECTIONGRIDOBJECT, widthstr, "SYMBOLWIDTH", width);
	SetGemIntVar ( XSECTIONGRIDOBJECT, skipstr, "SYMBOLPACKING", filter );

	sprintf ( cint, "%s/%s/%s", xot->cint, xot->cmin, xot->cmax );
	sprintf ( fint, "%s/%s/%s", xot->fint, xot->fmin, xot->fmax );

	bpad ( cint, GEMPAKSTRING, &iret );
	bpad ( fint, GEMPAKSTRING, &iret );

/*
 *	Check for plotting contour lines, fill or symbol.
 */
	strcpy ( ctype, "" );
	if ( xot->contour ) strcat ( ctype, "C" );
	if ( xot->fill )    strcat ( ctype, "/F" );

/*
 *	Set contour line characteristics.
 */
	sprintf ( line, "%s/%s/%s/%s", xot->line_color, xot->line_type,
	                               xot->line_width, xot->label_freq );

	sprintf ( wind, "%s%s/%s/%s/%s/%s",
			xot->wind_symbol, xot->line_color,
			sizestr         , widthstr       ,
			xot->symbol_type, xot->symbol_headsize );
/*
 *	Make sure SCALE is defined.
 */
	if ( ! xot->scale ) strcpy ( xot->scale, "0" );

/*
 *	Title.
 */
	strcpy (title, mot->titleString );
	SetGemTitle ( line, mot->titleIndex, title );

	pxsec(	mot->filename,		xot->gdatim,
 	        xot->gfunc,		xot->gvcord,
		xot->gvect,		xot->cxstns,
		ctype,			xot->ptype,
		xot->yaxis,		xot->scale,
		wind,			cint,
	        line,			xot->contur,
		fint,			xot->fline,
		xot->clrbar,		title,
		skipstr,		xot->refvec,
		fontstr,		&frame,
		&(mot->titleIndex),
		&verbose,		&iret, 
	        strlen(mot->filename),	strlen(xot->gdatim),
		strlen(xot->gfunc),	strlen(xot->gvcord),
	        strlen(xot->gvect),	strlen(xot->cxstns),
	        strlen(ctype),		strlen(xot->ptype),
	        strlen(xot->yaxis),	strlen(xot->scale),
	        strlen(wind),		GEMPAKSTRING-1,
	        strlen(line),		strlen(xot->contur),
	        GEMPAKSTRING-1,		strlen(xot->fline),
		strlen(xot->clrbar),	strlen(title),
		strlen(skipstr),	strlen(xot->refvec),
		strlen(fontstr) );

/*
 *	Save the contour and fill info back into the metobject
 */
	if ( xot->contour ) {
	    cint[GEMPAKSTRING-1] = '\0';
	    strbrm (cint);
	    strarray = SplitByDelimeter ( cint, "/", &num );
	    if ( strarray [0] != NULL ) strcpy ( xot->cint, strarray [0]);
	    if ( strarray [1] != NULL ) strcpy ( xot->cmin, strarray [1]);
	    if ( strarray [2] != NULL ) strcpy ( xot->cmax, strarray [2]);
	    StringListFree ( num, strarray );
	}
	if ( xot->fill ) {
	    fint[GEMPAKSTRING-1] = '\0';
	    strbrm (fint);
	    strarray = SplitByDelimeter ( fint, "/", &num );
	    if ( strarray [0] != NULL ) strcpy ( xot->fint, strarray [0]);
	    if ( strarray [1] != NULL ) strcpy ( xot->fmin, strarray [1]);
	    if ( strarray [2] != NULL ) strcpy ( xot->fmax, strarray [2]);
	    StringListFree ( num, strarray );
	}

/*
	if ( iret ) PrintGemError ( errmsg, errparm );
*/
	SetMetObjectDrawnFlag( mot, True );

	return(0);

}
