/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	enhance.h
 *
 *	Structure for saving image enhancement widget info.
 *
 *	History:
 *
 *	6/97	COMET	Renamed from imgobj.h
 *
 ***********************************************************************/


#ifndef _ENHANCE_H
#define _ENHANCE_H

#define MAIN_CONTROL_FADER	1
#define ENHANCE_POPUP_FADER	2

#include "children.h"

/*
 *	Dialogs.
 */

typedef struct {
	ScaleObjectType		roll;		/* roll scale info */
	ScaleObjectType		fade;		/* fade scale info */
	ScaleObjectType		sat_fade;	/* fade scale info */
	ToggleButtonObjectType	invert;		/* color invert	   */
} GuiImageEnhanceDialogType;


GuiImageEnhanceDialogType *GetGuiImageEnhanceDialog();


#define SetImageRollScaleW(s,c)			((s)->roll.w=(c))
#define GetImageRollScaleW(s)			((s)->roll.w)
#define SetImageRollScaleValue(s,c)		((s)->roll.value=(c))
#define GetImageRollScaleValue(s)		((s)->roll.value)

#define SetImageFadeScaleW(s,c)			((s)->fade.w=(c))
#define GetImageFadeScaleW(s)			((s)->fade.w)
#define SetImageFadeScaleValue(s,c)		((s)->fade.value=(c))
#define GetImageFadeScaleValue(s)		((s)->fade.value)

#define SetSatImageFadeScaleW(s,c)		((s)->sat_fade.w=(c))
#define GetSatImageFadeScaleW(s)		((s)->sat_fade.w)
#define SetSatImageFadeScaleValue(s,c)		((s)->sat_fade.value=(c))
#define GetSatImageFadeScaleValue(s)		((s)->sat_fade.value)

#define SetColorInvertW(s,c)			((s)->invert.w=(c))
#define GetColorInvertW(s)			((s)->invert.w)
#define SetColorInvertToggle(s,c)		((s)->invert.toggled=(c))
#define GetColorInvertToggle(s)			((s)->invert.toggled)

#endif
