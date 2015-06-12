/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	latlongrid.h
 *
 *	Lat/longrid background dialog components
 *
 *	History:
 *
 *	10/97	COMET	Original copy
 *	11/97	COMET	Added flag for state changing
 *
 ***********************************************************************/


#ifndef _LATLONGRID_H
#define _LATLONGRID_H

#include "children.h"

void PopLatLonLineAttributesDialogCB ( Widget, XtPointer, XtPointer );

/*
 *	Dialogs.
 */

typedef struct {
	ToggleButtonObjectType	state;		/* on/off state */
	TextFieldObjectType	latinc;
	TextFieldObjectType	loninc;
	TextFieldObjectType	lblinc;
	int			color;
	int			type;
	int			width;
	Widget			sampleLineW;
	BooleanType		changed;
} GuiLatLonGridDialogType;

GuiLatLonGridDialogType *GetGuiLatLonGridDialog();

#define SetLatLonGridStateW(s,c)	((s)->state.w=(c))
#define GetLatLonGridStateW(s)		((s)->state.w)

#define SetLatIncrementW(s,c)		((s)->latinc.w=(c))
#define GetLatIncrementW(s)		((s)->latinc.w)

#define SetLonIncrementW(s,c)		((s)->loninc.w=(c))
#define GetLonIncrementW(s)		((s)->loninc.w)

#define SetLatLonGridLabelIncW(s,c)	((s)->lblinc.w=(c))
#define GetLatLonGridLabelIncW(s)	((s)->lblinc.w)

#define SetLatLonGridSampleW(s,c)	((s)->sampleLineW=(c))
#define GetLatLonGridSampleW(s)		((s)->sampleLineW)

#define SetLatLonGridWidth(s,c)		((s)->width=(c))
#define GetLatLonGridWidth(s)		((s)->width)

#define SetLatLonGridType(s,c)		((s)->type=(c))
#define GetLatLonGridType(s)		((s)->type)

#define SetLatLonGridColor(s,c)		((s)->color=(c))
#define GetLatLonGridColor(s)		((s)->color)

#define SetLatLonGridChangeState(s,c)	((s)->changed=(c))
#define GetLatLonGridChangeState(s)	((s)->changed)


#endif  /*   _LATLONGRID_H   */
