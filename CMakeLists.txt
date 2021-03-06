cmake_minimum_required(VERSION 3.0)
project(libkeyfinder)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(BUILD_STATIC_LIBS)
  message(STATUS "Building libKeyFinder as a static library")
  add_library(keyfinder STATIC
    audiodata.cpp
    chromagram.cpp
    chromatransform.cpp
    chromatransformfactory.cpp
    fftadapter.cpp
    keyclassifier.cpp
    keyfinder.cpp
    lowpassfilter.cpp
    lowpassfilterfactory.cpp
    spectrumanalyser.cpp
    temporalwindowfactory.cpp
    toneprofiles.cpp
    windowfunctions.cpp
    workspace.cpp
    constants.cpp
  )
else()
  message(STATUS "Building libKeyFinder as a dynamic library")
  add_library(keyfinder SHARED
    audiodata.cpp
    chromagram.cpp
    chromatransform.cpp
    chromatransformfactory.cpp
    fftadapter.cpp
    keyclassifier.cpp
    keyfinder.cpp
    lowpassfilter.cpp
    lowpassfilterfactory.cpp
    spectrumanalyser.cpp
    temporalwindowfactory.cpp
    toneprofiles.cpp
    windowfunctions.cpp
    workspace.cpp
    constants.cpp
    keyfinder_api.cpp
  )
endif()


target_link_libraries(keyfinder fftw3)

install(TARGETS keyfinder DESTINATION lib)
install(FILES
  audiodata.h
  chromagram.h
  chromatransform.h
  chromatransformfactory.h
  fftadapter.h
  keyclassifier.h
  keyfinder.h
  lowpassfilter.h
  lowpassfilterfactory.h
  spectrumanalyser.h
  temporalwindowfactory.h
  toneprofiles.h
  windowfunctions.h
  workspace.h
  constants.h
  exception.h
  binode.h
  keyfinder_api.h
  DESTINATION include/keyfinder)

set(LIBKEYFINDER_VERSION 2.2.2)
set(LIBKEYFINDER_SO_VERSION 2)
add_compile_definitions(VERSION=${LIBKEYFINDER_VERSION})
set_target_properties(keyfinder PROPERTIES VERSION ${LIBKEYFINDER_VERSION} SOVERSION ${LIBKEYFINDER_SO_VERSION})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libKeyFinder.pc.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/libKeyFinder.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libKeyFinder.pc DESTINATION lib/pkgconfig)

# add_subdirectory(tests)
