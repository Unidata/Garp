/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	time_cb.c
 *
 *	Handles time widget callbacks and utilities.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	11/97	COMET	Fixed small leak
 *	12/99	COMET	Changed time_t data type to AbsTime.
 *
 ***********************************************************************/


#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <stdio.h>
#include <Xm/PushB.h>
#include <Xm/Scale.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>

#include <sys/types.h>
#include <time.h>

#include "guimacros.h"
#include "timeutil.h"
#include "genglobs.h"
#include "timedef.h"
#include "winobj.h"
#include "_proto.h"

/* --- Global widgets --- */

extern Widget	statusbar_label;


/* --- Static globals --- */

static AbsTime	initial_time;        /* Time when callback is called       */
static AbsTime	picked_time;         /* Time set by user                   */
#if 0
static AbsTime	tz_time;             /* Seconds between local and GMT time */
#endif
static AbsTime	stop_time;	     /* Time when STOP button was pressed  */
static AbsTime	start_time;          /* Time when START button was pressed */
static int	clock_stopped_offset;/* Time between clock stop and start  */
static int	user_set_clock;	     /* Flag when user used time widget    */

static int 	time_choice;
static Widget	month_b, day_b, hour_b, min_b, sec_b, year_b, tz_b;
static Widget	arrow_l, arrow_r;

static XtIntervalId	arrow_timer_id;
static XtIntervalId	clock_timer_id;
static Widget		save_w;
static int		save_incr;

/* Prototypes */
time_t present_time ();
void UpdateStatusBar( WindowObjectType *wo );
Widget GetTopShell ( Widget w );

#define ONEHOUR ( 3600 )
#define THREEHOUR ( 10800 )
#define SIXHOUR ( 21600 )
#define TWELVEHOUR ( 43200 )

void time_cb  (Widget w, XtPointer client_data, XtPointer xt_call_data )

/************************************************************************
* time_cb								*
*									*
* This routine is the callback for the time menu.			*
*									*
* Input parameters:							*
*	w		Widget		Input widget			*
*	client_data	XtPointer	Item number for the menu	*
*	xt_call_data	XtPointer	Callback data			*
*									*
***********************************************************************/
{
	struct tm	*timeptr;
	AbsTime		time_offset;
	int		clock_is_running;
	int		which;
	int		iret;

/*---------------------------------------------------------------------*/

        XmPushButtonCallbackStruct *call_data =
	                    (XmPushButtonCallbackStruct *) xt_call_data;
	which = (int) client_data;


	if( GetVerboseLevel() > VERBOSE_1 )
		printf("time_cb:  option = %d\n", which );

	iret = NORMAL;

/*
 *	Toggle setting time flag to turn off running clock.
 */
	put_int ( "setting_time_flag", TRUE, &iret );
	get_int ( "clock_is_running", &clock_is_running, &iret );
	get_time ( "time_offset", &time_offset, &iret );

/*
 *	Get the current time in seconds. Save it.
 */
	initial_time = present_time();

#if 0 /* himes test */
	tz_time = get_tz ( &iret );
	systime ( time_offset, &initial_time, &iret );
#endif

	picked_time = initial_time;

/*
 *	Set the time.
 */
	switch ( which ) {
	  case REALTIME:			/* Today		*/
	    put_time ( "time_offset", 0, &iret );
	    put_int ( "setting_time_flag", FALSE, &iret );
	    break;

	  case BACK_1HR:			/* Back 1 hr		*/
	    time_offset += ONEHOUR;
	    put_time ( "time_offset", time_offset, &iret );
	    put_int ( "setting_time_flag", FALSE, &iret );
	    break;

	  case BACK_3HR:			/* Back 3 hrs		*/
	    time_offset += THREEHOUR;
	    put_time ( "time_offset", time_offset, &iret );
	    put_int ( "setting_time_flag", FALSE, &iret );
	    break;

	  case BACK_6HR:			/* Back 6 hrs		*/
	    time_offset += SIXHOUR;
	    put_time ( "time_offset", time_offset, &iret );
	    put_int ( "setting_time_flag", FALSE, &iret );
	    break;

	  case BACK_12HR:			/* Back 12 hrs		*/
	    time_offset += TWELVEHOUR;
	    put_time ( "time_offset", time_offset, &iret );
	    put_int ( "setting_time_flag", FALSE, &iret );
	    break;

	  case BACK_24HR:			/* Back 24 hrs		*/
	    time_offset += SECONDS_PER_DAY;
	    put_time ( "time_offset", time_offset, &iret );
	    put_int ( "setting_time_flag", FALSE, &iret );
	    break;

	  case FORWARD_1HR:			/* Forward 1 hr	*/
	    time_offset -= ONEHOUR;
	    put_time ( "time_offset", time_offset, &iret );
	    put_int ( "setting_time_flag", FALSE, &iret );
	    break;

	  case FORWARD_3HR:			/* Forward 3 hrs	*/
	    time_offset -= THREEHOUR;
	    put_time ( "time_offset", time_offset, &iret );
	    put_int ( "setting_time_flag", FALSE, &iret );
	    break;

	  case FORWARD_6HR:			/* Forward 6 hrs	*/
	    time_offset -= SIXHOUR;
	    put_time ( "time_offset", time_offset, &iret );
	    put_int ( "setting_time_flag", FALSE, &iret );
	    break;

	  case FORWARD_12HR:			/* Forward 12 hrs	*/
	    time_offset -= TWELVEHOUR;
	    put_time ( "time_offset", time_offset, &iret );
	    put_int ( "setting_time_flag", FALSE, &iret );
	    break;

	  case FORWARD_24HR:			/* Forward 24 hrs	*/
	    time_offset -= SECONDS_PER_DAY;
	    put_time ( "time_offset", time_offset, &iret );
	    put_int ( "setting_time_flag", FALSE, &iret );
	    break;

	  case SETCLOCK:			/* User Define		*/

	    user_set_clock = TRUE;
	    XtRemoveTimeOut ( clock_timer_id );
/*
 *	    Use previous time if clock is stopped.
 */
            if ( clock_is_running == FALSE && stop_time > 0 ) {
            	initial_time = stop_time;
		timeptr = gmtime ( &initial_time );
		put_timeptr ( "timeptr", timeptr, &iret );
	    }

/*
 *	    Define widgets.
 */
	    build_clock ( w, &iret );

	    break;

	case STOPCLOCK:
/*
 *	    If clock is already stopped, do nothing.
 */
	    if ( clock_is_running == TRUE ) {
		stop_time = initial_time;	    /* Initialize stop time */
	        put_int ( "clock_is_running", FALSE, &iret ); /* Stop clock */
		user_set_clock = FALSE;		              /* Reset      */
	    }
	    break;
	case STARTCLOCK:
/*
 *	    If clock is already going, do nothing.
 */
	    if ( clock_is_running == FALSE ) {
		start_time = initial_time;	/* Initialize start time */
		clock_stopped_offset = start_time - stop_time;

/*		If the user defined a new time with the clock widget,
 *		clock_stopped_offset is no longer valid or relevant.
 */
		if ( user_set_clock == FALSE ) {
	            time_offset += + clock_stopped_offset;
		    put_time ( "time_offset", time_offset, &iret );
		}

		clock_stopped_offset = 0;	/* Reset                 */
		user_set_clock = FALSE;		/* Reset                 */
		                                /* Finished setting time */
	        put_int ( "setting_time_flag", FALSE, &iret );
	        put_int ( "clock_is_running", TRUE, &iret );  /* Restart */
	    }
/*
 *	    No changes but restart the clock.
 */
	    else {
		put_int ( "setting_time_flag", FALSE, &iret );
	    }
	    break;
	default:
	    printf("Case %d is undefined\n", which );
	    break;
	}

}




static void
change_time ( w, choice, call_data )
	Widget			w;
	int			choice;
	XmAnyCallbackStruct	*call_data;

/************************************************************************
* change_time								*
*									*
* This callback function sets the currently active time button.		*
*									*
* Input parameters:							*
*	w		Widget			Input widget		*
*	choice		int			Current time button	*
*	*call_data	XmAnyCallbackStruct	( not used )		*
*									*
* Output parameters:							*
*	none								*
*									*
************************************************************************/
{

/*---------------------------------------------------------------------*/

/*
 *	Set the current active time button.
 */
	time_choice = choice;
}


time_t present_time ()

/************************************************************************
* present_time								*
*									*
* This function returns the current time in integer format, accounting	*
* for whether or not the clock is ticking.				*
*									*
* Input parameters:							*
*									*
* Output parameters:							*
*	present_time	AbsTime		Time in integer format.		*
*									*
************************************************************************/
{
	static AbsTime	lt;
	static AbsTime	save_lt = 0;
	int		setting_time_flag;
	int		clock_is_running;
	AbsTime		time_offset;
	time_t		lclock;
	int		iret;

/*-------------------------------------------------------------*/

/*
 *	Retrieve saved variables.
 */
	get_int ( "setting_time_flag", &setting_time_flag, &iret );
	get_time ( "time_offset", &time_offset, &iret );
	get_int ( "clock_is_running", &clock_is_running, &iret );


/*
 *	Get an initial time to start from.  This is important
 *	when the application comes up, but clock is off.
 */
	if( save_lt == 0 )
	    {
	    (void)time(&lclock);
	    lt = save_lt = (AbsTime)lclock;
	    }

/*
 *	Do not update clock while setting new time.
 */
	if ( setting_time_flag == FALSE ) { 
/*
 *	    Update running clock.
 */
	    if ( clock_is_running == TRUE ) {

		(void)time(&lclock);
		save_lt = (AbsTime)lclock;
		lt = save_lt - time_offset;

	    } else {

		/*
 		 *  Update stopped clock.
 		 */
		lt = save_lt - time_offset;
	    }
	}

	lclock = (time_t)lt;
	return(lclock);
}

static BooleanType
check_time ( AbsTime time_offset )

/************************************************************************
* check_time								*
*									*
* This function compares the time picked by user with any time		*
* constraints for the begin and end times which were set at startup.	*
*									*
* Input parameters:							*
*	lt	AbsTime		Set time in integer format.		*
*									*
* Output parameters:							*
*	none								*
*									*
************************************************************************/
{

	TimeObjectType		*td;
	BooleanType		error=FALSE;
	char			begin_time[12], end_time[12];
	char			error_str[256];
	AbsTime			clock_time, begintm, endtm;

/*---------------------------------------------------------------------*/

/*
 *	Get the begin and end times.
 */
	td = GetTimeObject();
	strcpy ( begin_time, GetCaseBeginTime ( td ) );
	strcpy ( end_time, GetCaseEndTime ( td ) );

/*
 *	Put clock time in integer format.
 */
	clock_time = present_time();
	clock_time -= time_offset;

/*
 *	Convert begin and end times to integer format.
 */
	begintm = ch_to_int_time ( begin_time );
	if ( begintm > 0 ) {
	    if ( clock_time < begintm ) error = TRUE;
	    sprintf ( error_str, "You cannot set the time before %s", begin_time );
	}

	endtm = ch_to_int_time ( end_time );
	if ( endtm > 0 ) {
	    if ( clock_time > endtm ) error = TRUE;
	    sprintf ( error_str, "You cannot set the time after %s", end_time );
	}

	if ( error ) {
	    if ( begintm > 0 && endtm > 0 )
		sprintf ( error_str,
		  "You must set the time between %s and %s",
		  begin_time, end_time );

	    printf( "%s\n", error_str );
	}

	return error;
}



static void
reset_time_cb ( w, client_data, call_data )
	Widget			w;
	XtPointer		client_data;
	XmAnyCallbackStruct	*call_data;

/************************************************************************
* reset_time_cb								*
*									*
* This callback function resets the clock to local time.		*
*									*
* Input parameters:							*
*	w		Widget			Input widget		*
*	client_data	XtPointer		( not used )		*
*	*call_data	XmAnyCallbackStruct	( not used )		*
*									*
* Output parameters:							*
*	none								*
*									*
************************************************************************/
{
	AbsTime	time_offset;
	AbsTime	int_time;
	int	iret;

/*---------------------------------------------------------------------*/

	iret = NORMAL;

/*
 *	Retrieve time offset.
 */
	get_time ( "time_offset", &time_offset, &iret );

/*
 *	Get the current system time in seconds.
 */
	systime ( time_offset, &int_time, &iret );
	picked_time = int_time;

/*
 *	Update the current system time.
 */
	update_time ( month_b, day_b, hour_b, min_b, sec_b,
	              year_b, tz_b );

/*
 *	Restart ticking clock setting time label widget.
 */
	current_time ( statusbar_label);
}




static void
set_time_cb ( w, client_data, call_data )
	Widget			w;
	XtPointer		client_data;
	XmAnyCallbackStruct	*call_data;

/************************************************************************
* set_time_cb								*
*									*
* This callback function resets the clock to local time.		*
*									*
* Input parameters:							*
*	w		Widget			Input widget		*
*	client_data	XtPointer		( not used )		*
*	*call_data	XmAnyCallbackStruct	( not used )		*
*									*
* Output parameters:							*
*	none								*
*									*
************************************************************************/
{
	AbsTime	lt;
	AbsTime	widget_time;
	AbsTime	time_offset;
	int	clock_is_running;
	int	iret;

/*---------------------------------------------------------------------*/

/*
 *	Retrieve saved variables.
 */
	get_int ( "clock_is_running", &clock_is_running, &iret );
	get_time ( "time_offset", &time_offset, &iret );

/*
 *	Widget time is the time interval starting at the last time that
 *	was picked and ending when time/date was applied.
 */
        lt = time ( '\0' ); 
	widget_time = lt - initial_time - time_offset;

/*
 *	If the clock is stopped while user sets time, widget_time is not
 *	relevant so zero it out.
 */
	if ( clock_is_running == FALSE ) widget_time = 0;

/*
 *	Compute the time offset from system time.
 */
	time_offset = time_offset + initial_time - 
	                                    picked_time + widget_time;
/*
 *	Check to see if the time set by user is valid.
 */
	if ( ! check_time ( time_offset ) ) {
/*
 *	    Save time offset.
 */
	    put_time ( "time_offset", time_offset, &iret );

/*
 *	    Update widget strings so time is accurate if clock is running.
 */
	    if ( clock_is_running == TRUE )
	        update_time ( month_b, day_b, hour_b, min_b, sec_b,
	                      year_b, tz_b );
	}

/*
 *	Toggle off reset time flag to restart clock.
 */
	put_int ( "setting_time_flag", FALSE, &iret );

/*
 *	Restart ticking clock setting time label widget.
 */
	current_time ( statusbar_label);
} 


static void
cancel_time ( int *iret )

/************************************************************************
* cancel_time								*
*									*
* This function sets the time widget had never been called.		*
*									*
* Input parameters:							*
*	w		Widget			Widget which initiated	*
*						callback.		*
*	client_data	XtPointer		pointer			*
*	*call_data	XmAnyCallbackStruct	Callback structure	*
*									*
* Output parameters:							*
*	none								*
*									*
************************************************************************/
{

/*---------------------------------------------------------------------*/

/*
 *	Restart clock.
 */
	put_int ( "setting_time_flag", FALSE, iret );

/*
 *	Restart ticking clock setting time label widget.
 */

	current_time (statusbar_label);
}


static void
cancel_cb ( w, client_data, call_data )
        Widget                  w;
        XtPointer               client_data;
        XmAnyCallbackStruct     *call_data;

/************************************************************************
* cancel_cb								*
*									*
* This function exits the time widget with the time set as if the time	*
* widget had never been called.						*
*									*
* Input parameters:							*
*	w		Widget			Widget which initiated	*
*						callback.		*
*	client_data	XtPointer		pointer			*
*	*call_data	XmAnyCallbackStruct	Callback structure	*
*									*
* Output parameters:							*
*	none								*
*									*
************************************************************************/
{
	int	iret;

/*---------------------------------------------------------------------*/

/*
 *	Restart clock with time reset.
 */

	cancel_time ( &iret );
}



void
 current_time ( Widget w )

/************************************************************************
* current_time								*
*									*
* This callback function updates a running clock at a frequency of one	*
* second.								*
*									*
************************************************************************/
{

	WindowObjectType *wo;
	static int	clockInterval = 0;
	char		*p;


/*
 *	Get the interval between clock updates from the configuration
 *	file. 
 */
	if( clockInterval == 0 ) {
		if( p = GetConfigValue( CLOCKINTERVAL ) ) {
			clockInterval = atoi( p ) * 1000;
			Free (p);
		}
		else
			clockInterval = 1000;
	}		

/*
 *	Update the status bar with the application time. It's possible
 *	that the window object is not instantiated yet, so check it.
 */
	wo = GetActiveWindowObject ();
	if ( wo ) UpdateStatusBar( wo);

/*
 *	Make sure function is called again in clockInterval milli-seconds.
 */

	clock_timer_id = 
		XtAppAddTimeOut ( XtWidgetToApplicationContext (w),
				  clockInterval,
				  (XtTimerCallbackProc) current_time, w);

}



void GetClockString( char *string )
{
	time_t		lclock;
	struct tm       *tmp = NULL;
	struct tm	timeptr, usertime;
	int		iret;

/*
 *	Get the clock time in integer format.
 */
	lclock = present_time ();
	/*(void)time(&lclock);*/
	tmp = gmtime(&lclock);

	usertime = *tmp;

/*
 *	Format the  application clock time into a string.
 */

	if ( usertime.tm_year < 0 ) {
	    fprintf(stdout,"look present %d\n",lclock);
            fprintf(stdout,"look tm year %d  ",usertime.tm_year);
            fprintf(stdout,"look tm mon %d  ",usertime.tm_mon);
            fprintf(stdout,"look tm mday %d  ",usertime.tm_mday);
            fprintf(stdout,"look tm hour %d  ",usertime.tm_hour);
            fprintf(stdout,"look tm min %d  ",usertime.tm_min);
            fprintf(stdout,"look tm sec %d\n",usertime.tm_sec);
	    (void)time(&lclock);
	    tmp = gmtime(&lclock);

	    usertime = *tmp;
	    put_timeptr ( "timeptr", &usertime, &iret );
	    strftime ( string, 256, "%a %b %d %H:%M:%S UTC %Y", &usertime);
	    /*sprintf( string, "Narf time UTC blah\0");*/
	    fprintf(stdout,"again present %d ",lclock);
            fprintf(stdout,"look tm year %d\n",usertime.tm_year);
	}
	else {
	    put_timeptr ( "timeptr", &usertime, &iret );
	    if ( iret != 0 ) fprintf(stdout,"put_timeptr %d\n",iret);

	    strftime ( string, 256, "%a %b %d %H:%M:%S UTC %Y", &usertime);
	}


}


static void
build_clock ( w, iret )
	Widget	w;
	int	*iret;

/************************************************************************
* build_clock								*
*									*
* This routine is the callback for the date/time menu.			*
*									*
* Input parameters:							*
*	w		Widget		Input widget			*
*	which		int		Item number for the menu	*
*	cb_data		XtPointer	( not used )			*
*									*
* Output parameters:							*
*									*
***********************************************************************/
{
	Widget		time_dialog, pane, form, action_area, widget;
	XmString        xmlabel;

	struct tm	time_struct;
	struct tm	*timeptr = &time_struct;
        char            str[5];
        char            tmp[3];
        int             temp_year;
        int             x, y;
        static char* month[]={"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                              "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

/*---------------------------------------------------------------------*/

	*iret = NORMAL;

/*
 *	Retrieve current time values.
 */
	get_timeptr ( "timeptr", timeptr, &iret );

/*
 *	Popup shell.
 */
	time_dialog = XtVaCreatePopupShell("Set Time",
				xmDialogShellWidgetClass,
				( Widget ) GetTopShell(w),
				XmNdeleteResponse, XmDESTROY,
				NULL);
/*
 *	Create a PanedWindow to manage the stuff in this dialog.
 */
	pane = XtVaCreateWidget("pane", 
				xmPanedWindowWidgetClass, time_dialog,
				XmNsashWidth,  1,
				XmNsashHeight, 1,
				NULL);

/*
 *	Create a Form for the time widgets.
 */
	form = XtVaCreateWidget("form1", 
				xmFormWidgetClass, pane, 
				XmNfractionBase, 5,
				NULL);
	x = 0; y = 0;
/*
 *	Build the "month" button.
 */
	xmlabel = XmStringCreateLtoR(month[timeptr->tm_mon],
                                XmSTRING_DEFAULT_CHARSET);

	month_b = XtVaCreateManagedWidget("month",
				xmPushButtonWidgetClass, form,
				XmNfillOnArm, TRUE,
				XmNlabelString, xmlabel,
				XmNtopAttachment, XmATTACH_POSITION,
				XmNtopPosition, y,
				XmNleftAttachment, XmATTACH_POSITION,
				XmNleftPosition, x,
				XmNbottomAttachment, XmATTACH_POSITION,
				XmNbottomPosition, y+1,
				XmNrightAttachment, XmATTACH_POSITION,
				XmNrightPosition, x+1,
				NULL); x++;

	XmStringFree ( xmlabel );
	XtAddCallback(month_b, XmNarmCallback,
	             ( XtCallbackProc ) change_time, ( XtPointer ) 1 ); 
/*
 *	Build the "day" button.
 */
	sprintf ( str, "%.2d", timeptr->tm_mday );
	xmlabel = XmStringCreateLtoR(str,
                                XmSTRING_DEFAULT_CHARSET);

	day_b = XtVaCreateManagedWidget("day",
	xmPushButtonWidgetClass, form,
				XmNfillOnArm, TRUE,
				XmNlabelString, xmlabel,
				XmNtopAttachment, XmATTACH_POSITION,
				XmNtopPosition, y,
				XmNleftAttachment, XmATTACH_POSITION,
				XmNleftPosition, x,
				XmNbottomAttachment, XmATTACH_POSITION,
				XmNbottomPosition, y+1,
				XmNrightAttachment, XmATTACH_POSITION,
				XmNrightPosition, x+1,
				NULL); x++;

	XmStringFree ( xmlabel );
	XtAddCallback ( day_b, XmNarmCallback,
	              ( XtCallbackProc ) change_time, ( XtPointer ) 2);

/*
 *	Build the "year" button.
 *	In unix time, if tm_year < 100 we are looking at 1900s.
 */
	if ( timeptr->tm_year < 100 ) {
	    strcpy ( str, "19" );
	    sprintf ( tmp, "%d", timeptr->tm_year );
	    strcat ( str, tmp );
	}
	else {
	    temp_year = timeptr->tm_year - 100;
	    strcpy ( str, "20" );
	    sprintf ( tmp, "%.2d", temp_year );
	    strcat ( str, tmp );
	}
	    xmlabel = XmStringCreateLtoR(str,
                                XmSTRING_DEFAULT_CHARSET);

	    year_b = XtVaCreateManagedWidget("year",
				xmPushButtonWidgetClass, form,
				XmNfillOnArm, TRUE,
				XmNlabelString, xmlabel,
				XmNtopAttachment, XmATTACH_POSITION,
				XmNtopPosition, y,
				XmNleftAttachment, XmATTACH_POSITION,
				XmNleftPosition, x,
				XmNbottomAttachment, XmATTACH_POSITION,
				XmNbottomPosition, y+1,
				XmNrightAttachment, XmATTACH_POSITION,
				XmNrightPosition, x+1,
				NULL); x++; x++;

	XmStringFree ( xmlabel );
	XtAddCallback ( year_b, XmNarmCallback,
	              ( XtCallbackProc ) change_time, ( XtPointer ) 3 );

/*
 *      Build the "left arrow" button.
 */

        arrow_l = XtVaCreateManagedWidget("arrow_left",
                    xmArrowButtonGadgetClass, form,
                    XmNarrowDirection, XmARROW_LEFT,
                    XmNtopAttachment, XmATTACH_POSITION,
                    XmNtopPosition, y,
                    XmNleftAttachment, XmATTACH_POSITION,
                    XmNleftPosition, x,
                    XmNbottomAttachment, XmATTACH_POSITION,
                    XmNbottomPosition, y+1,
                    XmNrightAttachment, XmATTACH_POSITION,
                    XmNrightPosition, x+1,
                    NULL); x=0; y++;

	XtAddCallback ( arrow_l, XmNarmCallback,
	              ( XtCallbackProc ) arrow_left, NULL );
	XtAddCallback ( arrow_l, XmNdisarmCallback,
	              ( XtCallbackProc ) arrow_left, NULL );

/*
 *      Build the "hour" button.
 */

	sprintf ( str, "%.2d", timeptr->tm_hour );
	xmlabel = XmStringCreateLtoR(str,
                                XmSTRING_DEFAULT_CHARSET);

	hour_b = XtVaCreateManagedWidget("hour",
				xmPushButtonWidgetClass, form,
				XmNfillOnArm, TRUE,
				XmNlabelString, xmlabel,
				XmNtopAttachment, XmATTACH_POSITION,
				XmNtopPosition, y,
				XmNleftAttachment, XmATTACH_POSITION,
				XmNleftPosition, x,
				XmNbottomAttachment, XmATTACH_POSITION,
				XmNbottomPosition, y+1,
				XmNrightAttachment, XmATTACH_POSITION,
				XmNrightPosition, x+1,
				NULL); x++;

	XmStringFree ( xmlabel );
	XtAddCallback ( hour_b, XmNarmCallback,
	              ( XtCallbackProc ) change_time, ( XtPointer ) 4 );

/*
 *	Build the "minute" button.
 */
	sprintf ( str, "%.2d", timeptr->tm_min );
	xmlabel = XmStringCreateLtoR(str,
                                XmSTRING_DEFAULT_CHARSET);

	min_b = XtVaCreateManagedWidget("minute",
				xmPushButtonWidgetClass, form,
				XmNfillOnArm, TRUE,
				XmNlabelString, xmlabel,
				XmNtopAttachment, XmATTACH_POSITION,
				XmNtopPosition, y,
				XmNleftAttachment, XmATTACH_POSITION,
				XmNleftPosition, x,
				XmNbottomAttachment, XmATTACH_POSITION,
				XmNbottomPosition, y+1,
				XmNrightAttachment, XmATTACH_POSITION,
				XmNrightPosition, x+1,
				NULL); x++;

	XmStringFree ( xmlabel );
	XtAddCallback ( min_b, XmNarmCallback, 
	              ( XtCallbackProc ) change_time, ( XtPointer ) 5 );

/*
 *	Build the "second" button.
 */
	sprintf ( str, "%.2d", timeptr->tm_sec );
	xmlabel = XmStringCreateLtoR(str,
                                XmSTRING_DEFAULT_CHARSET);

	sec_b = XtVaCreateManagedWidget("second",
				xmPushButtonWidgetClass, form,
				XmNfillOnArm, TRUE,
				XmNlabelString, xmlabel,
				XmNtopAttachment, XmATTACH_POSITION,
				XmNtopPosition, y,
				XmNleftAttachment, XmATTACH_POSITION,
				XmNleftPosition, x,
				XmNbottomAttachment, XmATTACH_POSITION,
				XmNbottomPosition, y+1,
				XmNrightAttachment, XmATTACH_POSITION,
				XmNrightPosition, x+1,
				NULL); x++;

	XmStringFree ( xmlabel );
	XtAddCallback ( sec_b, XmNarmCallback,
	              ( XtCallbackProc ) change_time, ( XtPointer ) 6 );

/*
 *	Build the "time zone" button.
 */
	tz_b = XtVaCreateManagedWidget("GMT",
				xmPushButtonWidgetClass, form,
				XmNtopAttachment, XmATTACH_POSITION,
				XmNtopPosition, y,
				XmNleftAttachment, XmATTACH_POSITION,
				XmNleftPosition, x,
				XmNbottomAttachment, XmATTACH_POSITION,
				XmNbottomPosition, y+1,
				XmNrightAttachment, XmATTACH_POSITION,
				XmNrightPosition, x+1,
				NULL); x++; 


/*
 *	Build the "right arrow" button.
 */
	arrow_r = XtVaCreateManagedWidget("arrow_right",
				xmArrowButtonGadgetClass, form,
				XmNarrowDirection, XmARROW_RIGHT,
				XmNtopAttachment, XmATTACH_POSITION,
				XmNtopPosition, y,
				XmNleftAttachment, XmATTACH_POSITION,
				XmNleftPosition, x,
				XmNbottomAttachment, XmATTACH_POSITION,
				XmNbottomPosition, y+1,
				XmNrightAttachment, XmATTACH_POSITION,
				XmNrightPosition, x+1,
				NULL, 0); x=2; y++;

	XtAddCallback ( arrow_r, XmNarmCallback,
	              ( XtCallbackProc ) arrow_right, NULL );
	XtAddCallback ( arrow_r, XmNdisarmCallback,
	              ( XtCallbackProc ) arrow_right, NULL );

/*
 *	Create another form to act as the action area for the dialog.
 */
	action_area = XtVaCreateWidget("form2", 
				xmFormWidgetClass, pane,
				XmNfractionBase,    5,
				NULL);

/*	The Ok button is under the pane's separator and is
 *	attached to the left edge of the form.  It spreads from
 *	position 0 to 1 along the bottom (the form is split into
 *	5 separate grids ala XmNfractionBase upon creation).
 */
	widget = XtVaCreateManagedWidget("Apply",
				xmPushButtonGadgetClass, action_area,
				XmNtopAttachment,        XmATTACH_FORM,
				XmNbottomAttachment,     XmATTACH_FORM,
				XmNleftAttachment,       XmATTACH_POSITION,
				XmNleftPosition,         1,
				XmNrightAttachment,      XmATTACH_POSITION,
				XmNrightPosition,        2,
				XmNshowAsDefault,        True,
				XmNdefaultButtonShadowThickness, 1,
				NULL);

	XtAddCallback ( widget, XmNarmCallback,
	              ( XtCallbackProc ) set_time_cb, NULL );
	XtAddCallback ( widget, XmNactivateCallback,
	              ( XtCallbackProc ) DestroyShell, time_dialog );

	widget = XtVaCreateManagedWidget("Reset",
				xmPushButtonGadgetClass, action_area,
				XmNtopAttachment,        XmATTACH_FORM,
				XmNbottomAttachment,     XmATTACH_FORM,
				XmNleftAttachment,       XmATTACH_POSITION,
				XmNleftPosition,         2,
				XmNrightAttachment,      XmATTACH_POSITION,
				XmNrightPosition,        3,
				XmNshowAsDefault,        False,
				XmNdefaultButtonShadowThickness, 1,
				NULL);

	XtAddCallback ( widget, XmNarmCallback,
	              ( XtCallbackProc ) reset_time_cb, NULL);

	widget = XtVaCreateManagedWidget("Close",
				xmPushButtonGadgetClass, action_area,
				XmNtopAttachment,        XmATTACH_FORM,
				XmNbottomAttachment,     XmATTACH_FORM,
				XmNleftAttachment,       XmATTACH_POSITION,
				XmNleftPosition,         3,
				XmNrightAttachment,      XmATTACH_POSITION,
				XmNrightPosition,        4,
				XmNshowAsDefault,        False,
				XmNdefaultButtonShadowThickness, 1,
				NULL);

	XtAddCallback ( widget, XmNarmCallback,
	              ( XtCallbackProc ) cancel_cb, NULL);
	XtAddCallback ( widget, XmNactivateCallback,
	              ( XtCallbackProc ) DestroyShell, time_dialog);



	XtManageChild(form);
	XtManageChild(action_area);
	XtManageChild(pane);

	XtPopup(time_dialog, XtGrabNone);
}



static void
arrow_left ( w, client_data, call_data )
	Widget				w;
	XtPointer			client_data;
	XmArrowButtonCallbackStruct	*call_data;

/************************************************************************
* arrow_left								*
*									*
* This callback function decrements a pointer for the currently active	*
* time button.								*
*									*
* Input parameters:							*
*	w		Widget				Input widget	*
*	client_data	XtPointer			( not used )	*
*	*call_data	XmArrowButtonCallbackStruct	Callback info	*
*									*
* Output parameters:							*
*	none								*
*									*
************************************************************************/
{
	int	incr = -1;

/*---------------------------------------------------------------------*/


	if ( call_data->reason == XmCR_ARM ) {
	    save_w = w;
	    save_incr = incr;
	    next_time ( w, incr, call_data->event->type == ButtonPress );
	}
	else if ( call_data->reason == XmCR_DISARM ) {
	    XtRemoveTimeOut ( arrow_timer_id );
	}
}


static void
arrow_right(w, client_data, call_data)
	Widget				w;
	XtPointer			client_data;
	XmArrowButtonCallbackStruct	*call_data;

/************************************************************************
* arrow_right								*
*									*
* This callback function increments a pointer for the currently active	*
* time button.								*
*									*
* Input parameters:							*
*	w		Widget				Input widget	*
*	client_data	XtPointer			( not used )	*
*	*call_data	XmArrowButtonCallbackStruct	Callback info	*
*									*
* Output parameters:							*
*									*
************************************************************************/
{
	int	incr = 1;

/*---------------------------------------------------------------------*/

	if ( call_data->reason == XmCR_ARM ) {
	    save_w = w;
	    save_incr = incr;
	    next_time ( w, incr, call_data->event->type == ButtonPress );
	}
	else if ( call_data->reason == XmCR_DISARM ) {
	    XtRemoveTimeOut ( arrow_timer_id );
	}
}


static void
next_time ( w, incr, id )
	Widget		w;
	int		incr;
	int		id;

/************************************************************************
* next_time								*
*									*
* This function sets a timer for changing the value of the currently 	*
* active time button.							*
*									*
* Input parameters:							*
*	w		Widget		Input widget			*
*	incr		int		Increment/decrement flag	*
*	id		int		ButtonPress event flag		*
*									*
* Output parameters:							*
*	none								*
*									*
************************************************************************/
{
	struct tm	time_struct;
	struct tm	*timeptr = &time_struct;

	int		choice;
	int		iret;

/*---------------------------------------------------------------------*/

	iret = NORMAL;

/*
 *	Get statically saved values. "w" and "incr" are not passed 
 *	through function calls properly when the timer is invoked.
 */
	choice = time_choice;
	w = save_w;
	incr = save_incr;

/*
 *	Get values of time pointer.
 */
	get_timeptr ( "timeptr", timeptr, &iret );

	if ( incr < 0 ) {
/*
 *	    Decrement appropriate pointer.
 */
	    switch (choice) {
	    case 1:
	    	timeptr->tm_mon--;
	    	if ( timeptr->tm_mon == -1) timeptr->tm_mon=11;
	    	break;
	    case 2:
	    	timeptr->tm_mday--;
	    	break;
	    case 3:
	    	timeptr->tm_year--;
	    	break;
	    case 4:
	    	timeptr->tm_hour--;
	    	if ( timeptr->tm_hour == -1) timeptr->tm_hour=23;
	    	break;
	    case 5:
	    	timeptr->tm_min--;
	    	if ( timeptr->tm_min == -1) timeptr->tm_min=59;
	    	break;
	    case 6:
	    	timeptr->tm_sec--;
	    	if ( timeptr->tm_sec == -1) timeptr->tm_sec=59;
	    	break;
	    }
        }
	else if ( incr > 0 ) {
/*
 *	    Increment appropriate pointer.
 */
            switch (choice) {
            case 1:
          	timeptr->tm_mon++;
          	if ( timeptr->tm_mon == 12) timeptr->tm_mon=0;
          	break;
            case 2:
          	timeptr->tm_mday++;
          	break;
            case 3:
          	timeptr->tm_year++;
          	break;
            case 4:
          	timeptr->tm_hour++;
          	if ( timeptr->tm_hour == 24) timeptr->tm_hour=0;
          	break;
            case 5:
          	timeptr->tm_min++;
          	if ( timeptr->tm_min == 60) timeptr->tm_min=0;
          	break;
            case 6:
          	timeptr->tm_sec++;
          	if ( timeptr->tm_sec == 60) timeptr->tm_sec=0;
          	break;
	    }
	}
/*
 *	Update the time structure.
 */
	drt_time ( &picked_time, timeptr, &iret );

/*
 *	Update the time.
 */
	update_time ( month_b, day_b, hour_b, min_b, sec_b, 
	              year_b, tz_b );

/*
 *	Start timer for multi-click on a time widget.
 */
	    arrow_timer_id =
	         XtAppAddTimeOut ( XtWidgetToApplicationContext ( w ),
	          200,
	         ( XtTimerCallbackProc ) next_time, ( XtPointer ) incr );
}
