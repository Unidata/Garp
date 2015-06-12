/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	lineattr.h
 *
 *	Line attribute dialog components
 *
 *	History:
 *
 *	10/97	COMET	Original copy
 *
 ***********************************************************************/


#ifndef _LINEATTR_H
#define _LINEATTR_H

#include "winobj.h"
#include "children.h"

void SetLineAttributesTypeCB  ( Widget, XtPointer, XtPointer );
void SetLineAttributesColorCB ( Widget, XtPointer, XtPointer );
void SetLineAttributesWidthCB ( Widget, XtPointer, XtPointer );
void PopLineAttributesDialogCB ( Widget, XtPointer, XtPointer );


/*
 *	Dialogs.
 */

typedef struct {
	ButtonObject	lineWidthB[4];		/* Line width buttons	*/
	ButtonObject	lineTypeB[10];		/* Line type buttons	*/
	ButtonObject	lineColorB[32];		/* Line color buttons	*/
	Widget		sampleLineW; 		/* Sample line widget	*/
	Widget		la_shellW; 		/* line attributes shell widget */
	Widget		la_dialogW; 		/* line attributes dialog widget */
	Widget		applyButton; 		/* line attributes "apply" widget */
	int		currentWidth;		/* current line width	*/
	int		currentType;		/* current line type	*/
	int		currentColor;		/* current line color	*/
	Widget		currentSampleW;		/* current sample line Widget */
	XtCallbackProc	currentApplyCallback;	/* current callback for apply button */
	XtPointer	currentClientData;	/* client data for callback */

} GuiLineAttributesDialogType;

GuiLineAttributesDialogType *GetGuiLineAttributesDialog();


#define SetLineAttributesSampleW(s,c)	((s)->sampleLineW=(c))
#define GetLineAttributesSampleW(s)	((s)->sampleLineW)

#define SetLineAttributesShellW(s,c)	((s)->la_shellW=(c))
#define GetLineAttributesShellW(s)	((s)->la_shellW)

#define SetLineAttributesDialogW(s,c)	((s)->la_dialogW=(c))
#define GetLineAttributesDialogW(s)	((s)->la_dialogW)

#define SetLineAttributesApplyW(s,c)	((s)->applyButton=(c))
#define GetLineAttributesApplyW(s)	((s)->applyButton)

#define SetLineAttributesWidth(s,c)	((s)->currentWidth=(c))
#define GetLineAttributesWidth(s)	((s)->currentWidth)

#define SetLineAttributesType(s,c)	((s)->currentType=(c))
#define GetLineAttributesType(s)	((s)->currentType)

#define SetLineAttributesColor(s,c)	((s)->currentColor=(c))
#define GetLineAttributesColor(s)	((s)->currentColor)

#define SetLineAttributesWidthW(s,i,c)	((s)->lineWidthB[i].w=(c))
#define GetLineAttributesWidthW(s,i)	((s)->lineWidthB[i].w)

#define SetLineAttributesTypeW(s,i,c)	((s)->lineTypeB[i].w=(c))
#define GetLineAttributesTypeW(s,i)	((s)->lineTypeB[i].w)

#define SetLineAttributesColorW(s,i,c)	((s)->lineColorB[i].w=(c))
#define GetLineAttributesColorW(s,i)	((s)->lineColorB[i].w)

#define SetLineAttributesCallback(s,c)	((s)->currentApplyCallback=(c))
#define GetLineAttributesCallback(s)	((s)->currentApplyCallback)

#define SetLineAttributesClientData(s,c) ((s)->currentClientData=(c))
#define GetLineAttributesClientData(s)   ((s)->currentClientData)

#define SetLineAttributesCurrentSampleW(s,c)	((s)->currentSampleW=(c))
#define GetLineAttributesCurrentSampleW(s)	((s)->currentSampleW)

#endif  /*   _LINEATTR_H   */
