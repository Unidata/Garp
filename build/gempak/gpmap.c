/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gpmap.c
 *
 *	Image display routine.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	04/97	COMET	Added call to graphdraw() for cross section
 *			images. Removed drawMapFlag argument.
 *	05/97	COMET	Reworked for version 2.0
 *	11/97	COMET	Add check for bad return from renderimage()
 *	11/97	COMET	Added ptitle to display clickable titles.
 *	11/97	COMET	Added call to dscolor, cleaned up.
 *	12/97	COMET	Added args to arg list, call MakeMotTitle instead
 *			of gg_sttl()
 *	 1/98	COMET	Removed some unused title string stuff, changed
 *			some NULLS to '\0'.
 *	 3/98	COMET	Fixed text size bug. Get, then set text attributes
 *			in the device driver.
 *	 3/98	COMET	Moved text stuff into ptitle().
 *	 5/99	COMET	Added GEMPAK color setting routines to get
 *			correct title color.
 *	06/02	Penn State - A. Person
 *			Updated to support 16- and 24-bit graphics
 *
 ***********************************************************************/
/*
 *  Copyright 1996, University Corporation for Atmospheric Research.
 */

#include <string.h>

#include "genglobs.h"
#include "underscore.h"
#include "winobj.h"

#ifdef UNDERSCORE
#define gqtext  gqtext_
#define dstext  dstext_
#define gqcolr  gqcolr_
#define dscolr  dscolr_
#endif

void gpmap (	char *image, char *dattim, char *product,
		char *scale, char *lutfile, char *title,
		int color, int line, char *text, char *cbar,
		int titleIndex, int verbose, int *ret) 
{

	int	iret, mode;
        int     gcolr, oldcolr;
        int     gtxfn, gtxhw, gtxwid, dtxfn, dtxhw, dtxwid;
        float   gsztext, dsztext;
        int     ibrdr, irrotn, ijust, dbrdr, drrotn, djust; /* chiz */

	
	if ( verbose > 0 ) printf ("gpmap\n");
	if ( verbose > 1 ) {
	    if ( dattim  != NULL ) printf ("  dattim  = %s\n",dattim);
	    else                   printf ("  dattim  = NULL\n");
	    if ( product != NULL ) printf ("  product = %s\n",product);
	    else                   printf ("  product = NULL\n");
	    if ( scale   != NULL ) printf ("  scale   = %s\n",scale);
	    else                   printf ("  scale   = NULL\n");
	    if ( lutfile != NULL ) printf ("  lutfile = %s\n",lutfile);
	    else                   printf ("  lutfile = NULL\n");
	    if ( title   != NULL ) printf ("  title   = %s\n",title);
	    else                   printf ("  title   = NULL\n");
	    printf ("  title color = %d\n",color);
	    printf ("  title line  = %d\n",line);
	    if ( text    != NULL ) printf ("  text     = %s\n",text);
	    else                   printf ("  text     = NULL\n");
	    if ( cbar    != NULL ) printf ("  colorbar = %s\n",cbar);
	    else                   printf ("  colorbar = NULL\n");
	    printf ("  title index = %d\n",titleIndex);
	    printf ("  verbose  = %d\n",verbose);
	}

/*
 *      Load the LUT file if specified
 */
        if ( lutfile [0] != '\0' )
		newlut ( lutfile );
	
/*
 *	Display the image.
 */
	iret = renderimg ( image );

	if ( iret != 0 ) 
	    printf ("Error: could not allocate image colors.\n");

/*
 *	Load the LUT file if specified
 */ 
/*
	if ( lutfile [0] != '\0' )
		newlut ( lutfile );
*/

/*
 *	If the title string is NULL, create one here.
 */
	if ( title [0] == '\0' ) {
	    MakeMotTitle ( title, dattim, product, NULL, NULL, NULL, scale );
	}

/*
 *	Set the text attributes
 */
	in_text ( text, &iret, strlen (text));
	gscolr ( &color, &iret);

/*
 *      Make sure the text attributes are set correctly, since we are
 *      bypassing GEMPAK routines to draw the title.
 */
        gqtext ( &gtxfn, &gtxhw, &gsztext, &gtxwid, &ibrdr, &irrotn, &ijust, &iret);
        dstext ( &gtxfn, &gtxhw, &gsztext, &gtxwid, &ibrdr, &irrotn, &ijust,
                 &dtxfn, &dtxhw, &dsztext, &dtxwid, &dbrdr, &drrotn, &djust, &iret);
        gqcolr ( &gcolr, &iret );
        dscolr ( &gcolr, &oldcolr, &iret );

/*
 *	Write title.
 */
	ptitle ( title, &titleIndex );

/*
 *	Get the plot mode and draw a graph if we're in graph mode. It
 *	would be better not to do this here, but right now GARP has no
 *	concept of a non-plan view images.
 */
	gqmode ( &mode, &iret );
	if ( mode == 2 ) graphdraw ();

/*
 *	Draw the color bar
 */
	colorbardraw ( cbar, strlen (cbar) );

/*
 *	Flush any pending gempak graphics
 */
	/*gflush( &iret );*/

	*ret = iret;
}
