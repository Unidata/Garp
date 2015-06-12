/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	configfilemgmt.c
 *
 *	Config (defaults) file management functions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "genglobs.h"
#include "getapp.h"
#include "_proto.h"

void
error ( error_num, parameter, function, status )
	int	error_num;
	char	*parameter;
	char	*function;
	char	*status;

/************************************************************************
* error									*
*									*
* This function prints error messages for the get_app_name library.	*
*									*
* error ( error_num, parameter, function, status )			*
*									*
* Input parameters:							*
*	error_num	int	error number				*
*	parameter	char	parameter which occasioned error	*
*	function	char	function in which error occurred	*
*	status		char	run status of error			*
*									*
* Output parameters:							*
*	NONE								*
*									*
************************************************************************/
{

/*---------------------------------------------------------------------*/
/*        printf("error_num = %d\n", error_num);
	printf("parameter = %s\n", parameter);
        printf("function = %s\n", function);
        printf("status = %s\n", status);
*/

	printf("***\n");
	printf("*** Error # %d", error_num );
	printf(" in function (%s)\n", function);
	switch (error_num)
	{
	    case 1:
		printf("*** Could not malloc parameter: %s\n", parameter);
		break;
	    case 2:
		printf("*** Table: %s is not defined\n", parameter);
		break;
	    case 3:
		printf("*** Could not open:\n");
		printf("*** %s\n", parameter);
		break;
	    case 4:
		printf("*** Could not find the environment variable:\n");
		printf("*** %s\n", parameter);
		printf("*** which is used to find the Garp_defaults file.\n");
		break;
	    case 5:
		printf("*** Could not find configuration file:\n");
		printf("*** %s\n", parameter);
		break;
	    case 6:
		printf("\n");
		break;
	    case 7:
		printf("\n");
		break;
	    case 8:
		printf("\n");
		break;
	    case 9:
		printf("\n");
		break;
	    case 10:
		printf("\n");
		break;
	    case 11:
		printf("\n");
		break;
	    case 12:
		printf("\n");
		break;
	    case 13:
		printf("\n");
		break;
	    case 14:
		printf("\n");
		break;
	    case 15:
		printf("\n");
		break;
	    case 16:
		printf("\n");
		break;
	    case 17:
		printf("\n");
		break;
	    case 18:
		printf("\n");
		break;
	}

	if (strstr ( status, "fatal") ) {
	    printf("*** This error is fatal, program exiting ...\n");
	    printf("***\n");
	    exit(1);
	}
	if (strstr ( status, "continue") ) {
	    printf("*** This error is not fatal, program continuing ...\n");
	    printf("***\n");
	}
}

int
find_file ( file_name )
	char	*file_name;

/************************************************************************
* find_file								*
*									*
* This function checks for the existance of a file (with read access)	*
*									*
* find_file ( file_name )						*
*									*
* Input parameters:							*
*	*file_name	char		Name of input file		*
*									*
* Output parameters:							*
*	NONE								*
*									*
************************************************************************/
{
	int	exist;
	FILE	*fp;
	char	*this_function="find_file";

/*---------------------------------------------------------------------*/

	if (( fp = fopen ( file_name, "r" )) == NULL ) {
	    exist = FALSE;
	}
	else {
	    exist = TRUE;
	    fclose ( fp );
	}
	return exist;
}

void
full_name ( file_path, file_name, name )
	char	*file_path;
	char	*file_name;
	char	*name;

/************************************************************************
* full_name								*
*									*
* This function appends a file name to path name to create a full	*
* file name.								*
*									*
* full_name ( file_path, file_name, name )				*
*									*
* Input parameters:							*
*	*file_path	char		Path name for file		*
*	*file_name	char		File name			*
*									*
* Output parameters:							*
*	*name		char		Absolute file name		*
*									*
************************************************************************/
{
	int	last_char;
	char	*this_function="full_name";

/*---------------------------------------------------------------------*/

	if ( file_path == NULL ) {
	    strcpy ( name, file_name );
	    return;
	}
	last_char = strlen ( file_path ) - 1;
	strcpy ( name, file_path );
	if ( name[last_char] != '/' ) strcat ( name, "/\0");
	strcat ( name, file_name );
	return;
}

void
get_app_name(file, request, reply)
	char	*file;
	char	*request;
	char	*reply;

/************************************************************************
* get_app_name								*
*									*
* This function is a driver routine to read a configuration file and	*
* find the requested parameter value.					*
*									*
* get_app_name(file, request, reply)					*
*									*
* Input parameters:							*
*	*file		char	Name of configuration file to read	*
*	*request	char	symbol to find				*
*									*
* Output parameters:							*
*	*reply		char	symbol value				*
*									*
************************************************************************/
{
	char	full_file_name[500];
	char	*this_function="get_app_name";
	int	request_len, reply_len;
	int	exist;

/*---------------------------------------------------------------------*/

/*
 *	Find a configuration file which defines directories where
 *	table files are located.
 */
	get_configfile ( file, full_file_name );

/*
 *	Open and read this configuration file to define directory locations
 *	of table files.
 */
	request_len = strlen (request);

	get_apps_defaults ( full_file_name, request, &request_len, 
				reply, &reply_len);
}
/*
 *  2/28/96 - dave himes.  Increased some array values for from
 *  128 to 8196.  Current config file values blew out the 128 byte
 *  array, when looking at case studies.
 */

#define MAXLIN 1024                    /* dh max. length of file line */
#define TOKEN_LENGTH 128               /* max. length of token in file */
#define TOKEN_VALUE_LEN 8196           /* max. length of token value */
#define ENV_VAR_LENGTH 25              /* dflt length of env. var. */
#define RFR_OPEN "$("                  /* referback opening string */
#define RFR_CLOSE ")"                  /* referback closing string */
#define DELIM ':'                      /* delimiter character */
#define COMMENT '#'                    /* comment character */
#define QUOTE1 '\"'                    /* 1st valid quote character */
#define QUOTE2 '\''                    /* 2nd valid quote character */
#define QPHRASE1 opt_line[ilast] != QUOTE1
#define QPHRASE2 opt_line[ilast] != QUOTE2
#define NPHRASE2 isspace(opt_line[ilast]) == 0

int
get_apps_defaults ( char *opts_file, char *request,
			 int *request_len, char *reply, int *reply_len)

/************************************************************************
* get_apps_defaults.c							*
*									*
*  Function to resolve the value of a requested string.			*
*									*
* get_apps_defaults ( opts_file, request, request_len, reply, reply_len)*
*									*
* Input parameters:							*
*	*opts_file	char	File to parse				*
*	*request	char	String to search for			*
*	*request_len	int	Length of requested string 		*
*									*
* Output parameters:							*
*	*reply		char	Value of return string			*
*	*reply_len	int	Length of return string			*
*									*
*  The requested string to be resolved is supplied as the string	*
*   variable <request>, the resolved request is returned as the string	*
*   variable <reply>.							*
*									*
*  Request resolution occurs in one of three ways:			*
*									*
*   1. an environment variable matching in name to <request> is found;	*
*      <reply> is then the value of that env. variable unless the	*
*      value is null,							*
*									*
*   2. <request> is found as a match in a file that establishes		*
*      token - resource (t-r) relationships.  One of two files are	*
*      scanned; each file name is itself referenced as an environment	*
*      variable.  The first file name is held as the value of 		*
*      environment variable APPS_DEFAULTS_USER. If either that file is	*
*      not found or <request> is not resolved in that file, the second	*
*      file is read.  The second file name is held as the value of the	*
*      env. variable APPS_DEFAULTS,					*
*									*
*   3. if <request> can not be resolved, <reply> is assigned as the	*
*      null string.							*
*									*
*  Each file is scanned from top to bottom looking for the first match	*
*   between <request> and a defined token.  The syntax needed in either	*
*   file is:								*
*									*
*   <token> <delimiter> <resource>					*
*									*
*   where: <token> is defined as a string delimited by white space or	*
*           <delimiter>,						*
*          <delimiter> is the : (colon),				*
*          <resource> is any string, the value returned depends		*
*            on certain file conventions:				*
*									*
*            1. A valid t-r requires a valid token followed by a valid 	*
*               resource,						*
*            2. the t-r relationship must be contained on a single line,*
*            3. no white space needs to surround <delimiter>,		*
*            4. comments are indicated by a  #,				*
*            5. neither <token> nor <resource> can begin with a  # or :,*
*            6. a  # or a : can be embedded within <resource>,		*
*            7. <resource> can contain white space if it is bounded by	*
*               the ' or " characters,					*
*            8. blank lines are allowed in the file,			*
*            9. referbacks are indicated by $(...). The '...' is	*
*               resolved the same way any other token is, and is	*
*               substituted for the $(...) string to compose the final	*
*               resource value.						*
*           10. Multiple referbacks are allowed in <resource>, but	*
*               embedded referbacks are not allowed  (i.e. no		*
*               $($(...)) allowed).					*
*           11. First in wins.  That is, first finding of <token>	*
*               matching <request> uses that resource value, even if 	*
*               null.							*
*									*
*  A sample of a t-r file:						*
#-----------------------------------------------------------------------
#  This is a comment line; so was previous line. Blank lines are
#   intentional and are allowed in file.

ofs_level       : testcase               # this is a comment on valid t-r
ofs_reor_lvl    : test:reor              # ':' allowed in body of <resource>
ofs_inpt_grp    : "test case"            # white space allowed in <resource>

ofs_file_grp    : /home/$(ofs_level)/files # referback to prior token;
                                         # returned resource will be
                                         #  /home/testcase/files

ofs_xxx           xxx                    # invalid t-r, no delimiter
ofs_yyy         : #yyy                   # invalid t-r, no resource

#  This is comment line; so is following line
#-----------------------------------------------------------------------
*									*
**									*
* Log:									*
* JTOstrowski/HRL	11/92						*
* S. Mahlers/RTi	????	Debugged for port			*
* S. Drake/COMET	7/95	Modified and reformatted		*
************************************************************************/
{
FILE *in;
int   ifile = 0;                /* file loop counter */
int   len = 0;                  /* length of a string */
int   i = 0;                    /* miscellaneous counter */
int   ilast = 0;                /* last character position holder */
int   iphrase = 0;              /* conditional phrase-ending indicator */
unsigned int diff = 0;          /* string comparison result */
char  opt_line[MAXLIN];         /* t-r file line array */
char  inquest[TOKEN_LENGTH+1];  /* entered token string recopied */
char  token[TOKEN_LENGTH];      /* working token array */
char  resource[TOKEN_VALUE_LEN];   /* working resource array */
char  referback[TOKEN_LENGTH];  /* referback token array */
char  refer_val[TOKEN_VALUE_LEN];  /* referback value array */
char  substitute[TOKEN_VALUE_LEN]; /* expanded referback-ed resource */
char *popen;                    /* referback opening position holder */
char *pclose;                   /* referback closing position holder */
char *as_env_var;               /* returned env. var. value */

/*---------------------------------------------------------------------*/

/*  Initialize the result to NULL */

memset(resource, '\0', sizeof(resource));

/*  Place entered string into local variable and append '\0' */

   for(i=0; i<*request_len; i++) inquest[i] = request[i];
   inquest[*request_len] = '\0';

/*  Initialize output variable reply_len to zero in case early out from rtn */

*reply_len = 0;

/*  Check for the inquest found as an environment variable */

as_env_var = getenv(inquest);

if ( as_env_var )                               /* RTi  SAM */
  strcpy(resource,as_env_var);

else
  {

  /*  The resource file to be read is indicated by the value of an
      environment variable */

  /*  See if file can be opened for reading */

    if (( in = fopen(opts_file,"r")) != NULL)
      {
  /*  Read file until either match is found or EOF reached */

      while (fgets(opt_line,MAXLIN,in) != NULL)
        {
        i = 0;

  /*  Ignore blank lines (nl only) */

        if (strlen(opt_line) > 1)
          {

  /*  Only scan lines with the delimiter in them */

          if (strchr(opt_line,DELIM) != NULL)
            {
            i = 0;

  /*  Look for first non-blank character on line */
            while (i < strlen(opt_line)-1 && isspace(opt_line[i]) != 0)
              {
              i++;
              }
              if (i < strlen(opt_line))

  /*  Discard line if first character is either delimiter or comment indicator */

                if (opt_line[i] != COMMENT && opt_line[i] != DELIM)
                  {
                  ilast  = i;
                  i      = 0;
                  token[0] = '\0';

  /*  Look for token based on rules for delimiting tokens */

                  while (isprint(opt_line[ilast]) != 0  &&
                         isspace(opt_line[ilast]) == 0  &&
                         opt_line[ilast]          != DELIM)
                    {
                    token[i++] = opt_line[ilast++];
                    }
                    token[i] = '\0';

  /*  See if token on line is one to be retrieved */

                  if (strlen(inquest) == strlen(token) &&
                      strncmp(token,inquest,i) == 0)

  /*  Match found, now determine associated resource.
       Resource can not start with DELIM or COMMENT characters
        or any non-printing characters
  */
                    {
                    i = ilast;
                    resource[0] = '\0';
                    while (i < strlen(opt_line)-1     &&
                           (isspace(opt_line[i]) != 0 ||
                            opt_line[i]          == DELIM))
                      {
                      i++;
                      }

  /*
     Determine contents of resource until:
       1. End of line is reached, or
       2. White space is found for resources not quoted, or
       3. Closing matching quote character is found for quoted strings.
  */

                      if (i < strlen(opt_line))
                        if (opt_line[i] != COMMENT)
                          {
                          ilast  = i;
                          i      = 0;

  /* Check to see if resource string is quoted (single or double) */

                          if (opt_line[ilast] == QUOTE1)
                            iphrase = 1;
                          else if (opt_line[ilast] == QUOTE2)
                            iphrase = 2;
                          else
                            iphrase = 0;

  /*  Complete resource based on start character conditions */

                            switch (iphrase)
                              {
                              case 0:
                                while (isprint(opt_line[ilast]) != 0 &&
                                       NPHRASE2)
                                  resource[i++] = opt_line[ilast++];
                                break;
                              case 1:
                                ilast++;
                                while (isprint(opt_line[ilast]) != 0 &&
                                       QPHRASE1)
                                  resource[i++] = opt_line[ilast++];
                                break;
                              case 2:
                                ilast++;
                                while (isprint(opt_line[ilast]) != 0 &&
                                       QPHRASE2)
                                  resource[i++] = opt_line[ilast++];
                                break;
                              }
                            resource[i] = '\0';

  /*  Now look for any embedded referbacks in the resource string */

                            while ((popen = strstr(resource,RFR_OPEN)) != NULL)
                              {
                              memset(referback, '\0', sizeof(referback));
                              memset(refer_val, '\0', sizeof(refer_val));
                              if ((pclose = strstr(popen,RFR_CLOSE)) != NULL)
                                {
                                diff = (int)(pclose - popen) - 2;
                                strncpy(referback,popen+2,diff);
                                }
                              else
                                strcpy(referback,popen+2);
                              len = strlen(referback);
                              get_apps_defaults(opts_file,referback,&len,refer_val,reply_len);

                              memset(substitute, '\0', sizeof(substitute));
                               if (strcmp(resource, popen) )    /* RTi  SAM */
                                strncpy(substitute,resource,(popen-&resource[0]));
                              strcat(substitute,refer_val);
                              if (diff > 0)
                                strcat(substitute,pclose+1);
                              strcpy(resource,substitute);
                  }  /* end of referback expansion while-loop */
                            break;
              }     /* end of resource inquest resolution if-loop */
            }       /* end of token-been-found if-loop */
          }         /* end of token match scanning if-loop */
        }           /* end of valid t-r line if-loop */
      }             /* end of non-blank line scan if-loop */
    }               /* end of t-r file read if-loop */
      fclose(in);
    }               /* end of loop thru files named by env. vars. */
  }                 /* end of loop for not finding inquest as env. var. */
strcpy(reply,&resource[0]);
*reply_len = strlen(reply);
if( !*reply )           /* SAM, RTi */
  return 1;
else
  return 0;
}

void
get_configfile ( file, absolute_name )
	char	*file;
	char	*absolute_name;
 
/************************************************************************
* get_configfile							*
*									*
* This function parses a path defined as an environment variable	*
* and searches this path for a file.					*
*									*
* get_configfile ( file, absolute_name )				*
*									*
* Input parameters:							*
*	*file		char		Name of file to find		*
*									*
* Output parameters:							*
*	*absolute_name	char		Full path name of found file	*
*									*
************************************************************************/
{
	char    	*p;
	char    	garp_path[512];
	char    	*this_function="get_configfile";
	int     	exist;
	static  int	firstPass = 1;

/*---------------------------------------------------------------------*/

/*
 *	Check to see if the file name is an absolute or relative
 *	pathname already.  If it is, then see if it exists and return it.
 *
 *	Examples are:
 *		/home/comet/user/Garp
 *		
 *	or
 *
 *		etc/Garpdefaults
 *
 */


	if( strchr( file, '/' ) ) {
		if( find_file( file ) ) {
			strcpy( absolute_name, file );
			return;
		}
	}

	
/*
 *	Get an environment variable which defines the path for configuration
 *	files.
 */
	if ( getenv( "GARP_PATH" ) ) {

	    strcpy ( garp_path, getenv( "GARP_PATH" ) );

	} else { 

		error ( 4, "$GARP_PATH", this_function, "fatal" ); 

	}

/*
 *	Tokenize the path and search for the default file.
 */
	p = strtok ( garp_path, ":" );
	full_name ( p, file, absolute_name );
	exist = find_file ( absolute_name );
	if ( !exist ) {
	    do {
	        p = strtok ( '\0', ":" );
	        if (p) {
		    full_name ( p, file, absolute_name );
		    exist = find_file ( absolute_name );
		    if ( exist ) break;
	        }
	    } while (p);
	}

/*
 *	If configuration file could not be found in GARP_PATH, exit.
 */
	if ( !exist ) 
		error ( 5, absolute_name, this_function, "fatal" );

/*
 *	Print out which configuration file, but only once.
 */
	if( firstPass  && GetVerboseLevel() > 0 ) {
		firstPass = 0;
		printf("Using Garp Defaults file: \"%s\"\n",
				absolute_name );
	}

}
