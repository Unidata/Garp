/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gdinfobj.c
 *
 *	Grid information object. This object extracts and buffers grid 
 *	information for each model type.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	11/97	COMET	Fixed small leak
 *	12/97	COMET	Bug fix in GetVcoordGridLevels().
 *	12/97	COMET	Changed a verbose block to VERBOSE_4.
 *	 1/98	COMET	Added a couple casts
 *
 ***********************************************************************/


#include <stdio.h>
#include <strings.h>

#include "underscore.h"
#include "genglobs.h"
#include "winobj.h"
#include "gdinfo.h"
#include "model.h"
#include "_proto.h"


char *GetGridInfoText( char *fileName )
{
	int				verbose;
	static GridInfoObjectType	*gio = NULL;
	char				*p=NULL;

	SetupAbortProcess( GetActiveWindowObject() );
	gio = CreateGridInfoObject( fileName );
	CancelAbortProcess( GetActiveWindowObject(), False );

	if ( gio )
           {
	   p = (*gio->textFunc)( gio );
	   (*gio->destroyFunc)( gio );
	   }

	return( p );

}

	
ProcessGridInfo( char *fileName )
{

	int				verbose;
	static GridInfoObjectType	*gio = NULL;
	char				*p;

	verbose = GetVerboseLevel();

	if( gio )
		(*gio->destroyFunc)( gio );
		
	SetupAbortProcess( GetActiveWindowObject() );
	gio = CreateGridInfoObject( fileName );
	CancelAbortProcess( GetActiveWindowObject(), False );

	if( ! gio )
		return( 0 );

	if( verbose > VERBOSE_0 ) {

		(*gio->printfFunc)( gio );

		printf("  Levels:\n");
		IntegerTablePrintf( gio->numLevels, gio->levels );

		printf("  Vert Coord:\n");
		StringTablePrintf( gio->numVerticalCoordinates, 
				gio->verticalCoordinates );

		printf("  Fields:\n");
		StringTablePrintf( gio->numGridFields, gio->gridFields );

		printf("  Times:\n");
		StringTablePrintf( gio->numTimes, gio->times );
	}

	if( IsGridFieldOnThisVCoord( gio, "RELH", "PRES" ) )
		printf("IsGridFieldAtThisVCoord: RELH is on PRES.\n");
	else
		printf("IsGridFieldAtThisVCoord: RELH is NOT on PRES.\n");

	if( IsGridFieldOnThisVCoord( gio, "I AM", "PRES" ) )
		printf("IsGridFieldAtThisVCoord: I AM is on PRES.\n");
	else
		printf("IsGridFieldAtThisVCoord: I AM is NOT on PRES.\n");

	if( IsGridFieldAtThisLevel( gio, "RELH", 900 ) )
		printf("IsGridFieldAtThisVCoord: RELH is on 900.\n");
	else
		printf("IsGridFieldAtThisVCoord: RELH is NOT on 900.\n");

	if( IsGridFieldAtThisLevel( gio, "RELH", 666 ) )
		printf("IsGridFieldAtThisVCoord: RELH is on 666.\n");
	else
		printf("IsGridFieldAtThisVCoord: RELH is NOT on 666.\n");

	p = (*gio->textFunc)( gio );
	if( p )
		free( p );

	return;
}
 
/*
 *
 *	Simplistic routine that takes in a time from a gempak grid
 *	file and returns a pointer to the forecast interval part
 *	of the time by searching for the first "F" or "f".
 *
 *	Example
 *		960426/1200F000   --->   F000
 *
 *		960426/0000F012   --->   F012
 *
 *		960426/0000f024   --->   f012
 *
 *		960426F/000F012   --->   F/000F012 (hope something like
 *			this doesn't happen since we don't check for it:).
 *
 *		960426/0000       --->   NULL
 *
 *
 */
char *ParseOffForecast( char *gridTime )
{

	char	*p;

	if ( p = strchr( gridTime, (int) 'F' ) )
		return( p );
		
	return( strchr( gridTime, (int) 'f' ) );

}
	
	
GridInfoObjectType *CreateGridInfoObject( char *fileName )
{
	int			numberGrids;
	int			maxPossibleGrids;
	int			gridFileID;
	int			iret;
	int			gridCounter;
	GridInfoObjectType	*gio;
	GridType		*gt;

/*
 *	Character Strings passed to GEMPAK fortran subroutines.  When passing
 *	the length of the string to GEMPAK, subtract out one for the NULL.
 *
 */
	char	lastTime[GEMPAKSTRING];
	char	time1[GEMPAKSTRING];
	char	time2[GEMPAKSTRING];
	char	gridField[GEMPAKSTRING];

	int	levels[2];
	int	jvcoordinate;  /* may not be right declaration - check common */
	int	verbose;

	verbose = GetVerboseLevel();
	if ( verbose > VERBOSE_0 )
	    printf ("CreateGridInfoObject\n" );

	memset( lastTime,  0, sizeof( lastTime ) );
	memset( time1,     0, sizeof( time1 ) );
	memset( time2,     0, sizeof( time2 ) );
	memset( gridField, 0, sizeof( gridField) ); 


	gdinfo( fileName, lastTime, &numberGrids, &maxPossibleGrids, 
			&gridFileID, &iret, strlen( fileName ), 
			sizeof( lastTime ) - 1);

	if( iret )
		return( NULL );

/*
 *	Allocate all the memory...
 */
	gio = (GridInfoObjectType *) malloc( sizeof(GridInfoObjectType) );
	gio->gridList = (GridType *) malloc( numberGrids * 
				sizeof( GridType) );

/*
 *	....and then check for errors.
 */
	if( ! gio || ! gio->gridList ) {
		printf("*** CreateGridInfoObject: malloc failure.\n");
		return( NULL );
	}

/*
 *	Intialize the object.
 */
	gt = gio->gridList;
	memset( gt, 0, numberGrids * sizeof( GridType) ); 

	gio->fileName                   = strdup( fileName );
	gio->lastTime                   = strdup( lastTime );
	gio->numberOfGrids              = numberGrids;
	gio->maxPossibleGrids           = maxPossibleGrids;
	gio->destroyFunc                = GridInfoObjectDestroy;
	gio->printfFunc                 = GridInfoObjectPrintf;
	gio->textFunc                   = GridInfoObjectToText;

	gio->numLevels                  = 0;
	gio->levels                     = NULL;

	gio->numTimes                   = 0;
	gio->times                      = NULL;

	gio->numVerticalCoordinates	= 0;
	gio->verticalCoordinates	= NULL;

	gio->numGridFields	        = 0;
	gio->gridFields	                = NULL;

	gio->numForecastIntervals	= 0;
	gio->forecastIntervals	        = NULL;
	
	if ( verbose > VERBOSE_1 ) {
		printf("  file is %s\n", fileName );
		printf("  numberGrids %d,", numberGrids );
		printf(" maxPossibleGrids %d,", maxPossibleGrids );
		printf(" iret %d\n", iret );
	}

	iret = 0;
	for( gridCounter = 1; gridCounter <= numberGrids; gridCounter++, gt++ ){

		getnxtgrd( &gridFileID, &gridCounter, gt->time1, gt->time2, 
				gt->levels, gt->verticalCoordinate, 
				gt->gridField, &iret, sizeof(gt->time1) - 1, 
				sizeof(gt->time2) - 1,
				sizeof(gt->verticalCoordinate) - 1,
				sizeof(gt->gridField) - 1 );

		if( iret  ) {
			printf("  CreateGridInfoObject: getnxtgrd ret %d\n", 
					iret );
			break;
		}
		
		StringClean( gt->time1 );
		StringClean( gt->time2 );
		StringClean( gt->gridField ); 
		StringClean( gt->verticalCoordinate ); 
		
		if ( verbose > VERBOSE_3 ) {
			printf("  Grid # %d:\n", gridCounter );
			printf("  time1 %s,", gt->time1 );
			printf(" time2 %s,", gt->time2 );
			printf(" lev1 %d,", gt->levels[0] );
			printf(" lev2 %d,", gt->levels[1] );
			printf(" jv %s,", gt->verticalCoordinate );
			printf(" gridField %s\n", gt->gridField );
		}

		AddString( gio->numberOfGrids, &gio->numForecastIntervals, 
				&gio->forecastIntervals, 
				ParseOffForecast(gt->time1) );

		AddString( gio->numberOfGrids, &gio->numTimes, 
				&gio->times, gt->time1 );

		AddString( gio->numberOfGrids, &gio->numGridFields, 
				&gio->gridFields, 
				gt->gridField );

		AddString( gio->numberOfGrids, 
				&gio->numVerticalCoordinates, 
				&gio->verticalCoordinates, 
				gt->verticalCoordinate );

		AddInteger( gio->numberOfGrids, &gio->numLevels, 
				&gio->levels, gt->levels[0] );

	} /* for(;;) */

/*
 *	Cleanup and go home if there was an error.
 */
	if( iret ) {
		(*gio->destroyFunc)( gio );
		gio = NULL;
	}

/*
 *	Close the gridfile for next time...
 */
	gd_clos( &gridFileID, &iret );

	return( gio );

}

BooleanType IsGridFieldOnThisVCoord( GridInfoObjectType *gio, 
			char *gridField, char *vCoord )
{

	int		i;
	GridType	*gt;

	if( ! gio )
		return( (BooleanType)NULL );
	
	for( i = 0, gt = gio->gridList; i < gio->numberOfGrids; i++, gt++ )

		if( EQSTRING( gt->verticalCoordinate, vCoord ) )

			if( EQSTRING( gt->gridField, gridField ) )

				return( True );

	return( False );

}
BooleanType 
IsGridFieldAtThisLevel( GridInfoObjectType *gio, char *gridField, int level )
{

	int		i;
	GridType	*gt;

	if( ! gio )
		return( (BooleanType)NULL );
	
	for( i = 0, gt = gio->gridList; i < gio->numberOfGrids; i++, gt++ )

		if( gt->levels[0] == level )

			if( EQSTRING( gt->gridField, gridField ) )

				return( True );

	return( False );

}

	
char **GetAllGridVerticalCoordinates( GridInfoObjectType *gio, 
				int *numElements )
{

	*numElements = 0;
	if( ! gio )
		return( NULL );

	*numElements = gio->numVerticalCoordinates;
	return( gio->verticalCoordinates );

}

int GetNumForecastIntervals( GridInfoObjectType *gio )
{

	if( ! gio )
		return( 0 );

	return( gio->numForecastIntervals );
}

char **GetAllForecastIntervals( GridInfoObjectType *gio, int *numElements )
{

	*numElements = 0;
	if( ! gio )
		return( NULL );

	*numElements = gio->numForecastIntervals;
	return( gio->forecastIntervals );

}

char **GetAllGridTimes( GridInfoObjectType *gio, int *numElements )
{

	*numElements = 0;
	if( ! gio )
		return( NULL );

	*numElements = gio->numTimes;
	return( gio->times );
}

int GetNumTimes( GridInfoObjectType *gio )
{

	if( ! gio )
		return( 0 );

	return( gio->numTimes );
}

char **GetAllGridFields( GridInfoObjectType *gio, int *numElements )
{

	*numElements = 0;
	if( ! gio )
		return( NULL );

	*numElements = gio->numGridFields;
	return( gio->gridFields );
}

int GetNumGridFields( GridInfoObjectType *gio )
{

	if( ! gio )
		return( 0 );

	return( gio->numGridFields );
}

int *GetAllGridLevels( GridInfoObjectType *gio, int *numElements )
{

	*numElements = 0;
	if( ! gio )
		return( NULL );

	*numElements = gio->numLevels;
	return( gio->levels );

}
int GetVcoordGridLevels( GridInfoObjectType *gio,
			 char ***levels1, char ***levels2 )
/*
 * Get the number of levels as well as arrays for two levels given
 * in a GEMPAK file.
 */
{
	ModelInfoType	*mdl;

	char		*model, *vcoord;
	char		**vcoords;
	char		**p1, **p2;
	int		i, num, numElements = 0;
	int		verbose;
	BooleanType	flag = False;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
		printf ("GetVcoordGridLevels\n" );
    
	mdl = GetGuiModelInfo();

/*
 *	Get the current model type and vertical coordinate.
 */
	model = strdup ( GetModelType ( mdl ) );
	vcoord = strdup ( GetModelVCoord ( mdl ) );

/*
 *	Check current vcoord against a list of valid vertical coordinates.
 */
	vcoords = GetAllGridVerticalCoordinates ( gio, &num );
	for ( i=0; i<num; i++ )
		if ( strcasecmp ( vcoord, vcoords[i] ) == 0 ) flag = True;

	if ( !flag ) {
	    Free ( model );
	    Free ( vcoord);
	    *levels1 = NULL;
	    *levels2 = NULL;
	    return numElements;
	}

/*
 *	Create levels lists for this vertical coordinate. Allow for
 *	duplication to speed processing - we'll filter later.
 */
	*levels1 = StringListAllocate ( gio->numberOfGrids );
	*levels2 = StringListAllocate ( gio->numberOfGrids );
	p1 = *levels1;
	p2 = *levels2;
	for ( i=0; i<gio->numberOfGrids; i++ ) {
		if ( strcasecmp ( vcoord,
		    gio->gridList[i].verticalCoordinate ) == 0 ) {
		    *p1 = int2str ( gio->gridList[i].levels[0] );
		    *p2 = int2str ( gio->gridList[i].levels[1] );
		    p1++; p2++; numElements++;
		}
	}

/*
 *	Free.
 */
	Free ( model );
	Free ( vcoord );

	return numElements;
}

int GetNumGridLevels( GridInfoObjectType *gio )
{

	if( ! gio )
		return( 0 );

	return( gio->numLevels );
}

int AddString(int maxEntries, int *numEntries, 
		char ***stringTable, char *newElement)
{

	char			*fv;
	int			i;


/*
 *	Check to see if we have a string to add, this is not technically
 *	an error. 
 */
	if( ! newElement || ! *newElement )
		return( 0 );

/*
 *	Allocate a table big enough for the maximum possible
 *	entries.  In reality, this is way too big, but there is
 *	no efficient way to guess how many we will need as the
 *	the file is being read by GEMPAK.
 *
 */
	if( ! *stringTable ) {

		*stringTable = 
			(char **) malloc( sizeof(char **) *  maxEntries + 1 );

		memset( *stringTable, 0, sizeof(char **) * maxEntries + 1 );

		*numEntries = 0;
	}

/*
 *	Saftey check for table overrun....
 */
	if( *numEntries >= maxEntries) {
		printf("*** AddString:  table overrun.\n");
		return( -1 );
	}

/*
 *	Loop through the table, looking for matches.  Just return on
 *	a match otherwise bail out and add the newElement to the end
 *	of the list.
 *
 */
	for( i = 0; i < *numEntries; i++ )
		if( EQSTRING( (*stringTable)[i], newElement ) )
			return( 0 );


/*
 *	Append the new entry to the table.
 */
	(*stringTable)[i] = strdup( newElement );
	(*numEntries)++;

	return( 1 );

}

int AddInteger(int maxEntries, int *numEntries, int **intTable, int newElement)
{

	int			i;

/*
 *	Allocate a table big enough for the maximum possible
 *	entries.  In reality, this is way too big, but there is
 *	no efficient way to guess how many we will need as the
 *	the file is being read by GEMPAK.
 *
 */
	if( ! *intTable ) {
		*intTable = (int *) malloc( sizeof(int) *  maxEntries );
		for( i = 0; i < maxEntries; i++ )
			(*intTable)[i] = -9999;

		*numEntries = 0;
	}
			

/*
 *	Saftey check for table overrun....
 */
	if( *numEntries >= maxEntries ) {
		printf("*** AddInteger:  table overrun.\n");
		return( -1 );
	}

/*
 *	Loop through the table, looking for matches.  Just return on
 *	a match otherwise bail out and add the newElement to the end
 *	of the list.
 *
 */
	for( i = 0; i < *numEntries && (*intTable)[i] != -9999; i++ )
		if( (*intTable)[i] == newElement )
			return( 0 );


	(*intTable)[i] = newElement;
	(*numEntries)++;

	return( 1 );

}

void StringTablePrintf( int numElements, char **elements )
{
	int		i;

	for( i = 0; i < numElements; i++ )
		printf("  %s\n", elements[i] );

	return;

}

void IntegerTablePrintf( int numElements, int *elements )
{
	int		i;

	for( i = 0; i < numElements; i++ )
		printf("  %d\n", elements[i] );

	return;

}


void GridInfoObjectDestroy( GridInfoObjectType *gio )
{
	if( ! gio )
		return;

/*
 *	String dup's...
 */
	free( gio->fileName );
	free( gio->lastTime );

/*
 *	String tables...
 */
	if( gio->gridFields && gio->numGridFields )
		StringListFree( gio->numGridFields, gio->gridFields );

	if( gio->times && gio->numTimes )
		StringListFree( gio->numTimes, gio->times );

	if( gio->forecastIntervals && gio->numForecastIntervals )
		StringListFree( gio->numForecastIntervals, 
				gio->forecastIntervals );
		
	if( gio->verticalCoordinates && gio->numVerticalCoordinates )
		StringListFree( gio->numVerticalCoordinates, 
				gio->verticalCoordinates );

/*
 *	Array's of "things" allocated as one big chunk...
 */
	if( gio->levels )
		free( gio->levels );

	if( gio->gridList )
		free( gio->gridList );

/*
 *	Main object....
 */
	free( gio );

	return;

}

char *GridInfoObjectToText( GridInfoObjectType *gio )
{
	int		byteCount;
	int		i;
	int		verbose;
	GridType	*gt;
	char		*allText;
	int		lineCount;
	char		*tileFormatLine =
"Count     Time1            Time2           Level1   Level2    VCoord     Field\n\n";

	
	verbose = GetVerboseLevel();

	byteCount = lineCount = 0;

	allText = malloc( (EXTRA_LINES + gio->numberOfGrids) * 
				CHARS_PER_LINE );

	if( ! allText ) {
		printf("*** GridInfoObjectToText:  Malloc failed\n");
		return( NULL );
	}

	byteCount += sprintf(allText, 
		 "File is %s.\nNumber of Grids %d, maxPossibleGrids %d.\n",
		 gio->fileName, gio->numberOfGrids, 
		 gio->maxPossibleGrids );
	lineCount++;


	byteCount += sprintf( &(allText[byteCount]), tileFormatLine );
	lineCount++;

	for( i = 0, gt = gio->gridList; i < gio->numberOfGrids; i++, gt++ ) {

		byteCount += sprintf(&(allText[byteCount]), 
			"%5d %15s %15s       %5d    %5d   %6s    %6s\n", 
			i, gt->time1, gt->time2, gt->levels[0], gt->levels[1],
			gt->verticalCoordinate, gt->gridField );

		lineCount++;

	}

	if ( verbose > VERBOSE_1 ) {
		printf( "  GridInfoObjectToText:  %d lines", lineCount );
		printf( "%d bytes\n", byteCount );
		printf( "  %s\n", allText );
	}

	return( allText );

}

void GridInfoObjectPrintf( GridInfoObjectType *gio )
{

	int		i;
	GridType	*gt;

	printf("  File is %s,", gio->fileName );
	printf(" numberGrids %d,", gio->numberOfGrids );
	printf(" maxPossibleGrids %d.\n", gio->maxPossibleGrids );

	for( i = 0, gt = gio->gridList; i < gio->numberOfGrids; i++, gt++ ) {
		printf("  Grid # %d:\n", i );
		printf("  time1 %s,", gt->time1 );
		printf(" time2 %s,", gt->time2 );
		printf(" lev1 %d,", gt->levels[0] );
		printf(" lev2 %d,", gt->levels[1] );
		printf(" jv %s,", gt->verticalCoordinate );
		printf(" gridField %s\n", gt->gridField );
	}

	return;

}
