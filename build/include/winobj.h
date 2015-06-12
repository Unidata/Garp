/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	winobj.h
 *
 *	Main window header file. The big cahuna.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/97	COMET	Added object type UPPERAIRPROFILEOBJECT.
 *	 3/97	COMET	Added XSECTIONGRIDFILLOBJECT macro
 *	 4/97	COMET	Added "scale" parameter to VECTORGRIDOBJECT.
 *	 6/97	COMET	Cleaned up image-related objects
 *	 8/97	COMET	Added DataTimeObject for time matching.
 *	 9/97	COMET	Added type and label to surface object, tfilter
 *			to Upperair objects, model to grid data objects.
 *	10/97	COMET	Added nstations to surface, upperair objects
 *	10/97	COMET	Added LATLONGRIDOBJECT, LatLonGridObjectType, 
 *			changed contents of MapObjectType
 *	11/97	COMET	Removed drawMapFlag, overlayMapFlag and numMaps from
 *			pixmapObject. Removed mapIndex from MapObjectType
 *	11/97	COMET	Redefined and enabled titleObject. Added field_label
 *			model_label and model_name to some metObjects. 
 *	12/97	COMET	Added scale to image objects
 *	 2/98	COMET	Added ABORTtype macros
 *
 ***********************************************************************/


#ifndef _WINOBJ_H
#define _WINOBJ_H

#include <assert.h>
#include <math.h>

#include "gendef.h"
#include "gui.h"

#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>


#define ABORTNONE		0
#define ABORTCLEAR		1
#define ABORTRESET		2


#define	STDSTRINGSIZE		(80 + 1)
#define	GEMPAKSTRING		(80 + 1)
#define FILENAMESTRING		(512 + 1)


/*
 * MetObjectTypes
 */
#define ANYOBJECT			0
#define SURFACEOBJECT			1
#define RADAROBJECT			2
#define SATOBJECT			3
#define MAPOBJECT			4
#define MARKEROBJECT			5
#define RADARRINGOBJECT			6
#define SCALARGRIDOBJECT		7
#define VECTORGRIDOBJECT		8
#define NOWRADOBJECT			9
#define UPPERAIROBJECT			10
#define XSECTIONGRIDOBJECT		11
#define WINDPROFILEROBJECT		12
#define SCALARGRIDFILLOBJECT		13
#define TIMEHEIGHTOBJECT		14
#define VERTICALPROFILEOBJECT		15
#define UPPERAIRPROFILEOBJECT		16
#define XSECTIONGRIDFILLOBJECT		17
#define BACKGROUNDIMAGEOBJECT		18
#define LATLONGRIDOBJECT		19


/*
 * Generic descriptive terms for data types
 */
#define STATICOBJECT			0	/* Static object, map, overlay.. */
#define NONSTATICOBJECT			1	/* non-static (data)		 */


/*
 * View types
 */
#define PLAN				1
#define SPATIALCROSSSECTION		2
#define TIMEHEIGHTCROSSSECTION		3
#define TIMESERIES			4
#define VERTICALPROFILE			5


/*
 * Overlay class - order is important here. These define the order in
 * which met objects get drawn. LASTCLASS is not really a class but a value
 * indicating what the last class value is. If you add a class, change
 * LASTCLASS appropriately.
 */
#define UNDEFINED			0	/* nothing		*/
#define BACKGROUND			1	/* background image	*/
#define IMAGE				2	/* data image		*/
#define COLORFILL			3	/* color fill, duh	*/
#define MAP				4	/* map graphic		*/
#define GRAPHICS			5	/* data graphics	*/
#define OVERLAY				6	/* overlay graphics	*/
#define LASTCLASS			6





/*
 * Compile time storage allocation constraints
 */
#define MAXMETOBJS			512
#define MAXPIXMAPOBJS			128
#define MAXMAPOBJS			8



typedef struct {

	int		viewType;
	char		proj[GEMPAKSTRING];
	char		garea[GEMPAKSTRING];
	char		filename[FILENAMESTRING];
	int		prevViewType;
	char		prevProj[GEMPAKSTRING];
	char		prevGarea[GEMPAKSTRING];
	char		prevFilename[FILENAMESTRING];

} GeoRefObjectType;

/*
 *	Structure containing information that describes a map
 *	projection.
 */

typedef struct {

	float	angle1;		/* reference angles 1, 2, 3 */
	float	angle2;
	float	angle3;
	float	latll;		/* lower left lat, lon  */
	float	lonll;
	float	latur;		/* upper right lat, lon */
	float	lonur;

} MapProjType;

/*
 *	Structure containing information that describes a
 *	satellite projection for MCIDAS AREA files
 */

typedef struct {

	char	imgnam [80];	/* image name */
	int	area [64];	/* area block  */
	int	nav [640];	/* nav block */
	int	ilef;		/* left image coord */
	int	itop;		/* top  image coord */
	int	irit;		/* right image coord */
	int	ibot;		/* bottom image coord */

} SatProjType;

/*
 *	Structure containing the x, y coordinates and associated latitude and 
 *	longitude of two reference points on the display used for verifying 
 *	mapping between successive pixmapObjects. 
 */

typedef struct {

	float	x1;
	float	y1;
	float	x2;
	float	y2;
	float	lat1;
	float	lon1;
	float	lat2;
	float	lon2;

} ReferencePts;

typedef struct {

	Dimension	x1;
	Dimension	x2;
	Dimension	y1;
	Dimension	y2;

} RectangularRegionType;

/*
 *	Animation Object definition.  Created on the fly for series
 *	pixmap objects that are loaded.  Destroyed during a clear.
 */
typedef struct {

	Display			*display; 
	XtAppContext		appContext;
	Window			windowID; 
	XtIntervalId		xtIntervalID;
	GC			gc;

	int			command;
	int			direction;
	int			isAnimating;
	int			dwellFirst;
	int			dwellMiddle;
	int			dwellLast;
	int			dwellRate; 

	int			activePixmapIndex; 

	struct WindowObjectS	*windowObject;
	unsigned int		width; 
	unsigned int		height; 
	int			numPixmaps; 
	Pixmap			*pixmaps;

} AnimationObjectType;


/*
 *	Contains title string information for each pixmap object.
 */
typedef struct TitleObjectS {

/*
	GC			gc;

	int			displayState;

	unsigned int		xOrigin;
	unsigned int		yOrigin;
	unsigned int		width; 
	unsigned int		height; 

	Widget			dialogWidget;
	Widget			listWidget;

	XmString		*xmStrings;
*/
	unsigned int		numTitles;
	RectangularRegionType	region[20];

} TitleObjectType;

typedef struct {
	AbsTime			*refTimes;
	AbsTime			*fcstTimes;
	AbsTime			*validTimes;
	int			numTimes;
	int			matchType;
	int			forecastType;
} DataTimeObjectType;

typedef struct {
	int			pad;
} AnyObjectType;


/*
 *	Struct for holding image information (least common
 *	denominator of all the image-related objects)
 */
typedef struct {
	int		imftyp;		/* The following items mirror the  */
	int		imbank;		/* GEMPAK image common block  	   */
	int		imdoff;		/* (imgdef.cmn)		           */
	int		imldat;		/* ...				   */
	int		imnpix;
	int		imnlin;
	int		imdpth;
	float		rmxres;
	float		rmyres;
	int		imleft;
	int		imtop;
	int		imrght;
	int		imbot;
	float		rmxysc;
	int		imbswp;
	int		imnchl;
	int		imprsz;
	int		imdcsz;
	int		imclsz;
	int		imlvsz;
	int		imvald;
	int		imrdfl;
	int		immnpx;
	int		immxpx;
	int		imsorc;
	int		imtype;
	int		imradf;
	float		rmbelv;
	int		imdate;
	int		imtime;
	int		imndlv;
	char		cmblev[256][9];
	char		cmbunt[9];
	char		cmsorc[21];
	char		cmtype[9];
	char		cmlutf[17];
	char		cmfile[133];
	char		cmcalb[9];	/* ... end of GEMPAK image common */

	char		colorbar[GEMPAKSTRING];
	char		scale[GEMPAKSTRING];
	unsigned char	*imageData;

} ImageCommonType;

/*
 *	Specific image objects. Includes ImageCommonType and whatever
 *	else makes them unique.
 */

typedef struct {
	ImageCommonType	imgCmn;
} SatObjectType;

typedef struct {
	ImageCommonType	imgCmn;
} RadarObjectType;

typedef struct {
	ImageCommonType	imgCmn;
} BackgroundImageObjectType;

/*
 *	This one is useful as a generic image object.
 */
typedef struct {
	ImageCommonType	imgCmn;
} AnyImageObjectType;


typedef struct {
	char		ftime[GEMPAKSTRING];
	char		vcoord[GEMPAKSTRING];
	char		field[GEMPAKSTRING];
	char		field_label[GEMPAKSTRING];
	char		vector[GEMPAKSTRING];
	char		point[GEMPAKSTRING];
	char		cint[GEMPAKSTRING];
	char		cmin[GEMPAKSTRING];
	char		cmax[GEMPAKSTRING];
	char		line_color[GEMPAKSTRING];
	char		line_type[GEMPAKSTRING];
	char		line_width[GEMPAKSTRING];
	char		label_freq[GEMPAKSTRING];
	char		text[GEMPAKSTRING];
	char		scale[GEMPAKSTRING];
	char		yaxis[GEMPAKSTRING];
	char		taxis[GEMPAKSTRING];
	char		ptype[GEMPAKSTRING];
	char		refvec[GEMPAKSTRING];
	char		wind_symbol[GEMPAKSTRING];
	char		symbol_size[GEMPAKSTRING];
	char		symbol_width[GEMPAKSTRING];
	char		symbol_type[GEMPAKSTRING];
	char		symbol_headsize[GEMPAKSTRING];
	char		model_name[GEMPAKSTRING];
	char		model_label[GEMPAKSTRING];
} TimeHeightObjectType;

typedef struct {
	char		ftime[GEMPAKSTRING];
	char		vcoord[GEMPAKSTRING];
	char		field[GEMPAKSTRING];
	char		field_label[GEMPAKSTRING];
	char		vector[GEMPAKSTRING];
	char		point[GEMPAKSTRING];
	char		cint[GEMPAKSTRING];
	char		cmin[GEMPAKSTRING];
	char		cmax[GEMPAKSTRING];
	char		line_color[GEMPAKSTRING];
	char		line_type[GEMPAKSTRING];
	char		line_width[GEMPAKSTRING];
	char		label_freq[GEMPAKSTRING];
	char		text[GEMPAKSTRING];
	char		scale[GEMPAKSTRING];
	char		xaxis[GEMPAKSTRING];
	char		yaxis[GEMPAKSTRING];
	char		ptype[GEMPAKSTRING];
	char		refvec[GEMPAKSTRING];
	char		thtaln[GEMPAKSTRING];
	char		thteln[GEMPAKSTRING];
	char		mixrln[GEMPAKSTRING];
	char		filter[GEMPAKSTRING];
	char		winpos[GEMPAKSTRING];
	char		wind_symbol[GEMPAKSTRING];
	char		symbol_size[GEMPAKSTRING];
	char		symbol_width[GEMPAKSTRING];
	char		symbol_type[GEMPAKSTRING];
	char		symbol_headsize[GEMPAKSTRING];
	char		model_name[GEMPAKSTRING];
	char		model_label[GEMPAKSTRING];
} VertProfileObjectType;

typedef struct {
	int		type;
	char		gdatim[GEMPAKSTRING];
	char		gvcord[GEMPAKSTRING];
	char		gfunc[GEMPAKSTRING];
	char		field_label[GEMPAKSTRING];
	char		gvect[GEMPAKSTRING];
	char		cxstns[GEMPAKSTRING];
	char		cint[GEMPAKSTRING];
	char		cmin[GEMPAKSTRING];
	char		cmax[GEMPAKSTRING];
	char		line_color[GEMPAKSTRING];
	char		line_type[GEMPAKSTRING];
	char		line_width[GEMPAKSTRING];
	char		label_freq[GEMPAKSTRING];
	char		text[GEMPAKSTRING];
	char		scale[GEMPAKSTRING];
	char		clrbar[GEMPAKSTRING];
	char		contur[GEMPAKSTRING];
	char		skip[GEMPAKSTRING];
	char		fint[GEMPAKSTRING];
	char		fmin[GEMPAKSTRING];
	char		fmax[GEMPAKSTRING];
	char		fline[GEMPAKSTRING];
	char		yaxis[GEMPAKSTRING];
	char		ptype[GEMPAKSTRING];
	char		refvec[GEMPAKSTRING];
	char		wind_symbol[GEMPAKSTRING];
	char		symbol_size[GEMPAKSTRING];
	char		symbol_width[GEMPAKSTRING];
	char		symbol_type[GEMPAKSTRING];
	char		symbol_headsize[GEMPAKSTRING];
	char		model_name[GEMPAKSTRING];
	char		model_label[GEMPAKSTRING];
	BooleanType	contour;
	BooleanType	fill;
} XSectionGridObjectType;

typedef struct {
	char		gdatim[GEMPAKSTRING];
	char		glevel[GEMPAKSTRING];
	char		gvcord[GEMPAKSTRING];
	char		gfunc[1024];
	char		field_label[GEMPAKSTRING];
	char		cint[GEMPAKSTRING];
	char		cmin[GEMPAKSTRING];
	char		cmax[GEMPAKSTRING];
	char		line_color[GEMPAKSTRING];
	char		line_type[GEMPAKSTRING];
	char		line_width[GEMPAKSTRING];
	char		label_freq[GEMPAKSTRING];
	char		scale[GEMPAKSTRING];
	char		hilo[GEMPAKSTRING];
	char		hlsym[GEMPAKSTRING];
	char		clrbar[GEMPAKSTRING];
	char		contur[GEMPAKSTRING];
	char		skip_contour[GEMPAKSTRING];
	char		skip_plot_x[GEMPAKSTRING];
	char		skip_plot_y[GEMPAKSTRING];
	char		fint[GEMPAKSTRING];
	char		fmin[GEMPAKSTRING];
	char		fmax[GEMPAKSTRING];
	char		fline[GEMPAKSTRING];
	char		text[GEMPAKSTRING];
	char		gridtype[GEMPAKSTRING];
	char		model_name[GEMPAKSTRING];
	char		model_label[GEMPAKSTRING];
	int		nfunction;
	BooleanType	contour;
	BooleanType	value;
	BooleanType	fill;
	BooleanType	symbol;
} ScalarGridObjectType;

typedef struct {
	char		gdatim[GEMPAKSTRING];
	char		glevel[GEMPAKSTRING];
	char		gvcord[GEMPAKSTRING];
	char		gvect[1024];
	char		field_label[GEMPAKSTRING];
	char		wind_symbol[GEMPAKSTRING];
	char		symbol_color[GEMPAKSTRING];
	char		symbol_size[GEMPAKSTRING];
	char		symbol_width[GEMPAKSTRING];
	char		symbol_type[GEMPAKSTRING];
	char		symbol_headsize[GEMPAKSTRING];
	char		refvec[GEMPAKSTRING];
	char		scale[GEMPAKSTRING];
	char		skip_contour[GEMPAKSTRING];
	char		skip_plot_x[GEMPAKSTRING];
	char		skip_plot_y[GEMPAKSTRING];
	char		text[GEMPAKSTRING];
	char		gridtype[GEMPAKSTRING];
	char		model_name[GEMPAKSTRING];
	char		model_label[GEMPAKSTRING];
	int		nfunction;
} VectorGridObjectType;


typedef struct {

	char		cxstn[GEMPAKSTRING];
	char		snfiles[20][FILENAMESTRING];
	char		dattim[GEMPAKSTRING];
	char		yaxis[GEMPAKSTRING];
	char		taxis[GEMPAKSTRING];
	char		fint[GEMPAKSTRING];
	char		text[GEMPAKSTRING];
	char		wind_symbol[GEMPAKSTRING];
	char		symbol_size[GEMPAKSTRING];
	char		symbol_width[GEMPAKSTRING];
	int		nfiles;

} WindProfilerObjectType;


typedef struct {

	BooleanType	plot_stn;
	BooleanType	plot_marker;
	char		stations[LLSTFL*GEMPAKSTRING];
	char		symbol_size[GEMPAKSTRING];
	char		symbol_width[GEMPAKSTRING];
	char		text[GEMPAKSTRING];
	float		lat[LLSTFL];
	float		lon[LLSTFL];
	int		stidlen;
	int		color;
	int		marker;
	int		nstn;

} MarkerObjectType;


typedef struct {

	char		stations[LLSTFL*GEMPAKSTRING];
	char		stid[GEMPAKSTRING];
	char		units[GEMPAKSTRING];
	float		lat[LLSTFL];
	float		lon[LLSTFL];
	float		increment;
	float		minimum;
	int		color;
	int		number;
	int		nstn;

} RadarRingObjectType;


typedef struct {

	char    	sfparm[FILENAMESTRING];
	char    	dattim[GEMPAKSTRING];
	char    	colors[GEMPAKSTRING];
	char    	filter[GEMPAKSTRING];
	char    	text[GEMPAKSTRING];
	char		type[GEMPAKSTRING];
	char		label[GEMPAKSTRING];
	char		symbol_size[GEMPAKSTRING];
	char		symbol_width[GEMPAKSTRING];
	int		marker;
	int		pmarker;
	int     	skpmis;
	int		nstations;

} SurfaceObjectType;


typedef struct {

	char    	snparm[GEMPAKSTRING];
	char		dattim[GEMPAKSTRING];
	char		levels[GEMPAKSTRING];
	char		vcoord[GEMPAKSTRING];
	char    	colors[GEMPAKSTRING];
	char    	marker[GEMPAKSTRING];
	char    	filter[GEMPAKSTRING];
	char		text[GEMPAKSTRING];
	int		skpmis;
	BooleanType	tfilter;
	int		nstations;

} UpperairObjectType;

typedef struct {

	char		dattim[GEMPAKSTRING];
	char		stndex[GEMPAKSTRING];
	char		stncol[GEMPAKSTRING];
	char		vcoord[GEMPAKSTRING];
	char		colors[GEMPAKSTRING];
	char		wind_symbol[GEMPAKSTRING];
	char		symbol_size[GEMPAKSTRING];
	char		symbol_width[GEMPAKSTRING];
	char		line_type[GEMPAKSTRING];
	char		line_width[GEMPAKSTRING];
	char		winpos[GEMPAKSTRING];
	char		ptype[GEMPAKSTRING];
	char		xaxis[GEMPAKSTRING];
	char		yaxis[GEMPAKSTRING];
	char		marker[GEMPAKSTRING];
	char		filter[GEMPAKSTRING];
	char		text[GEMPAKSTRING];
	BooleanType	tfilter;

} UpaProfileObjectType;

typedef struct {
	
	char		line_color[GEMPAKSTRING];
	char		line_type[GEMPAKSTRING];
	char		line_width[GEMPAKSTRING];
	char		lat_inc[GEMPAKSTRING];
	char		lon_inc[GEMPAKSTRING];
	char		label_freq[GEMPAKSTRING];

} LatLonGridObjectType;

typedef struct {

	char		mapfile[FILENAMESTRING];
	char		line_color[GEMPAKSTRING];
	char		line_type[GEMPAKSTRING];
	char		line_width[GEMPAKSTRING];

} MapObjectType;


typedef union {
	AnyObjectType			anyObj;
	SatObjectType			satObj;
	RadarObjectType			radObj;
	RadarRingObjectType		rringObj;
	SurfaceObjectType		surfaceObj;
	MapObjectType			mapObj;
	UpaProfileObjectType		upapObj;
	UpperairObjectType		upaObj;
	MarkerObjectType		mrkObj;
	WindProfilerObjectType		wproObj;
	VectorGridObjectType		vgribObj;
	ScalarGridObjectType		sgribObj;
	XSectionGridObjectType		xsecObj;
	VertProfileObjectType		vprofObj;
	TimeHeightObjectType		thObj;
	BackgroundImageObjectType	bgimgObj;
} MetObjectContentType;


/*
 *	Enumerated type of available graphics data that can be
 *	drawn into a pixmap.  More than one of these can be
 *	associated with a pixmaps to overlay grahics.
 */
typedef struct {

	int			type;

	int			inUse;         /* in use? */
	int			drawnFlag;     /* has been drawn? */
	int			toggledOn;     /* clicked off by user? */
	int			titleIndex;
	AbsTime			refTime;
	AbsTime			fcstTime;
	AbsTime			validTime;

	char			timestamp[32];
	char			filename[FILENAMESTRING];
	char			template[FILENAMESTRING];
	int			(*displayFunc)();
	void			(*destroyFunc)();

	char			proj[GEMPAKSTRING];
	char			area[GEMPAKSTRING];
	char			garea[GEMPAKSTRING];

	char			titleString[FILENAMESTRING];

	MetObjectContentType	*metObjectContent;

} MetObjectType;


typedef struct {

	int		inUse;        /* 0 if not in use, !0 otherwise */

	int		inLoop;	      /* 1 = include this frame in a loop */
	                              /* 0 = omit from loop               */

	Display		*display;
	Dimension	width;
	Dimension	height;
	Pixmap		pixmap;

	char		proj [10];
	MapProjType	mapProjection;
	SatProjType	satProjection;
	ReferencePts	referenceLatLons;

	int		numMetObjects;
	MetObjectType	*metObjects[MAXMETOBJS];

} PixmapObjectType;

/*
 *	Per X window (or drawing area) data structure.
 */
typedef struct WindowObjectS {
					/*  window background color? */
	int			inUse;	/* 0 if in use; !0 otherwise */
	char			windowName[STDSTRINGSIZE];
	Widget			canvas;
	int			abortFlag;
	int                     processExposeEventsFlag;
	BooleanType		showPartialFrameFlag;

	XtAppContext		appContext;
	Window			windowID;
	Display			*display;
	Dimension		width;
	Dimension		height;
	GC			gc;
	int			root;
	int			depth;

	Pixmap			backgroundPixmap;
	int			numPixmapObjects;
	int			activePixmapObjectIndex;
	PixmapObjectType	*pixmapObjects[MAXPIXMAPOBJS]; 

	GeoRefObjectType	*geoRefObject;

	AnimationObjectType	*animationObject; 

	TitleObjectType		*titleObject;

	DataTimeObjectType	*dataTimeObject;

} WindowObjectType;

typedef struct {

	time_t			timestamp;
	int			metObjectType;
	MetObjectType		*metObject;

} TimestampInfoType;


/*
 *	Macro's to access "hidden" elements of the WindowObject
 *	data structure.
 */
#define GetActivePixmapObjectIndex(w)    ((w)->activePixmapObjectIndex)

#define GetWindowObjectNumPixmapObjects(w)    ((w)->numPixmapObjects)

#define GetWindowObjectPixmapObjects(w)  ((w)->pixmapObjects)

#define SetWindowObjectCanvas(w,c)       ((w)->canvas = (c))
#define GetWindowObjectCanvas(w)         ((w)->canvas)

#define SetWindowObjectDisplay(w,c)      ((w)->display = (c))
#define GetWindowObjectDisplay(w)        ((w)->display)

#define GetWindowObjectAppContext(w)     ((w)->appContext)

#define SetWindowObjectName(w,c)         (strcpy((w)->windowName, (c))
#define GetWindowObjectName(w)           ((w)->windowName)

#define SetWindowObjectWidth(w,c)        ((w)->width = (c))
#define GetWindowObjectWidth(w)          ((w)->width)

#define SetWindowObjectHeight(w,c)       ((w)->height = (c))
#define GetWindowObjectHeight(w)         ((w)->height)

#define SetWindowObjectRoot(w,c)         ((w)->root = (c))
#define GetWindowObjectRoot(w)           ((w)->root)

#define SetWindowObjectDepth(w,c)        ((w)->depth = (c))
#define GetWindowObjectDepth(w)          ((w)->depth)

#define SetWindowObjectGC(w,c)           ((w)->gc = (c))
#define GetWindowObjectGC(w)             ((w)->gc)

#define SetWindowObjectWindowID(w,c)     ((w)->windowID = (c))
#define GetWindowObjectWindowID(w)       ((w)->windowID)

#define SetWindowObjectAbortFlag(w,c)    ((w)->abortFlag = (c))
/* #define GetWindowObjectAbortFlag(w)      ((w)->abortFlag) */

/*
 *	Global data
 */
extern WindowObjectType		mainWindow[];

/*
 *	WindowObjectType access functions.
 */
void        SetWindowObjectXstuff( WindowObjectType *wo );
void        InitWindowObjects( WindowObjectType *wo, int cnt );

Pixmap      CreatePixmapFromWindowObject( WindowObjectType *wo );
Pixmap      GetWindowObjectBackgroundPixmap(WindowObjectType *wo);

Pixmap      GetActivePixmap(WindowObjectType *wo);
void        SetActivePixmapObjectIndex( WindowObjectType *wo, int index );
void        SetActivePixmapObject( WindowObjectType *wo, Pixmap pixmap );

Pixmap      *GetWindowObjectPixmapList( WindowObjectType *wo );


WindowObjectType *CreateWindowObject( char *windowName );
WindowObjectType *GetActiveWindowObject();
WindowObjectType *GetWindowObjectByName( char *windowName );
WindowObjectType *SetActiveWindowObject( WindowObjectType *wo );
WindowObjectType *SetActiveWindowObjectByName( char *windowName );
WindowObjectType *GetWindowObjectByWidget( Widget w );
void             WindowObjectResize( WindowObjectType *wo, 
                                     Dimension width, Dimension height );

/*
 *	PixmapObjectType access functions
 *
 */
PixmapObjectType *GetNextPot( WindowObjectType *wo );
PixmapObjectType *GetActivePixmapObject( WindowObjectType *wo );
void PixmapObjectResize( WindowObjectType	*wo, 
			PixmapObjectType	*pot,
			Dimension		width,
			Dimension		height);

#define GetPixmapObjectPixmaps(p)          ((p)->pixmap)


/*
 *	MetObjectType access functions
 *
 */
MetObjectType *GetNextMot( PixmapObjectType *pot );
void          FreeMot( MetObjectType *m);
void          ReleaseMot( MetObjectType *m);
MetObjectType *AllocateMot();
MetObjectType *CloneMot();
int           GetMetObjectClass( int metObjectType );

#define GetMetObjectType(x)            ((x)->type)

#define GetMetObjectDrawnFlag(x)     ((x)->drawnFlag)
#define SetMetObjectDrawnFlag(x,c)     ((x)->drawnFlag = (c))



/*
 *
 *	MetObject functions.
 */
void DestroySurfaceObject( MetObjectType *mot );
void DestroyUpperairObject( MetObjectType *mot );
void DestroyUpaProfileObject( MetObjectType *mot );
void DestroyMapObject( MetObjectType *mot );
void DestroyLatLonGridObject( MetObjectType *mot );
void DestroyTimeHeight ( MetObjectType *mot );
void DestroyVerticalProfile ( MetObjectType *mot );
void DestroySatObject ( MetObjectType *mot );
void DestroyRadarObject ( MetObjectType *mot );
void DestroyBackgroundImageObject ( MetObjectType *mot );
void DoImageObjects( int metObjectType, char *path, int cnt, char **filev );


MetObjectType *MakeSatObject( int, char *, char *, char *, int, char *);
MetObjectType *MakeRadarObject( int, char *, char *, char *, int, char *);
MetObjectType *MakeBackgroundImageObject( int, char *, char *, char *, int, char *);
MetObjectType *MakeBackgroundMapObject( int, char * );
MetObjectType **MakeImageObjectList( int, int, char **, char ** );

MetObjectType *MakeMapObject( char *, char *, char *, char *, char * );
MetObjectType *MakeLatLonGridObject( char *, char *, char *, char *,
				     char *, char * );
MetObjectType *MakeRadarRingObject ( int, int, float, float,
			             char*, char *, int, int * );
MetObjectType *MakeSurfaceObject ( char *, char *, char *, char *,
                                   char *, char *,
                                   char *,char *, int, int, int, int );
MetObjectType *MakeUpperairObject ( char *, char *, char *, char *,
                                    char *, char *, char *, char *,
                                    char *, char *, int, int, BooleanType );
MetObjectType *MakeUpaProfileObject ( char *, char *, char *, char *,
                                      char *, char *, char *, char *,
                                      char *, char *, char *, char *,
                                      char *, char *, char *, char *,
                                      char *, char *, char *, int   ,
				      BooleanType );

MetObjectType *
MakeHorizontalScalarGridObject( char *,	char *, char *,	char *, char *,
				char *,	char *, char *, char *, char *,
			        char *, char *, char *, char *, char *,
				char *, char *, char *, char *, char *,
				char *, char *, char *, char *, char *,
				char *, char *, char *, char *, int   ,
				int   , BooleanType,
				BooleanType, BooleanType, BooleanType );

MetObjectType *
MakeTimeHeightObject ( char *, char *, char *, char *, char *,
                       char *, char *, char *, char *, char *,
                       char *, char *, char *, char *, char *,
                       char *, char *, char *, char *, char *,
                       char *, char *, char *, char *, char *,
		       char *, int );

MetObjectType *
MakeVerticalProfileObject ( char *, char *, char *, char *, char *,
                            char *, char *, char *, char *, char *,
                            char *, char *, char *, char *, char *,
                            char *, char *, char *, char *, char *,
                            char *, char *, char *, char *, char *,
			    char *, char *, char *, int );

MetObjectType *
MakeXSectionGridObject (char *, char *, char *, char *, char *,
			char *, char *, char *, char *, char *,
			char *, char *, char *, char *, char *,
			char *, char *, char *, char *, char *,
			char *, char *, char *, char *, char *,
			char *, char *, char *, char *, char *,
			char *, char *, char *, 
			int   , BooleanType, BooleanType );

MetObjectType *
MakeHorizontalVectorGridObject ( char *, char *, char *, char *, char *,
				 char *, char *, char *, char *, char *,
				 char *, char *, char *, char *, char *,
				 char *, char *, char *, char *, char *,
				 char *, int   , int );
MetObjectType *
MakeWindProfilerObject (char *, char **,int   , char *, char *,
			char *, char *, char *, char *, char *,
		        char *, int );
/*
 *
 *   Object Management routines
 *
 */
void
AddTimeMatchedObjects( WindowObjectType *wo, int cnt, TimestampInfoType *tsi);
void AddObjectListToDisplay( int cnt, MetObjectType **mlist );
PixmapObjectType *GetTimeMatchedPot( WindowObjectType *wo, time_t timestamp );
GeoRefObjectType *GetGeoRefObject( WindowObjectType *wo );
GeoRefObjectType *CreateGeoRefObject();

void UpdateTitleObject( WindowObjectType *wo );
void GetMetObjectTitles( PixmapObjectType *pot, int *cnt, char ***titlev);

/*
 *	Callbacks that  interface with GEMPAK libraries.
 *
 */
int	DisplaySurface( WindowObjectType *mw, MetObjectType *mot );
int	DisplayUpperair( WindowObjectType *mw, MetObjectType *mot );
int	DisplayUpaProfile( WindowObjectType *mw, MetObjectType *mot );
int	DisplayScalarGrid ( WindowObjectType *mw, MetObjectType *mot );
int	DisplayVectorGrid ( WindowObjectType *mw, MetObjectType *mot );
int	DisplayWindProfiler ( WindowObjectType *mw, MetObjectType *mot );
int	DisplayTimeHeight ( WindowObjectType *mw, MetObjectType *mot );
int	DisplayVerticalProfile ( WindowObjectType *mw, MetObjectType *mot );
int	DisplayXSection ( WindowObjectType *mw, MetObjectType *mot );
int	DisplayBackgroundImage( WindowObjectType *mw, MetObjectType *mot );
int	DisplayRadar( WindowObjectType *mw, MetObjectType *mot );
int	DisplaySat( WindowObjectType *mw, MetObjectType *mot );
int	DisplayBackgroundObject( WindowObjectType *mw, MetObjectType *mot );
int	DisplayMarker( WindowObjectType *mw, MetObjectType *mot );
int	DisplayRadarRing( WindowObjectType *mw, MetObjectType *mot );

/*
 *	Animation control functions
 */
void AnimateCB( Widget w, XtPointer client, XtPointer calldata );
void AnimationObjectStop( WindowObjectType *wo );
void AnimationObjectPlayFwd( WindowObjectType *wo );
void AnimationObjectPlayBack( WindowObjectType *wo );
void AnimationObjectStepFwd( WindowObjectType *wo );
void AnimationObjectStepBack( WindowObjectType *wo );
AnimationObjectType *AnimationObjectCreate( WindowObjectType *wo );

/*
 *	External Macro's for animation control
 */
#define AnimationRunning(x)	( ( (x)->animationObject) && \
					( (x)->animationObject->isAnimating) )

#define GetAnimationCommand(x)	( ( (x)->animationObject) ? \
				   ( (x)->animationObject->command ) : \
				     NOLOOP )

XmString *CreateMotifStringTable( int cnt, char *argv[], char *fontTag );
void FreeMotifStringTable( int cnt, XmString *xmstrs );
TitleObjectType *TitleObjectAllocate();
DataTimeObjectType *GetDataTimeObject();

#endif	/* _WINOBJ_H */
