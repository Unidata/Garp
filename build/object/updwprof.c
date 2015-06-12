/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	updwprof.c
 *
 *	Update utilities for wind profiler data
 *
 *	History:
 *
 *	 9/97	COMET	Original copy
 *	 1/98	COMET	Initialized **snfiles to NULL in 
 *			AddNewWindProfilerObs. Bumped maxfiles to 100.
 *	12/99	COMET	Changed datemake() and makedate() function calls
 *			to sec2yyyymmddhhmm() and yyyymmddhhmm2sec().
 *	01/03	Unidata	Bumped AddString to 300, added fuz to time for 6 min data
 *
 ***********************************************************************/


#include <X11/X.h>

#include "winobj.h"
#include "mainwin.h"
#include "gendef.h"
#include "genglobs.h"
#include "_proto.h"
#include "guimacros.h"
#include "timeutil.h"


/*
 *	Searches for wind profiler data (times) newer than what is contained
 *	in the input reference metobject. Returns a list of times and filenames.
 */

void
GetNewWindProfilerObs ( MetObjectType *m, int *count,
			char ***newFiles, char ***newTimestamps )
{
	int			nfiles, index;
	int			i, iret;
	char			*directory;
	char			*fileFilter;
	char			**files;
	ListParentObjectType	list;


	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("GetNewWindProfilerObs()\n");

	*count = 0;

/*
 *	Get directory and filename filter.
 */
	directory  = GetConfigValue( "profiler" );
	fileFilter = GetConfigValue( "profilerK" );

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
 *		Return the whole list. We will decide later what will be
 *		used.
 */
		*count = list.listcount;
		*newFiles = list.string;
		*newTimestamps = list.label;

	    }		 

	}
/*
 *	free stuff
 */
 	Free ( directory);
 	Free ( fileFilter);
	StringListFree( nfiles, files );

}


/*
 *	Adds new times to wind profiler data plots. Profiler updating
 *	is a litle different than other updating, since each frame is
 *	a different location, and one frame is a full time series.
 */

void
AddNewWindProfilerObs (	WindowObjectType *wo, int mindex, int *count,
			char **files, char **timestamps)
{
	MetObjectType		*mo;
	PixmapObjectType	*po;
	WindProfilerObjectType	*wpo;
	AbsTime			list_time, refTime;
	AbsTime			int_begin, int_end;
	int			dummyOffset;
	char			*begin_time;
	char			*end_time;
	char			*interval_time;
	char			**curdattim, **snfiles=NULL;
	char			dattim[GEMPAKSTRING];
	char			taxis[GEMPAKSTRING];
	int			i, j, nfiles, iret;
	int			int_duration;
	int			int_label;
	int			int_time, numParts;
	
	
	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("AddNewWindProfilerObs()\n");


	if ( *count < 1 ) return;

/*
 *	Get a handle on the windprofiler object. Use the one in the first
 *	pixmap for convenience.
 */
	po = wo->pixmapObjects[0];
	mo = po->metObjects[mindex];
	wpo = (WindProfilerObjectType *) mo->metObjectContent;

/*
 *	Test to see if the latest time in the list is newer than what's
 *	already plotted. If not, free the lists. This is a bit different
 *	than the other data types, but then this is a time series. Set the
 *	count = 0 (to indicate there is nothing to update) and exit.
 */
	refTime = String2LongTime ( timestamps[(*count) - 1], &dummyOffset );

	if ( refTime <= mo->refTime ) {
	    StringListFree ( *count, files);
	    StringListFree ( *count, timestamps);	
	    *count = 0;
	    return;
	}

/*
 *	Grab the current dattim string and break it into components,
 *	(begin-end-interval)
 */
	curdattim = SplitByDelimeter( wpo->dattim, "-", &numParts);

	if ( numParts != 3 ) {
	    printf ("AddNewWindProfilerObs: Bad date/time: %s\n",wpo->dattim);
	    return;
	}
	
/*
 *	Get the currently plotted data time info.
 */
	begin_time = strdup ( curdattim[0] );
	end_time = strdup ( curdattim[1] );
	interval_time = strdup ( curdattim[2] );

	int_end = yyyymmddhhmm2sec ( end_time );
	int_begin = yyyymmddhhmm2sec ( begin_time );
	int_duration = (int_end - int_begin) / SECONDS_PER_HOUR;
	
/*
 *	Make the new time string
 */
	Free ( end_time );
	end_time = strdup ( timestamps[(*count) - 1] );
	int_end = yyyymmddhhmm2sec ( end_time );
	int_begin = int_end - int_duration * SECONDS_PER_HOUR;

	Free ( begin_time);
	begin_time = sec2yyyymmddhhmm ( int_begin );
	sprintf ( dattim, "%s-%s-%s", begin_time, end_time, interval_time );

/*
 *	Add an hour to end_time label so barbs won't overlay axis.
 */
	int_label = int_duration / 8;
	int_end = int_end + SECONDS_PER_HOUR;
	Free ( end_time );
	end_time = sec2yyyymmddhhmm ( int_end );
	sprintf ( taxis, "%s-%s-%d;1;0;1",
			      end_time, begin_time, int_label );

/*
 *	Now get the list of files needed to plot these times.
 */
	nfiles = 0;
	for ( j = 0; j < int_duration; j++ ) {

	    for ( i = 0; i < *count; i++ ) {
/*
 *	        Do comparisons in integer time.
 */
		int_time = yyyymmddhhmm2sec ( begin_time );
		int_time += j * SECONDS_PER_HOUR;
		list_time = yyyymmddhhmm2sec ( timestamps[i] );
		/*if ( int_time == list_time ) {*/
		if ( ( int_time > ( list_time - SECONDS_PER_HOUR ) ) &&
		     ( int_time < ( list_time + SECONDS_PER_HOUR ) ) ) {
                    if( GetVerboseLevel() > VERBOSE_1 )
                        printf("AddNewWindProfilerObs: adding %s\n",
		                timestamps[i] );
/*
 *		    Add only new file to list.
 */
                    AddString ( 300, &nfiles, &snfiles, files[i] );
                    break;
                }
            }
        }


	if ( GetVerboseLevel () > VERBOSE_0 ) 
		printf("AddNewWindProfilerObs: new time: %s\n",dattim);

/*
 *	Loop through pixmaps and update each one.
 */
	for ( i = 0; i < wo->numPixmapObjects; i++ ) {

	    po = wo->pixmapObjects[i];
	    mo = po->metObjects[mindex];
	    wpo = (WindProfilerObjectType *) mo->metObjectContent;
/*
 *	    Set the new time info
 */
	    strcpy (mo->timestamp, dattim);
	    mo->refTime   = refTime;
	    mo->fcstTime  = 0;
	    mo->validTime = mo->refTime + mo->fcstTime;    

/*
 *	    Update dattim in the object's content
 */
	    strcpy (wpo->dattim, dattim);
	    strcpy (wpo->taxis, taxis);

/*
 *	    Copy the list of files into the object.
 */
	    wpo->nfiles = nfiles;
	    for ( j = 0; j < nfiles; j++ )
	    	strcpy ( wpo->snfiles[j], snfiles[j]);

/*
 *	    Clear the pixmap. Set it so that it will be redrawn.
 */
	    ClearPot ( wo, po );
	    SetPixmapObjectDrawnFlag ( po, False);

	}

	Free (begin_time);
	Free (end_time);
	Free (interval_time);
	StringListFree ( numParts, curdattim );
	StringListFree ( nfiles, snfiles );

	return;

}
