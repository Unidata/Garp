/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gdinfo.h
 *
 *	Grid file info declarations.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#ifndef __GDINFO_H
#define __GDINFO_H

#include "gendef.h"

#define		CHARS_PER_LINE	(80 + 2)
#define		EXTRA_LINES	(100)
#define		GEMPAKTIMESTAMP	(32)
#define		GEMPAKFIELD	(16)

typedef struct {

	char	time1[GEMPAKTIMESTAMP];   /* yymmddhhmmss/0000 f000 */
	char	time2[GEMPAKTIMESTAMP];   /* yymmddhhmmss/0000 f000 */
	char	gridField[GEMPAKFIELD];   /* tmpk */
	int	levels[2];
	char	verticalCoordinate[GEMPAKFIELD];  /* pres */

} GridType;
	

typedef struct {

/*
 *
 */
	char		*fileName;
	char		*lastTime;

/*
 *	All the grid information.
 */
	int		numberOfGrids;
	int		maxPossibleGrids;
	GridType	*gridList;


/*
 *	List of unique gridFields, times, verticalCoordinates and levels.
 */
	int		numGridFields;
	char		**gridFields;

	int		numTimes;
	char		**times;

	int		numLevels;
	int		*levels;

	int		numVerticalCoordinates;
	char		**verticalCoordinates;

	int		numForecastIntervals;
	char		**forecastIntervals;

	void		(*destroyFunc)();
	void		(*printfFunc)();
	char		*(*textFunc)();

} GridInfoObjectType;
	

/* 
 *	Forward declarations of callback functions.
 */
char *GetGridInfoText( char *fileName );
GridInfoObjectType *CreateGridInfoObject( char *fileName );
BooleanType IsGridFieldOnThisVCoord( GridInfoObjectType *gio, 
			char *gridField, char *vCoord );
BooleanType IsGridFieldAtThisLevel( GridInfoObjectType *gio, 
			char *gridField, int level );
char **GetAllGridVerticalCoordinates( GridInfoObjectType *gio, 
				int *numElements );
char **GetAllGridTimes( GridInfoObjectType *gio, int *numElements );
int GetNumTimes( GridInfoObjectType *gio );
char **GetAllGridFields( GridInfoObjectType *gio, int *numElements );
int GetNumGridFields( GridInfoObjectType *gio );
int *GetAllGridLevels( GridInfoObjectType *gio, int *numElements );
int GetNumGridLevels( GridInfoObjectType *gio );
int AddString(int maxEntries, int *numEntries, 
		char ***stringTable, char *newElement);
int AddInteger(int maxEntries, int *numEntries, int **intTable, int newElement);
void StringTablePrintf( int numElements, char **elements );
void IntegerTablePrintf( int numElements, int *elements );
void GridInfoObjectDestroy( GridInfoObjectType *gio );
char *GridInfoObjectToText( GridInfoObjectType *gio );
void GridInfoObjectPrintf( GridInfoObjectType *gio );

int GetNumForecastIntervals( GridInfoObjectType *gio );
char **GetAllForecastIntervals( GridInfoObjectType *gio, int *numElements );

#endif
