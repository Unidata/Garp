/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gifcb.c
 *
 *	Set of routines and data to implement gui and callbacks for
 *	GIF saving code.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 7/97	COMET	Added include file and code for logging.
 *	 9/97	COMET	Put in checks for number of pixmaps before
 *			saving GIF.
 *	10/97	COMET	Removed call to ResetgarpLogo()
 *	 8/02	S. Chiswell/Unidata	modified for 16/24 bit display
 *
 ***********************************************************************/


#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/PanedW.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/ToggleB.h>
#include <Xm/CascadeB.h>
#include <Xm/Form.h>

#include "winobj.h"
#include "genglobs.h"
#include "_proto.h"

#include "xwcmn.h" /* needed for color map access in 16/24 bit */


/*
 *	Public data and functions
 */
int    GetFrameToggleState();
void   GIFSaveOKCB( Widget , XtPointer , XtPointer );
void   GIFSaveCancelCB( Widget , XtPointer ,XtPointer );
void   PopupGIF();
void   PopdownGIF();
/* Widget CreateSaveGIFDialog( Widget ); */
void   CreateSaveGIFDialog( Widget , XtPointer , XtPointer );
void   SetFilenameText(Widget , XtPointer , XtPointer );
char   *GetFilenameText();


/*
 *	Private functions
 */
#define	CURRENT_FRAME	0
#define	ALL_FRAMES	1

static Widget          gifDescriptionLabel;
static Widget          gifOKButton;
static Widget          gifCancelButton;
static Widget          gifTextLabelWidget;
static Widget          gifSeparator;
static Widget          gifDialogWidget;
static Widget          gifPaneWidget;
static Widget          gifFilenameWidget;
static Widget          gifRadioButtonWidget;
static Widget          gifActionButtonWidget;
static Widget          actionForm;
static Widget          mainForm;
static int             FrameRadioButton = 0 ;
static int             previousFilenameCount;
static char            *previousFilenames[512];

static void            DoFormLayout();
static Widget          CreateRadioButtons( Widget );
static void            ToggleCB(Widget , XtPointer , XtPointer );
static Widget          CreateActionButton( Widget , char *);
static Widget          CreateTextInput( Widget parent );
static void            TextReturnCB( Widget , XtPointer , XtPointer );

static Widget CreateRadioButtons( Widget parent )
{

/*
 *	Must match up with #define for frame numbers.
 */
        char     *labels[] = {
                "Current Frame",
                "All Frames"
        };

        int     i;
        Widget  radioRC;
        Widget   buttons[ XtNumber( labels ) ];

        radioRC = XtVaCreateManagedWidget("gifrowcolumn",
                                  xmRowColumnWidgetClass, parent,
                                  XmNnumColumns,          1,
                                  XmNorientation,         XmHORIZONTAL,
                                  XmNradioBehavior,       True,
                                  XmNspacing,             4,
                                  NULL );



	buttons[CURRENT_FRAME] = 
			XtVaCreateManagedWidget(labels[CURRENT_FRAME],
				xmToggleButtonWidgetClass, radioRC,
				NULL );

	XtAddCallback(buttons[CURRENT_FRAME], XmNarmCallback,
                        (XtCallbackProc) ToggleCB, 
			(XtPointer) CURRENT_FRAME);


	buttons[ALL_FRAMES] = 
			XtVaCreateManagedWidget(labels[ALL_FRAMES],
				xmToggleButtonWidgetClass, radioRC,
				NULL );

	XtAddCallback(buttons[ALL_FRAMES], XmNarmCallback,
                        (XtCallbackProc) ToggleCB, 
			(XtPointer) ALL_FRAMES);

        XmToggleButtonSetState(buttons[CURRENT_FRAME], True, True);

        return( radioRC );
}


void SetFilenameText(Widget w, XtPointer clientData, XtPointer callData)
{

        XmTextSetString( gifFilenameWidget, (char *) clientData );

}

char *GetFilenameText()
{
        return( XmTextGetString( gifFilenameWidget ) );
}

static void ToggleCB(Widget w, XtPointer clientData, XtPointer callData)
{

        FrameRadioButton = (int) clientData;

}


int GetFrameToggleState()
{
        return( FrameRadioButton );
}

static Widget CreateTextInput( Widget parent )

{
        Widget          textWidget;

        textWidget = XtVaCreateManagedWidget("giftextfield",
                                  xmTextWidgetClass, parent,
                                  XmNeditable,   True,
                                  XmNorientation,         XmHORIZONTAL,
                                  XmNpacking,             XmPACK_TIGHT,
                                  XmNcolumns,             2,
                                  NULL );

        return( textWidget );
}


static Widget CreateActionButton( Widget parent, char *buttonName )
{

        int             i;
        Arg             args[4];
        Cardinal        argcnt;
        Widget          pushbutton;


        argcnt = 0;
        XtSetArg(args[argcnt], XmNshadowThickness,3);  argcnt++;

        pushbutton = XtCreateManagedWidget( buttonName,
                                          xmPushButtonWidgetClass,
                                          parent, args, argcnt);

        return( pushbutton );
}


/*
 *  Creates a single GIF of the specified frame
 */

void
WriteOneGIF (  WindowObjectType	*wo, char *filname, int index )
{	

#define mxcol 254

	XColor			*colors;
	XImage			*image ;
	Colormap		cmap;
	char			*tp1, *tp2 ;
	FILE			*fp;
	int			i, rbytes, _mxcol;
	unsigned char		xred[mxcol], xgreen[mxcol], xblue[mxcol];
	unsigned char		*imgdatptr;


/*
 *	Get the image data after checking that this pixmap is valid
 */	
	if ( wo->pixmapObjects[index]->inUse == 0 ) return;
	cmap = DefaultColormap( wo->display, DefaultScreen(wo->display));
	image = XGetImage( wo->display, wo->pixmapObjects[index]->pixmap,
	    		0, 0, wo->width, wo->height, AllPlanes, ZPixmap );

/* 
 *	Allocate the XColor array, query colors.
 */
	if( (colors = (XColor *)calloc (mxcol, sizeof(XColor))) == NULL ) {
	    fprintf( stderr, "No Memory For Setting Up Colormap\n" );
	    return;
	}


	if(image->depth <= 8) {
/*
 *	   Remove any padding bytes at the end of each line
 */
	   if ( image->bytes_per_line > image->width ) {
 	       tp1 = tp2 = image->data;
 	       rbytes = image->bytes_per_line - image->width;
 	       for ( i = 0; i < image->height; i++ ) {
 		   memcpy ( tp1, tp2, image->width);
 		   tp1 += image->width;
 		   tp2 += image->width + rbytes;
 	       }
 	   }
 		    
	   
	   for ( i = 0; i < mxcol; i++ ) {
	       colors[i].pixel = i;
	       colors[i].flags = DoRed | DoGreen | DoBlue;
	   }
	   imgdatptr = (unsigned char *)image->data;
	   _mxcol = mxcol;
        }
	else {
	    /*
             * for 16/24 bit data, create an indexed pixmap
             */
            int nBpp, istart=0, negflg = 0, ii, jj, inc=0, ifc;
            int nbanks,ir,il, ier;
            unsigned char uctmp;
	    int ivalue;

            nbanks = ColorBanks.nbank;

            nBpp = image->bits_per_pixel / 8;

            for(ii = 0; ii < nbanks; ii++) {

	       if ( ( ii == 0 ) && ( !GColorIsInitialized ) )  continue;

               if ( ( ii > 0 ) && ( !allocflag[ii]) )  continue;

               for(jj = 0; jj<ColorBanks.banks[ii]; jj++) {
                  colors[inc].pixel = ColorBanks.colrs[ii][jj];
                  colors[inc].flags = DoRed | DoGreen | DoBlue;
                  inc++;
               }
            }

	    if(inc > mxcol) {
	       fprintf( stderr, "Warning: Number of colors %d is greater than allowed\n", inc );
	       _mxcol = mxcol;
	    }
	    else
               _mxcol = inc;

            imgdatptr = (unsigned char *)malloc(image->width * image->height);

            for(ii = 0; ii < (image->width * image->height); ii++) {
               if(image->bitmap_bit_order == LSBFirst) {
                  /* need to flip bytes */
                  il = 0; ir = nBpp - 1;
                  while (il < ir) {
                     uctmp = image->data[istart + il];
                     image->data[istart + il] = image->data[istart + ir];
                     image->data[istart + ir] = uctmp;
                     il++;
                     ir--;
                   }
                }

                mv_btoi ( (unsigned char *)image->data, &istart, &nBpp, &negflg,
                        &ivalue, &ier);

		if((ivalue < 0)&&(ier != 0))
			fprintf(stderr,"oops %d %d\n",negflg,ivalue,ier);
                istart += nBpp;


                /* find value */
                jj = 0; ifc = -1;
                do {
                   if((unsigned long)ivalue == colors[jj].pixel)
                      ifc = jj;
                   else
                      jj++;
                } while ( ( jj < _mxcol )&&(ifc == -1) );

                if (ifc == -1) {
		   if (inc < (mxcol-1) )  { /* this shouldn't happen */
	              fprintf( stderr, "Trying to allocate color %d %u\n", ii, (unsigned long)ivalue );
                      colors[inc].pixel = (unsigned long)ivalue;
                      colors[inc].flags = DoRed | DoGreen | DoBlue;
		      ifc = inc;
		      inc++; _mxcol++;
		   }
		   else {
	               fprintf( stderr, "Warning: could not map color %d %u %d %d\n", ii, (unsigned long)ivalue,
			        ifc, inc );
		       ifc = 0;
		   }
		}

                imgdatptr[ii] = (unsigned char)ifc;
             }
	}

	XQueryColors( wo->display, cmap, colors, _mxcol );

/*
 *	Save color values and write th GIF file
 */
	for( i = 0; i < _mxcol; i++ ) {
	    xred[i]   = colors[i].red >> 8;
	    xblue[i]  = colors[i].blue >> 8;
	    xgreen[i] = colors[i].green >> 8;
	}
/*
 *	Open the GIF file, write the data
 */
	if ( ( fp = fopen (filname,"wb")) == NULL) {
	    fprintf (stderr, "Cannot open file %s for writing.\n", filname);
	    return;
	}
	else 
	    WriteGIF( fp, imgdatptr, wo->width, wo->height, xred, xgreen,
		      xblue, _mxcol, 3 );
/*
 *	Free the XImage, XColors and close the file
 */		          
	XDestroyImage ( image );
	if(image->depth > 8) free(imgdatptr);
	free (colors);
	fclose (fp);
}


/*
 *  Creates one GIF file of the current frame, or a series of
 *  GIFs for a series of frames.
 */

void
MakeGIFs ( WindowObjectType *wo, char *filtmp, char *frame )
{

	char		giffil[100], fnum[10];
	int		index;
	

/*
 *	If the one-frame toggle is set, just GIF the current frame. 
 */
	if ( strcmp ( frame, "CURRENT" ) == 0) {

	    if ( wo->numPixmapObjects == 0 ) {
		printf("There are no frames to make GIFs from.\n");
		return;
	    }

	    strcpy ( giffil, filtmp );
	    index = GetActivePixmapObjectIndex (wo);
	    WriteOneGIF ( wo, giffil, index );
	}

/*
 *	Otherwise, loop through all the pixmaps
 */
	else {

	    if ( wo->numPixmapObjects == 0 ) {
		printf("There are no frames to make GIFs from.\n");
		return;
	    }

	    for ( index = 0; index < wo->numPixmapObjects; index++ ) {

		if ( wo->numPixmapObjects < 100 )
		    sprintf ( fnum, "%02d.gif", index+1);
		else
		    sprintf ( fnum, "%03d.gif", index+1);
		    
		strcpy ( giffil, filtmp );
		strcat ( giffil, fnum);    
		WriteOneGIF ( wo, giffil, index );
	    }
	}

}	

void GIFSaveCancelCB( Widget w,XtPointer clientData,XtPointer callData )
{

	PopdownGIF();

}

void GIFSaveOKCB( Widget w, XtPointer clientData, XtPointer callData )
{

        char    		*p, *file;
	WindowObjectType	*wo;
	BooleanType             forceCancel = False;
	char			string[]="Saving GIF file(s)";
		
	PopdownGIF();

	p = GetFrameToggleState() == CURRENT_FRAME ? "CURRENT" : "ALL" ;
	file = GetFilenameText();

	StringClean ( file );
	
	if ( file != NULL && file [0] != '\0' ) {

            printf("GIF Filename is %s", file );
	    printf(" and frame toggle is %s.\n", p ); 
	    wo = GetActiveWindowObject ();
	    SetGarpLogo (wo, string);
	    SetupAbortProcess( wo );
	    MakeGIFs ( wo, file, p );
	    CancelAbortProcess( wo, forceCancel );
	}
	else
	    printf("Error: GIF file name not specified.\n");

	XtFree ( file );
}

void PopupGIF()
{

        if ( ! XtIsManaged( gifPaneWidget ) )
		XtManageChild( gifPaneWidget );

	XtPopup( gifDialogWidget, XtGrabNone );

}




void PopdownGIF()
{

	XtUnmanageChild( gifPaneWidget );
	/* XtPopdown( gifDialogWidget ); */


}



#ifdef TEST
main(int argc, char **argv )
{

        XtAppContext    app;
        Widget          toplevel;
	Widget		w;
	Widget		b;


	
        int             i;
        Arg             args[4];
        Cardinal        argcnt;

        toplevel = XtVaAppInitialize( &app, "D", NULL, 0, &argc, argv, NULL,
                        NULL );

        argcnt = 0;
        XtSetArg(args[argcnt], XmNshadowThickness,3);  argcnt++;

        b = XtCreateManagedWidget( "newbutton",
                                          xmPushButtonWidgetClass,
                                          toplevel, args, argcnt);

        XtAddCallback( b, XmNactivateCallback, PopupGIF,
                        NULL );

	
        CreateSaveGIFDialog( toplevel );


        XtRealizeWidget( toplevel );

        XtAppMainLoop( app );

}
#endif


/* Widget CreateSaveGIFDialog( Widget parent ) */
void
CreateSaveGIFDialog( Widget parent, XtPointer client_data, XtPointer call_data )
{
        Arg                     args[5];
        Cardinal                argcnt;
        Widget			shellWidget;
        int                     i;
        Dimension               h;
	static Boolean		exist;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

/*
 *	If dialog has been created, manage it.
 */
	if ( exist ) {
	    PopupGIF();
	    return;
	}

	exist = TRUE;
/*
 *	Create the dialog shell for
 */
        gifDialogWidget = XtVaCreateWidget("gifdialog", 
					xmDialogShellWidgetClass, parent,
					NULL );


        gifPaneWidget = XtVaCreateWidget("gifpanewidget",
                                xmPanedWindowWidgetClass,
                                gifDialogWidget,
                                XmNsashWidth, 1,
                                XmNsashHeight, 1,
                                NULL);

        mainForm = XtVaCreateWidget( "mainform",
                                        xmFormWidgetClass, gifPaneWidget,
                                        NULL, 0 );


        gifRadioButtonWidget = CreateRadioButtons( mainForm );

        gifDescriptionLabel =
                XtVaCreateManagedWidget( "description", xmLabelWidgetClass,
                                mainForm,
                                XmNalignment, XmALIGNMENT_CENTER,
                                NULL );


        gifTextLabelWidget =
                XtVaCreateManagedWidget( "Filename", xmLabelWidgetClass,
                                mainForm, NULL );

        gifFilenameWidget = CreateTextInput( mainForm );
        XtAddCallback( gifFilenameWidget, XmNactivateCallback, TextReturnCB,
                        NULL );
        actionForm = XtVaCreateWidget( "actionform",
                                        xmFormWidgetClass, gifPaneWidget,
                                        XmNfractionBase, 5,
                                        NULL, 0 );

        gifOKButton = CreateActionButton( actionForm, "Apply" );
        XtAddCallback(gifOKButton, XmNactivateCallback,
                        (XtCallbackProc) GIFSaveOKCB,
                        (XtPointer)NULL);

        gifCancelButton = CreateActionButton( actionForm, "Close" );
        XtAddCallback(gifCancelButton, XmNactivateCallback,
                        (XtCallbackProc) GIFSaveCancelCB,
                        (XtPointer)i);

        XtManageChild( mainForm );
        XtManageChild( actionForm );
        XtVaGetValues( gifCancelButton, XmNheight, &h, NULL );
        XtVaSetValues( actionForm, XmNpaneMaximum, h, XmNpaneMinimum, h, NULL);

        DoFormLayout();

/*
 *	Return the panedwindow widget instead of the shell so that
 *	xtmanage/xtunmanage work correctly.
 */
        /* return( gifPaneWidget ); */
	PopupGIF();
	return;


}


static void DoFormLayout()
{

        XtVaSetValues( gifRadioButtonWidget,
                        XmNtopAttachment,   XmATTACH_FORM,
                        XmNleftAttachment,  XmATTACH_FORM,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL );

        XtVaSetValues( gifDescriptionLabel,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNtopWidget, gifRadioButtonWidget,
                        XmNleftAttachment,  XmATTACH_FORM,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL );

        XtVaSetValues( gifTextLabelWidget,
                        XmNtopOffset, 5,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNtopWidget, gifDescriptionLabel,
                        XmNleftAttachment,  XmATTACH_OPPOSITE_WIDGET,
                        XmNleftWidget,  gifDescriptionLabel,
                        NULL );


        XtVaSetValues( gifFilenameWidget,
                        XmNtopAttachment, XmATTACH_WIDGET,
                        XmNtopWidget, gifDescriptionLabel,
                        XmNleftAttachment,  XmATTACH_WIDGET,
                        XmNleftWidget,  gifTextLabelWidget,
                        XmNrightAttachment, XmATTACH_FORM,
                        NULL );

        XtVaSetValues( gifOKButton,
			XmNtopAttachment, XmATTACH_FORM,
			XmNbottomAttachment, XmATTACH_FORM,
			XmNleftAttachment, XmATTACH_POSITION,
			XmNleftPosition, 1,
			XmNrightAttachment, XmATTACH_POSITION,
			XmNrightPosition, 2,
			NULL );

        XtVaSetValues( gifCancelButton,
			XmNtopAttachment, XmATTACH_FORM,
			XmNbottomAttachment, XmATTACH_FORM,
			XmNleftAttachment, XmATTACH_POSITION,
			XmNleftPosition, 3,
			XmNrightAttachment, XmATTACH_POSITION,
			XmNrightPosition, 4,
			NULL );

}


static void TextReturnCB( Widget w, XtPointer clientData, XtPointer callData )
{

        XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *) callData;
        Widget defaultButton;

#ifdef broken
        XtVaGetValues( actionForm, XmNdefaultButton, &defaultButton, NULL );
        if( defaultButton )
#endif
                XtCallActionProc( gifOKButton, "ArmAndActivate",
                                        cbs->event, NULL, 0 );

}



