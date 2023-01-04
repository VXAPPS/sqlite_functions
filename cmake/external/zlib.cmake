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
if (UNIX)
  set(ZLIB_LIBRARY ${ZLIB_INSTALL}/lib/libz.a)
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
  GIT_TAG v1.2.13
  GIT_SHALLOW 1
  CMAKE_ARGS
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX:PATH=${ZLIB_INSTALL}
  INSTALL_DIR ${ZLIB_INSTALL}
  BUILD_BYPRODUCTS ${ZLIB_LIBRARY}
  UPDATE_COMMAND ""
)

# We cannot use find_library because ExternalProject_Add() is performed at build time.
# And to please the property INTERFACE_INCLUDE_DIRECTORIES,
# we make the include directory in advance.
file(MAKE_DIRECTORY ${ZLIB_INCLUDE_DIR})

add_library(ZLIB::ZLIB STATIC IMPORTED GLOBAL)
set_property(TARGET ZLIB::ZLIB PROPERTY IMPORTED_LOCATION ${ZLIB_LIBRARY})
set_property(TARGET ZLIB::ZLIB PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${ZLIB_INCLUDE_DIR})
add_dependencies(ZLIB::ZLIB ZLIB)
