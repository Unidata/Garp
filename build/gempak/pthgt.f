C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	pthgt.f
C*
C*	Time/height cross section. Derived from the GEMPAK program
C*	GDTHGT.
C*
C*	History:
C*
C*	11/96	COMET		Original copy
C*	 2/97	J. Cowie/COMET	Cleaned up, changed title time
C*	 5/97	COMET		Added gprintf to support logging.
C*	11/97	COMET		Added ptitle to display clickable titles.
C*	 5/99	COMET		Added calls to reset current color.
C*	 3/07	S. Chiswell/Unidata	Updated gdtxlv call, gclgrn
C*
C************************************************************************


	SUBROUTINE pthgt ( gdfile, gdatim, gvcord, gfunc, gpoint,
     +                     gvect, ptype, taxis, yaxis, line, cint,
     +                     wind, title, scale, text, ititle, verbose,
     +                     iret )

C************************************************************************
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
C*
	CHARACTER	gdfile*(*), gdatim*(*), gvcord*(*),
     +			gfunc*(*), gpoint*(*), gvect*(*),
     +			ptype*(*), taxis*(*), yaxis*(*), line*(*),
     +			cint*(*), wind*(*), title*(*),
     +			scale*(*), text*(*)

	CHARACTER*80	border, shrttl


	INTEGER		verbose
C*
	REAL 		grdout (LLMXGD), grdoutu (LLMXGD),
     +			grdoutv (LLMXGD)
	REAL		rlvld (LLMXLV), rlvlc (LLMXLV), rlvlv (LLMXLV)
	REAL	        xgrd (LLMXGD), x (LLMXTM), xptsc (LLMXGD),
     +			ylbl (LLAXIS),
     +                  rgx(1), rgy(1), rlat(1), rlon(1),
     +			vclsfc, ggrd (LLMXGD), xtlbl (LLAXIS),
     +			grdt1 (LLMXGD)
	CHARACTER	ttlstr*72, ctlbl (LLAXIS)*40, pfunc*80,
     +			trange (2)*40, timfnd (LLMXTM)*20, ttl*72,
     +			time (2)*40, blank*2, clbl(LLCLEV)*24
	LOGICAL		done, proces, havsfc, havvec, havcon, scflag
C*
	REAL		clvl (LLCLEV), rmargn (4)
	INTEGER		icolor (LLCLEV), iline (LLCLEV),
     +			ilwid  (LLCLEV), labflg (LLCLEV),
     +			levels (LLMXLV), levl (2)
C-----------------------------------------------------------------------
	iperr = 0
	iret = 0
	iscale = 0
	ioldclr = 0
	border='1'
	blank = ' ' // char(0)
	done = .false.
	proces = .true.

	if ( verbose .gt. 0 ) 
     +      call gfprints ( 'pthgt'//char(0), blank )
	if ( verbose .gt. 1 ) then
	    call gfprints ( '  gdfile = ' // char(0), gdfile )
	    call gfprints ( '  gdatim = ' // char(0), gdatim )
	    call gfprints ( '  gvcord = ' // char(0), gvcord )
	    call gfprints ( '  gfunc = ' // char(0), gfunc )
	    call gfprints ( '  gpoint = ' // char(0), gpoint )
	    call gfprints ( '  gvect = ' // char(0), gvect )
	    call gfprints ( '  ptype = ' // char(0), ptype )
	    call gfprints ( '  taxis = ' // char(0), taxis )
	    call gfprints ( '  yaxis = ' // char(0), yaxis )
	    call gfprints ( '  line = ' // char(0), line )
	    call gfprints ( '  cint = ' // char(0), cint )
	    call gfprints ( '  wind = ' // char(0), wind )
	    call gfprints ( '  title = ' // char(0), title )
	    call gfprints ( '  scale = ' // char(0), scale )
	    call gfprints ( '  text = ' // char(0), text )
	    call gfprinti ( '  ititle = ' // char(0), ititle )
	    call gfprinti ( '  verbose = ' // char(0), verbose )
	    call gfprinti ( '  iret = ' // char(0), iret )
	end if

C
C	Added these calls to make sure the GEMPAK color scheme is
C	in a proper state. When a title is toggled off, the
C	X function XSetForeground() in DrawToggledOffTitle() sets 
C	the color to a non-GEMPAK color. This discombobultes the 
C	GEMPAK method of buffering the current color.
C 
        CALL gqtext ( igtxfn, igtxhw, gsztext, igtxwid, 
     +     ibrdr, irrotn, ijust, iret)
        CALL dstext ( igtxfn, igtxhw, gsztext, igtxwid,
     +                ibrdr, irrotn, ijust,
     +                idtxfn, idtxhw, dsztext, idtxwid, 
     +                dbrdr, drrotn, djust, iret)
        CALL gqcolr ( igcolr, iret )
        CALL dscolr ( igcolr, ioldcolr, iret )

C
C*      Process the GDFILE input.
C
        CALL DG_NFIL ( gdfile, ' ', ier )
        IF ( ier .ne. 0 ) THEN
           CALL ER_WMSG ( 'DG', ier, ' ', irr )
           proces = .false.
	ELSE
C
C*          Process the GDATTIM input; setup the time server.
C
            CALL DG_NDTM ( gdatim, ier )
            IF ( ier .ne. 0 ) THEN
                CALL ER_WMSG ( 'DG', ier, gdatim, irr )
                proces = .false.
	    ELSE
C
C*              Get time information
C
                CALL DG_QTMS ( LLMXGT, .false., timfnd, ntime,
     +                             trange (1), iret )
                IF ( iret .ne. 0 ) proces = .false.
C
C*              Check for number of times greater than zero.
C
                proces = ( ntime .gt. 0 )
            END IF
	END IF

	IF ( proces ) THEN
C
C	    Set text.
C
	    CALL IN_TEXT ( text, iret )
c
	    CALL ST_LCUC ( gvcord, gvcord, iret )
	    CALL LV_CORD ( gvcord, gvcord, ivcord, iret )
	    IF ( ( iret .ne. 0 ) .or. ( ivcord .eq. 0 ) )
     +	        proces = .false.
	END IF
C
C*	Get y-axis parameters
C
	IF ( proces ) THEN
	    CALL GDTXYY( ptype, yaxis, ivcord, iyaxis, ratio,
     +			 ystrt, ystop, ylbl, nylbl, rmargn,
     +			 ilbfrq, iglfrq, itmfrq, iret )
	    IF ( iret .ne. 0 ) THEN
		CALL ER_WMSG( 'GDTHGT', iret, ' ', ier )
		proces = .false.
	    END IF
	END IF
C
C*	Get levels
C
	IF ( proces ) THEN
	    CALL GDTXGL ( 1, timfnd, 1, ystrt, ystop,
     +				  ivcord, levels, nlev, iret )
	    IF ( iret .ne. 0 ) proces = .false.
	END IF
	IF ( proces ) THEN
            DO i = 1, ntime
                CALL TG_VALD ( timfnd(i), timfnd(i), iret )
                IF ( iret .ne. 0 ) proces = .false.
            END DO
        END IF
C
C*	Get x-axis parameters
C
	IF ( proces ) THEN
	    CALL GDTXTA ( taxis, LLAXIS, ntime, timfnd, x,
     +			  xstrt, xstop, xtlbl, ctlbl, nxlbl,
     +			  xmndst, itlbfr, itglfr, ittmfr,
     +			  iret )
	    IF ( iret .ne. 0 ) proces = .false.
	END IF
C
C*	Get even levels
C
	IF ( proces ) THEN
	    DO i = 1, nlev
		rlvld (i) = float (levels (i))
	    END DO
	    CALL GDTXEV ( rlvld, nlev, iyaxis, rlvlc, nlvlc,
     +			  iret )
	    CALL GDTXEV ( rlvld, nlev, 1, rlvlv, nlvlv, iret )
	    IF ( iret .ne. 0 ) proces = .false.
	END IF
C
	IF ( proces ) THEN
	    CALL GDTXEV ( x, ntime, 1, xptsc, nptsc, iret)
	    IF ( iret .ne. 0 ) proces = .false.
	END IF
C
C*	Get data for this list of times and levels
C
	havcon = .false.
	IF ( proces ) THEN
	    havcon = .true.
	    CALL GDTXDA ( igdfln, timfnd, ntime, gvcord,
     +			  ystrt, ystop, iyaxis, gfunc, ivcord,
     +			  gpoint, levels, nlev, grdt1, rlvlc,
     +			  nlvlc, iret )
	    IF ( iret .eq. 7 ) havcon = .false.
	    IF ( iret .lt. 0 ) proces = .false.
	    IF ( iret .eq. -19 )
     +			print*,'*** Error in pthgt - Invalid function'
	END IF
C
C*	Put contour data in evenly spaced grid
C
	IF ( proces .and. havcon ) THEN
	    CALL GDTXTI( grdt1, ntime, nlvlc, x, xptsc, nptsc,
     +			 grdout, iret)
	    IF ( iret .ne. 0) proces = .false.
	END IF
C
C*	Get Vector data for this list of times and levels
C
	havvec = .false.
	IF ( proces ) THEN
	    havvec = .true.
	    CALL GDTXVV ( igdfln, timfnd, ntime, gvcord,
     +			  ystrt, ystop, iyaxis, gvect, ivcord,
     +			  gpoint, levels, nlev, grdoutu,
     +			  grdoutv, rlvlv, nlvlv ,iret )
	    IF ( iret .eq. 7 ) havvec = .false.
	    IF ( iret .lt. 0 ) proces = .false.
	    IF ( iret .eq. -1 ) print*,
     +'*** Error in pthgt - Cannot use CIRC function in time/height'
	END IF
C
	IF ( proces .and. havcon ) THEN
	    CALL GDTXLV( line, cint, scale, nptsc, nlvlc, 1, 1,
     +				 nptsc, nlvlc, grdout, icolor, iline,
     +				 ilwid, labflg, iscale, dmin, dmax,
     +				 clvl, nlvl, clbl, scflag, iret )
	    IF ( iret .ne. 0 ) proces = .false.
	ELSE
	    dmin = 0.0
	    dmax = 0.0
	    nlvl = 0
	END IF
C

	IF ( proces ) THEN
	    CALL GQMODE ( mode, ier )
	    CALL GSMODE ( 2, ier )
C
	    havsfc = .false.
	    CALL GDTXPL( border, iyaxis, ystrt, ystop, vclsfc,
     +			 havsfc, ylbl, nylbl, xstrt, xstop,
     +			 xtlbl, ctlbl, nxlbl, ntime, itlbfr,
     +			 itglfr, ittmfr, ratio, rmargn, ilbfrq,
     +			 iglfrq, itmfrq, iret )
C
	    CALL GSGGRF ( 1, iyaxis, nptsc, nlvlc, xptsc (1),
     +			  ystrt, xptsc (nptsc), ystop, iret )
	    IF ( havcon ) THEN
		CALL GCLGRN ( nptsc, nlvlc, grdout, 0, 0, 0,
     +			      nlvl, clvl, clbl, icolor, iline, ilwid,
     +			      labflg, scflag, iret )
	    END IF
	    IF ( havvec ) THEN
		indx = 1
		DO k = 1, nlvlv
		    DO i = 1, ntime
			xgrd (indx) = x (i)
			ggrd (indx) = rlvlv (k)
			indx = indx + 1
		    END DO
		END DO
		CALL GDTXPV ( gvect, grdoutu, grdoutv, xgrd,
     +			      ggrd, ntime, nlvlv, wind,
     +			      points, ier )
	    END IF
C
C*	    Write the title.
C
	    CALL IN_TITL ( title, 0, ititl, linttl, ttlstr,
     +			   ier )
	    IF ( ititl .ne. 0 ) THEN
		CALL GSCOLR ( ititl, ier )
		CALL DSCOLR  ( ititl, ioldclr, iret )
		lens = LEN ( ttlstr )
		ttlstr(lens:lens) = char(0)
		call ptitle ( ttlstr, ititle )
	    END IF
C
C*	    Reset plotting mode
C
	    CALL GSMODE ( mode, ier )
C
	END IF
C
C	flush buffers, close file.
C
C	CALL GFLUSH  ( iret )
	CALL DG_NEND ( iret )
C
	if ( verbose .gt. 0 ) call gfprinti ( 
     +  'returning from pthgt - iret = ' // char(0), iret )

	return
	END
