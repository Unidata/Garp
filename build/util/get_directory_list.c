/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	get_directory_list.c
 *
 *	Gets a directory listing.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <dirent.h>
    
int
get_directory_list ( const char *default_directory, char ***directory_list )
{
	int		i, count=0;
	const char	*tokensep="/";
	char		*buf;
	char		*token;
	char		**directory_listp;

	buf = (char *) malloc (sizeof(char *) * 
	                           strlen ( default_directory+1 ));
	strcpy ( buf, default_directory );
	token = strtok ( buf, tokensep );

/*
 *	First just count the number of entries.
 */
	while ( token != NULL ) {
	    token = strtok ( NULL, tokensep );
	    count++;
	}
/*
 *	Reinitialize token.
 */
	strcpy ( buf, default_directory );
	token = strtok ( buf, tokensep );

/*
 *	Allocate memory for list.
 */
	*directory_list = (char **) malloc (sizeof(char *) * (count + 1));

/*
 *	Now save list members.
 */
	for (i=0, directory_listp = *directory_list; i<count; i++) {
	    *directory_listp = (char *) malloc (strlen (token) + 1);
	    strcpy ( *directory_listp, token );
	    *directory_listp++;
	    token = strtok ( NULL, tokensep );
	}
/*
 *	Null terminate the array.
 */
	*directory_listp = (char *) NULL;
	return ( count );
}
