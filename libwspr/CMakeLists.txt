set(sdr_wspr_FORTRAN_SOURCES decode162.f90 mix162.f90 sync162.f90 twkfreq.f90 db.f90 fano232.f90
    fchisq.f90 inter_mept.f90 pctile.f90 ps162.f90 wqdecode.f90 ccf2.f90 cs_stubs.f90 four2a.f90
    hash.f90 sort.f90 unpack50.f90 unpackcall.f90 unpackgrid.f90 unpackpfx.f90 deg2grid.f90
    ssort.f90 nhash.c xfft.f90 wspr.cc azdist.f90 geodist.f90 grid2deg.f90)
add_library(sdrwspr ${sdr_wspr_FORTRAN_SOURCES})
#target_link_libraries(sdrwspr "${CMAKE_CURRENT_SOURCE_DIR}/../dist/libfftw3f.a")
target_link_libraries(sdrwspr ${FFTWSingle_LIBRARIES})


