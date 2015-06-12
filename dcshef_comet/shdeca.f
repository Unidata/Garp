      SUBROUTINE SH_DECA ( report, lenr, iyear, maxparm, numstid, 
     + corflg, repstid, reptims, repparms, repdata, istid, nparm, iret)

C***********************************************************************
C* SUBROUTINE SH_DECA                                                  *
C*                                                                     *
C* This subroutine decodes SHEF bulletins (.A format )into a single    *
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
C*     repstid(NUMSTID)	 char*8   	Array of station IDs	       *
C*     repparms(MAXPARM,NUMSTID,2) char*7  Array of SHEF parm codes    *
C*     reptims(NUMSTID)  char*12        Array of times per report      *
C*     repdata(MAXPARM,NUMSTID)  real   Array of SHEF data per report  *
C*     istid             integer        Number of station IDs          *
C*     nparm(NUMSTID)    integer        Number of parameters per stn   *
C*     iret		 integer	return code                    *
C*					  0 = normal return            *
C*					  2 = input error	       *
C*					 -1 = invalid report           *
C*								       *
C**								       *
C*Log:						       		       *
C*P.Bruehl/NWS	3/98						       * 
C*A.Person/Penn State  12/02  Bug fix in placement of ifld=ifld+2      *
C***********************************************************************
	INCLUDE         'GEMPRM.PRM'
        INCLUDE         'shcmn.cmn'
	CHARACTER*(*)	report
	CHARACTER*8	repstid (NUMSTID)
        CHARACTER*12    reptims (NUMSTID)
        CHARACTER*7     repparms (MAXPARM,NUMSTID,2)
C*
        CHARACTER*12    tmpdate, zone
        CHARACTER*36    tmpstr
        CHARACTER*3     ccc
        CHARACTER*1     siunit
C*
        REAL		repdata(MAXPARM, NUMSTID)
        REAL            data
C*
	INTEGER		istid, iret
        INTEGER         nparm(NUMSTID)
        LOGICAL         corflg, good, first
C-----------------------------------------------------------------------
	iret = 0
        istid = 0
        iparm = 0
        siunit = 'E'
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

C
C*  Start decoding the .A fields
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
C*      D group
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
                 repstid(istid) = repstid(istid-1)
                ELSE 
                 ier = -2
                 tmpstr = ': too many times/report'
                 CALL DC_WLOG ( 1, 'SH', istid, tmpstr, ier )
                 RETURN
                END IF
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
                ELSE
                  istid = istid + 1
                  IF ( istid .le. NUMSTID ) THEN
                   iparm = 0
                   reptims(istid) = tmpdate
                   repstid(istid) = repstid(istid-1)
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
C*      Not important in .A reports
C
            ifld = ifld + 1

          ELSE IF ( iftype(ifld) .eq. 1 ) THEN
C
C*      Data (parameter code, data value, or data + qualifier)
C
            first = .false.
            IF ( iparm .lt. MAXPARM ) THEN
              iparm = iparm + 1
              repparms(iparm,istid,1) = cfield(ifld)
              repparms(iparm,istid,2) = siunit
              IF ( iftype(ifld+1) .eq. 2 ) THEN
C
C*	Data value
C
                repdata(iparm,istid) = rfval(ifld+1) 
                IF ( rfval(ifld+1) .eq. -9002. )
     +                  repdata(iparm,istid) = RMISSD

              ELSE IF ( iftype(ifld+1) .eq. 1 ) THEN
C
C*      Character string (missing, trace, or qualifier)
C
                IF ( ( cfield(ifld+1) .eq. "M" ) .or.
     +             ( cfield(ifld+1) .eq. "m" ) .or.
     +             ( cfield(ifld+1) .eq. "+" ) ) THEN
C
C*              Missing Value 
C

                   repdata(iparm,istid) = RMISSD

                ELSE IF ( ( cfield(ifld+1) .eq. "T" ) .or.
     +                  ( cfield(ifld+1) .eq. "t" ) )  THEN
C
C*              Trace precip
C
                   repdata(iparm,istid) =  0.0

                ELSE 
C
C*              Data + qualifier, ignore qualifier
C              
                  CALL ST_LSTR (cfield(ifld+1), lenr, ier)
                  CALL ST_C2R (cfield(ifld+1)(1:lenr-1),1,
     +                                        data,nval,ierr)
                  repdata(iparm,istid) = data

                END IF


              ELSE
C
C*       Unknown data value
C
                 repparms(iparm,istid,1) = " "
                 repparms(iparm,istid,2) = " "
                 iparm = iparm - 1 

              ENDIF
C              nparm = IMAX( iparm,nparm)
              nparm(istid) = MAX0 ( iparm,nparm(istid) )

            ENDIF
            ifld = ifld + 2
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
