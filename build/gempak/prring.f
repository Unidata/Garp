C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	prring.f
C*
C*	Radar ring drawing functions.
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*	 5/97	COMET	Added gprintf to support logging.
C*	 3/98	COMET	Allow for station ids with different string lengths.
C*	 1/01	Unidata	Updated call to GCIRCL
C*
C************************************************************************
C************************************************************************

	subroutine dist ( slat, slon, distance, length, iret )

	REAL		slat, slon, distance, length
	INTEGER		iret

	REAL		meters

C* This subroutine computes the distance in N coordinate system from a
C* point outward along an arc on a sphere.

	INCLUDE		'GEMPRM.PRM'

	meters = distance * 1000.

C*	Compute arc endpoints.

	theta = RTD * meters / RADIUS

	rlon = slon
	rlat = slat + theta

C*	Convert endpoint positions to normalize coordinates.

	CALL GTRANS  ( 'M', 'N', 1, slat, slon, sx, sy, ier )
	CALL GTRANS  ( 'M', 'N', 1, rlat, rlon, rx, ry, ier )

	dx = sx - rx
	dy = sy - ry

C*	Determine arc length in normalize coordinates.

	length = SQRT ( dx**2 + dy**2 )

	return
	end

C************************************************************************

	SUBROUTINE prring ( nstns, stations, stid, slat, slon, 
     +                      color, nrings, rincr, rmin, units, 
     +                      verbose, iret )

	INCLUDE		'GEMPRM.PRM'
C*
	CHARACTER*(*)	stations, stid, units
	INTEGER		color, verbose
	REAL		slat(*), slon(*)

	CHARACTER	stn*8, blank*2, buf*40, output*60
	REAL		sx(LLSTFL), sy(LLSTFL)
	REAL		kmpermi, kmpernm
C*
	INTEGER		ixof (10), iyof (10)
C*
	DATA		ixof  / 0, 0, 2, 2,  0,  2, 0,  0, 0,  0 /
	DATA		iyof  / 2, 0, 2, 0, -2, -2, 4, -4, 2, -2 /
	DATA		kmpermi, kmpernm / 1.609, 1.85 / 
C
C*	Here, we are assuming that the character 'stations' is a single
C*	string containing a list of station ids. Each id is 3 
C*	characters long and separated by a single space.
C
C*
C------------------------------------------------------------------------
	iret = 0
	blank = ' ' // char(0)

	if ( verbose .gt. 0 )
     +         call gfprints ( 'prring'//char(0), blank )
	if ( verbose .gt. 1 ) then
	    call gfprinti ( ' nstns = ' // char(0), nstns )
	    call gfprints ( ' stations = ' // char(0), stations )
	    call gfprints ( ' stid = ' // char(0), stid )
	    call gfprinti ( ' color = ' // char(0), color )
	    call gfprinti ( ' nrings = ' // char(0), nrings )
	    call gfprintf ( ' rincr = ' // char(0), rincr )
	    call gfprintf ( ' rmin = ' // char(0), rmin )
	    call gfprints ( ' units = ' // char(0), units )
	    call gfprinti ( ' iret = ' // char(0), iret )
	end if

	if ( verbose .gt. 2 ) then
	    do i=1, nstns
		write (buf,'(i8,2(1x,f10.4))' ) i, slat(i), slon(i)
		output = '  i, lat, lon = ' // buf
		call gfprints ( '  ' // char(0), output//char(0) )
	  end do
	end if

C
C*	Set color.
C
	CALL GSCOLR  ( color, ier )
C
C*	"Force" the color to be set. The preceeding GSCOLR sets the new
C*	color in a common block and subsequent calls to plotting routines
C*	calls DSCOLR which changes the color in the device. But the GCIRCL
C*	routine called below does not do this (bug) so, calling DSCOLR
C*	will force the color to change right now. If this is ever fixed
C*	in GEMPAK, then this call to DSCOLR can be removed.
C
	CALL DSCOLR ( color, icol, ier )
C
C*	Convert to plot coordinates.
C
	CALL GTRANS  ( 'M', 'P', nstns, slat, slon, sx, sy, ier )
C
C*	Plot radar rings.
C
	ibeg = 1
	iend = ibeg

	do i=1, nstns
C
C*	Get the next station.
C
	    do while ( stations(iend:iend) .ne. ' ' )
		iend = iend + 1
	    end do
	    len = iend - ibeg

	    stn = stations(ibeg:iend)

C
C*	If only one station is to be plotted check for that station.
C
	    if ( stid .eq. 'ALL' .or. 
     +	       ( stid .ne. 'ALL' .and.
     +           stn(1:len) .eq. stid ) ) then


	    	do j=1, nrings
C
C*	Check map boundaries.
C
	            if ( ( sx(i) .gt. 0 .and. sx(i) .lt. 1 ) .and. 
     +                   ( sy(i) .gt. 0 .and. sy(i) .lt. 1 ) ) then

		        drad = rincr * ( j-1 ) + rmin

C		Chiz/Unidata changed this, GCIRCL change in 5.6
C	                CALL DIST ( slat(i), slon(i), drad, rlen,
C     +                                                       iret )
C		        if ( units .eq. 'mi' ) rlen = kmpermi * rlen
C		        if ( units .eq. 'nm' ) rlen = kmpernm * rlen
C
C		        CALL GCIRCL ( 'M', slat(i), slon(i), rlen,
C     +                           1, iret )

			rlen = drad
			if ( units .eq. 'mi' ) rlen = kmpermi * rlen
			if ( units .eq. 'nm' ) rlen = kmpernm * rlen
		        CALL GCIRCL ( 'M', slat(i), slon(i), 
     +				slat(i) + (rlen / 111.111), slon(i),
     +                           10, iret )
		endif

	    enddo
	    endif
C
C*	Got to the start of the next station.
C
	    do while ( stations(iend:iend) .eq. ' ' )
		iend = iend + 1
	    end do
	    ibeg = iend
	enddo
C*
C	CALL GFLUSH ( iret )
C*
	if ( verbose .gt. 0 ) call gfprinti ( 
     +  'returning from prring - iret = '//char(0), iret )
	RETURN
	END
