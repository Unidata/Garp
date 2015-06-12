/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	displayupaprof.c
 *
 *	Radiosonde data vertical view display driver.
 *
 *	History:
 *
 *	 1/97	COMET	Original copy
 *	 3/97	COMET	Added wind position indicator.
 *	11/97	COMET	Added titleIndex to psnprof arg list.
 *	12/97	COMET	Fixed small leaks
 *
 ***********************************************************************/

 
#include "genglobs.h"
#include "underscore.h"
#include "winobj.h"
#include "wincb.h"
#include "_proto.h"


int DisplayUpaProfile( WindowObjectType *wo, MetObjectType *mot )
{
	char			title [FILENAMESTRING], wind[GEMPAKSTRING],
				sizestr[GEMPAKSTRING], widthstr[GEMPAKSTRING],
				parms [GEMPAKSTRING], line[GEMPAKSTRING],
				fontstr[GEMPAKSTRING], filterstr[GEMPAKSTRING];

	char			*border, *thtaln, *thteln, 
				*mixrln, *panel;

	int			size, width, font_size, filter;
	int			verbose, iret;

	UpaProfileObjectType	*uobj;
	MetObjectType		*gmobj;


	uobj = (UpaProfileObjectType *) mot->metObjectContent;

	assert( uobj );

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "DisplayUpaProfile\n" );

/*
 *	Save original FDF settings.
 */
	strcpy ( sizestr, uobj->symbol_size );
	strcpy ( widthstr, uobj->symbol_width );
	strcpy ( fontstr, uobj->text );
	strcpy ( filterstr, uobj->filter );

/*
 *	Set plotting parameters according to generalized values.
 */
	GetDepictableValue ( "SYMBOLSIZE", &size );
	GetDepictableValue ( "SYMBOLWIDTH", &width );
	GetDepictableValue ( "FONTSIZE", &font_size );
	GetDepictableValue ( "SYMBOLPACKING", &filter );

	SetGemText ( UPPERAIRPROFILEOBJECT, fontstr, "FONTSIZE", font_size );
	SetGemFloatVar( UPPERAIRPROFILEOBJECT, sizestr, "SYMBOLSIZE", size );
	SetGemFloatVar( UPPERAIRPROFILEOBJECT, widthstr, "SYMBOLWIDTH", width );
	SetGemFloatVar( UPPERAIRPROFILEOBJECT,filterstr,"SYMBOLPACKING",filter);
	strcpy ( title, mot->titleString );
	SetGemTitle ( uobj->colors, mot->titleIndex, title );

	sprintf ( wind, "%s%s/%s/%s",
			uobj->wind_symbol, uobj->colors,
			sizestr         , widthstr );

/*
 *	Hardwire for now.
 */
	border= strdup ("1");
	thtaln= strdup ("12/3/1");
	thteln= strdup ("15/1/1");
	mixrln= strdup ("5/10/2");
	panel= strdup ("0");
	strcpy ( parms, "tmpc;dwpt");
	strcpy ( uobj->line_width, "3" );

/*
 *	Check for an overlay.
 */
	if ( strcmp ( uobj->winpos, "1" ) != 0 )
	    sprintf ( line, "%s/%s/%s",
			 uobj->colors, uobj->line_type, uobj->line_width );
	else
	    sprintf ( line, "2;3/%s/%s",
		         uobj->line_type, uobj->line_width );


	psnprof(mot->filename,		parms,
	        mot->area,		line,
		uobj->ptype,		uobj->stndex,
		uobj->stncol,		wind,
		uobj->winpos,		uobj->marker,
		border,			title,
		thtaln,			thteln,
		mixrln,			uobj->yaxis,
		uobj->xaxis,		uobj->vcoord,
		uobj->dattim,		panel,
		fontstr,		filterstr,
		&(mot->titleIndex),
		&verbose,		&iret, 
		strlen(mot->filename),	strlen(parms),
		strlen(mot->area),	strlen(line),
		strlen(uobj->ptype),	strlen(uobj->stndex),
		strlen(uobj->stncol),	strlen(wind),
		strlen(uobj->winpos),	strlen(uobj->marker),
		strlen(border),
		strlen(title),		strlen(thtaln),
		strlen(thteln),		strlen(mixrln),
		strlen(uobj->yaxis),	strlen(uobj->xaxis),
		strlen(uobj->vcoord),	strlen(uobj->dattim),
		strlen(panel),
		strlen(fontstr),	strlen(filterstr));

	Free ( border );
	Free ( thtaln );
	Free ( thteln );
	Free ( mixrln );
	Free ( panel );


	SetMetObjectDrawnFlag( mot, True );

	return(0);

}
