/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	displaymap.c
 *
 *	Map background display driver.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	06/97	COMET	Changed PutBackgroundMapOnPixmap to
 *			PutMapBackgroundOnPixmap
 *
 ***********************************************************************/


#include "genglobs.h"
#include "winobj.h"
#include "underscore.h"


int
DisplayMap ( WindowObjectType *wo, MetObjectType *mot )
{
	int			iret, pixIndex;
	MapObjectType		*mobj;
	PixmapObjectType	*po;
	Pixmap			pixmap;


	mobj = (MapObjectType *) mot->metObjectContent;

	assert( mobj );

	pixIndex = wo->activePixmapObjectIndex;

	po = wo->pixmapObjects[pixIndex];


/*
 *	If we need a new map, draw it into the background pixmap
 */	
	if ( po->drawMapFlag ) {
	
	    pixmap = GetWindowObjectBackgroundPixmap( wo );

/*
 *	    If this is the first map, clear the pixmap
 */
	    if ( mobj->mapIndex == 1 )

		    ClearPixmap ( wo, pixmap );

/*
 *	    Tell GEMPAK to draw into the background pixmap
 */
	    xsetpxmdata( 0,  0, 
			GetWindowObjectWidth( wo ),
			GetWindowObjectHeight( wo ),
			GetWindowObjectDepth( wo ),
			pixmap,
			&iret );

/*
 *	    Draw, sucka
 */
	    mapdraw(	mobj->mapfile,
			mobj->attributes,
		  	&iret,
		  	strlen( mobj->mapfile),
			strlen( mobj->attributes) );

/*
 *	    Set the pixmap back
 */
	    xsetpxmdata( 0,  0, 
			GetWindowObjectWidth( wo ),
			GetWindowObjectHeight( wo ),
			GetWindowObjectDepth( wo ),
			GetActivePixmap( wo ),
			&iret );

	}
	

/*
 *	If this is the last map, copy the background to the active pixmap,
 *	and set the flag in the pixmapObject that maps have been drawn
 */
	if ( mobj->mapIndex == po->numMaps )

	    PutMapBackgroundOnPixmap ( wo, po->drawMapFlag, po->overlayMapFlag );


	SetMetObjectDrawnFlag( mot, True );


}

