/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	readlist.c
 *
 *	Table file reading utility.
 *
 *	History:
 *
 *	 7/97	COMET	Original copy
 *
 ***********************************************************************/


#include "genglobs.h"
#include "_proto.h"


void
readlist ( char *directory, char *infile, int maxline, int len,
	   int *nstn, char ***labels, int *iret )
{
/*
 * Read in a single column file.
 */
	FILE    *ftbl;
	char    *record, **listp, **listptr;
	char	file[256], output[256];
char *format="%s";
	int	i, ilat, ilon, ielv;
	int	verbose;

	FILE	*cfl_sopn();

/*---------------------------------------------------------------------*/
	*iret = 0;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "readstn1\n" );

/*
 *	Build full path name.
 */
	if ( !infile || !directory ) return;
	strcpy ( file, directory );
	strcat ( file, "/" );
	strcat ( file, infile );

/*
 *	Open the table file.
 */
	if ( verbose > VERBOSE_1 ) printf ("  Opening %s\n", file );

	if ( ( ftbl = fopen ( file, "r") ) == NULL ) {
	    printf ("readlist - Could not open table file = %s\n", file );
	    *iret = -3;
	    return;
	}
/*
 *	Allocate space for table and initialize.
 */
	listp = StringListAllocate( maxline );
	listptr = listp;
	record = (char *) malloc ( len * sizeof( char ) );
	for ( i=0; i<maxline; i++ ) {
	    listp[i] = (char *) malloc ( len * sizeof( char ) );
	    strcpy ( listp[i], " " );
	}

/*
 *	Set counter to zero.
 */
	i = 0;

/*
 *	Read in the next record, check for a comment,
 *	and process valid table entries.
 */
	while ( fgets ( record, len-1, ftbl ) != NULL ) {
	    if ( ( record[0] != '!' ) &&
		 ( i < maxline ) ) {
		if ( sscanf ( record, format, listp[i] ) > 0 ) {

		    if ( verbose > VERBOSE_2 ) {
			sprintf ( output, "%s", listp[i] );
			printf ("%s \n", output );
		    }
		    i++;
		}
	    }
	}

	*nstn = i;
	*labels = listptr;

	fclose ( ftbl );

	Free ( record );
}
