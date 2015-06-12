C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	get_sao_files.f
C*
C*	Surface data file time functions.
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*	 5/97	COMET	Change some printing formats.
C*	 1/98	COMET	Changed printf statement
C*
C************************************************************************


	subroutine sfc_list ( file, times, nt, len_t, verbose, ier )

C************************************************************************
C subroutine sfc_list							*
C									*
C This subroutine reads a GEMPAK surface station data file and		*
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


	character	file*(*)
	character	times*(*)

     	CHARACTER	time (LLMXTM)*15
	CHARACTER	parms (MMPARM)*4
	CHARACTER	buf*20, output*60
	INTEGER		verbose
	LOGICAL		wrtflg 
	DATA 		wrtflg /.false./
	DATA 		buf / '                    '/
C-----------------------------------------------------------------------

	ier = 0

	CALL IN_BDTA ( iret )
C*
	nt = 0
C
C	Open surface data file.
C
	CALL SF_OPNF (file, wrtflg, isf, isr, np, parms, iret)

C
C	Read times from surface data file.
C
	CALL SF_GTIM (isf, LLMXTM, nt, time, ier)

C
C	Close surface data file.
C
	CALL SF_CLOS ( isf, ier )

C
C	Return if no times are available.
C
	if ( nt .lt. 1 ) then
	    return
	end if

C
C	Put all times in a single character string.
C
	if ( verbose .gt. 1 ) then
csd	    print*,'list_hours - nt = ', nt
	    buf = '                    '
	    write (buf,'(i8)' ) nt
	    call gfprints (
     +          'list_hours - nt  = '//char(0), buf//char(0) )
	end if

	do i=1, nt
	    ibeg = (i-1)*15 + 1
	    write ( times(ibeg:ibeg+14), 101 ) time(i)
	    if ( verbose .gt. 1 ) then
csd	        print*,'i, time[i] = ', i , time(i)
	    	buf = '                    '
		write (buf,'(i6)' ) i
		output = 'i, time[i] = ' // buf // time(i)
		call gfprints ( '  ' // char(0), output//char(0) )
	    end if
	enddo

	if ( verbose .gt. 1 ) then
csd	    print*,'times = ', times
cjc	    call gfprints ( '  times = ' // char(0), times//char(0) )
            if(len(times).gt.60) then
               output = times(1:60)
            else
               output = times(1:len(times))
            endif
            call gfprints ( '  times = ' // char(0), output//char(0) )

	end if

101	format ( a15 )
	return
	end



	subroutine sfc_count ( file, nt, ier )

C************************************************************************
C subroutine sfc_count							*
C									*
C This subroutine reads a GEMPAK surface station data file and		*
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

	character	file*(*)

     	CHARACTER	time (LLMXTM)*15
	CHARACTER	parms (MMPARM)*4
	LOGICAL		wrtflg 
	DATA 		wrtflg /.false./
C-----------------------------------------------------------------------

	ier = 0

	CALL IN_BDTA ( iret )
C*
	nt = 0

C
C	Open surface data file.
C
	CALL SF_OPNF (file, wrtflg, isf, isr, np, parms, iret)

C
C	Read times from surface data file.
C
	CALL SF_GTIM (isf, LLMXTM, nt, time, ier)

C
C	Close surface data file.
C
	CALL SF_CLOS ( isf, ier )

	return
	end
