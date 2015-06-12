/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	displaythgt.c
 *
 *	Time/height cross section display driver.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	11/97	COMET	Added titleIndex to pthgt arg list.
 *
 ***********************************************************************/


#include "underscore.h"
#include "winobj.h"
#include "wincb.h"
#include "genglobs.h"


int DisplayTimeHeight( WindowObjectType *wo, MetObjectType *mot )
{
	char			title[FILENAMESTRING], wind[GEMPAKSTRING];
	char			cint[GEMPAKSTRING], line[GEMPAKSTRING];
	char			file[GEMPAKSTRING];
	char			sizestr[GEMPAKSTRING], widthstr[GEMPAKSTRING];
	char			fontstr[GEMPAKSTRING];
	int			size, width, font_size;
	int			i, verbose, iret;

	TimeHeightObjectType	*tot;
	MetObjectType		*gmobj;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "DisplayTimeHeight\n" );

	tot = (TimeHeightObjectType *) mot->metObjectContent;

	assert( tot );

/*
 *	Save original FDF settings.
 */
	strcpy ( sizestr, tot->symbol_size );
	strcpy ( widthstr, tot->symbol_width );
	strcpy ( fontstr, tot->text );

/*
 *	Set plotting parameters according to generalized values.
 */
	GetDepictableValue ( "SYMBOLSIZE", &size );
	GetDepictableValue ( "SYMBOLWIDTH", &width );
	GetDepictableValue ( "FONTSIZE", &font_size );

	SetGemFloatVar (TIMEHEIGHTOBJECT, sizestr, "SYMBOLSIZE", size );
	SetGemFloatVar (TIMEHEIGHTOBJECT, widthstr, "SYMBOLWIDTH", width );
	SetGemText (TIMEHEIGHTOBJECT, fontstr, "FONTSIZE", font_size );

/*
 *	Set contour line characteristics.
 */
	sprintf ( cint, "%s/%s/%s", tot->cint, tot->cmin, tot->cmax );

	sprintf ( line, "%s/%s/%s/%s", tot->line_color, tot->line_type,
				       tot->line_width, tot->label_freq );

	sprintf ( wind, "%s%s/%s/%s/%s/%s",
                        tot->wind_symbol, tot->line_color,
			sizestr         , widthstr       ,
                        tot->symbol_type, tot->symbol_headsize );

	strcpy (title, mot->titleString );
	SetGemTitle ( line, mot->titleIndex, title );

/*
 *	Gempak driver.
 */
	pthgt(	mot->filename,		tot->ftime,
		tot->vcoord,		tot->field,
		tot->point,		tot->vector,
		tot->ptype,		tot->taxis,
		tot->yaxis,		line,
                cint,			wind,
		title,			tot->scale,
		fontstr,		&(mot->titleIndex),
		&verbose,		&iret,
		strlen(mot->filename),	strlen(tot->ftime),
		strlen(tot->vcoord),	strlen(tot->field),
		strlen(tot->point),	strlen(tot->vector),
		strlen(tot->ptype),	strlen(tot->taxis),
		strlen(tot->yaxis),	strlen(line),
		strlen(cint),		strlen(wind),
		strlen(title),		strlen(tot->scale),
		strlen(fontstr) );


	SetMetObjectDrawnFlag( mot, True );

	return(0);

}
