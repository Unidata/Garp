/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	readstn2.c
 *
 *	Table reading utility. Derived from the GEMPAK functions 
 *	ctb_rdtyp.
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
readstn2 ( tblnam, directory, maxdtyp, ndtyp, dtyparr, iret )
	char			*tblnam;
	char			*directory;
	int			*maxdtyp;
	int			*ndtyp;
	struct datatype_list	*dtyparr;
	int			*iret;
{
	FILE    *ftbl;
	char    record[133];
	char	file[256];
	int	i, kdtyp;

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
	for ( i = 0; i < *maxdtyp; i++ ) {
	    strcpy ( dtyparr[i].datatyp, " " );
	    strcpy ( dtyparr[i].loctbl,  " " );
	    strcpy ( dtyparr[i].bsflag,  " " );
	    strcpy ( dtyparr[i].datadir, " " );
	    strcpy ( dtyparr[i].filext,  " " );
	}

/*
 *	Open the table file.
 */
	if ( ( ftbl = fopen ( file, "r") ) == NULL ) {
	    *iret = -3;
	    return;
	}

/*
 *	Set station counter to zero.
 */
	kdtyp = 0;

/*
 *	Read in the next record, check for a comment,
 *	and process valid table entries.
 */
	while ( fgets ( record, 132, ftbl ) != NULL ) {
	    if ( ( record[0] != '!' ) &&
		 ( kdtyp < *maxdtyp ) ) {
		if ( sscanf ( record, "%s %s %s %s %s",
			      dtyparr[kdtyp].datatyp,
			      dtyparr[kdtyp].loctbl,
			      dtyparr[kdtyp].bsflag,
			      dtyparr[kdtyp].datadir, 
			      dtyparr[kdtyp].filext ) > 0 ) {
		    kdtyp++;
		}
	    }
	}

	*ndtyp = kdtyp;

	fclose ( ftbl );
}
