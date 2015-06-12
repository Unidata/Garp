/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gridhobj.c
 *
 *	Plan view scalar grid object creation and destruction functions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 8/97	COMET	Added reference, forecast and valid times.
 *	 8/97	COMET	Get preferred projection from Garp_defaults.
 *	 9/97	COMET	Added model_name to object (for autoupdate)
 *	11/97	COMET	Added call to MakeMotTitle. Added model_label,
 *			field_label and titleIndex.
 *	 2/98	COMET	Added vcoord to the title
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



void DestroyHorizontalGridObject( MetObjectType *mot )
{
	if( mot && mot->metObjectContent )
		free( mot->metObjectContent );

	return;

}


MetObjectType *
MakeHorizontalScalarGridObject( char *file,		char *ftime,
	                        char *vcoord,		char *level,
				char *field,		char *gridtype,
				char *fline,		char *hilo,
				char *hlsym,		char *clrbar,
				char *skip,		char *skip_x,
				char *skip_y,		char *contur,
				char *text,		char *scale,
				char *line_color,
				char *line_type, 	char *line_width,
				char *label_freq,	char *cint,
				char *cmin,		char *cmax,
	                        char *fint,		char *fmin,
				char *fmax,		char * model_name, 
				char *model_label,	char *field_label,
				int  nfunction,         int  titleIndex,
				BooleanType do_contour,
				BooleanType do_value,	BooleanType do_fill,
				BooleanType do_symbol )
/*
 * Create a horizontal scalar grid object.
 */
{
	
	MetObjectType		*mot;
	ScalarGridObjectType	*sot;

	char	*proj, *area, *garea;
	char	*dattim;
	char	lv[GEMPAKSTRING], vc[GEMPAKSTRING];

	mot = AllocateMot();
	if ( !mot ) {
	    printf("MakeHorizontalScalarGridObject: Mot allocation error.\n");
	    return( ( MetObjectType * ) NULL );
	}

	sot = (ScalarGridObjectType *)
	                  malloc( sizeof( ScalarGridObjectType ) );
	if( ! sot ) {
	    printf("MakeHorizontalScalarGridObject: malloc error.\n");
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
	parse_gfunc ( field, lv, vc, strlen (field), sizeof (lv),
			sizeof (vc) );

/*
 *	Build title.
 */
	MakeMotTitle ( mot->titleString, dattim, model_label, lv,
	                      vc, field_label, NULL );

/*
 *	Fill in the common metObject data.
 */
	strcpy( mot->filename,  file );
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

	if ( do_fill ) 
	    mot->type         = SCALARGRIDFILLOBJECT;
	else
	    mot->type         = SCALARGRIDOBJECT;

	mot->displayFunc      = DisplayScalarGrid;
	mot->destroyFunc      = DestroyHorizontalGridObject;


/*
 *	Fill in Horizontal Grid Object Data.
 */
        if(strcspn(ftime,"/") > 6)
	   strcpy ( sot->gdatim, ftime+2 );
        else
	   strcpy ( sot->gdatim, ftime );
	strcpy ( sot->glevel, level );
	strcpy ( sot->gvcord, vcoord );	
	strcpy ( sot->gfunc, field );
	strcpy ( sot->gridtype, gridtype );
	strcpy ( sot->cint, cint );
	strcpy ( sot->cmin, cmin );
	strcpy ( sot->cmax, cmax );
	strcpy ( sot->line_color, line_color );
	strcpy ( sot->line_type, line_type );
	strcpy ( sot->line_width, line_width );
	strcpy ( sot->label_freq, label_freq );
	strcpy ( sot->text, text );
	strcpy ( sot->scale, scale );
	strcpy ( sot->hilo, hilo );
	strcpy ( sot->hlsym, hlsym );
	strcpy ( sot->clrbar, clrbar );
	strcpy ( sot->contur, contur );
	strcpy ( sot->skip_contour, skip );
	strcpy ( sot->skip_plot_x, skip_x );
	strcpy ( sot->skip_plot_y, skip_y );
	strcpy ( sot->fint, fint );
	strcpy ( sot->fmin, fmin );
	strcpy ( sot->fmax, fmax );
	strcpy ( sot->fline, fline );
	strcpy ( sot->model_name, model_name );
	strcpy ( sot->model_label, model_label );
	strcpy ( sot->field_label, field_label );
	sot->nfunction = nfunction;
	sot->contour = do_contour;
	sot->value   = do_value;
	sot->fill    = do_fill;
	sot->symbol  = do_symbol;



/*
 *	Tie the MetObjectContentType data into the metObject.
 */
	mot->metObjectContent = (MetObjectContentType *) sot;
/*
 *	Free.
 */
	Free ( proj );
	Free ( area );
	Free ( garea );
	Free ( dattim );

	return( mot );
}
