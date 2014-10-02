!-------------------------------------------------------------------------------
!
! This file is part of the WSPR application, Weak Signal Propogation Reporter
!
! File Name:    geodist.f90
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
subroutine geodist(Eplat, Eplon, Stlat, Stlon, Az, Baz, Dist)
  implicit none
  real eplat, eplon, stlat, stlon, az, baz, dist

!JHT: In actual fact, I use the first two arguments for "My Location",
!     the second two for "His location"; West longitude is positive.

!      Taken directly from:
!      Thomas, P.D., 1970, Spheroidal geodesics, reference systems,
!      & local geometry, U.S. Naval Oceanographic Office SP-138,
!      165 pp.

!      assumes North Latitude and East Longitude are positive

!      EpLat, EpLon = End point Lat/Long
!      Stlat, Stlon = Start point lat/long
!      Az, BAz = direct & reverse azimuith
!      Dist = Dist (km); Deg = central angle, discarded

  real BOA, F, P1R, P2R, L1R, L2R, DLR, T1R, T2R, TM,         &
       DTM, STM, CTM, SDTM,CDTM, KL, KK, SDLMR, L,            &
       CD, DL, SD, T, U, V, D, X, E, Y, A, FF64, TDLPM,       &
       HAPBR, HAMBR, A1M2, A2M1

  real AL,BL,D2R,Pi2

  data AL/6378206.4/                ! Clarke 1866 ellipsoid
  data BL/6356583.8/
!      real pi /3.14159265359/
  data D2R/0.01745329251994/        ! degrees to radians conversion factor
  data Pi2/6.28318530718/

  BOA = BL/AL
  F = 1.0 - BOA
!convert st/end pts to radians
  P1R = Eplat * D2R
  P2R = Stlat * D2R
  L1R = Eplon * D2R
  L2R = StLon * D2R
  DLR = L2R - L1R                        ! DLR = Delta Long in Rads
  T1R = ATan(BOA * Tan(P1R))
  T2R = ATan(BOA * Tan(P2R))
  TM = (T1R + T2R) / 2.0
  DTM = (T2R - T1R) / 2.0
  STM = Sin(TM)
  CTM = Cos(TM)
  SDTM = Sin(DTM)
  CDTM = Cos(DTM)
  KL = STM * CDTM
  KK = SDTM * CTM
  SDLMR = Sin(DLR/2.0)
  L = SDTM * SDTM + SDLMR * SDLMR * (CDTM * CDTM - STM * STM)
  CD = 1.0 - 2.0 * L
  DL = ACos(CD)
  SD = Sin(DL)
  T = DL/SD
  U = 2.0 * KL * KL / (1.0 - L)
  V = 2.0 * KK * KK / L
  D = 4.0 * T * T
  X = U + V
  E = -2.0 * CD
  Y = U - V
  A = -D * E
  FF64 = F * F / 64.0
  Dist = AL*SD*(T -(F/4.0)*(T*X-Y)+FF64*(X*(A+(T-(A+E)              &
       /2.0)*X)+Y*(-2.0*D+E*Y)+D*X*Y))/1000.0
  TDLPM = Tan((DLR+(-((E*(4.0-X)+2.0*Y)*((F/2.0)*T+FF64*            &
       (32.0*T+(A-20.0*T)*X-2.0*(D+2.0)*Y))/4.0)*Tan(DLR)))/2.0)
  HAPBR = ATan2(SDTM,(CTM*TDLPM))
  HAMBR = Atan2(CDTM,(STM*TDLPM))
  A1M2 = Pi2 + HAMBR - HAPBR
  A2M1 = Pi2 - HAMBR - HAPBR

1 If ((A1M2 .ge. 0.0) .AND. (A1M2 .lt. Pi2)) GOTO 5
  If (A1M2 .lt. Pi2) GOTO 4
  A1M2 = A1M2 - Pi2
  GOTO 1
4 A1M2 = A1M2 + Pi2
  GOTO 1

!all of this gens the proper az, baz (forward and back azimuth)

5 If ((A2M1 .ge. 0.0) .AND. (A2M1 .lt. Pi2)) GOTO 9
  If (A2M1 .lt. Pi2) GOTO 8
  A2M1 = A2M1 - Pi2
  GOTO 5
8 A2M1 = A2M1 + Pi2
  GOTO 5

9 Az = A1M2 / D2R
  BAZ = A2M1 / D2R

!Fix the mirrored coords here.

  az = 360.0 - az
  baz = 360.0 - baz
end subroutine geodist
