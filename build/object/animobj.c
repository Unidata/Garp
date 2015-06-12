/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	animobj.c
 *
 *	Animation object callbacks and utilities.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/98	COMET	Cast a NULL to a XtIntervalID
 *
 ***********************************************************************/


#include <Xm/Xm.h>

#include "winobj.h"
#include "guimacros.h"


#define FORWARD	 1
#define BACKWARD 2
#define RANDOM	 3

#define FWDONE(x)	((x)->activePixmapIndex = \
				( ++((x)->activePixmapIndex) %  \
					(x)->numPixmaps ))


#define BACKONE(x)	((x)->activePixmapIndex  = \
				--((x)->activePixmapIndex) < 0  ? \
					(x)->numPixmaps - 1 : \
				((x)->activePixmapIndex))

#define COPYPIXMAP(a) (XCopyArea((a)->display, \
				(a)->pixmaps[(a)->activePixmapIndex], \
				(a)->windowID, (a)->gc, 0, 0, \
				(a)->width, (a)->height, 0, 0))
				
void AnimationObjectStop( WindowObjectType *wo );
void AnimationObjectStepBack( WindowObjectType *wo );
void AnimationObjectStepFwd( WindowObjectType *wo );
void AnimationObjectStepToFrame( WindowObjectType *wo, int frame );
void AnimationObjectPlayFwdBack( WindowObjectType *wo );
void AnimationObjectPlayBackward( WindowObjectType *wo );
void AnimationObjectPlayFwd( WindowObjectType *wo );
void SetNextFrame ( AnimationObjectType	*a );
int  CountEligibleFrames ( WindowObjectType *wo );
void FreeAnimationObject( AnimationObjectType *a );



void SetNextFrame ( AnimationObjectType	*a )
{

	PixmapObjectType	*pot;
	int			ntests;
	int			last;


	if ( ! a ) return;


	if ( a->direction != RANDOM ) {

/*
 *	    Keep looping until either it finds a valid frame in the loop
 *	    or we have tested all possible frames
 */
	    ntests = 0;
	    do {
/*
 *		For forward-backward loop, change direction at each end
 */
		if ( a->command == LOOPFORWARDBACKWARD ) {

		    last = a->numPixmaps - 1;
			
		    if( a->activePixmapIndex == 0 && a->direction == BACKWARD )
			a->direction = FORWARD;
		    if( a->activePixmapIndex == last && a->direction == FORWARD )
			a->direction = BACKWARD;
		}

		switch( a->direction ) {

			case FORWARD:
				FWDONE( a );
				break;
			case BACKWARD:
				BACKONE( a );
				break;
		}

		pot = a->windowObject->pixmapObjects[a->activePixmapIndex]; 

		ntests++;

	    }  while ( ! pot->inLoop && (ntests < a->numPixmaps) );

	}  /* ( != RANDOM ) */

	COPYPIXMAP( a ) ;

/*
 *	Set the "current" pixmap pointer back in the "windowObject" 
 *	itself or expose events won't be processed correctly.
 *	Note that the animation objects activePixmapIndex is *not*
 *	the same as the windowObjects activePixmapIndex, but
 *	the pointer to the actual pixmap is shared.  
 *
 *	There may be frames excluded from the animation that exist in
 *	the window object. 
 */
/*	SetActivePixmapObject( a->windowObject, 
				a->pixmaps[a->activePixmapIndex] ); */

	SetActivePixmapObjectIndex ( a->windowObject, a->activePixmapIndex );

/*
 *	Update all the necessary info for this frame
 */
	UpdateFrameComponents ( a->windowObject );


}

void ScheduleAnimation( AnimationObjectType *a )
{

	int		timeout;
	int		last;

/*
 *	Saftey check. If things are working properly, the xt timer
 *	has been de-scheduled because AnimationStop was called.
 */
	if ( ! a->isAnimating )
		return;


	last = a->numPixmaps - 1;

	SetNextFrame ( a );

	timeout = a->dwellMiddle;

	if (a->activePixmapIndex == 0 && a->direction == FORWARD)
		timeout = a->dwellFirst;

	if (( a->activePixmapIndex == 0 && a->direction == BACKWARD ) || 
	    ( a->activePixmapIndex == last && a->direction == FORWARD ) )
		timeout = a->dwellLast;


/*
 *	Reinstall the application timer.
 */
	a->xtIntervalID = XtAppAddTimeOut( a->appContext, 
				timeout, 
				(XtTimerCallbackProc) ScheduleAnimation, 
				(XtPointer) a );

	return;

}


void AnimationObjectPlayFwd( WindowObjectType *wo )
{

	AnimationObjectType	*a;

	a =  wo->animationObject;
	if( ! a ) {
		a = wo->animationObject = AnimationObjectCreate( wo );
		if( ! a ) 
			return;
	}

/*
 *	See if we have enough frames in the loop for an animation
 */
	if ( CountEligibleFrames ( wo ) < 2 ) return;
		
/*
 *	Start things going by setting the animation flag and
 *	scheduling the animation to start...
 */
	a->direction   = FORWARD;
	a->command     = LOOPFORWARD;

	if ( ! a->isAnimating ) {
		a->isAnimating = True;
		ScheduleAnimation( a );
	}
	
	return;

}
void AnimationObjectPlayBackward( WindowObjectType *wo )
{

	AnimationObjectType	*a;

	a =  wo->animationObject;
	if( ! a ) {
		a = wo->animationObject = AnimationObjectCreate( wo );
		if( ! a ) 
			return;
	}

/*
 *	See if we have enough frames in the loop for an animation
 */
	if ( CountEligibleFrames ( wo ) < 2 ) return;
		
/*
 *	Start things going by setting the animation flag and
 *	scheduling the animation to start...
 */
	a->direction   = BACKWARD;
	a->command     = LOOPBACKWARD;
	if ( ! a->isAnimating ) {
		a->isAnimating = True;
		ScheduleAnimation( a );
	}
	
	return;

}

void AnimationObjectPlayFwdBack( WindowObjectType *wo )
{

	AnimationObjectType	*a;

	a =  wo->animationObject;
	if( ! a ) {
		a = wo->animationObject = AnimationObjectCreate( wo );
		if( ! a ) 
			return;
	}

/*
 *	See if we have enough frames in the loop for an animation
 */
	if ( CountEligibleFrames ( wo ) < 2 ) return;
		
/*
 *	Start things going by setting the animation flag and
 *	scheduling the animation to start...
 */
	a->direction   = FORWARD;
	a->command     = LOOPFORWARDBACKWARD;
	if ( ! a->isAnimating ) {
		a->isAnimating = True;
		ScheduleAnimation( a );
	}
	
	return;

}
		
void AnimationObjectStepFwd( WindowObjectType *wo )
{
	AnimationObjectType	*a;

	a =  wo->animationObject;
	if( ! a ) {
		a = wo->animationObject = AnimationObjectCreate( wo );
		if( ! a ) 
			return;
	}

	if( a->isAnimating ) {
		AnimationObjectStop( a->windowObject );
		return;
	}

	a->direction   = FORWARD;
	a->command     = STEPFORWARD;

	SetNextFrame ( a );

	return;
}

void AnimationObjectStepBack( WindowObjectType *wo )
{
	AnimationObjectType	*a;


	a =  wo->animationObject;
	if( ! a ) {
		a = wo->animationObject = AnimationObjectCreate( wo );
		if( ! a ) 
			return;
	}

	if( a->isAnimating ) {
		AnimationObjectStop( a->windowObject );
		return;
	}

	a->direction   = BACKWARD;
	a->command     = STEPBACKWARD;

	SetNextFrame ( a );

	return;
}

void AnimationObjectStepToFrame( WindowObjectType *wo, int index )
{
	AnimationObjectType	*a;

	a =  wo->animationObject;
	if( ! a ) {
		a = wo->animationObject = AnimationObjectCreate( wo );
		if( ! a ) 
			return;
	}

	if( a->isAnimating )
		AnimationObjectStop( a->windowObject );


	a->direction   = RANDOM;
	a->command     = STEPTOFRAME;	
	a->activePixmapIndex = index;	

	SetNextFrame ( a );

	return;
}

void AnimationObjectStop( WindowObjectType *wo )
{
	

	AnimationObjectType	*a;

	assert( wo );

	a = wo->animationObject;
	if( ! a ) {
		return;
	}

/*
 *	Remove the XtTimer so that it won't fire again.
 */
	if ( a->xtIntervalID ) XtRemoveTimeOut( a->xtIntervalID );

	a->isAnimating = False;

/*
 *	Update all the necessary info where the animation stopped
 */
	UpdateFrameComponents ( wo );
	
	return;
}
	
AnimationObjectType *AnimationObjectDestroy( WindowObjectType *wo )
{

	assert( wo );

	if( ! wo->animationObject ) 
		return(wo->animationObject);

	FreeAnimationObject( wo->animationObject );
	wo->animationObject = (AnimationObjectType *) NULL;

	return(wo->animationObject);

} 

AnimationObjectType *AnimationObjectCreate( WindowObjectType *wo )
{


	AnimationObjectType	*a;
	int			numPixmaps;

	numPixmaps         = GetWindowObjectNumPixmapObjects( wo );

	if( numPixmaps < 2 )
		return( (AnimationObjectType *) NULL );

	a = (AnimationObjectType *) malloc( sizeof( *a ) );
	if( ! a ) {
		printf("CreateAnimationObject: Bad malloc of AnimateObject.\n");
		return( (AnimationObjectType *) NULL );
	}

	a->pixmaps            = GetWindowObjectPixmapList( wo );
	a->display            = GetWindowObjectDisplay( wo );
	a->appContext         = GetWindowObjectAppContext( wo );
	a->windowID           = GetWindowObjectWindowID( wo );
	a->activePixmapIndex  = GetActivePixmapObjectIndex( wo );
	a->width              = GetWindowObjectWidth( wo );
	a->height             = GetWindowObjectHeight( wo );
	a->gc                 = GetWindowObjectGC( wo );
	a->numPixmaps         = numPixmaps;
	a->isAnimating        = False;
	a->windowObject       = wo;
	a->xtIntervalID	      = (XtIntervalId) NULL;
	a->dwellFirst	      = GetDwellFirstValue ();
	a->dwellMiddle	      = GetDwellMiddleValue () ;
	a->dwellLast	      = GetDwellLastValue () ;


	return( a );

}

void FreeAnimationObject( AnimationObjectType *a )
{

	assert( a );
	assert( a->pixmaps );

	if( a->isAnimating )
		AnimationObjectStop( a->windowObject );

	free( a->pixmaps );
	free( a );
}


int
CountEligibleFrames ( WindowObjectType *wo )
{

	int	i, numPixmaps, count;


	numPixmaps = wo->numPixmapObjects;

	for ( i = 0, count = 0; i < numPixmaps; i++ )
	    if ( wo->pixmapObjects[i]->inLoop ) count++;

	return (count);
}

void	
AnimationObjectPrintf( AnimationObjectType *a )
{

	printf("AnimationObjectPrintf:\n");
	printf("   appContext   ->   %X\n", a->appContext );
	printf("   display      ->   %X\n", a->display );
	printf("   WindowID     ->   %X\n", a->windowID );
	printf("   windowObject ->   %X\n", a->windowObject );
	printf("   xtIntervalID ->   %X\n", a->xtIntervalID );
	printf("   GC           ->   %X\n", a->gc );

	printf("   Width        ->   %d\n", a->width);
	printf("   Height       ->   %d\n", a->height);

	printf("   numPixmaps   ->   %d\n", a->numPixmaps );
	printf("   activePixmap ->   %d\n", a->activePixmapIndex );
	printf("   dwellMiddle  ->   %d\n", a->dwellMiddle );
	printf("   dwellLast    ->   %d\n", a->dwellLast );
	printf("   dwellRate    ->   %d\n", a->dwellRate );
	printf("   isAnimating  ->   %d\n", a->isAnimating );

}

