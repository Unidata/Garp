/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	filterdirlist.c
 *
 *	Filters a file list to exclude certain files.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include <strings.h>
#include "genglobs.h"

int filterdirlist(char *fileName)
{

/*
 *	Exclude "." files and files containing the string "sao".
 */
	GlobalDataObjectType	*gd;

	gd = GetGlobalDataObject();

#if 0
	GetSearchString ( gd );
#endif

        if ( fileName[0] == '.' || 
		( strstr ( fileName, GetSearchString(gd) ) == NULL ) )
            return(0);

        return(1);
}
