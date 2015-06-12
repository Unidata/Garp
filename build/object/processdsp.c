/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	processdsp.c
 *
 *	Processes the display list of metObjects.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	10/97	COMET	Removed call to ResetGarpLogo()
 *	11/97	COMET	Changed args in call to UpdatePixmapObjectGeoRef
 *	11/97	COMET	Added toggledOn flag to metObject. Moved the
 *			global int TitleLine into titleObject.
 *	11/97	COMET	Added call to dscolr to guarantee color change.
 *	11/97	COMET	Increment titleIndex within ProcessDisplayList.
 *	01/98	COMET	Moved toggled off metobject's title plotting
 *	 2/98	COMET	Fixed title index bug for lat/lon grid and
 *			background images
 *	 2/98	COMET	Added mode detailed check for abortType
 *
 ***********************************************************************/


#include <Xm/Xm.h>

#include "winobj.h"
#include "underscore.h"


void ProcessDisplayList( WindowObjectType *wo )
{

    int			numPixmapObjects;
    int			lastPotDrawn;
    PixmapObjectType	*pot;
    MetObjectType	*mot;

    BooleanType		refreshPartialFrames;
    int			iret, i, titleIndex;
    int			order [MAXMETOBJS];
    int			potIndex;
    int			motIndex;
    int			overlayClass;
    int			abortType;
    char		busyString[] = "Processing...";


/*
 *  Get a count of pixmapObjects...
 */
    lastPotDrawn = 0;
    numPixmapObjects = wo->numPixmapObjects;

/*
 *  Setup the conditions so that we can be interrupted during
 *  the loop.
 */ 
    SetupAbortProcess( wo );
    SetGarpLogo ( wo, busyString );

/*
 *  Add a flag so that expose events aren't processed while 
 *  updating the display objects
 *
 */
    SetProcessExposeEventsFlag( wo, FALSE );

/*
 *  Get a flag that indicates whether imtermediate products in the
 *  metObject loop should be displayed.
 */
    refreshPartialFrames = GetPartialFrameRefreshFlag( wo );  

/*
 *  Loop through everything, redisplaying the object...
 */

    for( potIndex = 0; potIndex < numPixmapObjects; potIndex++ ) {

/*
 *	Get a convenient pixmapObject pointer...
 */
	pot = wo->pixmapObjects[potIndex];

/*
 *	Set the index in the Window Object to point to the
 *	current pixmap.  This is used right now to indicate which
 *	pixmap is in the Xwindow and for handling expose events.
 *	It is also set so that the georeferencing information gets
 *	saved with the correct pixmapobject.
 */
	SetActivePixmapObjectIndex( wo, potIndex );

	xsetpxmdata(	0,  0, 
			GetWindowObjectWidth( wo ),
			GetWindowObjectHeight( wo ),
			GetWindowObjectDepth( wo ),
			pot->pixmap,
			&iret );


/*
 *	Update georeferencing for this pixmapObject
 */
	UpdatePixmapObjectGeoRef ( wo, potIndex);

	
/*
 *	Determine the metObject draw order
 */
	GetMetObjectDrawOrder ( pot->metObjects, pot->numMetObjects, order ); 

/*
 *	Reset title index.
 */
	titleIndex = 0;

	for( motIndex = 0; motIndex <  pot->numMetObjects; motIndex++ ) {


/*
 *		Check for abort action...
 */
		abortType = GetWindowObjectAbortFlag( wo );
		if( abortType == ABORTCLEAR ) {
		    ClearMainWindowCB ( NULL, NULL, NULL );
		    return;
		}
		else if ( abortType == ABORTRESET ) {
		    WindowObjectFullReset( wo );
		    return;
		}
			
/*
 *		Get a convenient pointer to this metObject...
 */
		mot = pot->metObjects[order[motIndex]];

/*
 *		Increment title index.
 */
		overlayClass = GetMetObjectClass( mot->type );
		if ( overlayClass == MAP ||
		     overlayClass == OVERLAY ||
		     overlayClass == BACKGROUND )  {
		    mot->titleIndex = -1;
		}
		else {
		    mot->titleIndex = titleIndex;
		    wo->titleObject->numTitles = titleIndex + 1;
		    titleIndex++;
		}
/*
 *		Check to see if the metObject is being used.  It
 *		could be for instance toggled off, although this
 *		functionality is not implemented in general
 */
		if ( ! mot->inUse )
			continue;

/*
 *		Because metObjects get "added" to the display, there
 *		may be some objects that have been drawn into the display
 *		already and don't need redrawn.  The drawn characteristic
 *		is set by the object's display function.
 */
		if ( GetMetObjectDrawnFlag( mot ) )
			continue;

/*
 *		Check if the metObject is toggled on or off. It is on
 *		by default but can be toggled off by clicking on the
 *		title. If it is off, plot just the title.
 */
		if ( ! mot->toggledOn && mot->titleIndex >= 0 ) {
			DrawToggledOffTitle ( mot->titleString, mot->titleIndex );	
			continue;
		}

/*
 *		Really just a saftey check...a null func pointer at this
 *		stage is a bad thing.
 */
		if ( ! mot->displayFunc ) 
			continue;


		UpdateTitleObject( wo );

/*
 *		Keep track of the last pixmap object that we have
 *		drawn into, so that the frame pointers can be adjusted
 *		correctly.  
 */
		lastPotDrawn = potIndex;

/*
 *		Do the dirty work...
 */
		(*mot->displayFunc)( wo, mot );

/*
 *		Refresh pixmap to the screen if so desired by the user
 */
		if ( refreshPartialFrames ) WindowObjectPixmapRefresh( wo );

/*
 *		Refresh the Motif interface.  Has side effect
 *		of processing pending callbacks.
 */
		XmUpdateDisplay( GetWindowObjectCanvas( wo ) );


	}  /*  metobject loop */

/*
 *	Refresh the current pixmap to the screen (but avoid doing this
 *	if refreshing is being done between metObjects), and update frame
 *	component gui components.
 */
	if ( ! refreshPartialFrames ) WindowObjectPixmapRefresh( wo );

	UpdateFrameComponents ( wo ); 


    }  /*  pixmap loop  */



/*
 *
 *  Allow expose events to be processed now.  Set the current 
 *  pixmapObject *  to be the last frame we updated so that 
 *  expose events get  processed correctly.  A heavy handed 
 *  approach is to set the active pixmap object and force an 
 *  expose event.  I've left the WindowObjectRedraw() call 
 *  commented out to see if it works OK with out it.  This 
 *  whole chunck of code is to fix the bug that occurs when 
 *  overlaying data and the number of metObjects to overlay 
 *  doesn't match up with the number of pixmapObjects.
 *
 *
 */
    SetProcessExposeEventsFlag( wo, TRUE );
    SetActivePixmapObjectIndex( wo, lastPotDrawn );
    UpdateFrameComponents ( wo ); 

    /* WindowObjectRedraw( wo );	*/

    WindowObjectPixmapRefresh( wo );


/*
 *  Cancel the abort handler...
 */
    CancelAbortProcess( wo, False );

/*
 *  Give the canvas the input focus. Otherwise, in certain situations the
 *  keyboard loop controls won't work unless the user leaves and re-enters
 *  the window.
 */
    GetInputFocus ( GetWindowObjectCanvas( wo), NULL, NULL, NULL );

    return;

}
