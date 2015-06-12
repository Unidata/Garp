/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	upaprof.c
 *
 *	Radiosonde object creation and destruction functions for vertical
 *	projection.
 *
 *	History:
 *
 *	 1/97	COMET	Original copy
 *	 3/97	COMET	Added wind position indicator.
 *	 8/97	COMET	Added reference, forecast and valid times.
 *	 9/97	COMET	Added tfilter.
 *	11/97	COMET	Added call to MakeMotTitle.
 *
 ***********************************************************************/


#include "guimacros.h"
#include "winobj.h"


void DestroyUpaProfileObject( MetObjectType *m )
{
	if( m && m->metObjectContent )
		free( m->metObjectContent );

	return;

}

MetObjectType *MakeUpaProfileObject( 
		char *snfile,		char *area,
		char *dattim,		char *stndex,
		char *stncol,		char *vcoord,
		char *colors,		char *wind_symbol,
		char *symbol_size,	char *symbol_width,
		char *line_type,	char *winpos,
		char *ptype,		char *xaxis,
		char *yaxis,
		char *marker,		char *label,
		char *filter,		char *text,
		int  titleIndex,        BooleanType tfilter )
{
	
	MetObjectType		*m;
	UpaProfileObjectType	*uobj;

	m = AllocateMot();
	if( !m ) {
		printf("MakeUpaProfileObject: Mot allocation error.\n");
		return( ( MetObjectType * ) NULL );
	}

	uobj = (UpaProfileObjectType *)
			 malloc( sizeof( UpaProfileObjectType ) );
	if( ! uobj ) {
		printf("MakeUpaProfileObject: malloc error.\n");
		return( ( MetObjectType * ) NULL );
	}
/*
 *
 */
	MakeMotTitle ( m->titleString, dattim, NULL, NULL, NULL,
	                label, area );

/*
 *	Fill in the common metObject data.
 */
	strcpy( m->filename, snfile );
	sprintf( m->area, "%s%s", "@", area );
	strcpy( m->timestamp, dattim );

	m->refTime          = String2LongTime( dattim, &(m->fcstTime) );
	m->validTime        = m->refTime + m->fcstTime;
	m->drawnFlag        = False;
	m->inUse            = True;
	m->toggledOn        = True;
	m->titleIndex       = titleIndex;
	m->type             = UPPERAIRPROFILEOBJECT;
	m->displayFunc      = DisplayUpaProfile;
	m->destroyFunc      = DestroyUpaProfileObject;

/*
 *	Fill in the specific details of the Object.
 */
	strcpy ( uobj->dattim, dattim );
	strcpy ( uobj->stndex, stndex );
	strcpy ( uobj->stncol, stncol );
	strcpy ( uobj->vcoord, vcoord );
	strcpy ( uobj->colors, colors );
	strcpy ( uobj->wind_symbol, wind_symbol );
	strcpy ( uobj->symbol_size, symbol_size );
	strcpy ( uobj->symbol_width, symbol_width );
	strcpy ( uobj->line_type, line_type );
	strcpy ( uobj->winpos, winpos );
	strcpy ( uobj->ptype, ptype );
	strcpy ( uobj->xaxis, xaxis );
	strcpy ( uobj->yaxis, yaxis );
	strcpy ( uobj->marker, marker );
	strcpy ( uobj->filter, filter );
	strcpy ( uobj->text,   text );
	uobj->tfilter = tfilter;

/*
 *	Tie the MetObjectContentType data into the metObject.
 */
	m->metObjectContent = (MetObjectContentType *) uobj;

	return( m );

}
