/***********************************************************************
 *
 *	Copyright 1997, University Corporation for Atmospheric Research.
 *
 *	math.c
 *
 *	Math utilities.
 *
 *	History:
 *
 *	 8/97	COMET	Original copy
 *
 ***********************************************************************/

#include "gendef.h"
#include "_proto.h"


int
IntMode ( int *intTimes, int num )
{
/*
 * Return the arithmetic mode of an integer array.
 */
	int		cnt, maxCount, retVal;
	int		*bin1, *bin2;
	int		i, j;
	BooleanType	match;

	if ( num < 1 ) return -1;

	bin1 = (int *) malloc (sizeof(AbsTime) * num );
	bin2 = (int *) malloc (sizeof(AbsTime) * num );

/*
 *	Count up number of bins.
 */
	bin1[0] = 1;
	bin2[0] = intTimes[0];
	maxCount = 1;
	match = FALSE;
	for ( i=1; i<num; i++ ) {
	    for ( j=0; j<maxCount; j++ )
		if ( bin2[j] == intTimes[i] ) {
		    match = TRUE;
		    bin1[j] = bin1[j] + 1;
		    break;
		}
	    if ( ! match ) {
		bin1[maxCount] = 1;
		bin2[maxCount] = intTimes[i];
		maxCount++;
		match = FALSE;
	    }
	}

/*
 *	All the values were the same so just return the first.
 */
	if ( maxCount == 1 )
	    retVal = intTimes[0];
	else {

/*
 *	Find the largest bin.
 */
	    cnt = 0;
	    for ( i=1; i<maxCount; i++ )
	    	if ( bin1[i] > bin1[cnt] )
		    cnt = i;

	    retVal = bin2[cnt];
	}

	Free ( bin1 );
	Free ( bin2 );

	return retVal;
}
