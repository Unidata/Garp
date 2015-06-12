/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	georefobj.c
 *
 *	Georeferencing object utilities.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	10/97	COMET	Changed names of external widget buttons
 *	11/97	COMET	Removed overlayMapFlag and drawMapFlag
 *	12/97	COMET	Made a couple changes to prevent some GEMPAK
 *			warning messages, and to deal better with NIDS
 *			X-section images.
 *
 ***********************************************************************/


#include "winobj.h"
#include "update.h"


extern Widget		area, mapBackgroundsButton, overlaysButton,
			topoMapsButton, latlonGridButton;


void SetGeoRefObjectViewType( WindowObjectType *wo, int type);
void SetGeoRefObjectProjection( WindowObjectType *wo, char *proj);
void SetGeoRefObjectGarea( WindowObjectType *wo, char *garea);
void SetGeoRefObjectFilename( WindowObjectType *wo, char *filename);
void SetGeoRefObject( WindowObjectType *wo, GeoRefObjectType *geo );

void DestroyGeoRefObject( GeoRefObjectType *geo )
{

	assert( geo );

	free( geo );

}

void ReleaseGeoRefObject( WindowObjectType *wo )
{
	GeoRefObjectType	*geo;

	if( geo = GetGeoRefObject( wo ) ) {
		DestroyGeoRefObject( geo );
		SetGeoRefObject( wo, (GeoRefObjectType *) NULL );
	}

	XtVaSetValues ( mapBackgroundsButton, XmNsensitive, True, NULL );
	XtVaSetValues ( topoMapsButton, XmNsensitive, True, NULL );
	XtVaSetValues ( latlonGridButton, XmNsensitive, True, NULL );
	XtVaSetValues ( overlaysButton, XmNsensitive, True, NULL );
	XtVaSetValues ( area, XmNsensitive, True, NULL );

	return;

}
GeoRefObjectType *CreateGeoRefObject()
{
	GeoRefObjectType        *geo;

	geo = (GeoRefObjectType *) malloc( sizeof(*geo) );

	assert( geo );

	memset( geo, 0, sizeof( *geo ) );

	return( geo );

}

	
GeoRefObjectType *GetGeoRefObject( WindowObjectType *wo )
{

	return( wo->geoRefObject );

}

void SetGeoRefObject( WindowObjectType *wo, GeoRefObjectType *geo )
{
	wo->geoRefObject = geo;
	return;
}

void SetGeoRefObjectViewType( WindowObjectType *wo, int type)
{
	GeoRefObjectType	*geo;

	geo = GetGeoRefObject( wo );
	if( ! geo ) {
		geo = CreateGeoRefObject();
		wo->geoRefObject = geo;
	}

	geo->prevViewType = geo->viewType;
		
	geo->viewType = type;

	return;

}

void SetGeoRefObjectProjection( WindowObjectType *wo, char *proj)
{
	GeoRefObjectType	*geo;

	geo = GetGeoRefObject( wo );
	if( ! geo ) {
		geo = CreateGeoRefObject();
		wo->geoRefObject = geo;
	}
		
	strcpy ( geo->prevProj, geo->proj);

	if( proj )
		strcpy( geo->proj,  proj );
	else
		memset( geo->proj, 0, sizeof( geo->proj) );

	return;

}
void SetGeoRefObjectFilename( WindowObjectType *wo, char *filename)
{
	GeoRefObjectType	*geo;

	geo = GetGeoRefObject( wo );
	if( ! geo ) {
		geo = CreateGeoRefObject();
		wo->geoRefObject = geo;
	}

	strcpy ( geo->prevFilename, geo->filename);

	if( filename )
		strcpy( geo->filename,  filename );
	else
		memset( geo->filename, 0, sizeof( geo->filename) );

	return;

}
void SetGeoRefObjectGarea( WindowObjectType *wo, char *garea)
{
	GeoRefObjectType	*geo;

	geo = GetGeoRefObject( wo );
	if( ! geo ) {
		geo = CreateGeoRefObject();
		wo->geoRefObject = geo;
	}

	strcpy ( geo->prevGarea, geo->garea);

	if( garea )
		strcpy( geo->garea,  garea );
	else
		memset( geo->garea, 0, sizeof( geo->garea) );

	return;

}


/*
 *	Compares the georeferencing information stored with
 *	each pixmapObject and returns 0 if they are the same
 *	and 1 if they differ.
 */

int CompareGeoReferencing (	PixmapObjectType *po1,
				PixmapObjectType *po2 )

{

	ReferencePts	*ref1, *ref2;

/*
 *	First, see if the projections are different.
 */
	if ( strcmp ( po1->proj, po2->proj ) != 0 )
	    return (1);

/*
 *	Otherwise, compare the two reference points to see if they
 *	differ. If the absolute value of the difference of the lats
 *	and lons are all less than the fuzziness value, then these
 *	two pixmapObjects can use the same map.
 */
	ref1 = &(po1->referenceLatLons);
	ref2 = &(po2->referenceLatLons);

	if ( ((float) fabs (ref1->lat1 - ref2->lat1) < FLOATFUZZ ) &&
	     ((float) fabs (ref1->lat2 - ref2->lat2) < FLOATFUZZ ) &&
	     ((float) fabs (ref1->lon1 - ref2->lon1) < FLOATFUZZ ) &&
	     ((float) fabs (ref1->lon2 - ref2->lon2) < FLOATFUZZ ) )
	     
	    return (0);

	else 
	
	    return (1);


}


/*
 *	Handles georeferencing details when objects are being
 *	readied for display.
 */

void  UpdatePixmapObjectGeoRef ( WindowObjectType *wo, int potIndex )
{

	PixmapObjectType	*po;
	MetObjectType		*mot;
	GeoRefObjectType	*geo;
	int			motIndex;
	int			overlayClass;
	int			metObjectType;
	int			viewType;
	int			geoIndex;
	int			gemmode;
	BooleanType		isUpdating;


	po = wo->pixmapObjects[potIndex];

	if ( po->numMetObjects > 0 ) {

/*
 *	    Get the metObject for georeferencing. Images have priority, 
 *	    otherwise, just use the first metObject.
 */
	    geoIndex = 0;

	    for( motIndex = 0; motIndex <  po->numMetObjects; motIndex++ ) {

		mot = po->metObjects[motIndex];
		overlayClass = GetMetObjectClass( GetMetObjectType( mot ) );

		if ( overlayClass <= COLORFILL )
			geoIndex = motIndex;

	    }

	    mot = po->metObjects[geoIndex];
	    metObjectType = GetMetObjectType( mot );
	    overlayClass = GetMetObjectClass(metObjectType );
	    viewType     = GetMetObjectView( metObjectType );

/*
 *	    If the georeferencing object is as yet undefined, make this
 *	    metObject's garea and projection global.
 */
	    if( !( geo = GetGeoRefObject( wo )) ) {
	
		SetGeoRefObjectViewType( wo, viewType );
		SetGeoRefObjectProjection( wo, mot->proj );
		SetGeoRefObjectGarea( wo, mot->garea );
	    }

/*
 *	    If we have an image, save the projection and filename.
 */
	    if( overlayClass == IMAGE || overlayClass == BACKGROUND ) {

		SetGeoRefObjectProjection( wo, mot->proj);
		SetGeoRefObjectFilename( wo, mot->filename);

	    }

	}

/*
 *	Set the GEMPAK mapping for PLAN view only. Other view
 *	types are handled by the respective driver routines.
 */
 	geo = GetGeoRefObject( wo );
	if ( geo->viewType == PLAN )
		GempakUpdateGeoRef( wo );


/*
 *	Check for the GEMPAK plot mode to deal with NIDS cross-section images, 
 *	which as far as GARP is concerned are PLAN view, but to GEMPAK they are
 *	graphs (mode=2). Remove any map backgrounds.
 */
	gemmode = getgemplotmode ();
	if ( gemmode == 2 ) {
	    SetGeoRefObjectGarea( wo, "DSET" );
	    WindowObjectDestroyBackground (wo);
	    RemoveBackgroundObjects( po );
	}

/*
 *	Update the background maps for PLAN view, (but not for NIDS Xsections)
 *	and set the senstitivity of certain widgets based on the view type. 
 */
	if ( geo->viewType == PLAN && gemmode != 2 ) {

	    UpdateBackgroundObjects ( wo,  potIndex);

	    XtVaSetValues ( mapBackgroundsButton, XmNsensitive, True, NULL );
	    XtVaSetValues ( topoMapsButton, XmNsensitive, True, NULL );
	    XtVaSetValues ( latlonGridButton, XmNsensitive, True, NULL );
	    XtVaSetValues ( overlaysButton, XmNsensitive, True, NULL );
	    XtVaSetValues ( area, XmNsensitive, True, NULL );

	}
	else {
	
	    XtVaSetValues ( mapBackgroundsButton, XmNsensitive, False, NULL );
	    XtVaSetValues ( topoMapsButton, XmNsensitive, False, NULL );
	    XtVaSetValues ( latlonGridButton, XmNsensitive, False, NULL );
	    XtVaSetValues ( overlaysButton, XmNsensitive, False, NULL );
	    XtVaSetValues ( area, XmNsensitive, False, NULL );

	}

}
