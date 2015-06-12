/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	version.h
 *
 *	Contains definitions for the version of GARP and the compatible
 *	NAWIPS version.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 4/97	COMET	version 2.0
 *	 1/98	COMET	Removed "beta" from 2.0
 *	 2/98	COMET	Added expirationDate
 *	10/98	COMET	version 2.02
 *	 8/99	COMET	version 2.03
 *	11/99	COMET	version 2.04
 *	12/99	COMET	version 2.05
 *	12/99	COMET	version 2.1
 *
 ***********************************************************************/

 
#define GARP_version "G A R P - v2.1"

#define NAWIPS_version "NAWIPS5.4"

#ifdef GARP_Version_Global
XmString	GARP_versionXmS;
#else
extern XmString	GARP_versionXmS;
#endif


/*
 *	The expiration date of GARP. Set to some valid future date string
 *	(YYMMDD/HHNN) if this GARP binary will expire, otherwise set to "0"
 *
 *	This is probably not Y2K compliant if the exp data is past 2000
 */
/* or a valid date like: "980401/1200" */
/* #define	expirationDate "20000229/1600" */
#define	expirationDate "0"

