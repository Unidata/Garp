/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	displayprof.c
 *
 *	Vertical profile display driver.
 *
 *	History:
 *
 *	12/96	COMET	Original copy
 *	 1/97	COMET	Use title string in mot for consistency across
 *			data types.
 *	 3/97	COMET	Added wind position indicator.
 *	 3/97	COMET	Added frame number
 *	11/97	COMET	Added titleIndex to pvprof arg list.
 *
 ***********************************************************************/


#include "underscore.h"
#include "winobj.h"
#include "wincb.h"
#include "genglobs.h"


int DisplayVerticalProfile( WindowObjectType *wo, MetObjectType *mot )
{
	char			title[FILENAMESTRING], wind[GEMPAKSTRING];
        char			tmp_datim[GEMPAKSTRING];
	char			line[GEMPAKSTRING];
	char			file[GEMPAKSTRING];
	char			sizestr[GEMPAKSTRING], widthstr[GEMPAKSTRING];
	char			fontstr[GEMPAKSTRING];
	int			size, width, font_size;
	int			i, verbose, frame, iret;
	char			gfunc[GEMPAKSTRING], gvect[GEMPAKSTRING];

	VertProfileObjectType	*pot;
	MetObjectType		*gmobj;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "DisplayVerticalProfile\n" );

	frame = GetActivePixmapObjectIndex (wo) + 1;

	pot = (VertProfileObjectType *) mot->metObjectContent;

	assert( pot );

/*
 *	Save original FDF settings.
 */
	strcpy ( sizestr, pot->symbol_size );
	strcpy ( widthstr, pot->symbol_width );
	strcpy ( fontstr, pot->text );

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
	sprintf ( line, "%s/%s/%s", pot->line_color, pot->line_type,
				    pot->line_width );

	sprintf ( wind, "%s%s/%s/%s/%s/%s",
                        pot->wind_symbol, pot->line_color,
			sizestr         , widthstr       ,
                        pot->symbol_type, pot->symbol_headsize );

	strcpy ( title, mot->titleString );
	SetGemTitle ( line, mot->titleIndex, title );

        if(strcspn(pot->ftime,"/") > 6)
           strcpy(tmp_datim,pot->ftime+2);
        else
           strcpy(tmp_datim,pot->ftime);

/*
 *	Gempak driver.
 */
	if ( strstr ( pot->ptype, "skewt" ) ||
	     strstr ( pot->ptype, "stuve" ) ) {

/*
 *	Use red for the TMPC line color for the first plot.
 */
	if ( strcmp ( pot->winpos, "1" ) == 0 )
	    sprintf ( line, "2/%s/3", pot->line_type );
	else
	    sprintf ( line, "%s/%s/3", pot->line_color, pot->line_type );

	sprintf(gfunc,"tmpc"); sprintf(gvect,"wind");
	pvprof(	mot->filename,		tmp_datim,
		pot->vcoord,		gfunc,
		pot->point,		gvect,
		pot->ptype,		pot->xaxis,
		pot->yaxis,		line,
                wind,			pot->refvec,
		pot->winpos,		pot->thtaln,
		pot->thteln,		pot->mixrln,
		pot->filter,		title,
		pot->scale,		fontstr,
		&frame,			&(mot->titleIndex),
		&verbose,		&iret,
		strlen(mot->filename),	strlen(tmp_datim),
		strlen(pot->vcoord),	strlen(gfunc),
		strlen(pot->point),	strlen(gvect),
		strlen(pot->ptype),	strlen(pot->xaxis),
		strlen(pot->yaxis),	strlen(line),
		strlen(wind),		strlen(pot->refvec),
		strlen(pot->winpos),	strlen(pot->thtaln),
		strlen(pot->thteln),	strlen(pot->mixrln),
		strlen(pot->filter),	strlen(title),
		strlen(pot->scale),	strlen(fontstr) );

/*
 *	Use green for the DWPC line color for the first plot.
 */
	if ( strcmp ( pot->winpos, "1" ) == 0 )
	    sprintf ( line, "3/%s/3", pot->line_type );
	else
	    sprintf ( line, "%s/%s/3", pot->line_color, pot->line_type );

	sprintf(gfunc,"dwpc"); sprintf(gvect," ");
	pvprof(	mot->filename,		tmp_datim,
		pot->vcoord,		gfunc,
		pot->point,		gvect,
		pot->ptype,		pot->xaxis,
		pot->yaxis,		line,
                wind,			pot->refvec,
		pot->winpos,		pot->thtaln,
		pot->thteln,		pot->mixrln,
		pot->filter,		title,
		pot->scale,		fontstr,
		&frame,			&(mot->titleIndex),
		&verbose,		&iret,
		strlen(mot->filename),	strlen(tmp_datim),
		strlen(pot->vcoord),	strlen(gfunc),
		strlen(pot->point),	strlen(gvect),
		strlen(pot->ptype),	strlen(pot->xaxis),
		strlen(pot->yaxis),	strlen(line),
		strlen(wind),		strlen(pot->refvec),
		strlen(pot->winpos),	strlen(pot->thtaln),
		strlen(pot->thteln),	strlen(pot->mixrln),
		strlen(pot->filter),	strlen(title),
		strlen(pot->scale),	strlen(fontstr) );
	}
	else 

	pvprof(	mot->filename,		tmp_datim,
		pot->vcoord,		pot->field,
		pot->point,		pot->vector,
		pot->ptype,		pot->xaxis,
		pot->yaxis,		line,
               	wind,			pot->refvec,
		pot->winpos,		pot->thtaln,
		pot->thteln,		pot->mixrln,
		pot->filter,		title,
		pot->scale,		fontstr,
		&frame,			&(mot->titleIndex),
		&verbose,		&iret,
		strlen(mot->filename),	strlen(tmp_datim),
		strlen(pot->vcoord),	strlen(pot->field),
		strlen(pot->point),	strlen(pot->vector),
		strlen(pot->ptype),	strlen(pot->xaxis),
		strlen(pot->yaxis),	strlen(line),
		strlen(wind),		strlen(pot->refvec),
		strlen(pot->winpos),	strlen(pot->thtaln),
		strlen(pot->thteln),	strlen(pot->mixrln),
		strlen(pot->filter),	strlen(title),
		strlen(pot->scale),	strlen(fontstr) );

	SetMetObjectDrawnFlag( mot, True );

	return(0);

}
