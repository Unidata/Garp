/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	displayvectorh.c
 *
 *	Plan view vector grid display driver.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 4/97	COMET	Added "scale" parameter to VECTORGRIDOBJECT.
 *	11/97	COMET	Added titleIndex to pstream and pvgrid arg lists.
 *
 ***********************************************************************/


#include "underscore.h"
#include "winobj.h"
#include "wincb.h"
#include "genglobs.h"


int DisplayVectorGrid( WindowObjectType *wo, MetObjectType *mot )
{
/*
 *	Prepare a horizontal vector Met object for plotting with a 
 *	GEMPAK driver subroutine.
 */
	char			wind[GEMPAKSTRING], title[FILENAMESTRING];
	char			sizestr[GEMPAKSTRING], widthstr[GEMPAKSTRING];
	char			fontstr[GEMPAKSTRING], skipstr[GEMPAKSTRING];
	char			skip[GEMPAKSTRING], skip_x[GEMPAKSTRING],
				skip_y[GEMPAKSTRING];
	int			size, width, font_size, filter;
	int			verbose, iret;

	VectorGridObjectType	*vot;

	vot = (VectorGridObjectType *) mot->metObjectContent;

	assert( vot );

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "DisplayVectorGrid\n" );


/*
 *	Save original FDF settings.
 */
	strcpy ( sizestr, vot->symbol_size );
	strcpy ( widthstr, vot->symbol_width );
	strcpy ( fontstr, vot->text );
	strcpy ( skip, vot->skip_contour );
	strcpy ( skip_x, vot->skip_plot_x );
	strcpy ( skip_y, vot->skip_plot_y );

/*
 *	Set plotting parameters according to generalized values.
 */
	GetDepictableValue ( "SYMBOLSIZE", &size );
	GetDepictableValue ( "SYMBOLWIDTH", &width );
	GetDepictableValue ( "FONTSIZE", &font_size );
	GetDepictableValue ( "SYMBOLPACKING", &filter );

	SetGemText ( VECTORGRIDOBJECT, fontstr, "FONTSIZE", font_size );
	SetGemFloatVar ( VECTORGRIDOBJECT, sizestr, "SYMBOLSIZE", size );
	SetGemFloatVar ( VECTORGRIDOBJECT, widthstr, "SYMBOLWIDTH", width );
	SetGemIntVar ( VECTORGRIDOBJECT, skip, "SYMBOLPACKING", filter );
	SetGemIntVar ( VECTORGRIDOBJECT, skip_x, "SYMBOLPACKING", filter );
	SetGemIntVar ( VECTORGRIDOBJECT, skip_y, "SYMBOLPACKING", filter );

	sprintf ( skipstr, "%s/%s;%s", skip, skip_x, skip_y );
	sprintf ( wind, "%s%s/%s/%s/%s/%s",
			vot->wind_symbol, vot->symbol_color,
		 	sizestr         , widthstr         ,
			vot->symbol_type, vot->symbol_headsize );
	strcpy ( title, mot->titleString );
	SetGemTitle ( wind, mot->titleIndex, title );

/*
 *	Gempak driver.
 */
	if (  strcasecmp ( vot->wind_symbol, "st" ) == 0 )

	    pstream(
		mot->filename,		vot->gdatim,
 	        vot->glevel,		vot->gvcord,
		vot->gvect,		vot->symbol_color,
	        wind,			vot->refvec,
	        skipstr,		title,
		fontstr,		&(vot->nfunction),
		&(mot->titleIndex),
		&verbose,		&iret, 
	        strlen(mot->filename),	strlen(vot->gdatim),
		strlen(vot->glevel),	strlen(vot->gvcord),
	        strlen(vot->gvect),	strlen(vot->symbol_color),
	        strlen(wind),		strlen(vot->refvec),
	        strlen(skipstr),	strlen(title),
		strlen(fontstr) );

	else
	    pvgrid(
		mot->filename,		vot->gdatim,
 	        vot->glevel,		vot->gvcord,
		vot->gvect,		vot->gridtype,
		wind,			vot->refvec,
		vot->scale,		skipstr,
		title,			fontstr,
		&(vot->nfunction),	&(mot->titleIndex),
		&verbose,		&iret, 
	        strlen(mot->filename),	strlen(vot->gdatim),
		strlen(vot->glevel),	strlen(vot->gvcord),
	        strlen(vot->gvect),	strlen(vot->gridtype),
		strlen(wind),		strlen(vot->refvec),
		strlen(vot->scale),	strlen(skipstr),
		strlen(title),		strlen(fontstr) );

	SetMetObjectDrawnFlag( mot, True );

	return(0);

}
