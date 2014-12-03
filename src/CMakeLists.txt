set(sdr_wspr_SOURCES main.cc wspr2.cc
    waterfallview.cc receiver.cc rxctrlview.cc rtlinput.cc mapview.cc)
set(sdr_wspr_MOC_HEADERS
    waterfallview.hh receiver.hh rxctrlview.hh rtlinput.hh mapview.hh)
set(sdr_wspr_HEADERS ${sdr_wspr_MOC_HEADERS})

qt5_wrap_cpp(sdr_wspr_MOC_SOURCES ${sdr_wspr_MOC_HEADERS})
#qt5_add_resources(sdr_wspr_RCC_SOURCES ../resources.qrc)

add_executable(sdr_wspr ${sdr_wspr_SOURCES} ${sdr_wspr_MOC_SOURCES} ${sdr_wspr_RCC_SOURCES})
target_link_libraries(sdr_wspr
  ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5WebKitWidgets_LIBRARIES}
  ${LIBS} libsdr libsdr-gui sdrwspr)

qt5_wrap_cpp(sdr_wspr_map_test_MOC_SOURCES mapview.hh)
qt5_add_resources(sdr_wspr_map_test_RCC_SOURCES ../resources.qrc)
add_executable(map_test map_test.cc mapview.cc
  ${sdr_wspr_map_test_MOC_SOURCES} ${sdr_wspr_map_test_RCC_SOURCES})
target_link_libraries(map_test
  ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5WebKitWidgets_LIBRARIES}
  ${LIBS} libsdr libsdr-gui sdrwspr)

add_executable(sdr_wspr_cli wspr2.cc sdr_wspr_cli.cc)
target_link_libraries(sdr_wspr_cli ${LIBS} libsdr libsdr-gui sdrwspr)
