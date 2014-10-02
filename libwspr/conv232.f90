!-------------------------------------------------------------------------------
!
! This file is part of the WSPR application, Weak Signal Propogation Reporter
!
! File Name:    conv232.f90
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

! Layland-Lushbaugh polynomials for a K=32, r=1/2 convolutional code,
! and 8-bit parity lookup table.

  data npoly1/-221228207/,npoly2/-463389625/
  integer*1 partab(0:255)
  data partab/                 &
       0, 1, 1, 0, 1, 0, 0, 1, &
       1, 0, 0, 1, 0, 1, 1, 0, &
       1, 0, 0, 1, 0, 1, 1, 0, &
       0, 1, 1, 0, 1, 0, 0, 1, &
       1, 0, 0, 1, 0, 1, 1, 0, &
       0, 1, 1, 0, 1, 0, 0, 1, &
       0, 1, 1, 0, 1, 0, 0, 1, &
       1, 0, 0, 1, 0, 1, 1, 0, &
       1, 0, 0, 1, 0, 1, 1, 0, &
       0, 1, 1, 0, 1, 0, 0, 1, &
       0, 1, 1, 0, 1, 0, 0, 1, &
       1, 0, 0, 1, 0, 1, 1, 0, &
       0, 1, 1, 0, 1, 0, 0, 1, &
       1, 0, 0, 1, 0, 1, 1, 0, &
       1, 0, 0, 1, 0, 1, 1, 0, &
       0, 1, 1, 0, 1, 0, 0, 1, &
       1, 0, 0, 1, 0, 1, 1, 0, &
       0, 1, 1, 0, 1, 0, 0, 1, &
       0, 1, 1, 0, 1, 0, 0, 1, &
       1, 0, 0, 1, 0, 1, 1, 0, &
       0, 1, 1, 0, 1, 0, 0, 1, &
       1, 0, 0, 1, 0, 1, 1, 0, &
       1, 0, 0, 1, 0, 1, 1, 0, &
       0, 1, 1, 0, 1, 0, 0, 1, &
       0, 1, 1, 0, 1, 0, 0, 1, &
       1, 0, 0, 1, 0, 1, 1, 0, &
       1, 0, 0, 1, 0, 1, 1, 0, &
       0, 1, 1, 0, 1, 0, 0, 1, &
       1, 0, 0, 1, 0, 1, 1, 0, &
       0, 1, 1, 0, 1, 0, 0, 1, &
       0, 1, 1, 0, 1, 0, 0, 1, &
       1, 0, 0, 1, 0, 1, 1, 0/
