/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	displaysurf.c
 *
 *	Surface observation display driver.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/97	COMET	Use title string in mot for consistency across
 *			data types.
 *	10/97	COMET	Added nstations to psurf arg list
 *	11/97	COMET	Added titleIndex to psurf arg list.
 *
 ***********************************************************************/


#include "genglobs.h"
#include "underscore.h"
#include "winobj.h"
#include "wincb.h"


int DisplaySurface( WindowObjectType *wo, MetObjectType *mot )
{
	char			title [FILENAMESTRING], 
				sfparmstr[FILENAMESTRING];
	char			sizestr[GEMPAKSTRING], widthstr[GEMPAKSTRING],
				fontstr[GEMPAKSTRING], filterstr[GEMPAKSTRING];
	int			size, width, font_size, filter;
	int			verbose, iret;

	SurfaceObjectType       *sobj;
	MetObjectType		*gmobj;


	sobj = (SurfaceObjectType *) mot->metObjectContent;


	assert( sobj );

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "DisplaySurface\n" );

/*
 *	Save original FDF settings.
 */
	strcpy ( sfparmstr, sobj->sfparm );
	strcpy ( fontstr, sobj->text );
	strcpy ( filterstr, sobj->filter );
	strcpy ( sizestr, sobj->symbol_size );
	strcpy ( widthstr, sobj->symbol_width );

/*
 *	Set plotting parameters according to generalized values.
 */
	GetDepictableValue ( "SYMBOLSIZE", &size );
	GetDepictableValue ( "SYMBOLWIDTH", &width );
	GetDepictableValue ( "FONTSIZE", &font_size );
	GetDepictableValue ( "SYMBOLPACKING", &filter );
	
	SetGemParmVal ( SURFACEOBJECT, sfparmstr, "SYMBOLSIZE", size );
	SetGemParmVal ( SURFACEOBJECT, sfparmstr, "SYMBOLWIDTH", width );
	SetGemText ( SURFACEOBJECT, fontstr, "FONTSIZE", font_size );
	SetGemFloatVar ( SURFACEOBJECT, filterstr, "SYMBOLPACKING", filter );
	SetGemFloatVar ( SURFACEOBJECT, sizestr, "SYMBOLSIZE", size );
	SetGemIntVar ( SURFACEOBJECT, widthstr, "SYMBOLWIDTH", width );
	strcpy ( title, mot->titleString );
	SetGemTitle ( sobj->colors, mot->titleIndex, title );

	psurf(	mot->area,		sfparmstr,
		sobj->dattim,		mot->filename,
		sobj->colors,		title,
		filterstr,		fontstr,
		sizestr,		widthstr,
		&(sobj->marker),	&(sobj->pmarker),	
		&(sobj->skpmis),	&(sobj->nstations),
		&(mot->titleIndex),	&verbose,		
		&iret, 
		strlen(mot->area),	strlen(sfparmstr),
		strlen(sobj->dattim),	strlen(mot->filename),
		strlen(sobj->colors),	strlen(title),
		strlen(filterstr),	strlen(fontstr),
		strlen(sizestr),	strlen(widthstr) );

	SetMetObjectDrawnFlag( mot, True );

	return(0);

}
