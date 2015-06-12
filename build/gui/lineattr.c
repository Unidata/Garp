/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	lineattr.c
 *
 *	Line attributes dialog routines.	
 *
 *	History:
 *
 *	10/97	COMET	Original copy
 *	 1/98	COMET	Added cast to NULL in call to XCreateGC
 *
 ***********************************************************************/


#include "lineattr.h"
#include "genglobs.h"

#define DRAW_MARGIN	10	/* margin in button 		*/
#define LSCALE		2	/* Line scaling term		*/


static int linetypes[10][8] =		/* Line types	*/
	{ 1, 0, 0, 0, 0, 0, 0, 0,
	  2, 3, 0, 0, 0, 0, 0, 0,
	  4, 4, 0, 0, 0, 0, 0, 0,
	  6, 3, 2, 3, 0, 0, 0, 0,
	  5, 2, 0, 0, 0, 0, 0, 0,
	  8, 3, 2, 3, 2, 3, 2, 3,
	  8, -4, 0, 0, 0, 0, 0, 0,
	  6, -3, -3, -3, 0, 0, 0, 0,
	  4, -4, 0, 0, 0, 0, 0, 0,
	 -3, -3, 0, 0, 0, 0, 0, 0 };



static GC gcs = NULL ;		/* Sample line graphics context */
static GC gct = NULL ;		/* Line type graphics context */
static GC gcw = NULL ;		/* Line width graphics context */




/*
 *	Redraws the sample line for the specified widget. Used by many
 *	routines that need up refresh a sample line widget.
 */
void
DrawSampleLine (Widget w, int color, int type, int wid )
{
	int				temp, p_sign;
	int				i, x1, x2, y;
	Dimension			width, height;
	int				bgcolor;
	XGCValues			values;
	Display				*dp_id;
	Drawable			draw_id;
	Widget				parent;
	Boolean				isSensitive;


	XtVaGetValues ( w, XmNwidth, &width,
			XmNheight, &height,
			XmNsensitive, &isSensitive, 
			NULL );

	XtVaGetValues ( XtParent (w), XmNbackground, &bgcolor, NULL );

	dp_id   = XtDisplay(w);
	draw_id = XtWindow(w);

	values.foreground = GemColorToPixel (color);
	values.line_width = wid;

	if ( gcs == NULL )  
	    gcs = XCreateGC( dp_id, draw_id,
			GCForeground|GCLineWidth,
			&values);
	else
	    XChangeGC(  dp_id, gcs,
	    		GCForeground|GCLineWidth,
	    		&values);

	if ( ! isSensitive ) {

	    XSetWindowBackground(dp_id, draw_id, bgcolor );
	    XClearWindow(dp_id, draw_id);
	    return;
	}

	XSetWindowBackground(dp_id, draw_id, GetGemBackgroundPixel());
	XClearWindow(dp_id, draw_id);

	x1 = x2 = DRAW_MARGIN;
	y = height/2;

	while( x1 < (width - DRAW_MARGIN) ) {
 
	    for ( i = 0; i < 8; i++ ) {

		temp = linetypes[type-1][i];

		if (temp == 0)
			break;
			
		p_sign = temp/abs(temp);

		switch (p_sign) {
		case 1:
			x2 = x1 + LSCALE*temp;
		
			if ( x2 > (width-DRAW_MARGIN) )
			    x2 = width-DRAW_MARGIN;

			if (i%2 == 0)
			    XDrawLine(dp_id, draw_id, gcs, x1, y, x2, y);
			break;
		case -1:
			x2 = x1 + LSCALE;
			
                        if ( x2 > (width-DRAW_MARGIN) )
			    return;
			if (i%2 == 0)
			    XDrawLine(dp_id, draw_id, gcs, x1, y, x2, y);
			break;
		}         
		x1 = x2;
	    }
	}
	
}

/*
 *	Redraws the line attributes sample line widget
 */
void
RefreshLineAttributesSampleCB (	Widget w,
				XtPointer client_data,
				XtPointer xt_call_data )
{
	int				width, type, color;
	Widget				sample;
	GuiLineAttributesDialogType	*atr;

	atr = GetGuiLineAttributesDialog();

	width = GetLineAttributesWidth(atr);
	type  = GetLineAttributesType(atr);
	color = GetLineAttributesColor(atr);
	sample = GetLineAttributesSampleW (atr);

	if (GetVerboseLevel() > VERBOSE_0 ) {
		printf("RefreshSampleLine() width: %d",width);
		printf(" type: %d",type);
		printf(" color: %d\n",color);
	}

	DrawSampleLine ( sample, color, type, width ) ;

}

/*
 *	Callback for when the user clicks on a new line width
 */
void
SetLineAttributesWidthCB ( Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	GuiLineAttributesDialogType	*atr;
	Widget				sample;
	int				width;
	int				color;
	int				type;
	
	width = (int) client_data;

	atr = GetGuiLineAttributesDialog();
	
	SetLineAttributesWidth(atr, width);

	type  = GetLineAttributesType(atr);
	color = GetLineAttributesColor(atr);
	sample = GetLineAttributesSampleW (atr);

	DrawSampleLine ( sample, color, type, width ) ;

}


/*
 *	Callback for when the user clicks on a new line type
 */
void
SetLineAttributesTypeCB ( Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	GuiLineAttributesDialogType	*atr;
	Widget				sample;
	int				width;
	int				color;
	int				type;
	
	type = (int) client_data;

	atr = GetGuiLineAttributesDialog();
	
	SetLineAttributesType(atr, type);

	width  = GetLineAttributesWidth(atr);
	color = GetLineAttributesColor(atr);
	sample = GetLineAttributesSampleW (atr);

	DrawSampleLine ( sample, color, type, width ) ;

}


/*
 *	Callback for when the user clicks on a new line color
 */
void
SetLineAttributesColorCB ( Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	GuiLineAttributesDialogType	*atr;
	int				color;
	Widget				sample;
	int				width;
	int				type;
	int				cur_color;
	Widget				label, frame;
	
	color = (int) client_data;

	atr = GetGuiLineAttributesDialog();

/*
 *	Make it appear that the button has been depressed (after un-
 *	depressing the current button).
 */
	cur_color = GetLineAttributesColor(atr);
	label = GetLineAttributesColorW(atr,cur_color-1);
	frame = XtParent (label);

	XtVaSetValues( frame, XmNshadowType, XmSHADOW_OUT, NULL);
	
	SetLineAttributesColor(atr, color);

	label = GetLineAttributesColorW(atr,color-1);
	frame = XtParent (label);
	XtVaSetValues( frame, XmNshadowType, XmSHADOW_IN, NULL);

	width = GetLineAttributesWidth(atr);
	type  = GetLineAttributesType(atr);
	sample = GetLineAttributesSampleW (atr);

	DrawSampleLine ( sample, color, type, width ) ;

}


/*
 *	Initializes the line attributes dialog. Just does colors, since
 *	the other widgets are redrawn with expose callbacks.
 */
void
InitLineAttributesPrefDialog ()
{
	Widget				w;
	int				i;
	int				*clrs;
	int				nclrs;
	GuiLineAttributesDialogType	*atr;


	atr = GetGuiLineAttributesDialog();

        GetGemGraphicsColorBank ( &nclrs,&clrs );
	
	for ( i = 1; i < 33; i++ ) {
	
	    w = GetLineAttributesColorW(atr,i-1);

	    XtVaSetValues ( w,
		XmNbackground,       clrs[i],
		XmNtopShadowColor,   clrs[i],
		XmNbottomShadowColor,clrs[i],
                NULL);

	}

/*
 *	Set the default-defaults
 */
 	SetLineAttributesWidth(atr,1);
 	SetLineAttributesType (atr,1);
 	SetLineAttributesColor(atr,1);
	SetLineAttributesCallback(atr,NULL);
	SetLineAttributesCurrentSampleW(atr,NULL);

}

/*
 *	Callback to draw a line width pattern. Called upon expose event.
 */
 
void
DrawLineWidthPattern ( Widget w, XtPointer client_data, XtPointer xt_call_data )
{


	Display		*dp_id;
	Drawable	draw_id;
	XGCValues	values;
	Dimension	width, height;
	int		which;


	which = (int) client_data;

	XtVaGetValues ( w, XmNwidth, &width, XmNheight, &height, NULL );

	dp_id = XtDisplay(w);
	draw_id = XtWindow(w);

	values.line_width = which;

	if ( gcw == NULL )
	    gcw = XCreateGC(dp_id, draw_id, GCLineWidth, &values);

	else
	    XChangeGC(dp_id, gcw, GCLineWidth, &values);


        XDrawLine( dp_id, draw_id, gcw, DRAW_MARGIN, height/2,
        		width - DRAW_MARGIN, height/2);
 
}



/*
 *	Callback to draw a line type pattern. Called upon expose event.
 */
 
void
DrawLineTypePattern ( Widget w, XtPointer client_data, XtPointer xt_call_data )
{

	int		x1, x2, y, i, temp;
	int		p_sign, scale, which;
	Dimension	width, height;
	Display		*dp_id;
	Drawable	draw_id;
	XGCValues	values;


	which = (int) client_data;

	XtVaGetValues ( w, XmNwidth, &width, XmNheight, &height, NULL );

	x1 = x2 = DRAW_MARGIN;
	y = height/2;

	dp_id = XtDisplay(w);
	draw_id = XtWindow(w);

	if ( gct == NULL ) {
	    values.line_width = 3;
	    gct = XCreateGC(dp_id, draw_id, (unsigned long)NULL, &values);
	}

	XClearArea(dp_id, draw_id, DRAW_MARGIN/2, DRAW_MARGIN/2,
			width - DRAW_MARGIN, DRAW_MARGIN, False);

	while( x1 < width - DRAW_MARGIN) {

	    for ( i = 0; i < 8; i++) {

		temp = linetypes[which-1][i];


		if (temp == 0)
		    break;

		p_sign = temp/abs(temp);

		switch (p_sign) {

		case 1:

		    x2 = x1 + LSCALE*temp;

		    if ( x2 > (width - DRAW_MARGIN) )
		         x2 = width - DRAW_MARGIN;

		    if (i%2 == 0)
			XDrawLine(dp_id, draw_id, gct, x1, y, x2, y);
		    break;

		case -1:

		    x2 = x1 + LSCALE;

		    if ( x2 >  (width - DRAW_MARGIN) )
                        return;

		    if (i%2 == 0)
			XDrawLine(dp_id, draw_id, gct, x1, y, x2, y );
		    break;
	        }
	        x1 = x2;

            }
        }

}

