!-------------------------------------------------------------------------------
!
! This file is part of the WSPR application, Weak Signal Propogation Reporter
!
! File Name:    decode162.f90
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
subroutine decode162(c4,npts,message,ncycles,metric,nerr)

! Decode MEPT_JT data, assuming that DT and DF have already been determined.

  complex c4(npts)
  character*22 message
  real*8 dt,df,twopi,f0,f1,dphi0,dphi1
  complex*16 c0,c1
  complex*16 w0,w1,ws0,ws1
  integer*1 symbol(162)
  integer*1 data1(11)
  integer amp
  integer mettab(0:255,0:1)
  logical first
  data first/.true./
  integer npr3(162)
  data npr3/                                    &
       1,1,0,0,0,0,0,0,1,0,0,0,1,1,1,0,0,0,1,0, &
       0,1,0,1,1,1,1,0,0,0,0,0,0,0,1,0,0,1,0,1, &
       0,0,0,0,0,0,1,0,1,1,0,0,1,1,0,1,0,0,0,1, &
       1,0,1,0,0,0,0,1,1,0,1,0,1,0,1,0,1,0,0,1, &
       0,0,1,0,1,1,0,0,0,1,1,0,1,0,1,0,0,0,1,0, &
       0,0,0,0,1,0,0,1,0,0,1,1,1,0,1,1,0,0,1,1, &
       0,1,0,0,0,1,1,1,0,0,0,0,0,1,0,1,0,0,1,1, &
       0,0,0,0,0,0,0,1,1,0,1,0,1,1,0,0,0,1,1,0, &
       0,0/
  data mettab/                                           &
       5,   5,   5,   5,   5,   5,   5,   5,   5,   5, &
       5,   5,   5,   5,   5,   5,   5,   5,   5,   5, &
       5,   5,   5,   5,   5,   5,   5,   5,   5,   5, &
       5,   5,   5,   5,   5,   5,   5,   5,   5,   5, &
       5,   5,   5,   5,   5,   5,   5,   5,   5,   5, &
       5,   5,   5,   5,   5,   5,   5,   5,   5,   5, &
       5,   5,   5,   5,   5,   5,   5,   5,   5,   5, &
       5,   5,   5,   5,   5,   5,   5,   5,   5,   4, &
       4,   4,   4,   4,   4,   4,   4,   4,   4,   4, &
       4,   4,   4,   4,   4,   4,   4,   4,   4,   4, &
       3,   3,   3,   3,   3,   3,   3,   3,   3,   2, &
       2,   2,   2,   2,   1,   1,   1,   1,   0,   0, &
       -1,  -1,  -1,  -2,  -2,  -3,  -4,  -4,  -5,  -6, &
       -7,  -7,  -8,  -9, -10, -11, -12, -12, -13, -14, &
       -15, -16, -17, -17, -18, -19, -20, -21, -22, -22, &
       -23, -24, -25, -26, -26, -27, -28, -29, -30, -30, &
       -31, -32, -33, -33, -34, -35, -36, -36, -37, -38, &
       -38, -39, -40, -41, -41, -42, -43, -43, -44, -45, &
       -45, -46, -47, -47, -48, -49, -49, -50, -51, -51, &
       -52, -53, -53, -54, -54, -55, -56, -56, -57, -57, &
       -58, -59, -59, -60, -60, -61, -62, -62, -62, -63, &
       -64, -64, -65, -65, -66, -67, -67, -67, -68, -69, &
       -69, -70, -70, -71, -72, -72, -72, -72, -73, -74, &
       -75, -75, -75, -77, -76, -76, -78, -78, -80, -81, &
       -80, -79, -83, -82, -81, -82, -82, -83, -84, -84, &
       -84, -87, -86, -87, -88, -89, -89, -89, -88, -87, &
       -86, -87, -84, -84, -84, -83, -82, -82, -81, -82, &
       -83, -79, -80, -81, -80, -78, -78, -76, -76, -77, &
       -75, -75, -75, -74, -73, -72, -72, -72, -72, -71, &
       -70, -70, -69, -69, -68, -67, -67, -67, -66, -65, &
       -65, -64, -64, -63, -62, -62, -62, -61, -60, -60, &
       -59, -59, -58, -57, -57, -56, -56, -55, -54, -54, &
       -53, -53, -52, -51, -51, -50, -49, -49, -48, -47, &
       -47, -46, -45, -45, -44, -43, -43, -42, -41, -41, &
       -40, -39, -38, -38, -37, -36, -36, -35, -34, -33, &
       -33, -32, -31, -30, -30, -29, -28, -27, -26, -26, &
       -25, -24, -23, -22, -22, -21, -20, -19, -18, -17, &
       -17, -16, -15, -14, -13, -12, -12, -11, -10,  -9, &
       -8,  -7,  -7,  -6,  -5,  -4,  -4,  -3,  -2,  -2, &
       -1,  -1,  -1,   0,   0,   1,   1,   1,   1,   2, &
       2,   2,   2,   2,   3,   3,   3,   3,   3,   3, &
       3,   3,   3,   4,   4,   4,   4,   4,   4,   4, &
       4,   4,   4,   4,   4,   4,   4,   4,   4,   4, &
       4,   4,   4,   4,   5,   5,   5,   5,   5,   5, &
       5,   5,   5,   5,   5,   5,   5,   5,   5,   5, &
       5,   5,   5,   5,   5,   5,   5,   5,   5,   5, &
       5,   5,   5,   5,   5,   5,   5,   5,   5,   5, &
       5,   5,   5,   5,   5,   5,   5,   5,   5,   5, &
       5,   5,   5,   5,   5,   5,   5,   5,   5,   5, &
       5,   5,   5,   5,   5,   5,   5,   5,   5,   5, &
       5,   5,   5,   5,   5,   5,   5,   5,   5,   5, &
       5,   5/
  save

  if(first) then
     twopi=8*atan(1.d0)
     dt=1.d0/375.d0                        !Sample interval
     df=375.d0/256.d0
     nsym=162
     nbits=50+31
     amp=20                                !### 32 ??? ###
     ndelta=50
     limit=10000
     first=.false.
  endif

! Should amp be adjusted according to signal strength?
! Compute soft symbols
  k=0
  nsps=256
  fac2=0.001
  w0=1.0
  w1=1.0

  do j=1,nsym
     f0=(npr3(j)-1.5)*df
     f1=(2+npr3(j)-1.5)*df
     dphi0=twopi*dt*f0
     dphi1=twopi*dt*f1
     ws0=dcmplx(cos(dphi0),-sin(dphi0))
     ws1=dcmplx(cos(dphi1),-sin(dphi1))
     c0=0.
     c1=0.
     do i=1,nsps
        k=k+1
        w0=w0*ws0
        w1=w1*ws1
        c0=c0 + w0*c4(k)
        c1=c1 + w1*c4(k)
     enddo

     sq0=fac2*(real(c0)**2 + aimag(c0)**2)
     sq1=fac2*(real(c1)**2 + aimag(c1)**2)
     rsym=amp*(sq1-sq0)
     r=rsym+128.
     if(r.gt.255.0) r=255.0
     if(r.lt.0.0) r=0.0
     n4=nint(r)
     if(n4.gt.127) n4=n4-256
     symbol(j)=n4
  enddo

  call inter_mept(symbol,-1)                      !Remove interleaving
  call fano232(symbol,nbits,mettab,ndelta,limit,                        &
       data1,ncycles,metric,nerr)
  message='                      '
  if(nerr.ge.0) call wqdecode(data1,message,ntype2)

  return
end subroutine decode162
