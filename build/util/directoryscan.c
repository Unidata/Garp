/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	directoryscan.c
 *
 *	Directory reading routines.
 *
 *	History:
 *
 *	11/96	COMET	Original copy.
 *	12/97	COMET	Return NULL list if no directory in 
 *			readdirectory().
 *
 ***********************************************************************/


/*************************************************************************

 *    Wrapper around scandir for reading directories.  Passes
 *    back an array of pointers to strings  and the function returns
 *    a count files found, -1 on directory read errors and 0 if no 
 *    files were found. 
 *
 *    Malloc() is used to allocate the pointer array and each string
 *    allocated and the caller is responsible for freeing this memory or
 *    can call free_readdirectory().
 *
 *    The sort and select functions passed in as paramaters, are similar
 *    in purpose to what scandir uses.  
 *
 *    The select function is called for
 *    each item in the directory.  A char * (ie. a simple string) is passed
 *    to the select routine.  If select returns 0 then this file is not included
 *    in the final directory listing, otherwise it is.
 *
 *    The sort function is called indirectly via qsort.  Qsort will call this
 *    function with two char ** to be compared.   If you use strcmp() as the
 *    compare function, the directory list will be sorted alphabetically.
 *
 *    The default, if NULL is passed as the sort and select functions, is
 *    to sort the filenames alphabetically and no filenames
 *    that start with "." are returned.
 */


#include <sys/types.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <dirent.h>

extern int scandir();
extern int alphasort();

static int nodot( char *fileName );
static int reverse ( char **a , char **b );
int free_readdirectory( int filec, char **filelist );
int readdirectory(char *dirname, char ***filelist, int (*comparfunc)(),
                  int (*sortfunc)() );


static int AlphaSort( char **a, char **b)
{
        return (  strcmp ( *a, *b ) );
}

static int reverse ( char **a , char **b )
{
        return ( -1 * strcmp ( *a, *b ) );
}


static int nodot(char *fileName )
{
	
	if( fileName[0] == '.')
		return(0);

	return(1);
}

/*
 *	Free the filelist array allocated by readdirectory.  This
 *	version has lots of error checking that can be remove later.
 */

int free_readdirectory( int filec, char **filelist )
{
	char **list;
	int i;

	if ( filec <= 0 || filelist == NULL ) {
		printf ("free_readdirectory: Null or zero length paramaters\n");
		return;
	}

	for (i = 0, list = filelist; i < filec; i++, *list++ ) {
		if ( ! *list ) {
			printf("free_readdirectory: Bad pointer in filelist\n");
			return;
		}

		free(*list);
	}

	free(filelist);

}

int
readdirectory(char *dirname, char ***filelist, int (*selectFunc)(),
              int (*sortFunc)() )
{
	int i;
	int adjustedFileCount;
	int num_entries;
	char **filelistp;
	struct dirent **namelist, **list;
        int (*realSortFunc)();
	int (*realSelectFunc)();

        
	realSortFunc = AlphaSort;
        if ( sortFunc != NULL )
		realSortFunc = sortFunc;

	realSelectFunc = nodot;
	if ( selectFunc != NULL )
		realSelectFunc = selectFunc;

#if 0
	num_entries = scandir(dirname, &namelist, realSelectFunc,
	                      realSortFunc);
#endif

	num_entries = scandir(dirname, &namelist, NULL,
	                      NULL);

	if ( num_entries < 0 ) {
		*filelist = NULL;
		return(0);
	}

/*
 *	Check to see if anything came back. 
 */
	if (num_entries == 0) {
	/*	free(namelist); */
		return(num_entries);
	}


/*
 *	Copy all the entries into a simple array of strings
 *	like "argv" and free the other junk.
 */
	
	*filelist = (char **) malloc (sizeof(char *) * (num_entries + 1));


	adjustedFileCount = 0;
	list              = namelist;
	filelistp         = *filelist;

	for (i=0; i<num_entries; i++) {

		if( (*realSelectFunc)( (*list)->d_name) ) {

			*filelistp = 
				(char *) malloc(strlen((*list)->d_name) + 1);

			strcpy(*filelistp, (*list)->d_name);

			*filelistp++;

			adjustedFileCount++;

		}


#if 0
		printf(" %s", *filelistp); 
		printf(" %s", (*list)->d_name); 
#endif

		free(*list);
		*list++; 

	}

	if( realSortFunc )
		qsort( *filelist, adjustedFileCount, sizeof(char *), 
				realSortFunc );

/* 
 *	Null terminate the array
 */
	*filelistp = (char *) NULL;

	free( namelist );
/*
 *	If there is nothing to return, free the malloc'ed memory
 */
	if ( adjustedFileCount == 0 ) {
		free ( *filelist );
		*filelist = NULL;
	}

	return ( adjustedFileCount );

}
		
	
	
#ifdef TESTIT
main(int argc, char **argv)
{
	
	int num_entries, i;
	char **namelist, **list;
	char *dir = "/tmp";

	if(argc == 2)
		dir  = argv[1];

	
	if ((num_entries = readdirectory(dir, &namelist, NULL, NULL)) < 0) {
		fprintf(stderr, "Unexpected error\n");
		exit(1);
	}

	printf("Number of entries is %d\n", num_entries);
	if (num_entries) {
		printf("Entries are:\n");
		for (i=0, list=namelist; i<num_entries; i++) {
			printf(" %s\n", *list);
			/* free(*list); */
			*list++;
		}
		/* free(namelist); */
	}

	free_readdirectory ( num_entries, namelist );

	exit(0);
}

#endif /* TESTIT */
