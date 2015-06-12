/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	getapp.h
 *
 *	Program parameter access function declarations.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void error ( int, char *, char *, char * );
extern void full_name ( char *, char * , char * );
extern int find_file ( char * );
extern void get_configfile ( char *, char * );
extern int get_apps_defaults ( char *, char *, int *, char *, int * );
extern void get_app_name(char *, char *, char *);

