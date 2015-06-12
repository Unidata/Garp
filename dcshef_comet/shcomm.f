	SUBROUTINE SH_COMM  ( report, lenr, iret )
C************************************************************************
C* SH_COMM								*
C*									*
C* This subroutine extracts comments from a SHEF bulletin 		*
C* Comments are delimited by ":" or a line feed.                        *
C*									*
C* SH_GFLD  ( REPORT, LENR, IRET )					*
C*									*
C* Input/Output parameters:						*
C*	REPORT		CHAR*		SHEF bulletin			*
C*	LENR		INTEGER		Length of bulletin		*
C*									*
C* Output parameters:							*
C*	IRET		INTEGER		Return code			*
C*					  0 = normal return		*
C*					  1 = empty bulletin		*
C**									*
C* Log:									*
C* P.Bruehl/NWS		4/98	                       			*
C* P.Bruehl/NWS         8/98    Bug fix for comments at end of line     *
C* A.Person/Penn State  12/2002 Re-designed bug-prone comment removal   *
C*                              algorithm                               *
C************************************************************************
	INCLUDE         'BRIDGE.PRM'
	INCLUDE         'GEMPRM.PRM'

	CHARACTER*(*)  report

        LOGICAL OFF
C*
C------------------------------------------------------------------------
	iret   = 0
C
C*   Remove text after or between ":"'s
C
        OFF=.FALSE.
        J=0
        DO I=1,lenr
        IF ( report(I:I) .EQ. ':' .AND. OFF ) THEN
           OFF=.FALSE.
        ELSE IF ( report(I:I) .EQ. ':' .AND. (.NOT. OFF) ) THEN
           OFF=.TRUE.
        ELSE IF ( report(I:I) .LT. ' ' ) THEN
           J=J+1
           report(J:J)=report(I:I)
           OFF=.FALSE.
        ELSE
           if( .NOT. OFF ) THEN
              J=J+1
              report(J:J)=report(I:I)
           END IF
        END IF
        END DO
        lenr=J

        IF ( lenr .eq. 0 )  THEN
            iret = 1
            RETURN 
        ENDIF
C*
	RETURN
	END
