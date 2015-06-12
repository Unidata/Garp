/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	sfcobj.h
 *
 *	Structure for saving sfc data widget info.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 7/97	COMET	Dynamically initialize surface station parameters.
 *	 8/97	COMET	Added ListIndexObjectType for time matching.
 *	11/97	COMET	Added timelistlbl
 *	12/97	COMET	Added Set/GetSfcDialogW()
 *	12/97	COMET	Added select_list object for date/time lists.
 *
 ***********************************************************************/


#ifndef _SFCOBJ_H
#define _SFCOBJ_H

#include "children.h"


/*
 *	Dialogs.
 */

typedef struct {
	ListParentObjectType	time_list;	/* List of date/times	*/
	ListIndexObjectType	match_list;	/* Time matched indices */
	ListIndexObjectType	select_list;	/* Selected indices     */
	ListParentObjectType	parm_list;	/* Station Parameters	*/
	ToggleButtonObjectType  color;		/* Multi-colored plot	*/
	ToggleButtonObjectType  marker;		/* Display marker	*/
	ButtonParentObjectType	marker_type_pm;	/* Marker type menu	*/
	TextFieldObjectType	position_1;	/* Position 1 parameter	*/
	TextFieldObjectType	position_2;	/* Position 2 parameter	*/
	TextFieldObjectType	position_3;	/* Position 3 parameter	*/
	TextFieldObjectType	position_4;	/* Position 4 parameter	*/
	TextFieldObjectType	position_5;	/* Position 5 parameter	*/
	TextFieldObjectType	position_6;	/* Position 6 parameter	*/
	TextFieldObjectType	position_7;	/* Position 7 parameter	*/
	TextFieldObjectType	position_8;	/* Position 8 parameter	*/
	TextFieldObjectType	position_9;	/* Position 9 parameter	*/
	TextFieldObjectType	position_10;	/* Position 10 parameter*/
	TextFieldObjectType	position_11;	/* Position 11 parameter*/
	ButtonParentObjectType	plot_type_pm;	/* Plot type menu	*/
	LabelObjectType		timelistlbl;	/* Time list label	*/
	Widget			main_dialog;	/* main dialog 		   */
} GuiSfcObsDialogType;



GuiSfcObsDialogType *GetGuiSfcObsDialog();


#define SetSfcScrolledListW(s,c)		((s)->time_list.w=(c))
#define GetSfcScrolledListW(s)			((s)->time_list.w)

#define SetSfcTimeMatchListW(s,c)		((s)->match_list.w=(c))
#define GetSfcTimeMatchListW(s)			((s)->match_list.w)

#define SetSfcParameterListW(s,c)		((s)->parm_list.w=(c))
#define GetSfcParameterListW(s)			((s)->parm_list.w)

#define SetSfcColorW(s,c)			((s)->color.w=(c))
#define GetSfcColorW(s)				((s)->color.w)

#define SetSfcMarkerW(s,c)			((s)->marker.w=(c))
#define GetSfcMarkerW(s)			((s)->marker.w)

#define SetSfcMarkerTypePmSFW(s,c)	((s)->marker_type_pm.parent=(c))
#define GetSfcMarkerTypePmSFW(s)	((s)->marker_type_pm.parent)
#define SetSfcMarkerTypeOmSFW(s,c)	((s)->marker_type_pm.menu=(c))
#define GetSfcMarkerTypeOmSFW(s)	((s)->marker_type_pm.menu)
#define SetSfcMarkerType(s,c)		((s)->marker_type_pm.string = (c) ? \
					 strdup(c) : (c))
#define GetSfcMarkerType(s)		((s)->marker_type_pm.string)
#define FreeSfcMarkerType(s)		((s)->marker_type_pm.string ?	  \
				free((s)->marker_type_pm.string) : (void) 1)

#define SetSfcParm1(s,c)			((s)->position_1.w=(c))
#define GetSfcParm1(s)				((s)->position_1.w)

#define SetSfcParm2(s,c)			((s)->position_2.w=(c))
#define GetSfcParm2(s)				((s)->position_2.w)

#define SetSfcParm3(s,c)			((s)->position_3.w=(c))
#define GetSfcParm3(s)				((s)->position_3.w)

#define SetSfcParm4(s,c)			((s)->position_4.w=(c))
#define GetSfcParm4(s)				((s)->position_4.w)

#define SetSfcParm5(s,c)			((s)->position_5.w=(c))
#define GetSfcParm5(s)				((s)->position_5.w)

#define SetSfcParm6(s,c)			((s)->position_6.w=(c))
#define GetSfcParm6(s)				((s)->position_6.w)

#define SetSfcParm7(s,c)			((s)->position_7.w=(c))
#define GetSfcParm7(s)				((s)->position_7.w)

#define SetSfcParm8(s,c)			((s)->position_8.w=(c))
#define GetSfcParm8(s)				((s)->position_8.w)

#define SetSfcParm9(s,c)			((s)->position_9.w=(c))
#define GetSfcParm9(s)				((s)->position_9.w)

#define SetSfcParm10(s,c)			((s)->position_10.w=(c))
#define GetSfcParm10(s)				((s)->position_10.w)

#define SetSfcParm11(s,c)			((s)->position_11.w=(c))
#define GetSfcParm11(s)				((s)->position_11.w)

#define SetSfcPlotTypePmSFW(s,c)	((s)->plot_type_pm.parent=(c))
#define GetSfcPlotTypePmSFW(s)		((s)->plot_type_pm.parent)
#define SetSfcPlotTypeOmSFW(s,c)	((s)->plot_type_pm.menu=(c))
#define GetSfcPlotTypeOmSFW(s)		((s)->plot_type_pm.menu)
#define SetGuiSfcObsType(s,c)		((s)->plot_type_pm.string = (c) ? \
					 strdup(c) : (c))
#define GetGuiSfcObsType(s)		((s)->plot_type_pm.string)
#define FreeGuiSfcObsType(s)		((s)->plot_type_pm.string ?	  \
				 free((s)->plot_type_pm.string) : (void) 1)
#define SetGuiSfcObsLabel(s,c)		((s)->plot_type_pm.label = (c) ? \
					 strdup(c) : (c))
#define GetGuiSfcObsLabel(s)		((s)->plot_type_pm.label)
#define FreeGuiSfcObsLabel(s)		((s)->plot_type_pm.label ?	  \
				 free((s)->plot_type_pm.label) : (void) 1)

#define	SetSfcTimeListLabelW(s,c)		((s)->timelistlbl.w=(c))
#define	GetSfcTimeListLabelW(s)			((s)->timelistlbl.w)

#define	SetSfcDialogW(s,c)			((s)->main_dialog=(c))
#define	GetSfcDialogW(s)			((s)->main_dialog)


#endif  /*   _SFCOBJ_H   */
