C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	pwprof.f
C*
C*	Profiler data drawer. Derived from the GEMPAK program SNCROSS.
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*	 2/97	COMET	Fixed bug that sometimes did not allow more than
C*			one station to be plotted.
C*	 5/97	COMET	Added gprintf to support logging.
C*	11/97	COMET	Added ptitle to display clickable titles.
C*	11/97	COMET	Added call to dscolor, cleaned up.
C*
C************************************************************************


	SUBROUTINE pwprof ( cxstns, snfile, dattim, yaxis, taxis,
     +			   wind, title, fint, text, ititle, verbose,
     +                     iret )
C************************************************************************
C************************************************************************
	INCLUDE 	'GEMPRM.PRM'

	CHARACTER	cxstns*(*), snfile*(*), dattim*(*),
     +			yaxis*(*) , taxis*(*) , wind*(*)  ,
     +			title*(*) , fint*(*)  , text*(*)
	INTEGER		verbose
C*
	CHARACTER	border*72, cint*72
	CHARACTER	line*72, ptype*72, filter*72, shrttl*72
	CHARACTER	snparm*72, vcoord*72
	CHARACTER	curve*72
	CHARACTER	contur*72, fline*72, ctype*72,
     +			clrbar*72
	LOGICAL		clear
C
C*	The "old" values keep track of the current values.  NEWSTN
C*	is set when station data must be read in.  NEWPRM is set when
C*	a new parameter is selected.  NEWTHA is read when new theta
C*	data for an isentropic display must be read.
C
	CHARACTER	cxsold*72, snfold*72, datold*48, prmold*4,
     +			taxold*48, prmhld*16
	LOGICAL		newstn, newprm, newtha, newvco
C*
	CHARACTER	times (LLMXTM)*20, stns (LLTMCX)*20, parm*4
	CHARACTER	wintyp*1, winuni*1, vcord*4
	CHARACTER	ctlbl (LLMXTM)*12, ttlstr*72, blank*2
	PARAMETER	( MSDSIZ = 100000 )
	REAL		stndat ( MSDSIZ ), xtlbl (LLMXTM)
	REAL		sloc (LLTMCX), xx (LLMAXD), yy (LLMAXD), 
     +			pontha (LLMAXD,200), clvl (LLCLEV), 
     +			flvl (LLCLEV), pdat (LLTMCX,200),
     +			rmargn (4), grid (LLMAXD,LLMAXD),
     +			toptmp (LLTMCX), topwnd (LLTMCX),
     +			yaxval (LLAXIS)
	INTEGER		iltype (LLCLEV), linwid (LLCLEV), 
     +			ilabel (LLCLEV), icolor (LLCLEV),
     +			ifcolr (LLCLEV), iflabl (LLCLEV),
     +			ipsdat (LLTMCX), nlvls (LLTMCX), 
     +			idtype (LLMXLV,LLTMCX)
	LOGICAL		proces, respnd, done, isnflg, timflg,prmexs,
     +			wndexs, lflag, sflag, bflag, fflag, nflag
C*
	DATA		snfold, cxsold, prmold, datold, taxold
     +					  / 5 * ' ' /
	DATA		newstn, newprm, newvco  / 3 * .true. /
	DATA		ivcold / -9999 /
C------------------------------------------------------------------------
	 SNPARM   = 'barb' // char(0)
	 VCOORD   = 'hght' // char(0)
	 PTYPE    = 'lin' // char(0)
	 LINE     = '0/0/0' // char(0)
	 BORDER   = '1/1/3' // char(0)
	 CINT     = '1' // char(0)
	 CLEAR    = .FALSE.
	 FILTER   = 'no' // char(0)
	 CURVE    = '2' // char(0)
	 CLRBAR   = ' ' // char(0)
	 CONTUR   = '3/9' // char(0)
	 FLINE    = '10-20' // char(0)
	 CTYPE    = 'C' // char(0)
C
C*	Do not try to buffer file opens.
C
	snfold = ' '
	datold = ' '
	cxsold = ' '
	prmold = ' '
	taxold = ' '
	iret = 0
	ioldclr = 0
	blank = ' ' // char(0)

	if ( verbose .gt. 0 ) call gfprints ( 
     +     'pwprof' // char(0), blank )
	if ( verbose .gt. 1 ) then
	    call gfprints ( '  cxstns = ' // char(0), cxstns )
	    call gfprints ( '  snparm = ' // char(0), snparm )
	    call gfprints ( '  snfile = ' // char(0), snfile )
	    call gfprints ( '  dattim = ' // char(0), dattim )
	    call gfprints ( '  vcoord = ' // char(0), vcoord )
	    call gfprints ( '  ptype = ' // char(0),  ptype )
	    call gfprints ( '  yaxis = ' // char(0),  yaxis )
	    call gfprints ( '  taxis = ' // char(0),  taxis )
	    call gfprints ( '  line = ' // char(0),   line )
	    call gfprints ( '  border =  // char(0)', border )
	    call gfprints ( '  cint = ' // char(0),   cint )
	    call gfprints ( '  wind = ' // char(0),   wind )
	    call gfprints ( '  title = ' // char(0),  title )
	    call gfprinti ( '  clear = ' // char(0),  clear )
	    call gfprints ( '  filter = ' // char(0), filter )
	    call gfprints ( '  curve = ' // char(0),  curve )
	    call gfprints ( '  clrbar = ' // char(0), clrbar )
	    call gfprints ( '  contur = ' // char(0), contur )
	    call gfprints ( '  fint = ' // char(0),   fint )
	    call gfprints ( '  fline = ' // char(0),  fline )
	    call gfprints ( '  ctype = ' // char(0),  ctype )
	    call gfprints ( '  text = ' // char(0),   text )
	    call gfprinti ( '  ititle = ' // char(0), ititle )
	    call gfprinti ( '  verbose = ' // char(0),verbose )
	    call gfprinti ( '  iret = ' // char(0),   iret )
	end if

C
C	Indicate that file is closed.
C
	isnfln = 0
	isnflg = .FALSE.

C
C	Set text.
C
	CALL IN_TEXT  ( text, iret )

C
C	Set plotting mode to graph mode.
C
	CALL GQMODE  ( mode, ier )
	CALL GSMODE  ( 2, ier )

C
C*	    Set flag to indicate processing will be done.
C
	    proces = .true.
	    prmexs = .true.
	    wndexs = .true.
C
C*	    Exit if there is an error.
C
	    IF  ( iret .ne. 0 ) THEN
		done = .true.
	      ELSE
C
C*		Open the sounding file.
C
		CALL SNSFIL  ( snfile, snfold, isnfln, newstn, nparms,
     +			       iret )
		IF  ( iret .ne. 0 )  THEN
		    proces  = .false.
		  ELSE
		    CALL SNSDAT  ( isnfln, dattim, datold, newstn,
     +				   times, ntime, iret )
		    IF  ( iret .ne. 0 )  THEN
			proces = .false.
		      ELSE IF  ( ntime .eq. 1 )  THEN
			timflg = .false.
                   ELSE IF  ( ntime .gt. 1 )  THEN
			timflg = .true.
			IF  ( ntime .gt. LLTMCX )  THEN
			    ntime = LLTMCX 
			END IF
		      ELSE
			iret  = -6
			CALL ER_WMSG  ( 'pwprof', iret, dattim, ier )
			proces = .false.
			datold = ' '
		    END IF
		END IF
C
C*		Check to see if the station(s) have changed.
C
		CALL ST_LCUC  ( cxstns, cxstns, ier )
		IF  ( cxsold .ne. cxstns )  THEN
		    newstn = .true.
		    cxsold = ' '
		END IF
C
C*		Get wind information.
C
		CALL IN_WIND  ( wind, wintyp, winuni, iwnclr, ier )
C
C*		Get the parameter to evaluate.
C
		CALL ST_CLST  ( snparm, ';', ' ', 1, prmhld, n, ier )
		parm = prmhld (1:4)
		CALL ST_LCUC  ( parm, parm, ier )
		IF  ( ( parm .eq. ' ' ) .and. ( iwnclr .eq. 0 ) )  THEN
		    iret = -13
		    CALL ER_WMSG  ( 'pwprof', iret, snparm, ier )
		    proces = .false.
		    newprm = .true.
		  ELSE IF  ( parm .ne. prmold )  THEN
		    newprm = .true.
		    IF  ( parm .eq. 'ISEN' )  THEN
			isnflg = .true.
		      ELSE
			isnflg = .false.
		    END IF
		END IF
C
C*		Check to see if the time axis has changed for a time
C*		series.
C
		IF  ( taxis .ne. taxold )  newstn = .true.
		taxold = taxis
C
C*		Get the data for the selected stations or times.
C
		iret = 0
		IF  ( proces .and. ( .not. timflg ) .and. newstn )
     +							    THEN
		    CALL SNSSST ( cxstns, isnfln, nparms, MSDSIZ, 
     +				  nstn, stns, ipsdat, nlvls, stndat, 
     +				  idtype, sloc, xmin, xmax, iret )
		    IF  ( iret .ne. 0 )  proces = .false.
		  ELSE IF  ( proces .and. timflg .and. newstn )  THEN

		    CALL wprofstm (cxstns, isnfln, nparms, MSDSIZ, times,
     +				  ntime, taxis, nstn, ipsdat, nlvls,
     +				  stndat, idtype, sloc, xmin, xmax,
     +				  xtlbl, ctlbl, nxlbl, ixlbfr, ixglfr,
     +				  ixtmfr, iret )
		    IF  ( iret .ne. 0 )  proces = .false.
		END IF
C
C*           Check for at least one station.
C
             IF  ( proces .and. ( nstn .ge. 1 ) )  THEN
		    cxsold = cxstns
		    prmold = parm
		  ELSE
		    cxsold = ' '
		    prmold = ' '
		    proces = .false.
		END IF
C
C*		Get information about the y axes.  Find the y points
C*		on a LLMAXD by LLMAXD grid.
C
		IF  ( proces ) THEN
		    CALL SNSYAX  ( ptype, vcoord, yaxis, iytype, ratio,
     +				   rmargn, ybot, ytop, ivcord, vcord,
     +				   yaxval, nyval, iylbfr, iyglfr, 
     +				   iytmfr, iret )
		    IF  ( iret .ne. 0 )  THEN
			proces = .false.
		      ELSE IF  ( isnflg .and. ( ivcord .ne. 1 ) )  THEN
			iret = -4
			CALL ER_WMSG  ( 'pwprof', iret, ' ', ier )
			proces = .false.
		      ELSE
			IF ( ivcold .eq. ivcord ) newvco = .false.
		        ivcold = ivcord
			CALL SNSGRD  ( iytype, xmin, xmax, ybot, ytop, 
     +				       ratio, rmargn, yy, xx, iret )
			IF  ( iret .ne. 0 ) proces = .false.
		    END IF
		END IF
C
C*		Get the minimum pressure for temperature and winds.
C
		IF  ( proces )  THEN
		    CALL SNSRGE  ( vcord, nstn, stndat, nlvls, ipsdat, 
     +				   toptmp, topwnd, iret )
C
C*		    Get interval and curve type.
C
		    CALL SNSGTH  ( cint, curve, ithinc, icvtyp, newtha,
     +				   iret )
		END IF
C
C*		Get either grid or isentropic data.
C
		IF  ( proces )  THEN
	      		IF ( ( .not. isnflg ) .and. 
     +			      ( newprm .or. newstn .or. newvco ) ) THEN
			CALL SNSPRM ( snparm, nstn, stns, ipsdat, nlvls,
     +				      stndat, sloc, xx, yy, vcord,
     +				      ivcord, icvtyp, grid, pontha,
     +				      iret )
			IF  ( iret .eq. 0 .and. parm .ne. ' ' )  THEN
			    CALL IN_CONT ( contur, ier )
			    CALL IN_CTYP ( ctype, nflag, lflag, sflag, 
     +					   bflag, fflag, ier )
			END IF
			IF  ( iret .ne. 0 )  THEN
			    prmexs = .false.
			  ELSE
			    CALL GSGGRF  ( 1, iytype, LLMAXD, LLMAXD,
     +					   xmin, ybot, xmax, ytop, ier )
			    IF  ( ier .ne. 0 )  THEN
				iret = -15
				CALL ER_WMSG ('pwprof', iret, ' ', ier)
				proces = .false.
			    END IF
			END IF
		    END IF
		END IF
C
C*		Check to see if wind data exist.		
C
	       IF ( proces ) THEN
	           CALL SNSWWE  ( winuni, vcord, ier )
	           IF  ( ier .ne. 0 )  THEN
		       wndexs = .false.
	           END IF
	       END IF
C
C*		Draw the cross section.
C
		IF  ( proces .and. ( prmexs .or. wndexs ) )  THEN
C
C*		    Flush out the contours.
C
C		    CALL GFLUSH ( ier )

		    CALL wprofbor ( border, ybot, ytop, yaxval, nyval, 
     +				   iylbfr, iyglfr, iytmfr, xmin, xmax, 
     +				   nstn, stns, sloc, xx, pontha, 
     +				   toptmp, topwnd, xtlbl, ctlbl,
     +				   nxlbl, ixlbfr, ixglfr, ixtmfr, 
     +				   timflg, iret )
C
C*		    Draw the winds.
C
		    IF ( wndexs )  THEN
		        IF  ( iwnclr .gt. 0 )  THEN
			    CALL GSCOLR  ( iwnclr, ier )
			    CALL IN_FILT ( filter, filtfc, ier )
			    CALL wprofwnd ( wintyp, nstn, ipsdat, 
     +				           nlvls, stndat, idtype, sloc, 
     +				           ybot, ytop, filtfc, vcord, 
     +				           iret )
		        END IF
		    END IF
C
C*		    Write the title.
C
		    CALL SNSTTL (title, timflg, cxstns, times, ntime,
     +				 icttl, linttl, ttlstr, shrttl, iret)
		    IF  ( clear )  CALL GMESG ( shrttl, ier )
		    IF  ( icttl .gt. 0 )  THEN
			CALL GSCOLR  ( icttl, ier )
			CALL DSCOLR  ( icttl, ioldclr, iret )
			lens = LEN ( ttlstr )
			ttlstr(lens:lens) = char(0)
			call ptitle ( ttlstr, ititle )
		    END IF
		END IF
	    END IF
C
C	Reset plotting mode and flush buffers.
C
	CALL GSMODE  ( mode, ier )
C
C	Close file.
C
	if ( isnfln .gt. 0 ) CALL SN_CLOS ( isnfln, ier )
C	CALL GFLUSH  ( iret )
C*
	if ( verbose .gt. 0 ) call gfprinti (
     +     'returning from pwprof - iret = ' // char(0), iret )
	RETURN
	END
