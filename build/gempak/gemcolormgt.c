/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gemcolormgt.c
 *
 *	X-GEMPAK color utilities
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 3/97	COMET	Changed comment for clarity
 *	10/97	COMET	Added GetGemGraphicsColorBank()
 *	06/02	Penn State - A. Person
 *			Updated to support 16- and 24-bit graphics
 *
 ***********************************************************************/


#include "genglobs.h"

static Pixel		graphicsColorBank[256];
static int		numColors = -1;

#define	GEMPAK_BLACK			0
#define	GEMPAK_WHITE			1
#define	GEMPAK_RED			2

#define	GEMPAK_BACKGROUND		GEMPAK_BLACK
#define	GEMPAK_FOREGROUND		GEMPAK_WHITE

Pixel GemColorToPixel ( int gemColor )
{

	int		cbank = 0;      /* graphics color bank */
	int		iret;

/*
 *	Retrieve the gempak graphics colors. (Do only once)
 */

	if( numColors == -1 ) {

		xqclrs ( &cbank, &numColors, graphicsColorBank, &iret );
/*
		{
		   int i;
		   printf("GemColorToPixel:  cbank=%d,",cbank);
		   printf("GemColorToPixel:  numColors=%d\n",numColors);
		   for(i=0;i<256;i++) {
		      printf("GemColorToPixel:  graphicsColorBank[%d]=",i);
		      printf("%d\n",(int)graphicsColorBank[i]);
		   }
		}
*/


/*
 *		On error, print a message and return gemColor 0.
 */
		if( iret || numColors < 1 ) {

			printf("GemColorToPixel:");
			printf(" xqclrs returned %d, numColors is %d.\n", 
					iret, numColors );

			gemColor = 0;

		}
	}

/*
 *	One more check.  If we would otherwise go out of range,
 *	return color 0.
 */
	if( gemColor >= numColors )
		gemColor = 0;
		
	return( graphicsColorBank[gemColor] );

}

	
void GetGemGraphicsColorBank ( int *nclrs, int **gcbank )
{

	int		cbank = 0;      /* graphics color bank */
	int		iret;

/*
 *	Retrieve the gempak graphics colors. (Do only once)
 */

	if( numColors == -1 ) {

		xqclrs ( &cbank, &numColors, graphicsColorBank, &iret );

/*
 *		On error, print a message and return gemColor 0.
 */
		if( iret || numColors < 1 ) {

			printf("GetGemGraphicsColorBank:");
			printf(" xqclrs returned %d, numColors is %d.\n", 
					iret, numColors );

		}
	}

	*nclrs = numColors;
	*gcbank = (int *)(&graphicsColorBank[0]);
	
	return;

}

void	
PixelToXcolor( Pixel pixel,  XColor *xColor )
{
	extern Display	*display;

	Colormap	cmap;
	int		scr;
	XColor		color;
	int		xdpth;

	scr  = DefaultScreen( display);
	cmap = DefaultColormap( display, scr );
	xdpth = DefaultDepth( display, scr );

	color.pixel = pixel;

	XQueryColor( display, cmap, &color );
	
	*xColor = color;

	return;

}

void
GemColorToXcolor( int gemColor, XColor *xColor )
{
	Pixel		pixel;

	pixel = GemColorToPixel( gemColor );

	PixelToXcolor( pixel, xColor );

	return;

}

/*
 *	Convenience functions for the Gempak background, foreground
 *	and cursor (red)  colors.  
 */
Pixel GetGemBackgroundPixel()
{

	return( GemColorToPixel( GEMPAK_BACKGROUND ) );
}

Pixel GetGemForegroundPixel()
{

	return( GemColorToPixel( GEMPAK_FOREGROUND ) );
}

Pixel GetGemRedPixel()
{
	return( GemColorToPixel( GEMPAK_RED ) );
}
