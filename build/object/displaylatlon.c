/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	displaylatlon.c
 *
 *	Lat/lon grid display driver.
 *
 *	History:
 *
 *	10/97	COMET	Original copy
 *
 ***********************************************************************/


#include "winobj.h"
#include "genglobs.h"
#include "underscore.h"


int DisplayLatLonGrid( WindowObjectType *wo, MetObjectType *mot )
{

	int			verbose, iret;
	LatLonGridObjectType	*lobj;

	lobj = (LatLonGridObjectType *) mot->metObjectContent;

	assert( lobj );

        verbose = GetVerboseLevel();
        if( verbose > VERBOSE_0 )
            printf ( "DisplayLatLonGrid\n" );


	pllgrid ( lobj->line_color,
		lobj->line_type,
		lobj->line_width,
		lobj->lat_inc,
		lobj->lon_inc,
		lobj->label_freq,
		&iret,
		strlen( lobj->line_color),
		strlen( lobj->line_type),
		strlen( lobj->line_width),
		strlen( lobj->lat_inc),
		strlen( lobj->lon_inc),
		strlen( lobj->label_freq));


	SetMetObjectDrawnFlag( mot, True );

	return(0);

}
