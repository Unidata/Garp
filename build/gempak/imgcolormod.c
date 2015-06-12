/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	imgcolormod.c
 *
 *	Image color manipulation routines.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 6/97	COMET	Changed imgobj.h to enhance.h
 *	12/97	COMET	Added a color refresh routine, fixed small bug,
 *			keep state of roll/fade/invert better.
 *	11/00	Unidata	changes colindex from ulong to uint (OSF problems)
 *	 4/01	Unidata	changed colindex from uint to int (prototype)
 *	06/02	Penn State - A. Person
 *			Updated to support 16- and 24-bit graphics
 *
 ***********************************************************************/
#include "stdio.h"

#include "genglobs.h"
#include "underscore.h"
#include "winobj.h"
#include "wincb.h"
#include "enhance.h"

#define  MAXCVAL  65535

int		ncolors = 0;
int		current_cbank = 0;
/*unsigned long	colindex [256];*/
Pixel	colindex [256];
Colormap	cmap;
Display		*display;
XColor		base_colors [256];
XColor		new_colors [256];

static int		got_base_colors = 0;
static int		fade = 50;
static int		roll = 0;
static int		invert = 0;

static int xdpth = -1;


/*
 * GetBaseImageColors - saves the current/default state of the colormap
 * 		   so that we can interact with it later. Set colors
 *		   in base_colors (and new_colors to stay synchronized).
 */

void GetBaseImageColors ( WindowObjectType *wo, int bank )
{

	int		i, iret;
	int		verbose;


	verbose = GetVerboseLevel();
	
	if( verbose > VERBOSE_0 )
		printf("Getting new base colors for cbank %d\n",bank);
	
	xqclrs ( &bank, &ncolors, colindex, &iret );

	if ( iret != 0 ) return;

	display = wo->display;
	cmap = DefaultColormap( display, DefaultScreen(display));

	if ( xdpth == -1 )
	   xdpth = DefaultDepth( display, DefaultScreen(display));
	    
	for ( i = 0; i < ncolors; i++ ) {
	    base_colors[i].pixel = colindex [i];
	    base_colors[i].flags = DoRed | DoBlue | DoGreen;
	}
	    
	if( xdpth == 8 ) {
	    XQueryColors( display, cmap, base_colors, ncolors );

	    for ( i = 0; i < ncolors; i++ ) {
	        new_colors[i].pixel = base_colors[i].pixel;
	        new_colors[i].flags = base_colors[i].flags;
	        new_colors[i].red   = base_colors[i].red;
	        new_colors[i].green = base_colors[i].green;
	        new_colors[i].blue  = base_colors[i].blue;
	    }
        }
	else {
	    int red[256], green[256], blue[256];
	    xqcmps(&bank, &ncolors, red, green, blue, &iret);
	    for(i=0; i<ncolors; i++) {
	       new_colors[i].pixel = base_colors[i].pixel; /* this is colindex[i] */
	       new_colors[i].flags =  base_colors[i].flags;
	       new_colors[i].red = base_colors[i].red = red[i] * 256;
	       new_colors[i].green = base_colors[i].green =  green[i] * 256;
	       new_colors[i].blue = base_colors[i].blue = blue[i] * 256;
	    }
        }

/*
 *	Set flags
 */
	got_base_colors = 1;
	current_cbank = bank;
	
}


void RefreshImageColorMap ( )
{
	int i, icols[256], red[256], green[256], blue[256], iret;

	if ( ! got_base_colors ) return;

	if ( xdpth == -1 )
	   xdpth = DefaultDepth ( display, DefaultScreen(display) );

	if( xdpth == 8 ) 
	    XStoreColors( display, cmap, new_colors, ncolors);
	else
	    {
	    for ( i = 0; i < ncolors; i++)
	       {
	       red[i] = new_colors[i].red / 256;
	       green[i] = new_colors[i].green / 256;
	       blue[i] = new_colors[i].blue / 256;
	       icols[i] = i;
	       }
	    gsbrgb(&current_cbank, &ncolors, icols, red, green, blue, &iret);
	    }

}


void SetBaseImageColorsFlag ( BooleanType set )
{

	got_base_colors = (int) set;

}


void SetNewImageColors( WindowObjectType *wo, int cbank )
{

	int		i, j, tmp_pixel;
	int		index, shift, verbose;
	int		new_red, new_green, new_blue;
	float		scalef;


	if ( !got_base_colors || cbank != current_cbank )
		GetBaseImageColors ( wo, cbank );

	verbose = GetVerboseLevel();
	
	if( verbose > VERBOSE_0 ) {
		printf("SetNewImageColors(), cbank: %d",cbank);
		printf(", fade: %d",fade);
		printf(", roll: %d",roll);
		printf(", invert: %d\n",invert);
	}

/*
 *	Start with the base colors. 
 */
	for ( i = 0; i < ncolors; i++ )
	    new_colors[i].pixel = base_colors[i].pixel;

/*
 *	Do the roll first. "roll" should be 0 - 100 !!
 */
	shift = (int) roll * ( (float) ncolors / 100. ) + .5;

	for ( i = 0; i < ncolors; i++ ) {
	    index = i - shift;
	    index = (index < 0) ? ncolors + index : index;
	    new_colors[i].red   = base_colors[index].red;
	    new_colors[i].green = base_colors[index].green;
	    new_colors[i].blue  = base_colors[index].blue;      
	    if ( xdpth > 8 )
	       new_colors[i].pixel = base_colors[index].pixel;
	}

/*
 *	Fade the colors, (rgb components) based on "fade" value (1 - 100).
 */	
	scalef = (float) fade / 50.;

	for ( i = 0; i < ncolors; i++ ) {
	    new_red   = new_colors[i].red   * scalef;
	    new_green = new_colors[i].green * scalef;
	    new_blue  = new_colors[i].blue  * scalef;

	    new_colors[i].red   = (new_red   <= MAXCVAL) ? new_red   : MAXCVAL;
	    new_colors[i].green = (new_green <= MAXCVAL) ? new_green : MAXCVAL;
	    new_colors[i].blue  = (new_blue  <= MAXCVAL) ? new_blue  : MAXCVAL;
	    new_colors[i].flags = DoRed | DoBlue | DoGreen; 

	    if(xdpth > 8) /* get the pixel value for the new color */
		XAllocColor ( display, cmap, &(new_colors[i]));
	}

/*
 *	Invert the colors if necessary
 */
	if ( invert ) {
	    for ( i = 0, j = ncolors-1 ; i < ncolors/2; i++, j-- ) {
		tmp_pixel = new_colors[i].pixel;
		new_colors [i].pixel = new_colors[j].pixel;
		new_colors [j].pixel = tmp_pixel;
		if ( xdpth > 8 ) {
		   tmp_pixel = new_colors[i].red;
		   new_colors [i].red = new_colors[j].red;
		   new_colors [j].red = tmp_pixel;
		   tmp_pixel = new_colors[i].green;
		   new_colors [i].green = new_colors[j].green;
		   new_colors [j].green = tmp_pixel;
		   tmp_pixel = new_colors[i].blue;
		   new_colors [i].blue = new_colors[j].blue;
		   new_colors [j].blue = tmp_pixel;
		}
	    }
	}

/*
 *	Set the new colors
 */ 
	RefreshImageColorMap ();
}


void InvertImageColors( WindowObjectType *wo, int cbank, int toggle )
{

	invert = toggle;
	SetNewImageColors (wo, cbank );

}


void ChangeImageBrightness( WindowObjectType *wo, int cbank, int new_fade )
{

	fade = new_fade;
	SetNewImageColors ( wo, cbank );

}


void RollColorEnhancement( WindowObjectType *wo, int cbank, int new_roll )
{

	roll = new_roll;
	SetNewImageColors ( wo, cbank );

}

void
ResetImageColors ( WindowObjectType *wo, int cbank ) 
{

	Widget				fadeW, rollW, invertW;
	GuiImageEnhanceDialogType	*img;

	fade = 50;
	roll = 0;
	invert = 0;

/*
 *	Set all the enhancement widget controls to their default positions 
 */
 	img = GetGuiImageEnhanceDialog();

	rollW = GetImageRollScaleW ( img );
	if( rollW )
		XtVaSetValues ( rollW, XmNvalue, roll, NULL );

	fadeW = GetImageFadeScaleW ( img );
	if( fadeW )
		XtVaSetValues ( fadeW, XmNvalue, fade, NULL );

	fadeW = GetSatImageFadeScaleW ( img );
	if( fadeW )
		XtVaSetValues ( fadeW, XmNvalue, fade, NULL );

	invertW = GetColorInvertW ( img );
	if( invertW )
		XtVaSetValues ( invertW, XmNset, False, NULL );


	SetNewImageColors ( wo, cbank );

}


void
newlut ( char *lutfile )
{
	int			cbank, iret;
	WindowObjectType	*wo;

/*
 *	Change color table.
 */
	im_lutf( lutfile, &iret, strlen(lutfile) );


	wo = GetActiveWindowObject();
	cbank = GetGempakColorBank( wo );

/*
 *	Force it to get new colors
 */
	got_base_colors = 0;
	
 	SetNewImageColors( wo, cbank );

}
