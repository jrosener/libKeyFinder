#*************************************************************************
#
# Copyright 2011-2014 Ibrahim Sha'ath
#
# This file is part of LibKeyFinder.
#
# LibKeyFinder is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# LibKeyFinder is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with LibKeyFinder.  If not, see <http://www.gnu.org/licenses/>.
#
#*************************************************************************

cache()

QT -= gui
CONFIG -= qt

TARGET = keyfinder
TEMPLATE = lib

VERSION = 2.2.1

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

DEFINES += LIBKEYFINDER_LIBRARY
DEFINES += VERSION=$${VERSION}

HEADERS += \
    audiodata.h \
    binode.h \
    chromagram.h \
    chromatransform.h \
    chromatransformfactory.h \
    constants.h \
    exception.h \
    fftadapter.h \
    keyclassifier.h \
    keyfinder.h \
    lowpassfilter.h \
    lowpassfilterfactory.h \
    spectrumanalyser.h \
    temporalwindowfactory.h \
    toneprofiles.h \
    windowfunctions.h \
    workspace.h \
    keyfinder_api.h

SOURCES += \
    audiodata.cpp \
    chromagram.cpp \
    chromatransform.cpp \
    chromatransformfactory.cpp \
    fftadapter.cpp \
    keyclassifier.cpp \
    keyfinder.cpp \
    lowpassfilter.cpp \
    lowpassfilterfactory.cpp \
    spectrumanalyser.cpp \
    temporalwindowfactory.cpp \
    toneprofiles.cpp \
    windowfunctions.cpp \
    workspace.cpp \
    constants.cpp \
    keyfinder_api.cpp

OTHER_FILES += README \
    dist/ubuntu/generate_libkeyfinder_deb.sh \
    dist/ubuntu/debian/changelog \
    dist/ubuntu/debian/compat \
    dist/ubuntu/debian/control \
    dist/ubuntu/debian/copyright \
    dist/ubuntu/debian/docs \
    dist/ubuntu/debian/libkeyfinder0.dirs \
    dist/ubuntu/debian/libkeyfinder0.install \
    dist/ubuntu/debian/libkeyfinder-dev.dirs \
    dist/ubuntu/debian/libkeyfinder-dev.install \
    dist/ubuntu/debian/rules \
    dist/debian/generate_libkeyfinder_deb.sh \
    dist/debian/debian/changelog \
    dist/debian/debian/compat \
    dist/debian/debian/control \
    dist/debian/debian/copyright \
    dist/debian/debian/docs \
    dist/debian/debian/libkeyfinder0.dirs \
    dist/debian/debian/libkeyfinder0.install \
    dist/debian/debian/libkeyfinder-dev.dirs \
    dist/debian/debian/libkeyfinder-dev.install \
    dist/debian/debian/rules

macx{
  LIBS += -stdlib=libc++
  QMAKE_CXXFLAGS += -stdlib=libc++
  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
  QMAKE_MAC_SDK = macosx10.12
  CONFIG -= ppc ppc64 x86
  CONFIG += x86_64

  # installation of headers and the shared object
  target.path = /usr/local/lib
  headers.path = /usr/local/include/$$TARGET
  QMAKE_LFLAGS_SONAME = -Wl,-install_name,/usr/local/lib/
}

unix:!macx{
  target.path = /usr/lib
  headers.path = /usr/include/$$TARGET
}

unix|macx{
  LIBS += -lfftw3 -lboost_system -lboost_thread
}

unix{
  header.path = /usr/include
  header.files = keyfinder_api.h
  INSTALLS += header
}

unix{
  target.path = /usr/lib
  INSTALLS += target
}

win32{
  # FFTW3 (dynamic lib)
  LIBS += -L$$PWD/vs2010-external_libs/fftw-3.3.3-dll32/ -llibfftw3-3
  INCLUDEPATH += $$PWD/vs2010-external_libs/fftw-3.3.3-dll32
  DEPENDPATH += $$PWD/vs2010-external_libs/fftw-3.3.3-dll32

  # Lib Boost (static libs)
  INCLUDEPATH += $$PWD/vs2010-external_libs/boost_1_53_0
  DEPENDPATH += $$PWD/vs2010-external_libs/boost_1_53_0
  LIBS += -L$$PWD/vs2010-external_libs/boost_1_53_0/lib32/

  # Copy FFTW3 DLL
  DLLS = \
      $${PWD}/vs2010-external_libs/fftw-3.3.3-dll32/libfftw3-3.dll
  DESTDIR_WIN = $${DESTDIR}
  CONFIG(debug, debug|release) {
    DESTDIR_WIN += debug
  } else {
    DESTDIR_WIN += release
  }
  DLLS ~= s,/,\\,g
  DESTDIR_WIN ~= s,/,\\,g
  for(FILE, DLLS){
      QMAKE_POST_LINK += $${QMAKE_COPY} $$quote($${FILE}) $$quote($${DESTDIR_WIN}) $$escape_expand(\\n\\t)
  }

  # Copy API file.
  QMAKE_POST_LINK += $${QMAKE_COPY} $$quote(keyfinder_api.h) $$quote($${DESTDIR_WIN}) $$escape_expand(\\n\\t)
}
