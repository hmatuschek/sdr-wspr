!-------------------------------------------------------------------------------
!
! This file is part of the WSPR application, Weak Signal Propogation Reporter
!
! File Name:    ccf2.f90
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
subroutine ccf2(ss,nz,lag1,lag2,ccfbest,lagpk)

  real ss(nz)
  real pr(162)
  logical first

! The WSPR pseudo-random sync pattern:
  integer npr(162)
  data npr/                                     &
       1,1,0,0,0,0,0,0,1,0,0,0,1,1,1,0,0,0,1,0, &
       0,1,0,1,1,1,1,0,0,0,0,0,0,0,1,0,0,1,0,1, &
       0,0,0,0,0,0,1,0,1,1,0,0,1,1,0,1,0,0,0,1, &
       1,0,1,0,0,0,0,1,1,0,1,0,1,0,1,0,1,0,0,1, &
       0,0,1,0,1,1,0,0,0,1,1,0,1,0,1,0,0,0,1,0, &
       0,0,0,0,1,0,0,1,0,0,1,1,1,0,1,1,0,0,1,1, &
       0,1,0,0,0,1,1,1,0,0,0,0,0,1,0,1,0,0,1,1, &
       0,0,0,0,0,0,0,1,1,0,1,0,1,1,0,0,0,1,1,0, &
       0,0/
  data first/.true./
  save

  if(first) then
     nsym=162
     do i=1,nsym
        pr(i)=2*npr(i)-1
     enddo
  endif

  ccfbest=0.

  do lag=lag1,lag2
     x=0.
     do i=1,nsym
        j=16*i + lag
        if(j.ge.1 .and. j.le.nz) x=x+ss(j)*pr(i)
     enddo
     if(x.gt.ccfbest) then
        ccfbest=x
        lagpk=lag
     endif
  enddo

  return
end subroutine ccf2
