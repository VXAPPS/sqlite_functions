#
# Copyright (C) 09/07/2022 VX APPS <sales@vxapps.com>
#
# This document is property of VX APPS. It is strictly prohibited
# to modify, sell or publish it in any way. In case you have access
# to this document, you are obligated to ensure its nondisclosure.
# Noncompliances will be prosecuted.
#
# Diese Datei ist Eigentum der VX APPS. Jegliche Änderung, Verkauf
# oder andere Verbreitung und Veröffentlichung ist strikt untersagt.
# Falls Sie Zugang zu dieser Datei haben, sind Sie verpflichtet,
# alles in Ihrer Macht stehende für deren Geheimhaltung zu tun.
# Zuwiderhandlungen werden strafrechtlich verfolgt.
#

include(ExternalProject)

set(ZLIB_SRC ${CMAKE_BINARY_DIR}/_deps/zlib-src)
set(ZLIB_INSTALL ${CMAKE_BINARY_DIR}/_deps/zlib-install)
if(UNIX)
  set(ZLIB_LIBRARY ${ZLIB_INSTALL}/lib/libz.a)
  set(ZLIB_OS_ARGS -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE})
else()
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(ZLIB_LIBRARY ${ZLIB_INSTALL}/lib/zlibstaticd.lib)
  else()
    set(ZLIB_LIBRARY ${ZLIB_INSTALL}/lib/zlibstatic.lib)
  endif()
endif()
set(ZLIB_INCLUDE_DIR ${ZLIB_INSTALL}/include)

ExternalProject_Add(ZLIB
  PREFIX ${ZLIB_SRC}
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG v1.3.1
  GIT_SHALLOW 1
  CMAKE_ARGS
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -DCMAKE_C_STANDARD:STRING=${CMAKE_C_STANDARD}
    -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
    -DCMAKE_INSTALL_PREFIX:PATH=${ZLIB_INSTALL}
    -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
    ${ZLIB_OS_ARGS}
  INSTALL_DIR ${ZLIB_INSTALL}
  BUILD_BYPRODUCTS ${ZLIB_LIBRARY}
  UPDATE_COMMAND ""
  PATCH_COMMAND patch -t < ${CMAKE_SOURCE_DIR}/cmake/patches/zlib.patch
)

# We cannot use find_library because ExternalProject_Add() is performed at build time.
# And to please the property INTERFACE_INCLUDE_DIRECTORIES,
# we make the include directory in advance.
file(MAKE_DIRECTORY ${ZLIB_INCLUDE_DIR})

add_library(ZLIB::ZLIB STATIC IMPORTED GLOBAL)
set_property(TARGET ZLIB::ZLIB PROPERTY IMPORTED_LOCATION ${ZLIB_LIBRARY})
set_property(TARGET ZLIB::ZLIB PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${ZLIB_INCLUDE_DIR})
add_dependencies(ZLIB::ZLIB ZLIB)
