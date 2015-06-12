/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	displayupa.c
 *
 *	Radiosonde data display driver.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	1/97	COMET	Use title string in mot for consistency across
 *			data types.
 *	10/97	COMET	Added nstations.
 *	11/97	COMET	Added titleIndex to pupa arg list.
 *
 ***********************************************************************/

 
#include "genglobs.h"
#include "underscore.h"
#include "winobj.h"
#include "wincb.h"


int DisplayUpperair( WindowObjectType *wo, MetObjectType *mot )
{
	char			title [FILENAMESTRING];
	char			snparmstr[GEMPAKSTRING];
	char			fontstr[GEMPAKSTRING], filterstr[GEMPAKSTRING];
	int			size, width, font_size, filter;
	int			verbose, iret;

	UpperairObjectType	*uobj;
	MetObjectType		*gmobj;


	uobj = (UpperairObjectType *) mot->metObjectContent;

	assert( uobj );

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "DisplayUpperair\n" );

/*
 *	Save original FDF settings.
 */
	strcpy ( snparmstr, uobj->snparm );
	strcpy ( fontstr, uobj->text );
	strcpy ( filterstr, uobj->filter );

/*
 *	Set plotting parameters according to generalized values.
 */
	GetDepictableValue ( "SYMBOLSIZE", &size );
	GetDepictableValue ( "SYMBOLWIDTH", &width );
	GetDepictableValue ( "FONTSIZE", &font_size );
	GetDepictableValue ( "SYMBOLPACKING", &filter );

	SetGemParmVal ( UPPERAIROBJECT, snparmstr, "SYMBOLSIZE", size );
	SetGemParmVal ( UPPERAIROBJECT, snparmstr, "SYMBOLWIDTH", width );
	SetGemText ( UPPERAIROBJECT, fontstr, "FONTSIZE", font_size );
	SetGemFloatVar ( UPPERAIROBJECT, filterstr, "SYMBOLPACKING", filter);
	strcpy ( title, mot->titleString);
	SetGemTitle ( uobj->colors, mot->titleIndex, title );

	pupa(	mot->filename,		snparmstr,
	        mot->area,		uobj->dattim,
		uobj->levels,		uobj->vcoord,
		uobj->colors,		uobj->marker,
		title,			filterstr,
	        fontstr,		&(uobj->skpmis),
	        &(uobj->nstations),	&(mot->titleIndex),
		&verbose,		&iret, 
		strlen(mot->filename),	strlen(snparmstr),
	        strlen(mot->area),	strlen(uobj->dattim),
		strlen(uobj->levels),	strlen(uobj->vcoord),
		strlen(uobj->colors),	strlen(uobj->marker),
	        strlen(title),		strlen(filterstr),
	        strlen(fontstr));

	SetMetObjectDrawnFlag( mot, True );

	return(0);

}
