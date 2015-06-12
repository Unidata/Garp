C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	pmap.f
C*
C*	Grid data ploting function.
C*
C*	History:
C*
C*	11/96	COMET		Original copy
C*	 2/97	J. Cowie/COMET	Cleaned up, changed title string
C*	 5/97	COMET		Added gprintf to support logging.
C*	11/97	COMET		Added ptitle to display clickable titles.
C*	 8/05	S. Chiswell/Unidata	Updated for new grid library
C*
C************************************************************************


	SUBROUTINE pmap ( gdfile, gdatim, glevel, gvcord, ggfunc,
     +                    gridtype, color, cint, scale, title, skip,
     +                    text, nfunc, ititle, verbose, iperr )

C************************************************************************
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
C*

	CHARACTER	positn*72, marker*72, grdlbl*72, panel*72
	

        CHARACTER       gdfile*(*), gdatim*(*), glevel*(*)  ,
     +                  gvcord*(*), ggfunc*(*), gridtype*(*),
     +                  color*(*) , cint*(*)  , scale*(*)   ,
     +                  title*(*) , skip*(*)  , text*(*)

	INTEGER		verbose
C*
	REAL		grid (LLMXGD), grid1 (LLMXGD), grid2 (LLMXGD),
     +			rarr (3), farr(4)
	INTEGER		level (2), iskplt(2)
	CHARACTER	time (2)*20, ttlstr*72,
     +			parm*12, parmu*12, parmv*12,
     +			pfunc*72, gfunc*72, gtype*12, ttl*72, blank*2,
     +			pstring*(LLMXLN), gstring*(LLMXLN), mproj*72,
     +			garut*72, prjut*72, timfnd*36, strout*72
	LOGICAL		proces, first, nxttim
C*
C-----------------------------------------------------------------------

	positn = '0'
	marker = '0'
	grdlbl = '0'
        panel = '0'
        iperr = 0
	ioldclr = 0
	blank = ' ' // char(0)

	if ( verbose .gt. 0 )
     +	       call gfprints ( 'pmap'//char(0), blank )
	if ( verbose .gt. 1 ) then
	    call gfprints ( '  gdfile = ' // char(0), gdfile )
	    call gfprints ( '  gdatim = ' // char(0), gdatim )
	    call gfprints ( '  glevel = ' // char(0), glevel )
	    call gfprints ( '  gvcord = ' // char(0), gvcord )
	    call gfprints ( '  gfunc = ' // char(0), ggfunc )
	    call gfprints ( '  color = ' // char(0), color )
	    call gfprints ( '  cint = ' // char(0), cint )
	    call gfprints ( '  scale = ' // char(0), scale )
	    call gfprints ( '  title = ' // char(0), title )
	    call gfprints ( '  skip = ' // char(0), skip )
	    call gfprints ( '  text = ' // char(0), text )
	    call gfprinti ( '  nfunc = ' // char(0), nfunc )
	    call gfprinti ( '  ititle = ' // char(0), ititle )
	    call gfprinti ( '  verbose = ' // char(0), verbose )
	    call gfprinti ( '  iperr = ' // char(0), iperr )
	end if

C
C	Set text.
C
	CALL IN_TEXT ( text, ier )

C
C*	Set flag to indicate processing will be done.
C
	proces = .true.
C
        CALL DG_NFIL ( gdfile, ' ', ier )
        IF ( ier .ne. 0 ) THEN
            CALL ER_WMSG ( 'DG', ier, ' ', irr )
            proces = .false.
        END IF
C
C*      Process the GDATTIM input; setup the time server.
C
        CALL DG_NDTM ( gdatim, ier )
        IF ( ier .ne. 0 ) THEN
            CALL ER_WMSG ( 'DG', ier, gdatim, irr )
            proces = .false.
        END IF
C
C*      Get the next time to process from time server.
C
        CALL DG_NTIM ( .true., .true., time, nxttim, ier )

        IF  ( ier .eq. 0 )  THEN
               
            CALL TG_DUAL ( time, timfnd, ier )

            CALL GQMPRJ  ( mproj, rarr(1), rarr(2), rarr(3),
     +                      farr(1), farr(2), farr(3), farr(4), ier )
C
            CALL ST_LSTF ( rarr, 3, ';', 1, strout, ier )
            CALL ST_LSTR ( mproj, ilen, ier )
            pstring = mproj ( 1 : ilen ) // '/' // strout
C
            CALL ST_LSTF ( farr, 4, ';', 1, gstring, ier )
C
            CALL DG_FIXA ( gstring, pstring, garut, prjut, ier )
            CALL DG_SUBG ( 'N', ix1, iy1, ix2, iy2, iret )
            IF  ( iret .ne. 0 )  THEN
                CALL ER_WMSG ( 'DG', iret, ' ', ier )
                proces = .false.
            END IF
        ELSE
            proces = .false.
        END IF

	IF ( proces ) THEN
C
C*          Check for points to skip.
C
	    CALL IN_SKIP  ( skip, iskpcn, iskplt, ier )
	    ixinc = iskplt (1)
	    iyinc = iskplt (2)
C
C*	    Check for stagger.
C
	    IF  ( ixinc .ge. 0 ) THEN
		ixstep = ixinc + 1
		istag  = 0
	    ELSE
		ixstep = - ixinc + 1
		istag  = ixstep / 2
	    END IF
	    iystep = iyinc + 1
	END IF
C
C*	Process the grid identifier only if the grid file was 
C*	successfully opened.
C
	IF  ( proces )  THEN
	    first = .true.
C
C*	    Compute the requested grid.
C
	    DO i=1, nfunc

		llen = 72
		ibeg = llen * ( i - 1 ) + 1
		iend = ibeg + llen - 1

		igl = 2
		igb = igl * ( i - 1 ) + 1
		ige = igb + 1

		gfunc = ggfunc(ibeg:iend)
		pfunc = gfunc
		gtype = gridtype(ige:ige)


	        if      ( gtype .eq. 's' ) then
		    CALL DG_GRID ( timfnd, glevel, gvcord,
     +				    gfunc, pfunc, grid, kx, ky, time,
     +				    level, ivcord, parm, iret )
		else if ( gtype .eq. 'v' ) then
                     CALL DG_VECT ( timfnd, glevel, gvcord, gfunc,
     +                              pfunc, grid1, grid2, kx, ky, time,
     +                              level, ivcord, parmu, parmv, iret )
	        endif
	    ENDDO 

	    IF  ( iret .ne. 0 )  THEN
		    CALL ER_WMSG ( 'DG', iret, pfunc, ier )
		    proces = .false.
	    END IF
	END IF
C
C*	Get scaling factor.
C
	IF  ( ( proces ) .and. (gfunc .ne. ' ' ) )  THEN
	    CALL IN_SCAL  ( scale, iscale, iscalv, iret )
	    CALL GR_SSCL  ( iscale, kx, ky, ix1, iy1, ix2, 
     +                                  iy2, grid, rmin, rmax, iret )
            IF  ( iret .ne. 0 )  proces = .false.
	END IF
C
C*	Plot data.
C
	IF  ( proces ) THEN
C
C*	    Draw markers.
C
	    CALL GDMMRK  ( marker, kx, ky, ix1, iy1,
     +					ix2, iy2, ixstep, istag,
     +					iystep, ier )
C
C*	    Label grid axes.
C
	    CALL GDMLBL  ( grdlbl, kx, ky, ix1, iy1,
     +					ix2, iy2, ixstep, iystep, ier )
C
C*	    Get range for data.
C
	    IF  ( gfunc .ne. ' ' )  THEN
		CALL ST_RLST ( cint, '/', RMISSD, 3, rarr, n, ier )
csd Need ermiss include file.
csd		IF  ( ERMISS ( rarr (2) ) )  THEN
		    rmind = -1. E10
csd		ELSE
csd		    rmind = rarr (2)
csd		END IF
csd		IF  ( ERMISS ( rarr (3) ) )  THEN
		    rmaxd = +1. E10
csd		ELSE
csd		    rmaxd = rarr (3)
csd		END IF
C
C*              Plot data.
C
		CALL GDMPLT ( grid, kx, ky, ix1, iy1,
     +					   ix2, iy2, ixstep, istag,
     +					   iystep, color, positn,
     +					   rmind, rmaxd, iret )
 	    END IF
C
C*	    Write title.
C
	    CALL IN_TITL  ( title, 0, ititl, linttl,
     +					 ttlstr, ier )
	    IF ( ititl .ne. 0 ) THEN
		CALL GSCOLR   ( ititl, ier )
		CALL DSCOLR  ( ititl, ioldclr, iret )
		lens = LEN ( ttlstr )
		ttlstr(lens:lens) = char(0)
		call ptitle ( ttlstr, ititle )
	    END IF
	END IF
C
C*
C	CALL GFLUSH ( iret )
	CALL DG_NEND ( ier )
C
	if ( verbose .gt. 0 ) call gfprinti ( 
     +  'returning from pmap - iret = '//char(0), iret )
	RETURN
	END
