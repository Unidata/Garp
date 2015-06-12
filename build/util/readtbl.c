/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	readtbl.c
 *
 *	Table reading utilities.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/98	COMET	Added sanity tests for undefined keys. Removed
 *			some deprecated functions.
 *
 ***********************************************************************/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

#include "guimacros.h"
#include "gendef.h"
#include "genglobs.h"
#include "_proto.h"
#include "utils.h"
#include "winobj.h"


int
ReadSatTable ( struct enhance_list *enhance )
{
	char	*file, *filename, *directory;
	char	**elist, **blist, **flist;
	char	*key1="satenhancetbl";
	char	*key2="tabledir";
	char	buf1[81], buf2[81], buf3[81];
	int	type;
	int	count;

	FILE	*fp;


	if ( GetVerboseLevel() > VERBOSE_0 )
	    printf("ReadSatTable\n");

/*
 *	Open file.
 */
	filename = GetConfigValue( key1 );
	if ( filename == NULL ) {
	    printf("  Warning: key '%s' is not defined in Garp_defaults\n",
	    key1);
	    return -1;
	}

	if((fp = fopen(filename,"r")) == NULL) {

/*
 *	    Get full file name.
 */
	    directory = GetConfigValue( key2 );
	    if ( directory == NULL ) {
		printf("  Warning: key '%s' is not defined in Garp_defaults\n",
		key2);
		return -2;
	    }

	    file = builddirpath ( directory, filename );
	    if (( fp = fopen(file, "r")) == NULL ) {
	    	printf("  Warning: Cannot open file %s\n", file);
	    	return -3;
	    }
	}

/*
 *	Allocate space for list.
 */
	elist = (char **) malloc (MAX_LIST * sizeof( char * ) );
	enhance->list = elist;

	blist = (char **) malloc (MAX_LIST * sizeof( char * ) );
	enhance->background = blist;

	flist = (char **) malloc (MAX_LIST * sizeof( char * ) );
	enhance->foreground = flist;
/*
 *	Read enhancement table list.
 */

	count=0;
	while ( fscanf(fp, "%s %s %s", buf1, buf2, buf3 ) != EOF ) {
	    *elist = (char *) malloc ( strlen (buf1) + 1 );
	    *blist = (char *) malloc ( strlen (buf2) + 1 );
	    *flist = (char *) malloc ( strlen (buf3) + 1 );
	    strcpy ( *elist, buf1 );
	    strcpy ( *blist, buf2 );
	    strcpy ( *flist, buf3 );
	    count ++;
	    elist++, blist++, flist++;
	}
	fclose(fp);

/*
 *	Null terminate the array.
 */
	*elist = (char *) NULL;
	*blist = (char *) NULL;
	*flist = (char *) NULL;

/*
 *	Save count.
 */
	enhance->size = count;

/*
 *	Free.
 */
	Free ( file );
	Free ( filename );
	Free ( directory );

	return 0;
}
