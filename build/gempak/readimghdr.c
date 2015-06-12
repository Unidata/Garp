/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	readimghdr.c
 *
 *	Get image header from the image file. Function readimghdr returns
 *	1 if image file header read OK, otherwise return 0 for error. 
 *
 *	NOTE: This module is tied very closely to the GEMPAK image
 *	common block (imgdef.cmn). Any changes there will most likely
 *	necessitate changes here.
 *
 *	History:
 *
 *	06/97	COMET	Original copy
 *	12/97	COMET	Added verbose printf's for showing the image
 *			source, type and raw flag (or depth)
 *	 1/98	COMET	Changed some NULLs to '\0'
 *
 ***********************************************************************/


#include "winobj.h"
#include "underscore.h"
#include "genglobs.h"



float getimgcomflt ( char *, int );
int   getimgcomint ( char *, int );
int   getimgcomstr ( char *, int *, char *, int, int );

 
int readimghdr ( MetObjectType *mot )
{

	char			tmpstr [132];
	int			i, dropflg, iret;
	int			len;
	GeoRefObjectType	*geo;	
	AnyImageObjectType	*iot;


	iot = (AnyImageObjectType *) mot->metObjectContent;
	geo = GetGeoRefObject( GetActiveWindowObject() );

/*
 *	Read the image header
 */
	gg_maps ( geo->proj, geo->garea, mot->filename, &dropflg, &iret,
		  strlen (geo->proj), strlen (geo->garea), strlen (mot->filename) );

	if ( iret != 0 ) return 0;

	im_rtbl ( &iret );

/*
 *	Get everything from the GEMPAK image common block and save in
 *	the object.
 */
	iot->imgCmn.imftyp = getimgcomint ( "imftyp", strlen("imftyp"));
	iot->imgCmn.imbank = getimgcomint ( "imbank", strlen("imbank"));
	iot->imgCmn.imdoff = getimgcomint ( "imdoff", strlen("imdoff"));
	iot->imgCmn.imldat = getimgcomint ( "imldat", strlen("imldat"));
	iot->imgCmn.imnpix = getimgcomint ( "imnpix", strlen("imnpix"));
	iot->imgCmn.imnlin = getimgcomint ( "imnlin", strlen("imnlin"));
	iot->imgCmn.imdpth = getimgcomint ( "imdpth", strlen("imdpth"));
	iot->imgCmn.rmxres = (float) getimgcomflt ( "rmxres", strlen("rmxres"));
	iot->imgCmn.rmyres = (float) getimgcomflt ( "rmyres", strlen("rmyres"));
	iot->imgCmn.imleft = getimgcomint ( "imleft", strlen("imleft"));
	iot->imgCmn.imtop  = getimgcomint ( "imtop ", strlen("imtop "));
	iot->imgCmn.imrght = getimgcomint ( "imrght", strlen("imrght"));
	iot->imgCmn.imbot  = getimgcomint ( "imbot",  strlen("imbot") );
	iot->imgCmn.rmxysc = (float) getimgcomflt ( "rmxysc", strlen("rmxysc"));
	iot->imgCmn.imbswp = getimgcomint ( "imbswp", strlen("imbswp"));
	iot->imgCmn.imnchl = getimgcomint ( "imnchl", strlen("imnchl"));
	iot->imgCmn.imprsz = getimgcomint ( "imprsz", strlen("imprsz"));
	iot->imgCmn.imdcsz = getimgcomint ( "imdcsz", strlen("imdcsz"));
	iot->imgCmn.imclsz = getimgcomint ( "imclsz", strlen("imclsz"));
	iot->imgCmn.imlvsz = getimgcomint ( "imlvsz", strlen("imlvsz"));
	iot->imgCmn.imvald = getimgcomint ( "imvald", strlen("imvald"));
	iot->imgCmn.imrdfl = getimgcomint ( "imrdfl", strlen("imrdfl"));
	iot->imgCmn.immnpx = getimgcomint ( "immnpx", strlen("immnpx"));
	iot->imgCmn.immxpx = getimgcomint ( "immxpx", strlen("immxpx"));
	iot->imgCmn.imsorc = getimgcomint ( "imsorc", strlen("imsorc"));
	iot->imgCmn.imtype = getimgcomint ( "imtype", strlen("imtype"));
	iot->imgCmn.imradf = getimgcomint ( "imradf", strlen("imradf"));
	iot->imgCmn.rmbelv = (float) getimgcomflt ( "rmbelv", strlen("rmbelv"));
	iot->imgCmn.imdate = getimgcomint ( "imdate", strlen("imdate"));
	iot->imgCmn.imtime = getimgcomint ( "imtime", strlen("imtime"));
	iot->imgCmn.imndlv = getimgcomint ( "imndlv", strlen("imndlv"));

/*
 *	Strings need to be null-terminated
 */
	for ( i = 1; i <= 256; i++ ) {
	    len = getimgcomstr ( "cmblev", &i, tmpstr, strlen("cmblev"), sizeof(tmpstr) );
	    tmpstr [len] = '\0'; 
	    strcpy (iot->imgCmn.cmblev[i-1], tmpstr);
	}
 
	len = getimgcomstr ( "cmbunt", &i, tmpstr, strlen("cmbunt"), sizeof(tmpstr) );
	tmpstr [len] = '\0'; 
	strcpy (iot->imgCmn.cmbunt, tmpstr);

	len = getimgcomstr ( "cmsorc", &i, tmpstr, strlen("cmsorc"), sizeof(tmpstr) );
	tmpstr [len] = '\0';
	strcpy (iot->imgCmn.cmsorc, tmpstr);

	len = getimgcomstr ( "cmtype", &i, tmpstr, strlen("cmtype"), sizeof(tmpstr) );
	tmpstr [len] = '\0';
	strcpy (iot->imgCmn.cmtype, tmpstr);

	len = getimgcomstr ( "cmlutf", &i, tmpstr, strlen("cmlutf"), sizeof(tmpstr) );
	tmpstr [len] = '\0';
	strcpy (iot->imgCmn.cmlutf, tmpstr);

	len = getimgcomstr ( "cmfile", &i, tmpstr, strlen("cmfile"), sizeof(tmpstr) );
	tmpstr [len] = '\0';
	strcpy (iot->imgCmn.cmfile, tmpstr);

	len = getimgcomstr ( "cmcalb", &i, tmpstr, strlen("cmcalb"), sizeof(tmpstr) );
	tmpstr [len] = '\0';
	strcpy (iot->imgCmn.cmcalb, tmpstr);

/*
 *	Pass the GEMPAK image common block down through GPLT to the XW code.
 *	(This is necessary before we read the image data.)
 */
	gsicmn ( &iret );

	if ( GetVerboseLevel() > VERBOSE_1 ) {
	    printf ("readimghdr() source: %d, ", iot->imgCmn.imsorc);
	    printf (" type: %d, ", iot->imgCmn.imtype);
	    printf (" depth (or raw flag): %d\n", iot->imgCmn.imdpth);
	}
	

	return 1;

}

