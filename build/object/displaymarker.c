/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	displaymarker.c
 *
 *	Marker display driver.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include "underscore.h"
#include "winobj.h"
#include "wincb.h"
#include "genglobs.h"


int DisplayMarker( WindowObjectType *wo, MetObjectType *mot )
{
	char			sizestr[GEMPAKSTRING], widthstr[GEMPAKSTRING],
				fontstr[GEMPAKSTRING];
	int			size, width, font_size;
	int			verbose, iret;

	MarkerObjectType        *mobj;
	MetObjectType		*gmobj;

	mobj = (MarkerObjectType *) mot->metObjectContent;

	assert( mobj );

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "DisplayMarker\n" );

/*
 *	Save original FDF settings.
 */
	strcpy ( sizestr, mobj->symbol_size );
	strcpy ( widthstr, mobj->symbol_width );
	strcpy ( fontstr, mobj->text );

/*
 *	Set plotting parameters according to generalized values.
 */
	GetDepictableValue ( "SYMBOLSIZE", &size );
	GetDepictableValue ( "SYMBOLWIDTH", &width );
	GetDepictableValue ( "FONTSIZE", &font_size );

	SetGemFloatVar ( MARKEROBJECT, sizestr, "SYMBOLSIZE", size );
	SetGemIntVar ( MARKEROBJECT, widthstr, "SYMBOLWIDTH", width );
	SetGemText ( MARKEROBJECT, fontstr, "FONTSIZE", font_size );

	psym ( 	mobj->stations,		sizestr,
		widthstr,		fontstr,
		mobj->lat,
		mobj->lon,		&mobj->nstn,
		&mobj->stidlen,		&mobj->marker,
		&mobj->color,		&mobj->plot_stn,
		&mobj->plot_marker,	&verbose,
		&iret,
		strlen(mobj->stations), strlen(sizestr),
		strlen(widthstr),	strlen(fontstr)  );

	SetMetObjectDrawnFlag( mot, True );

	return(0);

}
