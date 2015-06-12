/*
** Do not change these include commands.
** All necessary header files are included in "geminc.h".
** All macros and constants are in "gemprm.h" and "bridge.h".
*/
#include "geminc.h"
#include "gemprm.h"
#include "bridge.h"

main ( argc, argv )
	int	argc;
	char	*argv[];

/************************************************************************
 * DCSHEF								*
 *									*
 * This program decodes SHEF (standard hydrometeorological exchange 	*
 * format) data and writes the output to a GEMPAK file.			*
 *									*
 *  Command line:							*
 *  dcshef [options] filename						*
 *	filename	output file name/template			*
 **									*
 * Log:									*
 * P.Bruehl/NCEP	 3/98						*	
 ***********************************************************************/

{

/*
**	Change the values of these default variables for the
**	specific decoder.
**
**	These variables are the number of expected command line
**	parameters; the program name; the packing and station tables;
**	values for the the number of additional stations and the
**	number of times; and the number of hours, prior to the
**	"current" time, to decode (real time & archived).
*/
#define NUMEXP	1
	int	nexp    = NUMEXP;
	char	*prgnam = "DCSHEF";

	char	*defprm = "shef_comet.pack";
	char	*defstn = "shef_ncdc.tbl";
	char	*dfstn2 = " ";
	int	idfadd  =  0;
	int	idfmax  = 100;
	int	ndfhr1  = 24;
	int	ndfhr2  = 24;
	int     idfwdh  = 0;
	int	circflg;

/*
**	Do not change these variables. These variables are used by all
**	decoders for getting the command line parameters.
*/
	char	parms[NUMEXP][DCMXLN], curtim[DCMXLN];
	int	num, iret, ier;

	char	gemfil[DCMXLN], stntbl[DCMXLN], stntb2[DCMXLN], prmfil[DCMXLN];
	int	iadstn, maxtim, nhours, txtflg;

/*
**	variables for getopt (3c)
*/
	int optch;
        extern char *optarg;
        extern int optind;
	char GEMTBL[133];
/*
**	log variables
 */
	char errgrp[]="DCSHEF",errmsg[256];
/*---------------------------------------------------------------------*/

/*
**	Chiz/Unidata -- Local command line options (don't let these 
**                      go to dc_init!
**	See if GEMTBL is defined on the command line
*/
	while ((optch = getopt(argc, argv, "g:v:c:b:d:t:p:s:a:m:nh")) != EOF)
	   {
           switch (optch) {
              case 'g':
                 sprintf(GEMTBL,"GEMTBL=%s\0",optarg);
                 if(putenv(GEMTBL) != 0)
                    {
		    sprintf(errmsg,"Can't set GEMTBL=%s environmental variable\0",GEMTBL);
                    dc_wclg(0,errgrp,-2,errmsg,&ier);
                    }
                 for(num=optind;num<argc;num++)
                    argv[num-2] = argv[num];
                 argc = argc - 2; optind = optind - 2;
                 break;
	      }
           }
	optind = 1;

/*
**	Initialize the output logs, set the time out and 
**	parse the command line parameters.
*/
	dc_init ( prgnam, argc, argv, nexp, parms, &num, &iret );

/*
**	Check for an initialization error.
**	On an error, exit gracefully.
*/
	if  ( iret < 0 )  {
	    sprintf ( errmsg, "Error initializing\0" );
            dc_wclg(0,errgrp,-1,errmsg,&ier);
	    dc_exit ( &iret );
	}

/*
**	Set the decoder parameters to the command line entries or
**	default values.
*
	dc_gopt ( defprm, defstn, dfstn2, idfadd, idfmax, ndfhr1, ndfhr2,
		  idfwdh, prmfil, stntbl, stntb2, &iadstn, &maxtim, curtim, &nhours,
		  &txtflg, &idfwdh, &iret );
*/
        dc_gopt ( defprm, defstn, dfstn2, idfadd, idfmax, ndfhr1, ndfhr2,
                  idfwdh, prmfil, stntbl, stntb2, &iadstn, &maxtim, curtim,
                  &nhours, &txtflg, &circflg, &idfwdh, &iret );


/*
**	The output file name must be present.
**
**	Change this section for the specific decoder.
*/
	strcpy ( gemfil, parms[0] );

/*
**	Call the decoding routine.
**
**	Change this function call, and the define command,
**	for the specific decoder.
*/
#ifdef UNDERSCORE
#define dcshcd	dcshcd_
#endif
	dcshcd ( curtim, gemfil, prmfil, stntbl, &iadstn, &maxtim,
		 &nhours, &iret, strlen(curtim),
		 strlen(gemfil), strlen(prmfil), strlen(stntbl) );

/*
**	Send a shut down message to the logs and close the log files.
*/
	dc_exit ( &iret );

}
