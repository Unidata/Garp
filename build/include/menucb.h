/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	menucb.h
 *
 *	Some callback prototypes.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/

#ifndef _MENUCB_H
#define _MENUCB_H

void ImageCancelCB(Widget w,
                           XtPointer clientData,
	                   XtPointer callData);

void ImageOKCB     (Widget w,
                           XtPointer clientData,
                           XtPointer callData);

void QuitCB        ( Widget    w, 
                           XtPointer clientData, 
                           XtPointer callData );
void SaveCB        ( Widget    w, 
                           XtPointer clientData, 
                           XtPointer callData );
void LoadCB        ( Widget    w, 
                           XtPointer clientData, 
                           XtPointer callData );

void MenuRadarItemCB( Widget w,
                     XtPointer clientData,
                     XtPointer callData);

void MenuSatItemCB( Widget w,
                     XtPointer clientData,
                     XtPointer callData);

void MenuGenericImageCB( Widget w,
                     XtPointer clientData,
                     XtPointer callData,
                     Widget *dialog,
                     void (*okFuncCB)(),
                     char *defaultDir);

void DispatchCB(     Widget w,
                XtPointer client,
                XtPointer calldata);

void RadarObjectCB( Widget w,
               XtPointer clientData,
               XtPointer callData);

void SatObjectCB( Widget  w,
               XtPointer   clientData,
               XtPointer   callData);

void SurfaceCB ( Widget  w,
               XtPointer   clientData,
               XtPointer   callData);

void GenericCB();
void StopCB();
void FwdCB();

#endif /*  _MENUCB_H  */
