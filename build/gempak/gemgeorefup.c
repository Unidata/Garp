/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gemgeorefup.c
 *
 *	GEMPAK georefererencing routines.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	07/97	COMET	Changed to save changed garea in georef object
 *	07/97	COMET	Add //nm to projection string that don't have it.
 *	11/97	COMET	Small change to account for removal of .5 factor
 *			in  getimagecoords(). Fixed bug when zooming using
 *			image coords.
 *	01/98	COMET	Changed getimagecoords() calling sequence
 *	 2/98	COMET	Changed 1st arg in getimagecoords() to NULL
 *	 3/98	COMET	Fixed zoom shifting bug again. Now, save the
 *			new garea in the geoRef object for the first
 *			frame only for SAT and RAD projections.
 *
 ***********************************************************************/


#include "underscore.h"
#include "winobj.h"


/*
 *	"full_area_draw_setting" is used to know whether the user
 *	wants the entire drawing window used, or allow plots to
 *	be clipped on lat/lon corners. Default is full area.
 */

BooleanType	full_area_draw_setting = True;


void GempakUpdateGeoRef( WindowObjectType *wo )
{

	int			dropFlag;
	int			iret;
	GeoRefObjectType	*geo;
	PixmapObjectType	*po;
	char			filename[512], fullgarea[80];
	char			Dcoords [] = "D";
	float			lat1, lat2, lon1, lon2;
	float			x1, y1, x2, y2;
	float			ix1, ix2, iy1, iy2;

	po = GetActivePixmapObject (wo);

	if( ! (geo = GetGeoRefObject( wo )) ) {
		printf("GempakUpdateGeoRef: There's trouble brewing - ");
		printf("GeoRefObjectType not set.\n");
		return;
	}

/*
 *	Make sure the projection has an //nm or //NM on the end. Needed
 *	to make the full window plotting work correctly.
 */
	if ( strstr ( geo->proj, "/nm" ) == NULL &&
	     strstr ( geo->proj, "/NM" ) == NULL )
	{
	    strcat ( geo->proj, "/nm");
	}

        sprintf(filename, "^%s", geo->filename);
        gg_maps( geo->proj, geo->garea, filename, &dropFlag, &iret, 
		strlen(geo->proj),
                strlen(geo->garea), 
		strlen(filename) );

/*
 *	If the full area drawing mode is set, we will recompute the GAREA
 *	corners to use the full window. Do not do this if GAREA = DSET
 *	however.
 */
	if ( strcmp ( geo->garea, "DSET" ) != 0 &&
		full_area_draw_setting  ) {

	    x1 = 0;
	    x2 = (float) GetWindowObjectWidth(wo) - 1;
	    y1 = (float) GetWindowObjectHeight(wo) - 1;
	    y2 = 0;

/*
 *	    Try to get lat/lon points first, if that fails, try getting
 *	    image coords (may be off earth)
 */
	    if ( getlatlon ( Dcoords, &x1, &y1, &lat1, &lon1 ) &&
	         getlatlon ( Dcoords, &x2, &y2, &lat2, &lon2 ) )

	        sprintf ( fullgarea, "%.2f;%.2f;%.2f;%.2f", lat1, lon1, lat2, lon2);

	    else if ( getimagecoords ( NULL, Dcoords, &x1, &y1, &ix1, &iy1 ) &&
	              getimagecoords ( NULL, Dcoords, &x2, &y2, &ix2, &iy2 ) )

	        sprintf ( fullgarea, "!%d;%d;%d;%d",
	        	(int) (ix1 + .5), (int) (iy1 + .5),
	        	(int) (ix2 + .5), (int) (iy2 + .5));

	    else
	        return;

/*
 *	    For images (proj=RAD or SAT), save the new garea in the geo
 *	    object. It is needed because gg_maps is called again later 
 *	    to read the image header, and so it must have the same garea.
 *	    Also, save the new area only for the first pixmap. This fixes
 *	    a problem with "map creep" when zooming off the earth.
 */
            if ( (strstr ( geo->proj, "SAT" ) != NULL ||
                  strstr ( geo->proj, "RAD" ) != NULL ) &&
	          GetActivePixmapObjectIndex(wo) == 0 )
		 
	    	strcpy ( geo->garea, fullgarea);
	    
/*
 *	    Reset the mapping.
 */
            gg_maps( geo->proj, fullgarea, filename, &dropFlag, &iret,
			strlen(geo->proj),
                	strlen(fullgarea), 
			strlen(filename) );

	}

	SavePixmapGeoMapping ( wo );

}


void
ToggleAreaDisplaySetting ( BooleanType toggled )
{

	if ( toggled )
		full_area_draw_setting = True;
	else
		full_area_draw_setting = False;
}
