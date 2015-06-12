	SUBROUTINE SH_GRPT  ( bultin, lenb, ibpnt, ireptyp, report, 
     +                           lenr, iret )
C************************************************************************
C* SH_GRPT								*
C*									*
C* This subroutine gets the next report from a SHEF bulletin		*
C* Reports must begin with either .A .B or .E				*
C* 									*
C* IBPNT is assumed to be a pointer to the first character after the	*
C* end of the bulletin header for the initial pass through the	 	*
C* routine. The value of IBPNT is updated for subsequent passes to 	*
C* the first character after the end of the current report.		*
C*									*
C* SH_GRPT  ( BULTIN, LENB, IBPNT, IREPTYP, REPORT, LENR, IRET )	*
C*									*
C* Input parameters:							*
C*	BULTIN		CHAR*		Bulletin			*
C*	LENB		INTEGER		Bulletin length			*
C*									*
C* Input and Output parameters:						*
C*	IBPNT		INTEGER		Pointer in bulletin		*
C*									*
C* Output parameters:							*
C*      IREPTYP        INTEGER         Report type			*
C*                                      1=.A, 2=.B, 3=.E 		*
C*	REPORT		CHAR*		Report				*
C*	LENR		INTEGER		Length of report		*
C*	IRET		INTEGER		Return code			*
C*					  0 = normal return		*
C*					 -2 = no more reports		*
C**									*
C* Log:									*
C* P.Bruehl/NWS		3/98	Based on RA_GRPT			*
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
	INCLUDE         'BRIDGE.PRM'
C*
        CHARACTER*3     cont
        CHARACTER*1     cnum
	CHARACTER*(*) bultin, report
        CHARACTER*(DCMXBF*2) tmpstr
C-----------------------------------------------------------------------
        iret = 0 
	lenr = 0
        ia = 0
        ib = 0
        ie = 0
        istart = 0
        iposnx = 0
        iend = 0
        istart2 = 0
        inext = 0
        iend2 = 0
        cont = " "
C
C*	Check for end of bulletin.
C
	IF  ( ibpnt .ge. lenb )  THEN 
	    report = ' '
            iret = -2
            RETURN
        END IF

	CALL ST_LSTR  ( bultin(ibpnt:lenb), lenin, ier )


C
C* 	Find the first report: .A, .B, or .E
C
        ia = INDEX (bultin (ibpnt:lenb), ".A")
        ib = INDEX (bultin (ibpnt:lenb), ".B")
        ie = INDEX (bultin (ibpnt:lenb), ".E")
        IF ( ia .eq. 0 ) ia = 99999
        IF ( ib .eq. 0 ) ib = 99999
        IF ( ie .eq. 0 ) ie = 99999

        IF ( (ia .eq. 99999) .and. (ib .eq. 99999) .and. 
     +       (ie .eq. 99999) ) THEN
C
C*      No valid reports
C
           iret = -2
           RETURN

        ELSE IF ( (ia .lt. ib) .and. (ia .lt. ie) ) THEN
C
C*      We have an .A report
C
          ireptyp = 1
 	  istart = ibpnt + ia + 1
C
C*	Check for end of bulletin.
C
          IF  ( istart .ge. lenb )  THEN
		ibpnt = lenb + 1
		report = ' '
		iret  = -2
		RETURN
          END IF
C
C*      Find the end of the report. Set the end point. 
C
	  iposnx = INDEX ( bultin ( istart : lenb ), CHCR // CHLF )
C
C*      If end of report not found, save end point at end of bulletin
C
	  IF  ( iposnx .eq. 0 )  THEN
		iend = lenb
	  ELSE
		iend = istart + iposnx 
	  END IF
C
C*	 Reset pointer in bulletin.
C
	  ibpnt  = iend

        ELSE IF ( (ib .lt. ia) .and. (ib .lt. ie) ) THEN
C
C*	We have a .B report
C
          ireptyp = 2 
	  istart = ibpnt + ib + 1
          IF  ( istart .ge. lenb )  THEN
		ibpnt = lenb + 1
		report = ' '
		iret  = -2
		RETURN
          END IF

	  iposnx = INDEX ( bultin ( istart : lenb ), ".END" )

	  IF  ( iposnx .eq. 0 )  THEN
	    iend = lenb
	  ELSE
	    iend = istart + iposnx  + 2
	  END IF

	  ibpnt  = iend 
          

        ELSE IF ( (ie .lt. ia) .and. (ie .lt. ib) ) THEN
C 
C*      We have an .E report
C 
          ireptyp = 3 
 	  istart = ibpnt + ie + 1
          IF  ( istart .ge. lenb )  THEN
		ibpnt = lenb + 1
		report = ' '
		iret  = -2
		RETURN
          END IF

	  iposnx = INDEX ( bultin ( istart : lenb ), CHCR//CHLF )

	  IF  ( iposnx .eq. 0 )  THEN
		iend = lenb
	  ELSE
		iend = istart + iposnx 
	  END IF

	  ibpnt  = iend 

        ENDIF

C
C*  Construct report
C
 	  report = bultin  ( istart : iend )
	  CALL ST_LSTR  ( report, lenin, ier )
C
C*	For .B reports, convert line feeds and commas to " -9991 " 
C*      Makes decoding each station easier later
C
          IF ( ireptyp .eq. 2 ) THEN
            ichar = 0
            DO WHILE ( ichar .le. lenin )
             ichar = ichar + 1
             IF ( ( report(ichar:ichar) .eq. CHLF ) .or.
     +            ( report(ichar:ichar) .eq. "," ) ) THEN
                tmpstr = report(1:ichar-1) // " -9991 " //
     +                   report(ichar+1:lenin)
                lenin = lenin + 6
                report = tmpstr (1:lenin)
             ENDIF
            END DO
           ENDIF 
C
C*	Strip unreadable/control characters
C
	  CALL ST_UNPR  ( report, lenin, report, lenr, ier )
C
C*	Check that this is a real report.
C
	  IF  ( lenr .eq. 0 )  THEN
	    iret = -2
	  END IF
C
C*      Check for .AN or .EN (N=1,2,3...) continuation lines
C
          IF ( ireptyp .eq. 2 ) THEN 
             RETURN
          ELSE IF  ( ireptyp .eq. 1 ) THEN
             cont = '.A1'
          ELSE IF  ( ireptyp .eq. 3) THEN 
             cont = '.E1'
          ENDIF

          IF ( bultin (ibpnt+1:ibpnt+3) .eq. cont ) then

           cnum = bultin (ibpnt+3:ibpnt+3)
           DO WHILE ( (cnum .ge. '0' ) .and. ( cnum .le. '9') )
             inext = 0
             istart2 = ibpnt + 4
             IF  ( istart2 .ge. lenb )  THEN
                 RETURN
             END IF

             inext = INDEX ( bultin ( istart2 : lenb ), CHCR//CHLF )
             IF  ( inext .eq. 0 )  THEN
                 iend2 = lenb
             ELSE
                 iend2 = istart2 + inext
             END IF
C
             ibpnt  = iend2 

             IF ( ( bultin(iend:iend ) .eq. "/" ) .or. 
     +          ( bultin(istart2+1:istart2+1) .eq. "/" ) ) THEN
C               report = bultin ( istart:iend) //
               tmpstr = report (1:lenr) // 
     +                     bultin ( istart2:iend2)
             ELSE

C               report = bultin ( istart:iend) // "/" //
               tmpstr = report (1:lenr) // "/" //
     +                     bultin ( istart2:iend2)
             ENDIF


             CALL ST_LSTR  ( tmpstr, lenin, ier )
             report = tmpstr (1:lenin)
             CALL ST_UNPR  ( report, lenin, report, lenr, ier )
             IF  ( lenr .eq. 0 )  THEN
                 iret = -2
             END IF

             cnum = bultin (ibpnt+3:ibpnt+3)
          END DO

         END IF

	RETURN
	END
