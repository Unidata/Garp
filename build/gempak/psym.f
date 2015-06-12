C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	psym.f
C*
C*	Plots symbols. Derived from the GEMPAK program SFMPLT.
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*	 3/97	COMET	Made chbuf larger to print station names.
C*	 5/97	COMET	Added gprintf to support logging.
C*
C************************************************************************


	SUBROUTINE psym ( stations, msize, mwidth, text, slat, slon,
     +                    nstns, lstn, marker, color, pstn, pmarker,
     +                    verbose, iret )

C************************************************************************
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
C*
	CHARACTER*(*)	stations, msize, mwidth, text
	INTEGER		color, verbose
	REAL		slat(*), slon(*)
	REAL		sx(LLSTFL), sy(LLSTFL)
	REAL		filled_circle, size
	INTEGER		marker, mrkwid, pstn, pmarker
C*
	CHARACTER	chbuf*32, blank*2, output*78
	INTEGER		ixof (10), iyof (10)
C*
	DATA		ixof  / 0, 0, 2, 2,  0,  2, 0,  0, 0,  0 /
	DATA		iyof  / 2, 0, 2, 0, -2, -2, 4, -4, 2, -2 /
C*
C------------------------------------------------------------------------
	iret   = 0
	blank = ' ' // char(0)
	filled_circle = 8.
	read ( msize, '(f5.1)' ) size
	read ( mwidth, '(i5)' ) mrkwid

	if ( verbose .gt. 0 ) call gfprints (
     +     'psurf'//char(0), blank )
	if ( verbose .gt. 1 ) then
	    call gfprinti ( ' nstns = ' // char(0), nstns )
	    call gfprinti ( ' lstn = ' // char(0), lstn )
	    call gfprintf ( ' marker = ' // char(0), marker )
	    call gfprints ( ' msize = ' // char(0), msize )
	    call gfprints ( ' mwidth = ' // char(0), mwidth )
	    call gfprinti ( ' color = ' // char(0), color )
	    call gfprintf ( ' pstn = ' // char(0), pstn )
	    call gfprintf ( ' pmarker = ' // char(0), pmarker )
	    call gfprints ( ' text = ' // char(0), text )
	    call gfprinti ( ' verbose = ' // char(0), verbose )
	    call gfprinti ( ' iret = ' // char(0), iret )
	end if

C
	CALL GSCOLR  ( color, ier )
C
C*	For marker type 0 (circle), we draw this as a sky symbol,
C*	so set it's size differently.
C
	if ( marker .eq. 0 ) then
	    CALL GSSKY ( size/2., 2, mrkwid, ier )
	else
	    CALL GSMRKR ( marker, 0, size, mrkwid, ier )
	endif
C
C*	Set text attributes
C
	CALL IN_TEXT ( text, ier )
C
C*	Convert to plot coordinates.
C
	CALL GTRANS  ( 'M', 'P', nstns, slat, slon, sx, sy, ier )
C
C*	Plot each parameter.
C
	DO  ip = 1, nstns

	ibeg = lstn * ( ip - 1 ) + 1
	iend = ibeg + lstn - 1

C
C*          Station position is one less than the element number
C
            ipos = 0
C
C*          Set size for offset
C
            isiz = 1
C
C*              Get number of characters to plot
C
		chbuf = ' '
		    chbuf = stations(ibeg:iend)
C	Delete leading blanks and tabs.

C		    CALL ST_LDSP  ( chbuf, chbuf, isiz, ier )

C	Delete extraneous blanks.

		    CALL ST_RXBL ( chbuf, chbuf, isiz, ier )
C
C*	    Set offsets for station positions and plot data if 
C*	    string is not blank.  Weather symbols are offset
C*	    in the x direction to be centered on two characters
C*	    for positions to the left and right of the station.
C*
		    IF  ( ipos .eq. 0 )  THEN
                        ix = -isiz + 1
                        ixs = 0
                        iy = 0
C*
		      ELSE IF  ( ( ipos .eq. 1 ) .or. ( ipos .eq. 2 ) 
     +				.or.  ( ipos .eq. 5 ) )  THEN
	                ix = ixof (ipos) - (isiz * 2) 
                        ixs = -3
                        iy = iyof (ipos)
C*
		      ELSE IF  ( ( ipos .eq. 3 ) .or. ( ipos .eq. 4 ) 
     +				.or.  ( ipos .eq. 6 ) )  THEN
                        ix = ixof (ipos)
                        ixs = 3
                        iy = iyof (ipos)
C*
		      ELSE IF  ( ( ipos .eq. 7 ) .or. ( ipos .eq. 8 ) 
     +				.or.  ( ipos .eq. 9 ) .or. 
     +                          ( ipos .eq. 10 ) ) THEN
                        ix = -isiz + 1
                        ixs = 0
                        iy = iyof (ipos)
	            END IF
C*
C
C*	Check map boundaries.
C
	if ( ( sx(ip) .gt. 0 .and. sx(ip) .lt. 1 ) .and. 
     +       ( sy(ip) .gt. 0 .and. sy(ip) .lt. 1 ) ) then

	ix = 0
	ixs = 0
	iy = 0

            if ( pmarker .eq. 1 ) then
		if ( marker .eq. 0 ) then
                    CALL GSKY  ( 'P', 1, filled_circle, sx(ip), sy(ip), 
     +                           ixs, iy, iret )

		else
		    CALL GMARK ( 'P', 1, sx(ip), sy(ip), ier )
		endif
	    endif
	ix = 2
	iy = 2

	    if ( pstn .eq. 1 ) then
                CALL GTEXT  ( 'P',  sx(ip), sy(ip), chbuf (1:isiz), 
     +				      0., ix, iy, iret )
	    endif
	endif
		
	if ( verbose .gt. 2 ) then
	    write (output,'(i5,1x,a,2(1x,f10.4))' )
     +                      ip, chbuf, slat(ip), slon(ip)
	    call gfprints ( '  ' // char(0), output//char(0) )
	end if

	
	END DO
C*
C	CALL GFLUSH ( iret )
C*
	if ( verbose .gt. 0 ) call gfprinti (
     +  'returning from psym - iret = ' // char(0), iret )

	RETURN
	END
