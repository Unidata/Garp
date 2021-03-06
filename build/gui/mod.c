/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	mod.c
 *
 *	Utilities for accessing and buffering model gridded data information.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 7/97	COMET	Added model defined subdirectories.
 *	 8/97	COMET	Use arithmetic mode to determine forecast intervals.
 *	 8/97	COMET	Added RemoveModelListCacheEntry()
 *	 8/97	COMET	Added FilterForecastTimes.
 *	 8/97	COMET	Added GetModelConfigValue.
 *	10/97	COMET	Fixed bug in RemoveModelListCacheEntry()
 *	11/97	COMET	Fixed leak in BuildFieldList(), change the
 *			FilterDuplicate*() routines to return a list
 *			of the correct size. Fixed bug in SearchModelList..
 *			and RemoveModelListCacheEntry
 *	12/97	COMET	Fixed leak in BuildModelAnalysisList()
 *	12/97	COMET	Removed NextLine(). Added sanity tests to 
 *			FilterDuplicateLayers() and FilterDuplicateLevels().
 *	12/97	COMET	Added metObjectType, list_label args to BuildModel-
 *			ForecastList()
 *	12/97	COMET	Added a "select_list" object to track date/time
 *			selections when time matching is turned off.
 *	 1/98	COMET	Added sanity tests for undefined keys.
 *	 2/98	COMET	Added GetFilteredModelList
 *	 4/98	COMET	Fixed leak in BuildModelAnalysisList()
 *	 8/99	COMET	Redefined FDF model directory.
 *	12/99	COMET	Added function call Y2KList to put year in YYYY.
 *
 ***********************************************************************/


#include <strings.h>
#include <stdlib.h>
#include <assert.h>

#include "genglobs.h"
#include "guimacros.h"
#include "hsobj.h"
#include "model.h"
#include "_proto.h"
#include "winobj.h"
#include "utils.h"
#include "gdinfo.h"
#include "stringlist.h"
#include "menucb.h"


DataTimeObjectType * AllocateDataTimeObject ( int );
GridInfoObjectType *GetGridInfo();
void FilterForecastTimes ( ListParentObjectType * );

void templregex (char *instr, char *outstr);
void GetDatatypeList ( char *model, char **directory, char ***files, int *list_size );

extern int filter_dirlist ();

int UpdateGuiListStruct( char *fileName,
		int numDateTimestamps, 
		char **dateTimestamps,
		char **fileNameList,
		char **labelList );

int CheckMemoryAllocation( int initialAllocSize, 
				int	numEntriesToAdd, 
				int	numEntriesUsed,
				int	*numEntriesAllocated,
				char	***fileStringPtr,
				char	***labelStringPtr );


void
BuildOptionMenuButtons ( Widget pm,
			 char **labels, char **strings, int labelc,
	                 void (*func)(),
	                 ButtonParentObjectType *bpo )
{
	ButtonObject		*bo;

	int			i;


	if ( labels == NULL | strings == NULL ) return;

/*
 *	Initialize.
 */
	bpo->parent = pm;
	bpo->button_cnt = 0;
	bpo->button_alloc_cnt = 0;
	bpo->button_cnt = labelc;

/*
 *	Button labels are given by directory names.
 */
	bo = &(bpo->buttons[0]);
	for ( i = 0; i < labelc; i++, bo++ ) {

	    bo->w = XtCreateManagedWidget (labels[i],
			xmPushButtonWidgetClass,
			bpo->parent, NULL, 0 );

	    bo->ismanaged = 1;

	    XtAddCallback ( bo->w,
	                    XmNactivateCallback,
	                    func, bo );

	    bo->string = strdup ( strings[i] );

	    /* will get free'd when button destroyed */
	    bo->label = strdup ( labels[i] );  

	}

	bpo->button_cnt = labelc;
	bpo->button_alloc_cnt = bpo->button_cnt;
}


void
RemoveOptionMenuButtons ( Widget menu )
/*
 * Remove buttons from an option pulldown menu.
 */
{
	if( GetVerboseLevel() > VERBOSE_1 )
		printf("need to remove old buttons\n");
/*
    ButtonObject  *b =  grd_time->buttons;
    int i;

    for ( i = 0; i < bpo->button_cnt; i++, b++ ) {
        XtDestroyWidget ( b->w );
        b->ismanaged = 0;
        Free ( b->label );
    }
*/

    /*
     *  Reset the button count to reflect no buttons.
     */
/*
    bpo->button_cnt = 0;

*/
}


int
GetModelLevels ( char *model, char *coordinate, char  ***levels )
{
	ModelInfoType		*mdl;
	GridInfoObjectType	*gio;

	char	**ptr;
	int	num_levels = 0;
	int	i;

	mdl = GetGuiModelInfo();
	gio = GetGridInfo();

/*
 *	Allocate memory for forecast list.
 */
	*levels = (char **) malloc (MAX_LIST * sizeof( char * ) );

/*
 *	Store forecast list.
 */
	ptr = *levels;
	for ( i=0; i<mdl->levlist.listcount; i++ ) {
	    *ptr = strdup ( mdl->levlist.string[i] );
	    ptr++;
	}
	num_levels = i;
/*
 *	Null terminate list.
 */
	*ptr = (char *) NULL;

	return num_levels;
}


int
GetModelLayers ( char *model, char *coordinate, char  ***layers )
{
	ModelInfoType		*mdl;
	GridInfoObjectType	*gio;

	char	**ptr;
	int	num_layers = 0;
	int	i;

	mdl = GetGuiModelInfo();
	gio = GetGridInfo();

/*
 *	Allocate memory for forecast list.
 */
	*layers = (char **) malloc (MAX_LIST * sizeof( char * ) );

/*
 *	Store forecast list.
 */
	ptr = *layers;
	for ( i=0; i<mdl->layerlist.listcount; i++ ) {
	    *ptr = strdup ( mdl->layerlist.string[i] );
	    ptr++;
	}
	num_layers = i;
/*
 *	Null terminate list.
 */
	*ptr = (char *) NULL;

	return num_layers;
}


int
FilterDuplicateLayers ( int num,
			char **oldlist1, char **oldlist2,
			char ***newlist1, char ***newlist2 )
{
	char		**p1, **p2;
	char		**ptr1, **ptr2;
	int		numElements=0;
	int		i, j;
	BooleanType	flag;

/*
 *	Sanity test.
 */
	if ( num < 1 ) {
	    *newlist1 = NULL;
	    *newlist2 = NULL;
	    return numElements;
	}

/*
 *	Initialize temporary character arrays.
 */
	p1 = StringListAllocate ( num );
	p2 = StringListAllocate ( num );
	ptr1 = p1;
	ptr2 = p2;

/*
 *	Loop through old lists. Search for duplicates as you go.
 */
	for ( i=0; i<num; i++ ) {
	    flag = True;
	    for ( j=0; j<numElements; j++ ) {

/*
 *	If any match already exists in the new list, skip it.
 */
		if ( ( EQSTRING ( oldlist1[i], ptr1[j] ) ) &&
		     ( EQSTRING ( oldlist2[i], ptr2[j] ) ) )
		    flag = False;
	    }
	    	if ( EQSTRING ( oldlist2[i], "-1" ) )
		    flag = False;

	    if ( flag ) {
		*p1 = strdup ( oldlist1[i] );
		*p2 = strdup ( oldlist2[i] );
		p1++; p2++; numElements++;
	    }
	}

/*
 *	Make a copy of the new lists which reflects the correct size
 */
	*newlist1 = StringListDup ( numElements, ptr1 );
	*newlist2 = StringListDup ( numElements, ptr2 );
	
	StringListFree ( num, ptr1);
	StringListFree ( num, ptr2);
 	
	return numElements;
}


int
FilterDuplicateLevels ( int num,
			char **oldlist1, char **oldlist2,
			char ***newlist1, char ***newlist2 )
{
	char		**p1, **p2;
	char		**ptr1, **ptr2;
	int		numElements=0;
	int		i, j;
	BooleanType	flag;

/*
 *	Sanity test.
 */
	if ( num < 1 ) {
	    *newlist1 = NULL;
	    *newlist2 = NULL;
	    return numElements;
	}

/*
 *	Initialize temporary character arrays.
 */
	p1 = StringListAllocate ( num );
	p2 = StringListAllocate ( num );
	ptr1 = p1;
	ptr2 = p2;

/*
 *	Loop through old lists. Search for duplicates as you go.
 */
	for ( i=0; i<num; i++ ) {
	    flag = True;
	    for ( j=0; j<numElements; j++ ) {

/*
 *	If any match already exists in the new list, skip it.
 */
		if ( ( EQSTRING ( oldlist1[i], ptr1[j] ) ) )
	    	    flag = False;
	    }
	    	if ( ! ( EQSTRING ( oldlist2[i], "-1" ) ) )
		    flag = False;

	    if ( flag ) {
		*p1 = strdup ( oldlist1[i] );
		*p2 = strdup ( oldlist2[i] );
		p1++; p2++; numElements++;
	    }
	}

/*
 *	Make a copy of the new lists which reflects the correct size
 */
	*newlist1 = StringListDup ( numElements, ptr1 );
	*newlist2 = StringListDup ( numElements, ptr2 );
	
	StringListFree ( num, ptr1);
	StringListFree ( num, ptr2);

	return numElements;
}


int
FilterDuplicatesList ( int num, char **oldlist, char ***newlist )
{
/*
 * Filter duplicate string entries in a string list. Memory for the 
 * new list is allocated within this function and must be freed after 
 * use.
 */
	char		**p, **ptr;
	int		numElements=0;
	int		i, j;
	BooleanType	flag;

/*
 *	Initialize character arrays.
 */
	p = StringListAllocate ( num );
	ptr = p;

/*
 *	Loop through old list. Search for duplicates as you go.
 */
	for ( i=0; i<num; i++ ) {
	    flag = True;
	    for ( j=0; j<numElements; j++ ) {
/*
 *	If any match already exists in the new list, skip it.
 */
		if ( ( strcmp ( oldlist[i], ptr[j] ) == 0 ) )
		    flag = False;
	    }
	    if ( flag ) {
		*p = strdup ( oldlist[i] );
		p++; numElements++;
	    }
	}

/*
 *	Make a copy of the new lists which reflects the correct size
 */
	*newlist = StringListDup ( numElements, ptr );
	
	StringListFree ( num, ptr);

	return numElements;
}


typedef struct ModelListCacheStruct {
	char			*model;
	int			fileCount;
	char			*firstFilename;
	char			*lastFilename;

	int			numStrings;
	char			**fileStringPtr;
	char			**labelStringPtr;

	struct ModelListCacheStruct *next;

	GridInfoObjectType	*gio; 

	

} ModelListCacheType;

static ModelListCacheType *cacheHead = NULL;
	
	
static void AddToModelListCache( char *model,
			int fileCount, 
			char	*firstFilename, 
			char	*lastFilename,
			int 	numStrings, 
			char	**fileStringPtr,
			char	**labelStringPtr,
			GridInfoObjectType *gio );

static ModelListCacheType *AllocateCacheEntry(char *model,
			int  fileCount,
			char *firstFilename,
			char *lastFilename,
			int  numStrings,
			char **fileStringPtr,
			char **labelStringPtr,
			GridInfoObjectType *gio );

static void FreeCacheEntry( ModelListCacheType *cp );


void RemoveModelListCacheEntry( char *model );


static ModelListCacheType *SearchModelListCache( char *model,
			int fileCount, 
			char	*firstFilename, 
			char	*lastFilename );

static ModelListCacheType *SearchModelListCache( char *model,
			int fileCount, 
			char	*firstFilename, 
			char	*lastFilename )

{
	ModelListCacheType *cp;
	ModelListCacheType *previousCp;

	previousCp = NULL;
	for( cp = cacheHead; cp; cp = cp->next ) {

		if( EQSTRING( cp->model, model ) ) {

			if( cp->fileCount == fileCount && 
				*cp->firstFilename && 
				EQSTRING( cp->firstFilename, firstFilename) && 
				*cp->lastFilename && 
				EQSTRING( cp->lastFilename, lastFilename ) ) {

				/*
				 *  We have a cache hit.
				 */
				break;

			} else {

				/*
				 *  We have a cache hit, but the data is old.
				 *  Delete this entry and return NULL.
				 */
				if( previousCp )
					previousCp->next = cp->next;
				else
					cacheHead = cp->next;

				FreeCacheEntry( cp );
				cp = NULL;
				break;
			}
		}

		previousCp = cp;

	}

	return( cp );

}

static void FreeCacheEntry( ModelListCacheType *cp )
{

	GridInfoObjectType *gio;

	if( ! cp )
		return;

	if( cp->model )
		Free( cp->model );

	if( cp->firstFilename )
		Free( cp->firstFilename );

	if( cp->lastFilename )
		Free( cp->lastFilename );
	
	if( cp->numStrings > 0 && cp->fileStringPtr )
		StringListFree( cp->numStrings, cp->fileStringPtr );

	if( cp->numStrings > 0 && cp->labelStringPtr )
		StringListFree( cp->numStrings, cp->labelStringPtr );

	gio = cp->gio;
	if( gio && gio->destroyFunc )
		(*gio->destroyFunc)( gio );

/*
 *	And last but least...
 */
	Free ( cp );

}

void RemoveModelListCacheEntry( char *model)
{
	ModelListCacheType *cp;
	ModelListCacheType *previousCp;

	previousCp = NULL;
	for( cp = cacheHead; cp; cp = cp->next ) {

		if( EQSTRING( cp->model, model ) ) {

			if( previousCp )
				previousCp->next = cp->next;
			else
				cacheHead = cp->next;

			FreeCacheEntry( cp );
			break;

		}

		previousCp = cp;

	}

}

static ModelListCacheType *AllocateCacheEntry(char *model,
			int  fileCount,
			char *firstFilename,
			char *lastFilename,
			int  numStrings,
			char **fileStringPtr,
			char **labelStringPtr,
			GridInfoObjectType *gio )
{

	ModelListCacheType *cp;

	assert( cp = (ModelListCacheType *) malloc( sizeof( *cp ) ) );

	cp->fileCount     = fileCount;
	cp->numStrings    = numStrings;
	cp->model         = strdup( model );
	cp->firstFilename = strdup( firstFilename );
	cp->lastFilename  = strdup( lastFilename );
	cp->fileStringPtr = fileStringPtr;
	cp->labelStringPtr= labelStringPtr;
	cp->gio           = gio;

	return( cp );
}


static void AddToModelListCache( char *model,
			int fileCount, 
			char	*firstFilename, 
			char	*lastFilename,
			int 	numStrings, 
			char	**fileStringPtr,
			char	**labelStringPtr,
			GridInfoObjectType *gio )

{

	ModelListCacheType *cp;
	ModelListCacheType *newCp;

	
	newCp = AllocateCacheEntry(model, fileCount,
			firstFilename, lastFilename,
			numStrings, fileStringPtr, labelStringPtr, gio );
	
	assert( newCp );

	newCp->next = cacheHead;

	cacheHead = newCp;

	return;

}


void
GetModelLabelsFromList ( char *model,
	                 char *directory,
	                 char **files,
	                 int list_size,
	                 ListParentObjectType *list_struct )
{

	int			numEntriesAllocated;
	int			numForecastIntervals;

	char			**forecastIntervals;
	char			**dateTimestamps;

	int			i, j;
	int			ret;
	int			ntimes;

	char			fileName[1024];

	int			useCachedData;

	int			numEntriesUsed = 0;
	char			**fileStringPtr = NULL;
	char			**labelStringPtr = NULL;

#if 0
	static char 			firstFilename[1024];
	static char 			lastFilename[1024];

	static int			lastFileCount;
#endif

	GridInfoObjectType		*gio = NULL;
	ModelListCacheType *cp;

/*
 *	Intialize the output paramaters and local variables.
 */
	list_struct->listcount = 0;
	list_struct->string    = NULL;
	list_struct->label     = NULL;

/*	
 *	Early out.  No point in going on...
 *
 */
	if( list_size <= 0 )
		return;

	cp = SearchModelListCache( model, list_size, files[0],
			files[list_size -1] );
	if( cp ) {

		list_struct->listcount = cp->numStrings;

		list_struct->string = 
			StringListDup( cp->numStrings, cp->fileStringPtr);

		list_struct->label = 
			StringListDup( cp->numStrings, cp->labelStringPtr );

		gio = cp->gio;

/*
 *	Save present state of gio.
 */
		PutGio ( gio );

/*
 *	Set things which depend on model type such as the vertical
 *	coordinates, the number of vertical levels and lists for 
 *	level 1 and level 2.
 */ 
		SetModelLevels ();
		SetModelLayers ();

		return;
	}


/*
 *	This will be non-null if we have previously cached a
 *	gio for this fileName.
 */
	if( ! gio ) {
	    sprintf( fileName, "%s/%s", directory, files[0] );
	    gio = CreateGridInfoObject( fileName );

	    if( GetVerboseLevel() > VERBOSE_1 ) {
		printf("  GetModelLabelsFromList: Using ");
		printf("filename %s to get grid information\n",
					 fileName);
	    }
	}


	
/*
 *
 *	Get time/date stamp from file name. 
 *
 */ 
	numEntriesUsed = 0;
	numEntriesAllocated = 0;
	
	for(i = 0; i < list_size; i++){

/*
 *		Construct the full file name.
 */
		sprintf( fileName, "%s/%s", directory, files[i] );

/*
 *		Get all the date/timestamps out of this grid file.
 */
		dateTimestamps = 
			(char **) GemGetGridTimes ( fileName, &ntimes );

/*
 *		If we get an error, just blow by this file....
 */
		if( ntimes <= 0 || dateTimestamps == NULL )
			continue;

/*
 *		Dynamically allocate/grow the number of pointers we have
 *		allocated for the string lists.  The initial list size
 *		is a generous guess (assuming max. of 64 timestamps in 
 *		a file	multiplied by the number of files. 
 */
		ret = CheckMemoryAllocation( 
				64 * list_size + 1,
				ntimes,
				numEntriesUsed, 
				&numEntriesAllocated,
				&fileStringPtr, 
				&labelStringPtr );
	
/*
 *		Memory allocation errors are terminal...
 */
		if( ! ret ) {
			printf("GetModelLabelsFromList: Allocation error.\n");
			return;
		}

/*
 *		Add the entries to the list...
 */
		UpdateGuiListStruct( fileName, 
				ntimes, 
				dateTimestamps,
				&fileStringPtr[numEntriesUsed],
				&labelStringPtr[numEntriesUsed] ); 

/*
 *		Free the pointer array.  Note that the strings are
 *		now owned by labelStringPtr.
 */
		Free( dateTimestamps );

/*
 *		Update our counters and go again...
 */
		numEntriesUsed += ntimes;
		

	}
			

/*
 *	Null terminate the last entry to aide in traversing the list
 *	without out a counter.
 */
	fileStringPtr[numEntriesUsed] = NULL;
	labelStringPtr[numEntriesUsed]  = NULL;

/*
 *	Fill in the output paramaters.
 */
	list_struct->listcount = numEntriesUsed;
	list_struct->string   = StringListDup( numEntriesUsed, fileStringPtr);
	list_struct->label    = StringListDup( numEntriesUsed, labelStringPtr );

	AddToModelListCache( model, list_size, files[0], 
		files[list_size - 1 ], numEntriesUsed,
		fileStringPtr, labelStringPtr, gio );

/*
 *	Save present state of gio.
 */
	PutGio ( gio );

/*
 *	Set things which depend on model type such as the vertical
 *	coordinates, the number of vertical levels and lists for 
 *	level 1 and level 2.
 */ 
	SetModelLevels ();
	SetModelLayers ();

	return;

}


int CheckMemoryAllocation( int initialAllocSize, 
				int	numEntriesToAdd,
				int	numEntriesUsed,
				int	*numEntriesAllocated,
				char	***fileStringPtr,
				char	***labelStringPtr )
{

	int		byteCount;

/*
 *
 *	First time through both fileStringPtr and labelStringPtr will
 *	be NULL.
 *
 */
	if( ! *fileStringPtr ) {

		byteCount            = initialAllocSize * sizeof(char *);
		*fileStringPtr       = (char **) malloc( byteCount );
		*labelStringPtr      = (char **) malloc( byteCount );
		*numEntriesAllocated = initialAllocSize;

	}
			
/*
 *
 *	Check to see if the allocation size was incorrect at the start 
 *	and double our initial guess.
 *
 */
	if( *numEntriesAllocated <= numEntriesToAdd + numEntriesUsed ) {

		byteCount = (*numEntriesAllocated * 2 ) * sizeof( char * );

		*fileStringPtr  = realloc( *fileStringPtr, byteCount );
		*labelStringPtr = realloc( *labelStringPtr, byteCount );

		printf("CheckMemoryAllocation: reallocation required\n");
		printf("   %d entries changed to", *numEntriesAllocated );
		printf(" %d entries\n", *numEntriesAllocated * 2 );

		*numEntriesAllocated = *numEntriesAllocated * 2;

	}
			

/*
 *
 *	Return true if both pointer and not NULL.
 *
 */
	return( *labelStringPtr && *fileStringPtr );

}

int UpdateGuiListStruct( char *fileName,
		int numDateTimestamps, 
		char **dateTimestamps,
		char **fileNameList,
		char **labelList )

{

	int		i;
	char		*p;


#ifdef PAD_DATTIM_WITH_SPACES
/*
 *	Put the datetimestamps in the list.  NOTE, that we
 *	are giving away the pointer for the dateTimestamp
 *	so it should not be free'd.
 */
	labelList[0]     =  dateTimestamps[0];
	fileNameList[0]  = strdup( fileName );
	for( i = 1; i < numDateTimestamps; i++ ) {
		
		p = strchr( dateTimestamps[i], 'F' );	
		if( p )
			memset( dateTimestamps[i], (int) ' ', 
				p - dateTimestamps[i] );
		labelList[i]     =  dateTimestamps[i];
		fileNameList[i]  = strdup( fileName );

	}
#else

/* 
 *	Make sure year is in 4 digits.
 */
	Y2KList ( &dateTimestamps, numDateTimestamps );

/*
 *	Put the datetimestamps in the list.  NOTE, that we
 *	are giving away the pointer for the dateTimestamp
 *	so it should not be free'd.
 */
	for( i = 0; i < numDateTimestamps; i++ ) {
		
		labelList[i]     =  dateTimestamps[i];
		fileNameList[i]  = strdup( fileName );

	}

#endif /*  PAD_DATTIM_WITH_SPACES */

	return;
}



int
GetFilteredDirectoryList ( char *filter,
	                   char *directory,
	                   char ***list,
	                   int *list_size )
/*
 * This function gets a filtered list of files from a directory.
 *
 * Note: Programmer is responsible for deallocating returned list.
 */
{

	GlobalDataObjectType	*gd;

	int			i;
	int			iret, verbose;

	gd = GetGlobalDataObject();
	iret = FALSE;

	verbose = GetVerboseLevel();
	if ( verbose > VERBOSE_0 )
	    printf("GetFilteredDirectoryList\n");

	if ( filter == NULL ) {
	    if ( verbose > VERBOSE_1 )
		printf("  filter is not defined\n");
	    return iret;
	}
/*
 *	Set string to search for in file name.
 */
	SetSearchString ( gd, filter );
	*list_size = readdirectory ( directory, list,
	                                  filter_dirlist, NULL );
	if ( *list_size > 0 ) iret = TRUE;

	return iret;
}


void
BuildModelForecastList ( int metObjectType,
			 Widget scrolled_list,
			 Widget list_label,
	                 char *model,
	                 ListParentObjectType *list_struct,
			 ListIndexObjectType  *match_list,
			 ListIndexObjectType  *select_list )
/*
 * This function is called when the user initially enters a model
 * grid dialog. It creates a file list for the model times available.
 */
{
	char		*directory;
	char		**files;
	char		**list;
	int		list_size=0, verbose;
	int		scrollbar_position=0;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf("BuildModelForecastList %s\n",model);

/*
 *	Get upper tier directory.
 */
	GetDatatypeList ( model, &directory, &files, &list_size );

        if ( list_size == 0 )
	   directory = GetConfigValue( "grids" );

/*
 *	Free previously allocated memory.
 */
	if ( list_struct->listcount > 0 ) {
	    StringListFree ( list_struct->listcount, list_struct->string );
	    StringListFree ( list_struct->listcount, list_struct->label );
	}
	list_struct->listcount = 0;

	if ( match_list->listcount > 0 ) {
	    Free ( match_list->intList );
	    match_list->listcount = 0;
	}

/*
 *	Check for valid model and directory definition.
 */
	if( verbose > VERBOSE_1 ) {
	    if ( ! model )     printf("*** Model type is undefined\n");
	    if ( ! directory )
	           printf("*** Gridded data directory is undefined\n");
	}
	if( ! model || ! directory ) {
	    if ( directory != NULL ) Free( directory );
	    return;
	}

/*
 *	Get filtered directory list.
 */
	if ( list_size <= 0 )
	   GetFilteredDirectoryList ( model, directory, &files,
                                                   &list_size );
	/*if ( GetFilteredDirectoryList ( model, directory, &files,
	                                           &list_size ) > 0 ) {*/
	if ( list_size > 0 ) {
/*
 *	Build labels for scrolled list.  
 */
	    GetModelLabelsFromList ( model, directory, files,
	                             list_size, list_struct );
	    StringListFree( list_size, files );

/*
 *	Filter forecast times, if applicable.
 */
	    FilterForecastTimes ( list_struct );

/*
 *	Add labels to scrolled list.
 */
	    SetScrolledList ( metObjectType, list_label, scrolled_list,
	                      list_struct, match_list, select_list );
	}
	else {
	    if( verbose > VERBOSE_1 ) {
		printf("*** There are no grid files in directory:\n");
		printf("*** %s\n", directory );
	    }
	}
/*
 *	Free malloced memory.
 */
	Free ( directory );


}


void
FilterForecastTimes ( ListParentObjectType *list_struct )
{
/*
 * Filter out non-analysis times from a list of forecast times.
 */
	DataTimeObjectType	*dto;
	ListParentObjectType	list;
	char			**string, **label;
	int			i, count;

	dto = GetDataTimeObject();

	if ( dto->forecastType == ANALYSIS_TIME )  {
/*
 *	Filter non-analysis times from list.
 */
	    label  = StringListAllocate ( list_struct->listcount );
	    string = StringListAllocate ( list_struct->listcount );

	    count = 0;
	    for ( i=0; i<list_struct->listcount; i++ ) {
		if ( strstr ( list_struct->label[i], "F000" ) != NULL ) {
		    label [count] = strdup ( list_struct->label[i] );
		    string[count] = strdup ( list_struct->string[i] );
		    count++;
		}
	    }
/*
 *	Save analysis time.
 */
	    if ( list_struct->listcount > 0 ) {
	    	StringListFree (list_struct->listcount, list_struct->string);
	    	StringListFree (list_struct->listcount, list_struct->label);

	    	list_struct->listcount = count;
	    	list_struct->label  = label;
	    	list_struct->string = string;
	    }
	}
	
}


void
BuildModelAnalysisList ( Widget scrolled_list,
	                 char *model,
	                 ListParentObjectType *list_struct,
	                 ListIndexObjectType *select_list )
/*
 * This function is called when the user initially enters a model
 * grid dialog. It creates a file list for the model times available.
 */
{
	ListParentObjectType	list;
	char			*directory;
	char			**files;
	int			list_size, verbose;
	int			i, scrollbar_position=0;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf("BuildModelAnalysisList\n");

        GetDatatypeList ( model, &directory, &files, &list_size );
/*
 *	Get upper tier directory.
 */
	if ( list_size <= 0 )
	   directory = GetConfigValue( "grids" );

/*
 *	Free previously allocated memory.
 */
	if ( list_struct->listcount > 0 ) {
	    StringListFree ( list_struct->listcount, list_struct->string );
	    StringListFree ( list_struct->listcount, list_struct->label );
	}

	list_struct->listcount = 0;
	list.listcount = 0;

/*
 *	Check for valid model and directory definition.
 */
	if( verbose > VERBOSE_1 ) {
	    if ( ! model )     printf("*** Model type is undefined\n");
	    if ( ! directory )
	           printf("*** Gridded data directory is undefined\n");
	}
	if( ! model || ! directory ) {
	    if ( directory != NULL ) Free( directory );
	    return;
	}

/*
 *	Get filtered directory list.
 *
 */
	if ( list_size <= 0 )
	   GetFilteredDirectoryList ( model, directory, &files,
	                                           &list_size );

	if ( list_size > 0 ) {
/*
 *	    Build labels for scrolled list.  
 *
 */
	    GetModelLabelsFromList ( model, directory, files,
	                             list_size, &list );
	    StringListFree( list_size, files );

/*
 *	    Filter non-analysis times from list.
 */
	    list_struct->label  = StringListAllocate ( list.listcount );
	    list_struct->string = StringListAllocate ( list.listcount );

	    for ( i=0; i<list.listcount; i++ ) {
		if ( strstr ( list.label[i], "F000" ) != NULL ) {
		    list_struct->label[list_struct->listcount] = 
						strdup ( list.label[i] );
		    list_struct->string[list_struct->listcount] = 
						strdup ( list.string[i] );
		    list_struct->listcount++;
		}
	    }
	}

	if ( list_size > 0 ) {

/*
 *	    Add labels to scrolled list.
 */
	    AddProductsToList ( scrolled_list, list_struct->listcount,
	                                        list_struct->label );
/*
 *	    Highlight list items.
 */
	    HighLightList ( scrolled_list, TIMEHEIGHTOBJECT,
	                                   list_struct, select_list );

/*
 *	    Sensitize list.
 */
	    XtSetSensitive ( scrolled_list, TRUE );
	}
	else {
	    if( verbose > VERBOSE_1 ) {
		printf("*** There are no grid files in directory:\n");
		printf("*** %s\n", directory );
	    }
	}
/*
 *	Free malloced memory.
 */
	Free ( directory );

	if ( list.listcount > 0 ) {
	    StringListFree ( list.listcount, list.string );
	    StringListFree ( list.listcount, list.label );
	    list.listcount = 0;
	}

	

}


void
BuildFieldList ( Widget scrolled_list,
	         char *directory,
		 char *chapter,
		 char *model,
	         ListParentObjectType *list_struct )
/*
 * This function is called when the user initially enters a model
 * grid dialog. It creates a list of FDFs.
 */
{
	char		**list;
	char		**model_list;
	char		*modeldir, *chapdir;
	int		list_size, model_size, len;
	int		scrollbar_position=0;
	int		verbose;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf("BuildFieldList\n");

/*
 *	Check upper tier directory.
 */
	if ( directory == NULL ) {
	    printf("BuildFieldList - directory is NULL, returning");
	    return;
	}
	if ( chapter == NULL ) {
	    printf("BuildFieldList - chapter is NULL, returning");
	    return;
	}

        len = strlen (directory) + strlen (model) + strlen (chapter) + 100;

        chapdir  = (char *) malloc ( len * sizeof (char) );
        modeldir = (char *) malloc ( len * sizeof (char) );

	sprintf ( chapdir,  "%s%s%s%s%s", directory, "/",
	          "default", "/", chapter );
	sprintf ( modeldir, "%s%s%s%s%s", directory, "/",
	          model, "/", chapter );

	if (verbose>1) printf("Reading FDF's from directory: %s\n", chapdir);
	if (verbose>1) printf("Reading FDF's from directory: %s\n", modeldir);

/*
 *	Free previously allocated memory. list_struct->string is not used.
 */
	if ( list_struct->listcount > 0 ) {
	    StringListFree ( list_struct->listcount, list_struct->label );
	    list_struct->listcount = 0;
	}

/*
 *	Get filtered directory lists.
 */
	list_size  = readdirectory( chapdir, &(list), NULL, NULL );
	model_size = readdirectory( modeldir, &(model_list), NULL, NULL );

/*
 *	Append lists and sort.
 */
	list_struct->listcount = 
	    StringListsAppend ( list, model_list, list_size, model_size, 
						  &(list_struct->label) );

	if ( list_struct->listcount < 1 ) {
	    if (verbose > 1) printf("No FDF files found ... returning.");
	    return;
	}

/*
 *	Add labels to scrolled list.
 */
	AddProductsToList ( scrolled_list, list_struct->listcount,
	                                        list_struct->label );
/*
 *	Set scrollbar to bottom of list.
 */
/*	XmListSetBottomPos ( scrolled_list, scrollbar_position ); */

	Free ( chapdir );
	Free ( modeldir );
	StringListFree ( list_size, list );
	StringListFree ( model_size, model_list );

}



void
AdjustGridToggles (Widget w, XtPointer client_data, XtPointer xt_call_data )
/*
 * This callback insures that toggles for contour lines, color fill and
 * hi/low symbols are aligned in both the main grid dialog and it's
 * subordinate dialog.
 */
{
	GuiScalarGridDialogType	*grd;

        XmToggleButtonCallbackStruct *cbs = 
			(XmToggleButtonCallbackStruct *) xt_call_data;

	grd = GetGuiScalarGridDialog();

/*
 *	Contour line toggles.
 */
	if      ( w == GetLineToggleHSW1 ( grd ) ) {
	    if ( cbs->set ) {
		XmToggleButtonSetState (
			GetLineToggleHSW2 ( grd ), TRUE, FALSE );
		XmToggleButtonSetState (
			GetGridValueToggleHSW ( grd ), FALSE, FALSE );
	    }
	    else
		XmToggleButtonSetState (
			GetLineToggleHSW2 ( grd ), FALSE, FALSE );
	}
	else if ( w == GetLineToggleHSW2 ( grd ) ) {
	    if ( cbs->set ) {
		XmToggleButtonSetState (
			GetLineToggleHSW1 ( grd ), TRUE, FALSE );
		XmToggleButtonSetState (
			GetGridValueToggleHSW ( grd ), FALSE, FALSE );
	    }
	    else
		XmToggleButtonSetState (
			GetLineToggleHSW1 ( grd ), FALSE, FALSE );
	}

/*
 *	Grid point values.
 */
	else if ( w == GetGridValueToggleHSW ( grd ) ) {
	    if ( cbs->set ) {
		XmToggleButtonSetState (
			GetLineToggleHSW1 ( grd ), FALSE, FALSE );
		XmToggleButtonSetState (
			GetLineToggleHSW2 ( grd ), FALSE, FALSE );
	    }
	}

/*
 *	Color fill toggles.
 */
	else if ( w == GetFillToggleHSW1 ( grd ) ) {
	    if ( cbs->set )
		XmToggleButtonSetState (
			GetFillToggleHSW2 ( grd ), TRUE, FALSE );
	    else
		XmToggleButtonSetState (
			GetFillToggleHSW2 ( grd ), FALSE, FALSE );
	}
	else if ( w == GetFillToggleHSW2 ( grd ) ) {
	    if ( cbs->set )
		XmToggleButtonSetState (
			GetFillToggleHSW1 ( grd ), TRUE, FALSE );
	    else
		XmToggleButtonSetState (
			GetFillToggleHSW1 ( grd ), FALSE, FALSE );
	}

/*
 *	Hi/Low symbol toggles.
 */
	else if ( w == GetSymbolToggleHSW1 ( grd ) ) {
	    if ( cbs->set )
		XmToggleButtonSetState (
			GetSymbolToggleHSW2 ( grd ), TRUE, FALSE );
	    else
		XmToggleButtonSetState (
			GetSymbolToggleHSW2 ( grd ), FALSE, FALSE );
	}
	else if ( w == GetSymbolToggleHSW2 ( grd ) ) {
	    if ( cbs->set )
		XmToggleButtonSetState (
			GetSymbolToggleHSW1 ( grd ), TRUE, FALSE );
	    else
		XmToggleButtonSetState (
			GetSymbolToggleHSW1 ( grd ), FALSE, FALSE );
	}
}


int
GetLine ( char *text, char *line )
{
	int		iret;

	iret = NORMAL;

/*
 *      Get a line of text. Step over carriage return if it is the
 *	first character.
 */
	while ( *text == '\n' ) *text++;

        while ( *text && *text != '\n' ) {
            *line = *text;
            *line++; *text++;
        }
/*
 *      Null terminate line of text.
 */
        *line = '\0';

	return iret;
}


void
AddButtonToOptionMenu ( char *label, char *string, int button_count,
			void (*func)(),
			ButtonParentObjectType *bpo )
/*
 * This function will add a button to a previously created option menu.
 */
{
	ButtonObject		*bo;

	int			i;

	
	bo = &(bpo->buttons[0]);

/*
 *	Step to end of button list.
 */
	for ( i = 0; i < button_count; i++, bo++ );

/*
 *	Add button.
 */
	bo->w = XtCreateManagedWidget ( label,
			xmPushButtonWidgetClass,
			bpo->parent, NULL, 0 );
	bo->ismanaged = 1;
	XtAddCallback ( bo->w,
			XmNactivateCallback,
			func, bo );

	bo->string = strdup ( string);
	bo->label = strdup ( label );

	bpo->button_cnt++;
	bpo->button_alloc_cnt = bpo->button_cnt;

}


int
IntrinsicForecastInterval()
{
	GuiScalarGridDialogType	*grd;
	ListParentObjectType	*list;
	DataTimeObjectType	*dto;

	int			*intTimes;
	int			i, j, cnt, size, mode;
	int			retVal = 86400;
	BooleanType		haveForecasts;

	grd = GetGuiScalarGridDialog();

	list = &(grd->time_list);

/*
 *	Allocate DataTimeObject.
 */
	size = list->listcount;
	dto = AllocateDataTimeObject ( size );

/*
 *	Fill in a DataTimeObject.
 */
	SetLongDataTimes ( list->label, SCALARGRIDOBJECT, dto->refTimes,
	                   dto->fcstTimes, dto->validTimes, size );

/*
 *	Check for forecast times.
 */
	haveForecasts = FALSE;
	for ( i=0; i<size; i++ ) {
	    if ( dto->fcstTimes[i] != 0 ) {
		haveForecasts = TRUE;
		break;
	    }
	}
/*
 *	Find most common forecast interval.
 */
	intTimes = malloc (sizeof (AbsTime) * size );
	if ( haveForecasts ) {
	    cnt = 0;
	    for ( i=0; i<size-1; i++ ) {
		j=i+1;
		if ( dto->fcstTimes[i] == 0 ) {
		    intTimes[cnt] = dto->fcstTimes[j];
		    cnt++;
		}
	    }
	    retVal = IntMode ( intTimes, cnt );
	}
	else {
	    for ( i=0; i<size-1; i++ ) {
		j=i+1;
		intTimes[i] = dto->refTimes[j] - dto->refTimes[i];
	    }
	    retVal = IntMode ( intTimes, size-1 );
	}

/*
 *	Free.
 */
	FreeDataTimeObject ( dto );
	Free ( intTimes );

	return	retVal;
}


char *
GetModelConfigValue ( char *substring )
{

	ModelInfoType		*mdl;
	char			*retVal, *model, key[1024];

	mdl = GetGuiModelInfo();

	model = strdup ( GetModelType ( mdl ) );
	sprintf ( key, "%s%s", model, substring );

	retVal = GetConfigValue ( key );

	Free ( model );

	return retVal;
}

void 
GetFilteredModelList ( int *nmodels, char ***models, char ***labels)
{

	char	**filelist;
	char	*modeldir;
	char	**p1, **p2;
	char	**ptr1, **ptr2;
	int	i, j, count, numfiles;


/*
 *	Initialize temporary character arrays.
 */
	p1 = StringListAllocate ( *nmodels );
	p2 = StringListAllocate ( *nmodels );
	ptr1 = p1;
	ptr2 = p2;

/*
 *	Get the list of files in the grid directory
 */
	modeldir = GetConfigValue( "grids" );
	numfiles = readdirectory ( modeldir, &filelist, NULL, NULL );
	
/*
 *	For each model type, check for files, then add it to the list
 */
	count = 0;
	for (i = 0; i < *nmodels; i++ ) {
	   int iret; 
           char tmplt[255];

           ctb_dttmpl ( (*models)[i], tmplt, &iret );
           if ( iret == 0 )
	      {
              *p1 = strdup ( (*models)[i]);
              *p2 = strdup ( (*labels)[i]);
              p1++; p2++;
              count++;
	      }
           else
	      {
	      for (j = 0; j < numfiles; j++ ) {

		   if ( strstr ( filelist[j], (*models)[i] ) != NULL ) {
		
		       *p1 = strdup ( (*models)[i]);
		       *p2 = strdup ( (*labels)[i]);
		       p1++; p2++;
		       count++;
 		       break;

		   }
   	
	        }
	     }
	
	}

/*
 *	Make a copy of the new lists which reflects the correct size
 */
	StringListFree ( *nmodels, *models );
	StringListFree ( *nmodels, *labels );

	*models = StringListDup ( count, ptr1 );
	*labels = StringListDup ( count, ptr2 );

	StringListFree ( *nmodels, ptr1 );
	StringListFree ( *nmodels, ptr2 );

	*nmodels = count;	

	StringListFree ( numfiles, filelist );
	Free ( modeldir );
	
}

void templregex (char *instr, char *outstr)
{
int ier;
cst_rpst(instr, "YYYY", "[0-9][0-9][0-9][0-9]", outstr, &ier);
cst_rpst(outstr, "YY", "[0-9][0-9]", outstr, &ier);
cst_rpst(outstr, "MMM", "[A-Za-z][A-Za-z][A-Za-z]", outstr, &ier);
cst_rpst(outstr, "MM", "[01][0-9]", outstr, &ier);
cst_rpst(outstr, "DD", "[0-3][0-9]", outstr, &ier);
cst_rpst(outstr, "HH", "[0-2][0-9]", outstr, &ier);
cst_rpst(outstr, "NN", "[0-6][0-9]", outstr, &ier);
cst_rpst(outstr, "DWK", "[A-Za-z][A-Za-z][A-Za-z]", outstr, &ier);
cst_rpst(outstr, "FFF", "[0-9][0-9][0-9]", outstr, &ier);
cst_rpst(outstr, "FF", "[0-9][0-9]", outstr, &ier);
}

void GetDatatypeList ( char *model, char **directory, char ***files, int *list_size )
{
	*list_size = 0;
	*directory = NULL;

	if ( model ) /* try to get list from GEMPAK datatype.tbl */
           {
	   int iret, ier1, ier2, lpath, ltmpl;
	   int flen, nf, i;
	   static int maxlen=200000, dorder=1;
	   static char sep[]=";";
	   char path[255], tmplt[255], newtmplt[300], *filstr, *cpos;

	   ctb_dttmpl ( model, tmplt, &ier1 );
           ctb_dtpath ( model, path, &ier2 );
           if ( ( ier1 == 0 ) && ( ier2 == 0 ) )
	      {
	      lpath = strlen(path);
	      templregex ( tmplt, newtmplt ); ltmpl = strlen(newtmplt);
	      filstr = (char *)malloc(maxlen);
	      cfl_scnd ( path, &lpath, newtmplt, &ltmpl, sep, &maxlen, &dorder,
		filstr, &flen, &nf, &iret);
	      if ( nf > 0 )
	         {
	         *files = (char **)malloc( sizeof(char *) * (nf + 1) );
		 for ( i = 0; i < nf; i++ )
		    {
		    if ( i == 0 )
		       cpos = strtok ( filstr, sep );
		    else
		       cpos = strtok ( NULL, sep );
		    (*files)[i] = (char *)malloc ( strlen(cpos) + 1);
		    strcpy ( (*files)[i], cpos );
		    }
		 (*files)[nf] = NULL;
		 *list_size = nf;
		 *directory = (char *)malloc(MAXPATHLEN); 
		 css_envr ( path, *directory, &iret);
		 }
	      free (filstr);
              }
	   }
}

