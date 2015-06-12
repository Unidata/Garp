/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	winactions.c
 *
 *	This module contains action functions called by the Xt translation
 *	manager.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 4/97	COMET	Moved status bar routines out, reworked some other
 *			routines. added new action for right clicks.
 *	11/97	COMET	Small change to account for removal of .5 factor
 *			in  getimagecoords(), added call to
 *			SetMapListChangeState
 *	11/97	COMET	Added TitleClick to make titles clickable.
 *	11/97	COMET	Look at mot->titleIndex to get correct metObject
 *			in TitleClick.
 *	01/98	COMET	Changed getimagecoords() calling sequence
 *
 ***********************************************************************/

#define WINACTIONS_GLOBAL
#include "winactions.h"
#include "guimacros.h"
#include "genglobs.h"
#include "maplist.h"
#include "_proto.h"


static unsigned long	downUpTime, downTime;
static Boolean		haveSingleClick = False;
static Boolean		buttonDownMotion = False;
static int 		curX, curY, clickX, clickY;
static int		startX, startY, endX, endY;
static int		trackingMotion = False;
static int		somethingDrawn = False;
static int		offwindow = False;
static int		minzoombox;
static int		zoomboxcolor;
static GC		xorGC, xcopyGC;
static float		anchorLat, anchorLon;
static int		anchorX, anchorY;
static Boolean		anchorSet = False;
static XtIntervalId	timerID;


#ifdef max
#undef max
#endif
#ifdef abs
#undef abs
#endif
#ifdef min
#undef min
#endif

#define max(a, b)           ((a) < (b) ? (b) : (a))
#define min(a, b)           ((a) > (b) ? (b) : (a))
#define abs(x)              ((x) >= 0 ? (x) : -(x))
#define swapvalues(a,b)	    { int xx; xx=(a);(a)=(b);(b)=xx;}


void InitMinZoomBox ( )
{
	int	verbose;
	char	*zboxstr;

	zboxstr = GetConfigValue ( "minzoomboxsize" );
	minzoombox = atoi (zboxstr );
	Free ( zboxstr );

	zboxstr = GetConfigValue ( "zoomboxcolor" );
	zoomboxcolor = atoi (zboxstr );
	Free ( zboxstr );

	verbose = GetVerboseLevel ();
	if ( verbose > VERBOSE_0 ) {
	    printf ( "Minumum zoom box size set at: %d\n",minzoombox);
	    printf ( "Zoom box color number: %d\n", zoomboxcolor);
	}
	return;

}

int GetCurrentX ( WindowObjectType *wo )
{
	return ( curX );
}


int GetCurrentY ( WindowObjectType *wo )
{
	return ( curY );
}

void SetCurrentX ( WindowObjectType *wo, int X )
{
	curX = X;
}

void SetCurrentY ( WindowObjectType *wo, int Y )
{
	curY= Y;
}

int GetCurrentAnchorX ( WindowObjectType *wo )
{
	return ( anchorX );
}


int GetCurrentAnchorY ( WindowObjectType *wo )
{
	return ( anchorY );
}


Boolean GetAnchorFlag ( WindowObjectType *wo )
{
	return ( anchorSet );
}


static NormalizeBoxPointsForX( int *x1, int *y1, int *x2, int *y2 )
{

	*x1 = max( 0, *x1 );
	*x2 = max( 0, *x2 );
	*y1 = max( 0, *y1 );
	*y2 = max( 0, *y2 );

	if( *x2 < *x1 )
		swapvalues( *x1, *x2 );

	if( *y2 < *y1 )
		swapvalues(*y1, *y2 );

}


static GC InitXorGC( Widget w )
{

	XGCValues	gcv;

	gcv.function   = GXxor;
	gcv.line_width = 3;

/*
 *	Query the gempak graphics colors. Use color #2 (index 1) which
 *	is "vanilla" and seems to work better than pure white.
 */
	gcv.foreground = GemColorToPixel( zoomboxcolor );
	gcv.background = GetGemBackgroundPixel();

	return ( XCreateGC( XtDisplay( w ), XtWindow( w ),
			GCBackground | GCForeground |
			GCLineWidth | GCFunction, &gcv ));

}


static GC InitXcopyGC( Widget w )
{

	XGCValues	gcv;

	gcv.function   = GXcopy;
	gcv.line_width = 2;

	gcv.foreground = GetGemRedPixel();
	gcv.background = GetGemBackgroundPixel();

	return (XCreateGC( XtDisplay( w ), XtWindow( w ),
			GCBackground | GCForeground |
			GCLineWidth | GCFunction, &gcv ));

}


void DrawBox( Widget w, int x1, int y1, int x2, int y2 )
{

        int		width, height;

        NormalizeBoxPointsForX( &x1, &y1, &x2, &y2 );

        width  = x2 - x1;
        height = y2 - y1;

        XDrawRectangle( XtDisplay( w ), XtWindow( w ), xorGC,
                                x1, y1, width, height );

        return;

}


void RubberBandBox ( WindowObjectType *wo )		    
{

	int	x, y;

	x = GetCurrentX (wo);
	y = GetCurrentY (wo);

/*
 *	Set some things up the first time through
 */
	if( ! trackingMotion ) {

		if( xorGC == NULL )
			xorGC = InitXorGC( GetWindowObjectCanvas(wo) );

		endX = startX = x;
		endY = startY = y;

		trackingMotion = True;
	}

/*
 *	Check to see if we need to undraw.
 */
	if( somethingDrawn )
		DrawBox ( GetWindowObjectCanvas(wo),
			  startX, startY, endX, endY );

/*
 *	Draw the new box
 */
        endX = x;
        endY = y;

        DrawBox ( GetWindowObjectCanvas(wo),
        	   startX, startY, endX, endY );
	somethingDrawn = True;

	return;

}


void EndRubberBandBox( WindowObjectType *wo )
{

	if( trackingMotion && somethingDrawn )
	DrawBox( GetWindowObjectCanvas(wo), startX, startY, endX, endY );

	trackingMotion = False;
	somethingDrawn = False;

	return;
}


void ModifyGarea( WindowObjectType *wo, char *garea )
{

	GeoRefObjectType	*geo;
	char			proj[GEMPAKSTRING];
	GuiMapBackgroundDialogType *mbg;


	geo = GetGeoRefObject( wo );

/*
 *      Set the new garea. Also, set the projection to keep the previous
 *	values in sync.
 */
        SetGeoRefObjectGarea( wo, garea );

	strcpy ( proj, geo->proj);
	SetGeoRefObjectProjection( wo, proj );

/*
 *	Set the flag in the map backgrounds struct so that maps
 *	will get recreated. 
 */
	mbg = GetGuiMapBackgroundDialog();
  	SetMapListChangeState ( mbg, True);

/*
 *      Clear all the pixmaps and refresh the display.
 */
	RefreshDisplay();

}


void ZoomBox( WindowObjectType *wo )		    
{

	char			garea[GEMPAKSTRING];
	char			Dcoords[] = "D";
	float			x1, x2, y1, y2, ix1, ix2, iy1, iy2;
	float			lat1, lon1, lat2, lon2;
	PixmapObjectType	*po;

/*
 *	Make sure that georeferencing is set
 */
	if ( ! GetGeoRefObject( wo ) ) return;

	if ( offwindow ) return;

	po = GetActivePixmapObject (wo);
	
/*
 *	Don't zoom if the box is tiny. This might be a result
 *	of a slight move during a click.
 */
	if ( abs ( startX - endX ) < minzoombox &&
	     abs ( startY - endY ) < minzoombox ) return;

/*
 *	GEMPAK needs lower left and upper right coords.
 */
	x1 =  min ( startX, endX);
	y1 =  max ( startY, endY);
	x2 =  max ( startX, endX);
	y2 =  min ( startY, endY);

/*
 *	Transform the points. Lat/lon first 
 */
	if ( getlatlon ( Dcoords, &x1, &y1, &lat1, &lon1 ) &&
	     getlatlon ( Dcoords, &x2, &y2, &lat2, &lon2 ) )

	    sprintf ( garea, "%.2f;%.2f;%.2f;%.2f", lat1, lon1, lat2, lon2); 

/*
 *	If lat/lon transform failed, try image coords (user may have
 *	selected off-earth coords).
 */
	else if ( getimagecoords ( po, Dcoords, &x1, &y1, &ix1, &iy1 ) &&
 	          getimagecoords ( po, Dcoords, &x2, &y2, &ix2, &iy2 ) )

	    sprintf ( garea, "!%d;%d;%d;%d",
	        	(int) (ix1 + .5), (int) (iy1 + .5),
	        	(int) (ix2 + .5), (int) (iy2 + .5));

/*
 *	Hopeless, bail out...
 */
 	else
	    return;

/*
 *	Change the garea
 */
	ModifyGarea ( wo, garea );

/*
 *	Reset the start and end points
 */
	startX = endX;
	startY = endY;
}


void ZoomAtCursor ( WindowObjectType *wo )		    
{

	char			garea[GEMPAKSTRING];
	char			Pcoords [] = "P", Mcoords [] = "M", Dcoords [] = "D";
	float			x, y, lat1, lat2, lon1, lon2;
	float			xl, yb, xr, yt, xdif, ydif, px, py;
	float			ix, iy, ix1, ix2, iy1, iy2;
	float			x1, x2, y1, y2;
	GeoRefObjectType	*geo;
	PixmapObjectType	*po;
	

/*
 *	Make sure we have a georeference object
 */
	if ( ! (geo = GetGeoRefObject( wo )) ) return;
	if ( geo->viewType != PLAN ) return;

	po = GetActivePixmapObject (wo);

/*
 *	Get lat/lon of the cursor point and window corners. If can't
 *	get lat/lon, try image coords.
 */
	x1 = 0;
	y2 = 0;
	x2 = (float) GetWindowObjectWidth( wo) - 1;
	y1 = (float) GetWindowObjectHeight( wo) - 1;

	x =  GetCurrentX (wo);
	y =  GetCurrentY (wo);
	
	if ( ! getbounds ( Pcoords, &xl, &yb, &xr, &yt ) ) return;
	if ( ! convertcoords ( Dcoords, Pcoords, &x, &y, &px, &py ) ) return;

	xdif = ( xr - xl ) / 4.;
	ydif = ( yt - yb ) / 4.;

	xl = px - xdif;
	xr = px + xdif;
	yb = py - ydif;
	yt = py + ydif;

	if ( getlatlon ( Pcoords, &xl, &yb, &lat1, &lon1 ) &&
	     getlatlon ( Pcoords, &xr, &yt, &lat2, &lon2 ) )

	    sprintf ( garea, "%.2f;%.2f;%.2f;%.2f", lat1, lon1, lat2, lon2);

	else if ( getimagecoords ( po, Dcoords, &x1, &y1, &ix1, &iy1 ) &&
	          getimagecoords ( po, Dcoords, &x2, &y2, &ix2, &iy2 ) &&
	          getimagecoords ( po, Dcoords, &x, &y, &ix, &iy ) ) {

	    xdif = ( ix2 - ix1 ) / 4.;
	    ydif = ( iy1 - iy2 ) / 4.;

	    ix1 = ix - xdif;
	    ix2 = ix + xdif;
	    iy1 = iy + ydif;
	    iy2 = iy - ydif;

	    sprintf ( garea, "!%d;%d;%d;%d",
	        	(int) (ix1 + .5), (int) (iy1 + .5),
	        	(int) (ix2 + .5), (int) (iy2 + .5));
	}
	else
	    return;

/*
 *	Change the garea
 */
	ModifyGarea ( wo, garea );

}


void DrawLatLonAnchor( WindowObjectType *wo )
{

	int	x, y, x1, x2, y1, y2;
	int	size = 30;
	Widget	w;

	x = GetCurrentAnchorX (wo);
	y = GetCurrentAnchorY (wo);

	w = GetWindowObjectCanvas(wo);
	
	if( xcopyGC == NULL )
		xcopyGC = InitXcopyGC( w );
	
	x1 = x - size;
	x2 = x + size;
	y1 = y2 = y;
        XDrawLine( XtDisplay( w ), XtWindow( w ), xcopyGC,
                                x1, y1, x2, y2 );
		
	x1 = x2 = x;
	y1 = y - size;
	y2 = y + size;
        XDrawLine( XtDisplay( w ), XtWindow( w ), xcopyGC,
                                x1, y1, x2, y2 );
}


void UpdateAnchorPoint( WindowObjectType *wo )
{

	char	Dcoords [] = "D", Mcoords [] = "M";
	float	x, y;

	if ( ! anchorSet ) return;

	if ( convertcoords( Mcoords, Dcoords, &anchorLat, &anchorLon, &x, &y)) {

	    anchorX = x + .5;
	    anchorY = y + .5;
	    if ( anchorX < 0 || anchorX >= (int) GetWindowObjectWidth (wo) ||
	         anchorY < 0 || anchorY >= (int) GetWindowObjectHeight (wo) ) return;
	    DrawLatLonAnchor ( wo );
	}

	UpdateStatusBar ( wo );
}


void SetAnchorPointFlag( Boolean flag )
{

	anchorSet = flag;
	
}


void SetLatLonAnchorPoint( WindowObjectType *wo )
{

	char			Dcoords [] = "D";
	Pixmap			pixmap;
	int			x, y;
	float			x1, y1, lat, lon;
	GeoRefObjectType	*geo;

/*
 *	If an anchor point is already set, (ie; visible) set the
 *	anchor flag to false and refresh the current pixmap so as
 *	to obliterate the old anchor point.
 */
	if ( anchorSet ) {
 
	    anchorSet = False;
	    WindowObjectPixmapRefresh ( wo);

	}

/*
 *	Can't set an anchor point if we aint got no PLAN view georeferencing.
 */
	if ( ! (geo = GetGeoRefObject ( wo)) ) return;
	if ( geo->viewType != PLAN ) return;

	x = GetCurrentX (wo);
	y = GetCurrentY (wo);

	x1 = x;
	y1 = y;
	
	if ( getlatlon ( Dcoords, &x1, &y1, &lat, &lon ) ) {
	    anchorSet = True;
	    anchorX = x;
	    anchorY = y;
	    anchorLat = lat;
	    anchorLon = lon;
	}
}


BooleanType
TitleClick ( WindowObjectType *wo )
{
	PixmapObjectType	*pot;
	MetObjectType		*mot;
	TitleObjectType		*tot;

	BooleanType	iret;
	int		i, j;
	int		x1, y1, x2, y2;
	int		titleIndex;

	tot = wo->titleObject;

	iret = False;
/*
 *	Loop over titles to find one that matches cursor position.
 *	The number of titles should be no more than the number of
 *	metObjects in a pixmap object.
 */
	for ( i=0; i<tot->numTitles; i++ ) {

	    pot = wo->pixmapObjects[i];

	    x1 = tot->region[i].x1;
	    y1 = tot->region[i].y1;
	    x2 = tot->region[i].x2;
	    y2 = tot->region[i].y2;

	    if ( clickX > x1 && clickY < y1 &&
	         clickX < x2 && clickY > y2 ) {
		iret = True;
		titleIndex = i;
		break;
	    }
	}

	if ( iret ) {
/*
 *	Mark title position in each pixmap.
 */
	    for ( i=0; i<wo->numPixmapObjects; i++ ) {    /* pixmaps */
	        pot = wo->pixmapObjects[i];
	
	        for ( j=0; j<pot->numMetObjects; j++ ) {  /* metObjects */
		    mot = pot->metObjects[j];

		    if ( mot->titleIndex == titleIndex ) {
		    	mot->drawnFlag = False;

/*
 *	Toggle inUse flag for this metObject.
 */
		    	if ( mot->toggledOn )
		    	    mot->toggledOn = False;
		    	else
		    	    mot->toggledOn = True;
		    }
		}
	    }
/*
 *	Clear window and process display list.
 */
	RefreshDisplay();

	}

	return iret;
}


void DoSingleClickTask ( WindowObjectType *wo)
{


	if ( ! haveSingleClick ) return;
	haveSingleClick = False;

	if ( AnimationRunning (wo) ) return;

/*
 *	Check if there was motion during a button click and remove
 *	whatever might be drawn.
 */
	if ( buttonDownMotion ) {
	    EndRubberBandBox ( wo );
	    buttonDownMotion = False;
	}

/*
 *	Check if the user clicked on a title.
 */
	if ( TitleClick ( wo ) )
	    return;

	SetLatLonAnchorPoint ( wo );

	UpdateAnchorPoint ( wo );

}


void DoCtrlClickTask (WindowObjectType *wo)
{


	if ( AnimationRunning (wo) ) return;

/*
 *	Check if there was motion during a button click and remove
 *	whatever might be drawn.
 */
	if ( buttonDownMotion ) {
	    EndRubberBandBox ( wo );
	    buttonDownMotion = False;
	}

/*
 *	Set the anchor point to false, and refresh the screen to 
 *	obliterate the cross-hair
 */
	SetAnchorPointFlag ( False );
	WindowObjectPixmapRefresh ( wo);

	UpdateStatusBar ( wo );
}



void DoDoubleClickTask (WindowObjectType *wo)
{


	if ( AnimationRunning (wo) ) return;

/*
 *	Check if there was motion during a button click and remove
 *	whatever might be drawn.
 */
	if ( buttonDownMotion ) {
	    EndRubberBandBox ( wo );
	    buttonDownMotion = False;
	}

	ZoomAtCursor ( wo );

}


void DoButtonDownMotionTask (WindowObjectType *wo)
{

	GeoRefObjectType	*geo;


	if ( AnimationRunning (wo) ) return;

/*
 *	For now, only allow this to work for plan views
 */	
	if ( ! (geo = GetGeoRefObject( wo ))) return;
	if ( geo->viewType != PLAN ) return;

	RubberBandBox ( wo );

	UpdateStatusBar ( wo );

	buttonDownMotion = True;

}


void DoEndButtonDownMotionTask (WindowObjectType *wo)
{
	
	
	EndRubberBandBox ( wo );

	ZoomBox ( wo );

	buttonDownMotion = False;

}


void DoPointerMotionTask (WindowObjectType *wo)
{

	
	UpdateStatusBar ( wo );

}


void HandleBtn1Clicks(	Widget w,
			XEvent *event,
			String *parms,
			Cardinal *nparms )	    
{

	WindowObjectType	*wo;

	XButtonEvent *xb = (XButtonEvent *) event;

	wo = GetActiveWindowObject();

/*
 *	Handle the "down" part of a mouse click. Just need to note
 *	the time for this one.
 */
	if ( strcmp ( parms[0], "down" ) == 0 )

	    downTime = xb->time;

/*
 *	Handle the "up" portion of the click. This is where we start
 *	making decisions.
 */
	else if ( strcmp ( parms[0], "up" ) == 0 ) {
	
	    downUpTime = xb->time - downTime;

	    clickX = xb->x;
	    clickY = xb->y;
	
/*
 *	    Check for the end of a "slow" click. If no motion has been
 *	    detected during the click, perform the single click task,
 *	    otherwise the end of button down motion task.
 */
	    if ( downUpTime > DOWNUPMAXTIME ) {
		if ( ! buttonDownMotion ) {
		    haveSingleClick = True;
		    DoSingleClickTask (wo);
		}
		else
		    DoEndButtonDownMotionTask (wo);
		return;
	    }

/*
 *	    If we have already detected a single click, then this is a
 *	    double click. Set the single click flag to false, and remove
 *	    the single click function from the Xt timer (possibly redundant).
 */
	    if ( haveSingleClick ) {
	    
		haveSingleClick = False;
		XtRemoveTimeOut (timerID);
		DoDoubleClickTask (wo);
		
	    }
	    else {

/*
 *	        At the moment this appears to be a single click, so register
 *	        the single click routine with the Xt timer to fire off in a
 *	        short time (DOUBLECLICKTIMEOUT)
 */
		haveSingleClick = True;
		timerID = XtAppAddTimeOut (XtWidgetToApplicationContext ( w ),
					   DOUBLECLICKTIMEOUT,
					   (XtTimerCallbackProc) DoSingleClickTask,
					   (XtPointer) wo );
	    }
	}

}


void HandleBtn1DownMotion(	Widget w,
				XEvent *event,
				String *parms,
				Cardinal *nparms )	    
{

	WindowObjectType	*wo;

	XButtonEvent *xb = (XButtonEvent *) event;

/*
 *	Check the valid range of this point. It's possible to get
 *	"off-window" values for x and y. Ick.
 */	
	wo = GetActiveWindowObject();

	if ( xb->x >= 0 && xb->x < GetWindowObjectWidth  (wo) &&  
	     xb->y >= 0 && xb->y < GetWindowObjectHeight (wo) ) {

	    curX = xb->x;
	    curY = xb->y;

	    DoButtonDownMotionTask (wo);	    
	    offwindow = False;

	}
	else
	    offwindow = True;

}


void HandlePointerMotion(	Widget w,
				XEvent *event,
				String *parms,
				Cardinal *nparms )	    
{
	WindowObjectType	*wo;

	XButtonEvent	*xb = (XButtonEvent *) event;

/*
 *	Check the valid range of this point.
 */	
	wo = GetActiveWindowObject();

	if ( xb->x >= 0 && xb->x < GetWindowObjectWidth  (wo) &&  
	     xb->y >= 0 && xb->y < GetWindowObjectHeight (wo) ) {

	    curX = xb->x;
	    curY = xb->y;
	    offwindow = False;

	    DoPointerMotionTask (wo);

	}
	else
	    offwindow = True;

}


void HandleCtrlBtn1Click(	Widget w,
				XEvent *event,
				String *parms,
				Cardinal *nparms )	    
{
	WindowObjectType	*wo;
	
	wo = GetActiveWindowObject();

	DoCtrlClickTask(wo);
}


void HandleBtn3Clicks(		Widget w,
				XEvent *event,
				String *parms,
				Cardinal *nparms )	    
{

	extern Widget general_pref_dialog, sb_pref_dialog;

	if ( strcmp ( parms[0], "canvas" ) == 0 )

		XtManageChild ( general_pref_dialog );

	else if ( strcmp ( parms[0], "statusbar" ) == 0 )

		XtManageChild ( sb_pref_dialog );
	

	
}



/*
 *	Action functions that control animation capabilities from
 *	the keyboard
 */

void AnimPlayToggle(	Widget  w,
			XEvent *event, 
			String *parms,
			Cardinal *nparms )
{
	WindowObjectType	*wo;
	
	wo = GetActiveWindowObject();

	if ( AnimationRunning (wo) )
	    AnimationObjectStop (wo);
	else
	    AnimationObjectPlayFwd (wo);

}


void AnimFwd(		Widget  w,
			XEvent *event, 
			String *parms,
			Cardinal *nparms )
{
	WindowObjectType	*wo;
	
	wo = GetActiveWindowObject();

	if ( AnimationRunning (wo) )
	    AnimationObjectPlayFwd (wo);
	else
	    AnimationObjectStepFwd (wo);
}


void AnimBack(		Widget  w,
			XEvent *event, 
			String *parms,
			Cardinal *nparms )
{
	WindowObjectType	*wo;
	
	wo = GetActiveWindowObject();

	if ( AnimationRunning (wo) )
	    AnimationObjectPlayBackward (wo);
	else
	    AnimationObjectStepBack (wo);
}


void AnimRockToggle(	Widget  w,
			XEvent *event, 
			String *parms,
			Cardinal *nparms )
{
	WindowObjectType	*wo;
	
	wo = GetActiveWindowObject();

	if ( AnimationRunning (wo) &
	     GetAnimationCommand (wo) == LOOPFORWARDBACKWARD )
	    AnimationObjectStop (wo);
	else
	    AnimationObjectPlayFwdBack(wo );
}

/*
 *	QueryPointerPosition(): utility routine for getting the current
 *	pointer position. Returns True and the coordinates if pointer
 *	is in the Garp window.
 */


BooleanType QueryPointerPosition ( WindowObjectType *wo, int *X, int *Y )
{

	Display		*d;
	Window		w, rootw, childw;
	int		rx, ry, wx, wy;
	unsigned int	mask;
	BooleanType	onscreen;
	
	
	d = GetWindowObjectDisplay ( wo );
	w = GetWindowObjectWindowID ( wo );

	onscreen = XQueryPointer ( d, w, &rootw, &childw, &rx, &ry, &wx, &wy, &mask );

	if ( onscreen &&
	     wx >= 0 && wx < GetWindowObjectWidth  ( wo ) &&
	     wy >= 0 && wy < GetWindowObjectHeight ( wo ) ) {
	    *X = wx;
	    *Y = wy;
	    return (True);
	}
	
	else
	    return (False);
	    
}

void ToggleCursorTracking ()
{



}


