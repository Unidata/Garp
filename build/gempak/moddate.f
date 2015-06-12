C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	moddate.f
C*
C*	Modifies date string to include the day of the week.
C*
C*	History:
C*
C*	02/97	COMET	Original copy
C*
C************************************************************************


	SUBROUTINE MODDATE ( strin, strout, iret )
C************************************************************************
C*									*
C* This subroutine modifies the input date string to include the day	*
C* of the week in the string. It is called by numerous routines	.	*
C*									*
C*	strin		char*		Input string			*
C*	strout		char*		Output string			*
C*									*
C************************************************************************
	CHARACTER*(*)	strin, strout
	CHARACTER	cday (7)*3
	INTEGER		itime (5)
C
	DATA	cday / 'SUN', 'MON', 'TUE', 'WED', 'THU', 'FRI', 'SAT'/
C------------------------------------------------------------------------
	iret = 0
C
C*	Set the output time to the input time in case anything fails
C
	strout(1:) = strin(1:)
C
C*	Convert to integer time array, determine day number
C
	CALL TI_CTOI ( strin(1:11), itime, iret )
	CALL TI_DAYW ( itime, idayw, ier )
C
	IF ( iret .ne. 0 .or. idayw .lt. 1 .or. idayw .gt. 7 ) RETURN
C
	strout(1:) = cday(idayw)//' '//strin(1:)
C
	RETURN
	END
