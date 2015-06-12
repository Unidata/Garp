/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	xsetpxmdata.c
 *
 *	Hooks the pixmap into GEMPAK.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 **********************************************************************/


#include "xwcmn.h"

void xsetpxmdata (	int	windowindex, 
		int	pixmapno,
		int	w, 
		int	h, 
		int	depth, 
		Pixmap	pixmapdata,
		int	*iret )

{

/*
 *	Install an application created pixmap into XW's gemwindow 
 *	pixmap array.  
 */

	Window_str	*g;

/*---------------------------------------------------------------------*/

	*iret    = G_NORMAL;

/*
 *	Set global window index to the new window.
 */
	current_window = windowindex;

/*
 *	Quick pointer access
 */
	g = &gemwindow[windowindex];

/*	g->window                =  window; */

	g->curpxm[0]             =  pixmapno;
	g->width                 =  w;
	g->height                =  h;
	g->depth                 =  depth;

	/* g->gc                    =  gc; */

	/* chiz/Unidata, update for xwcmn pixmap array */
	/*g->pixmaps[pixmapno]     =  pixmapdata;*/
	g->pxms[0][pixmapno]     =  pixmapdata;

/*
 *	Sanity check only.
 */
	if ( g->npxms != 0 && g->npxms != 1 ) {
		printf("xsetpxmdata: pixmap cnt should be 1 or 0, not %d\n",
			g->npxms);
		return;
	}

	g->npxms = 1;

	return;

}
