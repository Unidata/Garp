C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	psgrid.f
C*
C*	Grid contouring routine. Derived from the GEMPAK program GDCNTR.
C*
C*	History:
C*
C*	11/96	COMET		Original copy
C*	 2/97	J. Cowie/COMET	Changed title time
C*	 3/97	J. Cowie/COMET	Changed to deal with unspecified contour
C*				or fill interval
C*	 5/97	COMET		Added gprintf to support logging.
C*	11/97	COMET		Added ptitle to display clickable titles.
C*	11/97	COMET		Added call to dscolor, cleaned up.
C*	12/97	COMET		Changed variable "len" to "lent".
C*	 2/98	COMET		Increased title length
C*	 3/07	S. Chiswell/Unidata	new in_intc argument clbl, flbl,
C*					update call to gclgrn
C*
C************************************************************************

	SUBROUTINE psgrid ( gdfile, gdatim, glevel, gvcord, ggfunc, 
     +			    gridtype, cint, line, scale, hilo, hlsym,
     +                      clrbar, title, contur, skip, fint, fline,
     +                      ctype, text, nfunc, frame, ititle, verbose,
     +                      iperr )
	
C************************************************************************
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
C*
	CHARACTER	gdfile*(*), gdatim*(*), glevel*(*)  ,
     +                  gvcord*(*), ggfunc*(*), gridtype*(*),
     +                  cint*(*)  , line*(*)  , scale*(*)   , hilo*(*), 
     +			hlsym*(*) , clrbar*(*), title*(*)   , 
     +			contur*(*), skip*(*)  , fint*(*)    ,
     +			fline*(*) , ctype*(*) , text*(*)
	INTEGER		verbose, frame

	CHARACTER	shrttl*72, carr(3)*36, timfnd*36,
     +			garut*72, prjut*72, mproj*72, strout*72,
     +			gstring*(LLMXLN), pstring*(LLMXLN)
C*
	CHARACTER	time (2)*20, ttlstr*132, ttl*132,
     +			parm*12, parmu*12, parmv*12,
     +                  pfunc*72, gfunc*72, gtype*2, blank*2
	LOGICAL		proces, nxttim
C
	REAL		grid (LLMXGD), grid1 (LLMXGD), grid2 (LLMXGD),
     +                  subgrd (LLMXGD), clvl (LLCLEV), rarr(4),
     +			flvl (LLCLEV), farr(4)
	INTEGER		level (2), icolor (LLCLEV), lintyp (LLCLEV),
     +			linwid (LLCLEV), linlbl (LLCLEV),
     +			ifcolr (LLCLEV), iflabl (LLCLEV),
     +			ifltyp (LLCLEV), iflwid (LLCLEV),
     +			iswind (2)
	LOGICAL		misflg, cflag, lflag, bflag, fflag, sflag, nflag

	LOGICAL		newlev, scflag
	CHARACTER	panel*81, pfcint*72, pffint*72, clbl(LLCLEV)*24,
     +			flbl(LLCLEV)*24
	SAVE		pffint, pfcint
C
C----------------------------------------------------------------------
C
	panel = '0'
	iperr = 0
	ioldclr = 0
	blank = ' ' // char(0)

	if ( verbose .gt. 0 )
     +         call gfprints ( 'psgrid'//char(0), blank )
	if ( verbose .gt. 1 ) then
	    call gfprints ( '  gdfile = ' // char(0), gdfile )
	    call gfprints ( '  gdatim = ' // char(0), gdatim )
	    call gfprints ( '  glevel = ' // char(0), glevel )
	    call gfprints ( '  gvcord = ' // char(0), gvcord )
	    call gfprints ( '  gfunc = ' // char(0), ggfunc )
	    call gfprinti ( '  nfunc = ' // char(0), nfunc )
	    call gfprints ( '  cint = ' // char(0), cint )
	    call gfprints ( '  line = ' // char(0), line )
	    call gfprints ( '  scale = ' // char(0), scale )
	    call gfprints ( '  hilo = ' // char(0), hilo )
	    call gfprints ( '  hlsym = ' // char(0), hlsym )
	    call gfprints ( '  clrbar = ' // char(0), clrbar )
	    call gfprints ( '  title = ' // char(0), title )
	    call gfprints ( '  contur = ' // char(0), contur )
	    call gfprints ( '  skip = ' // char(0), skip )
	    call gfprints ( '  fint = ' // char(0), fint )
	    call gfprints ( '  fline = ' // char(0), fline )
	    call gfprints ( '  ctype = ' // char(0), ctype )
	    call gfprints ( '  text = ' // char(0), text )
	    call gfprinti ( '  frame = ' // char(0), frame )
	    call gfprinti ( '  ititle = ' // char(0), ititle )
	    call gfprinti ( '  verbose = ' // char(0), verbose )
	    call gfprinti ( '  iperr = ' // char(0), iperr )
	end if

C
C*	Clear out the contour/fill info from any previous frames.
C
	if ( frame .eq. 1 ) then
	    if ( INDEX ( ctype, 'C') .gt. 0 ) pfcint(1:) = ' '
	    if ( INDEX ( ctype, 'F') .gt. 0 ) pffint(1:) = ' '
	endif
C
C	Set text.
C
	CALL IN_TEXT ( text, ier )

C
C*	Set flag to indicate processing will be done.
C
	proces = .true.
C
C*      Process the GDFILE input.
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
C
	IF  ( ier .eq. 0 )  THEN
		   
            CALL TG_DUAL ( time, timfnd, ier )

	    CALL GQMPRJ  ( mproj, rarr(1), rarr(2), rarr(3), 
     +		       farr(1), farr(2), farr(3), farr(4), ier )
C
            CALL ST_LSTF ( rarr, 3, ';', 1, strout, ier )
	    CALL ST_LSTR ( mproj, ilen, ier )	
            pstring = mproj ( 1 : ilen ) // '/' // strout
C
	    CALL ST_LSTF ( farr, 4, ';', 1, gstring, ier )
C
	    CALL DG_FIXA ( gstring, pstring, garut, prjut, ier )
	    CALL DG_SUBG ( 'N', iminx, iminy, imaxx, imaxy, iret )
	    IF  ( iret .ne. 0 )  THEN
                CALL ER_WMSG ( 'DG', iret, ' ', ier )
                proces = .false.
            END IF
        ELSE
	    proces = .false.
	END IF
C
	IF  ( proces )  THEN
C
C*		Set contour attributes; get contour skip factor.
C
		CALL IN_CONT  ( contur, ier )
		CALL IN_SKIP  ( skip, iskpxy, iswind, ier )
C
C*		Get contouring type.
C
		CALL IN_CTYP  ( ctype, nflag, lflag, sflag, bflag, 
     +				fflag, ier )
		IF  ( lflag .or. sflag .or. bflag .or. nflag)  THEN
		    cflag = .true.
		  ELSE
		    cflag = .false.
		END IF
	END IF
C
	IF ( proces ) THEN
C
C*	    Compute the requested grid.  If the requested grid
C*	    cannot be computed, continue.
C
	    do i=1, nfunc

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
		    CALL DG_GRID ( timfnd, glevel, gvcord, gfunc, 
     +			           pfunc, grid, kx, ky, time, level, 
     +			           ivcord, parm, iret )
		else if ( gtype .eq. 'v' ) then
                    CALL DG_VECT ( timfnd, glevel, gvcord, gfunc,
     +                             pfunc, grid1, grid2, kx, ky, time,
     +                             level, ivcord, parmu, parmv, iret )
	        endif

	        IF  ( iret .ne. 0 )  THEN
		    iperr = -1
		    proces = .false.
		    CALL ER_WMSG  ( 'DG', iret, pfunc, ier )
	        END IF
	    enddo

	END IF
C
	IF  ( proces )  THEN
C
C*	    Define scaling.
C
     	    CALL IN_SCAL ( scale, iscale, iscalv, iret)
	    CALL GR_SSCL ( iscale, kx, ky, iminx, iminy,
     +				 imaxx, imaxy, grid, dmin, dmax, iret )
C
C*	    Define contour levels and characteristics.
C
	    IF ( cflag ) THEN
C
C*		Use the preceeding frame's cint if not defined
C
		CALL ST_CLST ( cint, '/', ' ', 3, carr, num, iret)
		IF ( (carr(1) .eq. ' ' ) .and.
     +			   (pfcint  .ne. ' '    )) cint = pfcint

		CALL IN_INTC ( cint, dmin, dmax, clvl, nclvl, clbl,
     +				     rint, cmin, cmax, iret )
		IF  ( iret .ne. 0 )  THEN
		    nclvl = 0
		    rint  = 0.
		END IF
C
C*		If undefined, save the new cint for the next frame
C
		IF ( (carr(1) .eq. ' ' ) .and. 
     +			    (pfcint  .eq. ' '    ) ) THEN
     		    write( pfcint,'(F10.2,A,F10.2,A,F10.2)')
     +				rint, '/',cmin,'/',cmax
		    call ST_RMBL (pfcint, pfcint, lent, ier)
		    cint = pfcint
		END IF

		CALL IN_LINE ( line, clvl, nclvl, icolor, 
     +				     lintyp, linwid, linlbl, 
     +                               smooth, filter, scflag, iret )
C
C*		Check for duplicate contours & sort contours
C
		CALL GR_NLEV ( nclvl, clvl, clbl, icolor, lintyp,
     +				     linwid, linlbl, iret ) 
C
	    END IF
C
C*	    Process color fill contours
C
	    IF ( fflag ) THEN
C
C*		Use the preceeding frame's fint if not defined. If
C*		a list has been specified, just use that.
C
		iflist = INDEX ( fint, ';' )
		CALL ST_CLST ( fint, '/', ' ', 3, carr, num, iret )
		CALL ST_CRNM ( carr(1), rarr(1), ier )
		CALL ST_CRNM ( carr(2), rarr(2), ier )
		CALL ST_CRNM ( carr(3), rarr(3), ier )
		IF (((rarr(1) .eq. RMISSD ) .or.
     +			   (rarr(2) .eq. RMISSD ) .or.
     +			   (rarr(3) .eq. RMISSD )) .and.
     +			   (pffint  .ne. ' '    )  .and.
     +			   (iflist  .eq. 0 )) fint = pffint
C
C*		Define color fill contours. If the min or max is
C*		already specified, use it.
C
		CALL ST_CLST ( fint, '/', ' ', 3, carr, num, iret )
		CALL ST_CRNM ( carr(2), rarr(2), ier )
		CALL ST_CRNM ( carr(3), rarr(3), ier )
		if ( rarr(2) .ne. RMISSD ) dmin = rarr(2)
		if ( rarr(3) .ne. RMISSD ) dmax = rarr(3)

		CALL IN_INTC ( fint, dmin, dmax, flvl, nflvl, flbl,
     +				   rfint, fmin, fmax, iret )

		IF ( iret .ne. 0 ) THEN
		            nflvl = 0
			    rfint = 0.
		END IF
C
C*		If undefined, save the new fint for the next frame
C
		IF (((rarr(1) .eq. RMISSD )  .or.
     +			     (rarr(2) .eq. RMISSD )  .or.
     +			     (rarr(3) .eq. RMISSD )) .and. 
     +			     (pffint  .eq. ' '    )  .and.
     +			     (iflist  .eq. 0 )) THEN
     		    write( pffint,'(F10.2,A,F10.2,A,F10.2)')
     +				rfint, '/',flvl(1),'/',flvl(nflvl)
		    call ST_RMBL (pffint, pffint, lent, ier)
		    fint = pffint
		END IF
C
C*		Get the colors, line types, line widths and labels
C
		IF  ( nflvl .eq. LLCLEV )  THEN
		            nflvl = nflvl - 1
		END IF
		nflvl1 = nflvl + 1
		CALL IN_LINE ( fline, flvl, nflvl1, ifcolr, 
     +				   ifltyp, iflwid, iflabl, 
     +                             smooth, filter, scflag, iret )
C
C*		Check for duplicate fill contours & sort.
C
		CALL GR_NLEV ( nflvl, flvl, flbl, ifcolr, ifltyp,
     +				   iflwid, iflabl, iret ) 
	    END IF
	END IF
C
C*	Draw contours.
C
	IF  ( proces )  THEN
C*	    Draw contours.
C
	    IF  ( cflag .or. fflag )  THEN
		misflg = .true.
		CALL GR_SUBX ( kx, ky, grid, iminx, iminy, 
     +				imaxx, imaxy, iskpxy, misflg, kxsub, 
     +				kysub, subgrd, ioffx, ioffy, iskip, 
     +				ier )
		IF  ( fflag )  THEN
		    CALL GCFILL  ( kxsub, kysub, subgrd, ioffx, 
     +				     ioffy, iskip, nflvl, flvl, 
     +				     ifcolr, iflabl, ifltyp, iret )
		    IF  ( iret .ne. 0 )  
     +			CALL ER_WMSG ( 'GEMPLT', iret, ' ', ier )
		END IF
		IF  ( cflag )  THEN
		    IF  ( lflag )  THEN

			IF  ( smooth .ne. 0.0 ) THEN
			    CALL GSSMTH ( 2, smooth, ier )
			END IF
C
			CALL GSRDUC ( filter, ier )
C
		        CALL GCLGRN (kxsub, kysub, subgrd, ioffx, 
     +				ioffy, iskip, nclvl, clvl, clbl,
     +				icolor, lintyp, linwid, linlbl, 
     +				scflag, iret )
		        IF (iret .ne. 0) CALL ER_WMSG ( 'GEMPLT', 
     +					iret, ' ', ier )
			IF  ( smooth .ne. 0.0 )  THEN
			    CALL GSSMTH ( 0, 0.0, ier )
			END IF
			CALL GSRDUC ( 0.0, ier )
		    END IF
C
		    IF  ( bflag )  THEN
		        CALL GCBOXX (kxsub, kysub, subgrd, ioffx, 
     +				ioffy, iskip, nclvl, clvl, icolor, 
     +				lintyp, linwid, linlbl, iret )
		        IF (iret .ne. 0) CALL ER_WMSG ( 'GEMPLT', 
     +					iret, ' ', ier )
		    END IF
		END IF
	    END IF
C
C*	    Mark highs and lows.
C
	    CALL GDNHLO ( hilo, hlsym, grid, kx, ky, ier )
C
C*	    Plot the color bar.
C
	    IF  ( fflag )  CALL GG_CBAR ( clrbar, nflvl, flvl, 
     +					ifcolr, ier )
C
C*	    Write title.
C
	    CALL IN_TITL ( title, 0, ititl, linttl, ttlstr, ier )
	    IF ( ititl .ne. 0 ) THEN
		CALL GSCOLR   ( ititl, ier )
                CALL DSCOLR  ( ititl, ioldclr, iret)
		lens = LEN ( ttlstr )
		ttlstr(lens:lens) = char(0)
		call ptitle ( ttlstr, ititle )
	    END IF
C
C       Not processing but at least plot a title.
C
	ELSE
            CALL IN_TITL ( title, 0, ititl, linttl,
     +                           ttlstr, ier )
            IF  ( ititl .gt. 0 )  THEN
                CALL GSCOLR  ( ititl, ier )
                CALL DSCOLR  ( ititl, ioldclr, iret)
		lens = LEN ( ttlstr )
		ttlstr(lens:lens) = char(0)
                call ptitle ( ttlstr, ititle )
            END IF
	END IF

C
C 	End the plot to flush everything out
C
        CALL DG_NEND ( ier )
C
C	CALL GFLUSH ( iret )
C
	if ( verbose .gt. 0 ) call gfprinti ( 
     +  'returning from psgrid - iret = '//char(0), iret )
	RETURN
	END
