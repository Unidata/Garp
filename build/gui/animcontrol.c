/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	animcontrol.c
 *
 *	Contains functions that control animations from the GUI.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 2/97	COMET	Chanegd internal min value to 0, changed
 *			dwell slider/speed slider linkage
 *	 3/97	COMET	Added verbose output
 *	 5/97	COMET	Fixed bug in dwell value readings (Chiz/UNIDATA)
 *	 1/98	COMET	Added sanity tests for undefined keys.
 *
 ***********************************************************************/


#include "guimacros.h"
#include "winobj.h"
#include "genglobs.h"
#include "framecount.h"
#include "_proto.h"


/*
 *  Default dwell values for animation sliders (milliseconds)
 */
#define DWELLFIRST	 600
#define DWELLMIDDLE	 100
#define DWELLLAST	 500
#define DWELLMIN	   0
#define DWELLMAX	3000
#define NODWELL		-999

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#define max(a, b)           ((a) < (b) ? (b) : (a))
#define min(a, b)           ((a) > (b) ? (b) : (a))


void	SetSpeedScaleWidgetValue( int value );
void	SetDwellFirstScaleWidgetValue( int value );
void	SetDwellMiddleScaleWidgetValue( int value );
void	SetDwellLastScaleWidgetValue( int value );
void	GetPredefinedDwellValues ( int button, int *, int *, int * );
void	InitDwellDefaults ();
void	GetDwellValues ( char *, int *, int *, int * );
int	GetDwellValue ( char * );
int	GetDwellFirstValue();
int	GetDwellMiddleValue();
int	GetDwellLastValue();
int	GetDwellMinValue();
int	GetDwellMaxValue();
int	GetSpeedMinValue();
int	GetSpeedMaxValue();
int	GetSpeedValue();

static Widget	animSpeedScaleWidget;

/*
 *	Current dwell settings in milliseconds. These are the values
 *	used by the animation object and represented on the actual
 *	slider widget.
 */
int	currentDwellFirst;
int	currentDwellMiddle;
int	currentDwellLast;
int	currentSpeed;

/*
 *	Max and min values for the dwell sliders
 */
int	currentDwellMin;
int	currentDwellMax;

/*
 *	Internal dwell settings, used for keeping the speed slider and 
 *	dwell sliders in sync. These can go beyond min and max to stay
 *	in sync with the speed slider. The currentDwell* ones cannot.
 */
int	syncedDwellFirst;
int	syncedDwellMiddle;
int	syncedDwellLast;
	
	
/*
 *	ConvertToLinear and ConvertToLinear convert their args
 *	to the opposite scale. Internally, the speed values are
 *	all on linear scales, but the speed slider is displayed on
 *	a non-linear scale. This is to give better control over the
 *	fast-speed area of the slider.
 */

int	ConvertToLinear ( int in )
{
	float	fin, fmin, fmax; 
	int	out;


	fin  = in;
	fmin = currentDwellMin;
	fmax = currentDwellMax;
	
	out = (int) ((( fin - fmin) * ( fin - fmin)/( fmax - fmin )) + fmin + .5);

	return ( out );
	
}


int	ConvertToNonLinear ( int in )
{
	float	fin, fmin, fmax; 
	int	out;


	fin  = in;
	fmin = currentDwellMin;
	fmax = currentDwellMax;

	out = (int) (sqrt ( ( fin - fmin ) * ( fmax - fmin ) ) + fmin + .5);

	return ( out );
	
}

/*
 *	InitDwellDefaults(): Initializes all the internal default dwell
 *	settings. Priority is from the GARP_defaults file, if they
 *	are missing or invalid, then internal defaults are used.
 */

void   InitDwellDefaults ()
{

	int	verbose;

	currentDwellMin = GetDwellValue ( "dwellmin" );
	if ( currentDwellMin == NODWELL ) currentDwellMin = DWELLMIN;
	currentDwellMax = GetDwellValue ( "dwellmax" );
	if ( currentDwellMax == NODWELL ) currentDwellMax = DWELLMAX;
	if ( currentDwellMin >= currentDwellMax ) {
		printf("Bad min (%f)", currentDwellMin/1000. );
		printf(" or max (%f) dwell value(s)\n", currentDwellMax/1000.);
	}
	GetDwellValues ( "defaultdwell", &currentDwellFirst, &currentDwellMiddle,
			&currentDwellLast);
	if ( currentDwellFirst == NODWELL ) currentDwellFirst = DWELLFIRST;
	if ( currentDwellMiddle == NODWELL ) currentDwellMiddle = DWELLMIDDLE;
	if ( currentDwellLast  == NODWELL ) currentDwellLast = DWELLLAST;
	syncedDwellFirst  = currentDwellFirst;
	syncedDwellMiddle = currentDwellMiddle;
	syncedDwellLast   = currentDwellLast;

	currentSpeed = GetSpeedValue ();

	verbose = GetVerboseLevel ();
	if ( verbose > VERBOSE_0 ) {
	    printf ("Dwell min set to: %f\n", currentDwellMin/1000.);
	    printf ("Dwell max set to: %f\n",currentDwellMax/1000.);
	    printf ("Dwell first set to: %f\n",currentDwellFirst/1000.);
	    printf ("Dwell middle set to: %f\n",currentDwellMiddle/1000.);
	    printf ("Dwell last set to: %f\n",currentDwellLast/1000.);
	}
	
}


int  GetDwellValue ( char *token )
{
	char	*string;
	float	val;
	int	dwell;

	string = GetConfigValue ( token );

	dwell = NODWELL;

	if ( string != NULL ) {
	    sscanf ( string, "%f", &val);
	    free (string);
	    if ( val <= 0 ) 
		printf("Invalid dwell value (%f). Using default.\n",val);
	    else
		dwell = (int) (val * 1000);
	}

	return (dwell);
}


void  GetDwellValues( char *token , int *one, int *two, int *three)
{
	char	*string;
	char	**dwells;
	float	idwell[3];
	int	count, i;

	*one   = NODWELL;
	*two   = NODWELL;
	*three = NODWELL;

	string = GetConfigValue ( token );
	dwells = SplitByDelimeter ( string, ",", &count );

	if ( count == 3 ) {

	    for ( i = 0; i < count; i++ ) {

		idwell [i] = (int) (atof ( dwells [i] ) * 1000.);

		if ( idwell [i] < (float) (currentDwellMin) ||
		     idwell [i] > (float) (currentDwellMax)) {
		    printf("Invalid dwell value: %s\n",dwells[i]);
		    idwell [i] = NODWELL ;
		}
	    }
	    *one   = (int) idwell [0];
	    *two   = (int) idwell [1];
	    *three = (int) idwell [2];
	}
	else {
	    printf("Invalid number of items for %s: \n", token );
	    if ( string != NULL ) printf(" %s\n",string);
	}

	if ( string != NULL ) Free (string);
	StringListFree ( count, dwells );

}



void   SetSpeedScaleWidgetValue( int value )
{
	Widget			w;
	GuiFrameCounterType	*frm;


        frm = GetGuiFrameCounter();

	w = GetSpeedScaleW( frm);
	if( ! w ) {
		printf("SetSpeedScaleWidgetValue: Widget not set.\n");
		return;
	}

	XtVaSetValues ( w, XmNvalue, value, NULL );

}


void   SetDwellFirstScaleWidgetValue( int value )
{
	Widget			w;
	GuiFrameCounterType	*frm;


        frm = GetGuiFrameCounter();

	w = GetDwellFirstW(frm);
	if( ! w ) {
		printf("SetDwellFirstScaleWidgetValue: Widget not set.\n");
		return;
	}

	XtVaSetValues ( w, XmNvalue, value, NULL );

}


void   SetDwellMiddleScaleWidgetValue( int value )
{
	Widget			w;
	GuiFrameCounterType	*frm;


        frm = GetGuiFrameCounter();

	w = GetDwellMiddleW(frm);
	if( ! w ) {
		printf("SetDwellMiddleScaleWidgetValue: Widget not set.\n");
		return;
	}

	XtVaSetValues ( w, XmNvalue, value, NULL );

}


void   SetDwellLastScaleWidgetValue( int value )
{
	Widget			w;
	GuiFrameCounterType	*frm;


        frm = GetGuiFrameCounter();

	w = GetDwellLastW(frm);
	if( ! w ) {
		printf("SetDwellLastScaleWidgetValue: Widget not set.\n");
		return;
	}

	XtVaSetValues ( w, XmNvalue, value, NULL );

}



/*
 * These routines are called from the main gui startup routine, gui.c.
 * They are used to get default values for the widgets. For the dwell
 * sliders they just return the current min or max. For the speed slider,
 * it returns the sum of the mins or maxes or current dwell settings.
 */

int GetDwellMinValue( )
{
	return( currentDwellMin );
}

int GetDwellMaxValue( )
{
	return( currentDwellMax );
}

int GetDwellFirstValue( )
{
	return( currentDwellFirst );
}

int GetDwellMiddleValue()
{
	return( currentDwellMiddle );
}

int GetDwellLastValue()
{
	return( currentDwellLast );
}

int GetSpeedMinValue( )
{
	return( currentDwellMin );
}

int GetSpeedMaxValue( )
{
	return( currentDwellMax );
}
	
int GetSpeedValue( )
{
	return ((int)((currentDwellFirst + 
	         currentDwellMiddle + 
	         currentDwellLast)/3.));
}


	
void   SetAnimSpeedScaleWidget( Widget w )
{
	animSpeedScaleWidget = w;
}


void GetPredefinedDwellValues ( int button, int *first, int *middle, int *last )
{

	char	name [20];


	sprintf ( name, "presetdwell%d", button );

	GetDwellValues ( name, first, middle, last);

	if ( *first  == NODWELL ) *first  = DWELLFIRST;
	if ( *middle == NODWELL ) *middle = DWELLMIDDLE;
	if ( *last   == NODWELL ) *last   = DWELLLAST;
	
}


void
PreDefinedDwellCB  (Widget w, XtPointer client_data, XtPointer xt_call_data )
{

	WindowObjectType	*wo;
	int			verbose;
	int			button, speed;
	int			first, middle, last;
	float 			buf;

        XmPushButtonCallbackStruct *cbs =
	            (XmPushButtonCallbackStruct *) xt_call_data;


	button = (int) client_data;

	wo = GetActiveWindowObject();

	if( ! wo  ) return;

/*
 *	Get the speed values
 */
	GetPredefinedDwellValues ( button, &first, &middle, &last );

	verbose = GetVerboseLevel ();
	if ( verbose > VERBOSE_0 ) {
	    printf ("Dwell #%d first set to: ", button );
	    printf ("%.2f\n", first/1000.);
	    printf ("Dwell #%d middle set to: ", button );
	    printf ("%.2f\n", middle/1000.);
	    printf ("Dwell #%d last set to: ", button );
	    printf ("%.2f\n", last/1000.);
	}
/*
 *	Save the new values
 */
	currentDwellFirst  = first;
	currentDwellMiddle = middle;
	currentDwellLast   = last;
	syncedDwellFirst   = currentDwellFirst;
	syncedDwellMiddle  = currentDwellMiddle;
	syncedDwellLast    = currentDwellLast;
	currentSpeed       = GetSpeedValue ();

/*
 *	Set all the widgets to the new values. Values are divided by
 *	10 to get the right units displayed on the slider scale.
 */
	SetDwellFirstScaleWidgetValue( first/10 );
	SetDwellMiddleScaleWidgetValue( middle/10 );
	SetDwellLastScaleWidgetValue( last/10 );

	speed = ConvertToNonLinear ( currentSpeed );
	SetSpeedScaleWidgetValue( speed/10 );

/*
 *	Set the values in the animationObject
 */
	if ( wo->animationObject != NULL ) {	

	    wo->animationObject->dwellFirst  = first;
	    wo->animationObject->dwellMiddle = middle;
	    wo->animationObject->dwellLast   = last;

	}

}


void
SpeedScaleCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	WindowObjectType	*wo;
	int			ivalue, value;
	int			change, speed;
	
        XmScaleCallbackStruct *cbs = 
	                      (XmScaleCallbackStruct *) xt_call_data;

	wo = GetActiveWindowObject();

	if( ! wo  ) return;

	value = cbs->value * 10;

/*
 *	Convert it to the internal linear scale
 */
	ivalue = ConvertToLinear ( value );

	change = currentSpeed - ivalue ;

/*
 *	Get the new slider values. They will move together as the
 *	speed slider moves. Allow the sliders to go past min or max
 *	so that they can all be pegged, but retain relative positions.
 */	
	syncedDwellFirst  = syncedDwellFirst  - change;
	syncedDwellMiddle = syncedDwellMiddle - change;
	syncedDwellLast   = syncedDwellLast   - change;

/*
 *	Get the slider values between the allowable limits. These are
 *	used for the slider widgets and the animation objects dwells.
 */
	currentDwellFirst  = max ( syncedDwellFirst,   currentDwellMin );
	currentDwellFirst  = min ( currentDwellFirst,  currentDwellMax );
	currentDwellMiddle = max ( syncedDwellMiddle,  currentDwellMin );
	currentDwellMiddle = min ( currentDwellMiddle, currentDwellMax );
	currentDwellLast   = max ( syncedDwellLast,    currentDwellMin );
	currentDwellLast   = min ( currentDwellLast,   currentDwellMax );
	currentSpeed       = GetSpeedValue ();

	SetDwellFirstScaleWidgetValue(  currentDwellFirst/10 );
	SetDwellMiddleScaleWidgetValue( currentDwellMiddle/10 );
	SetDwellLastScaleWidgetValue(   currentDwellLast/10 );

	speed = ConvertToNonLinear ( currentSpeed );
	SetSpeedScaleWidgetValue(    speed/10 );

	if ( wo->animationObject != NULL ) {
	
		wo->animationObject->dwellFirst  = currentDwellFirst;
		wo->animationObject->dwellMiddle = currentDwellMiddle;
		wo->animationObject->dwellLast   = currentDwellLast;
	}


	return;

}


void
DwellFirstCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{

	WindowObjectType	*wo;
	int			value, speed;

        XmScaleCallbackStruct *cbs = 
	                      (XmScaleCallbackStruct *) xt_call_data;


	wo = GetActiveWindowObject();

	if ( ! wo  ) return;
	
	value = cbs->value;

	currentDwellFirst = value * 10;
	syncedDwellFirst  = currentDwellFirst;

	if ( wo->animationObject != NULL )
	
		wo->animationObject->dwellFirst = currentDwellFirst;

/*
 *	Adjust the "speed" slider. Display the speed slider value 
 *	in a non-linear scale.
 */
	currentSpeed = GetSpeedValue ();
	speed = ConvertToNonLinear ( currentSpeed );

	SetSpeedScaleWidgetValue( speed/10 );

	return;
}


void
DwellMiddleCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	WindowObjectType	*wo;
	int			value, speed;

        XmScaleCallbackStruct *cbs = 
	                      (XmScaleCallbackStruct *) xt_call_data;


	wo = GetActiveWindowObject();

	if ( ! wo ) return;
	
	value = cbs->value;

	currentDwellMiddle = value * 10;
	syncedDwellMiddle  = currentDwellMiddle;

	if ( wo->animationObject != NULL )
	
		wo->animationObject->dwellMiddle = currentDwellMiddle;

/*
 *	Adjust the "speed" slider. Display the speed slider value 
 *	in a non-linear scale.
 */
	currentSpeed = GetSpeedValue ();
	speed = ConvertToNonLinear ( currentSpeed );

	SetSpeedScaleWidgetValue( speed/10 );

	return;
}


void
DwellLastCB (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	WindowObjectType	*wo;
	int			value, speed;

        XmScaleCallbackStruct *cbs = 
	                      (XmScaleCallbackStruct *) xt_call_data;


	wo = GetActiveWindowObject();

	if( !  wo  ) return;
	
	value = cbs->value;

	currentDwellLast = value * 10;
	syncedDwellLast  = currentDwellLast;

	if ( wo->animationObject != NULL )
	
		wo->animationObject->dwellLast = currentDwellLast;

/*
 *	Adjust the "speed" slider. Display the speed slider value 
 *	in a non-linear scale.
 */
	currentSpeed = GetSpeedValue ();
	speed = ConvertToNonLinear ( currentSpeed );

	SetSpeedScaleWidgetValue( speed/10  );

	return;
}


void 
animation_cb  (Widget w, XtPointer client_data, XtPointer xt_call_data )
{
	XmPushButtonCallbackStruct *cbs = 
	            (XmPushButtonCallbackStruct *) xt_call_data;

	WindowObjectType	*wo;

	int animateButton = (int) client_data;

#ifdef DEBUG
	printf("animation_cb - animateButton = %d\n", animateButton ); 
#endif

        wo = GetActiveWindowObject();

	if( ! wo ) 
		return;


	switch( animateButton ) {
	case STOPLOOP:
		AnimationObjectStop( wo );
		break;

	case LOOPFORWARD:

#if DEBUG
		printf("is running is %d\n", AnimationRunning( wo ));
		printf("command is %d\n", GetAnimationCommand( wo ));
#endif

		if( AnimationRunning( wo ) && 
				GetAnimationCommand( wo ) == LOOPFORWARD ) {
			AnimationObjectStop( wo );
			break;
		}
				
		AnimationObjectPlayFwd( wo );
		break;

	case LOOPBACKWARD:
		if( AnimationRunning( wo ) && 
				GetAnimationCommand( wo ) == LOOPBACKWARD ) {
			AnimationObjectStop( wo );
			break;
		}
		AnimationObjectPlayBackward( wo );
		break;

	case STEPFORWARD:
		AnimationObjectStepFwd( wo );
		break;

	case LOOPFORWARDBACKWARD:
		if( AnimationRunning( wo ) && 
			GetAnimationCommand( wo ) == LOOPFORWARDBACKWARD ) {
				AnimationObjectStop( wo );
			break;
		}
		AnimationObjectPlayFwdBack( wo );
		break;

	case STEPBACKWARD:
		AnimationObjectStepBack( wo );
		break;

	default:
		break;

	}   /* switch() */

	return;


}

