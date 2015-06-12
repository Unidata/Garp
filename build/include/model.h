/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	model.h
 *
 *	Model data info saving structure.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Added sanity tests to strdup and free macros.
 *	11/97	COMET	Added model_label.
 *
 ***********************************************************************/


#ifndef _MODEL_H
#define _MODEL_H

#include "children.h"


/*
 *	Dialogs.
 */

typedef struct {
	char			*model;		/* Numerical model	*/
	char			*model_label;	/* Numerical model	*/
	TextFieldObjectType	vcoord;		/* Vertical coordinate	*/
	int			nlevels;	/* Number of model levels */
	int			nlayers;	/* Number of model layers */
	ListParentObjectType	levlist;	/* List of lev1, lev2	*/
	ListParentObjectType	layerlist;	/* List of layers	*/
	TextFieldObjectType	level1;		/* Vertical level 1	*/
	TextFieldObjectType	level2; 	/* Vertical level 2	*/
	ButtonObject		level_b;	/* Level radio button	*/
	ButtonObject		layer_b;	/* Layer radio button	*/
	int			level;  	/* Vertical level counter */
	int			proj;		/* Projection		*/
	int			plot_type;	/* Gridded plot type    */
} ModelInfoType;


ModelInfoType *GetGuiModelInfo();

void TimeButtonCB ( Widget w, XtPointer clientData, XtPointer callData );
void SetModelLevelCB ( Widget w, XtPointer clientData, XtPointer callData );


#define SetModelType(m,c)		((m)->model = (c) ? strdup(c) : (c))
#define GetModelType(m)			((m)->model)
#define FreeModelType(m)		((m)->model ? 			\
					 free((m)->model) : (void)1 )

#define SetModelLabelType(m,c)		((m)->model_label = (c) ? strdup(c) : (c))
#define GetModelLabelType(m)		((m)->model_label)
#define FreeModelLabelType(m)		((m)->model_label ? 		\
					 free((m)->model_label) : (void)1 )

#define SetModelVCoordHSW(m,c)		((m)->vcoord.w=(c))
#define GetModelVCoordHSW(m)		((m)->vcoord.w)
#define SetModelVCoord(m,c)		((m)->vcoord.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelVCoord(m)		((m)->vcoord.string)
#define FreeModelVCoord(m)		((m)->vcoord.string ?		\
					 free((m)->vcoord.string) : (void) 1)


#define SetNumberModelLevels(m,c)	((m)->nlevels=(c))
#define GetNumberModelLevels(m)		((m)->nlevels)


#define SetNumberModelLayers(m,c)	((m)->nlayers=(c))
#define GetNumberModelLayers(m)		((m)->nlayers)


#define GetModelLevelsList(m)		((m)->levlist)


#define GetModelLayersList(m)		((m)->layerlist)


#define SetModelLevel1HSW(m,c)		((m)->level1.w=(c))
#define GetModelLevel1HSW(m)		((m)->level1.w)
#define SetModelLevel1(m,c)		((m)->level1.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelLevel1(m)		((m)->level1.string)
#define FreeModelLevel1(m)		((m)->level1.string ?		\
					 free((m)->level1.string) : (void) 1)


#define SetModelLevel2HSW(m,c)		((m)->level2.w=(c))
#define GetModelLevel2HSW(m)		((m)->level2.w)
#define SetModelLevel2(m,c)		((m)->level2.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelLevel2(m)		((m)->level2.string)
#define FreeModelLevel2(m)		((m)->level2.string ?		\
					 free((m)->level2.string) : (void) 1)


#define SetModelLevelToggleHSW(m,c)	((m)->level_b.w=(c))
#define GetModelLevelToggleHSW(m)	((m)->level_b.w)


#define SetModelLayerToggleHSW(m,c)	((m)->layer_b.w=(c))
#define GetModelLayerToggleHSW(m)	((m)->layer_b.w)


#define SetActiveModelLevel(m,c)	((m)->level=c)
#define GetActiveModelLevel(m)		((m)->level)


#define SetModelProjType(m,c)		((m)->proj=c)
#define GetModelProjType(m)		((m)->proj)


#define SetGridType(m,c)		((m)->plot_type=(c))
#define GetGridType(m)			((m)->plot_type)


#endif  /*   _MODEL_H   */
