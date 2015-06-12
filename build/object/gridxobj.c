/***********************************************************************
 *
 *	Copyright 1996, University Corporation for Atmospheric Research.
 *
 *	gridxobj.c
 *
 *	Cross section object creation and destruction functions.
 *
 *	History:
 *
 *	11/96	COMET	Original copy
 *	 3/97	COMET	Added XXSECTIONGRIDFILLOBJECT
 *	 8/97	COMET	Added reference, forecast and valid times.
 *	 9/97	COMET	Added model_name to object (for autoupdate)
 *	11/97	COMET	Added call to MakeMotTitle. Added model_label,
 *			field_label and titleIndex.
 *
 ***********************************************************************/


#include "winobj.h"
#include "guimacros.h"
#include "_proto.h"

char * MakeForecastDattim ( char * );



void DestroyXSectionGridObject( MetObjectType *mot )
{
	if( mot && mot->metObjectContent )
		free( mot->metObjectContent );

	return;

}


MetObjectType *
MakeXSectionGridObject( char *file,		char *ftime,
                        char *vcoord,
			char *func,		char *vect,
			char *wind_symbol,	char *symbol_size,
			char *symbol_width,	char *symbol_type,
			char *symbol_headsize,	char *cxstns,
			char *fline,		char *clrbar,
			char *skip,		char *contur,
			char *text,		char *scale,
			char *line_color,
			char *line_type, 	char *line_width,
			char *label_freq,	char *cint,
			char *cmin,		char *cmax,
                        char *fint,		char *fmin,
			char *fmax,
			char *yaxis,		char *ptype,
			char *refvec,		char *model_name,
			char *model_label, 	char *field_label,
			int  titleIndex,
			BooleanType do_contour,
			BooleanType do_fill )
/*
 * Create a vertical cross section grid object.
 */
{
	
	MetObjectType		*mot;
	XSectionGridObjectType	*xot;

	char	area[81], garea[81], proj[81];
	char	*dattim;

	mot = AllocateMot();
	if ( !mot ) {
	    printf("MakeXSectionGridObject: Mot allocation error.\n");
	    return( ( MetObjectType * ) NULL );
	}

	xot = (XSectionGridObjectType *)
	                  malloc( sizeof( XSectionGridObjectType ) );
	if( ! xot ) {
	    printf("MakeXSectionGridObject: malloc error.\n");
	    return( ( MetObjectType * ) NULL );
	}

/*
 *	Get projection information.
 */
	strcpy ( proj, "XSEC" );
	strcpy ( garea, "" );
	strcpy ( area, "" );

/*
 *	Set valid date/time for title.
 */
	dattim = MakeForecastDattim ( ftime );

/*
 *	Build title.
 */
	MakeMotTitle ( mot->titleString, dattim, model_label, NULL,
                              NULL, field_label, NULL );

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
		mot->type     = XSECTIONGRIDFILLOBJECT;
	else
		mot->type     = XSECTIONGRIDOBJECT;

	mot->displayFunc      = DisplayXSection;
	mot->destroyFunc      = DestroyXSectionGridObject;

/*
 *	Fill in Horizontal Grid Object Data.
 */
        if(strcspn(ftime,"/") > 6)
	   strcpy ( xot->gdatim, ftime+2 );
        else
	   strcpy ( xot->gdatim, ftime );
	strcpy ( xot->gvcord, vcoord );	
	strcpy ( xot->gfunc, func );
	strcpy ( xot->gvect, vect );
	strcpy ( xot->wind_symbol, wind_symbol );
	strcpy ( xot->symbol_size, symbol_size );
	strcpy ( xot->symbol_width, symbol_width );
	strcpy ( xot->symbol_type, symbol_type );
	strcpy ( xot->symbol_headsize, symbol_headsize );
	strcpy ( xot->cxstns, cxstns );
	strcpy ( xot->cint, cint );
	strcpy ( xot->cmin, cmin );
	strcpy ( xot->cmax, cmax );
	strcpy ( xot->line_color, line_color );
	strcpy ( xot->line_type, line_type );
	strcpy ( xot->line_width, line_width );
	strcpy ( xot->label_freq, label_freq );
	strcpy ( xot->text, text );
	strcpy ( xot->scale, scale );
	strcpy ( xot->clrbar, clrbar );
	strcpy ( xot->contur, contur );
	strcpy ( xot->skip, skip );
	strcpy ( xot->fint, fint );
	strcpy ( xot->fmin, fmin );
	strcpy ( xot->fmax, fmax );
	strcpy ( xot->fline, fline );
	strcpy ( xot->yaxis, yaxis );
	strcpy ( xot->ptype, ptype );
	strcpy ( xot->refvec, "" );
	strcpy ( xot->model_name, model_name );
	strcpy ( xot->model_label, model_label );
	strcpy ( xot->field_label, field_label );
	xot->contour = do_contour;
	xot->fill    = do_fill;

/*
 *	Tie the MetObjectContentType data into the metObject.
 */
	mot->metObjectContent = (MetObjectContentType *) xot;
/*
 *	Free.
 */
	Free ( dattim );

	return( mot );
}
