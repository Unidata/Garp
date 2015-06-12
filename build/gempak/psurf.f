C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	psurf.f
C*
C*	Plots surface data. Derived from the GEMPAK program SFMAP.
C*
C*	History:
C*
C*	11/96	COMET		Original copy
C*	 2/97 J. Cowie/COMET	Add day of the week to title.
C*	 4/97	COMET		Added gprintf to support logging.
C*	 8/97	COMET		Added pmarker, marker.
C*	10/97	COMET		Added nstns, call to get_data_area()
C*	11/97	COMET		Added ptitle to display clickable titles.
C*	11/97	COMET		Added call to dscolor, cleaned up.
C*
C************************************************************************



	SUBROUTINE psurf ( area, sfparm, dattim, sffile, colors, title, 
     +                     filter, text, msize, mwidth, marker, pmarker,
     +                     skpmis, nstns, ititle, verbose, iret )
C************************************************************************
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
C*
	CHARACTER	area*(*), sfparm*(*), 
     +			dattim*(*), sffile*(*), colors*(*),
     +			title*(*), filter*(*),
     +			text*(*), msize*(*), mwidth*(*)
    	INTEGER		verbose, pmarker, mrkwid
 
	LOGICAL		skpmis
C*
	CHARACTER	sffcur*72, arecur*48, fmt*80, arg*80
	CHARACTER 	pmdset (MMPARM)*4, prmlst (MMPARM)*4
	CHARACTER	tstn*8, sta*8, ttlstr*48, ttt*72
        CHARACTER       prcons (MMPARM)*16, chd (MMPARM)*8
	CHARACTER	area1*48, area2*48
	CHARACTER	blank*2
	INTEGER		icolor (MMPARM), iscale (MMPARM)
	LOGICAL		proces, newfil, chrflg (MMPARM)
	LOGICAL		wndflg, plot
	REAL		offset (4), sxplt (LLSTFL), outd (MMPARM)
	REAL		syplt (LLSTFL), data (MMPARM), size
	REAL		ccvals (MMPARM*LLCLEV), tsize(MMPARM)
	INTEGER         numccc (MMPARM), icclrs (MMPARM*LLCLEV)
	INTEGER         icrprm (MMPARM), jwide(MMPARM)
	CHARACTER	endflg*1

C------------------------------------------------------------------------
	iret = 0
	nstns = 0
	blank = ' ' // char(0)
C
C	Set up the initial conditions
C
	proces = .true.
	iflno  = 0
	iopt   = 0
	imark  = 0
	ioldclr = 0
	sffcur = ' '
	read ( msize, '(f5.1)' ) size
	read ( mwidth, '(i5)' ) mrkwid


	if ( verbose .gt. 0 ) call gfprints ( 
     +     'psurf'//char(0), blank )
	if ( verbose .gt. 1 ) then
	    call gfprints ( '  area = ' // char(0), area )
	    call gfprints ( '  sfparm = ' // char(0), sfparm )
	    call gfprints ( '  dattim = ' // char(0), dattim )
	    call gfprints ( '  sffile = ' // char(0), sffile )
	    call gfprints ( '  colors = ' // char(0), colors )
	    call gfprints ( '  title = ' // char(0), title )
	    call gfprints ( '  filter = ' // char(0), filter )
	    call gfprints ( '  text = ' // char(0), text )
	    call gfprints ( '  msize = ' // char(0), msize )
	    call gfprints ( '  mwidth = ' // char(0), mwidth )
	    call gfprinti ( '  marker = ' // char(0), marker )
	    call gfprinti ( '  pmarker = ' // char(0), pmarker )
	    call gfprinti ( '  skpmis = ' // char(0), skpmis )
	    call gfprinti ( '  ititle = ' // char(0), ititle )
	    call gfprinti ( '  verbose = ' // char(0), verbose )
	    call gfprinti ( '  iret = ' // char(0), iret )
	end if

C
        CALL IN_TEXT ( text, ier )
C
C*	Process filename.
C
	CALL SFMFIL  (  sffile, sffcur, iflno, newfil, pmdset, 
     +			npmdst, iret )
	IF  ( iret .ne. 0 )  proces = .false.
C
C*          Process text, marker, title, filter and parms.
C
	IF  ( proces)  THEN              
C
	        CALL GSMRKR ( marker, 0, size, mrkwid, ier )
		CALL IN_FILT ( filter, filtfc, ier )
C
C*		Process parameter names.
C
C		CALL SFMPRM  ( newfil, sfparm, pmdset, npmdst, 
C    +			       prmlst, chrflg, ncprm, prcons,
C    +			       wndflg, ier )
		CALL SFMPRM ( sfparm, pmdset, npmdst, colors,
     +                        prmlst, chrflg, ncprm, prcons,
     +                        wndflg, icolor, 
     +                        ccvals, icclrs, numccc, icrprm, 
     +			      tsize, jwide,
     +                        iaddcl, endflg, ier )
C
C*		Determine whether any data will be plotted.
C
		IF   ( (ncprm .eq. 0) .and. (imark .eq. 0) ) THEN
	             plot = .false.
		  ELSE
	             plot = .true.
		END IF
C
C*		Translate input colors.
C
c		IF ( ncprm .gt. 0 )
c     +              CALL SFMCLR ( ncprm, prmlst, colors, icolor, ier )
		IF ( pmarker .gt. 0 )  imark = icolor(1)
C
C*              Get offsets for filtering.
C
		IF  ( ( filtfc .ne. 0. ) .and. plot ) 
     +             CALL SFMCOF ( ncprm - iaddcl, prmlst, wndflg,
     +			    	 filtfc, offset, ier )
C
C*		Take care of the special case of plotting a list of 
C*		stations before plotting an area with the filter on.
C
		ipos2 = INDEX ( area, '/' )
		IF  ( area(1:1) .eq. '@' .and. ( ipos2 .gt. 4 ) ) THEN
		    area1 = area(:ipos2-1)
		    area2 = area(ipos2+1:)
		    iloop = 1
		  ELSE
		    area1 = area
		    iloop = 2
		END IF
C
C*		Convert the garea to a broader data area
C
		CALL get_data_area ( area1, iret )
C
		CALL LC_UARE  ( area1, newfil, iflno, arecur, tstn, 
     +		  	        ier )
		IF  ( ier .ne. 0 )  proces = .false.
	END IF
C
C*	    Begin processing if inputs are ok. 
C
		nplot = 0
	        CALL SF_STIM  ( iflno, dattim, ier )
C
C*	        Process clear, define panel, set up filtering
C
		IF  ( proces )  THEN
C
C*		   Intialize coordinate arrays for filtering.
C
	           IF  ( ( filtfc .ne. 0. ) .and. plot )  THEN
		       DO  m = 1, LLSTFL
			   sxplt (m) = RMISSD
		           syplt (m) = RMISSD
		       END DO
		   END IF
C
C*		   For special plotting, change the area on the 
C*		   second time through.
C
		   DO  lll = iloop, 2
		     IF  ( ( lll .eq. 2 ) .and. ( iloop .eq. 1 ) )
     +							 THEN
		       CALL LC_UARE  ( area2, newfil, iflno, 
     +		      		       arecur, tstn, ier )
		       IF  ( ier .ne. 0 )  plot = .false.
		     END IF
C
C*		     Station loop.
C
		     iout = 0
		     DO  WHILE  ( plot .and. (iout .eq. 0) )
			 CALL SF_SNXT ( iflno, sta, id, slat, 
     +					slon, selv, ispri, iout )
	                 IF  ( iout .eq. 0 )  THEN
C
C*			     Get the data.
C
			     CALL SF_RDAT ( iflno, data, ihhmm, ier )
C
C*			     Increment the station counter
C
			     if ( ier .eq. 0 ) nstns = nstns + 1
C
C*			     Check for missing data and filter.
C
		             IF  ( ier .eq. 0 ) THEN
				 CALL PC_SSTN ( sta, id, slat, slon, 
     +					selv, ispri, ihhmm, 1, ier )
				 CALL PC_CMVS ( 0., 0, data, 
     +						outd, chd, ier )
			     END IF
C*
		             IF  ( ier .eq. 0 ) THEN

C*				Convert to plot coordinates.
C
                                CALL GTRANS  ( 'M', 'P', 1, slat, slon,
     +					        sx, sy, ier )
C
C*			        Filter, if requested.
C
			        IF  ( ( filtfc .ne. 0. ) .and.
     +				      ( lll .eq. 2 ) ) THEN
				   CALL SFMOVR  ( sx, sy, sxplt, syplt,
     +						  nplot, offset, ier )
C
C*				   Save x/y for no overlap.
C
				   IF  ( ier .eq. 0 )  THEN
				      nplot = nplot + 1
				      sxplt (nplot) = sx
				      syplt (nplot) = sy
				   END IF
				 ELSE IF  ( ( filtfc .ne. 0. ) ) THEN
				   nplot = nplot + 1
				   sxplt (nplot) = sx
				   syplt (nplot) = sy
		                 END IF
			     END IF
C
C*    	                     Plot if we are ok to here.
C
	                     IF  ( ier .eq. 0 )  THEN
c			       CALL SFMPLT  ( icolor, imark, sx, sy, 
c     +					      slat, slon, chrflg, 
c     +					      prmlst, ncprm, outd, chd, 
c     +					      skpmis, ier )
				CALL SFMPLT  ( icolor, sx, sy, slat,
     +                             slon, chrflg, prmlst, ncprm, outd,
     +                             chd, ccvals, icclrs, numccc,
     +                             icrprm, tsize, jwide, endflg, ier)
			       IF ( pmarker .ne. 0 )
     +                         CALL GMARK ( 'P', 1, sx, sy, ier )
C
	                     END IF
			 END IF
	             END DO
C
C*		     Create the title string.
C
		     CALL IN_TITL ( title, -3, ititl, linttl,
     +				    ttlstr, ier )
		     DO ii = 1, ncprm
			iscale (ii) = 0
		     END DO
		     IF  ( ititl .gt. 0 )  THEN
			CALL GSCOLR  ( ititl, ier )
			CALL DSCOLR  ( ititl, ioldclr, iret )
			lens = LEN ( ttlstr )
			ttlstr(lens:lens) = char(0)
			call ptitle ( ttlstr, ititle )
		     END IF
C
		   END DO
C
C	Not processing but at least plot a title.
C
		ELSE
		     CALL IN_TITL ( title, -3, ititl, linttl,
     +				    ttlstr, ier )
		     IF  ( ititl .gt. 0 )  THEN
			CALL GSCOLR  ( ititl, ier )
			CALL DSCOLR  ( ititl, ioldclr, iret )
		        call ptitle ( ttlstr, ititle )
		     END IF
	     	END IF

C
C	End the plot to flush everything out
C
        CALL SF_CLOS(iflno,iret)
C	CALL GFLUSH ( iret )
C*
	if ( verbose .gt. 0 ) call gfprinti ( 
     +  'returning from psurf - iret = ' // char(0), iret )

	RETURN
	END
