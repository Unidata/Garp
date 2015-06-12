C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	psnprof.f
C*
C*	Vertical profile drawing function. Derived from the GEMPAK
C*	program SNPROF.
C*
C*	History:
C*
C*	 1/97	COMET		Original copy
C*	 2/97	J. Cowie/COMET	Cleaned up, changed title time
C*	 3/97	COMET		Added wind position indicator.
C*	 5/97	COMET		Added gprintf to support logging.
C*	11/97	COMET		Added ptitle to display clickable titles.
C*	11/97	COMET		Added call to dscolor, cleaned up.
C*
C************************************************************************

	SUBROUTINE psnprof ( snfile, snparm, area, line, ptype, stndex, 
     +                       stncol, wind, winpos, marker, border,
     +                       title, thtaln, thteln, mixrln, yaxis,
     +                       xaxis, vcoord, dattim, panel,
     +                       text, filter, ititle, verbose, iret )
C************************************************************************
C************************************************************************

	INCLUDE		'GEMPRM.PRM'
C*
	CHARACTER	snfile*(*), snparm*(*), area*(*), line*(*),
     +			ptype*(*),  stndex*(*), stncol*(*), wind*(*),
     +			winpos*(*), marker*(*), border*(*), title*(*),
     +			thtaln*(*), thteln*(*), mixrln*(*), yaxis*(*),
     +			xaxis*(*),  vcoord*(*), dattim*(*),
     +			panel*(*),  text*(*),   filter*(*)

	INTEGER		verbose, iret
C*
	REAL		ylbl (LLAXIS), xlbl (LLAXIS)
	REAL		dwpt(LLMXLV,2), wnd(LLMXLV,3), temp(LLMXLV,2)
C*
	CHARACTER	snfcur*72, stn*8, arecur*48, vparm*12, tunit*1
	LOGICAL		done, proces, newfil, first, tmpflg
	CHARACTER	stparm (MMPARM)*4, pname1*4, pname2*4, wintyp*1
	INTEGER		ip1arr (3), ip2arr (3), ithtal (6), ithtel (6),
     +			imixrl (6), ibordr (3), idtype (LLMXLV)
	CHARACTER	ttlstr*72, ttl*72, blank*2
	REAL		data  (LLMXDT), rmargn (4), filtfc

C-----------------------------------------------------------------------

	iperr = 0
	iret = 0
	ioldclr = 0
	proces = .true.
	done = .false.
	blank = ' ' // char(0)

	if ( verbose .gt. 0 )
     +         call gfprints ( 'psnprof'//char(0), blank )
	if ( verbose .gt. 1 ) then
	    call gfprints ( '  snfile = ' // char(0), snfile )
	    call gfprints ( '  dattim = ' // char(0), dattim )
	    call gfprints ( '  area = ' // char(0), area )
	    call gfprints ( '  snparm = ' // char(0), snparm )
	    call gfprints ( '  line = ' // char(0), line )
	    call gfprints ( '  ptype = ' // char(0), ptype )
	    call gfprints ( '  vcoord = ' // char(0), vcoord )
	    call gfprints ( '  stndex = ' // char(0), stndex )
	    call gfprints ( '  stncol = ' // char(0), stncol )
	    call gfprints ( '  wind = ' // char(0), wind )
	    call gfprints ( '  winpos = ' // char(0), winpos )
	    call gfprints ( '  marker = ' // char(0), marker )
	    call gfprints ( '  border = ' // char(0), border )
	    call gfprints ( '  title = ' // char(0), title )
	    call gfprints ( '  yaxis = ' // char(0), yaxis )
	    call gfprints ( '  xaxis = ' // char(0), xaxis )
	    call gfprints ( '  filter = ' // char(0), filter )
	    call gfprints ( '  panel = ' // char(0), panel )
	    call gfprints ( '  text = ' // char(0), text )
	    call gfprints ( '  thtaln = ' // char(0), thtaln )
	    call gfprints ( '  thteln = ' // char(0), thteln )
	    call gfprints ( '  mixrln = ' // char(0), mixrln )
	    call gfprinti ( '  ititle = ' // char(0), ititle )
	    call gfprinti ( '  verbose = ' // char(0), verbose )
	    call gfprinti ( '  iret = ' // char(0), iret )
	end if

C
C	Set text.
C
	CALL IN_TEXT ( text, iret )

C
C*      Set plotting mode to graph mode.
C
        CALL GQMODE  ( mode, ier )
        CALL GSMODE  ( 2, ier )

C
C*	Indicate that file is closed.
C
        isnfln = 0
        snfcur = ' '
C
C*	    Open the data file.
C
	    IF  ( proces )  THEN
		CALL SNPFIL  ( snfile, snfcur, isnfln, newfil, iret )
C
C*		If file is opened, set up area.
C
		IF  ( iret .eq. 0 )  THEN
		    CALL LC_UARE  ( area, newfil, isnfln, arecur, stn,
     +				    iret )
		END IF
		IF  ( iret .ne. 0 )  proces = .false.
	    END IF
C
C*	    Get the vertical coordinate.
C
	    IF  ( proces )  THEN
		CALL ST_LCUC  ( vcoord, vparm, ier )
		CALL LV_CORD  ( vparm, vparm, ivcord, iret )
		IF  ( iret .ne. 0 .or.
     +		      ivcord .lt. 0 .or. ivcord .gt. 5 )  THEN
		    CALL ER_WMSG  ( 'LV', iret, vcoord, ier )
		    proces = .false.
		END IF
	    END IF
C
C*	    Check the parameters to be computed.
C
	    IF  ( proces )  THEN
C
C*		Get the level parameters to compute.
C
		CALL SNPPRM  ( vparm, snparm, line, wind, pname1, 
     +			       pname2, ip1arr, ip2arr, wintyp, iwncol, 
     +			       tunit, iret )
		IF  ( iret .ne. 0 )  proces = .false.
C
C*		Get information on background lines and border.
C
		CALL SNPLIN  ( border, thtaln, thteln, mixrln,
     +			       ibordr, ithtal, ithtel, imixrl, iret )
	    END IF
C
C*	    Set TMPFLG if abscissa is temperature and background lines
C*	    can be drawn.
C
	    IF ( proces ) THEN
		it = INDEX ( pname1, 'TMP' ) + INDEX ( pname1, 'TEMP' )
		IF ( it .eq. 0 ) THEN
		    tmpflg = .false.
		ELSE
		    tmpflg = .true.
		END IF
	    END IF
C
C*	    Get the stability indicies to list.
C
	    CALL SNPSTB  ( stndex, stncol, stparm, nstprm, istcol,
     +			   iret )
C
C*	    Get y-axis information.
C
	    IF  ( proces )  THEN
		CALL SNPYAX  ( ptype, yaxis, ivcord, iyaxis, ratio, 
     +			       ystrt, ystop, ylbl, nylbl, rmargn, 
     +                         iylbfr, iyglfr, iytmfr, iret )
		IF  ( iret .ne. 0 )  THEN
		    CALL ER_WMSG  ( 'SNPROF', iret, ' ', ier )
		    proces = .false.
		END IF
	    END IF
C
C*	    Set marker type and get wind information.
C
	    IF  ( proces )  THEN
		CALL IN_MARK  ( marker, mkcolr, ier )
		CALL ST_NUMB  ( winpos, iwposn, ier )
		IF  ( ( iwposn .lt. 1 ) .or. ( iwposn .gt. 3 ) )
     +						iwposn = 1
		CALL IN_FILT  ( filter, filtfc, ier )
	    END IF
C
C*		Set the time.
C
		CALL SN_STIM  ( isnfln, dattim, ier )
C
C*		    Get next station.
C
		    CALL SN_SNXT  ( isnfln, stn, istnm, slat, slon,
     +				    selv, iret )
		    IF  ( iret .ne. 0 )  THEN
			iout = iret
		      ELSE
			CALL SN_RDAT ( isnfln, nlev, data, ihhmm, iret )
			IF  ( iret .eq. 0 )  CALL SN_RTYP  ( isnfln,
     +						nnlev, idtype, ier )
		    END IF
C
C*		    Set station 
C
		    IF  ( iret .eq. 0 )  THEN
			CALL PC_SSTN  ( stn, istnm, slat, slon, selv,
     +					ispri,ihhmm, nlev, ier )
C
		    END IF
C
		    IF  ( iret .eq. 0 )  THEN
C
C*			Set panel.
C
			CALL GG_PANL  ( panel, ier )
C
C*		        Get the data for this plot.
C
			CALL SNPDTW
     +		           ( ip1arr, ip2arr, iwncol, data, nlev,
     +			     idtype, ystrt, ystop, temp, ntemp,
     +                       dwpt, ndwpt, wnd, nwind, 
     +                       xmin, xmax, iret )
C
C*	    		Get x axis information and set graph
C*			coordinates.
C
	    		IF  ( iret .eq. 0 )
     +		          CALL SNPXAX  
     +                           ( pname1, pname2, xaxis, iyaxis,
     +                             xmin, xmax,
     +				   xstrt, xstop, xlbl, nxlbl, 
     +                             ixlbfr, ixglfr, ixtmfr, iret )
   	   	        IF  ( iret .eq. 0 )
     +		      	  CALL SNPGRF
     +			     ( iyaxis, ratio, xstrt, xstop, ystrt, 
     +			       ystop, rmargn, windxn, windyn, iret )
   	   	        IF  ( iret .eq. 0 .and. iyaxis .eq. 4 ) 
     +                    CALL GG_SKEW ( xaxis, yaxis, pname1, ratio,
     +                                   xstrt, ystrt, xstop, ystop,
     +                                   xlbl, nxlbl, iret )
			IF  ( iret .eq. 0  )  THEN
C
C*			  Draw background.
C
			  CALL SNPBCK
     +				     ( ibordr, xstrt, xstop, ystrt, 
     +			  	       ystop, xlbl, nxlbl, ylbl, nylbl, 
     +				       ixlbfr, ixglfr, ixtmfr,
     +                                 iylbfr, iyglfr, iytmfr, ier )
C
C*			  Plot data.
C
			  CALL SNPPLT
     + 			       ( temp, ntemp, dwpt, ndwpt, wnd, nwind,
     +			         ip1arr, ip2arr, iwncol, iwposn, windxn,
     +			         windyn, filtfc, wintyp, mkcolr, iret )
C
C*			  Write out stability indicies.
C
			  CALL SNPPST  ( istcol, stparm, nstprm, data,
     +				         first, iwposn, stn, istnm,
     +				         dattim, iret )
C
C*			  Draw background lines.
C
			  IF ( tmpflg )
     +			  CALL SNPPLN  ( ithtal, ithtel, imixrl, pname1,
     +				         pname2, tunit, vparm, ystrt,
     +					 ystop, xstrt, xstop, ier )
C
C*			  Plot title.
C
			  CALL IN_TITL  ( title, -1, ititl, linttl, 
     +					  ttlstr, ier )
			  IF  ( ititl .gt. 0 )  THEN
			    CALL GSCOLR   ( ititl, ier )
			    CALL DSCOLR  ( ititl, ioldclr, iret )
			    lens = LEN ( ttlstr )
			    ttlstr(lens:lens) = char(0)
			    call ptitle ( ttlstr, ititle )
			  END IF
C
	       	        END IF
		    END IF
C
C       Not processing but at least plot a title.
C
                IF ( .not. proces ) THEN
                     CALL IN_TITL ( title, -1, ititl, linttl,
     +                              ttlstr, ier )
                     IF  ( ititl .gt. 0 )  THEN
                        CALL GSCOLR  ( ititl, ier )
                        CALL DSCOLR  ( ititl, ioldclr, iret )
                        call ptitle ( ttlstr, ititle )
                     END IF
                END IF

C
C*	    Write out message if no valid stations were found
C
	    IF  ( iret .ne. 0 ) 
     +          CALL ER_WMSG ( 'SNPROF', -9, ' ', ier )
C
C       Close sounding file.
C
        IF  ( isnfln .gt. 0 )  CALL SN_CLOS  ( isnfln, ier )

C	CALL GFLUSH  ( iret )

csd	if ( verbose .gt. 1 ) print*,'returning from psnprof'
	if ( verbose .gt. 0 ) call gfprinti ( 
     +  'returning from psnprof - iret = '//char(0), iret )

	return
	END
