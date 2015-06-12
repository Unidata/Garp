/***********************************************************************
 *
 *	Copyright 1997, University Corporation for Atmospheric Research.
 *
 *	readimgdata.c
 *
 *	Image reading function. Component of xsatim() that only reads
 *	image data without displaying it.
 *
 *	History:
 *
 *	05/97	COMET	Original copy
 *	10/97	COMET	Changed to get values from GEMPAK IMGCMN common
 *			block, rather than using them from imgdef.h
 *	05/05	Unidata	Added csinit call to initialize imgdef.h commons
 *	11/10	Unidata	Added support for Hi-Res NIDS products
 *
 ***********************************************************************/
 
 
#include "xwcmn.h"
#include "imgdef.h"
#include "genglobs.h"


/* Define these here instead of including underscore.h because including
   xwcmn.h causes underscore.h to not do anything */

#ifdef UNDERSCORE
#define getimgcomint	getimgcomint_
#endif   


int   getimgcomint ( char *, int );


/* These are definied in imgdef.h and initialized with csinit()
unsigned char *imgData = NULL;

unsigned char *rawData = NULL;
size_t last_rawsize = 0;
*/
size_t last_imgsize = 0;


unsigned char *readimgdata ( char *filnam )
{

	FILE		*fp;
	int             ier;
	int		npixls, nlines, depth, imgtyp;
	int		verbose;
	int		imageType;
	size_t		imgsize;

/*---------------------------------------------------------------------*/

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "readimgdata(): file: %s\n",filnam );

	csinit( &ier );
	
/*
 *	Get the values we need from the GEMPAK image common block
 */
	npixls = getimgcomint ( "imnpix", strlen("imnpix"));
	nlines = getimgcomint ( "imnlin", strlen("imnlin"));
	depth  = getimgcomint ( "imdpth", strlen("imdpth"));
 	imgtyp = getimgcomint ( "imftyp", strlen("imftyp"));

/*
 * 	Allocate image data space
 */	     
	imgsize = npixls * nlines * depth;

	if ( ( imgData != (unsigned char *)NULL ) &&
	     ( imgsize <= last_imgsize ) ) {
	    memset( (unsigned char *)imgData, 0, imgsize );
	}
	else {
	    if (imgData != (unsigned char *)NULL)
		free(imgData);

	    imgData = (unsigned char *)calloc(imgsize, 
		sizeof(unsigned char));

	    if ( imgData == (unsigned char *)NULL ) {
		return (0);
	    }
	}

/*
 *	Read the image data for the type of image file format
 */
	switch ( imgtyp ) {

		case IFAREA:	/* AREA file */

				crarea ( filnam, &ier ) ;
				break;
		
		case IFGINI:	/* AWIPS GINI files */   

			        crgini ( filnam, &ier ) ;
				break;

		case IFNIDS:	/* NIDS radar files */   

				crnids ( filnam, &ier ) ;
				break;
	    
		case IFHINIDS:	/* Hi-Res NIDS radar files */   

				crnids ( filnam, &ier ) ;
				break;
	    
		case IFNEXZ:	/* NIDS radar files */   

				crnexz ( filnam, &ier ) ;
				break;
	    
		case IFNOWR:	/* WSI NOWRAD radar files */

				crnowr ( filnam, &ier ) ;
				break;

		case IFNCDF:  /* NetCDF files */

				crncdf ( filnam, &ier ) ;
				break;

		default:	/* error format */

				ier = G_NIMGFMT ;
				break;
	}


/*
 * 	Check for error. Set this file as the 'last' one if OK
 */
		
	if ( ier != G_NORMAL ) {
		return (0);
	}
	last_imgsize = imgsize ;

/*
 *	Return the image data
 */
	return (imgData);

}

