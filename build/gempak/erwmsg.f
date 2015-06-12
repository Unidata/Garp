	SUBROUTINE ER_WMSG  ( errgrp, numerr, errstr, iret )
C************************************************************************
C* ER_WMSG								*
C*									*
C* This subroutine writes an error message to the user's terminal.	*
C* The output message will contain the error group and error number 	*
C* in brackets followed by the message.  If the error file or error	*
C* number cannot be found, only the error group and number will be	*
C* written.								*
C*									*
C* The string, ERRSTR, will replace an !AS found in the message.	*
C*									*
C* The messages are stored in error files.  The message is read		*
C* from the file GEMERR:'ERRGRP'.ERR.					*
C*									*
C* ER_WMSG  ( ERRGRP, NUMERR, ERRSTR, IRET )				*
C*									*
C* Input parameters:							*
C*	ERRGRP		CHAR*		Error group			*
C*	NUMERR		INTEGER		Error number			*
C*	ERRSTR		CHAR*		String to be embedded		*
C*									*
C* Output parameters:							*
C*	IRET		INTEGER		Return code			*
C*					  3 = error number not found 	*
C*					  2 = error file not found	*
C*					  0 = normal return		*
C************************************************************************
C
C
C This copy of ER_WMSG() adds the string "GEMPAK:" to the start of the
C message and calls the routine GARPERR for the actual output.
C
C
C*************************************************************************
C
	INCLUDE		'GEMPRM.PRM'
	INCLUDE		'GMBDTA.CMN'
C*
	CHARACTER*(*)  	errgrp, errstr 
	CHARACTER*136	outmsg
C------------------------------------------------------------------------
C*      Return if error number is 0.
C
        iret = 0
        IF  ( numerr .eq. 0 )  RETURN
C
C*	Create the error message.
C
C	CALL ER_MMSG ( errgrp, numerr, errstr, outmsg, iret )
	CALL ER_MMSG ( errgrp, numerr, errstr, .false., 
     +     outmsg, iret )
C
C*	Write the error message to the terminal.
C
	outmsg(1:) = 'GEMPAK: '//outmsg(1:)
	
	CALL ST_LSTR ( outmsg, len, ier) 

	CALL GARPERR ( outmsg, len )
C*
	RETURN
	END
