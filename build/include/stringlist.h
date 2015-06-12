/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	stringlist.h
 *
 *	Header file for string function declarations.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#ifndef __STRINGLIST_H
#define __STRINGLIST_H

void StringListFree( int cnt, char **m );
void FreeList( int cnt, char **m );
char **StringListDup( int cnt, char **src );
char **StringListAllocate( int cnt );

#endif  /*  __STRINGLIST_H  */
