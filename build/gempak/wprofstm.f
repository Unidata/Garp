C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	wprofstm.f
C*
C*	Used by wind profiler program to get stations for analysis.
C*	Derived from the GEMPAK program SNSSTM.
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*
C************************************************************************


	SUBROUTINE wprofstm ( cxstns, isnfln, nparms, MSDSIZ, times,
     +			      ntime, taxis, ndtim, ipsdat, nlvls, 
     +			      stndat, idtype, sloc, xmin, xmax, xtlbl,
     +			      ctlbl, nxlbl, ixlbfr, ixglfr, ixtmfr, 
     +			      iret )
C************************************************************************
C************************************************************************
	INCLUDE 	'GEMPRM.PRM'
C*
	CHARACTER*(*)	cxstns, times (*), taxis, ctlbl (*)
	REAL		stndat (*), sloc (*), xtlbl (*)
	INTEGER		ipsdat (*), nlvls (*), idtype (LLMXLV, *)
C*
	CHARACTER	stnin*8, cid*8, dtim (LLMXTM)*12
C------------------------------------------------------------------------
C*	Break CXSTNS into station array.
C
	CALL ST_LCUC ( cxstns, cxstns, ier )
	CALL ST_CLST ( cxstns, ';', ' ', 1, stnin, nstin, ier )
C
C*	Return error if there is more than one station.
C
	IF  ( stnin .eq. ' ' )  THEN
	    iret = -6
	  ELSE IF  ( ier .ne. 0 )  THEN
	    iret = -16
	  ELSE
	    iret = 0
	END IF
	IF  ( iret .ne. 0 )  THEN
	    CALL ER_WMSG  ( 'SNCROSS', iret, cxstns, ier )
	    RETURN
	END IF
C
C*	Read in the data from the stations.
C
	ndtim  = 0
	iptr   = 1
C
C*	Loop through the times.
C
	DO  itime = 1, ntime
C
C*	    Set the next time and station.
C
	    CALL SN_STIM  ( isnfln, times (itime), ier )
	    CALL SN_SSTN  ( isnfln, stnin, cid, idnum, rlat, rlon, elv,
     +			    ier2 )
	    IF  ( ( ier .ne. 0 ) .or. ( ier2 .ne. 0 ) )  THEN
		ier = -17
		CALL ER_WMSG  ( 'SNCROSS', ier, times (itime), ier2 )
	      ELSE
		CALL SN_RDAT  ( isnfln, numlev, stndat (iptr), ihhmm,
     +				ier )
		IF  ( ier .ne. 0 )  THEN
		    CALL ER_WMSG  ( 'SNCROSS', -18, times (itime), ir2 )
		END IF
	    END IF
C*
	    IF  ( ier .eq. 0 )  THEN
		ndtim = ndtim + 1
	        CALL SN_RTYP  ( isnfln, numlev, idtype (1, ndtim), ier )
		dtim (ndtim)  = times (itime)
		nlvls (ndtim)  = numlev
		ipsdat (ndtim) = iptr
		iptr = iptr + numlev * nparms
		IF  ( iptr .gt. MSDSIZ )  THEN
		    iret = -7
		    CALL ER_WMSG ('SNCROSS', iret, ' ', ier )
		    RETURN
		END IF
	    END IF
	END DO
C
C*	Check that at least one valid time was found.
C
	IF  ( ndtim .lt. 1 )  THEN
	    iret = -6
	    CALL ER_WMSG ( 'SNCROSS', iret , ' ', ier )
	    RETURN
	END IF
C
C*	Set up time axis.
C
	CALL IN_TAXS  ( taxis, LLTMCX, ndtim, dtim, sloc, xmin, 
     +		       xmax, xtlbl, ctlbl, nxlbl, xmndst, ixlbfr,
     +		       ixglfr, ixtmfr, ier )
C*
	RETURN
	END
