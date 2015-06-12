C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	dataarea.f
C*
C*	Utility for getting a more broad data area. (Needed because 
C*	data data can get "cut off" in some projections.)
C*
C*	History:
C*
C*	10/97	COMET	Original copy
C*
C************************************************************************


	SUBROUTINE GET_DATA_AREA ( area, iret )
C************************************************************************
C************************************************************************
	CHARACTER	area*(*)
C*
C------------------------------------------------------------------------
	iret = 0
C
C*	Get the lat/lon extremes, set area to DSET if the area
C*	looks odd
C
	CALL GQBND ( 'M', rlat1, rlon1, rlat2, rlon2, iret )
	IF ( rlon1 .gt.  180. ) rlon1 = rlon1 - 360.
	IF ( rlon2 .gt.  180. ) rlon2 = rlon2 - 360.
	IF ( rlon1 .lt. -180. ) rlon1 = rlon1 + 360.
	IF ( rlon2 .lt. -180. ) rlon2 = rlon2 + 360.
C
	IF ( rlon1 .gt. rlon2 ) THEN
	    area = 'DSET'
	ELSE
	    WRITE ( area, '(4(F10.2,A))')rlat1,';',rlon1,';',
     &						  rlat2,';',rlon2
	    CALL ST_RXBL ( area, area, lens, iret )
	END IF
C
	RETURN
	END
