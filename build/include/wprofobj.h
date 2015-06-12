/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	wprofobj.h
 *
 *	Header file for wind profiler object.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Added sanity tests to strdup and free macros.
 *
 ***********************************************************************/


#ifndef _WPROFOBJ_H
#define _WPROFOBJ_H

#include "children.h"


/*
 *	Dialogs.
 */

typedef struct {
	ListParentObjectType	station_list;	/* List of stations	*/
	ListParentObjectType	dattim_list;	/* List of dattims	*/
	TextFieldObjectType	bottom;		/* Bottom of plot	*/
	TextFieldObjectType	top;		/* Top of plot		*/
	TextFieldObjectType	increment;	/* Tick mark increment	*/
	TextFieldObjectType	end_time;	/* End time for plot	*/
	TextFieldObjectType	duration;	/* Duration for plot	*/
	TextFieldObjectType	time_int;	/* Time between 	*/
} GuiProfilerDialogType;


GuiProfilerDialogType *GetGuiProfilerDialog();


#define SetProfilerScrolledListW(p,c)		((p)->station_list.w=(c))
#define GetProfilerScrolledListW(p)		((p)->station_list.w)


#define SetProfilerBottomW(p,c)			((p)->bottom.w=(c))
#define GetProfilerBottomW(p)			((p)->bottom.w)
#define SetProfilerBottom(p,c)			((p)->bottom.string = (c) ?	\
						 strdup(c) : (c))
#define GetProfilerBottom(p)			((p)->bottom.string)
#define FreeProfilerBottom(p)			((p)->bottom.string ?		\
						 free((p)->bottom.string) : (void)1)


#define SetProfilerTopW(p,c)			((p)->top.w=(c))
#define GetProfilerTopW(p)			((p)->top.w)
#define SetProfilerTop(p,c)			((p)->top.string = (c) ?	\
						 strdup(c) : (c))
#define GetProfilerTop(p)			((p)->top.string)
#define FreeProfilerTop(p)			((p)->top.string ?		\
						 free((p)->top.string) : (void)1)


#define SetProfilerIncrementW(p,c)		((p)->increment.w=(c))
#define GetProfilerIncrementW(p)		((p)->increment.w)
#define SetProfilerIncrement(p,c)		((p)->increment.string = (c) ?	\
						 strdup(c) : (c))
#define GetProfilerIncrement(p)			((p)->increment.string)
#define FreeProfilerIncrement(p)		((p)->increment.string ?		\
						 free((p)->increment.string) : (void)1)


#define SetProfilerEndTimeW(p,c)		((p)->end_time.w=(c))
#define GetProfilerEndTimeW(p)			((p)->end_time.w)
#define SetProfilerEndTime(p,c)			((p)->end_time.string = (c) ?	\
						 strdup(c) : (c))
#define GetProfilerEndTime(p)			((p)->end_time.string)
#define FreeProfilerEndTime(p)			((p)->end_time.string ?		\
						 free((p)->end_time.string) : (void)1)


#define SetProfilerDurationW(p,c)		((p)->duration.w=(c))
#define GetProfilerDurationW(p)			((p)->duration.w)
#define SetProfilerDuration(p,c)		((p)->duration.string = (c) ?	\
						 strdup(c) : (c))
#define GetProfilerDuration(p)			((p)->duration.string)
#define FreeProfilerDuration(p)			((p)->duration.string ?		\
						 free((p)->duration.string) : (void)1)


#define SetProfilerTimeIntervalW(p,c)		((p)->time_int.w=(c))
#define GetProfilerTimeIntervalW(p)		((p)->time_int.w)
#define SetProfilerTimeInterval(p,c)		((p)->time_int.string = (c) ?	\
						 strdup(c) : (c))
#define GetProfilerTimeInterval(p)		((p)->time_int.string)
#define FreeProfilerTimeInterval(p)		((p)->time_int.string ?		\
						 free((p)->time_int.string) : (void)1)


#endif  /*   _WPROFOBJ_H   */
