/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	pot.c
 *
 *	Functions that manage pixmap objects.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	05/97	COMET	For version 2.0, removed image data
 *	 9/97	COMET	Added ClearPot()
 *	11/97	COMET	Removed	drawMapFlag numMaps and overlayMapFlag from 
 *			pixmapObject
 *
 ***********************************************************************/


#include "winobj.h"
#include "_proto.h"

void FreePixmap ( Display *display, Pixmap p );


void ClearPixmap( WindowObjectType *wo, Pixmap p )
{

	
/*
 *	Set the drawing color to GEMPAK's background color.
 */
	XSetForeground ( GetWindowObjectDisplay( wo ),
			GetWindowObjectGC( wo ),
			GetGemBackgroundPixel() );


/*
 *
 *	Flood the pixmap with the new color.
 *
 */
	XFillRectangle( GetWindowObjectDisplay( wo ), p, 
				GetWindowObjectGC( wo ), 0, 0,
				GetWindowObjectWidth( wo ),
				GetWindowObjectHeight( wo ) );

	return;

}

PixmapObjectType *CreatePot( WindowObjectType *wo )
{

	PixmapObjectType	*pot;
	int			i;


	pot = (PixmapObjectType *) malloc( sizeof( *pot ) );
	if( !pot ) {
		printf("CreatePot:  malloc error\n");
		return( (PixmapObjectType *) NULL );
	}

	memset( pot, 0, sizeof( *pot ) );

	pot->display        = GetWindowObjectDisplay( wo );
	pot->pixmap         = CreatePixmapFromWindowObject( wo );
	pot->width          = GetWindowObjectWidth( wo );
	pot->height         = GetWindowObjectHeight( wo );
	pot->inUse          = True;
	pot->inLoop	    = True;
	pot->numMetObjects  = 0;

	return( pot );

}

Pixmap CreatePixmapFromWindowObject( WindowObjectType *wo )
{

	Pixmap		p;

	p = XCreatePixmap( GetWindowObjectDisplay(wo),
				GetWindowObjectRoot(wo),
				GetWindowObjectWidth(wo),
				GetWindowObjectHeight(wo),
				GetWindowObjectDepth(wo) );

	return( p );
	
}


void FreePot( PixmapObjectType *pot )
{
	
	assert( pot );

	FreeAllMots( pot );
	FreePixmap( pot->display, pot->pixmap );
	Free( pot );

}

void FreeAllPots( WindowObjectType *wo )
{
	int			i;
	PixmapObjectType	**p;
	
	assert( wo );

	for( p = wo->pixmapObjects; *p; p++ ) {

		FreePot( *p ); 

		*p = (PixmapObjectType *) NULL;

		wo->numPixmapObjects--;

	}

	assert( wo->numPixmapObjects == 0 );

	return;

}

void FreePixmap ( Display *display, Pixmap p )
{
	assert( p );
	XFreePixmap( display, p );

}


PixmapObjectType *GetNextPot( WindowObjectType *wo )
{
	PixmapObjectType	**p;
	int			i;

	for( i = 0, p = wo->pixmapObjects; i < MAXPIXMAPOBJS; i++, p++ ) {
		if( ! *p )
			break;

		if( !(*p)->inUse )
			break;
	}

	if( ! (*p) && i >= MAXPIXMAPOBJS ) {
		printf( "GetNextPot: Out of PixmapObjects.\n");
		return( (PixmapObjectType *) NULL );
	}

	if( ! *p )
		*p = CreatePot( wo );

	(*p)->inUse         = True;

	(*p)->inLoop	    = True;

	ClearPixmap( wo,  (*p)->pixmap );

	wo->numPixmapObjects++;

	return( *p );

}

void ReleasePot( PixmapObjectType *pot )
{
	assert( pot );

	pot->inUse = False;

	pot->inLoop = True;

	ReleaseMotList( pot->numMetObjects, pot->metObjects );
	pot->numMetObjects = 0;

	memset( pot->proj, 0, sizeof( pot->proj ) );
	
	return;
}

void ReleaseAllPots( WindowObjectType *wo )
{
	int			i;
	PixmapObjectType	**p;

	assert( wo );

	for( p = wo->pixmapObjects; *p; p++ )  {

		if( ! (*p)->inUse )
			continue;

		ReleasePot( *p );

		wo->numPixmapObjects--; 

	}

	assert( wo->numPixmapObjects == 0 );

	return;

}

/*
 *  Clear out all the pixmaps for window Object.
 */
void ClearAllPots( WindowObjectType *wo )
{
	int			i;
	PixmapObjectType	**p;

	assert( wo );

	for( p = wo->pixmapObjects; *p; p++ )  {

		if( ! (*p)->inUse )
			continue;

		ClearPixmap( wo,  (*p)->pixmap );

	}

	return;

}


/*
 *  Clear out the pixmap for this Object.
 */
void ClearPot( WindowObjectType *wo, PixmapObjectType *po )
{

	assert( po );

	ClearPixmap( wo,  po->pixmap );

	return;

}

void PixmapObjectResize( WindowObjectType	*wo, 
			PixmapObjectType	*pot,
			Dimension		width,
			Dimension		height)
{

	int		numMetObjects;
	int		i;
	MetObjectType	**mot;

	pot->width = width;
	pot->height = height;

	FreePixmap( GetWindowObjectDisplay( wo ), pot->pixmap );

/*
 *	Recreate the Pixmap from the window object.
 */
	pot->pixmap = CreatePixmapFromWindowObject( wo );

	ClearPixmap( wo,  pot->pixmap );

	numMetObjects = pot->numMetObjects;

	for( i = 0, mot = pot->metObjects; i < numMetObjects; i++, mot++ ) {

		SetMetObjectDrawnFlag( *mot, False );

	}

	return;

}


void SavePixmapGeoMapping ( WindowObjectType *wo )
{

	PixmapObjectType	*po;

	po = GetActivePixmapObject( wo );

	savemapping( po ) ;

	return;
}

void SetPixmapGeoMapping ( WindowObjectType *wo )
{

	PixmapObjectType	*po;

	po = GetActivePixmapObject( wo );
	
	setmapping( po ) ;

	return;
	
}
