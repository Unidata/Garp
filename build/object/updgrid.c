/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	updsurf.c
 *
 *	Update utilities for plan, xsection and profile gridded data
 *	displays.
 *
 *	History:
 *
 *	09/97	COMET	Original copy
 *	11/97	COMET	Added calls to MakeMotTitle.
 *	12/97	COMET	Added a safety check for fewer new items.
 *	01/98	COMET	Fixed bug in updating secondary Time-height objects
 *	01/98	COMET	Set timestamp and filename to MISSING for new
 *			secondary metObjects. Added special check to prevent
 *			updates that would combine different model runs.
 *	 2/98	COMET	Added gvcord to MakeMotTitile for plan view grid
 *	 3/98	COMET	Changes to title drawing for plan view grids.
 *
 ***********************************************************************/


#include <X11/X.h>

#include "winobj.h"
#include "mainwin.h"
#include "gendef.h"
#include "genglobs.h"
#include "_proto.h"
#include "guimacros.h"
#include "underscore.h"

int * MatchIndicesToList ( char **, int, int, int * );
char * MakeForecastDattim ( char * );


/*
 *	The static variables and "RefreshedModelList" functions below are
 *	used to save the names of models that have had their file/time
 *	lists refreshed in the current auto-update cycle. Consulting this
 *	list prevents redundant querying of model time lists.
 */

static int	msize = 0;
static char	**mlist;
static int	MAX_MODELS = 50;


void
FlushRefreshedModelList ( )
{

	StringListFree ( msize, mlist );
	msize = 0;

}

void
AddToRefreshedModelList ( char *model )
{

	if ( msize == 0 )
	    mlist = StringListAllocate ( MAX_MODELS );

	mlist[msize] = (char *) malloc(strlen(model) + 1);
	strcpy ( mlist[msize], model );
	msize++;

}

BooleanType
InRefreshedModelList ( char *model )
{
	int	i;

	for ( i = 0; i < msize; i++ ) {
	    if ( EQSTRING ( mlist[i], model ) )
	    	return (True);
	}
	
	return ( False);	    	

}



/*
 *	Searches for gridded data (times) newer than what is contained in the
 *	input reference metobject. Returns a list of times and filenames.
 */

void
GetNewGrids (	MetObjectType *mo, int maxFrames, int *count,
			char ***newFiles, char ***newTimestamps )
{
	int			nfiles, index;
	int			start, cnt;
	int			i, metObjectType;
	int			offset;
	char			*directory;
	char			*model;
	char			**files;
	BooleanType		newCycle;
	ListParentObjectType	list;
	AbsTime			time;
	ScalarGridObjectType    *sot;
	VectorGridObjectType    *vot;
	VertProfileObjectType	*vpt;
	XSectionGridObjectType	*xot;
	TimeHeightObjectType	*tot;

	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("GetNewGrids()\n");


	*count = 0;

/*
 *	Get model name string
 */
	metObjectType = GetMetObjectType ( mo ) ;
	if ( metObjectType == VECTORGRIDOBJECT ) {
	    vot = (VectorGridObjectType *)mo->metObjectContent;
	    model = strdup ( vot->model_name );
	}
	else if ( metObjectType == SCALARGRIDOBJECT ||
		  metObjectType == SCALARGRIDFILLOBJECT ) {
	    sot = (ScalarGridObjectType *)mo->metObjectContent;
	    model = strdup ( sot->model_name );
	}
	else if ( metObjectType == XSECTIONGRIDOBJECT ||
		  metObjectType == XSECTIONGRIDFILLOBJECT ) {
	    xot = (XSectionGridObjectType *)mo->metObjectContent;
	    model = strdup ( xot->model_name );
	}
	else if ( metObjectType ==  VERTICALPROFILEOBJECT) {
	    vpt = (VertProfileObjectType *)mo->metObjectContent;
	    model = strdup ( vpt->model_name );
	}
	else if ( metObjectType ==  TIMEHEIGHTOBJECT) {
	    tot = (TimeHeightObjectType *)mo->metObjectContent;
	    model = strdup ( tot->model_name );
	}

	directory = GetConfigValue( "grids" );

/*
 *	Flush the model name list (local to autoupdate)
 */
	FlushRefreshedModelList ();

/*
 *	Flush the cache for this model type so that a fresh list will
 *	be generated.
 */
	RemoveModelListCacheEntry ( model );

/*
 *	Get filtered directory list.
 */
	if ( GetFilteredDirectoryList ( model, directory, &files,
	                                           &nfiles ) > 0 ) {
/*
 *	    Build labels for scrolled list.  
 */
	    GetModelLabelsFromList ( model, directory, files,
	                             nfiles, &list );

/*
 *	    Filter forecast times, if applicable.
 */
	    FilterForecastTimes ( &list );

/*
 *	    Save the model name to a list of recently refreshed models
 */
	    AddToRefreshedModelList ( model );

/*
 *	    See if any times (up to maxFrames) are newer. Start at the end
 *	    of the list and scan backwards. Look for new model runs. Look for
 *	    a forecast time greater than or equal to the last forecast time
 *	    in the current loop. This will ensure that only new and complete
 *	    model cycles are updated.
 */
	    index = list.listcount - 1;
	    newCycle = False;
 	    for ( i = 0; i < maxFrames; i++ ) {

		if ( index >= 0 ) {
		
		    time = String2LongTime ( list.label[index],
		    				&offset );
	    
		    if ( time <= mo->refTime ) break;

		    if ( offset >= mo->fcstTime ) newCycle = True;
		    
		    if ( !newCycle ) break;

		    index--;
	    	}
	    	
	    }

/*
 *	    Increment the index so that it's pointing to the first new time.
 */
	    index++;
	    cnt = list.listcount - index;
	    start = index;

/*
 *	    Allocate space for the list of new times, and save them.
 */
	    *newFiles = StringListAllocate ( cnt );
	    *newTimestamps = StringListAllocate ( cnt );

	    *count = 0;
	    for ( i = start; i < start + cnt; i++ ) {

	        (*newFiles)[*count] = (char *) malloc(strlen(list.string[i]) + 1);
	        strcpy((*newFiles)[*count], list.string[i]);

	        (*newTimestamps)[*count] = (char *) malloc(strlen(list.label[i]) + 1);
	        strcpy((*newTimestamps)[*count], list.label[i] );
	    
		(*count)++;
	    }
	}
/*
 *	free stuff
 */
 	Free ( directory );
 	Free ( model );
	StringListFree( nfiles, files );
	StringListFree( list.listcount, list.string );
	StringListFree( list.listcount, list.label );

}


/*
 *	Adds new gridded data plots. Recycles old (aged off) frames and
 *	uses the old ones to plot the new data.
 */

void
AddNewGrids (	WindowObjectType *wo, int mindex, int count,
		char **files, char **timestamps)
{
	MetObjectType		*mo;
	PixmapObjectType	*po;
	ScalarGridObjectType    *sot;
	VectorGridObjectType    *vot;
	VertProfileObjectType	*vpt;
	XSectionGridObjectType	*xot;
	TimeHeightObjectType	*tot;
	int			i, findex;
	int			metObjectType;
	char			*dattim;
	char			lv[GEMPAKSTRING], vc[GEMPAKSTRING];
	

	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("AddNewGrids()\n");

	if ( count < 1 ) return;

/*
 *	Re-use as many pixmaps need to be updated.
 */
 	RecyclePixmapObjects ( wo, count );

/*
 *	Loop through the pixmaps to update
 */
	findex = 0;
	for ( i = wo->numPixmapObjects - count; i < wo->numPixmapObjects; i++  ) {

	    if ( GetVerboseLevel () > VERBOSE_0 ) {
		printf("AddNewGrids: new reference time: %s\n",
			timestamps[findex]);
	    }

	    po = wo->pixmapObjects[i];

	    mo = po->metObjects[mindex];

/*
 *	    Set the new filename, time info in the metObject
 */
	    strcpy (mo->filename, files[findex]);
	    strcpy (mo->timestamp, timestamps[findex]);

	    mo->refTime   = String2LongTime( mo->timestamp, &(mo->fcstTime) );
	    mo->validTime = mo->refTime + mo->fcstTime;    
	    dattim = MakeForecastDattim ( mo->timestamp );

/*
 *	    Set the object-specific items (date/time), title
 */
	    metObjectType = GetMetObjectType ( mo );

	    if ( metObjectType == VECTORGRIDOBJECT ) {
		vot = (VectorGridObjectType *)mo->metObjectContent;	    
		strcpy (vot->gdatim, mo->timestamp);
		strcpy ( lv, vot->glevel );
		strcpy ( vc, vot->gvcord );
		parse_gfunc ( vot->gvect, lv, vc, strlen (vot->gvect),
				sizeof (lv), sizeof (vc) );
		MakeMotTitle ( mo->titleString, dattim, vot->model_label,
                               lv, vc, vot->field_label, NULL );
 	    }

	    else if ( metObjectType == SCALARGRIDOBJECT ||
		   metObjectType == SCALARGRIDFILLOBJECT ) {	    
		sot = (ScalarGridObjectType *)mo->metObjectContent;	    
		strcpy (sot->gdatim, mo->timestamp);
		strcpy ( lv, sot->glevel );
		strcpy ( vc, sot->gvcord );
		parse_gfunc ( sot->gfunc, lv, vc, strlen (sot->gfunc),
				sizeof (lv), sizeof (vc) );
		MakeMotTitle ( mo->titleString, dattim, sot->model_label,
                               lv, vc, sot->field_label, NULL );
	    }

	    else if ( metObjectType == XSECTIONGRIDOBJECT ||
		      metObjectType == XSECTIONGRIDFILLOBJECT ) {
		xot = (XSectionGridObjectType *)mo->metObjectContent;
		strcpy (xot->gdatim, mo->timestamp);
		MakeMotTitle ( mo->titleString, dattim, xot->model_label,
                               NULL, NULL, xot->field_label, NULL );
	    }

	    else if ( metObjectType ==  VERTICALPROFILEOBJECT) {
		vpt = (VertProfileObjectType *)mo->metObjectContent;
		strcpy (vpt->ftime, mo->timestamp);
		MakeMotTitle ( mo->titleString, dattim, vpt->model_label,
                               NULL, NULL, vpt->field_label, vpt->point );
	    }

	    else if ( metObjectType ==  TIMEHEIGHTOBJECT) {
		tot = (TimeHeightObjectType *)mo->metObjectContent;
		MakeMotTitle ( mo->titleString, NULL, tot->model_label,
                               NULL, NULL, tot->field_label, tot->point );
	    }

/*
 *	    Update the time matching object
 */
	    UpdateDataTimeObject ( wo, mo->refTime, mo->fcstTime,
			 	   mo->validTime );

/*
 *	    Clear the old pixmap. Set all the metobjects to be redrawn.
 */
	    ClearPot ( wo, po );
	    SetPixmapObjectDrawnFlag ( po, False);

	    findex++;
	    Free ( dattim );
	}
    
	return;

}


/*
 *	This function adds new data if a new frame was added (count > 0,
 *	and time matching), and reviews existing metObjects to see if they
 *	need to be redrawn.
 */

void
UpdateGridObjects ( WindowObjectType *wo,
			    int mindex,
			    int count, 
			    BooleanType *newData )
{
	int			nfiles;
	int			i, metObjectType;
	int			newFrameIndex;
	char			*directory, *model, *dattim;
	char			**files;
	ListParentObjectType	list;
	ListIndexObjectType	match_list;
	ScalarGridObjectType    *sot;
	VectorGridObjectType    *vot;
	VertProfileObjectType	*vpt;
	XSectionGridObjectType	*xot;
	TimeHeightObjectType	*tot;
        DataTimeObjectType      *dto;
	PixmapObjectType	*po;
	MetObjectType		*mo;
	char			lv[GEMPAKSTRING], vc[GEMPAKSTRING];


	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("UpdateGridObjects()\n");


	*newData = False;

/*
 *	Exit if this is the reference metObject, since everything should
 *	be up to date.
 */
	if ( mindex == 0 ) return;

/*
 *	Set the index demarking new frames from old ones.
 */
	newFrameIndex = wo->numPixmapObjects - count;

/*
 *	For new frames, turn the metobjects' inUse flag off. We'll
 *	turn them back on later if we find a timematch for each frame.
 *	Set some things to missing.
 */
	for ( i = newFrameIndex; i < wo->numPixmapObjects; i++) {

	    po = wo->pixmapObjects[i];

	    mo = po->metObjects[mindex];

	    mo->inUse = False;
	    strcpy ( mo->timestamp, "MISSING" );
	    strcpy ( mo->filename, "MISSING" );    
	}

/*
 *	If we're not time matching, our work is done.
 */
        dto = GetDataTimeObject();
	if ( dto->numTimes == 0 || dto->matchType == NOTIMEMATCH )
		return;

/*
 *	Grab an object type content handle
 */
	po = wo->pixmapObjects[0];
	mo = po->metObjects[mindex];

/*
 *	Get model name string
 */
	metObjectType = GetMetObjectType ( mo );
	if ( metObjectType == VECTORGRIDOBJECT ) {
	    vot = (VectorGridObjectType *)mo->metObjectContent;
	    model = strdup ( vot->model_name );
	}
	else if ( metObjectType == SCALARGRIDOBJECT ||
		  metObjectType == SCALARGRIDFILLOBJECT ) {
	    sot = (ScalarGridObjectType *)mo->metObjectContent;
	    model = strdup ( sot->model_name );
	}
	else if ( metObjectType == XSECTIONGRIDOBJECT ||
		  metObjectType == XSECTIONGRIDFILLOBJECT ) {
	    xot = (XSectionGridObjectType *)mo->metObjectContent;
	    model = strdup ( xot->model_name );
	}
	else if ( metObjectType ==  VERTICALPROFILEOBJECT) {
	    vpt = (VertProfileObjectType *)mo->metObjectContent;
	    model = strdup ( vpt->model_name );
	}
	else if ( metObjectType ==  TIMEHEIGHTOBJECT) {
	    tot = (TimeHeightObjectType *)mo->metObjectContent;
	    model = strdup ( tot->model_name );
	}

	directory = GetConfigValue( "grids" );

/*
 *	Check to see if a fresh listing has been generated for this model
 *	in this autoupdate cycle. If not, flush the cache for this model
 *	type so that a fresh list will be generated.
 */
	if ( ! InRefreshedModelList ( model ) )
		RemoveModelListCacheEntry (model );

/*
 *	Get filtered directory list.
 */
	if ( GetFilteredDirectoryList ( model, directory, &files,
	                                           &nfiles ) > 0 ) {
/*
 *	    Build labels for scrolled list.  
 */
	    GetModelLabelsFromList ( model, directory, files,
	                             nfiles, &list );

/*
 *	    Filter forecast times, if applicable.
 */
	    FilterForecastTimes ( &list );

/*
 *	    Time match
 */
	    match_list.intList = MatchIndicesToList (
                                	list.label,
					list.listcount,
					metObjectType,
					&(match_list.listcount) );

/*
 *	    Loop through all the frames and update as needed.
 */
	    for ( i = 0; i < wo->numPixmapObjects; i++) {

		po = wo->pixmapObjects[i];

		mo = po->metObjects[mindex];

/*
 *	    	If the list index is valid, update the metObject with new
 *	    	info. There are 2 scenarios: 1) new frame, or 2) an old
 *		frame that had a MISSING time but now matches.
 */
	    	if ( match_list.intList[i] != -1 ) {

		    if ( (i >= newFrameIndex) ||
			 (i < newFrameIndex &&
			  strncmp (mo->timestamp, "MISSING", 7) == 0 )) {

			strcpy (mo->filename, list.string[match_list.intList[i]]);
			strcpy (mo->timestamp, list.label[match_list.intList[i]]);

			mo->refTime   = String2LongTime( mo->timestamp, &(mo->fcstTime) );
			mo->validTime = mo->refTime + mo->fcstTime;    
	    	 	dattim = MakeForecastDattim ( mo->timestamp );
		
/*
 *			Set the object-specific items (date/time), title
 */
			if ( metObjectType == VECTORGRIDOBJECT ) {
			    vot = (VectorGridObjectType *)mo->metObjectContent;	    
			    strcpy (vot->gdatim, mo->timestamp);
			    strcpy ( lv, vot->glevel );
			    strcpy ( vc, vot->gvcord );
			    parse_gfunc ( vot->gvect, lv, vc, strlen (vot->gvect),
					 sizeof (lv), sizeof (vc) );
			    MakeMotTitle ( mo->titleString, dattim,
	                                   vot->model_label, lv,
                                           vc, vot->field_label, NULL );
 			}

			else if ( metObjectType == SCALARGRIDOBJECT ||
			       metObjectType == SCALARGRIDFILLOBJECT ){	    
			    sot = (ScalarGridObjectType *)mo->metObjectContent;	    
			    strcpy (sot->gdatim, mo->timestamp);
			    strcpy ( lv, sot->glevel );
			    strcpy ( vc, sot->gvcord );
			    parse_gfunc ( sot->gfunc, lv, vc, strlen (sot->gfunc),
					  sizeof (lv), sizeof (vc) );
			    MakeMotTitle ( mo->titleString, dattim,
	                                   sot->model_label, lv,
                                           vc, sot->field_label, NULL );
			}

			else if ( metObjectType == XSECTIONGRIDOBJECT ||
		      		  metObjectType == XSECTIONGRIDFILLOBJECT ) {
			    xot = (XSectionGridObjectType *)mo->metObjectContent;
			    strcpy (xot->gdatim, mo->timestamp);
			    MakeMotTitle ( mo->titleString, dattim,
	                                   xot->model_label, NULL, NULL,
                                           xot->field_label, NULL );
			}

			else if ( metObjectType == VERTICALPROFILEOBJECT) {
			    vpt = (VertProfileObjectType *)mo->metObjectContent;
			    strcpy (vpt->ftime, mo->timestamp);
			    MakeMotTitle ( mo->titleString, dattim,
	                                   vpt->model_label, NULL, NULL,
                                           vpt->field_label, vpt->point );
			}

			else if ( metObjectType == TIMEHEIGHTOBJECT) {
			    tot = (TimeHeightObjectType *)mo->metObjectContent;
			    MakeMotTitle ( mo->titleString, NULL,
			    		   tot->model_label, NULL, NULL,
			    		   tot->field_label, tot->point );
			}

			mo->inUse = True;
			    
			if ( i < newFrameIndex ) {

			    ClearPot ( wo, po );
			    SetPixmapObjectDrawnFlag ( po, False);
			    *newData = True;

			}

			if ( GetVerboseLevel () > VERBOSE_0 ) {
			    if ( i < newFrameIndex )
				printf("UpdateGridObjects: adding previously missing time: %s\n",mo->timestamp);
			    else
				printf("UpdateGridObjects: adding matching time: %s\n",mo->timestamp);
			}
 			Free ( dattim );
		    }
		}
	    }
	}

/*
 *	free stuff
 */
 	Free ( directory );
 	Free ( model );
 	Free ( match_list.intList );
	StringListFree( nfiles, files );
	StringListFree( list.listcount, list.string );
	StringListFree( list.listcount, list.label );


}
