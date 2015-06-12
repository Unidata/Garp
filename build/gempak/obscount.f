C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	sfccount.f
C*
C*	Utilities to count the number of stations for a certain time and
C*	area for surface and upperair obs.
C*
C*	History:
C*
C*	10/97	COMET		Original copy
C*
C************************************************************************



	SUBROUTINE countsfcobs ( sffile, dattim, nstns )
C************************************************************************
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
C*
	CHARACTER	dattim*(*), sffile*(*), area*48
C*
	CHARACTER	sffcur*72, arecur*48, pmdset (MMPARM)*4
	CHARACTER	tstn*8, sta*8
        LOGICAL		newfil, dtaflg

C------------------------------------------------------------------------
	nstns = 0
	arecur = ' '
	sffcur = ' '
C
C*	Process filename.
C
	CALL SFMFIL  (  sffile, sffcur, iflno, newfil, pmdset, 
     +			npmdst, iret )
	IF  ( iret .ne. 0 )  RETURN
C
	CALL get_data_area ( area, ier )
C
C*	Set area.
C
	CALL LC_UARE  ( area, newfil, iflno, arecur, tstn, ier )
C
C*	Set the time
C
	CALL SF_STIM  ( iflno, dattim, ier )
C
C*	Station loop.
C
	iout = 0
	DO  WHILE  ( iout .eq. 0 )

	    CALL SF_SNXT ( iflno, sta, id, slat, 
     +				slon, selv, ispri, iout )
	    IF  ( iout .eq. 0 )  THEN
C
C*	        Query for data.
C
		CALL SF_QDAT ( iflno, dtaflg, ier )
		if ( dtaflg ) nstns = nstns + 1
C
	    END IF
C
	END DO
C
        CALL SF_CLOS (iflno, iret)

	RETURN
	END
C
C
	SUBROUTINE countupaobs ( snfile, dattim, nstns )
C************************************************************************
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
C*
	CHARACTER	area*48, dattim*(*), snfile*(*)
C*
	CHARACTER	snfcur*72, arecur*48, pmdset (MMPARM)*4
	CHARACTER	tstn*8, sta*8
        LOGICAL		newfil, dtaflg

C------------------------------------------------------------------------
	nstns = 0
	arecur = ' '
	snfcur = ' '
C
C*	Process filename.
C
	CALL SNMFIL  (  snfile, snfcur, iflno, newfil, pmdset, 
     +			npmdst, ivert, iret )
	IF  ( iret .ne. 0 )  RETURN
C
	CALL get_data_area ( area, ier )
C
C*	Set area.
C
	CALL LC_UARE  ( area, newfil, iflno, arecur, tstn, ier )
C
C*	Set the time
C
	CALL SN_STIM  ( iflno, dattim, ier )
C
C*	Station loop.
C
	iout = 0
	DO  WHILE  ( iout .eq. 0 )

	    CALL SN_SNXT ( iflno, sta, id, slat, 
     +					slon, selv, iout )
	    IF  ( iout .eq. 0 )  THEN
C
C*	        Query for data.
C
		CALL SN_QDAT ( iflno, dtaflg, ier )
		if ( dtaflg ) nstns = nstns + 1
C
	    END IF
C
	END DO
C
        CALL SN_CLOS (iflno, iret)

	RETURN
	END
