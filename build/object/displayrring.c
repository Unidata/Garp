/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	displayrring.c
 *
 *	Radar ring display driver.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include "underscore.h"
#include "winobj.h"
#include "wincb.h"
#include "genglobs.h"
#include "_proto.h"


int DisplayRadarRing( WindowObjectType *wo, MetObjectType *mot )
{
	RadarRingObjectType     *mobj;
	MetObjectType		*gmobj;

	int			verbose, iret;

	mobj = (RadarRingObjectType *) mot->metObjectContent;

	assert( mobj );

        verbose = GetVerboseLevel();
        if( verbose > VERBOSE_0 )
            printf ( "DisplayRadarRing\n" );


	prring (
		&mobj->nstn,		mobj->stations,
		mobj->stid,
                mobj->lat,		mobj->lon,
	        &mobj->color,		&mobj->number,
	        &mobj->increment,	&mobj->minimum,
		mobj->units,		&verbose,
	        &iret,
		strlen(mobj->stations),	strlen(mobj->stid),
		strlen(mobj->units) );

	SetMetObjectDrawnFlag( mot, True );

	return(0);

}
