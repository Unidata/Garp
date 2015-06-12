/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	vecthobj.c
 *
 *	Plan view vector object creation and destruction functions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 8/97	COMET	Added reference, forecast and valid times.
 *	 8/97	COMET	Get preferred projection from Garp_defaults.
 *	 9/97	COMET	Added model_name to object (for autoupdate)
 *	11/97	COMET	Added call to MakeMotTitle. Added model_label,
 *			field_label and titleIndex.
 *	 3/98	COMET	Added call to parse_gfunc()
 *
 ***********************************************************************/


#include "winobj.h"
#include "guimacros.h"
#include "underscore.h"
#include "_proto.h"


char * GetModelConfigValue ( char * );
char * GetConfigValue ( char * );
char * MakeForecastDattim ( char * );



void DestroyHorizontalVectorGridObject( MetObjectType *mot )
{
	if( mot && mot->metObjectContent )
		free( mot->metObjectContent );

	return;

}


MetObjectType *
MakeHorizontalVectorGridObject( char *file,		char *ftime,
	                        char *vcoord,		char *level,
				char *vector,		char *gridtype,
				char *text,
	                        char *wind_symbol,	char *symbol_color,
				char *symbol_size,	char *symbol_width,
				char *symbol_type, 	char *symbol_headsize,
				char *refvec,		char *scale,
				char *skip,
				char *skip_x,		char *skip_y,
				char *model_name,	char *model_label,
				char *field_label,      int  nfunction,
				int titleIndex )
/*
 * Create a horizontal vector Met object.
 */
{
	
	char			template[512];
	MetObjectType		*mot;
	VectorGridObjectType	*vot;

	char	*proj, *area, *garea;
	char	*dattim;
	char	lv[GEMPAKSTRING], vc[GEMPAKSTRING];

	mot = AllocateMot();
	if ( !mot ) {
	    printf("MakeHorizontalVectorGridObject: Mot allocation error.\n");
	    return( ( MetObjectType * ) NULL );
	}

	vot = (VectorGridObjectType *)
	                  malloc( sizeof( VectorGridObjectType ) );
	if( ! vot ) {
	    printf("MakeHorizontalVectorGridObject: malloc error.\n");
	    return( ( MetObjectType * ) NULL );
	}

/*
 *	Get projection information.
 */
	proj  = GetModelConfigValue ( "_proj" );
	if ( proj == NULL ) proj = GetConfigValue ( "default_proj" );
	if ( proj == NULL ) proj = strdup ( "lcc/30.0;-100;50.0//nm" );

	garea = GetModelConfigValue ( "_garea" );
	if ( garea == NULL ) garea = GetConfigValue ( "default_garea" );
	if ( garea == NULL ) garea = strdup ( "23.0;-120.0;47.0;-65.0" );

	area  = strdup ( garea );

/*
 *	Set valid date/time for title.
 */
	dattim = MakeForecastDattim ( ftime );

/*
 *	Parse the gfunc to see if there are any in-line modifiers
 *	we should use in the title
 */
	strcpy ( lv, level );
	strcpy ( vc, vcoord );
	parse_gfunc ( vector, lv, vc, strlen (vector), sizeof (lv),
			sizeof (vc) );

/*
 *	Build title.
 */
	MakeMotTitle ( mot->titleString, dattim, model_label, lv,
                              vc, field_label, NULL );
	
/*
 *	Fill in the common metObject data.
 */
	strcpy ( mot->filename,	file );
	strcpy( mot->area,	area );
	strcpy( mot->garea,	garea );
	strcpy( mot->proj,	proj );
	strcpy( mot->timestamp, ftime );

	mot->refTime          = String2LongTime( ftime, &(mot->fcstTime) );
	mot->validTime        = mot->refTime + mot->fcstTime;
	mot->drawnFlag        = False;
	mot->inUse            = True;
	mot->toggledOn        = True;
	mot->titleIndex       = titleIndex;
	mot->type             = VECTORGRIDOBJECT;
	mot->displayFunc      = DisplayVectorGrid;
	mot->destroyFunc      = DestroyHorizontalVectorGridObject;

/*
 *	Fill in Horizontal Vector Object Data.
 */
        if(strcspn(ftime,"/") > 6)
	   strcpy ( vot->gdatim, ftime+2 );
        else
	   strcpy ( vot->gdatim, ftime );
	strcpy ( vot->glevel, level );
	strcpy ( vot->gvcord, vcoord );	
	strcpy ( vot->gvect,  vector );
	strcpy ( vot->gridtype, gridtype );
	strcpy ( vot->text,   text );
	strcpy ( vot->refvec, refvec );
	strcpy ( vot->scale, scale );
	strcpy ( vot->skip_contour, skip );
	strcpy ( vot->skip_plot_x, skip_x );
	strcpy ( vot->skip_plot_y, skip_y );
	strcpy ( vot->wind_symbol,     wind_symbol );
	strcpy ( vot->symbol_color,    symbol_color );
	strcpy ( vot->symbol_size,     symbol_size );
	strcpy ( vot->symbol_width,    symbol_width );
	strcpy ( vot->symbol_type,     symbol_type );
	strcpy ( vot->symbol_headsize, symbol_headsize );
	strcpy ( vot->model_name, model_name );
	strcpy ( vot->model_label, model_label );
	strcpy ( vot->field_label, field_label );
	vot->nfunction = nfunction;



/*
 *	Tie the MetObjectContentType data into the metObject.
 */
	mot->metObjectContent = (MetObjectContentType *) vot;
/*
 *	Free.
 */
	Free ( proj );
	Free ( area );
	Free ( garea );
	Free ( dattim );

	return( mot );
}
