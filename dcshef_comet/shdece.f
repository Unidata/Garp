      SUBROUTINE SH_DECE ( report, lenr, iyear, maxparm, numstid, 
     + corflg, repstid, reptims, repparms, repdata, istid, nparm, iret)

C***********************************************************************
C* SUBROUTINE SH_DECE                                                  *
C*                                                                     *
C* This subroutine decodes SHEF bulletins (.E format )into a single    *
C* representative format.                                	       *
C* 								       *
C* Input parameters: 						       *
C*	REPORT		char*      	report to decode	       *
C*	LENR		integer		length of report to decode     *
C*	IYEAR		integer		Year of report (YYYY)          *
C*	MAXPARM		integer		Maximum number of parameters   *
C*	NUMSTID		integer		Maximum number of stations     *
C*								       *
C* Output parameters:						       *
C*     repstid(NUMSTID)	 char*8 	Array of station IDs	       *
C*     repparms(MAXPARM,NUMSTID,2) char*7 Array of SHEF parameter codes*
C*     reptims(NUMSTID)  char*12        Array of times per report      *
C*     repdata(MAXPARM,NUMSTID)  real   Array of SHEF data per report  *
C*     istid             integer        Number of station IDs          *
C*     nparm(NUMSTID)    integer        Number of parameters per stn   *
C*     iret		 integer	return code                    *
C*					  0 = normal return            *
C*					  2 = input error	       *
C*					 -1 = invalid report           *
C*                                       -2 = too many times           *
C*								       *
C**								       *
C*Log:						       		       *
C*P.Bruehl/NWS	3/98						       * 
C***********************************************************************
        INCLUDE         'shcmn.cmn'
        INCLUDE         'GEMPRM.PRM'
	CHARACTER*(*)	report
	CHARACTER*8	repstid (NUMSTID)
        CHARACTER*12    reptims (NUMSTID)
        CHARACTER*7     repparms (MAXPARM,NUMSTID,2)
C*
        CHARACTER*12    tmpdate, zone, dinc, odate, rdate
        CHARACTER*36    tmpstr
        CHARACTER*3     ccc
        CHARACTER*1     siunit
C*
        REAL		repdata(MAXPARM, NUMSTID)
        REAL            data
C*
	INTEGER		istid, iret
        INTEGER         nparm(NUMSTID)
        LOGICAL         corflg, good, first, incflag
C-----------------------------------------------------------------------
	iret = 0
        istid = 0
        siunit = 'E'
        corflg = .false.
        first = .false.
        incflag = .false.
        good = .true.
        zone = "ZZ"
        next = 1
        iexplicit = 1
        iparm = 0
        inum = 1
        DO i = 1, NUMSTID
         nparm(i) = 0
        END DO 
C
C*  Check for R in first character for correction
C
        IF ( report(1:2) .eq. 'R ' ) THEN
                   corflg = .TRUE.
                   inum = 2
        END IF


C
C*  Break report into individual fields for decoding
C
        CALL SH_GFLD ( report(inum:), lenr, ierr )


C
C*  Start decoding the .E fields
C
C*  Position Field
C*  Station ID

        istid = 1
        repstid(istid) = cfield(1)

        IF ( iftype(2) .eq. 2 ) THEN
C
C*  Parse date/time
C
           CALL SH_DATE ( iyear, next, tmpdate, zone, ierr )
           reptims(istid) = tmpdate
           first = .true.
        ENDIF 
        IF ( ( iftype(2) .ne. 2 ) .or. ( ierr .ne. 0 ) ) THEN 
           iret = -1
           tmpstr = 'Date not found. Skip report'
           CALL DC_WLOG ( 0, 'SH', iftype(2), tmpstr, ier )
           RETURN
        ENDIF

C
C*  Data Field
C
C*  Loop until end of report
C
        ifld = next
        tmpdate = reptims(istid)
        DO WHILE ( ifld .le. nfield )

          IF ( cfield(ifld)(1:1) .eq. "D" )  THEN
C
C*      D Group 
C
            ccc = cfield(ifld)(1:3)
            IF ( ccc(1:2) .eq. 'DU' ) THEN
C
C*       DUS = SI Units, DUE = English Units, default E
C
               IF ( ccc .eq. "DUS" ) THEN
                  siunit = 'S'
               ELSE
                  siunit = 'E'
               ENDIF 

            ELSE IF ( ccc(1:2) .eq. 'DR' ) THEN
C
C*  Date relative group.  Use last explicitly set date as reference.
C
               first = .false.
               tmpdate = reptims(iexplicit)
               CALL SH_DGRP ( cfield(ifld), tmpdate, zone, ierr)
               IF ( ierr .eq. 0 ) THEN 
                istid = istid + 1
                IF ( istid .le. NUMSTID ) THEN
                 iparm = 0
                 reptims(istid) = tmpdate
                 repstid(istid) = repstid(1)
                 first = .true.
                ELSE
                 tmpstr = ': too many times/report' 
                 ier = -2
                 CALL DC_WLOG ( 1, 'SH', istid, tmpstr, ier )
                 RETURN
                ENDIF
               ENDIF 

            ELSE IF ( ccc(1:2) .eq. 'DI' ) THEN
C
C*  Date/time increment group
C*  Formats: DInx, DInxx, DIn+x, DIn-x, DIn+xx, DIn-xx
C
               dinc = cfield(ifld)
               IF ( ifsize(ifld) .lt. 6 ) THEN
                 ineg = INDEX ( dinc , "-" )
                 ipos = INDEX ( dinc , "+" ) 
                 IF ( ( ifsize(ifld) .eq. 5 ) .and.
     +                ( ipos .eq. 0 ) .and. ( ineg .eq. 0 ) ) THEN
                   dinc = cfield(ifld)(1:3) // "+" // cfield(ifld)(4:5)
                 ELSE IF ( ineg .gt. 0 ) THEN
                   dinc = cfield(ifld)(1:4) // "0" // cfield(ifld)(5:5) 
                 ELSE IF ( ifsize(ifld) .eq. 4 ) THEN
                   dinc = cfield(ifld)(1:3) // "+0" // cfield(ifld)(4:4)
                 ELSE IF ( ( ifsize(ifld) .eq. 5 ) .and.
     +                     ( ipos .gt. 0 ) ) THEN
                   dinc = cfield(ifld)(1:4) // "0" // cfield(ifld)(5:5)
                 ELSE
                   tmpstr = 'Invalid increment ' // cfield(ifld)
                   CALL DC_WLOG ( 5, 'SH', ifsize(ifld), tmpstr, ier )
                   iret = -1
                   RETURN
                 ENDIF
               ENDIF
               CALL ST_C2I  ( dinc(5:6), 2, inn, nval, iret )
               IF ( (inn .lt. 15) .and.
     +                  (cfield(ifld)(3:3) .eq. "N") ) THEN
                tmpstr = 'Warning: Incr under 15min: ' // cfield(ifld)
                CALL DC_WLOG ( 2, 'SH', inn, tmpstr, ier )
                incflag = .true.
               ENDIF


            ELSE
C
C*  Standard Date group (also handles misc D groups)
C
               CALL SH_DGRP ( cfield(ifld), tmpdate, zone, ierr)
               IF ( ierr .eq. 0 ) THEN
                IF ( first )  THEN
                   reptims(istid) = tmpdate
                   iexplicit = istid
                   CALL SH_RNDT ( 3, tmpdate, rdate, ihhmm, iret )
                   odate = rdate
                ELSE
                  istid = istid + 1
                  IF ( istid .le. NUMSTID ) THEN
                   iparm = 0
                   reptims(istid) = tmpdate
                   repstid(istid) = repstid(1)
                   first = .true.
                   iexplicit = istid
                  ELSE
                   ier = -2
                   tmpstr = ': too many times/report' 
                   CALL DC_WLOG ( 1, 'SH', istid, tmpstr, ier )
                   RETURN
                  ENDIF
                ENDIF
               ENDIF

            ENDIF
            ifld = ifld + 1

          ELSE IF ( iftype(ifld) .eq. 3 ) THEN
C
C*      Slash
C*      Check for two in a row
C
            ifld = ifld + 1
            IF ( iftype(ifld) .eq. 3 ) THEN
C
C*      Null field - Increment time and move on
C
             IF ( istid .lt. NUMSTID ) THEN
               tmpdate = reptims(iexplicit)
               CALL SH_DINC (dinc, tmpdate, ier )
               IF ( incflag ) THEN
                 good = .true.
                 CALL SH_RNDT ( 3, tmpdate, rdate, ihhmm, iret )
                 IF ( rdate(1:11) .eq. odate(1:11) ) THEN
                   tmpstr = 'Can not handle this incr ' // tmpdate
                   CALL DC_WLOG ( 1, 'SH', istid, tmpstr, ier )
                   iexplicit = iexplicit+1
                   reptims(iexplicit) = tmpdate
                   good = .false.
                 ENDIF
                 odate = rdate
               ENDIF
               IF ( good ) THEN
                 istid = istid + 1
                 repstid(istid) = repstid(1)
                 iexplicit = iexplicit+1
                 reptims(istid) = tmpdate
                 repdata(1,istid) = RMISSD
                 nparm(istid) = 1
                 repparms(1,istid,1)=repparms(1,istid-1,1)
                 repparms(1,istid,2)=repparms(1,istid-1,2)
               ENDIF
             ELSE
               ier = -2
               tmpstr = ': too many times/report' 
               CALL DC_WLOG ( 1, 'SH', istid, tmpstr, ier )
               RETURN
             ENDIF 

             ifld = ifld + 1
            END IF

          ELSE IF ( iftype(ifld) .eq. 1 ) THEN
C
C*      Character field: parameter code or qualified data
C

            IF ( iparm .eq. 0 ) THEN
C
C*      Parameter code 
C
              nparm(istid) = 1
              repparms(1,istid,1) = cfield(ifld)
              repparms(1,istid,2) = siunit
              iparm = 1
            ELSE
C
C*      Data value with qualifier or missing/trace
C
             IF ( ( cfield(ifld) .eq. "M" ) .or.
     +            ( cfield(ifld) .eq. "m" ) .or.
     +            ( cfield(ifld) .eq. "+" ) ) THEN
C
C*       Missing Value
C
                IF ( .not. first ) THEN
                 IF ( istid .lt. NUMSTID ) THEN
                    tmpdate = reptims(iexplicit)
                    CALL SH_DINC (dinc, tmpdate, ier )
                    IF ( incflag ) THEN
                      good = .true.
                      CALL SH_RNDT ( 3, tmpdate, rdate, ihhmm, iret )
                      IF ( rdate(1:11) .eq. odate(1:11) ) THEN
                        tmpstr = 'Can not handle this incr ' // tmpdate
                        CALL DC_WLOG ( 1, 'SH', istid, tmpstr, ier )
                        iexplicit = iexplicit+1
                        reptims(iexplicit) = tmpdate
                        good = .false.
                      ENDIF
                      odate = rdate
                    ENDIF
                    IF ( good ) THEN
                      istid = istid + 1
                      repstid(istid) = repstid(1)
                      iexplicit = iexplicit+1
                      reptims(istid) = tmpdate
                      nparm(istid) = 1
                      repparms(1,istid,1)=repparms(1,istid-1,1)
                      repparms(1,istid,2)=repparms(1,istid-1,2)
                    ENDIF
                 ELSE
                    ier = -2
                    tmpstr = ': too many times/report' 
                    CALL DC_WLOG ( 1, 'SH', istid, tmpstr, ier )
                    RETURN
                 ENDIF 
                ENDIF
                IF ( good ) repdata(1,istid) = RMISSD

             ELSE IF ( ( cfield(ifld) .eq. "T" ) .or.
     +                 ( cfield(ifld) .eq. "t" ) )  THEN

C
C*       Trace precip
C
                IF ( .not. first ) THEN
                 IF ( istid .lt. NUMSTID ) THEN
                    tmpdate = reptims(iexplicit)
                    CALL SH_DINC (dinc, tmpdate, ier )

                    IF ( incflag ) THEN
                      good = .true.
                      CALL SH_RNDT ( 3, tmpdate, rdate, ihhmm, iret )
                      IF ( rdate(1:11) .eq. odate(1:11) ) THEN
                        tmpstr = 'Can not handle this incr ' // tmpdate 
                        CALL DC_WLOG ( 1, 'SH', istid, tmpstr, ier )
                        iexplicit = iexplicit+1
                        reptims(iexplicit) = tmpdate
                        good = .false.
                      ENDIF
                      odate = rdate
                    ENDIF
                    IF ( good ) THEN
                       istid = istid + 1
                       repstid(istid) = repstid(1)
                       iexplicit = iexplicit+1
                       reptims(istid) = tmpdate
                       nparm(istid) = 1
                       repparms(1,istid,1)=repparms(1,istid-1,1)
                       repparms(1,istid,2)=repparms(1,istid-1,2)
                    ENDIF
                 ELSE
                    ier = -2
                    tmpstr = ': too many times/report' 
                    CALL DC_WLOG ( 1, 'SH', istid, tmpstr, ier )
                    RETURN
                 ENDIF 
                ENDIF
                IF ( good ) repdata(1,istid) =  0.0

              ELSE 
C
C*       Data + qualifier, ignore 
C              
                IF ( .not. first ) THEN
                 IF ( istid .lt. NUMSTID ) THEN
                    tmpdate = reptims(iexplicit)
                    CALL SH_DINC (dinc, tmpdate, ier )

                    IF ( incflag ) THEN
                      good = .true.
                      CALL SH_RNDT ( 3, tmpdate, rdate, ihhmm, iret )
                      IF ( rdate(1:11) .eq. odate(1:11) ) THEN
                        tmpstr = 'Can not handle this incr ' // tmpdate
                        CALL DC_WLOG ( 1, 'SH', istid, tmpstr, ier )
                        iexplicit = iexplicit+1
                        reptims(iexplicit) = tmpdate
                        good = .false.
                      ENDIF
                      odate = rdate
                    ENDIF
                    IF ( good ) THEN
                      istid = istid + 1
                      repstid(istid) = repstid(1)
                      iexplicit = iexplicit+1
                      reptims(istid) = tmpdate
                      nparm(istid) = 1
                      repparms(1,istid,1)=repparms(1,istid-1,1)
                      repparms(1,istid,2)=repparms(1,istid-1,2)
                    ENDIF
                 ELSE
                    ier = -2
                    tmpstr = ': too many times/report' 
                    CALL DC_WLOG ( 1, 'SH', istid, tmpstr, ier )
                    RETURN
                 ENDIF 
                ENDIF
                IF ( good )  THEN
                  CALL ST_LSTR (cfield(ifld), lenr, ier)
                  CALL ST_C2R (cfield(ifld)(1:lenr-1),1,data,nval,ierr)
                  repdata(1,istid) = data
                ENDIF

              END IF
              first = .false.

            ENDIF
            ifld = ifld + 1

          ELSE IF ( iftype(ifld) .eq. 2 ) THEN
C
C*	Real: Data value
C
                IF ( .not. first ) THEN
                 IF ( istid .lt. NUMSTID ) THEN
                    tmpdate = reptims(iexplicit)
                    CALL SH_DINC (dinc, tmpdate, ier )

                    IF ( incflag ) THEN
                      good = .true.
                      CALL SH_RNDT ( 3, tmpdate, rdate, ihhmm, iret )
                      IF ( rdate(1:11) .eq. odate(1:11) ) THEN
                        tmpstr = 'Can not handle this incr ' // tmpdate
                        CALL DC_WLOG ( 1, 'SH', istid, tmpstr, ier )
                        iexplicit = iexplicit+1
                        reptims(iexplicit) = tmpdate
                        good = .false.
                      ENDIF
                      odate = rdate
                    ENDIF
                    IF ( good ) THEN
                      istid = istid + 1
                      repstid(istid) = repstid(1)
                      iexplicit = iexplicit+1
                      reptims(istid) = tmpdate
                      nparm(istid) = 1
                      repparms(1,istid,1)=repparms(1,istid-1,1)
                      repparms(1,istid,2)=repparms(1,istid-1,2)
                    ENDIF
                 ELSE
                    ier = -2
                    tmpstr = ': too many times/report' 
                    CALL DC_WLOG ( 1, 'SH', istid, tmpstr, ier )
                    RETURN
                 ENDIF 
                ENDIF
                IF ( good ) THEN
                  repdata(1,istid) = rfval(ifld) 
                  IF ( rfval(ifld) .eq. -9002. )
     +                  repdata(1,istid) = RMISSD
                ENDIF
                first = .false.
                ifld = ifld + 1

          ELSE
C
C*	Unknown field
C
           ifld = ifld + 1
           tmpstr = 'Unk field ' // cfield(ifld)
           CALL DC_WLOG ( 5, 'SH', iftype(ifld), tmpstr, ier )
         

         ENDIF
        END DO 
        RETURN
        END
