       SUBROUTINE SH_DECB ( report, lenr, iyear, maxparm, numstid, 
     + corflg, repstid, reptims, repparms, repdata, istid, nparm, iret)

C***********************************************************************
C* SUBROUTINE SH_DECB                                                  *
C*                                                                     *
C* This subroutine decodes SHEF bulletins (.B format )into a single    *
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
C*     repparms(MAXPARM,NUMSTID,2) char*7Array of SHEF parameter codes *
C*     reptims(NUMSTID)  char*12        Array of times per report      *
C*     repdata(MAXPARM,NUMSTID)  real   Array of SHEF data per report  *
C*     istid             integer        Number of station IDs          *
C*     nparm(NUMSTID)   integer        Number of parameters per stn    *
C*     iret		 integer	return code                    *
C*					  0 = normal return            *
C*					  2 = input error	       *
C*					 -1 = invalid report           *
C*                                       -2 = too many times/station   *
C*                                       -3 = too many parameters      *
C*								       *
C**								       *
C*Log:						       		       *
C*P.Bruehl/NWS	3/98						       * 
C*P.Bruehl/NWS	11/98	Fixed leading slash bug			       * 
C***********************************************************************
        INCLUDE         'shcmn.cmn'
        INCLUDE         'GEMPRM.PRM'
	CHARACTER*(*)	report
	CHARACTER*8	repstid (NUMSTID)
        CHARACTER*12    reptims (NUMSTID) 
        CHARACTER*7     repparms (MAXPARM,NUMSTID,2) 
C*  
        CHARACTER*12    tmpdate, zone
        CHARACTER*36    tmpstr
        CHARACTER*3     ccc
        CHARACTER*1     siunit, sihead
C*
        REAL		repdata(MAXPARM, NUMSTID)
        REAL            data
C*
        INTEGER         nparm(NUMSTID)
	INTEGER		istid, iret
        LOGICAL         corflg, good, first
C-----------------------------------------------------------------------
	iret = 0
        istid = 0
        iparm = 0
        siunit = 'E'
        sihead = 'E'
        corflg = .false.
        first = .false.
        good = .true.
        zone = "ZZ"
        next = 1
        iexplicit = 1
        inum = 1
        DO i = 1,NUMSTID
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

        DO i = 1, nfield
        END DO


C
C*  Start decoding the .B fields
C
C*  Header:  Source ID, (YY)MMDD, Zone
C*  Source ID is not stored
C

        IF ( iftype(2) .eq. 2 ) THEN
C
C*  Parse date/time
C
           CALL SH_DATE ( iyear, next, tmpdate, zone, ierr )
           reptims(1) = tmpdate
           first = .true.
        ENDIF 
        IF ( ( iftype(2) .ne. 2 ) .or. ( ierr .ne. 0 ) ) THEN 
           iret = -1
           tmpstr = 'Date not found. Skip report'
           CALL DC_WLOG ( 0, 'SH', iftype(2), tmpstr, ier )
           RETURN
        ENDIF

C
C*  Header: Parameter control string
C*  (Obs Time)/Parm1/Parm2/.../-9991
C

        ifld = next
        tmpdate = reptims(1)
        DO WHILE ( ifld .le. nfield )  
                                       
          IF ( rfval(ifld) .eq. -9991. ) THEN
            ifld = ifld + 1
C
C*        Check next line for continuation
C
            IF ( cfield(ifld)(1:2) .eq. ".B") THEN
               ifld = ifld + 1
               continue
            ELSE
C
C*          Skip to data section
C
               goto 1000
            ENDIF

          ELSE IF ( cfield(ifld)(1:1) .eq. "D" )  THEN
C
C*        D Group 
C
            ccc = cfield(ifld)(1:3)
            IF ( ccc(1:2) .eq. 'DU' ) THEN
C
C*             DUS = SI Units, DUE = English Units, default E
C
               IF ( ccc .eq. "DUS" ) THEN
                  sihead = 'S'
               ELSE
                  sihead = 'E'
               ENDIF 

            ELSE IF ( (ccc(1:2) .eq. 'DR') .and.
     +                (iparm .gt. 0 ) ) THEN
C
C*          Invalid Date Relative group 
C
                 iret = -2
                 tmpstr = 'Can not handle more than 1 time/station' 
                 CALL DC_WLOG ( 1, 'SH', -1, tmpstr, ier )
                 RETURN

            ELSE 
C
C*          Standard Date group or date relative group
C
               CALL SH_DGRP ( cfield(ifld), tmpdate, zone, ierr)
               reptims(1) = tmpdate

            ENDIF
            ifld = ifld + 1

          ELSE IF ( iftype(ifld) .eq. 3 ) THEN
C
C*        Slash. Skip.
C
            ifld = ifld + 1

          ELSE IF ( iftype(ifld) .eq. 1 ) THEN
C
C*        Character field: parameter code 
C
            iparm = iparm + 1
            IF ( iparm .le. MAXPARM ) THEN
              repparms(iparm,1,1) = cfield(ifld)
              repparms(iparm,1,2) = sihead
            ELSE
              tmpstr = ': too many parms/report'
              iret = -3
              CALL DC_WLOG ( 1, 'SH', 1, tmpstr, ier )
              RETURN
            ENDIF
            ifld = ifld + 1
          ELSE
C
C*        Unknown field
C
           ifld = ifld + 1
           tmpstr = 'Unk field ' // cfield(ifld)
           CALL DC_WLOG ( 5, 'SH', iftype(ifld), tmpstr, ier )

          ENDIF
          nparm(1) = iparm

        END DO 

C
C*  Data Section: Station ID  (date) data1/data2/.../-9991
C*  Loop until end of report
C
 1000   continue
        tmpdate = reptims(1)
        DO WHILE ( ifld .le. nfield )

         IF ( rfval(ifld) .eq. -9991 ) THEN
C
C*           Go to next line
C
             ifld = ifld + 1

         ELSE IF ( cfield(ifld) .eq. ".END" ) THEN
C
C*           Check for .END -- get out of DO loop
C
             ifld = nfield + 1

         ELSE
C
C*       Process row of data
C

         istid = istid +1
         repstid(istid) = cfield(ifld)
         istidfld = ifld
         ifld = ifld + 1

         first = .true.
         iparm = 1
         siunit = sihead
         reptims(istid) = reptims(1)

         DO WHILE ( ( rfval(ifld) .ne. -9991 ) .and. 
     +                ( ifld .le. nfield ) )
C
C*       Loop over data for this station
C

          IF ( cfield(ifld)(1:1) .eq. "D" )  THEN
C
C*        D Group 
C
            ccc = cfield(ifld)(1:3)
            IF ( ccc(1:2) .eq. 'DU' ) THEN
C
C*           DUS = SI Units, DUE = English Units, default E
C
               IF ( ccc .eq. "DUS" ) THEN
                  siunit = 'S'
               ELSE
                  siunit = 'E'
               ENDIF 

            ELSE IF ( ccc(1:2) .eq. 'DR' ) THEN
C
C*          Date relative group.  Use date set in header as reference.
C
               IF ( first ) THEN
                 tmpdate = reptims(1)
                 CALL SH_DGRP ( cfield(ifld), tmpdate, zone, ierr)
                 reptims(istid) = tmpdate
                 first = .false.
               ELSE
                 iret = -2
                 tmpstr = 'Can not handle more than 1 time/station' 
                 CALL DC_WLOG ( 1, 'SH', -1, tmpstr, ier )
                 RETURN
               ENDIF

            ELSE IF ( ( ccc(1:2) .eq. 'DC' ) .or.
     x                ( ccc(1:2) .eq. 'DV' ) .or.
     x                ( ccc(1:2) .eq. 'DQ' ) ) THEN
C
C*          These fields not used.  Skip
C
               tmpstr = 'Ignored ' // cfield(ifld)
               ier = 1
               CALL DC_WLOG ( 5, 'SH', 1, tmpstr, ier )
               continue

            ELSE
C
C*          Standard Date group
C
               CALL SH_DGRP ( cfield(ifld), tmpdate, zone, ierr)
               IF ( first )  THEN
                  reptims(istid) = tmpdate
                  first = .false.
               ELSE 
                  iret = -2
                  tmpstr = 'Can not handle more than 1 time/station' 
                  CALL DC_WLOG ( 1, 'SH', -1, tmpstr, ier )
                  RETURN
               ENDIF

            ENDIF
            ifld = ifld + 1

          ELSE IF ( iftype(ifld) .eq. 3 ) THEN
C
C*      Slash
C*      Check for two in a row
C
            IF ( iftype(ifld-1) .eq. 3 ) THEN
C
C*          Null field - Increment parameter and move on
C
                iparm = iparm + 1 
            END IF
            IF ( ifld .eq. (istidfld + 1) ) THEN
C
C*          Leading slash - increment parameter and move on
C
                iparm = iparm + 1
            END IF

            ifld = ifld + 1

          ELSE IF ( iftype(ifld) .eq. 1 ) THEN
C
C*      Character field: 
C
C*      Data value with qualifier or missing/trace
C
              IF ( ( cfield(ifld) .eq. "M" ) .or.
     +             ( cfield(ifld) .eq. "m" ) .or.
     +             ( cfield(ifld) .eq. "+" ) ) THEN
C
C*            Missing Value 
C

                   repdata(iparm,istid) = RMISSD

              ELSE IF ( ( cfield(ifld) .eq. "T" ) .or.
     +                  ( cfield(ifld) .eq. "t" ) )  THEN
C
C*       Trace precip
C
                   repdata(iparm,istid) =  0.0

              ELSE 
C
C*       Data + qualifier, ignore qualifier
C              
                CALL ST_LSTR (cfield(ifld), lenr, ier)
                CALL ST_C2R (cfield(ifld)(1:lenr-1),1,data,nval,ierr)
                repdata(iparm,istid) = data

              END IF

            ifld = ifld + 1
            iparm = iparm + 1

          ELSE IF ( iftype(ifld) .eq. 2 ) THEN
C
C*	Real: Data value
C
                repdata(iparm,istid) = rfval(ifld) 
             IF ( rfval(ifld) .eq. -9002. ) 
     +                  repdata(iparm,istid) = RMISSD

                iparm = iparm + 1 
                ifld = ifld + 1

          ELSE
C
C*	Unknown field
C
           ifld = ifld + 1
           ier = 1
           tmpstr = 'Unk field ' // cfield(ifld)
           CALL DC_WLOG ( 5, 'SH', iftype(ifld), tmpstr, ier )
         

          ENDIF
         END DO 
        ENDIF
        END DO 

        IF ( ifld .gt. nfield ) THEN
C
C*           Fill in repparms array with parameter names
C*           for each station ID
C
             iret = 0 
             DO i = 1,istid
               nparm(i) = nparm(1)
               DO j = 1,nparm(1)
                   repparms(j,i,1) = repparms(j,1,1)
                   repparms(j,i,2) = repparms(j,1,2)
               END DO
             END DO

             tmpstr = "Processed all available fields"
             CALL DC_WLOG ( 5, 'SH', ifld, tmpstr, ier )
             RETURN
        ENDIF

        RETURN 
        END
