/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gemresize.c
 *
 *	Window resizing function
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include "underscore.h"
void GempakResize( char *windowName, int width, int height )
{

	int	iret;

	gresize( &width, &height, &iret );
	
	return;

}
