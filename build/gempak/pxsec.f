C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	pxsec.f
C*
C*	Cross section drawing function. Derived from the GEMPAK program
C*	GDCROSS.
C*
C*	History:
C*
C*	11/96	COMET		Original copy
C*	 2/97	J. Cowie/COMET	Changed title time
C*	 3/97	J. Cowie/COMET	Changed to deal with unspecified contour
C*				or fill interval
C*	 5/97	COMET		Added gprintf to support logging.
C*	11/97	COMET		Added ptitle to display clickable titles.
C*	12/97	COMET		Changed varible "len" to "lent".
C*	 5/99	COMET		Set fflag=F as default to eliminate bug
C*	 3/07	S.Chiswell/Unidata	new in_intc argument clbl, flbl,
C*					updated call to gcntln, gclgrn
C*	 8/07	S.Chiswell/Unidata	removed call to GCNTLN
C*
C************************************************************************


	SUBROUTINE pxsec ( gdfile, gdatim, gfunc, gvcord, gvect, cxstns,
     +			   ctype, ptype, yaxis, scale, wind, cint, line,
     +			   contur, fint, fline, clrbar, title, skip,
     +			   refvec, text, frame, ititle, verbose, iperr )
C************************************************************************
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
C*
	CHARACTER	gdfile*(*), gdatim*(*), gfunc*(*), gvcord*(*),
     +			gvect*(*) , cxstns*(*), ctype*(*), ptype*(*) ,
     +			yaxis*(*) , scale*(*) , wind*(*) , cint*(*)  ,
     +			line*(*)  , contur*(*), fint*(*) , fline*(*) ,
     +			clrbar*(*), title*(*) , skip*(*) , refvec*(*),
     +			text*(*)
	INTEGER		verbose, frame

	CHARACTER	border*72, panel*72, shrttl*72, ttl*72
	CHARACTER	pfcint*80, pffint*80, carr(3)*36
	CHARACTER	blank*2
C*
	LOGICAL         lscal, lvert
	LOGICAL		cflag, lflag, sflag, bflag, fflag, nflag
C*
	REAL 		ugrd (LLMXGD), vgrd (LLMXGD), ponth (LLMXGD)
	REAL	        xgrd (LLMXGD), qgrd (LLMXGD), rlvls (LLMXLV),
     +			qlvls (LLMXLV), vlvls (LLMXLV), ylbl (LLAXIS),
     +                  rgx (1000), rgy (1000), rlat (1000), 
     +			rlon (1000), vclsfc (1000), frarr(3)
	CHARACTER	time (2)*20, lastim*20, ttlstr*72, parm*12,
     +			timev (2)*20, parmv*12, firstm*20, prmlbl*12,
     +			timfnd*36, times (2)*20
	CHARACTER	cproj*4, clbl(LLCLEV)*24, flbl(LLCLEV)*24
	LOGICAL		proces, havsfc, havscl, havvec, gottm, first,
     +			scflag
C*
	REAL		clvl (LLCLEV), flvl (LLCLEV), rmargn (4)
	INTEGER		icolor (LLCLEV), iline (LLCLEV), ilwid (LLCLEV),
     +			labflg (LLCLEV), ifcolr (LLCLEV),ifltyp(LLCLEV),
     +			iflabl (LLCLEV), level(2), iflwid (LLCLEV)
	SAVE		pffint, pfcint
	COMMON/GDXS/	cproj,  angle1, angle2, angle3, imx, imy,
     +			dlatll, dlonll, dlatur, dlonur
C-----------------------------------------------------------------------

	iperr = 0
	ioldclr = 0
	blank = ' ' // char(0)
	border='1'
	panel='0'
C	text= '1/2//hw'
	shrttl=' '
	ier = 0
	iflno = 0
	fflag = .false.

	if ( verbose .gt. 0 ) call gfprints ( 
     +     'pxsec' // char(0), blank )
	if ( verbose .gt. 1 ) then
	    call gfprints ( '  gdfile = ' // char(0), gdfile )
	    call gfprints ( '  gdatim = ' // char(0), gdatim )
	    call gfprints ( '  gfunc = ' // char(0), gfunc )
	    call gfprints ( '  gvcord = ' // char(0), gvcord )
	    call gfprints ( '  gvect = ' // char(0), gvect )
	    call gfprints ( '  cxstns = ' // char(0), cxstns )
	    call gfprints ( '  ctype = ' // char(0), ctype )
	    call gfprints ( '  ptype = ' // char(0), ptype )
	    call gfprints ( '  yaxis = ' // char(0), yaxis )
	    call gfprints ( '  scale = ' // char(0), scale )
	    call gfprints ( '  wind = ' // char(0), wind )
	    call gfprints ( '  cint = ' // char(0), cint )
	    call gfprints ( '  line = ' // char(0), line )
	    call gfprints ( '  contur = ' // char(0), contur )
	    call gfprints ( '  fint = ' // char(0), fint )
	    call gfprints ( '  fline = ' // char(0), fline )
	    call gfprints ( '  clrbar = ' // char(0), clrbar )
	    call gfprints ( '  title = ' // char(0), title )
	    call gfprints ( '  skip = ' // char(0), skip )
	    call gfprints ( '  refvec = ' // char(0), refvec )
	    call gfprints ( '  text = ' // char(0), text )
	    call gfprinti ( '  frame = ' // char(0), frame )
	    call gfprinti ( '  ititle = ' // char(0), ititle )
	    call gfprinti ( '  verbose = ' // char(0), verbose )
	    call gfprinti ( '  iperr = ' // char(0), iperr )
	end if

C
C*	Clear out the contour/fill info from any previous frames
C
	if ( frame .eq. 1 ) then
	    if ( INDEX ( ctype, 'C') .gt. 0 ) pfcint(1:) = ' '
	    if ( INDEX ( ctype, 'F') .gt. 0 ) pffint(1:) = ' '
	endif
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
        ELSE
C
C*          Process the GDATTIM input; setup the time server.
C
            CALL DG_NDTM ( gdatim, ier )
            IF ( ier .ne. 0 ) THEN
                CALL ER_WMSG ( 'DG', ier, gdatim, irr )
                proces = .false.
            END IF
	END IF
C
C*	Set the attributes that do not vary within the time loop.
C
	IF ( proces ) THEN
C
C*	    Set the text attributes, especially the size,
C*	    before setting the margins.
C
	    CALL IN_TEXT ( text, ier )
C
C*	    Get contouring type.
C
	    CALL IN_CONT ( contur, ier )
	    CALL IN_CTYP ( ctype, nflag, lflag, sflag, bflag, fflag, 
     +				ier )
	    IF ( lflag .or. sflag .or. bflag .or. nflag ) THEN
		  cflag = .true.
		ELSE
		  cflag = .false.
	    END IF
	END IF
C
C*      Loop over times.
C
        gottm = proces
        first = .true.
        DO  WHILE ( gottm )
C
C*          Get the next time to process from the time server.
C
            CALL DG_NTIM ( .true., .true., time, gottm, ier )
            proces = ( ier .eq. 0 .and. gottm )
            IF ( ier .ne. 0 ) THEN
                ier = 3
                CALL ER_WMSG ( 'GDCROSS', ier, time(1), irr)
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
C*	    Get time and vertical coordinate to use.
C
	    IF  ( proces )  THEN
		CALL GDXDTV  ( timfnd, gvcord, gfunc, firstm, lastim, 
     +			       times,   ivcord, iret )
		IF  ( iret .ne. 0 )  THEN
		    CALL ER_WMSG  ( 'GDCROSS', iret, ' ', ier )
		    proces = .false.
		END IF
	    END IF
C*
	    IF  ( proces .and. first )  THEN
	        CALL DG_FLNO ( gvect, iflnov, ier1 )
		CALL DG_QDTM ( iflnov, firstm, lastim, ier )
                IF ( firstm .eq. ' ' ) THEN
                    proces = .false.
                END IF
            END IF
C
	    IF ( proces ) THEN
		CALL GDXDTV  ( timfnd, gvcord, gvect, firstm, lastim, 
     +			       timev,  jvcord, iret )
		IF  ( iret .ne. 0 )  THEN
		    CALL ER_WMSG  ( 'GDCROSS', iret, ' ', ier )
		    proces = .false.
		END IF
	    END IF
C
C*	    Get information about y axis.
C
	    IF  ( proces .and. first )  THEN
		CALL GDXYAX  ( ptype, yaxis, ivcord, iyaxis, ratio, 
     +			       ystrt, ystop, ylbl, nylbl, rmargn, 
     +                         ilbfrq, iglfrq, itmfrq, iret )
		IF  ( iret .ne. 0 )  THEN
		    CALL ER_WMSG  ( 'GDCROSS', iret, ' ', ier )
		    proces = .false.
		END IF
	    END IF

	    igt = INDEX ( cxstns, '>' )
            IF ( proces .and. igt .eq. 0 ) THEN
                iret = -4
                CALL ER_WMSG  ( 'GDCROSS', iret, ' ', ier )
                proces = .false.
            END IF
C
C*          Compute subset grid needed for cross section path
C
	    IF ( proces ) THEN
	        CALL DG_CXGP ( cxstns, 'YES', 1000, 
     +			nhxs, rgx, rgy, rlat, rlon, iret )
	    END IF
            IF  ( iret .ne. 0 ) proces = .false.
C
C*          Compute length of cross section.
C
	    IF ( proces ) THEN
                CALL GDXLEN ( nhxs, rlat, rlon, rlngth, iier )
	    END IF
C
C*          Check that there are some points.
C
            IF  ( nhxs .le. 0 )  THEN
                    proces = .false.
            END IF
C
C*	    Set the origin of the cross section for MSFC calculation.
C
	    IF  ( proces .and. first )  THEN
	        CALL DG_ORGN ( rlat (1), rlon (1), ier )
	    END IF
C
	    IF ( proces ) THEN
C
C*	        Get the surface data.
C
	        CALL GDXGTS  ( times, ivcord, rgx, rgy, nhxs,  
     +		             vclsfc, havsfc, parm, ier )
C
C*		Get scalar data to plot.
C
	        CALL GDXDTA  ( iflnos, timfnd, gvcord, ystrt,
     +                    ystop, gfunc, times, ivcord,
     +	                  rgx, rgy, nhxs, rlvls, xgrd,
     +		          nvxs, prmlbl, ybeg, yend, iret )
C
C*  	        If all is well, create a regularly spaced grid.
C
	        IF ( iret .eq. 0 ) THEN
	            havscl = .true.
	            CALL GDXGRD ( xgrd, nhxs, nvxs, ivcord, iyaxis, 
     +				  rlvls, ystrt, ystop, .false.,
     +			    	  qgrd, qlvls, nvo, iret )
	      	    IF ( iret .ne. 0 ) THEN
	        	iret = - 10
	        	CALL ER_WMSG ( 'GDCROSS', iret, ' ', ier )
	            ELSE
C
C*	                Set underground values to missing.
C
                	IF ( havsfc ) THEN
	          	    CALL GDXSFM ( ivcord, qgrd, qlvls, nhxs, 
     +					  nvo, vclsfc, iret )
	        	END IF
              	    END IF
	    	ELSE
	      	    havscl = .false.
	      	    IF ( iret .lt. 0 ) proces = .false.
	    	END IF
	    END IF
C
C*	    Get the vector components defined by GVECT.
C
	    IF ( proces ) THEN
	        CALL GDXDVV ( iflnov, timfnd, gvcord, ystrt, ystop,
     +			  gvect, timev, ivcord, rgx, rgy,
     +			  nhxs, rlvls, ugrd, vgrd, ponth, nvv,
     +		          parm, parmv, lvert, lscal, iret )
	    	IF ( iret .eq. 0 ) THEN
	            havvec = .true.
		    IF ( .not. havscl ) prmlbl = parm
C
		    DO  ik = 1, nvv
		        vlvls (ik) = rlvls (ik)
		    END DO
C
		    IF ( havsfc ) THEN
		        CALL GDXSFM ( ivcord, ugrd, vlvls, nhxs, nvv,
     +	                        vclsfc, iret )
		        CALL GDXSFM ( ivcord, vgrd, vlvls, nhxs, nvv,
     +	                        vclsfc, iret )
	            END IF
	        ELSE
	            havvec = .false.
	            IF ( iret .lt. 0 ) proces = .false.
	        END IF        
	    END IF
C
C*	    Define contour levels and characteristics.
C*	    Write warning if there are no contour levels.
C
	    nlvl = 0
	    IF ( proces .and. havscl ) THEN

c	        CALL GDXLEV ( cflag, line, cint, fflag, fline, fint,
c     +			    scale, nhxs, nvo, 1, 1, nhxs, nvo, qgrd,
c     +			    nlvl, clvl, icolor, iline, ilwid, labflg,
c     +			    nflvl, flvl, ifcolr, iflabl, iscale, dmin,
c     +			    dmax, iret )

***
*** the following stuff was added in place of the call to GDXLEV above.
*** Much of it duplicates what GDXLEV does, we need to tweak things
*** a little though. -jrc
***

     	        CALL IN_SCAL ( scale, iscale, iscalv, iret)
	        CALL GR_SSCL ( iscale, nhxs, nvo, 1, 1,
     +			     nhxs, nvo, qgrd, dmin, dmax, iret )
C
C*	        Do the regular contours
C
	        IF ( cflag ) THEN

		    CALL ST_CLST ( cint, '/', ' ', 3, carr, num, iret )
	            IF ( (carr(1) .eq. ' ' ) .and.
     +		       (pfcint  .ne. ' '    ) ) cint = pfcint

		    CALL IN_INTC ( cint, dmin, dmax, clvl, nlvl, clbl,
     +				     rint, cmin, cmax, iret )
		    IF  ( iret .ne. 0 )  THEN
			nclvl = 0
			rint  = 0.
		    END IF
C
C*	            If undefined, save the new cint for the next frame
C
	            IF ( (carr(1) .eq. ' ' ) .and.
     +		       (pfcint  .eq. ' '    )) THEN
     		       write(pfcint,'(F10.2,A,F10.2,A,F10.2)')
     +			rint,'/',cmin,'/',cmax
		       call ST_RMBL (pfcint, pfcint, lent, ier)
		       cint = pfcint
	            END IF

		    CALL IN_LINE ( line, clvl, nlvl, icolor, 
     +				 iline, ilwid, labflg, 
     +                           smooth, filter, scflag, iret )
C
C*		    Check for duplicate contours & sort contours
C
		    CALL GR_NLEV ( nlvl, clvl, clbl, icolor, iline,
     +				 ilwid, labflg, iret ) 

	        END IF
C
C*	        Get the filled contours.
C
	        IF ( fflag ) THEN

		    iflist = INDEX ( fint, ';' )
		    CALL ST_CLST ( fint, '/', ' ', 3, carr, num, iret )
		    CALL ST_CRNM ( carr(1), frarr(1), ier )
		    CALL ST_CRNM ( carr(2), frarr(2), ier )
		    CALL ST_CRNM ( carr(3), frarr(3), ier )
	            IF (((frarr(1) .eq. RMISSD ) .or.
     +		       (frarr(2) .eq. RMISSD ) .or.
     +		       (frarr(3) .eq. RMISSD )) .and. 
     +		       (pffint   .ne. ' '    )  .and.
     +		       (iflist   .eq. 0 )) fint = pffint
C
C*		    Define color fill contours. If the min or max is
C*		    already specified, use it.
C
		    CALL ST_CLST ( fint, '/', ' ', 3, carr, num, iret )
		    CALL ST_CRNM ( carr(2), frarr(2), ier )
		    CALL ST_CRNM ( carr(3), frarr(3), ier )
		    if ( frarr(2) .ne. RMISSD ) dmin = frarr(2)
		    if ( frarr(3) .ne. RMISSD ) dmax = frarr(3)

		    CALL IN_INTC ( fint, dmin, dmax, flvl, nflvl, flbl,
     +				   rfint, fmin, fmax, iret )

		    IF ( iret .ne. 0 ) THEN
		        nflvl = 0
			rfint = 0.
		    END IF

	            IF (((frarr(1) .eq. RMISSD )  .or.
     +		       (frarr(2) .eq. RMISSD )  .or.
     +		       (frarr(3) .eq. RMISSD )) .and.
     +                 (pffint   .eq. ' '    )  .and.
     +		       (iflist   .eq. 0 )) THEN
     		        write( pffint,'(F10.2,A,F10.2,A,F10.2)')
     +			(flvl(2)-flvl(1)), '/',flvl(1),'/',flvl(nflvl)
		        call ST_RMBL (pffint, pffint, lent, ier)
		        fint = pffint
	            END IF
C
C*		    Get the colors, line types, line widths and labels
C
		    IF  ( nflvl .eq. LLCLEV )  THEN
		        nflvl = nflvl - 1
		    END IF
		    nflvl1 = nflvl + 1
		    CALL IN_LINE ( fline, flvl, nflvl1, ifcolr, 
     +				   ifltyp, iflwid, iflabl, 
     +                           smooth, filter, scflag, iret )
C
C*		    Check for duplicate fill contours & sort.
C
		    CALL GR_NLEV ( nflvl, flvl, flbl, ifcolr, ifltyp,
     +				   iflwid, iflabl, iret ) 
		ENDIF
C*
C*		End COMET Tweak
C*
	        IF ( ( nlvl .eq. 0 .and. nflvl .eq. 0 ) .or. 
     +			iret .ne. 0 ) THEN
		    CALL ER_WMSG ( 'GDCROSS',1,' ',ier)
		END IF
C
	        IF ( nlvl .eq. 0 ) cflag = .false.
	        IF ( nflvl .eq. 0 ) fflag = .false.
            END IF
C
C*	    Set plotting mode to graph mode.
C
	    CALL GQMODE  ( mode, ier )
	    CALL GSMODE  ( 2, ier )
C
C*	    Draw the cross section.
C
	    IF  ( proces ) THEN
C
C*	        Set first to false upon first successful plot.
C
	        first = .false.
C
C*		Clear screen if requested and set panel.
C
		CALL GG_PANL  ( panel, ier )
C
C*		Set up the graph.
C	
		xstrt = 1.00
	        xstop = FLOAT ( nhxs )
		CALL GDXSUG ( iyaxis, ystrt, ystop, xstrt, xstop,
     +			      ratio, rmargn, iret )
C
C*	        Draw the contours.
C
	        IF ( havscl ) THEN
		    CALL GSGGRF ( 1, iyaxis, nhxs, nvo, xstrt, ystrt,
     +				xstop, ystop, iret )
C
C*		    Do side labels for THTA.
C
c	            parmv = ' '
c	            parmv = gfunc (1:4)
c	            CALL ST_LCUC ( parmv, parmv, ier )
c	            IF ( parmv (1:4) .eq. 'THTA' .and. iret .eq. 0 ) THEN
c	  	      IF ( cflag ) CALL GDXSDL ( nhxs, nvo, qgrd, nlvl,
c     +					       clvl, labflg, iret )
c		      IF ( fflag ) CALL GDXSDL ( nhxs, nvo, qgrd, nflvl,
c     +					       flvl, iflabl, iret )
c		    END IF
C
	            IF ( iret .eq. 0 ) THEN
                        IF  ( fflag )  THEN
                            CALL GCFILL ( nhxs, nvo, qgrd, 0, 0, 0,
     +                                  nflvl, flvl, ifcolr, iflabl,
     +                                  ifltyp, iret )
                            IF ( iret .ne. 0 ) CALL ER_WMSG ('GEMPLT',
     +						        iret, ' ', ier)
                        END IF
C
                        IF  ( cflag )  THEN
                            IF  ( lflag )  THEN
                                CALL GCLGRN ( nhxs, nvo, qgrd, 0, 0, 0,
     +                                       nlvl, clvl, clbl, icolor,
     +                                       iline, ilwid,
     +                                       labflg, scflag, iret )
                                IF ( iret .ne. 0 ) CALL ER_WMSG
     +                                      ( 'GEMPLT', iret, ' ', ier )
C
                      	    END IF
			    
                            IF  ( bflag )  THEN
                                CALL GCBOXX  ( nhxs, nvo, qgrd, 0, 0, 0,
     +                                       nlvl, clvl, icolor,
     +                                         iline, ilwid, labflg,
     +                                         iret )
                                IF ( iret .ne. 0 ) CALL ER_WMSG
     +                                      ( 'GEMPLT', iret, ' ', ier )
                            END IF
		        END IF
	            ELSE
	                iret = -11
		        CALL ER_WMSG  ( 'GDCROSS', iret, ' ', ier )
	            END IF
	        END IF
C
	        IF ( havvec ) THEN
	            IF ( lscal ) THEN
C
C*		        Scale the vertical component.
C
		        asprat=0.0
	                CALL GDXSCV ( vgrd, ponth, vlvls, nhxs, nvv,
     +	                          rlngth, ivcord, iyaxis, ystrt,
     +                            ystop, asprat, vgrd, iiir )
	                IF ( iiir .ne. 0 ) THEN
                            CALL ER_WMSG  ( 'GDCROSS', iiir, ' ', ier )
		        END IF
		    END IF
C
C*		    Load the locations of the wind points into
C*		    arrays xgrd and qgrd.
C
		    indx = 1
		    DO k = 1, nvv
		    DO i = 1, nhxs
		      xgrd ( indx ) = FLOAT ( i )
		      qgrd ( indx ) = vlvls ( k )
		      indx = indx + 1
		    END DO
	            END DO
C
C*		    Plot the vector field.
C
	            CALL GDXPUW ( gvect, ugrd, vgrd, xgrd, qgrd,
     +			        nhxs, nvv, wind, skip, refvec, ier )
	        END IF
C
C*		Plot background axes with labels.
C
	        CALL GDXPLT  ( border, ystrt, ystop, vclsfc, havsfc,
     +			       ylbl, nylbl, xstrt, xstop, cxstns,
     +			       nhxs, ilbfrq, iglfrq, itmfrq, iret )
C
C*		Plot the color bar.
C
		IF  ( fflag ) CALL GG_CBAR ( clrbar, nflvl, flvl,
     +					      ifcolr, ier )
	    END IF
C
C*	    Write title., even if proces was .false.
C
	    CALL IN_TITL  ( title, 0, ititl, linttl, ttlstr, ier )
	    IF  ( ititl .ne. 0 )  THEN
	        CALL GSCOLR  ( ititl, ier )
		CALL DSCOLR  ( ititl, ioldclr, iret )
		lens = LEN ( ttlstr )
		ttlstr(lens:lens) = char(0)
		call ptitle ( ttlstr, ititle )
	    END IF

C
C*	    Reset the plotting mode
	    CALL  GSMODE  ( mode, ier )

	END DO
C
	CALL DG_NEND( iret )
C
C	CALL GFLUSH ( iret )
C
	if ( verbose .gt. 0 ) call gfprinti (
     +     'returning from pxsec - iret = ' // char(0), iret )
	RETURN
	END
                             
