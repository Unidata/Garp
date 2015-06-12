	SUBROUTINE SH_GFLD  ( report, lenr, iret )
C************************************************************************
C* SH_GFLD								*
C*									*
C* This subroutine divides a SHEF report into individual		*
C* fields for decoding.  Fields must be separated by blanks or		*
C* slashes. Fields can be numbers, characters, or slashes. Unprintable  *
C* characters must be replaced by blanks before this subroutine is	*
C* called.  The fields are stored in / SHCMN /.				*
C*									*
C* SH_GFLD  ( REPORT, LENR, IRET )					*
C*									*
C* Input parameters:							*
C*	REPORT		CHAR*		SHEF report			*
C*	LENR		INTEGER		Length of report		*
C*									*
C* Output parameters:							*
C*	IRET		INTEGER		Return code			*
C*					  0 = normal return		*
C*					 -4 = invalid report		*
C**									*
C* Log:									*
C* P.Bruehl/NWS		4/98	Based on RA_GFLD			*
C* P.Bruehl/NWS		6/98	Changed integer values to reals		*
C************************************************************************
	INCLUDE		'shcmn.cmn'
C*
	CHARACTER*(*)	report
C*
	CHARACTER	cc*1
C------------------------------------------------------------------------
	iret   = 0
C
C*	The first field is the station id.
C
	is = 0
	ie = 0
	i  = 1
	DO WHILE  ( ( ie .eq. 0 ) .and. ( i .le. lenr ) )
	    IF  ( ( report (i:i) .eq. ' ' ) .and. ( is .ne. 0 ) )  THEN
		ie = i - 1
            ELSE IF  ( ( report (i:i) .ne. ' ' ) .and. ( is .eq. 0 ) )
     +								THEN
		is = i
	    END IF
	    i = i + 1
	END DO
C*
	IF  ( ie .eq. 0 )  THEN
	    iret   = -1
	    nfield = 0
	    RETURN
	  ELSE
	    nfield = 1
	    ifstrt ( nfield ) = is
	    ifendp ( nfield ) = ie
	    ifsize ( nfield ) = ie - is + 1
	    iftype ( nfield ) = 1
	END IF
C
C*	Loop through report checking type of character. 
C*	    ITYPE = 0    no current field
C*	    ITYPE = 1    character field
C*	    ITYPE = 2    numeric field
C
C*          IFTYPE = 1   character
C*          IFTYPE = 2   number
C*          IFTYPE = 3   slash
C
	itype = 0
	DO  i = ie + 1, lenr
	    cc = report (i:i)

	    IF  ( ( cc .ge. '0' ) .and. ( cc .le. '9' ) )  THEN
C
C*	     Found a number.
C
		IF ( ( itype .eq. 0 ) .and. 
     +                      ( nfield .lt. IFLDMX ) ) THEN
C
C*                Start a number field
C
			nfield = nfield + 1
			ifstrt ( nfield ) = i
			iftype ( nfield ) = 2
			itype = 2
		END IF
	        IF  ( itype .eq. 4 )  THEN
C
C*                It is a number.  Continue w/ number field
C
		    itype = 2
		    iftype ( nfield ) = 2
		END IF


	    ELSE IF  ( cc .eq. '/' )  THEN
C
C*		Found a slash ( / ) delimiter
C
                IF ( itype .eq. 4 ) THEN
C
C*                It was a character field after all
C
                    iftype(nfield) = 1
                ENDIF 
		IF  ( itype .ne. 0 )  THEN
C
C*                End the current character or numeric field
C
		    ifendp ( nfield ) = i - 1
		    ifsize ( nfield ) = i - ifstrt ( nfield )
		END IF
		IF  ( nfield .lt. IFLDMX )  THEN
C
C*                 Start & end the current / field
C
		    nfield = nfield + 1
		    ifstrt ( nfield ) = i
		    ifendp ( nfield ) = i
		    ifsize ( nfield ) = 1
		    iftype ( nfield ) = 3
		    itype = 0
		END IF

	    ELSE IF  ( cc .eq. ' ' )  THEN
C
C*		Found a blank ( ) delimiter
C
                IF ( itype .eq. 4 ) THEN
C
C*                It was a character field after all
C
                    iftype(nfield) = 1
                ENDIF 
		IF  ( itype .ne. 0 )  THEN
C
C*                End the current character or numeric field
C
		    ifendp ( nfield ) = i - 1
		    ifsize ( nfield ) = i - ifstrt ( nfield )
                    itype = 0
		END IF

	    ELSE IF  ( cc .eq. '-' )  THEN
C
C*		Found a dash. Could be a numeric field.
C
     	      IF  ( (itype .eq. 2) .or. (itype .eq. 4) )  THEN
C
C*                 It's a character field after all
C
                    iftype(nfield) = 1
	      ELSE IF  ( ( itype .eq. 0 ) .and. 
     +                   ( nfield .lt. IFLDMX ) ) THEN
		    nfield = nfield + 1
		    ifstrt ( nfield ) = i
		    itype = 4
	      END IF

            ELSE IF ( cc .eq. '.' ) THEN
C
C*              Found a ".".  
C*              Could still be a numeric field if itype = 4 or 2.
C
     	      IF ( itype .eq. 1 ) THEN
C
C*                 It's a character field 
C
                    iftype(nfield) = 1
              ELSE IF  ( ( itype .eq. 0 ) .and. 
     +                   ( nfield .lt. IFLDMX ) ) THEN
		    nfield = nfield + 1
		    ifstrt ( nfield ) = i
		    itype = 4
	      END IF

	    ELSE IF  ( itype .ne. 1 )  THEN
C
C*            Found a character string (by default)
C
		IF ( (itype .eq. 2) .or. (itype .eq. 4) ) THEN
		    itype = 1
		    iftype ( nfield ) = 1
		ELSE IF  ( ( itype .eq. 0 ) .and.
     +                     ( nfield .lt. IFLDMX ) ) THEN
		    nfield = nfield + 1
		    ifstrt ( nfield ) = i
		    iftype ( nfield ) = 1
		    itype = 1
		END IF
	    END IF
	END DO
C
C*	Check whether last field is finished.
C
	IF  ( itype .ne. 0 )  THEN
	    ifendp ( nfield ) = lenr
	    ifsize ( nfield ) = lenr - ifstrt ( nfield ) + 1
	END IF
C
C*	After loop, convert all numbers to reals and get
C*	character fields.
C
	DO  i = 1, nfield
	    CALL ST_RMBL  ( report ( ifstrt (i) : ifendp (i) ), 
     +			    cfield (i), ifsize (i), ier )
	    IF  ( iftype ( i ) .eq. 2 )  THEN
		CALL ST_CRNM ( cfield (i), rfval (i), ier )
	      ELSE
		rfval (i) = 0.
	    END IF
	END DO
C*
	RETURN
	END
