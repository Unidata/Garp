/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	filecheck.c
 *
 *	Directory and file existance checkers.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>

int DirExists( char *pathname )
{

	struct stat	buf;

	if( stat( pathname, & buf ) < 0 )
		return( 0 );

	return( S_ISDIR( buf.st_mode ) );

}

int FileExists( char *pathname )
{

	struct stat	buf;

	if( stat( pathname, & buf ) < 0 )
		return( 0 );

	return( S_ISREG( buf.st_mode ) );


}


#ifdef TESTIT

main( int argc, char **argv )
{

	if( DirExists( argv[1] ) )
		printf("Directory %s exists\n", argv[1] );

	if( FileExists( argv[1] ) )
		printf("File %s exists\n", argv[1] );
}
#endif
