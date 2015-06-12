C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	gemimgcommon.f
C*
C*	Utility for getting and setting GEMPAK image common block
C*	variables.
C*
C*	NOTE: This module will need to be updated if the GEMPAK image
C*	common block (IMGDEF.CMN) changes in any way.
C*
C*	History:
C*
C*	06/97	COMET	Original copy
C*
C************************************************************************
C
C
	INTEGER FUNCTION GETIMGCOMINT ( item )
C***********************************************************************
C
C* This subroutine returns the value of an INTEGER item from the
C* GEMPAK image common block.
C*
C* Input parameters:
C*	ITEM		CHAR*(*)	Name of desired item
C*
C* Returned value:
C*	GETIMGCOMINT	INTEGER		Returned value or -9999 for
C*					invalid item
C*	
C***********************************************************************
C
	INCLUDE		'IMGDEF.CMN'
C
	CHARACTER*(*)	item
c	INTEGER		GETIMGCOMINT
C
C-----------------------------------------------------------------------
C
	IF ( item (1:6) .eq. 'imftyp' ) THEN
	    GETIMGCOMINT = imftyp
	ELSE IF ( item (1:6) .eq. 'imbank' ) THEN
	    GETIMGCOMINT = imbank
	ELSE IF ( item (1:6) .eq. 'imdoff' ) THEN
	    GETIMGCOMINT = imdoff
	ELSE IF ( item (1:6) .eq. 'imldat' ) THEN
	    GETIMGCOMINT = imldat
	ELSE IF ( item (1:6) .eq. 'imnpix' ) THEN
	    GETIMGCOMINT = imnpix
	ELSE IF ( item (1:6) .eq. 'imnlin' ) THEN
	    GETIMGCOMINT = imnlin
	ELSE IF ( item (1:6) .eq. 'imdpth' ) THEN
	    GETIMGCOMINT = imdpth
	ELSE IF ( item (1:6) .eq. 'imleft' ) THEN
	    GETIMGCOMINT = imleft
	ELSE IF ( item (1:5) .eq. 'imtop'  ) THEN
	    GETIMGCOMINT = imtop
	ELSE IF ( item (1:6) .eq. 'imrght' ) THEN
	    GETIMGCOMINT = imrght
	ELSE IF ( item (1:5) .eq. 'imbot'  ) THEN
	    GETIMGCOMINT = imbot
	ELSE IF ( item (1:6) .eq. 'imbswp' ) THEN
	    GETIMGCOMINT = imbswp
	ELSE IF ( item (1:6) .eq. 'imnchl' ) THEN
	    GETIMGCOMINT = imnchl
	ELSE IF ( item (1:6) .eq. 'imprsz' ) THEN
	    GETIMGCOMINT = imprsz
	ELSE IF ( item (1:6) .eq. 'imdcsz' ) THEN
	    GETIMGCOMINT = imdcsz
	ELSE IF ( item (1:6) .eq. 'imclsz' ) THEN
	    GETIMGCOMINT = imclsz
	ELSE IF ( item (1:6) .eq. 'imlvsz' ) THEN
	    GETIMGCOMINT = imlvsz
	ELSE IF ( item (1:6) .eq. 'imvald' ) THEN
	    GETIMGCOMINT = imvald
	ELSE IF ( item (1:6) .eq. 'imrdfl' ) THEN
	    GETIMGCOMINT = imrdfl
	ELSE IF ( item (1:6) .eq. 'immnpx' ) THEN
	    GETIMGCOMINT = immnpx
	ELSE IF ( item (1:6) .eq. 'immxpx' ) THEN
	    GETIMGCOMINT = immxpx
	ELSE IF ( item (1:6) .eq. 'imsorc' ) THEN
	    GETIMGCOMINT = imsorc
	ELSE IF ( item (1:6) .eq. 'imtype' ) THEN
	    GETIMGCOMINT = imtype
	ELSE IF ( item (1:6) .eq. 'imradf' ) THEN
	    GETIMGCOMINT = imradf
	ELSE IF ( item (1:6) .eq. 'imdate' ) THEN
	    GETIMGCOMINT = imdate
	ELSE IF ( item (1:6) .eq. 'imtime' ) THEN
	    GETIMGCOMINT = imtime
	ELSE IF ( item (1:6) .eq. 'imndlv' ) THEN
	    GETIMGCOMINT = imndlv
	ELSE
	    GETIMGCOMINT = -9999
	ENDIF
C
	RETURN
	END
C
C*
C
	REAL FUNCTION GETIMGCOMFLT ( item )
C***********************************************************************
C
C* This subroutine returns the value of a FLOAT item from the
C* GEMPAK image common block
C*
C* Input parameters:
C*	ITEM		CHAR*(*)	Name of desired item
C*
C* Output Paramaters:
C*	GETIMGCOMFLT	REAL		Returned value or -9999 for
C*					invalid item
C*	
C***********************************************************************
C
	INCLUDE		'IMGDEF.CMN'
C
	CHARACTER*(*)	item
c	REAL		GETIMGCOMFLT
C
C-----------------------------------------------------------------------
C
	IF ( item (1:6) .eq. 'rmxres' ) THEN
	    GETIMGCOMFLT = rmxres
	ELSE IF ( item (1:6) .eq. 'rmyres' ) THEN
	    GETIMGCOMFLT = rmyres
	ELSE IF ( item (1:6) .eq. 'rmxysc' ) THEN
	    GETIMGCOMFLT = rmxysc
	ELSE IF ( item (1:6) .eq. 'rmbelv' ) THEN
	    GETIMGCOMFLT = rmbelv
	ELSE
	    GETIMGCOMFLT = -9999.
	ENDIF
C
	RETURN
	END
C
C*
C
	INTEGER FUNCTION GETIMGCOMSTR ( item, index, cval )
C***********************************************************************
C
C* This subroutine returns the value of a STRING item from the
C* GEMPAK image common block
C*
C* Input parameters:
C*	ITEM		CHAR*(*)	Name of desired item
C*	INDEX		INTEGER		For array items, the index
C*
C* Output Parameters:
C*	CVAL		CHAR*(*)	Value of string
C*	GETIMGCOMSTR	INTEGER		Length of string					
C*	
C***********************************************************************
C
	INCLUDE		'IMGDEF.CMN'
C
	CHARACTER*(*)	item, cval
c	INTEGER		GETIMGCOMSTR
C
C-----------------------------------------------------------------------
C	
	IF ( item (1:6) .eq. 'cmblev' ) THEN
	    cval = cmblev (index)
	ELSE IF ( item (1:6) .eq. 'cmbunt' ) THEN
	    cval = cmbunt
	ELSE IF ( item (1:6) .eq. 'cmsorc' ) THEN
	    cval = cmsorc
	ELSE IF ( item (1:6) .eq. 'cmtype' ) THEN
	    cval = cmtype
	ELSE IF ( item (1:6) .eq. 'cmlutf' ) THEN
	    cval = cmlutf
	ELSE IF ( item (1:6) .eq. 'cmfile' ) THEN
	     cval= cmfile
	ELSE IF ( item (1:6) .eq. 'cmcalb' ) THEN
	    cval = cmcalb
	ELSE
	    cval = ' '
	ENDIF
C
	CALL ST_LSTR ( cval, GETIMGCOMSTR, ier )
	RETURN
	END
C
C*
C
	INTEGER FUNCTION SETIMGCOMINT ( item, ival )
C***********************************************************************
C
C* This subroutine sets the value of an INTEGER item in the
C* GEMPAK image common block.
C*
C* Input parameters:
C*	ITEM		CHAR*(*)	Name of desired item
C*	IVAL		INTEGER		Value of item
C*
C* Returned value:
C*	GETIMGCOMINT	INTEGER		0:     Success
C*					-9999: Invalid item
C*	
C***********************************************************************
C
	INCLUDE		'IMGDEF.CMN'
C
	CHARACTER*(*)	item
c	INTEGER		SETIMGCOMINT
C
C-----------------------------------------------------------------------
C
	SETIMGCOMINT = 0
C
	IF ( item (1:6) .eq. 'imftyp' ) THEN
	    imftyp = ival
	ELSE IF ( item (1:6) .eq. 'imbank' ) THEN
	    imbank = ival
	ELSE IF ( item (1:6) .eq. 'imdoff' ) THEN
	    imdoff = ival
	ELSE IF ( item (1:6) .eq. 'imldat' ) THEN
	    imldat = ival
	ELSE IF ( item (1:6) .eq. 'imnpix' ) THEN
	    imnpix = ival
	ELSE IF ( item (1:6) .eq. 'imnlin' ) THEN
	    imnlin = ival
	ELSE IF ( item (1:6) .eq. 'imdpth' ) THEN
	    imdpth = ival
	ELSE IF ( item (1:6) .eq. 'imleft' ) THEN
	    imleft = ival
	ELSE IF ( item (1:5) .eq. 'imtop'  ) THEN
	    imtop = ival
	ELSE IF ( item (1:6) .eq. 'imrght' ) THEN
	    imrght = ival
	ELSE IF ( item (1:5) .eq. 'imbot'  ) THEN
	    imbot = ival
	ELSE IF ( item (1:6) .eq. 'imbswp' ) THEN
	    imbswp = ival
	ELSE IF ( item (1:6) .eq. 'imnchl' ) THEN
	    imnchl = ival
	ELSE IF ( item (1:6) .eq. 'imprsz' ) THEN
	    imprsz = ival
	ELSE IF ( item (1:6) .eq. 'imdcsz' ) THEN
	    imdcsz = ival
	ELSE IF ( item (1:6) .eq. 'imclsz' ) THEN
	    imclsz = ival
	ELSE IF ( item (1:6) .eq. 'imlvsz' ) THEN
	    imlvsz = ival
	ELSE IF ( item (1:6) .eq. 'imvald' ) THEN
	    imvald = ival
	ELSE IF ( item (1:6) .eq. 'imrdfl' ) THEN
	    imrdfl = ival
	ELSE IF ( item (1:6) .eq. 'immnpx' ) THEN
	    immnpx = ival
	ELSE IF ( item (1:6) .eq. 'immxpx' ) THEN
	    immxpx = ival
	ELSE IF ( item (1:6) .eq. 'imsorc' ) THEN
	    imsorc = ival
	ELSE IF ( item (1:6) .eq. 'imtype' ) THEN
	    imtype = ival
	ELSE IF ( item (1:6) .eq. 'imradf' ) THEN
	    imradf = ival
	ELSE IF ( item (1:6) .eq. 'imdate' ) THEN
	    imdate = ival
	ELSE IF ( item (1:6) .eq. 'imtime' ) THEN
	    imtime = ival
	ELSE IF ( item (1:6) .eq. 'imndlv' ) THEN
	    imndlv = ival
	ELSE
	    SETIMGCOMINT = -9999
	ENDIF
C
	RETURN
	END
C
C*
C
	INTEGER FUNCTION SETIMGCOMFLT ( item, fval )
C***********************************************************************
C
C* This subroutine returns the values of FLOAT items from the
C* GEMPAK image common block
C*
C* Input parameters:
C*	ITEM		CHAR*(*)	Name of desired item
C*	FVAL		REAL		Value of item
C*
C* Output Paramaters:
C*	GETIMGCOMFLT	INTEGER		0:     Success
C*					-9999: Invalid item
C*	
C***********************************************************************
C
	INCLUDE		'IMGDEF.CMN'
C
	CHARACTER*(*)	item
c	INTEGER 	SETIMGCOMFLT
C
C-----------------------------------------------------------------------
C
	SETIMGCOMFLT = 0
C
	IF ( item (1:6) .eq. 'rmxres' ) THEN
	    rmxres = fval
	ELSE IF ( item (1:6) .eq. 'rmyres' ) THEN
	    rmyres = fval
	ELSE IF ( item (1:6) .eq. 'rmxysc' ) THEN
	    rmxysc = fval
	ELSE IF ( item (1:6) .eq. 'rmbelv' ) THEN
	    rmbelv = fval
	ELSE
	    SETIMGCOMFLT = -9999
	ENDIF
C
	RETURN
	END
C
C
	INTEGER FUNCTION SETIMGCOMSTR ( item, index, cval )
C***********************************************************************
C
C* This subroutine sets the value of a STRING item in the
C* GEMPAK image common block
C*
C* Input parameters:
C*	ITEM		CHAR*(*)	Name of desired item
C*	INDEX		INTEGER		For array items, the index
C*	CVAL		CHAR*(*)	Value of string
C*
C* Output Parameters:
C*	GETIMGCOMSTR	INTEGER		Length of string					
C*	
C***********************************************************************
C
	INCLUDE		'IMGDEF.CMN'
C
	CHARACTER*(*)	item, cval
c	INTEGER 	SETIMGCOMSTR
C
C-----------------------------------------------------------------------
C	
	SETIMGCOMSTR = 0
C
	IF ( item (1:6) .eq. 'cmblev' ) THEN
	    cmblev (index) = cval
	ELSE IF ( item (1:6) .eq. 'cmbunt' ) THEN
	    cmbunt = cval
	ELSE IF ( item (1:6) .eq. 'cmsorc' ) THEN
	    cmsorc = cval
	ELSE IF ( item (1:6) .eq. 'cmtype' ) THEN
	    cmtype = cval
	ELSE IF ( item (1:6) .eq. 'cmlutf' ) THEN
	    cmlutf = cval
	ELSE IF ( item (1:6) .eq. 'cmfile' ) THEN
	    cmfile = cval
	ELSE IF ( item (1:6) .eq. 'cmcalb' ) THEN
	    cmcalb = cval
	ELSE
	    SETIMGCOMSTR = -9999
	ENDIF
C
	RETURN
	END
C

