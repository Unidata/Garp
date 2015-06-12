/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	bgimageobj.c
 *
 *	Background image object creation and destruction functions.
 *
 *	History:
 *
 *	 6/97	COMET	Original copy
 *	 8/97	COMET	Initialize time stamps to zero.
 *	 9/97	COMET	Added timeStamp to MakeBackgroundImageObject() arg
 *			list
 *	11/97	COMET	Added toggledOn flag to metObject.
 *	11/97	COMET	Added titleIndex to metObject.
 *	12/97	COMET	Added scale to object creation routine
 *	 1/98	COMET	changed NULL to '\0'
 *
 ***********************************************************************/


#include "winobj.h"
#include "guimacros.h"

#include "_proto.h"

void DestroyBackgroundImageObject( MetObjectType *m )
{

	BackgroundImageObjectType	*bot;

	bot = (BackgroundImageObjectType *) m->metObjectContent;

	if ( bot->imgCmn.imageData ) {
		Free ( bot->imgCmn.imageData );
        	bot->imgCmn.imageData = NULL;
	}

	if( m && m->metObjectContent )
		free( m->metObjectContent );


	return;

}


MetObjectType *MakeBackgroundImageObject( int metObjectType,
	                                  char *fileName,
					  char *timeStamp,
	                                  char *cbar,
	                                  int  titleIndex,
	                                  char *scale )
{
	
	MetObjectType			*mot;
	BackgroundImageObjectType	*bot;


	assert( fileName );

	mot = AllocateMot();
	if( !mot ) {
		printf("MakeBackgroundImageObject: Allocation error.\n");
		return( ( MetObjectType * ) NULL );
	}

	bot = (BackgroundImageObjectType *) malloc( sizeof( BackgroundImageObjectType ) );
	if( ! bot ) {
		printf("MakeBackgroundImageObject: malloc error.\n");
		return( ( MetObjectType * ) NULL );
	}

	strcpy( mot->proj, 	"SAT//nm" );
	strcpy( mot->garea,	"DSET");
	strcpy( mot->area,	"DSET");
	strcpy( mot->filename,	fileName );
	strcpy( mot->timestamp, timeStamp );

/*
 *	Leave the title string NULL, a default title will be created later
 */
	mot->titleString[0]   = '\0';

/*
 *	Fill in the rest of the stuff
 */
	mot->refTime          = 0;
	mot->fcstTime         = 0;
	mot->validTime        = 0;
	mot->drawnFlag        = False;
	mot->inUse            = True;
	mot->toggledOn        = True;
	mot->titleIndex       = titleIndex;
	mot->type             = metObjectType;
	mot->displayFunc      = DisplayBackgroundImage;
	mot->destroyFunc      = DestroyBackgroundImageObject;

/*
 *	Object-specific stuff
 */
	strcpy( bot->imgCmn.colorbar, cbar );
	strcpy( bot->imgCmn.scale, scale );
	bot->imgCmn.imageData = NULL;


/*
 *	Tie the MetObjectContentType data into the metObject.
 */
	mot->metObjectContent = (MetObjectContentType *) bot;

	return( mot );
}

