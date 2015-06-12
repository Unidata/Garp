C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	pbgmap.f
C*
C*	Map drawing utility
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*	04/97	COMET	Check for map mode
C*	10/97	COMET	Changed MAPDRAW() to pbgmap(), args.
C*	 2/98	COMET	Added RETURN if mapfil not set OK
C*
C************************************************************************


	SUBROUTINE pbgmap ( mapfil, lcolor, ltype, lwidth, iret )

C************************************************************************
	CHARACTER*(*)	mapfil, lcolor, ltype, lwidth
	CHARACTER	mapatt*80
C*
C------------------------------------------------------------------------
	iret = 0
C
C*	Check for map mode
C
	CALL GQMODE ( mode, ier )
	IF ( mode .ne. 1 ) RETURN
C
C*	Set the map file.
C
	CALL GSMFIL ( mapfil, ier )
	IF  ( ier .ne. 0 )  THEN
	    CALL ER_WMSG ( 'GEMPLT', ier, ' ', ierr )
	    RETURN
	END IF
C
C*	Draw the map
C
	mapatt = lcolor // '/' // ltype // '/' // lwidth
	CALL GG_MAP  ( mapatt, ier )
C
C*	Flush GEMPAK graphics
C
C	CALL GFLUSH ( ier )
C
	RETURN
	END
