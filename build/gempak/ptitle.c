#include "xwcmn.h"
#include "winobj.h"
#include "_proto.h"
#include "genglobs.h"


/*
 *  Do the underscore thing this way, instead of including underscore.h, 
 *  because of problems with including xwcmn.h as well.
 */

#ifdef UNDERSCORE
#define gqtext  gqtext_
#define dstext  dstext_
#define gqcolr  gqcolr_
#define dscolr  dscolr_
void ptitle_ ( char *title, int *titleIndex )
#else
void ptitle ( char *title, int *titleIndex )
#endif


/***********************************************************************
 *
 *	Copyright 1997, University Corporation for Atmospheric Research.
 *
 *	ptitle.c
 *
 *	Image display routine.
 *
 *	History:
 *
 *	11/97	COMET	Original copy
 *	11/97	COMET	Changed titleIndex to index starting at "0"
 *			instead of "1">
 *	 3/98	COMET	Added test for title display.
 *	 3/98	COMET	Added XFlush() call and reset text attributes.
 *	 3/98	COMET	Removed XFlush() and added color setting
 *	 5/99	COMET	Return if title length < 2 since a blank title
 *			is lengthened to 1 by MakeMotTitle.
 *	 5/99	COMET	Commented out GEMPAK color setting functions
 *			to get correct title color.
 *
 ***********************************************************************/

{
	WindowObjectType	*wo;
	GlobalDataObjectType	*gd;
	TitleObjectType		*tot;

	int    		ipxm; 
	Pixmap 		gempixmap; 
	GC     		gemgc; 
	Dimension	width;		/* string width in pixels */
	Dimension	iy, ix;
        int             direction, ascent, descent;
	int		lenstr;
	int		itype = 0, lineWidth;
	int		i, iret, verbose, titleLine;
	Dimension	xmax, ymax;
        XCharStruct     overall;
	Display		*display;
	char		*str;

        int     gcolr, oldcolr;
        int     mode, gtxfn, gtxhw, gtxwid, dtxfn, dtxhw, dtxwid;
        float   gsztext, dsztext;
	int     ibrdr, irrotn, ijust, dbrdr, drrotn, djust; /* chiz */

/*---------------------------------------------------------------------*/

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "ptitle\n" );

	wo = GetActiveWindowObject();
	tot = wo->titleObject;
	gd = GetGlobalDataObject();

/*
 *	Return if there is no title or if title is toggled off.
 *	MakeMotTitle returns a title of at least one character.
 */
	if ( title == NULL || strlen ( title ) < 2 ) return;
	if ( ! GetTitleFlag ( gd ) ) return;

/*
 *	Remove trailing blanks.
 */
	strbrm ( title );

/*
 *	Set the title line number.
 */
	titleLine = -1 * (*titleIndex + 1);
	i = *titleIndex;

	str = strdup ( title );
	strbrm ( str );
	lenstr = strlen ( str );

	/* chiz/Unidata update for xwcmn.h array of pixmaps */	
        ipxm      = gemwindow[current_window].curpxm[0]; 
        gempixmap = gemwindow[current_window].pxms[0][ipxm]; 
        gemgc     = gemwindow[current_window].gc; 
	display   = GetWindowObjectDisplay( wo );

/*
 *	Make sure the text attributes are set correctly, since we are
 *	bypassing GEMPAK routines to draw the title.
 */
/*
        gqtext ( &gtxfn, &gtxhw, &gsztext, &gtxwid, &ibrdr, &irrotn, &ijust, &iret);
        dstext ( &gtxfn, &gtxhw, &gsztext, &gtxwid, &ibrdr, &irrotn, &ijust,
                 &dtxfn, &dtxhw, &dsztext, &dtxwid, &dbrdr, &drrotn, &djust, &iret);
	gqcolr ( &gcolr, &iret );
	dscolr ( &gcolr, &oldcolr, &iret );
*/

        /*
         * Get string width for alignment
         */

        XQueryTextExtents(gemdisplay, XGContextFromGC(gemgc),
                str, lenstr, &direction, &ascent, &descent,
                &overall);

        width = overall.width;
	lineWidth = ascent + descent;

	xmax = GetWindowObjectWidth ( wo );
	ymax = GetWindowObjectHeight ( wo );

	/*
	 * Justify alignment for different value of *itype
	 */

	switch (itype) {
	    case  0: /* centered */

			ix = (xmax - width) / 2;
			break;

	    case  1: /* right justified */

			ix = xmax - width;
			break;

	    case -1: /* left justified */
			ix = 2;
			break;

	    default: 
			ix = 2;
			break;
	    }
	
	/*
	 * Draw string on window
	 */

	iy = ymax + ((titleLine + 1) * lineWidth) - descent;

/*
 *	Define cursor active region for title.
 */
	if ( verbose > VERBOSE_1 )
	    printf("  title number = %d\n", i );

	tot->region[i].x1 = ix;
	tot->region[i].y1 = iy;
	tot->region[i].x2 = ix + width;
	tot->region[i].y2 = iy - lineWidth;

	if ( verbose > VERBOSE_1 ) {
	    printf("  region: %d,", tot->region[i].x1 );
	    printf(" %d",           tot->region[i].y1 );
	    printf("     %d,",      tot->region[i].x2 );
	    printf(" %d\n",         tot->region[i].y2 ); 
	}
/*
 *	Write title.
 */
	XDrawString ( gemdisplay, gempixmap, gemgc, ix, iy, str,
                      lenstr );

/*
 *	Draw rectangle around string.
 */
/*
	XDrawRectangle ( gemdisplay, gempixmap, gemgc,
	  tot->region[i].x1,
	  tot->region[i].y2,
	  tot->region[i].x2 - tot->region[i].x1,
	  tot->region[i].y1 - tot->region[i].y2 );
*/
	
	Free ( str );
}
