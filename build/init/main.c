/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	main.c
 *
 *	GARP main function
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 1/97	COMET	Added creation routine for upper air profile.
 *	 5/97	COMET	Added logging window creation routine.
 *	 5/97	COMET	Added status bar preferences creation routine.
 *	 6/97	COMET	Changed create_sw_pref_shell to create_lw_pref_shell
 *	 2/98	COMET	Added expiration date check.
 *	12/99	COMET	Replace time_t data type with AbsTime.
 *
 ***********************************************************************/


#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <Xm/Xm.h>
#include <stdio.h>

#include "gendef.h"
#define	 GARP_Version_Global
#include "version.h"

#include "_proto.h"

extern Widget main_canvas;  /* drawing canvas */
extern Widget main_shell;  /* drawing canvas */

extern XtAppContext app_context;
extern Display *display;       /*  Display             */

int main (int argc, char **argv)
{

	int 	iret;
	AbsTime	currentTime, expireTime;


/*
 *	Check on the expirationDate (defined in version.h)
 */
	if ( strcmp ( expirationDate, "0" ) != 0 ) {
	
	    currentTime = (AbsTime)present_time();
	    expireTime = ch_to_int_time ( expirationDate );

	    if ( currentTime > expireTime ) {
	    	printf ("\n\nSorry, the evaluation periord for this version of GARP\n"); 
		printf("expired at %s UTC.\nExiting...\n", expirationDate );
		exit (0);
	    }
	    printf ("Note: This evaluation copy of GARP will expire at %s UTC.\n",expirationDate );

	}
	
	printf ("%s starting...\n",GARP_version);

/*
 * 	Ensure we are always using UTC0 as time: (Chiz/Unidata)
 */
	putenv("TZ=UTC0");
   	tzset();

/*
 *	Parse the command line options and set some initial configuration
 *	parameters.  No return on an error.
 */
	ConfigInit( &argc, argv );

/*
 *	Initialize data information.
 */
	InitData ( &iret );

	XtSetLanguageProc ( (XtAppContext) NULL, 
			(XtLanguageProc) NULL, (XtPointer) NULL );
	XtToolkitInitialize ();
	app_context = XtCreateApplicationContext ();
	display = XtOpenDisplay (app_context, NULL, argv[0], "Garp",
	                         NULL, 0, &argc, argv);
	if (!display)
	{
	    printf("%s: can't open display, exiting...\n", argv[0]);
	    exit (-1);
	}
/*
 *	Dialogs.
 */
	create_main_shell ( display, argv[0], argc, argv );
	create_image_shell ( main_shell );
	create_point_plot_shell ( main_shell );
	create_sfc_model_shell_2 ( main_shell );
	create_prf_shell ( main_shell );
	create_upa_profile_shell ( main_shell );
	create_upa_shell ( main_shell );
	create_upa_model_shell ( main_shell );
	create_horiz_grid_shell ( main_shell );
	create_grid_option_shell ( main_shell );
	create_cross_section_shell ( main_shell );
	create_xsec_option_shell ( main_shell );
	create_th_shell ( main_shell );
	create_th_option_shell ( main_shell );
	create_vp_shell ( main_shell );
	create_vp_option_shell ( main_shell );
	create_description_shell ( main_shell );

	create_garea_shell ( main_shell );
	create_model_level_shell ( main_shell );
	create_stn_overlay_shell ( main_shell );
	create_animation_shell ( main_shell );
	create_general_pref_shell ( main_shell );
	create_sat_enhancement_shell ( main_shell );
	create_rad_overlay_shell ( main_shell );
	create_dw_pref_shell ( main_shell );
	create_lw_pref_shell ( main_shell );
	create_sb_pref_shell ( main_shell );
	create_log_shell ( main_shell );
	create_auto_update_shell ( main_shell );
	create_time_matching_shell ( main_shell );
	create_map_latlon_shell ( main_shell  );
	create_line_attributes_shell ( main_shell );
	create_license_shell ( main_shell );
	create_trademark_shell ( main_shell );

/*	CreateErrorWindowShell ( main_shell, app_context ); */
/*	CreateHelpPopup ( main_shell ); */

	InitActions ();

	XtRealizeWidget (main_shell);

/*
 *	Initialize popup dialogs.
 */
	InitDialogs( &iret );
	if ( iret < -1 ) exit (0);
/*
 *	Initialize garp.
 */
	if( GarpInitialize( display, app_context, main_shell, main_canvas ) ) {
		printf("Main: Initialization failed.  Exiting\n");
		exit( 1 );
	}

/*
 *	Wait for user input.
 */
	XtAppMainLoop (app_context);

/*
 *	White glove treatment ...
 */
	exit (0);
}

