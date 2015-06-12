/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	updateobj.c
 *
 *	Auto Update utilities.
 *
 *	History:
 *
 *	 9/97	COMET	Original copy
 *	10/97	COMET	Set a state flag in GuiAutoUpdateDialogType struct
 *	11/97	COMET	Fix bug causing blank update frames
 *	 1/98	COMET	Added sanity tests for undefined keys. Added cast
 *
 ***********************************************************************/


#include <X11/X.h>

#include "winobj.h"
#include "mainwin.h"
#include "timeutil.h"
#include "gendef.h"
#include "genglobs.h"
#include "_proto.h"
#include "guimacros.h"
#include "update.h"



#define MAXINTERVAL 60
#define DEFAULTINTERVAL 5


extern Widget main_shell;

static TimeObjectType time_data;
static XtIntervalId timer_id = (XtIntervalId) NULL;
static int clockInterval;
static BooleanType on;
static BooleanType first;

void AutoUpdate();
void SetAutoUpdateTimer();
void SetAutoUpdateObject( BooleanType, int);
void SetGarpLogo (WindowObjectType *wo, char *string);
void SetAutoUpdateWidgets ( BooleanType, int );

TimeObjectType *GetTimeObject()
{
        return( &time_data );
}


/*
 *  InitAutoUpdate(): Initializes the auto update components.
 */

void
InitAutoUpdate()
{
	int		interval;
	char		*update, *timer;
	char		*key1="autoupdate";
	char		*key2="update_timer";
	BooleanType	updateOn;


/*
 *	Read in configuration parameters
 */
	update = GetConfigValue( key1 );
	if ( update == NULL ) {
            printf("Warning: key '%s' is not defined in Garp_defaults\n",
                    key1);
	    return;
	}

	timer = GetConfigValue( key2 );
	if ( timer == NULL ) {
            printf("Warning: key '%s' is not defined in Garp_defaults\n",
                    key2);
	    return;
	}

/*
 *	Check for invalid interval
 */
	interval = atoi ( timer );
	if ( interval > MAXINTERVAL || interval <= 0 )
	{
	    printf ("InitAutoUpdate: Invalid auto update interval (%d).\n", interval);
	    interval = DEFAULTINTERVAL;
	}

	updateOn = False;
	if ( *update == 'y' || *update == 'Y' ) updateOn = True;

	SetAutoUpdateWidgets ( updateOn, interval );

	SetAutoUpdateObject ( updateOn, interval );

	if ( update ) Free (update);
	if ( timer  ) Free (timer);
	
}


/*
 *  SetAutoUpdateObject(): Handles changes to the state of the Auto Update
 *  system.
 */

void
SetAutoUpdateObject( BooleanType onoff, int interval )
{

	int	newInterval;
	int	verbose;


	verbose = GetVerboseLevel();
	
	newInterval = interval * 60 * 1000;	

/*
 *	Auto update is currently off and is being turned off (do nothing)
 */
	if ( ! on  &&  ! onoff ) 
	{
	}

/*
 *	Auto update is currently on and is being turned off
 */	
	else if ( on && ! onoff )
	{
	    if ( timer_id ) XtRemoveTimeOut ( timer_id );
	}

/*
 *	Auto update is currently off and is being turned on
 */
	else if ( ! on  && onoff )
	{
	    clockInterval = newInterval;
	    first = True;
	    SetAutoUpdateTimer();
	}
/*
 *	Auto update is currently on and is being turned on. ie, the 
 *	timer interval has changed.
 */
	else if ( on && onoff )
	{
	    if ( newInterval != clockInterval )
	    {
		if ( timer_id ) XtRemoveTimeOut ( timer_id );
		clockInterval = newInterval;
	        first = True;
		SetAutoUpdateTimer();
	    }
	}

	on = onoff;

	if ( verbose > VERBOSE_0 )
	{
	    if ( on )
	    	printf("SetAutoUpdateObject: Auto update on.\n");
	    else
	    	printf("SetAutoUpdateObject: Auto update off.\n");

	    printf("SetAutoUpdateObject: Auto update interval = %d minutes\n",interval);
	}

}



void
SetAutoUpdateTimer()
{

	if ( clockInterval > 0 )
	{

	    timer_id = XtAppAddTimeOut (
				XtWidgetToApplicationContext (main_shell),
				clockInterval,
				(XtTimerCallbackProc) SetAutoUpdateTimer,
				(XtPointer) NULL );

	    if ( first ) 
		first = False;
	    else
		AutoUpdate();

	}

}


void
AutoUpdate ()
{

	char			string[] = "Auto-Updating...";
	WindowObjectType	*wo;
	MetObjectType		*mo;
	PixmapObjectType	*po;
	char			**files, **timestamps;
	int			verbose, count, nframes;
	int			i, metObjectType;
	BooleanType		wasAnimating, staticObj;
	BooleanType		newData, replotData;
	int			animationCommand;
	GuiAutoUpdateDialogType	*au;


	wo = GetActiveWindowObject ();
	au = GetGuiAutoUpdateDialog();
	verbose = GetVerboseLevel();

	if ( verbose > VERBOSE_0 )
	    	printf ("Starting AutoUpdate.\n");


/*
 *	If there are no pixmaps, do nothing
 */
	nframes = wo->numPixmapObjects;
	if ( nframes == 0 )
	{
	    if ( verbose > VERBOSE_0 )
	    	printf ("AutoUpdate: Nothing to update.\n");

	    return;
	}

	SetGarpLogo ( wo, string);
	SetupAbortProcess (wo );
 	SetAutoUpdateStateFlag( au, True);

/*
 *	Check to see if we're animating. If so, find out how. Destroy the
 *	animation. (Necessary since we might be moving frames around.)
 */
	wasAnimating = AnimationRunning(wo);
	if ( wasAnimating )
	    animationCommand = GetAnimationCommand(wo);

	AnimationObjectDestroy( wo );

/*
 *	Find the first non-static metObject in the last pixmap.
 */
	po = wo->pixmapObjects[nframes - 1];

	for ( i = 0; i < po->numMetObjects; i++ )
	{
	    mo = po->metObjects[i];
	    
	    staticObj = False;
	    metObjectType = GetMetObjectType(mo);

	    switch ( metObjectType ) {

	    case SATOBJECT:
	    case RADAROBJECT:
	    case NOWRADOBJECT:
		GetNewImages ( mo, nframes, &count, &files, &timestamps);
		AddNewImages ( wo, i, count, files, timestamps);
		break;

	    case SURFACEOBJECT:
		GetNewSurfaceObs ( mo, nframes, &count, &files, &timestamps);
		AddNewSurfaceObs ( wo, i, count, files, timestamps);
		break;

	    case UPPERAIROBJECT:
	    case UPPERAIRPROFILEOBJECT:
		GetNewUpperAirObs ( mo, nframes, &count, &files, &timestamps);
		AddNewUpperAirObs ( wo, i, count, files, timestamps);
		break;

	    case WINDPROFILEROBJECT:
		GetNewWindProfilerObs ( mo, &count, &files, &timestamps);
		AddNewWindProfilerObs ( wo, i, &count, files, timestamps);
		break;

	    case SCALARGRIDOBJECT:
	    case VECTORGRIDOBJECT:
	    case SCALARGRIDFILLOBJECT:
	    case XSECTIONGRIDOBJECT:
	    case XSECTIONGRIDFILLOBJECT:
	    case VERTICALPROFILEOBJECT:
	    case TIMEHEIGHTOBJECT:
		GetNewGrids ( mo, nframes, &count, &files, &timestamps);
		AddNewGrids ( wo, i, count, files, timestamps);
		break;

/*
 *	    We get here if we have a static object (map, etc...). Do nothing.
 */
	    default:
		staticObj = True;
		count = 0;

	    }

/*
 *	    If we found an update-able object, break out of the loop
 */
	    if ( ! staticObj ) break;
	
	}

/*
 *	Free Stuff!
 */
	StringListFree ( count, files);
	StringListFree ( count, timestamps);


/*
 *	Update existing metObjects and/or add (time-match) additional
 *	metobjects into new frames.
 */
	po = wo->pixmapObjects[0];
	replotData = False;

	for ( i = 0; i < po->numMetObjects; i++) {

	    mo = po->metObjects[i];
	    metObjectType = GetMetObjectType(mo);
	
	    switch ( metObjectType ) {

	    case SATOBJECT:
	    case RADAROBJECT:
	    case NOWRADOBJECT:
		UpdateImageObjects ( wo, i, count, &newData);
		replotData = replotData || newData;
		break;

	    case SURFACEOBJECT:
		UpdateSurfaceObjects ( wo, i, count, &newData);
		replotData = replotData || newData;
		break;

	    case UPPERAIROBJECT:
	    case UPPERAIRPROFILEOBJECT:
		UpdateUpperAirObjects ( wo, i, count, &newData);
		replotData = replotData || newData;
		break;

	    case SCALARGRIDOBJECT:
	    case VECTORGRIDOBJECT:
	    case SCALARGRIDFILLOBJECT:
	    case XSECTIONGRIDOBJECT:
	    case XSECTIONGRIDFILLOBJECT:
	    case VERTICALPROFILEOBJECT:
	    case TIMEHEIGHTOBJECT:
		UpdateGridObjects ( wo, i, count, &newData);
		replotData = replotData || newData;
		break;

	    }

	}

		
/*
 *	Replot if there are new frames or something needs replotting
 */
	if ( count > 0 || replotData )
	    			ProcessDisplayList (wo);


/*
 *	Restore things. 
 */
	CancelAbortProcess (wo );

	if ( wasAnimating ) {
	
	    switch (animationCommand) {
	    
	    	case LOOPFORWARD:
		    AnimationObjectPlayFwd(wo);
		    break;

	    	case LOOPBACKWARD:
		    AnimationObjectPlayBackward(wo);
		    break;
	    
	    	case LOOPFORWARDBACKWARD:
		    AnimationObjectPlayFwdBack(wo);
		    break;

		default:
		    break;

	    }
	
	}

 	SetAutoUpdateStateFlag( au, False);

	if ( verbose > VERBOSE_0 )
	    	printf ("End of AutoUpdate.\n");

}

