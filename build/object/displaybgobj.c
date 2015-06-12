/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	displaybgobj.c (formerly displaymap.c)
 *
 *	Map background display driver.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 6/97	COMET	Changed PutBackgroundMapOnPixmap to
 *			PutMapBackgroundOnPixmap
 *	10/97	COMET	Renamed Display routine, changed mapdraw() to pbgmap(),
 *			changed args. Added code to allow lat/lon grid drawing
 *			from here.
 *	11/97	COMET	Added the determination of drawNewMapFlag,
 *			overlayMapFlag firstMapFlag, and lastMapFlaginto
 *			here. Removed them from pixmapObjectType.
 *	12/97	COMET	Fixed loop index bug in last map determination.
 *	12/97	COMET	Get default text settings from keys in "Garp_defaults".
 *
 ***********************************************************************/


#include "genglobs.h"
#include "winobj.h"
#include "underscore.h"


int
DisplayBackgroundObject ( WindowObjectType *wo, MetObjectType *mot )
{
	int			i, iret, pixIndex;
	MapObjectType		*mobj;
	LatLonGridObjectType	*lobj;
	PixmapObjectType	*po, *ppo;
	GlobalDataObjectType	*gd;
	Pixmap			pixmap;
	int			font_size, overlayClass;
	MetObjectType		*m;
	BooleanType		drawNewMapFlag, overlayMapFlag;
	BooleanType		firstMapFlag, lastMapFlag;
	char			text[GEMPAKSTRING];


	pixIndex = wo->activePixmapObjectIndex;
	po = wo->pixmapObjects[pixIndex];
	gd = GetGlobalDataObject();

	if ( mot->type == LATLONGRIDOBJECT ) {
	    lobj = (LatLonGridObjectType *) mot->metObjectContent;
	    assert( lobj );
	}
	else {   /* itsa map */
	    mobj = (MapObjectType *) mot->metObjectContent;
	    assert( mobj );
	}

/*
 *	Get the font info.
 */
	sprintf ( text, "%s/%s/%s/%s", gd->text_size, gd->text_font,
	                               gd->text_width, gd->text_hw_flag );
	GetDepictableValue ( "FONTSIZE", &font_size );
	SetGemText ( MARKEROBJECT, text, "FONTSIZE", font_size );


/*
 *	Determine whether we need to draw a new map or not. If this is the
 *	first frame, or of the georeferencing between this and the previous
 *	frame are different enough (wobbly GOES images for example), then
 *	we will draw a new map.
 */
	if ( pixIndex == 0 ) 
		drawNewMapFlag = True;
 
	else {

	    ppo = wo->pixmapObjects[pixIndex-1];

	    if ( CompareGeoReferencing ( po, ppo ) != 0 )

		drawNewMapFlag = True;
	    else
		drawNewMapFlag = False;	
	}

/*
 *	Set the map overlay flag if we have IMAGE or COLORFILL
 *	class objects in the metObject list
 */
	overlayMapFlag = False;
	for( i = 0; i <  po->numMetObjects; i++ ) {

	    m = po->metObjects[i];

	    overlayClass = GetMetObjectClass( GetMetObjectType( m ) );

	    if ( overlayClass <= COLORFILL) {
		overlayMapFlag = True;
		break;
	    }
	}

/*
 *	Determine if this is the first background (map) object
 */
	for( i = 0; i < po->numMetObjects; i++ ) {
	    m = po->metObjects[i];
	    if ( GetMetObjectClass( m->type ) == MAP ) break;
	}
	if ( m == mot )
	    firstMapFlag = True;
	else
	    firstMapFlag = False;
	
/*
 *	Determine if this is the last background (map) object
 */
	for( i = po->numMetObjects-1; i >= 0; i-- ) {
	    m = po->metObjects[i];
	    if ( GetMetObjectClass( m->type ) == MAP ) break;
	}
	if ( m == mot )
	    lastMapFlag = True;
	else
	    lastMapFlag = False;

/*
 *	Draw the map into the background pixmap
 */
	if ( drawNewMapFlag ) {
	
	    pixmap = GetWindowObjectBackgroundPixmap( wo );

/*
 *	    If this is the first map, clear the pixmap
 */
	    if ( firstMapFlag )

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

	    if ( mot->type == LATLONGRIDOBJECT ) {

	    	pllgrid ( lobj->line_color,
		lobj->line_type,
		lobj->line_width,
		lobj->lat_inc,
		lobj->lon_inc,
		lobj->label_freq,
		text,
		&iret,
		strlen( lobj->line_color),
		strlen( lobj->line_type),
		strlen( lobj->line_width),
		strlen( lobj->lat_inc),
		strlen( lobj->lon_inc),
		strlen( lobj->label_freq),
		strlen(text) );
	    }
	    else {

	    	pbgmap(	mobj->mapfile,
			mobj->line_color,
			mobj->line_type,
			mobj->line_width,
		  	&iret,
		  	strlen( mobj->mapfile),
			strlen( mobj->line_color),
			strlen( mobj->line_type),
			strlen( mobj->line_width) );
	    }

/*
 *	    Set the pixmap back to the frame's pixmap
 */
	    xsetpxmdata( 0,  0, 
			GetWindowObjectWidth( wo ),
			GetWindowObjectHeight( wo ),
			GetWindowObjectDepth( wo ),
			GetActivePixmap( wo ),
			&iret );

	}
	

/*
 *	If this is the last background object, copy the background pixmap to
 *	the active pixmap.
 */
	if ( lastMapFlag )

	    PutMapBackgroundOnPixmap ( wo, drawNewMapFlag, overlayMapFlag );


	SetMetObjectDrawnFlag( mot, True );


}

