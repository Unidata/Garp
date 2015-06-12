C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	gridtime.f
C*
C*	GEMPAK grid file date/time utilities
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*
C************************************************************************


	subroutine  grid_numtimes ( gdfile, otimes, iret )

	character	gdfile*(*)
	integer		otimes
	integer		iret
	
        include         'GEMPRM.PRM'
	include         'GMBDTA.CMN'

	integer		ntimes
	character*20    timlst (LLMXTM)
	character	trange*(LLMXLN)

	COMMON /GARP_GDNTIM/ ntimes, timlst


C-----------------------------------------------------------------------
C
C	Output paramater
C
	otimes = 0
	ntimes = 0

	CALL DG_NFIL ( gdfile, ' ', iret )
        CALL DG_NDTM ( 'FIRST-LAST', iret )

	CALL DG_QTMS ( LLMXTM, .false., timlst, ntimes,
     +		trange, iret )
	CALL DG_NEND ( iret )

	IF ( iret .ne. 0 ) THEN
	    ntimes = 0
	ELSE
	    otimes = ntimes
	END IF

	return

	end


C
C	New routine, called from "C" to return each date/time
C	stamp for a given grid file.
C
C	
C
	subroutine  grid_nexttime ( nexttime, dattim, iret )

	integer		nexttime
	character*(*)	dattim
	integer		iret

	include         'GEMPRM.PRM'
	integer		ntimes
	character*20	timlst (LLMXTM)

	COMMON /GARP_GDNTIM/ ntimes, timlst

	iret = 0
	dattim = ' '

C
C	Error check
C
	if( nexttime .gt. ntimes ) then
		return
	endif


C
C	Get the next time string from the list.
C
	dattim = timlst(nexttime)

        return

        end
