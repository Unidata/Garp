C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	get_upa_files.f
C*
C*	Sounding data utilities
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*
C************************************************************************


	subroutine upa_count ( file, nt, ier )

C************************************************************************
C subroutine upa_count							*
C									*
C This subroutine reads a GEMPAK sounding station data file and		*
C returns a list of times for which data is available in that file.	*
C									*
C Input:								*
C	file		char		File to read			*
C									*
C Output:								*
C	nt		int		Number of times listed		*
C	time		char		List of times available		*
C									*
C************************************************************************

	INCLUDE		'GEMPRM.PRM'

	CHARACTER	file*(*)
	INTEGER		nt, ier

     	CHARACTER	time (LLMXTM)*15
	CHARACTER	parms (MMPARM)*4
	LOGICAL		wrtflg, mrgdat
	DATA 		wrtflg, mrgdat /.false., .false./
C-----------------------------------------------------------------------

	ier = 0


	CALL IN_BDTA ( iret )
C*
	nt = 0
C
C	Open sounding file.
C
	CALL SN_OPNF (file, wrtflg, isf, isr, np, parms, ivert,
     +                mrgdat, iret)

C
C	Read times from sounding file.
C
	CALL SN_GTIM (isf, LLMXTM, nt, time, ier)

C
C	Close sounding file.
C
	CALL SN_CLOS  ( isf, ier )

	return
	end



	subroutine upa_list ( file, times, nt, len_t, ier)

C************************************************************************
C subroutine upa_list							*
C									*
C This subroutine reads a GEMPAK sounding  data file and		*
C returns a list of times for which data is available in that file.	*
C									*
C Input:								*
C	file		char		File to read			*
C									*
C Output:								*
C	nt		int		Number of times listed		*
C	time		char		List of times available		*
C									*
C Note:									*
C	ivert = 1 -> pressure, = 2 -> theta, = 3 -> height		*
C************************************************************************

	INCLUDE		'GEMPRM.PRM'

	CHARACTER	file*(*)
	CHARACTER	times*(*)
	INTEGER		nt, ier

     	CHARACTER	time (LLMXTM)*15
	CHARACTER	parms (MMPARM)*4
	LOGICAL		wrtflg, mrgdat 
	DATA 		wrtflg, mrgdat /.false., .false./ 
C-----------------------------------------------------------------------

	ier = 0

	CALL IN_BDTA ( iret )
C*
	nt = 0
C
C	Open sounding file.
C
	CALL SN_OPNF (file, wrtflg, isf, isr, np, parms, ivert,
     +                mrgdat, iret)

C
C	Read times from sounding file.
C
	CALL SN_GTIM (isf, LLMXTM, nt, time, ier)

C
C	Close sounding file.
C
	CALL SN_CLOS  ( isf, ier )

C
C       Return if no times are available.
C
        if ( nt .lt. 1 ) then
            return
        end if

C
C	Put all times in a single character string.
C
	do i=1, nt
	    ibeg = (i-1)*15 + 1
	    write ( times(ibeg:ibeg+14), 101 ) time(i)
	enddo

101	format ( a )
	return
	end
