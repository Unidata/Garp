/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	imagecb.c
 *
 *	Image data callbacks.
 *
 *	History:
 *
 *	 1/97	COMET	Original copy
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory
 *			sanity.
 *	 8/97	COMET	Major revamping to use the GuiImgSelectDialogType
 *			struct for getting/setting GUI info. Also broke out
 *			directory reading code into separate routine called
 *			GetImageFileList().
 *	11/97	COMET	Fixed leak in ImageDisplayCB()
 *	12/97	COMET	Added code to refresh the time scrolled list after
 *			displaying data. Changed args to SetScrolledList()
 *	12/97	COMET	Added a "select_list" object to track date/time
 *			selections when time matching is turned off.
 *	12/99	COMET	Added function call Y2KList to utilize year in YYYY.
 *
 ***********************************************************************/

#include "guimacros.h"
#include "gendef.h"
#include "genglobs.h"
#include "winobj.h"
#include "utils.h"
#include "menucb.h"
#include "imgobj.h"
#include "timeutil.h"
#include "_proto.h"

ListParentObjectType *
ReturnListStruct ( Widget, ListParentObjectType,
	           ListIndexObjectType, ListIndexObjectType * );
int * MatchIndicesToList ( char **, int, int, int * );

/*
 *	This callback is called when the image icon is pushed.
 */

void
ImageIconCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	WindowObjectType	*wo;
	ButtonObject		*bo;
	GuiImgSelectDialogType	*img;
	static Boolean		first = True;

        XmPushButtonCallbackStruct *cbs = 
	            (XmPushButtonCallbackStruct *) xt_call_data;

	wo = GetActiveWindowObject();
	img = GetGuiImgSelectDialog();

	if ( GetVerboseLevel() > VERBOSE_0 )
	    printf("ImageIconCB\n");

/*
 *	Initiate busy cursor.
 */
	SetupAbortProcess( wo );

/*
 *	Build image source panel first time through
 */
	if ( first )
	{
		BuildImageSource ( GetImgSourcePanelObj(img) );
		RemoveProductsFromList ( GetImgTimeListW(img) );

	}

/*
 *	Update the scrolled file list if it has existed before
 */
	bo = GetImgProdPanelSetbutton(img);
	if ( bo ) UpdateScrolledList( bo );

/*
 *	Cancel busy cursor.
 */
	CancelAbortProcess( wo, False );
	
	first = False;
}



void
BuildImageSource ( ButtonParentObjectType *bpo )
{
	char			*ptr, *tmpargv[1];
	char			**pathComponents;
	DefaultDirectory	*p;
	int			i, cnt;

	if ( GetVerboseLevel() > VERBOSE_0 )
	    printf("BuildImageSource\n");


	for ( p = ImageSourcePaths; p->imageSource ;  p++ ) {

	    p->path = (char *) GetConfigValue( p->imageSource );


	    if ( GetVerboseLevel() > VERBOSE_1 ) {
		printf("  imageSource is %s,", p->imageSource );
		printf(" path is %s\n", p->path ? p->path : "" );
	    }

	    if( ! p->path )
		continue;

	    pathComponents = SplitByDelimeter ( p->path, " ", &cnt );

	    for( i = 0; i < cnt; i++ ) {
		char *ptr;

/*
 *		Skip it, if it isn't a directory.
 */
		if( ! DirExists( pathComponents[i] ) ) {
		    if( GetVerboseLevel() > 1 ) {
			printf("  Can't find directory %s\n",
					pathComponents[i]);
		    }
		    continue;
		}

/*
 *		Skip it, if it doesn't look like a full path.
 */
		if( ! ( ptr = strrchr( pathComponents[i], '/' ) ) )
			continue;

/*
 *		Munge the string around so that the path name
 *		is broken up into a directory path and 
 *		file name.   The tricky part here, is that
 *		memory is deallocated by the RemoveButton
 *		function for the path and label portions of
 *		it's data structure, but not the path component.
 */
		*ptr = '\0';
		tmpargv[0] = strdup( ++ptr );

/*
 *
 *		Put the components  into the GUI.
 */
		UpdateButtonPanel( bpo, SourcePanelCB,
				   p->metObjectType,
				   pathComponents[i], 
				   1, tmpargv );
	    }

	    StringListFree ( cnt, pathComponents);
	}
}


void
SourcePanelCB ( Widget w, XtPointer clientData, XtPointer callData )
{
	GuiImgSelectDialogType	*img;
	char			*str;
	
	ButtonObject *bo =  (ButtonObject *) clientData;
	XmToggleButtonCallbackStruct *cbs =
                         ( XmToggleButtonCallbackStruct * ) callData;


	if ( GetVerboseLevel() > VERBOSE_0 )
		printf("imagepanelcb\n");

	if ( ! cbs->set ) return;

/*
 *	Build a new scale panel.
 */
	img = GetGuiImgSelectDialog();	

        RemoveButtonsFromPanel ( GetImgScalePanelObj(img) );
        str = builddirpath ( bo->string, bo->label );
        AddButtonsToPanel (GetImgScalePanelObj(img), ScalePanelCB, bo->i, str);
	SetImgSourcePanelSetbutton(img, bo);

/*
 *	Clear out the product panel and file list panel as these are
 *	no longer valid
 */
        RemoveButtonsFromPanel ( GetImgProdPanelObj(img) );
        RemoveProductsFromList ( GetImgTimeListW(img) );

        Free ( str );

}


void
ScalePanelCB ( Widget w, XtPointer clientData, XtPointer callData )
{
	GuiImgSelectDialogType	*img;
	char 			*str;

	ButtonObject *bo =  (ButtonObject *) clientData;
	XmToggleButtonCallbackStruct *cbs =
                         ( XmToggleButtonCallbackStruct * ) callData;

	if ( GetVerboseLevel() > VERBOSE_0 )
        	printf("scalepanelcb\n");

	if ( ! cbs->set ) return;

/*
 *	Build a new product panel.
 */
 	img = GetGuiImgSelectDialog();

        RemoveButtonsFromPanel (GetImgProdPanelObj(img));
        str = builddirpath ( bo->string, bo->label );
        AddButtonsToPanel (GetImgProdPanelObj(img), ProductPanelCB, 
				bo->i, str);
        SetImgScalePanelSetbutton(img, bo);

/*
 *	Clear out the file list panel.
 */
        RemoveProductsFromList ( GetImgTimeListW(img) );

        Free ( str );

}


void
ProductPanelCB ( Widget w, XtPointer clientData, XtPointer callData )
{
	GuiImgSelectDialogType		*img;

	ButtonObject			*bo =  (ButtonObject *) clientData;
	XmToggleButtonCallbackStruct	*cbs =
				( XmToggleButtonCallbackStruct * ) callData;

	if ( GetVerboseLevel() > VERBOSE_0 )
	    printf("productpanelcb\n");

	if ( ! cbs->set ) return ;

 	img = GetGuiImgSelectDialog();
	SetImgProdPanelSetbutton(img, bo);

/*
 *	Build a new list of files
 */
	UpdateScrolledList( bo );

}




/*
 *	This callback is called when the user presses display.
 */

void
ImageDisplayCB ( Widget w, XtPointer clientData, XtPointer callData )
{

	GuiImgSelectDialogType	*img;
	ListParentObjectType	*inList;
	MetObjectType		**mlist;
	WindowObjectType 	*wo;
	GeoRefObjectType	*geo;
	DataTimeObjectType      *dto;
	int			viewType;
	int  			metObjectType;
	int			count,matchSize ;
	int			*indexList;


	XmPushButtonCallbackStruct *cbs =
                         ( XmPushButtonCallbackStruct * ) callData;


	if ( GetVerboseLevel() > VERBOSE_0 )
		printf("actioncb\n");

	wo = GetActiveWindowObject();
 	img = GetGuiImgSelectDialog();

/*
 *	If not enough of the dialog has not been selected, just return.
 */
        if ( ! GetImgSourcePanelSetbutton(img) || 
	     ! GetImgScalePanelSetbutton(img) || 
             ! GetImgProdPanelSetbutton(img)   ||
               img->time_list.listcount == 0 )
            	return;

        inList = ReturnListStruct ( GetImgTimeListW( img ),
                                    img->time_list,
                                    img->match_list,
                                    &(img->select_list) );
        count = inList->listcount;

/*
 *	Check on the view types for compatibility. Compare this object 
 *	with the current georeference view state and do a reset if
 *	they don't match.
 */
        metObjectType = GetImgProdPanelSetbuttonType(img);
	viewType  = GetMetObjectView( metObjectType );

	if ( geo = GetGeoRefObject( wo) )
		if ( geo->viewType != viewType ) WindowObjectFullReset( wo );

/*
 *	Save the first time in the list for selection purposes.
 */
	if ( img->select_list.listcount > 0 )
	    PreviousSelectedTime ( "SET", metObjectType, inList->label[0] );


/*
 *	Call metObject factory to make metObjects of the appropriate
 *	type.
 */
	mlist = MakeImageObjectList( metObjectType, count, inList->string,
						inList->label );

/*
 *	Put the objects into the display list.
 */
	AddObjectListToDisplay( count, mlist );

/*
 *	Create a DataTimeObject for time matching.
 */
	CreateDataTimeObject ( wo, mlist );

/*
 *	Save list indices in case dialog is not closed.
 */
	if ( img->match_list.listcount < 1 ) {
	    indexList = MatchIndicesToList (
			img->time_list.label,
			img->time_list.listcount,
			metObjectType, &matchSize );
	    img->match_list.listcount = matchSize;
	    img->match_list.intList = indexList;
	}

/*
 *	Adding images into the list means any previous met objects
 *	should be invalidated (redrawn). 
 */

	ClearAllPots (wo);

	SetWindowObjectDrawnFlag ( wo, False );

	if( wo ) {
		ProcessDisplayList( wo );
	}

/*
 *	Refresh the image time list if time matching
 */
	RefreshScrolledTimeLists ( True );

/*
 *	Free.
 */
	FreeListParentObject ( inList );
	Free ( mlist );


}



int
GetImageFileList ( int type, char *path, char ***files, int *start, int *count)
{
	int	argc;
	int	newstart, newcnt;
	

/*
 *	Read the directory and time filter 
 */
	argc  = readdirectory(path, files, NULL, NULL);
	FilterProducts( type, argc, path, *files, &newstart, &newcnt );

	*start = newstart;
	*count = newcnt;

	return (argc);

}


void
UpdateScrolledList( ButtonObject *bo )
{

	char			*path;
	char			*fullpath;
	char			**files;
	char			*timestamp;
	int			nfiles;
	int			i, start, count;
	int			metObjectType;
	int 			start_position, end_position, num_selected;
	ListParentObjectType	*time_list;
	ListIndexObjectType	*match_list;
	ListIndexObjectType	*select_list;
	GuiImgSelectDialogType	*img;
	Widget			timeListW;

 	img = GetGuiImgSelectDialog();
	time_list  = GetImgTimeListObj(img);
	match_list  = GetImgMatchListObj(img);
	select_list  = GetImgSelectListObj(img);
	metObjectType = GetImgProdPanelSetbuttonType(img);
	timeListW = GetImgTimeListW(img);	

	if ( GetVerboseLevel() > VERBOSE_0 )
	    printf("UpdateScrolledList\n");

/*
 *	Extract the path information associated with the toggle button, 
 *	and get the filelist in our time domain.
 */
	path = builddirpath ( bo->string, bo->label );
	nfiles = GetImageFileList ( metObjectType, path, &files, &start, &count );

/*
 *	Free previously allocated memory in the list struct
 */
	if ( time_list->listcount > 0 ) {
	    StringListFree ( time_list->listcount, time_list->string );
	    StringListFree ( time_list->listcount, time_list->label );
	    time_list->listcount = 0;
	}

	if ( match_list->listcount > 0 ) {
	    Free ( match_list->intList );
	    match_list->listcount = 0;
	}

/*
 *	If there are no files in the directory clear out the scrolled
 *	list and return.
 */
	if ( nfiles <= 0 ) {
		RemoveProductsFromList ( timeListW );
		Free ( path );
		return ;
	}

/*
 *	if everything has been filtered out, then just return back.
 */
	if( count <= 0 ) {
		StringListFree( nfiles, files );
		Free ( path );
		return;
	}


/*
 *	Allocate storage for the file path and label arrays
 */
	time_list->string = StringListAllocate ( count );
	time_list->label  = StringListAllocate ( count );

/*
 *	Add the path and label (filename) into the list struct
 */
	for ( i = start; i < start + count; i++ ) {

	    fullpath = builddirpath ( path, files[i] );
            time_list->string[i] = (char *) malloc(strlen(fullpath) + 1);
            strcpy(time_list->string[i], fullpath );

	    timestamp = FileToDattim ( metObjectType, fullpath);

            time_list->label[i] = (char *) malloc(strlen(timestamp) + 1);
            strcpy(time_list->label[i], timestamp );

	    Free ( fullpath );
	    Free ( timestamp );
	}
	
	time_list->listcount = count;

/*
 *	Make sure year is in 4 digits.
 */
	Y2KList ( &time_list->label, time_list->listcount);

/*
 *	Process the list, ie; time match 
 */
	SetScrolledList ( metObjectType, GetImgTimeListLabelW(img),
			  timeListW, time_list, match_list, select_list );

/*
 *	Free remaining stuff
 */
	StringListFree( nfiles, files );
	Free (path);

	return;

}



/*
 *
 *  Add a list of directory entries buttons to a RC widget.
 *
 *
 */ 
int AddButtonsToPanel ( ButtonParentObjectType  *bpo,
		 void		(*func)(), 
		 int		metObjectType,
		 char		*fulldirectorypath )
{

	int			cnt, i, j, count;
	ButtonObject		*bo;
	int			argc; 
	char			**argv;
	int			returnStatus;

	if ( GetVerboseLevel() > VERBOSE_0 )
	    printf("AddButtonsToPanel\n");

/*
 *	Get the sorted directory listing...
 */
	argc = readdirectory ( fulldirectorypath, &argv , NULL, NULL);
	if( argc <= 0 )
		return ( 1 );


	returnStatus = UpdateButtonPanel( bpo, func, metObjectType, 
				fulldirectorypath, argc, argv);

	free ( argv );

	return ( returnStatus );

}


/*
 *
 *	Look at the files in argv and return the count of products
 *	which is cronologically before the time in the "td"
 *	struct.  argv is sorted alphabetically (which 
 *	for these file names, is chronological order also ).
 *
 */
void
FilterProducts ( int type, int argc, char *path, char **argv, int *newstart, int *newcnt )
{

	TimeObjectType	*td;
	int		i;
	char		*file;
	AbsTime		fileTime, starttm, endtm, currenttm;
	char		begin_time[12], end_time[12], current_time[12];

	int		newEnd;
	int		newCnt;
	int		newStart;

	char        **dattimList;


	int verbose = GetVerboseLevel();

	if ( verbose > VERBOSE_0 ) printf ("FilterProducts\n" );
	if ( verbose > VERBOSE_1 )
	    printf ("  Initial file count is %d\n", argc );

/*
 *	Initialize things...
 */
	*newstart = 0;
	*newcnt   = argc;

	newStart = 0; 
	newEnd   = argc - 1;

/*
 *	Check for input. 
 */
	if ( argc <= 0 ) 
		return;

/*
 *  	Get case start and end times. 
 */
	td = GetTimeObject();
	strcpy ( current_time, GetCaseCurrentTime ( td ) );
	strcpy ( begin_time,   GetCaseBeginTime ( td ) );
	strcpy ( end_time,     GetCaseEndTime ( td ) );

/*
 *	Convert case start and end times to integer format.
 */
	starttm = ch_to_int_time ( begin_time );
	endtm   = ch_to_int_time ( end_time );

/*
 *	Get the current time...
 */
	currenttm = (AbsTime)present_time();

/*
 *	We could bail out early if the present time is "real"
 *	time +/- (or the clock hasn't been set) and GetCaseCurrentTime()
 *	is null.
 */
	if ( verbose > VERBOSE_1 ) {
		printf("  FilterProducts: clock time = %d\n", currenttm );
		printf("  FilterProducts: start time = %d\n", starttm );
		printf("  FilterProducts: end   time = %d\n", endtm );
	}


	if( starttm <= 0  && endtm <= 0 &&  currenttm > 0 ) {

		char *p;

		file = builddirpath (path, argv[newEnd]);
		
		p = FileToDattim( type, file );

		fileTime = ch_to_int_time ( p );

		Free( p );
		Free(file);
		
		if( currenttm >= fileTime  ) {
			if ( verbose > VERBOSE_1 )
				printf("  FilterProducts.  Early exit\n");
			return;
		}

	}

/*
 *	Filter out files that are before currenttm.
 */
	if ( currenttm > 0 ) {

		char	*p;
		int	fileTime;

		if( verbose > VERBOSE_1 )
			printf("  FilterProducts(currenttime):\n"); 


		for( i = argc - 1; i >= 0; i-- ) {

/*
 *			Convert the filename to a dattime stamp.
 */
 			file = builddirpath ( path, argv[i]);
			p = FileToDattim( type, file );
			if( !p ) {
			    printf("FilterProducts: FileToDattim returns null for %s\n",argv[i]); 
			    break;
			}

			fileTime = ch_to_int_time ( p );

			if( verbose > VERBOSE_1 ) {
				printf("  Compare %s", argv[i] );
				printf("(%s -> ", p );
				printf("%d)", fileTime );
				printf(" to %d\n", currenttm );
			}	
			Free( p );
			Free(file);

			if( currenttm >= fileTime  )
				break;

		}

/*
 *		See if we bailed out of the loop.
 *		Set the subscript of the new ending point (ie. the file
 *		before we broke out of the loop ).
 */
		if( i >= 0 )
			newEnd = i > 0 ? i : 0;

	}

/*
 *	Filter out files before "start_time".
 */
	if( starttm > 0 ) {

		char	*p;
		int	fileTime;

		if( verbose > VERBOSE_1 )
			printf("  FilterProducts(starttime):\n");

		newCnt = newEnd + 1;
		for( i = 0; i < newCnt ; i++ ) {

/*
 *			Convert the filename to a dattime stamp.
 */
			file = builddirpath ( path, argv[i]);
			p = FileToDattim( type, file );
			if( !p ) {
			    printf("FilterProducts: FileToDattim returns null for %s\n",argv[i]); 
			    break;
			}

			fileTime = ch_to_int_time ( p );

			if( verbose > VERBOSE_1 ) {
				printf("  Compare %s", argv[i] );
				printf("(%s ->", p );
				printf(" %d) to", fileTime );
				printf(" %d\n", currenttm ); 
			}
	
			Free( p );
			Free(file);

			if( fileTime <= 0 ) {
			    printf("FilterProducts: FileToDattim returns -1 for file %s\n",argv[i]);
			}


			if ( fileTime > starttm ) 
				break;
		}

/*
 *		See if we bailed out early
 */
		if( i < newCnt )
			newStart = i > 0 ? i - 1 : 0;

	}

	*newstart = newStart;
	*newcnt   = newEnd - newStart + 1;

	if ( verbose > VERBOSE_1 ) {
		printf("\n\n  New start is %d", *newstart );
		printf(" for %d items\n", *newcnt);
		printf("  First elem. is %s", argv[*newstart] );
		printf(" and last is %s", argv[ (*newstart) + (*newcnt) - 1] );
		printf(" at subscript = %d\n", (*newstart) + (*newcnt) - 1 ); 
	}

	return ;
}


void RemoveButtonsFromPanel ( ButtonParentObjectType  *bpo )
{
	ButtonObject	*bp =  bpo->buttons;
	int 		i;

	if ( GetVerboseLevel() > VERBOSE_0 )
		printf("RemoveButtonsFromPanel\n");

	for ( i = 0; i < bpo->button_cnt; i++, bp++ ) {
	    XtDestroyWidget ( bp->w );
	    bp->ismanaged = 0;
	    Free ( bp->string );
	    Free ( bp->label );
	}

/*
 *	Reset the button count to reflect no buttons.
 */
	bpo->button_cnt = 0;
	bpo->button_alloc_cnt = 0;
	bpo->setbutton = NULL;

}


int
UpdateButtonPanel (  ButtonParentObjectType  *bpo,
		        void   (*func)(), 
			int	metObjectType,
			char   *fulldirectorypath,
			int    argc,
			char   **argv)
{

	ButtonObject	*bo;
	int		cnt, i, j, count;

	if ( GetVerboseLevel() > VERBOSE_0 )
	    printf("UpdateButtonPanel\n");

	cnt = bpo->button_cnt;

	if ( cnt < 0 || cnt > MAXBUTTONS )
		return( 1 );


	for ( bo = &(bpo->buttons[cnt]), i = 0; i < argc; i++, bo++ ) {

		bo->w = XtCreateManagedWidget (argv[i], 
			     xmToggleButtonWidgetClass, bpo->parent, NULL, 0 );

		XtAddCallback ( bo->w, XmNvalueChangedCallback, func, bo );

/*
 *		Fill in all the fields for this button object. Save the
 *		metObjectType in the integer placeholder (i).
 */
		bo->ismanaged		= 1;
		bo->i			= metObjectType;
		bo->string		= strdup ( fulldirectorypath );
		bo->label 		= argv[i];

/*
 *		Each string will get free'd when button destroyed
 */

	}

	bpo->button_cnt		+= argc;
	bpo->button_alloc_cnt	= bpo->button_cnt;

	return ( 0 );

}


