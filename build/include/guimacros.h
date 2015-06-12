/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	guimacros.h
 *
 *	Macro definitions for the GUI.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 7/97	COMET	Removed some defunct macros.
 *
 ***********************************************************************/


/* 
 *  Animation buttons 
 */
#define NOLOOP			(-1)
#define LOOPBACKWARD		0
#define STEPBACKWARD		1
#define STOPLOOP		2
#define STEPFORWARD		3
#define LOOPFORWARD		4
#define LOOPFORWARDBACKWARD	5
#define STEPTOFRAME		6



/* Map defintions */

#define NORTHAMERICA	0
#define US		1
#define CANADA		2
#define ALASKA		3
#define HAWAII		4
#define NORTHEAST	5
#define SOUTHEAST	6
#define NORTHCENTRAL	7
#define SOUTHCENTRAL	8
#define NORTHWEST	9
#define SOUTHWEST	10
#define FULLIMAGE	11
#define CARIBBEAN	12
#define REGIONAL	13



/* Number of graphics panels */

#define PANEL_1	0
#define PANEL_2	1
#define PANEL_4	2
#define PANEL_6	3



/* Time buttons. */

#define REALTIME	0
#define BACK_1HR	4
#define BACK_3HR	5
#define BACK_6HR	6
#define BACK_12HR	7
#define BACK_24HR	8
#define FORWARD_1HR	9
#define FORWARD_3HR	10
#define FORWARD_6HR	11
#define FORWARD_12HR	12
#define FORWARD_24HR	13
#define SETCLOCK	1
#define STOPCLOCK	2
#define STARTCLOCK	3



/* Distance units */

#define KILOMETERS	1
#define MILES		2
#define NAUTICAL_MILES	3


/* Temp units (used on status bar) */

#define CELCIUS		1
#define KELVIN		2
#define FAHRENHEIT	3



/* Wind symbol types */

#define ARROW_MS	1
#define ARROW_KT	2
#define ARROW_NONE	3
#define BARB_MS		4
#define BARB_KT		5
#define STREAMLINES	10

#define METERSPERSECOND	1
#define KNOTS		2

#define BARB		1
#define ARROW		2



/* Vertical coordinate types */

#define PRESSURE	1
#define SIGMA		2
#define THETA		3
#define HEIGHT		4



/* Time match types */

#define NOTIMEMATCH	1
#define INTERVAL_ON	2
#define INTERVAL_OFF	3

#define ALL_TIMES	1
#define ANALYSIS_TIME	2
#define DPROG_DT	3



/* Image enhancement tables */

#define VISGRAY         0
#define SATIR           1
#define WATERVAPOR      2



/* Frame widgets */

#define ICONFRAME	0
#define STATUSBARFRAME	1



/* Toggle button values. */

#define ARM	0
#define DISARM	1



/* Information dialogs. */

#define ERROR		0
#define INFORMATION	1
#define QUESTION	2
#define WARNING		3
#define WORKING		4



/* Help Menus */

#define ABOUT_GARP	0
