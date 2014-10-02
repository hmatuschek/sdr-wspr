!-------------------------------------------------------------------------------
!
! This file is part of the WSPR application, Weak Signal Propogation Reporter
!
! File Name:    grid2deg.f90
! Description:  Converts Maidenhead grid locator to degrees of West longitude
!               and North latitude.
!
! Copyright (C) 2001-2014 Joseph Taylor, K1JT
! License: GPL-3+
!
! This program is free software; you can redistribute it and/or modify it under
! the terms of the GNU General Public License as published by the Free Software
! Foundation; either version 3 of the License, or (at your option) any later
! version.
!
! This program is distributed in the hope that it will be useful, but WITHOUT
! ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
! FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
! details.
!
! You should have received a copy of the GNU General Public License along with
! this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
! Street, Fifth Floor, Boston, MA 02110-1301, USA.
!
!-------------------------------------------------------------------------------
subroutine grid2deg(grid0,dlong,dlat)

! Converts Maidenhead grid locator to degrees of West longitude
! and North latitude.

  character*6 grid0,grid
  character*1 g1,g2,g3,g4,g5,g6

  grid=grid0
  i=ichar(grid(5:5))
  if(grid(5:5).eq.' ' .or. i.le.64 .or. i.ge.128) grid(5:6)='mm'

  if(grid(1:1).ge.'a' .and. grid(1:1).le.'z') grid(1:1)=            &
       char(ichar(grid(1:1))+ichar('A')-ichar('a'))
  if(grid(2:2).ge.'a' .and. grid(2:2).le.'z') grid(2:2)=            &
       char(ichar(grid(2:2))+ichar('A')-ichar('a'))
  if(grid(5:5).ge.'A' .and. grid(5:5).le.'Z') grid(5:5)=            &
       char(ichar(grid(5:5))-ichar('A')+ichar('a'))
  if(grid(6:6).ge.'A' .and. grid(6:6).le.'Z') grid(6:6)=            &
       char(ichar(grid(6:6))-ichar('A')+ichar('a'))

  g1=grid(1:1)
  g2=grid(2:2)
  g3=grid(3:3)
  g4=grid(4:4)
  g5=grid(5:5)
  g6=grid(6:6)

  nlong = 180 - 20*(ichar(g1)-ichar('A'))
  n20d = 2*(ichar(g3)-ichar('0'))
  xminlong = 5*(ichar(g5)-ichar('a')+0.5)
  dlong = nlong - n20d - xminlong/60.0
  nlat = -90+10*(ichar(g2)-ichar('A')) + ichar(g4)-ichar('0')
  xminlat = 2.5*(ichar(g6)-ichar('a')+0.5)
  dlat = nlat + xminlat/60.0

  return
end subroutine grid2deg
