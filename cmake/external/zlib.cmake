#
# Copyright (C) 09/07/2022 VX STATS <sales@vxstats.com>
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

set(ZLIB_SRC ${CMAKE_BINARY_DIR}/_deps/zlib-src)
set(ZLIB_INSTALL ${CMAKE_BINARY_DIR}/_deps/zlib-install)
if (UNIX)
  set(ZLIB_LIBRARY ${ZLIB_INSTALL}/lib/libz.a)
else()
  set(ZLIB_LIBRARY ${ZLIB_INSTALL}/lib/zlibstatic.lib)
endif()
set(ZLIB_INCLUDE_DIR ${ZLIB_INSTALL}/include)

ExternalProject_Add(zlib
  PREFIX ${ZLIB_SRC}
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG v1.2.12
  GIT_SHALLOW 1
  CMAKE_ARGS
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX:PATH=${ZLIB_INSTALL}
  INSTALL_DIR ${ZLIB_INSTALL}
  BUILD_BYPRODUCTS ${ZLIB_LIBRARY}
  UPDATE_COMMAND ""
)
