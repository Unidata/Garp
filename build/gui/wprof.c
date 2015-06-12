/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	wprof.c
 *
 *	Callbacks and utilities for wind profiler data.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 6/97	COMET	Removed imgobj.h
 *	11/97	COMET	Added "titleIndex" to keep track of title line.
 *	11/97	COMET	Fixed small leak in ProfilerCB()
 *	12/97	COMET	Get default text settings from keys in "Garp_defaults".
 *	 2/98	COMET	Descritize time interval in minutes.
 *	 2/98	COMET	Added print info if file is not found.
 *	12/99	COMET	Replaced makedate() and datemake() functions with
 *			sec2yyyymmddhhmm() and yyyymmddhhmm2sec().
 *	01/03	Unidata	Increased list to 300, added time fuzz
 *
 ***********************************************************************/

#include "geminc.h"
#include "gemprm.h"
#include "guimacros.h"
#include "genglobs.h"
#include "winobj.h"
#include "mainwin.h"
#include "timeutil.h"
#include "wprofobj.h"
#include "utils.h"
#include "ctbcmn.h"
#include "command.h"
#include "_proto.h"


char * sec2yyyymmddhhmm ( AbsTime );
char * ProfilerTimeInterval ( char * );

void
ProfilerIconCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
/*
 *  This function is called when the user clicks on the wind profiler
 *  icon or menu item.
 */
{
	GuiProfilerDialogType		*wpf;
	WindowObjectType		*wo;

	int				nstn, nfiles, iret;
	int				i, verbose;

	XmPushButtonCallbackStruct *call_data = 
			(XmPushButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("ProfilerIconCB\n");

/*
 *	Get dialog structure.
 */
	wpf = GetGuiProfilerDialog();
	wo = GetActiveWindowObject();
	
/*
 *	Enguage busy cursor.
 */
	SetupAbortProcess( wo );

/*
 *	Get date/times and files.
 */
	BuildProfilerList ( nfiles, &(wpf->dattim_list) );

/*
 *	Cancel busy cursor.
 */
	CancelAbortProcess( wo, False );

}


void
GetProfilerFiles ( char ***snfile, char *begin_time, int duration,
		   ListParentObjectType *dattim_list, int *nfiles )
/*
 *  Get a list of files which will be needed to plot the cross section.
 */
{
	AbsTime		int_time, list_time;
	int		i, j=0, iret;
	int		verbose;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    {
	    printf ( "GetProfilerFiles [duration %d]\n", duration );
	    printf ( "GetProfilerFiles [listcound %d]\n", dattim_list->listcount );
	    printf ( "begin time %s\n", begin_time);
	    }

	for ( j=0; j<duration; j++ ) {
	    for ( i=0; i<dattim_list->listcount; i++ ) {
/*
 *	Do comparisons in integer time.
 */
		int_time = yyyymmddhhmm2sec ( begin_time );
		int_time += j * SECONDS_PER_HOUR;
		list_time = yyyymmddhhmm2sec ( dattim_list->label[i] );
		/*if ( int_time == list_time ) {*/
		if ( ( int_time > ( list_time - SECONDS_PER_HOUR ) ) &&
		     ( int_time < ( list_time + SECONDS_PER_HOUR ) ) ) {
		    if( verbose > VERBOSE_1 )
		        {
			printf("adding %d ?\n", i );
			printf("adding %s ?\n", dattim_list->label[i] );
			printf("adding %s ?\n", dattim_list->string[i] );
			}
/*
 *	Add only new file to list.
 */
		    AddString ( 300, nfiles, snfile, dattim_list->string[i] );
		    break;
		}
	    }
	}

}


char *
ProfilerTimeInterval ( char *time_int )
{
	int	interval, hhh, mm;
	char	buf[10], *retstr;

	interval = str2int ( time_int );

	hhh = interval / 60;
	mm  = interval % 60;

	sprintf ( buf, "%3d%2d", hhh, mm );
	ReplaceChar ( buf, ' ', '0' );
	retstr = strdup ( buf );
	return retstr;
}


char *
ProfilerLabelInterval ( int duration )
{
	int	interval, hhh, mm;
	char	buf[10], *retstr;

	interval = (duration * 60) / 8;

	hhh = interval / 60;
	mm  = interval % 60;

	sprintf ( buf, "%3d%2d", hhh, mm );
	ReplaceChar ( buf, ' ', '0' );
	retstr = strdup ( buf );
	return retstr;
}


void
ProfilerCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
/*
 *  This function is called when the user clicks on "Display" from the
 *  wind profiler dialog.
 */
{
	extern Widget		main_shell;

	MetObjectType		*mlist[MAXMETOBJS];
	GuiProfilerDialogType	*wpf;
	WindowObjectType	*wo;
	GlobalDataObjectType	*gd;
	GeoRefObjectType	*geo;

	char			**cxstns    ,	**snfiles = NULL;
	char			*wind_symbol,	*symbol_size,	*symbol_width,
				*fint       ,	*bottom     ,	*top         ,
				*begin_time ,	*end_time   ,   *duration    ,
				*time_int   ,   *hhhmm      ,   *xlabel_inc,
				*ylabel_inc;
	char			dattim[80], taxis[80], yaxis[80], text[81];
	char			*buf;
	AbsTime			int_begin, int_end;
	int			viewType;
	int			i, j;
	int			sw_count, *sw_index, int_duration, nfiles=0;
	int			titleIndex;
	int			iret, verbose;

	XmPushButtonCallbackStruct *call_data = 
			(XmPushButtonCallbackStruct *) xt_call_data;

	
	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "ProfilerCB\n" );

	wpf = GetGuiProfilerDialog();
	gd  = GetGlobalDataObject();
	wo  = GetActiveWindowObject();

/*
 *	Get highlighted position indices from list widget.
 */
	XmListGetSelectedPos ( wpf->station_list.w, &sw_index, &sw_count );
	if ( sw_count <= 0 ) {
	    InfoPop ( main_shell, INFORMATION,
	    "\nSorry boss, you need to choose\nat least one station.\n",
	    FALSE, FALSE, NULL, NULL );
	    
	    return;
	}

/*
 *	File Selection widgets count starting at 1.
 */
	AdjustIndexOffset ( sw_count, sw_index );

/*
 *	Check on the view types for compatibility. Compare this object 
 *	with the current georeference view state and do a reset if
 *	they don't match.
 */
	viewType  = GetMetObjectView( WINDPROFILEROBJECT );

	if ( geo = GetGeoRefObject( wo) )
		if ( geo->viewType != viewType ) WindowObjectFullReset( wo );


	cxstns = StringListAllocate ( sw_count );
	for ( i=0; i<sw_count; i++ ) {
	    cxstns[i] = (char *) malloc ( STIDSIZE * sizeof( char ) + 1 );
	    sscanf ( wpf->station_list.label[sw_index[i]], "%s", cxstns[i] );
	}

	Free ( sw_index);

	if( verbose > VERBOSE_1 )
	    for ( i=0; i<sw_count; i++ )
	    	printf ( "station = %s\n", cxstns[i] );

	if ( buf = XmTextGetString ( GetProfilerBottomW ( wpf ) ) ) {
	    bottom = strdup ( buf );
	    XtFree ( buf );
	    FreeProfilerBottom ( wpf );
	    SetProfilerBottom ( wpf, bottom );
	}
	else
	    bottom = strdup (" ");

	if ( buf = XmTextGetString ( GetProfilerTopW ( wpf ) ) ) {
	    top = strdup ( buf );
	    XtFree ( buf );
	    FreeProfilerTop ( wpf );
	    SetProfilerTop ( wpf, top );
	}
	else
	    top = strdup (" ");

	if ( buf = XmTextGetString ( GetProfilerIncrementW ( wpf ) ) ) {
	    ylabel_inc = strdup ( buf );
	    XtFree ( buf );
	    FreeProfilerIncrement ( wpf );
	    SetProfilerIncrement ( wpf, ylabel_inc );
	}
	else
	    ylabel_inc = strdup (" ");

	if ( buf = XmTextGetString ( GetProfilerEndTimeW ( wpf ) ) ) {
	    end_time = strdup ( buf );
	    XtFree ( buf );
	    FreeProfilerEndTime ( wpf );
	    SetProfilerEndTime ( wpf, end_time );
	}
	else
	    end_time = strdup (" ");

	if ( buf = XmTextGetString ( GetProfilerDurationW ( wpf ) ) ) {
	    duration = strdup ( buf );
	    XtFree ( buf );
	    FreeProfilerDuration ( wpf );
	    SetProfilerDuration ( wpf, duration );
	}
	else
	    duration = strdup (" ");

	if ( buf = XmTextGetString ( GetProfilerTimeIntervalW ( wpf ) ) ) {
	    time_int = strdup ( buf );
	    hhhmm = ProfilerTimeInterval ( time_int );
	    XtFree ( buf );
	    FreeProfilerTimeInterval ( wpf );
	    SetProfilerTimeInterval ( wpf, hhhmm );
	}
	else {
	    time_int = strdup (" ");
	    hhhmm = strdup ("00000");
	}

	sprintf( yaxis, "%s/%s/%s/.1;0;1", bottom, top, ylabel_inc );
/*
 *	Get date/time stamps for time range and time axis.
 */
	if ( CheckDateTimeString ( end_time ) ) {
	    int_end = yyyymmddhhmm2sec ( end_time );
	    int_duration = str2int ( duration );
	    int_begin = int_end - int_duration * SECONDS_PER_HOUR;
	    begin_time = sec2yyyymmddhhmm ( int_begin );
	    sprintf ( dattim, "%s-%s-%s", begin_time, end_time, hhhmm );
	    if( verbose > VERBOSE_0 )
	       {
	        char *testme;
	 	testme = sec2yyyymmddhhmm ( int_end );
               printf ( "test end time %s \n", testme);
               printf ( "end time %s \n", end_time);
               printf ( "duration %d \n", int_duration);
               printf ( "sec per hour %d \n", SECONDS_PER_HOUR);
               printf ( "dattim time %s \n", dattim);
	       }
/*
 *	Figure out a reasonable label frequency.
 */
	xlabel_inc = ProfilerLabelInterval ( int_duration );

/*
 *	Add an hour to end_time label so barbs won't overlay axis.
 */
	    int_end = int_end + SECONDS_PER_HOUR;
	    Free ( end_time );
	    end_time = sec2yyyymmddhhmm ( int_end );
	    sprintf ( taxis, "%s-%s-%s;1;0;1",
			      end_time+2, begin_time+2, xlabel_inc );
	}
	
/*
 *	Get a list of files to be read.
 */
	GetProfilerFiles ( &snfiles, begin_time, int_duration,
			   &(wpf->dattim_list), &nfiles );
	if( verbose > VERBOSE_1 )
	    printf( "  nfiles = %d\n", nfiles );

	if ( nfiles <= 0 ) {
	    printf("  Warning: no profiler files were found\n");
	    InfoPop ( main_shell, WARNING,
	              "\nWarning: no profiler files were found\n",
	              FALSE, FALSE, NULL, NULL );
	    return;
	}

	if( verbose > VERBOSE_1 )
	    for ( i=0; i<nfiles; i++ )
	    	printf( " file = %s\n", snfiles[i] );
/*
 *	Set font characteristics.
 */
	sprintf ( text, "%s/%s/%s/%s", gd->text_size, gd->text_font,
	                               gd->text_width, gd->text_hw_flag );

/*
 *	Presently hardwired.
 */
	wind_symbol = strdup ( "bk1" );
	symbol_size = strdup ( "1.0" );
	symbol_width  = strdup ( "1.5" );
	fint   = strdup ("1000/-1000/0");

	if ( verbose > 2 ) {
	    printf ("  begin_time = %s\n", begin_time );
	    printf ("  end_time = %s\n", begin_time );
	    printf ("  taxis = %s\n", taxis );
	    printf ("  yaxis = %s\n", yaxis );
	}

	titleIndex = GetNextTitleIndex ( wo );

/*
 *	Loop over stations. Loop over number of files.
 */
	for ( i=0; i<sw_count; i++ ) {
	    mlist[i] = (MetObjectType *)
		MakeWindProfilerObject ( cxstns[i],		snfiles,
					 nfiles,		dattim,
					 yaxis,			taxis,
					 wind_symbol,		symbol_size,
					 symbol_width,		fint,
					 text,                  titleIndex );
	}
/*
 *	Free.
 */
	Free ( begin_time );
	Free ( bottom );
	Free ( duration );
	Free ( end_time );
	Free ( fint );
	Free ( time_int );
	Free ( hhhmm );
	Free ( xlabel_inc );
	Free ( ylabel_inc );
	Free ( top );
	Free ( wind_symbol );
	Free ( symbol_size );
	Free ( symbol_width );

	StringListFree ( sw_count, cxstns );
	StringListFree ( nfiles, snfiles );

	AddObjectListToDisplay( sw_count, mlist );
	if( wo )
		ProcessDisplayList( wo );

	return;	
}

