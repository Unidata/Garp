/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	upaprof.h
 *
 *	Structure for saving upper air profile info.
 *
 *	History:
 *
 *	 1/97	COMET	Original copy
 *	 4/97	COMET	Changed the names of some macros for consistency
 *			with other data types. Added line type.
 *	11/97	COMET	Added timelistlbl
 *	12/97	COMET	Added Set/GetUpaProfDialogW()
 *	12/97	COMET	Added select_list object for date/time lists.
 *
 ***********************************************************************/


#ifndef _UPAPROF_H
#define _UPAPROF_H

#include "children.h"


/*
 *	Dialogs.
 */

typedef struct {
	ListParentObjectType	time_list;	/* List of date/times	*/
	ListIndexObjectType	match_list;	/* Time matched indices */
	ListIndexObjectType	select_list;	/* Selected indices     */
	ListParentObjectType	station_list;	/* List of stations	*/
	ToggleButtonObjectType	time_filter;	/* 00Z & 12Z toggle	*/
	ButtonParentObjectType	ptype_om;	/* Projection menu	*/
	ButtonObject		plot_type;	/* Projection		*/
	ButtonObject		diagram;	/* Thermodynamic Diagram*/
	ButtonObject		indices;	/* Thermodynamic Indices*/
	ButtonParentObjectType	line_type_pm;	/* Line type menu	*/
	LabelObjectType         timelistlbl;    /* Time list label      */
	Widget			main_dialog;	/* main dialog 		*/
} GuiUpaProfileDialogType;



GuiUpaProfileDialogType *GetGuiUpaProfileDialog();

void TimeButtonCB ( Widget w, XtPointer clientData, XtPointer callData );


#define SetScrolledListUFW(u,c)			((u)->time_list.w=(c))
#define GetScrolledListUFW(u)			((u)->time_list.w)


#define SetTimeFilterUFW(u,c)			((u)->time_filter.w=(c))
#define GetTimeFilterUFW(u)			((u)->time_filter.w)


#define SetStationListUFW(u,c)			((u)->station_list.w=(c))
#define GetStationListUFW(u)			((u)->station_list.w)


#define SetPlotTypeOmUFW(u,c)			((u)->ptype_om.parent=(c))
#define GetPlotTypeOmUFW(u)			((u)->ptype_om.parent)


#define SetPlotTypeUFW(u,c)			((u)->plot_type.w=(c))
#define GetPlotTypeUFW(u)			((u)->plot_type.w)


#define SetDiagramTypeUF(u,c)			((u)->diagram.string = (c) ?	\
						 strdup(c) : (c))
#define GetDiagramTypeUF(u)			((u)->diagram.string)
#define FreeDiagramTypeUF(u)			((u)->diagram.string ?		\
						 free((u)->diagram.string) : (void) 1)


#define SetIndicesUF(u,c)			((u)->indices.string = (c) ?	\
						 strdup(c) : (c))
#define GetIndicesUF(u)				((u)->indices.string)
#define FreeIndicesUF(u)			((u)->indices.string ?		\
						 free((u)->indices.string) : (void) 1)


#define SetLineTypePmUFW(u,c)		((u)->line_type_pm.parent=(c))
#define GetLineTypePmUFW(u)		((u)->line_type_pm.parent)
#define SetLineTypeOmUFW(u,c)		((u)->line_type_pm.menu=(c))
#define GetLineTypeOmUFW(u)		((u)->line_type_pm.menu)
#define SetLineTypeUF(u,c)		((u)->line_type_pm.string = (c) ? \
				         strdup(c) : (c))
#define GetLineTypeUF(u)		((u)->line_type_pm.string)
#define FreeLineTypeUF(u)		((u)->line_type_pm.string ?	  \
					 free((u)->line_type_pm.string) : (void) 1)

#define SetUpaProfTimeListLabelW(s,c)		((s)->timelistlbl.w=(c))
#define GetUpaProfTimeListLabelW(s)		((s)->timelistlbl.w)

#define	SetUpaProfDialogW(s,c)		((s)->main_dialog=(c))
#define	GetUpaProfDialogW(s)		((s)->main_dialog)

#endif  /*   _UPAPROF_H   */
