#include "dg.h"
#include "underscore.h"

/*C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	gridtime.f
C*
C*	GEMPAK grid function (gfunc) utility
C*
C*	History:
C*
C*	 2/98	COMET	Original copy
C*	 3/98	COMET	look through table better
C*
C************************************************************************

C
C*	This routine parses a gfunc string and if it contains embedded
C*	modifiers (% for vcoord, @ for level, ), returns
C*	the modifier values
C*/
	void  parse_gfunc ( char *gfunc, char *level, char *vcoord, size_t int1, size_t int2, size_t int3 )

/*
C
	INCLUDE         'DGCMN.CMN'
C
	CHARACTER*(*)	gfunc, level, vcoord
	INTEGER		iret
*/
/*C
C*------------------------------------------------------------------------
C*/
{
	int i, ier;

	dg_pfun ( gfunc, &ier );

	level[0] = '\0';
	for ( i = _dgtabl.ltabl; i >=0; i-- )
           {
	   if ( _dgtabl.ctabl[i][0] != '*' && _dgtabl.ctabl[i][0] != ' ' )
              {
	      strcat(level,_dgtabl.ctabl[i]);
              }
           }

	vcoord[0] = '\0';
	for ( i = _dgtabl.ltabl; i >=0; i-- )
           {
	   if ( _dgtabl.cvcord[i][0] != '*' && _dgtabl.cvcord[i][0] != ' ' )
              {
	      strcat(vcoord,_dgtabl.cvcord[i]);
              }
           }
}
/*
C
C*	Yank the parsed values out of the common block
C
	do i = ltabl, 1, -1
	    if (ctabl (i)(1:1) .ne. '*' .and. clevel (i) .ne. ' ') then
		level = clevel ( i )
		call st_lstr ( level, len, ier )
		level (len+1:) = CHAR (0)
	    end if
	end do
C	
	do i = ltabl, 1, -1
	    if (ctabl (i)(1:1) .ne. '*' .and. cvcord (i) .ne. ' ') then
		vcoord = cvcord ( i )
		call st_lstr ( vcoord, len, ier )
		vcoord (len+1:) = CHAR (0)
	    end if
	end do

	return

	end
*/


