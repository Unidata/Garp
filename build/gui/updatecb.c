/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	updatecb.c
 *
 *	AutoUpdate and Refresh callbacks and GUI code.
 *
 *	History:
 *
 *	 8/97	COMET	Original copy
 *	10/97	COMET	Set auto update initial state
 *
 ***********************************************************************/


#include "genglobs.h"
#include "winobj.h"
#include "wincb.h"
#include "update.h"
#include "version.h"
#include "imgobj.h"
#include "model.h"
#include "xsecobj.h"
#include "thgtobj.h"
#include "vprofobj.h"



/*
 *  SetAutoUpdateCB(): called when the user presses the APPLY button in the
 *  auto update dialog. Gets the auto update dialog widget values and starts
 *  auto update object with them.
 */

void SetAutoUpdateCB( Widget    w,
                 XtPointer clientData,
                 XtPointer callData )
{
	BooleanType		on;
	int			interval;
	Widget			toggleW, scaleW;
	GuiAutoUpdateDialogType	*au;
	
	
/*
 *	Get the values of the auto update widgets
 */
	au = GetGuiAutoUpdateDialog();
	scaleW = GetAutoUpdateScaleW(au);
	toggleW = GetAutoUpdateToggleW(au);

	on =  XmToggleButtonGetState ( toggleW );

	XtVaGetValues(scaleW,
		XmNvalue, &interval,
		NULL);

/*
 *	Start autoupdate with these values
 */
	SetAutoUpdateObject ( on, interval );

}



/*
 *  ToggleAutoUpdateCB(): This function is called when the user presses
 *  the autoupdate toggle button. It only sensitizes or de-sensitizes
 *  the scale widget. It does not start autoupdate, that is done when the
 *  user presses APPLY.
 */

void ToggleAutoUpdateCB( Widget    w,
                 XtPointer clientData,
                 XtPointer callData)
{
	BooleanType		on;
	Widget			scaleW, toggleW;
	GuiAutoUpdateDialogType	*au;
	

/*
 *	Get the auto update widget ID's
 */
	au = GetGuiAutoUpdateDialog();	
	scaleW  = GetAutoUpdateScaleW(au);
	toggleW = GetAutoUpdateToggleW(au);

	on =  XmToggleButtonGetState ( toggleW );

/*
 *	Sensitize the scale widget based on whether update is on or off.
 */
	XtVaSetValues(scaleW,
		XmNsensitive, on,
		NULL);


}



/*
 *  SetAutoUpdateWidgets(): Set the values of the widgets in the auto
 *  update dialog. This function is called only at GARP startup when the
 *  default values are read from the Garp_defaults file.
 */

void SetAutoUpdateWidgets( BooleanType on, int scale)
{
	Widget			scaleW, toggleW;
	GuiAutoUpdateDialogType	*au;
	

/*
 *	Get the auto update widget ID's
 */
	au = GetGuiAutoUpdateDialog();
	scaleW  = GetAutoUpdateScaleW(au);
	toggleW = GetAutoUpdateToggleW(au);

/*
 *	Set the values. Also set the scale widget sensitivity based on the
 *	toggle widget status. (ie, if auto-update is off, gray out the scale
 *	widget)
 */
	XtVaSetValues(toggleW,
		XmNset, on,
		NULL);

	XtVaSetValues(scaleW,
		XmNvalue, scale,
		XmNsensitive, on,
		NULL);

/*
 *	Set the state flag to False (ie, not currently updating)
 */
 	SetAutoUpdateStateFlag( au, False);


}


/*
 *  RefreshDataCB(): Called when user presses the "Refresh" button in the
 *  data display dialog.
 */

void RefreshDataCB( Widget    w,
                 XtPointer clientData,
                 XtPointer callData)
{
	int			verbose;
	int			metObjectType;
	ButtonObject 		*prodb;
	ButtonObject 		*scaleb;
	ButtonObject 		*sourceb;
        ModelInfoType           *mdl;
        char                    *model = NULL;
	WindowObjectType	*wo;
	GuiImgSelectDialogType	*img;
        GuiTimeHeightDialogType  *tht;
	GuiVertProfileObjectType *vpt;
	GuiXSectionDialogType    *vcs;
	char			string[] = "Refreshing data...";
	XmToggleButtonCallbackStruct cbs;
	

	
	metObjectType = (int) clientData;
	wo = GetActiveWindowObject ();
	verbose = GetVerboseLevel();

	if ( verbose > VERBOSE_0 ) 
		printf("RefreshDataCB: type = %d\n",metObjectType);


	SetGarpLogo ( wo, string);

	switch ( metObjectType ) {

/*
 *	For SAT (any imagery) refresh the lowest panel that has something
 *	selected (files, product, resolution, then source)
 */
	case SATOBJECT:
 		img = GetGuiImgSelectDialog();
		prodb = GetImgProdPanelSetbutton(img);
		scaleb = GetImgScalePanelSetbutton(img);
		sourceb = GetImgSourcePanelSetbutton(img);
		cbs.set = True;

		if ( prodb )
			ProductPanelCB ( img->time_list.w,
					 (XtPointer)prodb,
					 (XtPointer)&cbs);
		else if ( scaleb )
			ScalePanelCB ( img->time_list.w,
					(XtPointer)scaleb,
					(XtPointer)&cbs);
		else if ( sourceb )
			SourcePanelCB ( img->time_list.w,
					(XtPointer)sourceb,
					(XtPointer)&cbs);
		
                break;

	case SURFACEOBJECT:
		SurfaceObsIconCB (w, clientData, callData );
                break;

	case WINDPROFILEROBJECT:
		ProfilerIconCB (w, clientData, callData );
                break;

	case UPPERAIROBJECT:
		SelectUpaIconCB (w, clientData, callData );
                break;

	case UPPERAIRPROFILEOBJECT:
		SelectUpaIconCB (w, clientData, callData );
                break;
	
/*
 *	To refresh model data, remove the cache entry for the current
 *	model, then call the callback to update the specific dialog.
 *	This will cause the data to be re-read from the filesystem
 *	instead of the cache.
 */
	case SCALARGRIDOBJECT:
        	mdl = GetGuiModelInfo();
        	model = GetModelType ( mdl );
		if ( model ) RemoveModelListCacheEntry ( model );
		GridIconCB (w, clientData, callData );
                break;

	case XSECTIONGRIDOBJECT:
		vcs = GetGuiXSectionDialog();
        	model = GetModelTypeXS ( vcs );
		if ( model ) RemoveModelListCacheEntry ( model );
		VCSIconCB (w, clientData, callData );
                break;

	case TIMEHEIGHTOBJECT:
		tht = GetGuiTimeHeightDialog();
        	model =  GetModelTypeTH ( tht );
		if ( model ) RemoveModelListCacheEntry ( model );
		TimeHeightIconCB (w, clientData, callData );
                break;

	case VERTICALPROFILEOBJECT:	
		vpt = GetGuiVertProfileDialog();
        	model = GetModelTypeVP ( vpt );
		if ( model ) RemoveModelListCacheEntry ( model );
		VerticalProfileIconCB (w, clientData, callData );
                break;

	}


	ResetGarpLogo ( wo );

}


