/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	objectmgr.c
 *
 *	Routines which manage displayable objects.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 6/97	COMET	changed BACKGROUNDOBJECT to BACKGROUNDIMAGEOBJECT
 *	 6/97	COMET	Added check for resetting policy
 *	 8/97	COMET	Added tests for NULL metObjects. Removed
 *			TimestampInfoType related code.
 *	 9/97	COMET	Add static objects to new pixmaps, make the number
 *			of metobjects consistent among frames (changed in
 *			CopyMetObjectsToList)
 *	11/97	COMET	Modified CopyMetObjectsToList to not clone MAP
 *			objects when it creates new pixmaps. Removed 
 *			incrementing of pot->numMaps.
 *	11/97	COMET	Added toggledOn flag and titleIndex to metObjects.
 *	 1/98	COMET	Fixed bug passing *wo to GetNextTitle
 *
 ***********************************************************************/


#include <string.h>
#include "winobj.h"
#include "genglobs.h"


/*
 *	RemoveMetObjectsFromPixObject()
 *
 *	Removes all occurrences of a particular class of metObjects
 *	from the input pixmap object.
 */

void
RemoveMetObjectsFromPixObject ( PixmapObjectType *pot, int class )
{

	int		j, k, kk;
	int		numMetObjects;
	int		objectClass;


/*
 *	Loop through metobjects and remove the offending met objects.
 *	Upon releasing an object, slide the remaining ones "down".
 */
	numMetObjects = pot->numMetObjects;

	for ( j = 0, k = 0; j < numMetObjects; j++ ) {

	    objectClass = GetMetObjectClass ( 
			  GetMetObjectType (pot->metObjects[k]));

	    if ( objectClass == class ) {

		ReleaseMot ( pot->metObjects[k] );

		for ( kk = k; kk < pot->numMetObjects - 1; kk++ )
			pot->metObjects[kk] = pot->metObjects[kk+1];

		pot->numMetObjects--;

		pot->metObjects[pot->numMetObjects] = NULL;

	    }
	    else
		k++;

	}

}


/*
 *	RemoveMetObjectsFromWinObject()
 *
 *	Removes all occurrences of a particular class of metObjects
 *	from the all the pixmap objects.
 */

void
RemoveMetObjectsFromWinObject ( WindowObjectType *wo, int class )
{

	int			i;
	PixmapObjectType	**pv;
	int			numPixmapObjects;
	

	numPixmapObjects = wo->numPixmapObjects;
	
	for(i = 0, pv = wo->pixmapObjects; i < numPixmapObjects; i++, pv++ )

	    RemoveMetObjectsFromPixObject ( *pv, class );

}



/*
 *	Copy metObjects to successive pixmapObjects. (Time matching is done
 *	before we get here.) New pixmapObjects will be created if necessary.
 *
 *	This routine keeps the number of metobjects constant for all the
 *	pixmaps. This makes auto update simpler.
 */

void CopyMetObjectsToList( WindowObjectType *wo, 
			int cnt, 
			MetObjectType **mlist )
{

	PixmapObjectType	**pv;
	PixmapObjectType	*pot, *firstPot;
	MetObjectType		*mot, *m;
	int			i, j, k;
	int			staticType;
	int			numPixmapObjects;



/*
 *	If we have more objects coming in than pixmap objects, create new
 *	pixmapObjects. 
 *
 *	NOTE:  We have to use a temp to hold wo->numPixmapObjects,
 *		because, it can/will get modified by GetNextPot().
 */
	numPixmapObjects = wo->numPixmapObjects;
	if( numPixmapObjects < cnt ) {
	    for(i = 0; i < (cnt - numPixmapObjects); i++ ) {
		pot =  GetNextPot( wo );

/*
 *		When a new pixmapObject is created, clone metObjects
 *		(except MAPS) from the first pixmapObject and add them.
 *		Make static metobjects drawable, and others (data) not in use.
 */
		firstPot = wo->pixmapObjects[0];
		for ( j = 0; j < firstPot->numMetObjects; j++ ) {
		
		    mot = firstPot->metObjects[j];

		    if ( GetMetObjectClass ( mot->type ) == MAP ) continue;
		    
		    m = CloneMot ( mot );
		    m->drawnFlag = False;
		    m->inUse = False;
		    pot->metObjects[pot->numMetObjects] = m;
		    pot->numMetObjects++;

		    staticType = GetMetObjectStaticType (GetMetObjectType (mot));

		    if ( staticType == STATICOBJECT ) {
		    	m->inUse = True;
		    }
		}	
	    }
	}

/*
 *	Insert the incoming metObject between the last non-static
 *	metObject and the first static one. In other words, keep
 *	data objects first, followed by static objects.
 */
	for ( i = 0; i < wo->numPixmapObjects; i++ ) {

	    pot = wo->pixmapObjects[i];
	    
	    for ( j = 0; j < pot->numMetObjects; j++ ) {
    
		mot = pot->metObjects[j];
		staticType = GetMetObjectStaticType (GetMetObjectType (mot));

		if ( staticType == STATICOBJECT ) {

		    for ( k = pot->numMetObjects; k > j; k-- ) 
		    	pot->metObjects[k] = pot->metObjects[k-1];

		    break;
		}
	    }

	    pot->numMetObjects++;

	    if ( i < cnt ) 
		pot->metObjects[j] = mlist[i];
	    else {
		m = CloneMot ( mlist[0] );
		m->drawnFlag = False;
		m->inUse = False;
		m->toggledOn = False;
		pot->metObjects[j] = m;
	    }

	}

	return;
}



/*
 *	Take a list of metObjects (all of the same type) and adds 
 *	it to successive frames	in each Pixmap object.  
 *
 *	It makes decisions on what to do for the whole display, based
 *	on the current state of the windowObject and on what is being
 *	added. Normally it just adds objects to the existing list. But
 *	there are two scenarios where a full reset is performed.
 *
 *	1) if images are already loaded, and images are being added, and
 *	   the implicit reset for image loading flag is on
 *	2) if the view type changes 
 *
 */
void AddObjectListToDisplay( int cnt, MetObjectType **mlist )
{

	WindowObjectType	*wo;
	PixmapObjectType	*po;
	MetObjectType		*mot;
	int			metObjectType;
	int			overlayClass, oldclass;
	int			i;
	BooleanType		reset;
        GlobalDataObjectType    *gd;


	assert( cnt > 0 && mlist );

	if( (wo = GetActiveWindowObject()) == (WindowObjectType *) NULL ) {
		printf("AddObjListToDisplay: No active window.\n");
		return;
	}

	AnimationObjectDestroy( wo );

/*
 *	If these metObjects are of class IMAGE, then check to see if
 *	other IMAGE metObjects are already loaded and if the implicit
 *	reset for images flag is on, then do a reset.
 */
        gd = GetGlobalDataObject();

	for ( i=0; i<cnt; i++ ) {
	    if ( mlist[i] != NULL ) {
		metObjectType = GetMetObjectType ( mlist[i] );
		break;
	    }
	}

	overlayClass  = GetMetObjectClass( metObjectType );
	po = wo->pixmapObjects [0];

	if ( po && (overlayClass == IMAGE )) {
	    
	    reset = GetImageResetFlag (gd);

	    for ( i = 0; i < po->numMetObjects; i++ ) {

		oldclass = GetMetObjectClass( GetMetObjectType( po->metObjects[i]));

		if (( oldclass == IMAGE || oldclass == BACKGROUND ) && reset ) {
		    WindowObjectFullReset( wo );
		    mot = mlist[i];
		    mot->titleIndex = GetNextTitleIndex ( wo );
		    break;
		}
	    }
	}

/*
 *	Blast everything in..
 */
	CopyMetObjectsToList( wo, cnt, mlist );

}


/*
 *	Copies one metObject to all the pixmapObjects in the display
 *	(window). Creates clones of the metObject so that they are
 *	unique data structures. If BACKGROUND images are being added,
 *	and an IMAGE or BACKGROUND is already loaded, then the new
 *	BACKGROUND replaces the IMAGE or BACKGROUND. No view type
 *	checks are performed here, assuming that the interface 
 *	will prevent the user from selecting incompatible views.
 */

void CopyObjectListToDisplay(	WindowObjectType *wo, int cnt, 
				MetObjectType **mlist )
{

	PixmapObjectType	**pv, *po;
	MetObjectType		*m;
	int			metObjectType;
	int			overlayClass, oldclass;
	int			i, j;
	int			numPixmapObjects;



	metObjectType = GetMetObjectType ( mlist[0] );
	overlayClass  = GetMetObjectClass( metObjectType );

	if ( wo->numPixmapObjects == 0 ) 
	    po =  GetNextPot( wo );

	po = wo->pixmapObjects[0];

/*
 *	If the incoming object is a BACKGROUND object, and we have images
 *	loaded, do a clear.
 */
	if ( overlayClass == BACKGROUND )

	    for ( i = 0; i < po->numMetObjects; i++ ) {

		oldclass = GetMetObjectClass( GetMetObjectType( po->metObjects[i]));

		if ( oldclass == IMAGE ||  oldclass == BACKGROUND ) {

		    RemoveMetObjectsFromWinObject ( wo, oldclass);
		    ClearAllPots (wo);
		    SetWindowObjectDrawnFlag (wo, False);

		    break;
		}
	    }		 


/*
 *	Copy the input metObject(s) to the first pixmapObject. For the 
 *	remaining pixmaps, make copies (clones) of the metObject(s) so that
 *	they are unique.
 */
	numPixmapObjects = wo->numPixmapObjects;
	for ( i = 0, pv = wo->pixmapObjects; i < numPixmapObjects; i++, pv++ )

	    for ( j = 0; j < cnt; j++ ) {

		if ( i == 0 )
		    m = mlist[j];
		else
		    m = CloneMot ( mlist[j] );

		((*pv)->metObjects)[(*pv)->numMetObjects] = m;

		(*pv)->numMetObjects++;

	    }
		
	return;
}


/*
 *
 *	Adds a list of background metobjects (maps) to a pixmapObject
 *
 */

void AddMapBackgroundObjects (	PixmapObjectType *pot,
				int cnt,
				MetObjectType **mlist )
{

	WindowObjectType	*wo;
	int			j;


	if( (wo = GetActiveWindowObject()) == (WindowObjectType *) NULL ) {
		printf("AddMapListToDisplay: No active window.\n");
		return;
	}

/*
 *	Make sure we have at least one pixmap
 */
	if ( wo->numPixmapObjects == 0 )
		pot = GetNextPot ( wo );

/*
 *	Add the list of mapObjects to this pixmapObject
 */

	for ( j = 0; j < cnt; j++ ) {
	    
	    pot->metObjects[pot->numMetObjects] = mlist[j];

	    pot->numMetObjects++;

	}

}


/*
 *	Creates an order for the metobjects to be drawn in. The order
 *	follows the "class" definition ranking. It would be nice to be 
 *	able to use something like qsort() but we would like to leave
 *	same-valued items in the order they appear; most sorting
 *	algorithms don't guarantee this.
 */

void GetMetObjectDrawOrder (MetObjectType **mots, int numMetObjects, int *order )
{
		
	int	pos, i, j, class, pclass;


	pos = 0;

	for ( i = 0; i <= LASTCLASS; i++ )

	    for ( j = 0; j < numMetObjects; j++ )

		if ( GetMetObjectClass (GetMetObjectType( mots[j] )) == i ) 
		    order [pos++] = j;

/*
 *	Walk through the metObjects again and see if there are multiple
 *	IMAGE class objects. We will only display the last one, so
 *	set the inUse flag on the previous ones to false so that they
 *	will be ignored.
 */
	for ( j = 1; j < numMetObjects; j++ ) {

	    class  = GetMetObjectClass (GetMetObjectType( mots[order[j]] ));
	    pclass = GetMetObjectClass (GetMetObjectType( mots[order[j-1]] ));

	    if ( class == IMAGE && class == pclass )
		mots[order[j-1]]->inUse = False;
		
	}

	return;

}


/*
 *
 *	Return a argv type array of pointers, pointing to the actual
 *	"private" title strings in each metObject.  The intention
 *	of this function, is to immediately use these strings to
 *	create XmStrings to be displayed in a status window somewhere.
 *
 *	On return, this routine will have filled in cnt and the 
 *	titlev pointer array.  It is up to the caller to free the titlev
 *	pointer array.
 *
 */

void GetMetObjectTitles( PixmapObjectType *pot, int *cnt, char ***titlev)
{

	MetObjectType 		**mv;
		
	int		j;
	int		numMetObjects;
	char		**argv;
	int		n;

	*cnt = 0;
	*titlev = NULL;

	if( ! pot )
		return;

	*cnt = numMetObjects = pot->numMetObjects;
	mv = pot->metObjects;
	
	argv = (char **) malloc( sizeof(char *) * (numMetObjects + 1 ));
	
	for( j = 0; j < numMetObjects; j++, mv++ ) {

		if( (*mv)->inUse )
			argv[j] = (*mv)->titleString;

	}

	*titlev = argv;

	return;

}

/*
 *
 *	Update the titleObject.
 *
 */
void UpdateTitleObject( WindowObjectType *wo )
{

	char		**argv;
	int		cnt;

	argv = NULL;

	GetMetObjectTitles( GetActivePixmapObject( wo ), 
				&cnt, &argv);

	TitleObjectUpdateEntries( wo, cnt, argv );

	if( argv )
		free( argv );

	return;

}



/*
 *
 *	This function was created so that the system could determine
 *	which image color bank is in effect.
 *
 *	Loop through all of the met objects looking at the type
 *	of the first image object found (sat, radar or nowrad).
 *	Based on that, return a gempak color bank number.
 *
 *	Not sure where this routine actually fits.
 *
 *	Later on, this simplistic approach may not be sufficient
 *	for complicated color handling, but it will do for now.
 *
 * 	7/96 - Cowie - added a check for BACKGROUNDOBJECT so that it
 *	will return color bank 1 (satellite). Backgrounds in use
 *	so far are topo maps (disguised sat images).
 *	
 *	
 */
int GetGempakColorBank( WindowObjectType *wo )
{

	PixmapObjectType	**pv;
	MetObjectType 		**mv;
		
	int		i, j;
	int		numPixmapObjects;
	int		numMetObjects;
	int		colorBank;

	numPixmapObjects = wo->numPixmapObjects; 

	pv = wo->pixmapObjects;

	colorBank = -1;
	for( i = 0; colorBank == -1 && i < numPixmapObjects; i++, pv++ ) {

		numMetObjects = (*pv)->numMetObjects;

		mv = (*pv)->metObjects;

		for( j = 0; j < numMetObjects; j++, mv++ ) {

			switch ( (*mv)->type ) {
			    case RADAROBJECT:
			    case NOWRADOBJECT:
				colorBank = 2;
				break;
			    case SATOBJECT:
			    case BACKGROUNDIMAGEOBJECT:
				colorBank = 1;
				break;
			    default:
				break;
			}
			
		}

		if( j < numMetObjects )
			break;

	}

	return( colorBank );

}
