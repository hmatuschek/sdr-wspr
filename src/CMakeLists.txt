set(sdr_wspr_SOURCES main.cc wspr2.cc
    waterfallview.cc receiver.cc)
set(sdr_wspr_MOC_HEADERS
    waterfallview.hh receiver.hh)
qt5_wrap_cpp(sdr_wspr_MOC_SOURCES ${sdr_wspr_MOC_HEADERS})

set(sdr_wspr_HEADERS ${sdr_wspr_MOC_HEADERS})
add_executable(sdr_wspr ${sdr_wspr_SOURCES} ${sdr_wspr_MOC_SOURCES})
target_link_libraries(sdr_wspr
 ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${LIBS} libsdr libsdr-gui sdrwspr)
