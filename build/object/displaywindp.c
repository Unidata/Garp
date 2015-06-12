/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	displaywindp.c
 *
 *	Wind profiler display driver.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	11/97	COMET	Added titleIndex to pwprof arg list.
 *
 ***********************************************************************/


#include "underscore.h"
#include "winobj.h"
#include "wincb.h"
#include "genglobs.h"


int DisplayWindProfiler( WindowObjectType *wo, MetObjectType *mot )
{
	char			title[FILENAMESTRING], wind[GEMPAKSTRING];
	char			tmp_datim[FILENAMESTRING];
	char			sizestr[GEMPAKSTRING], widthstr[GEMPAKSTRING];
	char			line[GEMPAKSTRING], fontstr[GEMPAKSTRING];
	int			size, width, font_size;
	int			i, verbose, iret;

	WindProfilerObjectType	*wobj;
	MetObjectType		*gmobj;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "DisplayWindProfiler\n" );

	wobj = (WindProfilerObjectType *) mot->metObjectContent;

	assert( wobj );

/*
 *	Save original FDF settings.
 */
	strcpy ( sizestr, wobj->symbol_size );
	strcpy ( widthstr, wobj->symbol_width );
	strcpy ( fontstr, wobj->text );

/*
 *	Set plotting parameters according to generalized values.
 */
	GetDepictableValue ( "SYMBOLSIZE", &size );
	GetDepictableValue ( "SYMBOLWIDTH", &width );
	GetDepictableValue ( "FONTSIZE", &font_size );

	SetGemFloatVar (WINDPROFILEROBJECT, sizestr, "SYMBOLSIZE", size );
	SetGemFloatVar (WINDPROFILEROBJECT, widthstr, "SYMBOLWIDTH", width );
	SetGemText (WINDPROFILEROBJECT, fontstr, "FONTSIZE", font_size );

	sprintf ( wind, "%s%s/%s/%s",
                        wobj->wind_symbol, "1",
			sizestr         , widthstr );

	strcpy ( line, "1/-1/" );
	strcpy ( title, mot->titleString );
	SetGemTitle ( line, mot->titleIndex, title );

/*
 *	Gempak driver.
 */

        tmp_datim[0] = '\0';
        strncat(tmp_datim,wobj->dattim+2,strcspn(wobj->dattim+2,"-")+1);
        strcat(tmp_datim,strchr(wobj->dattim,'-')+3);
	for ( i=0; i<wobj->nfiles; i++ ) 
	    pwprof(   wobj->cxstn,		wobj->snfiles[i],
		      tmp_datim,		wobj->yaxis,
		      wobj->taxis,		wind,
		      title,			wobj->fint,
		      fontstr,			&(mot->titleIndex),
		      &verbose,			&iret,
		      strlen(wobj->cxstn),	strlen(wobj->snfiles[i]),
		      strlen(tmp_datim),	strlen(wobj->yaxis),
		      strlen(wobj->taxis),	strlen(wind),
		      strlen(title),		strlen(wobj->fint),
		      strlen(fontstr) );

	SetMetObjectDrawnFlag( mot, True );

	return(0);

}
