/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	wprofobj.c
 *
 *	Wind profiler object creation and destruction functions.	
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 8/97	COMET	Added reference, forecast and valid times.
 *	 9/97	COMET	Changed which time becomes reference time
 *	11/97	COMET	Added call to MakeMotTitle.
 *
 ***********************************************************************/


#include "guimacros.h"
#include "winobj.h"
#include "genglobs.h"


void DestroyWindProfilerObject( MetObjectType *m )
{
	if( m && m->metObjectContent )
		free( m->metObjectContent );

	return;

}

MetObjectType *MakeWindProfilerObject ( char *cxstn,	   char **snfiles,
					int  nfiles,
					char *dattim,	   char *yaxis,
					char *taxis,	   char *wind_symbol,
					char *symbol_size, char *symbol_width,
					char *fint,	   char *text,
					int  titleIndex )
{
	
	MetObjectType		*m;
	WindProfilerObjectType	*wobj;

	char			area[81], garea[81], proj[81];
	char			*refTimeStr;
	char			*label="<-- Time GMT (DD/HHMM)   Wind Profiler Plot    Station";
	int			i, verbose;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "MakeWindProfilerObject\n" );

	m = AllocateMot();
	if( !m ) {
		printf("*** MakeWindProfilerObject: Mot allocation error.\n");
		return( ( MetObjectType * ) NULL );
	}

	wobj = (WindProfilerObjectType *) malloc( sizeof
					( WindProfilerObjectType ) );
	if( ! wobj ) {
		printf("*** WindProfilerObjectType: malloc error.\n");
		return( ( MetObjectType * ) NULL );
	}

/*
 *	Get projection information.
 */
        strcpy ( proj, "XSEC" );
        strcpy ( garea, "" );
        strcpy ( area, "" );

/*
 *	Fill in the common metObject data.
 */
	strcpy( m->area,      area );
	strcpy( m->garea,     garea );
	strcpy( m->proj,      proj );
	strcpy( m->timestamp, dattim );

/*
 *	Extract the middle time ("end time") for reference
 */
	refTimeStr = strchr ( dattim, '-' ) + 1;
	m->refTime          = String2LongTime( refTimeStr, &(m->fcstTime) );
	m->validTime        = m->refTime + m->fcstTime;
	m->drawnFlag        = False;
	m->inUse            = True;
	m->toggledOn        = True;
	m->titleIndex       = titleIndex;
	m->type             = WINDPROFILEROBJECT;
	m->displayFunc      = DisplayWindProfiler;
	m->destroyFunc      = DestroyWindProfilerObject;

/*
 *	Create a title string.
 */
	MakeMotTitle ( m->titleString, NULL, NULL, NULL, NULL, label, cxstn );

/*
 *	Fill in the specific details of the Wind Profiler Object.
 */
	strcpy ( wobj->cxstn,  cxstn  );
	strcpy ( wobj->dattim, dattim );
	strcpy ( wobj->yaxis,  yaxis  );
	strcpy ( wobj->taxis,  taxis  );
	strcpy ( wobj->fint,   fint   );
	strcpy ( wobj->text,   text   );
	strcpy ( wobj->wind_symbol,  wind_symbol );
	strcpy ( wobj->symbol_size,  symbol_size );
	strcpy ( wobj->symbol_width, symbol_width );

	for ( i=0; i<nfiles; i++ )
	    strcpy ( wobj->snfiles[i], snfiles[i] );
	wobj->nfiles = nfiles;

/*
 *	Tie the MetObjectContentType data into the metObject.
 */
	m->metObjectContent = (MetObjectContentType *) wobj;

	return( m );

}
