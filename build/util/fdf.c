/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	fdf.c
 *
 *	FDF utilities to save a keylist and build a GEMPAK parameter.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 3/97	COMET	Added TAXIS parameter to time/height FDFs.
 *	12/97	COMET	Fixed small leak (fdf_template)
 *	 2/97	COMET	Amended GetMacro to allow for model specific macros.
 *	 4/98	COMET	Fixed leak in GetFDF()
 *	 8/99	COMET	Redefined FDF model directory.
 *
 ***********************************************************************/
 
#include "geminc.h"
#include "gemprm.h" 
#include "utils.h"
#include "ctbcmn.h"
#include "guimacros.h"
#include "fdfobj.h"
#include "hsobj.h"
#include "hvobj.h"
#include "model.h"
#include "genglobs.h"
#include "winobj.h"
#include "menucb.h"
#include "_proto.h"

void GetModelVectorCB();
void GridDataCB();



int
SaveFdfObject ( KeyListType *kl, KeyListType *fdf )
/*
 *	Check input keylist to assure required parameters are defined.
 *	If so, save the input FDF into a static keylist.
 */
{
	char		*type, *layer, *function, *description;
	int		verbose;

	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "SaveFdfObject\n" );
/*
 *	Check for required parameters.
 */
	type		= GetFdfKeyValue ( kl, "type" );
	if ( type == NULL ) {
	    printf ( "*** SaveFdfObject - FDF key 'type' is undefined\n" );
	    return -1;
	}
	Free ( type );

/*
 *	If the keylist is a macro, don't need to check for "layer" or
 *	"function".
 */
	type            = GetFdfKeyValue ( kl, "type" );
	ToUpper ( type );
	if ( strcmp ( type, "MACRO" ) != 0 ) {
	    layer		= GetFdfKeyValue ( kl, "layer" );
	    if ( layer == NULL ) {
	    	printf ( 
		  "*** SaveFdfObject - FDF key 'layer' is undefined\n" );
		Free ( type );
	    	return -2;
	    }
	    Free ( layer );

	    function	= GetFdfKeyValue ( kl, "function" );
	    if ( function == NULL ) {
	    	printf ( 
		  "*** SaveFdfObject - FDF key 'function' is undefined\n" );
		Free ( type );
	    	return -3;
	    }
	    Free ( function );
	}
	Free ( type );

	description	= GetFdfKeyValue ( kl, "description" );
	if ( description == NULL ) {
	    printf ( "*** SaveFdfObject - FDF key 'description' is undefined\n" );
	    return -4;
	}
	Free ( description );

	if( verbose > VERBOSE_1 )
	    printf ( "  SaveFdfObject - calling CopyFdf\n" );

	CopyFdf ( kl, fdf );

	if( verbose > VERBOSE_1 )
	    printf ( "  SaveFdfObject - returned from CopyFdf\n" );

	return 0;
}


int
GetFDF ( char *file, char *path, char *subdir, char *model, KeyListType *fdf )
/*
 * Read and save and FDF.
 */
{
	KeyListType	*dl, *kl, *ml;
	char		*infile, *mdlfile;
	char		*fdf_template;
	int		len, verbose;
	int		err = NORMAL;


	verbose = GetVerboseLevel();
	if( verbose > VERBOSE_0 )
	    printf ( "GetFDF\n" );

/*
 *	Initialize keylists.
 */
	ml = NULL;
	kl = NULL;

/*
 *	Get template file defining valid keys.
 */
	fdf_template = GetConfigValue( "valid_fdf_keys" );
	if ( fdf_template == NULL ) {
	    printf ( "*** Error in GetFDF - FDF valid_fdf_keys is null\n" );
	    return -1;
	}

        len = strlen (path)+strlen(file)+strlen(subdir)+strlen(model)+100;

	infile  = (char *) malloc ( len * sizeof (char) );
	mdlfile = (char *) malloc ( len * sizeof (char) );

	sprintf ( infile, "%s%s%s%s%s%s%s", path, "/",
	     "default", "/", subdir, "/", file );
	sprintf ( mdlfile, "%s%s%s%s%s%s%s", path, "/",
	     model, "/", subdir, "/", file );

/*
 *	Read in template file.
 */
	dl = ReadFDF( fdf_template, 0, OVERWRITE );
	if ( dl == NULL ) {
	    printf ("*** Error in GetFDF - Could not read:\n%s\n",
							 fdf_template );
	    err = -1;
	}
/*
 *	Read in Field Description file. Look for a model specific version
 *	first.
 */
	else {
	    ml = ReadFDF( mdlfile, dl, OVERWRITE );
	    if ( ml == NULL ) {
		kl = ReadFDF( infile, dl, OVERWRITE );
		if ( kl == NULL ) {
		    if( verbose > VERBOSE_1 )
		        printf (
			"*** Possible Error in GetFDF - Could not read:\n%s\n",
							 infile );
		    err = -2;
		}
		else {
		    strcpy ( kl->fileName, file );
		    strcpy ( kl->path,     path );
		}
	    }
	    else {
		strcpy ( ml->fileName, file );
		strcpy ( ml->path,     path );
	    }
	}


	if ( dl != NULL ) {
/*
 *	    Save FDF object.
 */
	    if ( ml != NULL )
	        err = SaveFdfObject ( ml, fdf );
	    else if ( kl != NULL )
		err = SaveFdfObject ( kl, fdf );
	}

	if ( dl != NULL ) DestroyFDF ( dl );
	if ( kl != NULL ) DestroyFDF ( kl );
	if ( ml != NULL ) DestroyFDF ( ml );

	Free ( infile );
	Free ( mdlfile );
	Free ( fdf_template );

	return err;
}


BooleanType
CheckForMacro ( KeyListType *fdf )
{
/*
 * Look at the first key in an FDF to see if it defines a macro.
 */
	char		*buf;
	BooleanType	retVal;

	retVal = FALSE;

	buf = GetFdfKeyValue ( fdf, "type" );
	ToUpper ( buf );
	if ( buf != NULL ) {
	    if ( strcmp ( buf, "MACRO" ) == 0 )
		retVal = TRUE;

	    Free ( buf );
	}

	return retVal;
} 


int
GetMacro ( char *file, char *path, char *subdir,
	   char *model, KeyListType **macro )
/*
 * Read and save and KeyList.
 */
{
	KeyListType	*ml, *kl;

	char		*mdlfile, *infile;
	int		len, err=0;

/*
 *	Initialize keylist.
 */
	ml = NULL;
	kl = NULL;

/*
 *	Build absolute file names.
 */
        len = strlen (path)+strlen(file)+strlen(subdir)+strlen(model)+100;

        infile  = (char *) malloc ( len * sizeof (char) );
        mdlfile = (char *) malloc ( len * sizeof (char) );

        sprintf ( infile, "%s%s%s%s%s%s%s", path, "/",
             "default", "/", subdir, "/", file );
        sprintf ( mdlfile, "%s%s%s%s%s%s%s", path, "/",
             model, "/", subdir, "/", file );

/*
 *	Read in Field Description file. Look for a model specific version
 *	first.
 */
	ml = ReadFDF ( mdlfile, NULL, APPEND );
	if ( ml == NULL ) {
	    kl = ReadFDF ( infile, NULL, APPEND );
	    if ( kl == NULL ) {
	        printf ("*** Error in GetMacro - Could not read:\n%s\n",
		                infile );
		err = -1;
	    }
	    else {
		strcpy ( kl->fileName, file );
		strcpy ( kl->path,     path );
	    }
	}
	else {
	    strcpy ( ml->fileName, file );
	    sprintf ( ml->path, "%s%s%s%s%s", path, "/", model, "/", subdir );
	}

/*
 *	Save macro object.
 */
	if ( ml != NULL )
	    *macro = ml;
	else if ( kl != NULL )
	    *macro = kl;

	Free ( mdlfile );
	Free ( infile );

	return err;
}


char *
BuildGemParm ( char *parm, KeyListType *fdf )
{
	ModelInfoType	*mdl;

	char		buf[1024], *gemparm, *parms[20];
	int		i, nparms;
	int		iret, verbose;

	mdl = GetGuiModelInfo();
	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "BuildGemParm\n" );

	/* HILO	*/
	if ( strcmp ( parm, "hilo" ) == 0 ) {
	    nparms = 10;
	    parms[0] = GetFdfKeyValue ( fdf, "colorh" );
	    parms[1] = GetFdfKeyValue ( fdf, "colorl" );
	    parms[2] = GetFdfKeyValue ( fdf, "symbolh" );
	    parms[3] = GetFdfKeyValue ( fdf, "symboll" );
	    parms[4] = GetFdfKeyValue ( fdf, "rangeh" );
	    parms[5] = GetFdfKeyValue ( fdf, "rangel" );
	    parms[6] = GetFdfKeyValue ( fdf, "radius" );
	    parms[7] = GetFdfKeyValue ( fdf, "counth" );
	    parms[8] = GetFdfKeyValue ( fdf, "countl" );
	    parms[9] = GetFdfKeyValue ( fdf, "interp" );
/*
 *	Test for NULL strings.
 */
	    for ( i=0; i<nparms; i++ )
		if ( parms[i] == NULL )
			parms[i] = strdup ( "" );

	    sprintf ( buf, "%s;%s/%s;%s/%s;%s/%s/%s;%s/%s",
			parms[0], parms[1], parms[2], parms[3],
	                parms[4], parms[5], parms[6], parms[7],
			parms[8], parms[9] );
	}

	/* HLSYM */
	else if ( strcmp ( parm, "symbol" ) == 0 ) {
	    nparms = 9;
	    parms[0] = GetFdfKeyValue ( fdf, "sizes" );
	    parms[1] = GetFdfKeyValue ( fdf, "sizev" );
	    parms[2] = GetFdfKeyValue ( fdf, "position" );
	    parms[3] = GetFdfKeyValue ( fdf, "fonts" );
	    parms[4] = GetFdfKeyValue ( fdf, "fontv" );
	    parms[5] = GetFdfKeyValue ( fdf, "widths" );
	    parms[6] = GetFdfKeyValue ( fdf, "widthv" );
	    parms[7] = GetFdfKeyValue ( fdf, "hwflgs" );
	    parms[8] = GetFdfKeyValue ( fdf, "hwflgv" );
/*
 *	Test for NULL strings.
 */
	    for ( i=0; i<nparms; i++ )
		if ( parms[i] == NULL )
			parms[i] = strdup ( "" );

	    sprintf ( buf, "%s;%s/%s/%s;%s/%s;%s/%s;%s",
			parms[0], parms[1], parms[2], parms[3],
	                parms[4], parms[5], parms[6], parms[7],
			parms[8] );
	}

	/* CLRBAR */
	else if ( strcmp ( parm, "colorbar" ) == 0 ) {
	    nparms = 8;
	    parms[0] = GetFdfKeyValue ( fdf, "colorbar" );
	    parms[1] = GetFdfKeyValue ( fdf, "colorbar_orientation" );
	    parms[2] = GetFdfKeyValue ( fdf, "colorbar_anchor" );
	    parms[3] = GetFdfKeyValue ( fdf, "colorbar_x" );
	    parms[4] = GetFdfKeyValue ( fdf, "colorbar_y" );
	    parms[5] = GetFdfKeyValue ( fdf, "colorbar_length" );
	    parms[6] = GetFdfKeyValue ( fdf, "colorbar_width" );
	    parms[7] = GetFdfKeyValue ( fdf, "colorbar_frequency" );
/*
 *	Test for NULL strings.
 */
	    for ( i=0; i<nparms; i++ )
		if ( parms[i] == NULL )
			parms[i] = strdup ( "" );

	    sprintf ( buf, "%s/%s/%s/%s;%s/%s;%s/%s",
			parms[0], parms[1], parms[2], parms[3],
	                parms[4], parms[5], parms[6], parms[7] );
	}

	/* CONTUR */
	else if ( strcmp ( parm, "contour" ) == 0 ) {
	    nparms = 2;
	    parms[0] = GetFdfKeyValue ( fdf, "subbox" );
	    parms[1] = GetFdfKeyValue ( fdf, "smoothing" );
/*
 *	Test for NULL strings.
 */
	    for ( i=0; i<nparms; i++ )
		if ( parms[i] == NULL )
			parms[i] = strdup ( "" );

	    sprintf ( buf, "%s/%s", parms[0], parms[1] );
	}

	/* TEXT */
	else if ( strcmp ( parm, "text" ) == 0 ) {
	    nparms = 4;
	    parms[0] = GetFdfKeyValue ( fdf, "text_size" );
	    parms[1] = GetFdfKeyValue ( fdf, "text_font" );
	    parms[2] = GetFdfKeyValue ( fdf, "text_width" );
	    parms[3] = GetFdfKeyValue ( fdf, "text_hw_flag" );
/*
 *	Test for NULL strings.
 */
	    for ( i=0; i<nparms; i++ )
		if ( parms[i] == NULL )
			parms[i] = strdup ( "" );

	    sprintf ( buf, "%s/%s/%s/%s", parms[0],
			parms[1], parms[2], parms[3], parms[4] );
	}

	/* SCALE */
	else if ( strcmp ( parm, "scale" ) == 0 ) {
	    nparms = 1;
	    parms[0] = GetFdfKeyValue ( fdf, "scale" );
/*
 *	Test for NULL strings.
 */
	    for ( i=0; i<nparms; i++ )
		if ( parms[i] == NULL )
			parms[i] = strdup ( "" );

	    sprintf ( buf, "%s", parms[0] );
	}

	/* FLINE */
	else if ( strcmp ( parm, "fline" ) == 0 ) {
	    nparms = 1;
	    parms[0] = GetFdfKeyValue ( fdf, "fline" );
/*
 *	Test for NULL strings.
 */
	    for ( i=0; i<nparms; i++ )
		if ( parms[i] == NULL )
			parms[i] = strdup ( "" );

	    sprintf ( buf, "%s", parms[0] );
	}

	/* LINE */
	else if ( strcmp ( parm, "" ) == 0 ) {
	    nparms = 4;
	    parms[0] = GetFdfKeyValue ( fdf, "line_color" );
	    parms[1] = GetFdfKeyValue ( fdf, "line_type" );
	    parms[2] = GetFdfKeyValue ( fdf, "line_width" );
	    parms[3] = GetFdfKeyValue ( fdf, "line_label_frequency" );

	}

	/* LEVEL */
	else if ( strcmp ( parm, "level" ) == 0 ) {
	    nparms = 3;
	    parms[0] = strdup ( GetModelLevel1 ( mdl ) );
	    parms[1] = strdup ( GetModelLevel2 ( mdl ) );
	    parms[2] = GetFdfKeyValue ( fdf, "layer" );
/*
 *	Test for NULL strings.
 */
	    for ( i=0; i<nparms; i++ )
		if ( parms[i] == NULL )
			parms[i] = strdup ( "" );
/*
 *	If level1 = "all", mark is so and indicate level or layer field.
 */
	    if ( strcasecmp ( parms[0], "all" ) == 0 ) {
		if ( strcasecmp ( parms[2], "y" ) == 0 )
		    strcpy ( buf, "all_layer" );
		else
		    strcpy ( buf, "all_level" );
	    }
	    else {
/*
 *	If it is a layer, write 2 levels. Otherwise, write one level.
 */
	    	if (  strcasecmp ( parms[2], "y" ) == 0 )
		    sprintf ( buf, "%s:%s", parms[0], parms[1] );
	    	else
		    strcpy ( buf, parms[0] );
	    }
	}

	/* REFVEC */
	else if ( strcmp ( parm, "refvec" ) == 0 ) {
	    nparms = 8;
	    parms[0] = GetFdfKeyValue ( fdf, "refvec_mag" );
	    parms[1] = GetFdfKeyValue ( fdf, "refvec_x" );
	    parms[2] = GetFdfKeyValue ( fdf, "refvec_y" );
	    parms[3] = GetFdfKeyValue ( fdf, "refvec_textsize" );
	    parms[4] = GetFdfKeyValue ( fdf, "refvec_font" );
	    parms[5] = GetFdfKeyValue ( fdf, "refvec_width" );
	    parms[6] = GetFdfKeyValue ( fdf, "refvec_hw_flag" );
	    parms[7] = GetFdfKeyValue ( fdf, "refvec_label" );
/*
 *	Test for NULL strings.
 */
	    for ( i=0; i<nparms; i++ )
		if ( parms[i] == NULL )
			parms[i] = strdup ( "" );
	    
	    sprintf ( buf, "%s;%s;%s;%s/%s/%s/%s;%s",
			parms[0], parms[1], parms[2], parms[3],
			parms[4], parms[5], parms[6], parms[7] );
	}

	/* PTYPE */
	else if ( strcmp ( parm, "ptype" ) == 0 ) {
	    nparms = 6;
	    parms[0] = GetFdfKeyValue ( fdf, "ptype_axis" );
	    parms[1] = strdup ( "0" );
	    parms[2] = GetFdfKeyValue ( fdf, "ptype_left" );
	    parms[3] = GetFdfKeyValue ( fdf, "ptype_bottom" );
	    parms[4] = GetFdfKeyValue ( fdf, "ptype_right" );
	    parms[5] = GetFdfKeyValue ( fdf, "ptype_top" );
/*
 *	Test for NULL strings.
 */
	    for ( i=0; i<nparms; i++ )
		if ( parms[i] == NULL )
			parms[i] = strdup ( "" );

	    sprintf ( buf, "%s/%s/%s;%s;%s;%s",
			parms[0], parms[1], parms[2],
			parms[3], parms[4], parms[5] );
	}

	/* XAXIS */
	else if ( strcmp ( parm, "xaxis" ) == 0 ) {
	    nparms = 6;
	    parms[0] = GetFdfKeyValue ( fdf, "xaxis_lower" );
	    parms[1] = GetFdfKeyValue ( fdf, "xaxis_upper" );
	    parms[2] = GetFdfKeyValue ( fdf, "xaxis_increment" );
	    parms[3] = GetFdfKeyValue ( fdf, "xaxis_lbfq" );
	    parms[4] = GetFdfKeyValue ( fdf, "xaxis_gdfq" );
	    parms[5] = GetFdfKeyValue ( fdf, "xaxis_tkfq" );
/*
 *	Test for NULL strings.
 */
	    for ( i=0; i<nparms; i++ )
		if ( parms[i] == NULL )
			parms[i] = strdup ( "" );

	    sprintf ( buf, "%s/%s/%s/%s;%s;%s",
			parms[0], parms[1], parms[2],
			parms[3], parms[4], parms[5] );
	}

	/* YAXIS */
	else if ( strcmp ( parm, "yaxis" ) == 0 ) {
	    nparms = 6;
	    parms[0] = GetFdfKeyValue ( fdf, "yaxis_lower" );
	    parms[1] = GetFdfKeyValue ( fdf, "yaxis_upper" );
	    parms[2] = GetFdfKeyValue ( fdf, "yaxis_increment" );
	    parms[3] = GetFdfKeyValue ( fdf, "yaxis_lbfq" );
	    parms[4] = GetFdfKeyValue ( fdf, "yaxis_gdfq" );
	    parms[5] = GetFdfKeyValue ( fdf, "yaxis_tkfq" );
/*
 *	Test for NULL strings.
 */
	    for ( i=0; i<nparms; i++ )
		if ( parms[i] == NULL )
			parms[i] = strdup ( "" );

	    sprintf ( buf, "%s/%s/%s/%s;%s;%s",
			parms[0], parms[1], parms[2],
			parms[3], parms[4], parms[5] );
	}

	/* TAXIS */
	else if ( strcmp ( parm, "taxis" ) == 0 ) {
	    nparms = 7;
	    parms[0] = GetFdfKeyValue ( fdf, "taxis_dir" );
	    parms[1] = GetFdfKeyValue ( fdf, "taxis_start" );
	    parms[2] = GetFdfKeyValue ( fdf, "taxis_stop" );
	    parms[3] = GetFdfKeyValue ( fdf, "taxis_inc" );
	    parms[4] = GetFdfKeyValue ( fdf, "taxis_lb" );
	    parms[5] = GetFdfKeyValue ( fdf, "taxis_gl" );
	    parms[6] = GetFdfKeyValue ( fdf, "taxis_tm" );
/*
 *	Test for NULL strings.
 */
	    for ( i=0; i<nparms; i++ )
		if ( parms[i] == NULL )
			parms[i] = strdup ( "" );

	    sprintf ( buf, "%s%s-%s-%s;%s;%s;%s",
			parms[0], parms[1], parms[2],
			parms[3], parms[4], parms[5],
			parms[6] );
	}

	/* No Match */
	else
	    return NULL;

/*
 *	Free.
 */
	for ( i=0; i<nparms; i++ ) free ( parms[i] );

/*
 *	Return value.
 */
	gemparm = strdup ( buf );
	return gemparm;

}

/*
	    parms[0] = GetFdfKeyValue ( fdf, "" );
	    parms[1] = GetFdfKeyValue ( fdf, "" );
	    parms[2] = GetFdfKeyValue ( fdf, "" );
	    parms[3] = GetFdfKeyValue ( fdf, "" );
	    parms[4] = GetFdfKeyValue ( fdf, "" );
	    parms[5] = GetFdfKeyValue ( fdf, "" );
	    parms[6] = GetFdfKeyValue ( fdf, "" );
	    parms[7] = GetFdfKeyValue ( fdf, "" );
	    parms[8] = GetFdfKeyValue ( fdf, "" );
	    parms[9] = GetFdfKeyValue ( fdf, "" );

*/



char *
BuildGemFunction ( char *parm, char *gridtype, KeyListType *fdf )
{
	char		buf[1024], oneparm[80], *parms[20];
	char		*str, *gemparm;
	int		i, nparms;
	int		iret, verbose;

	verbose = GetVerboseLevel();

	if( verbose > VERBOSE_0 )
	    printf ( "BuildGemFunction\n" );
	/* FUNCTION */
	if ( strcmp ( parm, "function" ) == 0 ) {
	    nparms = 10;
	    parms[0] = GetFdfKeyValue ( fdf, "function" );
	    parms[1] = GetFdfKeyValue ( fdf, "function2" );
	    parms[2] = GetFdfKeyValue ( fdf, "function3" );
	    parms[3] = GetFdfKeyValue ( fdf, "function4" );
	    parms[4] = GetFdfKeyValue ( fdf, "function5" );
	    parms[5] = GetFdfKeyValue ( fdf, "function6" );
	    parms[6] = GetFdfKeyValue ( fdf, "function7" );
	    parms[7] = GetFdfKeyValue ( fdf, "function8" );
	    parms[8] = GetFdfKeyValue ( fdf, "function9" );
	    parms[9] = GetFdfKeyValue ( fdf, "function10" );
/*
 *	Write functions into a string.
 */
	    strcpy ( buf, " " );
	    strcpy ( gridtype, " " );

	    for ( i=0; i<nparms; i++ ) {
		if ( parms[i] != NULL ) {
/*
 *	Save information which shows if this is a scalar or vector grid.
 */
		    if      ( *parms[i] == 's' ) 
			strcat ( gridtype, "s " );
		    else if ( *parms[i] == 'v' )
			strcat ( gridtype, "v " );

		    str = strdelupto2 ( parms[i], '=' );
		    strcpy ( oneparm, str );
		    if ( str != NULL ) free ( str );
		    bpad ( oneparm, 73, &iret );
		    strcat ( buf, oneparm );
		}
	    }
	}
	/* No Match */
	else
	    return NULL;

/*
 *	Free.
 */
	for ( i=0; i<nparms; i++ ) free ( parms[i] );

/*
 *	Return value.
 */
	gemparm = strdup ( buf );
	return gemparm;

}
