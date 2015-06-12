      SUBROUTINE SH_DINC ( sheftm, datatm, iret)
C***********************************************************************
C* SUBROUTINE SH_DINC                                                  *
C*                                                                     *
C* This subroutine decodes the increment date/time from a SHEF         *
C* .E report and increments the GEMPAK time (YYMMDD/HHMM) in UTC.      *
C* 								       *
C* Input parameters: 						       *
C*	sheftm		char*12      	Increment (DI group)           *
C*								       *
C* Input/Output parameters:                                            *
C*  	datatm		char*12		Incremented SHEF date/time     *
C*                                                                     *
C* Output parameters:						       *
C*      iret		 integer	return code                    *
C*					  0 = normal return            *
C*								       *
C**								       *
C*Log:						       		       *
C*P.Bruehl/NWS	3/98						       *
C***********************************************************************
	CHARACTER*12	sheftm, datatm, tmpdate
        CHARACTER*36    tmpstr
        INTEGER         idtarr(5)

C*
        CHARACTER*2     cc, nn
C-----------------------------------------------------------------------
	iret = 0

        cc = sheftm(3:4)
        nn = sheftm(5:6)
        CALL ST_LSTR ( nn, lenn, iret )
        CALL ST_C2I  ( nn, lenn, inn, nval, iret )
        CALL TI_CTOI ( datatm, idtarr, ierr)

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
          imin = inn * 60 * 24
          IF ( cc(2:2) .eq. '+' ) THEN
            CALL TI_ADDM ( idtarr, imin, idtarr, ierr)
          ELSE
            CALL TI_SUBM ( idtarr, imin, idtarr, ierr)
          ENDIF

        ELSE IF ( cc(1:1) .eq. 'M' ) THEN
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

        ELSE IF ( cc(1:1) .eq. 'Y' ) THEN
            iyy = inn
          IF ( cc(2:2) .eq. '+' ) THEN
            idtarr(1) = idtarr(1) + inn
          ELSE
            idtarr(1) = idtarr(1) - inn
          ENDIF

        ELSE IF ( cc(1:1) .eq. 'E' ) THEN

          IF ( cc(2:2) .eq. '+' ) THEN
C
C*         End of month: 
C*          add n+1 months, set to first day, subtract day
C
            idtarr(2) = idtarr(2) + inn + 1
            IF ( idtarr(2) .ge. 13 ) THEN
               idtarr(1) = idtarr(1) + 1
               idtarr(2) = idtarr(2) - 12
            ENDIF
          ELSE
C
C*         subtract n-1 months, set to first day, sub day
C
            idtarr(2) = idtarr(2) - inn + 1
            IF ( idtarr(2) .ge. 0 ) THEN
               idtarr(1) = idtarr(1) - 1
               idtarr(2) = idtarr(2) + 12
            ENDIF
          ENDIF
          idtarr(3) = 1
          CALL TI_SUBD ( idtarr, idtarr, iret )  

        ELSE
C
C*  Time modifier not handled.  Return
C
          tmpdate = datatm
          tmpstr = 'Unk DI grp ' // sheftm
          CALL DC_WLOG ( 5, 'SHA', -1 , tmpstr, ier )
        ENDIF

        CALL TI_ITOC ( idtarr, tmpdate, ierr)

        datatm = tmpdate

        RETURN
        END
