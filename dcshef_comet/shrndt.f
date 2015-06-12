	SUBROUTINE SH_RNDT  ( ireptyp, time, dattim, ihhmm, iret)
C************************************************************************
C* SH_RNDT								*
C*									*
C* Takes input character GEMPAK time YYMMDD/HHMM and rounds the minutes *
C* up.  For .A & .B reports (ireptyp=1,2), time is rounded to the next 	*
C* hour, if MM is greater than or equal to 45 minutes.  For .E reports  *
C* (ireptyp=3), time is rounded up to the nearest 15 minutes		*
C*									*
C* Returns rounded time in GEMPAK time format YYMMDD/HHMM.  Also        *
C* original HHMM field as an integer.  This can be used as the station  *
C* time.								*
C*									*
C* SH_RNDT  ( IREPTYP, TIME, DATTIM, IHHMM, IRET )			*
C*									*
C* Input parameters:							*
C*	IREPTYP	 	INTEGER		Report type			*
C*	TIME	 	CHAR*12		YYMMDD/HHMM			*
C*									*
C* Output parameters:							*
C*      DATTIM          CHAR*12         Rounded time YYMMDD/HHMM        *
C*      IHHMM           INTEGER         Original HHMM                   *
C*	IRET		INTEGER		Return code			*
C*					  0 = normal return		*
C*					 -1 = invalid time		*
C**									*
C* Log:									*
C* P.Bruehl/NWS 	 6/98	Based on SH_RTIM                      	* 
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
C*
	INTEGER		ihhmm, ireptyp
	CHARACTER*(12)	time, dattim
C*
	LOGICAL		addday
C*
	INTEGER		idtar (5)
	EQUIVALENCE	( idtar (1), iyear ), ( idtar (2), imnth ),
     +			( idtar (3), iday  ), ( idtar (4), ihour ),
     +			( idtar (5), imin  )
C*
C------------------------------------------------------------------------
	iret   = 0
	addday = .false.
C
C* Convert character YYMMDD/HHMM to 5 integer array:
C* idtar(1)=YYYY, idtar(2)=MM, idtar(3)=DD, idtar(4)=HH idtar(5)=MM
C
        CALL TI_CTOI ( time, idtar, iret )
        IF ( iret .ne. 0 ) THEN
           RETURN
        ENDIF
C
C* Store original HHMM as integer
C
        ihhmm = (ihour * 100) + imin
C
C* Round the time
C
        IF ( ireptyp .eq. 3 ) THEN
C
C*      .E report - round to 15 minutes
C
          IF ( imin .eq. 0 ) THEN
             imin = 00
          ELSE IF ( imin .le. 15 ) THEN
             imin = 15 
          ELSE IF ( imin .le. 30 ) THEN
             imin = 30
          ELSE IF ( imin .le. 45 ) THEN
             imin = 45
          ELSE 
             imin = 00
	     ihour = ihour + 1
      	     IF  ( ihour .ge. 24 )  THEN
	        ihour = ihour - 24
	        addday = .true.
	     END IF
           ENDIF

        ELSE
C
C*      .A or .B report - round to hour
C
         IF  ( imin .ge. 45 )  THEN
	  ihour = ihour + 1
	  IF  ( ihour .ge. 24 )  THEN
	     ihour = ihour - 24
	     addday = .true.
	  END IF
 	 END IF
         imin = 00

        ENDIF
C
C
C* Add day if necessary.
C
        IF  ( addday )  CALL TI_ADDD  ( idtar, idtar, iret )
        IF ( iret .ne. 0 ) THEN
           RETURN
        ENDIF
C
C*	    Convert back to a GEMPAK time.
C
	CALL TI_ITOC  ( idtar, dattim, iret )
C*
	RETURN
	END

