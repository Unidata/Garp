/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	command.h
 *
 *	Header file for gui command dialogs.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *
 ***********************************************************************/


#ifndef _COMMAND_H
#define _COMMAND_H

#include "children.h"


/*
 *	Dialogs.
 */

typedef struct {
	TextFieldObjectType	outfile;	/* Output file name	*/
} GuiCommandDialogType;


GuiCommandDialogType *GetGuiCommandDialog();


#define SetSaveGifTextFieldW(g,c)	((g)->outfile.w=(c))
#define GetSaveGifTextFieldW(g)		((g)->outfile.w)

#endif  /*   _COMMAND_H   */
