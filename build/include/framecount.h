/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	framecount.h
 *
 *	Frame counter widget info saving structure.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#ifndef _FRAMECOUNT_H
#define _FRAMECOUNT_H

#include "children.h"

/*
 *	Dialogs.
 */

typedef struct {
	ButtonObject		frmcount;	/* The frame counter button */
	ButtonParentObjectType	frmbuttons;	/* Frame button info        */
	Widget			counter_frameW; /* frame counter frame widget ID */
	Widget			current_frameW; /* current frame label widget ID */
	Widget			remove_buttonW; /* remove button widget ID   */
	Widget			undo_buttonW;   /* undo button widget ID     */
	BooleanType		buildButtons;	/* flag to know whether to build */
						/* new frame buttons         */
	Widget			speed_scale;	/* Speed scale slider widget */
	Widget			dwell_first;	/* First dwell slider widget */
	Widget			dwell_middle;	/* Middle dwell slider widget */
	Widget			dwell_last;	/* Last dwell slider widget */

} GuiFrameCounterType;


GuiFrameCounterType *GetGuiFrameCounter();


#define SetFrameCounterW(s,c)			((s)->frmcount.w=(c))
#define GetFrameCounterW(s)			((s)->frmcount.w)
#define SetFrameCounterLabel(s,c)		((s)->frmcount.label=strdup(c))
#define FreeFrameCounterLabel(s,c)		(free((s)->frmcount.label))

#define SetCounterFrameW(s,c)			((s)->counter_frameW=(c))
#define GetCounterFrameW(s)			((s)->counter_frameW)

#define SetCurrentFrameLabelW(s,c)		((s)->current_frameW=(c))
#define GetCurrentFrameLabelW(s)		((s)->current_frameW)

#define SetFrameEditRemoveW(s,c)		((s)->remove_buttonW=(c))
#define GetFrameEditRemoveW(s)			((s)->remove_buttonW)

#define SetFrameEditUndoW(s,c)			((s)->undo_buttonW=(c))
#define GetFrameEditUndoW(s)			((s)->undo_buttonW)

#define SetBuildButtonsFlag(s,c)		((s)->buildButtons=(c))
#define GetBuildButtonsFlag(s)			((s)->buildButtons)

#define SetSpeedScaleW(s,c)			((s)->speed_scale=(c))
#define GetSpeedScaleW(s)			((s)->speed_scale)

#define SetDwellFirstW(s,c)			((s)->dwell_first=(c))
#define GetDwellFirstW(s)			((s)->dwell_first)

#define SetDwellMiddleW(s,c)			((s)->dwell_middle=(c))
#define GetDwellMiddleW(s)			((s)->dwell_middle)

#define SetDwellLastW(s,c)			((s)->dwell_last=(c))
#define GetDwellLastW(s)			((s)->dwell_last)


#endif  /*   _FRAMECOUNT_H   */
