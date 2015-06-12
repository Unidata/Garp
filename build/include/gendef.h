/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gendef.h
 *
 *	Some general definitions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 8/97	COMET	Added AbsTime definition.
 *	 1/98	COMET	Added LONG_BIT (for linux build)
 *
 ***********************************************************************/




#ifndef __GENDEF_H
#define __GENDEF_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>


#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define NORMAL    0
#define BADVALUE  -999
#define FLOATFUZZ 0.01

#ifndef BooleanType
#define BooleanType	int
#endif

#ifndef AbsTime
#define AbsTime		time_t
#endif

#ifndef LONG_BIT
#define LONG_BIT 32
#endif



/*
 *      A macro to make string comparisons a bit more efficient.
 */

#ifndef EQSTRING
#define EQSTRING(x,y)	((*(x)) != (*(y)) ? 0 : ! strcmp((x), (y)) )
#endif

/*
 *	A macro to check strings before freeing them.
 */
/*
#ifndef Free

BooleanType GetMemCheck();

void _Free(void *x) {							\
	if (((x) == (void *)NULL) || ((x) == (void *) 0xffffffff)) {	\
		printf("!!!!!Whoa!!!!!!  Free() got a 0x%x\n", (x));	\
		if( ! GetMemCheck() ) {					\
			free((x));					\
		} 							\
	} else {							\
		free(x);						\
		x = (void *) 0xffffffff;				\
	}								\
}

#define Free(x) _Free(x)
#endif
*/

/*
 *	Include all the GEMPAK header stuff
 *	Chiz/unidata** if VARARGS is used, don't include
 *	stdarg.h from geminc.h
 */
/* AIX */
#ifdef _H_VARARGS
#define _H_STDARG
#endif

/* OSF/1 */
#ifdef _VARARGS_H_
#define	_STDARG_H
#endif

/* Solaris, Linux */
#ifdef _VARARGS_H
#define _STDARG_H
#endif

/* IRIX */
#ifdef __VARARGS_H__
#define __STDARG_H__
#endif

/* HPUX */
#ifdef _VARARGS_INCLUDED
#define _STDARG_INCLUDED
#endif

/* freebsd */
#ifdef _VARARGS_H_
#define _STDARG_H_
#endif

 
#include "geminc.h"
#include "gemprm.h"


#endif  /*  __GENDEF_H  */
