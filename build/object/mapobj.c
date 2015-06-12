/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	mapobj.c
 *
 *	Map object creation and destruction functions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	10/97	COMET	Changed args to MakeMapObject, name of display 
 *			function
 *	11/97	COMET	Removed mapIndex, added toggledOn flag.
 *
 ***********************************************************************/

#include "genglobs.h"
#include "winobj.h"
#include "_proto.h"



/*
 *	Called automatically when the MetObject is destroyed.
 */

void DestroyMapObject( MetObjectType *m )
{
	if( m && m->metObjectContent )
		free( (char *) m->metObjectContent );
	return;

}

MetObjectType *MakeMapObject( 
		char	*mapname,
		char	*mapfile,
		char	*line_color,
		char	*line_type,
		char	*line_width)
{
	
	MetObjectType		*m;
	MapObjectType		*mobj;
	char			*proj, *area, *garea;

	if( GetVerboseLevel() > VERBOSE_0 )
        	printf("MakeMapObject\n");

	
	m = AllocateMot();
	if( !m ) {
		printf("*** MakeMapObject: Mot allocation error.\n");
		return( ( MetObjectType * ) NULL );
	}

	mobj = (MapObjectType *) malloc( sizeof( MapObjectType ) );
	if( ! mobj ) {
		printf("*** MakeMapObject: malloc error.\n");
		return( ( MetObjectType * ) NULL );
	}
/*
 *	Get projection information.
 */
        proj = GetConfigValue ( "default_proj" );
	if ( proj == NULL ) proj = strdup ( "lcc/30.0;-100;50.0//nm" );

	garea = GetConfigValue ( "default_garea" );
	if ( garea == NULL ) garea = strdup ( "23.0;-120.0;47.0;-65.0" );

	area  = strdup ( garea );

/*
 *	Fill in the common metObject data.
 */
	strcpy( m->filename,    mapfile );
	strcpy( m->area,        area );
	strcpy( m->garea,       garea );
	strcpy( m->proj,        proj );
	strcpy( m->timestamp,   "0" );
	strcpy( m->titleString,  mapname );

	m->refTime          = 0;
	m->validTime        = 0;
	m->fcstTime         = 0;
	m->drawnFlag   	    = False;
	m->inUse            = True;
	m->toggledOn        = True;
	m->type             = MAPOBJECT;
	m->displayFunc      = DisplayBackgroundObject;
	m->destroyFunc      = DestroyMapObject;


/*
 *	Fill in the specific details of the Map Object.
 */
	strcpy(mobj->mapfile, mapfile);
	strcpy(mobj->line_color, line_color);
	strcpy(mobj->line_type,  line_type);
	strcpy(mobj->line_width, line_width);

/*
 *	Tie the MetObjectContentType data into the metObject.
 */
	m->metObjectContent = (MetObjectContentType *) mobj;

/*
 *	Free.
 */
	Free ( proj );
	Free ( area );
	Free ( garea );
	
	return( m );

}

