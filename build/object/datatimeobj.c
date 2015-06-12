/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	timeobj.c
 *
 *	Time object.
 *
 *	History:
 *
 *	 8/97	COMET	Original copy
 *	 9/97	COMET	Added UpdateDataTimeObject()
 *	11/97	COMET	Fixed minor leak
 *	11/97	COMET	Replaced function InitDataTimeObject() with
 *			InitTimeMatchDialog().
 *	12/97	COMET	Changed IndicesOfMatchedTimes1() so model data
 *			will preferrentially exact time match.
 *	 1/98	COMET	Removed "=" from time match algorithms so for
 *			strict time matching, intervals must be less than
 *			those given in "Garp_defaults" to match.
 *	12/99	COMET	Implemented time in YYYYMMDD/HHMM.
 *
 ***********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "gendef.h"
#include "guimacros.h"
#include "_proto.h"
#include "timeutil.h"
#include "winobj.h"
#include "genglobs.h"

#define ANALYSIS	2
#define BADTIME		9999999

char ** ConvertToTimeStamp ( char **, int, int, int * );

int * MatchIndicesToList ( char **, int, int, int * );
int * IndicesOfMatchedTimes1 ( char **, int, int );
int * IndicesOfMatchedTimes2 ( char **, int, int );
int * AllIndicesOfMatchedTimes ( char **, int );
int * IntListDup ( int, char ** );


AbsTime String2LongTime ( char *, AbsTime * ) ;

void CreateDataTimes ( char **, int, int );
void SetLongDataTimes ( char **, int, AbsTime *, AbsTime *, AbsTime *, int );

char * GetDataTypeTemplate ( int );
char * sec2yyyymmddhhmm ( AbsTime );
char * TemplateTime ( char *, char * );
int GetobTypeMaxInterval ( int );
void SetDataTypeTemplate ();
void PrintIndexList ( int *, int );
void StringListFree( int cnt, char **m );



static DataTimeObjectType		dto; /* Used by *wo */

DataTimeObjectType *GetDataTimeObject()
{
	return( &dto );
}



void
CreateDataTimeObject ( WindowObjectType *wo, MetObjectType **mlist )
{
	DataTimeObjectType	*dto;
	char			**list;
	int			i, listSize, obType;

	dto = GetDataTimeObject();

	if ( dto->numTimes > 0 ) return;

	obType = mlist[0]->type;
	listSize = wo->numPixmapObjects;
	list = StringListAllocate ( listSize );
	
	for ( i=0; i<listSize; i++ ) {
	    list[i] = NULL;
	    if ( mlist[i]->timestamp != NULL );
		list[i] = strdup ( mlist[i]->timestamp );
	}

	CreateDataTimes ( list, listSize, obType );	

	StringListFree ( listSize, list );

	wo->dataTimeObject = dto;
}


/*
 *	This routine updates the DataTimeObject by cascading all the entries
 *	down one, and adding the new values to the end. Used by auto-update.
 */

void
UpdateDataTimeObject (  WindowObjectType *wo,
			AbsTime refTime,
			AbsTime fcstTime,
			AbsTime validTime )
{

	DataTimeObjectType	*dto;
	int			i;


	dto = wo->dataTimeObject;

	if ( GetVerboseLevel() > VERBOSE_0 )
		printf ("UpdateDataTimeObject\n");


	if ( ! dto || dto->numTimes == 0 ) return;
	
	for ( i = 1; i < dto->numTimes; i++ ) {
	
	    dto->refTimes[i-1]   = dto->refTimes[i];
	    dto->fcstTimes[i-1]  = dto->fcstTimes[i];
	    dto->validTimes[i-1] = dto->validTimes[i];
	}
	
	dto->refTimes[dto->numTimes-1]   = refTime;
	dto->fcstTimes[dto->numTimes-1]  = fcstTime;
	dto->validTimes[dto->numTimes-1] = validTime;


}


int *
MatchIndicesToList ( char **list, int size, int obType, int *matchSize )
{
/*
 * Return a list of indices for times which match the underlying 
 * depictable.
 */
	DataTimeObjectType	*dto;
	int			*outlist;
	int			interval;

	dto = GetDataTimeObject();

	if ( GetVerboseLevel() > VERBOSE_0 )
		printf ("MatchIndicesToList\n");
/*
 *	Get maximum time interval for time match for this obType.
 */
	interval = GetobTypeMaxInterval ( obType );

/*
 *	Apply current time matching algorithm.
 */
	switch ( dto->matchType ) {
	case NOTIMEMATCH:
	  *matchSize = size;
	  outlist = IntListDup ( size, list );
	  break;
	case INTERVAL_ON:
	  *matchSize = dto->numTimes;
	  outlist = IndicesOfMatchedTimes1 ( list, size, interval );
	  break;
	case INTERVAL_OFF:
	  *matchSize = dto->numTimes;
	  outlist = AllIndicesOfMatchedTimes ( list, size );
	  break;
	default:
	  *matchSize = 0;
	  break;
	}

	return outlist;
}


int *
IntListDup ( int size, char **list )
{
/*
 * Return an integer array comprising indices of a string list.
 */
	int			*outlist;
	int			i;

/*
 *	Allocate maximum string list size.
 */
	outlist = malloc ( sizeof(int) * size );

        for ( i=0; i<size; i++ )
	    outlist[i] = i;

	return outlist;
}


int *
IndicesOfMatchedTimes1 ( char **list, int size, int interval )
{
/*
 * Return indices of date/time stamps which has the same number of
 * elements as the original list. If no time match is found for a given
 * time that element is demarked by a -1.
 */

	DataTimeObjectType	*dto;
	AbsTime			intTime, fcstTime;
	AbsTime			compTime, savTime;
	int			*outlist;
	int			i, j, bestIndex;


	dto = GetDataTimeObject();

/*
 *	Allocate maximum string list size.
 */
	outlist = malloc ( sizeof(int) * dto->numTimes );

/*
 *	Step through both list and save times which match.  Mark times
 *	which do not match as NULL.
 */
        for ( i=0; i<dto->numTimes; i++ ) {

	    savTime = BADTIME;
	    bestIndex = -1;
/*
 *	Find the closest time match. For model data, find an exact
 *	time match, if possible.
 */
	    for ( j=0; j<size; j++ ) {

		intTime = String2LongTime ( list[j], &fcstTime );
/*
 *	Look for an exact time match first.
 */
		if ( dto->refTimes[i] == intTime &&
	             dto->fcstTimes[i] == fcstTime ) {
	    	    savTime = compTime;
	    	    bestIndex = j;
		    break;
		}
/*
 *	Check for a valid time. If a time match equals a previous one,
 *	use the most recent.
 */
		if ( intTime > 0 ) {
	    	    intTime += fcstTime;
		    compTime = labs ( intTime - dto->validTimes[i] );
		    if ( compTime < interval && compTime <= savTime ) {
			savTime = compTime;
			bestIndex = j;
		    }
		}
	    }

	    outlist[i] = bestIndex;
	}

	return outlist;
}


int *
IndicesOfMatchedTimes2 ( char **list, int size, int interval )
{
/*
 * Return indices of date/time stamps which has the same number of
 * elements as the original list. If no time match is found for a given
 * time that element is demarked by a -1.
 */

	DataTimeObjectType	*dto;
	AbsTime			intTime, fcstTime;
	AbsTime			compTime, savTime;
	int			*outlist;
	int			i, j, bestIndex;


	dto = GetDataTimeObject();

/*
 *	Allocate maximum string list size.
 */
	outlist = malloc ( sizeof(int) * dto->numTimes );

/*
 *	Step through both list and save times which match.  Mark times
 *	which do not match as NULL.
 */
        for ( i=0; i<dto->numTimes; i++ ) {

	    savTime = BADTIME;
	    bestIndex = -1;
/*
 *	Find the closest time match. For model data, prefer the most
 *	recent data over an exact time match.
 */
	    for ( j=0; j<size; j++ ) {

		intTime = String2LongTime ( list[j], &fcstTime );
/*
 *	Check for a valid time. If a time match equals a previous one,
 *	use the most recent.
 */
		if ( intTime > 0 ) {
	    	    intTime += fcstTime;
		    compTime = labs ( intTime - dto->validTimes[i] );
		    if ( compTime < interval && compTime <= savTime ) {
			savTime = compTime;
			bestIndex = j;
		    }
		}
	    }

	    outlist[i] = bestIndex;
	}

	return outlist;
}


int *
AllIndicesOfMatchedTimes ( char **list, int size )
{
/*
 * Return indices of date/time stamps which has the same number of
 * elements as the original list. If no time match is found for a given
 * time that element is demarked by a -1.
 */

	DataTimeObjectType	*dto;
	AbsTime			intTime, fcstTime;
	AbsTime			compTime, savTime;
	int			*outlist;
	int			i, j, bestIndex;


	dto = GetDataTimeObject();

/*
 *	Allocate maximum string list size.
 */
	outlist = malloc ( sizeof(int) * dto->numTimes );

/*
 *	Step through both list and save times which match.  Mark times
 *	which do not match as NULL.
 */
        for ( i=0; i<dto->numTimes; i++ ) {

	    savTime = BADTIME;
	    bestIndex = -1;
/*
 *	Find the closest time match.
 */
	    for ( j=0; j<size; j++ ) {

		intTime = String2LongTime ( list[j], &fcstTime );
/*
 *	Check for a valid time. If a time match equals a previous one,
 *	use the most recent.
 */
		if ( intTime > 0 ) {
	    	    intTime += fcstTime;
		    compTime = labs ( intTime - dto->validTimes[i] );
		    if ( compTime < savTime ) {
			savTime = compTime;
			bestIndex = j;
		    }
		}
	    }

	    outlist[i] = bestIndex;
	}

	return outlist;
}


void
CreateDataTimes ( char **list, int size, int obType )
{
/*
 * Convert a list of date/time strings to long integers. Store these
 * in 3 long integer arrays. Note: validTime = refTime + fcstTime.
 */
	DataTimeObjectType	*dto;
	AbsTime			*refTimes, *fcstTimes, *validTimes;
	int			i;

	dto = GetDataTimeObject();

/*
 *	Allocate.
 */
	refTimes   = malloc (sizeof (AbsTime) * size );
	fcstTimes  = malloc (sizeof (AbsTime) * size );
	validTimes = malloc (sizeof (AbsTime) * size );

/*
 *	Initialize.
 */
	for ( i=0; i<size; i++ ) {
	    refTimes[i]   = 0;
	    fcstTimes[i]  = 0;
	    validTimes[i] = 0;
	}

/*
 *	Store reference, forecast and valid times in AbsTime arrays.
 */
	SetLongDataTimes ( list, obType, refTimes, fcstTimes,
	                                           validTimes, size );

	dto->refTimes   = refTimes;
	dto->fcstTimes  = fcstTimes;
	dto->validTimes = validTimes;
	dto->numTimes   = size;
}


void
PrintDataTimeObject ( DataTimeObjectType *dto )
{
/*
 * Print out the contents of a DataTimeObject.
 */
	int	i;

	for ( i=0; i<dto->numTimes; i++ ) {
	    printf("refTimes = %d  ", dto->refTimes[i] );
	    printf("fcstTimes = %d  ", dto->fcstTimes[i] );
	    printf("validTimes = %d\n", dto->validTimes[i] );
	}
}


void
ClearDataTimeObject ( WindowObjectType *wo )
{
/*
 * Free elements of a static DataTimeObject owned by a WindowObjectType.
 */
	DataTimeObjectType	*dto;

	if ( GetVerboseLevel () > VERBOSE_0 )
		printf ("ClearDataTimeObject\n");

/*
 *	Sanity check.
 */
	if ( ! wo->dataTimeObject ) return;
	dto = wo->dataTimeObject;

/*
 *	Since the DataTimeObject is static, we need to test if the
 *	elements of it have already been freed.
 */
	if ( dto->numTimes < 1 ) return;

	Free ( dto->refTimes );
	Free ( dto->fcstTimes );
	Free ( dto->validTimes );

	dto->numTimes = 0;
}


DataTimeObjectType *
AllocateDataTimeObject ( int size )
{
	DataTimeObjectType	*dto;

	dto = (DataTimeObjectType *) malloc ( sizeof( DataTimeObjectType ) );
	dto->refTimes   = malloc (sizeof (AbsTime) * size );
	dto->fcstTimes  = malloc (sizeof (AbsTime) * size );
	dto->validTimes = malloc (sizeof (AbsTime) * size );
	dto->numTimes   = size;

	return dto;
}


void
FreeDataTimeObject ( DataTimeObjectType *dto )
{
/*
 * Free a previously allocated DataTimeObject.
 */

	if ( GetVerboseLevel () > VERBOSE_0 )
		printf ("FreeDataTimeObject\n");

	if ( dto->numTimes < 1 ) return;

	dto->numTimes = 0;

	Free ( dto->refTimes );
	Free ( dto->fcstTimes );
	Free ( dto->validTimes );
	Free ( dto );
}


void
SetLongDataTimes ( char **list, int obType, AbsTime *refTimes,
	               AbsTime *fcstTimes, AbsTime *validTimes, int numTimes )
{
/*
 * Create integer lists of reference, forecast and valid times from
 * a date/time string. Note: validTime = refTime + fcstTime.
 */
	AbsTime		time, offset;
	int		i;
	
	if ( GetVerboseLevel () > VERBOSE_0 )
		printf ("SetLongDataTimes\n");
/*
 *	Convert each item in the list to long integers. Assume list is in 
 *	the format YYYYMMDD/NNSS.
 */

	for ( i=0; i<numTimes; i++ ) {
	    time = String2LongTime ( list[i], &offset );
	    refTimes[i]   = time;
	    fcstTimes[i]  = offset;
	    validTimes[i] = time + offset;
	}
}


int
GetobTypeMaxInterval ( int obType )
{
	int	retVal;
	char	*str;

	switch (obType) {
	case SURFACEOBJECT:
	  str = GetConfigValue( "SfcObsTimeInterval" );
	  retVal = str2int ( str );
	  Free ( str );
	  break;
	case RADAROBJECT:
	  str = GetConfigValue( "RadarTimeInterval" );
	  retVal = str2int ( str );
	  Free ( str );
	  break;
	case NOWRADOBJECT:
	  str = GetConfigValue( "RadarTimeInterval" );
	  retVal = str2int ( str );
	  Free ( str );
	  break;
	case SATOBJECT:
	  str = GetConfigValue( "SatelliteTimeInterval" );
	  retVal = str2int ( str );
	  Free ( str );
	  break;
	case UPPERAIROBJECT:
	  str = GetConfigValue( "UpaObsTimeInterval" );
	  retVal = str2int ( str );
	  Free ( str );
	  break;
	case SCALARGRIDFILLOBJECT:
	  retVal = IntrinsicForecastInterval();
	  break;
	case SCALARGRIDOBJECT:
	  retVal = IntrinsicForecastInterval();
	  break;
	case VECTORGRIDOBJECT:
	  retVal = IntrinsicForecastInterval();
	  break;
	default:
	  retVal = SECONDS_PER_HOUR;
	  break;
	}

	if ( GetVerboseLevel() > VERBOSE_1 )
		printf( "GetobTypeMaxInterval - time interval = %d\n", retVal );
	return retVal;
}


void
PrintIndexList ( int *ind, int size )
{
	int	i;

	for ( i=0; i<size; i++ )
	    printf( "%d\n", ind[i] );

	printf("\n");
}


char **
ConvertToTimeStamp ( char **list, int size, int obType, int *listSize )
{
/*
 * Use the file template for a given data type to convert a string with the
 * date/time information imbedded in it to a date time stamp of the form
 * YYYYMMDD/HHNN.
 */
	char 	**outlist;
	char	*template, *dattim;
	int	i;

	if ( GetVerboseLevel() > VERBOSE_0 )
		printf ( "ConvertToTimeStamp\n" );

/*
 *	Get date/time template for this data type.
 */
	template = GetDataTypeTemplate ( obType );

/*
 *	No character interpretation of the template is required.
 */
	if ( template == NULL ) {
	    *listSize = size;
	    outlist = StringListDup ( size, list );
	    return outlist;
	}

/*
 *	Interpret date/time strings from template.
 */
	else {
	    outlist = StringListAllocate ( size );
	    *listSize = size;

	    for ( i=0; i<size; i++ ) {
		outlist[i] = NULL;
		dattim = TemplateTime ( list[i], template );
		if ( dattim != NULL ) {
		    outlist[i] = strdup ( dattim );
		    Free ( dattim );
		}
	    }
	}

/*
 *	Free.
 */
	Free ( template );

	return outlist;
}
