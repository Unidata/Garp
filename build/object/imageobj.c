/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	imageobj.c
 *
 *	Routines for creating groups of image metObjects (nowrad, nids,
 *	satellite, and background objects that are images) and doing
 *	other generic image-related things.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 5/97	COMET	Cleaned up, added color bar toggle
 *	 8/97	COMET	Added time matching code.
 *	 9/97	COMET	Moved DoImageObjects() into gui/imagecb.c
 *	11/97	COMET	Added titleIndex to image creation routines.
 *	12/97	COMET	Fixed incorrect arg count in call to PrintArgv
 *	12/97	COMET	Added scale to image object creation routines
 *	 2/98	COMET	Added fix to scale for backgound topo maps
 *
 ***********************************************************************/


#include "underscore.h"
#include "winobj.h"
#include "guimacros.h"
#include "genglobs.h"
#include "imgobj.h"
#include "_proto.h"



/*
 *
 *  MetObjectFactory routine for creating radar, satellite and
 *  Nowrad Objects.  Takes as input a metObjectType and a file
 *  name.
 *
 *
 */
MetObjectType **MakeImageObjectList( int metObjectType, int cnt, char **files,
					char **timestamps)

{

	int				i, titleIndex;
	MetObjectType			**motv;
	MetObjectType                   *(*createFunc)();
	WindowObjectType		*wo;
	int				verbose;
	char				cbar[GEMPAKSTRING];
	char				time[GEMPAKSTRING];
	char				file[FILENAMESTRING];
	char				scale[GEMPAKSTRING];
	Widget				colorBarW;
	GuiImgSelectDialogType		*img;
	ButtonObject			*scaleButton;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 ) printf ( "MakeImageObjectList\n" );
	if( verbose > VERBOSE_1 ) {
		printf("  metObjectType = %d\n", metObjectType );
		printf("  cnt is %d\n", cnt );

		PrintArgv( cnt, files );
	}

	wo = GetActiveWindowObject();
	titleIndex = GetNextTitleIndex ( wo );

/*
 *	See if we will be drawing a colorbar...
 */
	img = GetGuiImgSelectDialog();
	colorBarW = GetImgColorBarW(img);
	if ( XmToggleButtonGetState ( colorBarW ))
		strcpy ( cbar, "1/V/LL/0;.05/.90" );
	else
		strcpy ( cbar, " " );


/*
 *	Get the scale label
 */
	scaleButton = GetImgScalePanelSetbutton(img);
	if ( scaleButton != NULL )
	    strcpy( scale, scaleButton->label );
	else
	    strcpy( scale, "None" );

/*
 *	Choose the correct image creation rotuine...
 */
	switch( metObjectType ) {
	case RADAROBJECT:
		createFunc = MakeRadarObject;
		break;
	case NOWRADOBJECT:
		createFunc = MakeRadarObject;
		break;
	case SATOBJECT:
		createFunc = MakeSatObject;
		break;
	case BACKGROUNDIMAGEOBJECT:
		createFunc = MakeBackgroundImageObject;
		strcpy ( cbar, " " ); /* No colorbar for background objects */
		strcpy ( scale, " " ); /* No scale for background objects */
		break;
	default:
		createFunc = NULL;
		printf("*** MakeImageObjectList: Unsupported object %d\n",
				metObjectType);
		break;
	}


	if( cnt <= 0 || ! createFunc ) {
		printf("  MakeImageObjectList: got %d filenames.\n", cnt);
		return( ( MetObjectType ** ) NULL );
	}


/*
 *	Allocate an array of pointers to MetObjects.
 */
	motv = ( MetObjectType ** ) malloc( sizeof(MetObjectType *) * 
					( cnt ) );


/*
 *	Loop over all file names
 */
	for( i = 0; i < cnt; i++ ) {

/*
 *	    If the timestamp is NULL, then create this metobject with
 *	    "missing" filename and date/time
 */
 	    if ( timestamps[i] == NULL ) {
		strcpy ( file, "MISSING" );
		strcpy ( time, "MISSING" );
 	    }
	    else {
		strcpy ( file, files[i] );
		strcpy ( time, timestamps[i] );
	    }

/*
 *	    Fill in the details...
 */
	    motv[i] = (*createFunc)( metObjectType, file, time,
	                             cbar, titleIndex, scale );

/*
 *	    Die quickly and loudly if we have a failure (these are
 *	    mostly malloc() failures).
 */
	    assert( motv[i] );


	} /*   for(;;)   */

/*
 *	Return the reward...
 */
	return( motv );
}



void
ToggleImageDataSaving ( BooleanType toggled )
{
 
        GlobalDataObjectType	*gd;
        WindowObjectType	*wo;
        PixmapObjectType	**po;
        MetObjectType		**mo;
        AnyImageObjectType	*iot;
        int			i, j, verbose;
        int			metObjectType;
	extern Widget		main_shell;

	gd = GetGlobalDataObject();

	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "ToggleImageDataSaving: " );

	if ( toggled ){
	    SetSaveImageDataFlag ( gd, True );

	    if( verbose > VERBOSE_0 )
	    	printf ( "ON. Images must be reloaded for pixel readout.\n" );

            InfoPop ( main_shell, INFORMATION,
            "\nAny existing images will have to be\nreloaded for pixel value readout.\n",
            FALSE, FALSE, NULL, NULL );

	}
	else {
	    SetSaveImageDataFlag ( gd, False );
/*
 *	    Now that the value is false, go through and release all the
 *	    image data
 */
	    if ( verbose > VERBOSE_0 )
		printf ( "OFF.\n" );

	    wo = GetActiveWindowObject (); 
	    for ( i=0,po = wo->pixmapObjects; i < wo->numPixmapObjects; i++,po++ )

		for ( j=0,mo = (*po)->metObjects; j < (*po)->numMetObjects; j++ ) {
		
		    metObjectType = GetMetObjectType ( *mo );	    

		    if ( metObjectType == SATOBJECT ||
			 metObjectType == RADAROBJECT ||
			 metObjectType == NOWRADOBJECT ||
			 metObjectType == BACKGROUNDIMAGEOBJECT ) {
			 
			iot = (AnyImageObjectType *) (*mo)->metObjectContent;

			if ( iot->imgCmn.imageData ) {

			    if ( verbose > VERBOSE_0 )
				printf ( "Releasing image %d, frame %d\n",j+1,i+1);

	    		    Free ( iot->imgCmn.imageData );
	    		    iot->imgCmn.imageData = NULL;

	    		}
			
		    }
		}

	}
}
