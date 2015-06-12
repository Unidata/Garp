C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	pupa.f
C*
C*	Plots upper-air data. Derived from the GEMPAK program SNMAP.
C*
C*	History:
C*
C*	11/96	COMET		Original copy
C*	02/97 J. Cowie/COMET	Cleaned up, changed title time
C*	 5/97	COMET		Added gprintf to support logging.
C*	10/97	COMET		Added nstns, call to get_data_area()
C*	11/97	COMET		Added ptitle to display clickable titles.
C*	11/97	COMET		Added call to dscolor, cleaned up.
C*
C************************************************************************


	SUBROUTINE pupa ( snfile, prminp, area, dattim, levels, vcoord,
     +                   colors, marker, title, filter, text, skpmis,
     +                   nstns, ititle, verbose, iret )

C************************************************************************
C************************************************************************
	INCLUDE		'GEMPRM.PRM'

	CHARACTER	snfile*(*), prminp*(*), area*(*), dattim*(*),
     +			levels*(*), vcoord*(*), colors*(*),marker*(*),
     +			title*(*), filter*(*), text*(*)
	INTEGER		verbose

C	LOGICAL		skpmis, clear
C*
	CHARACTER	snfcur*72, arecur*48
	CHARACTER 	pmdset (MMPARM)*4
	CHARACTER	parms (MMPARM)*4, vcuc*4
        CHARACTER	prcons (MMPARM)*16, chd (MMPARM)*8
	CHARACTER	tstn*8, sta*8, ttl*80, ttlstr*80
	CHARACTER	area1*48, area2*48, blank*2
	INTEGER		icolor (MMPARM)
	LOGICAL		proces, newfil, chrflg (MMPARM)
	LOGICAL		plot, wndflg
	REAL		offset (4), sxplt (LLSTFL), outd (MMPARM)
	REAL		syplt (LLSTFL), data (LLMXDT)
C
C*	This can be removed when lutfil is an input parameter
C
C------------------------------------------------------------------------
	iret = 0
	nstns = 0
	ioldclr = 0
	blank = ' ' // char(0)
        proces = .true.
	iopt  = 0

	if ( verbose .gt. 0 ) call gfprints (
     +      'pupa'//char(0), blank )
	if ( verbose .gt. 1 ) then
	    call gfprints ( '  snfile = ' // char(0), snfile )
	    call gfprints ( '  prminp = ' // char(0), prminp )
	    call gfprints ( '  area = ' // char(0), area )
	    call gfprints ( '  dattim = ' // char(0), dattim )
	    call gfprints ( '  levels = ' // char(0), levels )
	    call gfprints ( '  vcoord = ' // char(0), vcoord )
	    call gfprints ( '  colors = ' // char(0), colors )
	    call gfprints ( '  marker = ' // char(0), marker )
	    call gfprints ( '  title = ' // char(0), title )
	    call gfprints ( '  filter = ' // char(0), filter )
	    call gfprints ( '  text = ' // char(0), text )
	    call gfprinti ( '  skpmis = ' // char(0), int(skpmis) )
	    call gfprinti ( '  ititle = ' // char(0), ititle )
	    call gfprinti ( '  verbose = ' // char(0), verbose )
	    call gfprinti ( '  iret = ' // char(0), iret )
	end if

C
C	Indicate that file is closed.
C
        iflno  = 0
	snfcur = ' '
	CALL LV_DECD ( levels, rlevel, ier )
	CALL ST_LCUC ( vcoord, vcuc, ier )
	CALL LV_CORD ( vcuc, vcuc, lvert, ier )
C*
	    IF ( proces ) THEN
C
C*		Set text.
C
	    	CALL IN_TEXT ( text, ier )
C
		IF ( iret .eq. 0 ) CALL SNMFIL ( snfile, snfcur,
     +						 iflno, newfil, pmdset, 
     +			        		 npmdst, ivert, iret )
		IF  ( iret .ne. 0 )  proces = .false.
	    END IF
	    IF  ( proces )  THEN
	    	CALL IN_MARK ( marker, imark, ier )
		CALL IN_FILT ( filter, filtfc, ier )
C
C*	   	Process parameter names.
C
	    	CALL SNMPRM ( newfil, prminp, pmdset, npmdst, 
     +			      parms, chrflg, ncprm,
     +			      prcons, wndflg, iret )
C
C*	  	Determine whether any data will be plotted.
C
		IF  ( ( ncprm .eq. 0 ) .and. ( imark .eq. 0 ) )  THEN
                        plot = .false.
                  ELSE
                        plot = .true.
                END IF
		IF  (  ncprm .gt. 0  )
     +		     CALL SNMCLR (ncprm, parms, colors, icolor, ier)
	    END IF
C
C*	    Get offsets for filtering
C
	    IF  ( proces )  THEN
	        IF  ( ( filtfc .ne. 0. ) .and. plot ) 
     +	             CALL SNMCOF ( ncprm, parms, wndflg, 
     +			           filtfc, offset, ier )	
C
C*	    Set area.
C
C	    IF  ( proces )  THEN
	    	ipos2 = INDEX ( area, '/' )
		IF ( area (1:1) .eq. '@' .and. ( ipos2 .gt. 4 ) ) THEN
		    area1 = area ( :ipos2-1 )
		    area2 = area ( ipos2+1: )
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
C*		Set the data area
C
		CALL LC_UARE  ( area1, newfil, iflno, arecur, tstn, 
     +			        ier )
		IF  ( ier .ne. 0 )  proces = .false.
C*
	    END IF
C
C*	    Begin processing if inputs are ok.
C
	    IF  ( proces )  THEN
C
	      ipass = 1
C
	        nplot = 0
	        CALL SN_STIM  ( iflno, dattim, ier )
C
C*	          Process clear, define panel, set up
C*		  filtering and draw map.
C
		    IF  ( ( filtfc .ne. 0. ) .and. plot )  THEN
		        DO  m = 1, LLSTFL
		          sxplt (m) = RMISSD
			  syplt (m) = RMISSD
		        END DO
		    END IF
C
C*		    For special plotting, change the area on the 
C*		    second time through.
C
		    DO  lll = iloop, 2
		      IF (( lll .eq. 2 ) .and. ( iloop .eq. 1 )) THEN
			  CALL LC_UARE ( area2, newfil, iflno,
     +					 arecur, tstn, ier )
			  IF  ( ier .ne. 0 ) plot = .false.
		      END IF
C
C*		      Station loop.
C
		      iout = 0
		      DO WHILE  ( plot .and. ( iout .eq. 0 )  )
		        CALL SN_SNXT ( iflno, sta, id, slat, 
     +			               slon, selv, iout )
	                IF  ( iout .eq. 0 )  THEN
C
C*			  Get the data.
C
			  CALL SN_RDAT  ( iflno, numlev, data, 
     +			                  ihhmm, ier )
C
C*			  Increment the station counter
C
			  if ( ier .eq. 0 ) nstns = nstns + 1
C
C*			  Filter, first parm filter and 
C*			  second sta filter, if requested.
C
			  IF  ( ier .eq. 0 )  THEN
			  
			      CALL PC_SSTN ( sta, id, slat, slon, 
     +				   selv, ispri, ihhmm, numlev, ier )
			      CALL PC_CMVS ( rlevel, lvert, data, 
     +				             outd, chd, ier )
			      IF  ( ier .eq. 0 )  THEN
				CALL GTRANS ( 'M', 'P', 1, slat,
     +					       slon, sx, sy, ier2 )
			      END IF
			  END IF
C*
			  IF  ( ( ier .eq. 0 ) .and. ( filtfc .ne. 0. ) 
     +				.and. ( lll .eq. 2 ) )  THEN
			    CALL SNMOVR  ( sx, sy, sxplt, syplt,
     +				             nplot, offset, ier )
			  END IF
			  IF ( ier .eq. 0 .and. filtfc .ne. 0. )  THEN
C
C*			     Save x/y for no overlap.
C
			     nplot = nplot + 1
			     sxplt (nplot) = sx
			     syplt (nplot) = sy
			  END IF
C
C*    	                  Plot if we are ok to here.
C
	                  IF  ( ier .eq. 0 )  THEN
			    CALL SNMPLT ( icolor, parms, sx, sy,
     +					  slat, slon, chrflg, ncprm,
     +					  outd, chd, ier ) 
	                  END IF
			END IF
		      END DO
C
C*		      Create and draw the title.
C
		      CALL IN_TITL ( title, -3, ititl, linttl,
     +				     ttlstr, ier )
		      IF  ( ititl .gt. 0 )  THEN
			CALL GSCOLR  ( ititl, ier )
                        CALL DSCOLR  ( ititl, ioldclr, iret)
			lens = LEN ( ttlstr )
			ttlstr(lens:lens) = char(0)
			call ptitle ( ttlstr, ititle )
		      END IF
C
		    END DO

C
C       Not processing but at least plot a title.
C
                ELSE
                     CALL IN_TITL ( title, -3, ititl, linttl,
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
	IF  ( iflno .gt. 0 )  CALL SN_CLOS  ( iflno, ier )
C	CALL GFLUSH  ( iret )

C*
	if ( verbose .gt. 0 ) call gfprinti (
     +     'returning from pupa - iret = ' // char(0), iret )

	RETURN
	END
