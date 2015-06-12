/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	latlonobj.c
 *
 *	Map object creation and destruction functions.
 *
 *	History:
 *
 *	10/97	COMET	Original copy
 *	11/97	COMET	Added toggledOn flag to metObject.
 *
 ***********************************************************************/


#include "genglobs.h"
#include "winobj.h"
#include "_proto.h"




/*
 *	Called automatically when the MetObject is destroyed.
 */
void DestroyLatLonGridObject( MetObjectType *m )
{
	if( m && m->metObjectContent )
		free( (char *) m->metObjectContent );
	return;

}

MetObjectType *MakeLatLonGridObject( 
		char	*latinc,
		char	*loninc,
		char	*lblinc,
		char	*line_color,
		char	*line_type,
		char	*line_width )
{
	
	MetObjectType		*m;
	LatLonGridObjectType	*lobj;
	char			*proj, *area, *garea;

	if( GetVerboseLevel() > VERBOSE_0 )
        	printf("MakeLatLonGridObject\n");

	
	m = AllocateMot();
	if( !m ) {
		printf("*** MakeLatLonGridObject: Mot allocation error.\n");
		return( ( MetObjectType * ) NULL );
	}

	lobj = (LatLonGridObjectType *) malloc( sizeof( LatLonGridObjectType ) );
	if( ! lobj ) {
		printf("*** MakeLatLonGridObject: malloc error.\n");
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
 *	Fill in the common metObject data.
 */
	strcpy( m->filename,    " ");
	strcpy( m->area,        area );
	strcpy( m->garea,       garea );
	strcpy( m->proj,        proj );
	strcpy( m->timestamp,   "0" );
	strcpy( m->titleString, "Lat/Lon Grid" );

	m->refTime          = 0;
	m->validTime        = 0;
	m->fcstTime         = 0;
	m->drawnFlag        = False;
	m->inUse            = True;
	m->toggledOn        = True;
	m->type             = LATLONGRIDOBJECT;
	m->displayFunc      = DisplayBackgroundObject;
	m->destroyFunc      = DestroyLatLonGridObject;


/*
 *	Fill in the specific details of the Latlon Grid Object.
 */
	strcpy(lobj->line_color, line_color);
	strcpy(lobj->line_type,  line_type);
	strcpy(lobj->line_width, line_width);
	strcpy(lobj->lat_inc,    latinc);
	strcpy(lobj->lon_inc,    loninc);
	strcpy(lobj->label_freq, lblinc);

/*
 *	Tie the MetObjectContentType data into the metObject.
 */
	m->metObjectContent = (MetObjectContentType *) lobj;

/*
 *	Free.
 */
	Free ( proj );
	Free ( area );
	Free ( garea );
	
	return( m );

}

