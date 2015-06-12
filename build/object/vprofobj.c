/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	vprofobj.c
 *
 *	Vertical profile object creation and destruction functions.	
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/97	COMET	Use title string in mot for consistency across
 *			data types.
 *	 3/97	COMET	Added wind position indicator.
 *	 8/97	COMET	Added reference, forecast and valid times.
 *	 9/97	COMET	Added model_name to object (for autoupdate)
 *	11/97	COMET	Added call to MakeMotTitle. Added model_label,
 *			field_label and titleIndex.
 *
 ***********************************************************************/


#include "guimacros.h"
#include "winobj.h"
#include "genglobs.h"
#include "_proto.h"

char * MakeForecastDattim ( char * );



void DestroyVerticalProfile( MetObjectType *m )
{
	if( m && m->metObjectContent )
		free( m->metObjectContent );

	return;

}


MetObjectType *MakeVerticalProfileObject (
				 char *file,        char *ftime,
			         char *vcoord,      char *field,
				 char *point,       char *vector,
				 char *ptype,       char *xaxis, 
				 char *yaxis,       char *refvec,
				 char *thtaln,	    char *thteln,
				 char *mixrln,	    char *filter,
                                 char *winpos,
				 char *wind_symbol, 
				 char *symbol_size, char *symbol_width, 
				 char *symbol_type, char *symbol_headsize,
				 char *line_color,  char *line_type, 
				 char *line_width,
				 char *scale,       char *text,
				 char *model_name,  char *model_label,
				 char *field_label, int titleIndex )

{
	
	MetObjectType		*m;
	VertProfileObjectType	*vot;

	char			area[81], garea[81], proj[81];
	char			*dattim;
	int			i, verbose;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "MakeVerticalProfileObject\n" );

	m = AllocateMot();
	if( !m ) {
		printf("*** MakeVerticalProfileObject: Mot allocation error.\n");
		return( ( MetObjectType * ) NULL );
	}

	vot = (VertProfileObjectType *) malloc( sizeof
					( VertProfileObjectType ) );
	if( ! vot ) {
		printf("*** MakeVerticalProfileObject: malloc error.\n");
		return( ( MetObjectType * ) NULL );
	}

/*
 *	Get projection information.
 */
        strcpy ( proj, "XSEC" );
        strcpy ( garea, "" );
        strcpy ( area, "" );

/*
 *	Set valid date/time for title.
 */
	dattim = MakeForecastDattim ( ftime );

/*
 *	Build title.
 */
	MakeMotTitle ( m->titleString, dattim, model_label, NULL,
                              NULL, field_label, point );

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
	m->type             = VERTICALPROFILEOBJECT;
	m->displayFunc      = DisplayVerticalProfile;
	m->destroyFunc      = DestroyVerticalProfile;

/*
 *	Fill in the specific details of the Vertical Profile Object.
 */

	strcpy ( vot->ftime,  ftime );
	strcpy ( vot->vcoord, vcoord );
	strcpy ( vot->field,  field );
	strcpy ( vot->point,  point );
	strcpy ( vot->vector, vector );
	strcpy ( vot->ptype,  ptype );
	strcpy ( vot->xaxis,  xaxis );
	strcpy ( vot->yaxis,  yaxis );
	strcpy ( vot->refvec, refvec );
	strcpy ( vot->thtaln, thtaln );
	strcpy ( vot->thteln, thteln );
	strcpy ( vot->mixrln, mixrln );
	strcpy ( vot->filter, filter );
	strcpy ( vot->winpos,  winpos );
	strcpy ( vot->wind_symbol,  wind_symbol );
	strcpy ( vot->symbol_size,  symbol_size );
	strcpy ( vot->symbol_width, symbol_width );
	strcpy ( vot->symbol_type, symbol_type );
	strcpy ( vot->symbol_headsize, symbol_headsize );
	strcpy ( vot->line_color,   line_color );
	strcpy ( vot->line_type,    line_type );
	strcpy ( vot->line_width,   line_width );
	strcpy ( vot->scale, scale );
	strcpy ( vot->text,  text );
	strcpy ( vot->model_name, model_name );
	strcpy ( vot->model_label, model_label );
	strcpy ( vot->field_label, field_label );

/*
 *	Tie the MetObjectContentType data into the metObject.
 */
	m->metObjectContent = (MetObjectContentType *) vot;
/*
 *	Free.
 */
	Free ( dattim );

	return( m );

}
