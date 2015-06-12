/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	timematch.h
 *
 *	Structure for saving time match widget info.
 *
 *	History:
 *
 *	11/97	COMET	Original
 *
 ***********************************************************************/


#ifndef _TIMEMATCH_H
#define _TIMEMATCH_H


#include "children.h"

/*
 *	Dialogs.
 */

typedef struct {
	ButtonObject		none_b;		/* no time matching */
	ButtonObject		strict_b;	/* strict time match */
	ButtonObject		closest_b;	/* closest time match */
	ButtonObject		all_b;		/* all forecast times */
	ButtonObject		analysis_b;	/* analysis only times */
} GuiTimeMatchDialogType;


GuiTimeMatchDialogType *GetGuiTimeMatchDialog();

/*
 * Widget ID setting and getting macros
 */

#define SetTimeMatchButton1W(t,c)		((t)->none_b.w=(c))
#define GetTimeMatchButton1W(t)			((t)->none_b.w)
#define SetTimeMatchButton2W(t,c)		((t)->strict_b.w=(c))
#define GetTimeMatchButton2W(t)			((t)->strict_b.w)
#define SetTimeMatchButton3W(t,c)		((t)->closest_b.w=(c))
#define GetTimeMatchButton3W(t)			((t)->closest_b.w)

#define SetModelTimesButton1W(t,c)		((t)->all_b.w=(c))
#define GetModelTimesButton1W(t)		((t)->all_b.w)
#define SetModelTimesButton2W(t,c)		((t)->analysis_b.w=(c))
#define GetModelTimesButton2W(t)		((t)->analysis_b.w)

#endif
