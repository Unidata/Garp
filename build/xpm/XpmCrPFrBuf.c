/* Copyright 1989-94 GROUPE BULL -- See license conditions in file COPYRIGHT */
/*****************************************************************************\
* XpmCrPFrBuf.c:                                                              *
*                                                                             *
*  XPM library                                                                *
*  Parse an Xpm buffer and create the pixmap and possibly its mask            *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

#include "xpmP.h"

int
XpmCreatePixmapFromBuffer(display, d, buffer, pixmap_return,
			  shapemask_return, attributes)
    Display *display;
    Drawable d;
    char *buffer;
    Pixmap *pixmap_return;
    Pixmap *shapemask_return;
    XpmAttributes *attributes;
{
    XImage *ximage, *shapeimage;
    int ErrorStatus;

    /* initialize return values */
    if (pixmap_return)
	*pixmap_return = 0;
    if (shapemask_return)
	*shapemask_return = 0;

    /* create the images */
    ErrorStatus = XpmCreateImageFromBuffer(display, buffer,
					   (pixmap_return ? &ximage : NULL),
					   (shapemask_return ?
					    &shapeimage : NULL),
					   attributes);

    if (ErrorStatus < 0)	/* fatal error */
        return (ErrorStatus);

    /* create the pixmaps and destroy images */
     if (pixmap_return && ximage) {
	xpmCreatePixmapFromImage(display, d, ximage, pixmap_return);
	XDestroyImage(ximage);
    }
    if (shapemask_return && shapeimage) {
	xpmCreatePixmapFromImage(display, d, shapeimage, shapemask_return);
	XDestroyImage(shapeimage);
    }

    return (ErrorStatus);
}
