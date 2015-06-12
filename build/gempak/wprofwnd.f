C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	wprofwnd.f
C*
C*	Called by wind profiler driver to draw wind barbs. Derived from 
C*	the GEMPAK program SNSWND.
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*
C************************************************************************


	SUBROUTINE wprofwnd ( wintyp, nstn, ipsdat, nlvls, 
     +			      stndat, idtype, sloc, ybot, ytop, filtfc, 
     +			      vcord, iret )
C************************************************************************
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
C*
	CHARACTER*(*)	wintyp, vcord
	REAL		stndat (*), sloc (*)
	INTEGER		ipsdat (*), nlvls (*), idtype (LLMXLV, * )
C*
	CHARACTER	cdata (3)*8
	REAL		data (3), p (2000), d (2000), s (2000),
     +			xx (2000), vx (2000), vy (2000)
csd
        integer set_barb_color, BY_SPEED
        data BY_SPEED/1/
csd        INCLUDE         'DEVSET.CMN'
csd
C*
csd	INCLUDE		'ERMISS.FNC'
C------------------------------------------------------------------------
	iret = 0
C
C*	Get barb size and calculate filter value
C
	CALL GQSYSZ  ( rmkw, rmkh, rtxw, rtxh, brbx, brby, ier )
	brbftr = .6 * brbx * filtfc
C
C*	Extract data for each station.  Don't keep levels which are
C*	outside the plot area; this allows plotting in V coord.,
C*	with barbs extending outside the plot region.  Levels with
C*	missing data are also ignored.  Only winds at mandatory and
C*	significant wind levels are drawn.
C
	DO  i = 1, nstn
	    nlv = 0
	    CALL PC_SSTN  ( '    ',0,0,0,0,ispri,IMISSD, nlvls (i), 
     +			iret )
	    DO  j = 1, nlvls (i)
		CALL PC_CMLV (j, stndat (ipsdat (i)), data, cdata, iret)
csd		IF  ( ( .not. ERMISS (data (1)) ) .and.
csd     *		      ( .not. ERMISS (data (2)) ) .and.
csd     *		      ( .not. ERMISS (data (3)) ) .and.
csd     *                ( idtype ( j, i ) .ne. 2  ) .and. 
		IF (
     *		      ( ( ( vcord .eq. 'PRES' )   .and.
     *		          ( data (1) .le. ybot )  .and.
     *			  ( data (1) .ge. ytop ) ) .or.
     *			( ( vcord .ne. 'PRES' )   .and.
     *			  ( data (1) .ge. ybot )  .and.
     *			  ( data (1) .le. ytop ) ) ) ) THEN
		    nlv = nlv + 1
		    p (nlv) = data (1)
		    d (nlv) = data (2)
		    s (nlv) = data (3)
		    xx (nlv) = sloc (i)
		END IF
	    END DO
C
C*	    Transform pressures and x locations to V coordinates.
C
	    CALL GTRANS  ( 'M', 'V', nlv, xx, p, vx, vy, iret )
C
C*	    If wind arrows, draw them.
C
	    IF  ( wintyp .eq. 'A' )  THEN
		CALL GARRW   ( 'V', nlv, vx, vy, s, d, iret )
	      ELSE
C
C*		If no filter then draw the barbs.
C
		IF  ( filtfc .eq. 0. )  THEN
csd
csd  Call GBARB one barb at a time to reset color if necessary.
csd
                    do m=1, nlv
                      iwnclr = set_barb_color(s(m),BY_SPEED)
                      CALL GSCOLR  ( iwnclr, ier )
                      CALL GBARB  ( 'V', 1, vx (m), vy (m), s (m),
     *                               d (m), iret )
                    enddo

		  ELSE
C
C*		    Draw the first barb
C
		    yold = vy (1)
                    iwnclr = set_barb_color(s(1),1)
                    CALL GSCOLR  ( iwnclr, ier )
		    CALL GBARB  ( 'V', 1, vx (1), vy (1), s (1), d (1),
     +				  iret )
C
C*		    Check for overlap and draw if ok.
C
		    DO  m = 2, nlv
			IF  ( vy (m) - yold .ge. brbftr )  THEN
                            iwnclr = set_barb_color(s(m),1)
                            CALL GSCOLR  ( iwnclr, ier )
			    CALL GBARB  ( 'V', 1, vx (m), vy (m), 
     +					  s (m), d (m), iret )
			    yold = vy (m)
			END IF
		    END DO
		END IF
	    END IF
	END DO
C*
	RETURN
	END
