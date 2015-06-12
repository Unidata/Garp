/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	fdfobj.h
 *
 *	Header file for fdf function declarations.
 *
 *	History:
 *
 *	11/96	COMET	Original copy.
 *	 4/97	COMET	Increased the size of MAXKEYS.
 *
 ***********************************************************************/

/*____________________________________________________________________
 | 
 |** Data Types:
 |
 |  KeyType     - A record that contains a keyName and a value for that 
 |		  name.
 |  KeyListType - Contains a list of KeyTypes
 |
 |** Primary Functions:
 |
 |  ReadFDF     - Read in a file and put all keys and values into a 
 |	          KeyListType that is returned. Dynamically allocates
 |                keylist.
 |
 |  PrintFDF    - Prints out each of the keys and values in a formatted 
 |	          manner.
 |
 |  PutInFDF    - Put a specific key and value into the KeyListType.
 |
 |  GetFDFByKey - Returns the KeyType for which the "key" is the keyName.
 |
 |  DestroyFDF  - Free up all memory that has been allocated for a 
 |	          KeyListType, including all of the KeyType's in the list.
 |
 |** Access Functions:
 |
 |  GetFdfKeyValue - Input keylist and key, output key value.
 _____________________________________________________________________*/


#ifndef FDFOBJ_H
#define FDFOBJ_H

#define MAXKEYS	500
#define OVERWRITE 1
#define APPEND    2

typedef struct KeyType
{
	char*		keyName;
	char*		value;
} KeyType;

typedef struct KeyListType
{
	char		fileName[81];
	char		path[512];
	KeyType		keys[MAXKEYS];
	int		numKeys;
} KeyListType;

KeyListType *GetHSGuiFdfInfo();
KeyListType *GetXSGuiFdfInfo();
KeyListType *GetTHGuiFdfInfo();
KeyListType *GetVPGuiFdfInfo();


KeyType* GetFDFByKey( char *key, KeyListType* kl );

KeyListType* ReadFDF( char *, KeyListType *, int );
KeyListType* PutInFDF(char *, char *, KeyListType *, int );

int PrintFDF(KeyListType* printOut);

char *GetFdfKeyValue ( KeyListType *, char * );
char *GetValue ( char * );
void DestroyFDF(KeyListType* kl);

#endif	/* FDFOBJ_H */
