/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	upaobj.c
 *
 *	Radiosonde object creation and destruction functions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/97	COMET	Use title string in mot for consistency across
 *			data types.
 *	 8/97	COMET	Added reference, forecast and valid times.
 *	 8/97	COMET	Get preferred projection from Garp_defaults.
 *	 9/97	COMET	Added tfilter.
 *	10/97	COMET	Added nstations.
 *	11/97	COMET	Added call to MakeMotTitle.
 *
 ***********************************************************************/


#include "guimacros.h"
#include "winobj.h"
#include "_proto.h"


char * GetConfigValue ( char * );



void DestroyUpperairObject( MetObjectType *m )
{
	if( m && m->metObjectContent )
		free( m->metObjectContent );

	return;

}

MetObjectType *MakeUpperairObject( 
		char	*snfile,
		char	*snparm,
		char	*dattim,
		char	*levels,
		char	*vcoord,
		char	*colors,
		char	*marker,
		char	*label,
		char	*filter,
		char	*text,
		int	skpmis,
		int	titleIndex,
		BooleanType tfilter )
{
	
	MetObjectType		*m;
	UpperairObjectType	*uobj;
	char			*proj, *area, *garea;
	char			*vlev = "mb";

	m = AllocateMot();
	if( !m ) {
		printf("MakeUpperairObject: Mot allocation error.\n");
		return( ( MetObjectType * ) NULL );
	}

	uobj = (UpperairObjectType *) malloc( sizeof( UpperairObjectType ) );
	if( ! uobj ) {
		printf("MakeUpperairObject: malloc error.\n");
		return( ( MetObjectType * ) NULL );
	}

/*
 *	Get projection information.
 */
        proj = GetConfigValue ( "default_proj" );
	if ( proj == NULL ) proj = strdup ( "lcc/30.0;-100;50.0//nm" );

	garea = GetConfigValue ( "default_garea" );
	if ( garea == NULL ) garea = strdup ( "23.0;-120.0;47.0;-65.0" );

	area  = strdup ( garea );
/*
 *	Build title.
 */
	MakeMotTitle ( m->titleString, dattim, NULL, levels, vlev,
	               label, NULL );

/*
 *	Fill in the common metObject data.
 */
	strcpy( m->filename,  snfile );
	strcpy( m->area,      area );
	strcpy( m->garea,     garea );
	strcpy( m->proj,      proj );
	strcpy( m->timestamp, dattim );

	m->refTime          = String2LongTime( dattim, &(m->fcstTime) );
	m->validTime        = m->refTime + m->fcstTime;
	m->drawnFlag        = False;
	m->inUse            = True;
	m->toggledOn        = True;
	m->titleIndex       = titleIndex;
	m->type             = UPPERAIROBJECT;
	m->displayFunc      = DisplayUpperair;
	m->destroyFunc      = DestroyUpperairObject;

/*
 *	Fill in the specific details of the Surface Object.
 */
	strcpy ( uobj->snparm, snparm );
	strcpy ( uobj->dattim, dattim );
	strcpy ( uobj->levels, levels );
	strcpy ( uobj->vcoord, vcoord );
	strcpy ( uobj->colors, colors );
	strcpy ( uobj->marker, marker );
	strcpy ( uobj->filter, filter );
	strcpy ( uobj->text,   text );
	uobj->skpmis = skpmis;
	uobj->tfilter = tfilter;
	uobj->nstations = 0;


/*
 *	Tie the MetObjectContentType data into the metObject.
 */
	m->metObjectContent = (MetObjectContentType *) uobj;

/*
 *	Free.
 */
	Free ( proj );
	Free ( area );
	Free ( garea );

	return( m );

}
