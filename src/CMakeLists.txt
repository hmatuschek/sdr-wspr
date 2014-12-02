set(sdr_wspr_SOURCES main.cc wspr2.cc
    waterfallview.cc receiver.cc rxctrlview.cc rtlinput.cc)
set(sdr_wspr_MOC_HEADERS
    waterfallview.hh receiver.hh rxctrlview.hh rtlinput.hh)
qt5_wrap_cpp(sdr_wspr_MOC_SOURCES ${sdr_wspr_MOC_HEADERS})

set(sdr_wspr_HEADERS ${sdr_wspr_MOC_HEADERS})
add_executable(sdr_wspr ${sdr_wspr_SOURCES} ${sdr_wspr_MOC_SOURCES})
target_link_libraries(sdr_wspr
 ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${LIBS} libsdr libsdr-gui sdrwspr)


add_executable(sdr_wspr_cli wspr2.cc sdr_wspr_cli.cc)
target_link_libraries(sdr_wspr_cli ${LIBS} libsdr libsdr-gui sdrwspr)
