!-------------------------------------------------------------------------------
!
! This file is part of the WSPR application, Weak Signal Propogation Reporter
!
! File Name:    fchisq.f90
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
real function fchisq(cx,npts,fsample,a,lag1,lag2,ccfmax,dtmax)

  parameter (NMAX=120*375)
  complex cx(npts)
  real a(5)
  complex*16 w1,ws1
  complex*16 w2,ws2
  complex*16 w3,ws3
  complex*16 w4,ws4
  complex*16 cs1(0:NMAX)
  complex*16 cs2(0:NMAX)
  complex*16 cs3(0:NMAX)
  complex*16 cs4(0:NMAX)
  complex z1,z2,z3,z4
  real*8 twopi,baud,p2
  real ss(5624)
  save

  twopi=8.d0*atan(1.d0)
  baud=12000.d0/8192

! Mix and integrate four channels
  cs1(0)=0.
  cs2(0)=0.
  cs3(0)=0.
  cs4(0)=0.
  w1=1.0
  w2=1.0
  w3=1.0
  w4=1.0
  x0=0.5*(npts+1)           !Middle sample
  s=2.0/npts
  dt=1.0/fsample
  do i=1,npts
     x=s*(i-x0)             !x runs from -1 to +1
     if(mod(i,100).eq.1) then
        p2=1.5*x*x - 0.5
!           p3=2.5*(x**3) - 1.5*x
!           p4=4.375*(x**4) - 3.75*(x**2) + 0.375
        dphi1=twopi*dt*(a(1) + x*a(2) + p2*a(3) + 1.5*baud)
        dphi2=twopi*dt*(a(1) + x*a(2) + p2*a(3) + 0.5*baud)
        dphi3=twopi*dt*(a(1) + x*a(2) + p2*a(3) - 0.5*baud)
        dphi4=twopi*dt*(a(1) + x*a(2) + p2*a(3) - 1.5*baud)
        ws1=cmplx(cos(dphi1),sin(dphi1))
        ws2=cmplx(cos(dphi2),sin(dphi2))
        ws3=cmplx(cos(dphi3),sin(dphi3))
        ws4=cmplx(cos(dphi4),sin(dphi4))
     endif
     w1=w1*ws1
     w2=w2*ws2
     w3=w3*ws3
     w4=w4*ws4
     cs1(i)=cs1(i-1) + w1*cx(i)
     cs2(i)=cs2(i-1) + w2*cx(i)
     cs3(i)=cs3(i-1) + w3*cx(i)
     cs4(i)=cs4(i-1) + w4*cx(i)
  enddo

! Compute full-symbol powers at 1/16-symbol steps.
  nsps=nint(fsample/baud)                  !Samples per symbol
  ndiv=16                                  !Steps per symbol
  nout=ndiv*npts/nsps                      !Total steps
  dtstep=1.0/(ndiv*baud)                   !Time per output step
  fac=1.e-5

  ss=0.
  do i=1,nout
     j=i*nsps/ndiv
     k=j - nsps
     ss(i)=0.
     if(k.ge.1) then
        z1=cs1(j)-cs1(k)
        z2=cs2(j)-cs2(k)
        z3=cs3(j)-cs3(k)
        z4=cs4(j)-cs4(k)

        p1=real(z1)**2 + aimag(z1)**2
        p2=real(z2)**2 + aimag(z2)**2
        p3=real(z3)**2 + aimag(z3)**2
        p4=real(z4)**2 + aimag(z4)**2
!            ss(i)=fac*(max(p2,p4) - max(p1,p3))
        ss(i)=fac*((p2+p4) - (p1+p3))
     endif
  enddo

  ccfmax=0.
  call ccf2(ss,nout,lag1,lag2,ccf,lagpk)
  if(ccf.gt.ccfmax) then
     ccfmax=ccf
     dtmax=lagpk*dtstep
  endif

! Reverse sign (and offset!) because we will be minimizing fchisq
  fchisq=-ccfmax + 100.0

  return
end function fchisq
