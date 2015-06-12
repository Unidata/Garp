/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	init_tables.c
 *
 *	Initialize GARP tables function
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/

#include "stdio.h"
#include "string.h"
#include "geminc.h"
#include "gemprm.h"
#include "ctbcmn.h"
#include "genglobs.h"

void
init_tables ( struct datatype_list *table, int *iret )
{
	char			name[256];
	char			directory[256];
	int			num;
	int			maxnum = MAXTYP;

/*
 *	Get name and location of master table.
 */
	expand_var ( "mastertbl", name );
	expand_var ( "tabledir", directory );

/*
 *	Read master table into structure.
 */
	readstn2 ( name, directory, &maxnum, &num, table, iret );
}
