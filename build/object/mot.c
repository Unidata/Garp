/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	mot.c
 *
 *	Contains routines for handling metObjects.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/97	COMET	Added UPPERAIRPROFILEOBJECT.
 *	 3/97	COMET	Added XSECTIONGRIDFILLOBJECT case
 *	 5/97	COMET	Fixed FreeMot bug (was not calling metobject's
 *			destroy function)
 *	 6/97	COMET	Renamed BACKGROUNDOBJECT to BACKGROUNDIMAGEOBJECT
 *	 9/97	COMET	Added GetMetObjectStaticType()
 *	11/97	COMET	Added MakeMotTitle().
 *	 3/98	COMET	Added special case for SFC (level=0, vcord=NONE)
 *	 3/98	COMET	Removed Z from data time in MakeMotTitle()
 *	12/99	COMET	Save dattim in the format YYYYMMDD/HHMM.
 *
 ***********************************************************************/

#include "winobj.h"
#include "_proto.h"


char * DayOfWeek ( char * );
char * MonthOfYear ( int );


static int		metObjectFreeCnt = 0;
static MetObjectType	*metObjectFreeList[MAXMETOBJS];

int GetMetObjectClass( int metObjectType )
{

	int		class;

	switch ( metObjectType ) {

	case SCALARGRIDOBJECT:
		class = GRAPHICS;
		break;

	case SCALARGRIDFILLOBJECT:
		class = COLORFILL;
		break;

	case VECTORGRIDOBJECT:
		class = GRAPHICS;
		break;

	case SURFACEOBJECT:
		class = GRAPHICS;
		break;
		 
	case UPPERAIROBJECT:
		class = GRAPHICS;
		break;

	case UPPERAIRPROFILEOBJECT:
		class = GRAPHICS;
		break;

	case TIMEHEIGHTOBJECT:
		class = GRAPHICS;
		break;

	case VERTICALPROFILEOBJECT:
		class = GRAPHICS;
		break;

	case XSECTIONGRIDOBJECT:
		class = GRAPHICS;
		break;

	case XSECTIONGRIDFILLOBJECT:
		class = COLORFILL;
		break;

	case WINDPROFILEROBJECT:
		class = GRAPHICS;
		break;

	case RADAROBJECT:
		class = IMAGE;
		break;

	case SATOBJECT:
		class = IMAGE;
		break;

	case MAPOBJECT:
		class = MAP;
		break;

	case LATLONGRIDOBJECT:
		class = MAP;
		break;

	case MARKEROBJECT:
		class = OVERLAY;
		break;

	case RADARRINGOBJECT:
		class = OVERLAY;
		break;

	case NOWRADOBJECT:
		class = IMAGE;
		break;

	case BACKGROUNDIMAGEOBJECT:
		class = BACKGROUND;
		break;

	default:
		printf("GetMetObjectClass: Warning unknown ObjectType, %d\n", 
				metObjectType);
		class = GRAPHICS;
		break;
	}

	return( class );

}


int GetMetObjectView( int metObjectType )
{

	int		view;

	switch ( metObjectType ) {

	case SCALARGRIDOBJECT:
		view = PLAN;
		break;

	case SCALARGRIDFILLOBJECT:
		view = PLAN;
		break;

	case VECTORGRIDOBJECT:
		view = PLAN;
		break;

	case SURFACEOBJECT:
		view = PLAN;
		break;
		 
	case UPPERAIROBJECT:
		view = PLAN;
		break;

	case UPPERAIRPROFILEOBJECT:
		view = VERTICALPROFILE;
		break;

	case TIMEHEIGHTOBJECT:
		view = TIMEHEIGHTCROSSSECTION;
		break;

	case VERTICALPROFILEOBJECT:
		view = VERTICALPROFILE;
		break;

	case XSECTIONGRIDOBJECT:
		view = SPATIALCROSSSECTION;
		break;

	case XSECTIONGRIDFILLOBJECT:
		view = SPATIALCROSSSECTION;
		break;

	case WINDPROFILEROBJECT:
		view = TIMEHEIGHTCROSSSECTION;
		break;

	case RADAROBJECT:
		view = PLAN;
		break;

	case SATOBJECT:
		view = PLAN;
		break;

	case MAPOBJECT:
		view = PLAN;
		break;

	case LATLONGRIDOBJECT:
		view = PLAN;
		break;

	case MARKEROBJECT:
		view = PLAN;
		break;

	case RADARRINGOBJECT:
		view = PLAN;
		break;

	case NOWRADOBJECT:
		view = PLAN;
		break;

	case BACKGROUNDIMAGEOBJECT:
		view = PLAN;
		break;

	default:
		printf("GetMetObjectView: Warning unknown ObjectType, %d\n", 
				metObjectType);
		view = PLAN;
		break;
	}

	return( view );
}


int GetMetObjectStaticType( int metObjectType )
{

	switch ( metObjectType ) {

	case MAPOBJECT:
	case MARKEROBJECT:
	case RADARRINGOBJECT:
	case BACKGROUNDIMAGEOBJECT:
		return (STATICOBJECT);
		
	default:
		return( NONSTATICOBJECT);

	}


}

MetObjectType *CreateMot()
{

	MetObjectType		*m;

	/* printf("Sizeof MetObject is %d\n", sizeof( *m ) ); */

	m = (MetObjectType *) malloc( sizeof( *m ) );
	if( !m ) {
		printf("CreateMot:  malloc error.\n");
		return( (MetObjectType *) NULL );
	}


	return( m );

}


MetObjectType *CloneMot( MetObjectType *mot)
{

	MetObjectType	*m;

	m = AllocateMot ();

	if( !m ) {
		printf("CreateMot:  malloc error.\n");
		return( (MetObjectType *) NULL );
	}

	memcpy ( m, mot, sizeof (*mot));

	return( m );

}


void ReleaseMot( MetObjectType *m)
{

	int	i;
		
	assert( m );

	if( m->destroyFunc ) 
		(*m->destroyFunc)( m );

	if( metObjectFreeCnt >= MAXMETOBJS ) {
		printf("ReleaseMot:  Warning: Freelist overflow.\n");
		FreeMot( m );
	}

/*
 *	Add it to the freelist if it's not already there.
 */
	for ( i = 0; i < metObjectFreeCnt; i++ )
	    if ( metObjectFreeList[i] == m ) return;

	metObjectFreeList[metObjectFreeCnt++] = m;

	return;

}
	
void ReleaseMotList( int cnt, MetObjectType **m)
{
	
	int	i;

	assert( m );

	for( i = 0; i < cnt; i++, m++ ) {

		if( ! *m )
			continue;

		ReleaseMot( *m );

		*m = NULL;
	}

	return;

}
	
MetObjectType *AllocateMot()
{

	MetObjectType		*m;
	int			i;

	if( metObjectFreeCnt > 0 ) {
		m = metObjectFreeList[--metObjectFreeCnt];
		metObjectFreeList[metObjectFreeCnt] = (MetObjectType *)NULL;
	} else {
		m = CreateMot();
	}

	memset( m, 0, sizeof( MetObjectType ) );

	m->inUse       = True;
	m->drawnFlag   = False;

	return( m );

}

MetObjectType *GetNextMot( PixmapObjectType *pot )
{

	MetObjectType		**m;
	int			i;

	assert( pot );

	if( pot->numMetObjects >= MAXMETOBJS ) {
		printf( "GetNextMot: Hit maximum number of MetObjects.\n");
		return( (MetObjectType *) NULL );
	}

	*m = AllocateMot();

	(*m)->inUse       = True;
	(*m)->drawnFlag   = False;

	pot->numMetObjects++;

	return( *m );

}

void FreeMot( MetObjectType *m )
{
	
	assert( m );

	if( m->destroyFunc ) 
		(*m->destroyFunc)( m );

	free( m );

	return;

}

void FreeAllMots( PixmapObjectType *pot )
{
	int		i, numMetObjects;
	MetObjectType	**m;
	
	assert( pot );

	numMetObjects = pot->numMetObjects;
	
	for( i = 0, m = pot->metObjects; i < numMetObjects; i++, m++ ) {

		if( ! *m ) {
			printf("FreeAllMots: metobject list inconsistant.\n");
			continue;
		}

		FreeMot( *m ); 

		*m = (MetObjectType *) NULL;

		pot->numMetObjects--;

	}

	assert( pot->numMetObjects == 0 );

	return;

}

void FreeMotList( int cnt, MetObjectType **m )
{
	int i;

	for(i = 0; i < cnt; i++ )
		FreeMot( m[i] );
}


void
MakeMotTitle ( char *title, char *dattim, char *product, char *level,
	       char *vlevel, char *label, char *location )
{
	char	buf[GEMPAKSTRING];
	char	time[20], day[4], imon[3], year[5], slash[2];
	char	*dayw, *month;
	int	len=FILENAMESTRING, iret;
	BooleanType	pdattim = False;

	strcpy ( title, " " );
	bpad ( title, len, &iret );
	strcpy ( title, " " );

/*
 *	Parse the input dattim string formatted as: YYYYMMDD/HHMM????? where
 *	the question marks (?) could represent model forecast time.
 */
	if ( dattim != NULL ) {
/*
 *	Write date/time to title as a dattim string.
 */
	    if ( pdattim ) {
		if ( strcmp ( dattim, "MISSING" ) != 0 )
	    	    strcat ( title, dattim );
	    }
/*
 *	Write date/time to title in components.
 */
	    else if ( strlen (dattim)>10 ) {
	    	sscanf ( dattim, "%4s%2s%2s%1s%s",
	                         year, imon, day, slash, time );
	    	dayw = DayOfWeek ( dattim );
	    	month = MonthOfYear ( str2int(imon) );;

		strcat ( title, dayw );
		strcat ( title, " " );
		strcat ( title, month );
		strcat ( title, " " );
		strcat ( title, day );
		strcat ( title, " " );
		strcat ( title, year );
		strcat ( title, " " );
		strcat ( title, time );

		Free ( dayw );
		Free ( month );
	    }
/*
 *	Date time is missing so don't write it to the title.
 */
	    else if ( strcmp ( dattim, "MISSING" ) == 0 )
		(void) 0;

/*
 *	Error check.
 */
	    else
	    	printf ("MakeMotTitle - input time is invalid\n");
	}

/*
 *	Write product ( or model ) to title.
 */
	if ( product != NULL ) {
	    strcat ( title, " " );
	    strcat ( title, product );
	}

/*
 *	Special case for level = 0, vcord = none
 */
	if ( level && vlevel &&
	     strncmp (level, "0", 1 ) == 0 &&
	   ( strncmp (vlevel, "NONE", 4 ) == 0 || strncmp (vlevel, "none", 4 ) == 0 )) {
	     strcat ( title, " (SFC)" );
	}
	else {
/*
 *	    Write vertical level to title.
 */
	    if ( level != NULL ) {
		strcat ( title, " (" );
		strcat ( title, level );
	    }

/*
 *	    Write vertical coordinate units to title.
 */
	    if ( vlevel != NULL ) {
		if ( level == NULL )
		    strcat ( title, " (" );
		else
		    strcat ( title, " " );
		strcat ( title, vlevel );
		strcat ( title, ")" );
	    }
	    else
		if ( level != NULL ) strcat ( title, ")" );
	}

/*
 *	Write label ( or model field ) to title.
 */
	if ( label != NULL ) {
	    strcat ( title, " " );
	    strcat ( title, label );
	}

/*
 *	Write station location to title.
 */
	if ( location != NULL ) {
	    sprintf ( buf, "%s%s%s", " (", location, ")" );
	    strcat ( title, buf );
	}
}
