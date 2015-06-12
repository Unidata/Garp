/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	upaobj.h
 *
 *	Structure for saving upa data widget info.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/97	COMET	Added plot_type to distinguish between plan view
 *			and soundings.
 *	 2/97	COMET	Added sanity tests to strdup and free macros.
 *	 8/97	COMET	Added ListIndexObjectType for time matching.
 *	11/97	COMET	Added timelistlbl
 *	12/97	COMET	Added Set/GetUpaDialogW()
 *	12/97	COMET	Added select_list object for date/time lists.
 *
 ***********************************************************************/


#ifndef _UPAOBJ_H
#define _UPAOBJ_H

#include "children.h"


/*
 *	Dialogs.
 */

typedef struct {
	LabelObjectType		level;		/* Vertical level	*/
	ListParentObjectType	time_list;	/* List of date/times	*/
	ListIndexObjectType	match_list;	/* Time matched indices */
	ListIndexObjectType	select_list;	/* Selected indices     */
	ButtonParentObjectType	level_b;	/* Level buttons	*/
	ToggleButtonObjectType	time_filter;	/* 00Z & 12Z toggle	*/
	ToggleButtonObjectType	temp;		/* Temperature toggle	*/
	ToggleButtonObjectType	dwpt;		/* Dew Point toggle	*/
	ToggleButtonObjectType	stid;		/* Station ID toggle	*/
	ToggleButtonObjectType	height;		/* Geopotential Height	*/
	ToggleButtonObjectType	hght_change;	/* 12-hr Height Change	*/
	ToggleButtonObjectType	wind;		/* Wind toggle		*/
	ToggleButtonObjectType	ms;		/* Wind units in m/s	*/
	ToggleButtonObjectType	knots;		/* Wind units in knots	*/
	ToggleButtonObjectType	barb;		/* Barb symbol		*/
	ToggleButtonObjectType	arrow;		/* Arrow symbol		*/
	ButtonParentObjectType  ptype_om;	/* Projection menu	*/
	ButtonObject		plot_type;	/* Projection		*/
	LabelObjectType		timelistlbl;	/* Time list label	   */
	Widget			main_dialog;	/* main dialog 		   */
} GuiUpperairDialogType;



GuiUpperairDialogType *GetGuiUpperairDialog();

void TimeButtonCB ( Widget w, XtPointer clientData, XtPointer callData );
void SetModelLevelCB ( Widget w, XtPointer clientData, XtPointer callData );


#define SetUpaLevelLabel(u,c)			((u)->level.string = (c) ?	\
						 strdup(c) : (c))
#define GetUpaLevelLabel(u)			((u)->level.string)
#define FreeUpaLevelLabel(u)			((u)->level.string ?		\
						 free((u)->level.string) : (void) 1)
#define SetUpaLevelLabelW(u,c)			((u)->level.w=(c))
#define GetUpaLevelLabelW(u)			((u)->level.w)

#define SetUpaScrolledListW(u,c)		((u)->time_list.w=(c))
#define GetUpaScrolledListW(u)			((u)->time_list.w)

#define SetUpaTimeFilterW(u,c)			((u)->time_filter.w=(c))
#define GetUpaTimeFilterW(u)			((u)->time_filter.w)

#define SetUpaTempW(u,c)			((u)->temp.w=(c))
#define GetUpaTempW(u)				((u)->temp.w)

#define SetUpaDwptW(u,c)			((u)->dwpt.w=(c))
#define GetUpaDwptW(u)				((u)->dwpt.w)

#define SetUpaStidW(u,c)			((u)->stid.w=(c))
#define GetUpaStidW(u)				((u)->stid.w)

#define SetUpaHeightW(u,c)			((u)->height.w=(c))
#define GetUpaHeightW(u)			((u)->height.w)

#define SetUpaHeightChW(u,c)			((u)->hght_change.w=(c))
#define GetUpaHeightChW(u)			((u)->hght_change.w)

#define SetUpaWindW(u,c)			((u)->wind.w=(c))
#define GetUpaWindW(u)				((u)->wind.w)

#define SetUpaWindUnitsMsW(u,c)			((u)->ms.w=(c))
#define GetUpaWindUnitsMsW(u)			((u)->ms.w)

#define SetUpaWindUnitsKnotsW(u,c)		((u)->knots.w=(c))
#define GetUpaWindUnitsKnotsW(u)		((u)->knots.w)

#define SetUpaWindSymbolBarbW(u,c)		((u)->barb.w=(c))
#define GetUpaWindSymbolBarbW(u)		((u)->barb.w)

#define SetUpaWindSymbolArrowW(u,c)		((u)->arrow.w=(c))
#define GetUpaWindSymbolArrowW(u)		((u)->arrow.w)

#define SetUpaPlotTypeOMW(u,c)			((u)->ptype_om.parent=(c))
#define GetUpaPlotTypeOMW(u)			((u)->ptype_om.parent)

#define SetUpaPlotType(u,c)			((u)->plot_type.i=(c))
#define GetUpaPlotType(u)			((u)->plot_type.i)
#define SetUpaPlotTypeW(u,c)			((u)->plot_type.w=(c))
#define GetUpaPlotTypeW(u)			((u)->plot_type.w)

#define	SetUpaTimeListLabelW(s,c)		((s)->timelistlbl.w=(c))
#define	GetUpaTimeListLabelW(s)			((s)->timelistlbl.w)

#define	SetUpaDialogW(s,c)			((s)->main_dialog=(c))
#define	GetUpaDialogW(s)			((s)->main_dialog)

#endif  /*   _UPAOBJ_H   */
