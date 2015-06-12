/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	scrolledw.c
 *
 *	Utilities used by scrolled list widgets.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 3/97	COMET	Removed printf statement from SetSatEnhanceList()
 *	 8/97	COMET	Added DataTimeObjectType object for time matching,
 *			removed TimestampInfoType object.
 *	 8/97	COMET	Highlight a model forecast run in date/time list.
 *	 8/97	COMET	Moved AddProductsToList, SelectListItems and 
 *			RemoveProductsFromList to here from imagecb.c
 *	11/97	COMET	Added SetTimeListLabel(). Modify AddProductsToList
 *			and SelectListItems to check for null labels
 *	11/97	COMET	Fix bug in HighLightList() for lists < the number
 *			to highlight.
 *	12/97	COMET	Added routines to refresh scrolled time lists.
 *	12/97	COMET	Added a "select_list" object to track date/time
 *			selections when time matching is turned off.
 *	 1/98	COMET	Fix off-by-one errors in HighLightList() and
 *			HighLightForecastInterval() .
 *	 2/98	COMET	Set scrollbar position so the last highlighted 
 *			item is at the bottom of the scrolled list.
 *	 3/98	COMET	Wrote GetPreviousModel() to reset gridded data
 *			scrolled list to a forecast interval when a
 *			new model is chosen.
 *	12/99	COMET	Added Y2KList to make list times YYYYMMDD/HHMM.
 *
 ***********************************************************************/


#include <time.h>
#include <string.h>
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/PanedW.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/MessageB.h>
#include <Xm/Separator.h>
#include <Xm/List.h>
#include <Xm/Protocols.h>
#include <X11/cursorfont.h>
#include <stdlib.h>
#include <dirent.h>

#include "guimacros.h"
#include "gendef.h"
#include "genglobs.h"
#include "utils.h"
#include "winobj.h"
#include "children.h"
#include "_proto.h"

#include "imgobj.h"
#include "sfcobj.h"
#include "upaobj.h"
#include "upaprof.h"
#include "vprofobj.h"
#include "thgtobj.h"
#include "xsecobj.h"
#include "hsobj.h"
#include "hvobj.h"
#include "model.h"



int * MatchIndicesToList ( char **, int, int, int * );
void HighLightList ( Widget,int,ListParentObjectType *,ListIndexObjectType *);
void PreviousSelectedTime ( char *, int, char * );



AbsTime
GetTimeInterval ( int this_object )
{
	AbsTime	interval=0;

	if      ( this_object == SURFACEOBJECT )
	    interval = 3600;

	else if ( this_object == UPPERAIROBJECT )
	    interval = 43200;

	else {
		 printf( "GetTimeInterval - Need to define time \
		          interval for object type = %d\n", this_object );
	}
	return interval;
}

void
SelectListItems ( Widget list, int argc, char **argv )
{
	XmString	*xmstr;
	int		i;
	char	    missingString[] = "No Match";

/*
 *	Convert all command-line arguments to an array of
 *	type XmString. 
 */
	xmstr = ( XmString * ) XtMalloc ( sizeof ( XmString ) * argc );

	for ( i = 0; i < argc; i++ ) {
	    if ( argv[i] == NULL ) 
		xmstr [ i ] = XmStringCreateLtoR ( missingString, XmFONTLIST_DEFAULT_TAG );
	    else
		xmstr [ i ] = XmStringCreateLtoR ( argv[i], XmFONTLIST_DEFAULT_TAG );
	}

/*
 *	Programmatically select (ie. highlight) some items.
 */
	XtVaSetValues ( list,
                  	  XmNselectedItemCount,    argc,
                  	  XmNselectedItems,        xmstr,
                   	 NULL );
/*
 *	Do some clean up.
 */
	FreeMotifStringTable( argc, xmstr );

	return;

}


void AddProductsToList ( Widget list, int argc, char **argv )
{

	XmString    *xmstr;
	int          i;
	char	    missingString[] = "No Match";
	

	if ( GetVerboseLevel() > VERBOSE_0 )
		printf("AddProductsToList\n");

/*
 *	Convert all command-line arguments to an array of
 *	type XmString. 
 */
	xmstr = ( XmString * ) XtMalloc ( sizeof ( XmString ) * argc );

	for ( i = 0; i < argc; i++ ) {
	    if ( argv[i] == NULL ) 
		xmstr [ i ] = XmStringCreateLtoR ( missingString, XmFONTLIST_DEFAULT_TAG );
	    else
		xmstr [ i ] = XmStringCreateLtoR ( argv[i], XmFONTLIST_DEFAULT_TAG );
	}

/*
 *	Create the list widget and register a browse callback.
 */
	XtVaSetValues ( list,
                    	XmNitems,            xmstr,
                    	XmNitemCount,        argc,
                    	NULL );

/*
 *	Do some clean up.
 */

	FreeMotifStringTable( argc, xmstr );

	return;

}


void RemoveProductsFromList ( Widget list )
{
	char *tmpargv[1] = { " " };

	AddProductsToList ( list, 1, tmpargv );
	XtVaSetValues ( list, 
                    XmNitems,                NULL,
                    XmNitemCount,            0,
		    XmNselectedItemCount,    0,
                    NULL );

}

void
SavePickedList ( int num, int *index, char **labels, char ***picked_list )
/*
 * This function stores list items at given indices to to another list.
 */ 
{
	int	i;
	int	ind;
	char	**listp;

	
	*picked_list = StringListAllocate ( num );

	listp = *picked_list;

	for ( i=0; i<num; i++ ) {
	    ind = index[i];
/*
 *	Check for a real element.
 */
	    if ( ind > -1 ) {
	    	*listp = (char *) malloc ( strlen ( labels[ind]) + 1 );
	    	strcpy ( *listp, labels[ind] );
	    }
	    else
		*listp = NULL;

	    listp++;
	}
}


void
SetScrolledList ( int obType, Widget list_label,
                  Widget scrolled_list,
                  ListParentObjectType *list_info,
                  ListIndexObjectType *match_list,
                  ListIndexObjectType *select_list )
{
	WindowObjectType	*wo;
	DataTimeObjectType	*dto;
	XmString                label;

	char			**MatchList;
	int			*indexList;
	int			matchSize;
	int			i;
	BooleanType		verbose;
	char			string1[] = "Matched Times";
	char			string2[] = "Available Times";


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "SetScrolledList\n" );

	dto = GetDataTimeObject();
/*
 *	Time match.
 */
	if ( dto->numTimes > 0 && dto->matchType != NOTIMEMATCH ) {
	    indexList = MatchIndicesToList (
				list_info->label,
				list_info->listcount,
				obType, &matchSize );

	    match_list->listcount = matchSize;
	    match_list->intList = indexList;

/*
 *	    Save indices which time match to a list.
 */
	    SavePickedList ( matchSize, indexList, list_info->label,
                                                &MatchList );
/*
 *	    Write list to scrolled window.
 */
	    AddProductsToList ( scrolled_list, matchSize, MatchList );

/*
 *	    Highlight selected items.
 */
	    SelectListItems ( scrolled_list, matchSize, MatchList );
/*
 *	    Desensitize list.
 */
	    XtSetSensitive ( scrolled_list, FALSE );

/*
 *	    Free.
 */
	    StringListFree ( matchSize, MatchList );
	}

/*
 *	DataTimeObject does not exist.
 */
	else {

	    AddProductsToList ( scrolled_list,
	                        list_info->listcount,
	                        list_info->label );
/*
 *	    Highlight list items.
 */
	    HighLightList ( scrolled_list, obType, list_info, select_list );

/*
 *	    Sensitize list.
 */
	    XtSetSensitive ( scrolled_list, TRUE );
	}

/*
 *	Set the label of the list based on the time matching on or off.
 */
	if ( list_label != NULL ) {

	    if ( dto->numTimes > 0 && dto->matchType != NOTIMEMATCH )

		label = XmStringLtoRCreate ( string1,
                	(XmStringCharSet)XmFONTLIST_DEFAULT_TAG );
	    else

		label = XmStringLtoRCreate ( string2,
                	(XmStringCharSet)XmFONTLIST_DEFAULT_TAG );

            XtVaSetValues ( list_label, XmNlabelString, label, NULL );
            XmStringFree ( label );

	}

	return;
}


int
HighLightForecastInterval ( ListParentObjectType *list_info )
{
/*
 * Highlight the date/time stamps in a scrolled list for a model
 * forecast run.
 */
	int	i, count = 0;

	for ( i=list_info->listcount-1; i>-1; i-- ) {
	    if ( strstr ( list_info->label[i], "F000" ) != NULL ) {
		count++;
		break;
	    }
	    count++;
	}

	return count;
}


BooleanType
GetPreviousModel ( int obType )
{
	ModelInfoType			*mdl;
	GuiXSectionDialogType		*vcs;
	GuiTimeHeightDialogType		*tht;
	GuiVertProfileObjectType	*vpt;
	
	BooleanType	newModel = True;
	char		*model;
	static char	grdModel[GEMPAKSTRING] = " ",
			vcsModel[GEMPAKSTRING] = " ",
			thtModel[GEMPAKSTRING] = " ", 
			vptModel[GEMPAKSTRING] = " ";

	mdl = GetGuiModelInfo();
	vcs = GetGuiXSectionDialog();
	tht = GetGuiTimeHeightDialog();
	vpt = GetGuiVertProfileDialog();

	if ( obType == SCALARGRIDOBJECT ||
	     obType == VECTORGRIDOBJECT ||
	     obType == SCALARGRIDFILLOBJECT ) {
	    model = strdup ( GetModelType ( mdl ) );
	    if ( strcmp ( model, grdModel ) == 0 ) newModel = False;
	    strcpy ( grdModel, model );
	}

	else if ( obType == XSECTIONGRIDOBJECT ||
	          obType == XSECTIONGRIDFILLOBJECT ) {
	    model = strdup ( GetModelTypeXS ( vcs ) );
	    if ( strcmp ( model, vcsModel ) == 0 ) newModel = False;
	    strcpy ( vcsModel, model );
	}

	else if ( obType == TIMEHEIGHTOBJECT ) {
	    model = strdup ( GetModelTypeTH ( tht ) );
	    if ( strcmp ( model, thtModel ) == 0 ) newModel = False;
	    strcpy ( thtModel, model );
	}

	else if ( obType == VERTICALPROFILEOBJECT ) {
	    model = strdup ( GetModelTypeVP ( vpt ) );
	    if ( strcmp ( model, vptModel ) == 0 ) newModel = False;
	    strcpy ( vptModel, model );
	}

	Free ( model );

	return newModel;
}


void
HighLightList ( Widget scrolled_list,
	        int obType,
	        ListParentObjectType *list_info,
	        ListIndexObjectType *select_list )
{
/*
 * If some items in this scrolled window were previously highlighted, then
 * highlight them again. Otherwise, just highlight a range of items.
 */
	char			**list, previousTime[16];

	int			highlight_position;
	int			scrollbar_position=0;
	int			i, num, count = 0;
	int			startcnt, offset;

	BooleanType		fallback = True;
	BooleanType		newModel = False;

/*
 *	If this is a gridded data list, check to see if a new model
 *	type was chosen. If so, use the fallback highlighting mechanism.
 */
	if ( obType == SCALARGRIDOBJECT ||
	     obType == VECTORGRIDOBJECT ||
	     obType == XSECTIONGRIDOBJECT ||
	     obType == TIMEHEIGHTOBJECT ||
	     obType == VERTICALPROFILEOBJECT ||
	     obType == SCALARGRIDFILLOBJECT ||
	     obType == XSECTIONGRIDFILLOBJECT )
	    newModel = GetPreviousModel (obType );

/*
 *	If the number of previously highlighted items exceeds the number
 *	of items in the current list, use the fallback method.
 */
	if ( !newModel &&
	     list_info->listcount >= select_list->listcount &&
	                             select_list->listcount > 0 ) {
	    fallback = False;
	    num = select_list->listcount;

/*
 *	Time match the previous first time to the first dattim in the
 *	scrolled list.
 */
	    PreviousSelectedTime ( "GET", obType, previousTime );
	    startcnt = ClosestIndexTimeMatch ( list_info, previousTime );
	    offset = select_list->intList[0] - startcnt;

/*
 *	Make sure you don't try to highlight out-of-bounds indices. If
 *	any one index is out-of-bounds, skip to the backup highlight method.
 */
	    for ( i=0; i<num; i++ )
		if ( list_info->listcount <
	                        (select_list->intList[i]-offset+1) ) {
		    fallback = True;
		    break;
		}

/*
 *	Mark the remaining items while preserving the offset between
 *	items in the list.
 */
	    if ( !fallback ) {

	    	list = StringListAllocate ( num );

	    	for ( i=0; i<num; i++ ) {
		    count = select_list->intList[i] - offset;
	            list[i] =
	              strdup ( list_info->label[count] );
		}

/*
 *	Highlist items in list.
 */
	    	SelectListItems ( scrolled_list, num, list );

/*
 *	Set scrollbar position. Account for difference in indexing.
 */
	    	scrollbar_position = select_list->intList[0] - offset + num;
	    	XmListSetBottomPos ( scrolled_list, scrollbar_position );

	    	StringListFree ( num, list );
	    }
	}

	if ( fallback ) {
/*
 * When not time matching, highlight forecast time in a scrolled
 * list by an obType dependent algorithm.
 */

	    switch (obType) {
	    case SURFACEOBJECT:
	      count = 6;
	      break;
	    case RADAROBJECT:
	      count = 6;
	      break;
	    case SATOBJECT:
	      count = 6;
	      break;
	    case SCALARGRIDOBJECT:
	      count = HighLightForecastInterval ( list_info );
	      break;
	    case VECTORGRIDOBJECT:
	      count = HighLightForecastInterval ( list_info );
	      break;
	    case UPPERAIROBJECT:
	      count = 1;
	      break;
	    case XSECTIONGRIDOBJECT:
	      count = HighLightForecastInterval ( list_info );
	      break;
	    case SCALARGRIDFILLOBJECT:
	      count = HighLightForecastInterval ( list_info );
	      break;
	    case TIMEHEIGHTOBJECT:
	      count = 1;
	      break;
	    case VERTICALPROFILEOBJECT:
	      count = HighLightForecastInterval ( list_info );
	      break;
	    case UPPERAIRPROFILEOBJECT:
	      count = 1;
	      break;
	    case XSECTIONGRIDFILLOBJECT:
	      count = HighLightForecastInterval ( list_info );
	      break;
	    default:
	      count = 6;
	    }
	    highlight_position = list_info->listcount - count;
/*
 *	Handle situation where there are fewer items in the list than
 *	we want to highlight
 */
	    if ( highlight_position < 0 ) {
	        highlight_position = 0;
	        count = list_info->listcount;
	    }

	    if ( count > 0 )
	        SelectListItems ( scrolled_list, count, 
	                      &(list_info->label[highlight_position]) );

/*
 *	Set scrollbar to bottom of list.
 */
	    XmListSetBottomPos ( scrolled_list, scrollbar_position );

	}

}


void
SetSatEnhanceList ( Widget scrolled_list )
{
	extern struct enhance_list	satenhance_list;

/*
 *	Unmanage scrolled list.
 */
	XtUnmanageChild ( scrolled_list );

/*
 *	Get enhancement table.
 */
	if ( ReadSatTable ( &satenhance_list ) != 0 )
	    return;

/*
 *	Write list to widget.
 */

	AddProductsToList ( scrolled_list, satenhance_list.size,
	                    satenhance_list.list );

/*
 *	Manage scrolled list.
 */
	XtManageChild ( scrolled_list );
}


void
AdjustIndexOffset ( int sw_count, int *sw_index )
/*
 * This function decrements the indices of an integer array by one. It
 * is intended to allign indices from scrolled lists ( which start at one )
 * with indices in C arrays ( which start at zero ).
 *
 */
{
	int	i;
	int	verbose;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "AdjustIndexOffset\n" );

	for ( i=0; i<sw_count; i++ )
	    sw_index[i]--;
}


/*
 *	Refreshes a single scrolled time list, including updating the
 *	label widget.
 */
void
RefreshScrolledTimeList (int metObjectType,
			 Widget timeListW,
			 Widget listLabelW,
			 ListParentObjectType *time_list,
			 ListIndexObjectType  *match_list,
			 ListIndexObjectType  *select_list )
{

	if ( match_list->listcount > 0 ) {
	    Free ( match_list->intList );
	    match_list->listcount = 0;
	}

/*
 *	Update the list
 */
	SetScrolledList ( metObjectType,
			  listLabelW,
			  timeListW,
			  time_list,
			  match_list,
			  select_list );

	XmListSetBottomPos ( timeListW, 0 );
}



/*
 *	Refreshes any managed scrolled time list. Called when the time
 *	matching mode changes. If a dialog is currently managed, it
 *	is refreshed, otherwise ignored. If timeMatchOnlyFlag is true,
 *	the list is refreshed only if we are in time matching mode,
 *	otherwise the list is refreshed regardless of the time matching
 *	mode.
 */
void
RefreshScrolledTimeLists ( BooleanType timeMatchOnlyFlag )
{

	GuiImgSelectDialogType		*img;
	GuiSfcObsDialogType		*sfc;
	GuiUpperairDialogType		*upa;
	GuiVertProfileObjectType	*vpt;
	GuiUpaProfileDialogType		*upf;
	GuiXSectionDialogType		*vcs;
	GuiScalarGridDialogType		*grd;
	DataTimeObjectType		*dto;
	int				verbose;
	

	verbose = GetVerboseLevel ();

/*
 *	If the time matching only flag is true, exit if we are not doing
 *	time matching.
 */
 	dto = GetDataTimeObject();
	if ( timeMatchOnlyFlag && 
	   ( dto->numTimes == 0 || dto->matchType == NOTIMEMATCH) ) return;

/*
 *	Update image time list
 */
	img = GetGuiImgSelectDialog();
	if ( XtIsManaged ( GetImgDialogW(img) ) ) {

	    if ( verbose > VERBOSE_0 )
	    	printf("RefreshScrolledTimeLists(): doing image time list...\n");

/*
 *	    Update the list
 */
	    RefreshScrolledTimeList (	SATOBJECT,
				 	GetImgTimeListW (img),
				 	GetImgTimeListLabelW(img),
				 	&(img->time_list),
				 	&(img->match_list),
				 	&(img->select_list) );
	}

/*
 *	Update surface list ...
 */
	sfc = GetGuiSfcObsDialog();
	if ( XtIsManaged ( GetSfcDialogW(sfc))) {

	    if ( verbose > VERBOSE_0 )
	    	printf("RefreshScrolledTimeLists(): doing surface time list...\n");

/*
 *	    Update the list
 */
	    RefreshScrolledTimeList ( 	SURFACEOBJECT,				 
				 	GetSfcScrolledListW(sfc),
				 	GetSfcTimeListLabelW(sfc),
				 	&(sfc->time_list),
				 	&(sfc->match_list),
				 	&(sfc->select_list) );
	}

/*
 *	Update upper air ...
 */
	upa = GetGuiUpperairDialog();
	if ( XtIsManaged ( GetUpaDialogW(upa) )) {

	    if ( verbose > VERBOSE_0 )
	    	printf("RefreshScrolledTimeLists(): doing upperair time list...\n");

/*
 *	    Update the list
 */
	    RefreshScrolledTimeList ( 	UPPERAIROBJECT,
				 	GetUpaScrolledListW(upa),
				 	GetUpaTimeListLabelW(upa),
				 	&(upa->time_list),
				 	&(upa->match_list),
				 	&(upa->select_list) );
	}

/*
 *	Update upper air profile ...
 */
	upf = GetGuiUpaProfileDialog();
	if ( XtIsManaged ( GetUpaProfDialogW(upf))) {

	    if ( verbose > VERBOSE_0 )
	    	printf("RefreshScrolledTimeLists(): doing upperair profile time list...\n");

/*
 *	    Update the list
 */
	    RefreshScrolledTimeList (	UPPERAIRPROFILEOBJECT,
					GetScrolledListUFW(upf),
					GetUpaProfTimeListLabelW(upf),
					&(upf->time_list),
					&(upf->match_list),
					&(upf->select_list)  );
	}


/*
 *	Update model plan view ...
 */
	grd = GetGuiScalarGridDialog();
	if ( XtIsManaged ( GetModelHSDialogW(grd))) {

	    if ( verbose > VERBOSE_0 )
	    	printf("RefreshScrolledTimeLists(): doing model plan view time list...\n");

/*
 *	    Update the list
 */
	    RefreshScrolledTimeList (	SCALARGRIDOBJECT,
					GetModelScrolledListHSW(grd),
				 	GetModelTimeListLabelW(grd),
				 	&(grd->time_list),
				 	&(grd->match_list),
				 	&(grd->select_list) );
	}


/*
 *	Update model vertical profile ...
 */
	vpt = GetGuiVertProfileDialog();
	if ( XtIsManaged ( GetModelVPDialogW(vpt))) {

	    if ( verbose > VERBOSE_0 )
	    	printf("RefreshScrolledTimeLists(): doing model vertical profile time list...\n");

/*
 *	    Update the list
 */
	    RefreshScrolledTimeList (	VERTICALPROFILEOBJECT,
				 	GetModelScrolledListVPW(vpt),
				 	GetModelVPTimeListLabelW(vpt),
				 	&(vpt->time_list),
				 	&(vpt->match_list),
				 	&(vpt->select_list) );
	}


/*
 *	Update model cross section ...
 */
	vcs = GetGuiXSectionDialog();
	if ( XtIsManaged (GetModelXSDialogW(vcs) )) {

	    if ( verbose > VERBOSE_0 )
	    	printf("RefreshScrolledTimeLists(): doing model cross section time list...\n");

/*
 *	    Update the list
 */
	    RefreshScrolledTimeList (	XSECTIONGRIDOBJECT,
				 	GetModelScrolledListXSW(vcs),
				 	GetModelCSTimeListLabelW(vcs),
				 	&(vcs->time_list),
				 	&(vcs->match_list),
				 	&(vcs->select_list) );
	}


}


void
PreviousSelectedTime ( char *inout, int obType, char *previousTime )
{
/*
 * Buffer the first time in each date/time scrolled list. This is used
 * to highlight list items when time matching is turned off.
 */
	static char		obTypeTime[MAXMETOBJS][20];
	int			i;
	static BooleanType	first=True;


/*
 *	Initialize buffer first time through.
 */
	if ( first ) {
	    for ( i=0; i<MAXMETOBJS; i++ )
		strcpy ( obTypeTime[i], "0" );
	    first = False;
	}


	if ( strcmp ( inout, "GET" ) == 0 ) {
	    strcpy ( previousTime, obTypeTime[obType] );
	}
	else if ( strcmp ( inout, "SET" ) == 0 ) {
	    strcpy ( obTypeTime[obType], previousTime );
/*
 *	The following code will tie the image metObject types together
 *	such that once a previous time is set for one type, it is set
 *	for the others also. It would be preferrable to loop through these.
 */

	    if ( GetMetObjectClass ( obType ) == IMAGE ) {
	    	switch ( obType ) {
	    	case SATOBJECT:
	      	  strcpy ( obTypeTime[NOWRADOBJECT], previousTime );
	      	  strcpy ( obTypeTime[RADAROBJECT], previousTime );
	      	  break;

	    	case NOWRADOBJECT:
	      	  strcpy ( obTypeTime[SATOBJECT], previousTime );
	      	  strcpy ( obTypeTime[RADAROBJECT], previousTime );
	      	  break;

	    	case RADAROBJECT:
	      	  strcpy ( obTypeTime[SATOBJECT], previousTime );
	      	  strcpy ( obTypeTime[NOWRADOBJECT], previousTime );
	      	  break;
		}
	    }
/*
 *	Since the plan view scalar and vector grid objects share the same
 *	date/time list, ensure the saved times are in sync.
 */
	    if ( obType == SCALARGRIDOBJECT )
		strcpy ( obTypeTime[VECTORGRIDOBJECT], previousTime );
	    if ( obType == VECTORGRIDOBJECT )
		strcpy ( obTypeTime[SCALARGRIDOBJECT], previousTime );
	}
	else {
	    printf("*** Error in PreviousSelectedTime - inout = %s\n", inout );
	}
}


void Y2KList ( char ***oldTimeList, int numEntrees ) {
	char **timeList;
	char newTime[50];
	int i, year, century;

	timeList = *oldTimeList;
/*
 *	Check for 4 digit year. True if year starts with a "1" or "2".
 *	We assume all the times in the list have the same format.
 */
	sscanf( timeList[0], "%02d", &year);
	if ( year == 19 || year == 20) return;

/*
 *	Get the century. Rename the list with a 4 index year.
 */

	for (i=0; i<numEntrees; i++) {
	    sscanf( timeList[i], "%02d", &year);
	    century = GetCentury(year);
	    sprintf(newTime, "%d%s", century, timeList[i]);
	    Free(timeList[i]);
	    timeList[i] = strdup (newTime);
	}
}
