/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gridupfcb.c
 *
 *	Upper air callbacks for dynamically initialized widgets.	
 *
 *	History:
 *
 *	 4/97	COMET	Original copy
 *
 ***********************************************************************/

#include "geminc.h"
#include "gemprm.h"
#include "utils.h"
#include "ctbcmn.h"
#include "guimacros.h"
#include "upaprof.h"
#include "fdfobj.h"
#include "genglobs.h"
#include "winobj.h"
#include "menucb.h"
#include "_proto.h"


void
UFLineTypeCB ( Widget    w,
               XtPointer clientData,
               XtPointer call_data )
/*
 * Called when a model line type button is picked.
 */
{
	GuiUpaProfileDialogType	*upf;

	char			*linetype;
	int			verbose;


	ButtonObject *bo = (ButtonObject *) clientData;

	XmPushButtonCallbackStruct *cbs =
			(XmPushButtonCallbackStruct *) call_data;

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "UFLineTypeCB\n" );

	upf = GetGuiUpaProfileDialog();

	linetype = strdup ( bo->string );

	FreeLineTypeUF ( upf );
	SetLineTypeUF ( upf, linetype );

	Free ( linetype );
	return;
}
