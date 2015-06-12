/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	stringclean.c
 *
 *	String cleaning utility.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>

/*
 *
 *	Squeeze out all unprintable string junk from given character
 *	string. The input parameter is re-written in place.
 *
 */
StringClean( char *s )
{
        char *t;

        t = s;
        while( *s ) {

                if( ! isspace(*s) && ! iscntrl(*s) )
                        *t++ = *s;
                *s++;
        }

        *t = '\0';

}
