/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	_proto.h
 *
 *	Utility prototypes.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	12/99	COMET	Include gendef.h for AbsTime definition
 *
 ***********************************************************************/

#include "gendef.h"

char * builddirpath ( char *, char * );
char * FileToDattim ( int, char * );
char * GetConfigValue ( char * );
char * GetIcon ( char * );
char * float2str ( float );
char * int2str ( int );
char * newstrcat ( char *, char * );
char * newstrcatln ( char *, char * );
char * sec2yyyymmddhhmm ( AbsTime );
char **SplitByDelimeter ( char *, char *, int * );
/*
char **StringListDup( int , char ** );
char **StringListAllocate( int );*/
#include "stringlist.h"
char * strdelafter ( char *, char );
char * strdelupto ( char *, char );
char * strdelupto2 ( char *, char );
char * strsaveafter ( char *, char );
char * strsavebefore ( char *, char );
char * strsub ( char *, char *, char *, int * );

float str2float ( char * );

int next_word ( char **, char *, int * );
int strbrm ( char * );
int strndel ( char *, int );
int strndelend ( char *, int );
int str2int ( char * );
int ToUpper ( char * );
int ToLower ( char * );

void bpad ( char *, int , int * );
void cutfloatstr ( char *, int );
void left_just ( char *, int * );
void strdelc ( char *s, char c, int *iret );
void CancelCB();

AbsTime yyyymmddhhmm2sec ( char * );

/*
 * Added prototypes
 */
int
GetKeyList ( char *keyname,
             char *labelname,
             char *delimiter,
             int *numkeys,
             char ***keys,
             char ***labels );

void InitGuiSurfaceObsDialog();

void DefaultSfcParameters();

void gprint(char *fmt, ...);

void GetFilteredModelList ( int *nmodels, char ***models, char ***labels);

void InitStatusBarPrefDialog ();

void InitTimeMatchDialog ();

BooleanType GetMemCheck();
void Free(void *);

void put_timeptr ( char *name, struct tm *timeptr, int *iret );

int ch_to_int_time ( char *date );

time_t present_time();

void get_int ( char *name, int *value, int *iret );
void put_int ( char *name, int value, int *iret );
void get_time ( char *name, AbsTime *value, int *iret);
void put_time ( char *name, AbsTime value, int *iret );
