#
# Copyright (c) 2022 Florian Becker <fb@vxapps.com> (VX APPS).
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

if(SQLite3_FOUND)
  return()
endif()

include(ExternalProject)

set(SQLITE_SRC ${CMAKE_BINARY_DIR}/_deps/sqlite-src)
set(SQLITE_INSTALL ${CMAKE_BINARY_DIR}/_deps/sqlite-install)
if (UNIX)
  set(SQLITE_LIBRARY ${SQLITE_INSTALL}/lib/libsqlite3.a)
  set(SQLITE_INCLUDE_DIR ${SQLITE_INSTALL}/include)
else()
  set(SQLITE_LIBRARY ${SQLITE_SRC}/src/SQLite/sqlite3.lib)
  set(SQLITE_INCLUDE_DIR ${SQLITE_SRC}/src/SQLite)
endif()
if(APPLE)
  set(SQLITE_OS_ARGS -arch ${CMAKE_OSX_ARCHITECTURES})
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(SQLITE_DEBUG --enable-debug)
endif()

if(WIN32)
  set(SQLITE_CONFIGURE_COMMAND echo)
#  set(SQLITE_BUILD_COMMAND nmake /f ${SQLITE_SRC}/src/SQLite/Makefile.msc TOP=${SQLITE_SRC}/src/SQLite DEBUG=3)
  set(SQLITE_BUILD_COMMAND nmake /f Makefile.msc DEBUG=3)
  set(SQLITE_BUILD_IN_SOURCE TRUE)
  set(SQLITE_INSTALL_COMMAND lib sqlite3.obj)
else()
  set(SQLITE_CONFIGURE_COMMAND
        CC=${CMAKE_C_COMPILER}
        CFLAGS=--std=c${CMAKE_C_STANDARD} ${SQLITE_OS_ARGS}
        LDFLAGS=${ZLIB_DIR}
        ${SQLITE_SRC}/src/SQLite/configure
    #    -q #quite
        --prefix=${SQLITE_INSTALL}
        --enable-static=yes
        --enable-shared=no
        --enable-threadsafe
        ${SQLITE_DEBUG}
  )
  set(SQLITE_BUILD_COMMAND make -j${CPU_COUNT})
  set(SQLITE_BUILD_IN_SOURCE FALSE)
  set(SQLITE_INSTALL_COMMAND make install)
endif()

ExternalProject_Add(SQLite
  DEPENDS ZLIB
  PREFIX ${SQLITE_SRC}
  URL https://sqlite.org/2024/sqlite-autoconf-3470000.tar.gz
  URL_HASH SHA512=698e28a3f1c3da5b45b86a0b50f84c696658d4e56ab45f5cc65dce995601c3bcf1c0050386a1fc08b4b0e0f508e8a046e5c8317b09fe805154b76437e73f8f0e
#  GIT_REPOSITORY https://github.com/sqlite/sqlite.git
#  GIT_TAG version-3.47.0
#  GIT_SHALLOW 1
  USES_TERMINAL_DOWNLOAD FALSE
  BUILD_IN_SOURCE ${SQLITE_BUILD_IN_SOURCE}
  CONFIGURE_COMMAND ${SQLITE_CONFIGURE_COMMAND}
  BUILD_COMMAND ${SQLITE_BUILD_COMMAND}
  TEST_COMMAND ""
  INSTALL_COMMAND ${SQLITE_INSTALL_COMMAND}
  INSTALL_DIR ${SQLITE_INSTALL}
  BUILD_BYPRODUCTS ${SQLITE_LIBRARY}
  UPDATE_COMMAND ""
)

# We cannot use find_library because ExternalProject_Add() is performed at build time.
# And to please the property INTERFACE_INCLUDE_DIRECTORIES,
# we make the include directory in advance.
file(MAKE_DIRECTORY ${SQLITE_INCLUDE_DIR})

add_library(SQLite::SQLite3 STATIC IMPORTED GLOBAL)
set_property(TARGET SQLite::SQLite3 PROPERTY IMPORTED_LOCATION ${SQLITE_LIBRARY})
set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${SQLITE_INCLUDE_DIR})
add_dependencies(SQLite::SQLite3 SQLite)
