/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	framecontrolcb.c
 *
 *	Callbacks and utilities to control frame widgets.	
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include "winobj.h"
#include "framecount.h"
#include "_proto.h"


void SetCurrentFrameCB (Widget w, XtPointer client_data, XtPointer xt_call_data );
void UpdateFrameButtons (WindowObjectType *wo);
void FillAnimationFrameFormCB (Widget w, XtPointer client_data, XtPointer xt_call_data);
void UpdateFrameCounterWidgets ( WindowObjectType *wo );
void UpdateFrameComponents ( WindowObjectType *wo );
void UpdateFrameEditButtons ( WindowObjectType *wo );
void ReverseFrameButtonColors ( int index );
void RemoveFrameFromLoopCB (Widget w, XtPointer client_data, XtPointer xt_call_data );
void AddFrameIntoLoopCB (Widget w, XtPointer client_data, XtPointer xt_call_data );
BooleanType QueryPointerPosition ( WindowObjectType *wo, int *X, int *Y );


/*
 *	SetCurrentFrameCB ()
 *
 *	Steps to a particular frame of the loop in response to user
 *	frame button push.
 */

void
SetCurrentFrameCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{

	WindowObjectType	*wo;
	int			index;
	
	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;


	index = (int) client_data;
	wo = GetActiveWindowObject ();

	if ( ! wo->pixmapObjects[index]->inUse ) return;

	AnimationObjectStepToFrame ( wo, index );

	UpdateFrameEditButtons ( wo );
}


/*
 *	UpdateFrameButtons ()
 *
 *	Creates the frame button pop-up that allows the user to access frames
 *	randomly, remove or add frames in the loop
 */

void
UpdateFrameButtons (WindowObjectType *wo)
{

	Widget			counter_frame;
	static Widget		frame_rowcol;
	ButtonObject		*bo;
	ButtonParentObjectType	*bpo;
	GuiFrameCounterType	*frm;
	int			total_frames, columns, i;
	char			frame [4];


/*
 *	See if the frame buttons need to be remade or not. If the
 *	current number of buttons and the number of pixmaps agree,
 *	exit.
 */
 	frm = GetGuiFrameCounter();
	bpo = &( frm->frmbuttons );

	total_frames = GetWindowObjectNumPixmapObjects (wo);

	if ( bpo->button_cnt == total_frames ) return;
	
/*
 *	Build frame button row/column widget. Try to get 9 max in each row.
 */	
	if ( frame_rowcol ) XtDestroyWidget (frame_rowcol );
	
	if ( total_frames == 0 ) {
	    frame_rowcol = NULL;
	    bpo->button_cnt = 0;
	    return;
	}

	columns = (total_frames + 8)/ 9;	

	counter_frame = GetCounterFrameW ( frm );

	frame_rowcol = XtVaCreateWidget( "framerowcolumn",
				xmRowColumnWidgetClass, counter_frame,
				XmNorientation, XmHORIZONTAL,
				XmNpacking, XmPACK_COLUMN,
				XmNnumColumns, columns,
				NULL);


/*
 *	Build all the buttons. If the inLoop flag is not set,
 *	change the button appearance. (reverse colors)
 */
	bpo->parent = counter_frame;
	bpo->button_cnt = total_frames;
	bpo->button_alloc_cnt = bpo->button_cnt;
	bo = &(bpo->buttons[0]);

	for ( i=0; i<total_frames; i++, bo++ ) {
	    sprintf ( frame, "%d", i + 1);
	    bo->w = XtVaCreateManagedWidget( frame,
				xmPushButtonWidgetClass, frame_rowcol,
				XmNfillOnArm, TRUE,
				NULL);

	    XtAddCallback ( bo->w, XmNactivateCallback, SetCurrentFrameCB,
	    		    (XtPointer) i );
	    		    
	    bo->ismanaged = 1;
	    if ( bo->label != NULL ) Free (bo->label);
	    bo->label = strdup ( frame );
	    
	    if ( ! wo->pixmapObjects[i]->inLoop ) ReverseFrameButtonColors (i);

	}

	XtManageChild ( frame_rowcol );

}


/*
 *	FillAnimationFrameFormCB ()
 *
 *	Callback accessed when the user presses the frame counter button
 *	on the main GARP gui control panel
 */

void
FillAnimationFrameFormCB (Widget w, XtPointer client_data, XtPointer xt_call_data)
{

	WindowObjectType	*wo;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	wo = GetActiveWindowObject ();
	
	UpdateFrameButtons ( wo );
	
	UpdateFrameEditButtons ( wo );

}
	

/*
 *	UpdateFrameCounterWidgets ()
 *
 *	Updates all the frame number widgets.
 */
 
void
UpdateFrameCounterWidgets ( WindowObjectType *wo )

{

	char			string [20];
	GuiFrameCounterType	*frm;
	XmString		label;
	Widget			counterW, currentW;
	int			index, frame, total_frames;

/*
 *	Update the frame counter widget first. (eg; "5 of 10")
 */
 	frm = GetGuiFrameCounter();
	counterW = GetFrameCounterW ( frm );

	index = GetActivePixmapObjectIndex (wo);
	frame = index + 1;
	total_frames = GetWindowObjectNumPixmapObjects (wo);

	if ( frame > total_frames ) frame = total_frames;
	
	sprintf (string, "%d\nof\n%d", frame, total_frames);
	label = XmStringLtoRCreate ( string,
	        (XmStringCharSet)XmFONTLIST_DEFAULT_TAG );

	XtVaSetValues ( counterW, XmNlabelString, label, NULL );
	RefreshWidgetNow ( counterW );
	XmStringFree ( label );

/*
 *	Update the current frame widget in the frame button pop-up if
 *	it is currently managed (visible)
 */
	currentW = GetCurrentFrameLabelW ( frm );
	if ( ! XtIsManaged (currentW )) return;

	sprintf (string, "%d", frame);
	label = XmStringLtoRCreate ( string,
	        (XmStringCharSet)XmFONTLIST_DEFAULT_TAG );
	
	XtVaSetValues ( currentW, XmNlabelString, label, NULL );
	RefreshWidgetNow ( currentW );
	XmStringFree ( label );

}


/*
 *	UpdateFrameEditButtons ()
 *
 *	Updates the frame editing buttons (remove, undo) to be
 *	sensitive or not depending on the status of the current
 *	frame.
 */

void
UpdateFrameEditButtons ( WindowObjectType *wo )
{

	GuiFrameCounterType	*frm;
	Widget			removeW, undoW;
	int			index;


 	frm = GetGuiFrameCounter();
	removeW = GetFrameEditRemoveW ( frm );
	undoW   = GetFrameEditUndoW ( frm );

	index = GetActivePixmapObjectIndex (wo);

	if ( wo->numPixmapObjects == 0 || 
	     AnimationRunning ( wo ) ) {

	    XtVaSetValues ( removeW, XmNsensitive, False, NULL );
	    XtVaSetValues ( undoW, XmNsensitive, False, NULL );
	}
	else if ( wo->pixmapObjects[index]->inLoop ) {

	    XtVaSetValues ( removeW, XmNsensitive, True, NULL );
	    XtVaSetValues ( undoW, XmNsensitive, False, NULL );
	}
	else {
	
	    XtVaSetValues ( removeW, XmNsensitive, False, NULL );
	    XtVaSetValues ( undoW, XmNsensitive, True, NULL );
	}

}

/*
 *	UpdateFrameComponents ()
 *
 *	This function updates things that need updating whenever a new
 *	pixmap (frame) is put up on the screen. The current X and Y cursor
 *	position is obtained from the Xserver, since the last X,Y 
 *	position might be out of date when this routine is called
 */
 
void
UpdateFrameComponents ( WindowObjectType *wo )
{

	int		X, Y;
	

	if ( QueryPointerPosition ( wo, &X, &Y ) ) {
	
	    SetCurrentX ( wo, X );
	    SetCurrentY ( wo, Y );
 	}

	UpdateTitleObject( wo );

	SetPixmapGeoMapping( wo );

	UpdateStatusBar( wo );

	UpdateFrameCounterWidgets ( wo );

	UpdateFrameButtons ( wo );

	UpdateFrameEditButtons ( wo );
	
	UpdateAnchorPoint ( wo );

}


/*
 *	ReverseFrameButtonColors ()
 *
 *	Reverses the colors of the frame button. Swaps foreground
 *	and background colors.
 */
 
void
ReverseFrameButtonColors ( int index )
{

	ButtonObject		*bo;
	ButtonParentObjectType	*bpo;
	GuiFrameCounterType	*frm;
	int			frame;
	Pixel			fcolor, bcolor;

	
 	frm = GetGuiFrameCounter();
	bpo = &( frm->frmbuttons );
	bo = &(bpo->buttons[index]);

	if ( ! bo->w ) return;


	XtVaGetValues ( bo->w, XmNforeground, &fcolor, NULL );
	XtVaGetValues ( bo->w, XmNbackground, &bcolor, NULL );

	XtVaSetValues ( bo->w,
			XmNforeground, bcolor, 
			XmNbackground, fcolor, 
			NULL );

}


/*
 *	RemoveFrameFromLoopCB ()
 *
 *	Callback to remove the current frame from the loop. Changes
 *	button appearance, toggles inloop flag off.
 */

void 
RemoveFrameFromLoopCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{

	WindowObjectType	*wo;
	int			index;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;

	wo = GetActiveWindowObject ();
	index = GetActivePixmapObjectIndex (wo);

	if ( ! wo->pixmapObjects[index]->inUse ) return;

/*
 *	Order is important here
 */
	wo->pixmapObjects[index]->inLoop = False;

	UpdateFrameEditButtons ( wo );

	ReverseFrameButtonColors ( index );

}


/*
 *	AddFrameIntoLoopCB ()
 *
 *	Callback to add back in a frame that has already been taken out
 *	of the loop
 */

void 
AddFrameIntoLoopCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	WindowObjectType	*wo;
	int			index;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) xt_call_data;
			
	wo = GetActiveWindowObject ();
	index = GetActivePixmapObjectIndex (wo);

	if ( ! wo->pixmapObjects[index]->inUse ) return;

/*
 *	Order is important here
 */
	wo->pixmapObjects[index]->inLoop = True;

	UpdateFrameEditButtons ( wo );

	ReverseFrameButtonColors ( index );

}

