/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	garperr.c
 *
 *	Fortran to C error printing routine.
 *
 *	History:
 *
 *	 4/97	COMET	Original copy
 *	12/97	COMET	Changed garperr() to use printf instead of
 *			wprint()
 *	 1/98	COMET	Added length arg. Changed NULL to '\0'
 *
 ***********************************************************************/


#include "underscore.h"
#include "genglobs.h"
#include "_proto.h"

void
garperr  ( char *outmsg, int *len )
{

/*
 *	NULL-ify the string. len is the string length to the last character,
 *	so add a NULL after that position.
 */
	outmsg[*len] = '\0';
	
	printf ( "%s\n", outmsg );
}
