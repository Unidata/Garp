/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	xsecobj.h
 *
 *	GUI objects for model cross sections.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Added sanity tests to strdup and free macros.
 *	 3/97	COMET	Added scale factor macros.
 *	 8/97	COMET	Added ListIndexObjectType for time matching.
 *	11/97	COMET	Added timelistlbl
 *	11/97	COMET	Added model_label.
 *	12/97	COMET	Added Set/GetModelXSDialogW()
 *	12/97	COMET	Added select_list object for date/time lists.
 *	 5/98	COMET	Added option menu handles to scalarpm.
 *
 ***********************************************************************/

 
#ifndef _XSECOBJ_H
#define _XSECOBJ_H

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
	TextFieldObjectType	xsection;	/* Cross section	*/
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
	TextFieldObjectType	cint;		/* Contour interval     */
	TextFieldObjectType	cmin;		/* Contour maximum      */
	TextFieldObjectType	cmax;		/* Contour maximum      */
	LabelObjectType		line_color;	/* Contour line color	*/
	ButtonParentObjectType	line_type_pm;	/* Contour line type	*/
	LabelObjectType		line_width;	/* Contour line width	*/
	ButtonParentObjectType	label_freq_pm;	/* Label frequency	*/
	ButtonObject		scalar_b;	/* Scalar radio button	*/
	ButtonObject		vector_b;	/* Vector radio button	*/
	TextFieldObjectType	fint;		/* Color fill interval	*/
	TextFieldObjectType	fmin;		/* Color fill minimum	*/
	TextFieldObjectType	fmax;		/* Color fill maximum	*/
	ButtonParentObjectType  color_fillpm;   /* Fill colors          */
	LabelObjectType		fill_color;	/* Fill colors		*/
	ToggleButtonObjectType	line_toggle1;	/* Plot contour lines	*/
	ToggleButtonObjectType	line_toggle2;	/* Plot contour lines	*/
	ToggleButtonObjectType	fill_toggle1;	/* Plot color fill	*/
	ToggleButtonObjectType	fill_toggle2;	/* Plot color fill	*/
	ToggleButtonObjectType	symbol_toggle1;	/* Plot symbols		*/
	ToggleButtonObjectType	symbol_toggle2;	/* Plot symbols		*/
	int			plot_type;	/* Scalar or vector 	*/
	ButtonParentObjectType	axis_type_pm;	/* Vertical axis	*/
	ButtonParentObjectType	vector_sym_pm;	/* Wind symbol          */
	TextFieldObjectType	scale;		/* Scale factor		*/
	LabelObjectType		timelistlbl;	/* Time list label	   */
	Widget			main_dialog;	/* main dialog 		   */
} GuiXSectionDialogType;


GuiXSectionDialogType *GetGuiXSectionDialog();


#define SetModelTypePmXSW(x,c)		((x)->modelpm.parent=(c))
#define GetModelTypePmXSW(x)		((x)->modelpm.parent)
#define SetModelTypeOmXSW(x,c)		((x)->modelpm.menu=(c))
#define GetModelTypeOmXSW(x)		((x)->modelpm.menu)
#define SetModelTypeXS(x,c)		((x)->modelpm.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelTypeXS(x)		((x)->modelpm.string)
#define FreeModelTypeXS(x)		((x)->modelpm.string ?		\
					 free((x)->modelpm.string) : (void) 1)

#define SetModelLabelTypeXS(x,c)	((x)->model_label = (c) ? strdup(c) : (c))
#define GetModelLabelTypeXS(x)		((x)->model_label)
#define FreeModelLabelTypeXS(x)		((x)->model_label ? 		\
					 free((x)->model_label) : (void)1 )


#define SetModelVCoordPmXSW(x,c)	((x)->vcoordpm.parent=(c))
#define GetModelVCoordPmXSW(x)		((x)->vcoordpm.parent)
#define SetModelVCoordOmXSW(x,c)	((x)->vcoordpm.menu=(c))
#define GetModelVCoordOmXSW(x)		((x)->vcoordpm.menu)
#define SetModelVCoordXS(x,c)		((x)->vcoordpm.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelVCoordXS(x)		((x)->vcoordpm.string)
#define FreeModelVCoordXS(x)		((x)->vcoordpm.string ?		\
					 free((x)->vcoordpm.string) : (void) 1)


#define SetModelVCoordXSW(x,c)		((x)->vcoord.w=(c))
#define GetModelVCoordXSW(x)		((x)->vcoord.w)


#define SetLevel1XSW(x,c)		((x)->level1.w=(c))
#define GetLevel1XSW(x)			((x)->level1.w)
#define SetLevel1XS(x,c)		((x)->level1.string = (c) ?	\
					 strdup(c) : (c))
#define GetLevel1XS(x)			((x)->level1.string)
#define FreeLevel1XS(x)			((x)->level1.string ?		\
					 free((x)->level1.string) : (void) 1)


#define SetLevel2XSW(x,c)		((x)->level2.w=(c))
#define GetLevel2XSW(x)			((x)->level2.w)
#define SetLevel2XS(x,c)		((x)->level2.string = (c) ?	\
					 strdup(c) : (c))
#define GetLevel2XS(x)			((x)->level2.string)
#define FreeLevel2XS(x)			((x)->level2.string ?		\
					 free((x)->level2.string) : (void) 1)


#define SetXSectionXSW(x,c)		((x)->xsection.w=(c))
#define GetXSectionXSW(x)		((x)->xsection.w)
#define SetXSectionXS(x,c)		((x)->xsection.string = (c) ?	\
					 strdup(c) : (c))
#define GetXSectionXS(x)		((x)->xsection.string)
#define FreeXSectionXS(x)		((x)->xsection.string ?		\
					 free((x)->xsection.string) : (void) 1)


#define SetModelScalarPmXSW(x,c)	((x)->scalarpm.parent=(c))
#define GetModelScalarPmXSW(x)		((x)->scalarpm.parent)
#define SetModelScalarOmXSW(x,c)	((x)->scalarpm.menu=(c))
#define GetModelScalarOmXSW(x)		((x)->scalarpm.menu)
#define SetModelXSScalarDir(x,c)	((x)->scalarpm.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelXSScalarDir(x)		((x)->scalarpm.string)
#define FreeModelXSScalarDir(x)		((x)->scalarpm.string ?		\
					 free((x)->scalarpm.string) : (void) 1)


#define SetModelVectorPmXSW(x,c)	((x)->vectorpm.parent=(c))
#define GetModelVectorPmXSW(x)		((x)->vectorpm.parent)
#define SetModelXSVectorDir(x,c)	((x)->vectorpm.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelXSVectorDir(x)		((x)->vectorpm.string)
#define FreeModelXSVectorDir(x)		((x)->vectorpm.string ?		\
					 free((x)->vectorpm.string) : (void) 1)


#define SetModelFdfXSW(x,c)		((x)->fdf.w=(c))
#define GetModelFdfXSW(x)		((x)->fdf.w)
#define SetModelFdfXS(x,c)		((x)->fdf.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelFdfXS(x)		((x)->fdf.string)
#define FreeModelFdfXS(x)		((x)->fdf.string ?		\
					 free((x)->fdf.string) : (void) 1)


#define SetContourIntervalXSW(x,c)	((x)->cint.w=(c))
#define GetContourIntervalXSW(x)	((x)->cint.w)
#define SetContourIntervalXS(x,c)	((x)->cint.string = (c) ?	\
					 strdup(c) : (c))
#define GetContourIntervalXS(x)		((x)->cint.string)
#define FreeContourIntervalXS(x)	((x)->cint.string ?		\
					 free((x)->cint.string) : (void) 1)


#define SetContourMinimumXSW(x,c)	((x)->cmin.w=(c))
#define GetContourMinimumXSW(x)		((x)->cmin.w)
#define SetContourMinimumXS(x,c)	((x)->cmin.string = (c) ?	\
					 strdup(c) : (c))
#define GetContourMinimumXS(x)		((x)->cmin.string)
#define FreeContourMinimumXS(x)		((x)->cmin.string ?		\
					 free((x)->cmin.string) : (void) 1)


#define SetContourMaximumXSW(x,c)	((x)->cmax.w=(c))
#define GetContourMaximumXSW(x)		((x)->cmax.w)
#define SetContourMaximumXS(x,c)	((x)->cmax.string = (c) ?	\
					 strdup(c) : (c))
#define GetContourMaximumXS(x)		((x)->cmax.string)
#define FreeContourMaximumXS(x)		((x)->cmax.string ?		\
					 free((x)->cmax.string) : (void) 1)


#define SetModelScrolledListXSW(x,c)	((x)->time_list.w=(c))
#define GetModelScrolledListXSW(x)	((x)->time_list.w)

#define SetScalarListXSW(x,c)		((x)->scalar_list.w=(c))
#define GetScalarListXSW(x)		((x)->scalar_list.w)

#define SetVectorListXSW(x,c)		((x)->vector_list.w=(c))
#define GetVectorListXSW(x)		((x)->vector_list.w)

#define SetFieldInfoXSW(x,c)		((x)->field_info.w=(c))
#define GetFieldInfoXSW(x)		((x)->field_info.w)
#define SetFieldInfoXS(x,c)		((x)->field_info.string = (c) ?	\
					 strdup(c) : (c))
#define GetFieldInfoXS(x)		((x)->field_info.string)
#define FreeFieldInfoXS(x)		((x)->field_info.string ?	\
					 free((x)->field_info.string) : (void) 1)


#define SetFunctionXSW(x,c)		((x)->func_string.w=(c))
#define GetFunctionXSW(x)		((x)->func_string.w)
#define SetFunctionXS(x,c)		((x)->func_string.string = (c) ? \
				         strdup(c) : (c))
#define GetFunctionXS(x)		((x)->func_string.string)
#define FreeFunctionXS(x)		((x)->func_string.string ?	 \
				         free((x)->func_string.string) : (void) 1)


#define SetLineColorXS(x,c)		((x)->line_color.string = (c) ?	\
					 strdup(c) : (c))
#define GetLineColorXS(x)		((x)->line_color.string)
#define FreeLineColorXS(x)		((x)->line_color.string ?	\
					 free((x)->line_color.string) : (void) 1)


#define SetLineTypePmXSW(x,c)		((x)->line_type_pm.parent=(c))
#define GetLineTypePmXSW(x)		((x)->line_type_pm.parent)
#define SetLineTypeOmXSW(x,c)		((x)->line_type_pm.menu=(c))
#define GetLineTypeOmXSW(x)		((x)->line_type_pm.menu)
#define SetLineTypeXS(x,c)		((x)->line_type_pm.string = (c) ? \
				         strdup(c) : (c))
#define GetLineTypeXS(x)		((x)->line_type_pm.string)
#define FreeLineTypeXS(x)		((x)->line_type_pm.string ?	  \
					 free((x)->line_type_pm.string) : (void) 1)


#define SetLineWidthXS(x,c)		((x)->line_width.string = (c) ?	\
					 strdup(c) : (c))
#define GetLineWidthXS(x)		((x)->line_width.string)
#define FreeLineWidthXS(x)		((x)->line_width.string ?	\
					 free((x)->line_width.string) : (void) 1)


#define SetLabelFrequencyPmXSW(x,c)	((x)->label_freq_pm.parent=(c))
#define GetLabelFrequencyPmXSW(x)	((x)->label_freq_pm.parent)
#define SetLabelFrequencyOmXSW(x,c)	((x)->label_freq_pm.menu=(c))
#define GetLabelFrequencyOmXSW(x)	((x)->label_freq_pm.menu)
#define SetLabelFrequencyXS(x,c)	((x)->label_freq_pm.string = (c) ? \
					 strdup(c) : (c))
#define GetLabelFrequencyXS(x)		((x)->label_freq_pm.string)
#define FreeLabelFrequencyXS(x)		((x)->label_freq_pm.string ?	   \
					 free((x)->label_freq_pm.string) : (void) 1)


#define SetFillColorsXSW(x,c)		((x)->color_fillpm.parent=(c))
#define GetFillColorsXSW(x)		((x)->color_fillpm.parent)


#define SetFillColorsXS(x,c)		((x)->fill_color.string = (c) ?	\
					 strdup(c) : (c))
#define GetFillColorsXS(x)		((x)->fill_color.string)
#define FreeFillColorsXS(x)		((x)->fill_color.string ?	\
					 free((x)->fill_color.string) : (void) 1)


#define SetModelScalarButtonXSW(x,c)	((x)->scalar_b.w=(c))
#define GetModelScalarButtonXSW(x)	((x)->scalar_b.w)


#define SetModelVectorButtonXSW(x,c)	((x)->vector_b.w=(c))
#define GetModelVectorButtonXSW(x)	((x)->vector_b.w)


#define SetFillIntervalXSW(x,c)		((x)->fint.w=(c))
#define GetFillIntervalXSW(x)		((x)->fint.w)
#define SetFillIntervalXS(x,c)		((x)->fint.string = (c) ?	\
					 strdup(c) : (c))
#define GetFillIntervalXS(x)		((x)->fint.string)
#define FreeFillIntervalXS(x)		((x)->fint.string ?		\
					 free((x)->fint.string) : (void) 1)


#define SetFillMinimumXSW(x,c)		((x)->fmin.w=(c))
#define GetFillMinimumXSW(x)		((x)->fmin.w)
#define SetFillMinimumXS(x,c)		((x)->fmin.string = (c) ?	\
					 strdup(c) : (c))
#define GetFillMinimumXS(x)		((x)->fmin.string)
#define FreeFillMinimumXS(x)		((x)->fmin.string ?		\
					 free((x)->fmin.string) : (void) 1)


#define SetFillMaximumXSW(x,c)		((x)->fmax.w=(c))
#define GetFillMaximumXSW(x)		((x)->fmax.w)
#define SetFillMaximumXS(x,c)		((x)->fmax.string = (c) ?	\
					 strdup(c) : (c))
#define GetFillMaximumXS(x)		((x)->fmax.string)
#define FreeFillMaximumXS(x)		((x)->fmax.string ?		\
					 free((x)->fmax.string) : (void) 1)


#define SetLineToggleXSW1(x,c)		((x)->line_toggle1.w=(c))
#define GetLineToggleXSW1(x)		((x)->line_toggle1.w)
#define SetLineToggleXSW2(x,c)		((x)->line_toggle2.w=(c))
#define GetLineToggleXSW2(x)		((x)->line_toggle2.w)


#define SetFillToggleXSW1(x,c)		((x)->fill_toggle1.w=(c))
#define GetFillToggleXSW1(x)		((x)->fill_toggle1.w)
#define SetFillToggleXSW2(x,c)		((x)->fill_toggle2.w=(c))
#define GetFillToggleXSW2(x)		((x)->fill_toggle2.w)


#define SetSymbolToggleXSW1(x,c)	((x)->symbol_toggle1.w=(c))
#define GetSymbolToggleXSW1(x)		((x)->symbol_toggle1.w)
#define SetSymbolToggleXSW2(x,c)	((x)->symbol_toggle2.w=(c))
#define GetSymbolToggleXSW2(x)		((x)->symbol_toggle2.w)


#define SetXSGridType(x,c)		((x)->plot_type=(c))
#define GetXSGridType(x)		((x)->plot_type)


#define SetAxisTypePmXSW(x,c)		((x)->axis_type_pm.parent=(c))
#define GetAxisTypePmXSW(x)		((x)->axis_type_pm.parent)
#define SetAxisTypeOmXSW(x,c)		((x)->axis_type_pm.menu=(c))
#define GetAxisTypeOmXSW(x)		((x)->axis_type_pm.menu)
#define SetAxisTypeXS(x,c)		((x)->axis_type_pm.string = (c) ? \
					 strdup(c) : (c))
#define GetAxisTypeXS(x)		((x)->axis_type_pm.string)
#define FreeAxisTypeXS(x)		((x)->axis_type_pm.string ?	  \
					 free((x)->axis_type_pm.string) : (void) 1)


#define SetVectorSymbolPmXSW(x,c)	((x)->vector_sym_pm.parent=(c))
#define GetVectorSymbolPmXSW(x)		((x)->vector_sym_pm.parent)
#define SetVectorSymbolOmXSW(x,c)	((x)->vector_sym_pm.menu=(c))
#define GetVectorSymbolOmXSW(x)		((x)->vector_sym_pm.menu)
#define SetVectorSymbolXS(x,c)		((x)->vector_sym_pm.string = (c) ? \
					 strdup(c) : (c))
#define GetVectorSymbolXS(x)		((x)->vector_sym_pm.string)
#define FreeVectorSymbolXS(x)		((x)->vector_sym_pm.string ?	   \
					 free((x)->vector_sym_pm.string) : (void) 1)



#define SetScaleFactorXSW(x,c)		((x)->scale.w=(c))
#define GetScaleFactorXSW(x)		((x)->scale.w)
#define SetScaleFactorXS(x,c)		((x)->scale.string = (c) ?	\
					 strdup(c) : (c))
#define GetScaleFactorXS(x)		((x)->scale.string)
#define FreeScaleFactorXS(x)		((x)->scale.string ?		\
					 free((x)->scale.string) : (void) 1)

#define	SetModelCSTimeListLabelW(s,c)	((s)->timelistlbl.w=(c))
#define	GetModelCSTimeListLabelW(s)	((s)->timelistlbl.w)

#define	SetModelXSDialogW(s,c)		((s)->main_dialog=(c))
#define	GetModelXSDialogW(s)		((s)->main_dialog)

#endif  /*   _XSECOBJ_H   */
