C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	pvprof.f
C*
C*	Vertical profile drawing function. Derived from the GEMPAK
C*	program GDPROF.
C*
C*	History:
C*
C*	11/96	COMET		Original copy
C*	 2/97	J. Cowie/COMET	Cleaned up, changed title time
C*       3/97   COMET		Added wind position indicator.
C*       4/97   COMET		Zero out scalar field when a vector field
C*				is plotted.
C*	 5/97	COMET		Added gprintf to support logging.
C*	 7/97	COMET		Initialize npts=0 to avoid overflow.
C*	11/97	COMET		Added ptitle to display clickable titles.
C*	11/97	COMET		Added call to dscolor, cleaned up.
C*	 5/99	COMET		Added calls to reset current color.
C*
C************************************************************************


	SUBROUTINE pvprof ( gdfile, gdatim, gvcord, gfunc, gpoint,
     +                      gvect, ptype, xaxis, yaxis, line, wind,
     +                      refvec, winpos, thtaln, thteln, mixrln,
     +                      filter, title, scale, text, frame, ititle,
     +                      verbose, iret )
C************************************************************************
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
C*
	CHARACTER	gdfile*(*), gdatim*(*), gvcord*(*),
     +			gfunc*(*),  gpoint*(*), gvect*(*),
     +			ptype*(*),  xaxis*(*),  yaxis*(*), line*(*),
     +			wind*(*),   refvec*(*), winpos*(*),
     +			thtaln*(*), thteln*(*), mixrln*(*),
     +			filter*(*), title*(*),
     +			scale*(*),  text*(*)

	CHARACTER	border*72, marker*72, shrttl*72,
     +			ttl*72, panel*72
C*
	REAL		x (LLMXLV), y (LLMXLV), xlbl (LLAXIS), 
     +			ylbl (LLAXIS), u (LLMXLV), v (LLMXLV),
     +                  yv (LLMXLV), rmargn (4)
C*
	INTEGER		verbose, frame
	INTEGER         ithtal (6), ithtel (6), imixrl (6), level (2)
	CHARACTER	time (2)*20, lastim*20, ttlstr*72, parm*12,
     +			parmu*12, parmv*12, firstm*20, blank*2,
     +			timfnd*36
	LOGICAL		proces, havwnd, havscl, first, gottm
C-----------------------------------------------------------------------

	iperr = 0
	iret = 0
	iscale = 0
	ioldclr = 0
	blank = ' ' // char(0)
	npts = 0

	MARKER   = '0'
	border   = '1'
	panel    = '0'

	if ( verbose .gt. 0 ) call gfprints ( 
     +     'pvprof'//char(0), blank )
	if ( verbose .gt. 1 ) then
	    call gfprints ( '  gdfile = ' // char(0), gdfile )
	    call gfprints ( '  gdatim = ' // char(0), gdatim )
	    call gfprints ( '  gvcord = ' // char(0), gvcord )
	    call gfprints ( '  gfunc = ' // char(0), gfunc )
	    call gfprints ( '  gpoint = ' // char(0), gpoint )
	    call gfprints ( '  gvect = ' // char(0), gvect )
	    call gfprints ( '  ptype = ' // char(0), ptype )
	    call gfprints ( '  xaxis = ' // char(0), xaxis )
	    call gfprints ( '  yaxis = ' // char(0), yaxis )
	    call gfprints ( '  line = ' // char(0), line )
	    call gfprints ( '  wind = ' // char(0), wind )
	    call gfprints ( '  refvec = ' // char(0), refvec )
	    call gfprints ( '  winpos = ' // char(0), winpos )
	    call gfprints ( '  thtaln = ' // char(0), thtaln )
	    call gfprints ( '  thteln = ' // char(0), thteln )
	    call gfprints ( '  mixrln = ' // char(0), mixrln )
	    call gfprints ( '  filter = ' // char(0), filter )
	    call gfprints ( '  title = ' // char(0), title )
	    call gfprints ( '  scale = ' // char(0), scale )
	    call gfprints ( '  text = ' // char(0), text )
	    call gfprinti ( '  frame = ' // char(0), frame )
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
     +                ibrdr, irrotn, ijust, iret)
        CALL dstext ( igtxfn, igtxhw, gsztext, igtxwid,
     +                ibrdr, irrotn, ijust,
     +                idtxfn, idtxhw, dsztext, idtxwid, 
     +                dbrdr, drrotn, djust, iret)
        CALL gqcolr ( igcolr, iret )
        CALL dscolr ( igcolr, ioldcolr, iret )

C
C*	Set the first frame flag
C
	if ( frame .eq. 1 ) then
	    first = .true.
	else
	    first = .false.
	endif

	proces = .true.
C
C*      Process the GDFILE input.
C
        CALL DG_NFIL ( gdfile, ' ', ier )
        IF ( ier .ne. 0 ) THEN
           CALL ER_WMSG ( 'DG', ier, ' ', irr )
           proces = .false.
        ELSE
C
C*      Process the GDATTIM input; setup the time server.
C
            CALL DG_NDTM ( gdatim, ier )
            IF ( ier .ne. 0 ) THEN
                CALL ER_WMSG ( 'DG', ier, gdatim, irr )
                proces = .false.
            END IF
	END IF
C
C	Set text.
C
	IF ( proces ) THEN
	    CALL IN_TEXT ( text, iret )
	    CALL IN_FILT  ( filter, filtfc, ier )
C
C*	    Set flags for scalars and winds.
C
	    havscl = .true.
	    havwnd = .true.
	    IF ( gfunc .eq. ' ' ) havscl = .false.
	    IF ( gvect .eq. ' ' ) havwnd = .false.
C
C*	    Get information about the background lines.
C
	    CALL GDPLIN ( thtaln, thteln, mixrln,
     +                    ithtal, ithtel, imixrl, iret )
	END IF

C
C*      Loop over times.
C
        itime = 1
        gottm = proces
        first = .true.
        DO  WHILE ( gottm )
C
C*          Get the next time to process from the time server.
C
            CALL DG_NTIM ( .true., .true., time, gottm, ier )
            proces = ( ier .eq. 0 .and. gottm )
            IF ( ier .ne. 0 ) THEN
                ier = 2
                CALL ER_WMSG ( 'GDPROF', ier, time(1), irr)
            END IF
            CALL TG_DUAL ( time, timfnd, ier )
            IF ( proces .and. first ) THEN
                CALL DG_FLNO  ( gfunc, iflnos, ier )
                CALL DG_QDTM ( iflnos, firstm, lastim, ier )
                IF ( firstm .eq. ' ' ) THEN
                    proces = .false.
                END IF
            END IF
C
C*	    Get time and vertical coordinate to use for 1st plot.
C
	    IF  ( proces )  THEN
		CALL GDPDTV  ( timfnd, gvcord, gfunc, firstm, lastim, 
     +			       time, ivcord, iret )
		IF  ( iret .ne. 0 )  THEN
		    CALL ER_WMSG  ( 'GDPROF', iret, ' ', ier )
		    proces = .false.
		END IF
	    END IF
C
C*	    Get information about y axis.
C
	    IF  ( proces .and. first )  THEN
		CALL GDPYAX  ( ptype, yaxis, ivcord, iyaxis, ratio, 
     +			       ystrt, ystop, ylbl, nylbl, rmargn,
     +			       iylbsf, iyglsf, iytmsf, iret )
		IF  ( iret .ne. 0 )  THEN
		    CALL ER_WMSG  ( 'GDPROF', iret, ' ', ier )
		    proces = .false.
		END IF
	    END IF
C
C*	    Get data to plot.
C
	    IF  ( proces )  THEN
	        IF ( havscl ) THEN
		    CALL GDPDTA ( iflnos, timfnd, gvcord, gfunc,
     +			      gpoint, time, ivcord, ystrt, ystop,
     +                        rgx, rgy, rlat, rlon, npts, x, y,
     +                        parm, iret )
	
		    IF  ( iret .ne. 0 )  havscl = .false.
	            IF  ( iret .eq. -12 .or. iret .eq. -13 ) THEN
		        proces = .false.
	            END IF

	        ELSE
C
C*	There is a bug in GDPPLT in that it does not test for the
C*	presence of data. So zero out components as a workaround.
C
	    	    parm = ' '
		    do i=1,npts
			x(i) = 0.
			y(i) = 0.
		    end do
	
	    	END IF
C
	        IF ( havwnd ) THEN
		    CALL GDPDUV ( iflnos, timfnd, gvcord, gvect,
     +			      gpoint, time, ivcord, ystrt, ystop,
     +                        rgx, rgy, rlat,  rlon, nuv, u, v, yv,
     +                        parmu, parmv, iret )
		    IF  ( iret .ne. 0 )  havwnd = .false.
	        END IF
	        IF ( .not. havscl .and. .not. havwnd ) proces = .false.
	    END IF
C
C*	    Get information about x axis.
C
	    IF  ( proces .and. first )  THEN
C
C*		Set the x axis and scale the data on the first time.
C
		CALL GDPXAX  ( havscl, xaxis, scale, iyaxis, npts, 
     +			       x, iscale, xstrt, xstop, xlbl, 
     +			       nxlbl, rmindt, rmaxdt, ixlbsf, 
     +			       ixglsf, ixtmsf, iret )
		IF  ( iret .ne. 0 )  THEN
		    CALL ER_WMSG  ( 'GDPROF', iret, ' ', ier )
		    proces = .false.
		END IF
	    ELSE IF  ( proces .and. ( .not. first ) )  THEN
C
C*              Only scale the data on subsequent frames.
C
                CALL GR_SSCL  ( iscale, 1, npts, 1, 1, 1, npts, x,
     +                          rmindt, rmaxdt, iret )
	    END IF
C
C*	    Set plotting mode to graph mode.
C
	    CALL GQMODE  ( mode, ier )
	    CALL GSMODE  ( 2, ier )
C
C*	    Draw the profiles.
C
	    IF  ( proces ) THEN
		CALL GG_PANL  ( panel, ier )
C
C*		Plot axes and data.
C
		CALL GDPPLT  ( parm, border, line, marker,
     +                         xaxis, yaxis, iyaxis, ystrt, 
     +			       ystop, ylbl, nylbl, xstrt, xstop, xlbl, 
     +			       nxlbl, npts, x, y, ratio, rmargn,
     +			       ixlbsf, ixglsf, ixtmsf, iylbsf, iyglsf,
     +                         iytmsf, wbszx, wbszy, iret )
C
C*		If a vector profile exists, plot it.
C
	        IF ( havwnd .and. iret .eq. 0 ) THEN
		  CALL GDPUVP ( gvect, u, v, yv, nuv, wind, winpos,
     +                          filtfc, wbszx, wbszy, refvec, iret )
		END IF
C
C*	        Draw background lines.
C
	        IF ( iret .eq. 0 )
     +	          CALL GDPPLN ( ithtal, ithtel, imixrl, gfunc, ivcord,
     +                        ystrt, ystop, xstrt, xstop, ier )
	    END IF
C
C*	    Write title even if proces is false
C
	    CALL IN_TITL  ( title, 0, ititl, linttl, ttlstr, ier )
	    IF  ( ititl .gt. 0 )  THEN
		CALL GSCOLR   ( ititl, ier )
                CALL DSCOLR  ( ititl, ioldclr, iret )
		lens = LEN ( ttlstr )
		ttlstr(lens:lens) = char(0)
		call ptitle ( ttlstr, ititle )
	    END IF
C
C*          Reset the plotting mode and flush buffers.
C
            CALL GSMODE  ( mode, ier )
	END DO
C
C	Close file.
C
	CALL DG_NEND ( iret )
C	CALL GFLUSH   ( iret )
C*
csd	if ( verbose .gt. 1 ) print*,'returning from pvprof'
	if ( verbose .gt. 0 ) call gfprinti (
     +     'returning from pvprof - iret = ' // char(0), iret )

	return
	END
                             
