/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	satobj.c
 *
 *	Satellite object creation and destruction functions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 6/97	COMET	Cleaned up
 *	 8/97	COMET	Added reference, forecast and valid times.
 *	 8/97	COMET	Added timeStamp to MakeSatObject() arg list.
 *	11/97	COMET	Added titleIndex to MakeSatObject() arg list.
 *	12/97	COMET	Added scale to image object creation routines
 *	 1/98	COMET	Changed NULL to '\0'
 *
 ***********************************************************************/


#include "winobj.h"
#include "guimacros.h"
#include "_proto.h"


void DestroySatObject( MetObjectType *m )
{

	SatObjectType		*sot;

	sot = (SatObjectType *) m->metObjectContent;

	if ( sot->imgCmn.imageData ) {
		Free ( sot->imgCmn.imageData );
        	sot->imgCmn.imageData = NULL;
	}

	if( m && m->metObjectContent )
		free( m->metObjectContent );


	return;

}


MetObjectType *MakeSatObject( int  metObjectType,
                              char *fileName,
                              char *timeStamp,
                              char *cbar,
                              int  titleIndex,
                              char *scale )
{
	
	MetObjectType		*mot;
	SatObjectType		*sot;

	assert( fileName );

	mot = AllocateMot();
	if( !mot ) {
		printf("MakeSatObject: Allocation error.\n");
		return( ( MetObjectType * ) NULL );
	}

	sot = (SatObjectType *) malloc( sizeof( SatObjectType ) );
	if( ! sot ) {
		printf("MakeSatObject: malloc error.\n");
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
	mot->refTime          = String2LongTime( timeStamp, &(mot->fcstTime) );
	mot->validTime        = mot->refTime + mot->fcstTime;
	mot->drawnFlag        = False;
	mot->inUse            = True;
	mot->toggledOn        = True;
	mot->titleIndex       = titleIndex;
	mot->type             = metObjectType;
	mot->displayFunc      = DisplaySat;
	mot->destroyFunc      = DestroySatObject;

/*
 *	Object-specific stuff
 */
	strcpy( sot->imgCmn.colorbar, cbar ); 
	strcpy( sot->imgCmn.scale, scale ); 
        sot->imgCmn.imageData = NULL;

/*
 *	Tie the MetObjectContentType data into the metObject.
 */
	mot->metObjectContent = (MetObjectContentType *) sot;

	return( mot );
}
