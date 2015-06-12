/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	timeutil.h
 *
 *	Time utility declarations
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#ifndef __TIMEUTIL_H
#define __TIMEUTIL_H

#include <time.h>


typedef struct {
	char		begin_time[12];
	char		end_time[12];
	char		current_time[12];
} TimeObjectType;



/* Macros */

#define SECONDS_PER_MINUTE		( 60 )
#define SECONDS_PER_HOUR		( 3600 )
#define SECONDS_PER_DAY			( SECONDS_PER_HOUR * 24 )

#define SetCaseBeginTime(g,c)		(strcpy( (g)->begin_time, (c) ))
#define GetCaseBeginTime(g)		((g)->begin_time)

#define SetCaseEndTime(g,c)		(strcpy( (g)->end_time, (c) ))
#define GetCaseEndTime(g)		((g)->end_time)

#define SetCaseCurrentTime(g,c)		(strcpy( (g)->current_time, (c) ))
#define GetCaseCurrentTime(g)		((g)->current_time)


/* Prototypes */

TimeObjectType     *GetTimeObject();

int           yymmddhhmm2time ( char *time_str, struct tm **tm, time_t *timet );
struct tm     *yymmddhhmm2tm ( char *time_str );
time_t        yymmddhhmm2sec ( char *time_str );
char          *sec2yymmddhhmm ( time_t sec );


#endif   /*  __TIMEUTIL_H  */
