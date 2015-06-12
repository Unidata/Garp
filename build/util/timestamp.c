/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	timestamp.c
 *
 *	Date/Time reading utilities.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 8/97	COMET	Removed TimestampInfoType related code.
 *	11/99	COMET	Himes.  Added support for 4 digit year and
 *			added more range checking on date string.
 *			Moved CheckDateTimeString() from stringutils.c 
 *			to here
 *	12/99	COMET	Changed time_t data type to AbsTime.
 *
 ***********************************************************************/


#include <time.h>

#include "guimacros.h"
#include "gendef.h"
#include "genglobs.h"
#include "utils.h"
#include "winobj.h"
#include "_proto.h"

char * TemplateTime ( char *, char * );
char * GetDataTypeTemplate ( int );


/************************************************************************
* CheckDateTimeString							*
*									*
* This function checks the format of a date/time string to insure it	*
* is of the format YYMMDD/HHMM or YYYYMMDD/HHMM.			*
*									*
*									*
* Input parameters:							*
*	*timestr	char	Input string				*
*									*
* Output parameters:							*
*	CheckDateTimeString	boolean		True if okay.		*
*									*
************************************************************************/
BooleanType
CheckDateTimeString ( char *timestr )
{

	BooleanType		check = TRUE;
	int			i;
	int		numDigitsInYear;
	int		lengthOfTimeString;

/*
 *	Make sure we have a non-null string pointer...
 */
	if( ! timestr ) 
		return FALSE;

/*
 *	Check string length.  Date
 *	   YYMMDD/HHMM   OR  YYYYMMDD/HHMM
 */
	switch(lengthOfTimeString = strlen ( timestr )) {
	case 11:
		numDigitsInYear = 2;
		break;
	case 13:
		numDigitsInYear = 4;
		break;
	default:
		return FALSE;
	}


/*
 *	Check for a '/' .
 */
	if ( timestr[numDigitsInYear + 4] != '/' ) 
		return FALSE;

/*
 *	Check for a number at each digit.
 */
	for ( i=0; i < numDigitsInYear + 4; i++ )
	    if ( isdigit ( timestr[i] ) == 0 ) check = FALSE;

	for ( i = numDigitsInYear + 4 + 1; i < lengthOfTimeString; i++ )
	    if ( isdigit ( timestr[i] ) == 0 ) check = FALSE;

	return check;
}

/*
 *
 *	Write date/timestamp imbedded in file name to integer time
 *      in "Unix" time format...that is:
 *         seconds since 00:00:00 UTC, January 1, 1970)
 *
 */
int
FileToTimestamp ( int metObjectType, char *file )
{
	int	int_time;
	int	iret;
	char	dattim[32];
	char	template[256];
	char	*tmp;

/*
 *	Error return code.
 */
	int_time = -1;
	template[0] = '\0';

/*
 *	Objects which are listed as date/time fields rather than file names.
 */
	if ( metObjectType == SURFACEOBJECT ) {
            strcpy ( dattim, file );
            int_time = ch_to_int_time ( dattim );
	    return int_time;
	}
/*
 *	Get file template.
 */
	GetFileTemplateByType ( metObjectType, template );

	if(  *template ) {
	    	template_time ( file, template, dattim );
	    	int_time = ch_to_int_time ( dattim );
	} else {
	    	printf("*** Could not define timestamp for metObjectType = %d\n",
	            	metObjectType);
	}

	return int_time;

}


/*
 *
 * Write time/date stamp imbedded in file name to a string with format
 * YYYYMMDD/HHMM .
 *
 */
char *
FileToDattim ( int obType, char *file )
{
	int	int_time;
	int	iret;
	char	*template;
	char	*dattim;
	char	*tmp;


/*
 *	Get file template.
 */
	template = GetDataTypeTemplate ( obType );

/*
 *	Get time string.
 */
	switch ( obType ) {
	case SURFACEOBJECT:
	  dattim = strdup ( file );
	  break;
	case RADAROBJECT:
	  dattim = TemplateTime ( file, template );
	  break;
	case SATOBJECT:
	  dattim = TemplateTime ( file, template );
	  break;
	case NOWRADOBJECT:
	  dattim = TemplateTime ( file, template );
	  break;
	case UPPERAIROBJECT:
	  dattim = strdup ( file );
	  break;
	case SCALARGRIDOBJECT:
	  dattim = TemplateTime ( file, template );
	  break;
	case VECTORGRIDOBJECT:
	  dattim = TemplateTime ( file, template );
	  break;
	default:
	  printf("*** Timestamp undefined for plot_type = %d\n", obType );
	  dattim = NULL;
	  break;
	}

	Free ( template );

	return dattim;
}


/*
 *	Write a list of date/timestamps in YYYYMMDD/HHMM from a list 
 *	of files.
 *
 *	Note: must free **dattims after use.
 */
int FilesToDattims ( int type, 
		char *dir, 
		char **files, 
		int num_files, 
		char ***dattims )
{
	char	template[512];
	char	file[256];
	char	dattim[32];

	char	**dattimp;
	char	*tmp;
	int	i;


	*template = '\0';
	*dattims  = NULL;

	GetFileTemplateByType ( type, template );

	if( ! *template ) {
		printf("*** FilesToDattims: No template for object type %d\n",
				type);
		return( 0 );
	}

/*
 *	Allocate an array of pointers.
 */
	*dattims = (char **) malloc (sizeof(char *) * (num_files + 1));

/*
 *	Loop through all the times, building a full path name, 
 *	and parsing out the timestamp from the file name.
 *
 */
	for ( i=0, dattimp = *dattims; i<num_files; i++ ) {
	    strcpy ( file, dir );
	    strcat ( file, "/" );
	    strcat ( file, files[i] );
	    template_time ( file, template, dattim );
	    *dattimp = strdup( dattim );
	    dattimp++;
	}

/*
 *      The list of pointers is terminate with  a NULL entry, so that 
 *      we can find the end of the list, without knowing the exact count.
 */
	*dattimp = (char *) NULL;

	return num_files;
}

int ch_to_int_time ( char *date ) 

{
	char		slash[2];
	struct tm	timeptr;
	AbsTime		tz_time;
	AbsTime		lt;

/*---------------------------------------------------------------------*/

/*
 *	Use GMT time.
 */
	putenv ("TZ=GMT0");

/*
 *	Check to see if we have a valid date/time stamp.
 */
	if ( ! date || ! CheckDateTimeString( date ) ) {
#if 0
		printf("ch_to_int_time\n");
	    	printf("  Date must be in YYMMDD/HHMM or YYYYMMDD/HHMM, given '%s'.\n",
				date ? date : "" );
#endif /* 0  */
	    return -1;
	}

/*
 *	Write date/time stamp to structure.
 */
	if( strlen(date) == 11 ) {
		sscanf ( date, "%02d%02d%02d%1c%02d%02d",
	    		&timeptr.tm_year,
	    		&timeptr.tm_mon,
	    		&timeptr.tm_mday,
	    		slash,
	    		&timeptr.tm_hour,
	    		&timeptr.tm_min);
	} else {
		sscanf ( date, "%04d%02d%02d%1c%02d%02d",
	    		&timeptr.tm_year,
	    		&timeptr.tm_mon,
	    		&timeptr.tm_mday,
	    		slash,
	    		&timeptr.tm_hour,
	    		&timeptr.tm_min);
/*
 *		32 bit Unix can only handle up to Jan 2038...
 */
		if( timeptr.tm_year < 1900 || timeptr.tm_year > 2037 ) {
	    		printf("ch_to_int_time: Invalid year, %d\n", 
				timeptr.tm_year);
			return(-1);
		}
/*
 *		tm_year is years since 1900...
 */
		timeptr.tm_year = timeptr.tm_year - 1900;
	}

/*
 *	In Unix time, months are counted [0-11] so decrement month.
 */
	timeptr.tm_mon--;
	if( timeptr.tm_mon < 0 || timeptr.tm_mon > 11 ) {
		printf("ch_to_int_time: Invalid month, %d\n", timeptr.tm_mon);
		return(-1);
	}

/*
 *	Range check days of month.  Sloppy check since some days don't
 *      have 31 days.
 */
	if( timeptr.tm_mday < 1 || timeptr.tm_mday > 31 ) {
		printf("ch_to_int_time: Invalid day of the month, %d\n", 
			timeptr.tm_mday);
		return(-1);
	}

/*
 *	Force Unix clock to figure out Daylight Savings setting.
 */
	timeptr.tm_isdst = -1;

/*
 *	Set remaining time pointer elements.
 */
	timeptr.tm_sec = 0;

/*
 *	Get time in seconds.
 */
	lt = mktime ( &timeptr );

	return lt;
}
