/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	surfobj.c
 *
 *	Surface object creation and destruction functions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/97	COMET	Use title string in mot for consistency across
 *			data types.
 *	 8/97	COMET	Added reference, forecast and valid times.
 *	 8/97	COMET	Added pmarker, marker.
 *	 8/97	COMET	Get preferred projection from Garp_defaults.
 *	 9/97	COMET	Added type and label to surface object
 *	10/97	COMET	Added nstations.
 *	11/97	COMET	Added call to MakeMotTitle.
 *
 ***********************************************************************/


#include "guimacros.h"
#include "winobj.h"
#include "_proto.h"


char * GetConfigValue ( char * );



void DestroySurfaceObject( MetObjectType *m )
{
	if( m && m->metObjectContent )
		free( m->metObjectContent );

	return;

}

MetObjectType *MakeSurfaceObject( 
		char	*sfparm,
		char	*sffile,
		char	*dattim,
		char	*colors,
		char	*label,
		char	*filter,
		char	*text,
		char	*type,
		int	marker,
		int	pmarker,
		int	skpmis,
		int	titleIndex )
{
	
	MetObjectType		*m;
	SurfaceObjectType	*sobj;
	
	char			*proj, *area, *garea;

	m = AllocateMot();
	if( !m ) {
		printf("MakeSurfaceObject: Mot allocation error.\n");
		return( ( MetObjectType * ) NULL );
	}

	sobj = (SurfaceObjectType *) malloc( sizeof( SurfaceObjectType ) );
	if( ! sobj ) {
		printf("MakeSurfaceObject: malloc error.\n");
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
	MakeMotTitle ( m->titleString, dattim, NULL, NULL, NULL,
	               label, NULL );

/*
 *	Fill in the common metObject data.
 */
	strcpy( m->filename,    sffile );
	strcpy( m->area,        area );
	strcpy( m->garea,       garea );
	strcpy( m->proj,        proj );
	strcpy( m->timestamp,   dattim );

	m->refTime          = String2LongTime( dattim, &(m->fcstTime) );
	m->validTime        = m->refTime + m->fcstTime;
	m->drawnFlag        = False;
	m->inUse            = True;
	m->toggledOn        = True;
	m->titleIndex       = titleIndex;
	m->type             = SURFACEOBJECT;
	m->displayFunc      = DisplaySurface;
	m->destroyFunc      = DestroySurfaceObject;

/*
 *	Fill in the specific details of the Surface Object.
 */
	strcpy ( sobj->sfparm, sfparm );
	strcpy ( sobj->dattim, dattim );
	strcpy ( sobj->colors, colors );
	strcpy ( sobj->filter, filter );
	strcpy ( sobj->text,   text );
	strcpy ( sobj->type,   type );
	strcpy ( sobj->label,  label );
	strcpy ( sobj->symbol_size, "1.0" );
	strcpy ( sobj->symbol_width, "1" );
	sobj->marker = marker;
	sobj->pmarker = pmarker;
	sobj->skpmis = skpmis;
	sobj->nstations = 0;


/*
 *	Tie the MetObjectContentType data into the metObject.
 */
	m->metObjectContent = (MetObjectContentType *) sobj;
/*
 *	Free.
 */
	Free ( proj );
	Free ( area );
	Free ( garea );

	return( m );

}

int DattimToTimestamp( char *dattim )
{

	return ( FileToTimestamp ( SURFACEOBJECT, dattim ) );

}
