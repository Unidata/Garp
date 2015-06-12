C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	gqsatm.f
C*
C*	Gets the GOES or GVAR navigation blocks from GEMPLT
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*	10/97	COMET	Modified to use function calls to get image
C*			common block values
C*
C************************************************************************


	SUBROUTINE GQSATM  ( proj, iarea, inav, ilef, itop, irit,
     +			     ibot, iret )
C************************************************************************
C* GQSATM								*
C*									*
C* This subroutine returns the MCIDAS GOES/GVAR image navigation infor-	*
C* mation.								*
C*									*
C* GQSATM  ( PROJ, IAREA, INAV, ILEF, ITOP, IRIT, IBOT, IRET )		*
C*                                                                      *
C* Input parameters:                                                    *
C*      PROJ            CHAR*           Satellite proj name (GOES/GVAR) *
C*									*
C* Output parameters:							*
C*      IAREA           INTEGER(*)      MCIDAS AREA DIR                 *
C*      INAV            INTEGER(*)      MCIDAS NAV block                *
C*      ILEF            INTEGER         Left image coordinate           *
C*      ITOP            INTEGER         Top image coordinate            *
C*      IRIT            INTEGER         Right image coordinate          *
C*      IBOT            INTEGER         Bottom image coordinate         *
C*	IRET		INTEGER		Return code			*
C*					  0 = normal return		*
C**									*
C* Log:									*
C* J. Cowie/COMET	 1/96						*
C************************************************************************
	INCLUDE		'GEMPRM.PRM'
	INCLUDE		'AREAFL.CMN'
C*
	INTEGER		iarea (64), inav (640)	
	INTEGER		GETIMGCOMINT
C------------------------------------------------------------------------
	iret = 0
C
C*	Copy AREA DIR and NAV blocks into arrays
C
	DO i = 1, 64
	    iarea (i) = iadir (i)
	END DO
C
	DO i = 1, 640
	    inav (i) = ianav (i)
	END DO
C
C*	Get image coord bounds
C

	ilef = GETIMGCOMINT ( 'imleft' )
	itop = GETIMGCOMINT ( 'imtop' )
	irit = GETIMGCOMINT ( 'imrght' )
	ibot = GETIMGCOMINT ( 'imbot' )
C
	RETURN
	END
