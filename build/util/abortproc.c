/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	abortproc.c
 *
 *	Handles user cancellations or interupts.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 6/97	COMET	Fixed bug removing icon bar
 *	 8/97	COMET	Removed SetGarpLogoBusy
 *	10/97	COMET	Added call to ResetGarpLogo() in CancelAbortProcess()
 *	 1/98	COMET	Added Free for fullIconname (not used though)
 *	 2/98	COMET	Added ABORTtype's
 *
 ***********************************************************************/

#include <X11/cursorfont.h>

#include "winobj.h"
#include "mainwin.h"
#include "_proto.h"

static int       setAbortCount = 0;
static void      SetNormalCursor( WindowObjectType *wo );
static void      SetBusyCursor( WindowObjectType *wo );


static Cursor GetNormalCursor( WindowObjectType *wo );
static void SetNormalCursor( WindowObjectType *wo );
static void SetBusyCursor( WindowObjectType *wo );

void	    SetGarpLogo (WindowObjectType *wo, char *string);
void	    ResetGarpLogo (WindowObjectType *wo);
void        CancelAbortProcess( WindowObjectType *wo, BooleanType forceCancel );
void        SetCrossHairCursor( WindowObjectType *wo );
int         SetCursor( Display *display, Window window, Cursor cursor, XColor *fg, XColor *bg );
void        ConsumeXEvents( Widget w, int flushAll );
void        SetIconInButton( Widget w, char *iconName );
int         GetWindowObjectAbortFlag( WindowObjectType *wo);
void        SetupAbortProcess( WindowObjectType *wo );
BooleanType CheckForInterrupt( Widget w );



/*
 *	Name of a couple of the icons.
 */
#define CLEAR_ICON	"iconclr.xbm"
#define STOP_ICON	"stop.xbm"


/*
 *
 *	Check for abort and set the abort value.  Returns ABORTNONE,
 *	ABORTCLEAR or ABORTRESET for no abort, abort on the clear
 *	button, and abort on the reset button, respectively.
 *
 */
int GetWindowObjectAbortFlag( WindowObjectType *wo)      
{
	GuiMainWinDialogType	*mwn;
	mwn = GetGuiMainWinDialog();

	GetGuiMainAppShellW ( mwn );

	if ( CheckForInterrupt( GetGuiClearIconW( mwn ) ))
	    wo->abortFlag = ABORTCLEAR;
	else if ( CheckForInterrupt( GetGuiResetIconW( mwn ) ))
	    wo->abortFlag = ABORTRESET;
	else 
	    wo->abortFlag = ABORTNONE;


	return( wo->abortFlag );
}

/*
 *	Initiate the abort process:  
 *		Count how many times this is called (to handle nested calls).
 *		Create busy cursor.
 *		Set the no-resizeshell resource (doesn't work).
 *		Desensitize the user-interface  (doesn't work right)
 *		return
 */
void SetupAbortProcess( WindowObjectType *wo )
{

	GuiMainWinDialogType	*mwn;
	char			string[] = "Processing...";


	mwn = GetGuiMainWinDialog();

	SetWindowObjectAbortFlag(wo, False );

	setAbortCount++;

	XtVaSetValues( GetGuiMainAppShellW(mwn),  
			XtNallowShellResize, False, NULL );

	SetBusyCursor( wo );

#ifdef CANT_GET_IT_WORKING
	SetIconInButton( GetGuiClearIconW(mwn), STOP_ICON );
#endif

	return;
}

/*
 *	Check the abortCount flag to determine how many setupabortprocess's
 *	were called.  If the count get's to Zero, then undo setupabort's work.
 *
 */
void CancelAbortProcess( WindowObjectType *wo, BooleanType forceCancel )
{
	Display *display;

	GuiMainWinDialogType	*mwn;


	if( setAbortCount <= 0 )
		return;

	if( forceCancel )
		setAbortCount = 0;
	else
		--setAbortCount;

	if ( setAbortCount != 0 ) return;


	mwn = GetGuiMainWinDialog();
	
	display = GetWindowObjectDisplay( wo );

#ifdef CANT_GET_IT_WORKING
	SetIconInButton( GetGuiClearIconW(mwn), CLEAR_ICON );
#endif

	XtVaSetValues( GetGuiMainAppShellW(mwn),  
			XtNallowShellResize, False, NULL );

	SetWindowObjectAbortFlag(wo, False );

	SetNormalCursor( wo );
	
	ResetGarpLogo ( wo );

	ConsumeXEvents( GetWindowObjectCanvas( wo ), FALSE );

	return;
}


/*
 *	Broke this out into a subroutine because it is called from
 *	so many places.
 */
static Cursor GetNormalCursor( WindowObjectType *wo )
{

	static Cursor		normalCursor = ( Cursor ) NULL;
	Display			*display = GetWindowObjectDisplay( wo );

/*
 *      Create a red arrow for the cursor.
 */
	if( ! normalCursor )
        	normalCursor = XCreateFontCursor ( display, XC_top_left_arrow );

	return( normalCursor );


}

/* XC_pirate */
static void SetNormalCursor( WindowObjectType *wo )
{
	Cursor			normalCursor;
	XColor			redCursor;

	Display			*display;
	Window			shellWindowID;
	Window			canvasWindowID;
	Window			clearWindowID;
	Window			resetWindowID;
	GuiMainWinDialogType	*mwn;


	mwn		= GetGuiMainWinDialog();
	display        = GetWindowObjectDisplay( wo );
	canvasWindowID = GetWindowObjectWindowID( wo ); 
	shellWindowID  = XtWindow( GetGuiMainAppShellW(mwn) );
	clearWindowID  = XtWindow( GetGuiClearIconW(mwn) );
	resetWindowID  = XtWindow( GetGuiResetIconW(mwn) );


	normalCursor = GetNormalCursor( wo );

	PixelToXcolor( GetGemRedPixel(), &redCursor );
	
	SetCursor(display, resetWindowID, normalCursor, &redCursor, &redCursor);
	SetCursor(display, clearWindowID, normalCursor, &redCursor, &redCursor);
	SetCursor(display, shellWindowID, normalCursor, &redCursor, &redCursor);
	SetCursor(display, canvasWindowID, normalCursor, &redCursor, &redCursor);

/*
 *	Make sure it get's out on the screen.
 */
	XFlush( display );

	return;

}


/*
 *
 *	Set the cursor to a crosshair cursor for the drawing
 *	area only.
 *
 */
void SetCrossHairCursor( WindowObjectType *wo )
{

	static Cursor		crosshairCursor;
	static XColor		cursorForeground;
	static XColor		canvasCursorBackground;
	static int		first = 1;

	Pixel			shellWidgetBackground;
	Display			*display;
	Window			canvasWindowID;


	display        = GetWindowObjectDisplay( wo );
	canvasWindowID = GetWindowObjectWindowID( wo );

	if( first ) {
/*
 *      	Create a x-hair cursor.
 */
        	crosshairCursor = XCreateFontCursor ( display, XC_crosshair );

		PixelToXcolor( GetGemRedPixel(), &cursorForeground );

		PixelToXcolor( GetGemBackgroundPixel(),
				&canvasCursorBackground );

		first = 0;
	}


	SetCursor( display, canvasWindowID, crosshairCursor, 
			&cursorForeground, &canvasCursorBackground );

	XFlush( display );

	return;

}



SetCursor( Display *display, Window window, 
		Cursor cursor, XColor *fg, XColor *bg )

{

        XDefineCursor ( display, window, cursor );
       	XRecolorCursor ( display, cursor, fg, bg );

}
	

static void SetBusyCursor( WindowObjectType *wo )
{
	Cursor			normalCursor;

	static Cursor		busyCursor;
	static Cursor		shellBusyCursor;
	static Cursor		pirateCursor;
	static XColor		cursorForeground;
	static XColor		shellCursorBackground;
	static XColor		canvasCursorBackground;
	Pixel			shellWidgetBackground;
	static int		first = 1;
	Display			*display;
	Window			shellWindowID;
	Window			canvasWindowID;
	GuiMainWinDialogType	*mwn;


	mwn		= GetGuiMainWinDialog();
	display		= GetWindowObjectDisplay( wo );
	canvasWindowID	= GetWindowObjectWindowID( wo );
	shellWindowID	=  XtWindow( GetGuiMainAppShellW(mwn) );

/*
 *
 *	Cache these values so that we don't have to retreive
 *	them everytime.
 *
 */
	if( first ) {

/*
 *
 *      	Create the busy cursor.  Must make one for the drawing
 *		area and the main shell widget.
 *
 *
 */
        	busyCursor      = XCreateFontCursor ( display, XC_watch );
        	shellBusyCursor = XCreateFontCursor ( display, XC_watch );
		pirateCursor    = XCreateFontCursor( display, XC_pirate );

/*
 *		Set the foreground color to red.  This is the main
 *		color of the cursor.
 */
		PixelToXcolor( GetGemRedPixel(), &cursorForeground );

/*
 *		Get the gempak background color for the drawing area.
 */
		PixelToXcolor( GetGemBackgroundPixel(),
				&canvasCursorBackground );

/*
 *		Get the background color the "shell".  This will
 *		be used when the cursor goes into the menu areas
 *		of the application.
 */
		XtVaGetValues( GetGuiMainAppShellW(mwn), 
			XmNbackground, &shellWidgetBackground, NULL );

/*
 *		Convert the pixel value to a XColor structure.
 */
		PixelToXcolor( shellWidgetBackground, &shellCursorBackground );

/*
 *		Only do this once.
 */
		first = 0;
	}

	
/*
 *	Make sure that the Interrupt button has different cursor.
 */

	SetCursor( display, XtWindow( GetGuiResetIconW(mwn)), pirateCursor, 
			&cursorForeground, &shellCursorBackground );

	SetCursor( display, XtWindow( GetGuiClearIconW(mwn)), pirateCursor, 
			&cursorForeground, &shellCursorBackground );
	
/*
 *	Create the cursor and set it's foreground and background color.
 */
	SetCursor( display, canvasWindowID, busyCursor, 
			&cursorForeground, &canvasCursorBackground );

	
	SetCursor( display, shellWindowID, shellBusyCursor, 
			&cursorForeground, &shellCursorBackground );

	XFlush( display );

	return;

}


void ConsumeXEvents( Widget w, int flushAll )
{

	Display		*display = XtDisplay( w );
	Window		win;
	XEvent		event;

/* 
 *	Process and dispose of all button and keyboard events 
 *	on the X event queue (for the entire connection or
 *	just a single widget).  
 */
	if( flushAll ) {

		while( XCheckMaskEvent(display,
			ButtonPressMask | ButtonReleaseMask | 
			ButtonMotionMask | PointerMotionMask | 
			KeyPressMask | LeaveWindowMask | EnterWindowMask, 
			&event) ) 

			/*  Do nothing */ ;

		return;

	}

/*
 *	Kludge to work around an apparent bug in the HP X server.
 *	Under certain circumstances, the  code that implements 
 *	the dropdown labels for buttons get's a EnterNotify event, 
 *	no LeaveNotify event.  This happens when the you pull down
 *	one of the menus (located directly above the buttons) and 
 *	the choice that is clicked on leaves the cursor inside the
 *	border of the one of the buttons.  Then, if you move the
 *	cursor immediately outside of the button.  The result is
 *	that the label dropdown text pop's up even though the cursor
 *	is outside of the button.  It won't pop back down until
 *	the cursor goes into another button.
 *
 */
	while( XCheckMaskEvent(display, LeaveWindowMask | EnterWindowMask, 
		&event) ) 
			/*  Do nothing */ ;

	if( w ) {

		win = XtWindow( w );

		while( XCheckWindowEvent( display, win, 
			ButtonPressMask | ButtonReleaseMask | 
			ButtonMotionMask | PointerMotionMask | 
			KeyPressMask | LeaveWindowMask | EnterWindowMask, 
			&event) ) 

			/*  Do nothing */;
	}
				
	return;
}


BooleanType CheckForInterrupt( Widget w )
{
	Display *dpy = XtDisplay(w);
	Window win  = XtWindow(w);
	XEvent event;


/* 
 *	Make sure all our requests get to the server 
 */
	XFlush(dpy);

/* 
 *	Let motif process all pending exposure events for us. 
 */
	XmUpdateDisplay(w);

/* 
 *
 *	Check the event loop for events in the dialog ("Stop"?) 
 *
 */
	if( XCheckWindowEvent( dpy, win, 
		ButtonPressMask | ButtonReleaseMask | 
		ButtonMotionMask | PointerMotionMask | 
		KeyPressMask,  &event) ) 

		return( True );

	return( False );

}


void SetIconInButton( Widget w, char *iconName )
{

	Pixel		fg;
	Pixel		bg;
	char		*fullIconName;

	XtVaGetValues( w, XmNforeground, &fg,
			XmNbackground, &bg,
			NULL );

	/* fullIconName = (char *) GetIcon( iconName ); */
	fullIconName = (char *) GetIcon( "icon05.xbm" );
	if( ! fullIconName  ) {
		printf("Error getting icon name %s\n", iconName);
		return;
	}

	XtVaSetValues( w, XmNlabelPixmap,  
			XmGetPixmap( XtScreen(w), fullIconName, fg, bg ),
			NULL );

	XmUpdateDisplay( w );
	Free ( fullIconName );
}


