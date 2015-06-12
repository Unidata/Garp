C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	gridopen.f
C*
C*	Opens GEMPAK grid file.
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*
C************************************************************************

	subroutine  grid_open ( gdfile, iflno, iret )

	character	gdfile*(*)
	integer		iflno
	integer		iret
	
        include         'GEMPRM.PRM'


	integer		maxgrds

	real		anl (LLNANL), rnav (LLNNAV)


C-----------------------------------------------------------------------

C
C
C
C	Output paramater
C
	iflno  = 0
C
C	Open the grid file
C
	call gd_open ( gdfile, .false., LLNANL, LLNNAV, iflno,
     +			anl, rnav, maxgrds, iret)
	
	return

	end
