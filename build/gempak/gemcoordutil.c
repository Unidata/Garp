/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gemcoordutil.c
 *
 *	Set of routines for doing GEMPAK coordinate translations and
 *	queries. (Isolates direct gempak calls to here.)
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	04/97	COMET	Added getgemplotmode()
 *	11/97	COMET	Changed getimagecoords to remove .5 value.
 *	01/98	COMET	Modified getimagecoords to get image bounds from
 *			the first image in the pixmap (fixed bug when images
 *			of different sizes are loaded in a loop).
 *	 2/98	COMET	Fixed a bug brought on by the previous change.
 *
 ***********************************************************************/


#include <string.h>
#include "winobj.h"
#include "underscore.h"



int getbounds ( char *incoords,
		float *left,
		float *bottom,
		float *right,
		float *top )
{
	int	iret;


	if ( strlen (incoords) != 1 ) return (0);
	
	gqbnd ( incoords, left, bottom, right, top, &iret, strlen (incoords));

	if ( iret == 0 && *left  != RMISSD && *bottom != RMISSD &&
	                  *right != RMISSD && *top    != RMISSD)
	    return (1);
	else
	    return (0);
	    
}


int getgemplotmode ( )
{
	int 	mode, iret;	

	gqmode ( &mode, &iret );

	return (mode);
	    
}

int getlatlon ( char *incoords, float *x, float *y, float *lat, float *lon )
{

	char		Mcoords [] = "M";
	int		np = 1;
	int		iret, mode;

	
	if ( strlen (incoords) != 1 ) return (0);

	gqmode ( &mode, &iret );
	
	if ( mode == 2 ) return (0) ;

	return ( convertcoords ( incoords, Mcoords, x, y, lat, lon,
	         strlen(incoords), strlen(Mcoords) ));

	    
}


int getimagecoords ( PixmapObjectType *po, char *incoords, float *x, float *y, float *ix, float *iy )
{

	int			xleft, ytop, xrght, ybot;
	int			iret, np = 1;
	int			i, overlayClass;
	char 			Lcoords [] = "L", Vcoords [] = "V";
	float			xout, yout;
	float			xl, yb, xr, yt;	
	AnyImageObjectType      *iot;
	MetObjectType		*mo;
	

	if ( strlen (incoords) != 1 ) return (0);

/*
 *	Get the intermediate linear (L) coordinate bounds. If not defined,
 *	(as in the case of a cross section image) get the view coords.
 */
	iret = getbounds ( Lcoords, &xl, &yb, &xr, &yt );

/*
 *	Transform the input coordinate to L coords
 */
	convertcoords ( incoords, Lcoords, x, y, &xout, &yout, 
		 strlen(incoords), strlen(Lcoords));


/*
 *	If po is defined, get the image size from the first image
 */
	if ( po != NULL ) {

	    for ( i = 0; i < po->numMetObjects; i++ ) {

		mo = po->metObjects[i];
		overlayClass = GetMetObjectClass( mo->type );

		if ( overlayClass == IMAGE || overlayClass == BACKGROUND ) {
		    iot = (AnyImageObjectType *) mo->metObjectContent;
		    xleft = iot->imgCmn.imleft;
		    ytop  = iot->imgCmn.imtop;
		    xrght = iot->imgCmn.imrght;
		    ybot  = iot->imgCmn.imbot;
		    break;
		}
	    }
	}

/*
 *	Otherwise, get the image size from the gempak image common block
 */
	else {
	    xleft = getimgcomint ( "imleft", strlen("imleft") );
	    ytop  = getimgcomint ( "imtop",  strlen("imtop")  );
	    xrght = getimgcomint ( "imrght", strlen("imrght") );
	    ybot  = getimgcomint ( "imbot",  strlen("imbot")  );	
	}

/*
 *	Determine the image coordinates
 */
	*ix = ( xleft + (xout - xl) * (xrght - xleft) / (xr - xl) );
	*iy = ( ytop  + (yout - yt) * (ybot- ytop) / (yb - yt) );

	return (1);
}


int convertcoords ( 	char *incoords,
			char *outcoords,
			float *xin,
			float *yin,
			float *xout,
			float *yout )
{

	int		np = 1;
	int		iret;

	if ( strlen (incoords) != 1 || strlen (outcoords) != 1 ) return (0);
/*
 *	Call GEMPAK GTRANS to convert coords
 */	
	gtrans ( incoords, outcoords, &np, xin, yin, xout, yout, &iret,
	         strlen(incoords), strlen(outcoords) );

	if ( iret == 0 && *xout != RMISSD && *yout != RMISSD )
	    return (1);
	else
	    return (0);
	    
}
