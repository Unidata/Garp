/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	displayradar.c
 *
 *	Radar image display driver.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 6/97	COMET	Cleaned up
 *	11/97	COMET	Added titleIndex to gpmap arg list.
 *	11/97	COMET	Write placeholder title when no data.
 *	12/97	COMET	Get default text settings from keys in "Garp_defaults".
 *	12/97	COMET	Added parameters in call to gpmap for title
 *	 1/98	COMET	Changed NULL to '\0'
 *	12/99	COMET	Save dattim in the format YYYYMMDD/HHMM.
 *
 ***********************************************************************/


#include "winobj.h"
#include "wincb.h"
#include "genglobs.h"
#include "underscore.h"
#include "_proto.h"


int DisplayRadar( WindowObjectType *wo, MetObjectType *mot )
{
	int			iret;
	int			verbose;
	int			index;
	BooleanType		retainLUTflag, saveImageFlag;
	int			tcolor, tline, oldcolor=0;
	char			text [GEMPAKSTRING];
	char			lutfile[GEMPAKSTRING];
	char			dattim[GEMPAKSTRING];
	char			level[GEMPAKSTRING];
	char			product[GEMPAKSTRING];
	char			*label="No Image";
	int			font_size;
        RadarObjectType		*rot;
        GlobalDataObjectType	*gd;


	rot = (RadarObjectType *) mot->metObjectContent;

	assert ( rot );

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "DisplayRadar\n" );

	gd = GetGlobalDataObject();

	tcolor = 2;
	tline = 0;

/*
 *	Get the data to display for the metobject. Bail out if no image.
 */
	if ( ! GetImage ( mot ) ) {
	    MakeMotTitle ( mot->titleString, NULL, NULL, NULL, NULL,
                       label, NULL );
	    gscolr ( &tcolor, &iret);
	    dscolr ( &tcolor, &oldcolor, &iret);
	    ptitle ( mot->titleString, &(mot->titleIndex) );
	    return;
	}

/*
 *	Set the text attributes.
 */
	sprintf ( text, "%s/%s/%s/%s", gd->text_size, gd->text_font,
	                               gd->text_width, gd->text_hw_flag );
	GetDepictableValue ( "FONTSIZE", &font_size );
	SetGemText ( SURFACEOBJECT, text, "FONTSIZE", font_size );

/*
 *	Set the LUT file (first frame only, and user has not pressed retain
 *	button)
 */
	index = GetActivePixmapObjectIndex( GetActiveWindowObject() );
	retainLUTflag = GetDefColortableRetain (gd) ;

	if ( index == 0 && retainLUTflag == 0 )
	    strcpy ( lutfile, rot->imgCmn.cmlutf );
	else 
	    lutfile [0] = '\0';

/*
 *	Set some additional attributes used for the title.  dattim:
 *	strip off the century part, put in yyyymmdd/hhmm form
 */
	sprintf ( dattim, "%08d/%04d",
	          rot->imgCmn.imdate, rot->imgCmn.imtime/100 );

/*
 *	Get the radar beam elevation
 */
	if ( rot->imgCmn.rmbelv != -9999. ) {
	    sprintf ( level, " %f", rot->imgCmn.rmbelv );
	    shortenfloatstr ( level );
	    strcat ( level, " Deg" );
	}
	else
	    strcpy ( level, "" );

/*
 *	Build the product name string
 */
	strcpy ( product, rot->imgCmn.cmsorc );
	strcat ( product, " " );
	strcat ( product, rot->imgCmn.cmtype );
	strcat ( product, level );

	gpmap(	rot->imgCmn.imageData,
		dattim,
		product,
		rot->imgCmn.scale,
		lutfile, 
		mot->titleString,
		tcolor,
		tline,	
		text,
		rot->imgCmn.colorbar,
		mot->titleIndex,
		verbose,
		&iret );


	SetMetObjectDrawnFlag( mot, True );

/*
 *	Release the image data if user doesn't want it saved
 */	
	saveImageFlag = GetSaveImageDataFlag ( gd);
	if ( ! saveImageFlag ) {

	    if ( verbose > VERBOSE_0 )
	    	printf ( "DisplayRadar: releasing image data...\n" );
	
	    if ( rot->imgCmn.imageData ) {
	    	Free ( rot->imgCmn.imageData );
	    	rot->imgCmn.imageData = NULL;
	    }
	}

	return(0);

}

