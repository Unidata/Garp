/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	thgtobj.h
 *
 *	Header file for wind TimeHeight object.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Added sanity tests to strdup and free macros.
 *	 3/97	COMET	Added scale factor and time axis macros.
 *	11/97	COMET	Added timelistlbl
 *	11/97	COMET	Added model_label.
 *	12/97	COMET	Added select_list object for date/time lists.
 *	 5/98	COMET	Added option menu handles to scalarpm.
 *
 ***********************************************************************/


#ifndef _THOBJ_H
#define _THOBJ_H

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
	ListIndexObjectType	select_list;	/* Selected indices     */
	ListParentObjectType	scalar_list;	/* Scalar fields 	*/
	ListParentObjectType	vector_list;	/* Vector fields	*/
	ScrolledWObjectType	field_info;	/* Field description	*/
	TextFieldObjectType	func_string;	/* Function text field	*/
	TextFieldObjectType	cint;		/* Contour interval     */
	TextFieldObjectType	cmin;		/* Contour maximum      */
	TextFieldObjectType	cmax;		/* Contour maximum      */
	LabelObjectType		line_color;	/* Contour line color	*/
	ButtonParentObjectType	line_type_pm;	/* Contour line type	*/
	LabelObjectType		line_width;	/* Contour line width	*/
	ButtonParentObjectType	label_freq_pm;	/* Label frequency	*/
	ButtonObject		scalar_b;	/* Scalar radio button	*/
	ButtonObject		vector_b;	/* Vector radio button	*/
	int			plot_type;	/* Scalar or vector 	*/
	ButtonParentObjectType	axis_type_pm;	/* Vertical axis	*/
	ButtonParentObjectType	vector_sym_pm;	/* Wind symbol          */
	TextFieldObjectType	scale;		/* Scale factor	 	*/
	ButtonParentObjectType	taxis;		/* Time axis direction 	*/
	LabelObjectType		timelistlbl;	/* Time list label	   */
} GuiTimeHeightDialogType;


GuiTimeHeightDialogType *GetGuiTimeHeightDialog();


#define SetModelTypePmTHW(x,c)		((x)->modelpm.parent=(c))
#define GetModelTypePmTHW(x)		((x)->modelpm.parent)
#define SetModelTypeOmTHW(x,c)		((x)->modelpm.menu=(c))
#define GetModelTypeOmTHW(x)		((x)->modelpm.menu)
#define SetModelTypeTH(x,c)		((x)->modelpm.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelTypeTH(x)		((x)->modelpm.string)
#define FreeModelTypeTH(x)		((x)->modelpm.string ?		\
					 free((x)->modelpm.string) : (void) 1)


#define SetModelVCoordPmTHW(x,c)	((x)->vcoordpm.parent=(c))
#define GetModelVCoordPmTHW(x)		((x)->vcoordpm.parent)
#define SetModelVCoordOmTHW(x,c)	((x)->vcoordpm.menu=(c))
#define GetModelVCoordOmTHW(x)		((x)->vcoordpm.menu)
#define SetModelVCoordTH(x,c)		((x)->vcoordpm.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelVCoordTH(x)		((x)->vcoordpm.string)
#define FreeModelVCoordTH(x)		((x)->vcoordpm.string ?		\
					 free((x)->vcoordpm.string) : (void) 1)

#define SetModelLabelTypeTH(x,c)	((x)->model_label = (c) ? strdup(c) : (c))
#define GetModelLabelTypeTH(x)		((x)->model_label)
#define FreeModelLabelTypeTH(x)		((x)->model_label ? 		\
					 free((x)->model_label) : (void)1 )

#define SetModelVCoordTHW(x,c)		((x)->vcoord.w=(c))
#define GetModelVCoordTHW(x)		((x)->vcoord.w)


#define SetLevel1THW(x,c)		((x)->level1.w=(c))
#define GetLevel1THW(x)			((x)->level1.w)
#define SetLevel1TH(x,c)		((x)->level1.string = (c) ?	\
					 strdup(c) : (c))
#define GetLevel1TH(x)			((x)->level1.string)
#define FreeLevel1TH(x)			((x)->level1.string ?		\
					 free((x)->level1.string) : (void) 1)


#define SetLevel2THW(x,c)		((x)->level2.w=(c))
#define GetLevel2THW(x)			((x)->level2.w)
#define SetLevel2TH(x,c)		((x)->level2.string = (c) ?	\
					 strdup(c) : (c))
#define GetLevel2TH(x)			((x)->level2.string)
#define FreeLevel2TH(x)			((x)->level2.string ?		\
					 free((x)->level2.string) : (void) 1)


#define SetStationTHW(x,c)		((x)->station.w=(c))
#define GetStationTHW(x)		((x)->station.w)
#define SetStationTH(x,c)		((x)->station.string = (c) ?	\
					 strdup(c) : (c))
#define GetStationTH(x)			((x)->station.string)
#define FreeStationTH(x)		((x)->station.string ?		\
					 free((x)->station.string) : (void) 1)


#define SetModelScalarPmTHW(x,c)	((x)->scalarpm.parent=(c))
#define GetModelScalarPmTHW(x)		((x)->scalarpm.parent)
#define SetModelScalarOmTHW(x,c)	((x)->scalarpm.menu=(c))
#define GetModelScalarOmTHW(x)		((x)->scalarpm.menu)
#define SetModelTHScalarDir(x,c)	((x)->scalarpm.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelTHScalarDir(x)		((x)->scalarpm.string)
#define FreeModelTHScalarDir(x)		((x)->scalarpm.string ?		\
					 free((x)->scalarpm.string) : (void) 1)



#define SetModelVectorPmTHW(x,c)	((x)->vectorpm.parent=(c))
#define GetModelVectorPmTHW(x)		((x)->vectorpm.parent)
#define SetModelTHVectorDir(x,c)	((x)->vectorpm.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelTHVectorDir(x)		((x)->vectorpm.string)
#define FreeModelTHVectorDir(x)		((x)->vectorpm.string ?		\
					 free((x)->vectorpm.string) : (void) 1)


#define SetModelFdfTHW(x,c)		((x)->fdf.w=(c))
#define GetModelFdfTHW(x)		((x)->fdf.w)
#define SetModelFdfTH(x,c)		((x)->fdf.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelFdfTH(x)		((x)->fdf.string)
#define FreeModelFdfTH(x)		((x)->fdf.string ?		\
					 free((x)->fdf.string) : (void) 1)


#define SetContourIntervalTHW(x,c)	((x)->cint.w=(c))
#define GetContourIntervalTHW(x)	((x)->cint.w)
#define SetContourIntervalTH(x,c)	((x)->cint.string = (c) ?	\
					 strdup(c) : (c))
#define GetContourIntervalTH(x)		((x)->cint.string)
#define FreeContourIntervalTH(x)	((x)->cint.string ?		\
					 free((x)->cint.string) : (void) 1)


#define SetContourMinimumTHW(x,c)	((x)->cmin.w=(c))
#define GetContourMinimumTHW(x)		((x)->cmin.w)
#define SetContourMinimumTH(x,c)	((x)->cmin.string = (c) ?	\
					 strdup(c) : (c))
#define GetContourMinimumTH(x)		((x)->cmin.string)
#define FreeContourMinimumTH(x)		((x)->cmin.string ?		\
					 free((x)->cmin.string) : (void) 1)


#define SetContourMaximumTHW(x,c)	((x)->cmax.w=(c))
#define GetContourMaximumTHW(x)		((x)->cmax.w)
#define SetContourMaximumTH(x,c)	((x)->cmax.string = (c) ?	\
					 strdup(c) : (c))
#define GetContourMaximumTH(x)		((x)->cmax.string)
#define FreeContourMaximumTH(x)		((x)->cmax.string ?		\
					 free((x)->cmax.string) : (void) 1)


#define SetModelScrolledListTHW(x,c)	((x)->time_list.w=(c))
#define GetModelScrolledListTHW(x)	((x)->time_list.w)


#define SetScalarListTHW(x,c)		((x)->scalar_list.w=(c))
#define GetScalarListTHW(x)		((x)->scalar_list.w)


#define SetVectorListTHW(x,c)		((x)->vector_list.w=(c))
#define GetVectorListTHW(x)		((x)->vector_list.w)


#define SetFieldInfoTHW(x,c)		((x)->field_info.w=(c))
#define GetFieldInfoTHW(x)		((x)->field_info.w)
#define SetFieldInfoTH(x,c)		((x)->field_info.string = (c) ?	\
					 strdup(c) : (c))
#define GetFieldInfoTH(x)		((x)->field_info.string)
#define FreeFieldInfoTH(x)		((x)->field_info.string ?	\
					 free((x)->field_info.string) : (void) 1)


#define SetFunctionTHW(x,c)		((x)->func_string.w=(c))
#define GetFunctionTHW(x)		((x)->func_string.w)
#define SetFunctionTH(x,c)		((x)->func_string.string = (c) ? \
					 strdup(c) : (c))
#define GetFunctionTH(x)		((x)->func_string.string)
#define FreeFunctionTH(x)		((x)->func_string.string ?	\
					 free((x)->func_string.string) : (void) 1)


#define SetLineColorTH(x,c)		((x)->line_color.string = (c) ?	\
					 strdup(c) : (c))
#define GetLineColorTH(x)		((x)->line_color.string)
#define FreeLineColorTH(x)		((x)->line_color.string ?	\
					 free((x)->line_color.string) : (void) 1)


#define SetLineTypePmTHW(x,c)		((x)->line_type_pm.parent=(c))
#define GetLineTypePmTHW(x)		((x)->line_type_pm.parent)
#define SetLineTypeOmTHW(x,c)		((x)->line_type_pm.menu=(c))
#define GetLineTypeOmTHW(x)		((x)->line_type_pm.menu)
#define SetLineTypeTH(x,c)		((x)->line_type_pm.string = (c) ? \
					 strdup(c) : (c))
#define GetLineTypeTH(x)		((x)->line_type_pm.string)
#define FreeLineTypeTH(x)		((x)->line_type_pm.string ?	  \
					 free((x)->line_type_pm.string) : (void) 1)


#define SetLineWidthTH(x,c)		((x)->line_width.string = (c) ?	\
					 strdup(c) : (c))
#define GetLineWidthTH(x)		((x)->line_width.string)
#define FreeLineWidthTH(x)		((x)->line_width.string ?	\
					 free((x)->line_width.string) : (void) 1)


#define SetLabelFrequencyPmTHW(x,c)	((x)->label_freq_pm.parent=(c))
#define GetLabelFrequencyPmTHW(x)	((x)->label_freq_pm.parent)
#define SetLabelFrequencyOmTHW(x,c)	((x)->label_freq_pm.menu=(c))
#define GetLabelFrequencyOmTHW(x)	((x)->label_freq_pm.menu)
#define SetLabelFrequencyTH(x,c)	((x)->label_freq_pm.string = (c) ? \
					 strdup(c) : (c))
#define GetLabelFrequencyTH(x)		((x)->label_freq_pm.string)
#define FreeLabelFrequencyTH(x)		((x)->label_freq_pm.string ?	   \
					 free((x)->label_freq_pm.string) : (void) 1)


#define SetModelScalarButtonTHW(x,c)	((x)->scalar_b.w=(c))
#define GetModelScalarButtonTHW(x)	((x)->scalar_b.w)


#define SetModelVectorButtonTHW(x,c)	((x)->vector_b.w=(c))
#define GetModelVectorButtonTHW(x)	((x)->vector_b.w)


#define SetTHGridType(x,c)		((x)->plot_type=(c))
#define GetTHGridType(x)		((x)->plot_type)


#define SetAxisTypePmTHW(x,c)		((x)->axis_type_pm.parent=(c))
#define GetAxisTypePmTHW(x)		((x)->axis_type_pm.parent)
#define SetAxisTypeOmTHW(x,c)		((x)->axis_type_pm.menu=(c))
#define GetAxisTypeOmTHW(x)		((x)->axis_type_pm.menu)
#define SetAxisTypeTH(x,c)		((x)->axis_type_pm.string = (c) ? \
					 strdup(c) : (c))
#define GetAxisTypeTH(x)		((x)->axis_type_pm.string)
#define FreeAxisTypeTH(x)		((x)->axis_type_pm.string ?	  \
					 free((x)->axis_type_pm.string) : (void) 1)


#define SetVectorSymbolPmTHW(x,c)	((x)->vector_sym_pm.parent=(c))
#define GetVectorSymbolPmTHW(x)		((x)->vector_sym_pm.parent)
#define SetVectorSymbolOmTHW(x,c)	((x)->vector_sym_pm.menu=(c))
#define GetVectorSymbolOmTHW(x)		((x)->vector_sym_pm.menu)
#define SetVectorSymbolTH(x,c)		((x)->vector_sym_pm.string = (c) ? \
					 strdup(c) : (c))
#define GetVectorSymbolTH(x)		((x)->vector_sym_pm.string)
#define FreeVectorSymbolTH(x)		((x)->vector_sym_pm.string ?	   \
					 free((x)->vector_sym_pm.string) : (void) 1)



#define SetScaleFactorTHW(x,c)		((x)->scale.w=(c))
#define GetScaleFactorTHW(x)		((x)->scale.w)
#define SetScaleFactorTH(x,c)		((x)->scale.string = (c) ?	\
					 strdup(c) : (c))
#define GetScaleFactorTH(x)		((x)->scale.string)
#define FreeScaleFactorTH(x)		((x)->scale.string ?		\
					 free((x)->scale.string) : (void) 1)


#define SetTimeAxisPmTHW(x,c)		((x)->taxis.parent=(c))
#define GetTimeAxisPmTHW(x)		((x)->taxis.parent)
#define SetTimeAxisOmTHW(x,c)		((x)->taxis.menu=(c))
#define GetTimeAxisOmTHW(x)		((x)->taxis.menu)
#define SetTimeAxisTH(x,c)		((x)->taxis.string = (c) ? \
					 strdup(c) : (c))
#define GetTimeAxisTH(x)		((x)->taxis.string)
#define FreeTimeAxisTH(x)		((x)->taxis.string ?	   \
					 free((x)->taxis.string) : (void) 1)

#define	SetModelTHTimeListLabelW(s,c)	((s)->timelistlbl.w=(c))
#define	GetModelTHTimeListLabelW(s)	((s)->timelistlbl.w)

#endif  /*   _THOBJ_H   */
