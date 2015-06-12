C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	pllgrid.f
C*
C*	Lat/lon grid drawing utility
C*
C*	History:
C*
C*	10/97	COMET	Original copy
C*
C************************************************************************


	SUBROUTINE pllgrid ( lcolor, ltype, lwidth, latinc, loninc,
     +			     lblinc, text, iret )
C************************************************************************
C
	CHARACTER*(*)	latinc, loninc, lblinc, lcolor, ltype, lwidth,
     +			text
	CHARACTER 	llgrid*80
C*
C------------------------------------------------------------------------
	iret = 0
C
C*	Set text attributes
C
	CALL IN_TEXT ( text, ier )
C
C*	Check for map mode
C
	CALL GQMODE ( mode, ier )
	IF ( mode .ne. 1 ) RETURN
C
	llgrid = lcolor // '/' // ltype // '/' // lwidth // '/' //
     +		 lblinc // '/' // latinc // ';' // loninc
C
C*	Draw the lat/lon grid
C
	CALL GG_LTLN ( llgrid, ier )
C
C*	Flush GEMPAK graphics
C
C	CALL GFLUSH ( ier )
C
	RETURN
	END
