/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	stringtools.c
 *
 *	String utilities.
 *
 *	History:
 *
 *	11/96	COMET	Original copy.
 *	03/97	COMET	Fixed bug in bpad(), use s[] = '\0' instead of
 *			strcat().
 *	 3/97	COMET	Fixed an off by one bug in shortenfloatstr().
 *	10/97	COMET	Rewrote "newstrcat" and added "newstrcatln".
 *	 1/98	COMET	Changed NULL --> '\0'.
 *	 2/98	COMET	Added ReplaceChar().
 *	 7/99	UNIDATA Added return values in null test for ToUpper
 *			and ToLower functions.
 *	 11/99	COMET   himes.  Moved CheckDateTimeString() to 
 *			util/timestamp.c
 *
 ***********************************************************************/


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#include "strfunc.h"
#include "gendef.h"
#include "genglobs.h"
#include "_proto.h"


void
bpad ( char *s, int final_length, int *iret )
/*
 * Pad the end of a fixed length character with blanks.
 */
{
	int	len;
	int	i;

/*
 *	If it is a NULL, return.
 */
	if ( s == NULL ) return;

	len = final_length - strlen ( s );

	if ( len < 0 ) {
	    printf("*** Error in function bpad - Cannot pad %s\n", s );
	    *iret = len;
	    return;
	}

/*
 *	Fill in with blanks. Null-terminate the string.
 */
	for ( i= strlen ( s ); i < final_length-1; i++ ) s[i] = ' ';
	s[final_length-1] = '\0';


}


void
left_just ( char *p, int *iret )

/************************************************************************
* left_just								*
*									*
* This routine removes leading blanks from a string.			*
*									*
*									*
* Input parameters:							*
*	*p		char		String pointer			*
*									*
* Output parameters:							*
*	*p		char		Left justified string pointer	*
*	*iret		int		Return code:			*
*				          0 = normal			*
************************************************************************/
{
	int	i=0;

/*---------------------------------------------------------------------*/

	*iret = NORMAL;

/*
 *	Remove leading spaces from string.
 */

	while ( *p  && isspace ( p[i] ) != 0 ) {
	    strcpy ( p, p+1 );
	}
}


int
next_word ( char **line, char *word, int *iret )

/************************************************************************
* next_word								*
*									*
* This function left justifies a string and gets the next word in it.	*
* The returned string is incremented to the next word.			*
*									*
*									*
* Input parameters:							*
*	*line		char		string to parse			*
*									*
* Output parameters:							*
*	*word		char		first word in string		*
*	*iret		int		Return code:			*
*					  0 = normal			*
*									*
************************************************************************/
{
        char    *p, *q;

/*---------------------------------------------------------------------*/

	*iret = NORMAL;

/*
 *	Left justify the string.
 */
	left_just ( *line, iret );

/*
 *	Reload address of word each time function is called.
 */
	q = word;
	p = *line;

/*
 *	Get next word. Valid delimiters are blanks, tabs and line breaks.
 */
	while ( *p && *p != ' ' && *p != '\n' && *p != '\t' ) { 
	    *q = *p;
	    p++;
	    q++;
	}
	if ( *p == ' ' || *p == '\n' ) p++;
/*
 *	Null terminate each word.
 */
	*q = '\0';
	word = q;

/*
 *	Set beginning of line to end of this word.
 */
	*line = p;

	return 0;
}


void
ReplaceChar ( char *s, char pattern, char replace )
/*
 * Replace all occurences of one character with another in a string.
 */
{
	int	i, len;
	
	if ( s == NULL ) return;

	len = strlen ( s );
	for ( i=0; i<len; i++ )
	    if ( s[i] == pattern ) s[i] = replace;

	return;
}


int
strbrm ( char *s )
/*
 * Delete trailing blanks from a string.
 */
{
	int	len;

	len = strlen ( s );
	len--;
	while ( isspace ( *( s + len ) ) )
	    len--;

	*(s + len + 1) = '\0';

	return 0;
}


int
strndel ( char *s, int n )
/*
 * Delete n characters from the beginning of a string.
 */
{
        int     len;

        len = strlen ( s );
        if ( len < n ) return 0;

        strcpy ( s, s + n );
        return 1;
}


int
shortenfloatstr ( char *s )
/*
 * Delete trailing 0's from a float written as a string.
 */
{
	int	len;

	len = strlen ( s );
	if ( len < 1 ) return 0;

	while ( *(s + len - 1) == '0' )
	    len--;

	*(s + len) = '\0';

	return 1;
}


int
strndelend ( char *s, int n )
/*
 * Delete a given number of characters from the end of a string.
 */
{
	int	len;

	len = strlen ( s );
	if ( len < n ) return 0;

	s[len - n] = '\0';
	return 1;
}


char *
strdelafter ( char *string, char marker )
/*
 * Delete all characters after that given by the character "marker".
 * This function does not alter the initial input string.
 */
{
	char	*str, *strpos;
	int	len;
	int	i=0;

	if ( string == NULL ) return NULL;

	str = strdup (string);
	strpos = str;

	len = strlen (string);

/*
 *	Count the number of characters up to that given by marker.
 */
	while ( *str && *str != marker ) {
	    *str++;
	    i++;
	}

/*
 *	Determine number of characters to delete from end of string.
 */
	len = len - i;

/*
 *	Delete n characters from end of string.
 */
	str = strpos;
	strndelend ( str, len );

	return str;
}


char *
strsaveafter ( char *string, char marker )
/*
 * Save all characters after that given by the character "marker".
 */
{
	char	*str, *strpos, *strsave;
	int	len;
	int	i=0;

	str = strdup (string);
	strpos = str;

	len = strlen (string);

/*
 *	Count the number of characters up to that given by marker.
 */
	while ( *str && *str != marker ) {
	    *str++;
	    i++;
	}
	*str++;

	strsave = strdup ( str );

	str = strpos;
	free ( str );

	return strsave;
}


void
strdelc ( char *s, char c, int *iret )

/************************************************************************
* strdelc								*
*									*
* This function deletes all occurences of a character from a string.	*
*									*
*									*
* Input parameters:							*
*	*s		char	Input string				*
*	c		char	Character to delete			*
*									*
* Output parameters:							*
*	*s		char	Output string				*
*	*iret		int	Return code:				*
*				  0 = normal				*
*									*
************************************************************************/
{

/*---------------------------------------------------------------------*/

	*iret = NORMAL;

/*
 *	Delete all occurences of a character from a string.
 */
	while ( *s ) {
	    if ( *s == c ) {
		strcpy ( s, s+1 );
	    }
	    else {
		s++;
	    }
	}
}


char *
strsavebefore ( char *string, char marker )
/*
 * Start from the end of a string and save characters of a string up 
 * to that given by "marker".
 */
{
	char	*str, *strpos, *strsave;
	int	i, len;

	str = strdup (string);
	strpos = str;

	len = strlen ( string );

	i = len - 1;
	while ( *str && str[i] != marker ) {
	    i--;
	}
	str[i] = '\0';

	strsave = strdup ( str );

/*
 *	Clean up.
 */
	str = strpos;
	free ( str );

	return strsave;
}


char *
strdelupto ( char *string, char marker )
/*
 * Delete all characters up to and including that given by the character "marker".
 */
{
	string = strchr ( string, marker );
	return string+1;
}


char *
strdelupto2 ( char *string, char marker )
/*
 * Delete all characters up to and including that given by the character "marker".
 */
{
	char	*str, *outstr;

	if ( string == NULL ) return NULL;

	str = strchr ( string, marker );
	outstr = strdup ( str + 1 );

	return outstr;
}


char *
newstrcat ( char *s1, char *s2 )
{

	char	*p;
	int	lenstr = 0;

	if ( s1 == NULL && s2 == NULL) {
            printf ("*** newstrcat - both entries are NULL\n");
	    return ( (char *) NULL );
	}
/*
 *	Determine string length.
 */
	if ( s1 != NULL ) lenstr = strlen ( s1 );
	if ( s2 != NULL ) lenstr += strlen ( s2 );

	p = (char *) malloc ( lenstr + 1 );

	if ( s1 != NULL ) {
	    strcpy ( p, s1 );
	    if ( s2 != NULL ) strcat ( p, s2 );
	}
	else
	    if ( s2 != NULL ) strcpy ( p, s2 );

	return ( p );
}


char *
newstrcatln ( char *s1, char *s2 )
{

	char	*p;
	int	lenstr = 0;

	if ( s1 == NULL && s2 == NULL) {
            printf ("*** newstrcatln - both entries are NULL\n");
	    return ( (char *) NULL );
	}
/*
 *	Determine string length.
 */
	if ( s1 != NULL ) lenstr = strlen ( s1 );
	if ( s2 != NULL ) lenstr += strlen ( s2 );

	p = (char *) malloc ( lenstr + 2 );

	if ( s1 != NULL ) {
	    strcpy ( p, s1 );
	    if ( s2 != NULL ) {
		strcat ( p, "\n" );
		strcat ( p, s2 );
	    }
	}
	else
	    if ( s2 != NULL ) strcpy ( p, s2 );

	return ( p );
}


char *
builddirpath ( char *s1, char *s2 )
{

    char *p;
    if ( s1 == NULL || s2 == NULL) {
        printf ("*** builddirpath - passed NULL pointer\n");
	return ( (char *) NULL );
    }
    p = (char *) malloc ( strlen ( s1 ) + strlen ( s2 ) + 2 );
    strcpy ( p, s1 );
    strcat ( p, "/" );
    return ( strcat ( p, s2 ) );

}
	

char *
strsub ( char *s, char *pattern, char *replace, int *iret )

/************************************************************************
* strsub								*
*									*
* This function replace one pattern with another in a string.		*
*									*
*									*
* Input parameters:							*
*	*s		char	String to work on.			*
*	*pattern	char	String pattern to be replaced		*
*	*replace	char	String pattern to be put into "s"	*
*									*
* Output parameters:							*
*	*strsub		char	String with substitution completed	*
*	*iret		int	Return code:				*
*				  0 = normal				*
*									*
************************************************************************/
{

	char	*p, *tp;
	int	plen, rlen;
	int	first_chars;
/*---------------------------------------------------------------------*/

	*iret = NORMAL;
	p = tp = s;

	plen = strlen ( pattern );
	rlen = strlen ( replace );
	first_chars = strlen ( s );
/*
 *	Check to see if the substring to be replaced is contained within 
 *	the original string.
 */
	if ( ( p = strstr ( s, pattern ) ) == NULL ) {
	    *iret = -1;
	    return NULL;
	}

/*
 *	Count number of characters in target string before encountering 
 *	substring.
 */
	first_chars = first_chars - strlen ( p ) + rlen;

/*
 *	Temporary point space.
 */
	tp = ( char * ) calloc ( strlen ( p ), sizeof ( char ) );

	if ( tp == NULL ) {
	    *iret = -2;
	    return NULL;
	}

	strcpy ( tp, p + plen );

/*
 *	Fill in replacement string.
 */
	while ( rlen-- ) {
	    *p++ = *replace++;
	}

/*
 *	Save updated string value.
 */
	strcpy ( p, tp );
	free ( tp );

	return p - first_chars;
}

char *
int2str ( int num )
/*
 * Write an integer value to a string pointer. String must be freed by
 * user.
 */
{
	char	*str;

/*
 *	Allocate space for the longest int possible.
 */
	str = (char *) malloc ( LONG_BIT + 1 );
	sprintf ( str, "%d", num );

	return str;
}


int
str2int ( char *str )
/*
 * Write a string pointer an integer value. String must be freed by
 * user.
 */
{
	int	an_int;

	sscanf ( str, "%d", &an_int );

	return an_int;
}


float
str2float ( char *str )
/*
 * Write a string to a float.
 */
{
	float	a_float;

	sscanf ( str, "%f", &a_float );

	return a_float;
}


char *
float2str ( float num )
/*
 * Write a float value to a string pointer. String must be freed by
 * user.
 */
{
	char	*str;

/*
 *	Allocate space for the longest int possible.
 */
	str = (char *) malloc ( LONG_BIT + 1 );
	sprintf ( str, "%f", num );

	return str;
}


void
cutfloatstr ( char *str, int precision )
/*
 * Cut of trailing digits from a string which contains a floating point
 * number.
 */
{
	int	i;

/*
 * 	Make sure it is a float.
 */
	if ( strstr ( str, "." ) == NULL ) return;

/*
 *	Find the decimal point.
 */
	while ( *str && *str != '.' )
	    str++;

/*
 *	Step through digits for required precision.
 */
	str++;
	for ( i=0; i<precision; i++ ) 
	    str++;

	*str = '\0';

	return;
}


int
ToUpper ( char *string )
{
/*
 * Covert a string to uppercase.
 */
	if ( string == NULL ) return 0;

	while ( *string ) {
	    *string = toupper ( *string );
	    string++;
	}

	return 0;
}


int
ToLower ( char *string )
{
/*
 * Covert a string to lowercase.
 */
	if ( string == NULL ) return 0;

	while ( *string ) {
	    *string = tolower ( *string );
	    string++;
	}

	return 0;
}
