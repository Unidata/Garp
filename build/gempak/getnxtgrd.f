C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	getnxtgrd.f
C*
C*	Gets next GEMPAK grid from file.
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*	05/97	COMET	Fixed jvcoord, removed pagflg (Chiz/UNIDATA)
C*
C************************************************************************


	SUBROUTINE getnxtgrd  ( igdfln, igrid, time1, time2, lvl,
     +			vertcord, field, iret )


	integer		igdfln
	integer		igrid

	character*(*)	time1
	character*(*)	time2

	character*(*)	field

	character*(*)	vertcord
	integer		lvl(2)

C
C
C	Local variables
C
C
	character	time (2)*18, tmpfield*12
	integer		lev (2)
	integer		jvcord
	character	stringjv*18


	iret   = 0
	
	call gd_gidn  ( igdfln, igrid, time, lev, jvcord, tmpfield, 
     +			iret )

	call lv_ccrd ( jvcord, stringjv, iret )


C
C	Stuff the output parameters
C
	time1   = time(1)
	time2   = time(2)
	lvl(1)  = lev(1)
	lvl(2)  = lev(2)
	vertcord= stringjv
	field   = tmpfield

	return
	end
