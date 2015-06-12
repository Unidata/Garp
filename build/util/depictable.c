/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	depictable.c
 *
 *	Depictable display utilities.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/97	COMET	Added UPPERAIRPROFILEOBJECT.
 *	 6/97	COMET	Changed first symbol packing value for surface
 *			data to 0.
 *	11/97	COMET	Guarantee title string length <= GEMPAKSTRING in
 *			SetGemTitle.
 *	 1/98	COMET	Changed some NULLs to '\0'
 *	 2/98	COMET	Changed ReplaceNumber so an initial parameter value
 *			is multiplied by the current depictable scale factor.
 *
 ***********************************************************************/


#include "winobj.h"
#include "_proto.h"
#include "wincb.h"
#include "genglobs.h"


static struct {
	int	size;
	int	width;
	int	frequency;
	int	font_size;
	int	skip;
} depictable;


void
DefaultDepictableValues ( void )
{
	depictable.size = 3;
	depictable.width = 3;
	depictable.frequency = 3;
	depictable.font_size = 3;
	depictable.skip = 3;
}


void
SetDepictableValue ( char *name, int value )
{
	if      ( strcmp ( name, "SYMBOLSIZE" ) == 0 )
	    depictable.size = value;
	else if ( strcmp ( name, "SYMBOLWIDTH" ) == 0 )
	    depictable.width = value;
	else if ( strcmp ( name, "SYMBOLPACKING" ) == 0 )
	    depictable.frequency = value;
	else if ( strcmp ( name, "FONTSIZE" ) == 0 )
	    depictable.font_size = value;
	else if ( strcmp ( name, "SCALARPACKING" ) == 0 )
	    depictable.skip = value;
	else
	    printf("*** Error in SetDepictableValue - could not define %s\n",
		name );

}


void
GetDepictableValue ( char *name, int *value )
{
	if      ( strcmp ( name, "SYMBOLSIZE" ) == 0 )
	    *value = depictable.size;
	else if ( strcmp ( name, "SYMBOLWIDTH" ) == 0 )
	    *value = depictable.width;
	else if ( strcmp ( name, "SYMBOLPACKING" ) == 0 )
	    *value = depictable.frequency;
	else if ( strcmp ( name, "FONTSIZE" ) == 0 )
	    *value = depictable.font_size;
	else if ( strcmp ( name, "SCALARPACKING" ) == 0 )
	    *value = depictable.skip;
	else
	    printf("*** Error in GetDepictableValue - could not define %s\n",
		name );
}


void
GetGemScaleFactor ( int object, char *type, int number, float *factor )
/*
 *	Apply some algorithm to map integers 1-5 to a float.
 */
{

/*
 *	General parameters.
 */
	if ( strstr ( type, "FONTSIZE" ) != NULL ) {
	    if      ( number == 1 ) *factor = 0.5;
	    else if ( number == 2 ) *factor = 0.75;
	    else if ( number == 3 ) *factor = 1.0;
	    else if ( number == 4 ) *factor = 1.5;
	    else if ( number == 5 ) *factor = 2.0;

	    return;
	}

/*
 *	Parameters which must be tuned for each display type.
 */
	switch (object) {
	case SURFACEOBJECT:
	    if ( strstr ( type, "SYMBOLSIZE" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.5;
	    	else if ( number == 2 ) *factor = 0.75;
	    	else if ( number == 3 ) *factor = 1.;
	    	else if ( number == 4 ) *factor = 2.;
	    	else if ( number == 5 ) *factor = 3.;
	    }
	    else if ( strstr ( type, "SYMBOLWIDTH" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 1.;
	    	else if ( number == 2 ) *factor = 1.;
	    	else if ( number == 3 ) *factor = 1.;
	    	else if ( number == 4 ) *factor = 2.;
	    	else if ( number == 5 ) *factor = 3.;
	    }
	    else if ( strstr ( type, "SYMBOLPACKING" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.0;
	    	else if ( number == 2 ) *factor = 0.75;
	    	else if ( number == 3 ) *factor = 1.0;
	    	else if ( number == 4 ) *factor = 1.25;
	    	else if ( number == 5 ) *factor = 1.5;
	    }
	    else {
		printf ("*** Error in GetGemScaleFactor - parmeter %s is undefined\n", type);
		*factor = 1.0;
		return;
	    }
	    break;

	case UPPERAIROBJECT:
	    if      ( strstr ( type, "SYMBOLSIZE" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.5;
	    	else if ( number == 2 ) *factor = 0.75;
	    	else if ( number == 3 ) *factor = 1.;
	    	else if ( number == 4 ) *factor = 2.;
	    	else if ( number == 5 ) *factor = 3.;
	    }
	    else if ( strstr ( type, "SYMBOLWIDTH" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.5;
	    	else if ( number == 2 ) *factor = 0.75;
	    	else if ( number == 3 ) *factor = 1.;
	    	else if ( number == 4 ) *factor = 2.;
	    	else if ( number == 5 ) *factor = 3.;
	    }
	    else if ( strstr ( type, "SYMBOLPACKING" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.0;
	    	else if ( number == 2 ) *factor = 0.5;
	    	else if ( number == 3 ) *factor = 0.75;
	    	else if ( number == 4 ) *factor = 1.0;
	    	else if ( number == 5 ) *factor = 1.5;
	    }
	    else {
		printf ("*** Error in GetGemScaleFactor - parmeter %s is undefined\n", type);
		*factor = 1.0;
		return;
	    }
	    break;

	case UPPERAIRPROFILEOBJECT:
	    if      ( strstr ( type, "FONTSIZE" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.5;
	    	else if ( number == 2 ) *factor = 0.75;
	    	else if ( number == 3 ) *factor = 1.0;
	    	else if ( number == 4 ) *factor = 1.5;
	    	else if ( number == 5 ) *factor = 2.0;
	    }
	    else if ( strstr ( type, "SYMBOLSIZE" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.5;
	    	else if ( number == 2 ) *factor = 0.75;
	    	else if ( number == 3 ) *factor = 1.;
	    	else if ( number == 4 ) *factor = 2.;
	    	else if ( number == 5 ) *factor = 3.;
	    }
	    else if ( strstr ( type, "SYMBOLWIDTH" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.5;
	    	else if ( number == 2 ) *factor = 0.75;
	    	else if ( number == 3 ) *factor = 1.;
	    	else if ( number == 4 ) *factor = 2.;
	    	else if ( number == 5 ) *factor = 3.;
	    }
	    else if ( strstr ( type, "SYMBOLPACKING" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.0;
	    	else if ( number == 2 ) *factor = 0.5;
	    	else if ( number == 3 ) *factor = 0.75;
	    	else if ( number == 4 ) *factor = 1.0;
	    	else if ( number == 5 ) *factor = 1.5;
	    }
	    else {
		printf ("*** Error in GetGemScaleFactor - parmeter %s is undefined\n", type);
		*factor = 1.0;
		return;
	    }
	    break;

	case SCALARGRIDOBJECT:
	    if ( strstr ( type, "SCALARPACKING" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.0;
	    	else if ( number == 2 ) *factor = 0.0;
	    	else if ( number == 3 ) *factor = 1.0;
	    	else if ( number == 4 ) *factor = 2.0;
	    	else if ( number == 5 ) *factor = 3.0;
	    }
	    else {
		printf ("*** Error in GetGemScaleFactor - parmeter %s is undefined\n", type);
		*factor = 1.0;
	    }
	    break;

	case VECTORGRIDOBJECT:
	    if ( strstr ( type, "SYMBOLSIZE" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.5;
	    	else if ( number == 2 ) *factor = 0.75;
	    	else if ( number == 3 ) *factor = 1.0;
	    	else if ( number == 4 ) *factor = 1.25;
	    	else if ( number == 5 ) *factor = 1.5;
	    }
	    else if ( strstr ( type, "SYMBOLWIDTH" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.8;
	    	else if ( number == 2 ) *factor = 1.0;
	    	else if ( number == 3 ) *factor = 1.2;
	    	else if ( number == 4 ) *factor = 1.5;
	    	else if ( number == 5 ) *factor = 1.75;
	    }
	    else if ( strstr ( type, "SYMBOLPACKING" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.0;
	    	else if ( number == 2 ) *factor = 0.0;
	    	else if ( number == 3 ) *factor = 1.0;
	    	else if ( number == 4 ) *factor = 2.0;
	    	else if ( number == 5 ) *factor = 3.0;
	    }
	    else {
		printf ("*** Error in GetGemScaleFactor - parmeter %s is undefined\n", type);
		*factor = 1.0;
		return;
	    }
	    break;

	case XSECTIONGRIDOBJECT:
	    if ( strstr ( type, "SYMBOLSIZE" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.5;
	    	else if ( number == 2 ) *factor = 0.75;
	    	else if ( number == 3 ) *factor = 1.0;
	    	else if ( number == 4 ) *factor = 1.25;
	    	else if ( number == 5 ) *factor = 1.5;
	    }
	    else if ( strstr ( type, "SYMBOLWIDTH" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.8;
	    	else if ( number == 2 ) *factor = 1.0;
	    	else if ( number == 3 ) *factor = 1.2;
	    	else if ( number == 4 ) *factor = 1.5;
	    	else if ( number == 5 ) *factor = 1.75;
	    }
	    else if ( strstr ( type, "SYMBOLPACKING" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.0;
	    	else if ( number == 2 ) *factor = 0.0;
	    	else if ( number == 3 ) *factor = 1.0;
	    	else if ( number == 4 ) *factor = 2.0;
	    	else if ( number == 5 ) *factor = 3.0;
	    }
	    else {
		printf ("*** Error in GetGemScaleFactor - parmeter %s is undefined\n", type);
		*factor = 1.0;
		return;
	    }
	    break;

	case TIMEHEIGHTOBJECT:
	    if ( strstr ( type, "SYMBOLSIZE" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.5;
	    	else if ( number == 2 ) *factor = 0.75;
	    	else if ( number == 3 ) *factor = 1.0;
	    	else if ( number == 4 ) *factor = 1.25;
	    	else if ( number == 5 ) *factor = 1.5;
	    }
	    else if ( strstr ( type, "SYMBOLWIDTH" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.8;
	    	else if ( number == 2 ) *factor = 1.0;
	    	else if ( number == 3 ) *factor = 1.2;
	    	else if ( number == 4 ) *factor = 1.5;
	    	else if ( number == 5 ) *factor = 1.75;
	    }
	    else if ( strstr ( type, "SYMBOLPACKING" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.0;
	    	else if ( number == 2 ) *factor = 0.0;
	    	else if ( number == 3 ) *factor = 1.0;
	    	else if ( number == 4 ) *factor = 2.0;
	    	else if ( number == 5 ) *factor = 3.0;
	    }
	    else {
		printf ("*** Error in GetGemScaleFactor - parmeter %s is undefined\n", type);
		*factor = 1.0;
		return;
	    }
	    break;

	case WINDPROFILEROBJECT:
	    if ( strstr ( type, "SYMBOLSIZE" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.5;
	    	else if ( number == 2 ) *factor = 0.75;
	    	else if ( number == 3 ) *factor = 1.0;
	    	else if ( number == 4 ) *factor = 2.0;
	    	else if ( number == 5 ) *factor = 3.0;
	    }
	    else if ( strstr ( type, "SYMBOLWIDTH" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.5;
	    	else if ( number == 2 ) *factor = 0.75;
	    	else if ( number == 3 ) *factor = 1.0;
	    	else if ( number == 4 ) *factor = 1.5;
	    	else if ( number == 5 ) *factor = 2.0;
	    }
	    else {
		printf ("*** Error in GetGemScaleFactor - parmeter %s is undefined\n", type);
		*factor = 1.0;
		return;
	    }
	    break;

	case MARKEROBJECT:
	    if ( strstr ( type, "SYMBOLSIZE" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 0.5;
	    	else if ( number == 2 ) *factor = 0.75;
	    	else if ( number == 3 ) *factor = 1.0;
	    	else if ( number == 4 ) *factor = 2.0;
	    	else if ( number == 5 ) *factor = 3.0;
	    }
	    else if ( strstr ( type, "SYMBOLWIDTH" ) != NULL ) {
	    	if      ( number == 1 ) *factor = 1.;
	    	else if ( number == 2 ) *factor = 1.;
	    	else if ( number == 3 ) *factor = 1.;
	    	else if ( number == 4 ) *factor = 2.;
	    	else if ( number == 5 ) *factor = 3.;
	    }
	    else {
		printf ("*** Error in GetGemScaleFactor - parmeter %s is undefined\n", type);
		*factor = 1.0;
		return;
	    }
	    break;

	default:
	    printf ("*** Error in GetGemScaleFactor - parmeter %s is undefined\n", type);
	    *factor = 1.0;
	}
}


char *
ReplaceNumber ( char *string, int index, char *marker, float factor )
/*
 *	Find a number which follows a marker and replace it with a
 *	different number.
 */
{
	char	*savestr1, *savestr2, *savestr3, *savestr4;
	char	*anum, *retstr;
	float	afloat;

	retstr = ( char * ) malloc ( GEMPAKSTRING );
	strcpy ( retstr, string );
/*
 *	Check for a marker in string.
 */
	if ( strstr ( string, marker ) == NULL ) return retstr;

/*
 *	Replace "size" value with current settting.
 */
        if ( index == 1 ) {
	    savestr1 = strsaveafter ( string, ':' );
	    savestr2 = strsaveafter ( savestr1, ':' );
	    savestr3 = strdelafter  ( savestr1, ':' );
	    savestr4 = strdelafter  ( string, ':' );
/*
 *	Multiply base by scale factor.
 */
	    if ( savestr3 != NULL ) {
	    	afloat = str2float ( savestr3 );
	    	afloat *= factor;
	    	anum = float2str ( afloat );	
	    	cutfloatstr ( anum, 1 );
	    
            	strcpy ( retstr, savestr4 );
            	strcat ( retstr, ":" );
            	strcat ( retstr, anum );
	    	strcat ( retstr, ":" );
	    	strcat ( retstr, savestr2 );
		Free ( anum );
	    }

	    Free ( savestr1 );
	    Free ( savestr2 );
	    Free ( savestr3 );
	    Free ( savestr4 );
        }

/*
 *	Replace "width" value with current settting.
 */

	else if ( index == 2 ) {
/*
 *	Fields for which thickening is not appropriate.
 */
	    if ( strstr ( string, "SKYC" ) != NULL ) return retstr;
	    if ( strstr ( string, "WSYM" ) != NULL ) return retstr;

	    savestr1 = strsavebefore ( string, ':' );
	    savestr2 = strsaveafter  ( string, ':' );
	    savestr3 = strsaveafter  ( savestr2, ':' );
/*
 *	Multiply base by scale factor.
 */
	    if ( savestr3 != NULL ) {
	    	afloat = str2float ( savestr3 );
	    	afloat *= factor;
	    	anum = float2str ( afloat );
	    	cutfloatstr ( anum, 1 );

	    	strcpy ( retstr, savestr1 );
	    	strcat ( retstr, ":" );
	    	strcat ( retstr, anum );
	    	Free ( anum );
	    }

	    Free ( savestr1 );
	    Free ( savestr2 );
	    Free ( savestr3 );
	}
	    
	return retstr;
}


void
SetGemParmVal ( int object, char *string, char *type, int number )
/*
 *	Replace all occurences of a number following a ":" with size.
 */
{
	char	*buf, *buf_save, *substr;
	char	str[20];
	char	*token;
	char	*tokensep = ";";
	char	*anum;

	float	factor;
	int	index, iret;
	int	verbose;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "SetGemParmVal\n" );

/*
 *	Define index in parameter string.
 */
	if      ( strcmp ( type, "SYMBOLSIZE" ) == 0 )	index = 1;
	else if ( strcmp ( type, "SYMBOLWIDTH" ) == 0 )	index = 2;
        else {
           printf("*** Error in SetGemParmVal - parameter type is undefined\n" );
           return;
        }
	
/*
 *	Adjust integer ranges 1-5 to a float.
 */
	GetGemScaleFactor ( object, type, number, &factor );
	anum = float2str ( factor );

/*
 *	Cut string length so final parm string will fit in GEMPAK 
 *	length constriants.
 */
	cutfloatstr ( anum, 1 );

/*
 *	Replace strings.
 */
	buf = strdup ( string );
	buf_save = buf;

	*string =  '\0';

	token = strtok ( buf, tokensep );
	while ( token != NULL ) {
	    strcpy ( str, token );
/*
 *	    Replace every number following the first ":" .
 */
	    substr = ReplaceNumber ( str, index, ":", factor );
	    strcat ( string, substr );
	    strcat ( string, ";" );
	    token = strtok ( NULL, tokensep );
	    free ( substr );
	}
	buf = buf_save;
	free ( buf );
	free ( anum );

	if( verbose > VERBOSE_1 )
	    printf("  string = %s\n", string );
	return;
}


void
SetGemText ( int object, char *string, char *type, int value )
/*
 * Adjust font size of a GEMPAK text string.
 */
{
	char	*savestr1, *savestr2, *savestr3;
	float	f1, factor;
	int	verbose;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "SetGemText\n" );

/*
 *	Get scale factor for text.
 */
	GetGemScaleFactor ( object, type, value, &factor );

/*
 *	Save input string in halves.
 */
	savestr1 = strdelafter  ( string, '/' );
	savestr2 = strsaveafter ( string, '/' );

	f1 = str2float ( savestr1 );
	f1 = f1 * factor;

	savestr3 = float2str ( f1 );

	cutfloatstr ( savestr3, 1 );
	strcpy ( string, savestr3 );
	strcat ( string, "/" );
	strcat ( string, savestr2 );

	free ( savestr1 );
	free ( savestr2 );
	free ( savestr3 );

	if( verbose > VERBOSE_1 )
	    printf("  string = %s\n", string );
	return;
}


SetGemFloatVar ( int object, char *string, char *type, int value )
/*
 *	Multiply a GEMPAK variable by a scale factor.
 */
{
	char	*str;
	float	f1, factor;
	int	verbose;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "SetGemFloatVar\n" );
/*
 *	Look up scale factor for radio button integer ranges 1-5.
 */
	GetGemScaleFactor ( object, type, value, &factor );

	f1 = str2float ( string );
	f1 = f1 * factor;

	str = float2str ( f1 );

	strcpy ( string, str );
	free ( str );

	cutfloatstr ( string, 1 );
	if( verbose > VERBOSE_1 )
	    printf("  string = %s\n", string );
	return;
}


SetGemIntVar ( int object, char *string, char *type, int value )
/*
 *	Multiply a GEMPAK variable by a scale factor.
 */
{
	char	*str;
	float	f1, factor;
	int	i1, verbose;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "SetGemIntVar\n" );

/*
 *	Look up scale factor for radio button integer ranges 1-5.
 */
	GetGemScaleFactor ( object, type, value, &factor );

	f1 = str2float ( string );
	f1 = f1 * factor;

	str = int2str ( (int) f1 );

	strcpy ( string, str );
	free ( str );

	if( verbose > VERBOSE_1 )
	    printf("  string = %s\n", string );
	return;
}


/*
 *	SetGemTitle  -  Sets a GEMPAK title string. This
 *	routine changes the title string to the form:
 *
 *	color_number/line_number/text
 *
 *	argument "object" is not used at present
 */


void
SetGemTitle ( char *color, int titleIndex, char *title )
{
	char			temptitle [FILENAMESTRING];
	int			tcolor, i;

/*
 *	The first number (digit) in the input color string will
 *	be used for the title color
 */
	for ( i = 0; i < strlen (color); i++ )
	    if ( isdigit ( color [i] )) {
	        tcolor = atoi ( &color [i] );
	        break;
	    }

/*
 *	Safety check
 */
	if ( tcolor < 1 ) tcolor = 1; 
 
/*
 *	Set the current title line position
 */

/*
 *	Make the final title string
 */
	strcpy ( temptitle, "");
	sprintf ( temptitle, "%d/%d/%s", tcolor, titleIndex, title);
	strcpy ( title, temptitle );

/*
 *	Check for overflow in GEMPAK.
 */
	if ( strlen (title) > GEMPAKSTRING )
	    title[GEMPAKSTRING-1] = '\0';
}

