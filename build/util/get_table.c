/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	get_table.c
 *
 *	Table access utility.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include "geminc.h"
#include "gemprm.h"
#include "ctbcmn.h"
#include "genglobs.h"

void
get_table ( struct datatype_list master,
            struct station_list *station_info,
	    int *nstn,
	    int *iret )
{
	int	maxstn = LLSTFL;
	char	directory[256];

/*
 *	Strip of leading '$' character.
 */
/*
	strcpy ( directory, master.datadir );
	strdelc ( directory, '$', iret );
*/
/*
 *	Get directory where table is located.
 */
	expand_var ( "tabledir", directory );

/*
 *	Read location table.
 */
	readstn1 ( master.loctbl, directory, &maxstn, nstn,
	           station_info, iret );
}
