/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	radarcb.c
 *
 *	Callbacks and utilities used to overlay radar rings.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 8/97	COMET	Removed prototype declaration for MakeImageObjectList
 *
 ***********************************************************************/


#include "winobj.h"
#include "menucb.h"
#include "guimacros.h"
#include "genglobs.h"
#include "background.h"



void
RadarOverlayCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	MetObjectType		*mlist[MAXMETOBJS];
	GuiBackgroundDialogType	*bak;
	WindowObjectType	*wo;
	GlobalDataObjectType	*gd;
	
	char	*text;
	int	index=2;
	int	color;
	int	verbose, iret;

	struct {
	    char	stid[STIDSIZE];
	    char	units[3];
	    BooleanType	display;
	    int		number;
	    float	increment;
	    float	minimum;
	} ring;

	XmPushButtonCallbackStruct *cbs =
	            (XmPushButtonCallbackStruct *) xt_call_data;

	bak = GetGuiBackgroundDialog();

/*
 *	Make sure window object exists.
 */
	wo = GetActiveWindowObject();

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "RadarOverlayCB\n" );
/*
 *	Get radar ring inputs.
 */
	if ( text = XmTextGetString ( GetRringNumberW ( bak ) ) ) {
	    ring.number = atoi ( text );
	    XtFree ( text );
	}

	if ( text = XmTextGetString ( GetRringMinimumW ( bak ) ) ) {
	    ring.minimum = (float) ( atof ( text ) );
	    XtFree ( text );
	}

	if ( text = XmTextGetString ( GetRringIncrementW ( bak ) ) ) {
	    ring.increment = (float) ( atof ( text ) );
	    XtFree ( text );
	}

	if ( text = XmTextGetString ( GetRringStidW ( bak ) ) ) {
	    strcpy ( ring.stid, text );
	    XtFree ( text );
	}

	strcpy ( ring.units, GetRringUnits ( bak ) );

/*
 *	Use the next color
 */
	gd = GetGlobalDataObject();
	GetNextObjectColor ( gd );
	sscanf ( gd->colors, "%d", &color );

/*
 *	Draw radar rings for all stations.
 */
	if ( XmToggleButtonGetState ( GetRringAllstidToggleW ( bak ) ) )
	    strcpy ( ring.stid, "ALL" );
	    
/*
 *	Create object for radar ring(s).
 */
	mlist[0] = (MetObjectType *)
	    MakeRadarRingObject ( index, ring.number, ring.increment,
	                          ring.minimum, ring.units, ring.stid,
	                          color, &iret );

	CopyObjectListToDisplay ( wo, 1, mlist );

	if( wo )
	    ProcessDisplayList( wo );

}


void
RringUnitsCB (Widget w, XtPointer client_data, XtPointer call_data )
{
/*
 * Set distance units for drawing radar rings.
 */
	GuiBackgroundDialogType	*bak;
	int			units;

	XmPushButtonCallbackStruct *cbs = 
		(XmPushButtonCallbackStruct *) call_data;

	units = (int) client_data;

	bak = GetGuiBackgroundDialog();

	FreeRringUnits ( bak );
	switch (units) {
	case KILOMETERS:
	  SetRringUnits ( bak, "km" );
	  break;
	case MILES:
	  SetRringUnits ( bak, "mi" );
	  break;
	case NAUTICAL_MILES:
	  SetRringUnits ( bak, "nm" );
	  break;
	default:
	  break;
	}
}

void RadarObjectCB( Widget  w,
               XtPointer   clientData,
               XtPointer   callData)
{

    XmFileSelectionBoxCallbackStruct *cbs =
        ( XmFileSelectionBoxCallbackStruct *) callData;

/*	int			timestamp; */
	char			**fv;
	int			fc;
	int			i;
	
	MetObjectType		*mot;
	PixmapObjectType	*pot;
	WindowObjectType	*wo;


        char *fileName;
        char *junkfv[1];

        fv = junkfv;

        printf("RadarObjectCB:  NYI\n");
        XtUnmanageChild ( w );
	return;


#if 0
/*
 *	Convert the list widget selection box to file argv type array.
 */
        XmStringGetLtoR ( cbs->value, XmFONTLIST_DEFAULT_TAG, &fileName);

	junkfv[0] = fileName;
	fc = 1;
    
        printf("Selected %s\n", fileName);

	if( fc <= 0 ) {
		printf("RadarObjectCB: returns 0 filenames\n");
		return;
	}

	wo = GetActiveWindowObject();

/*
 *	For imagery, clear out everthing.
 */
	ClearAllPots( wo );

	for( i = 0; i < fc; i++ ) {

/*
 *		Check to for user interrupt.
 */
		if( GetWindowObjectAbortFlag( wo ) )
			break;

/*
 *		File name to timestamp conversion.
 */
/*		timestamp = FileToTimestamp( RADAROBJECT, fv[i] ); */

		pot = GetNextPot( wo );
		if( !pot ) {
			printf("RadarObjectCB: Pot allocation error.\n");
			return;
		}

		mot = GetNextMot( pot );
		if( !mot ) {
			printf("RadarObjectCB: Mot allocation error.\n");
			return;
		}

		strcpy( mot->proj, "RAD" );
		strcpy( mot->filename, fv[i] );
		strcpy( mot->garea, "DSET");

/*		mot->timestamp        = timestamp; */
		mot->drawnFlag        = False;
		mot->inUse            = True;
		mot->type             = RADAROBJECT;
		mot->displayFunc      = DisplayRadar;
		mot->destroyFunc      = NULL;

	} /*   for(;;)   */

	ProcessDisplayList( wo );
	
	return;
#endif /* 0 - nyi */

}
