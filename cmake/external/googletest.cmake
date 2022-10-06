#
# Copyright (C) 09/14/2022 VX STATS <sales@vxstats.com>
#
# This document is property of VX STATS. It is strictly prohibited
# to modify, sell or publish it in any way. In case you have access
# to this document, you are obligated to ensure its nondisclosure.
# Noncompliances will be prosecuted.
#
# Diese Datei ist Eigentum der VX STATS. Jegliche Änderung, Verkauf
# oder andere Verbreitung und Veröffentlichung ist strikt untersagt.
# Falls Sie Zugang zu dieser Datei haben, sind Sie verpflichtet,
# alles in Ihrer Macht stehende für deren Geheimhaltung zu tun.
# Zuwiderhandlungen werden strafrechtlich verfolgt.
#

include(ExternalProject)

set(GOOGLETEST_SRC ${CMAKE_BINARY_DIR}/_deps/googletest-src)
set(GOOGLETEST_INSTALL ${CMAKE_BINARY_DIR}/_deps/googletest-install)
if (UNIX)
  set(GOOGLETEST_LIBRARY ${GOOGLETEST_INSTALL}/lib/libgtest.a)
else()
  set(GOOGLETEST_LIBRARY ${GOOGLETEST_INSTALL}/lib/gtest.lib)
endif()
set(GOOGLETEST_INCLUDE_DIR ${GOOGLETEST_INSTALL}/include)

ExternalProject_Add(googletest
  PREFIX ${GOOGLETEST_SRC}
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
  GIT_SHALLOW 1
  CMAKE_ARGS
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX:PATH=${GOOGLETEST_INSTALL}
    -Dgtest_force_shared_crt=ON
    -DBUILD_GMOCK=OFF
  INSTALL_DIR ${GOOGLETEST_INSTALL}
  BUILD_BYPRODUCTS ${GOOGLETEST_LIBRARY}
  UPDATE_COMMAND ""
)
