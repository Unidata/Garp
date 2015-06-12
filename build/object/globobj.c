/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	globobj.c
 *
 *	Global data object. This object comprises information relevant to 
 *	any depictable.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 3/97	COMET	Added wind position indicator.
 *	12/97	COMET	Wrote SetDefaultFont().
 *	 1/98	COMET	Fixed routine which was returning local variable
 *
 ***********************************************************************/


#include "genglobs.h"
#include "gendef.h"
#include "_proto.h"


/*
 * Make it a global, global for debugging - himes
 */
GlobalDataObjectType glob_data;

#if 0
static GlobalDataObjectType glob_data;
#endif

BooleanType GetMemCheck()
{
	return( glob_data.memCheck );
}

BooleanType SetMemCheck( BooleanType value )
{
	glob_data.memCheck = value;
}

GlobalDataObjectType *GetGlobalDataObject()
{
        return( &glob_data );
}


void
GetNextObjectColor ( GlobalDataObjectType *gd )
{
/*
 *  Get the next unused color in the depicatable color list.
 */
	int	i;
	int	initial_count;

	initial_count = gd->color_count;

/*
 *	Find the first available color in the color list.
 */

	for ( i=0; i<MAX_COLORS; i++ ) {
	    if ( !gd->color_used[i] ) {
		gd->color_used[i] = TRUE;
		strcpy ( gd->colors, gd->color_list[i] );
		gd->color_count++;
		return;
	    }
	}

/*
 *	If we ran out of colors, use the first color.
 */
	if ( initial_count == gd->color_count )
	    strcpy ( gd->colors, gd->color_list[0] );
}


void
SetColorAsUsed ( char *color )
{
/*
 *  For color indices specified explicitly by the user, these colors
 *  need to be set as used so they will not be reused by the automatic
 *  allocator.
 */
	GlobalDataObjectType	*gd;
	int			i;

	gd = GetGlobalDataObject();

/*
 *	Loop through colors and mark color as used.
 */
	for ( i=0; i<MAX_COLORS; i++ )
	    if ( strcmp ( color, gd->color_list[i] ) == 0 && 
	                                 !gd->color_used[i] ) {
		gd->color_used[i] = TRUE;
	}
}


void
SetColorList ( void )
{
/*
 *  Initialize list of colors to be used for depictables.
 */
	GlobalDataObjectType	*gd;
	char			*colors, **colorlist;
	int			i, j, numcolors;

	gd = GetGlobalDataObject();

/*
 *	Set fallback colors if not defined in configuration file.
 */
	colors = GetConfigValue( "colorlist" );

	if ( colors == NULL ) {
	    i = 0;
	    strcpy ( gd->color_list[i], "3" ); i++;
	    strcpy ( gd->color_list[i], "6" ); i++;
	    strcpy ( gd->color_list[i], "5" ); i++;
	    strcpy ( gd->color_list[i], "2" ); i++;
	    for ( j=i; j<MAX_COLORS; j++ )
		strcpy ( gd->color_list[j], "3" );
	}

	else {
	    colorlist = SplitByDelimeter ( colors, ",", &numcolors );
/*
 *	Test for overrun.
 */
	    if ( numcolors > MAX_COLORS ) numcolors = MAX_COLORS;

/*
 *	Set colors from configuration file.
 */
	    for ( i=0; i<numcolors; i++ ) 
		strcpy ( gd->color_list[i], colorlist[i] );

	    for ( i=numcolors; i<MAX_COLORS; i++ )
		strcpy ( gd->color_list[i], colorlist[0] );
	    
	    free ( colors );
	    StringListFree ( numcolors, colorlist );
	}
}


void
SetDefaultFont ( void )
{
/*
 *  Initialize default font to be used for depictables.
 */
	GlobalDataObjectType	*gd;
	char			*size, *font, *width, *flag;

	gd = GetGlobalDataObject();

/*
 *	Get default font characteristics.
 */
	size  = GetConfigValue( "text_size" );
	font  = GetConfigValue( "text_font" );
	width = GetConfigValue( "text_width" );
	flag  = GetConfigValue( "text_hw_flag" );

	if ( size  == NULL ) size = strdup ( "1.0" );
	if ( font  == NULL ) font = strdup ( "21" );
	if ( width == NULL ) width = strdup ( "1" );
	if ( flag  == NULL ) flag = strdup ( "hw" );

/*
 *	Set font characteristics in global data object.
 */
	strcpy ( gd->text_size, size );
	strcpy ( gd->text_font, font );
	strcpy ( gd->text_width, width );
	strcpy ( gd->text_hw_flag, flag );

/*
 *	Free.
 */
	Free ( size );
	Free ( font );
	Free ( width );
	Free ( flag );
}


void
ZeroObjectColors ( void )
{
/*
 *  Indicate that no colors in depictable color list are currently being used.
 */
	int			i;
	GlobalDataObjectType	*gd;

	gd = GetGlobalDataObject();

	gd->color_count = 0;
	for ( i=0; i<MAX_COLORS; i++ )
	    gd->color_used[i] = FALSE;

}


char *
GetNextWindPostition ( GlobalDataObjectType *gd )
{
/*
 *  Get the next unused wind position.
 */
	char	*buf;
	int	i;

/*
 *	Find the first available wind position in the list.
 */
	strcpy ( gd->winpos, "1" );

	for ( i=0; i<MAX_WINPOS; i++ ) {
	    if ( !gd->winpos_used[i] ) {
		gd->winpos_used[i] = TRUE;
		buf = int2str ( i+1 );
		strcpy ( gd->winpos, buf );
		return buf;
	    }
	}

	return buf;
}


void
ZeroWindPosition ( void )
{
/*
 *  Indicate that no wind positions are being used.
 */
	int			i;
	GlobalDataObjectType	*gd;

	gd = GetGlobalDataObject();

	for ( i=0; i<MAX_WINPOS; i++ )
	    gd->winpos_used[i] = FALSE;
}
