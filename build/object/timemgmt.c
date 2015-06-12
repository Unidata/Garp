/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	timemgmt.c
 *
 *	Time related utility routines.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	12/99	COMET	Changed time_t data type to AbsTime.
 *
 ***********************************************************************/


#include <time.h>
#include <string.h>

#include "genglobs.h"
#include "_proto.h"
#include "timeutil.h"


/*
 *	The following static variables are saved and retrieved from this
 *	file to avoid declaring them as global.
 */
static int		SettingTimeFlag = 0;
static int		ClockIsRunning  = 0;
static AbsTime		TimeOffset	= 0;
static struct tm	CurrentApplicationTime;
static struct tm	*CurrentApplicationTimePtr = &CurrentApplicationTime;

void put_timeptr();

void
drt_time ( int_time, timeptr, iret )
	AbsTime		*int_time;
	struct tm	*timeptr;
	int		*iret;

/************************************************************************
* drt_time								*
*									*
* This function converts time stored in a time structure as local time	*
* into GMT in seconds.							*
*									*
* Input parameters:							*
*	*timeptr	struct tm	Time pointer			*
*									*
* Output parameters:							*
*	*int_time	AbsTime		Integer time			*
*	*iret		int		Return code			*
*					 0 = normal			*
************************************************************************/
{
	AbsTime		lt, tz_time;

/*---------------------------------------------------------------------*/

	*iret = NORMAL;

/*
 *	Convert time in time structure to integer time.
 */
	lt = mktime ( timeptr );
	if ( lt < 0 ) {
	    *iret = -1;
	    *int_time = -1;
	    return;
	}

/*
 *	Get time zone offset in seconds.
 */
	tz_time = get_tz ( iret );

/*
 *	Add in GMT offset.
 */
	lt = lt - tz_time;

/*
 *	Reset time structure to account for daylight savings time.
 */
	timeptr = gmtime ( &lt );
	put_timeptr ( "timeptr", timeptr, iret );

/*
 *	Return integer time.
 */
	*int_time = lt;
}

int
get_tz( iret )
	int	*iret;

/************************************************************************
* get_tz								*
* 									*
* This function returns the number of hours between local time and GMT.	*
*									*
* Input parameters:							*
*	none								*
*									*
* Output parameters:							*
*	get_tz		int	Time difference in hours.		*
*	*iret		int	Return code:				*
*				  0 = normal				*
*									*
************************************************************************/
{
	struct tm	*timeptr;
	AbsTime		gt, lt;
	int		time_zone;

/*---------------------------------------------------------------------*/

	*iret = NORMAL;

/*
 *	Get integer time in local time.
 */
	lt = time ( NULL );

/*
 *	Save GMT time in time structure.
 */
	timeptr = gmtime ( &lt );

/*
 *	Get integer time in GMT.
 */
	gt = mktime ( timeptr );

/*
 *	Find difference between local time and GMT.
 */
	time_zone =  difftime ( gt, lt );
	return time_zone;
}

void
init_clock ( char *clock_time, int noTick, int *iret )

/************************************************************************
* init_clock								*
*									*
* This routine will initialize the application clock.			*
*									*
* Input parameters:							*
*	*clock_time	char	Initial clock time in YYMMDD/HHMM	*
*									*
* Output parameters:							*
*	*iret		int	Return code				*
**									*
* Log:									*
* CSDG/COMET	 1/96							*
************************************************************************/

{
	AbsTime		t1, t2;
	AbsTime		time_offset, tz_time;
	struct tm	*timeptr;
	char		slash;
	int		verbose;
	char		*format="%02d%02d%02d%c%02d%02d";

/*---------------------------------------------------------------------*/
	*iret = NORMAL;

	verbose = GetVerboseLevel();

/*
 *	Initialize statically maintained variables.
 */
	put_int ( "setting_time_flag", FALSE, iret );
	put_time ( "time_offset", 0, iret );


	if( noTick )
		put_int ( "clock_is_running", FALSE, iret );
	else
		put_int ( "clock_is_running", TRUE, iret );

	if ( verbose > VERBOSE_1 )
		printf("  init_clock: Clock is %s ticking\n", 
			noTick ? "not" : "" );
		

/*
 *
 *	After we have some default values installed.  Make a
 *	check to see if we can bail out early.
 *
 */
	if ( ! CheckDateTimeString ( clock_time ) ) {
		if ( verbose > VERBOSE_1 )
			printf("  init_clock: Bad time string %s\n", 
				clock_time ? clock_time : "Undefined value"  );
		return;
	}
		


/*
 *      Get the current GMT time.
 */
	t2 = time('\0');
	timeptr = gmtime(&t2);

/*
 *	Reset the current time for case studies.
 */
	sscanf ( clock_time, format,
		&timeptr->tm_year,
		&timeptr->tm_mon,
		&timeptr->tm_mday,
		&slash,
		&timeptr->tm_hour,
		&timeptr->tm_min);

		timeptr->tm_mon--;	/* months are 0-11 */
	/*himes	timeptr->tm_min = 0; */
		timeptr->tm_sec = 0;
	t1 = mktime ( timeptr );
	tz_time = get_tz ( iret );
	time_offset = t2 - t1 + tz_time;

/*
 *	Save time offset.
 */
	put_time ( "time_offset", time_offset, iret );


	if( GetVerboseLevel() > VERBOSE_1 ) {
	    printf("  Timevalue for string: %s:\n", 
			clock_time ? clock_time : "Undefined value"  );

	    printf("  year = %d\n",   timeptr->tm_year);
	    printf("  month = %d\n",  timeptr->tm_mon);
	    printf("  day = %d\n",    timeptr->tm_mday);
	    printf("  hour = %d\n",   timeptr->tm_hour);
	    printf("  minute = %d\n", timeptr->tm_min);
	    printf("  second = %d\n", timeptr->tm_sec);
	}
}


void
init_time (	char *current_time, 
		char *begin_time, 
		char *end_time, 
		int  noTick,
		int *iret )

/************************************************************************
* init_time								*
*									*
* This routine will initialize the initial time to set the application	*
* clock as well as defining times which the user may view data.		*
*									*
* Input parameters:							*
*	*current_time	char	Initial clock time in YYMMDD/HHMM	*
*	*begin_time	char	Time before which user cannot view data	*
*	*end_time	char	Time after which user cannot view data	*
*	noTick	        int	Flag to indicate if the clock is ticking*
*									*
* Output parameters:							*
*	*iret		int	Return code				*
**									*
* Log:									*
* CSDG/COMET	 1/96							*
************************************************************************/

{
	TimeObjectType		*td;
	AbsTime			currenttm, begintm, endtm;
	BooleanType		error = FALSE;

/*---------------------------------------------------------------------*/
	*iret = NORMAL;

/*
 *	Get time object.
 */
	td = GetTimeObject();

	currenttm = begintm = endtm = -1;

	SetCaseCurrentTime ( td, "" );
	SetCaseBeginTime ( td, "" );
	SetCaseEndTime ( td, "" );

/*
 *	Set time object members.
 */
	if ( CheckDateTimeString ( current_time ) ) {
		SetCaseCurrentTime ( td, current_time );
		currenttm = ch_to_int_time ( current_time );
	}

	if ( CheckDateTimeString ( begin_time ) ) {
		SetCaseBeginTime ( td, begin_time );
		begintm = ch_to_int_time ( begin_time );
	}

	if ( CheckDateTimeString ( end_time ) ) {
		SetCaseEndTime ( td, end_time );
		endtm = ch_to_int_time ( end_time );
	}

/*
 *	Sanity check to make sure input times are consistent.
 */

	if ( currenttm > 0 && begintm > 0 && begintm > currenttm )
	    error = TRUE;

	if ( currenttm > 0 && endtm > 0 && currenttm > endtm )
	    error = TRUE;

/*
 *	Exit program if input error.
 */
	if ( error ) {
	    printf("*** init_time: Error defining case input times. Exiting...\n");
	    exit ( 1 );
	}

/*
 *	Initialize application clock.
 */
	init_clock ( current_time, noTick, iret );

}

void
systime ( time_offset, new_time, iret )
	AbsTime		time_offset;
	AbsTime		*new_time;
	int		*iret;

/************************************************************************
* int_time								*
*									*
* This function returns integer time which is current time minus an	*
* offset.								*
*									*
* Input parameters:							*
*	time_offset	AbsTime		Time offset in seconds		*
*									*
* Output parameters:							*
*	*new_time	AbsTime		Time in seconds			*
*	*iret		int		Return code			*
*					 0 = normal			*
*									*
************************************************************************/
{
	struct tm	current_time;
	struct tm	*timeptr;
	AbsTime		lt;

/*---------------------------------------------------------------------*/

	*iret = NORMAL;

/*
 *	Get current integer time.
 */
	lt = time ( '\0' );

/*
 *	Subtract time offset.
 */
	lt = lt - time_offset;

/*
 *	Set time pointer.
 */
	timeptr = gmtime ( &lt );
	current_time = *timeptr;
	put_timeptr ( "timeptr", &current_time, iret );
/*
 *	Return integer time.
 */
	*new_time = lt;
}


void put_time ( char *name, AbsTime value, int *iret )
{
	*iret = NORMAL;
	if ( strncmp ( name, "time_offset",
					strlen ( name ) ) == 0 ) {
	    TimeOffset = value;
	}
}

void put_int ( char *name, int value, int *iret )

/************************************************************************
* put_int 								*
*									*
* This function saves integer values.					*
*									*
* Input parameters:							*
*	*name		char		Name of integer to save		*
*	value		int		Value of saved integer		*
*									*
* Output parameters:							*
*	*iret		int		Return code			*
*					 0 = normal			*
*									*
************************************************************************/
{

/*---------------------------------------------------------------------*/

	*iret = NORMAL;

/*
 *	Save integer value to static variable.
 */
	if ( strncmp ( name, "setting_time_flag",
	                                      strlen ( name ) ) == 0 ) {
	    SettingTimeFlag = value;
	}
	else if ( strncmp ( name, "clock_is_running",
	                                      strlen ( name ) ) == 0 ) {
	    ClockIsRunning = value;
	}
	else if ( strncmp ( name, "time_offset",
	                                      strlen ( name ) ) == 0 ) {
	    printf("incorrect use of out_int %s\n",name);
	}
}


void get_time ( char *name, AbsTime *value, int *iret )
{
	*iret = NORMAL;
	if ( strncmp ( name, "time_offset",
					strlen ( name ) ) == 0 ) {
	    *value = TimeOffset;
	}
}

void get_int ( char *name, int *value, int *iret )

/************************************************************************
* get_int								*
*									*
* This function retrieves a stored integer.				*
*									*
* Input parameters:							*
*	*name		char		Name of integer to retrieve	*
*									*
* Output parameters:							*
*	*value		int		Value of retrieved integer	*
*	*iret		int		Return code			*
*					 0 = normal			*
*									*
************************************************************************/
{

/*---------------------------------------------------------------------*/

	*iret = NORMAL;

/*
 *	Save integer value to static variable.
 */
	if ( strncmp ( name, "setting_time_flag",
	                                      strlen ( name ) ) == 0 ) {
	    *value = SettingTimeFlag;
	}
	else if ( strncmp ( name, "clock_is_running",
	                                      strlen ( name ) ) == 0 ) {
	    *value = ClockIsRunning;
	}
	else if ( strncmp ( name, "time_offset",
	                                      strlen ( name ) ) == 0 ) {
	    printf("Incorrect use of get_int\n");
	}
}



void
put_timeptr ( char *name, struct tm *timeptr, int *iret )

/************************************************************************
* put_timeptr 								*
*									*
* This function saves current values of a time pointer.			*
*									*
* Input parameters:							*
*	*name		char		Name of time pointer		*
*	*timeptr	struct tm	Time pointer			*
*									*
* Output parameters:							*
*	*iret		int		Return code			*
*					 0 = normal			*
*									*
************************************************************************/
{

/*---------------------------------------------------------------------*/

	*iret = NORMAL;

/*
 *	Save integer value to static variable.
 */
	if ( strncmp ( name, "timeptr",
	                                      strlen ( name ) ) == 0 ) {
	    *CurrentApplicationTimePtr = *timeptr;
	}
}



void
get_timeptr ( name, timeptr, iret )
	char		*name;
	struct tm	*timeptr;
	int		*iret;

/************************************************************************
* get_timeptr 								*
*									*
* This function retrieves current values of a time pointer.		*
*									*
* Input parameters:							*
*	*name		char		Name of time pointer		*
*									*
* Output parameters:							*
*	*timeptr	struct tm	Time pointer			*
*	*iret		int		Return code			*
*					 0 = normal			*
*									*
************************************************************************/
{

/*---------------------------------------------------------------------*/

	*iret = NORMAL;

/*
 *	Save integer value to static variable.
 */
	if ( strncmp ( name, "timeptr",
	                                      strlen ( name ) ) == 0 ) {
	    *timeptr = *CurrentApplicationTimePtr;
	}
}

void
update_time (  month_b, day_b, hour_b, min_b, sec_b, year_b, tz_b )
	Widget	month_b;
	Widget	day_b;
	Widget	hour_b;
	Widget	min_b;
	Widget	sec_b;
	Widget	year_b;
	Widget	tz_b;

/************************************************************************
* update_time								*
*									*
* This function updates the currently active time button.		*
*									*
* Input parameters:							*
*	month_b		Widget		Month widget			*
*	day_b		Widget		Day widget			*
*	hour_b		Widget		Hour widget			*
*	min_b		Widget		Minute widget			*
*	sec_b		Widget		Second widget			*
*	year_b		Widget		Year widget			*
*	tz_b		Widget		Time zone widget		*
*									*
* Output parameters:							*
*	same as above							*
*									*
************************************************************************/

{
        XmString xmlabel;
	static Arg wargs[10];

	struct tm		current_time;
	struct tm		*timeptr = &current_time;
	int			temp_year;
	int			iret;
	char			str[5];
	char			tmp[3];
	static char* month[]={"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                              "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

/*---------------------------------------------------------------------*/

/*
 *	Get time pointer values.
 */
	get_timeptr ( "timeptr", timeptr, &iret );

/*
 *	Month button.
 */
	xmlabel = XmStringCreateLtoR ( month[timeptr->tm_mon],
					 XmSTRING_DEFAULT_CHARSET);
	XtVaSetValues ( month_b, XmNlabelString, xmlabel, NULL);
	XmStringFree ( xmlabel );


/*
 *	Day button.
 */
	sprintf ( str, "%.2d", timeptr->tm_mday );
	xmlabel = XmStringCreateLtoR ( str,
                                XmSTRING_DEFAULT_CHARSET);
	XtVaSetValues ( day_b, XmNlabelString, xmlabel, NULL);
	XmStringFree ( xmlabel );


/*
 *	Year button. In unix time, if tm_year < 100 we are looking
 *	at 1900s.
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
	xmlabel = XmStringCreateLtoR ( str,
					 XmSTRING_DEFAULT_CHARSET);
	XtVaSetValues ( year_b, XmNlabelString, xmlabel, NULL);
	XmStringFree ( xmlabel );


/*
 *	Hour button.
 */
	sprintf ( str, "%.2d", timeptr->tm_hour );
	xmlabel = XmStringCreateLtoR ( str,
                                XmSTRING_DEFAULT_CHARSET);
	XtVaSetValues ( hour_b, XmNlabelString, xmlabel, NULL);
	XmStringFree ( xmlabel );


/*
 *	Minute button.
 */
	sprintf ( str, "%.2d", timeptr->tm_min );
	xmlabel = XmStringCreateLtoR ( str, 
					 XmSTRING_DEFAULT_CHARSET);
	XtVaSetValues ( min_b, XmNlabelString, xmlabel, NULL);
	XmStringFree ( xmlabel );


/*
 *	Second button.
 */
	sprintf ( str, "%.2d", timeptr->tm_sec );
	xmlabel = XmStringCreateLtoR ( str,
                                XmSTRING_DEFAULT_CHARSET);
	XtVaSetValues ( sec_b, XmNlabelString, xmlabel, NULL);
	XmStringFree ( xmlabel );

}
