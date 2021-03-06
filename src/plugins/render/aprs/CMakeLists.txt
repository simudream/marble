PROJECT(AprsPlugin)

macro_optional_find_package(QextSerialPort)
marble_set_package_properties( QextSerialPort PROPERTIES DESCRIPTION "access to serial ports" )
marble_set_package_properties( QextSerialPort PROPERTIES URL "http://code.google.com/p/qextserialport/" )
marble_set_package_properties( QextSerialPort PROPERTIES TYPE OPTIONAL PURPOSE "Reading from serial port in APRS plugin" )

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${QT_INCLUDE_DIR}
)
if( QT4_FOUND )
  INCLUDE(${QT_USE_FILE})
endif()

if (QEXTSERIALPORT_FOUND)
  include_directories (${QEXTSERIALPORT_INCLUDE_DIR})
  set (serial_src AprsTTY.cpp)
  list (APPEND AprsPlugin_LIBS ${QEXTSERIALPORT_LIBRARIES})
  set (HAVE_QEXTSERIALPORT TRUE)
endif ()

set( aprs_SRCS AprsPlugin.cpp
               AprsObject.cpp
	       AprsGatherer.cpp
	       GeoAprsCoordinates.cpp
	       ${CMAKE_CURRENT_BINARY_DIR}/AprsGatherer_mic_e.cpp

	       AprsSource.cpp
	       AprsTCPIP.cpp
	       AprsFile.cpp

	       ${serial_src} )

set( aprs_UI AprsConfigWidget.ui )
qt_wrap_ui( aprs_SRCS ${aprs_UI} )

if (QTONLY)
marble_qt4_automoc( ${aprs_SRCS} )
endif (QTONLY)

add_custom_command(
  OUTPUT AprsGatherer_mic_e.cpp
  COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/AprsGatherGen.pl > AprsGatherer_mic_e.cpp
  MAIN_DEPENDENCY AprsGatherGen.pl
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

marble_add_plugin( AprsPlugin ${aprs_SRCS} )
#target_link_libraries( Aprs ${QT_QTWEBKIT_LIBRARY} )

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/aprsconfig.h.in
	       ${CMAKE_CURRENT_BINARY_DIR}/aprsconfig.h)
