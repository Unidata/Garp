/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	imgobj.h (renamed from imagepopup.h)
 *
 *	Header file for image fileselection dialog.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 8/97	COMET	Re worked to be more consistent with the other
 *			GARP dialog structures.
 *	11/97	COMET	Added timelistlbl
 *	12/97	COMET	Added Get/SetImgDialogW()
 *	12/97	COMET	Added select_list object for date/time lists.
 *
 ***********************************************************************/


#ifndef _IMGOBJ_H
#define _IMGOBJ_H

#include "winobj.h"
#include "children.h"

/*
 *	Image file selection dialog components
 */

typedef struct {
	ButtonParentObjectType	source_panel;	/* Source selection panel  */
	ButtonParentObjectType	scale_panel;	/* Scale selection panel   */
	ButtonParentObjectType	prod_panel;	/* Product selection panel */
        ListParentObjectType    time_list;      /* List of date/times      */
        ListIndexObjectType     match_list;     /* Time matched indices    */
	ListIndexObjectType	select_list;	/* Selected indices        */
	ToggleButtonObjectType	color_bar;	/* Color Bar draw toggle   */
	LabelObjectType		timelistlbl;	/* Time list label	   */
	Widget			main_dialog;	/* main dialog 		   */
} GuiImgSelectDialogType;

GuiImgSelectDialogType *GetGuiImgSelectDialog();


/*
 *	Convenience macros
 */

#define	GetImgSourcePanelObj(s)			&((s)->source_panel)
#define	SetImgSourcePanelW(s,c)			((s)->source_panel.parent=(c))
#define	GetImgSourcePanelW(s)			((s)->source_panel.parent)
#define	SetImgSourcePanelSetbutton(s,c)		((s)->source_panel.setbutton=(c))
#define	GetImgSourcePanelSetbutton(s)		((s)->source_panel.setbutton)

#define	GetImgScalePanelObj(s)			&((s)->scale_panel)
#define	SetImgScalePanelW(s,c)			((s)->scale_panel.parent=(c))
#define	GetImgScalePanelW(s)			((s)->scale_panel.parent)
#define	SetImgScalePanelSetbutton(s,c)		((s)->scale_panel.setbutton=(c))
#define	GetImgScalePanelSetbutton(s)		((s)->scale_panel.setbutton)

#define	GetImgProdPanelObj(s)			&((s)->prod_panel)
#define	SetImgProdPanelW(s,c)			((s)->prod_panel.parent=(c))
#define	GetImgProdPanelW(s)			((s)->prod_panel.parent)
#define	SetImgProdPanelSetbutton(s,c)		((s)->prod_panel.setbutton=(c))
#define	GetImgProdPanelSetbutton(s)		((s)->prod_panel.setbutton)
#define	GetImgProdPanelSetbuttonString(s)	((s)->prod_panel.setbutton->string)
#define	GetImgProdPanelSetbuttonLabel(s)	((s)->prod_panel.setbutton->label)
#define	GetImgProdPanelSetbuttonType(s)		((s)->prod_panel.setbutton->i)

#define	GetImgTimeListObj(s)			&((s)->time_list)
#define	SetImgTimeListW(s,c)			((s)->time_list.w=(c))
#define	GetImgTimeListW(s)			((s)->time_list.w)
#define	GetImgTimeListCount(s)			((s)->time_list.listcount)
#define	SetImgTimeListCount(s,c)		((s)->time_list.listcount=(c))

#define	GetImgMatchListObj(s)			&((s)->match_list)
#define	SetImgMatchListW(s,c)			((s)->match_list.w=(c))
#define	GetImgMatchListW(s)			((s)->match_list.w)
#define	GetImgMatchListCount(s)			((s)->match_list.listcount)

#define	GetImgSelectListObj(s)			&((s)->select_list)

#define	SetImgColorBarW(s,c)			((s)->color_bar.w=(c))
#define	GetImgColorBarW(s)			((s)->color_bar.w)

#define	SetImgTimeListLabelW(s,c)		((s)->timelistlbl.w=(c))
#define	GetImgTimeListLabelW(s)			((s)->timelistlbl.w)

#define	SetImgDialogW(s,c)			((s)->main_dialog=(c))
#define	GetImgDialogW(s)			((s)->main_dialog)

/*
 *	Structure that contains the keys for finding the image data
 *	directories.
 */

typedef struct {
	int	metObjectType;
	char	*imageSource;
	char	*path;
} DefaultDirectory;


static DefaultDirectory ImageSourcePaths[] = {
	SATOBJECT,      "sat",     NULL,
	RADAROBJECT,    "nids",    NULL,
	NOWRADOBJECT,   "nowrad",  NULL,
	(int) NULL,     NULL,      NULL
};



/*
 *	Function prototypes
 */

char 	*builddirpath ( char *s1, char *s2 );
int     readdirectory ( char *path,  char ***argv,
                        int (*fileterfunc)(), int (*sortfunc)());

int     GetImageFileList ( int, char *, char ***, int *, int * );
void    RemoveButtonsFromPanel ( ButtonParentObjectType *bpo );
void    RemoveProductsFromList ( Widget list );
int     AddButtonsToPanel ( ButtonParentObjectType  *bpo, void (*func)(),
			    int metObjectType,char *fulldirectorypath );

void 	BuildImageSource ( ButtonParentObjectType * );
void 	ScalePanelCB ( Widget, XtPointer, XtPointer );
void 	ProductPanelCB ( Widget, XtPointer, XtPointer );
void 	SourcePanelCB ( Widget, XtPointer, XtPointer );
void 	UpdateScrolledList ( ButtonObject * );
void 	ImageDisplayCB ( Widget, XtPointer, XtPointer );
void 	FilterProducts ( int, int, char *, char **, int *, int * );
int 	UpdateButtonPanel ( ButtonParentObjectType *, void (*func)(), int,
                            char *, int, char ** );



#endif  /*   _IMGOBJ_H   */
