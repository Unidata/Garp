/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	upa_parms.c
 *
 *	Upper air parameter sifter.
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
upa_parms ( char *snparm,		BooleanType temperature,
	     BooleanType dewpt,		BooleanType stid,
             BooleanType height,	BooleanType height_change,
             BooleanType wind,		int units,
	     int symbol )
{
	char	filler[6];
	char	*space="SPAC;";
	char	*blank="BLNK;";
	char	*semicolon=";";

	strcpy ( filler, blank );

	strcpy ( snparm, "" );

	/* position 0  - wind */
	if ( wind ) {
	    if      ( units == METERSPERSECOND && symbol == BARB )
	                          strcat ( snparm, "BRBM:1:1;");

	    else if ( units == METERSPERSECOND && symbol == ARROW )
                                  strcat ( snparm, "ARRW:1:1;");	

	    else if ( units == KNOTS && symbol == BARB )
                                  strcat ( snparm, "BRBK:1:1;");

	    else if ( units == KNOTS && symbol == ARROW )
                                  strcat ( snparm, "ARRK:1:1;");

	    else                  strcat ( snparm, filler );
	}

	else                      strcat ( snparm, filler );

	/* position 1 - temperature in Celcius */
	if ( temperature ) strcat ( snparm, "TMPC;");
	else                      strcat ( snparm, filler );

	/* position 2 - NOT USED */
	strcat ( snparm, filler );

	/* position 3 - geopotential height */
	if ( height )             strcat ( snparm, "HGHT;");
	else                      strcat ( snparm, filler );

	/* position 4 - NOT USED */
	strcat ( snparm, filler );

	/* position 5 - dew point temperature */
	if ( dewpt )              strcat ( snparm, "DPDC;");
	else                      strcat ( snparm, filler );

	/* position 6 - 12 hour height change NOT AVAILABLE??? */
	strcat ( snparm, filler );

	/* position 7 - 3 letter station id */
	if ( stid )               strcat ( snparm, "STID;");
	else                      strcat ( snparm, filler );

	/* position 8 */
	strcat ( snparm, filler );

	/* position 9 */
	strcat ( snparm, filler );

	/* no position */


	return;
}
