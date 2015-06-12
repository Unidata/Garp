/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	config.c
 *
 *	Configured parameter functions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory 
 *			sanity.
 *	 5/97	COMET	Added -output option to support logging.
 *	10/97	COMET	Added some fallbacks for lat/lon grid
 *	 1/98	OCMET	Add a cast to remove compiler warning
 *
 ***********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "gendef.h"
#define  _NO_GPRINT		/* use regular printf function in these functions. */
#include "genglobs.h"
#include "_proto.h"

/* #define gprint printf	-* use regular printf function in these functions. */

/*
 *  Data structures private to this function.
 */
typedef struct {
        char    *key;
        char    *value;
} ConfigFileKeyValueType;


typedef struct {
        char    *option;
        char    *key;
        char    *value;
        int     hasArg;
} CommandLineOptionTypes;


void Usage();

CommandLineOptionTypes *SearchForOption( char *option, 
				CommandLineOptionTypes *cl );

/*
 *
 *      Fallback values for some config file keys. If a call is made
 *	to get a configvalue, then this source is checked last.
 *
 */
static ConfigFileKeyValueType FallBackValues[] = {
     VERBOSE,             "0",
     OUTPUT,              "0",
     STARTTIME,           NULL,
     ENDTIME,             NULL,
     CURRENTTIME,         NULL,
     MASTERTBL,           "master_garp.tbl",
     SATENHANCETBL,       "sat_enhance",
     RADENHANCETBL,       "rad_enhance.tbl",
     LOWRESCOSTAL,        "locowo.gsf",
     MEDRESPOLITICALWORLD,"mepowo.gsf",
     HIRESPOLITICALWORLD, "hipowo.gsf",
     HIRESCOUNTYUS,       "hicnus.gsf",
     TABLEDIR,            NULL,
     CLOCKINTERVAL,       "1",
     MINZOOMBOXSIZE,      "25",
     ZOOMBOXCOLOR,	  "1",
     SWIDTH,		  "0",
     SHEIGHT,		  "0",
     LATLON,		  "no",
     LONINC,		  "5.0",
     LATINC,		  "5.0",
     LBLINC,		  "2",
     GRIDLINECOLOR,	  "2",
     GRIDLINETYPE,	  "10",
     GRIDLINEWIDTH,	  "1",
     NULL,                NULL
};

/*
 *
 *	Data structure used to store the command line options
 *	and associated values.  Filled in by ParseCommandLine().
 *	
 *	Note that the command line options are decoupled from the
 *	config file values.  The CommandLineOptionTypes data structure
 *	associates an option line -start with the the config file
 *	key, like "StartTime".  The HELPN_OPTION is exception
 *	and only included here as a place holder.
 *
 *	Some values may be hardwired here to help bootstrap the system.
 *	When set, these values will take precedence over anyother values,
 *	so use this feture carefully.  The only value hardwired in
 *	this manner is:  CONFIGFILE.
 *
 *
 */
static 	CommandLineOptionTypes CommandLineValues[] = {

     HELP1_OPTION,         HELP1_OPTION,  NULL,            0,
     HELP2_OPTION,         HELP2_OPTION,  NULL,            0,
     PROGNAME,             PROGNAME,      NULL,            0,
     CLOCKINTERVAL_OPTION, CLOCKINTERVAL, NULL,            1,
     NOTICK_OPTION,        NOTICK,        NULL,            0,
     VERBOSE_OPTION,       VERBOSE,       NULL,            1,
     MEMCHECK_OPTION,      MEMCHECK,      NULL,            0,
     OUTPUT_OPTION,        OUTPUT,        NULL,            1,
     NOICONS_OPTION,       NOICONS,       NULL,            0,
     NOSTATUS_OPTION,      NOSTATUS,      NULL,            0,
     STARTTIME1_OPTION,    STARTTIME,     NULL,            1,
     STARTTIME2_OPTION,    STARTTIME,     NULL,            1,
     ENDTIME1_OPTION,      ENDTIME,       NULL,            1,
     ENDTIME2_OPTION,      ENDTIME,       NULL,            1,
     CURRENTTIME1_OPTION,  CURRENTTIME,   NULL,            1,
     CURRENTTIME2_OPTION,  CURRENTTIME,   NULL,            1,
     SWIDTH_OPTION, 	   SWIDTH,  	  NULL,            1,
     SHEIGHT_OPTION,  	   SHEIGHT,  	  NULL,            1,
     TABLEDIR_OPTION,      TABLEDIR,      NULL,            1,
     CONFIGFILE_OPTION,    CONFIGFILE,    "Garp_defaults", 1,
     NULL,                 NULL,          NULL,        (int)NULL

};


/*
 *	After we parse the command line and remove our parameters
 *	from the argv array, we leave null's in their place.  This
 *	routine heavy handedly removes the NULLS from argv.
 */
void SqueezeArgv( int newArgc, int argc, char **argv )
{

	char		**av1, **av2;
	int		i;


/*
 *	Check for the trivial and most common case where we've
 *	parsed everything out of the command line except the 
 *	program name.
 */ 
	if( newArgc == 1 )
		return;


/*
 *	Create a temporary argv array to hold the commandline
 *	vector.
 */
	av2 = av1 = (char **) malloc( sizeof( char **) * newArgc );

/*
 *	Copy the old one into the new one, skipping the nulls
 */
	for(i = 0; i < argc; i++ ) {

		if( argv[i] )
	
			*av1++ = argv[i];

	}

/*
 *	Copy the new argv back to the new one.
 */
	memcpy( argv, av2, sizeof(char *) * newArgc );

/*
 *	Release the malloc'd memory...
 */
	free( av2 );

	return;

}


/*
 *
 *	Parse the command line, pulling out the options we recognize 
 *	and stuff them into a data structure.
 *
 */
void ParseCommandLine( int *argc, char **argv )
{
	int			ac;
	CommandLineOptionTypes *cl;
	int			argsConsumed;
	char 			**sargv;

	argsConsumed = 0;
	sargv        = argv;


/*
 *	Get the program name....
 */
	if ( cl = SearchForOption( PROGNAME, CommandLineValues ) ) {
		char	*p;
		cl->value = argv[0];
		if(( p = strrchr( argv[0], '/' ) ) )
			cl->value = strdup( p + 1 );
	}

	ac = *argc;
	while( --ac ) {

		argv++;

		if( ! ( *argv[0] == '-' ) )

			continue;

		for( cl = CommandLineValues; cl->option; cl++ ) {

			if( ! EQSTRING( *argv, cl->option ) )

				continue;


			if( cl->hasArg ) {

				if(( ac - 1 == 0 ) || ( *(argv+1)[0] == '-' ))
					/* error - option without arg  */
					Usage();

/*
 *
 *				We found one of our options that has an
 *				argument behind it.  Blank out the argv 
 *				entries, snarf the options argument, 
 *				increment the number of args we've by two, 
 *				seen and break out of the for(;;) 
 *
 */
				*argv = NULL;  

				argv++; ac--;
				cl->value = *argv;

				*argv = NULL;

				argsConsumed += 2;

				break;

			} /* hasArg */

/* 
 *			Found a match, but the option has no arg, so
 *			we set the value to boolean true 
 */
			cl->value = "True";
			*argv = NULL;

			argsConsumed++;

			break;

		} /* for(;;) */

	}  /* while() */


/*
 *	Search for Help options and just print out a usage 
 *	message if found.
 */
	if ( cl = SearchForOption( HELP1_OPTION, CommandLineValues ) )
		if( cl->value )
			Usage();
	if ( cl = SearchForOption( HELP2_OPTION, CommandLineValues ) )
		if( cl->value )
			Usage();

/*
 *	Squeeze out all of the NULL parameters we left in argv.
 */
	SqueezeArgv( *argc - argsConsumed, *argc, sargv );

/*
 *	Adjust argc...
 */
	*argc =  (*argc) - argsConsumed;

	return;
}




/*
 *	Utility routine to return a pointer to a commandline
 *	entry, give the option.
 */
CommandLineOptionTypes 
*SearchForOption( char *option, CommandLineOptionTypes *cl )
{
	for( ; cl->option; cl++ ) {
		if( EQSTRING( option, cl->option ) )
			return( cl );
	}

	return( NULL );
}


	 
/*
 *	Utility function to search through the private 
 *	CommandLineOptionTypes data structure and return
 *	a copy of the  value.  Returns 1 on success and 0
 *	on failure.
 */
int GetCommandLineValue( char *key, char *value )
{

	CommandLineOptionTypes *cl;
	int			foundOne;

	*value = '\0';

/*
 *	Iterate through all the commandline values.  Returning
 *	the value for the entry that matches key.  There is a
 *	little bit of fooling around here to support the idea
 *	a given key might have more than one way of being set.
 *	For example, the option -start and -start_time set the
 *	same key/value pair (namely StartTime).  This code fragement
 *	will look through all the CommandLine possibilities, returning
 *	the key/value that actually has a value set, or NULL, if 
 *	that command line value isn't set.
 */
	for( foundOne = 0, cl = CommandLineValues; cl->option; cl++ ) {
		if( EQSTRING( key, cl->key ) ) {
			foundOne++;
			if( cl->value )
				strcpy( value, cl->value );
		}
	}

	return( foundOne );
}


/*
 *	Dump out the members of CommandLineOptionTypes.
 */
void
PrintCmdLineOptions( CommandLineOptionTypes *cl )
{

	for( ; cl->option; cl++ ) {
		printf("%s,", cl->option );
		printf(" %s\n", cl->key );
		if(cl->hasArg)
			printf("	%s\n", cl->value);
	}
}

/*
 *	Dump out a (lame) usage  message...
 */
void 
Usage()
{

	CommandLineOptionTypes *cl;

	cl = SearchForOption( PROGNAME, CommandLineValues );
	printf("Usage: %s ", cl->value );
	printf("[ %s |", HELP1_OPTION );
	printf(" %s ]", HELP2_OPTION );
	printf(" [ %s seconds ] ", CLOCKINTERVAL_OPTION );
	printf("\n       ");
	printf("[ %s filename] ",CONFIGFILE_OPTION );
	printf("[ %s YYMMDD/HHMM ] ", STARTTIME1_OPTION );
	printf("\n       ");
	printf("[ %s YYMMDD/HHMM ] ", CURRENTTIME1_OPTION );
	printf("[ %s YYMMDD/HHMM ] ", ENDTIME1_OPTION );
	printf("\n       ");
	printf("[ %s ] ", NOTICK_OPTION );
	printf("[ %s level ] ", VERBOSE_OPTION );
	printf("[ %s ] ", MEMCHECK_OPTION );
	printf("[ %s destination ] ", OUTPUT_OPTION );
	printf("\n       ");
	printf("[ %s file ]", TABLEDIR_OPTION );
	printf("[ %s ] ", NOICONS_OPTION );
	printf("[ %s ] ", NOSTATUS_OPTION );
	printf("\n       ");
	printf("[ %s w ] ", SWIDTH_OPTION );
	printf("[ %s h ] ", SHEIGHT_OPTION );
	printf("\n");


	printf("Where:\n\n");


	printf("  [ %s |", HELP1_OPTION );
	printf(" %s ]", HELP2_OPTION );
	printf(":\n");
	printf("               Prints out this usage message.\n");
	printf("  [ %s seconds ]", CLOCKINTERVAL_OPTION );
	printf(":\n");
	printf("               Set the clock update interval to seconds.\n");
	printf("  [ %s filename ]", CONFIGFILE_OPTION );
	printf(":\n");
	printf("               Specifies application configuration file.\n");
	printf("  [ %s YYMMDD/HHMM ]", STARTTIME1_OPTION );
	printf(":\n");
	printf("               Specifies start time of data set.\n");
	printf("  [ %s YYMMDD/HHMM ]", CURRENTTIME1_OPTION );
	printf(":\n");
	printf("               Specifies initial time of application clock.\n");
	printf("  [ %s YYMMDD/HHMM ]", ENDTIME1_OPTION );
	printf(":\n");
	printf("               Specifies end time of data set.\n");
	printf("  [ %s ]", NOTICK_OPTION );
	printf(":\n");
	printf("               Turns off application clock updates.\n");
	printf("  [ %s level ]", VERBOSE_OPTION );
	printf(":\n");
	printf("               Verbose logging level (ie. 0, 1, ..).\n");
	printf("  [ %s ]", MEMCHECK_OPTION );
	printf(":\n");
	printf("               Enable loose memory checking (for debug).\n");
	printf("  [ %s destination ]", OUTPUT_OPTION );
	printf(":\n");
	printf("               Output logging dest:	0 = Standard out\n");
	printf("                                        1 = Logging window\n");
	printf("                                        2 = File\n");
	printf("  [ %s file ]", TABLEDIR_OPTION );
	printf(":\n");
	printf("               Specifies path to application tables.\n");
	printf("  [ %s ]", NOICONS_OPTION );
	printf(":\n");
	printf("               Start GARP without the icon bar.\n");
	printf("  [ %s ]", NOSTATUS_OPTION );
	printf(":\n");
	printf("               Start GARP without the status bar.\n");
	printf("  [ %s w]", SWIDTH_OPTION );
	printf(":\n");
	printf("               Set drawing area width to w.\n");
	printf("  [ %s h]", SHEIGHT_OPTION );
	printf(":\n");
	printf("               Set drawing area height to h.\n");
	printf("\n");
	exit(1);

}

/*
 *	Given a key, iterate through Configfile fallback resources
 *	returning the value.  Return 1 on success and 0 on failure.
 *
 */
int GetFallBackConfigValue( char *key, char *value )
{

        ConfigFileKeyValueType		*cp;

	*value = '\0';

        for( cp = FallBackValues; cp->key ; cp++ )
		if( EQSTRING( key, cp->key ) )
			break;

	if( cp->key && cp->value ) {
		strcpy( value, cp->value );
		return( 1 );
	}

	return( 0 );

}
		

#if 0
#define CACHECHUNKSIZE	512
static ConfigFileKeyValueType *cachePointer;
static int cacheElementCount;
static int cacheSize;
static int cacheHits;
static int cacheMisses;

int CheckConfigCache( char *key, char *value )
{
	
	ConfigFileKeyValueType *cp = cachePointer;
	int	hit;
	int	i;
	
	return ( 0 );

	if( cacheElementCount == 0 )
		return ( 0 );

	hit = 0;
	for( cp = cachePointer, i = 0; i < cacheElementCount; cp++, i++ )
		if( EQSTRING( cp->key, key ) ) {
			hit++;
			break;
		}

	if( hit ) {
		cacheHits++;
		strcpy( value, cp->value );
		return( 1 );
	}

	cacheMisses++;
	return( 0 );

}

void GrowConfigCache()
{
	int	newSize;

/*
 *	Get new cache size, in terms of number of elements.
 *	Note that the first time through, cacheElementCount will
 *	be zero.
 */
	if( GetVerboseLevel() )
		printf("GrowConfigCache: current cache size is %d\n", 
				cacheSize);

	newSize = cacheSize + CACHECHUNKSIZE;
	
	if( GetVerboseLevel() )
		printf("GrowConfigCache: new cache size is %d\n", newSize);
/*
 *	Reallocate a larger chunk of memory for the cache.  On the 
 *	first time through (ie. cachePointer is NULL), realloc()
 *	will perform just like malloc().
 *
 */
	cachePointer = ( ConfigFileKeyValueType * ) realloc( cachePointer, 
					sizeof( *cachePointer ) * newSize);
	cacheSize = newSize;

	return;

}

char *AddToConfigCache( char *key, char *value )
{

	ConfigFileKeyValueType *cp;

	return( value );

/*
 *	See if the cache needs grown.  Will happen at least once, on
 *	the first cache reference.
 */
	if( cacheElementCount >= cacheSize )
		GrowConfigCache();

/*
 *	Get a new cachePointer.  Could have been realloced().
 */
	cp = cachePointer;

	if( GetVerboseLevel() > VERBOSE_1 ) {
		printf("AddToConfigCache: adding key %s", key );
		printf(", value %s\n", value);
	}
/*
 *	Save the element.
 */
	cp[cacheElementCount].key   = strdup(key);
	cp[cacheElementCount].value = strdup(value);


/*
 *	Increment the counter...
 */
	cacheElementCount++;

	if( GetVerboseLevel() > VERBOSE_1 ) {
		printf("AddToConfigCache:");
		printf("  cache size is: %d;", cacheSize );
		printf(" element count is %d\n", cacheElementCount );
		printf("  cachehits is %d,", cacheHits );
		printf(" cachemisses is %d\n", cacheMisses );
	}

	return( value );

}
#endif /* 0 */

/*
 *
 *	Get configuration value.  Configuration	values can be obtained 
 *	from the following sources, in the following order:
 *
 *		- Get the value from the commandline
 *		- Use the getappdefault interface to return 
 *		  value from a configuration file;  
 *		- Use hardcoded fallback resource value;
 *
 *	Return NULL on failure and a pointer to a malloced string on 
 *	success.
 *
 */
char *GetConfigValue( char *key )
{

	char			value[4096];


/*
 *	Saftey check....
 */
	if( ! key || ! *key ) {
		printf("GetConfigFileValue:  Null key\n");
		return( NULL );
	}

/*
 *
 *	Initialize the return value
 */
	*value = '\0';

#if 0
/*
 *	Check cache....
 */
	if( CheckConfigCache( key, value ) )
		if( *value )
			return( strdup( value ) );

#endif
/*
 *	Check the command line
 */
	if( GetCommandLineValue( key, value ) )
		if( *value )
			return( strdup( value ) );


/*
 *	Get the requested variable value from the configuration file
 *	or from an  environment variable (env variable takes precedence).
 */
	get_app_name ( GetConfigFile(), key, value );
	if( *value )
		return( strdup( value ) );


/*
 *	Check to see if there is a fallback resource for this value.
 */
	if( GetFallBackConfigValue( key, value ) )
		if( *value )
			return( strdup( value ) );


/*
 *	Utter failure....
 */
	return( NULL );

}


void expand_var ( char *request, char *reply )
{

/*
 *	Get requested variable value.
 */
	get_app_name ( GetConfigFile(), request, reply );

}


void ConfigInit( int *argc,char **argv )
{

	char			value[4096];
	char			*currentTime, *startTime, *endTime;
	char			*noTick;
	char			*noIcons;
	char			*noStatus;
	char			*verboseString;
	char			*memCheckString;
	char			*outputString;
	char			*width, *height;

/*
 *	Parse the commandline to hotwire in some of the important
 *	configuration parameters.  
 */
	ParseCommandLine( argc, argv );


/*
 *	Get the name of the config file, it may be specified
 *	on the commandline or hardwired in to look like it
 *	was specified on the command line.
 */
	if( ! GetCommandLineValue( CONFIGFILE, value ) ) {
		strcpy(value, "Garp_defaults");
		printf("ConfigInit:  Configuration error. Can't get ");
		printf("configfile name.  Using default: %s.\n", value);
	}

/*
 *	Set the configuration file name in the GlobalData structure.
 */
	SetConfigFile( value );

/*
 *
 *	Set some other important configuration values.
 *
 */
	if( verboseString = GetConfigValue( VERBOSE ) ) 
		SetVerboseLevel( atoi( verboseString ) );

	if( outputString = GetConfigValue( OUTPUT ) ) 
		SetOutputLevel( atoi( outputString ) );

/*
 *
 *	Get configuration values for the application clock and
 *	send those values to the clock object.
 *
 */
	endTime     = GetConfigValue( ENDTIME ) ;
	startTime   = GetConfigValue( STARTTIME );
	currentTime = GetConfigValue( CURRENTTIME );

	SetEndTime  ( endTime );
	SetStartTime( startTime );
	SetCurrentTime( currentTime );

	if( ( memCheckString = GetConfigValue( MEMCHECK ) ) )
		SetMemCheck( True );
	else
		SetMemCheck( False );

/*
 *	Decide if the application clock should be ticking.
 */
	if( ( noTick = GetConfigValue( NOTICK ) ) )
		SetNoTick( 1 );
	else
		SetNoTick( 0 );

/*
 *	Decide if GARP will start with the icon bar.
 */
	if( ( noIcons = GetConfigValue( NOICONS ) ) )
		SetNoIcons( True );
	else
		SetNoIcons( False );

/*
 *	Decide if GARP will start with the status bar.
 */
	if( ( noStatus = GetConfigValue( NOSTATUS ) ) )
		SetNoStatus( True );
	else
		SetNoStatus( False );	

/*
 *	Determine if GARP will start at a certain size
 */
	if( width = GetConfigValue( SWIDTH ) ) 
		SetStartingWidth( atoi( width ) );

	if( height = GetConfigValue( SHEIGHT ) ) 
		SetStartingHeight( atoi( height ) );
	

/*
 *	Release the memory that we don't need anymore...
 */
	if ( noTick != NULL )         Free( noTick );
	if ( verboseString != NULL )  Free( verboseString );
	if ( memCheckString != NULL ) Free( memCheckString );
	if ( outputString != NULL )   Free( outputString );
	if ( noIcons != NULL )        Free( noIcons );
	if ( noStatus != NULL )       Free( noStatus );
	if ( width != NULL )          Free( width );
	if ( height != NULL )         Free( height );

	return;

}
	
	

#ifdef TEST
main( int argc, char **argv )
{

	int savedArgc;

	savedArgc = argc;
	printf("Initial Argv\n");
	PrintArgv( argc, argv );

	ParseCommandLine( &argc, argv );

	printf("argv After parse.\n");
	PrintArgv( savedArgc, argv );

	printf("Config options\n");
	PrintCmdLineOptions( CommandLineValues );

}
#endif
