      SUBROUTINE SH_DATE (iyear, next, datatm, zone, iret)

C***********************************************************************
C* SUBROUTINE SH_DATE                                                  *
C*                                                                     *
C* This subroutine decodes the time field from SHEF bulletins.         *
C* 								       *
C* Input parameters: 						       *
C*	IYEAR		integer		Year of report (YYYY)          *
C*                                                                     *
C* Input/Output parameter:                                             *
C*      NEXT           integer          Pointer to next field	       *
C*								       *
C* Output parameters:						       *
C*     datatm            char*12        Report date/time               *
C*     zone              char*12        Time zone                      *
C*     iret		 integer	return code                    *
C*					  0 = normal return            *
C*					 -1 = invalid report           *
C*								       *
C**								       *
C*Log:						       		       *
C*P.Bruehl/NWS	4/98						       * 
C***********************************************************************
        INCLUDE         'shcmn.cmn'

        CHARACTER*12    datatm, tmpdate, zone
        CHARACTER*36    tmpstr
C*
        INTEGER         YD(5), YS(5)
	INTEGER		iret, next, iyear
C-----------------------------------------------------------------------
C
C*  Daylight savings time dates
C             Year:  1997, 1998, 1999, 2000,  Unk
        DATA  YD   /  406,  405,  404,  402, 1231 /
        DATA  YS   / 1026, 1025, 1031, 1029,  101 /
C-----------------------------------------------------------------------
	iret = 0
        zone = "ZZ"

           
C
C*  Time in YYMMDD/1200 or MMDD/1200
C*  Unspecified obs time in UTC defaults to 2400
C
        CALL ST_LSTR ( cfield(2), lenc, ierr )
        datatm = cfield(2)(1:lenc) // '/1200'
        idate = NINT ( rfval(2) )
        CALL ST_LSTR ( datatm, lent, ierr )
        IF ( lent .lt. 11 ) THEN 
              CALL ST_INCH ( iyear, tmpdate, ierr )
              datatm = tmpdate(3:4) // cfield(2)(1:lenc) // '/1200'
        ENDIF


        next = 3
        IF ( ( cfield(3)(1:1) .ne. "D" ) .and.
     +       ( cfield(3)(1:1) .ne. "/" )  )  THEN
C
C*   Time zone present
C
         zone = cfield(3)
	 IF ( zone .eq. "Z" ) zone = "ZZ"
         next = 4

         CALL ST_LSTR ( zone, lenz, ierr )
         IF ( lenz .eq. 1 ) THEN
C
C*  Determine correct local time zone 
C
          IF ( iyear .eq. 1997 ) THEN
            indx = 1
          ELSE IF ( iyear .eq. 1998 ) THEN
            indx = 2
          ELSE IF ( iyear .eq. 1999 ) THEN
            indx = 3
          ELSE IF ( iyear .eq. 2000 ) THEN
            indx = 4
          ELSE
C
C*           Can't determine time zone.  Assume standard time
C             
             indx = 5 
             tmpstr = 'Assuming local standard time'
             CALL DC_WLOG ( 0, 'SH', indx, tmpstr, ier )
          ENDIF

          IF ( idate .gt. 1231 ) jdate =  MOD ( idate , 10000)

          IF ( ( jdate .lt. YD(indx) ) .or.  
     +               ( jdate .ge. YS(indx) ) ) THEN
             tmpstr = zone(1:1) // "S"
             zone = tmpstr
          ELSE
             tmpstr = zone(1:1) // "D"
             zone = tmpstr
          ENDIF
C
C*      Unspecified obs time in local time zone defaults to 2400
C
         tmpdate = datatm
         datatm = tmpdate(1:7) // '2400'
         END IF

        ENDIF
        RETURN
        END
