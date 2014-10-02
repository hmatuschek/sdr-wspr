!-------------------------------------------------------------------------------
!
! This file is part of the WSPR application, Weak Signal Propogation Reporter
!
! File Name:    unpack50.f90
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
subroutine unpack50(dat,n1,n2)

  integer*1 dat(11)

  i=dat(1)
  i4=iand(i,255)
  n1=ishft(i4,20)
  i=dat(2)
  i4=iand(i,255)
  n1=n1 + ishft(i4,12)
  i=dat(3)
  i4=iand(i,255)
  n1=n1 + ishft(i4,4)
  i=dat(4)
  i4=iand(i,255)
  n1=n1 + iand(ishft(i4,-4),15)
  n2=ishft(iand(i4,15),18)
  i=dat(5)
  i4=iand(i,255)
  n2=n2 + ishft(i4,10)
  i=dat(6)
  i4=iand(i,255)
  n2=n2 + ishft(i4,2)
  i=dat(7)
  i4=iand(i,255)
  n2=n2 + iand(ishft(i4,-6),3)

  return
end subroutine unpack50

