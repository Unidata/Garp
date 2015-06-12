
/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	latloncb.c
 *
 *	Callbacks and other functions for handling lat/lon grid options.
 *
 *	History:
 *
 *	10/97	COMET	Original copy
 *
 ***********************************************************************/



#include "winobj.h"
#include "genglobs.h"
#include "lineattr.h"
#include "latlongrid.h"
#include "_proto.h"

#define	DEFAULTLATINC	"5.0"
#define	DEFAULTLONINC	"5.0"
#define	DEFAULTLBLINC	"2"
#define	DEFAULTCOLOR	2
#define	DEFAULTTYPE	10
#define	DEFAULTWIDTH	1


/*
 *	Called when the user presses the line attributes dialog apply
 *	button, when set up for the lat/lon grid.
 */
void
SetLatLonLineAttributesCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	int				width, type, color;
	Widget				sample;
	GuiLatLonGridDialogType		*llg;
	GuiLineAttributesDialogType	*atr;

	atr = GetGuiLineAttributesDialog();
	llg = GetGuiLatLonGridDialog();

/*
 *	Get the current state of the line attributes
 */
	width = GetLineAttributesWidth(atr);
	type  = GetLineAttributesType(atr);
	color = GetLineAttributesColor(atr);

/*
 *	Set them in the lat/lon grid GuiLatLonGridDialogType struct
 */
	SetLatLonGridWidth(llg, width);
	SetLatLonGridType(llg,type);
	SetLatLonGridColor(llg,color);

/*
 *	Redraw the lat/lon grid sample line widget to reflect the change
 */
	sample = GetLatLonGridSampleW (llg);
	DrawSampleLine ( sample, color, type, width ) ;

/*
 *	Set a flag indicating the latlon state has changed
 */
	SetLatLonGridChangeState ( llg, True);


}


/*
 *	Called when the text in any of the lat/lon grid text fields changes.
 *	Just sets a flag that the text changed.
 */
void
SetLatLonGridTextCB (Widget w, XtPointer client_data, XtPointer xt_call_data)
{
        GuiLatLonGridDialogType         *llg;

        llg = GetGuiLatLonGridDialog();

	SetLatLonGridChangeState ( llg, True);

}
 


/*
 *	Refreshes the lat/lon sample line widget
 */
void
RefreshLatLonSampleCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	int				width, type, color;
	Widget				sample;
	GuiLatLonGridDialogType		*llg;

	llg = GetGuiLatLonGridDialog();

	width = GetLatLonGridWidth(llg);
	type  = GetLatLonGridType(llg);
	color = GetLatLonGridColor(llg);
	sample = GetLatLonGridSampleW (llg);

	DrawSampleLine ( sample, color, type, width ) ;

}


/*
 *	Causes the line attributes dialog to appear, set up to change
 *	line attributes for the lat/lon grid lines.
 */
void
PopLatLonLineAttributesDialogCB ( Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	Widget				shell, dialog, prevSample;
	Widget				applyButton, frame;
	int				i, color;
	XtCallbackProc			callback;
	XtPointer			clientData;
	GuiLineAttributesDialogType 	*atr;
	GuiLatLonGridDialogType 	*llg;

	atr = GetGuiLineAttributesDialog();
	llg = GetGuiLatLonGridDialog();
/*
 *	Get various widgets
 */	
	shell  = GetLineAttributesShellW(atr);
	dialog = GetLineAttributesDialogW(atr);
	applyButton = GetLineAttributesApplyW(atr);

/*
 *	"Un-depress" the previous sample line widget, and make it appear
 *	that the new one is pressed. Save the new widget id.
 */
	prevSample = GetLineAttributesCurrentSampleW(atr);
	if ( prevSample != NULL )
	 	XtVaSetValues(  XtParent (prevSample),
	 			XmNshadowType, XmSHADOW_OUT, NULL);

 	XtVaSetValues( XtParent (w), XmNshadowType, XmSHADOW_IN, NULL);

	SetLineAttributesCurrentSampleW(atr, w);

/*
 *	Change the title in the dialog
 */	
	XtVaSetValues (shell, XmNtitle, "Lat/Lon Grid Line Attributes", NULL );

/*
 *	Set the state of all the color buttons
 */
        for ( i = 1; i < 33; i++ ) {
        
	    frame = XtParent (GetLineAttributesColorW(atr,i-1));
	    XtVaSetValues( frame, XmNshadowType, XmSHADOW_OUT, NULL);

        }

/*
 *	Set the current lat/lon line attributes into the line attributes
 *	dialog.
 */
	color = GetLatLonGridColor(llg);
	SetLineAttributesColor(atr, color);
	SetLineAttributesType (atr, GetLatLonGridType(llg));
	SetLineAttributesWidth(atr, GetLatLonGridWidth(llg));

/*
 *	"Depress" the color button
 */
	frame = XtParent (GetLineAttributesColorW(atr,color-1));
	XtVaSetValues( frame, XmNshadowType, XmSHADOW_IN, NULL);

/*
 *	Add a callback for the line attributes apply button (remove any
 *	existing one first)
 */
	callback = GetLineAttributesCallback(atr);
	clientData = GetLineAttributesClientData(atr);
	if ( callback != NULL ) {
	    XtRemoveCallback(	applyButton,
	    			XmNactivateCallback,
	    			callback,
	    			(XtPointer) clientData);
	}

	XtAddCallback(	applyButton,
	    		XmNactivateCallback,
	    		SetLatLonLineAttributesCB,
	    		(XtPointer)NULL);
/*
 *	Save the new callback name and client data.
 */	
	SetLineAttributesCallback(atr, SetLatLonLineAttributesCB);
	SetLineAttributesClientData(atr, (XtPointer)NULL);

/*
 *	Manage the bloody thing.
 */
	XtManageChild ( dialog );

	RefreshLineAttributesSampleCB (	w, client_data, xt_call_data );

}




/*
 *	Used to sensitize or desensitize parts of the lat/lon interface
 */
void
SetLatLonSensitive ( BooleanType sensitive)
{
	extern Widget		latlon_rc1;
	extern Widget		latlon_rc2;
	unsigned char		shadowType;
	GuiLatLonGridDialogType	*llg;
	

	llg = GetGuiLatLonGridDialog();

/*
 *	Set the row-column to desensitze everything.
 */ 
        XtSetSensitive (latlon_rc1, sensitive);
        XtSetSensitive (latlon_rc2, sensitive);

/*
 *	Also, set the specific sample line widget, since we will
 *	be checking on it later in the redrawing routine.
 */
        XtVaSetValues (GetLatLonGridSampleW(llg),
        	XmNsensitive, sensitive, NULL);

/*
 *	Set the appearance of the parent sample line widget (the frame)
 */
	if ( sensitive )
		shadowType = XmSHADOW_OUT;
	else
		shadowType = XmSHADOW_ETCHED_IN;

        XtVaSetValues (XtParent(GetLatLonGridSampleW(llg)),
        	XmNshadowType, shadowType, NULL);

}


/*
 *	Initializes the lat/lon grid dialog with default values
 */
void
InitLatLonGridDialog ()
{
	Widget			latIncW, lonIncW, lblIncW;
	Widget			stateW, sampleW;
	int			type, width, color;
	char			*latinc, *loninc;
	char			*lblinc, *state;
	char			*lcolor, *ltype, *lwidth;
	double			flat, flon, flbl;
	BooleanType		on;
	GuiLatLonGridDialogType	*llg;


	llg = GetGuiLatLonGridDialog();

	stateW = GetLatLonGridStateW(llg);
	latIncW = GetLatIncrementW(llg);
	lonIncW = GetLonIncrementW(llg);
	lblIncW = GetLatLonGridLabelIncW(llg);
	sampleW = GetLatLonGridSampleW(llg);
	
	state  = GetConfigValue ( "latlon" );
	latinc = GetConfigValue ( "latinc" );
	loninc = GetConfigValue ( "loninc" );
	lblinc = GetConfigValue ( "lblinc" );
	lcolor = GetConfigValue ( "gridlinecolor" );
	ltype  = GetConfigValue ( "gridlinetype" );
	lwidth = GetConfigValue ( "gridlinewidth" );

	on = False;
	if ( *state == 'y' || *state == 'Y' ) on = True;

	color = atoi ( lcolor);
	type  = atoi ( ltype);
	width = atoi ( lwidth);

	flat = atof (latinc );
	flon = atof (loninc );
	flbl = atof (lblinc );
	
/*
 *	Error-check
 */
	if ( color < 0 || color > 32 ) {
	    printf("Bad lat/lon grid line color (%s)",lcolor);
	    color = DEFAULTCOLOR;
	}
	if ( type < 0 || type > 10 ) {
	    printf("Bad lat/lon grid line type (%s)",ltype);
	    type = DEFAULTTYPE;
	}
	if ( width < 0 || width > 4 ) {
	    printf("Bad lat/lon grid line width (%s)",lwidth);
	    width = DEFAULTWIDTH;
	}
	if ( flat < 0. || flat > 90. )  {
	    printf("Bad lat line increment (%s)",latinc);
	    Free (latinc);
	    latinc = strdup (DEFAULTLATINC);
	}
	if ( flon < 0. || flon > 90. )  {
	    printf("Bad lon line increment (%s)",loninc);
	    Free (loninc);
	    loninc = strdup (DEFAULTLONINC);
	}
	if ( flbl < 0. || flbl > 10 )  {
	    printf("Bad lat/lon label increment (%s)",lblinc);
	    Free (lblinc);
	    lblinc = strdup (DEFAULTLBLINC);
	}

        if ( GetVerboseLevel () > VERBOSE_0 ) {
            printf ("Lat/lon grid on? %s\n",state);
            printf ("Lat grid increment: %s\n", latinc);
            printf ("Lon grid increment: %s\n", loninc);
            printf ("Lat/Lon label increment: %s\n", lblinc);
            printf ("Lat/Lon line color: %s\n", lcolor);
            printf ("Lat/Lon line type: %s\n",  ltype);
            printf ("Lat/Lon line width: %s\n", lwidth);
       }
	
	
/*
 *	Set the widgets to the start-up values
 */
        XtVaSetValues(stateW,
                XmNset, on,
                NULL);
      
	XmTextSetString ( latIncW, latinc );
	XmTextSetString ( lonIncW, loninc );
	XmTextSetString ( lblIncW, lblinc );

	SetLatLonGridWidth(llg, width);
	SetLatLonGridType (llg, type);
	SetLatLonGridColor(llg, color);

/*
 *	Set the proper sensitivity
 */
	SetLatLonSensitive (on);

	Free (state );
	Free (latinc );
	Free (loninc );
	Free (lblinc );
	Free (lcolor );
	Free (ltype  );
	Free (lwidth );

}


/*
 *	Callback for the lat/lon pre-valued convenience buttons.
 */
void
SetLatLonGridIncCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	int			which;
	char			value[10];
	Widget			latIncW, lonIncW;
	GuiLatLonGridDialogType	*llg;


	which = (int) client_data;

	llg = GetGuiLatLonGridDialog();

	latIncW = GetLatIncrementW(llg);
	lonIncW = GetLonIncrementW(llg);

	sprintf (value, "%d.0", which);
	XmTextSetString ( latIncW, value );
	XmTextSetString ( lonIncW, value );

	SetLatLonGridChangeState ( llg, True );
}

/*
 *	Callback for the lat/lon on/off button
 */
void
SetLatLonGridOnCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	
	GuiLatLonGridDialogType		*llg;
	GuiLineAttributesDialogType	*atr;
	Widget				sample;
	BooleanType			state;
	XtCallbackProc			callback;
	XtPointer			clientData;


	atr = GetGuiLineAttributesDialog();
	llg = GetGuiLatLonGridDialog();

	state = XmToggleButtonGetState (w);
	SetLatLonSensitive (state);

/*
 *	Force the sample line to redraw
 */
	sample = GetLatLonGridSampleW (llg);
	RefreshLatLonSampleCB (sample, client_data, xt_call_data );

/*
 *	Special case: check if the user has turned off lat/lon while it
 *	currently controls the line attributes dialog. If so, unmanage
 *	the line attributes dialog.
 */
	if ( (GetLatLonGridSampleW(llg) == GetLineAttributesCurrentSampleW(atr))
		 && ! state ) {

	    XtUnmanageChild ( GetLineAttributesDialogW(atr) );

	    SetLineAttributesCurrentSampleW(atr, NULL);

	    callback = GetLineAttributesCallback(atr);

	    if ( callback != NULL ) {

	        clientData = GetLineAttributesClientData(atr);
	    	XtRemoveCallback( GetLineAttributesApplyW(atr),
	    			XmNactivateCallback,
	    			callback,
	    			(XtPointer) clientData);
	    }
	    
	    SetLineAttributesCallback(atr, NULL);
	    SetLineAttributesClientData(atr, (XtPointer) NULL);
	}

/*
 *	Set the changed state flag
 */
	SetLatLonGridChangeState ( llg, True );

}

