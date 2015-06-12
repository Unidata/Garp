C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	pstream.f
C*
C*	Streamline drawing function. Derived from the GEMPAK program
C*	GDSTREAM.
C*
C*	History:
C*
C*	11/96	COMET		Original copy
C*	02/97	J. Cowie/COMET	Cleaned up, changed title string
C*	 5/97	COMET		Added gprintf to support logging.
C*	11/97	COMET		Added ptitle to display clickable titles.
C*	11/97	COMET		Added call to dscolor, cleaned up.
C*
C************************************************************************


	SUBROUTINE pstream ( gdfile, gdatim, glevel, gvcord, ggvect,
     +			    line, wind, stream, skip, title, text,
     +                      nfunc, ititle, verbose, iperr )
C************************************************************************
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
C*
        CHARACTER       gdfile*(*), gdatim*(*), glevel*(*)
        CHARACTER       gvcord*(*), ggvect*(*), line*(*)
        CHARACTER       wind*(*)  , stream*(*), skip*(*)
	CHARACTER	title*(*) , text*(*)
	INTEGER		verbose

        CHARACTER       shrttl*72, ttl*72, blank*2
C*
	REAL		grid1 (LLMXGD), grid2 (LLMXGD), values(2),
     +			rarr(3), farr(4)
	INTEGER		level (2), linwid(2), iline(2), ilabel(2)
	CHARACTER	time(2)*20, ttlstr*72, wintyp*1,
     +			winuni*1, pfunc*72, gvect*72, panel*70,
     +			parmu*12, parmv*12, timfnd*36,
     +			gstring*(LLMXLN), pstring*(LLMXLN), garut*72, 
     +			prjut*72, mproj*72, strout*72
	LOGICAL		done, proces, nxttim

	LOGICAL         first, scflag
C	
C-----------------------------------------------------------------------
C
        panel = '0'
        stream = ' '
	blank = ' ' // char(0)
	ioldclr = 0
        iperr = 0

	if ( verbose .gt. 0 ) call gfprints (
     +   'pstream '//char(0), blank )
	if ( verbose .gt. 1 ) then
	    call gfprints ( '  gdfile = ' // char(0), gdfile )
	    call gfprints ( '  gdatim = ' // char(0), gdatim )
	    call gfprints ( '  glevel = ' // char(0), glevel )
	    call gfprints ( '  gvcord = ' // char(0), gvcord )
	    call gfprints ( '  gvect = ' // char(0), ggvect )
	    call gfprints ( '  line = ' // char(0), line )
	    call gfprints ( '  wind = ' // char(0), wind )
	    call gfprints ( '  stream = ' // char(0), stream )
	    call gfprints ( '  skip = ' // char(0), skip )
	    call gfprints ( '  title = ' // char(0), title )
	    call gfprints ( '  text = ' // char(0), text )
	    call gfprinti ( '  nfunc = ' // char(0), nfunc )
	    call gfprinti ( '  ititle = ' // char(0), ititle )
	    call gfprinti ( '  verbose = ' // char(0), verbose )
	    call gfprinti ( '  iperr = ' // char(0), iperr )
	end if
C
C*	Set text.
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

        IF  ( ier .eq. 0 )  THEN

            CALL TG_DUAL ( time, timfnd, ier )

            CALL GQMPRJ  ( mproj, rarr(1), rarr(2), rarr(3),
     +                   farr(1), farr(2), farr(3), farr(4), ier )
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

	first = .true.
C
C*	Find the wind only if the grid file was successfully opened.
C
	IF  ( proces )  THEN
C
C*	    Compute the requested vector and rotate to grid-relative
C*	    orientation.
C
	    do i=1, nfunc

		llen = 72
		ibeg = llen * ( i - 1 ) + 1
		iend = ibeg + llen - 1
		gvect = ggvect(ibeg:iend)
		pfunc = gvect
		CALL DG_VECR  ( timfnd, glevel, gvcord, gvect, 
     +				pfunc, grid1, grid2, kx, ky, time, 
     +				level, ivcord, parmu, parmv, iret )
	    enddo

	    IF  ( iret .eq. 0 )  THEN
                CALL IN_LINE  ( line, values, 1, icolor, iline, 
     +                        linwid, ilabel, smooth, filter, 
     +			      scflag, ier )
	    ELSE
		proces = .false.
		CALL ER_WMSG  ( 'DG', iret, pfunc, ier )
	    END IF
	END IF

C
C*	Draw streamlines.
C
	IF  ( proces )  THEN
C
C*	    Set arrow head size.
C
	    CALL IN_WIND ( wind, wintyp, winuni, iicolr, ier )
C
	    CALL GQARRW ( szarrw, szarrh, iarwid, iartyp, ier )
	    CALL GQLINE ( lintyp, ilhw, iwidth, ilwhw, ier )
C
C*          Set color, line type and width for streamlines and 
C*          arrows
C
	    IF ( iartyp .eq. 2 )  THEN
		jartyp = 1
	    ELSE
		jartyp = iartyp
	    END IF
C*
	    CALL GSCOLR  ( icolor, ier )
	    CALL GSLINE  ( iline(1), 0, linwid(1), 0, ier )
            CALL GSARRW  ( szarrw, szarrh, linwid(1), jartyp, ier )
C
C*	    Process STREAM parameter.
C
	    CALL GDSTIN ( stream, filtst, filtar, ststop,
     +                          dispc, displ, ier )
C
C*	    Draw streamlines.
C
	    if ( verbose .gt. 1 ) then
		    call gfprinti ( '  kx = ' // char(0), kx )
		    call gfprinti ( '  ky = ' // char(0), ky )
		    call gfprinti ( '  ix1 = ' // char(0), ix1 )
		    call gfprinti ( '  iy1 = ' // char(0), iy1 )
		    call gfprinti ( '  ix2 = ' // char(0), ix2 )
		    call gfprinti ( '  iy2 = ' // char(0), iy2 )
	    endif

            CALL GSTRML  ( kx, ky, grid1, grid2, ix1, iy1, ix2, iy2,
     +                          .true., filtst, filtar, ststop,
     +                          dispc, displ, ier )

C
C*          Reset arrow and line attributes to previous
C
	    CALL GSARRW ( szarrw, szarrh, iarwid, iartyp, iret )
	    CALL GSLINE ( lintyp, 0, iwidth, 0, ier )
C
C*	    Write title.
C
	    CALL IN_TITL  ( title, 0, ititl, linttl, ttlstr, ier )
	    IF  ( ititl .ne. 0 )  THEN
		CALL GSCOLR  ( ititl, ier )
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
                call ptitle ( ttlstr, ititle )
            END IF
	END IF
C
C	End the plot to flush everything out
C
	CALL DG_NEND ( iret )
C	CALL GFLUSH ( iret )
C*
	if ( verbose .gt. 0 ) call gfprinti (
     +  'returning from pstream - iret = ' // char(0), iret )

	RETURN
	END
                                      
