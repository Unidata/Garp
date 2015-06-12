/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	geminit.c
 *
 *	GEMPAK initialization routines.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	8/02	S. Chiswell/Unidata	Added Graphics color initialization
 *
 ***********************************************************************/


#include <string.h>
#include <Xm/Xm.h>
#include "underscore.h"
#include "xwcmn.h"
#include "color.h"


/*	10/02/96 - Cowie/COMET - removed sat + rad color allocation */


int	InitGempakColorMap();


int GempakInit ( Widget toplevel )
{

	int		iret;
	int		mode = 1;

/*
 *      Initialize some of the global variables in the XW driver.
 */
        gemdisplay = XtDisplay( toplevel );
        root       = DefaultRootWindow( gemdisplay );
        gemmap     = DefaultColormap( gemdisplay,
                                       DefaultScreen ( gemdisplay ) );
        gemvis     = DefaultVisual( gemdisplay, DefaultScreen( gemdisplay ));

/*
 *	Do the colors
 */
	if( InitGempakColorMap() )
		return( 1 );


/*
 *	Kick start gempak...
 */
        in_bdta( &iret );
        gd_init( &iret );

        gg_init( &mode, &iret );

	/* Added by Chiz/Unidata for grid diagnostics initialization */
	dg_intl ( &iret );

	return( iret );

}

int InitGempakColorMap()
{

	int      ret;

        /*
         * set the color bank ID
         */
        /*GraphCid = 0; **these are defined in gempak's color.h**
        SatCid   = 1;
        RadCid   = 2;*/

        /*
         * Initialize the global color bank structure
         */
        xgbank(gemdisplay, &ret);

        /*
         * allocate read and write color cells for
         * graphics
         */
        xcaloc( GraphCid, &ret );
        if ( ret != G_NORMAL ) {
		printf("InitGempakColorMap:  Can't allocate Graphic colors.\n");
                return( 1 );
        }

	xscint( &ret );

        /*
         * set the color allocation flag
         */
        allocflag[GraphCid] = 1;



	return( 0 );




}
