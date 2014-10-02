!-------------------------------------------------------------------------------
!
! This file is part of the WSPR application, Weak Signal Propogation Reporter
!
! File Name:    unpackpfx.f90
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
subroutine unpackpfx(ng,call1)

  character*12 call1
  character*3 pfx

  if(ng.lt.60000) then
! Add-on prefix of 1 to 3 characters
     n=ng
     do i=3,1,-1
        nc=mod(n,37)
        if(nc.ge.0 .and. nc.le.9) then
           pfx(i:i)=char(nc+48)
        else if(nc.ge.10 .and. nc.le.35) then
           pfx(i:i)=char(nc+55)
        else
           pfx(i:i)=' '
        endif
        n=n/37
     enddo
     call1=pfx//'/'//call1
     if(call1(1:1).eq.' ') call1=call1(2:)
     if(call1(1:1).eq.' ') call1=call1(2:)
  else
! Add-on suffix, one or teo characters
     i1=index(call1,' ')
     nc=ng-60000
     if(nc.ge.0 .and. nc.le.9) then
        call1=call1(:i1-1)//'/'//char(nc+48)
     else if(nc.ge.10 .and. nc.le.35) then
        call1=call1(:i1-1)//'/'//char(nc+55)
     else if(nc.ge.36 .and. nc.le.125) then
        nc1=(nc-26)/10
        nc2=mod(nc-26,10)
        call1=call1(:i1-1)//'/'//char(nc1+48)//char(nc2+48)
     endif
  endif

  return
end subroutine unpackpfx
