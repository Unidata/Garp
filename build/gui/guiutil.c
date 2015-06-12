/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	guiutil.c
 *
 *	Utilities used by widgets.	
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 4/97	COMET	Added error checking to SetDefaultOptionButton.
 *	11/97	COMET	Added error checking to GetOptionButtonLabel.
 *	11/97	COMET	Added GetStringArrayLabel.
 *	 1/98	COMET	Added sanity tests for undefined keys.
 *	 4/99   COMET   Added keylist string to SetDefaultOptionButton.
 *       7/99   UNIDATA Added return value (0) to SetDefaultOptionButton.
 *
 ***********************************************************************/


#include "gui.h"
#include "children.h"
#include "genglobs.h"
#include "_proto.h"


Widget
GetTopShell ( w )
	Widget	w;

/************************************************************************
* GetTopShell								*
*									*
* This function determines a top level widget and returns the shell.	*
*									*
* Input parameters:							*
*	w		Widget		Widget whose parent we are	*
*					looking for.			*
* Output parameters:							*
*	w		Widget		Parent widget			*
*									*
************************************************************************/
{

/*---------------------------------------------------------------------*/

/*
 *	Find the parent widget.
 */
	while ( w && !XtIsWMShell ( w ) ) w = XtParent ( w );
	return w;
}

void
DestroyShell ( widget, shell )
	Widget	widget;
	Widget	shell;

/************************************************************************
* DestroyShell								*
*									*
* This function destroys a widget.					*
*									*
* Input parameters:							*
*	widget		Widget	Widget used to call this function	*
*	shell		Widget	Widget to destroy			*
*									*
* Output parameters:							*
*	none								*
*									*
************************************************************************/
{

/*---------------------------------------------------------------------*/

/*
 *	Destroy the widget.
 */
	XtDestroyWidget ( shell );
}


int
SetDefaultOptionButton ( Widget option_menu,
			 int numkeys,
			 char *key,
			 char **keylist,
			 ButtonParentObjectType *bpo,
                         char *keylistString )
/*
 *	Set option menu button.
 */
{
	ButtonObject	*bo;
	int		i, verbose;


	verbose = GetVerboseLevel();
	if ( verbose > VERBOSE_0 )
	    printf ( "SetDefaultOptionButton\n" );
/*
 *	Error check.
 */
	if ( key == NULL ) {
	    printf("  Warning: key %s is null\n", key );
	    printf("           Cannnot set option menu default button\n" );
	    return -1;
	}
	if ( keylist == NULL ) {
	    printf("  Warning: keylist %s is null\n", keylistString );
	    printf("           Cannnot set option menu default button\n" );
	    return -2;
	}

	bo = &(bpo->buttons[0]);
	for ( i=0; i<numkeys; i++, bo++ )
	    if ( strcmp ( key, keylist[i] ) == 0 )
		XtVaSetValues ( option_menu,
					XmNmenuHistory, bo->w,
					NULL );
	return 0;
}


char *
GetStringArrayLabel ( int numkeys,
	              char *key,
	              char **keylist,
	              char **labellist )
{
/*
 *	Get the string from one string array that matches the index in
 *	another string array.
 */
	char	*label=NULL;
	int	i, verbose;


	verbose = GetVerboseLevel();
	if ( verbose > VERBOSE_0 )
	    printf ( "GetStringArrayLabel\n" );
/*
 *	Error check.
 */
	if ( key == NULL ) {
	    printf("  Warning: key is null\n" );
	}
	if ( *keylist == NULL ) {
	    printf("  Warning: keylist is null\n" );
	}
	if ( *labellist == NULL ) {
	    printf("  Warning: labellist is null\n" );
	}

	for ( i=0; i<numkeys; i++ )
	    if ( strcmp ( key, keylist[i] ) == 0 )
		label = strdup ( labellist[i] );

	return label;
}
