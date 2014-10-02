!-------------------------------------------------------------------------------
!
! This file is part of the WSPR application, Weak Signal Propogation Reporter
!
! File Name:    deg2grid.f90
! Description:
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
subroutine deg2grid(dlong0,dlat,grid)

  real dlong                        !West longitude (deg)
  real dlat                         !Latitude (deg)
  character grid*6

  dlong=dlong0
  if(dlong.lt.-180.0) dlong=dlong+360.0
  if(dlong.gt.180.0) dlong=dlong-360.0

! Convert to units of 5 min of longitude, working east from 180 deg.
  nlong=60.0*(180.0-dlong)/5.0
  n1=nlong/240                      !20-degree field
  n2=(nlong-240*n1)/24              !2 degree square
  n3=nlong-240*n1-24*n2             !5 minute subsquare
  grid(1:1)=char(ichar('A')+n1)
  grid(3:3)=char(ichar('0')+n2)
  grid(5:5)=char(ichar('a')+n3)

! Convert to units of 2.5 min of latitude, working north from -90 deg.
  nlat=60.0*(dlat+90)/2.5
  n1=nlat/240                       !10-degree field
  n2=(nlat-240*n1)/24               !1 degree square
  n3=nlat-240*n1-24*n2              !2.5 minuts subsquare
  grid(2:2)=char(ichar('A')+n1)
  grid(4:4)=char(ichar('0')+n2)
  grid(6:6)=char(ichar('a')+n3)

  return
end subroutine deg2grid
