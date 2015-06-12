/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	maplist.h
 *
 *	Map background list dialog components
 *
 *	History:
 *
 *	10/97	COMET	Original copy
 *	11/97	COMET	Added flag to indicate state changed
 *
 ***********************************************************************/


#ifndef _MAPLIST_H
#define _MAPLIST_H

#include "children.h"
#define	 MAXMAPS	50	



/*
 *	Dialogs.
 */

typedef struct {
	Widget			maplist_formW;		/* form for the names */
	LabelObjectType		name[MAXMAPS];		/* map name label widget*/
	char			*path[MAXMAPS];		/* map path		*/
	ToggleButtonObjectType	on[MAXMAPS];		/* on/off button 	*/
	TextFieldObjectType	maxScale[MAXMAPS];	/* max scale 	*/
	TextFieldObjectType	minScale[MAXMAPS];	/* min scale  	*/
	int			color[MAXMAPS];		/* line color	*/
	int			type[MAXMAPS];		/* line type 	*/
	int			width[MAXMAPS];		/* line width	*/
	Widget			sampleFrameW[MAXMAPS];	/* sample line frame	*/
	Widget			sampleCanvas[MAXMAPS];	/* sample line canvas	*/
	int			nmaps;			/* number of maps in list*/
	BooleanType		changed;		/* indicates state has changed */
} GuiMapBackgroundDialogType;

GuiMapBackgroundDialogType *GetGuiMapBackgroundDialog();


#define SetMapListFormW(s,c)		((s)->maplist_formW=(c))
#define GetMapListFormW(s)		((s)->maplist_formW)

#define SetMapListMapNameW(s,i,c)	((s)->name[i].w=(c))
#define GetMapListMapNameW(s,i)		((s)->name[i].w)

#define SetMapListOnToggleW(s,i,c)	((s)->on[i].w=(c))
#define GetMapListOnToggleW(s,i)	((s)->on[i].w)

#define SetMapListMaxScaleW(s,i,c)	((s)->maxScale[i].w=(c))
#define GetMapListMaxScaleW(s,i)	((s)->maxScale[i].w)

#define SetMapListMinScaleW(s,i,c)	((s)->minScale[i].w=(c))
#define GetMapListMinScaleW(s,i)	((s)->minScale[i].w)

#define SetMapListLineWidth(s,i,c)	((s)->width[i]=(c))
#define GetMapListLineWidth(s,i)	((s)->width[i])

#define SetMapListLineType(s,i,c)	((s)->type[i]=(c))
#define GetMapListLineType(s,i)		((s)->type[i])

#define SetMapListLineColor(s,i,c)	((s)->color[i]=(c))
#define GetMapListLineColor(s,i)	((s)->color[i])

#define SetMapListSampleFrameW(s,i,c)	((s)->sampleFrameW[i]=(c))
#define GetMapListSampleFrameW(s,i)	((s)->sampleFrameW[i])

#define SetMapListSampleCanvasW(s,i,c)	((s)->sampleCanvas[i]=(c))
#define GetMapListSampleCanvasW(s,i)	((s)->sampleCanvas[i])

#define SetMapListCount(s,c)		((s)->nmaps=(c))
#define GetMapListCount(s)		((s)->nmaps)

#define SetMapListChangeState(s,c)	((s)->changed=(c))
#define GetMapListChangeState(s)	((s)->changed)

#endif  /*   _MAPLIST_H   */
