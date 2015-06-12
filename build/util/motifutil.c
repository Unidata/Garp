/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	motifutil.c
 *
 *	Motif XmString utilities.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	12/97	COMET	Fixed potential leak in FreeMotifStringTable()
 *
 ***********************************************************************/


#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/List.h>

#include "winobj.h"

/*
 *
 *	Free a commonly allocated Motif resource - the XmString table.
 *	The string table is typically allocated with the following
 *	segment of code:
 *
 *		char		*argv[] = {"first", "second", "third" };
 *		int		cnt = 3;
 *		XmString	*xmstrs;
 *		xmstrs = XtMalloc( sizeof( XmString ) * cnt );
 *		for( i = 0; i < cnt; i++ )
 *			xmstrs[i] = XmStringCreateLtoR( argv[i], 
 *					XmFONTLIST_DEFAULT_TAG );
 *
 *	OR 
 *
 *	By calling the convenience function, 
 *
 *		CreateMotifStringTable().
 *
 *
 */

void FreeMotifStringTable( int cnt, XmString *xmstr )
{
	int i;

	if( xmstr == (XmString *) NULL )
		return;

	for( i = 0; i < cnt; i++ )
		XmStringFree( xmstr[i] );

	XtFree( (void *) xmstr );

}

/*
 *
 *	Create a string table from the argv array.  If a font tag is
 *	specified, then it is used.  Otherwise, the  default font
 *	tag of XmFONTLIST_DEFAULT_TAG is used.  On success, an 
 *	XmString * is returned.  Null is returned on failure.  It
 *	is up to the caller to free both the array of pointers and
 *	each individual XmString.  That can be done the standard way
 *	or by calling the convenience function, FreeMotifStringTable().
 *
 */
XmString *CreateMotifStringTable( int cnt, char *argv[], char *fontTag )
{
	XmString	*xmstrs;
	char		*tag;
	int		i;
	

	if( cnt <= 0 || argv == NULL )
		return( NULL );

	xmstrs = ( XmString * ) XtMalloc( sizeof( XmString ) * cnt );

	tag = XmFONTLIST_DEFAULT_TAG;
	if( fontTag )
		tag = fontTag;

	for( i = 0; i < cnt; i++ )
		xmstrs[i] = XmStringCreateLtoR( argv[i], tag );

	return( xmstrs );

}
