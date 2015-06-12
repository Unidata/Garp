/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	winobj.c
 *
 *	WindowObject routines.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 8/97	COMET	Added DataTimeObject to WindowObjectType.
 *	 8/97	COMET	Added routine to recycle old pixmapobjects.
 *	 9/97	COMET	Added WindowObjectRefreshBackground()
 *			then removed it in favor of redrawing the 
 *			background map via ProcessDisplayList()
 *	11/97	COMET	Enabled titleObject.
 *	12/97	COMET	Added call to SetBaseImageColorsFlag
 *	 1/98	COMET	Added some casts.
 *
 ***********************************************************************/


#include "genglobs.h"
#include "winobj.h"


static WindowObjectType	*baseWindowArray;
static int		baseWindowCnt;
static WindowObjectType	*activeWindow;
static WindowObjectType *GetNextFree();




/*
 *	Returns the background pixmap. Makes one if it does not exist.
 */

Pixmap GetWindowObjectBackgroundPixmap (WindowObjectType *wo)
{

	if ( ! wo->backgroundPixmap )
            wo->backgroundPixmap = CreatePixmapFromWindowObject( wo );
	
	return (wo->backgroundPixmap);
}


/*
 *	Destroys the background pixmap.
 */

void WindowObjectDestroyBackground ( WindowObjectType *wo )
{

	if ( wo->backgroundPixmap ) {

	    XFreePixmap (	GetWindowObjectDisplay(wo),
	    			wo->backgroundPixmap );

	    wo->backgroundPixmap = (Pixmap) NULL;
	}
}


void WindowObjectRedraw( WindowObjectType *wo )
{
	
/*
 *	Redraw the window from the active pixmap by forcing
 *	an expose event for this window (if it is on the 
 *	screen).
 *
 */
	if( XtIsRealized( GetWindowObjectCanvas( wo ) ) )
		XClearArea( GetWindowObjectDisplay( wo ),
			GetWindowObjectWindowID( wo ),
			 0, 0, 0, 0, TRUE );

	return;

}



void WindowObjectPixmapRefresh( WindowObjectType *wo )
{

	Pixmap			pixmap;

	pixmap = GetActivePixmap( wo ); 

/*
 *	It's not an error for this pixmap to not exist, but only
 *	do the XCopyArea if there is pixmap data
 */
	if( pixmap != (Pixmap) NULL )

		XCopyArea (	GetWindowObjectDisplay(wo),
	    			pixmap,
				GetWindowObjectWindowID(wo),
				GetWindowObjectGC(wo),
				0, 0,
				GetWindowObjectWidth(wo),
				GetWindowObjectHeight(wo),
				0, 0);

/*
 *	The newly exposed portion may now contain the pointer anchor point,
 *	so redraw just in case.
 */
	UpdateAnchorPoint ( wo );

	return;

}


void WindowObjectClearCanvas( WindowObjectType *wo )
{
	
/*
 *	Set the drawing color to GEMPAK's background color.
 */
	 XSetForeground ( GetWindowObjectDisplay( wo ),
			GetWindowObjectGC( wo ),
			GetGemBackgroundPixel() );


/*
 *	Clear the drawing canvas.
 *
 */
	if( XtIsRealized( GetWindowObjectCanvas( wo ) ) )

		XFillRectangle( GetWindowObjectDisplay( wo ), 
			GetWindowObjectWindowID( wo ),
			GetWindowObjectGC( wo ), 0, 0,
			GetWindowObjectWidth( wo ),
			GetWindowObjectHeight( wo ) );


	return;

}


void SetProcessExposeEventsFlag( WindowObjectType *wo, BooleanType flag )
{

	if( ! wo )
		return; 

	wo->processExposeEventsFlag = flag;

}

BooleanType GetProcessExposeEventsFlag( WindowObjectType *wo )
{

	if( ! wo )
		return( FALSE ); 

	return( wo->processExposeEventsFlag );

}


void SetPartialFrameRefreshFlag( WindowObjectType *wo, BooleanType flag )
{

	if( ! wo )
		return; 

	wo->showPartialFrameFlag = flag;

}


BooleanType GetPartialFrameRefreshFlag( WindowObjectType *wo )
{

	if( ! wo )
		return( FALSE ); 

	return( wo->showPartialFrameFlag );

}



void SetPixmapObjectDrawnFlag( PixmapObjectType *po, int flag)
{

	MetObjectType		**mv;
	int			i;


	mv = po->metObjects;

	for( i = 0; i < po->numMetObjects; i++, mv++) 
		SetMetObjectDrawnFlag( *mv, flag );


	return;
}


void SetWindowObjectDrawnFlag( WindowObjectType *wo, int flag)
{

	PixmapObjectType	**pv;
	int			i;

/*
 *	Loop through each PixmapObjects, resetting the drawn flag.
 */
	pv = wo->pixmapObjects;

	for( i = 0; i < wo->numPixmapObjects; i++, pv++)
		SetPixmapObjectDrawnFlag ( *pv, flag);


	return;
}

	

Pixmap *GetWindowObjectPixmapList( WindowObjectType *wo )
{

	int i;
	Pixmap			*pixmaps;
	PixmapObjectType	**p;
	int			numPixmaps;

	p          = wo->pixmapObjects;
	numPixmaps = wo->numPixmapObjects;
	pixmaps    = ( Pixmap * ) malloc( sizeof(pixmaps) * numPixmaps );

	for( p = wo->pixmapObjects, i = 0; i < numPixmaps; i++, p++ ) {
		if( ! (*p)->inUse )
			printf("GetWindowObjectPixmapList: Found !inUse.\n");
		pixmaps[i] = (*p)->pixmap;
	}
		
	return( pixmaps );

}
		
PixmapObjectType *GetActivePixmapObject( WindowObjectType *wo )
{

	int 			index;
	PixmapObjectType	*p;

/*
 *	Get the index of the active PixmapObject.  This has the
 *	potential of not being set, depending on the state
 *	of the interface
 */
	if( ! wo )
		return( NULL );

	index = GetActivePixmapObjectIndex( wo );
	p     = wo->pixmapObjects[index];

/*
 *	Range check the index and other values.
 */
	if( index >= 0 && index < MAXPIXMAPOBJS && p && p->inUse && p->pixmap )
		return( p );

/*
 *	Sometimes there isn't one....
 */
	return( NULL );

}

Pixmap GetActivePixmap( WindowObjectType *wo )
{

	int 			i;
	PixmapObjectType	*p;

/*
 *	Get the index of the active Pixmap.  This has the
 *	potential of not being set, depending on the state
 *	of the interface
 */
	if( ! wo )
		return( (Pixmap)NULL );

	i = GetActivePixmapObjectIndex( wo );

	p = wo->pixmapObjects[i];

/*
 *	Range check the number.
 */
	if( i >= 0 && i < MAXPIXMAPOBJS && p && p->inUse && p->pixmap )
		return( p->pixmap );

/*
 *	Otherwise, sometimes there isn't one....
 */
	return( (Pixmap) NULL );

}


/*
 *
 *	Set the active PixmapIndex to index.
 *
 */
void SetActivePixmapObjectIndex( WindowObjectType *wo, int index )
{

	wo->activePixmapObjectIndex = index;
	return;
	
}

/*
 *
 *	Set the window objects active or current pixmap object to 
 *	the Object that is referenced by the formal paramater pixmap.
 *
 */
void SetActivePixmapObject( WindowObjectType *wo, Pixmap pixmap )
{

	int 			i;
	PixmapObjectType	**p;

	assert( wo );
	assert( pixmap );

	p = wo->pixmapObjects;

	for( i = 0; i < MAXPIXMAPOBJS; i++, p++ )
		if( *p && (*p)->inUse && (*p)->pixmap )
			if( (*p)->pixmap == pixmap )
				break;
	
/*
 *	What to do about errors?  Right now, just silently fail the 
 *	operation.
 */
	if( i < MAXPIXMAPOBJS )
		SetActivePixmapObjectIndex( wo, i );

	return;

}


/*
 *	Recycles PixmapObjects. Called by the autoupdate facility.
 *	Moves (circularly cycles) the pixmaps so the ones going out of date
 *	can be re-used for new plots.
 */

void RecyclePixmapObjects( WindowObjectType *wo, int count )
{

	int			i, j;
	int			numPixmaps;
	PixmapObjectType	*p;


	numPixmaps = wo->numPixmapObjects;

/*
 *	Return if the number to recycle is less than 1, or if the number
 *	to recycle is equal or more than the number of pixmaps (no need to
 *	move them around if they are all going to be redrawn anyway.)
 */
	if ( count < 1 || count >= numPixmaps ) return;

	for( i = 0; i < count; i++ )
	{
	    p = wo->pixmapObjects[0];

	    for ( j = 1; j < numPixmaps; j++ )
	    	wo->pixmapObjects[j-1] = wo->pixmapObjects[j];

	    wo->pixmapObjects[numPixmaps-1] = p;
	}
		
	return;

}

		
void SetWindowObjectXstuff( WindowObjectType *wo )
{


	Dimension	wdth, hght;

	assert( wo );

	wo->appContext = XtWidgetToApplicationContext ( wo->canvas );
	wo->display    = XtDisplay ( wo->canvas );
	wo->root       = DefaultRootWindow ( wo->display );
	wo->windowID   = XtWindow ( wo->canvas );
        wo->gc         = XCreateGC ( wo->display, wo->windowID, 0, 0 );
        wo->depth      = DefaultDepthOfScreen ( XtScreen ( wo->canvas ) );

	XtVaGetValues( wo->canvas, XmNwidth,  &wdth,
                               XmNheight, &hght,
                               NULL );

	wo->width      = wdth;
	wo->height     = hght;

	return;

}


void InitWindowObjects( WindowObjectType *wo, int cnt )
{
	assert( wo && cnt );

	baseWindowArray = wo;
	baseWindowCnt   = cnt;

	return;

}

static WindowObjectType *GetNextFree()
{

	int i;
	WindowObjectType *p;

/*
 *	Safety check
 */
	if ( baseWindowCnt <= 0 )
		return ( ( WindowObjectType * ) NULL );

/*
 *	Search through the windows looking for name.
 */
	for(i = 0, p = baseWindowArray; i < baseWindowCnt; i++, p++ ) {
		if (  p->inUse  )
			continue;
		break;
	}

/*
 *	If the window name was found return it, otherwise return NULL
 */
	if ( i < baseWindowCnt ) {
		memset( p, 0, sizeof( *p ) );
		return ( p );
	}

	return( (  WindowObjectType * ) NULL );

}


WindowObjectType *CreateWindowObject( char *windowName )
{
	WindowObjectType	*wot;
	PixmapObjectType	*pot;
	MetObjectType		*mot;
	TitleObjectType		*tot;

/*
 *	If it's there already, return an error.
 */
	if ( GetWindowObjectByName( windowName ) )
		return( ( WindowObjectType * ) NULL );
	
/*
 *	Return the next available window object.
 */
	if (( wot = GetNextFree() ) == ( WindowObjectType * ) NULL ) {
		printf("*** CreateWindowObject: Can't create new window.\n");
		return( ( WindowObjectType * ) NULL );
	}

	
	tot = TitleObjectAllocate();
	if ( ! tot ) {
		printf("*** CreateWindowObject: Can't create titleObject.\n");
		return( ( WindowObjectType * ) NULL );
	}

	wot->titleObject               = tot;
	wot->inUse                     = TRUE;
	wot->geoRefObject              = NULL;
	wot->processExposeEventsFlag   = TRUE;
	wot->showPartialFrameFlag      = FALSE;
	wot->backgroundPixmap	       = (Pixmap) NULL;
	
	strcpy( wot->windowName, windowName );

	return ( wot );

}
	
WindowObjectType *GetActiveWindowObject()
{
	return( activeWindow );	
}


WindowObjectType *GetWindowObjectByName( char *windowName )
{
	WindowObjectType	*p;
	int i;

/*
 *	Safety check
 */
	if ( baseWindowCnt <= 0 )
		return ( ( WindowObjectType * ) NULL );
	if ( windowName == NULL )
		return ( ( WindowObjectType * ) NULL );
	
/*
 *	Search through the windows looking for name.
 */
	for(i = 0, p = baseWindowArray; i < baseWindowCnt; i++, p++ ) {
		if ( ! p->inUse  )
			continue;
		if (strcmp( p->windowName, windowName ))
			continue;
		break;
	}

/*
 *	If the window name was found return it, otherwise return NULL
 */
	if ( i < baseWindowCnt )
		return ( p );

	return( (  WindowObjectType * ) NULL );

}
	

WindowObjectType *SetActiveWindowObject( WindowObjectType *wo )
{

/*
 *	Safety check.
 */
	if ( wo == NULL )
		printf("*** SetActiveWindowObject:  Active window set to NULL.\n");

	return ( activeWindow = wo );

}

WindowObjectType *SetActiveWindowObjectByName( char *windowName )
{
	WindowObjectType	*p;
	int i;

	p = GetWindowObjectByName ( windowName );

	if ( p == ( WindowObjectType * ) NULL )
		return( NULL );

	return ( activeWindow = p );

}

WindowObjectType *GetWindowObjectByWidget( Widget w )
{
	WindowObjectType	*wo;
	int i;

/*
 *	Safety check
 */
	if ( baseWindowCnt <= 0 )
		return ( ( WindowObjectType * ) NULL );
	
/*
 *	Search through the windows looking for name.
 */
	for(i = 0, wo = baseWindowArray; i < baseWindowCnt; i++, wo++ ) {
		if ( ! wo->inUse  )
			continue;
		if ( w == wo->canvas )
			break;
		break;
	}

/*
 *	If the window name was found return it, otherwise return NULL
 */
	if ( i < baseWindowCnt )
		return ( wo );

	return( (  WindowObjectType * ) NULL );

}

void WindowObjectResize( WindowObjectType *wo, 
                         Dimension width, 
                         Dimension height )

{

	int			i;
	PixmapObjectType	**pot;
	Dimension		orig_width;
	Dimension		orig_height;
	WindowObjectType	*orig_wo;

	orig_width  = GetWindowObjectWidth( wo );
	orig_height = GetWindowObjectHeight( wo );

/*
 *	Check for a new window size.
 */
	if( width == orig_width && height == orig_height )
		return;

/*
 *	See if the resize is ocurring in the current window or
 *	if we need to change the active window.
 */
	orig_wo = GetActiveWindowObject();
	if( wo != orig_wo ) {
		SetActiveWindowObject( wo );
	}
		

/*
 *	Resizing the window automatically invalidates
 *	the animation object (since it contains pointers to pixmaps
 *	in contained in the windowObject.
 */
	AnimationObjectDestroy( wo );

/*
 *	Set the new size.
 */
	SetWindowObjectWidth( wo, width );
	SetWindowObjectHeight( wo, height );

/*
 *      Recreate the Pixmap from the window object.
 */
	if ( wo->backgroundPixmap ) 
		FreePixmap( GetWindowObjectDisplay( wo ), wo->backgroundPixmap );

        wo->backgroundPixmap = CreatePixmapFromWindowObject( wo );

        ClearPixmap( wo,  wo->backgroundPixmap );

/*
 *	Reset Gempak cordinate transformation mappings to the new window
 *	size.
 */
	GempakResize( GetWindowObjectName( wo), (int) width, (int) height );

/*
 *	Loop through each Pixmap, resizing each one in turn.
 */
	for( i=0, pot = wo->pixmapObjects; i < wo->numPixmapObjects;i++,pot++)

		PixmapObjectResize( wo, *pot, width, height );

/*
 *	Redraw everything that is now out of date.
 */
	ProcessDisplayList( wo );

/*
 *	Reset the focus back to where it was...
 */
	if( wo != orig_wo ) {
		SetActiveWindowObject( orig_wo );
	}

/*
 *	That's it.
 */
	return;

}

/*
 *	Reset the Window Object to it's default state.  Deallocate
 *	resources and clear the window.
 */
void WindowObjectPartialReset( WindowObjectType *wo )
{

	CancelAbortProcess( wo, True );

	TitleObjectClearEntries( wo );

	AnimationObjectDestroy( wo );


/*	Use FreeAllPots instead of ReleaseAllPots because a pixmapObject
 *	could be reused that has the wrong characteristics - like size
 */
	FreeAllPots( wo );

	SetProcessExposeEventsFlag( wo, TRUE );

	WindowObjectClearCanvas( wo );

	ZeroObjectColors();

	SetBaseImageColorsFlag ( FALSE );

	ZeroWindPosition();

	UpdateFrameComponents ( wo );

	ClearDataTimeObject ( wo );
}

void WindowObjectFullReset( WindowObjectType *wo )
{

	ReleaseGeoRefObject( wo );	

	SetAnchorPointFlag ( False );	

	WindowObjectPartialReset( wo );

	WindowObjectDestroyBackground (wo);
}

/*
 *
 *	Clear out the pixmaps and xwindow so that we can redraw
 *	the current set of metObjects.
 *
 */
void WindowObjectClear( WindowObjectType *wo )
{

	AnimationObjectDestroy( wo );

	ClearAllPots( wo );

	WindowObjectClearCanvas( wo );


}


int
RefreshDisplay ( void )
/*
 * This function is used to redraw pixmaps when a general feature such
 * as garea or the size of a depictable has been changed. It will redraw
 * all pixmaps associated with the active window object.
 */
{
	WindowObjectType	*wo;

	wo = GetActiveWindowObject();
	if ( ! wo ) 
		return -1;

/*
 *	Clear all the pixmaps and the display.
 */
	WindowObjectClear( wo );

/*
 *	Set the drawn flag in each MetObject, so they can
 *	refresh themselves with the new garea and projection.
 */
	SetWindowObjectDrawnFlag( wo, False );

/*
 *	Process the display list.
 */
	ProcessDisplayList( wo );

	return 0;
}
