/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	updupa.c
 *
 *	Update utilities for upper air data 
 *
 *	History:
 *
 *	09/97	COMET	Original copy
 *	11/97	COMET	Added call to MakeMotTitle.
 *	12/97	COMET	Added a safety check for fewer new items.
 *	01/98	COMET	Fixed bug in title generation for secondary 
 *			upperair metObjects
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

int * MatchIndicesToList ( char **, int, int, int * );
void	countsfcobs ( char *, char *,int *, int, int );



/*
 *	Searches for upperair data (times) newer than what is contained in the
 *	input reference metobject. Returns a list of times and filenames.
 */

void
GetNewUpperAirObs (	MetObjectType *mo, int maxFrames, int *count,
			char ***newFiles, char ***newTimestamps )
{
	int			nfiles, index;
	int			start, cnt;
	int			i, iret;
	int			dummyOffset;
	char			*directory;
	char			*fileFilter;
	char			**files;
	BooleanType		tfilter;
	ListParentObjectType	list;
	AbsTime			time;
	UpperairObjectType	*upa;
	UpaProfileObjectType	*upf;

	
	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("GetNewUpperAirObs()\n");
	
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
 *	        Scrub duplicate entries from list.
 */	
		if ( GetMetObjectType ( mo ) == UPPERAIROBJECT ) {
		    upa = (UpperairObjectType *) mo->metObjectContent;
		    tfilter = upa->tfilter;
		}
		else {
		    upf = (UpaProfileObjectType *) mo->metObjectContent;
		    tfilter = upf->tfilter;
		}

	        if ( tfilter ) ScrubUpaList ( &list );
	        

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
	    
			if ( time <= mo->refTime ) break;
	    
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
 *	Adds new upperair data plots. Recycles old (aged off) frames and
 *	uses the old ones to plot the new data.
 */

void
AddNewUpperAirObs (	WindowObjectType *wo, int mindex, int count,
			char **files, char **timestamps)
{
	MetObjectType		*mo;
	PixmapObjectType	*po;
	UpperairObjectType	*upa;
	UpaProfileObjectType	*upf;
	char			stnid[GEMPAKSTRING];
	char			*vlev = "mb";
	char			*label1="Upperair Obs";
	char			*label2="Profile";
	int			i, findex;
	

	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("AddNewUpperAirObs()\n");

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
		printf("AddNewUpperAirObs: new reference time: %s\n",
			timestamps[findex]);
	    }

	    po = wo->pixmapObjects[i];

	    mo = po->metObjects[mindex];


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
	    if ( GetMetObjectType ( mo ) == UPPERAIROBJECT ) {
		upa = (UpperairObjectType *) mo->metObjectContent;
		strcpy (upa->dattim, mo->timestamp);
		MakeMotTitle ( mo->titleString, upa->dattim, NULL, 
	                        upa->levels, vlev, label1, NULL );
	    }
	    else {
		upf = ( UpaProfileObjectType *) mo->metObjectContent;
		strcpy ( upf->dattim, mo->timestamp);
		strcpy ( stnid, mo->area);
		strndel ( stnid, 1 );

		MakeMotTitle ( mo->titleString, upf->dattim, NULL, 
	                        NULL, NULL, label2, stnid );
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
 *	This function adds data if a new frame was added (count > 0, and
 *	time matching on), and loops through existing upperair objects,
 *	seeing if they need to be redrawn.
 */

void
UpdateUpperAirObjects ( WindowObjectType *wo,
			int mindex,
			int count, 
			BooleanType *newData )
{
	int			nfiles, nstns;
	int			i, iret;
	int			newFrameIndex;
	char			*directory;
	char			*fileFilter;
	char			**files;
	char			stnid[GEMPAKSTRING];
	char			*vlev = "mb";
	char			*label1="Upperair Obs";
	char			*label2="Profile";
	BooleanType		tfilter;
	ListParentObjectType	list;
	ListIndexObjectType	match_list;
	UpperairObjectType	*upa;
	UpaProfileObjectType	*upf;
        DataTimeObjectType      *dto;
	PixmapObjectType	*po;
	MetObjectType		*mo;


	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("UpdateUpperAirObjects()\n");


	*newData = False;
/*
 *	Set the index demarking new frames from old ones.
 */
	newFrameIndex = wo->numPixmapObjects - count;

/*
 *	Look at existing frames and see if any need to be redrawn. This might
 *	be necessary since upperair reports trickle in rather than arriving
 *	all at one time. Compare the number of stations plotted to what is
 *	available for this time. Do this for plan view only.
 */
	for ( i = 0; i < newFrameIndex; i++) {

	    po = wo->pixmapObjects[i];

	    mo = po->metObjects[mindex];

	    if ( GetMetObjectType ( mo ) != UPPERAIROBJECT ) 
	    	break;

	    upa = (UpperairObjectType *)mo->metObjectContent;

	    countupaobs ( mo->filename, mo->timestamp, &nstns,
	    	strlen(mo->filename), strlen(mo->timestamp));

	    if ( mo->inUse && (nstns > upa->nstations)) {

	    	ClearPot ( wo, po );
	    	SetPixmapObjectDrawnFlag ( po, False);
	    	*newData = True;

		if ( GetVerboseLevel () > VERBOSE_0 ) {
		    printf("UpdateUpperAirObjects: redraw %s\n",mo->timestamp);
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
 *	        See if time filtering was turned on. Scrub duplicate entries
 *		from list.
 */
		po = wo->pixmapObjects[0];
		mo = po->metObjects[mindex];

		if ( GetMetObjectType ( mo ) == UPPERAIROBJECT ) {
		    upa = (UpperairObjectType *) mo->metObjectContent;
		    tfilter = upa->tfilter;
		}
		else {
		    upf = (UpaProfileObjectType *) mo->metObjectContent;
		    tfilter = upf->tfilter;
		}
		
	        if ( tfilter) ScrubUpaList ( &list );

/*
 *		Time match
 */
		match_list.intList = MatchIndicesToList (
                                	list.label,
					list.listcount,
					UPPERAIROBJECT,
					&(match_list.listcount) );

/*
 *		Loop through all the frames and update as needed.
 */
		for ( i = 0; i < wo->numPixmapObjects; i++) {

		    po = wo->pixmapObjects[i];

		    mo = po->metObjects[mindex];
		    upa = (UpperairObjectType *)mo->metObjectContent;

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

			    mo->refTime   = String2LongTime( mo->timestamp, &(mo->fcstTime) );
			    mo->validTime = mo->refTime + mo->fcstTime;    

/*
 *	    		    Update the title, and dattim in the object's content
 */
	    		    if ( GetMetObjectType ( mo ) == UPPERAIROBJECT ) {
	    		    	upa = (UpperairObjectType *) mo->metObjectContent;
				strcpy (upa->dattim, mo->timestamp);
				MakeMotTitle ( mo->titleString, upa->dattim, NULL, 
	                        	       upa->levels, vlev, label1, NULL );
			    }
			    else {
				upf = ( UpaProfileObjectType *) mo->metObjectContent;
				strcpy (upf->dattim, mo->timestamp);
				strcpy ( stnid, mo->area);
				strndel ( stnid, 1 );
				MakeMotTitle ( mo->titleString, upf->dattim,
						NULL, NULL, NULL, label2, stnid );
			    }

			    mo->inUse = True;
			    
			    if ( i < newFrameIndex ) {
		
				ClearPot ( wo, po );
				SetPixmapObjectDrawnFlag ( po, False);
				*newData = True;

			    }

			    if ( GetVerboseLevel () > VERBOSE_0 ) {
				if ( i < newFrameIndex)
				    printf("UpdateUpperAirObjects: adding previously missing time: %s\n",mo->timestamp);
				else
				    printf("UpdateUpperAirObjects: adding matching time: %s\n",mo->timestamp);
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
