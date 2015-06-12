/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	pixmapping.c
 *
 *	Georeference saving/setting routines for the pixmap.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	04/97	COMET	Remove error message
 *	 2/98	COMET	Added check for MCRADR
 *
 ***********************************************************************/
 
 
#include "underscore.h"
#include "winobj.h"


void savemapping ( PixmapObjectType *po )
{

	MapProjType	*map;
	SatProjType	*sat;
	ReferencePts	*ref;

	int		i, iret;
	float		x, y;
	float		lat, lon;
	char		Dcoords [] = "D";


	if ( ! po ) return;

/*
 *	Assume that this is a map projection.
 */	
	map = &(po->mapProjection);
	
	gqmprj ( po->proj, &(map->angle1), &(map->angle2),
		 &(map->angle3), &(map->latll), &(map->lonll),
		 &(map->latur), &(map->lonur),
		 &iret, sizeof (po->proj) );
		 	
/*
 *	Check for Sat projection
 */
	if ( iret != 0 ) {
	
	    sat = &(po->satProjection);
	    
	    gqsatn ( po->proj, sat->imgnam, &iret,
	    	sizeof (po->proj), sizeof (sat->imgnam));

/*
 *	    Bail out if not a sat projection either
 */
	    if ( iret != 0 ) {
	        memset ( po->proj, 0, sizeof( po->proj ));
	        return ;
	    }

	    gqsatm ( po->proj, sat->area, sat->nav, &(sat->ilef),
	    	 &(sat->itop), &(sat->irit), &(sat->ibot),
	    	 &iret, sizeof (po->proj) );
	    for ( i = 0; i < sizeof (sat->imgnam); i++ )
		if ( sat->imgnam [i] == ' ' ) sat->imgnam [i] = '\0';
	}

/*
 *	Replace spaces in the proj string with NULL's
 */	
	for ( i = 0; i < sizeof (po->proj); i++ )
	    if ( po->proj [i] == ' ' ) po->proj [i] = '\0';

/*
 *	Get the lat/lon of 2 reference points and save them. This is
 *	used to see if the mapping has changed between pixmps. Reference
 *	point 1 is 1/2 way down and 1/3 into the window (from the left),
 *	point 2 is also 1/2 way down, and 2/3 into the display.
 */
	ref = &(po->referenceLatLons);

	x = (float) po->width / 3.;
	y = (float) po->height / 2.;

	getlatlon ( Dcoords, &x, &y, &lat, &lon );

	ref->x1 = x;
	ref->y1 = y;
	ref->lat1 = lat;
	ref->lon1 = lon;

	x *= 2;

	getlatlon ( Dcoords, &x, &y, &lat, &lon );

	ref->x2 = x;
	ref->y2 = y;
	ref->lat2 = lat;
	ref->lon2 = lon;

}


void setmapping ( PixmapObjectType *po )
{

	MapProjType	*map;
	SatProjType	*sat;
	ReferencePts	*ref;

	int		iret;
	float		x, y;
	float		lat, lon;
	char		Dcoords [] = "D";

/*
 *	Don't do squatt unless proj is set to something
 */
 	if ( ! po ) return ;
        if ( ( po->proj == NULL ) || ( strlen(po->proj) < 1 ) ) return;

/*
 *	See if we really need to do this or not. Check the location 
 *	of the reference points, if they haven't moved, then we
 *	don't need to reset the georeferencing. This can save
 *	a lot of time, especially for satellite projections.
 */
	ref = &(po->referenceLatLons);

	x = ref->x1;
	y = ref->y1;

	getlatlon ( Dcoords, &x, &y, &lat, &lon );

        if ( (float) fabs (ref->lat1 - lat) < FLOATFUZZ ||
             (float) fabs (ref->lon1 - lon) < FLOATFUZZ ) return;

	x = ref->x2;
	y = ref->y2;

	getlatlon ( Dcoords, &x, &y, &lat, &lon );

        if ( (float) fabs (ref->lat2 - lat) < FLOATFUZZ ||
             (float) fabs (ref->lon2 - lon) < FLOATFUZZ ) return;

/*
 *	Set the projection/mapping based on the projection name.
 *	Look for a few MCIDAS projections otherwise, assume we have a normal
 *	map projection.
 */

	if ( ( strncmp ( po->proj, "MCGOES", 6 ) == 0 ) ||
	     ( strncmp ( po->proj, "MCRADR", 6 ) == 0 ) ||
	     ( strncmp ( po->proj, "MCGVAR", 6 ) == 0 ) ) {
	
	    sat = &(po->satProjection);
	    
	    gsatmg ( sat->imgnam, sat->area, sat->nav, &(sat->ilef),
		     &(sat->itop), &(sat->irit), &(sat->ibot), &iret,
		     strlen (sat->imgnam) );
	}

	else {

	    map = &(po->mapProjection);

	    gsmprj ( po->proj, &(map->angle1), &(map->angle2),
		     &(map->angle3), &(map->latll), &(map->lonll),
		     &(map->latur), &(map->lonur),
		     &iret, strlen (po->proj) );	

	}

}
