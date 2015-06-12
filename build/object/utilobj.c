/***********************************************************************
 *
 *	Copyright 1997, University Corporation for Atmospheric Research.
 *
 *	utilobj.c
 *
 *	Utility objects.
 *
 *	History:
 *
 *	 8/97	COMET	Original copy
 *	12/97	COMET	Added select_list to ReturnListStruct.
 *
 ***********************************************************************/


#include "guimacros.h"
#include "gendef.h"
#include "genglobs.h"
#include "winobj.h"
#include "_proto.h"
#include "children.h"



void
FreeListParentObject ( ListParentObjectType *inList )
{
	int	i;

/*
 *	Free the lists.
 */
	for ( i=0; i<inList->listcount; i++ ) {
	    if ( inList->string[i] != NULL )
		Free ( inList->string[i] );
	    if ( inList->label[i] != NULL )
		Free ( inList->label[i] );
	}

	Free ( inList->string );
	Free ( inList->label );

	inList->listcount = 0;

/*
 *	Free the object.
 */
	free ( inList );
}


void
FreeListIndexObject ( ListIndexObjectType *indexList )
{
	Free ( indexList->intList );
	indexList->listcount = 0;

/*
 *	Free the object.
 */
	free ( indexList );
}


ListParentObjectType *
ReturnListStruct ( Widget scrolledList,
	           ListParentObjectType list_info,
	           ListIndexObjectType indexList,
	           ListIndexObjectType *select_list )
{
	extern Widget		main_shell;

	GlobalDataObjectType	*gd;
	DataTimeObjectType	*dto;
	ListParentObjectType	*inList;
	char			**dattims, **files;
	int			i, ind, verbose;
	int			sw_count, *sw_index;


	verbose = GetVerboseLevel();

	dto = GetDataTimeObject();

/*
 *	Time match.
 */
	if ( dto->numTimes > 0 && dto->matchType != NOTIMEMATCH ) {
/*
 *	Allocate.
 */
	    inList = (ListParentObjectType *)
			 malloc ( sizeof(ListParentObjectType) );
	    inList->listcount = indexList.listcount;

	    dattims = StringListAllocate ( inList->listcount );
	    files   = StringListAllocate ( inList->listcount );

/*
 *	Save time matched dattims and file names.
 */
	    for ( i=0; i<inList->listcount; i++ ) {
		ind = indexList.intList[i];
/*
 *	Test for NULL elements (ones that did not time match).
 */
		if ( ind > -1 ) {
		    dattims[i] = strdup ( list_info.label[ind] );
		    files[i]   = strdup ( list_info.string[ind] );
		}
		else {
		    dattims[i] = NULL;
		    files[i]   = NULL;
		}
	    }

	    inList->label  = dattims;
	    inList->string = files;
	}

/*
 *	No time matching.
 */
	else {
/*
 *	Get highlighted position indices from list widget.
 */
	    XmListGetSelectedPos ( scrolledList, &sw_index, &sw_count );
	    if ( sw_count <= 0 ) {
	    	InfoPop ( main_shell, INFORMATION,
	    	"\nSorry boss, you need to choose\nat least one date/time stamp.\n",
	    	FALSE, FALSE, NULL, NULL );

	    	return NULL;
	    }

/*
 *	File Selection widgets count starting at 1.
 */
	    AdjustIndexOffset ( sw_count, sw_index );

/*
 *	Store picked items in a list structure.
 */
	    inList = (ListParentObjectType *)
			 malloc ( sizeof(ListParentObjectType) );
	    inList->listcount = sw_count;

	    dattims = StringListAllocate ( inList->listcount );
	    files   = StringListAllocate ( inList->listcount );

/*
 *	Save time matched dattims and file names.
 */
	    for ( i=0; i<inList->listcount; i++ ) {
		ind = sw_index[i];
		dattims[i] = strdup ( list_info.label[ind] );
		files[i]   = strdup ( list_info.string[ind] );
	    }

	    inList->label  = dattims;
	    inList->string = files;

/*
 *	Save selected indices.
 */
	    if ( select_list->listcount > 0 )
		Free ( select_list->intList );

	    select_list->listcount = sw_count;
	    select_list->intList =
	              malloc ( sizeof(int) * select_list->listcount );
	    for ( i=0; i<select_list->listcount; i++ )
		select_list->intList[i] = sw_index[i];
/*
 *	Free.
 */
	    /*Free ( sw_index );*/
	   if(sw_index != NULL) free(sw_index); /*XtFree(sw_index);*/
	}

	return inList;
}
