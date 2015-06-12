/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	enhancecb.c
 *
 *	Callbacks and utilities to control image fading and enhancements.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	06/97	COMET	Changed imgobj.h to enhance.h
 *	06/02	Penn State - A. Person
 *			Updated to support 16- and 24-bit graphics
 *
 ***********************************************************************/


#include "guimacros.h"
#include "genglobs.h"
#include "winobj.h"
#include "utils.h"
#include "enhance.h"
#include "_proto.h"


void
FadeScaleCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{

	WindowObjectType		*wo;
	int				cbank;
	int				scale;
	int				fadeWid;
	int				verbose;
	GuiImageEnhanceDialogType	*img;
	Widget				fadeW = NULL;
	
        XmScaleCallbackStruct *cbs =
	       (XmScaleCallbackStruct *) xt_call_data;

	fadeWid = (int) client_data;

	verbose = GetVerboseLevel();

	wo = GetActiveWindowObject();


/*
 *	Get Gempak Color bank number.  
 *		-1   no image loaded
 *		 1   satellite
 *		 2   nids/nowrad 
 */
	cbank = GetGempakColorBank( wo );
	
	scale =   cbs->value ;
	
	if( verbose > VERBOSE_1 )
	    printf ("FadeScaleCB - value: %d\n", cbs->value );

	if ( cbank == -1 ) return;

	ChangeImageBrightness ( wo, cbank, scale );

        if( DefaultDepthOfScreen(DefaultScreenOfDisplay(XtDisplay(w))) != 8 ) {
            RefreshDisplay ( );  /* Necessary for > 8-bit displays to redraw
                                    images with new pixel values reflecting
                                    new read-only color table entries */
        }

/*
 *	Synchronize the fade slider controls. Set the one who did NOT
 *	call this routine.
 */
 	img = GetGuiImageEnhanceDialog();

	if ( fadeWid == MAIN_CONTROL_FADER )
		fadeW = GetSatImageFadeScaleW ( img );
	else if ( fadeWid == ENHANCE_POPUP_FADER )
		fadeW = GetImageFadeScaleW ( img );

	if( fadeW ) XtVaSetValues ( fadeW, XmNvalue, scale, NULL );

}


void
RollEnhanceCB ( Widget w, XtPointer client_data, XtPointer xt_call_data )
{

	WindowObjectType        *wo;
	int			cbank;
	int			scale;
	int			verbose;

	XmScaleCallbackStruct *cbs =
		(XmScaleCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();

        wo = GetActiveWindowObject();


	cbank = GetGempakColorBank( wo );
	
	scale =   cbs->value ;
	
	if( verbose > VERBOSE_1 )
		printf ("RollEnhanceCB - value: %d", cbs->value );

	if ( cbank == -1 ) return;

	RollColorEnhancement ( wo, cbank, scale );

        if( DefaultDepthOfScreen(DefaultScreenOfDisplay(XtDisplay(w))) != 8 ) {
            RefreshDisplay ( );  /* Necessary for > 8-bit displays to redraw
                                    images with new pixel values reflecting
                                    new read-only color table entries */
        }
}


void
pre_satenhance (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	extern Widget	sat_enhance_list;

	XmPushButtonCallbackStruct *cbs =
	            (XmPushButtonCallbackStruct *) xt_call_data;

	SetSatEnhanceList ( sat_enhance_list );
}


void
SatEnhanceRetainCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	extern Widget		sat_enhance_but;

	BooleanType		retain;
	GlobalDataObjectType	*gd;

        XmToggleButtonCallbackStruct *cbs =
	              (XmToggleButtonCallbackStruct *) xt_call_data;

/*
 *	Save color table as default if "Retain" button was pushed.
 */
	retain =  XmToggleButtonGetState ( sat_enhance_but );

/*
 *	Save parameters.
 */
	gd = GetGlobalDataObject();

	SetDefColortableRetain ( gd, retain );
}


void
SatEnhancementCB  (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	extern Widget	sat_enhance_list;
	extern Widget	sat_enhance_but;

	extern struct enhance_list	satenhance_list;

	char		color_table[20];
	int		*pos_list;
	int		pos_cnt;
	int		i;

	GlobalDataObjectType	*gd;

	XmListCallbackStruct *cbs =
	              (XmListCallbackStruct *) xt_call_data;

/*
 *	Get color table to use.
 */
	if ( ! ( XmListGetSelectedPos ( sat_enhance_list,
	                              &pos_list, &pos_cnt ) &&
	                            ( pos_cnt == 1 ) ) ) return;
	i = pos_list[0] - 1;
	free ( pos_list );
	strcpy ( color_table, satenhance_list.list[i] );

/*
 *	Add extension for color tables.
 */
	if ( ! ( strcmp ( color_table, "DEFAULT" ) == 0 ||
	         strcmp ( color_table, "gray" )    == 0  ) )
	    strcat ( color_table, ".tbl" );

/*
 *	Save parameters.
 */
	gd = GetGlobalDataObject();

	SetDefColortable ( gd, color_table );
	SetBackgroundColors ( gd, satenhance_list.background[i] );
	SetForegroundColors ( gd, satenhance_list.foreground[i] );

/*
 *	Change lutfile.
 */
	newlut ( color_table );

	if( DefaultDepthOfScreen(DefaultScreenOfDisplay(XtDisplay(w))) != 8 ) {
	    RefreshDisplay ( );  /* Necessary for > 8-bit displays to redraw
				    images with new pixel values reflecting
				    new read-only color table entries */
	}
}


void
ResetLutFileCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
/*
 * Reset image Look Up Table.
 */
{
	int			cbank, verbose;
	WindowObjectType	*wo;
	

        XmPushButtonCallbackStruct *cbs = 
		    (XmPushButtonCallbackStruct *) xt_call_data;

	wo = GetActiveWindowObject();

/*
 *	Get Gempak Color bank number.  
 *		-1   no image loaded
 *		 1   satellite
 *		 2   nids/nowrad 
 */
	cbank = GetGempakColorBank( wo );

	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_1 ) {
		printf ("ResetLutFileCB, bank: %d", cbank );
	}

	if ( cbank == -1 ) return;
		
	ResetImageColors ( wo, cbank );
}


void
InvertImageColorsCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
/*
 * Invert color table for image.
 */
{
	WindowObjectType	*wo;
	int			verbose, cbank;


        XmToggleButtonCallbackStruct *cbs = 
		      (XmToggleButtonCallbackStruct *) xt_call_data;

	verbose = GetVerboseLevel();

	wo = GetActiveWindowObject();

/*
 *	Get Gempak Color bank number.
 *		-1   no image loaded
 *		 1   satellite
 *		 2   nids/nowrad
 */
	cbank = GetGempakColorBank( wo );

	if( verbose > VERBOSE_1 ) {
		printf ("InvertImageColors, bank: %d", cbank );
		printf (" toggle setting: %d\n", cbs->set );
	}

	if ( cbank == -1 ) return;

	InvertImageColors ( wo, cbank, cbs->set );

	if( DefaultDepthOfScreen(DefaultScreenOfDisplay(XtDisplay(w))) > 8 )
            RefreshDisplay ( );  /* Necessary for > 8-bit displays to redraw
                                    images with new pixel values reflecting
                                    new read-only color table entries */
}



