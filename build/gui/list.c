/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	list.c
 *
 *	Callbacks and utilities used in scrolled windows for all data types.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/97	COMET	Wrote BuildStationList, a more general list 
 *			building routine.
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 7/97	COMET	Dynamically initialize surface station parameters.
 *	 8/97	COMET	Added time matching code.
 *	12/97	COMET	Added list_label arg to BuildUpperairList,
 *			BuildSfcObsList and BuildUpfList
 *	12/97	COMET	Added a "select_list" object to track date/time
 *			selections when time matching is turned off.
 *	 1/98	COMET	Added sanity tests for undefined keys.
 *
 ***********************************************************************/

#include "geminc.h"
#include "gemprm.h"
#include "guimacros.h"
#include "genglobs.h"
#include "utils.h"
#include "winobj.h"
#include "upaobj.h"
#include "sfcobj.h"
#include "ctbcmn.h"
#include "_proto.h"


void
ScrubSfcObsList ( ListParentObjectType *list_struct )
{
	return;
}


void
ScrubUpaList ( ListParentObjectType *list_struct )
{
	char		**labels, **strings;
	int		i, listcount;
	int		verbose;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "ScrubUpaList\n" );

/*
 *	Allocate maximum space required.
 */
	labels  = StringListAllocate ( list_struct->listcount );
	strings = StringListAllocate ( list_struct->listcount );
	listcount = 0;

/*
 *	Write label and file name to list if it is a 00Z or 12Z time.
 */
	for ( i=0; i<list_struct->listcount; i++ ) {
	    if ( strstr ( list_struct->label[i], "/00" ) ||
		 strstr ( list_struct->label[i], "/12" ) ) {
		labels[listcount] = strdup ( list_struct->label[i] );
		strings[listcount] = strdup ( list_struct->string[i] );
		listcount++;
	    }
	}

	if ( verbose > VERBOSE_1 ) {
	    for ( i=0; i<listcount; i++ ) {
		printf("label = %s\n", labels[i] );
		printf("strings = %s\n", strings[i] );
	    }
	}

/*
 *	Free memory and rewrite lists.
 */
	FreeList ( list_struct->listcount, list_struct->string );
	FreeList ( list_struct->listcount, list_struct->label );
	list_struct->listcount = listcount;
	list_struct->label = labels;
	list_struct->string = strings;

	return;
}


void
BuildUpperairList ( Widget scrolled_list,
		    Widget list_label,
		    Widget filter,
                    int nfiles,
                    ListParentObjectType *list_struct,
                    ListIndexObjectType *match_list,
	            ListIndexObjectType *select_list )
{
	GlobalDataObjectType	*gd;

	char			*directory;
	char			*string;
	char			**labels, **files;
	char			*key1="upperair";
	char			*key2="upperairK";
	int			nlabels, filec;
	int			iret, verbose;
	int			scrollbar_position=0;

	iret = NORMAL;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "BuildUpperairList\n" );

/*
 *	Get directory.
 */
	directory = GetConfigValue( key1 );
	if ( directory == NULL && verbose > VERBOSE_1 )
	    printf("  Warning: key '%s' is undefined in Garp_defaults\n", key1);

/*
 *	Free previously allocated memory.
 */
	if ( list_struct->listcount > 0 ) {
	    FreeList ( list_struct->listcount, list_struct->string );
	    FreeList ( list_struct->listcount, list_struct->label );
	    list_struct->listcount = 0;
	}

	if ( match_list->listcount > 0 ) {
	    Free ( match_list->intList );
	    match_list->listcount = 0;
	}

/*
 *	Get a filtered list of files to read.
 */
	string = GetConfigValue( key2 );
	if ( string == NULL && verbose > VERBOSE_1 )
	    printf("  Warning: key '%s' is undefined in Garp_defaults\n", key2);

	if ( GetFilteredDirectoryList 
		( string, directory, &files, &filec ) ) {

/*
 *	    Make sure directory and files exist.
 */
	    if ( filec > 0 ) {

/*
 *	        Get list of date/time stamps and associated files.
 */
	        nlabels = GetUpaList ( directory, files, filec,
		                       list_struct, &iret );
	        list_struct->listcount = nlabels;

/*
 *		Filter all but 00Z and 12Z times from list.
 */
		if ( XmToggleButtonGetState ( filter ) )
			ScrubUpaList ( list_struct );

/*
 *	        Add labels to scrolled list.
 */
		SetScrolledList ( UPPERAIROBJECT, list_label, scrolled_list,
	                          list_struct, match_list, select_list );

		FreeList ( filec, files );
	    }
	}
	else {
	    if( directory != NULL && verbose > VERBOSE_1 ) {
	    printf("*** There are no Upperair Obs files in directory:\n");
	    printf("*** %s\n", directory );
	    }
	}

/*
 *	Free.
 */
	if ( directory != NULL ) Free ( directory );
	if ( string != NULL )    Free ( string );

	return;
}


void
BuildUpfList ( Widget scrolled_list,
	       Widget list_label,
	       Widget filter,
               int nfiles,
               ListParentObjectType *list_struct,
               ListIndexObjectType *match_list,
	       ListIndexObjectType  *select_list )
{
	GlobalDataObjectType	*gd;

	char			*directory;
	char			*string;
	char			**labels, **files;
	char			*key1="upperair";
	char			*key2="upperairK";
	int			nlabels, filec;
	int			iret, verbose;
	int			scrollbar_position=0;

	iret = NORMAL;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "BuildUpfList\n" );

/*
 *	Get directory.
 */
	directory = GetConfigValue( key1 );
	if ( directory == NULL && verbose > VERBOSE_1 )
	    printf("  Warning: key '%s' is undefined in Garp_defaults\n", key1);

/*
 *	Free previously allocated memory.
 */
	if ( list_struct->listcount > 0 ) {
	    FreeList ( list_struct->listcount, list_struct->string );
	    FreeList ( list_struct->listcount, list_struct->label );
	    list_struct->listcount = 0;
	}

/*
 *	Get a filtered list of files to read.
 */
	string = GetConfigValue( key2 );
	if ( string == NULL && verbose > VERBOSE_1 )
	    printf("  Warning: key '%s' is undefined in Garp_defaults\n", key2);

	if ( GetFilteredDirectoryList 
		( string, directory, &files, &filec ) ) {

/*
 *	    Make sure directory and files exist.
 */
	    if ( filec > 0 ) {

/*
 *	        Get list of date/time stamps and associated files.
 */
	        nlabels = GetUpaList ( directory, files, filec,
		                       list_struct, &iret );
	        list_struct->listcount = nlabels;

/*
 *		Filter all but 00Z and 12Z times from list.
 */
		if ( XmToggleButtonGetState ( filter ) )
			ScrubUpaList ( list_struct );

/*
 *	        Add labels to scrolled list and time match
 */
		SetScrolledList ( UPPERAIRPROFILEOBJECT,
				  list_label, scrolled_list,
	                          list_struct, match_list, select_list );

		FreeList ( filec, files );
	    }
	}
	else {
	    if( directory != NULL && verbose > VERBOSE_1 ) {
	    printf("*** There are no Upperair Obs files in directory:\n");
	    printf("*** %s\n", directory );
	    }
	}

/*
 *	Free.
 */
	if ( directory != NULL ) Free ( directory );
	if ( string != NULL )    Free ( string );

	return;
}


void
BuildProfilerList ( int nfiles,
                    ListParentObjectType *list_struct )
{
	GlobalDataObjectType	*gd;

	char			*directory;
	char			*string;
	char			**labels, **files;
	char			*key1="profiler";
	char			*key2="profilerK";
	int			nlabels, filec;
	int			iret, verbose;

	iret = NORMAL;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "BuildProfilerList\n" );

/*
 *	Get directory.
 */
	directory = GetConfigValue( key1 );
	if ( directory == NULL && verbose > VERBOSE_1 )
	    printf("  Warning: key '%s' is undefined in Garp_defaults\n", key1);

/*
 *	Free previously allocated memory.
 */
	if ( list_struct->listcount > 0 ) {
	    FreeList ( list_struct->listcount, list_struct->string );
	    FreeList ( list_struct->listcount, list_struct->label );
	    list_struct->listcount = 0;
	}

/*
 *	Get a filtered list of files to read.
 */
	string = GetConfigValue( key2 );
	if ( string == NULL && verbose > VERBOSE_1 )
	    printf("  Warning: key '%s' is undefined in Garp_defaults\n", key2);

	if ( GetFilteredDirectoryList 
		( string, directory, &files, &filec ) ) {

/*
 *	    Make sure directory and files exist.
 */
	    if ( filec > 0 ) {

/*
 *	        Get list of date/time stamps and associated files.
 */
	        nlabels = GetUpaList ( directory, files, filec,
		                       list_struct, &iret );
	        list_struct->listcount = nlabels;

		FreeList ( filec, files );
	    }
	}
	else {
	    if( directory != NULL && verbose > VERBOSE_1 ) {
	    printf("*** There are no Profiler files in directory:\n");
	    printf("*** %s\n", directory );
	    }
	}

/*
 *	Free.
 */
	if ( directory != NULL ) Free ( directory );
	if ( string != NULL )    Free ( string );

	return;
}


void
BuildSfcObsList   ( Widget scrolled_list,
		    Widget list_label,
                    int nfiles,
		    char *plot_type,
                    ListParentObjectType *list_struct,
		    ListIndexObjectType  *match_list,
		    ListIndexObjectType  *select_list )
{
	GlobalDataObjectType	*gd;

	char			*directory, plot_dir[256];
	char			**labels, **files;
	int			nlabels, filec;
	int			verbose, iret;
	int			scrollbar_position=0;

	iret = NORMAL;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "BuildSfcObsList\n" );

	if ( plot_type == NULL ) return;

/*
 *	Get directory and search key.
 */
	sprintf ( plot_dir, "%s%s", plot_type, "_dir" );
	directory = GetConfigValue( plot_dir );
	if ( directory == NULL && verbose > VERBOSE_1 )
	    printf("  Warning: key '%s' is undefined in Garp_defaults\n",
	            plot_dir );

/*
 *	Free previously allocated memory.
 */
	if ( list_struct->listcount > 0 ) {
	    FreeList ( list_struct->listcount, list_struct->string );
	    FreeList ( list_struct->listcount, list_struct->label );
	    list_struct->listcount = 0;
	}

	if ( match_list->listcount > 0 ) {
	    Free ( match_list->intList );
	    match_list->listcount = 0;
	}

/*
 *	Get a filtered list of files to read.
 */
	if ( GetFilteredDirectoryList 
		( plot_type, directory, &files, &filec ) ) {

/*
 *	    Make sure directory exists.
 */
	    if ( filec > 0 ) {

/*
 *	        Get list of date/time stamps and associated files.
 */
	        nlabels = GetSfcList ( directory, files, filec,
		                       list_struct, &iret );
	        list_struct->listcount = nlabels;

/*
 *	        Scrub duplicate entries from list.
 */
	        ScrubSfcObsList ( list_struct );

/*
 *	        Add labels to scrolled list. Timematch.
 */
	        SetScrolledList ( SURFACEOBJECT, list_label, scrolled_list, 
			          list_struct, match_list, select_list );

		FreeList ( filec, files );
	    }

	}
	else {
	    if( directory != NULL && verbose > VERBOSE_1 ) {
	    printf("*** There are no Surface Obs files in directory:\n");
	    printf("*** %s\n", directory );
	    }
	}

/*
 *	Free.
 */
	if ( directory != NULL ) Free ( directory );

	return;
}


void
BuildProfilerStationList ( Widget scrolled_list,
                           ListParentObjectType *list_struct )
{
	extern struct datatype_list	mastertbl[MAXTYP];
	struct station_list		station_info[LLSTFL];
	GlobalDataObjectType	*gd;

	char			*directory, name[80];
	char			**labels;
	char			*key="profiler";
	int			nlabels;
	int			i, verbose, len, iret;

	iret = NORMAL;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "BuildProfilerStationList\n" );

/*
 *	Get directory.
 */
	directory = GetConfigValue( key );
	if ( directory == NULL && verbose > VERBOSE_1 )
	    printf("  Warning: key '%s' is undefined in Garp_defaults\n", key );

/*
 *	Free previously allocated memory.
 */
	if ( list_struct->listcount > 0 ) {
/*				Not currently used.
	    FreeList ( list_struct->listcount, list_struct->string ); */
	    FreeList ( list_struct->listcount, list_struct->label );
	    list_struct->listcount = 0;
	}

/*
 *	Read in a list of wind profiler stations. Create list.
 */
	get_table ( mastertbl[3], &station_info,
		    &(list_struct->listcount), &iret );

/*
 *	    Make sure directory and files exist.
 */
	if ( list_struct->listcount < 1 ) return;

/*
 *	Copy station information.
 */
	labels = StringListAllocate( 50 );
	for ( i=0; i<list_struct->listcount; i++ ) {
	    len = 5 + strlen ( station_info[i].stid ) +
		      strlen ( station_info[i].name ) +
		      strlen ( station_info[i].state );
	    labels[i] = (char *) malloc ( len * sizeof( char ) );
	    strbrm ( station_info[i].name );
	    sprintf ( labels[i], "%s  %s,  %s", station_info[i].stid,
			  station_info[i].name, station_info[i].state );
	}

	list_struct->label = labels;

/*
 *	 Add labels to scrolled list.
 */
	 AddProductsToList ( scrolled_list, list_struct->listcount,
	                            list_struct->label );

/*
 *	Free.
 */
	if ( directory != NULL ) Free ( directory );

	return;
}


void
BuildStationList ( Widget scrolled_list, char *directory, int index,
                   ListParentObjectType *list_struct )
{
	extern struct datatype_list	mastertbl[MAXTYP];
	struct station_list		station_info[LLSTFL];
	GlobalDataObjectType	*gd;

	char			name[80];
	char			**labels;
	int			nlabels;
	int			i, verbose, len, iret;

	iret = NORMAL;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "BuildStationList\n" );

/*
 *	Free previously allocated memory.
 */
	if ( list_struct->listcount > 0 ) {
/*				Not currently used.
	    FreeList ( list_struct->listcount, list_struct->string ); */
	    FreeList ( list_struct->listcount, list_struct->label );
	    list_struct->listcount = 0;
	}

/*
 *	Read in a list of stations using index in master list.
 *	Create list.
 */
	get_table ( mastertbl[index], &station_info,
		    &(list_struct->listcount), &iret );

/*
 *	    Make sure directory and files exist.
 */
	if ( list_struct->listcount < 1 ) return;

/*
 *	Copy station information.
 */
	labels = StringListAllocate( list_struct->listcount );
	for ( i=0; i<list_struct->listcount; i++ ) {
	    len = 5 + strlen ( station_info[i].stid ) +
		      strlen ( station_info[i].name ) +
		      strlen ( station_info[i].state );
	    labels[i] = (char *) malloc ( len * sizeof( char ) );
	    strbrm ( station_info[i].name );
	    sprintf ( labels[i], "%s  %s,  %s", station_info[i].stid,
			  station_info[i].name, station_info[i].state );
	}

	list_struct->label = labels;

/*
 *	 Add labels to scrolled list.
 */
	 AddProductsToList ( scrolled_list, list_struct->listcount,
	                            list_struct->label );

	return;
}


char *
ReturnKeyMatch ( char *key, char *keyname, char *labelname )
{
	char			*file=NULL, *keylist, *labellist;
	char			**keys=NULL, **labels=NULL;
	int			i, numkeys, numlabels;


	if ( key == NULL ) return file;

	keylist = GetConfigValue( keyname );
	labellist = GetConfigValue( labelname );

	keys = SplitByDelimeter ( keylist, ",", &numkeys );
	labels = SplitByDelimeter ( labellist, ",", &numlabels );

	for ( i=0; i<numkeys; i++ ) {
	    if ( strcmp ( key, keys[i] ) == 0 ) {
		file = strdup ( labels[i] );
		break;
	    }
	}
/*
 *	Free.
 */
	if ( keylist != NULL )   Free ( keylist );
	if ( labellist != NULL ) Free ( labellist );
	StringListFree ( numkeys, keys );
	StringListFree ( numlabels, labels );

	return file;
}


void
BuildSurfaceParameterList ( Widget scrolled_list,
                           ListParentObjectType *list_struct )
{
	GuiSfcObsDialogType	*sfc;
	GlobalDataObjectType	*gd;

	char			*directory, *file;
	char			**labels;
	char			*plot_type;
	char			*key1="tabledir";
	int			max;
	int			i, verbose, len, iret;

	

	iret = NORMAL;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "BuildSurfaceParameterList\n" );

	sfc = GetGuiSfcObsDialog();
/*
 *	Get directory and station parameter table file name to read.
 */
	directory = GetConfigValue( key1 );
	if ( directory == NULL && verbose > VERBOSE_1 )
	    printf("  Warning: key %s is undefined in Garp_defaults\n", key1 );

	plot_type = GetGuiSfcObsType ( sfc );

	file = ReturnKeyMatch ( plot_type, "sfc_keys", "sfc_tables" );

	max = 500;
	len = 100;

/*
 *	Free previously allocated memory.
 */
	if ( list_struct->listcount > 0 ) {
/*				Not currently used.
	    FreeList ( max, list_struct->string ); */
	    FreeList ( max, list_struct->label );
	    list_struct->listcount = 0;
	}

/*
 *	Read in a list of wind profiler stations. Create list.
 */
	readlist ( directory, file, max, len, &(list_struct->listcount), 
		      &labels, &iret );

/*
 *	Make sure directory and files exist.
 */
	if ( list_struct->listcount < 1 ) return;
	list_struct->label = labels;

/*
 *	 Add labels to scrolled list.
 */
	 AddProductsToList ( scrolled_list, list_struct->listcount,
	                            list_struct->label );
/*
 *	Free.
 */
	if ( directory != NULL ) Free ( directory );
	if ( file != NULL )      Free ( file );

	return;
}
