/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	hsobj.h
 *
 *	Gridded data widget info saving strutures.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/97	COMET	Added Widget objects for model level dialog.
 *	 2/97	COMET	Added sanity tests to strdup and free macros.
 *	 3/97	COMET	Added skip and scale factor macros. Skip not yet
 *			used.
 *	 8/97	COMET	Added ListIndexObjectType for time matching.
 *	11/97	COMET	Added timelistlbl
 *	12/97	COMET	Added Set/GetModelHSDialogW()
 *	12/97	COMET	Added select_list object for date/time lists.
 *	 2/97	COMET	Added individual forms for level, layer buttons.
 *	 5/98	COMET	Added option menu handles to scalarpm.
 *
 ***********************************************************************/

#ifndef _HSOBJ_H
#define _HSOBJ_H

#include "children.h"


/*
 *	Dialogs.
 */

typedef struct {
	ButtonParentObjectType	modelpm;	/* Numerical model	*/
	ButtonParentObjectType	vcoordpm;	/* Vertical coordinate	*/
	TextFieldObjectType	vcoord;		/* Vertical coordinate	*/
	ButtonParentObjectType	level_b;        /* Level buttons        */
	ButtonParentObjectType	layer_b;        /* Layer buttons        */
	ButtonObject		level1_b;	/* Level 1 button	*/
	ButtonObject		level2_b;	/* Level 2 button	*/
	Widget			level_form;	/* Level buttons form	*/
	Widget			layer_form;	/* Layer buttons form	*/
	ButtonParentObjectType	scalarpm;	/* Scalar pulldown menu	*/
	ButtonParentObjectType	vectorpm;	/* Vector pulldown menu */
	LabelObjectType		fdf;		/* Fld Description File */
	ListParentObjectType	time_list;	/* List of date/times	*/
	ListIndexObjectType	match_list;	/* Time matched indices */
	ListIndexObjectType	select_list;	/* Selected indices     */
	ListParentObjectType	field_list;	/* List of fields 	*/
	ScrolledWObjectType	field_info;	/* Field description	*/
	TextFieldObjectType	func_string;	/* Function text field	*/
	int			nfunction;	/* Number of functions  */
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
	ButtonParentObjectType	color_fillpm;	/* Fill colors		*/
	LabelObjectType		fill_color;	/* Current fill color	*/
	ToggleButtonObjectType	line_toggle1;	/* Plot contour lines	*/
	ToggleButtonObjectType	line_toggle2;	/* Plot contour lines	*/
	ToggleButtonObjectType	point_toggle;	/* Plot grid points	*/
	ToggleButtonObjectType	fill_toggle1;	/* Plot color fill	*/
	ToggleButtonObjectType	fill_toggle2;	/* Plot color fill	*/
	ToggleButtonObjectType	symbol_toggle1;	/* Plot symbols		*/
	ToggleButtonObjectType	symbol_toggle2;	/* Plot symbols		*/
	ButtonParentObjectType	vector_sym_pm;  /* Vector symbol type	*/
	TextFieldObjectType	scale;		/* Scale factor		*/
	TextFieldObjectType	skip;		/* Skip factor		*/
	LabelObjectType		timelistlbl;	/* Time list label	*/
	Widget			main_dialog;	/* main dialog widget   */
} GuiScalarGridDialogType;


GuiScalarGridDialogType *GetGuiScalarGridDialog();

void TimeButtonCB ( Widget w, XtPointer clientData, XtPointer callData );
void SetModelLevelCB ( Widget w, XtPointer clientData, XtPointer callData );


#define SetModelPmHSW(g,c)		((g)->modelpm.parent=(c))
#define GetModelPmHSW(g)		((g)->modelpm.parent)
#define SetModelOmHSW(g,c)		((g)->modelpm.menu=(c))
#define GetModelOmHSW(g)		((g)->modelpm.menu)


#define SetModelVCoordPmHSW(g,c)	((g)->vcoordpm.parent=(c))
#define GetModelVCoordPmHSW(g)		((g)->vcoordpm.parent)
#define SetModelVCoordOmHSW(g,c)	((g)->vcoordpm.menu=(c))
#define GetModelVCoordOmHSW(g)		((g)->vcoordpm.menu)


#define SetModelFdfHSW(g,c)		((g)->fdf.w=(c))
#define GetModelFdfHSW(g)		((g)->fdf.w)
#define SetModelFdfHS(g,c)		((g)->fdf.string = (c) ?	\
					 strdup(c) : (c))

#define GetModelFdfHS(g)		((g)->fdf.string)
#define FreeModelFdfHS(g)		((g)->fdf.string ?		\
					 free((g)->fdf.string) : (void) 1)


#define SetModelLevel1BHSW(g,c)		((g)->level1_b.w=(c))
#define GetModelLevel1BHSW(g)		((g)->level1_b.w)


#define SetModelLevel2BHSW(g,c)		((g)->level2_b.w=(c))
#define GetModelLevel2BHSW(g)		((g)->level2_b.w)


#define SetModelLevelFormW(g,c)		((g)->level_form=(c))
#define GetModelLevelFormW(g)		((g)->level_form)


#define SetModelLayerFormW(g,c)		((g)->layer_form=(c))
#define GetModelLayerFormW(g)		((g)->layer_form)


#define SetModelScalarPmHSW(g,c)	((g)->scalarpm.parent=(c))
#define GetModelScalarPmHSW(g)		((g)->scalarpm.parent)
#define SetModelScalarOmHSW(g,c)	((g)->scalarpm.menu=(c))
#define GetModelScalarOmHSW(g)		((g)->scalarpm.menu)
#define SetModelScalarDir(g,c)		((g)->scalarpm.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelScalarDir(g)		((g)->scalarpm.string)
#define FreeModelScalarDir(g)		((g)->scalarpm.string ?		\
					 free((g)->scalarpm.string) : (void) 1)


#define SetModelVectorPmHSW(g,c)	((g)->vectorpm.parent=(c))
#define GetModelVectorPmHSW(g)		((g)->vectorpm.parent)
#define SetModelVectorOmHSW(g,c)	((g)->vectorpm.menu=(c))
#define GetModelVectorOmHSW(g)		((g)->vectorpm.menu)
#define SetModelVectorDir(g,c)		((g)->vectorpm.string = (c) ?	\
					 strdup(c) : (c))
#define GetModelVectorDir(g)		((g)->vectorpm.string)
#define FreeModelVectorDir(g)		((g)->vectorpm.string ?		\
					 free((g)->vectorpm.string) : (void) 1)


#define SetContourIntervalHSW(g,c)	((g)->cint.w=(c))
#define GetContourIntervalHSW(g)	((g)->cint.w)
#define SetContourIntervalHS(g,c)	((g)->cint.string = (c) ?	\
					 strdup(c) : (c))
#define GetContourIntervalHS(g)		((g)->cint.string)
#define FreeContourIntervalHS(g)	((g)->cint.string ?		\
					 free((g)->cint.string) : (void) 1)


#define SetContourMinimumHSW(g,c)	((g)->cmin.w=(c))
#define GetContourMinimumHSW(g)		((g)->cmin.w)
#define SetContourMinimumHS(g,c)	((g)->cmin.string = (c) ?	\
					 strdup(c) : (c))
#define GetContourMinimumHS(g)		((g)->cmin.string)
#define FreeContourMinimumHS(g)		((g)->cmin.string ?		\
					 free((g)->cmin.string) : (void) 1)


#define SetContourMaximumHSW(g,c)	((g)->cmax.w=(c))
#define GetContourMaximumHSW(g)		((g)->cmax.w)
#define SetContourMaximumHS(g,c)	((g)->cmax.string = (c) ?	\
					 strdup(c) : (c))
#define GetContourMaximumHS(g)		((g)->cmax.string)
#define FreeContourMaximumHS(g)		((g)->cmax.string ?		\
					 free((g)->cmax.string) : (void) 1)


#define SetModelScrolledListHSW(g,c)	((g)->time_list.w=(c))
#define GetModelScrolledListHSW(g)	((g)->time_list.w)


#define SetScrolledListHSW(g,c)		((g)->field_list.w=(c))
#define GetScrolledListHSW(g)		((g)->field_list.w)


#define SetFieldInfoHSW(g,c)		((g)->field_info.w=(c))
#define GetFieldInfoHSW(g)		((g)->field_info.w)
#define SetFieldInfoHS(g,c)		((g)->field_info.string = (c) ?	\
					 strdup(c) : (c))
#define GetFieldInfoHS(g)		((g)->field_info.string)
#define FreeFieldInfoHS(g)		((g)->field_info.string ?	\
					 free((g)->field_info.string) : (void) 1)


#define SetFunctionHSW(g,c)		((g)->func_string.w=(c))
#define GetFunctionHSW(g)		((g)->func_string.w)
#define SetFunctionHS(g,c)		((g)->func_string.string = (c) ? \
					 strdup(c) : (c))
#define GetFunctionHS(g)		((g)->func_string.string)
#define FreeFunctionHS(g)		((g)->func_string.string ?	\
					 free((g)->func_string.string) : (void) 1)


#define SetNumFunctionHS(g,c)		((g)->nfunction=(c))
#define GetNumFunctionHS(g)		((g)->nfunction)

#define SetLineColorHS(g,c)		((g)->line_color.string = (c) ?	\
					 strdup(c) : (c))
#define GetLineColorHS(g)		((g)->line_color.string)
#define FreeLineColorHS(g)		((g)->line_color.string ?	\
					 free((g)->line_color.string) : (void) 1)


#define SetLineTypePmHSW(g,c)		((g)->line_type_pm.parent=(c))
#define GetLineTypePmHSW(g)		((g)->line_type_pm.parent)
#define SetLineTypeOmHSW(g,c)		((g)->line_type_pm.menu=(c))
#define GetLineTypeOmHSW(g)		((g)->line_type_pm.menu)
#define SetLineTypeHS(g,c)		((g)->line_type_pm.string = (c) ? \
					 strdup(c) : (c))
#define GetLineTypeHS(g)		((g)->line_type_pm.string)
#define FreeLineTypeHS(g)		((g)->line_type_pm.string ?	\
					 free((g)->line_type_pm.string) : (void) 1)


#define SetLineWidthHS(g,c)		((g)->line_width.string = (c) ?	\
					 strdup(c) : (c))
#define GetLineWidthHS(g)		((g)->line_width.string)
#define FreeLineWidthHS(g)		((g)->line_width.string ?	\
					 free((g)->line_width.string) : (void) 1)


#define SetLabelFrequencyPmHSW(g,c)	((g)->label_freq_pm.parent=(c))
#define GetLabelFrequencyPmHSW(g)	((g)->label_freq_pm.parent)
#define SetLabelFrequencyOmHSW(g,c)	((g)->label_freq_pm.menu=(c))
#define GetLabelFrequencyOmHSW(g)	((g)->label_freq_pm.menu)
#define SetLabelFrequencyHS(g,c)	((g)->label_freq_pm.string = (c) ? \
					 strdup(c) : (c))
#define GetLabelFrequencyHS(g)		((g)->label_freq_pm.string)
#define FreeLabelFrequencyHS(g)		((g)->label_freq_pm.string ?	\
					 free((g)->label_freq_pm.string) : (void) 1)


#define SetFillColorsHSW(g,c)		((g)->color_fillpm.parent=(c))
#define GetFillColorsHSW(g)		((g)->color_fillpm.parent)

#define SetFillColorsHS(g,c)		((g)->fill_color.string = (c) ?	\
					 strdup(c) : (c))
#define GetFillColorsHS(g)		((g)->fill_color.string)
#define FreeFillColorsHS(g)		((g)->fill_color.string ?	\
					 free((g)->fill_color.string) : (void) 1)


#define SetModelScalarButtonHSW(g,c)	((g)->scalar_b.w=(c))
#define GetModelScalarButtonHSW(g)	((g)->scalar_b.w)


#define SetModelVectorButtonHSW(g,c)	((g)->vector_b.w=(c))
#define GetModelVectorButtonHSW(g)	((g)->vector_b.w)


#define SetFillIntervalHSW(g,c)		((g)->fint.w=(c))
#define GetFillIntervalHSW(g)		((g)->fint.w)
#define SetFillIntervalHS(g,c)		((g)->fint.string = (c) ?	\
					 strdup(c) : (c))
#define GetFillIntervalHS(g)		((g)->fint.string)
#define FreeFillIntervalHS(g)		((g)->fint.string ?		\
					 free((g)->fint.string) : (void) 1)


#define SetFillMinimumHSW(g,c)		((g)->fmin.w=(c))
#define GetFillMinimumHSW(g)		((g)->fmin.w)
#define SetFillMinimumHS(g,c)		((g)->fmin.string = (c) ?	\
					 strdup(c) : (c))
#define GetFillMinimumHS(g)		((g)->fmin.string)
#define FreeFillMinimumHS(g)		((g)->fmin.string ?		\
					 free((g)->fmin.string) : (void) 1)


#define SetFillMaximumHSW(g,c)		((g)->fmax.w=(c))
#define GetFillMaximumHSW(g)		((g)->fmax.w)
#define SetFillMaximumHS(g,c)		((g)->fmax.string = (c) ?	\
					 strdup(c) : (c))
#define GetFillMaximumHS(g)		((g)->fmax.string)
#define FreeFillMaximumHS(g)		((g)->fmax.string ?		\
					 free((g)->fmax.string) : (void) 1)


#define SetLineToggleHSW1(g,c)		((g)->line_toggle1.w=(c))
#define GetLineToggleHSW1(g)		((g)->line_toggle1.w)
#define SetLineToggleHSW2(g,c)		((g)->line_toggle2.w=(c))
#define GetLineToggleHSW2(g)		((g)->line_toggle2.w)


#define SetGridValueToggleHSW(g,c)	((g)->point_toggle.w=(c))
#define GetGridValueToggleHSW(g)	((g)->point_toggle.w)


#define SetFillToggleHSW1(g,c)		((g)->fill_toggle1.w=(c))
#define GetFillToggleHSW1(g)		((g)->fill_toggle1.w)
#define SetFillToggleHSW2(g,c)		((g)->fill_toggle2.w=(c))
#define GetFillToggleHSW2(g)		((g)->fill_toggle2.w)


#define SetSymbolToggleHSW1(g,c)	((g)->symbol_toggle1.w=(c))
#define GetSymbolToggleHSW1(g)		((g)->symbol_toggle1.w)
#define SetSymbolToggleHSW2(g,c)	((g)->symbol_toggle2.w=(c))
#define GetSymbolToggleHSW2(g)		((g)->symbol_toggle2.w)


#define SetVectorSymbolPmHSW(g,c)	((g)->vector_sym_pm.parent=(c))
#define GetVectorSymbolPmHSW(g)		((g)->vector_sym_pm.parent)
#define SetVectorSymbolOmHSW(g,c)	((g)->vector_sym_pm.menu=(c))
#define GetVectorSymbolOmHSW(g)		((g)->vector_sym_pm.menu)
#define SetVectorSymbolHS(g,c)		((g)->vector_sym_pm.string = (c) ? \
					 strdup(c) : (c))
#define GetVectorSymbolHS(g)		((g)->vector_sym_pm.string)
#define FreeVectorSymbolHS(g)		((g)->vector_sym_pm.string ?	   \
					 free((g)->vector_sym_pm.string) : (void) 1)


#define SetScaleFactorHSW(g,c)		((g)->scale.w=(c))
#define GetScaleFactorHSW(g)		((g)->scale.w)
#define SetScaleFactorHS(g,c)		((g)->scale.string = (c) ?	\
					 strdup(c) : (c))
#define GetScaleFactorHS(g)		((g)->scale.string)
#define FreeScaleFactorHS(g)		((g)->scale.string ?		\
					 free((g)->scale.string) : (void) 1)


#define SetSkipFactorHSW(g,c)		((g)->skip.w=(c))
#define GetSkipFactorHSW(g)		((g)->skip.w)
#define SetSkipFactorHS(g,c)		((g)->skip.string = (c) ?	\
					 strdup(c) : (c))
#define GetSkipFactorHS(g)		((g)->skip.string)
#define FreeSkipFactorHS(g)		((g)->skip.string ?		\
					 free((g)->skip.string) : (void) 1)

#define	SetModelTimeListLabelW(s,c)	((s)->timelistlbl.w=(c))
#define	GetModelTimeListLabelW(s)	((s)->timelistlbl.w)

#define	SetModelHSDialogW(s,c)		((s)->main_dialog=(c))
#define	GetModelHSDialogW(s)		((s)->main_dialog)

#endif  /*   _HSOBJ_H   */
