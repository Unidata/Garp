/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	init_scrolledw.c
 *
 *	Scrolled list utilities.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	05/97	COMET	Changed printf statements.
 *	06/97	COMET	Fixed bug in one changed printf statement
 *	 1/98	COMET	Changed some NULLs to '\0'.
 *	12/99	COMET	Added function call Y2KList() in put year in YYYY.
 *
 ***********************************************************************/


#include <stdlib.h>
#include <string.h>


#include "guimacros.h"
#include "underscore.h"
#include "gendef.h"
#include "genglobs.h"
#include "utils.h"
#include "winobj.h"
#include "_proto.h"
#include "upaobj.h"
#include "sfcobj.h"


void
ScrubList ( struct list_struct *list_info )
/*
 * There is a *data* problem with GEMPAK station files. Data for
 * one day can show up in the next day's surface file and vice-versa.
 * To "fix" this, we will search downward through a file removing 
 * the duplicate listings for ?/2300 times and search upward 
 * removing duplicate listings for ?/0000 times.
 */
{
	int		i, j, k;
	char		dattim[81];


/*
 *	Step through list.
 */
	i=0;
	while ( i < list_info->list_size-1 ) {
	    strcpy ( dattim, list_info->list[i] );
	    j = i + 1;

/*
 *	Compare current item with the remaining items in the list.
 */
	    while ( j < list_info->list_size-1 ) {
		if ( strcmp ( dattim, list_info->list[j] ) == 0 ) {
		    k = j;

/*
 *	Overwrite current items and cascade remaining items in list.
 */
		    while ( k < list_info->list_size-1 ) {
		    	strcpy ( list_info->list[k], list_info->list[k+1] );
			strcpy ( list_info->file[k], list_info->file[k+1] );
			k++;
		    }
		    list_info->list_size--;
		}
		j++;
	    }
	    i++;
	}

}


int
filter_dirlist(char *fileName)
{

/*
 *	Exclude "." files and files containing the string "sao".
 */
	GlobalDataObjectType	*gd;

	gd = GetGlobalDataObject();

        if ( fileName[0] == '.' || 
		( strstr ( fileName, GetSearchString ( gd ) ) == NULL ) )
            return(0);

        return(1);
}

static void
add_list ( char *times, int *nt, char ***list, int *list_size, int *iret )
{
	char	word[20], buf[80];
	char	*save_times;
	char	**listp;
	char	**tlist;
	int	i;
	int	verbose;


	verbose = GetVerboseLevel();
        if( verbose > VERBOSE_0 )
            printf ( "add_list\n" );

	*iret = NORMAL;
	save_times = times;

/*
 *	Return if there are no times to add.
 */
	if ( *nt <= 0 ) return;
	if ( verbose > VERBOSE_1 ) {
	    printf("  list_size = %d\n", *list_size );
	    printf("  nt  = %d\n", *nt );
	}


	if ( *list_size == 0 ) {
	    *list = StringListAllocate ( MAX_LIST );
	}

	tlist = *list;

	for (i=(*list_size), listp = &(tlist[*list_size]);
	                               i<(*list_size+*nt); i++) {

	    next_word ( &times, word, iret );
	    *listp = (char *) malloc(strlen(word) + 1);
	    strcpy(*listp, word );
	    if ( verbose > VERBOSE_1 ) {
		sprintf(buf, "add_list -   %d  %s", i, *listp);
		printf ( "%s\n", buf );
	    }
	    listp++;
        }

	times = save_times;
/*
 *	Null terminate the array.
 */
        *listp = (char *) NULL;

}


static void
add_file ( char *infile, int *nt, char ***file, int *list_size, int *iret )
{
	char	**filep;
	char	**tfile;
	int	i;
	int	verbose;


	verbose = GetVerboseLevel();
        if( verbose > VERBOSE_0 )
            printf ( "add_file\n" );

	*iret = NORMAL;

/*
 *	Return if there are no file to add.
 */
	if ( *nt <= 0 ) return;
	if ( verbose > VERBOSE_1 ) {
	    printf("  list_size = %d\n", *list_size );
	    printf("  nt  = %d\n", *nt );
	}

	if ( *list_size == 0 ) {
	    *file = StringListAllocate ( MAX_LIST );
            /* *file = (char **) malloc (MAX_LIST * sizeof( char * ) ); */
	}

	tfile = *file;

	for (i=(*list_size), filep = &(tfile[*list_size]);
	                               i<(*list_size+*nt); i++) {
	    *filep = (char *) malloc(strlen(infile) + 1);
	    strcpy(*filep, infile );
	    if ( verbose > VERBOSE_1 ) printf("  %s\n", *filep);
	    filep++;
        }

/*
 *	Null terminate the array.
 */
/*        *filep = (char *) NULL; */

}


int
GetSfcList ( char *directory,
             char **files,
             int filec,
             ListParentObjectType *list_struct,
	     int *iret ) 
{
	char			*times;
	char			*next_file;
	int			nlabels = 0;
	int			len_f, len_t;
	int			nt = 0;
	int			i, j;
	int			verbose;

	verbose = GetVerboseLevel();
        if( verbose > VERBOSE_0 )
            printf ( "GetSfcList\n" );

/*
 *	Look at each file and build a list of valid times.
 */
	for ( i=0; i<filec; i++ ) {

/*
 *	    Look at the next file.
 */
	    next_file = builddirpath ( directory, files[i] );

/*
 *	    Count the number of time increments available in this file
 *	    to determine size of string to hold these values.
 */
	    len_f = strlen ( next_file );

	    if ( verbose > VERBOSE_1 )
		printf("GetSfcList - calling sfc_count\n" );
	    sfc_count ( next_file, &nt, iret, len_f );

/*
 *	    If there are no times in the file, don't bother adding them.
 */
	    if ( nt > 0 ) {

/*
 *	    Use same size string as used in GEMPAK. 
 */
	    len_t = nt * 15 + 1;
	    if ( verbose > VERBOSE_1 )
		printf("GetSfcList - Allocating len_t = %d\n", len_t );

/*
 *	    Allocate space and blank out string for Fortran compatibility.
 */
	    times = (char *) malloc ( ( len_t ) * sizeof (char) );
	    for ( j=0; j<len_t; j++ ) times[j]=' ';
	    times[len_t-1] = '\0';

/*
 *	    Get a list of times for which data is available in this file.
 */
	    if ( verbose > VERBOSE_1 )
		printf("GetSfcList - calling sfc_list\n" );
	    sfc_list ( next_file, times, &nt, &len_t, &verbose,
	                                     iret, len_f, len_t );

	    if ( verbose > VERBOSE_1 ) {
		printf("  file = %s\n", next_file );
		printf("  nt = %d\n", nt);
		printf("  iret = %d\n", *iret);
		printf("  times = %s\n", times );
	    }
/*
 *	    Add these times to the list.
 */
	    add_list ( times, &nt, &(list_struct->label),
	                                 &nlabels, iret );
/*
 *	    Add corresponding file name to structure.
 */
	    add_file ( next_file, &nt, &(list_struct->string),
                                              &nlabels, iret );

	    nlabels += nt;

	    if ( times != NULL ) {
		if ( verbose > VERBOSE_1 )
		    printf("GetSfcList - freeing times = %s\n", times );
	        free ( times );
		if ( verbose > VERBOSE_1 )
		    printf("GetSfcList - times freed\n");
	    }
	    } /* end of ( nt > 0 ) test */

	    if ( next_file != NULL ) {
		if ( verbose > VERBOSE_1 )
		    printf("GetSfcList - freeing next_file = %s\n", next_file );
		free ( next_file );
		if ( verbose > VERBOSE_1 )
		    printf("GetSfcList - next_file freed\n");
	    }
	}

	if ( verbose > VERBOSE_1 )
	    printf("  list_size = %d\n", nlabels );
/*
 *	Make sure year is in 4 digits.
 */
	Y2KList ( &(list_struct->label), nlabels);

	return nlabels;
}


int
GetUpaList ( char *directory,
             char **files,
             int filec,
             ListParentObjectType *list_struct,
	     int *iret ) 
{
	char			*times;
	char			*next_file;
	int			nlabels = 0;
	int			len_f, len_t;
	int			nt = 0;
	int			i, j;
	int			verbose;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "GetUpaList\n" );

/*
 *	Look at each file and build a list of valid times.
 */

	for ( i=0; i<filec; i++ ) {

/*
 *	    Look at the next file.
 */
	    next_file = builddirpath ( directory, files[i] );

/*
 *	    Count the number of time increments available in this file
 *	    to determine size of string to hold these values.
 */
	    len_f = strlen ( next_file );
	    if ( verbose > VERBOSE_1 )
		printf("GetUpaList - calling upa_count\n" );
	    upa_count ( next_file, &nt, iret, len_f );

/*
 *	    If there are no times in the file, don't bother adding them.
 */
	    if ( nt > 0 ) {

	    len_t = nt * 15 + 1;
	    if ( verbose > VERBOSE_1 )
		printf("GetUpaList - Allocating len_t = %d\n", len_t+1 );
/*
 *          Allocate space and blank out string for Fortran compatibility.
 */
	    times = (char *) malloc ( ( len_t ) * sizeof (char) );
	    for ( j=0; j<len_t; j++ ) times[j]=' ';
	    times[len_t-1] = '\0';

/*
 *	    Get a list of times for which data is available in this file.
 */
	    if ( verbose > VERBOSE_1 )
		printf("GetUpaList - calling upa_list\n" );
	    upa_list ( next_file, times, &nt, &len_t, iret, len_f, len_t );

	    if ( verbose > VERBOSE_1 ) {
		printf("  file = %s\n", next_file );
		printf("  nt = %d\n", nt);
		printf("  iret = %d\n", *iret);
		printf("  times = %s\n", times );
	    }
/*
 *	    Add these times to the list.
 */
	    add_list ( times, &nt, &(list_struct->label),
	                                 &nlabels, iret );

/*
 *	    Add corresponding file name to structure.
 */
	    if ( verbose > VERBOSE_1 )
		printf("GetUpaList - calling add_file\n");

	    add_file ( next_file, &nt, &(list_struct->string),
                                              &nlabels, iret );
	    if ( verbose > VERBOSE_1 )
		printf("GetUpaList - returned from add_file\n");

	    nlabels += nt;

/*
 *	    Free.
 */
	    if ( times != NULL ) {
		if ( verbose > VERBOSE_1 )
		    printf("GetUpaList - freeing times\n" );
	        free ( times );
		if ( verbose > VERBOSE_1 )
		    printf("GetUpaList - times freed\n");
	    }
	    } /* end of ( nt > 0 ) test */

	    if ( next_file != NULL ) {
		if ( verbose > VERBOSE_1 )
		    printf("GetUpaList - freeing next_file = %s\n", next_file );
		free ( next_file );
		if ( verbose > VERBOSE_1 )
		    printf("GetUpaList - next_file freed\n");
	    }

	}

	if ( verbose > VERBOSE_1 )
	    printf("  list_size = %d\n", nlabels );

/*
 *	Make sure year is in 4 digits.
 */
	Y2KList ( &(list_struct->label), nlabels);

	return nlabels;
}
