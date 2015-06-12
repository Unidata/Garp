/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gemgetgridtimes.c
 *
 *	Gets GEMPAK grid times from grid file.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include <string.h>
#include "winobj.h"
#include "underscore.h"
#include "_proto.h"


char **GemGetGridTimes ( char *fileName,  int *numTimes )
{

	int	iflno;
	int	ntimes;
	int	iret;
	char	**times;
	char	nextTime[128];
	int	i;
	ntimes = 0;
	iret = 0;

        grid_numtimes ( fileName, &ntimes, &iret, strlen(fileName) );

	if( ntimes <= 0 )
		return( NULL );

	times = (char **) StringListAllocate( ntimes );

/*
 *	Get the times out of the file using Fortran based 
 *	subscripting.
 */
	for( i = 1; i <= ntimes; i++ ) {

		memset( nextTime, 0, sizeof(nextTime) );
        	grid_nexttime ( &i, nextTime, &iret, 
				sizeof(nextTime) - 1 );

		StringClean( nextTime );

		times[i - 1] = strdup( nextTime );

	}

	*numTimes = ntimes;

	return( times );
}
