	SUBROUTINE SH_RTIM  ( isdtar, btime, irdtar, rtime, iret)
C************************************************************************
C* SH_RTIM								*
C*									*
C* Reads time from SHEF bulletin.					*
C*									*
C* This subroutine combines an integer system time, and the bulletin	*
C* time containing the day, month and hour into an observation time.	*
C* It is assumed that the system time accurately reflects the year 	*
C* and month of the observation.                         		*
C*									*
C* SH_RTIM  ( ISDTAR, BTIME, IRDTAR, RTIME, IRET )			*
C*									*
C* Input parameters:							*
C*	ISDTAR (5)	INTEGER		System time			*
C*	BTIME		CHAR*		Bulletin day, hour, minute	*
C*									*
C* Output parameters:							*
C*	IRDTAR (5)	INTEGER		Integer report time		*
C*	RTIME		CHAR*		Report date/time		*
C*	IRET		INTEGER		Return code			*
C*					  0 = normal return		*
C*					 -1 = invalid time		*
C**									*
C* Log:									*
C* P.Bruehl/NWS 	 3/98	Based on FF_RTIM                      	* 
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
C*
	INTEGER		isdtar (*), irdtar (*)
	CHARACTER*(*)	btime, rtime
C*
	LOGICAL		addday
C*
	INTEGER		iodtar (5)
	EQUIVALENCE	( iodtar (1), ioyear ), ( iodtar (2), iomnth ),
     +			( iodtar (3), ioday  ), ( iodtar (4), iohour ),
     +			( iodtar (5), iomin  )
C*
	DATA		iomin / 0 /
C------------------------------------------------------------------------
	iret   = 0
	addday = .false.
C
C*	Use the system time for the year and month.
C
	ioyear = isdtar (1)
	iomnth = isdtar (2)
C
C*	If no bulletin time was sent, use day from system time and 
C*	set hour to 0.
C
	IF  ( btime .eq. ' ' )  THEN
	    ioday  = isdtar (3)
	    iohour = 0
C
C*	    Convert to GEMPAK string YYMMDD/HHMM
C
	    CALL TI_ITOC  ( iodtar, rtime, ier )
	ELSE
C
C*	    Get the day, hour and minute from the bulletin.
C
	    CALL ST_INTG  ( btime (1:2), ioday,  ier )
	    CALL ST_INTG  ( btime (3:4), iohour, ier )
	    CALL ST_INTG  ( btime (5:6), iomin,  ier )
	    IF  ( iomin .ge. 45 )  THEN
		iohour = iohour + 1
                iomin = 00 
		IF  ( iohour .ge. 24 )  THEN
		    iohour = iohour - 24
		    addday = .true.
		END IF
	    END IF
C
C*          Add day if necessary.
C
            IF  ( addday )  CALL TI_ADDD  ( iodtar, iodtar, ier )
C
C*	    Convert to a GEMPAK time.
C
	    CALL TI_ITOC  ( iodtar, rtime, iret )
	    DO  i = 1, 5
		irdtar (i) = iodtar (i)
	    END DO
	END IF
C*
	RETURN
	END
