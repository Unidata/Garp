/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	init_data.c
 *
 *	Data structure initilization routine.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory 
 *			sanity.
 *	 2/97	COMET	Added minzoombox, and status bar items/units
 *	12/97	COMET	Added call to SetDefaultFont ().
 *
 ***********************************************************************/


#include "guimacros.h"
#include "genglobs.h"
#include "utils.h"
#include "ctbcmn.h"
#include "winobj.h"
#include "_proto.h"



/*
 *	Instantiate  a few other data types...
 */

struct list_struct      list_info;
struct datatype_list	mastertbl[MAXTYP];
struct enhance_list	satenhance_list;
struct station_list	station_info[LLSTFL];


char	template_file[2048];

void
InitData ( int *iret )
{

	char			*request="template_file";
	int			verbose;
	char			*templatep;
	
	*iret = 0;


	verbose = GetVerboseLevel();
	if ( verbose > VERBOSE_0 ) 
		printf("InitData\n");

/*
 *	Set color list for displaying depictables.
 */
	SetColorList ();
	ZeroObjectColors ();

/*
 *	Set default font characteristics.
 */
	SetDefaultFont ();


#if 0  /* not needed anywhere  right now */
	templatep = (char *) GetConfigValue( request );
	if ( verbose > VERBOSE_1 )
	    printf("template_file = %s\n", 
			templatep ? templatep : "Not defined" );

	if( tempaltep )
		Free( templatep );
#endif


/*
 *	Read and store master file for table information.
 */
	init_tables ( &mastertbl, iret );


/*
 *	Initialize clock.
 */
	init_time ( GetCurrentTime(), GetStartTime(), 
			GetEndTime(), GetNoTick(), iret );
	

/*
 *	Initialize generalized depictable values.
 */
	DefaultDepictableValues ();

/*
 *	Initialize animation dwell defaults
 */
	InitDwellDefaults ();

/*
 *	Initialize the minimum zoom box size
 */
	InitMinZoomBox ();

/*
 *	Initialize the status bar items
 */
	InitStatusBarDefaults ();


}
