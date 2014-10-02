!-------------------------------------------------------------------------------
!
! This file is part of the WSPR application, Weak Signal Propogation Reporter
!
! File Name:    twkfreq.f90
! Description:  Apply AFC corrections to ca, returning corrected data in cb
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
subroutine twkfreq(ca,cb,jz,a)

! Apply AFC corrections to ca, returning corrected data in cb

  complex ca(jz),cb(jz)
  real a(5)
  real*8 twopi
  complex*16 w,wstep
  data twopi/0.d0/
  save twopi

  if(twopi.eq.0.d0) twopi=8.d0*atan(1.d0)
  w=1.d0
  wstep=1.d0
  x0=0.5*(jz+1)
  s=2.0/jz
  do i=1,jz
     x=s*(i-x0)
     if(mod(i,100).eq.1) then
        p2=1.5*x*x - 0.5
!            p3=2.5*(x**3) - 1.5*x
!            p4=4.375*(x**4) - 3.75*(x**2) + 0.375
        dphi=(a(1) + x*a(2) + p2*a(3)) * (twopi/375.0)
        wstep=cmplx(cos(dphi),sin(dphi))
     endif
     w=w*wstep
     cb(i)=w*ca(i)
  enddo

  return
end subroutine twkfreq
