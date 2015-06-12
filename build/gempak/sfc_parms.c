/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	sfc_parms.c
 *
 *	Surface parameter function
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include <string.h>

#include "gendef.h"
#include "guimacros.h"

void
sfc_parms ( char *sfparm,			char *temperature,
             BooleanType temperature_toggle,	BooleanType weather_toggle,
             BooleanType dewpt_toggle,		BooleanType stid_toggle,
             BooleanType pressure_toggle,	BooleanType pres_change_toggle,
             BooleanType precip_toggle,		BooleanType marker_toggle,
             BooleanType barb_toggle,		BooleanType cloud_toggle,
	     int wind_units )
{
	char	filler[6];
	char	*space="SPAC;";
	char	*blank="BLNK;";
	char	*semicolon=";";

	strcpy ( filler, blank );

	strcpy ( sfparm, "" );

	/* position 0  - sky conditions */
	if ( cloud_toggle )       strcat ( sfparm, "SKYC:1:1;");
	else                      strcat ( sfparm, filler );

	/* position 1 - temperature */
	if ( temperature_toggle ) {
	    if      ( strcmp ( temperature, "F" ) == 0 )
		strcat ( sfparm, "TMPF;");
	    else if ( strcmp ( temperature, "C" ) == 0 )
		strcat ( sfparm, "TMPC;");
	}
	else                      strcat ( sfparm, filler );

	/* position 2 - current weather */
	if ( weather_toggle )     strcat ( sfparm, "WSYM:1:1;");
	else                      strcat ( sfparm, filler );

	/* position 3 - sea level pressure */
	if ( pressure_toggle )    strcat ( sfparm, "SMSL;");
	else                      strcat ( sfparm, filler );

	/* position 4 - */
	if ( pres_change_toggle ) strcat ( sfparm, "P03C;");
	else                      strcat ( sfparm, filler );

	/* position 5 */
	if ( dewpt_toggle ) {
	    if      ( strcmp ( temperature, "F" ) == 0 )
		strcat ( sfparm, "DWPF;");
	    else if ( strcmp ( temperature, "C" ) == 0 )
		strcat ( sfparm, "DWPC;");
	}
	else                      strcat ( sfparm, filler );

	/* position 6 */
	if ( precip_toggle )      strcat ( sfparm, "P03I;");
	else                      strcat ( sfparm, filler );

	/* position 7 */
	if ( stid_toggle )        strcat ( sfparm, "STID;");
	else                      strcat ( sfparm, filler );

	/* position 8 */
	strcat ( sfparm, filler );

	/* position 9 */
	strcat ( sfparm, filler );

	/* no position */
	if ( barb_toggle ) {
	    if      ( wind_units == KNOTS )
				  strcat ( sfparm, "BRBK:1:1;");

	    else if ( wind_units == METERSPERSECOND )
				  strcat ( sfparm, "BRBM:1:1;");

	    else                  strcat ( sfparm, filler );
	}

	else                      strcat ( sfparm, filler );


	return;
}
