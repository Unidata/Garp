/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	ctbrstn.c
 *
 *	Table file reading utility
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include "geminc.h"
#include "gemprm.h"
#include "ctbcmn.h"

void
ctb_rstn ( tblnam, directory, maxstn, nstn, stnarr, iret )
    char		  *tblnam;
    char		  *directory;
    int	 		  *maxstn;
    int			  *nstn;
    struct station_list	  *stnarr;
    int		 	  *iret;

/************************************************************************
 * ctb_rstn								*
 *									*
 * This routine will read a station table into an array of structures.	*
 *									*
 * ctb_rstn ( tblnam, directory, maxstn, nstn, stnarr, iret )		*
 *									*
 * Input parameters:							*
 *	*tblnam		char		Station table name		*
 *	*directory		char		Directory path/symbol		*
 *	*maxstn		int		Maximum number of stations	*
 *									*
 * Output parameters:							*
 *	*nstn		int		Number of stations		*
 *	*stnarr		struct		Station list structure		*
 *	*iret		int		Return code			*
 *					   As for cfl_sopn		*
 **									*
 * Log:									*
 * S. Jacobs/NMC	 6/94						*
 * L. Williams/EAI	 7/94  Reformat header				*
 * S. Drake/COMET	 2/95  Allow for blanks in station name read	*
 ***********************************************************************/

{
	FILE    *ftbl;
	char    record[133];
	char	file[256];
	int	i, kstn, ilat, ilon, ielv;

	FILE	*cfl_sopn();

/*---------------------------------------------------------------------*/
	*iret = 0;

/*
 *	Build full path name.
 */
	strcpy ( file, directory );
	strcat ( file, "/" );
	strcat ( file, tblnam );

/*
 *	Initialize the structure values.
 */
	for ( i = 0; i < *maxstn; i++ ) {
	    strcpy ( stnarr[i].stid,  " " );
	    stnarr[i].stnm  = 0;
	    strcpy ( stnarr[i].name,  " " );
	    strcpy ( stnarr[i].state, " " );
	    strcpy ( stnarr[i].coun,  " " );
	    stnarr[i].rlat  = 0.;
	    stnarr[i].rlon  = 0.;
	    stnarr[i].elev  = 0.;
	    stnarr[i].prior = 0;
	}

/*
 *	Open the table file.
 */
	if ( ( ftbl = fopen ( file, "r") ) == NULL ) {
	    printf ("ctb_rstn - Could not open table file = %s\n", file );
	    *iret = -3;
	    return;
	}
/*
 *	Set station counter to zero.
 */
	kstn = 0;

/*
 *	Read in the next record, check for a comment,
 *	and process valid table entries.
 */
	while ( fgets ( record, 132, ftbl ) != NULL ) {
	    if ( ( record[0] != '!' ) &&
		 ( kstn < *maxstn ) ) {
		if ( sscanf ( record, "%s %d %32c %s %s %d %d %d %d",
			       stnarr[kstn].stid, &stnarr[kstn].stnm,
			       stnarr[kstn].name,  stnarr[kstn].state,
			       stnarr[kstn].coun, &ilat, &ilon, &ielv,
			      &stnarr[kstn].prior ) > 0 ) {
		    stnarr[kstn].stid[8] = '\0';
		    stnarr[kstn].name[32] = '\0';
		    stnarr[kstn].state[2] = '\0';
		    stnarr[kstn].coun[2] = '\0';
		    stnarr[kstn].rlat = ( (float) ilat ) / 100.;
		    stnarr[kstn].rlon = ( (float) ilon ) / 100.;
		    stnarr[kstn].elev = (float) ielv;
		    kstn++;
		}
	    }
	}

	*nstn = kstn;

	fclose ( ftbl );
}
