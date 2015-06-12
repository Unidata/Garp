/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	radarobj.c
 *
 *	Radar object creation and destruction functions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 6/97	COMET	Cleaned up
 *	 8/97	COMET	Added reference, forecast and valid times.
 *	 8/97	COMET	Added timeStamp to MakeRadarObject() arg list
 *	11/97	COMET	Added titleIndex to MakeRadarObject() arg list.
 *	12/97	COMET	Added scale to image object creation routines
 *	 1/98	COMET	Changed NULL to '\0' 
 *
 ***********************************************************************/


#include "winobj.h"
#include "guimacros.h"
#include "_proto.h"


void DestroyRadarObject( MetObjectType *m )
{

	RadarObjectType		*rot;

	rot = (RadarObjectType *) m->metObjectContent;

	if ( rot->imgCmn.imageData ) {
		Free ( rot->imgCmn.imageData );
        	rot->imgCmn.imageData = NULL;
	}

	if( m && m->metObjectContent )
		free( m->metObjectContent );


	return;

}


MetObjectType *MakeRadarObject( int metObjectType, 
                                char *fileName, 
                                char *timeStamp,
				char *cbar,
                                int titleIndex,
                                char *scale )
{
	
	MetObjectType		*mot;
	RadarObjectType		*rot;

	assert( fileName );


	mot = AllocateMot();
	if( !mot ) {
		printf("MakeRadarObject: Allocation error.\n");
		return( ( MetObjectType * ) NULL );
	}

	rot = (RadarObjectType *) malloc( sizeof( RadarObjectType ) );
	if( ! rot ) {
		printf("MakeRadarObject: malloc error.\n");
		return( ( MetObjectType * ) NULL );
	}

	strcpy( mot->proj, 	"RAD//nm" );
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
	mot->refTime          = String2LongTime( timeStamp, &(mot->fcstTime) );
	mot->validTime        = mot->refTime + mot->fcstTime;
	mot->drawnFlag        = False;
	mot->inUse            = True;
	mot->toggledOn        = True;
	mot->titleIndex       = titleIndex;
	mot->type             = metObjectType;
	mot->displayFunc      = DisplayRadar;
	mot->destroyFunc      = DestroyRadarObject;

/*
 *	Object-specific stuff
 */
	strcpy( rot->imgCmn.colorbar, cbar ); 
	strcpy( rot->imgCmn.scale, scale ); 
        rot->imgCmn.imageData = NULL;

/*
 *	Tie the MetObjectContentType data into the metObject.
 */
	mot->metObjectContent = (MetObjectContentType *) rot;

	return( mot );
}


