/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	readstn1.c
 *
 *	Station table file reading utility. Derived from the GEMPAK
 *	function ctb_rstn.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 4/97	COMET	Changed printf format to support logging.
 *
 ***********************************************************************/


#include "geminc.h"
#include "gemprm.h"
#include "ctbcmn.h"
#include "genglobs.h"

void
readstn1 ( tblnam, directory, maxstn, nstn, stnarr, iret )
    char		  *tblnam;
    char		  *directory;
    int	 		  *maxstn;
    int			  *nstn;
    struct station_list	  *stnarr;
    int		 	  *iret;
{
	FILE    *ftbl;
	char    record[133];
	char	file[256], output[256];
	int	i, kstn, ilat, ilon, ielv;
	int	verbose;

	FILE	*cfl_sopn();

/*---------------------------------------------------------------------*/
	*iret = 0;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "readstn1\n" );

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
	if ( verbose > VERBOSE_1 ) printf ("  Opening %s\n", file );

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

		    if ( verbose > VERBOSE_2 ) {
			sprintf ( output, "%s %d %s %s %s %d\t%d\t%d\t%d",
                               stnarr[kstn].stid, stnarr[kstn].stnm,
                               stnarr[kstn].name, stnarr[kstn].state,
                               stnarr[kstn].coun, ilat, ilon, ielv,
                               stnarr[kstn].prior );
			printf ("%s \n", output );
		    }

		    kstn++;
		}
	    }
	}

	*nstn = kstn;

	fclose ( ftbl );
}
