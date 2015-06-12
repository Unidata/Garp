/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	update.h
 *
 *	Structure for saving auto update widget info.
 *
 *	History:
 *
 *	 8/97	COMET	Original
 *	10/97	COMET	Added "isUpdating" flag
 *
 ***********************************************************************/


#ifndef _UPDATE_H
#define _UPDATE_H


#include "children.h"

/*
 *	Dialogs.
 */

typedef struct {
	ScaleObjectType		interval;	/* update interval scale */
	ToggleButtonObjectType	updateOn;	/* update toggle         */
	BooleanType		isUpdating;	/* Updating state flag	 */
} GuiAutoUpdateDialogType;


GuiAutoUpdateDialogType *GetGuiAutoUpdateDialog();

/*
 * Widget ID setting and getting macros
 */

#define SetAutoUpdateToggleW(s,c)		((s)->updateOn.w=(c))
#define GetAutoUpdateToggleW(s)			((s)->updateOn.w)
#define SetAutoUpdateScaleW(s,c)		((s)->interval.w=(c))
#define GetAutoUpdateScaleW(s)			((s)->interval.w)

#define SetAutoUpdateStateFlag(s,c)		((s)->isUpdating=(c))
#define GetAutoUpdateStateFlag(s)		((s)->isUpdating)


/*
 * Value setting and getting macros (not used)
 */

#define SetAutoUpdateScaleValue(s,c)		((s)->interval.value=(c))
#define GetAutoUpdateScaleValue(s)		((s)->interval.value)
#define SetAutoUpdateToggleValue(s,c)		((s)->updateOn.toggled=(c))
#define GetAutoUpdateToggleValue(s)		((s)->updateOn.toggled)


#endif
