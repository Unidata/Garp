C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	pvgrid.f
C*
C*	Vector data plotter. Derived from the GEMPAK program GDWIND.
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*	 4/97	COMET	Added "scale" as an input parameter.
C*	 5/97	COMET	Added gprintf to support logging.
C*	11/97	COMET	Added ptitle to display clickable titles.
C*	11/97	COMET	Added call to dscolor, cleaned up.
C*       8/03   S. Chiswell/Unidata	Updated to remove use of subflg 
C*					for GEMPAK calls (8/03)
C*	 8/05	S. Chiswell/Unidata	Modified for new gempak grid library
C*
C************************************************************************


	SUBROUTINE pvgrid ( gdfile, gdatim, glevel, gvcord, ggvect,
     +                      gridtype, wind, refvec, scale, skip, title,
     +                      text, nfunc, ititle, verbose, iperr )

C************************************************************************

	INCLUDE		'GEMPRM.PRM'
C*
	CHARACTER	gdfile*(*), gdatim*(*),   glevel*(*),
     +			gvcord*(*), ggvect*(*), gridtype*(*), wind*(*),
     +            	refvec*(*), scale*(*) ,     skip*(*), title*(*)
	CHARACTER	text*(*)
	INTEGER		verbose

	CHARACTER	shrttl*72, timfnd*36, gstring*(LLMXLN),
     +			pstring*(LLMXLN), garut*72, prjut*72, mproj*72, 
     +			strout*72
C*
	REAL		grid (LLMXGD), grid1 (LLMXGD), grid2 (LLMXGD)
	REAL		sped  (LLMXGD), drct  (LLMXGD)
	REAL		fi (100), fj (100), s (100), d (100),
     +			rarr(3), farr(4)
  
	INTEGER		level (2)
	CHARACTER	parm*12, parmu*12, parmv*12,
     +                  pfunc*72, gvect*72, gtype*12
	CHARACTER	gv*72, time(2)*20, winuni*1, wintyp*1
	CHARACTER	ttlstr*72, defstr*12, ttl*72, blank*2
	LOGICAL		proces, nxttim
	LOGICAL         first, novect
	INTEGER		iskplt (2)

	character	panel*72
C-----------------------------------------------------------------------
C
	panel = '0'
	iperr = 0
	ioldclr = 0
	blank = ' ' // char(0)
	first = .TRUE.

	if ( verbose .gt. 0 ) call gfprints ( 
     +     'pvgrid'//char(0), blank )
	if ( verbose .gt. 1 ) then
	    call gfprints ( '  gdfile = ' // char(0), gdfile )
	    call gfprints ( '  gdatim = ' // char(0), gdatim )
	    call gfprints ( '  glevel = ' // char(0), glevel )
	    call gfprints ( '  gvcord = ' // char(0), gvcord )
	    call gfprints ( '  gvect = ' // char(0), ggvect )
	    call gfprints ( '  wind = ' // char(0), wind )
	    call gfprints ( '  refvec = ' // char(0), refvec )
	    call gfprints ( '  scale = ' // char(0), scale )
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
C
	IF  ( proces )  THEN
C
C*	    Process the parameters that do not change within the
C*	    time loop.
C
	    CALL IN_WIND  ( wind, wintyp, winuni, icolor, ier )
C
C*	    Check for points to skip.
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
C*	Compute the requested vector.
C
	IF  ( proces )  THEN
C
	    do i=1, nfunc

	        llen = 72
		ibeg = llen * ( i - 1 ) + 1
		iend = ibeg + llen - 1

		igl = 2
		igb = igl * ( i - 1 )  + 1
		ige = igb + 1

		gvect = ggvect(ibeg:iend)
		pfunc = gvect
		gtype = gridtype(ige:ige)
		    
	        if      ( gtype .eq. 's' ) then
                    CALL DG_GRID ( timfnd, glevel, gvcord,
     +                             gvect, pfunc, grid, kx, ky,
     +                             time, level, ivcord, parm, iret )
	        else if ( gtype .eq. 'v' ) then
		    CALL DG_VECT ( timfnd, glevel, gvcord,
     +				   gvect, pfunc, grid1, grid2, kx, ky,
     +				   time, level, ivcord, parmu, parmv, 
     +				   iret )
	        endif
	    enddo

C
C*	    Check for error message.
C
	    IF  ( iret .ne. 0 )  THEN
		CALL ER_WMSG  ( 'DG', iret, pfunc, ier )
		novect = .true.
		proces = .false.
	    ELSE
		novect = .false.
C
C*		Set the use flag to calculate the magnitude,
C*			now that we know kx, ky.
C
		npt = kx *  ky
C
C
C*		Calculate the wind speed and direction.
C
		CALL PD_SPED  ( grid1, grid2, npt,
     +					sped, iret )
		CALL PD_DRCT  ( grid1, grid2, npt,
     +					drct, iret )
		CALL ST_LCUC  ( gvect, gv, ier )
C
C*		Convert sped to knots, if necessary.
C
		iposk = INDEX ( gv, 'KNTV' )
		IF  ( winuni .eq. 'K' .and. iposk .eq. 0 )
     +			    CALL PD_MSKN ( sped, npt, sped, iret )
	    END IF
	END IF
C
	IF  ( proces )  THEN
C
C*	    Scale the data.
C
	    IF  ( first )  THEN
		CALL IN_SCAL ( scale, iscale, iscalv, iret )
		IF  ( wintyp .ne. 'B' )  THEN
		    CALL GR_SSCL  ( iscalv, kx, ky, ix1, iy1,
     +			ix2, iy2, sped, dmin, dmax, ier )
		ELSE 
C
C*	    	    Don't scale wind barbs
C
		    iscalv = 0
		    CALL GR_STAT ( sped, kx, ky, ix1, iy1,
     +				ix2, iy2, dmin, dmax, davg, ddev, ier )
		END IF
	    ELSE IF  ( iscalv .ne. 0 )  THEN
		CALL GR_SSCL  ( iscalv, kx, ky, ix1, iy1,
     +				ix2, iy2, sped, dmin, dmax, ier )
	    END IF
	END IF
C
C*	Draw wind symbols.
C
	IF  ( proces ) THEN
C
C*	    Draw winds.
C
	    IF  ( ( icolor .ne. 0 ) .and. ( .not. novect ) ) THEN
	        CALL GSCOLR ( icolor, ier )
		npts   = 0
		ixstrt = ix1
		DO  j = iy1, iy2, iystep
		    iy = ( j - 1 ) * kx
C*
		    DO  i = ixstrt, ix2, ixstep
			ixy = iy + i
csd need ermiss include file
csd			IF  ( ( .not. ERMISS ( sped (ixy) ) ) .and.
csd     +				  ( .not. ERMISS ( drct (ixy) ) ) )  THEN
			    npts = npts + 1
			    fi ( npts ) = FLOAT (i)
			    fj ( npts ) = FLOAT (j)
			    s ( npts )  = sped ( ixy )
			    d ( npts )  = drct ( ixy )
csd			END IF
			IF  ( ( npts .ge. 100 ) .or.
     +				  ( ( i + ixstep .gt. ix2 ) .and.
     +				    ( j + iystep .gt. iy2 ) ) )  THEN
			    IF  ( wintyp .eq. 'B' ) THEN
				CALL GBARB ( 'G', npts, fi, fj,
     +						 s, d, ier )
			    ELSE
				CALL GARRW ( 'G', npts, fi, fj,
     +						 s, d, ier )
			    END IF
			    npts = 0
			END IF
		    END DO
		    IF  ( ixstrt .eq. ix1 )  THEN
		         ixstrt = ixstrt + istag
		    ELSE
			 ixstrt = ix1
		    END IF
		END DO					
C
C*		Plot reference arrow if arrows were requested.
C*		Parse the parameter REFVEC and draw the arrow.
C
		IF  ( ( wintyp .eq. 'A' ) .and.
     +			      ( winuni .ne. 'N' ) )  THEN
		    IF  ( winuni .eq. 'K' )  defstr = 'kts'
		    IF  ( winuni .eq. 'M' )  defstr = 'm/s'
		    CALL GG_RVEC  ( refvec, defstr, ier )
		END IF
C
C*		Write title.
C
		CALL IN_TITL  ( title, 0, ititl, linttl,
     +					ttlstr, ier )
		IF  ( ititl .ne. 0 )  THEN
		    CALL GSCOLR  ( ititl, ier )
                    CALL DSCOLR  ( ititl, ioldclr, iret)
		    lens = LEN ( ttlstr )
		    ttlstr(lens:lens) = char(0)
		    call ptitle ( ttlstr, ititle )
	        END IF
	    END IF
C
C       Not processing but at least plot a title.
C
        ELSE
            CALL IN_TITL ( title, 0, ititl, linttl,
     +                              ttlstr, ier )
            IF  ( ititl .gt. 0 )  THEN
                CALL GSCOLR  ( ititl, ier )
                CALL DSCOLR  ( ititl, ioldclr, iret)
                call ptitle ( ttlstr, ititle )
            END IF
	END IF
C
C	End the plot to flush everything out
C
	CALL DG_NEND ( ier )
C	CALL GFLUSH ( iret )
C*
	if ( verbose .gt. 0 ) call gfprinti (
     +     'returning from pvgrid - iret = ' // char(0), iret )

	RETURN
	END
