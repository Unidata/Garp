/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	pointerinfo.c
 *
 *	Functions that return info at the pointer location
 *
 *	History:
 *
 *	11/96 	COMET	Original copy
 *	 2/97 	COMET	Added new units
 *	05/97	COMET	Changed to get image data from metObject
 *	11/97	COMET	Small change to account for removal of .5 factor
 *			in  getimagecoords()
 *	01/98	COMET	Changed getimagecoords() calling sequence
 *	04/04	Unidata	Fixed display of radar values for imgtyp.tbl change
 *
 ***********************************************************************/


#include <string.h>
#include "winobj.h"
#include "underscore.h"

#define KM2MI  0.6214
#define KM2NM  0.53939

#include "proto.h"
/*float	pr_tmkc(), pr_tmkf();*/


void GetCursorPosition( WindowObjectType *wo, int x, int y, char *string )
{

	char			Dcoords [] = "D";
	float			fx, fy, lat, lon;
	GeoRefObjectType	*geo;

	strcpy ( string, "" );

	geo = GetGeoRefObject( wo );
	if ( ! geo || geo->viewType != PLAN ) return;

	fx = x;
	fy = y;

	if ( getlatlon ( Dcoords, &fx, &fy, &lat, &lon ) )
	    sprintf ( string, "Latlon:%6.2f,%7.2f", lat, lon );
	else
	    strcpy  ( string, "Latlon:---.--,----.--");

	return;

}



void GetDistanceInfo(   WindowObjectType *wo, char *units, 
			int x1, int y1, int x2, int y2, char *string )
{

	char	Dcoords [] = "D";
	float	fx1, fy1, fx2, fy2;
	float	lat1, lon1, lat2, lon2;
	float	lat1r, lon1r, lat2r, lon2r;
	float	latdif, londif, az, angle, dist;


	strcpy ( string, "" );

	if ( ! GetAnchorFlag ( wo ) ) return;

	fx1 = x1;
	fy1 = y1;
	fx2 = x2;
	fy2 = y2;

/*
 *	Get the lat/lon of each point, convert to radians, take the 
 *	difference of each.
 */

	if ( getlatlon ( Dcoords, &fx1, &fy1, &lat1, &lon1 ) &&
	     getlatlon ( Dcoords, &fx2, &fy2, &lat2, &lon2 ) ) {

	    lat1r = lat1 * DTR;
	    lon1r = lon1 * DTR;
	    lat2r = lat2 * DTR;
	    lon2r = lon2 * DTR;

	    latdif = lat2r - lat1r;
	    londif = lon2r - lon1r;

	    if ( londif > PI ) londif = londif - TWOPI;
	    if ( londif < -PI ) londif = londif + TWOPI;

/*
 *	    Determine the azimuth direction from point 1 to 2
 */
	    if ( (x1 == x2) && (y1 == y2) ) az = 0.0;
	    else {
		az = ( atan2 ( londif, latdif ) ) * RTD;
		if ( az < 0. ) az += 360.;
	    }

/*
 *	    Get the distance from point 1 to 2 in km
 */
	    londif = londif * cos ( (lat1r + lat2r)/2.);
	    angle = sqrt ( (latdif*latdif) + (londif*londif) );    
	    dist = ( angle * RADIUS) / 1000.;

/*
 *	    Convert to user-specified units
 */
	    if ( strcmp ( units, "NM" ) == 0 )      dist *= KM2NM;
	    else if ( strcmp ( units, "MI" ) == 0 ) dist *= KM2MI;
	    
	    sprintf ( string, "Az: %5.1f Range: %.1f%s", az, dist, units );

	}
	else
	    strcpy  ( string, "Az: ----- Range: ---- ---" );


}


/*
 *	Gets data at X, Y position - assumes that the X, Y point has 
 *	been validated. (Still checks for a reasonable data offset location
 *	though.)
 */

void GetDataAtCursor( WindowObjectType *wo, char *units, int x, int y, char *string )
{

	unsigned int		dvalue;
	float			fdvalue, brtemp, temp;
	unsigned long		pixloc;
	unsigned char		*dptr;
	char			ctemp[30];
	PixmapObjectType	*po;
	AnyImageObjectType	*iot;
	MetObjectType		*mot;
	int			i, k, iret, mindex;
	int			overlayClass;
	float			ix, iy, dx, dy;
	int			nx, ny, np, vis;
	char			Dcoords [] = "D";


	strcpy ( string, "" );

	po = GetActivePixmapObject (wo);

	if ( ! po ) return;

/*
 *	Get the metobject content pointer. Find out which metObject is the
 *	image. If there is a background (image), bail out.
 */
	mindex = -1;
	for( i = 0; i <  po->numMetObjects; i++ ) {

	    mot = po->metObjects[i];
	    overlayClass = GetMetObjectClass( GetMetObjectType( mot ) );

	    if( overlayClass == IMAGE ) mindex = i;

	    if( overlayClass == BACKGROUND ) return;
	}


	if ( mindex == -1 ) return;

	iot = (AnyImageObjectType *) po->metObjects[mindex]->metObjectContent;

	if ( ! iot->imgCmn.imageData ) return;

/*
 *	Determine the image coordinates
 */
 	dx = (float) x;
 	dy = (float) y;

	getimagecoords ( po, Dcoords, &dx, &dy, &ix, &iy ) ;

/*
 *	Round to nearest pixel
 */
	nx = (int) (ix + .5);
	ny = (int) (iy + .5);

 	np = 1;
	/*gptvis ( Dcoords, &np, &dx, &dy, &vis, &iret, strlen ( Dcoords)) ;*/
	gptvis ( Dcoords, &np, &dx, &dy, &vis, &iret ) ;

	dvalue = 0;

/*printf("image coords: nx: %d, (max=%d), ny: %d, (max=%d), depth: %d, vis: %d\n",
	nx,iot->imgCmn.imnpix,ny,iot->imgCmn.imnlin,iot->imgCmn.imdpth,vis);*/

/*
 *	If this is a valid point, calculate offset.
 */
	if ( nx > 0 && nx <= iot->imgCmn.imnpix &&
	     ny > 0 && ny <= iot->imgCmn.imnlin && vis ) {

	    pixloc = iot->imgCmn.imdpth * ((ny - 1) * iot->imgCmn.imnpix + (nx - 1));

/*printf("   pixel location: %d\n",pixloc);*/

	    if ( iot->imgCmn.imdpth == 1 )
		dvalue = *(iot->imgCmn.imageData + pixloc);

	    else {
		if ( iot->imgCmn.imbswp )
		    dptr = iot->imgCmn.imageData + pixloc + iot->imgCmn.imdpth - 1;
		else
		    dptr = iot->imgCmn.imageData + pixloc;;
		
		for ( k = 0; k < iot->imgCmn.imdpth; k++ ) {
		    dvalue =  dvalue << 8;
		    dvalue += *dptr;
		    if ( iot->imgCmn.imbswp )
			dptr--;
		    else 
			dptr++ ;
		}
	    }

	}
	sprintf ( string, "  RAW: %3d",dvalue);

/*
 *	Convert brightness temp to deg K, C, or F.
 */
	if ((strncmp ( iot->imgCmn.cmtype, "IR", 2 ) == 0  ||
	     strncmp ( iot->imgCmn.cmtype, "WV", 2 ) == 0 ) &&
	     strncmp ( iot->imgCmn.cmcalb, "BRIT", 4 ) == 0 ) {

	    fdvalue = (float) dvalue;
	    /*temp = im_btot ( &fdvalue );*/

	    /* Chiz/Unidata change for im_btot now a subroutine*/
            np = 1;
	    im_btot ( &np, (int *)(&dvalue), &temp, &iret );

	    if ( strcmp ( units, "C" ) == 0 ) {
		brtemp = (float)pr_tmkc ( &temp ); }
	    else if ( strcmp ( units, "F" ) == 0 ) {
	        brtemp = (float)pr_tmkf ( &temp ); }
	    else{
		brtemp = temp;
	    }

	    sprintf ( ctemp, " (%5.1f%s)", brtemp, units);
	    strcat ( string, ctemp);
	}
	else if ((strncmp ( iot->imgCmn.cmcalb, "PROD",4) == 0)&&
                 iot->imgCmn.cmbunt[0] != '\0')
           {
           /* Chiz/Unidata ** Calculate a value from color bar settings */
           if(iot->imgCmn.cmblev[dvalue][0] != '\0')
              sprintf ( ctemp, " (%s %s)" , iot->imgCmn.cmblev[dvalue], 
                 iot->imgCmn.cmbunt);
           else
              {
              i = dvalue; ix = -9999.; iy = -9999.;
              ctemp[0] = '\0';
              while(i>iot->imgCmn.immnpx)
                 {
                 i--;
                 if(iot->imgCmn.cmblev[i][0] != '\0')
                    {
                    sscanf(iot->imgCmn.cmblev[i],"%f",&ix);
                    break;
                    }
                 }
              k = dvalue;
              while((k<iot->imgCmn.immxpx)&&(ix != -9999))
                 {
                 k++;
                 if(iot->imgCmn.cmblev[k][0] != '\0')
                    {
		    sscanf(iot->imgCmn.cmblev[k],"%f",&iy);
                    break;
                    }
                 }
              if((ix != -9999)&&(iy != -9999))
                 {
                 dx = (float)(dvalue - i);
                 dy = (float)(k - i);
                 fdvalue = ix + (dx/dy)*(iy - ix);
                 sprintf(ctemp," (%5.1f%s)",fdvalue,iot->imgCmn.cmbunt);
                 }
              }
           strcat (string, ctemp);
           }

/*
 *	Radar data is calibrated to provided units.
 */
	/*else if ((strncmp (iot->imgCmn.cmsorc, "NIDS", 4) == 0 ||
		  strncmp (iot->imgCmn.cmsorc, "WSI", 3) == 0 ) &&
		  iot->imgCmn.cmbunt[0] != '\0' ) {*/
	else if ( iot->imgCmn.imradf == 1 || iot->imgCmn.cmbunt[0] != '\0' ) {
	    sprintf ( ctemp, " (%s %s)" ,iot->imgCmn.cmblev[dvalue], iot->imgCmn.cmbunt);
	    strcat (string, ctemp);
	}
	
	return;

}
