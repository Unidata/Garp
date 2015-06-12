/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	updupa.c
 *
 *	Update utilities for upper air profile data
 *
 *	History:
 *
 *	09/97	COMET	Original copy
 *	11/97	COMET	Added call to MakeMotTitle.
 *	12/97	COMET	Added a safety check for fewer new items.
 *	01/98	COMET	Fix bug in title for secondary upper air objects
 *	01/98	COMET	Set timestamp and filename to MISSING for new
 *			secondary metObjects
 *
 ***********************************************************************/


#include <X11/X.h>

#include "winobj.h"
#include "mainwin.h"
#include "gendef.h"
#include "genglobs.h"
#include "_proto.h"
#include "guimacros.h"

int * MatchIndicesToList ( char **, int, int, int * );



/*
 *	Searches for upperair data (times) newer than what is contained in the
 *	input reference metobject. Returns a list of times and filenames.
 */

void
GetNewUpperAirProfObs ( MetObjectType *m, int maxFrames, int *count,
			char ***newFiles, char ***newTimestamps )
{
	int			nfiles, index;
	int			start, cnt;
	int			i, iret;
	int			dummyOffset;
	char			*directory;
	char			*fileFilter;
	char			**files;
	ListParentObjectType	list;
	AbsTime			time;
	UpaProfileObjectType	*upf;


	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("GetNewUpperProfAirObs()\n");

	*count = 0;

/*
 *	Get directory and filename filter.
 */
	directory  = GetConfigValue( "upperair" );
	fileFilter = GetConfigValue( "upperairK" );

/*
 *	Get a filtered list of files to read.
 */
	if ( GetFilteredDirectoryList 
		( fileFilter, directory, &files, &nfiles ) ) {
/*
 *	    Make sure directory exists.
 */
	    if ( nfiles > 0 ) {

/*
 *	        Get list of date/time stamps and associated files.
 */
	        list.listcount = GetUpaList ( directory, files, nfiles,
		                       &list, &iret );

/*
 *	        Scrub duplicate entries from list 
 */
		upf = ( UpaProfileObjectType *) m->metObjectContent;
	        if ( upf->tfilter) ScrubUpaList ( &list );
	        
/*
 *		See if any times (up to maxFrames) are newer. Start
 *		at the end of the list and scan backwards. Break as
 *		soon as we find one older or the same time as the reference
 *		metObject.
 */
		index = list.listcount - 1;

 		for ( i = 0; i < maxFrames; i++ ) {

		    if ( index >= 0 ) {

			time = String2LongTime ( list.label[index],
		    				 &dummyOffset );
	    
			if ( time <= m->refTime ) break;
	    
			index--;

		    }
		}

/*
 *		Increment the index so that it's pointing to the first new file.
 */
		index++;
		cnt = list.listcount - index;
		start = index;

/*
 *		Allocate space for the list of new times, and save them.
 */
		*newFiles = StringListAllocate ( cnt );
		*newTimestamps = StringListAllocate ( cnt );

		*count = 0;
		for ( i = start; i < start + cnt; i++ ) {

	            (*newFiles)[*count] =
	            	(char *) malloc(strlen(list.string[i]) + 1);
	            strcpy((*newFiles)[*count], list.string[i]);

	            (*newTimestamps)[*count] =
	            	(char *) malloc(strlen(list.label[i]) + 1);
	            strcpy((*newTimestamps)[*count], list.label[i] );
	    
		    (*count)++;
		}
	    }
	}
/*
 *	free stuff
 */
 	Free ( directory);
 	Free ( fileFilter);
	StringListFree( nfiles, files );
	StringListFree( list.listcount, list.string );
	StringListFree( list.listcount, list.label );

}


/*
 *	Adds new upperair profile data plots. Recycles old (aged off) frames
 *	and uses the old ones to plot the new data.
 */

void
AddNewUpperAirProfObs (	WindowObjectType *wo, int mindex, int count,
			char **files, char **timestamps)
{
	MetObjectType		*mo;
	PixmapObjectType	*po;
	UpaProfileObjectType	*upf;
	char			stnid[GEMPAKSTRING];
	char			*title="Profile";
	int			i, findex;


	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("AddNewUpperAirProfObs()\n");
	

	if ( count < 1 ) return;

/*
 *	Re-use as many pixmaps need to be updated.
 */
 	RecyclePixmapObjects ( wo, count );

/*
 *	Loop through the pixmaps to update
 */
	findex = 0;
	for ( i = wo->numPixmapObjects - count; i < wo->numPixmapObjects; i++ ) {

	    if ( GetVerboseLevel () > VERBOSE_0 ) {
		printf("AddNewUpperAirProfObs: new reference time: %s\n",
			timestamps[findex]);
	    }

	    po = wo->pixmapObjects[i];

	    mo = po->metObjects[mindex];

	    upf = ( UpaProfileObjectType *) mo->metObjectContent;

/*
 *	    Set the new filename, time info
 */
	    strcpy (mo->filename, files[findex]);
	    strcpy (mo->timestamp, timestamps[findex]);

	    mo->refTime   = String2LongTime( mo->timestamp, &(mo->fcstTime) );
	    mo->validTime = mo->refTime + mo->fcstTime;    

/*
 *	    Update the title, and dattim in the object's content
 */
	    strcpy ( upf->dattim, mo->timestamp);
	    strcpy ( stnid, mo->area);
	    strndel ( stnid, 1 );
	    MakeMotTitle ( mo->titleString, upf->dattim, NULL, NULL, NULL,
                           title, mo->area );
/*
	    sprintf( mo->titleString, "%s Profile (%s)",
		    			upf->dattim, stnid );
*/

/*
 *	    Update the time matching object
 */
	    UpdateDataTimeObject ( 	wo, mo->refTime, mo->fcstTime,
					mo->validTime );

/*
 *	    Clear the old pixmap. Set all the metobjects to be redrawn.
 */
	    ClearPot ( wo, po );
	    SetPixmapObjectDrawnFlag ( po, False);

	    findex++;
	}
    
	return;

}


/*
 *	Updates upperair profiler data in secondary metobjects. (Metobjects
 *	that are not the reference metObject.) This function adds new data if
 *	a new frame was added (count > 0, and time matching on), and loops
 *	through the other upperair objects, seeing if they need to be redrawn
 *	for any reason.
 */

void
UpdateUpperAirProfObjects ( WindowObjectType *wo,
				int mindex,
				int count, 
				BooleanType *newData )
{
	int			nfiles;
	int			i, iret;
	int			newFrameIndex;
	char			*directory;
	char			*fileFilter;
	char			**files;
	char			stnid[GEMPAKSTRING];
	char			*title="Profile";
	ListParentObjectType	list;
	ListIndexObjectType	match_list;
	UpaProfileObjectType	*upf;
        DataTimeObjectType      *dto;
	PixmapObjectType	*po;
	MetObjectType		*mo;


	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("UpdateUpperAirProfObjects()\n");


	*newData = False;
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
 *	Look at existing frames and see if any need to be redrawn. This might
 *	be necessary since upperair reports trickle in rather than arriving
 *	all at one time. The rule right now is to re-plot anything less than an
 *	3 hours old.
 */
	for ( i = 0; i < newFrameIndex; i++) {

	    po = wo->pixmapObjects[i];

	    mo = po->metObjects[mindex];

	    if ( mo->inUse && ((AbsTime)present_time() - mo->refTime) < 10800 ) {

	    	ClearPot ( wo, po );
	    	SetPixmapObjectDrawnFlag ( po, False);
	    	*newData = True;

		if ( GetVerboseLevel () > VERBOSE_0 ) {
		    printf("UpdateUpperAirProfObjects: redraw %s\n",mo->timestamp);
		}

	    }
	
	}

/*
 *	If we're not time matching, our work is done.
 */
        dto = GetDataTimeObject();
	if ( dto->numTimes == 0 || dto->matchType == NOTIMEMATCH )
		return;

/*
 *	Grab an upperairprof object type content handle
 */
	po = wo->pixmapObjects[0];
	mo = po->metObjects[mindex];
	upf = (UpaProfileObjectType *)mo->metObjectContent;

/*
 *	Get directory and filename filter.
 */
	directory  = GetConfigValue( "upperair" );
	fileFilter = GetConfigValue( "upperairK" );

/*
 *	Get a filtered list of files to read.
 */
	if ( GetFilteredDirectoryList 
		( fileFilter, directory, &files, &nfiles ) ) {
/*
 *	    Make sure directory exists.
 */
	    if ( nfiles > 0 ) {

/*
 *	        Get list of date/time stamps and associated files.
 */
	        list.listcount = GetUpaList ( directory, files, nfiles,
		                       &list, &iret );

/*
 *	        Scrub duplicate entries from list.
 */
	        if ( upf->tfilter) ScrubUpaList ( &list );

/*
 *		Time match
 */
		match_list.intList = MatchIndicesToList (
                                	list.label,
					list.listcount,
					UPPERAIRPROFILEOBJECT,
					&(match_list.listcount) );

/*
 *		Loop through all the frames and update as needed.
 */
		for ( i = 0; i < wo->numPixmapObjects; i++) {

		    po = wo->pixmapObjects[i];

		    mo = po->metObjects[mindex];
		    upf = (UpaProfileObjectType *)mo->metObjectContent;

/*
 *	    	    If the list index is valid, update the metObject with new
 *	    	    info. There are 2 scenarios: 1) new frame, or 2) an old
 *		    frame that had a MISSING time but now matches.
 */
	    	    if ( match_list.intList[i] != -1 ) {

			if ( (i >= newFrameIndex) ||
			     (i < newFrameIndex &&
			      strncmp (mo->timestamp, "MISSING", 7) == 0 )) {

			    strcpy (mo->filename, list.string[match_list.intList[i]]);
			    strcpy (mo->timestamp, list.label[match_list.intList[i]]);
			    strcpy (upf->dattim, list.label[match_list.intList[i]]);

			    mo->refTime   = String2LongTime( mo->timestamp, &(mo->fcstTime) );
			    mo->validTime = mo->refTime + mo->fcstTime;    

			    strcpy ( stnid, mo->area);
			    strndel ( stnid, 1 );
			    MakeMotTitle ( mo->titleString, upf->dattim, NULL,
			    		    NULL, NULL, title, mo->area );

			    mo->inUse = True;

			    if ( i < newFrameIndex ) {
		
				ClearPot ( wo, po );
				SetPixmapObjectDrawnFlag ( po, False);
				*newData = True;

			    }

			    if ( GetVerboseLevel () > VERBOSE_0 ) {
				if (i < newFrameIndex)
				    printf("UpdateUpperAirProfObjects: adding previously missing time: %s\n",mo->timestamp);
				else 
				    printf("UpdateUpperAirProfObjects: adding matching time: %s\n",mo->timestamp);
			    }
			}
		    }
		}
	    }
	}

/*
 *	free stuff
 */
 	Free ( directory);
 	Free ( fileFilter);
 	Free ( match_list.intList );
	StringListFree( nfiles, files );
	StringListFree( list.listcount, list.string );
	StringListFree( list.listcount, list.label );


}
