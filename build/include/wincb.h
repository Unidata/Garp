/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	wincb.h
 *
 *	Window callback function declarations.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	11/97	COMET	Added WindowStateChangeCB
 *
 ***********************************************************************/


#ifndef _WINCB_H
#define _WINCB_H

#include <X11/X.h>
#include <X11/Intrinsic.h>

#include "winobj.h"

void WindowObjectRegisterCallbacks( WindowObjectType *wo );

void WindowObjectExposeCB ( Widget    w,
                 XtPointer clientData,
                 XtPointer callData );


void CanvasResizeCB( Widget    w,
                 XtPointer clientData,
                 XtPointer callData );


void GetInputFocus( Widget    w,
                 XtPointer clientData,
                 XEvent *event,
                 Boolean *flag );

void WindowStateChangeCB( Widget    w,
                 XtPointer clientData,
                 XEvent *event,
                 Boolean *flag );


#endif /*  _WINCB_H  */
