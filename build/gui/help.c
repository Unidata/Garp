/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	help.c
 *
 *	HTML help callbacks and utilities.	
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 1/98	COMET	Changed icon_file to pointer to char in Create-
 *			HelpPopup (isn't used anyway)
 *
 ***********************************************************************/


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <Xm/MainW.h>
#include <Xm/DrawnB.h>

#include "../util/htmlw/HTML.h"		/* from HTML widget library */
#include "../util/htmlw/list.h"		/* from HTML widget library */
#include "genglobs.h"
#include "_proto.h"


#define HTML_LIST_MAX	20

void StringListFree ( int, char ** );
char **StringListAllocate ( int );
char *GetConfigValue ( char * );
char *GetIcon ( char * );
char *builddirpath ( char *, char * );
int AddString (int, int *, char ***, char * );


void HelpCB();
static void ExitHTMLCB();
static void HomeHTMLCB();
static void BackHTMLCB();
static void ForwardHTMLCB();
static void CrossingAnchorCB();
static void HTMLRefCB ( Widget , XtPointer, WbAnchorCallbackData* );
static void PrintHistoryList ( List );


extern unsigned char *ReadGIF(FILE *, int *, int *, XColor *);
ImageInfo *html_ImageResolve (Widget, char *, int );
unsigned char *ReadBitmap(char *, int *, int *, XColor *, int *);
char *ReadHyperTextFile(char *, int * );

static char *home_html="garp_home.html"; /* should use GetConfigValue */
static struct {
	Widget	dialog;
	Widget	htlmWid;
	char	**href;
	int	count;
} help;

static List historyList;


void
CreateHelpPopup ( Widget parent )
{
	Widget		pane, rowcol, b;
	Pixmap		pixmap;
	Pixel		fg, bg;
	Arg		args[15];
	int		n, error, verbose;
	char		*startuptextH;
	char		*file, *dpath, *icon_file;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("CreateHelpPopup\n" );

/*
 *	Define home page file.
 */
	dpath = GetConfigValue ("GarpHTML");
	file = builddirpath ( dpath, home_html );
	if ( file == NULL ) {
	    printf ("*** CreateHelpPopup: HTML homepage is undefined ... exiting\n" );
	    exit ( 1 );
	}
/*
 *	Create widgets for help dialog.
 */
	n = 0;
	XtSetArg( args[n], XmNallowOverlap, False);                  n++;
	XtSetArg( args[n], XmNdialogStyle, XmDIALOG_MODELESS);       n++;
	XtSetArg( args[n], XmNautoUnmanage, False);                  n++;
	XtSetArg( args[n], XmNwidth, 500); n++;
	XtSetArg( args[n], XmNheight, 700); n++;

	help.dialog = XmCreateFormDialog(parent,"help_dialog",args,n);

	rowcol = XtVaCreateManagedWidget ( "rowcol",
			xmRowColumnWidgetClass, help.dialog,
			XmNorientation, XmHORIZONTAL,
			NULL );

	XtVaGetValues ( rowcol,
			XmNforeground, &fg,
			XmNbackground, &bg,
			NULL );
/*
 *	Exit button.
 */
	icon_file = (char *) GetIcon ("exit.xbm");
	pixmap = XmGetPixmap ( XtScreen ( rowcol ), icon_file, fg, bg );
	b = XtVaCreateManagedWidget ( "b",
			xmDrawnButtonWidgetClass, rowcol,
			XmNlabelType, XmPIXMAP,
			XmNlabelPixmap, pixmap,
			NULL );
	XtAddCallback ( b, XmNactivateCallback, ExitHTMLCB, NULL );
	Free ( icon_file );

/*
 *	Home button.
 */
	icon_file = (char *) GetIcon ("icon11.xbm");
	pixmap = XmGetPixmap ( XtScreen ( rowcol ), icon_file, fg, bg );
	b = XtVaCreateManagedWidget ( "b",
			xmDrawnButtonWidgetClass, rowcol,
			XmNlabelType, XmPIXMAP,
			XmNlabelPixmap, pixmap,
			NULL );
	XtAddCallback ( b, XmNactivateCallback, HomeHTMLCB, NULL );
	Free ( icon_file );

/*
 *	Back button.
 */
	icon_file = (char *) GetIcon ("icon09.xbm");
	pixmap = XmGetPixmap ( XtScreen ( rowcol ), icon_file, fg, bg );
	b = XtVaCreateManagedWidget ( "b",
			xmDrawnButtonWidgetClass, rowcol,
			XmNlabelType, XmPIXMAP,
			XmNlabelPixmap, pixmap,
			NULL );
	XtAddCallback ( b, XmNactivateCallback, BackHTMLCB, NULL );
	Free ( icon_file );

/*
 *	Forward button.
 */
	icon_file = (char *) GetIcon ("icon09.xbm");
	pixmap = XmGetPixmap ( XtScreen ( rowcol ), icon_file, fg, bg );
	b = XtVaCreateManagedWidget ( "b",
			xmDrawnButtonWidgetClass, rowcol,
			XmNlabelType, XmPIXMAP,
			XmNlabelPixmap, pixmap,
			NULL );
	XtAddCallback ( b, XmNactivateCallback, ForwardHTMLCB, NULL );
	Free ( icon_file );

/*
 *	Pane widget to contain html widget.
 */
	pane = XtVaCreateWidget("help_pane",
			xmPanedWindowWidgetClass,
			help.dialog,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopWidget, rowcol,
			XmNleftAttachment, XmATTACH_FORM,
			XmNrightAttachment, XmATTACH_FORM,
			XmNbottomAttachment, XmATTACH_FORM,
			NULL);

/*
 *	HTML widget.
 */
	help.htlmWid = XtVaCreateWidget( "htlmWid",
			htmlWidgetClass,
			pane,
			XmNresizePolicy, XmRESIZE_ANY,
			WbNpointerMotionCallback, CrossingAnchorCB,
			WbNresolveImageFunction, *html_ImageResolve,
			XmNshadowThickness, 2,
			WbNverticalScrollOnRight, True,
			XmNwidth, 800,
			XmNheight, 600,
			NULL);

	XtAddCallback ( help.htlmWid, WbNanchorCallback, HTMLRefCB, NULL );

	
	XtManageChild ( help.htlmWid );
	startuptextH = ReadHyperTextFile ( file, &error );
	HTMLSetText ( help.htlmWid, startuptextH, NULL, NULL, 0, 0, NULL );
	XtFree ( startuptextH );

	XtManageChild(pane);

	Free ( dpath );
	Free ( file );
}


static void 
HTMLRefCB ( Widget widget,
	    XtPointer client_data,
	    WbAnchorCallbackData* call_data)
{
/*
 *  Callback for HTML widget.
 */
	char	*buffer;
	char	*file, *dpath;
	int	error, verbose;

/*
 *	Read in hypertext file.
 */
	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("HTMLRefCB\n");

	dpath = GetConfigValue ("GarpHTML");
	file = builddirpath ( dpath, call_data->href );

        buffer = ReadHyperTextFile ( file, &error );
/*
 *	Add hypertext file to href list.
 */
	if ( !error && file )
	    ListAddEntry ( historyList, strdup(file) );

/*
 *	Write hypertext file to HTML widget.
 */
	if ( buffer != NULL ) {
            XtVaSetValues( widget, WbNtext, buffer, NULL );
	    Free ( buffer );
	}

	Free ( dpath );
	Free ( file );
}


void
HelpCB ( Widget parent )
{
	char	*file, *dpath;
	int	verbose;
/*
 *  Callback to popup HTML dialog.
 */
	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("HelpCB\n");

        dpath = GetConfigValue ("GarpHTML");
        file = builddirpath ( dpath, home_html );

/*
 *	Initialize hypertext file list.
 */
	historyList = ListCreate();
	if ( file ) ListAddEntry ( historyList, strdup(file) );

/*
 *	Popup HTML dialog.
 */
	XtManageChild ( help.dialog );
	XtPopup ( XtParent ( help.dialog ), XtGrabNone);

	Free ( dpath );
	Free ( file );
}


static void
ExitHTMLCB ()
{
	int	verbose;
/*
 *  Callback to popdown HTML dialog.
 */
	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("ExitHTMLCB\n");

/*
 *	Free hypertext file list.
 */
	ListDestroy ( historyList );

/*
 *	Popdown HTML dialog.
 */
	XtPopdown ( XtParent ( help.dialog ) );
}


static void
HomeHTMLCB ()
{
/*
 *  Return to homepage callback.
 */
	WbAnchorCallbackData	call_data;
	XtPointer		client_data;
	int			verbose;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("HomeHTMLCB\n");

	call_data.href = strdup ( home_html );
	HTMLRefCB ( help.htlmWid, client_data, &call_data );

	Free ( call_data.href );
}


static void
BackHTMLCB ()
{
/*
 *  Callback to step back to the previous HTML document.
 */
	char	*htext;
	char	*file;
	int	error, verbose;
  
	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("BackHTMLCB\n");

/*
 *	Read in the previous file. Read in the home page if there is none.
 */
	file = ListPrev ( historyList );
printf("Back file = %s\n", file );
	if ( file == NULL )
	    file = ListHead ( historyList );
printf("Back file = %s\n", file );
	htext = ReadHyperTextFile ( file, &error );

	Free ( file );

	HTMLSetText ( help.htlmWid, htext, NULL, NULL, 0, 0, NULL );
	XtFree ( htext );
}


static void
ForwardHTMLCB ()
{
/*
 *  Callback to step back to the previous HTML document.
 */
	char	*htext;
	char	*file;
	int	error, verbose;
  
	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("ForwardHTMLCB\n");

/*
 *	Read in the previous file. Read in last file if at end of list.
 */
	file = ListNext ( historyList );
printf("Next file = %s\n", file );
	if ( file == NULL )
	    file = ListTail ( historyList );
printf("Next file = %s\n", file );
	htext = ReadHyperTextFile ( file, &error );

	Free ( file );

	HTMLSetText ( help.htlmWid, htext, NULL, NULL, 0, 0, NULL );
	XtFree ( htext );
}


static void
PrintHistoryList ( List theList )
{
	int	i, num;

	num = ListCount ( theList );
	printf ("history =  %s\n", ListHead ( theList ) );
	for ( i=0; i<num; i++ ) 
	    printf ("i, history = %d  %s\n", i, ListNext ( theList ) );
}


static void
CrossingAnchorCB()
{
	int	verbose;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("CrossingAnchorCB\n");
}


/*-----------------------------------------------------------------------------
   This routine is the image resolve function any html widget. It is 
   adapted from Mosaic-2.0 (img.c)
   Written by HRL. Modified by COMET.
   
------------------------------------------------------------------------------*/
ImageInfo
*html_ImageResolve ( Widget w, char *src, int noload )
{
	int		i, cnt;
	unsigned char	*data;
	unsigned char	*bg_map;
	unsigned char	*bgptr;
	unsigned char	*ptr;
	int		Used[256];
	XColor		colrs[256];
	ImageInfo	*img_data;
	char		*txt;
	int		widthbyheight = 0;
	char		*fnam;
	int		rc;
	int		bg, bg_red, bg_green, bg_blue;
	char		*file, *dpath;
	int		width,height;
	int		Rdata_colors_per_inlined_image = 50;
	int		verbose;
    
         
	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "html_ImageResolve\n" );

	dpath = GetConfigValue ("GarpHTML");
	if (src[0] == '~')
	    file = builddirpath ( dpath, home_html );
	else
	    file = builddirpath ( dpath, src );
   
	bg = -1;
	bg_map = NULL;
	data = ReadBitmap ( file, &width, &height, colrs, &bg );

	Free ( dpath );
	Free ( file );
   
   /* if we have a transparent background, prepare for it */
   if ((bg >= 0)&&(data != NULL)) {
      bg_red = colrs[bg].red;
      bg_green = colrs[bg].green;
      bg_blue = colrs[bg].blue;
      bg_map = (unsigned char *)malloc(width * height);
   }
      
   if (data == NULL)
      return NULL;
   else {
      img_data = (ImageInfo *) XtMalloc(sizeof(ImageInfo));
      img_data->width = width;
      img_data->height = height;
      img_data->image_data = data;
      img_data->image = NULL;
      img_data->internal = 0;
   }
       
  widthbyheight = img_data->width * img_data->height;

  /* Fill out used array. */
  for (i=0; i < 256; i++)
    {
      Used[i] = 0;
    }
  cnt = 1;
  bgptr = bg_map;
  ptr = img_data->image_data;
  for (i=0; i < widthbyheight; i++)
    {
      if (Used[(int)*ptr] == 0)
        {
          Used[(int)*ptr] = cnt;
          cnt++;
        }
      if (bg >= 0)
        {
		if (*ptr == bg)
		{
			*bgptr = (unsigned char)1;
		}
		else
		{
			*bgptr = (unsigned char)0;
		}
		bgptr++;
        }
      ptr++;
    }
  cnt--;
  
  /*
   * If the image has too many colors, apply a median cut algorithm to
   * reduce the color usage, and then reprocess it.
   * Don't cut colors for direct mapped visuals like TrueColor.
   */
  if ((cnt > Rdata_colors_per_inlined_image)/* SD &&(SWS_Vclass != TrueColor)*/)
    {
       /*printf("doing a Median Cut\n");*/
/* SD
      MedianCut(img_data->image_data, &img_data->width, 
                &img_data->height, colrs, cnt, 
                Rdata_colors_per_inlined_image);
*/
      /*printf("after Median Cut\n");*/
      
      for (i=0; i < 256; i++)
        Used[i] = 0;
      cnt = 1;
      ptr = img_data->image_data;
      for (i=0; i < widthbyheight; i++)
        {
          if (Used[(int)*ptr] == 0)
            {
              Used[(int)*ptr] = cnt;
              cnt++;
            }
          ptr++;
        }
      cnt--;

      /* if we had a transparent bg, MedianCut used it.  Get a new one */
      if (bg >= 0)
      {
	cnt++;
	bg = 256;
      }
   }

  img_data->num_colors = cnt;

  img_data->reds = (int *)malloc(sizeof(int) * cnt);
  img_data->greens = (int *)malloc(sizeof(int) * cnt);
  img_data->blues = (int *)malloc(sizeof(int) * cnt);

  for (i=0; i < 256; i++)
    {
      int indx;
      
      if (Used[i] != 0)
        {
          indx = Used[i] - 1;
          img_data->reds[indx] = colrs[i].red;
          img_data->greens[indx] = colrs[i].green;
          img_data->blues[indx] = colrs[i].blue;
	  /* squeegee in the background color */
	  if ((bg >= 0)&&(i == bg))
            {
              img_data->reds[indx] = bg_red;
              img_data->greens[indx] = bg_green;
              img_data->blues[indx] = bg_blue;
            }
        }
    }

    /* if MedianCut ate our background, add the new one now. */
    if (bg == 256)
      {
	img_data->reds[cnt - 1] = bg_red;
	img_data->greens[cnt - 1] = bg_green;
	img_data->blues[cnt - 1] = bg_blue;
      }
  
  bgptr = bg_map;
  ptr = img_data->image_data;
  for (i=0; i < widthbyheight; i++)
    {
      *ptr = (unsigned char)(Used[(int)*ptr] - 1);
      /* if MedianCut ate the background, enforce it here */
      if (bg == 256)
        {
          if ((int)*bgptr == 1)
            {
              *ptr = (unsigned char)(cnt - 1);
            }
          bgptr++;
        }
      ptr++;
    }

  /* free the background map if we have one */
    Free (bg_map);

  Free (src);
  return img_data;

}

   
/*---------------------------------------------------------------------------
   This routine is adapted from Mosaic-2.0. We only read GIF files, for now.
   Written by HRL. Modified by COMET.
   
----------------------------------------------------------------------------*/
unsigned char *ReadBitmap(char *datafile,int *w,int *h,XColor *colrs,int *bg)
{
	unsigned char	*bit_data;
	FILE		*fp;
	int		verbose;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "ReadBitmap\n" );

	*bg = -1;

        /* Obviously this isn't going to work. */
        fp = fopen(datafile, "r");

	if (fp != NULL)
	{
		bit_data = ReadGIF(fp, w, h, colrs);
		if (bit_data != NULL)
		{
	           if (fp != stdin) fclose(fp);
	           return(bit_data);
		}
		rewind(fp);

	/*	bit_data = ReadXbmBitmap(fp, datafile, w, h, colrs);
		if (bit_data != NULL)
		{
			if (fp != stdin) fclose(fp);
			return(bit_data);
		}
		rewind(fp);

		bit_data = ReadXpm3Pixmap(fp, datafile, w, h, colrs, bg);
		if (bit_data != NULL)
		{
			if (fp != stdin) fclose(fp);
			return(bit_data);
		}*/
	}
	if (fp != stdin) fclose(fp);
	return((unsigned char *)NULL);
}


/*-----------------------------------------------------------------------------
   This routine reads a file specified by "filename".
   NOTE: this routine allocates space! Written by HRL. Modified by COMET.
   
------------------------------------------------------------------------------*/
char *
ReadHyperTextFile ( char *filename, int *error )
{
	FILE		*fp;
	struct stat	statb;
	char		*text;
	char		jumpbackfromerror[512];
	char		*file, *dpath;
	int		verbose;
  
	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ("ReadHyperTextFile\n" );
	if( verbose > VERBOSE_1 )
	    printf ("  file = %s\n", filename );

	if ( filename == NULL ) return NULL;

/*
 *	Define home document to jump to.
 */
	dpath = GetConfigValue ("GarpHTML");
	file = builddirpath ( dpath, home_html );

	strcpy ( jumpbackfromerror,
	    "<html><body><h1>ERROR</h1><h2>reading the requested document!" );
	strcat ( jumpbackfromerror, " Sorry.</h2><h3>Better <a href=\"" );
	strcat ( jumpbackfromerror, file );
	strcat ( jumpbackfromerror, "\"> jump </a> back to the first page" );
	strcat ( jumpbackfromerror, " of Help</h3></body></html>" );
	Free ( file );

	Free ( dpath );
	Free ( file );

/* printf(" error message = %s\n", jumpbackfromerror ); */

/*
 *	Read in hypertext file.
 */
	if (stat (filename, (struct stat *) &statb) == -1 ||
	   (statb.st_mode & S_IFMT) != S_IFREG ||
	   !(fp = fopen(filename,"r"))) {
	    printf ( "*** Can't read file = " );
	    if ( file != NULL ) printf ("%s\n", file );
	    *error = 1;
	    return ( jumpbackfromerror );
	}
	else {   
	    if (!(text = XtMalloc((unsigned)(statb.st_size+1)))) {
	   	 printf("Cannot allocate enough space for %s\n",filename);
	    	*error = 1;
	    	return ( jumpbackfromerror );
	    }
      	    else {      
	 	if (!fread(text,sizeof(char),statb.st_size+1,fp))
	   	printf("Warning may not have read all of %s\n",filename);
         	text[statb.st_size] = 0;
      	    }
   	}

	*error = 0;
	return(text);
}
