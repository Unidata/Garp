C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	dgfclos.f
C*
C*	GEMPAK file closing function. Necessary to prevent conflicting
C*	open files.
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*	04/97	COMET	Modified to clear additional DGCMN items
C*			(ifiled, gparmd)
C*
C************************************************************************


	SUBROUTINE DG_FCLOS  ( iret )
C************************************************************************
C* DG_OFIL								*
C*									*
C* This subroutine forces all open grid files to be closed and clears	*
C* out any cached information.						*
C* This routine was written to work around a lun caching problem that	*
C* happens when sfmap, snmap and gdcntr are all included in the same	*
C* process.								*
C*									*
C* DG_FCLOS  ( IRET )							*
C*									*
C* Input parameters:							*
C*									*
C* Output parameters:							*
C*	IRET		INTEGER		Return code			*
C*					  0 = normal return		*
C* Log:									*
C* D. Himes/COMET	3/96	New
C************************************************************************
	INCLUDE		'DGCMN.CMN'
C------------------------------------------------------------------------

	iret   = 0

	DO  i = 1, MMFILE

		IF  ( idflnm (i) .gt. 0 )  THEN

			CALL GD_CLOS  ( idflnm (i), ier )
			idflnm (i) = 0
			tfirst (i) = ' '
			tlast  (i) = ' '
			gdcur  (i) = ' '

		END IF
	END DO

C
C*	Clear out the cached internal grid info. Guessing that this
C*	is sufficient...
C
	DO i = 1, maxdgg

		ifiled ( i ) = 0
		gparmd ( i ) = ' '

	END DO	
	
	dgset = .false.

	RETURN
	END 
