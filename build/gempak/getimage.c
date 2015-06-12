/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	getimage.c
 *
 *	Get image header and data.
 *
 *	History:
 *
 *	06/97	COMET	Original copy
 *	07/97	COMET	Optionally don't save image data
 *
 ***********************************************************************/


#include "winobj.h"
#include "underscore.h"
#include "genglobs.h"


int readimghdr (MetObjectType *mot);
unsigned char *readimgdata (char *filnam);


/*
 *	GetImage (): Acquires all the information about an image.
 *		     Returns 1 if OK, otherwise 0 for error
 */ 
 
int GetImage( MetObjectType *mot )
{

	unsigned char		*data;
	unsigned long		imgsize;
	AnyImageObjectType	*iot;
        GlobalDataObjectType	*gd;
        BooleanType		saveImageFlag;


	iot = (AnyImageObjectType *) mot->metObjectContent;

/*
 *	Read the image header
 */
	if ( ! readimghdr ( mot ) ) return 0;

/*
 *	Read image data (if not already read) and save it in the object unless
 *	the user turned off image data saving.
 */
	if ( iot->imgCmn.imageData == NULL ) {

	    data = readimgdata ( mot->filename );

	    gd  = GetGlobalDataObject();
	    saveImageFlag = GetSaveImageDataFlag ( gd);

	    if ( data == NULL ) {

	    	printf("Error: Could not allocate memory for image data. Try disabling the\n");
		printf("image data saving feature (Options->Status Bar/Pixel Value)\n");
		return 0;
	    }
	    else if ( saveImageFlag ) {
	    
		imgsize = iot->imgCmn.imnpix * iot->imgCmn.imnlin * iot->imgCmn.imdpth;
		iot->imgCmn.imageData = (unsigned char *) malloc ( imgsize );

		if ( iot->imgCmn.imageData != NULL )
		    memcpy ( iot->imgCmn.imageData, data, imgsize);

		else {
	    	    printf("Error: Could not allocate memory for image data. Try disabling the\n");
		    printf("image data saving feature (Options->Status Bar/Pixel Value)\n");
		    return 0;
		}
	    }

	}

	return 1;

}

