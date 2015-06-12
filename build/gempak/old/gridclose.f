C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	gridclose.f
C*
C*	Grid file closing routine.
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*
C************************************************************************


	SUBROUTINE grid_close  ( iret )
	integer		iret

C************************************************************************
C* grid_close								*
C*									*
C* This subroutine forces an open grid file to be closed and clears	*
C* out any cached information.						*
C* This routine was written to work around a lun caching problem that	*
C* happens when sfmap, snmap and gdcntr are all included in the same	*
C* process.								*
C*									*
C* grid_close  ( IRET )							*
C*									*
C* Input parameters:							*
C*									*
C* Output parameters:							*
C*	IRET		INTEGER		Return code			*
C*					  0 = normal return		*
C* Log:									*
C* D. Himes/COMET	5/14/96	New
C************************************************************************

	CALL GD_CLOS  ( -1, iret )

	RETURN
	END 
