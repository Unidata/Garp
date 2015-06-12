C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	wprofbor.f
C*
C*	Used by wind profiler function to draw background. Derived from 
C*	the GEMPAK program SNSBOR.
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*
C************************************************************************


	SUBROUTINE wprofbor ( border, ybot, ytop, yaxval, nyval, iylbfr, 
     +			      iyglfr, iytmfr, xmin, xmax, nstn, stns, 
     +			      sloc, x, pontha, toptmp, topwnd, xtlbl,
     +			      ctlbl, nxlbl, ixlbfr, ixglfr, ixtmfr, 
     +			      timflg, iret )
C************************************************************************
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
C*
	CHARACTER*(*)	border, stns (*), ctlbl (*)
	REAL		sloc(*), x(*), pontha(40,*), toptmp(*),
     +			topwnd(*), yaxval (*), xtlbl (*)
	LOGICAL		timflg, scflag
C*
	REAL		xx (3), yy (3)
	REAL		smooth, filter
csd	INCLUDE		'ERMISS.FNC'
C------------------------------------------------------------------------
	iret = 0
	values = 0.
C
C*	Get input values; the "surface" line is set to the background
C*	color.
C
	CALL IN_LINE  ( border, values, 1, icbor, ibtyp, ibwid, iblab,
     +                  smooth, filter, scflag, ier )
C
C*	Draw background.
C
	IF  ( icbor .ne. 0 ) THEN
C
C*	    Set background color and line attributes.
C
	    CALL GSCOLR  ( icbor, ier )
	    CALL GQLINE  ( i1, i2, i3, i4, ier )
	    CALL GSLINE  ( ibtyp, 0, ibwid, 0, ier )
C
C*	    Draw y axis.
C
	    CALL GDAXIS ( 2, xmin, .false., iylbfr, iytmfr, iyglfr, -1,
     +			   nyval, yaxval, ier )
C
C*	    Draw x axis.  
C
	    IF  ( timflg )  THEN
		CALL GAAXIS  ( 1, ybot, .true., ixlbfr, ixtmfr, ixglfr,
     +			       nxlbl, xtlbl, ctlbl, ier )
	      ELSE
		CALL GAAXIS ( 1, ybot, .true., 101, 0, 0, nstn, sloc,
     +	 		      stns, ier )
	    END IF
C
C*	    Draw station line.
C
       if( .not. timflg ) then
	    DO  i = 1, nstn
		xx (1) = sloc (i)
		xx (2) = sloc (i)
		yy (1) = ybot
csd		IF  ( .not. ERMISS ( topwnd (i) ) )  THEN
		    yy (2) = topwnd (i)
		    CALL GLINE  ( 'M', 2, xx, yy, ier )
		    yy (1) = yy (2)
csd		END IF
csd		IF  ( ( .not. ERMISS (toptmp (i) ) ) .and.
csd     +		      ( ( toptmp (i) .lt. topwnd (i) ) .or. 
csd     +			( ERMISS (topwnd (i) ) ) ) )  THEN
		    yy (2) = toptmp (i)
		    CALL GQLINE  ( i1, i2, i3, i4, ier )
		    CALL GSLINE  (  3,  0, i3,  0, ier )
		    CALL GLINE   ( 'M', 2, xx, yy, ier )
		    CALL GSLINE  ( i1,  0, i3,  0, ier )
csd		END IF
	    END DO
          endif
C
C*	    Reset solid line.
C
	    CALL GSLINE  (  i1, 0, i3, 0, ier )
C
C*	    Draw surface.
C
csd	    i=0
csd	    yy (1) = ybot
csd	    DO WHILE ( i .lt. 40 )
csd		i = i + 1
csd		IF ( ERMISS ( pontha(i,1) ) ) THEN
C
C*		    Skip this point.
C
csd		ELSE
csd		    xx (1) = x (i)
csd		    xx (2) = x (i)
csd		    yy (2) = pontha (i,1)
csd		    IF ( ERMISS ( pontha (i+1,1) ) ) THEN
csd			npts = 2
csd		    ELSE
csd			xx (3) = x (i+1)
csd			yy (3) = pontha (i+1,1)
csd			npts = 3
csd		    END IF
csd	            CALL GLINE  ( 'M', npts, xx, yy, ier )
csd		END IF
csd	    END DO
	END IF
C*
	RETURN
	END
