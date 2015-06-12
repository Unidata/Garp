/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	mapbgobj.c (formerly backgroundobj.c)
 *
 *	Map background and display functions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	06/97	COMET	Moved background image routines into separate
 *			file. Renamed PutBackgroundMapOnPixmap to
 *			PutMapBackgroundOnPixmap
 *	10/97	COMET	Added code for dealing with lat/lon grid, and
 *			user-specified map backgrounds.
 *	11/97	COMET	Modifications to check on the background object
 *			(maps, lat/lon) status.
 *	06/02	Penn State - A. Person
 *			Updated to support 16- and 24-bit graphics
 *	08/02	S. Chiswell/Unidata	Fixed byteswapping for pixmap
 *
 ***********************************************************************/


#include "winobj.h"
#include "genglobs.h"
#include "guimacros.h"
#include "latlongrid.h"
#include "maplist.h"
#include "_proto.h"



/*
 *	This structure contains all the locations for each pixel of
 *	each color set. This will be replaced by a more comprehensive
 *	vector caching method to be installed in the XW code later, if
 *	ever.
 */

typedef struct {

	unsigned long	color;		/* color			*/
	int		npoints;	/* number of points 		*/
	XPoint		*points;	/* x and y locations 		*/
	int		nmallocs;	/* number of malloc'd chunks	*/

} ColorGroupType;
 

/*
 *	Assume that we will need only 20 colors, tops. CHUNKSIZE is the
 *	size of objects that will be malloc'd each time more memory
 *	is needed for storing the point information. This memory is
 *	kept around and re-used for the duration of this GARP
 */

#define MAXCOLORS	20
#define CHUNKSIZE 	2048

static int		ncolors = 0;
static ColorGroupType	colorGroup [MAXCOLORS];



void PutMapBackgroundOnPixmap ( WindowObjectType *wo,
			     	BooleanType newMap, 
			     	BooleanType overlayMap )
{

	int		i, x, y;
	int		Width, Height;
	GC		gc;
	Display		*display;
	Pixmap		pixmap;
	Pixel		backgroundcolor, pixel;
	int		dpointer;
	int		npts, msize;
	XImage		*mapXimage;

	unsigned char	uctmp;
	int nBytes, negflg, ivalue, il, ir, ier;

	
	Width   = GetWindowObjectWidth (wo);
	Height  = GetWindowObjectHeight (wo);
	gc      = GetWindowObjectGC (wo);
	display = GetWindowObjectDisplay (wo);
	pixmap  = GetActivePixmap (wo);

/*
 *	No need to overlay map? then just do a pixmap to pixmap copy.
 */
	if ( ! overlayMap ) {
	
	    XCopyArea (	display,
			wo->backgroundPixmap,
			pixmap,
			gc,
			0, 0, 
			Width, Height,
			0, 0);
	    return;

	}

/*
 *	Otherwise, we must manually copy the map onto the existing pixmap. 
 *	If a new map is required, we need to save it for later use.
 */
	if ( newMap ) {

/*
 *	    Reset the color groups
 */
	    for ( i = 0; i < ncolors; i++ )
	        colorGroup[i].npoints = 0;

/*
 *	    Get the map pixmap into an XImage structure
 */	
	    mapXimage = XGetImage (	display,
					wo->backgroundPixmap,
					0, 0, 
					Width, Height,
					AllPlanes, ZPixmap );

/*
 *	    Find out the background color. We will be saving all pixels
 *	    *except* this one.
 */
	    backgroundcolor = GetGemBackgroundPixel ();

/*
 *	    Loop over all the points in the image. It is much faster to
 *	    manually access the pixel values with a pointer than calling
 *	    XGetPixel().
 */

	    nBytes = mapXimage->bits_per_pixel / 8;
	    negflg = 0;

	    for ( y = 0; y < Height; y++ ) {

	    	dpointer = y * mapXimage->bytes_per_line;

	        for ( x = 0; x < Width; x++ ) {

		    if(mapXimage->bitmap_bit_order == LSBFirst) {
                       /* need to flip bytes */
                       il = 0; ir = nBytes - 1;
                       while (il < ir) {
                          uctmp = mapXimage->data[dpointer + il];
                          mapXimage->data[dpointer + il] = mapXimage->data[dpointer + ir];
                          mapXimage->data[dpointer + ir] = uctmp;
                          il++;
                          ir--;
                        }
                    }
		    mv_btoi ( (unsigned char *)(mapXimage->data), &dpointer, &nBytes, &negflg,
                        &ivalue, &ier);

		    dpointer+=nBytes;

		    pixel=(Pixel)ivalue;


/*
 *		    Save this pixel
 */
		    if ( pixel != backgroundcolor ) {

/*
 *			Find which color index to use.
 */
			for ( i = 0; i < ncolors; i++ )
			    if ( pixel == colorGroup[i].color ) break;

/*
 *			If this is a new color, initialize some variables
 */			
			if ( i == ncolors ) {
			    colorGroup[i].npoints = 0;
			    colorGroup[i].nmallocs = 0;
			    colorGroup[i].color = pixel;
			    ncolors++;
			}

/*
 *			See if we need more memory for saving the point
 */			    
			npts = colorGroup[i].npoints;
			msize = colorGroup[i].nmallocs * CHUNKSIZE;
			if ( npts == msize ) {

			    colorGroup[i].nmallocs++;

			    if ( npts == 0 )
			    	colorGroup[i].points =
			    	    (XPoint *) (malloc ( colorGroup[i].nmallocs * CHUNKSIZE * sizeof (XPoint) ));
			    else 	
			    	colorGroup[i].points =
			    	    (XPoint *) (realloc ( colorGroup[i].points, colorGroup[i].nmallocs * CHUNKSIZE * sizeof (XPoint) ));
			}

		        colorGroup[i].points[npts].x = x;
		        colorGroup[i].points[npts].y = y;
		        colorGroup[i].npoints++;
		    }
		}
	    }
	
	    XDestroyImage ( mapXimage );

	}

/*
 *	Draw the map points onto the current pixmap
 */
	for ( i = 0; i < ncolors; i++ ) {

	    XSetForeground ( display, gc, colorGroup[i].color );

	    XDrawPoints (	display,
				pixmap,
				gc,
				colorGroup[i].points,
				colorGroup[i].npoints,
				CoordModeOrigin );
	}


}


void
RemoveBackgroundObjects ( PixmapObjectType *pot )
{
	RemoveMetObjectsFromPixObject ( pot, MAP );
}



/*
 *	Updates the background objects as necessary. Removes existing ones,
 *	and then looks at the interface and builds new ones.
 */
void
UpdateBackgroundObjects ( WindowObjectType *wo, int potIndex )
{

	MetObjectType		*mot, *mlist[MAXMETOBJS];
	int			i, count, nmaps;
	char			mapfile[GEMPAKSTRING];
	char			mapname[GEMPAKSTRING];
	char			*latinc, *loninc, *lblinc;
	char			line_color[GEMPAKSTRING];
	char			line_type[GEMPAKSTRING];
	char			line_width[GEMPAKSTRING];
	char			sysM [] = "M";
	float			lat1, lat2, lon1, lon2, latdiff;
	double			fminscale, fmaxscale;
	char			*minscale, *maxscale;
	Widget			stateW;
	BooleanType		stateChanged;
	GuiMapBackgroundDialogType *mbg;
	GuiLatLonGridDialogType	*llg;
	PixmapObjectType	*po, *ppo;


	llg = GetGuiLatLonGridDialog();
	mbg = GetGuiMapBackgroundDialog();
	po = wo->pixmapObjects[potIndex];

/*
 *	First, decide if we need to rebuild the background objects. If there
 *	are no MAP class objects, or if the backgrounds
 *	dialog info has changed, then we will build new ones.
 */
	nmaps = 0;
	for ( i = 0; i < po->numMetObjects; i++ ) {
	    mot = po->metObjects [i];
	    if ( GetMetObjectClass ( mot->type ) == MAP ) nmaps++;
	} 

	stateChanged =( GetLatLonGridChangeState (llg) || 
			GetMapListChangeState (mbg) );

	if ( nmaps == 0 || stateChanged ) 
		RemoveBackgroundObjects( po );
	else
	   	return;


/*
 *	Get the latitude range of the current window
 */
	if ( (getbounds ( sysM, &lat1, &lon1, &lat2, &lon2 )) == 0 )
	    latdiff = -9999;
	else
	    latdiff = (float) fabs (lat2 - lat1);


 	count = 0;

/*
 *	Loop over all the map objects. See if any are turned on. If so,
 *	check the scale range and add them if they are in the right scale.
 */
	for ( i = 0; i < mbg->nmaps; i++ ) {

	    if ( XmToggleButtonGetState ( GetMapListOnToggleW(mbg,i)) ) {
	    
	    	maxscale = XmTextGetString( GetMapListMaxScaleW(mbg,i));
	    	minscale = XmTextGetString( GetMapListMinScaleW(mbg,i));

		fmaxscale = atof ( maxscale);
		fminscale = atof ( minscale);
	        XtFree (  maxscale);
	        XtFree (  minscale);
		
		if ( ( fmaxscale <= 0.0 ) ||
		     ( latdiff >= fminscale && latdiff <= fmaxscale) ) {
		
		    strcpy ( mapfile, mbg->path[i] );
		    strcpy ( mapname, mbg->name[i].string );
		    sprintf ( line_color, "%d", GetMapListLineColor(mbg,i));
		    sprintf ( line_type,  "%d", GetMapListLineType(mbg,i));
		    sprintf ( line_width, "%d", GetMapListLineWidth(mbg,i));
				
		    mlist[count] = (MetObjectType *) MakeMapObject( 
						mapname,
						mapfile,
						line_color,
						line_type,
						line_width);
		    count++;
		}

	    }

	}

/*
 *	Add a lat/lon grid 
 */
	if ( XmToggleButtonGetState ( GetLatLonGridStateW (llg) )) {

	    latinc = XmTextGetString ( GetLatIncrementW (llg) );
	    loninc = XmTextGetString ( GetLonIncrementW (llg) );
	    lblinc = XmTextGetString ( GetLatLonGridLabelIncW(llg) );
	    
	    sprintf ( line_color, "%d", GetLatLonGridColor (llg));
	    sprintf ( line_type, "%d", GetLatLonGridType (llg));
	    sprintf ( line_width,  "%d", GetLatLonGridWidth (llg));

	    mlist[count] = (MetObjectType *) MakeLatLonGridObject( 
						latinc,
						loninc,
						lblinc,
						line_color,
						line_type,
						line_width);
	    XtFree ( latinc );
	    XtFree ( loninc );
	    XtFree ( lblinc );

	    count++;
	}


/*
 *	Add all the created objects in as background objects
 */
	AddMapBackgroundObjects ( po, count, mlist );

/*
 *	Since we have new maps, set all the drawn flags of all the metObjects
 *	in this pixmapObject false so that they will be redrawn.
 */
	SetPixmapObjectDrawnFlag ( po, False );
 
/*
 *	Reset the change state of the map and lat/lon dialogs for the last
 *	pixmap
 */
	if ( potIndex == wo->numPixmapObjects - 1 ) {
 	    SetLatLonGridChangeState ( llg, False);
  	    SetMapListChangeState ( mbg, False);
	}

}


/*
 *	Called when the user presses APPLY on the Map background and
 *	lat/lon grid dialog. Redraws everything.
 */

void
RefreshBackgroundsCB (Widget w, XtPointer client_data, XtPointer call_data )
{
	WindowObjectType        *wo;
	PixmapObjectType        *po;
	char			*proj, *garea;
	
	XmPushButtonCallbackStruct *cbs = 
			(XmPushButtonCallbackStruct *) call_data;

/*
 *	Make sure we have a pixmap to draw into and supply some default
 *	georeferencing
 */
	wo = GetActiveWindowObject();
	if ( wo->numPixmapObjects == 0 )
		po = GetNextPot (wo);

	if( ! GetGeoRefObject(wo) ) {
 
            proj = GetConfigValue ( "default_proj" );
	    if ( proj == NULL ) proj = strdup ( "lcc/30.0;-100;50.0//nm" );

	    garea = GetConfigValue ( "default_garea" );
	    if ( garea == NULL ) garea = strdup ( "23.0;-120.0;47.0;-65.0" );

	    SetGeoRefObjectViewType( wo, PLAN );
	    SetGeoRefObjectGarea( wo, garea );
	    SetGeoRefObjectFilename( wo, NULL );
	    SetGeoRefObjectProjection( wo, proj );

	    Free ( proj );
	    Free ( garea );

	}

/*
 *	Redraw
 */
	RefreshDisplay();

	return;
}



