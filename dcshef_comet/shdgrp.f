      SUBROUTINE SH_DGRP ( sheftm, datatm, zone, iret)
C***********************************************************************
C* SUBROUTINE SH_DGRP                                                  *
C*                                                                     *
C* This subroutine decodes the observation date/time from a SHEF       *
C* report.  Returns the GEMPAK time (YYMMDD/HHMM) in UTC.              *
C* 								       *
C* Input parameters: 						       *
C*	sheftm		char*12      	Obs date/time (D group)        *
C*  	zone     	char*12		Time Zone		       *
C*								       *
C* Input/Output parameters:                                            *
C*  	datatm		char*12		Decoded SHEF date/time	       *
C*                                                                     *
C* Output parameters:						       *
C*      iret		 integer	return code                    *
C*					  0 = normal return            *
C*					  1 = data group not handled   *
C*								       *
C**								       *
C*Log:						       		       *
C*P.Bruehl/NWS	3/98						       *
C***********************************************************************
	CHARACTER*12	sheftm, datatm, zone, tmpdate
        CHARACTER*36    tmpstr
        LOGICAL         found, convert, eotime
        INTEGER         idtarr(5)

C*
        PARAMETER       ( NZONES = 20 )
        CHARACTER*2     cc, nn, TZN(NZONES)
        INTEGER         TZ(NZONES)  
C*
C-----------------------------------------------------------------------
C
C*  Time Zone Data
C                       ZZ,ED,ES,CD,CS,MD,MS,PD,PS,LD,LS,HS
        DATA  TZ       / 0, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9,10, 
C                       ND,NS,AD,AS,YD,YS,BD,BS
     +                   2, 3, 3, 4, 8, 9, 9,10/
        DATA  TZN      /'ZZ', 'ED', 'ES', 'CD', 'CS', 'MD', 'MS',
     +                  'PD', 'PS', 'LD', 'LS', 'HS', 'ND', 'NS',
     +                  'AD', 'AS', 'YD', 'YS', 'BD', 'BS' /
C
C-----------------------------------------------------------------------
        convert = .true.
	iret = 0

        cc = sheftm(1:2)
        CALL ST_LSTR ( sheftm, lenc, iret )

C
C*  Key on first two characters in string
C
        IF ( cc .eq. 'DN' ) THEN
C
C*  Minutes: DNnn or DNnnss  
C*  (ignore seconds in all fields)
C
              tmpdate = datatm(1:9) // sheftm(3:4)  
              convert = .false.

        ELSE IF ( cc .eq. 'DH' ) THEN
C
C*  Hours: DHhh or DHhhnn
C
              IF ( lenc .eq. 4 ) THEN
                 tmpdate = datatm(1:7) // sheftm(3:4) // datatm(10:11) 
              ELSE IF ( lenc .ge. 6 ) THEN
                 tmpdate = datatm(1:7) // sheftm(3:6)
              ENDIF 

        ELSE IF ( cc .eq. 'DD' ) THEN
C
C*  Days: DDdd, DDddhh, or DDddhhnn
C
              IF ( lenc .eq. 4 ) THEN
                 tmpdate = datatm(1:4) // sheftm(3:4) // datatm(7:11) 
                 convert = .false.
              ELSE IF ( lenc .eq. 6 ) THEN
                 tmpdate = datatm(1:4) // sheftm(3:4) // '/'
     +                     // sheftm(5:6) // datatm(10:11) 
              ELSE IF ( lenc .ge. 8 ) THEN
                 tmpdate = datatm(1:4) // sheftm(3:4) // '/'
     +                     // sheftm(5:8)
              ENDIF 
      
        ELSE IF ( cc .eq. 'DM' ) THEN
C
C*  Months: DMmm, DMmmdd, DMmmddhh, DMmmddhhnn
C
              IF ( lenc .eq. 4 ) THEN
                 tmpdate = datatm(1:2) // sheftm(3:4) // datatm(5:11) 
                 convert = .false.
              ELSE IF ( lenc .eq. 6 ) THEN
                 tmpdate = datatm(1:2) // sheftm(3:6) // datatm(7:11) 
                 convert = .false.
              ELSE IF ( lenc .eq. 8 ) THEN
                 tmpdate = datatm(1:2) // sheftm(3:6) //
     +                  '/' // sheftm(7:8) // datatm(10:11) 
              ELSE IF ( lenc .ge. 10 ) THEN
                 tmpdate = datatm(1:2) // sheftm(3:6) //
     +                  '/' // sheftm(7:10) 
              ENDIF

        ELSE IF ( cc .eq. 'DJ' ) THEN
C
C*  Julian day of the year: DJddd or DJyyddd
C
 
              convert = .false.
              CALL TI_CTOI ( datatm, idtarr, ierr)
              IF ( lenc .ge. 7 ) THEN
                 tmpstr = sheftm(3:4)
                 tmpdate = sheftm(5:7)
              ELSE
                 tmpstr = datatm(1:2)
                 tmpdate = sheftm(3:5)
              ENDIF
              CALL ST_C2I  ( tmpdate, 3, iddd, nval, ierr )
              CALL ST_C2I  ( tmpstr, 2, iyy, nval, ierr )
              IF ( iyy .gt. 50 ) THEN
                  iyy = 1900 + iyy
              ELSE 
                  iyy = 2000 + iyy
              ENDIF
              CALL TI_JTOI ( iyy, iddd, idtarr, ierr)
              CALL TI_ITOC ( idtarr, tmpstr, ierr)
              tmpdate = tmpstr(1:6) // datatm(7:11) 

        ELSE IF ( cc .eq. 'DY' ) THEN
C
C*  Year: DYyy, DYyymm, DYyymmdd, DYyymmddhh, DYyymmddhhnn
C
              IF ( lenc .eq. 4 ) THEN
                 tmpdate = sheftm(3:4) // datatm(3:11) 
                 convert = .false.
              ELSE IF ( lenc .eq. 6 ) THEN
                 tmpdate = sheftm(3:6) // datatm(5:11) 
                 convert = .false.
              ELSE IF ( lenc .eq. 8 ) THEN
                 tmpdate = sheftm(3:8) // datatm(7:11)
                 convert = .false.
              ELSE IF ( lenc .eq. 10 ) THEN
                 tmpdate = sheftm(3:8) // '/' // 
     +                      sheftm(9:10) // datatm(10:11) 
              ELSE IF ( lenc .ge. 12 ) THEN
                 tmpdate = sheftm(3:8) // '/' // sheftm(9:12) 
              ENDIF

        ELSE IF ( cc .eq. 'DR' ) THEN
C
C*  Date Relative code
C
          convert = .false.
          tmpdate = datatm
C
C*  Watch for special times
C
          IF ( tmpdate(8:11) .eq. '2400' ) eotime = .true.

          cc = sheftm(3:4)
          nn = sheftm(5:6)
          CALL ST_LSTR ( nn, lenn, iret )
          CALL ST_C2I  ( nn, lenn, inn, nval, iret )
          CALL TI_CTOI ( tmpdate, idtarr, ierr)
          IF ( cc(1:1) .eq. 'H' ) THEN
            imin = inn * 60
            IF ( cc(2:2) .eq. '+' ) THEN
              CALL TI_ADDM ( idtarr, imin, idtarr, ierr)
            ELSE
              CALL TI_SUBM ( idtarr, imin, idtarr, ierr)
            ENDIF

          ELSE IF ( cc(1:1) .eq. 'N' ) THEN
            IF ( cc(2:2) .eq. '+' ) THEN
              CALL TI_ADDM ( idtarr, inn, idtarr, ierr)
            ELSE
              CALL TI_SUBM ( idtarr, inn, idtarr, ierr)
            ENDIF

          ELSE IF ( cc(1:1) .eq. 'D' ) THEN
            IF ( eotime ) idtarr(4) = 0
            imin = inn * 60 * 24
            IF ( cc(2:2) .eq. '+' ) THEN
              CALL TI_ADDM ( idtarr, imin, idtarr, ierr)
            ELSE
              CALL TI_SUBM ( idtarr, imin, idtarr, ierr)
            ENDIF
            IF ( eotime ) idtarr(4) = 24 

          ELSE IF ( cc(1:1) .eq. 'M' ) THEN
            IF ( eotime ) idtarr(4) = 0
            IF ( cc(2:2) .eq. '+' ) THEN
              idtarr(2) = idtarr(2) + inn
              IF ( idtarr(2) .ge. 13 ) THEN
                 idtarr(1) = idtarr(1) + 1
                 idtarr(2) = idtarr(2) - 12
              ENDIF
            ELSE
              idtarr(2) = idtarr(2) - inn
              IF ( idtarr(2) .le. 0 ) THEN
                 idtarr(1) = idtarr(1) - 1
                 idtarr(2) = idtarr(2) + 12
              ENDIF
            ENDIF
            IF ( eotime ) idtarr(4) = 24

          ELSE IF ( cc(1:1) .eq. 'Y' ) THEN
            IF ( eotime ) idtarr(4) = 0
            iyy = inn
            IF ( cc(2:2) .eq. '+' ) THEN
              idtarr(1) = idtarr(1) + inn
            ELSE
              idtarr(1) = idtarr(1) - inn
            ENDIF
            IF ( eotime ) idtarr(4) = 24

          ELSE IF ( cc(1:1) .eq. 'E' ) THEN

            IF ( eotime ) idtarr(4) = 0
            IF ( cc(2:2) .eq. '+' ) THEN
C
C*           End of month: 
C*            add n+1 months, set to first day, subtract day
C
              idtarr(2) = idtarr(2) + inn + 1
              IF ( idtarr(2) .ge. 13 ) THEN
                 idtarr(1) = idtarr(1) + 1
                 idtarr(2) = idtarr(2) - 12
              ENDIF
            ELSE
C
C*           subtract n-1 months, set to first day, sub day
C
              idtarr(2) = idtarr(2) - inn + 1
              IF ( idtarr(2) .le. 0 ) THEN
                 idtarr(1) = idtarr(1) - 1
                 idtarr(2) = idtarr(2) + 12
              ENDIF
            ENDIF
            idtarr(3) = 1
            CALL TI_SUBD ( idtarr, idtarr, iret )  
            IF ( eotime ) idtarr(4) = 24

          ELSE
C
C*  Time modifier not handled.  Return
C
            tmpdate = datatm
            tmpstr = 'Unk D grp ' // sheftm
            iret = 1
            CALL DC_WLOG ( 5, 'SHA', -1 , tmpstr, ier )
          ENDIF

          CALL TI_ITOC ( idtarr, tmpdate, ierr)

 
        ELSE
C
C*  Time modifier not handled.  Return
C
               convert = .false.
               tmpdate = datatm
               iret = 1
               tmpstr = 'Unk D grp ' // sheftm
               CALL DC_WLOG ( 5, 'SHA', -1 , tmpstr, ier )
        ENDIF

        datatm = tmpdate

C
C*  Convert time zone to UTC, if necessary
C*  Don't convert relative changes (DR) or if we've hour hasn't
C*  been set
C
C
C*  Leave 2400 hrs as is, even though it is in local time.
C*  Usually this means the "end of the day" or "end of the month"
C
C        IF ( ( .not. convert ) .and. 
C     +            ( tmpdate(8:11) .eq. '2400' ) ) THEN
C          convert = .true.
C        ENDIF


        IF ( convert ) THEN

        found = .false.
        DO iz = 1, NZONES
           IF ( zone(1:2) .eq. TZN(iz) ) THEN
C
C*	Convert number of hours to minutes and add to local time
C
              found = .true.
              iminutes = TZ(iz) * 60
              IF ( iminutes .gt. 0 ) THEN 
                CALL TI_CTOI ( datatm, idtarr, ierr)
                CALL TI_ADDM ( idtarr, iminutes, idtarr, ierr)
                CALL TI_ITOC ( idtarr, datatm, ierr)
              ENDIF
           ENDIF
        END DO
        IF ( .not. found ) THEN
               tmpstr = 'Unk time zone ' // zone
               CALL DC_WLOG ( 5, 'SHA', -1, tmpstr, ier )
        ENDIF 

        ENDIF

        RETURN
        END
