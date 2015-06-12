/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	geticon.c
 *
 *	Get an icon full path name.	
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/98	COMET	Fixed bug returning local variable, made static
 *
 ***********************************************************************/


#include <string.h>

#include "winobj.h"
#include "genglobs.h"
#include "_proto.h"

char *
GetIcon ( char *icon )
{
	static char	file[FILENAMESTRING];
	static char	*iconPath = NULL;


	if( ! iconPath )
		iconPath = GetConfigValue("ICONDIR");

	if( ! iconPath ) {
		file [0] = '\0';
		return file;
	}

/*
 *	Get directory path for icons and append icon name to path.
 */
	sprintf ( file, "%s/%s", iconPath, icon );

	return file;

}
