/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	thgtobj.c
 *
 *	Time/height cross section object creation and destruction functions.	
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 8/97	COMET	Added reference, forecast and valid times.
 *	 9/97	COMET	Added model_name to object (for autoupdate).
 *	10/97	COMET	Set time in metObject to real time, time in object
 *			content to "all". (So autoupdate has better time
 *			reference).
 *	11/97	COMET	Added call to MakeMotTitle. Added model_label,
 *			field_label and titleIndex.
 *
 ***********************************************************************/


#include "guimacros.h"
#include "winobj.h"
#include "genglobs.h"


void DestroyTimeHeightObject( MetObjectType *m )
{
	if( m && m->metObjectContent )
		free( m->metObjectContent );

	return;

}


MetObjectType *MakeTimeHeightObject ( char *file,        char *ftime,
			              char *vcoord,      char *field,
				      char *point,       char *vector,
				      char *ptype,       char *taxis, 
				      char *yaxis,       char *wind_symbol, 
				      char *symbol_size, char *symbol_width, 
				      char *symbol_type, char *symbol_headsize,
				      char *line_color,  char *line_type, 
				      char *line_width,  char *label_freq, 
				      char *cint,        char *cmin, 
				      char *cmax,        char *scale, 
				      char *text,	 char *model_name,
				      char *model_label, char *field_label,
				      int titleIndex )

{
	
	MetObjectType		*m;
	TimeHeightObjectType	*tot;

	char	*label="<-- Time GMT (DD/HH)    Time/height Cross Section";
	char			area[81], garea[81], proj[81];
	int			i, verbose;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "MakeTimeHeightObject\n" );

	m = AllocateMot();
	if( !m ) {
		printf("*** MakeTimeHeightObject: Mot allocation error.\n");
		return( ( MetObjectType * ) NULL );
	}

	tot = (TimeHeightObjectType *) malloc( sizeof
					( TimeHeightObjectType ) );
	if( ! tot ) {
		printf("*** TimeHeightObjectType: malloc error.\n");
		return( ( MetObjectType * ) NULL );
	}

/*
 *	Get projection information.
 */
        strcpy ( proj, "XSEC" );
        strcpy ( garea, "" );
        strcpy ( area, "" );

/*
 *	Build title.
 */
	MakeMotTitle ( m->titleString, NULL, model_label, NULL, NULL,
                       field_label, point );

/*
 *	Fill in the common metObject data.
 */
	strcpy( m->filename, file );
	strcpy( m->area,     area );
	strcpy( m->garea,    garea );
	strcpy( m->proj,     proj );
	strcpy( m->timestamp, ftime );

	m->refTime          = String2LongTime( ftime, &(m->fcstTime) );
	m->validTime        = m->refTime + m->fcstTime;
	m->drawnFlag        = False;
	m->inUse            = True;
	m->toggledOn        = True;
	m->titleIndex       = titleIndex;
	m->type             = TIMEHEIGHTOBJECT;
	m->displayFunc      = DisplayTimeHeight;
	m->destroyFunc      = DestroyTimeHeightObject;

/*
 *	Fill in the specific details of the Time/height Cross Section Object.
 */

	strcpy ( tot->ftime,  "FIRST-LAST" );
	strcpy ( tot->vcoord, vcoord );
	strcpy ( tot->field,  field );
	strcpy ( tot->point,  point );
	strcpy ( tot->vector, vector );
	strcpy ( tot->ptype,  ptype );
	strcpy ( tot->taxis,  taxis );
	strcpy ( tot->yaxis,  yaxis );
	strcpy ( tot->wind_symbol,  wind_symbol );
	strcpy ( tot->symbol_size,  symbol_size );
	strcpy ( tot->symbol_width, symbol_width );
	strcpy ( tot->symbol_type, symbol_type );
	strcpy ( tot->symbol_headsize, symbol_headsize );
	strcpy ( tot->line_color,   line_color );
	strcpy ( tot->line_type,    line_type );
	strcpy ( tot->line_width,   line_width );
	strcpy ( tot->label_freq,   label_freq );
	strcpy ( tot->cint,  cint );
	strcpy ( tot->cmin,  cmin );
	strcpy ( tot->cmax,  cmax );
	strcpy ( tot->scale, scale );
	strcpy ( tot->text,  text );
	strcpy ( tot->model_name, model_name );
	strcpy ( tot->model_label, model_label );
	strcpy ( tot->field_label, field_label );

/*
 *	Tie the MetObjectContentType data into the metObject.
 */
	m->metObjectContent = (MetObjectContentType *) tot;

	return( m );

}
