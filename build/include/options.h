/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	options.h
 *
 *	Header file for default application resource values and
 *	command line option definitions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#ifndef _OPTIONS_H
#define _OPTIONS_H

#include <Xm/Xm.h>
typedef struct {
	int 	debug;
	Boolean verbose;
	char	*start_time;
	char	*end_time;
	char	*current_time;
	Boolean	clock_stopped;
} GlobalOptionsStruct;


void usage ( char * );
Widget do_options ( int *argcptr, 
		char **argv, 
		XtAppContext *app, 
		char *xtapplication_class,
		GlobalOptionsStruct *globalDataptr );

extern GlobalOptionsStruct globalData;

#endif    /*   _OPTIONS_H  */
