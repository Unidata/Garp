/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	mainwin.h
 *
 *	Time utility prototypes.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/

/* timedef.h								*/
/* Function definitions for clock widget				*/


			/* Build clock widget				*/

static void build_clock ( Widget w,
                          int *iret );


/* --- Callback --- */


			/* Update time string every n seconds		*/

void current_time	( Widget w );


			/* Decrement current button value		*/

static void arrow_left	( Widget w,
			  XtPointer client_data,
			  XmArrowButtonCallbackStruct *call_data );


			/* Increment current button value		*/

static void arrow_right	( Widget w,
			  XtPointer client_data,
			  XmArrowButtonCallbackStruct *call_data );


			/* Set active time button			*/

static void change_time	( Widget w,
			  int choice,
			  XmAnyCallbackStruct *call_data );


			/* Exit						*/

static void cancel	( Widget w,
			  XtPointer client_data,
			  XmAnyCallbackStruct *call_data );


			/* Compute time offset from system time		*/

static void set_time	( Widget w,
			  XtPointer client_data,
			  XmAnyCallbackStruct *call_data );


			/* Reset to current time			*/

static void reset_time	( Widget w,
			  XtPointer client_data,
			  XmAnyCallbackStruct *call_data );


			/* Exit time dialog				*/

extern void DestroyShell ( Widget  widget,
                           Widget  shell );



/* --- Utilities --- */

			/* Update the time structure			*/

extern void drt_time	( time_t *int_time,
			  struct tm *timeptr,
			  int *iret );


			/* Get the local system time			*/

extern void systime	( time_t time_offset,
			  time_t *new_time,
			  int *iret);


			/* Time difference between local and GMT	*/

extern int get_tz	( int *iret );


			/* Update time/date buttons			*/

extern void update_time	( Widget  month_b,
			  Widget  day_b,
			  Widget  hour_b,
			  Widget  min_b,
			  Widget  sec_b,
			  Widget  year_b,
			  Widget  tz_b );

			/* Update time widget and start multi-click counter. */
static void next_time ( Widget w,
                        int incr,
                        int id );

extern void put_int ();
extern void get_int ();
extern void put_timeptr ();
extern void get_timeptr ();
