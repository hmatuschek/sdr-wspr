!-------------------------------------------------------------------------------
!
! This file is part of the WSPR application, Weak Signal Propogation Reporter
!
! File Name:    inter_mept.f90
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
subroutine inter_mept(id,ndir)

! Interleave (ndir=1) or de-interleave (ndir=-1) the array id.

  integer*1 id(0:161),itmp(0:161)
  integer j0(0:161)
  logical first
  data first/.true./
  save

  if(first) then
! Compute the interleave table using bit reversal.
     k=-1
     do i=0,255
        n=0
        ii=i
        do j=0,7
           n=n+n
           if(iand(ii,1).ne.0) n=n+1
           ii=ii/2
        enddo
        if(n.le.161) then
           k=k+1
           j0(k)=n
        endif
     enddo
     first=.false.
  endif

  if(ndir.eq.1) then
     do i=0,161
        itmp(j0(i))=id(i)
     enddo
  else
     do i=0,161
        itmp(i)=id(j0(i))
     enddo
  endif

  do i=0,161
     id(i)=itmp(i)
  enddo

  return
end subroutine inter_mept
