C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	gresize.f
C*
C*	Window resizing function.
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*	10/97	COMET	Subtract 1 from the width and height.
C*
C************************************************************************


      SUBROUTINE GRESIZE  ( iwidth, iheight, iret )
C************************************************************************
C* GCLEAR								*
C* 									*
C* This subroutine resets the size of  the current device.          	*
C* 									*
C* GCLEAR  ( IWIDTH, IHEIGHT, IRET )					*
C* 									*
C* Output parameters:							*
C* 	IRET		INTEGER		Return code			*
C**									*
C* Log:									*
C************************************************************************
	INCLUDE 	'ERROR.PRM'
	INCLUDE 	'DEVCHR.CMN'
	INCLUDE		'XYDEF.CMN'
C------------------------------------------------------------------------
	iret = NORMAL
	IF  ( ddev .ne. ' ' )  THEN
C
C*	Flush the buffer before clearing the device.
C
C	    CALL GFLUSH  ( ier )
C	    CALL DCLEAR  ( iret )
C
C*	    If the return code from DCLEAR indicates that the size has
C*	    changed, set the new size.
C
C	    IF  ( iret .eq. NWSIZE )  THEN
		iret = NORMAL
C		CALL DQDCHR  ( nncolr, ier )
C
C*		Move the bounds into the XYDEF common area.
C*		Then calculate all the coordinate information.
C
C		ixbndl = ileft

		iybndb = iheight - 1
		ixbndr = iwidth  - 1

		ibot   = iybndb
		iright = ixbndr

C		iybndt = itop
C
C*		Update transformation information.
C
		CALL UPDDXY
C	    END IF
	END IF
C*
	RETURN
	END
