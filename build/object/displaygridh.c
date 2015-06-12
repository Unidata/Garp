/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	displaygridh.c
 *
 *	Plan view scalar object display driver.
 *
 *	History:
 *
 *	11/96	COMET		Original copy
 *	 3/97	J. Cowie/COMET	Changed to allow contour and fill info
 *				to be saved back into the metobject
 *	11/97	COMET		Added titleIndex to pmap and psgrid arg
 *				lists.
 *
 ***********************************************************************/


#include "underscore.h"
#include "winobj.h"
#include "wincb.h"
#include "genglobs.h"
#include "_proto.h"


void
PrintGemError ( char *message, char *parms )
{
	char		format[2560];
	char		**argv;
	int		argc = 0;

	detok ( parms, &argv, " " );

	sprintf ( format, "( %s ) ", message );
	while ( *argv ) {
	    strcat ( format, "\"" );
	    strcat ( format, *argv );
	    strcat ( format, "\", " );
	    *argv++; argc++;
	}
/*
 *	Strip of last comma.
 */
	strndelend ( format, 2 );
	printf ( format );

	FreeList ( argc, argv );

}

int DisplayScalarGrid( WindowObjectType *wo, MetObjectType *mot )
{
/*
 *	Prepare a horizontal scalar grid Met object for plotting with a 
 *	GEMPAK driver subroutine.
 */

/*	char			errmsg[256]; */
	char			**strarray;
	char			title[FILENAMESTRING], ctype[GEMPAKSTRING];
	char			cint[GEMPAKSTRING], fint[GEMPAKSTRING];
	char			line[GEMPAKSTRING];
	char			fontstr[GEMPAKSTRING], skipstr[GEMPAKSTRING];
	char			skip[GEMPAKSTRING];
	char			skip_x[GEMPAKSTRING], skip_y[GEMPAKSTRING];
	int			font_size, filter;
	int			i, verbose, frame, num, len, iret;

	ScalarGridObjectType	*sot;


	sot = (ScalarGridObjectType *) mot->metObjectContent;

	assert( sot );

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "DisplayScalarGrid\n" );

	frame = GetActivePixmapObjectIndex (wo) + 1;

/*
 *	Save original FDF settings.
 */
	strcpy ( fontstr, sot->text );
	strcpy ( skip, sot->skip_contour );
	strcpy ( skip_x, sot->skip_plot_x );
	strcpy ( skip_y, sot->skip_plot_y );

/*
 *	Set plotting parameters according to generalized values.
 */
	GetDepictableValue ( "FONTSIZE", &font_size );
	GetDepictableValue ( "SCALARPACKING", &filter );

	SetGemText ( SCALARGRIDOBJECT, fontstr, "FONTSIZE", font_size );
	SetGemIntVar ( SCALARGRIDOBJECT, skip, "SCALARPACKING", filter );
	SetGemIntVar ( SCALARGRIDOBJECT, skip_x, "SCALARPACKING", filter );
	SetGemIntVar ( SCALARGRIDOBJECT, skip_y, "SCALARPACKING", filter );

	sprintf ( skipstr, "%s/%s;%s", skip, skip_x, skip_y );
	sprintf ( cint, "%s/%s/%s", sot->cint, sot->cmin, sot->cmax );
	sprintf ( fint, "%s/%s/%s", sot->fint, sot->fmin, sot->fmax );

	bpad ( cint, GEMPAKSTRING, &iret );
	bpad ( fint, GEMPAKSTRING, &iret );

/*
 *	Check for plotting contour lines, fill or symbol.
 */
	strcpy ( ctype, "" );
	if ( sot->contour ) strcat ( ctype, "C" );
	if ( sot->fill )    strcat ( ctype, "/F" );
	if ( ! sot->symbol ) strcpy ( sot->hilo, "" );

/*
 *	Set contour line characteristics.
 */
	sprintf ( line, "%s/%s/%s/%s", sot->line_color, sot->line_type,
	                               sot->line_width, sot->label_freq );

/*
 *	Title.
 */
	strcpy (title, mot->titleString );
	SetGemTitle ( line, mot->titleIndex, title );

	if ( sot->value )

	pmap(	mot->filename,		sot->gdatim,
 	        sot->glevel,		sot->gvcord,
		sot->gfunc,		sot->gridtype,
		sot->line_color,
		cint,			sot->scale,
		title,			skipstr,
		fontstr,		&(sot->nfunction),
		&(mot->titleIndex),	&verbose,
		&iret, 
	        strlen(mot->filename),	strlen(sot->gdatim),
		strlen(sot->glevel),	strlen(sot->gvcord),
	        strlen(sot->gfunc),	strlen(sot->gridtype),
		strlen(sot->line_color),
		strlen(cint),		strlen(sot->scale),
		strlen(title),		strlen(skipstr),
		strlen(fontstr) );

	else

	psgrid(	mot->filename,		sot->gdatim,
 	        sot->glevel,		sot->gvcord,
		sot->gfunc,		sot->gridtype,
		cint,
	        line,			sot->scale,
		sot->hilo,		sot->hlsym,
		sot->clrbar,		title,
		sot->contur,		skipstr,
		fint,			sot->fline,
		ctype,			fontstr,
		&(sot->nfunction),	&frame,
		&(mot->titleIndex),
		&verbose,		&iret, 
	        strlen(mot->filename),	strlen(sot->gdatim),
		strlen(sot->glevel),	strlen(sot->gvcord),
	        strlen(sot->gfunc),	strlen(sot->gridtype),
		GEMPAKSTRING-1,
	        strlen(line),		strlen(sot->scale),
	        strlen(sot->hilo),	strlen(sot->hlsym),
	        strlen(sot->clrbar),	strlen(title),
	        strlen(sot->contur),	strlen(skipstr),
	        GEMPAKSTRING-1,		strlen(sot->fline),
		strlen(ctype),		strlen(fontstr) );

/*
 *	Save the contour and fill info back into the metobject
 */
	if ( sot->contour ) {
	    cint[GEMPAKSTRING-1] = '\0';
	    strbrm (cint);
	    strarray = SplitByDelimeter ( cint, "/", &num );
	    if ( strarray [0] != NULL ) strcpy ( sot->cint, strarray [0]);
	    if ( strarray [1] != NULL ) strcpy ( sot->cmin, strarray [1]);
	    if ( strarray [2] != NULL ) strcpy ( sot->cmax, strarray [2]);
	    StringListFree ( num, strarray );
	}

	if ( sot->fill ) {
	    fint[GEMPAKSTRING-1] = '\0';
	    strbrm (fint);
	    strarray = SplitByDelimeter ( fint, "/", &num );
	    if ( strarray [0] != NULL ) strcpy ( sot->fint, strarray [0]);
	    if ( strarray [1] != NULL ) strcpy ( sot->fmin, strarray [1]);
	    if ( strarray [2] != NULL ) strcpy ( sot->fmax, strarray [2]);
	    StringListFree ( num, strarray );
	}
/*
	if ( iret ) PrintGemError ( errmsg, errparm );
*/
	SetMetObjectDrawnFlag( mot, True );

	return(0);

}
