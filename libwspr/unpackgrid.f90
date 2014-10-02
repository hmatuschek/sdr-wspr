!-------------------------------------------------------------------------------
!
! This file is part of the WSPR application, Weak Signal Propogation Reporter
!
! File Name:    unpackgrid.f90
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
subroutine unpackgrid(ng,grid)

  parameter (NGBASE=180*180)
  character grid*4,grid6*6,digit*10
  data digit/'0123456789'/

  grid='    '
  if(ng.ge.32400) go to 10
  dlat=mod(ng,180)-90
  dlong=(ng/180)*2 - 180 + 2
  call deg2grid(dlong,dlat,grid6)
  grid=grid6(1:4) !XXX explicitly truncate this -db
  go to 100

10 n=ng-NGBASE-1
  if(n.ge.1 .and.n.le.30) then
     grid(1:1)='-'
     grid(2:2)=char(48+n/10)
     grid(3:3)=char(48+mod(n,10))
  else if(n.ge.31 .and.n.le.60) then
     n=n-30
     grid(1:2)='R-'
     grid(3:3)=char(48+n/10)
     grid(4:4)=char(48+mod(n,10))
  else if(n.eq.61) then
     grid='RO'
  else if(n.eq.62) then
     grid='RRR'
  else if(n.eq.63) then
     grid='73'
  endif

100 return
end subroutine unpackgrid

