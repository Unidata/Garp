/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	guiobj.c
 *
 *	Static warehouse of GUI objects.	
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/97	COMET	Added upper air profile object.
 *	 4/97	COMET	Added window preference dialog object.
 *	 6/97	COMET	Changed imgobj.h to enhance.h
 *	10/97	COMET	Added GuiLineAttributesDialogType,
 *			GuiLatLonGridDialogType and GuiMapBackgroundDialogType
 *	11/97	COMET	Added time match dialog.
 *
 ***********************************************************************/


#include "background.h"
#include "command.h"
#include "enhance.h"
#include "fdfobj.h"
#include "framecount.h"
#include "gdinfo.h"
#include "gendef.h"
#include "guimacros.h"
#include "hsobj.h"
#include "hvobj.h"
#include "mainwin.h"
#include "model.h"
#include "_proto.h"
#include "sfcobj.h"
#include "thgtobj.h"
#include "timematch.h"
#include "upaobj.h"
#include "upaprof.h"
#include "vprofobj.h"
#include "wprofobj.h"
#include "winobj.h"
#include "winpref.h"
#include "xsecobj.h"
#include "imgobj.h"
#include "update.h"
#include "lineattr.h"
#include "latlongrid.h"
#include "maplist.h"


static GuiMainWinDialogType		mwn;	/* Main Window 		   */
static GuiBackgroundDialogType		bak;	/* Background objects      */
static ModelInfoType			mdl;	/* Shared Model Info       */
static GuiScalarGridDialogType		grd;	/* Horizontal Scalar Grid */
static GuiVectorGridDialogType		vec;	/* Horizontal Vector Grid  */
static GuiXSectionDialogType 		vcs;	/* Vertical Cross Section  */
static GuiTimeHeightDialogType		tht;	/* Time height section	   */
static GuiSfcObsDialogType		sfc;	/* Surface Obs             */
static GuiUpperairDialogType		upa;	/* Upper Air Obs           */
static GuiUpaProfileDialogType		upf;	/* Upper Air Obs           */
static GuiImageEnhanceDialogType   	enh;	/* Img enhancement dialog  */
static GuiVertProfileObjectType 	vpt;	/* Vertical Profile	   */
static GuiProfilerDialogType		wpf;	/* Wind Profiler 	   */
static GuiCommandDialogType		com;	/* Command info; e.g. save */
static GuiFrameCounterType		frm;	/* frame counter info      */
static GuiWinPrefDialogType		wpi;	/* Preference info.        */
static KeyListType			gdf;	/* Plan view model FDF     */
static KeyListType			xdf;	/* Cross section model FDF */
static KeyListType			tdf;	/* Time height FDF	   */
static KeyListType			vdf;	/* Vertical profile FDF	   */
static GridInfoObjectType		*gio;	/* Model grid info	   */
static GuiAutoUpdateDialogType		upd;	/* Auto Update dialog	   */
static GuiTimeMatchDialogType		tim;	/* Time Match dialog	   */
static GuiImgSelectDialogType		img;	/* Image selection dialog  */
static GuiLineAttributesDialogType	atr;	/* Line attributes dialog  */
static GuiLatLonGridDialogType		llg;	/* lat/lon grid gui dialog */
static GuiMapBackgroundDialogType	mbg;	/* map background gui dialog */



GuiMainWinDialogType	*GetGuiMainWinDialog()
{
	return( &mwn );
}


GuiBackgroundDialogType	*GetGuiBackgroundDialog()
{
	return( &bak );
}


ModelInfoType		*GetGuiModelInfo()
{
	return( &mdl );
}


GuiScalarGridDialogType	*GetGuiScalarGridDialog()
{
        return( &grd );
}


GuiVectorGridDialogType *GetGuiVectorGridDialog()
{
	return( &vec );
}


GuiXSectionDialogType	*GetGuiXSectionDialog()
{
	return( &vcs );
}


GuiTimeHeightDialogType *GetGuiTimeHeightDialog()
{
	return( &tht );
}


GuiSfcObsDialogType	*GetGuiSfcObsDialog()
{
	return( &sfc );
}


GuiUpperairDialogType *GetGuiUpperairDialog()
{
	return( &upa );
}


GuiUpaProfileDialogType *GetGuiUpaProfileDialog()
{
	return( &upf );
}


GuiImageEnhanceDialogType *GetGuiImageEnhanceDialog()
{
	return( &enh );
}


GuiImgSelectDialogType *GetGuiImgSelectDialog()
{
	return( &img );
}


GuiLineAttributesDialogType *GetGuiLineAttributesDialog()
{
	return( &atr );
}

GuiMapBackgroundDialogType *GetGuiMapBackgroundDialog()
{
	return( &mbg );
}

GuiAutoUpdateDialogType *GetGuiAutoUpdateDialog()
{
	return( &upd );
}

GuiTimeMatchDialogType *GetGuiTimeMatchDialog()
{
	return( &tim );
}

GuiVertProfileObjectType *GetGuiVertProfileDialog()
{
	return( &vpt );
}


GuiProfilerDialogType *GetGuiProfilerDialog()
{
	return( &wpf );
}


GuiFrameCounterType *GetGuiFrameCounter()
{
	return( &frm );
}


GuiCommandDialogType *GetGuiCommandDialog()
{
	return( &com );
}


GuiWinPrefDialogType *GetGuiWinPrefDialog()
{
	return( &wpi );
}


KeyListType *GetHSGuiFdfInfo()
{
	return( &gdf );
}


KeyListType *GetXSGuiFdfInfo()
{
	return( &xdf );
}


KeyListType *GetTHGuiFdfInfo()
{
	return( &tdf );
}


KeyListType *GetVPGuiFdfInfo()
{
	return( &vdf );
}


GridInfoObjectType *GetGridInfo()
{
	return( gio );
}


GuiLatLonGridDialogType *GetGuiLatLonGridDialog()
{
	return( &llg );
}

void
PutGio ( GridInfoObjectType *newgio )
{
        gio = newgio;
}
