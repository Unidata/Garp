/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	updsurf.c
 *
 *	Update utilities for surface data
 *
 *	History:
 *
 *	09/97	COMET	Original copy
 *	11/97	COMET	Added call to MakeMotTitle.
 *	12/97	COMET	Added a safety check for fewer new items.
 *	01/98	COMET	Fixed bug in title generation for secondary 
 *			surface metObjects
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
#include "underscore.h"

int	*MatchIndicesToList ( char **, int, int, int * );
void	countsfcobs ( char *, char *,int *, int, int );

/*
 *	Searches for surface data (times) newer than what is contained in the
 *	input reference metobject. Returns a list of times and filenames.
 */

void
GetNewSurfaceObs (	MetObjectType *m, int maxFrames, int *count,
			char ***newFiles, char ***newTimestamps )
{
	int			nfiles, index;
	int			start, cnt;
	int			i, iret;
	int			dummyOffset;
	char			*directory;
	char			plot_dir[FILENAMESTRING];
	char			**files;
	ListParentObjectType	list;
	AbsTime			time;
	SurfaceObjectType	*sot;


	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("GetNewSurfaceObs()\n");


	*count = 0;

/*
 *	Get directory and search key.
 */
	sot = (SurfaceObjectType *)m->metObjectContent;
	sprintf ( plot_dir, "%s%s", sot->type, "_dir" );
	directory = GetConfigValue( plot_dir );

/*
 *	Get a filtered list of files to read.
 */
	if ( GetFilteredDirectoryList 
		( sot->type, directory, &files, &nfiles ) ) {
/*
 *	    Make sure directory exists.
 */
	    if ( nfiles > 0 ) {

/*
 *	        Get list of date/time stamps and associated files.
 */
	        list.listcount = GetSfcList ( directory, files, nfiles,
		                       &list, &iret );

/*
 *	        Scrub duplicate entries from list.
 */
	        ScrubSfcObsList ( &list );

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
	StringListFree( nfiles, files );
	StringListFree( list.listcount, list.string );
	StringListFree( list.listcount, list.label );

}


/*
 *	Adds new surface data plots. Recycles old (aged off) frames and
 *	uses the old ones to plot the new data.
 */

void
AddNewSurfaceObs (	WindowObjectType *wo, int mindex, int count,
			char **files, char **timestamps)
{
	MetObjectType		*mo;
	PixmapObjectType	*po;
	SurfaceObjectType	*sot;
	int			i, findex;
	

	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("AddNewSurfaceObs()\n");

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
		printf("AddNewSurfaceObs: new reference time: %s\n",
			timestamps[findex]);
	    }

	    po = wo->pixmapObjects[i];

	    mo = po->metObjects[mindex];

	    sot = (SurfaceObjectType *)mo->metObjectContent;	    
/*
 *	    Set the new filename, time info
 */
	    strcpy (mo->filename, files[findex]);
	    strcpy (mo->timestamp, timestamps[findex]);
	    strcpy (sot->dattim, mo->timestamp);

	    mo->refTime   = String2LongTime( mo->timestamp, &(mo->fcstTime) );
	    mo->validTime = mo->refTime + mo->fcstTime;    
		
/*
 *	    Update the title
 */
	    MakeMotTitle ( mo->titleString, mo->timestamp, NULL, NULL, NULL,
	                   sot->label, NULL );

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
	}
    
	return;

}


/*
 *	This function adds new surface data if a new frame was added 
 *	(count > 0, and time matching on), and loops through the exising
 *	surface objects, seeing if they need to be redrawn.
 */

void
UpdateSurfaceObjects ( WindowObjectType *wo,
			    int mindex,
			    int count, 
			    BooleanType *newData )
{
	int			nfiles;
	int			i, iret;
	int			newFrameIndex;
	int			nstns;
	char			*directory;
	char			plot_dir[FILENAMESTRING];
	char			**files;
	ListParentObjectType	list;
	ListIndexObjectType	match_list;
	SurfaceObjectType	*sot;
        DataTimeObjectType      *dto;
	PixmapObjectType	*po;
	MetObjectType		*mo;


	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("UpdateSurfaceObjects()\n");


	*newData = False;
/*
 *	Set the index demarking new frames from old ones.
 */
	newFrameIndex = wo->numPixmapObjects - count;

/*
 *	Look at existing frames and see if any need to be redrawn. This might
 *	be necessary since surface data trickles in rather than arriving
 *	all at one time. Compare the number of reports plotted before and
 *	how many are currently available.
 */
	for ( i = 0; i < newFrameIndex; i++) {

	    po = wo->pixmapObjects[i];

	    mo = po->metObjects[mindex];

	    sot = (SurfaceObjectType *)mo->metObjectContent;

	    countsfcobs ( mo->filename, mo->timestamp, &nstns,
	    	strlen(mo->filename), strlen(mo->timestamp));

	    if ( mo->inUse && (nstns > sot->nstations)) {

	    	ClearPot ( wo, po );
	    	SetPixmapObjectDrawnFlag ( po, False);
	    	*newData = True;

		if ( GetVerboseLevel () > VERBOSE_0 ) {
		    printf("UpdateSurfaceObjects: redraw %s\n",mo->timestamp);
		}

	    }
	
	}

/*
 *	If the metObjectIndex is 0 (ie, it is the reference metObject),
 *	exit, since everything should now be up to date.
 */
	if ( mindex == 0 ) return;

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
 *	Grab a surface object type content handle
 */
	po = wo->pixmapObjects[0];
	mo = po->metObjects[mindex];
	sot = (SurfaceObjectType *)mo->metObjectContent;

/*
 *	Get directory and search key.
 */
	sprintf ( plot_dir, "%s%s", sot->type, "_dir" );
	directory = GetConfigValue( plot_dir );

/*
 *	Get a filtered list of files to read.
 */
	if ( GetFilteredDirectoryList 
		( sot->type, directory, &files, &nfiles ) ) {
/*
 *	    Make sure directory exists.
 */
	    if ( nfiles > 0 ) {

/*
 *	        Get list of date/time stamps and associated files.
 */
	        list.listcount = GetSfcList ( directory, files, nfiles,
		                       &list, &iret );

/*
 *	        Scrub duplicate entries from list.
 */
	        ScrubSfcObsList ( &list );

/*
 *		Time match
 */
		match_list.intList = MatchIndicesToList (
                                	list.label,
					list.listcount,
					SURFACEOBJECT,
					&(match_list.listcount) );

/*
 *		Loop through all the frames and update as needed.
 */
		for ( i = 0; i < wo->numPixmapObjects; i++) {

		    po = wo->pixmapObjects[i];

		    mo = po->metObjects[mindex];
		    sot = (SurfaceObjectType *)mo->metObjectContent;

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
			    strcpy (sot->dattim, list.label[match_list.intList[i]]);

			    mo->refTime   = String2LongTime( mo->timestamp, &(mo->fcstTime) );
			    mo->validTime = mo->refTime + mo->fcstTime;    
		
/*
 *			    Update the title
 */
			    MakeMotTitle ( mo->titleString, mo->timestamp,
			    		   NULL, NULL, NULL, sot->label, NULL);

			    mo->inUse = True;
			    
			    if ( i < newFrameIndex ) {

				ClearPot ( wo, po );
				SetPixmapObjectDrawnFlag ( po, False);
				*newData = True;

			    }

			    if ( GetVerboseLevel () > VERBOSE_0 ) {
				if ( i < newFrameIndex )
				    printf("UpdateSurfaceObjects: adding previously missing time: %s\n",mo->timestamp);
				else
				    printf("UpdateSurfaceObjects: adding matching time: %s\n",mo->timestamp);
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
 	Free ( match_list.intList );
	StringListFree( nfiles, files );
	StringListFree( list.listcount, list.string );
	StringListFree( list.listcount, list.label );


}
