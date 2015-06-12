C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	gdinfo.f
C*
C*	Gets GEMPAK grid file info.
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*
C************************************************************************


	SUBROUTINE  GDINFO ( gdfile, lastim, numgrids, 
     +			maxgrids, gfd, iret )

	CHARACTER	gdfile*(*)
	CHARACTER	lastim*(*)
	INTEGER		numgrds
	INTEGER		maxgrds
	INTEGER		gfd
	
	INCLUDE		'GEMPRM.PRM'

C	CHARACTER	devs (4)*1, ans*1
C	INTEGER		luns (4)
C	LOGICAL		respnd, done, proces

	CHARACTER	firstim*36
	LOGICAL		newfil
	REAL		anl (LLNANL), rnav (LLNNAV)


C-----------------------------------------------------------------------

C
C	Open the grid file
C
	CALL GD_OPEN  ( gdfile, .false., LLNANL, LLNNAV, iflno, anl, 
     +		rnav, maxgrd, iret )

	IF ( iret .ne. 0 ) return

	CALL GD_NGRD ( iflno, numgrd, firstim, lastim, iret )

	numgrids = numgrd
	maxgrids = maxgrd
	gfd      = iflno

	END
