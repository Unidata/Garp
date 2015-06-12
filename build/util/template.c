/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	timeobj.c
 *
 *	Time object.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 8/97	COMET	Created a template object to store templates.
 *	 8/97	COMET	Removed GetFileTemplate.
 *	 1/98	COMET	Changed some NULLs
 *	 7/99	UNIDATA Minor bug fixes for 4-digit year.
 *	12/99	COMET	Added function call tm2yyyymmddhhmm() in put year 
 *			in YYYY.
 *
 ***********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "template.h"
#include "_proto.h"
#include "winobj.h"

#include <time.h>
#include "genglobs.h"
#include "gendef.h"
#include "utils.h"


typedef struct {
	int	metObjectType;
	int	foundKey;
	char	*templateKey;
	char	*templateValue;
} TemplateMappingType;
	

#define MAXMETOBJS                      512

#ifndef NORMAL
#define NORMAL          1
#endif

static struct tm        CurrentApplicationTime;
static struct tm        *CurrentApplicationTimePtr = &CurrentApplicationTime;


char * TemplateTime ( char *, char * );
char * tm2yyyymmddhhmm ( struct tm * );
void template_time ( char *, char *, char * );
char * GetDataTypeTemplate ( int );
void SetDataTypeTemplate ();

void get_timeptr ( char *, struct tm *, int * );


static TemplateObjectType	tot;



TemplateObjectType *GetTemplateObject()
{
	return( &tot );
}


char *
TemplateTime ( char *string, char *template )
{
	char	*dattim;

	dattim = (char *) malloc (sizeof(char) * 200 );
/*
 *
 */
	template_time ( string, template, dattim );

	return dattim;
}

char *
GetDataTypeTemplate ( int obType )
{
	TemplateObjectType	*tot;
	char			*template;
	int			verbose;

	tot = GetTemplateObject();


	verbose = GetVerboseLevel();

	if ( verbose > VERBOSE_0 )
		printf ( "GetDataTypeTemplate\n" );

/*
 *	Get date/time template for this data type.
 */
	template = NULL;
	if ( tot->list[obType] != NULL )
	    template = strdup ( tot->list[obType] );

	return template;
}


void
SetDataTypeTemplate ()
{
/*
 * Initialize the file template object.
 */
	TemplateObjectType	*tot;
	char			**list;
	int			i, size;


	tot = GetTemplateObject();

/*
 *	Allocate space for list.
 */
	list = StringListAllocate ( MAXMETOBJS );
	for ( i=0; i<MAXMETOBJS; i++ )
	    list[i] = NULL;

/*
 *	Read in template keys from Garp_defaults.
 */
	size = 0;
	list[SURFACEOBJECT]          = GetConfigValue ( "surfaceT" );
	size++;
	list[RADAROBJECT]            = GetConfigValue ( "nidsT" );
	size++;
	list[SATOBJECT]              = GetConfigValue ( "satT" );
	size++;
	list[SCALARGRIDOBJECT]       = GetConfigValue ( "gridsT" );
	size++;
	list[VECTORGRIDOBJECT]       = GetConfigValue ( "gridsT" );
	size++;
	list[NOWRADOBJECT]           = GetConfigValue ( "nowradT" );
	size++;
	list[UPPERAIROBJECT]         = GetConfigValue ( "upperairT" );
	size++;
	list[XSECTIONGRIDOBJECT]     = GetConfigValue ( "gridsT" );
	size++;
	list[WINDPROFILEROBJECT]     = GetConfigValue ( "profilerT" );
	size++;
	list[SCALARGRIDFILLOBJECT]   = GetConfigValue ( "gridsT" );
	size++;
	list[TIMEHEIGHTOBJECT]       = GetConfigValue ( "gridsT" );
	size++;
	list[VERTICALPROFILEOBJECT]  = GetConfigValue ( "gridsT" );
	size++;
	list[UPPERAIRPROFILEOBJECT]  = GetConfigValue ( "gridsT" );
	size++;
	list[XSECTIONGRIDFILLOBJECT] = GetConfigValue ( "gridsT" );
	size++;

	tot->list = list;
	tot->size = size;
}


/*
 *  Private cache of template key to template values.
 */
static TemplateMappingType templateMapping[] = {
	RADAROBJECT,      0, "nidsT",    NULL,
	NOWRADOBJECT,     0, "nowradT",  NULL,
	SATOBJECT,        0, "satT",     NULL,
	SURFACEOBJECT,    0, "surfaceT", NULL,
	SCALARGRIDOBJECT, 0, "gridsT",   NULL,
	VECTORGRIDOBJECT, 0, "gridsT",   NULL,
	(int)NULL,        0, NULL,       NULL,
};
	
	
/*
 *
 *  Maintain a cache of template key to key-value mappings, based
 *  on the object type (vs. the filename).  This will avoid having
 *  to go to the configuration file constantly.
 *
 */
static char *SearchForTemplate( int metObjectType )
{
	TemplateMappingType	*tm = templateMapping ;

	while( tm->templateKey ) {

		if( metObjectType != tm->metObjectType ) {
			tm++;
			continue;
		}

		if( tm->foundKey )
			break;

		tm->templateValue = GetConfigValue( tm->templateKey );

		tm->foundKey++;

		break;

	}
	
	return( tm->templateValue );

}

			
/*
 * Get file template for a given file type.
 */
void
GetFileTemplateByType ( int metObjectType, char *template )
{
	char			*appfile;
	char			*tp;



/*
 *	Initialized the output paramater
 */
	*template = '\0';

	tp = SearchForTemplate( metObjectType );

/*
 *	Check the return value..
 */
	if( ! tp ) {
	    printf("*** Could not find template for objecType %d\n", 
			metObjectType );
	    return;
	}


/*
 *
 *	We actually got something and copy the string into template.  
 *
 */
	strcpy( template, tp );

	return;
}


void
detok ( const char *file, char ***list, const char *tokensep )
{
	char	token[2048];
	char	*tokenPtr;
	char	*p;
	char	**listp;

/*
 *	Allocate space for pointer list.
 */
	listp = *list = (char **) malloc (MAX_LIST * sizeof( char * ) );

/*
 *
 *	Make a copy of the pathname that needs to be parsed.
 *
 */
	strcpy( token, file );
	tokenPtr = token;
	
/*
 *
 *	Parse the pathname into it's components....
 *
 */
	tokenPtr = strtok( tokenPtr, tokensep );
	while ( tokenPtr != NULL ) {

	    *listp = strdup( tokenPtr );

	    listp++;

	    tokenPtr = strtok ( NULL, tokensep );
	}

	*listp = (char *) NULL;

}

void
parse_template ( char *file, char *template, struct tm *timeptr )
{
	char	*token;
	char	*p, *fp;
	int	year, month;
	char	*token_sep_end = ")";
						/* Examples	*/
	char	*token_sep[] = {"YYYY",		/* 1995		*/
		                "YY"  , 	/* 95		*/
		                "MMM" ,		/* JUN		*/
		                "MM"  ,		/* 06		*/
		                "DDD" ,		/* 211		*/
		                "DD"  ,		/* 31		*/
		                "D"   ,         /* 1            */
		                "HH"  ,		/* 12		*/
		                "NN"  ,		/* 30		*/
		                "SS"  ,		/* 30		*/
				"" };		/* end		*/
	int	i, j, len, ibuf;

	if ( *template != '@' ) {
	    printf("*** parse_template: Error parsing template.\n");
	    exit ( 0 );
	}

/*
 *	Initialize pointer, save address.
 */
	token = strdup ( template );
	p = token;

/*
 *	Strip off tags delineating time field.
 */
	*token++; *token++;
	token = strtok ( token, token_sep_end );
	len = strlen ( token );

/*
 *	Write date/time stamp to time structure.
 */
	fp = file;	
/*
 *	    Year in YYYY.
 */
	    if ( token && strncmp ( token, token_sep[0], 4 ) == 0 ) {
		sscanf ( file, "%04d", &ibuf );
		timeptr->tm_year = ibuf - 1900;
		*token++; *token++; *token++; *token++;
		*file++ ; *file++ ; *file++ ; *file++ ;
	    }
/*
 *	    Year in YY.
 */
	    if ( token && strncmp ( token, token_sep[1], 2 ) == 0 ) {
		sscanf ( file, "%02d", &year );
		timeptr->tm_year = year;
		if ( year < 50 ) timeptr->tm_year = year + 100;
		*token++; *token++;
		*file++ ; *file++ ;
	    }
/*
 *	    Month of year in MMM.
 */
/*
	    if ( token && strncmp ( token, token_sep[2], 3 ) == 0 ) {
		get_month ( &timeptr->tm_mon, str ); 
		sscanf ( file, "%3c", str );
		*token++; *token++; *token++;
		*file++ ; *file++ ; *file++ ;
	    }
*/
/*
 *	    Month of year in MM.
 */

	    if ( token && strncmp ( token, token_sep[3], 2 ) == 0 ) {
		sscanf ( file, "%02d", &month );
		timeptr->tm_mon = month-1;
		*token++; *token++;
		*file++ ; *file++ ;
	    }
/*
 *	    Day of year in DDD.
 */
	    if ( token && strncmp ( token, token_sep[4], 3 ) == 0 ) {
		sscanf ( file, "%03d", &timeptr->tm_yday );
		*token++; *token++; *token++;
		*file++ ; *file++ ; *file++ ;
	    }
/*
 *	    Day of month in DD.
 */
	    if ( token && strncmp ( token, token_sep[5], 2 ) == 0 ) {
		sscanf ( file, "%02d", &timeptr->tm_mday );
		*token++; *token++;
		*file++ ; *file++ ;
	    }
/*
 *	    Day of week in D.
 */
	    if ( token && strncmp ( token, token_sep[6], 1 ) == 0 ) {
		sscanf ( file, "%02d", &timeptr->tm_wday );
		*token++;
		*file++ ;
	    }
/*
 *	    Hour in HH.
 */
	    if ( token && strncmp ( token, token_sep[7], 2 ) == 0 ) {
		sscanf ( file, "%02d", &timeptr->tm_hour );
		*token++; *token++;
		*file++ ; *file++ ;
	    }
/*
 *	    Minute in NN.
 */
	    if ( token && strncmp ( token, token_sep[8], 2 ) == 0 ) {
		sscanf ( file, "%02d", &timeptr->tm_min );
		*token++; *token++;
		*file++ ; *file++ ;
	    }
	file = fp;
	token = p;
	free ( token );
/*
 *	Overwrite characters which have been parsed.
 */

	for ( i=0; i<len+3; i++ ) {
	    for ( j=0; j<strlen( template ); j++ ) {
		template[j] = template[j+1];
	    }
	    template[j] = '\0';
	}

	for ( i=0; i<len; i++ ) {
	    for ( j=0; j<strlen( file ); j++ ) {
		file[j] = file[j+1];
	    }
	    file[j] = '\0';
	}

}


void
expand_time ( char *file, char *template, struct tm *timeptr )
{
	char		mark;
	int		i, done;

	if( GetVerboseLevel() > VERBOSE_1 )
	    printf ( "expand_time\n" );

/*
 *	Test for wildcard character '*' before '@' is reached.
 */
/*
	if( GetVerboseLevel() > VERBOSE_1 ) {
		printf("  file is %s, template is %s\n",
			*file ? file : "", *template ? template : "" );
	}
*/

	done = False; mark = '\0';
	while ( !done &&  *template ) {
	    if      ( *template == '@' ) {
		done = True;
	/*	parse_template ( file, template, timeptr ); */
	    }
	    else if ( *template == '*' ) {
		done = True;
		for ( i=0; i<strlen (template) - 1; i++ )
		    template[i] = template[i+1];
		template[i] = '\0';
		mark = *template;
	    }
	    else {
/*
 *		Overwrite first character.
 */
		for ( i=0; i<strlen (template) - 1; i++ )
		    template[i] = template[i+1];
		template[i] = '\0';

		for ( i=0; i<strlen (file) - 1; i++ )
		    file[i] = file[i+1];
		file[i] = '\0';

	    }
	}

/*
 *	A '*' has been found. Look at next character to remap.
 */
	while ( mark &&  *file ) {
	    if ( *file == mark ) mark = '\0';
	    else
		for ( i=0; i<strlen (file) - 1; i++ )
		    file[i] = file[i+1];
		file[i] = '\0';
	}
/*
 *	Parse date/time when leading characters have been striped.
 */
	if ( *template == '@' ) parse_template ( file, template, timeptr );

	
/*
 *	Call your bad self.
 */
	if ( *file &&  *template )
	    expand_time ( file, template, timeptr );

}


BooleanType
CheckTemplate ( char *file, char *template )
/*
 * Count the number of '/' in a file and a template. Return true if they
 * are the same.
 */
{
	int		i;
	int		cf=0, ct=0;
	BooleanType	check;


	for ( i=0; i<strlen ( file ); i++ )
	    if ( file[i] == '/' ) cf++;

	for ( i=0; i<strlen ( template ); i++ )
	    if ( template[i] == '/' ) ct++;

	check = FALSE;
	if ( cf == ct ) check = TRUE;

	return check;
}


void
template_time ( char *file, char *template, char *dattim )
{
	struct tm	time_struct;
	struct tm	*timeptr = &time_struct;

	char		**argv_file;
	char		**argv_template;
	char		**fp, **tp;
	char		*thisDattim;
	int		argc = 0;
	int		iret;

	if( GetVerboseLevel() > VERBOSE_0 )
            printf("template_time\n" );

/*
 *	Check for valid input.
 */
	if ( strlen ( template ) <= 0 ) {
	    strcpy ( dattim, "" );
	    return;
	}
/*
 *	Initialize time pointer to current time.
 */
	get_timeptr ( "timeptr", timeptr, &iret );
			
/*
 *	Check file against template to insure they have the same number
 *	of subdirectories.
 */
	if ( !CheckTemplate ( file, template ) ) {
	    printf( "*** File: %s\n does not match\n", file );
	    printf( "    Template: %s\n", template );
	    return;
	}

/*
 *	Break directories into a list.
 */
	detok ( file, &argv_file, "/" );
	detok ( template, &argv_template, "/" );
	fp = argv_file;
	tp = argv_template;

/*
 *	Step through full path name defining time as you go.
 */
	while ( *argv_file && *argv_template ) {
	    
/*
 *	    Expand date/time string.
 */
	    if ( strchr ( *argv_template, '@' ) != NULL )
		expand_time ( *argv_file, *argv_template, timeptr );

	    *argv_file++; *argv_template++;
	    argc++;

	}

	argv_file = fp;
	argv_template = tp;
	StringListFree ( argc, argv_file );
	StringListFree ( argc, argv_template );

/*
 *	If the template does not define minutes, set to zero.
 */
	if ( !strstr ( template, "NN" ) ) timeptr->tm_min = 0;

/*
 *	Write current time to time string.
 */
/*
	sprintf ( dattim, "%.2d%.2d%.2d/%.2d%.2d",
			timeptr->tm_year,
			timeptr->tm_mon+1,
			timeptr->tm_mday,
			timeptr->tm_hour,
			timeptr->tm_min );
*/
	thisDattim = tm2yyyymmddhhmm ( timeptr );
	if (thisDattim) {
	    strcpy (dattim, thisDattim );
	    Free (thisDattim);
	}

	if( GetVerboseLevel() > VERBOSE_1 )
		printf("  dattim = %s\n", dattim );
}

