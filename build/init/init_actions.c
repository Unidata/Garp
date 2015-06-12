/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	init_actions.c
 *
 *	Action Table initilization function.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/

/*
 *  Copyright 1996, University Corporation for Atmospheric Research.
 */


#include "winactions.h"


/*
 *	Actions table initialization for event translations
 */

extern XtAppContext app_context;

void InitActions ()
{

	static  XtActionsRec actions[] = {
                {"HandlePointerMotion",		HandlePointerMotion},
                {"HandleBtn1Clicks",		HandleBtn1Clicks},
                {"HandleBtn1DownMotion",	HandleBtn1DownMotion},
                {"HandleCtrlBtn1Click",		HandleCtrlBtn1Click},
                {"HandleBtn3Clicks",		HandleBtn3Clicks},
                {"AnimPlayToggle",		AnimPlayToggle},
                {"AnimFwd",			AnimFwd},
                {"AnimBack",			AnimBack},	
                {"AnimRockToggle",		AnimRockToggle},
        };

        XtAppAddActions(app_context, actions, XtNumber(actions));

}

