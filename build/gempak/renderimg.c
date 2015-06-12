/***********************************************************************
 *
 *	Copyright 1997, University Corporation for Atmospheric Research.
 *
 *	renderimg.c
 *
 *	Image rendering function. Component of xsatim(), except this
 *	copy only renders the image into the pixmap.
 *
 *	History:
 *
 *	 5/97	COMET	Original copy
 *	 7/97	COMET	Use image passed in or last one read
 *	10/97	COMET	Use function calls to get common block values
 *			rather than use them directly from imgdef.h
 *	 7/99	COMET	Re-implemented some byte copies using memcpy
 *                      and memset to avoid OSF compiler warnings.
 *	06/02	Penn State - A. Person
 *			Updated to support 16- and 24-bit graphics
 *
 ***********************************************************************/
 
 
#include <math.h>
#include "xwcmn.h"
#include "genglobs.h"
#include "color.h"


/* Define these here instead of including underscore.h because including
   xwcmn.h causes underscore.h to not do anything */

#ifdef UNDERSCORE
#define gqbnd	gqbnd_
#define gtrans	gtrans_
#define getimgcomflt	getimgcomflt_
#define getimgcomint	getimgcomint_
#endif   


#define MAXROWCOL       5000
#define MIN_SCALE	0.0001  /* minimum for rmxysc */

#define NONSHARE_SAT_COLOR 32
#define NONSHARE_RAD_COLOR 8


float getimgcomflt ( char *, int );
int   getimgcomint ( char *, int );


static XImage *ximg = (XImage *)NULL;
static unsigned char *imgDpy = NULL;
size_t last_dpysize;
extern unsigned char *imgData;  /* in case a NULL image is passed into here */

int renderimg ( unsigned char *image )



{
	unsigned char	*dptr, *ddptr;
	unsigned char	*imgptr;
	unsigned char	*tptr;
	unsigned int	background;
	unsigned int	pix,loadpix;
	unsigned long	row, col, newdim, remainder;
	int		ximage0, ximage1, yimage0, yimage1;
	int		xispace0, xispace1, yispace0, yispace1;
	int		rowtrans [MAXROWCOL], coltrans [MAXROWCOL];
	int		right, left, top, bottom;
	unsigned int   datamap [256];
	int		linestart, linesize, element_size;
	int		imgwdth, imghght, plotwdth, plothght;
	float		scalefact, imgratio, plotratio;
	int             i, k, drange;
	size_t		dpysize, imgsize;
	int		nBpp, nBpad; /* Number of bytes per pixel */

	Window          gwin; 
	Pixmap          gempixmap; 
	GC              gemgc; 

	int             ier, ipxm; 
	int             xwdth, xhght, xdpth; 
	int		format, offset, bitpad;
	int             ncolors, index;
	int		cbank, depth, minpix, maxpix;
	int		npixls, nlines, bswap;
	float           ratio, doffset;
	float		xyscale;

	unsigned long   pxls[MAXCOLORS];

	char		Pcoords [] = "P", Dcoords [] = "D";
	float		xl, xr, yt, yb, x, y;
	int		verbose, np;

	/*const int	istart=0;*/
	int	istart=0;

/*---------------------------------------------------------------------*/

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "renderimg()\n" );
/*
 *	Get items from the common block that we will need
 */
	left   = getimgcomint ( "imleft", strlen("imleft"));
	top    = getimgcomint ( "imtop ", strlen("imtop "));
	right  = getimgcomint ( "imrght", strlen("imrght"));
	bottom = getimgcomint ( "imbot",  strlen("imbot") );
	npixls = getimgcomint ( "imnpix", strlen("imnpix"));
	nlines = getimgcomint ( "imnlin", strlen("imnlin"));
	depth  = getimgcomint ( "imdpth", strlen("imdpth"));
	cbank  = getimgcomint ( "imbank", strlen("imbank"));
	xyscale= (float) getimgcomflt ( "rmxysc", strlen("rmxysc"));
	minpix = getimgcomint ( "immnpx", strlen("immnpx"));
	maxpix = getimgcomint ( "immxpx", strlen("immxpx"));
	bswap  = getimgcomint ( "imbswp", strlen("imbswp"));
		
/*
 *	Get current window parameters
 */
	xwdth = gemwindow[current_window].width;  
	xhght = gemwindow[current_window].height;  
	xdpth = gemwindow[current_window].depth;  

	gwin  = gemwindow[current_window].window; 
	gemgc = gemwindow[current_window].gc; 
	ipxm  = gemwindow[current_window].curpxm[0]; 
	gempixmap = gemwindow[current_window].pxms[0][ipxm]; 

/*
 *	Image area.
 */
	imgwdth = (right  - left) + 1 ;
	imghght = (bottom - top)  + 1 ;

	imgratio = (float)imghght / (float)imgwdth ;
	if ( ( xyscale != 1.0) && ( xyscale > MIN_SCALE )) 
		imgratio /= xyscale;
	
/*
 *	Plot area 
 */
	np = 1;
 	gqbnd ( Pcoords, &xl, &yb, &xr, &yt, &ier, strlen(Pcoords) );
        gtrans ( Pcoords, Dcoords, &np, &xl, &yb, &x, &y, &ier,
                 strlen(Pcoords), strlen(Dcoords));
	xispace0 = x + .5;
	yispace0 = y + .5;
        gtrans ( Pcoords, Dcoords, &np, &xr, &yt, &x, &y, &ier,
                 strlen(Pcoords), strlen(Dcoords));
	xispace1 = x + .5;
	yispace1 = y + .5;

	plotwdth  = xispace1 - xispace0; 
	plothght  = yispace0 - yispace1; 
	plotratio = (float)plothght/ (float)plotwdth; 

/*
 *	Calculate final image size: ( ximage0, ximage1, yimage0, yimage1 )
 *
 *	If the height to width of the space available for the image
 * 	is greater than the height to width of the image, width is
 * 	the limiting factor, and scale the height to keep the
 * 	appropriate aspect ratio.
 */
	if ( plotratio > imgratio ) { 

		ximage0 = xispace0;
		ximage1 = xispace1;

/*
 *		Center the image in the plot area
 */
		newdim    = plotwdth * imgratio;
		remainder = plothght - newdim;
		yimage0   = yispace0 - remainder / 2.;
		yimage1   = yispace1 + remainder / 2.;

	}

/*
 * 	Otherwise, If the height to width of the space available
 * 	for the image is less than the height to width of the
 * 	image, height is the limiting factor, and scale the width
 * 	to keep the appropriate aspect ratio.
 */
	else {
		yimage0 = yispace0;
		yimage1 = yispace1;

/*
 *		Center the image in the plot area
 */
		newdim    = (float)plothght / imgratio;
		remainder = plotwdth - newdim;
		ximage0   = xispace0 + remainder / 2.;
		ximage1   = xispace1 - remainder / 2.;

	}

/*
 *	Final image size
 */	 
	xwdth = ximage1 - ximage0 + 1;
	xhght = yimage0 - yimage1 + 1;

/*
 *	Allocate the satelllite/radar colors if needed
 */
	if ( ! allocflag[cbank] ) {

		xcaloc (cbank , &ier );
		if ( ier == G_ZEROCB ) {
		    /*
		     * allocate non-shareble colors
		     */
		     if (cbank == SatCid)
		       ColorBanks.banks[cbank] = NONSHARE_SAT_COLOR;
		     else if (cbank == RadCid)
                       ColorBanks.banks[cbank] = NONSHARE_RAD_COLOR;
		     ier = xgrwc ( gemdisplay, gemmap, pxls,
                                ColorBanks.banks[cbank]);
		     if ( ier == G_NORMAL ) {
                       /*
                        * allocate the memory for each color bank
                        */
                       ColorBanks.colrs[cbank] = (unsigned long *)
                                   malloc(ColorBanks.banks[cbank] * 
						sizeof(unsigned long));

			for ( i = 0; i < ColorBanks.banks[cbank]; i++) {
                	    ColorBanks.colrs[cbank][i] = pxls[i];
        		}
		     }
		}
		if  ( ier == G_NCLRAL ) {
			return ( ier );
		}

		allocflag[cbank] = G_TRUE;
	}

/*
 *	Construct the mapping from image data to final color
 * 	pixel index -- datamap[] (one byte images only)
 */

	drange = (int)pow (2., (double)(depth * 8 )); /* image data range */

	ncolors = ColorBanks.banks[cbank];
  
	ratio   = (float)(ncolors - 1)/ (float)(maxpix - minpix);
	doffset = minpix *ratio - 0.5; /* offset for data mapping */

	if ( depth == 1 ) {

		for ( i = 0; i < drange; i++ ) {

			if ( i < minpix )
			    datamap[i] = ColorBanks.colrs[cbank][0];
			else if ( i > maxpix )
			    datamap[i] = ColorBanks.colrs[cbank][ncolors - 1];
			else {
			    index = (int)(i * ratio  - doffset);
			    datamap[i] = ColorBanks.colrs[cbank][index];
			}

		}
	}

/*
 *	Fill the column translation array with incremental indecies
 *	into the original image columns with respect to the previous
 *	column. Off-image values are set to -1.
 */
        scalefact = (float)(imgwdth - 1) / (xwdth - 1);
        for ( col = 0; col < xwdth; col++ ) {
                coltrans [col] = (left - 1) + scalefact * col + .5 ;
                if ( coltrans [col] < 0 || coltrans [col] >= npixls )
                    coltrans [col] = -1;
		else
		    if ( (col != 0) && (coltrans[col -1] != -1) ) 
			coltrans[col] -= (int) ( (left - 1) + scalefact * (col-1) + .5);
        }

/*
 *	Fill the row translation array with indices into the
 * 	original image rows. Set off-image values to -1.
 */
	scalefact = (float)(imghght - 1) / (xhght -1);
        for ( row = 0; row < xhght; row++ ) {
                rowtrans [row] = (top - 1) + scalefact * row + .5 ;
                if ( rowtrans [row] < 0 || rowtrans [row] >= nlines )
                    rowtrans [row] = -1;
        }

/*
 *	Allocate memory for final image
 */
	nBpp = xdpth/8;  /* Number of bytes per screen pixel */

	/* 24-bit screen uses int size in cpu memory */
	if( nBpp == 3 ) 
	   nBpad = 4;
	else
	   nBpad = nBpp;

	dpysize = xhght * xwdth * nBpad;

	if( verbose > VERBOSE_0 )
		printf("renderimg(): dypsize is %d bytes\n", dpysize); 

        if ( ( imgDpy == (unsigned char *)NULL ) || 
                        (dpysize > last_dpysize) ) {

                if (imgDpy != (unsigned char *)NULL)
                        free(imgDpy);

                imgDpy = (unsigned char *)malloc(dpysize);

                if ( imgDpy == (unsigned char *)NULL ) {
                        return (G_NMEMRY);
                }
                last_dpysize = dpysize;
        }


/*
 *	Construct the final image for display. Loop over rows.
 */
	imgptr = imgDpy;
	background = ColorBanks.colrs[GraphCid][0];
 	element_size = depth ;
	linesize = npixls * element_size ;

	for ( row = 0; row < xhght; row++ ) {

/*
 *	    Opimization: no image for this line; set to background
 */
	    if ( rowtrans [row] == -1 ) {
                if( nBpp == 1 ) {
		    memset( imgptr, background, xwdth);
		    imgptr += xwdth;
                }
		else {
                    for ( col = 0; col < xwdth; col++) {
                       mv_itob ( (int *)(&background), &istart, &nBpad, imgptr, &ier);
                       imgptr += nBpad;
                    }
                }
		continue; /* next row */
	    }
/*
 *	    Opimization: line has same data as previous line; replicate
 */
	    else if ( (row != 0) && (rowtrans[row] == rowtrans[row -1]) ){
                /*
                 * replicate line
                 */
                memcpy(imgptr, imgptr - xwdth*nBpad, xwdth*nBpad);
                imgptr = imgptr + xwdth*nBpad ;
                continue; /* finish the row */
            }

/*
 *	    Normal case: loop over all the columns.
 */	     
	    linestart = rowtrans [row] * linesize;

/*
 *	    Use the image passed in or if NULL, the last one read.
 */
	    if ( image != NULL ) 
	        dptr = &image[linestart];
	    else
	        dptr = &imgData[linestart];

	    for ( col = 0; col < xwdth; col++) {

            	if ( coltrans [col] == -1 ) {
                    mv_itob ( (int *)(&background), &istart, &nBpad, imgptr, &ier);

                    imgptr += nBpad;
                }
		else { 

/*
 *		    Adjust the data pointer to the correct column 
 */
		    dptr += coltrans[col] * element_size;

/*
 *		    Get the image data. One byte images first.
 */
		    if ( depth == 1) 

			pix = *dptr;

/*
 *		    Multi-byte images; Check for byte swapping
 */
		    else {
		    
		    	pix = 0;
			if ( bswap )
			    ddptr = dptr + depth - 1;
			else
			    ddptr = dptr ;
			
		    	for ( k = 0; k < depth; k++ ) {
			    pix = pix << 8;
			    pix += *ddptr;
			    if ( bswap )
				ddptr --;
			    else 
				ddptr++ ;
		    	}	    	
		    }

/*
 *		    Convert pixel value to color. Use datamap[] for one-byte
 *		    images.
 */
		    if (depth < 2) {
                        loadpix = datamap[pix];
                        mv_itob ( (int *)(&loadpix), &istart, &nBpad, imgptr, &ier);
                        imgptr += nBpad;
		    }
		    else {
                        if (pix <= minpix) {
                            loadpix = ColorBanks.colrs[cbank][0];
                            mv_itob ( (int *)(&loadpix), &istart, &nBpad, imgptr, &ier);
                            imgptr += nBpad;
                        }
                        else if (pix >= maxpix) {
                            loadpix = ColorBanks.colrs[cbank][ncolors -1];
                            mv_itob ( (int *)(&loadpix), &istart, &nBpad, imgptr, &ier);
                            imgptr += nBpad;
                        }
                        else {
                            index = pix * ratio - doffset;
                            loadpix = ColorBanks.colrs[cbank][index];
                            mv_itob ( (int *)(&loadpix), &istart, &nBpad, imgptr, &ier);
                            imgptr += nBpad;
                        }
		    }

		}
	    }
	}
 
		    
/*
 *	Put the image into the pixmap.
 */
	offset = 0;
	format = ZPixmap;
	bitpad = 8;

	if ( ximg == (XImage *) NULL ) {
	    ximg = XCreateImage ( gemdisplay, gemvis, xdpth, format,
			offset, (char *)imgDpy, xwdth, xhght,
			bitpad, 0 );
	    /* all data will be MSB ordered */
            ximg->byte_order = MSBFirst;
	}
	else {
		ximg->width = xwdth;
		ximg->height = xhght;
		ximg->data = (char *)imgDpy;
		ximg->bytes_per_line = xwdth*nBpad;
	}

	XPutImage ( gemdisplay, gempixmap, gemgc, ximg, 0, 0,
		    ximage0, yimage1, xwdth, xhght );

	return ( G_NORMAL );
}
