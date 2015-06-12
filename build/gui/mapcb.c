/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	mapcb.c
 *
 *	Map listing utilities
 *
 *	History:
 *
 *	10/97	COMET	Original copy
 *	12/97	COMET	Fixed map "Sample Line" label attachment
 *	 1/98	COMET	Added sanity tests for undefined keys.
 *
 ***********************************************************************/


#include "genglobs.h"
#include "winobj.h"
#include "maplist.h"
#include "lineattr.h"
#include "strfunc.h"
#include "_proto.h"



typedef struct {
	char		*key;
	char		*value;
} MapKeyValues;

/*
 *
 *	Fallback values for Map backgrounds, if we fail to get
 *	them from the configuration file.
 *
 */
static MapKeyValues FallBackMapFiles[] = { 
		"LowResCostal",            "locowo.gsf",
		"MediumResPoliticalWorld", "mepowo.gsf",
		"HiResPoliticalWorld",     "hipowo.gsf",
		"HiResCountyUS",           "hicnus.gsf",
		NULL,                       NULL};

static MapKeyValues *MapFiles;


/*
 *
 *	Read mapfile names from the configuration file and stuff
 *	them into a data structure.
 *
 */
void InitMapObjects()
{

	MapKeyValues	*mp;
	MapKeyValues	*mp2;

	MapFiles = ( MapKeyValues *) XtMalloc( sizeof(FallBackMapFiles) );

	for( mp = MapFiles, mp2 = FallBackMapFiles; mp2->key ; mp2++, mp++ ) {

		mp->key = strdup( mp2->key );

		mp->value = GetConfigValue( mp->key );

		if( ! mp->value )
			mp->value = strdup( mp2->value );

	}

/*
 *
 *	Take care of the last one by ensuring the members have NULL
 *	values.
 *
 */
	mp->key = mp->value = NULL;

	if( GetVerboseLevel() > VERBOSE_0 )
		printf("InitMapObjects\n");

/*
 *	Debug...
 */
	for( mp = MapFiles; mp->key ; mp++ )
		if( GetVerboseLevel() > VERBOSE_1 ) {
			printf("  %s:", mp->key );
			printf("  %s\n", mp->value );
		}

	return;

}


/*
 *	Called when the text changes in any map list scale text box. Just
 *	sets a flag so we know that something has changed.
 */
void
SetScaleTextCB ( Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	GuiMapBackgroundDialogType      *mbg;

	mbg = GetGuiMapBackgroundDialog();

/*
 *	Just set a flag that the state of the map list has changed
 */
	SetMapListChangeState ( mbg, True );

}


/*
 *	Called when the user presses the line attributes dialog apply
 *	button, when set up for changing map line attributes.
 */
void
SetMapLineAttributesCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	int				width, type, color;
	int				which;
	Widget				sample;
	GuiMapBackgroundDialogType	*mbg;
	GuiLineAttributesDialogType	*atr;


	which = (int) client_data;

	atr = GetGuiLineAttributesDialog();
	mbg = GetGuiMapBackgroundDialog();

/*
 *	Get the current state of the line attributes
 */
	width = GetLineAttributesWidth(atr);
	type  = GetLineAttributesType(atr);
	color = GetLineAttributesColor(atr);

/*
 *	Set them in the map GuiMapBackgroundDialogType struct
 */
	SetMapListLineWidth(mbg, which, width);
	SetMapListLineType(mbg, which, type);
	SetMapListLineColor(mbg, which, color);

/*
 *	Redraw the map sample line widget to reflect the change
 */
	sample = GetMapListSampleCanvasW (mbg, which);
	DrawSampleLine ( sample, color, type, width ) ;

/*
 *	Indicate in the map list struct that the state has changed
 */
	SetMapListChangeState ( mbg, True );

}


/*
 *      Refreshes the map sample line widget
 */
void
RefreshMapSampleCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
        int                             width, type, color;
	int				which;
        Widget                          sample;
        GuiMapBackgroundDialogType	*mbg;

        mbg  = GetGuiMapBackgroundDialog();
	which = (int) client_data;

        width = GetMapListLineWidth(mbg,which);
        type  = GetMapListLineType(mbg,which);
        color = GetMapListLineColor(mbg,which);
        sample = GetMapListSampleCanvasW (mbg,which);

        DrawSampleLine ( sample, color, type, width ) ;

}



/* 
 *	Sensitizes or de-sensitizes a map entries widgets.
 */
void
ToggleMapListWidgets ( BooleanType state, int which )
{
	GuiMapBackgroundDialogType 	*mbg;
	unsigned char			shadowType;

	mbg = GetGuiMapBackgroundDialog();


	XtVaSetValues ( GetMapListMaxScaleW(mbg,which),
			XmNsensitive, (Boolean)state, NULL );

	XtVaSetValues ( GetMapListMinScaleW(mbg,which),
			XmNsensitive, (Boolean)state, NULL );

	if ( state )
		shadowType = XmSHADOW_OUT;
	else
		shadowType = XmSHADOW_ETCHED_IN;

	XtVaSetValues ( GetMapListSampleFrameW(mbg,which),
			XmNshadowType, shadowType,
			XmNsensitive, (Boolean)state, NULL );

	XtVaSetValues ( GetMapListSampleCanvasW(mbg,which),
			XmNsensitive, (Boolean)state, NULL );


}


/*
 *	Callback for when the user presses a map's On button. 
 */
void
ToggleMapCB ( Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	int				which;
	BooleanType			state;
	Widget				sample;
	XtCallbackProc			callback;
	XtPointer			clientData;
	GuiMapBackgroundDialogType 	*mbg;
	GuiLineAttributesDialogType	*atr;


	which = (int) client_data;
	mbg = GetGuiMapBackgroundDialog();
	atr = GetGuiLineAttributesDialog();

	state = XmToggleButtonGetState ( GetMapListOnToggleW(mbg,which) );

	ToggleMapListWidgets ( state, which );
/*
 *	Set flag indicating the map state has changed
 */
	SetMapListChangeState ( mbg, True );

/*
 *	Force the sample line to redraw
 */
        sample = GetMapListSampleCanvasW (mbg, which);
	RefreshMapSampleCB (sample, client_data, xt_call_data );

/*
 *	Special case: check if the user has turned off the map which
 *	currently controls the line attributes dialog. If so, unmanage
 *	the line attributes dialog and unset its current settings.
 */
	if ( (GetMapListSampleCanvasW(mbg,which) == 
		GetLineAttributesCurrentSampleW(atr)) && ! state ) {

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

}


/*
 *	Causes the line attributes dialog to appear, set up to change
 *	line attributes for a particular map.
 */
void
PopMapLineAttributesDialogCB ( Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	Widget				shell, dialog, prevSample;
	Widget				applyButton, frame;
	int				i, color;
	int				which;
	XtCallbackProc			callback;
	XtPointer			clientData;
	GuiLineAttributesDialogType 	*atr;
	GuiMapBackgroundDialogType 	*mbg;


	which = (int) client_data;

	atr = GetGuiLineAttributesDialog();
	mbg = GetGuiMapBackgroundDialog();
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
	XtVaSetValues (shell, XmNtitle, mbg->name[which].string, NULL );

/*
 *	Set the state of all the color buttons
 */
        for ( i = 1; i < 33; i++ ) {
        
	    frame = XtParent (GetLineAttributesColorW(atr,i-1));
	    XtVaSetValues( frame, XmNshadowType, XmSHADOW_OUT, NULL);

        }

/*
 *	Set the current map line attributes into the line attributes
 *	dialog.
 */
	color = GetMapListLineColor(mbg, which);
	SetLineAttributesColor(atr, color);
	SetLineAttributesType (atr, GetMapListLineType(mbg, which));
	SetLineAttributesWidth(atr, GetMapListLineWidth(mbg, which));

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
	    		SetMapLineAttributesCB,
	    		(XtPointer) which);
/*
 *	Save the new callback name
 */	
	SetLineAttributesCallback(atr, SetMapLineAttributesCB);
	SetLineAttributesClientData(atr, (XtPointer)which);

/*
 *	Manage the bloody thing.
 */
	XtManageChild ( dialog );

	RefreshLineAttributesSampleCB (	w, client_data, xt_call_data );

}

/*
 *	Builds the map list widgets in the map list dialog.
 */
 
void
InitMapList ()
{

        FILE    			*ftbl;
	Widget				nameLabel, onLabel,
					minScaleLabel, maxScaleLabel,
					sampleLabel, maplistFW;
	Widget				prevNameW, prevOnW,
					prevMinW, prevMaxW,
					prevFrameW;
        char				record[133];
	char				*tbldir, *listfile, *maplist;
        char				mname[27];
        char				mfile[21], minscale[7];
        char				onflag[4], maxscale[7];
	char				*key1="tabledir";
	char				*key2="maplisttbl";
        int				i, color, type, width;
        int				topOffset, leftOffset;
        Dimension			maxHeight, widthName,
        				widthOn, widthMax, widthMin,
        				widthFrame, widthW;
	BooleanType 			onToggle;
	GuiMapBackgroundDialogType	*mbg;


	mbg = GetGuiMapBackgroundDialog();
	
        mbg->nmaps = i = 0;	

/*
 *	Get the table file name 
 */
	tbldir = GetConfigValue ( key1 );
	if ( tbldir == NULL ) {
	    printf("  Warning: key '%s' is not defined in Garp_defaults\n",
                    key1);
	    return;
	}

	listfile = GetConfigValue ( key2 );
	if ( listfile == NULL ) {
	    printf("  Warning: key '%s' is not defined in Garp_defaults\n",
	           key2);
	    return;
	}

	maplist = builddirpath (tbldir, listfile);

	Free ( tbldir );
	Free ( listfile );

	if ( GetVerboseLevel() > VERBOSE_0 )
		printf ("InitMapList(), map list file = %s,", maplist);

/*
 *	Open the table file
 */
	if ( (ftbl = fopen ( maplist, "r")) == NULL ) {
	    printf ("Error: could not find map list table file (%s)\n",
	    		maplist);
	    Free ( maplist );
	    exit(1);
	}

	maplistFW = GetMapListFormW( mbg);

/*
 *	Create the top row labels, attach to form.
 */
	nameLabel = XtVaCreateManagedWidget( "Map Name", xmLabelWidgetClass,
	    			maplistFW, XmNtopAttachment, XmATTACH_FORM,
	    			XmNtopOffset, 5,
	    			XmNleftAttachment, XmATTACH_FORM,
	    			XmNleftOffset, 5,
	    			XmNalignment, XmALIGNMENT_BEGINNING,
	    			NULL);

	onLabel = XtVaCreateManagedWidget( "On", xmLabelWidgetClass,
	    			maplistFW, XmNtopAttachment, XmATTACH_FORM,
	    			XmNtopOffset, 5,
	    			XmNalignment, XmALIGNMENT_BEGINNING,
	    			XmNleftAttachment, XmATTACH_WIDGET,
	    			XmNleftWidget, nameLabel,
	    			XmNleftOffset, 5,
	    			NULL);

	maxScaleLabel = XtVaCreateManagedWidget( "Max Scale", xmLabelWidgetClass,
	    			maplistFW, XmNtopAttachment, XmATTACH_FORM,
	    			XmNtopOffset, 5,
	    			XmNalignment, XmALIGNMENT_BEGINNING,
	    			XmNleftAttachment, XmATTACH_WIDGET,
	    			XmNleftWidget, onLabel,
	    			XmNleftOffset, 5,
	    			NULL);

	minScaleLabel = XtVaCreateManagedWidget( "Min Scale", xmLabelWidgetClass,
	    			maplistFW, XmNtopAttachment, XmATTACH_FORM,
	    			XmNtopOffset, 5,
	    			XmNalignment, XmALIGNMENT_BEGINNING,
	    			XmNleftAttachment, XmATTACH_WIDGET,
	    			XmNleftWidget, maxScaleLabel,
	    			XmNleftOffset, 5,
	    			NULL);

	sampleLabel = XtVaCreateManagedWidget( "Sample Line", xmLabelWidgetClass,
	    			maplistFW, XmNtopAttachment, XmATTACH_FORM,
	    			XmNtopOffset, 5,
	    			XmNalignment, XmALIGNMENT_BEGINNING,
	    			XmNleftAttachment, XmATTACH_WIDGET,
	    			XmNleftWidget, minScaleLabel,
	    			XmNleftOffset, 5,
	    			XmNrightAttachment, XmATTACH_FORM,
	    			NULL);

/*
 *	Initialize the max widths for each column using the label widths
 */
	XtVaGetValues ( nameLabel, XmNwidth, &widthName, NULL );
	XtVaGetValues ( onLabel, XmNwidth, &widthOn, NULL );
	XtVaGetValues ( minScaleLabel, XmNwidth, &widthMin, NULL );
	XtVaGetValues ( maxScaleLabel, XmNwidth, &widthMax, NULL );
	XtVaGetValues ( sampleLabel, XmNwidth, &widthFrame, NULL );

/*
 *	Loop over the entries in the table file, building all the widgets.
 */
	while ( fgets ( record, 132, ftbl ) != NULL ) {

	    if ( record[0] == '!' ||
	         record[0] == ' ' ||
	         record[0] == '\n') continue;
	    
	    sscanf ( record, "%26c %s %s %s %s %d %d %d",
	    	mname, mfile, onflag, maxscale, minscale, &color, &type, &width);
	    mname[26] = '\0';

	    onToggle = False;
	    if ( onflag[0] == 'y' || onflag[0] == 'Y' ) onToggle = True;
	    	
/*
 *	    Map name label and path
 */
	    mbg->name[i].w = XtVaCreateManagedWidget( mname, 
	    			xmLabelWidgetClass,
	    			maplistFW,
	    			XmNalignment, XmALIGNMENT_BEGINNING,
	    			NULL);
	    			
	    mbg->name[i].string = strdup ( mname);

	    mbg->path[i] = strdup ( mfile );

/*
 *	    On/off toggle
 */
	    mbg->on[i].w = XtVaCreateManagedWidget( " ",
	    			xmToggleButtonWidgetClass,
	    			maplistFW,
	    			XmNset, onToggle,
	    			NULL );

	    XtAddCallback( mbg->on[i].w, XmNvalueChangedCallback,
				ToggleMapCB, (XtPointer) i );
/*
 *	    Max scale text widget
 */
	    mbg->maxScale[i].w = XtVaCreateManagedWidget ( " ",
				xmTextFieldWidgetClass,
				maplistFW, XmNcolumns, 10,
				XmNvalue, maxscale,
				NULL);

	    XtAddCallback( mbg->maxScale[i].w, XmNvalueChangedCallback,
				SetScaleTextCB, (XtPointer) i );
/*
 *	    Min scale text widget
 */
	    mbg->minScale[i].w = XtVaCreateManagedWidget ( " ",
				xmTextFieldWidgetClass,
				maplistFW, XmNcolumns, 10,
				XmNvalue, minscale,
				NULL);

	    XtAddCallback( mbg->minScale[i].w, XmNvalueChangedCallback,
				SetScaleTextCB, (XtPointer) i );
/*
 *	    Sample line attributes
 */
	    mbg->color[i] = color;
	    mbg->type[i]  = type;
	    mbg->width[i] = width;

	    mbg->sampleFrameW[i] = XtVaCreateManagedWidget("frame",
				xmFrameWidgetClass,
				maplistFW,
				XmNwidth, 110,
				NULL);

	    mbg->sampleCanvas[i] = XtVaCreateManagedWidget( "darea", 
				xmDrawingAreaWidgetClass,
				mbg->sampleFrameW[i],
				NULL);

	    XtAddEventHandler(mbg->sampleCanvas[i],
                        ButtonReleaseMask, FALSE,
                        (XtEventHandler)PopMapLineAttributesDialogCB,
                        (XtPointer) i);

	    XtAddCallback( mbg->sampleCanvas[i], XmNexposeCallback,
			RefreshMapSampleCB, (XtPointer) i );


/*
 *	    Save the max width's for each column.
 */
	    XtVaGetValues ( mbg->name[i].w, XmNwidth, &widthW, NULL );
	    widthName = ( widthW > widthName ) ? widthW : widthName;

	    XtVaGetValues ( mbg->on[i].w, XmNwidth, &widthW, NULL );
	    widthOn = ( widthW > widthOn) ? widthW : widthOn;

	    XtVaGetValues ( mbg->minScale[i].w, XmNwidth, &widthW, NULL );
	    widthMin = ( widthW > widthMin ) ? widthW : widthMin;

	    XtVaGetValues ( mbg->maxScale[i].w, XmNwidth, &widthW, NULL );
	    widthMax = ( widthW > widthMax ) ? widthW : widthMax;

	    XtVaGetValues ( mbg->sampleFrameW[i], XmNwidth, &widthW, NULL );
	    widthFrame = ( widthW > widthFrame ) ? widthW : widthFrame;

/*
 *	    Set the sensitivity of this map entry.
 */
	    ToggleMapListWidgets ( onToggle, i );

	    i++;
        }

	mbg->nmaps = i;


/*
 *	Save the max widget height and size the label widgets. Use one of the 
 *	text widgets for the max height since it will be the tallest.
 */
	if ( mbg->nmaps > 0 ) {

	    XtVaGetValues ( mbg->maxScale[0].w, XmNheight, &maxHeight, NULL );

	    XtVaSetValues ( nameLabel, XmNwidth, widthName,
			    XmNheight, maxHeight, NULL );
			
	    XtVaSetValues ( onLabel, XmNwidth, widthOn,
			    XmNheight, maxHeight, NULL );
			
	    XtVaSetValues ( minScaleLabel, XmNwidth, widthMin,
			    XmNheight, maxHeight, NULL );
	
	    XtVaSetValues ( maxScaleLabel, XmNwidth, widthMax,
			    XmNheight, maxHeight, NULL );
	
	    XtVaSetValues ( sampleLabel, XmNwidth, widthFrame,
			    XmNheight, maxHeight, NULL );
	}	

/*
 *	Make all the attachments and size the widgets consistently. (They
 *	should all the have same height. And within each column, they should
 *	have the same width.) This makes everything align.
 */
	for ( i = 0; i < mbg->nmaps; i++ ){

	    if ( i == 0 ) {
	        topOffset = 10;
	        prevNameW = nameLabel;
	        prevOnW = onLabel;
	        prevMaxW = maxScaleLabel;
	        prevMinW = minScaleLabel;
		prevFrameW = sampleLabel;
	    }
	    else {
	        topOffset = 5;
	        prevNameW = mbg->name[i-1].w;
	        prevOnW = mbg->on[i-1].w;
	        prevMaxW = mbg->maxScale[i-1].w;
	        prevMinW = mbg->minScale[i-1].w;
		prevFrameW = mbg->sampleFrameW[i-1];
	    }

	    XtVaSetValues (mbg->name[i].w,
	    	XmNleftAttachment, XmATTACH_FORM,
	    	XmNleftOffset, 5,
	    	XmNtopAttachment,  XmATTACH_WIDGET,
		XmNtopWidget, prevNameW,
	    	XmNtopOffset, topOffset,
	    	XmNheight, maxHeight, XmNwidth, widthName, NULL );

	    XtVaSetValues (mbg->on[i].w,
	    	XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, mbg->name[i].w,
	    	XmNleftOffset, 5,
	    	XmNtopAttachment,  XmATTACH_WIDGET,
		XmNtopWidget, prevOnW,
	    	XmNtopOffset, topOffset,
	    	XmNheight, maxHeight, XmNwidth, widthOn, NULL );
	    
	    XtVaSetValues (mbg->maxScale[i].w,
	    	XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, mbg->on[i].w,
	    	XmNleftOffset, 5,
	    	XmNtopAttachment,  XmATTACH_WIDGET,
		XmNtopWidget, prevMaxW,
	    	XmNtopOffset, topOffset,
	    	XmNheight, maxHeight, XmNwidth, widthMax, NULL );
	
	    XtVaSetValues (mbg->minScale[i].w,
	    	XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, mbg->maxScale[i].w,
	    	XmNleftOffset, 5,
	    	XmNtopAttachment,  XmATTACH_WIDGET,
		XmNtopWidget, prevMinW,
	    	XmNtopOffset, topOffset,
	    	XmNheight, maxHeight, XmNwidth, widthMin, NULL );
	    
	    XtVaSetValues (mbg->sampleFrameW[i],
	    	XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, mbg->minScale[i].w,
	    	XmNleftOffset, 5,
	    	XmNtopAttachment,  XmATTACH_WIDGET,
		XmNtopWidget, prevFrameW,
	    	XmNtopOffset, topOffset,
	    	XmNrightAttachment, XmATTACH_FORM,
	    	XmNheight, maxHeight, XmNwidth, widthFrame, NULL );

	}

	if ( GetVerboseLevel() > VERBOSE_0 )
		printf (" %d maps read.\n",mbg->nmaps);

	Free ( maplist );
	fclose ( ftbl );

}
