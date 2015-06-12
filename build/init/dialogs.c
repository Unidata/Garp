/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	dialogs.c
 *
 *	GUI dialog initialization.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/97	COMET	Added upper air profile object.
 *	 2/97	COMET	Replaced "free" with Free macro to insure memory 
 *			sanity.
 *	 3/97	COMET	Added keys to define time axis buttons for 
 *			Time/height dialog.
 *	 3/97	COMET	Added station parameter option menus to 
 *			background object.
 *       4/97   COMET   Added line type to upper air vertical profile.
 *	 5/97	COMET	Added window preference initialization routine.
 *	 5/97	COMET	Added status bar preference initilization.
 *	 7/97	COMET	Dynamically initialize surface station parameters.
 *	 7/97	COMET	Added model defined subdirectories.
 *	11/97	COMET	Fixed small leak setting default axis type.
 *	11/97	COMET	Save the model label to use in titleObject.
 *	11/97	COMET	Fixed some small leaks
 *	11/97	COMET	Added call to InitTimeMatchDialog().
 *	 1/98	COMET	Added sanity tests for undefined keys.
 *	 2/98	COMET	Added code to filter model list
 *	10/98	COMET	Added code to initialize wind profiler widgets.
 *	 4/99   COMET   Removed redundant calls to SetDefaultOptionButton.
 *	 5/99   COMET   Reworked FDF default directory definitions.
 *	 8/99	COMET	Redefined FDF model directory.
 *	12/99	COMET	Replace datemake() function with sec2yyyymmddhhmm().
 *
 ***********************************************************************/
#include <stdio.h>

#include "mainwin.h"
#include "background.h"
#include "hsobj.h"
#include "hvobj.h"
#include "xsecobj.h"
#include "thgtobj.h"
#include "vprofobj.h"
#include "sfcobj.h"
#include "upaobj.h"
#include "upaprof.h"
#include "wprofobj.h"
#include "model.h"
#include "gendef.h"
#include "genglobs.h"
#include "guimacros.h"
#include "winobj.h"
#include "command.h"
#include "framecount.h"
#include "fdfobj.h"
#include "winpref.h"
#include "_proto.h"


/*
 * Prototypes for dynamically built widgets.
 */
void HSModelButtonCB ( Widget, XtPointer, XtPointer );	  /* Plan view */
void HSVcoordButtonCB ( Widget, XtPointer, XtPointer );
void HSGridFillTableCB ( Widget, XtPointer, XtPointer );
void HSScalarFdfDirCB ( Widget, XtPointer, XtPointer );
void HSVectorFdfDirCB ( Widget, XtPointer, XtPointer );
void HSWindSymbolCB ( Widget, XtPointer, XtPointer );
void HSLineTypeCB ( Widget, XtPointer, XtPointer );
void HSLineLabelFrequencyCB ( Widget, XtPointer, XtPointer );

void XSModelButtonCB ( Widget, XtPointer, XtPointer );	  /* Cross section */
void XSVcoordButtonCB ( Widget, XtPointer, XtPointer );
void XSScalarFdfDirCB ( Widget, XtPointer, XtPointer );
void XSVectorFdfDirCB ( Widget, XtPointer, XtPointer );
void XSGridFillTableCB ( Widget, XtPointer, XtPointer );
void XSLineTypeCB ( Widget, XtPointer, XtPointer );
void XSLineLabelFrequencyCB ( Widget, XtPointer, XtPointer );
void XSWindSymbolCB ( Widget, XtPointer, XtPointer );
void XSVerticalAxisCB ( Widget, XtPointer, XtPointer );

void THModelButtonCB ( Widget, XtPointer, XtPointer );    /* Time/height */
void THVcoordButtonCB ( Widget, XtPointer, XtPointer );
void THScalarFdfDirCB ( Widget, XtPointer, XtPointer );
void THVectorFdfDirCB ( Widget, XtPointer, XtPointer );
void THLineTypeCB ( Widget, XtPointer, XtPointer );
void THLineLabelFrequencyCB ( Widget, XtPointer, XtPointer );
void THWindSymbolCB ( Widget, XtPointer, XtPointer );
void THVerticalAxisCB ( Widget, XtPointer, XtPointer );
void THTimeAxisCB ( Widget, XtPointer, XtPointer );

void VPModelButtonCB ( Widget, XtPointer, XtPointer );    /* Vertical profile */
void VPVcoordButtonCB ( Widget, XtPointer, XtPointer );
void VPScalarFdfDirCB ( Widget, XtPointer, XtPointer );
void VPVectorFdfDirCB ( Widget, XtPointer, XtPointer );
void VPLineTypeCB ( Widget, XtPointer, XtPointer );
void VPWindSymbolCB ( Widget, XtPointer, XtPointer );
void VPVerticalAxisCB ( Widget, XtPointer, XtPointer );

void UFLineTypeCB ( Widget, XtPointer, XtPointer );

void MWContourMapButtonCB ( Widget, XtPointer, XtPointer );
void MWGareaButtonCB ( Widget, XtPointer, XtPointer );
void MWUserSetGareaCB ( Widget, XtPointer, XtPointer );
void MWSetPreviousGareaCB ( Widget, XtPointer, XtPointer ); 

void SFPlotTypeCB ( Widget, XtPointer, XtPointer );
void SFMarkerTypeCB ( Widget, XtPointer, XtPointer );
void InitModelObject ( int * );
void InitStationOverlayDialog ();

char * GetStringArrayLabel ( int, char *, char **, char ** );

void InitGuiUpperAirDialog();
void InitWindProfilerDialog();
void InitGuiBackgroundDialog ( int *iret );
void InitGuiUpaProfileDialog ( char **linelabels, char **line, int numlinekeys );
void InitGuiHorizontalGridDialog (
		char **mlabels, char **models, int nummodelkeys,
		char **vlabels, char **vcoord, int numvcoordkeys,
		char **clabels, char **colors, int numcolorkeys,
		char **sclabels, char **scalarchapter, int numscalarkeys,
		char **vclabels, char **vectorchapter, int numvectorkeys,
		char **freqlabels, char **freqkeys, int numfreqkeys,
		char **linelabels, char **line, int numlinekeys );
void InitGuiVerticalProfileDialog (
	char **mlabels, char **models, int nummodelkeys,
	char **vlabels, char **vcoord, int numvcoordkeys,
	char **vpsclabels, char **vpscalarchapter, int numvpscalarkeys,
	char **vpvclabels, char **vpvectorchapter, int numvpvectorkeys,
	char **freqlabels, char **freqkeys, int numfreqkeys,
	char **linelabels, char **line, int numlinekeys,
	char **yaxislabels, char **yaxis, int numyaxiskeys );
void InitGuiTimeHeightDialog (
	char **mlabels, char **models, int nummodelkeys,
	char **vlabels, char **vcoord, int numvcoordkeys,
	char **thsclabels, char **thscalarchapter, int numthscalarkeys,
	char **thvclabels, char **thvectorchapter, int numthvectorkeys,
	char **freqlabels, char **freqkeys, int numfreqkeys,
	char **linelabels, char **line, int numlinekeys,
	char **yaxislabels, char **yaxis, int numyaxiskeys,
	char **taxislabels, char **taxis, int numtaxiskeys );
int SetDefaultOptionButton ( Widget option_menu,
                         int numkeys,
                         char *key,
                         char **keylist,
                         ButtonParentObjectType *bpo,
                         char *keylistString );
void BuildOptionMenuButtons ( Widget pm,
                         char **labels, char **strings, int labelc,
                         void (*func)(),
                         ButtonParentObjectType *bpo );
void InitGuiMainDialog ( char **clabels, char **colormaps, int nummapkeys,
                    char **glabels, char **gareas, int numgareakeys );

void InitWinPrefDialog ( int *iret );

void InitGuiVCSDialog (
	char **mlabels, char **models, int nummodelkeys,
	char **vlabels, char **vcoord, int numvcoordkeys,
	char **clabels, char **colors, int numcolorkeys,
	char **xssclabels, char **xsscalarchapter, int numxsscalarkeys,
	char **xsvclabels, char **xsvectorchapter, int numxsvectorkeys,
	char **freqlabels, char **freqkeys, int numfreqkeys,
	char **linelabels, char **line, int numlinekeys,
	char **yaxislabels, char **yaxis, int numyaxiskeys );

void BuildProfilerList ( int nfiles, ListParentObjectType *list_struct );

void
InitDialogs ( int *iret )
/*
 *	Initialize widgets values for dialogs. These widgets are the minimal
 *	set required to generate feedback to dynamically initialize the
 *	remaining widgets in that dialog.
 */
{
   
	char		**yaxis=NULL,           **yaxislabels=NULL,
			**taxis=NULL,           **taxislabels=NULL,
			**vpyaxis=NULL,         **vpyaxislabels=NULL,
			**models=NULL,          **mlabels=NULL,
			**vcoord=NULL,          **vlabels=NULL,
			**vpvcoord=NULL,        **vpvlabels=NULL,
			**gareas=NULL,          **glabels=NULL,
	                **projs=NULL,
			**colors=NULL,          **clabels=NULL,
			**colormaps=NULL,       **cmlabels=NULL,
			**scalarchapter=NULL,   **sclabels=NULL,
			**vectorchapter=NULL,   **vclabels=NULL,
			**xsscalarchapter=NULL, **xssclabels=NULL,
			**xsvectorchapter=NULL, **xsvclabels=NULL,
			**thscalarchapter=NULL, **thsclabels=NULL,
			**thvectorchapter=NULL, **thvclabels=NULL,
			**vpscalarchapter=NULL, **vpsclabels=NULL,
			**vpvectorchapter=NULL, **vpvclabels=NULL,
			**freqkeys=NULL,        **freqlabels=NULL,
			**linelabels=NULL,      **line=NULL;

	int		nummodelkeys    , numvcoordkeys  , nummapkeys,
			numcolorfillkeys, numgareakeys   ,
			numscalarkeys   , numvectorkeys  ,
			numxsscalarkeys , numxsvectorkeys, numfreqkeys,
			numthscalarkeys , numthvectorkeys,
			numvpscalarkeys , numvpvectorkeys,
			numlinekeys     , numyaxiskeys   , numtaxiskeys,
			numvpvcoordkeys , numvpyaxiskeys;

	*iret = NORMAL;

/*
 *	Set model object according to key/value pairs in configuration file.
 */
	InitModelObject ( iret );

/*
 *	Get Model information. Save lists for models and model labels.
 */
	*iret = GetKeyList ( "modelkeys", "modellabels", ",",
                                     &nummodelkeys, &models, &mlabels );
	if ( *iret < -1 ) return;

	GetFilteredModelList ( &nummodelkeys, &models, &mlabels );

/*
 *	Get Vertical Coordinate information. Save lists for coordinates
 *	and labels.
 */
	*iret = GetKeyList ( "vcoordkeys", "vcoordlabels", ",",
                                     &numvcoordkeys, &vcoord, &vlabels );
	if ( *iret < -1 ) return;


	*iret = GetKeyList ( "vpvcoordkeys", "vpvcoordlabels", ",",
                                     &numvpvcoordkeys, &vpvcoord, &vpvlabels );
	if ( *iret < -1 ) return;

/*
 *	Get geographic map list and labels.
 */
	*iret = GetKeyList ( "MapGarea", "MapLabels", ",",
                                     &numgareakeys, &gareas, &glabels );
	if ( *iret < -1 ) return;

	StringListFree ( numgareakeys, gareas );
	*iret = GetKeyList ( "MapGarea", "MapProjection", ",",
                                     &numgareakeys, &gareas, &projs );
	if ( *iret < -1 ) return;

/*
 *	Get color fill list and labels.
 */
	*iret = GetKeyList ( "colorfillkeys", "colorfilllabels", ",",
                                     &numcolorfillkeys, &colors, &clabels );
	if ( *iret < -1 ) return;

/*
 *	Get colored contour map labels and file names.
 */
	*iret = GetKeyList ( "colormapkeys", "colormaplabels", ",",
                                     &nummapkeys, &colormaps, &cmlabels );
	if ( *iret < -1 ) return;

/*
 *	Get FDF chapters.
 */
	*iret = GetKeyList ( "ScalarChapter", "SCLabels", ",",
                                   &numscalarkeys, &scalarchapter, &sclabels );
	if ( *iret < -1 ) return;


	*iret = GetKeyList ( "VectorChapter", "VCLabels", ",",
                                   &numvectorkeys, &vectorchapter, &vclabels );
	if ( *iret < -1 ) return;


	*iret = GetKeyList ( "XSScalarChapter", "XSSCLabels", ",",
                             &numxsscalarkeys, &xsscalarchapter, &xssclabels );
	if ( *iret < -1 ) return;


	*iret = GetKeyList ( "XSVectorChapter", "XSVCLabels", ",",
                             &numxsvectorkeys, &xsvectorchapter, &xsvclabels );
	if ( *iret < -1 ) return;


	*iret = GetKeyList ( "THScalarChapter", "THSCLabels", ",",
                             &numthscalarkeys, &thscalarchapter, &thsclabels );
	if ( *iret < -1 ) return;


	*iret = GetKeyList ( "THVectorChapter", "THVCLabels", ",",
                             &numthvectorkeys, &thvectorchapter, &thvclabels );
	if ( *iret < -1 ) return;


	*iret = GetKeyList ( "VPScalarChapter", "VPSCLabels", ",",
                             &numvpscalarkeys, &vpscalarchapter, &vpsclabels );
	if ( *iret < -1 ) return;


	*iret = GetKeyList ( "VPVectorChapter", "VPVCLabels", ",",
                             &numvpvectorkeys, &vpvectorchapter, &vpvclabels );
	if ( *iret < -1 ) return;

/*
 *	Get line type keys.
 */
	*iret = GetKeyList ( "linetypekeys", "linetype", ",",
                                     &numlinekeys, &line, &linelabels );
	if ( *iret < -1 ) return;

/*
 *	Get line label frequency keys.
 */
	*iret = GetKeyList ( "labelfreqkeys", "labelfreq", ",",
                                     &numfreqkeys, &freqkeys, &freqlabels );
	if ( *iret < -1 ) return;

/*
 *	Get vertical axis keys.
 */
	*iret = GetKeyList ( "yaxiskeys", "yaxislabels", ",",
                                     &numyaxiskeys, &yaxis, &yaxislabels );
	if ( *iret < -1 ) return;


	*iret = GetKeyList ( "vpyaxiskeys", "vpyaxislabels", ",",
                                   &numvpyaxiskeys, &vpyaxis, &vpyaxislabels );
	if ( *iret < -1 ) return;

/*
 *	Get time axis keys.
 */
	*iret = GetKeyList ( "thtaxiskeys", "thtaxislabels", ",",
                                     &numtaxiskeys, &taxis, &taxislabels );
	if ( *iret < -1 ) return;

/*
 *	Main dialog.
 */
	InitGuiMainDialog ( cmlabels, colormaps, nummapkeys,
			    glabels, gareas, numgareakeys );

/*
 *	Background objects dialog.
 */
	InitGuiBackgroundDialog ( iret );


/*
 *	Window preference dialog(s).
 */
	InitWinPrefDialog ( iret );
	
	
/*
 *	Status bar preferences dialog.
 */
	InitStatusBarPrefDialog ( );


/*
 *	Set time match dialog.
 */
	InitTimeMatchDialog();


/*
 *	Model Grid dialog.
 */
	InitGuiHorizontalGridDialog ( mlabels, models, nummodelkeys,
				      vlabels, vcoord, numvcoordkeys,
				      clabels, colors, numcolorfillkeys,
				      sclabels, scalarchapter, numscalarkeys,
				      vclabels, vectorchapter, numvectorkeys,
				      freqlabels, freqkeys, numfreqkeys,
				      linelabels, line, numlinekeys );
/*
 *	Vertical Cross Section dialog. 
 */
	InitGuiVCSDialog ( mlabels, models, nummodelkeys,
			   vpvlabels, vpvcoord, numvpvcoordkeys,
			   clabels, colors, numcolorfillkeys,
			   xssclabels, xsscalarchapter, numxsscalarkeys,
			   xsvclabels, xsvectorchapter, numxsvectorkeys,
			   freqlabels, freqkeys, numfreqkeys,
			   linelabels, line, numlinekeys,
			   yaxislabels, yaxis, numyaxiskeys );
/*
 *	Time Height dialog.
 */
	InitGuiTimeHeightDialog ( 
			mlabels, models, nummodelkeys,
			vpvlabels, vpvcoord, numvpvcoordkeys,
			thsclabels, thscalarchapter, numthscalarkeys,
			thvclabels, thvectorchapter, numthvectorkeys,
			freqlabels, freqkeys, numfreqkeys,
			linelabels, line, numlinekeys,
			yaxislabels, yaxis, numyaxiskeys,
			taxislabels, taxis, numtaxiskeys );
/*
 *	Vertical Profile dialog.
 */

	InitGuiVerticalProfileDialog ( 
			mlabels, models, nummodelkeys,
			vpvlabels, vpvcoord, numvpvcoordkeys,
			vpsclabels, vpscalarchapter, numvpscalarkeys,
			vpvclabels, vpvectorchapter, numvpvectorkeys,
			freqlabels, freqkeys, numfreqkeys,
			linelabels, line, numlinekeys,
			vpyaxislabels, vpyaxis, numvpyaxiskeys );

/*
 *	Surface Obs dialog.
 */
	InitGuiSurfaceObsDialog();

/*
 *	Upper Air horizontal projection dialog.
 */
	InitGuiUpperAirDialog();

/*
 *	Upper Air vertical projection dialog.
 */
	InitGuiUpaProfileDialog (
			linelabels, line, numlinekeys );

/*
 *	Wind Profiler dialog.
 */
	InitWindProfilerDialog();

/*
 *	Station overlay dialog.
 */
	InitStationOverlayDialog();

/*
 *	Free.
 */
	StringListFree ( numyaxiskeys, yaxislabels );
	StringListFree ( numyaxiskeys, yaxis );
	StringListFree ( numtaxiskeys, taxislabels );
	StringListFree ( numtaxiskeys, taxis );
	StringListFree ( numvpyaxiskeys, vpyaxislabels );
	StringListFree ( numvpyaxiskeys, vpyaxis );
	StringListFree ( nummodelkeys, mlabels );
	StringListFree ( nummodelkeys, models );
	StringListFree ( numvcoordkeys, vlabels );
	StringListFree ( numvcoordkeys, vcoord );
	StringListFree ( numvpvcoordkeys, vpvlabels );
	StringListFree ( numvpvcoordkeys, vpvcoord );
	StringListFree ( nummapkeys, cmlabels );
	StringListFree ( nummapkeys, colormaps );
	StringListFree ( numcolorfillkeys, clabels );
	StringListFree ( numcolorfillkeys, colors );
	StringListFree ( numgareakeys, glabels );
	StringListFree ( numgareakeys, gareas );
	StringListFree ( numgareakeys, projs );
	StringListFree ( numscalarkeys, sclabels );
	StringListFree ( numscalarkeys, scalarchapter );
	StringListFree ( numvectorkeys, vclabels );
	StringListFree ( numvectorkeys, vectorchapter );
	StringListFree ( numxsscalarkeys, xssclabels );
	StringListFree ( numxsscalarkeys, xsscalarchapter );
	StringListFree ( numxsvectorkeys, xsvclabels );
	StringListFree ( numxsvectorkeys, xsvectorchapter );
	StringListFree ( numthscalarkeys, thsclabels );
	StringListFree ( numthscalarkeys, thscalarchapter );
	StringListFree ( numthvectorkeys, thvclabels );
	StringListFree ( numthvectorkeys, thvectorchapter );
	StringListFree ( numvpscalarkeys, vpsclabels );
	StringListFree ( numvpscalarkeys, vpscalarchapter );
	StringListFree ( numvpvectorkeys, vpvclabels );
	StringListFree ( numvpvectorkeys, vpvectorchapter );
	StringListFree ( numfreqkeys, freqlabels );
	StringListFree ( numfreqkeys, freqkeys );
	StringListFree ( numlinekeys, linelabels );
	StringListFree ( numlinekeys, line );
	

	return;
}

void
InitGuiMainDialog ( char **clabels, char **colormaps, int nummapkeys,
		    char **glabels, char **gareas, int numgareakeys )
/*
 * Initialize main window widgets.
 */
{
	GuiMainWinDialogType	*mwn;

	mwn = GetGuiMainWinDialog();

/*
 *	Create option menu buttons for colored maps.
 */
	BuildOptionMenuButtons ( GetContourMapW ( mwn ),
				 clabels, colormaps, nummapkeys,
				 MWContourMapButtonCB,
				 &(mwn->mappm) );
/*
 *	Create option menu buttons for geographic regions.
 */
	BuildOptionMenuButtons ( GetGareaW ( mwn ),
				glabels, gareas, numgareakeys,
				MWGareaButtonCB,
				&(mwn->gareapm) );
/*
 *	Add another button for defining garea, and one for getting
 *	previous area.
 */
	AddButtonToOptionMenu ( "Define", "Define", mwn->gareapm.button_cnt,
			        MWUserSetGareaCB,
			        &(mwn->gareapm) );

	AddButtonToOptionMenu ( "Previous", "Previous", mwn->gareapm.button_cnt,
			        MWSetPreviousGareaCB,
			        &(mwn->gareapm) );
}


void
BKStationTypeCB ( Widget w, XtPointer clientData, XtPointer call_data )
{
/*
 *	Set the station table you want to read.
 */
	GuiBackgroundDialogType	*bak;
	char			*type;

	ButtonObject *bo = (ButtonObject *) clientData;

	type = strdup ( bo->string );

	bak = GetGuiBackgroundDialog();

	FreeStationType ( bak );
	SetStationType ( bak, type );

	Free ( type );
}


void
BKStationParmCB ( Widget w, XtPointer clientData, XtPointer call_data )
{
/*
 * Define what parameter will be displayed from a table file.
 */
	GuiBackgroundDialogType	*bak;
	char			*parm;

	ButtonObject *bo = (ButtonObject *) clientData;

	parm = strdup ( bo->string );

	bak = GetGuiBackgroundDialog();

	FreeStationParm ( bak );
	SetStationParm ( bak, parm );

	Free ( parm );
	return;
}

void
InitWinPrefDialog ( int *iret )
{
	GuiWinPrefDialogType	*wpi;
	int			verbose;
	int			output;

	wpi = GetGuiWinPrefDialog();
	verbose = GetVerboseLevel();
	output = GetOutputLevel();

/*
 *	Set verbose mode radio button.
 */
	switch (verbose) {
	case VERBOSE_0:
	  break;
	case VERBOSE_1:
	  XmToggleButtonSetState ( GetVerboseLevel_0W ( wpi ), FALSE, FALSE );
	  XmToggleButtonSetState ( GetVerboseLevel_1W ( wpi ), TRUE, FALSE );
	  break;
	case VERBOSE_2:
	  XmToggleButtonSetState ( GetVerboseLevel_0W ( wpi ), FALSE, FALSE );
	  XmToggleButtonSetState ( GetVerboseLevel_2W ( wpi ), TRUE, FALSE );
	  break;
	case VERBOSE_3:
	  XmToggleButtonSetState ( GetVerboseLevel_0W ( wpi ), FALSE, FALSE );
	  XmToggleButtonSetState ( GetVerboseLevel_3W ( wpi ), TRUE, FALSE );
	  break;
	default:
	  break;
	}

/*
 *	Set verbose output radio button.
 */
	switch (output) {
	case PRINT_TO_STDOUT:
	  break;
	case PRINT_TO_LOG_WINDOW:
	  XmToggleButtonSetState ( GetGuiStdoutW ( wpi ), FALSE, FALSE );
	  XmToggleButtonSetState ( GetGuiGarpLogW ( wpi ), TRUE, FALSE );
	  break;
	case PRINT_TO_FILE:
	  XmToggleButtonSetState ( GetGuiStdoutW ( wpi ), FALSE, FALSE );
	  XmToggleButtonSetState ( GetGuiGarpFileW ( wpi ), TRUE, FALSE );
	  break;
	default:
	  break;
	}
}

void
InitGuiBackgroundDialog ( int *iret )
{
	GuiBackgroundDialogType	*bak;
	char			*stid;
	char			*keylist, *labellist;
	char			**tables=NULL,   **tlabels=NULL,
				**stnparms=NULL, **stnlabels=NULL;
	int			numtablekeys, numstnparmkeys, numlabelkeys;

	bak = GetGuiBackgroundDialog();

/*
 *	Get Vertical Coordinate information. Save lists for coordinates
 *	and labels.
 */
	*iret = GetKeyList ( "stn_tables", "stn_labels", ",",
                                     &numtablekeys, &tables, &tlabels );
	if ( *iret < -1 ) return;

/*
 *	Get information pertaining to what station parameters may be
 *	displayed.
 */
	keylist   = strdup ("0,1,2,3,4");
	labellist = strdup (
	    "Station ID,Numeric Code,Name,Elevation (m),Elevation (ft)" );

	stnparms  = SplitByDelimeter ( keylist, ",", &numstnparmkeys );
	stnlabels = SplitByDelimeter ( labellist, ",", &numlabelkeys );

	Free ( keylist );
	Free ( labellist );
/*
 *	Initialize radar rings station id widget.
 */
	stid = GetConfigValue( "rring_stid" );;
	
	if ( stid == NULL ) stid = strdup ( "FTG" );

	SetRringStid ( bak, stid );
	XmTextSetString ( GetRringStidW ( bak ),
                                        GetRringStid ( bak ) );

	Free ( stid );
/*
 *	Initialize radar rings distance units.
 */
	SetRringUnits ( bak, "km" );

/*
 *	Create buttons for Vertical Coordinate option menu.
 */
	BuildOptionMenuButtons ( GetStationTypePmW ( bak ),
				tlabels, tables, numtablekeys,
				BKStationTypeCB,
				&(bak->stn_type) );

	XtUnmanageChild ( GetStationTypeOmW ( bak ) );
	SetDefaultOptionButton ( GetStationTypeOmW ( bak ),
				numtablekeys, GetStationType ( bak ),
				tables, &(bak->stn_type), " " );
	XtManageChild ( GetStationTypeOmW ( bak ) );

/*
 *	Create buttons which indicate what parameters may be displayed.
 */
	BuildOptionMenuButtons ( GetStationParmPmW( bak ),
				stnlabels, stnparms, numstnparmkeys,
				BKStationParmCB,
				&(bak->stn_parm) );
	XtUnmanageChild ( GetStationParmOmW ( bak ) );
	SetDefaultOptionButton ( GetStationParmOmW ( bak ),
				numstnparmkeys, GetStationParm ( bak ),
				stnparms, &(bak->stn_parm), " " );
	XtManageChild ( GetStationParmOmW ( bak ) );

/*
 *	Initialize station overlay parameters.
 */

	StringListFree ( numtablekeys, tables );
	StringListFree ( numtablekeys, tlabels );
	StringListFree ( numstnparmkeys, stnparms );
	StringListFree ( numstnparmkeys, stnlabels );
}

void
InitGuiVCSDialog (
	char **mlabels, char **models, int nummodelkeys,
	char **vlabels, char **vcoord, int numvcoordkeys,
	char **clabels, char **colors, int numcolorkeys,
	char **xssclabels, char **xsscalarchapter, int numxsscalarkeys,
	char **xsvclabels, char **xsvectorchapter, int numxsvectorkeys,
	char **freqlabels, char **freqkeys, int numfreqkeys,
	char **linelabels, char **line, int numlinekeys,
	char **yaxislabels, char **yaxis, int numyaxiskeys )
/*
 * Initialize widgets in Vertical Cross Section dialog.
 */
{
	GuiXSectionDialogType	*vcs;
	KeyListType		*fdf;
	ButtonParentObjectType	*spo;
	ButtonObject		*bo;

	XmString		xmstr;
	char			*xsection, *model;
	char			*file, *directory, *subdir;
	char			**symbols, **symlabels;
	
	int			numsymkeys;
	int			i, iret, err;

	vcs = GetGuiXSectionDialog();
	fdf = GetXSGuiFdfInfo();

/*
 *	Get configuration key values.
 */
	xsection = GetConfigValue( "cross-section" );
	model = GetConfigValue( "model" );

	SetXSGridType ( vcs, SCALARGRIDOBJECT );

/*
 *	Create buttons for Model option menu.
 */
	XtUnmanageChild ( GetModelTypeOmXSW ( vcs ) );
	BuildOptionMenuButtons ( GetModelTypePmXSW ( vcs ),
				 mlabels, models, nummodelkeys,
				 XSModelButtonCB,
				 &(vcs->modelpm) );
/*
 *	Set default Model button.
 */
	SetDefaultOptionButton ( GetModelTypeOmXSW ( vcs ),
				 nummodelkeys, model, models,
				 &(vcs->modelpm), " " );
	XtManageChild ( GetModelTypeOmXSW ( vcs ) );

/*
 *	Create buttons for Vertical Coordinate option menu.
 */
	BuildOptionMenuButtons ( GetModelVCoordPmXSW ( vcs ),
				vlabels, vcoord, numvcoordkeys,
				XSVcoordButtonCB,
				&(vcs->vcoordpm) );
/*
 *	Create buttons for scalar field lists.
 */
	BuildOptionMenuButtons ( GetModelScalarPmXSW ( vcs ),
				xssclabels, xsscalarchapter, numxsscalarkeys,
				XSScalarFdfDirCB,
				&(vcs->scalarpm) );
/*
 *	Create buttons for vector field lists.
 */
	BuildOptionMenuButtons ( GetModelVectorPmXSW ( vcs ),
				xsvclabels, xsvectorchapter, numxsvectorkeys,
				XSVectorFdfDirCB,
				&(vcs->vectorpm) );
/*
 *	Set default scalar and vector field chapters.
 */
	subdir = GetConfigValue( "xsfdf_directory" );
	if (subdir != NULL)
	    SetModelXSScalarDir ( vcs, subdir );
	else if ( xsscalarchapter != NULL )
	    SetModelXSScalarDir ( vcs, *xsscalarchapter );

	if ( xsvectorchapter != NULL )
	    SetModelXSVectorDir ( vcs, *xsvectorchapter );

/*
 *	Set default chapter button.
 */
	XtUnmanageChild ( GetModelScalarOmXSW ( vcs ) );
	SetDefaultOptionButton ( GetModelScalarOmXSW ( vcs ),
				 numxsscalarkeys,
                                 GetModelXSScalarDir( vcs ),
                                 xsscalarchapter,
				 &(vcs->scalarpm), " " );
	XtManageChild ( GetModelScalarOmXSW ( vcs ) );

/*
 *	Set default vertical coordinate.
 */
	if ( GetModelVCoordXS ( vcs ) != NULL )
	    XmTextSetString ( GetModelVCoordXSW ( vcs ),
					GetModelVCoordXS ( vcs ) );
/*
 *	Set default cross section.
 */
	if ( xsection != NULL ) {
	    XmTextSetString ( GetXSectionXSW ( vcs ), xsection );
	    Free ( xsection );
	}

/*
 *	Set color bar option menu.
 */
	BuildOptionMenuButtons ( GetFillColorsXSW ( vcs ),
                                clabels, colors, numcolorkeys,
                                XSGridFillTableCB,
                                &(vcs->color_fillpm) );

/*
 *	Set line type option menu.
 */
	BuildOptionMenuButtons ( GetLineTypePmXSW ( vcs ),
				linelabels, line, numlinekeys,
				XSLineTypeCB,
				&(vcs->line_type_pm) );

/*
	XtUnmanageChild ( GetLineTypeOmXSW ( vcs ) );
	SetDefaultOptionButton ( GetLineTypeOmXSW ( vcs ),
				numlinekeys, GetLineTypeXS ( vcs ),
				line, &(vcs->line_type_pm), " " );
	XtManageChild ( GetLineTypeOmXSW ( vcs ) );
*/

/*
 *	Set line label frequency option menu.
 */
	BuildOptionMenuButtons ( GetLabelFrequencyPmXSW ( vcs ),
				freqlabels, freqkeys, numfreqkeys,
				XSLineLabelFrequencyCB,
				&(vcs->label_freq_pm) );

/*
	XtUnmanageChild ( GetLabelFrequencyOmXSW ( vcs ) );
	SetDefaultOptionButton ( GetLabelFrequencyOmXSW ( vcs ),
				numfreqkeys, GetLabelFrequencyXS ( vcs ),
				freqkeys, &(vcs->label_freq_pm), " " );
	XtManageChild ( GetLabelFrequencyOmXSW ( vcs ) );
*/

/*
 *	Set wind symbol option menu.
 */
	iret = GetKeyList ( "VertVectorkeys", "VertVectorsymbols", ",",
                                     &numsymkeys, &symbols, &symlabels );
	if ( !iret ) {

	    BuildOptionMenuButtons ( GetVectorSymbolPmXSW ( vcs ),
				     symlabels, symbols, numsymkeys,
				     XSWindSymbolCB,
				     &(vcs->vector_sym_pm) );

/*
	    XtUnmanageChild ( GetVectorSymbolOmXSW ( vcs ) );
	    SetDefaultOptionButton ( GetVectorSymbolOmXSW ( vcs ),
				     numsymkeys, GetVectorSymbolXS ( vcs ),
				     symbols, &(vcs->vector_sym_pm), " " );
	    XtManageChild ( GetVectorSymbolOmXSW ( vcs ) );
*/

	    StringListFree ( numsymkeys, symbols );
	    StringListFree ( numsymkeys, symlabels );
	}

/*
 *	Set vertical yaxis option menu.
 */
	BuildOptionMenuButtons ( GetAxisTypePmXSW ( vcs ),
				yaxislabels, yaxis, numyaxiskeys,
				XSVerticalAxisCB,
				&(vcs->axis_type_pm) );
/*
	XtUnmanageChild ( GetAxisTypeOmXSW ( vcs ) );
	SetDefaultOptionButton ( GetAxisTypeOmXSW ( vcs ),
				numyaxiskeys, GetAxisTypeXS ( vcs ), yaxis,
				&(vcs->axis_type_pm), " " );
	XtManageChild ( GetAxisTypeOmXSW ( vcs ) );
*/

/*
 *	Set default FDF.
 */
	file = GetConfigValue( "xsfdf" );
	directory = GetConfigValue( "xsscalarfdf" );

	err = GetFDF ( file, directory, subdir, model, fdf );

	if ( !err ) {
	    SetXSFdfFallbackValues ( fdf );
	    SetXSGridWidgets ( fdf, file );
	}
	Free ( subdir );
	if ( file      != NULL ) Free ( file );
	if ( directory != NULL ) Free ( directory );
	if ( model     != NULL ) Free ( model );
}

void
InitGuiTimeHeightDialog (
	char **mlabels, char **models, int nummodelkeys,
	char **vlabels, char **vcoord, int numvcoordkeys,
	char **thsclabels, char **thscalarchapter, int numthscalarkeys,
	char **thvclabels, char **thvectorchapter, int numthvectorkeys,
	char **freqlabels, char **freqkeys, int numfreqkeys,
	char **linelabels, char **line, int numlinekeys,
	char **yaxislabels, char **yaxis, int numyaxiskeys,
	char **taxislabels, char **taxis, int numtaxiskeys )
/*
 * Initialize widgets in Time/height Section dialog.
 */
{
	GuiTimeHeightDialogType *tht;
	KeyListType		*fdf;
	ButtonParentObjectType	*bpo;
	ButtonObject		*bo;

	XmString		xmstr;
	char			*station, *model;
	char			*file, *directory, *subdir;
	char			**symbols, **symlabels;
	
	int			numsymkeys;
	int			i, iret, err;

	tht = GetGuiTimeHeightDialog();
	fdf = GetTHGuiFdfInfo();

/*
 *	Get configuration key values.
 */
	station = GetConfigValue( "time-height_stn" );
	model = GetConfigValue( "model" );

	SetTHGridType ( tht, SCALARGRIDOBJECT );

/*
 *	Create buttons for Model option menu.
 */
	XtUnmanageChild ( GetModelTypeOmTHW ( tht ) );
	BuildOptionMenuButtons ( GetModelTypePmTHW ( tht ),
				 mlabels, models, nummodelkeys,
				 THModelButtonCB,
				 &(tht->modelpm) );
/*
 *	Set default Model button.
 */
	SetDefaultOptionButton ( GetModelTypeOmTHW ( tht ),
				 nummodelkeys, model, models,
				 &(tht->modelpm), " " );
	XtManageChild ( GetModelTypeOmTHW ( tht ) );

/*
 *	Create buttons for Vertical Coordinate option menu.
 */
	BuildOptionMenuButtons ( GetModelVCoordPmTHW ( tht ),
				vlabels, vcoord, numvcoordkeys,
				THVcoordButtonCB,
				&(tht->vcoordpm) );
/*
 *	Create buttons for scalar field lists.
 */
	BuildOptionMenuButtons ( GetModelScalarPmTHW ( tht ),
				thsclabels, thscalarchapter, numthscalarkeys,
				THScalarFdfDirCB,
				&(tht->scalarpm) );
/*
 *	Create buttons for vector field lists.
 */
	BuildOptionMenuButtons ( GetModelVectorPmTHW ( tht ),
				thvclabels, thvectorchapter, numthvectorkeys,
				THVectorFdfDirCB,
				&(tht->vectorpm) );
/*
 *	Set default scalar and vector field chapters.
 */
	subdir = GetConfigValue( "thfdf_directory" );
	if (subdir != NULL)
	    SetModelTHScalarDir ( tht, subdir );
	else if ( thscalarchapter != NULL )
	    SetModelTHScalarDir ( tht, *thscalarchapter );

	if ( thvectorchapter != NULL )
	    SetModelTHVectorDir ( tht, *thvectorchapter );

/*
 *	Set default chapter button.
 */
	XtUnmanageChild ( GetModelScalarOmTHW ( tht ) );
	SetDefaultOptionButton ( GetModelScalarOmTHW ( tht ),
				 numthscalarkeys,
                                 GetModelTHScalarDir( tht ),
                                 thscalarchapter,
				 &(tht->scalarpm), " " );
	XtManageChild ( GetModelScalarOmTHW ( tht ) );

/*
 *	Set default vertical coordinate.
 */
	if ( GetModelVCoordTH ( tht ) != NULL )
	    XmTextSetString ( GetModelVCoordTHW ( tht ),
					GetModelVCoordTH ( tht ) );
/*
 *	Set default cross section.
 */
	if ( station != NULL ) {
	    XmTextSetString ( GetStationTHW ( tht ), station );
	    Free ( station );
	}

/*
 *	Create buttons for line type option menu.
 */
	BuildOptionMenuButtons ( GetLineTypePmTHW ( tht ),
				linelabels, line, numlinekeys,
				THLineTypeCB,
				&(tht->line_type_pm) );

/*
	XtUnmanageChild ( GetLineTypeOmTHW ( tht ) );
	SetDefaultOptionButton ( GetLineTypeOmTHW ( tht ),
				numlinekeys, GetLineTypeTH ( tht ),
				line, &(tht->line_type_pm), " " );
	XtManageChild ( GetLineTypeOmTHW ( tht ) );
*/

/*
 *	Create buttons for line label frequency option menu.
 */
	BuildOptionMenuButtons ( GetLabelFrequencyPmTHW ( tht ),
				freqlabels, freqkeys, numfreqkeys,
				THLineLabelFrequencyCB,
				&(tht->label_freq_pm) );

/*
	XtUnmanageChild ( GetLabelFrequencyOmTHW ( tht ) );
	SetDefaultOptionButton ( GetLabelFrequencyOmTHW ( tht ),
				numfreqkeys, GetLabelFrequencyTH ( tht ),
				freqkeys, &(tht->label_freq_pm), " " );
	XtManageChild ( GetLabelFrequencyOmTHW ( tht ) );
*/

/*
 *	Create buttons for wind symbol option menu.
 */
	iret = GetKeyList ( "VertVectorkeys", "VertVectorsymbols", ",",
                                     &numsymkeys, &symbols, &symlabels );
	if ( !iret ) {

	    BuildOptionMenuButtons ( GetVectorSymbolPmTHW ( tht ),
				     symlabels, symbols, numsymkeys,
				     THWindSymbolCB,
				     &(tht->vector_sym_pm) );

/*
	    XtUnmanageChild ( GetVectorSymbolOmTHW ( tht ) );
	    SetDefaultOptionButton ( GetVectorSymbolOmTHW ( tht ),
				     numsymkeys, GetVectorSymbolTH ( tht ),
				     symbols, &(tht->vector_sym_pm), " " );
	    XtManageChild ( GetVectorSymbolOmTHW ( tht ) );
*/

	    StringListFree ( numsymkeys, symbols );
	    StringListFree ( numsymkeys, symlabels );
	}

/*
 *	Create buttons for vertical yaxis option menu.
 */

	BuildOptionMenuButtons ( GetAxisTypePmTHW ( tht ),
				yaxislabels, yaxis, numyaxiskeys,
				THVerticalAxisCB,
				&(tht->axis_type_pm) );
/*
	XtUnmanageChild ( GetAxisTypeOmTHW ( tht ) );
	SetDefaultOptionButton ( GetAxisTypeOmTHW ( tht ),
				numyaxiskeys, GetAxisTypeTH ( tht ), yaxis,
				&(tht->axis_type_pm), " " );
	XtManageChild ( GetAxisTypeOmTHW ( tht ) );
*/

/*
 *	Create buttons for time axis direction.
 */
	BuildOptionMenuButtons ( GetTimeAxisPmTHW ( tht ),
				taxislabels, taxis, numtaxiskeys,
				THTimeAxisCB,
				&(tht->taxis) );
/*
	XtUnmanageChild ( GetTimeAxisOmTHW ( tht ) );
	SetDefaultOptionButton ( GetTimeAxisOmTHW ( tht ),
				numtaxiskeys, GetTimeAxisTH ( tht ), taxis,
				&(tht->taxis), " " );
	XtManageChild ( GetTimeAxisOmTHW ( tht ) );
*/


/*
 *	Set default FDF.
 */
	file = GetConfigValue( "thfdf" );
	directory = GetConfigValue( "thscalarfdf" );

	err = GetFDF ( file, directory, subdir, model, fdf );

	if ( !err ) {
	    SetTHFdfFallbackValues ( fdf );
	    SetTHGridWidgets ( fdf, file );
	}
	if ( subdir    != NULL ) Free ( subdir );
	if ( file      != NULL ) Free ( file );
	if ( directory != NULL ) Free ( directory );
	if ( model     != NULL ) Free ( model );
}

void
InitGuiVerticalProfileDialog (
	char **mlabels, char **models, int nummodelkeys,
	char **vlabels, char **vcoord, int numvcoordkeys,
	char **vpsclabels, char **vpscalarchapter, int numvpscalarkeys,
	char **vpvclabels, char **vpvectorchapter, int numvpvectorkeys,
	char **freqlabels, char **freqkeys, int numfreqkeys,
	char **linelabels, char **line, int numlinekeys,
	char **yaxislabels, char **yaxis, int numyaxiskeys )
/*
 * Initialize widgets in Vertical Profile dialog.
 */
{
	GuiVertProfileObjectType	*vpt;
	KeyListType			*fdf;
	ButtonParentObjectType		*bpo;
	ButtonObject			*bo;

	XmString		xmstr;
	char			*station, *model;
	char			*file, *directory, *subdir;
	char			**symbols, **symlabels;
	
	int			numsymkeys;
	int			i, iret, err;

	vpt = GetGuiVertProfileDialog();
	fdf = GetVPGuiFdfInfo();

/*
 *	Get configuration key values.
 */
	station = GetConfigValue( "time-height_stn" );
	model = GetConfigValue( "model" );

	SetVPGridType ( vpt, SCALARGRIDOBJECT );

/*
 *	Create buttons for Model option menu.
 */
	XtUnmanageChild ( GetModelTypeOmVPW ( vpt ) );
	BuildOptionMenuButtons ( GetModelTypePmVPW ( vpt ),
				 mlabels, models, nummodelkeys,
				 VPModelButtonCB,
				 &(vpt->modelpm) );
/*
 *	Set default Model button.
 */
	SetDefaultOptionButton ( GetModelTypeOmVPW ( vpt ),
				 nummodelkeys, model, models,
				 &(vpt->modelpm), " " );
	XtManageChild ( GetModelTypeOmVPW ( vpt ) );

/*
 *	Create buttons for Vertical Coordinate option menu.
 */
	BuildOptionMenuButtons ( GetModelVCoordPmVPW ( vpt ),
				vlabels, vcoord, numvcoordkeys,
				VPVcoordButtonCB,
				&(vpt->vcoordpm) );
/*
 *	Create buttons for scalar field lists.
 */
	BuildOptionMenuButtons ( GetModelScalarPmVPW ( vpt ),
				vpsclabels, vpscalarchapter, numvpscalarkeys,
				VPScalarFdfDirCB,
				&(vpt->scalarpm) );
/*
 *	Create buttons for vector field lists.
 */
	BuildOptionMenuButtons ( GetModelVectorPmVPW ( vpt ),
				vpvclabels, vpvectorchapter, numvpvectorkeys,
				VPVectorFdfDirCB,
				&(vpt->vectorpm) );
/*
 *	Set default scalar and vector field chapters.
 */
	subdir = GetConfigValue( "vpfdf_directory" );
	if (subdir != NULL)
	    SetModelVPScalarDir ( vpt, subdir );
	else if ( vpscalarchapter != NULL )
	    SetModelVPScalarDir ( vpt, *vpscalarchapter );

	if ( vpvectorchapter != NULL )
	    SetModelVPVectorDir ( vpt, *vpvectorchapter );

/*
 *	Set default chapter button.
 */
	XtUnmanageChild ( GetModelScalarOmVPW ( vpt ) );
	SetDefaultOptionButton ( GetModelScalarOmVPW ( vpt ),
				 numvpscalarkeys,
                                 GetModelVPScalarDir( vpt ),
                                 vpscalarchapter,
				 &(vpt->scalarpm), " " );
	XtManageChild ( GetModelScalarOmVPW ( vpt ) );

/*
 *	Set default vertical coordinate.
 */
	if ( GetModelVCoordVP ( vpt ) != NULL )
	    XmTextSetString ( GetModelVCoordVPW ( vpt ),
					GetModelVCoordVP ( vpt ) );
/*
 *	Set default cross section.
 */
	if ( station != NULL ) {
	    XmTextSetString ( GetStationVPW ( vpt ), station );
	    Free ( station );
	}

/*
 *	Set line type option menu.
 */
	BuildOptionMenuButtons ( GetLineTypePmVPW ( vpt ),
				linelabels, line, numlinekeys,
				VPLineTypeCB,
				&(vpt->line_type_pm) );

/*
	XtUnmanageChild ( GetLineTypeOmVPW ( vpt ) );
	SetDefaultOptionButton ( GetLineTypeOmVPW ( vpt ),
				numlinekeys, GetLineTypeVP ( vpt ),
				line, &(vpt->line_type_pm), " " );
	XtManageChild ( GetLineTypeOmVPW ( vpt ) );
*/

/*
 *	Set wind symbol option menu.
 */
	iret = GetKeyList ( "VertVectorkeys", "VertVectorsymbols", ",",
                                     &numsymkeys, &symbols, &symlabels );
	if ( !iret ) {

	    BuildOptionMenuButtons ( GetVectorSymbolPmVPW ( vpt ),
		  		     symlabels, symbols, numsymkeys,
				     VPWindSymbolCB,
				     &(vpt->vector_sym_pm) );

/*
	    XtUnmanageChild ( GetVectorSymbolOmVPW ( vpt ) );
	    SetDefaultOptionButton ( GetVectorSymbolOmVPW ( vpt ),
				     numsymkeys, GetVectorSymbolVP ( vpt ),
				     symbols, &(vpt->vector_sym_pm), " " );
	    XtManageChild ( GetVectorSymbolOmVPW ( vpt ) );
*/

	    StringListFree ( numsymkeys, symbols );
	    StringListFree ( numsymkeys, symlabels );
	}

/*
 *	Set vertical yaxis option menu.
 */

	BuildOptionMenuButtons ( GetAxisTypePmVPW ( vpt ),
				yaxislabels, yaxis, numyaxiskeys,
				VPVerticalAxisCB,
				&(vpt->axis_type_pm) );
	XtUnmanageChild ( GetAxisTypeOmVPW ( vpt ) );
	SetDefaultOptionButton ( GetAxisTypeOmVPW ( vpt ),
				numyaxiskeys, GetAxisTypeVP ( vpt ), yaxis,
				&(vpt->axis_type_pm), " " );
	XtManageChild ( GetAxisTypeOmVPW ( vpt ) );
	CheckYaxisTypeVP ( vpt, GetAxisTypeVP ( vpt ) );

/*
 *	Set default FDF.
 */
	file = GetConfigValue( "vpfdf" );
	directory = GetConfigValue( "vpscalarfdf" );

	err = GetFDF ( file, directory, subdir, model, fdf );

	if ( !err ) {
	    SetVPFdfFallbackValues ( fdf );
	    SetVPGridWidgets ( fdf, file );
	}
	if ( subdir    != NULL ) Free ( subdir );
	if ( file      != NULL ) Free ( file );
	if ( directory != NULL ) Free ( directory );
	if ( model     != NULL ) Free ( model );
}

void
InitGuiHorizontalGridDialog (
		char **mlabels, char **models, int nummodelkeys,
		char **vlabels, char **vcoord, int numvcoordkeys,
		char **clabels, char **colors, int numcolorkeys,
		char **sclabels, char **scalarchapter, int numscalarkeys,
		char **vclabels, char **vectorchapter, int numvectorkeys,
		char **freqlabels, char **freqkeys, int numfreqkeys,
		char **linelabels, char **line, int numlinekeys )

{
	GuiScalarGridDialogType *grd;
	ModelInfoType		*mdl;
	KeyListType		*fdf;

	XmString		xmstr;

	char			*level, *model;
	char			*file, *directory, *subdir;
	char			**symbols, **symlabels;
	
	int			numsymkeys;
	int			iret, err;


	grd = GetGuiScalarGridDialog();
	mdl = GetGuiModelInfo();
	fdf = GetHSGuiFdfInfo();

	model      = GetConfigValue( "model" );
/*
 *	Create buttons for Model option menu.
 */
	XtUnmanageChild ( GetModelOmHSW ( grd ) );
	BuildOptionMenuButtons ( GetModelPmHSW ( grd ),
				 mlabels, models, nummodelkeys,
				 HSModelButtonCB,
				 &(grd->modelpm) );
/*
 *	Set default Model button.
 */
	SetDefaultOptionButton ( GetModelOmHSW ( grd ),
				 nummodelkeys, model, models,
				 &(grd->modelpm), " " );
	XtManageChild ( GetModelOmHSW ( grd ) );

/*
 *	Create buttons for Vertical Coordinate option menu.
 */
	BuildOptionMenuButtons ( GetModelVCoordPmHSW ( grd ),
				vlabels, vcoord, numvcoordkeys,
				HSVcoordButtonCB,
				&(grd->vcoordpm) );
/*
 *	Set default vertical coordinate.
 */
	if ( GetModelVCoord ( mdl ) )
	    XmTextSetString ( GetModelVCoordHSW ( mdl ),
					GetModelVCoord ( mdl ) );
/*
 *	Create buttons for scalar field chapters.
 */
	BuildOptionMenuButtons ( GetModelScalarPmHSW ( grd ),
				sclabels, scalarchapter, numscalarkeys,
				HSScalarFdfDirCB,
				&(grd->scalarpm) );

/*
 *	Create buttons for vector field chapters.
 */
	BuildOptionMenuButtons ( GetModelVectorPmHSW ( grd ),
				vclabels, vectorchapter, numvectorkeys,
				HSVectorFdfDirCB,
				&(grd->vectorpm) );

/*
 *	Set default scalar and vector field chapters.
 */
	subdir = GetConfigValue( "fdf_directory" );
	if (subdir != NULL)
	    SetModelScalarDir ( grd, subdir );
	else if ( scalarchapter != NULL )
	    SetModelScalarDir ( grd, *scalarchapter );

	if ( vectorchapter != NULL )
	    SetModelVectorDir ( grd, *vectorchapter );

/*
 *	Set default chapter button.
 */
	XtUnmanageChild ( GetModelScalarOmHSW ( grd ) );
	SetDefaultOptionButton ( GetModelScalarOmHSW ( grd ),
				 numscalarkeys,
                                 GetModelScalarDir( grd ),
                                 scalarchapter,
				 &(grd->scalarpm), " " );
	XtManageChild ( GetModelScalarOmHSW ( grd ) );


/*
 *	Set level strings.
 */
        xmstr = XmStringLtoRCreate ( GetModelLevel1 ( mdl ),
	                             XmSTRING_DEFAULT_CHARSET );
	XtVaSetValues ( GetModelLevel1HSW ( mdl ),
				XmNvalue, GetModelLevel1 ( mdl ),
				NULL );
	XmStringFree ( xmstr );

        xmstr = XmStringLtoRCreate ( GetModelLevel2 ( mdl ),
	                             XmSTRING_DEFAULT_CHARSET );
	XtVaSetValues ( GetModelLevel2HSW ( mdl ),
				XmNvalue, GetModelLevel2 ( mdl ),
				NULL );
	XmStringFree ( xmstr );
/*
 *	Desensitize level 2 button and text field.
 */
	XtSetSensitive ( GetModelLevel2BHSW ( grd ), FALSE );
	XtSetSensitive ( GetModelLevel2HSW ( mdl ), FALSE );

/*
 *	Set fdf string.
 */
/*
        xmstr = XmStringLtoRCreate ( GetModelFdfHS ( grd ),
	                             XmSTRING_DEFAULT_CHARSET );
	XtVaSetValues ( GetModelFdfHSW ( grd ),
				XmNlabelString, xmstr,
				NULL );
        XmStringFree ( xmstr );
*/

/*
 *	Set FDF text field.
 */
/*
	if ( GetFunctionHS (grd) )
	    XmTextSetString ( GetFunctionHSW (grd), GetFunctionHS (grd) );
*/

/*
 *	Set color bar option menu.
 */
	BuildOptionMenuButtons ( GetFillColorsHSW ( grd ),
				clabels, colors, numcolorkeys,
				HSGridFillTableCB,
				&(grd->color_fillpm) );
/*
 *	Set line type option menu.
 */
	BuildOptionMenuButtons ( GetLineTypePmHSW ( grd ),
				linelabels, line, numlinekeys,
				HSLineTypeCB,
				&(grd->line_type_pm) );

/*
	XtUnmanageChild ( GetLineTypeOmHSW ( grd ) );
	SetDefaultOptionButton ( GetLineTypeOmHSW ( grd ),
				numlinekeys, GetLineTypeHS ( grd ),
				line, &(grd->line_type_pm), " " );
	XtManageChild ( GetLineTypeOmHSW ( grd ) );
*/

/*
 *	Set line label frequency option menu.
 */
	BuildOptionMenuButtons ( GetLabelFrequencyPmHSW ( grd ),
				freqlabels, freqkeys, numfreqkeys,
				HSLineLabelFrequencyCB,
				&(grd->label_freq_pm) );

/*
	XtUnmanageChild ( GetLabelFrequencyOmHSW ( grd ) );
	SetDefaultOptionButton ( GetLabelFrequencyOmHSW ( grd ),
				numfreqkeys, GetLabelFrequencyHS ( grd ),
				freqkeys, &(grd->label_freq_pm), " " );
	XtManageChild ( GetLabelFrequencyOmHSW ( grd ) );
*/

/*
 *	Set wind symbol option menu.
 */
	iret = GetKeyList ( "vectorkeys", "vectorsymbols", ",",
                                     &numsymkeys, &symbols, &symlabels );
	if ( !iret ) {

	    BuildOptionMenuButtons ( GetVectorSymbolPmHSW ( grd ),
				     symlabels, symbols, numsymkeys,
				     HSWindSymbolCB,
				     &(grd->vector_sym_pm) );

/*
	    XtUnmanageChild ( GetVectorSymbolOmHSW ( grd ) );
	    SetDefaultOptionButton ( GetVectorSymbolOmHSW ( grd ),
				     numsymkeys, GetVectorSymbolHS ( grd ),
				     symbols, &(grd->vector_sym_pm), " " );
	    XtManageChild ( GetVectorSymbolOmHSW ( grd ) );
*/

	    StringListFree ( numsymkeys, symbols );
	    StringListFree ( numsymkeys, symlabels );
	}

/*
 *	Set default FDF.
 */
	file = GetConfigValue( "fdf" );
	directory = GetConfigValue( "scalarfdf" );

	err = GetFDF ( file, directory, subdir, model, fdf );

	if ( !err ) {
	    SetFdfFallbackValues ( fdf );
	    SetGridWidgets ( fdf, file );
	}
	if ( subdir     != NULL ) Free ( subdir );
	if ( file       != NULL ) Free ( file );
	if ( directory  != NULL ) Free ( directory );
	if ( model      != NULL ) Free ( model );
}



void
InitGuiUpaProfileDialog (
	char **linelabels, char **line, int numlinekeys )

{
	GuiUpaProfileDialogType	*upf;
	char			*stndex;

	upf = GetGuiUpaProfileDialog();

/*
 *	Set the initial plot type.
 */
	SetDiagramTypeUF ( upf, "skewt" );

/*
 *	Set thermodynamic indices to be plotted.
 */
	stndex = GetConfigValue( "stndex" );
	if ( !stndex ) stndex = strdup (" ");
	SetIndicesUF ( upf, stndex );
	Free ( stndex );

/*
 *	Set line parameters for thermodynamic diagrams.
 */

/*
 *	Set line type option menu.
 */
	BuildOptionMenuButtons ( GetLineTypePmUFW ( upf ),
				linelabels, line, numlinekeys,
				UFLineTypeCB,
				&(upf->line_type_pm) );

	XtUnmanageChild ( GetLineTypeOmUFW ( upf ) );
	SetDefaultOptionButton ( GetLineTypeOmUFW ( upf ),
				numlinekeys, GetLineTypeUF ( upf ),
				line, &(upf->line_type_pm), " " );
	XtManageChild ( GetLineTypeOmUFW ( upf ) );
}


void InitGuiUpperAirDialog()
{

	XmString		xmstr;

	char			*level;

	GuiUpperairDialogType	*upa;

	upa = GetGuiUpperairDialog();


/*
 *	Define initial projection which determines which dialog is
 *	popped up.
 */
	SetUpaPlotType ( upa, UPPERAIROBJECT );

/*
 *	Define initial sounding level.
 */
	level = strdup ( "850" );
	SetUpaLevelLabel ( upa, level );
	Free ( level );
/*
 *	Set level string.
 */
        xmstr = XmStringLtoRCreate ( GetUpaLevelLabel ( upa ),
	                             XmSTRING_DEFAULT_CHARSET );
	XtVaSetValues ( GetUpaLevelLabelW ( upa ),
				XmNlabelString, xmstr,
				NULL );
	XmStringFree ( xmstr );


	return;
}

void
InitWindProfilerDialog()
{

	GuiProfilerDialogType	*wpf;
	Widget			scrolled_list, bottomW, topW,
				incrementW, durationW, timeintW;

	char			*bottom, *top, *increment, *duration, *timeint;
	char			*endtime;
	AbsTime			currenttm;

	wpf = GetGuiProfilerDialog();
/*
 *	Get config values.
 */
	bottom    = GetConfigValue( "wpbottom" );
	top       = GetConfigValue( "wptop" );
	increment = GetConfigValue( "wpincrement" );
	duration  = GetConfigValue( "wpduration" );
	timeint   = GetConfigValue( "wptimeint" );
	endtime   = GetConfigValue( "wpendtime" );

/*
 *	Get widget ids.
 */
	scrolled_list = GetProfilerScrolledListW ( wpf );
	bottomW       = GetProfilerBottomW ( wpf );
	topW          = GetProfilerTopW ( wpf );
	incrementW    = GetProfilerIncrementW ( wpf );
	durationW     = GetProfilerDurationW ( wpf );
	timeintW      = GetProfilerTimeIntervalW ( wpf );

/*
 *	Unmanage widgets.
 */
	XtUnmanageChild ( scrolled_list );
	XtUnmanageChild ( bottomW );
	XtUnmanageChild ( topW );
	XtUnmanageChild ( incrementW );
	XtUnmanageChild ( durationW );
	XtUnmanageChild ( timeintW );

/*
 *	Dynamically build scrolled list.
 */
	BuildProfilerStationList ( scrolled_list,
				&(wpf->station_list) );
/*
 *	Set cross section end time to current time.
 */
	if ( !endtime ) {
	    currenttm = (AbsTime)present_time();
	    endtime = sec2yyyymmddhhmm ( currenttm );
	    endtime[11]='0'; endtime[12]='0';
	}

/*
 *	Set textfield values.
 */
	if ( !bottom ) bottom = strdup (" ");
	SetProfilerBottom ( wpf, bottom);

	if ( !top ) top = strdup (" ");
	SetProfilerTop ( wpf, top );

	if ( !increment ) increment = strdup (" ");
	SetProfilerIncrement ( wpf, increment );

	if ( !duration ) duration = strdup (" ");
	SetProfilerDuration ( wpf, duration );

	if ( !timeint ) timeint = strdup (" ");
	SetProfilerTimeInterval ( wpf, timeint );

/*
 *	Initialize end time for time/height section.
 */
	XmTextSetString ( GetProfilerEndTimeW ( wpf ), endtime );
	XmTextSetString ( GetProfilerBottomW ( wpf ), GetProfilerBottom ( wpf ));
	XmTextSetString ( GetProfilerTopW ( wpf ), GetProfilerTop ( wpf ));
	XmTextSetString ( GetProfilerIncrementW ( wpf ), GetProfilerIncrement ( wpf ));
	XmTextSetString ( GetProfilerDurationW ( wpf ), GetProfilerDuration ( wpf ));
	XmTextSetString ( GetProfilerTimeIntervalW ( wpf ), GetProfilerTimeInterval ( wpf ));

/*
 *	Manage widgets.
 */
	XtManageChild ( scrolled_list );
	XtManageChild ( bottomW );
	XtManageChild ( topW );
	XtManageChild ( incrementW );
	XtManageChild ( durationW );
	XtManageChild ( timeintW );

/*
 *	Free.
 */
	if ( endtime != NULL )    Free ( endtime );
	if ( bottom != NULL )     Free ( bottom );
	if ( top != NULL )        Free ( top );
	if ( increment != NULL )  Free ( increment );
	if ( duration != NULL )   Free ( duration );
	if ( timeint != NULL )    Free ( timeint );

	return;
}


void
InitStationOverlayDialog ()
{

/*
 *	Create list of stations.
 */
/*
        BuildOptionMenuButtons ( GetFillColorsHSW ( grd ),
                                clabels, colors, numcolorkeys,
                                HSGridFillTableCB,
                                &(grd->color_fillpm) );
*/

}


void
InitModelObject ( int *iret )
/*
 * Initialize some dialog widgets.
 */
{
	ModelInfoType			*mdl;
	GuiScalarGridDialogType		*grd;
	GuiVectorGridDialogType		*vec;
	GuiXSectionDialogType		*vcs;
	GuiTimeHeightDialogType		*tht;
	GuiVertProfileObjectType	*vpt;
	GuiUpaProfileDialogType		*upf;
	GuiBackgroundDialogType		*bak;

	char			*model, *label, *vcoord,
				*level1, *level2,
				*yaxistype, *vpyaxistype;

	char			**models, **labels;
	int			numkeys;

	mdl = GetGuiModelInfo();
	grd = GetGuiScalarGridDialog();
	vec = GetGuiVectorGridDialog();
	vcs = GetGuiXSectionDialog();
	tht = GetGuiTimeHeightDialog();
	vpt = GetGuiVertProfileDialog();
	upf = GetGuiUpaProfileDialog();
	bak = GetGuiBackgroundDialog();

	*iret = NORMAL;

	model         = GetConfigValue( "model" );
	vcoord        = GetConfigValue( "vcoord" );
	level1        = GetConfigValue( "level1" );
	level2        = GetConfigValue( "level2" );
	yaxistype     = GetConfigValue( "yaxistype" );
	vpyaxistype   = GetConfigValue( "vpyaxistype" );
	
/*
 *	Initialize grid plot type.
 */
	SetGridType ( mdl, SCALARGRIDOBJECT );

/*
 *	Initialize model object values.
 */
	SetModelProjType ( mdl, -99 );

/*
 *	Define initial model type.
 */
	if ( model != NULL ) {
	    SetModelType ( mdl, model );
	    SetModelTypeXS ( vcs, model );
	    SetModelTypeTH ( tht, model );
	    SetModelTypeVP ( vpt, model );
/*
 *	Define model label.
 */
	    *iret = GetKeyList ( "modelkeys", "modellabels", ",",
	                           &numkeys, &models, &labels );

	    label = GetStringArrayLabel ( numkeys, model,
	                                  models, labels );

	    SetModelLabelType ( mdl, label );
	    SetModelLabelTypeXS ( vcs, label );
	    SetModelLabelTypeTH ( tht, label );
	    SetModelLabelTypeVP ( vpt, label );

	    StringListFree ( numkeys, models );
	    StringListFree ( numkeys, labels );
	    Free ( model );
	    Free ( label );
	}

/*
 *	Define vertical coordinate system.
 */
	if ( yaxistype != NULL ) {
	    SetAxisTypeXS ( vcs, yaxistype );
	    SetAxisTypeTH ( tht, yaxistype );
	    Free ( yaxistype );
	}

	if ( vpyaxistype != NULL ) {
	    SetAxisTypeVP ( vpt, vpyaxistype );
	    Free ( vpyaxistype );
	}

/*
 *	Define vertical axis type.
 */
	if ( vcoord != NULL ) {
	    SetModelVCoord ( mdl, vcoord );
	    SetModelVCoordXS ( vcs, vcoord );
	    SetModelVCoordTH ( tht, vcoord );
	    SetModelVCoordVP ( vpt, vcoord );
	    Free ( vcoord );
	}

/*
 *	Define horizontal axis type.
 */
	SetTimeAxisTH ( tht, "R" );


/*
 *	Define initial model level.
 */
	if ( level1 != NULL ) {
	    SetModelLevel1 ( mdl, level1 );
	    Free ( level1 );
	}
	if ( level2 != NULL ) {
	    SetModelLevel2 ( mdl, level2 );
	    Free ( level2 );
	}

/*
 *	Default line color.
 */
	SetLineColorHS ( grd, "NONE" );
	SetLineColorXS ( vcs, "NONE" );
	SetLineColorTH ( tht, "NONE" );
	SetLineColorVP ( vpt, "NONE" );

/*
 *	Default contour line type.
 */
	SetLineTypeHS ( grd, "1" );
	SetLineTypeXS ( vcs, "1" );
	SetLineTypeTH ( tht, "1" );
	SetLineTypeVP ( vpt, "1" );
	SetLineTypeUF ( upf, "1" );

/*
 *	Default contour line width.
 */
	SetLineWidthHS ( grd, "2" );
	SetLineWidthXS ( vcs, "2" );
	SetLineWidthTH ( tht, "2" );
	SetLineWidthVP ( vpt, "2" );

/*
 *	Default contour label frequency.
 */
	SetLabelFrequencyHS ( grd, "2" );
	SetLabelFrequencyXS ( vcs, "2" );
	SetLabelFrequencyTH ( tht, "2" );

/*
 *	Default vector symbol.
 */
	SetVectorSymbolHS ( grd, "bk" );
	SetVectorSymbolXS ( vcs, "bk" );
	SetVectorSymbolTH ( tht, "bk" );
	SetVectorSymbolVP ( vpt, "bk" );

/*
 *	Default color fill table.
 */
	SetFillColorsHS ( grd, "0;23-15" );
	SetFillColorsXS ( vcs, "0;23-15" );

/*
 *	Default vertical levels.
 */
	SetLevel1XS ( vcs, " " );
	SetLevel2XS ( vcs, " " );
	SetLevel1TH ( tht, " " );
	SetLevel2TH ( tht, " " );
	SetLevel1VP ( vpt, " " );
	SetLevel2VP ( vpt, " " );
/*
 *	Default map background station table.
 */
	SetStationType ( bak, "0" );
	SetStationParm ( bak, "0" );

}


int
GetKeyList ( char *keyname,
	     char *labelname,
	     char *delimiter,
	     int *numkeys,	
	     char ***keys,
	     char ***labels )
{
	char		**keyptr, **labelptr;
	char		*keylist, *labellist;
	int		numlabels;

/*
 *	Get name/value from configuration file.
 */
	keylist   = GetConfigValue( keyname );
	if ( keylist == NULL ) {
	    printf("Warning: key '%s' is not defined in Garp_defaults\n",
	            keyname);
	    return -1;
	}

	labellist = GetConfigValue( labelname );
	if ( labellist == NULL ) {
	    printf("Warning: key '%s' is not defined in Garp_defaults\n",
	            labelname);
	    Free ( keylist );
	    return -1;
	}
/*
 *
 *	Break string up into a list.
 */
	keyptr = SplitByDelimeter ( keylist, delimiter, numkeys );
	labelptr = SplitByDelimeter ( labellist, delimiter, &numlabels );

/*
 *	Make sure number of strings in each list are the same.
 */
	if ( *numkeys != numlabels ) {
	    printf ("***\n");
	    printf ("*** Fatal Error: Number of keys and labels " );
	    printf ("in configuration file do not match.\n" );
	    printf ("*** Problem keys are: '%s'", keyname );
	    printf (" and '%s'\n", labelname );
	    printf ("***\n");
	    Free ( keylist );
	    Free ( labellist );
	    StringListFree ( *numkeys, keyptr );
	    StringListFree ( numlabels, labelptr );
	    return -2;
	}

/*
 *	Return a pair of lists.
 */
	*keys = keyptr;
	*labels = labelptr;

	Free ( keylist );
	Free ( labellist );

/*
 *	Normal return.
 */
	return 0;
}

void InitGuiSurfaceObsDialog()
{
	GuiSfcObsDialogType     *sfc;

	char			**plot, **plotlabels;
	char			**marker, **markerlabels;
	int			numkeys;
	int			iret;

	sfc = GetGuiSfcObsDialog();

/*
 *	Get keys for plot types and labels.
 */
	iret = GetKeyList ( "sfc_keys", "sfc_labels", ",",
	                             &numkeys, &plot, &plotlabels );
/*
 *	Set plot type option menu.
 */
	if ( !iret ) {
	    SetGuiSfcObsType  ( sfc, *plot );
	    SetGuiSfcObsLabel ( sfc, *plotlabels );

	    BuildOptionMenuButtons ( GetSfcPlotTypePmSFW ( sfc ),
				plotlabels, plot, numkeys,
				SFPlotTypeCB,
				&(sfc->plot_type_pm) );

	    XtUnmanageChild ( GetSfcPlotTypeOmSFW ( sfc ) );
	    SetDefaultOptionButton ( GetSfcPlotTypeOmSFW ( sfc ),
				numkeys, GetGuiSfcObsType ( sfc ),
				plot, &(sfc->plot_type_pm), " " );
	    XtManageChild ( GetSfcPlotTypeOmSFW ( sfc ) );

	    StringListFree ( numkeys, plot );
	    StringListFree ( numkeys, plotlabels );
	}

/*
 *	Get keys for marker types and labels.
 */
	iret = GetKeyList ( "sfc_markers", "sfc_marker_labels", ",",
	                             &numkeys, &marker, &markerlabels );
/*
 *	Set marker type menu.
 */
	if ( !iret ) {
	    SetSfcMarkerType ( sfc, *marker );

	    BuildOptionMenuButtons ( GetSfcMarkerTypePmSFW ( sfc ),
				markerlabels, marker, numkeys,
				SFMarkerTypeCB,
				&(sfc->marker_type_pm) );

	    XtUnmanageChild ( GetSfcMarkerTypeOmSFW ( sfc ) );
	    SetDefaultOptionButton ( GetSfcMarkerTypeOmSFW ( sfc ),
				numkeys, GetSfcMarkerType ( sfc ),
				marker, &(sfc->marker_type_pm), " " );
	    XtManageChild ( GetSfcMarkerTypeOmSFW ( sfc ) );

	    StringListFree ( numkeys, marker );
	    StringListFree ( numkeys, markerlabels );
	}

/*
 *	Set station parameter textfield widgets.
 */
	DefaultSfcParameters();

}
