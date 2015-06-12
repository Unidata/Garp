/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	processfdf.c
 *
 *	Reads fdf's.
 *
 *	History:
 *
 *	11/96	COMET		Original copy
 *	 3/97	J. Cowie/COMET	Replaced call to AddNext() with strcat()
 *	 7/97	COMET		Moved FDFList allocation in "ReadFDF" so
 *				return can occur before allocation.
 *	 8/97	COMET		Rewrote ReadFDF, GetKeyName, GetValue and
 *				PutInFDF to clean up some memory leaks.
 *				PutInFDF now has a mode to toggle between
 *				appending or overwriting keys.
 *	11/97	COMET	In GetFdfKeyValue() added break, and fixed leak
 *	 1/98	COMET	Changed some NULLs to 0
 *
 ***********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "genglobs.h"
#include "_proto.h"

#include "fdfobj.h"

#define line_length 1024


char *
GetFdfKeyValue ( KeyListType *kl, char *key )
{
	char	*value = NULL;
	int	i;

/*
 *	Scan keys in key list and extract value.
 */
	for ( i=0; i< kl->numKeys; i++ ) {

	    if ( ( strcmp ( kl->keys[i].keyName, key ) == 0 ) &&
		   kl->keys[i].value != NULL &&
		   strcmp ( kl->keys[i].value, "")  != 0 ) {
                value = strdup ( kl->keys[i].value );
                break;
            }

        }

	return value;
}

char *
GetKeyName ( char *entry )
/* Returns the key to the left of the first '='. Only characters
 * greater than 32 are valid for the key name. This excludes any white
 * character, '\0', '\n', and other special characters.
 */
{
	char	*retVal;
	int	n = 0, size;


	if (entry[0] == '!' || entry[0] == '=') 
	    return NULL;

	retVal = (char * ) malloc ( line_length );

/*
 *	Add all values to the left of the equals.
 */
	while (entry[n] != '=' && entry[n] > 32 && n <= (strlen(entry))) {
	    retVal[n] = entry[n];
	    n++;
	}
/*
 *	Why this test?
 */
	if (n > strlen(entry)) {
	    Free ( retVal );
	    retVal = NULL;
	}
	else
	    retVal[n] = '\0';

/*
 *	Length is zero.
 */
	if ( !n ) {
	    Free ( retVal );
	    retVal = NULL;
	}

	return retVal;
}


char *
GetValue ( char *entry )
/* Returns the string to the left of the first '='.  
 * Special characters are escaped using the '\'. If a '\' is 
 * the last character on the line, then the next line is read, 
 * and added to the value
 */
{
	char		*retVal;
	int		n = 0, valueNum = 0, size, eqlAt = 0;
	int		inQuote = 0, bkSlash = 0, oneSpace = 1;

	if (entry[0] == '!') 
	    return NULL;

	retVal = (char * ) malloc ( line_length * sizeof(char) );

	while (entry[n] != '\n' && entry[n] != '\0' && n <= (strlen(entry))) {
	    if (eqlAt) {
/*
 *	All non-control and white space.
 */
		if (entry[n] > 32 && entry[n] != 34 && entry[n] != 92)
		    retVal[valueNum++] = entry[n];

/*
 *	and not '"' or '\'
 */
		if (oneSpace && entry[n] != 32 &&
		                entry[n] != 9 && !bkSlash && !inQuote)
		    oneSpace = 0;

		if (!oneSpace && (entry[n] == 32 || entry[n] == 9) &&
		                                   !bkSlash && !inQuote) {
			retVal[valueNum++] = ' ';
			oneSpace = 1;
		}
/*
 *	If the last char was a backslash.
 */
		if (bkSlash) {
		    retVal[valueNum++] = entry[n];  /* escape this char */
		    bkSlash = 0;
		}
		else {
		    if (inQuote && (entry[n] == 32 || entry[n] == 9))
			retVal[valueNum++] = entry[n];

		    if (entry[n] == 92) 
			bkSlash = 1;

		    if (inQuote && entry[n] == 34)  
			inQuote = 0;

		    if (!inQuote && entry[n] == 34) 
			inQuote = 1;
		}
	    }
/*
 *	Go through the array until '='.
 */
	    if (entry[n] == '=') 
		eqlAt = 1;
	    n++;
	}

	if ( !eqlAt ) {
	    Free ( retVal );
	    retVal = NULL;
	}
	else
	    retVal[valueNum++] = '\0';

	return retVal;
}


char*AddNext	(char*base, char*next)
/* This function removes the null character from 'base' and appends
 * next to base.  So if base = 'abc\0' and next = 'def\0', it would
 * return 'abcdef\0'.
 */
{
	int	n;
	char	*temp;
	if (!next) 
		return NULL;
	if (!base && next) 
		return next;
	if (base  && next) 
		temp = (char * )malloc(strlen(base) + strlen(next));

	for (n = 0; n < strlen(base) + 1; n++)
		temp[n] = base[n];
	strcat(temp, next);
	temp[strlen(temp)] = '\0';
	strcpy(base, temp);
	free(temp);
	return base;
}


KeyType*GetFDFByKey( char *key, KeyListType*kl )
/* Returns the KeyType for 'key' in 'kl' or NULL if not found */
{
	int	n;
	if (key && kl)
		for (n = 0; n < kl->numKeys; n++)
			if (!strcmp(key, kl->keys[n].keyName)) 
				return & kl->keys[n];
	return NULL;
}


int	FindKeyLoc(char*key, KeyListType*kl)
{
	int	n;

	for (n = 0; n < kl->numKeys; n++)
		if (!strcmp(kl->keys[n].keyName, key)) 
			return n;

	return 0;
}


KeyListType *
PutInFDF ( char*key, char*value, KeyListType*inputList, int mode )
{
	/* get the key type, if it's there put in the new value if not
   put in the new key */

	KeyType * tempKey;
	KeyType newKey;
	int	newPlace;

	if (!key || !value || !inputList || key[0] == 0) 
		return NULL;
/*
 *	Overwrite existing key value.
 */
	switch (mode) {
	case OVERWRITE:
	  tempKey = GetFDFByKey(key, inputList);
	  if (tempKey) {
	      newPlace = FindKeyLoc(key, inputList);
	      free(inputList->keys[newPlace].keyName);
	      free(inputList->keys[newPlace].value);
	  } 
	  else  {
	      if (inputList->numKeys > 0) 
		  inputList->numKeys += 1;
	      else 
		  inputList->numKeys = 1;

	      newPlace = (inputList->numKeys) - 1;
	  }
	  break;
/*
 *	Append key/value pair to FDF. 
 */
	case APPEND:
	  if (inputList->numKeys > 0) 
	      inputList->numKeys += 1;
	  else 
	      inputList->numKeys = 1;

	  newPlace = (inputList->numKeys) - 1;
	  break;

	default:
	  printf( "Invalid mode setting in PutInFDF\n" );
	  break;
	}

/*
	newKey.keyName = (char * )malloc(strlen(key) + 1);
	newKey.value = (char * )malloc(strlen(value) + 1);
	strcpy(newKey.keyName, key);
	strcpy(newKey.value  , value);
	strcat(newKey.keyName, "\0");
	strcat(newKey.value, "\0");
*/
	newKey.keyName = strdup ( key );
	newKey.value   = strdup ( value );
	inputList->keys[newPlace] = newKey;

	return inputList;
}


char	FirstChar(char*input)
{
	int	n = 0;
	if (!input) 
		return 0;
	while (input[n] < 33 && n <= strlen(input)) 
		n++;
	return input[n];
}


KeyListType *
ReadFDF ( char *fileName, KeyListType *validKeys, int mode )
/* This function opens 'fileName' and reads it line by line.  The
 * key is selected from the line using 'GetKeyName(inLine)'. The
 * value is selected using 'GetValue(inLine)'.  They are then put into
 * a KeyType and checked to make sure the key is in 'validKeys'.  If
 * key is a valid key then the KeyType is added to the KeyListType to be
 * returned. If 'validKeys' is NULL then all keys are entered.
 */
{
	FILE * IN;
	char	inLine[line_length], *nextVal;
	char	*tempVal=NULL, *tempName=NULL;
	char	*tempVal2=NULL;
	int	i, verbose;
	BooleanType	getNext, comment, first=TRUE;
	KeyListType * FDFList;


	verbose = GetVerboseLevel();
        if( verbose > VERBOSE_0 )
	    printf ( "ReadFDF\n" );

	if( verbose > VERBOSE_1 && fileName != NULL )
	    printf ( "  fileName = %s\n", fileName );


	if ((IN = fopen(fileName, "r")) == NULL)
	    return NULL;
/*
 *	Allocate.
 */
	FDFList = (KeyListType * )malloc(sizeof(KeyListType));
	FDFList->numKeys = 0;


	while ( fgets ( inLine, line_length, IN ) ) {
	    getNext = FALSE;
	    comment = FALSE;
/*
 *	Check for a continuation to the line at end of line. 
 */
	    if ( FirstChar(inLine) == '!' ) comment = TRUE;
	    if (strlen(inLine) > 2 && inLine[strlen(inLine)-2] == 92 && 
	        inLine[strlen(inLine)-3] != 92 && !comment )
		getNext = TRUE;
/*
 *	Read in and append line.
 */
	    if ( !comment ) {
	    if ( getNext ) {

		if ( first ) tempName = GetKeyName ( inLine );
		nextVal = GetValue ( inLine );
		tempVal = newstrcatln ( tempVal2, nextVal );
/*
		AddNext(tempVal, nextVal);
		strcat ( tempVal2, nextVal );
		PutInFDF(tempName2, tempVal2, FDFList, mode);
*/
/* PrintFDF ( FDFList ); */
		first = FALSE;
	        if ( nextVal  != NULL ) Free ( nextVal );
	    }

/*
 *	No continuation line.
 */
	    else {
/*
 *	The first line has no continuation line.
 */
		if ( first ) {
		    tempName = GetKeyName ( inLine );
		    tempVal = GetValue ( inLine );
		}
/*
 *	Last line of a continuation line.
 */
		else {
		    nextVal = GetValue ( inLine );
		    tempVal = newstrcatln ( tempVal2, nextVal );
		    if ( nextVal  != NULL ) Free ( nextVal );
		}
	    }

/*
 *	Write key/value pair to FDF.
 */
	    if ( !getNext ) {
		if (validKeys && GetFDFByKey(tempName, validKeys) &&
		        tempVal)
		    PutInFDF(tempName, tempVal, FDFList, mode);

		else if (tempName && tempVal && !validKeys)
		    PutInFDF(tempName, tempVal, FDFList, mode);

/*
 *	Clean slate.
 */
		first = TRUE;
		if ( tempName != NULL ) Free ( tempName ); tempName=NULL;
		if ( tempVal  != NULL ) Free ( tempVal );  tempVal =NULL;
		if ( tempVal2 != NULL ) Free ( tempVal2 ); tempVal2=NULL;
	    }

/*
 *	Save intermediate values in case there is a continuation line.
 */
	    if ( tempVal  != NULL ) {
		if ( tempVal2 != NULL ) Free ( tempVal2 );
		tempVal2 = strdup ( tempVal );
		Free ( tempVal ); tempVal = NULL;
	    }
	    }
	}

	fclose(IN);

	return FDFList;
}


int	PrintFDF(KeyListType*printOut)
/* Takes the KeyListType 'printOut', and prints out each one of the keys name
 * and value in a formatted manner.
 */
{
	int	n;
	char	output[256];


	if (!printOut || printOut->numKeys < 1) 
		return 0;
	printf ("FDF fileName = %s\n", printOut->fileName );
	printf ("FDF path     = %s\n", printOut->path );

	printf("|__Key_______________|______Value______________________________|\n");

	for (n = 0; n < printOut->numKeys; n++)
	    if ( printOut->keys[n].value != NULL ) {
		sprintf ( output, "%-22s%s", printOut->keys[n].keyName,
	                                     printOut->keys[n] .value);
		printf ( "%s\n", output );
	    }
	    else {
		sprintf ( output, "%-22s", printOut->keys[n].keyName );
		printf ( "%s\n", output );
	    }
	printf("________________________________________________________________\n");
	printf(" Total keys : %d\n", n);
	return 1;
}


void DestroyFDF(KeyListType*kl)
/* Free up memory that is used by 'kl' */
{
	int	n;
	int	verbose;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "DestroyFDF\n" );

	if (!kl) 
		return;

	for (n = 0; n < kl->numKeys; n++) {
		free(kl->keys[n].keyName);
		free(kl->keys[n].value);
	}
	free(kl);
}


void
CopyFdf ( KeyListType *kl, KeyListType *fdf )
/*
 * Copy a key list.
 */
{
	int		i;
	int		verbose;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "CopyFdf\n" );

	if ( !kl )
	    return;

/*
 *	Free any previously created key/value pairs.
 */
	if ( fdf->numKeys > 0 ) {

	    if( verbose > VERBOSE_1 ) printf ( "  Freeing fdf \n");

	    for ( i=0; i<fdf->numKeys; i++ ) {
		free ( fdf->keys[i].keyName );
		free ( fdf->keys[i].value );
	    }
	}

/*
 *	Copy key/value pairs.
 */
	if ( kl->fileName != NULL ) strcpy ( fdf->fileName, kl->fileName );
	else                        strcpy ( fdf->fileName, "0" );
	if ( kl->path     != NULL ) strcpy ( fdf->path, kl->path );
	else                        strcpy ( fdf->path, "0" );

	fdf->numKeys = kl->numKeys;
	for ( i=0; i<kl->numKeys; i++ ) {
	    fdf->keys[i].keyName = strdup ( kl->keys[i].keyName );
	    fdf->keys[i].value  = strdup ( kl->keys[i].value );
	}

	if( verbose > VERBOSE_1 ) {
	    printf("  Saved fdf\n" );
	    PrintFDF( fdf );
	}

}
