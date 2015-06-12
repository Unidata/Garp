/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	timeutil.c
 *
 *	Date/Time conversion utilities.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	11/97	COMET	Added routines GetCentury, DayOfWeek (based on
 *			moddate.f), MonthOfYear and MakeForecastDattim.
 *	12/97	COMET	Added ClosestIndexTimeMatch.
 *	12/99	COMET	Changed dattims kept internally in GARP from
 *			YYMMDD/HHMM to YYYYMMDD/HHMM. Removed datemake()
 *			and makedate() functions. Added tm2yyyymmddhhmm().
 *			Changed String2LongTime() to use yyyymmddhhmm2sec.
 *
 ***********************************************************************/


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "timeutil.h"
#include "gendef.h"
#include "children.h"
#include "underscore.h"
#include "_proto.h"


int
yyyymmddhhmm2time ( char *time_str, struct tm **tm, AbsTime *timet )
{

	struct tm	*tm_ptr, *tp;
	AbsTime		t1;
	AbsTime		tmpYear;
	int		tzoffset;
	char		slash[2];

/*
 *	Setup the return values.
 */
	*tm = (struct tm *) NULL;
	*timet = (AbsTime) -1;

/*
 *	Allocate the tm strucutre.  This must be freed by the
 *	calling program when they are done with it.
 */
	if (( tm_ptr = (struct tm *) malloc ( sizeof(struct tm))) == NULL ) {
	    printf("yyyymmddhhmm2time: malloc failed, errno is %d\n", errno);
	    return ( 0 );
	}

/*
 *	Copy and convert the elements of the string out into
 *	the tm structure.
 */
	memset ( tm_ptr, 0, sizeof ( struct tm ) );
	sscanf ( time_str, "%04d%02d%02d%1c%02d%02d",
		&tmpYear,
		&tm_ptr->tm_mon,
		&tm_ptr->tm_mday,
		slash,
		&tm_ptr->tm_hour,
		&tm_ptr->tm_min );

/*
 *	Adjust year to number of years since 1900.
 */
	tm_ptr->tm_year = tmpYear - 1900;

/* 
 *	In Unix time, months are counted [0-11] so decrement month.
 */
	tm_ptr->tm_mon--;

/*
 *	Force Unix clock to figure out Daylight Savings setting.
 */
	tm_ptr->tm_isdst = -1;

/*
 *	Fillout the tm structure and get the seconds.
 */
	tm_ptr->tm_sec = 0;
	t1 = mktime ( tm_ptr );

#if 0
{
AbsTime lt, gt;
struct tm *tt;
lt = time(NULL);
tt = gmtime(&lt);
gt = mktime(tt);
tzoffset = difftime (gt, lt);
}
/* 	tzoffset = get_tz(); */

	
	printf("Timezone offset is %d\n", tzoffset);
	t1 += tzoffset;
	tp = gmtime ( &t1 );
	*tm_ptr = *tp;
#endif /* 0 */

/*
 *	Check for errors.
 */
	if ( t1 == (AbsTime) -1 ) {
	    printf("yyyymmddhhmm2time: Can't convert string %s to tm structure\n", 
			time_str);
	    free ( tm_ptr );
	    return ( 0 );
	}

/*
 *	Stuff the return paramaters.
 */
	*timet = t1;
	*tm = tm_ptr;

/*
 *	OK status.
 */
	return ( 1 );

}   /*   yyyymmddhhmm2time()   */

/*
 *	Convert string to "tm" structure.
 */
struct tm *
yyyymmddhhmm2tm ( char *time_str )
{
	struct tm	*tm_ptr;
	AbsTime		t1;

	yyyymmddhhmm2time ( time_str, &tm_ptr, &t1 );
	return ( tm_ptr );

}  /*  yyyymmddhhmm2tm()  */
	
/*
 *	Convert string to AbsTime value.
 */
AbsTime 
yyyymmddhhmm2sec ( char *time_str )
{
	struct tm	*tm_ptr;
	AbsTime		t1;

	yyyymmddhhmm2time ( time_str, &tm_ptr, &t1 );
	free ( tm_ptr );
	return ( t1 );

}  /*  yyyymmddhhmm2sec()  */

/*
 *	Convert seconds to char string using format: YYYYMMDD/HHMM .
 */
char *
sec2yyyymmddhhmm ( AbsTime sec )
{
	struct tm	time_struct;
	struct tm	*tm_ptr;
	char		line[256];
	int		n;

	tm_ptr = gmtime ( &sec );
	time_struct = *tm_ptr;
	n = strftime ( line, 256, "%Y%m%d/%H%M", &time_struct);
	if ( n == 0 )
	    return ( (char *) NULL );

	return (strdup ( line ) );

}

char *
tm2yyyymmddhhmm ( struct tm *timeptr )
{
	int	tmpYear;
	char	*dattim;
	char	slash='/';

	dattim = ( char * ) malloc ( 14 );

	
	tmpYear = timeptr->tm_year + 1900;

	sprintf ( dattim, "%04d%02d%02d%1c%02d%02d",
		 	tmpYear,
			timeptr->tm_mon+1,
			timeptr->tm_mday,
			slash,
			timeptr->tm_hour,
			timeptr->tm_min );
	return dattim;
}


#ifdef TESTIT
main(argc, argv)
int argc;
char **argv;
{
	struct tm	*t;
	AbsTime		t1;

	yyyymmddhhmm2time ( argv[1], &t, &t1 );

	printf("Converting %s\n", argv[1]);
	printf("Converted to seconds %d\n", t1);
	printf("Converted to tm structure seconds %d\n", t->tm_sec);
	printf("Converted to tm structure min %d\n", t->tm_min);
	printf("Converted to tm structure hour %d\n", t->tm_hour);
	printf("Converted to tm structure mday %d\n", t->tm_mday);
	printf("Converted to tm structure mon %d\n", t->tm_mon);
	printf("Converted to tm structure year %d\n", t->tm_year);
	printf("Converted to tm structure wday %d\n", t->tm_wday);
	printf("Converted to tm structure yday %d\n", t->tm_yday);
	printf("Converted to tm structure isdst %d\n", t->tm_isdst);
	printf("Asc time is %s", asctime(t));
}
#endif
	

AbsTime
String2LongTime ( char *buf, AbsTime *offset ) 

/************************************************************************
* 									*
* This program convert a date/time stamp to a long integer referenced	*
* in Unix time.								*
*									*
* Input parameters:							*
*	*date		char *	Date/time stamp string			*
*									*
* Output parameters:							*
*	lt		AbsTime	Time in seconds from reference		*
*									*
* Example: date = 19950802/1200  ->  lt = 810068400			*
*									*
************************************************************************/
{
	char		*fcst;
	char		slash[2], date[64];
	struct tm	timeptr;
	AbsTime		tmpYear;
	AbsTime		tz_time;
	AbsTime		lt;

/*---------------------------------------------------------------------*/

/*
 *	Use GMT time.
 */
	putenv ("TZ=GMT0");

	*offset = 0;
	if ( buf == NULL ) return 0;
	strcpy ( date, buf );

/*
 *	Check to see if we have a valid date/time stamp.
 */

/*
	if ( ! date || ! CheckDateTimeString( date ) ) {
	    if ( GetVerboseLevel() > VERBOSE_0 )
		printf("ch_to_int_time\n");

	    if ( GetVerboseLevel() > VERBOSE_1 ) {
	    	printf("  Date must be in YYYYMMDD/HHMM, given '%s'.\n",
				date ? date : "" );
	    }

	    return -1;
	}
*/

/*
 *	Pull forecast time off of string.
 */
	if ( strchr ( date, 'f' ) || strchr ( date, 'F' ) ) {
	    ToUpper ( date );
	    fcst = strsaveafter ( date, 'F' );
	    *offset = str2int ( fcst );
	    *offset *= SECONDS_PER_HOUR;
	    Free ( fcst );
	}
	
	lt = yyyymmddhhmm2sec ( buf );

	return lt;
}


int
GetCentury ( int year )
{
	int	century;

	century = 19;
	if ( year < 50 ) century = 20;

	return century;
}


char *
DayOfWeek ( char *dattim )
{
	char	*outday;
	char	*cday[]={"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
	int	iret, idayw, itime[5], len;

/*
 *	Set the output time to the input time in case anything fails
 */
        outday = strdup ( dattim );

/*
 *	Convert to integer time array, determine day number using GEMPAK
 *	routines.
 */
	len = strlen ( dattim );
	ti_ctoi ( dattim, itime, &iret, len );
	if ( iret != 0 ) return outday;

	ti_dayw ( itime, &idayw, &iret );
	if ( iret != 0 || idayw < 1 || idayw > 7 ) return outday;

	strcpy ( outday, cday[idayw-1] );

	return outday;
}


char *
MonthOfYear ( int imon )
{
	char	*retmon;
	char	*month[]={"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	                  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	retmon = strdup ( month[imon-1] );

	return retmon;
}


char *
MakeForecastDattim ( char *ftime )
{
/*
 * Build a date/time string in which the forecast time is added to the 
 * reference time. Use this string for titles.
 */
	char		*dattim, *dateTime, *date, *time, *fcst;
	AbsTime		validTime, refTime, fcstTime;
	int		i;

	if ( ftime == NULL ) return NULL;
	if ( strcmp ( ftime, "MISSING" ) == 0 ) {
	    dattim = strdup ( "MISSING" );
	    return dattim;
	}

	dattim = ( char * ) malloc ( strlen (ftime) + 1 );

	refTime   = String2LongTime( ftime, &(fcstTime) );
	validTime = refTime + fcstTime;

	dateTime = sec2yyyymmddhhmm ( validTime );  /* e.g. 19970102/1200   */
	date     = strsavebefore ( dateTime, '/' ); /* e.g. 19970102        */
	time     = strsaveafter ( dateTime, '/' );  /* e.g.        1200     */
	fcst     = strsaveafter ( ftime, '/' );     /* e.g.        1200F012 */
	if (! strndel ( fcst, 4 ) )                 /* e.g.            F012 */
	    strcpy ( fcst, "" );

	sprintf ( dattim, "%s%s%s%s", date, "/", time, fcst );

	for ( i=0; i<strlen(dattim); i++ ) {
	    if ( dattim[i] == 'f' ) dattim[i] = 'v';
	    if ( dattim[i] == 'F' ) dattim[i] = 'V';
	}

	Free ( dateTime );
	Free ( date );
	Free ( time );
	Free ( fcst );

	return dattim;
}


int
ClosestIndexTimeMatch ( ListParentObjectType *list_info, char *dattim )
{
/*
 * Return the index to the time in the list which has the closest match to 
 * the input time.
 */
	int		retIndex;
	int		i, bestIndex=0;
	AbsTime		refTime1, refTime2, fcstTime1, fcstTime2;
	AbsTime		compTime, savTime;
	AbsTime		interval=99999999;


	refTime1 = String2LongTime ( dattim, &fcstTime1 );

	for ( i=0; i<list_info->listcount; i++ ) {

	    refTime2 = String2LongTime ( list_info->label[i], &fcstTime2 );
/*
 *	First look for an exact time match.
 */
            if ( refTime1 == refTime2 && fcstTime1 == fcstTime2 ) {
                savTime = compTime;
                retIndex = i;
                break;
            }

/*
 *	Now look for the closest time match.
 */
            if ( refTime2 > 0 ) {
                refTime2 += fcstTime2;
                compTime = labs ( refTime2 - refTime1 );
                if ( compTime <= interval && compTime <= savTime ) {
                    savTime = compTime;
                    bestIndex = i;
                }
            }

            retIndex = bestIndex;
        }

        return retIndex;
	    
}
