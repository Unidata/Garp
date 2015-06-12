	SUBROUTINE SH_UNIT  ( parm, rscale, iret )
C************************************************************************
C* SH_UNIT								*
C*									*
C* Reads SHEF variable list and converts from S.I. to English units	*
C* using Appendix B of the SHEF Users Manual.  The table is called	*
C* shefparm.tbl and is expected in $GEMTBL/stns/			*
C*									*
C* SH_RTIM  ( PARM, RSCALE, IRET )					*
C*									*
C* Input parameters:							*
C*	PARM		CHAR*		Parameter string		*
C*									*
C* Output parameters:							*
C*	RSCALE		REAL		Scale factor	 		*
C*	IRET		INTEGER		Return code			*
C*					  0 = normal return		*
C*	   			         -3 = table not opened		*
C*					 -4 = parm not in table		*
C**									*
C* Log:									*
C* P.Bruehl/NWS 	 3/98	Based on TB_FGEO			*
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
C*
	CHARACTER*(*) 	parm
C*
	LOGICAL 	datfnd
	REAL		rscale, tval
	CHARACTER 	dparm*2, tparm*2, tbline*136
C------------------------------------------------------------------------
	iret   = -4
	
	rscale = 1.0
C
C*	Open the file and check for errors.
C
	CALL FL_TBOP  ( 'shefunits.tbl', 'stns', lun, ier )
	IF  ( ier .ne. 0 )  THEN
	    CALL ER_WMSG  ( 'FL', ier, 'shefunits.tbl', ierr )
	    iret = -3
	END IF
C
C*	Read the table. Search for the first two chars of the parm
C
        dparm = parm(1:2)

	datfnd = .false.
	DO WHILE  ( .not. datfnd )
C
C*	    Read the next record.
C
	    READ ( lun, 1000, IOSTAT = iostat ) tbline
1000	    FORMAT ( A )
	    IF ( iostat .ne. 0 ) THEN
		CALL FL_CLOS ( lun, ier )
		RETURN
	    END IF
	    IF ( tbline (1:1) .eq. '!' ) THEN
		datfnd = .false.
	    ELSE
		CALL ST_RLST ( tbline (4:75), ' ', RMISSD, 1, tval,
     +			       nvals, iern )
		tparm = tbline (1:2)
		IF ( nvals .ne. 1 ) THEN
		    ier = -10
		    CALL ER_WMSG  ( 'TB', ier, 'geog.tbl', ierr )
		    CALL FL_CLOS  ( lun, ier )
		    RETURN
		ELSE IF ( tparm .eq. dparm ) THEN
		    IF ( (iern .eq. 0) .and. (nvals .eq. 1) ) THEN
			datfnd    = .true.
                        rscale = tval
			iret = 0
		    ELSE
			ier = -11
			CALL ER_WMSG ( 'TB', ier, 'geog', ierr )
			CALL FL_CLOS  ( lun, ier )
			RETURN
		    END IF
		END IF
	    END IF
	END DO
C
C*	Close the table file.
C
	CALL FL_CLOS  ( lun, ier )
C*
	RETURN
	END
