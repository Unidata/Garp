/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	updimage.c
 *
 *	Updating utilities for image data.
 *
 *	History:
 *
 *	 9/97	COMET	Original copy
 *	12/97	COMET	Added a safety check for fewer new items.
 *	01/98	COMET	Set timestamp and filename to MISSING for new
 *			secondary metObjects
 *	02/11	Unidata	&files/lfiles memory deallocation workaround
 *
 ***********************************************************************/


#include "winobj.h"
#include "mainwin.h"
#include "gendef.h"
#include "genglobs.h"
#include "_proto.h"
#include "guimacros.h"

int * MatchIndicesToList ( char **, int, int, int * );


/*
 *	This routine looks to see if new images have arrived after the
 *	image specified in the metobject, and returns the list of new
 *	images and timestamps.
 */

void
GetNewImages (	MetObjectType *m, int maxFrames, int *count,
		char ***newFiles, char ***newTimestamps )
{
	int		nfiles, index;
	int		start, cnt, i;
	int		metObjectType;
	char		*directory;
	char		*fullpath, *timestamp;
	char		**files;
	char		**lfiles;
	AbsTime		fileTime;
	int		dummyOffset;


	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("GetNewImages()\n");


	metObjectType = GetMetObjectType (m);

/*
 *	Get the filelist
 */
	directory = strsavebefore ( m->filename, '/' );
	nfiles = GetImageFileList ( metObjectType, directory, &lfiles, &start, &cnt );
	files = lfiles;
/*
 *	See if any files (up to maxFrames) are newer. To expedite this, start
 *	at the end of the filelist and scan backwards. Break from the loop as
 *	soon as we find a file older or the same time as the reference
 *	metObject.
 */
	index = start + cnt - 1;
 	for ( i = 0; i < maxFrames; i++ ) {

	    if ( index >= 0 ) {
		fullpath = builddirpath (directory, files[index]);
		timestamp = FileToDattim ( metObjectType, fullpath);
		fileTime = String2LongTime ( timestamp, &dummyOffset );

		if ( fileTime <= m->refTime ) {
			Free ( fullpath);
			Free ( timestamp);
			break;
		} else {
			if ( GetVerboseLevel () > VERBOSE_0 ) 
				printf("GetNewImages(): updating loop\n");
		}
	    
		index--;
	    }
	    
	}

/*
 *	Increment the index so that it's pointing to the first new file.
 */
	index++;
	cnt = cnt - index;
	start = index;
	*newFiles = StringListAllocate ( cnt );
	*newTimestamps = StringListAllocate ( cnt );

	*count = 0;
	for ( i = start; i < start + cnt; i++ ) {

	    fullpath = builddirpath ( directory, files[i] );
	    timestamp = FileToDattim ( metObjectType, fullpath);

            (*newFiles)[*count] = (char *) malloc(strlen(fullpath) + 1);
            strcpy((*newFiles)[*count], fullpath );

            (*newTimestamps)[*count] = (char *) malloc(strlen(timestamp) + 1);
            strcpy((*newTimestamps)[*count], timestamp );

	    Free ( fullpath );
	    Free ( timestamp );
	    
	    (*count)++;
	}

/*
 *	free stuff
 */
	StringListFree( nfiles, files );
	Free (directory);


}



/*
 *	Updates Image objects (sat/radar) with new date/time info so
 *	that they can be redrawn to display the new data. Also frees any
 *	image data associated with the metobject to ensure the new image
 *	file is read.
 */

void
AddNewImages (	WindowObjectType *wo, int mindex,
		int count, char **files,
		char **timestamps)
{

	AnyImageObjectType      *iot;
	MetObjectType		*mo;
	PixmapObjectType	*po;
	int			i, findex;


	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("GetNewImages()\n");


	if ( count < 1 ) return;

/*
 *	Re-use as many pixmaps need to be updated.
 */
 	RecyclePixmapObjects ( wo, count );

/*
 *	Loop through the pixmaps to update.
 */
	findex = 0;
	for ( i = wo->numPixmapObjects - count; i < wo->numPixmapObjects; i++ ){

	    if ( GetVerboseLevel () > VERBOSE_0 ) {
		printf("AddNewImages: new reference time: %s\n",
			timestamps[findex]);
	    }

	    po = wo->pixmapObjects[i];

	    mo = po->metObjects[mindex];

	    iot = (AnyImageObjectType *) mo->metObjectContent;

/*
 *	    Set the new filename, time info
 */
	    strcpy (mo->filename, files[findex]);
	    strcpy (mo->timestamp, timestamps[findex]);
	    strcpy (mo->titleString, "");
	    mo->refTime   = String2LongTime( mo->timestamp, &(mo->fcstTime) );
	    mo->validTime = mo->refTime + mo->fcstTime; 
	    
/*
 *	    Free any image data
 */

	    if ( iot->imgCmn.imageData ) {
		Free ( iot->imgCmn.imageData );
		iot->imgCmn.imageData = NULL;
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

	}
    
	return;

}


/*
 *	This function adds new images if a new frame was added (count > 0,
 *	and time matching on), and loops through the other image objects,
 *	seeing if they need to be redrawn.
 */

void
UpdateImageObjects ( 	WindowObjectType *wo,
			int mindex,
			int count, 
			BooleanType *newData )
{
	int			nfiles, start;
	int			i, cnt;
	int			newFrameIndex;
	char			*directory;
	char			*timestamp;
	char			**files;
	char			*fullpath;
	ListParentObjectType	list;
	ListIndexObjectType	match_list;
	AnyImageObjectType	*iot;
        DataTimeObjectType      *dto;
	PixmapObjectType	*po;
	MetObjectType		*mo;


	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("UpdateImageObjects()\n");

	*newData = False;

/*
 *	If the metobject index is 0 (ie, this is the reference metObject),
 *	then just exit, since everything should be up to date.
 */
	if ( mindex == 0 ) return;

/*
 *	Set the index demarking new frames from old ones.
 */
	newFrameIndex = wo->numPixmapObjects - count;

/*
 *	For new frames, turn the image metobjects' inUse flag off. We'll
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
 *	Get a handle on the object
 */
	po = wo->pixmapObjects[0];
	mo = po->metObjects[mindex];

/*
 *	Get the filelist
 */
	directory = strsavebefore ( mo->filename, '/' );
	nfiles = GetImageFileList ( mo->type, directory, &files, &start, &cnt );

/*
 *	Allocate storage for the file path and label arrays
 */
	list.string = StringListAllocate ( cnt );
	list.label  = StringListAllocate ( cnt );

/*
 *	Add the path and label (filename) into the list struct
 */
	for ( i = start; i < start + cnt; i++ ) {

	    fullpath = builddirpath ( directory, files[i] );
            list.string[i] = (char *) malloc(strlen(fullpath) + 1);
            strcpy(list.string[i], fullpath );

	    timestamp = FileToDattim ( mo->type, fullpath);

            list.label[i] = (char *) malloc(strlen(timestamp) + 1);
            strcpy(list.label[i], timestamp );

	    Free ( fullpath );
	    Free ( timestamp );
	}
	
	list.listcount = cnt;

/*
 *	Time match
 */
	match_list.intList = MatchIndicesToList (
                                list.label,
				list.listcount,
				mo->type,
				&(match_list.listcount) );

/*
 *	Loop through all the frames and update as needed.
 */
	for ( i = 0; i < wo->numPixmapObjects; i++) {

	    po = wo->pixmapObjects[i];

	    mo = po->metObjects[mindex];
	    iot = (AnyImageObjectType *)mo->metObjectContent;

/*
 *	    If the list index is valid, update the metObject with new
 *	    info. There are 2 scenarios: 1) new frame, or 2) an old
 *	    frame that had a MISSING time but now matches.
 */
	    if ( match_list.intList[i] != -1 ) {

		if ( (i >= newFrameIndex) ||
		     (i < newFrameIndex &&
		      strncmp (mo->timestamp, "MISSING", 7) == 0 )) {

		    strcpy (mo->filename, list.string[match_list.intList[i]]);
		    strcpy (mo->timestamp, list.label[match_list.intList[i]]);
		    mo->refTime   = String2LongTime( mo->timestamp, &(mo->fcstTime) );
		    mo->validTime = mo->refTime + mo->fcstTime;    
		    strcpy ( mo->titleString, "");

/*
 *		    Free any image data
 */

		    if ( iot->imgCmn.imageData ) {
			Free ( iot->imgCmn.imageData );
			iot->imgCmn.imageData = NULL;
		    }

		    mo->inUse = True;
			    
		    if ( i < newFrameIndex ) {

			ClearPot ( wo, po );
			SetPixmapObjectDrawnFlag ( po, False);
			*newData = True;

		    }

		    if ( GetVerboseLevel () > VERBOSE_0 ) {
			if ( i < newFrameIndex )
			    printf("UpdateImageObjects: adding previously missing time: %s\n",mo->timestamp);
			else
			    printf("UpdateImageObjects: adding matching time: %s\n",mo->timestamp);
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

