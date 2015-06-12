/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	vprof.h
 *
 *	Header file for vertical profile object.
 *
 *	History:
 *
 *	12/96	COMET	Original copy
 *	 2/97	COMET	Added sanity tests to strdup and free macros.
 *	 3/97	COMET	Added scale factor macros.
 *	 8/97	COMET	Added ListIndexObjectType for time matching.
 *	11/97	COMET	Added timelistlbl
 *	11/97	COMET	Added model_label.
 *	12/97	COMET	Added Set/GetModelVPDialogW()
 *	12/97	COMET	Added select_list object for date/time lists.
 *	 5/98	COMET	Added option menu handles to scalarpm.
 *
 ***********************************************************************/


#ifndef _VPROF_H
#define _VPROF_H

#include "children.h"


/*
 *	Dialogs.
 */

typedef struct {
	ButtonParentObjectType	modelpm;	/* Numerical model	*/
	char			*model_label;	/* Numerical model	*/
	ButtonParentObjectType	vcoordpm;	/* Vertical coordinate	*/
	TextFieldObjectType	vcoord;		/* Vertical coordinate	*/
	TextFieldObjectType	level1;		/* Level 1 		*/
	TextFieldObjectType	level2;		/* Level 2		*/
	TextFieldObjectType	station;	/* Cross section	*/
	ButtonParentObjectType	scalarpm;	/* Scalar pulldown menu */
	ButtonParentObjectType	vectorpm;	/* Vector pulldown menu */
	LabelObjectType		fdf;		/* Fld Description File */
	ListParentObjectType	time_list;	/* List of date/times	*/
	ListIndexObjectType	match_list;	/* Time matched indices */
	ListIndexObjectType	select_list;	/* Selected indices     */
	ListParentObjectType	scalar_list;	/* Scalar fields 	*/
	ListParentObjectType	vector_list;	/* Vector fields	*/
	ScrolledWObjectType	field_info;	/* Field description	*/
	TextFieldObjectType	func_string;	/* Function text field	*/
	LabelObjectType		line_color;	/* Contour line color	*/
	ButtonParentObjectType	line_type_pm;	/* Contour line type	*/
	LabelObjectType		line_width;	/* Contour line width	*/
	ButtonObject		scalar_b;	/* Scalar radio button	*/
	ButtonObject		vector_b;	/* Vector radio button	*/
	int			plot_type;	/* Scalar or vector 	*/
	ButtonParentObjectType	axis_type_pm;	/* Vertical axis	*/
	ButtonParentObjectType	vector_sym_pm;	/* Wind symbol          */
	TextFieldObjectType	scale;		/* Scale factor		*/
	LabelObjectType		timelistlbl;	/* Time list label	*/
	Widget			main_dialog;	/* main dialog 		   */
} GuiVertProfileObjectType;

GuiVertProfileObjectType *GetGuiVertProfileDialog();


#define SetModelTypePmVPW(x,c)		((x)->modelpm.parent=(c))
#define GetModelTypePmVPW(x)		((x)->modelpm.parent)
#define SetModelTypeOmVPW(x,c)		((x)->modelpm.menu=(c))
#define GetModelTypeOmVPW(x)		((x)->modelpm.menu)
#define SetModelTypeVP(x,c)		((x)->modelpm.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelTypeVP(x)		((x)->modelpm.string)
#define FreeModelTypeVP(x)		((x)->modelpm.string ?		\
					 free((x)->modelpm.string) : (void) 1)

#define SetModelLabelTypeVP(x,c)	((x)->model_label = (c) ? strdup(c) : (c))
#define GetModelLabelTypeVP(x)		((x)->model_label)
#define FreeModelLabelTypeVP(x)		((x)->model_label ? 		\
					 free((x)->model_label) : (void)1 )

#define SetModelVCoordPmVPW(x,c)	((x)->vcoordpm.parent=(c))
#define GetModelVCoordPmVPW(x)		((x)->vcoordpm.parent)
#define SetModelVCoordOmVPW(x,c)	((x)->vcoordpm.menu=(c))
#define GetModelVCoordOmVPW(x)		((x)->vcoordpm.menu)
#define SetModelVCoordVP(x,c)		((x)->vcoordpm.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelVCoordVP(x)		((x)->vcoordpm.string)
#define FreeModelVCoordVP(x)		((x)->vcoordpm.string ?		\
					 free((x)->vcoordpm.string) : (void) 1)

#define SetModelVCoordVPW(x,c)		((x)->vcoord.w=(c))
#define GetModelVCoordVPW(x)		((x)->vcoord.w)

#define SetLevel1VPW(x,c)		((x)->level1.w=(c))
#define GetLevel1VPW(x)			((x)->level1.w)
#define SetLevel1VP(x,c)		((x)->level1.string = (c) ?	\
					 strdup(c) : (c))
#define GetLevel1VP(x)			((x)->level1.string)
#define FreeLevel1VP(x)			((x)->level1.string ?		\
					 free((x)->level1.string) : (void) 1)

#define SetLevel2VPW(x,c)		((x)->level2.w=(c))
#define GetLevel2VPW(x)			((x)->level2.w)
#define SetLevel2VP(x,c)		((x)->level2.string = (c) ?	\
					 strdup(c) : (c))
#define GetLevel2VP(x)			((x)->level2.string)
#define FreeLevel2VP(x)			((x)->level2.string ?		\
					 free((x)->level2.string) : (void) 1)

#define SetStationVPW(x,c)		((x)->station.w=(c))
#define GetStationVPW(x)		((x)->station.w)
#define SetStationVP(x,c)		((x)->station.string = (c) ?	\
					 strdup(c) : (c))
#define GetStationVP(x)			((x)->station.string)
#define FreeStationVP(x)		((x)->station.string ?		\
					 free((x)->station.string) : (void) 1)

#define SetModelScalarPmVPW(x,c)	((x)->scalarpm.parent=(c))
#define GetModelScalarPmVPW(x)		((x)->scalarpm.parent)
#define SetModelScalarOmVPW(x,c)	((x)->scalarpm.menu=(c))
#define GetModelScalarOmVPW(x)		((x)->scalarpm.menu)
#define SetModelVPScalarDir(x,c)	((x)->scalarpm.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelVPScalarDir(x)		((x)->scalarpm.string)
#define FreeModelVPScalarDir(x)		((x)->scalarpm.string ?		\
					 free((x)->scalarpm.string) : (void) 1)


#define SetModelVectorPmVPW(x,c)	((x)->vectorpm.parent=(c))
#define GetModelVectorPmVPW(x)		((x)->vectorpm.parent)
#define SetModelVPVectorDir(x,c)	((x)->vectorpm.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelVPVectorDir(x)		((x)->vectorpm.string)
#define FreeModelVPVectorDir(x)		((x)->vectorpm.string ?		\
					 free((x)->vectorpm.string) : (void) 1)

#define SetModelFdfVPW(x,c)		((x)->fdf.w=(c))
#define GetModelFdfVPW(x)		((x)->fdf.w)
#define SetModelFdfVP(x,c)		((x)->fdf.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelFdfVP(x)		((x)->fdf.string)
#define FreeModelFdfVP(x)		((x)->fdf.string ?		\
					 free((x)->fdf.string) : (void) 1)

#define SetModelScrolledListVPW(x,c)	((x)->time_list.w=(c))
#define GetModelScrolledListVPW(x)	((x)->time_list.w)

#define SetScalarListVPW(x,c)		((x)->scalar_list.w=(c))
#define GetScalarListVPW(x)		((x)->scalar_list.w)

#define SetVectorListVPW(x,c)		((x)->vector_list.w=(c))
#define GetVectorListVPW(x)		((x)->vector_list.w)

#define SetFieldInfoVPW(x,c)		((x)->field_info.w=(c))
#define GetFieldInfoVPW(x)		((x)->field_info.w)
#define SetFieldInfoVP(x,c)		((x)->field_info.string = (c) ?	\
					 strdup(c) : (c))
#define GetFieldInfoVP(x)		((x)->field_info.string)
#define FreeFieldInfoVP(x)		((x)->field_info.string ?	\
					 free((x)->field_info.string) : (void) 1)

#define SetFunctionVPW(x,c)		((x)->func_string.w=(c))
#define GetFunctionVPW(x)		((x)->func_string.w)
#define SetFunctionVP(x,c)		((x)->func_string.string = (c) ? \
					 strdup(c) : (c))
#define GetFunctionVP(x)		((x)->func_string.string)
#define FreeFunctionVP(x)		((x)->func_string.string ?	\
					 free((x)->func_string.string) : (void) 1)

#define SetLineColorVP(x,c)		((x)->line_color.string = (c) ?	\
					 strdup(c) : (c))
#define GetLineColorVP(x)		((x)->line_color.string)
#define FreeLineColorVP(x)		((x)->line_color.string ?	\
					 free((x)->line_color.string) : (void) 1)

#define SetLineTypePmVPW(x,c)		((x)->line_type_pm.parent=(c))
#define GetLineTypePmVPW(x)		((x)->line_type_pm.parent)
#define SetLineTypeOmVPW(x,c)		((x)->line_type_pm.menu=(c))
#define GetLineTypeOmVPW(x)		((x)->line_type_pm.menu)
#define SetLineTypeVP(x,c)		((x)->line_type_pm.string = (c) ? \
					 strdup(c) : (c))
#define GetLineTypeVP(x)		((x)->line_type_pm.string)
#define FreeLineTypeVP(x)		((x)->line_type_pm.string ?	\
					 free((x)->line_type_pm.string) : (void) 1)

#define SetLineWidthVP(x,c)		((x)->line_width.string = (c) ?	\
					 strdup(c) : (c))
#define GetLineWidthVP(x)		((x)->line_width.string)
#define FreeLineWidthVP(x)		((x)->line_width.string ?	\
					 free((x)->line_width.string) : (void) 1)

#define SetModelScalarButtonVPW(x,c)	((x)->scalar_b.w=(c))
#define GetModelScalarButtonVPW(x)	((x)->scalar_b.w)

#define SetModelVectorButtonVPW(x,c)	((x)->vector_b.w=(c))
#define GetModelVectorButtonVPW(x)	((x)->vector_b.w)

#define SetVPGridType(x,c)		((x)->plot_type=(c))
#define GetVPGridType(x)		((x)->plot_type)

#define SetAxisTypePmVPW(x,c)		((x)->axis_type_pm.parent=(c))
#define GetAxisTypePmVPW(x)		((x)->axis_type_pm.parent)
#define SetAxisTypeOmVPW(x,c)		((x)->axis_type_pm.menu=(c))
#define GetAxisTypeOmVPW(x)		((x)->axis_type_pm.menu)
#define SetAxisTypeVP(x,c)		((x)->axis_type_pm.string = (c) ? \
					 strdup(c) : (c))
#define GetAxisTypeVP(x)		((x)->axis_type_pm.string)
#define FreeAxisTypeVP(x)		((x)->axis_type_pm.string ?	\
					 free((x)->axis_type_pm.string) : (void) 1)

#define SetVectorSymbolPmVPW(x,c)	((x)->vector_sym_pm.parent=(c))
#define GetVectorSymbolPmVPW(x)		((x)->vector_sym_pm.parent)
#define SetVectorSymbolOmVPW(x,c)	((x)->vector_sym_pm.menu=(c))
#define GetVectorSymbolOmVPW(x)		((x)->vector_sym_pm.menu)
#define SetVectorSymbolVP(x,c)		((x)->vector_sym_pm.string = (c) ? \
					 strdup(c) : (c))
#define GetVectorSymbolVP(x)		((x)->vector_sym_pm.string)
#define FreeVectorSymbolVP(x)		((x)->vector_sym_pm.string ?	   \
					 free((x)->vector_sym_pm.string) : (void) 1)

#define SetScaleFactorVPW(x,c)		((x)->scale.w=(c))
#define GetScaleFactorVPW(x)		((x)->scale.w)
#define SetScaleFactorVP(x,c)		((x)->scale.string = (c) ?	\
					 strdup(c) : (c))
#define GetScaleFactorVP(x)		((x)->scale.string)
#define FreeScaleFactorVP(x)		((x)->scale.string ?		\
					 free((x)->scale.string) : (void) 1)
#define	SetModelVPTimeListLabelW(s,c)	((s)->timelistlbl.w=(c))
#define	GetModelVPTimeListLabelW(s)	((s)->timelistlbl.w)

#define	SetModelVPDialogW(s,c)		((s)->main_dialog=(c))
#define	GetModelVPDialogW(s)		((s)->main_dialog)

#endif  /*   _VPROF_H   */
