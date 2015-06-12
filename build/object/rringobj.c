/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	rringobj.c
 *
 *	Radar ring creation and destruction functions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 8/97	COMET	Get preferred projection from Garp_defaults.
 *	11/97	COMET	Added toggledOn flag.
 *
 ***********************************************************************/


#include "guimacros.h"
#include "genglobs.h"
#include "winobj.h"
#include "utils.h"
#include "ctbcmn.h"
#include "_proto.h"


void DestroyRadarRingObject( MetObjectType *m )
{
	if( m && m->metObjectContent )
		free( m->metObjectContent );

	return;

}

MetObjectType *MakeRadarRingObject( 
		int	tblindex,
		int	number,
		float	increment, 
		float	minimum,
		char	*units,
		char	*station_id,
		int	color,
		int	*iret )
{
	
	MetObjectType		*m;
	RadarRingObjectType	*mobj;

	extern struct datatype_list	mastertbl[MAXTYP];
	extern struct station_list	station_info[LLSTFL];

	char	stid[STIDSIZE];
	char	*proj, *area, *garea;

	int	stidlen=STIDSIZE+1;
	int	nstn, jcolr;
	int	i;

	
	m = AllocateMot();
	if( !m ) {
		printf("*** MakeRadarRingObject: Mot allocation error.\n");
		return( ( MetObjectType * ) NULL );
	}

	mobj = (RadarRingObjectType *) malloc( sizeof( RadarRingObjectType ) );
	if( ! mobj ) {
	    printf("*** MakeRadarRingObject: malloc error.\n");
	    return( ( MetObjectType * ) NULL );
	}
/*
 *	Read and store NEXRAD radar sites.
 */
	get_table ( mastertbl[tblindex], &station_info, &nstn, iret );

/*
 *	Fill in the radaRadarRingObject data.
 */
	strcpy ( mobj->stations, "" );

	for ( i=0; i<nstn; i++ ) {
	    strcpy ( stid, station_info[i].stid );
	    bpad ( stid, stidlen, iret );
	    strcat ( mobj->stations, stid );
	    mobj->lat[i]  = station_info[i].rlat;
	    mobj->lon[i]  = station_info[i].rlon;
	}

	mobj->nstn          = nstn;
	mobj->number        = number;
	mobj->increment     = increment;
	mobj->minimum       = minimum;
	mobj->color         = color;
	strcpy ( mobj->stid, station_id );
	strcpy ( mobj->units, units );
	ToUpper ( mobj->stations );
	ToUpper ( mobj->stid );

	m->type             = RADARRINGOBJECT;
	m->displayFunc      = DisplayRadarRing;
	m->destroyFunc      = DestroyRadarRingObject;
	m->toggledOn        = True;

/*
 *	Provide some geographic defualt
 */
	proj = GetConfigValue ( "default_proj" );
	if ( proj == NULL ) proj = strdup ( "lcc/30.0;-100;50.0//nm" );

	garea = GetConfigValue ( "default_garea" );
	if ( garea == NULL ) garea = strdup ( "23.0;-120.0;47.0;-65.0" );

	area  = strdup ( garea );

/*
 *	Tie the MetObjectContentType data into the metObject.
 */
	m->metObjectContent = (MetObjectContentType *) mobj;

/*
 *	Free.
 */
 	Free ( proj );
	Free ( area );
	Free ( garea );

	return( m );

}
