/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	displaybgimage.c
 *
 *	Background image display driver.
 *
 *	History:
 *
 *	 6/97	COMET	Original
 *	11/97	COMET	Added titleIndex to gpmap arg list.
 *	12/97	COMET	Get default text settings from keys in "Garp_defaults".
 *	12/97	COMET	Added parameters in call to gpmap
 *	 1/98	COMET	Changed NULL to '\0'
 *
 ***********************************************************************/


#include "winobj.h"
#include "wincb.h"
#include "genglobs.h"
#include "_proto.h"


int DisplayBackgroundImage( WindowObjectType *wo, MetObjectType *mot )
{
	int			iret;
	int			verbose;
	int			index;
	BooleanType		retainLUTflag, saveImageFlag;
	int			tcolor, tline;
	int			date;
	char			text [GEMPAKSTRING];
	char			lutfile[GEMPAKSTRING];
	int			font_size;
        GlobalDataObjectType	*gd;
        BackgroundImageObjectType	*bot;


	bot = (BackgroundImageObjectType *) mot->metObjectContent;

	assert ( bot );

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "DisplayBackgroundImage\n" );

	gd = GetGlobalDataObject();

/*
 *	Get the data to display for the metobject. Bail out if no image.
 */
	if ( ! GetImage ( mot ) ) return(-1); /* chiz/Unidata added return value */

/*
 *	Set the text attributes.
 */
	sprintf ( text, "%s/%s/%s/%s", gd->text_size, gd->text_font,
	                               gd->text_width, gd->text_hw_flag );
	GetDepictableValue ( "FONTSIZE", &font_size );
	SetGemText ( SURFACEOBJECT, text, "FONTSIZE", font_size );

	tcolor = 2;
	tline = 0;

/*
 *	Set the LUT file (first frame only, and user has not pressed retain
 *	button)
 */
	index = GetActivePixmapObjectIndex( GetActiveWindowObject() );
	retainLUTflag = GetDefColortableRetain (gd) ;

	if ( index == 0 && retainLUTflag == 0 )
	    strcpy ( lutfile, bot->imgCmn.cmlutf );
	else 
	    lutfile [0] = '\0';

	gpmap(	bot->imgCmn.imageData,
		NULL,
		NULL,
		NULL,
		lutfile, 
		mot->titleString,
		tcolor,
		tline,	
		text,
		bot->imgCmn.colorbar,
		mot->titleIndex,
		verbose,
		&iret );


	SetMetObjectDrawnFlag( mot, True );

/*
 *	Release the image data (always done for background images)
 */	
	if ( verbose > VERBOSE_0 )
	    printf ( "DisplayBgImage: releasing image data...\n" );
	
	if ( bot->imgCmn.imageData ) {
	    Free ( bot->imgCmn.imageData );
	    bot->imgCmn.imageData = NULL;
	 }

	return(0);

}

