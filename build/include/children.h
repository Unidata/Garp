/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	children.h
 *
 *	Structures for named object widget info.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	1/97	COMET	Added "int i" to ButtonObject to save a generic
 *			integer value.
 *	8/97	COMET	Added ListIndexObjectType for time matching.
 *
 ***********************************************************************/


#ifndef _CHILDREN_H
#define _CHILDREN_H

#include "gendef.h"
#include "gui.h"

#define MAXBUTTONS		200
#define ITEMCNT			15
#define IMAGECNT		10      
#define MAX_OPTION_BUTTONS	20

/*
 *	Dialog children.
 */

typedef struct {
	Widget		w;          /* id of button */
	int		ismanaged;  /* 0 if unmanaged */
	int		i;          /* placeholder for an integer */
	char		*string;    /* full file name ( or string) */
	char		*label;     /* button label */
} ButtonObject;

typedef struct {
	Widget		parent;               /* parent of buttons, not panel */
	Widget		menu;		      /* option menu id */
	char		*string;	      /* string information */
	char		*label;		      /* more string information */
	ButtonObject	buttons[MAXBUTTONS];  /* button information */
	ButtonObject	*setbutton;          /* pointer to Button that is set */
	int		button_cnt;           /* number of buttons in use */
	int		button_alloc_cnt;     /* number of buttons allocated */
} ButtonParentObjectType;

typedef struct {
	Widget		w;          /* id of button */
	BooleanType	toggled;    /* 1 if toggled */
	char		*label;     /* button label */
} ToggleButtonObjectType;

typedef struct {
	Widget		parent;               /* parent of buttons, not panel */
	ButtonObject	buttons[MAXBUTTONS];  /* button information */
	ButtonObject	*setbutton;          /* pointer to Button that is set */
	int		button_cnt;           /* number of buttons in use */
	int		button_alloc_cnt;     /* number of buttons allocated */
} ToggleButtonParentObjectType;
    
typedef struct {
	Widget		parent;               /* parent of scrolled list */
	Widget		w;                    /* scrolled list widget */
	int		listcount;            /* number of things in list */
	char		**string;             /* full file name ( or string) */
	char		**label;              /* labels displayed in list */
} ListParentObjectType;

typedef struct {
	int		listcount;            /* number of things in array */
	int		*intList;             /* index array               */
} ListIndexObjectType;

typedef struct {
	Widget		parent;	              /* parent of scrolled window */
	Widget		w;                    /* scrolled window widget */
	char		*string;              /* text to write */
} ScrolledWObjectType;

typedef struct {
	Widget		parent;               /* parent of text field */
	Widget		w;                    /* text field widget */
	char		*string;              /* string in text widget */
} TextFieldObjectType;


typedef struct {
	Widget		parent;               /* parent of label       */
	Widget		w;                    /* text field widget     */
	char		*string;              /* string in text widget */
} LabelObjectType;

typedef struct {
	Widget		parent;		      /* parent of scale      */
	Widget		w;		      /* scale widget id      */
	int		value;		      /* present scale value  */
} ScaleObjectType;

#endif  /*   _CHILDREN_H   */
#define SCALARGRIDMODEL		1
#define SCALARGRIDTIME		2
#define SCALARGRIDMODELLEVEL	3
#define SCALARGRIDLINETYPE	4
