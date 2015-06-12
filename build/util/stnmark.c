/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	stnmark.c
 *
 *	Functions for handling station mark plotting.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include "gendef.h"
#include "geminc.h"
#include "gemprm.h"
#include "ctbcmn.h"
/* #include "nwxcmn.h" */

#define FILLEDCIRCLE 0

void
bpad ( char *s, int final_length, int *iret )
/*
 * Pad the end of a fixed length character with blanks.
 */
{
	int	len;
	int	i;

	len = final_length - strlen ( s ) - 1;

	if ( len < 0 ) {
	    printf("Error in function bpad\n");
	    *iret = len;
	    return;
	}

	for ( i=0; i<len; i++ ) strcat ( s, " " );
	strcat ( s, NULL );
}

void
stnmark ( int nstn, struct station_list *stn, int icolr,
          int marker, int pstn, int pmarker, int *iret )

{
	char	stid[5], *stations;
	float	lat[LLSTFL], lon[LLSTFL], elev[LLSTFL];
	float	values[LLSTFL], breaks [LLCLEV], sizmrk;
	int	color[LLSTFL];

	int	markid = 2;
	float	marksiz = 1.0;
	int     ncolrs, mrktyp, mrkwid, pltval, iposn;
	int	size_per = 5;
	int	len;
	int	i;
	
/*---------------------------------------------------------------------*/
	*iret = NORMAL;

/*
 *	Set the marker attributes.
 */

	for ( i=0; i<nstn; i++ ) color[i] = icolr;  /* one color */

/*
 *	Allocate space for station list.
 */
	len = nstn * size_per * sizeof (char);
	if ( ( stations = (char *) 
	       malloc ( ( len + 1 ) * sizeof (char) ) ) == NULL ) {
	    printf("Malloc failed\n" );
	    return;
	}

	strcpy ( stations, "" );
/*
 *	Prepare for FORTRAN consumption.
 */
	for ( i=0; i<nstn; i++ ) {
	    strcpy ( stid, stn->stid );
	    bpad ( stid, size_per, iret );
	    strcat ( stations, stid );
	    lat[i]  = stn->rlat;
	    lon[i]  = stn->rlon;
	    *stn++;
	}
	size_per--;

/*
 *	Draw the markers.
 */

	sym ( &nstn, &size_per, stations, &marker, &marksiz, &markid,
	      lat, lon, color, &pstn, &pmarker, &iret );

/*
 *	Free station list.
 */
	free ( stations );
}


