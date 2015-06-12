/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	titleobj.c
 *
 *	Title object creation and manipulation utilities.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	11/97	COMET	Enabled titleObject. Added GetNextTitleIndex and
 *			rewrote TitleObjectClearEntries.
 *	01/98	COMET	Added code to set toggled off object titles
 *
 ***********************************************************************/


#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/List.h>

#include "xwcmn.h"
#include "winobj.h"
#include "guimacros.h"


/* Define these here instead of including underscore.h because including
   xwcmn.h causes underscore.h to be ignored */

#ifdef UNDERSCORE
#define ptitle ptitle_
#define gqcolr  gqcolr_
#endif   


int		offColorIndex = -1;
XColor		color[1];
unsigned short	red = 50000;
unsigned short	green = 50000;
unsigned short	blue = 50000;
int		gemColor = 10;


TitleObjectType *TitleObjectAllocate();
void TitleObjectCreateList();
void TitleObjectClearEntries();
void TitleObjectUpdateEntries();
void TitleObjectAppendEntries();
void TitleObjectChangeDisplayState();


/*
 *	Draws a title in the "toggled off" color
 */
DrawToggledOffTitle ( char *title, int index )
{
	Display			*display;
	Colormap		cmap;
	GC			gemgc;
	WindowObjectType	*wo;
	Status			status;
	unsigned long		pixels [1];
	int			curGemColor, iret;
	static int		xdepth;

	wo = GetActiveWindowObject ();
	display = GetWindowObjectDisplay (wo);
	gemgc = gemwindow[current_window].gc;;
	cmap = DefaultColormap ( display, DefaultScreen (display) );


	if ( offColorIndex == -1 ) {
	    xdepth = DefaultDepthOfScreen(DefaultScreenOfDisplay(display));	
	    if (xdepth > 8 ) {
		color[0].red = red;
		color[0].green = green;
		color[0].blue = blue;
		color[0].flags = DoRed | DoBlue | DoGreen;

		if ( ! XAllocColor( display, cmap, color ) )
		    GemColorToXcolor ( gemColor, &color[0] );
		offColorIndex = color[0].pixel;
	    }

	    else {
	        status = XAllocColorCells ( display, cmap, False, NULL, 0,
					pixels, 1);

/*
 *	        Success getting new color. Set the attributes
 */
	        if ( status ) {
	    	    color[0].pixel = pixels[0];
		    color[0].red = red;
		    color[0].green = green;
		    color[0].blue = blue;
	        }

/*
 *	        Could not get color, use a GEMPAK one.
 */
	        else {
		    GemColorToXcolor ( gemColor, &color[0] );
	        }

	        offColorIndex = color[0].pixel; 
	        color[0].flags = DoRed | DoBlue | DoGreen;
	        XStoreColors ( display, cmap, color, 1 );
	    }

	}

/*
 *	Set the foreground color and draw the title
 */
	XSetForeground ( display, gemgc, offColorIndex );

	ptitle ( title, &index );

/*
 *	Get the current GEMPAK color and restore it to the foreground
 *	(otherwise, GEMPAK doesn't know the color changed and might keep
 *	drawing in the "offColorIndex" color.)
 */
	gqcolr ( &curGemColor, &iret );
	curGemColor = GemColorToPixel (curGemColor);
	XSetForeground ( display, gemgc, curGemColor);


}



/*
 *	Allocate the memory for the object and set everything
 *	to NULL.
 */
TitleObjectType *TitleObjectAllocate()
{

	TitleObjectType         *tot;

/*
 *	Disable for now...
 */
/*	return( ( TitleObjectType * ) NULL ); */

	tot = ( TitleObjectType * ) malloc ( sizeof( *tot ) );
	if ( ! tot ) {
		printf("TitleObjectAllocate: Can't create object.\n");
		return( ( TitleObjectType * ) NULL );
	}

	memset( tot, 0, sizeof( *tot ) );

	return( tot );
}


/*
 *
 *	Create the Motif stuff....
 *
 */
void TitleObjectCreateList( WindowObjectType *wo, Widget parent )
{

        Arg		al[64];
	int		ac;

	Widget		dialog, list;
	XmString	xmstrs[2];
	TitleObjectType	*tot;
	char		*argv[] = {"Radar is grand", "Data is too" };
	

/*
 *	Disable for now...
 */
	return;

	assert( wo->titleObject );

	tot =  wo->titleObject;

/*
 *	Set some default resources
 */
	ac = 0;
/*
 *	Tricky resource to remove all the motif decorations, but
 *	you can't move it then.
 */
/*
        XtSetArg( al[ac], XmNmwmDecorations, 0 );      ac++;

        XtSetArg( al[ac], XmNallowShellResize, True );      ac++;
        XtSetArg( al[ac], XmNtitle, "Display Information" ); ac++;
	XtSetArg( al[ac], XmNx, 10 );   ac++;
	XtSetArg( al[ac], XmNy, 10 );   ac++;

	dialog = tot->dialogWidget = 
		XmCreateBulletinBoardDialog( parent, "TitleObject", al, ac);

	ac = 0;
	XtSetArg( al[ac], XmNlistSizePolicy, XmCONSTANT );   ac++;
	XtSetArg( al[ac], XmNwidth, 100 );   ac++;
	XtSetArg( al[ac], XmNvisibleItemCount, 10 );   ac++;
	list = tot->listWidget = 
		XmCreateScrolledList ( dialog, "list", al, ac );

	XtManageChild ( list );
*/
/*
 *	Make sure it can show up on the screen.
 */
/*
	XtManageChild( dialog );
	
	return;
*/

}

void TitleObjectClearEntries( WindowObjectType *wo )
{
	TitleObjectType		*tot;
	int			i;

/*
 *	Check for valid input paramaters....
 */
	if( ! ( wo && wo->titleObject ) ) {
		printf("TitleObjectClearEntries:  Bad calling parameters:");
		printf("wo is %X,", wo );
		printf(" titleObject is %X\n", wo->titleObject);
		return;
	}

	tot =  wo->titleObject;

	for ( i=0; i<tot->numTitles; i++ ) {

	    tot->region[i].x1 = 0;
	    tot->region[i].y1 = 0;
	    tot->region[i].x2 = 0;
	    tot->region[i].y2 = 0;
	}

	tot->numTitles = 0;

	return;
}
	
void TitleObjectAppendEntries( WindowObjectType *wo, int cnt, char **argv )
{
	XmString		*xmstrs;
	TitleObjectType		*tot;
	
/*
 *	Disable for now...
 */
	return;

/*
 *	Check for valid input paramaters....
 */
	if( ! ( wo && wo->titleObject && cnt > 0 ) ) {
		printf("TitleObjectAppendEntries:  Bad calling parameters:");
		printf("wo is %X,", wo );
		printf(" titleObject is %X,", wo->titleObject );
		printf(" count is %d\n", cnt );
		return;
	}

/*
 *	Get a short-hand pointer...
 */
	tot =  wo->titleObject;
	
/*
 *	Create the XmStrings.....
 */
/*
	xmstrs = CreateMotifStringTable( cnt, argv, NULL );
*/

/*
 *	Put the items at the end of the list.
 */
	/*XmListAddItems(tot->listWidget, xmstrs, cnt, 0 ); */

/*
 *	Free the memory.
 */
	/* FreeMotifStringTable( cnt, xmstrs ); */

	return;

}
	
void TitleObjectUpdateEntries( WindowObjectType *wo, int cnt, char **argv )
{

	XmString		*xmstrs;
	TitleObjectType		*tot;
	
/*
 *	Disable for now...
 */
	return;


/*
 *	Check for valid input paramaters....
 */
	if( ! ( wo && wo->titleObject && cnt > 0 ) ) {
		printf("TitleObjectUpdateEntries:  Bad calling parameters:");
		printf("wo is %X,", wo );
		printf(" titleObject is %X,", wo->titleObject );
		printf("count is %d\n", cnt );
		return;
	}
	
/*
 *	Get a short-hand pointer...
 */
	tot =  wo->titleObject;

/*
 *	While we are debugging, Make sure it's in our face...
 */
/*
	if( ! XtIsManaged( tot->dialogWidget ) )
		XtManageChild( tot->dialogWidget );
*/
	
/*
 *	Create the XmStrings.....
 */
/*	xmstrs = CreateMotifStringTable( cnt, argv, NULL ); */

/*
 *	Set the scrolled list.....
 */
/*
	XtVaSetValues( tot->listWidget,
		XmNitems,	xmstrs,
		XmNitemCount,	cnt,
		NULL );
*/

/*
 *	Throw out the garbage....
 */
/*	FreeMotifStringTable( cnt, xmstrs ); */


/*
 *	That's all for now...
 */
	return;

}


int
GetNextTitleIndex ( WindowObjectType *wo )
{
	(wo->titleObject->numTitles)++;

	return wo->titleObject->numTitles;
}
