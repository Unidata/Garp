C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	graphdraw.f
C*
C*	Graph drawing utility
C*
C*	History:
C*
C*	04/97	COMET	Original
C*	07/97	COMET	removed "(AZ/RA)" label, fixed Solaris bug
C*			(dimensioned x and ylvls to LLCLEV)
C*
C************************************************************************


	SUBROUTINE GRAPHDRAW ()
C************************************************************************
C*									*
C* This subroutine draws a graph					*
C* S. Chiswell/Unidata	03/07	new in_intc argument clbl		*
C*									*
C*									*
C************************************************************************
C*
	INCLUDE         'GEMPRM.PRM'
C*
	REAL		ylvls(LLCLEV), xlvls(LLCLEV)
	CHARACTER	ycint*80, xcint*80, label*80, clbl(LLCLEV)*24
	CHARACTER	chaz1*8, chaz2*8, chra1*8, chra2*8
	INTEGER		getimgcomint, getimgcomstr
	REAL		getimgcomflt
C------------------------------------------------------------------------
	iret = 0
c	print*,'graphdraw '
C
C*	Check for graph mode
C
	CALL GQMODE ( mode, ier )
	IF ( mode .ne. 2 ) RETURN
C
C*	Query the bounds, original image size and scaling
C
	ixsize = getimgcomint ( 'imnpix' )
	iysize = getimgcomint ( 'imnlin' )
	xyscal = getimgcomflt ( 'rmxysc' )
	xlef = 0.
	xrit = ixsize
	ybot = 0.
	ytop = iysize / xyscal
	aspect = ytop / xrit
C
C*	Convert y scale to Kft and x scale to nm and adjust the plot
C*	aspect ratio.
C
	ytop = 70
	xrit  = xrit * .54
c	aspect = aspect * (ytop / xrit)

	ycint = '10//'
	IF ( xrit .LT. 50 ) THEN
	    xcint = '5//'
	ELSE
	    xcint = '10//'
	END IF

	CALL GSGRAF  ( 1, 1, aspect, xlef, ybot, xrit, ytop, iret )

	CALL IN_INTC ( ycint, ybot, ytop, ylvls, ny, clbl, yint, ymin, 
     +			ymax, iret )
	CALL IN_INTC ( xcint, xlef, xrit, xlvls, nx, clbl, xint, xmin,
     +			xmax, iret )
  
        CALL GSCOLR ( 1, ier )
        CALL GQLINE ( ilntyp, ilntsw, ilnwid, ilnwsw, ier )
        CALL GSLINE ( 1, 0, 1, 0, ier ) 

        CALL GDAXIS ( 1, ybot, .true., 1, 1, 0, -1, nx,
     +                xlvls, ier )
        CALL GDAXIS ( 3, ytop, .true., 0, 0, 0, -1, nx,
     +                xlvls , ier )
        CALL GDAXIS ( 2, xlef, .true., 1, 1, 1, -1, ny,
     +                ylvls, ier )
        CALL GDAXIS ( 4, xrit, .true., 0, 0, 0, -1, ny,
     +                ylvls, ier )
C
C*	Do axes labelling (use Normalized coords)
C
	label = 'KFT'
	CALL GTRANS ( 'M', 'N', 1, xlef, ytop, x, y, ier )
        CALL GTEXT  ( 'N', x, y, label, 0., -13, 0, ier )	
	label = 'NM'
	CALL GTRANS ( 'M', 'N', 1, (xrit-xlef)/2, ybot, x, y, ier )
        CALL GTEXT  ( 'N', x, y, label, 0., -2, -5, ier )	
C
C*	Get and draw the azimuth/range values for the end points.
C
	numdlv = getimgcomint ( 'imndlv' )
	lena1  = getimgcomstr ( 'cmblev', numdlv + 1, chaz1 )
	lenr1  = getimgcomstr ( 'cmblev', numdlv + 2, chra1 )
	lena2  = getimgcomstr ( 'cmblev', numdlv + 3, chaz2 )
	lenr2  = getimgcomstr ( 'cmblev', numdlv + 4, chra2 )

	CALL GTRANS ( 'M', 'N', 1, xlef, ybot, x, y, ier )
c	label = '(AZ/RA) (' // chaz1 (1:lena1) // '/' //
c     +		chra1 (1:lenr1) // ')'
	label = '(' // chaz1 (1:lena1) // '/' //
     +		chra1 (1:lenr1) // ')'
	CALL ST_LSTR ( label, len, ier )
	ixoff = -len
        CALL GTEXT  ( 'N', x, y, label, 0., ixoff, -5, ier )	
	CALL GTRANS ( 'M', 'N', 1, xrit, ybot, x, y, ier )
	label = '(' // chaz2 (1:lena2) // '/' // chra2 (1:lenr2) // ')'
	CALL ST_LSTR ( label, len, ier )
	ixoff = -len
        CALL GTEXT  ( 'N', x, y, label, 0., ixoff, -5, ier )	
C
C*	Switch back to map mode. (Set the aspect ratio back to 0
C*	first; for some reason it was messing up subsequent plots.)
C
	CALL GSGRAF ( 1, 1, 0., xlef, ybot, xrit, ytop, iret )
	CALL GSMODE ( 1, ier )
C
C*	Flush GEMPAK graphics
C
C	CALL GFLUSH ( ier )
C
	RETURN
	END
