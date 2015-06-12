/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	background.h
 *
 *	Structure for saving overlay widget info.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Added sanity tests to strdup and free macros.
 *	 3/97	COMET	Added option menus for station parameters.
 *
 ***********************************************************************/


#ifndef _BACKGROUND_H
#define _BACKGROUND_H

#include "children.h"


/*
 *	Dialogs.
 */

typedef struct {
	TextFieldObjectType	rring_number;	  /* number radar rings	*/
	TextFieldObjectType	rring_minimum;	  /* minimum distance   */
	TextFieldObjectType	rring_increment;  /* increment          */
	TextFieldObjectType	rring_stid;	  /* station id		*/
	ButtonParentObjectType	rring_units;	  /* units		*/
	ToggleButtonObjectType  rring_allstid;	  /* all stations button*/
	ToggleButtonObjectType	rring_onestid;	  /* all stations button*/
	ButtonParentObjectType  stn_type;	  /* Station type	*/
	ButtonParentObjectType	stn_parm;         /* Parameter to plot	*/
} GuiBackgroundDialogType;


GuiBackgroundDialogType *GetGuiBackgroundDialog();


#define SetRringNumberW(p,c)		((p)->rring_number.w=(c))
#define GetRringNumberW(p)		((p)->rring_number.w)
#define SetRringNumber(p,c)		((p)->rring_number.string = (c) ?	\
					 strdup(c) : (c))
#define GetRringNumber(p)		((p)->rring_number.string)
#define FreeRringNumber(p)		((p)->rring_number.string ?		\
					 free((p)->rring_number.string) : (void)1)


#define SetRringMinimumW(p,c)		((p)->rring_minimum.w=(c))
#define GetRringMinimumW(p)		((p)->rring_minimum.w)
#define SetRringMinimum(p,c)		((p)->rring_minimum.string = (c) ?	\
					 strdup(c) : (c))
#define GetRringMinimum(p)		((p)->rring_minimum.string)
#define FreeRringMinimum(p)		((p)->rring_minimum.string ?		\
					 free((p)->rring_minimum.string) : (void)1)


#define SetRringIncrementW(p,c)		((p)->rring_increment.w=(c))
#define GetRringIncrementW(p)		((p)->rring_increment.w)
#define SetRringIncrement(p,c)		((p)->rring_increment.string = (c) ?	\
					 strdup(c) : (c))
#define GetRringIncrement(p)		((p)->rring_increment.string)
#define FreeRringIncrement(p)		((p)->rring_increment.string ?		\
					 free((p)->rring_increment.string) : (void)1)


#define SetRringUnitsW(p,c)		((p)->rring_units.w=(c))
#define GetRringUnitsW(p)		((p)->rring_units.w)
#define SetRringUnits(p,c)		((p)->rring_units.string = (c) ?	\
					 strdup(c) : (c))
#define GetRringUnits(p)		((p)->rring_units.string)
#define FreeRringUnits(p)		((p)->rring_units.string ?		\
					 free((p)->rring_units.string) : (void)1)


#define SetRringStidW(p,c)		((p)->rring_stid.w=(c))
#define GetRringStidW(p)		((p)->rring_stid.w)
#define SetRringStid(p,c)		((p)->rring_stid.string = (c) ?		\
					 strdup(c) : (c))
#define GetRringStid(p)			((p)->rring_stid.string)
#define FreeRringStid(p)		((p)->rring_stid.string ?		\
					 free((p)->rring_stid.string) : (void)1)


#define SetRringAllstidToggleW(p,c)	((p)->rring_allstid.w=(c))
#define GetRringAllstidToggleW(p)	((p)->rring_allstid.w)


#define SetRringOnestidToggleW(p,c)	((p)->rring_onestid.w=(c))
#define GetRringOnestidToggleW(p)	((p)->rring_onestid.w)


#define SetStationTypePmW(p,c)		((p)->stn_type.parent=(c))
#define GetStationTypePmW(p)		((p)->stn_type.parent)
#define SetStationTypeOmW(p,c)		((p)->stn_type.menu=(c))
#define GetStationTypeOmW(p)		((p)->stn_type.menu)
#define SetStationType(p,c)		((p)->stn_type.string = (c) ? \
					 strdup(c) : (c))
#define GetStationType(p)		((p)->stn_type.string)
#define FreeStationType(p)		((p)->stn_type.string ?	\
					 free((p)->stn_type.string) : (void) 1)


#define SetStationParmPmW(p,c)		((p)->stn_parm.parent=(c))
#define GetStationParmPmW(p)		((p)->stn_parm.parent)
#define SetStationParmOmW(p,c)		((p)->stn_parm.menu=(c))
#define GetStationParmOmW(p)		((p)->stn_parm.menu)
#define SetStationParm(p,c)		((p)->stn_parm.string = (c) ? \
					 strdup(c) : (c))
#define GetStationParm(p)		((p)->stn_parm.string)
#define FreeStationParm(p)		((p)->stn_parm.string ?	\
					 free((p)->stn_parm.string) : (void) 1)


#endif  /*   _BACKGROUND_H   */
