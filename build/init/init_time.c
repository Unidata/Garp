/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	init_time.c
 *
 *	GUI dialog initialization.
 *
 *	History:
 *
 *	11/97	COMET	Original copy
 *	12/97	COMET	Fixed small leak (timeInList) and bug in InitTime-
 *			MatchDialog() 
 *
 ***********************************************************************/

#include "genglobs.h"
#include "guimacros.h"
#include "_proto.h"
#include "timematch.h"
#include "winobj.h"


void
InitTimeMatchDialog ()
/*
 *	Initialize widgets values for dialogs.
 */
{
	GuiTimeMatchDialogType	*tm;
	DataTimeObjectType	*dto;

	char			*timeMatch, *timesInList;


	tm = GetGuiTimeMatchDialog();
	dto = GetDataTimeObject();

/*
 *	Get key value for default time match type.
 */
	timeMatch   = GetConfigValue ( "TimeMatch" );
	timesInList = GetConfigValue ( "TimesInList" );

/*
 *	Set time match type.
 */
	if ( timeMatch != NULL ) {
	    ToLower ( timeMatch );
            if       ( strcmp ( timeMatch, "none" ) == 0 ) {
		dto->matchType = NOTIMEMATCH;
            	if ( ! XmToggleButtonGetState ( GetTimeMatchButton1W (tm) ) )
                    XmToggleButtonSetState ( GetTimeMatchButton1W(tm),
	                                     TRUE, FALSE );
            	if ( XmToggleButtonGetState ( GetTimeMatchButton2W(tm) ) )
                    XmToggleButtonSetState ( GetTimeMatchButton2W(tm),
	                                     FALSE, FALSE );
            	if ( XmToggleButtonGetState ( GetTimeMatchButton3W(tm) ) )
                    XmToggleButtonSetState ( GetTimeMatchButton3W(tm),
	                                     FALSE, FALSE );
       	    } 

            else if ( strcmp ( timeMatch, "strict" ) == 0 ) {
		dto->matchType = INTERVAL_ON;
            	if ( ! XmToggleButtonGetState ( GetTimeMatchButton2W (tm) ) )
                    XmToggleButtonSetState ( GetTimeMatchButton2W(tm),
	                                     TRUE, FALSE );
            	if ( XmToggleButtonGetState ( GetTimeMatchButton1W(tm) ) )
                    XmToggleButtonSetState ( GetTimeMatchButton1W(tm),
	                                     FALSE, FALSE );
            	if ( XmToggleButtonGetState ( GetTimeMatchButton3W(tm) ) )
                    XmToggleButtonSetState ( GetTimeMatchButton3W(tm),
	                                     FALSE, FALSE );
            }

            else if ( strcmp ( timeMatch, "closest" ) == 0 ) {
		dto->matchType = INTERVAL_OFF;
            	if ( ! XmToggleButtonGetState ( GetTimeMatchButton3W (tm) ) )
                    XmToggleButtonSetState ( GetTimeMatchButton3W(tm),
	                                     TRUE, FALSE );
            	if ( XmToggleButtonGetState ( GetTimeMatchButton1W(tm) ) )
                    XmToggleButtonSetState ( GetTimeMatchButton1W(tm),
	                                     FALSE, FALSE );
            	if ( XmToggleButtonGetState ( GetTimeMatchButton2W(tm) ) )
                    XmToggleButtonSetState ( GetTimeMatchButton2W(tm),
	                                     FALSE, FALSE );
            }

	    else {
		dto->matchType = INTERVAL_ON;
		printf("InitTimeMatchDialog - key for 'TimeMatch' is improperly defined\n" );
	    }

	    Free ( timeMatch );
	
	}
/*
 *	Determine whether all model times or analysis only times should
 *	be presented.
 */
	if ( timesInList != NULL ) {
	    ToLower ( timesInList );
            if       ( strcmp ( timesInList, "all" ) == 0 ) {
		dto->forecastType = ALL_TIMES;
            	if ( ! XmToggleButtonGetState ( GetModelTimesButton1W (tm) ) )
                    XmToggleButtonSetState ( GetModelTimesButton1W(tm),
	                                     TRUE, FALSE );
            	if ( XmToggleButtonGetState ( GetModelTimesButton2W(tm) ) )
                    XmToggleButtonSetState ( GetModelTimesButton2W(tm),
	                                     FALSE, FALSE );
       	    } 

            else if ( strcmp ( timesInList, "analysis" ) == 0 ) {
		dto->forecastType = ANALYSIS_TIME;
            	if ( ! XmToggleButtonGetState ( GetModelTimesButton2W(tm) ) )
                    XmToggleButtonSetState ( GetModelTimesButton2W(tm),
	                                     TRUE, FALSE );
            	if ( XmToggleButtonGetState ( GetModelTimesButton1W(tm) ) )
                    XmToggleButtonSetState ( GetModelTimesButton1W(tm),
	                                     FALSE, FALSE );
	    }

	    else {
		dto->forecastType = ALL_TIMES;
		printf("InitTimeMatchDialog - key for 'TimesInList' is improperly defined\n" );
	    }

	    Free ( timesInList );

	}

/*
 *	Initialize data type templates for time matching.
 */
	SetDataTypeTemplate();
}
