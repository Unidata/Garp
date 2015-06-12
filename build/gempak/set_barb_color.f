C***********************************************************************
C*
C*	Copyright 1996, University Corporation for Atmospheric Research.
C*
C*	set_barb_color.f
C*
C*	Sets wind barb colors
C*
C*	History:
C*
C*	11/96	COMET	Original copy
C*
C************************************************************************


c  This function sets color indices of wind barbs based on:
c                              1) wind speed
c                              2) quality control flag  (not yet implemented)
c                              3) veritcal velocity     (not yet implemented)
c
      integer function set_barb_color(spd,pflag)
      integer pflag, color(10)
      real spd
c
csd      data color/5,19,10,17,12,2,15,30,29,4,25,26,27,21,22,23/
csd      data color/1,5,17,2,29,4,25,6,21/
      data color/29,4,26,6,22,21,5,10,12,2/
c
c-------------------------------------------------------------------------------
c  Input:  spd - (REAL) wind speed in current units
c          pflag - (INTEGER) logic used to color wind barbs:
c                  1=wind speed, 2=qcflag, 3=vertical velocity
c
c  Output: set_barb_color - color index of wind barb from GEMPAK color table
c-------------------------------------------------------------------------------
c
c  Use code from IBARB.F to compute number of flags, barbs, half-barbs.
C*          Compute the number of flags, whole barbs and half barbs
C*          needed to represent the wind speed.
C
            nspd  = spd + 0.5
            nspd  = MOD ( nspd, 10 )
            spd1  = spd + 2.5
            nflag = spd1 / 50.
            bbarb = (spd1 - nflag*50.) / 10.
            nbarb = bbarb
            kbarb = bbarb + 0.5
            nhbarb = 0
            IF  ( nbarb .ne. kbarb )  nhbarb = 1
c
c  Set barb color according to wind speed.
c
c    Wind speed less then 50 units.
c
      if( nflag .eq. 0 ) then
        set_barb_color = color(1)
        if( nbarb .eq. 0 .and. nhbarb .eq. 1 ) then
          set_barb_color = color(1)
        elseif ( nbarb .eq. 1 .and. nhbarb .eq. 0 ) then
          set_barb_color = color(2)
        elseif ( nbarb .eq. 1 .and. nhbarb .eq. 1 ) then
          set_barb_color = color(2)
        elseif ( nbarb .eq. 2 .and. nhbarb .eq. 0 ) then
          set_barb_color = color(3)
        elseif ( nbarb .eq. 2 .and. nhbarb .eq. 1 ) then
          set_barb_color = color(3)
        elseif ( nbarb .eq. 3 .and. nhbarb .eq. 0 ) then
          set_barb_color = color(4)
        elseif ( nbarb .eq. 3 .and. nhbarb .eq. 1 ) then
          set_barb_color = color(4)
        elseif ( nbarb .eq. 4 .and. nhbarb .eq. 0 ) then
          set_barb_color = color(5)
        elseif ( nbarb .eq. 4 .and. nhbarb .eq. 1 ) then
          set_barb_color = color(5)
        endif
c
c    Wind speed >= 50 units and < 100 units.
c
      elseif( nflag .eq. 1) then
        set_barb_color = color(6)
        if( nbarb .eq. 0 .and. nhbarb .eq. 1 ) then
          set_barb_color = color(6)
        elseif ( nbarb .eq. 1 .and. nhbarb .eq. 0 ) then
          set_barb_color = color(7)
        elseif ( nbarb .eq. 1 .and. nhbarb .eq. 1 ) then
          set_barb_color = color(7)
        elseif ( nbarb .eq. 2 .and. nhbarb .eq. 0 ) then
          set_barb_color = color(8)
        elseif ( nbarb .eq. 2 .and. nhbarb .eq. 1 ) then
          set_barb_color = color(8)
        elseif ( nbarb .eq. 3 .and. nhbarb .eq. 0 ) then
          set_barb_color = color(9)
        elseif ( nbarb .eq. 3 .and. nhbarb .eq. 1 ) then
          set_barb_color = color(9)
        elseif ( nbarb .eq. 4 .and. nhbarb .eq. 0 ) then
          set_barb_color = color(10)
        elseif ( nbarb .eq. 4 .and. nhbarb .eq. 1 ) then
          set_barb_color = color(10)
        endif
      else
c
c    Wind speed >= 100 units.
c
        set_barb_color = color(10)
      endif
c
      return
      end
