/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	markerobj.c
 *
 *	Marker object creation and destruction functions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 3/97	COMET	Added "parm" to define what from station table
 *			should be displayed.
 *	11/97	COMET	Added toggledOn flag.
 *	12/97	COMET	Get default text settings from keys in "Garp_defaults".
 *
 ***********************************************************************/


#include "guimacros.h"
#include "genglobs.h"
#include "winobj.h"
#include "utils.h"
#include "ctbcmn.h"
#include "_proto.h"


void DestroyMarkerObject( MetObjectType *m )
{
	if( m && m->metObjectContent )
		free( m->metObjectContent );

	return;

}

MetObjectType *MakeMarkerObject( 
		int	tblindex,
		int	parm,
		int	marker,
		int	pstn,
		int	pmarker,
		int	*iret )
{
	
	MetObjectType		*m;
	MarkerObjectType	*mobj;
	GlobalDataObjectType	*gd;

	extern struct datatype_list	mastertbl[MAXTYP];
	extern struct station_list	station_info[LLSTFL];

	char	*str, stid[80];

	int	number, llen, stidlen=0;
	int	nstn, ncolrs, jcolr, icolrs[LLSTFL];
	int	i, verbose;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "MakeMarkerObject\n" );

	gd = GetGlobalDataObject();

	m = AllocateMot();
	if( !m ) {
		printf("*** MakeMarkerObject: Mot allocation error.\n");
		return( ( MetObjectType * ) NULL );
	}

	mobj = (MarkerObjectType *) malloc( sizeof( MarkerObjectType ) );
	if( ! mobj ) {
	    printf("*** MakeMarkerObject: malloc error.\n");
	    return( ( MetObjectType * ) NULL );
	}
/*
 *	Read and store NEXRAD radar sites.
 */
	get_table ( mastertbl[tblindex], &station_info, &nstn, iret );
	if ( *iret != NORMAL ) return;

/*
 *	Fill in the markerObject data.
 */
	GetNextObjectColor ( gd );
	mobj->color = atoi ( gd->colors );

	strcpy ( mobj->stations, "" );

/*
 *	Save the parameter to plot in a long string.
 */
	switch (parm) {
	case 0:
/*
 *	  Get length of the longest string.
 */
	  for ( i=0; i<nstn; i++ ) {
	    llen = strlen ( station_info[i].stid );
	    if ( llen > stidlen ) 
		stidlen = llen + 1;
	  }

	  for ( i=0; i<nstn; i++ ) {
	    strcpy ( stid,  station_info[i].stid );
	    bpad ( stid, stidlen, iret );
	    strcat ( mobj->stations, stid );
	  }
	  break;

	case 1:
	  stidlen = 9;
	  for ( i=0; i<nstn; i++ ) {
	    str = int2str ( station_info[i].stnm );
	    bpad ( str, stidlen, iret );
	    strcat ( mobj->stations, str );
	    Free ( str );
	  }
	  break;

	case 2:
/*
 *	  Get length of the longest string.
 */
	  for ( i=0; i<nstn; i++ ) {
	    llen = strlen ( station_info[i].name );
	    if ( llen > stidlen ) 
		stidlen = llen + 1;
	  }
	  for ( i=0; i<nstn; i++ ) {
	    strcpy ( stid, station_info[i].name );
	    bpad ( stid, stidlen, iret );
	    strcat ( mobj->stations, stid );
	  }
	  break;

	case 3:
	  stidlen = 9;
	  for ( i=0; i<nstn; i++ ) {
	    number = (int) station_info[i].elev;
	    str = int2str ( number );
	    bpad ( str, stidlen, iret );
	    strcat ( mobj->stations, str );
	    Free ( str );
	  }

	case 4:
	  stidlen = 9;
	  for ( i=0; i<nstn; i++ ) {
	    number = 3.2808 * (int) station_info[i].elev;
	    str = int2str ( number );
	    bpad ( str, stidlen, iret );
	    strcat ( mobj->stations, str );
	    Free ( str );
	  }
	  break;

	default:
	  break;
	}

	for ( i=0; i<nstn; i++ ) {
	    mobj->lat[i]  = station_info[i].rlat;
	    mobj->lon[i]  = station_info[i].rlon;
	}
	stidlen--;

	mobj->marker        = marker;
	mobj->nstn          = nstn;
	mobj->stidlen       = stidlen; 
	mobj->plot_stn      = pstn;
	mobj->plot_marker   = pmarker;
/*
 *	hard code the text type for now
 */
	strcpy ( mobj->symbol_size, "1.0" );
	strcpy ( mobj->symbol_width, "1" );
	sprintf ( mobj->text, "%s/%s/%s/%s", gd->text_size, gd->text_font,
	                              gd->text_width, gd->text_hw_flag );

	m->type             = MARKEROBJECT;
	m->displayFunc      = DisplayMarker;
	m->destroyFunc      = DestroyMarkerObject;
	m->toggledOn        = True;


/*
 *	Provide some geographic defualt
 */
	strcpy ( m->proj, "lcc/30.0;-100;50.0//nm" );
	strcpy ( m->garea, "23.0;-120.0;47.0;-65.0" );
	strcpy ( m->area, "23.0;-120.0;47.0;-65.0" );

/*
 *	Tie the MetObjectContentType data into the metObject.
 */
	m->metObjectContent = (MetObjectContentType *) mobj;

	return( m );

}
