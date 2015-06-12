C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	colorbardraw.f
C*
C*	Utility for drawing the image colorbar.
C*
C*	History:
C*
C*	06/97	COMET	Original copy
C*	10/97	COMET	Get image common block items from function calls
C*	 3/98	COMET	Draw color bar only if num levels > 0
C*	 3/04	Unidata	Updated call to im_cbar for GEMPAK 5.7
C*
C************************************************************************
	SUBROUTINE colorbardraw  ( cbar )
C*
C*	This subroutine calls the routine that draws the colorbar.
C*	It prepares data values in some cases, specifically for
C*	satellite imagery. (Radar data levels are calculated when the
C*	header is read, since those values are part of the image
C*	product.)

C
	INCLUDE		'GEMPRM.PRM'
C*
	character	cbar*(*)
	CHARACTER	cmbunt*8, cmcalb*8, cmblev*8
	INTEGER		idcols(256)
	REAL		PR_TMKC
	INTEGER		GETIMGCOMINT
C------------------------------------------------------------------------
C
C*	For sat images, create the data level labels.
C
	imradf = GETIMGCOMINT ( 'imradf' )
	immxpx = GETIMGCOMINT ( 'immxpx' )
	immnpx = GETIMGCOMINT ( 'immnpx' )
	imtype = GETIMGCOMINT ( 'imtype' )
	imbank = GETIMGCOMINT ( 'imbank' )
	imndlv = GETIMGCOMINT ( 'imndlv' )
	CALL GETIMGCOMSTR ( 'cmcalb', 0, cmcalb )
	CALL GETIMGCOMSTR ( 'cmbunt', 0, cmbunt )
	
	IF ( imradf .eq. 0 ) THEN
	    imndlv = MIN ( 256, immxpx - immnpx + 1 )
	    CALL SETIMGCOMINT ( 'imndlv', imndlv )
C
C*	    For IR images calibrated to brightness, create Deg C levels
C
	    IF ( cmcalb .eq. 'BRIT' .and. 
     +		(imtype .eq. 8 .or. imtype .eq. 128) ) THEN
	        cmbunt = 'DEG C'
		CALL SETIMGCOMSTR ( 'cmbunt', i, cmbunt )
		DO i = 1, imndlv
		    IF ( i .eq. 1 .or. i .eq. imndlv ) THEN
			CALL IM_BTOT ( 1, i-1+immnpx, temp, ier )
			temp = PR_TMKC ( temp )
		        CALL ST_RLCH ( temp, 1, cmblev, ier)
		    ELSE
		    	cmblev = ' '
		    END IF
		    CALL SETIMGCOMSTR ( 'cmblev', i, cmblev )
		END DO
		DO temp = -100., 50., 10.
		    CALL IM_TTOB ( 1, PR_TMCK ( temp ), brit, ier )
                    ibrit = NINT ( brit ) - immnpx
		    IF ( ibrit .gt. 0 ) THEN
     		        CALL ST_INCH ( INT (temp), cmblev, ier )
		    	CALL SETIMGCOMSTR ( 'cmblev', ibrit, cmblev )
 		    END IF
		END DO
	    ELSE IF (cmcalb .eq. 'PROD' ) THEN
	        imndlv = 256
	        CALL SETIMGCOMINT ( 'imndlv', imndlv )
C
C*	    For anything else, create labels in native units
C
	    ELSE
C
C*		GINI calbar values done in IM library
C
	        IF (cmcalb .ne. 'GINI' ) THEN
	           cmbunt = cmcalb
		   CALL SETIMGCOMSTR ( 'cmbunt', i, cmbunt )
	           ratio = (immxpx - immnpx)/255.
		   DO i = 1, imndlv
		      level = nint( (i-1) * ratio) + immnpx
		      IF ( (mod ( i-1,16) .eq. 0) .or. (i .eq. 1) .or.
     +			 (i .eq. imndlv) ) THEN
		    	 CALL ST_INCH ( level, cmblev, ier )
		      ELSE
		         cmblev = ' '
		      END IF
		      CALL SETIMGCOMSTR ( 'cmblev', i, cmblev )	    
		   END DO
		ENDIF
	    END IF
	END IF
C
C*	Create data level numbers for color bar
C
	CALL GQCLRS ( imbank, ncolr, ier )
	ratio = FLOAT (ncolr - 1) / (imndlv - 1)
	DO ic = 1, imndlv
	    idcols (ic) = (ic - 1) * ratio + .5
	END DO		   
C
C*	Draw color bar
C
 	if ( imndlv .gt. 0 ) CALL IM_CBAR ( cbar, ier )
C
	RETURN
	END
