/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gdinfocb.c
 *
 *	Callback to buffer and maintain status of gridded model data.	
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/FileSB.h>
#include <stdio.h>
#include <string.h>

#include "genglobs.h"
#include "winobj.h"
#include "gdinfo.h"


void GdInfoCB ( Widget w, XtPointer clientData, XtPointer callData )
/* void gdinfo_cb ( Widget w, XtPointer clientData, XtPointer callData ) */
{

	static Widget parentDialog = NULL;
	static Widget fileBrowserWidget = NULL;
	XmString	xmstr;
	Cardinal	argcnt;

/*
 *	Create the dialog on the first time through...
 */
	if ( ! parentDialog ) {
		
		parentDialog =
			XmCreateBulletinBoardDialog(w, "FileBrowser", 
				NULL, 0);

		fileBrowserWidget = (Widget) CreateFileBrowser( parentDialog, 
					GetConfigValue( "grids" ));

	}


/*
 *	Toggle the display state of dialog... 
 */
	if( XtIsManaged( parentDialog ) )
		XtUnmanageChild ( parentDialog );
	else
		XtManageChild ( parentDialog );

}
