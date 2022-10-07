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

include(ExternalProject)

set(SQLITE_SRC ${CMAKE_BINARY_DIR}/_deps/sqlite-src)
set(SQLITE_INSTALL ${CMAKE_BINARY_DIR}/_deps/sqlite-install)
if (UNIX)
  set(SQLITE_LIBRARY ${SQLITE_INSTALL}/lib/libsqlite3.a)
else()
  set(SQLITE_LIBRARY ${SQLITE_INSTALL}/lib/sqlite3.lib)
endif()
set(SQLITE_INCLUDE_DIR ${SQLITE_INSTALL}/include)

ExternalProject_Add(sqlite
  SOURCE_DIR ${SQLITE_SRC}
  URL https://sqlite.org/2022/sqlite-autoconf-3390400.tar.gz
  URL_HASH SHA512=cc1de214e69ef677cac3f6dd2000ccfcf4b672ab464a115d96f24707009a408630e762c3cda89741b728ab34c4d9f5b8f8b12e9b11448e8364642b4421c3393d
#  GIT_REPOSITORY https://github.com/sqlite/sqlite.git
#  GIT_TAG version-3.39.4
#  GIT_SHALLOW 1
  USES_TERMINAL_DOWNLOAD FALSE
  CONFIGURE_COMMAND
    CC=${CMAKE_C_COMPILER}
#    CFLAGS=${CMAKE_C_FLAGS}
#    LDFLAGS=${CMAKE_STATIC_LINKER_FLAGS}
#    LDFLAGS=ZLIB_DIR
    ${SQLITE_SRC}/configure
    -q #quite
    --prefix=${SQLITE_INSTALL}
    --enable-static=yes
    --enable-shared=no
    --enable-threadsafe=yes
    --enable-debug=no
#    --disable-largefile
  BUILD_COMMAND make
  TEST_COMMAND ""
  INSTALL_COMMAND make install
  INSTALL_DIR ${SQLITE_INSTALL}
  BUILD_BYPRODUCTS ${SQLITE_LIBRARY}
  UPDATE_COMMAND ""
)

# nmake /f ..\sqlite\Makefile.msc TOP=.

file(MAKE_DIRECTORY ${SQLITE_INCLUDE_DIR})

add_library(SQLite::SQLite3 STATIC IMPORTED GLOBAL)
set_property(TARGET SQLite::SQLite3 PROPERTY IMPORTED_LOCATION ${SQLITE_LIBRARY})
set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${SQLITE_INCLUDE_DIR})
add_dependencies(SQLite::SQLite3 sqlite)
