/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	hvobj.h
 *
 *	Vector grid structure thing.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/

#ifndef _HVOBJ_H
#define _HVOBJ_H

#include "children.h"


/*
 *	Dialogs.
 */

typedef struct {
	ListParentObjectType	field_list;	/* List of fields 	*/
} GuiVectorGridDialogType;


GuiVectorGridDialogType *GetGuiVectorGridDialog();


#define SetScrolledListHVW(g,c)		((g)->field_list.w=(c))
#define GetScrolledListHVW(g)		((g)->field_list.w)

#endif  /*   _HVOBJ_H   */
