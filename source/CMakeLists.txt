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

project(sqlite_functions)

add_library(${PROJECT_NAME}
  SqliteError.h
  SqliteUtils.cpp
  SqliteUtils.h
)

add_library(SQLite::Functions ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${PROJECT_SOURCE_DIR}
)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
  $<$<BOOL:${HAVE_SPAN}>:HAVE_SPAN>
)

if(CMAKE_CXX_COMPILER_ID MATCHES Clang AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
  set(${PROJECT_NAME}_LIBS -lc++fs)
endif()

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  modern.cpp::core
  SQLite::SQLite3
  ${${PROJECT_NAME}_LIBS}
  PRIVATE
  ICU::uc
  ICU::i18n
)

add_custom_target(${PROJECT_NAME}_documentation
  SOURCES
  ../docs/corefoundation-doxygen-web.tag.xml
  ../docs/cppreference-doxygen-web.tag.xml
  ../docs/Doxyfile.in
  ../docs/footer.html
  ../docs/header.html
  ../docs/magic_enum-doxygen-web.tag.xml
  ../docs/sqlite3-doxygen-web.tag.xml
  ../docs/post_doxygen.py
  ../README.md
)

add_custom_target(${PROJECT_NAME}_workflows
  SOURCES
  ../.github/workflows/linux-codeql.yml
  ../.github/workflows/linux.yml
  ../.github/workflows/macos-codeql.yml
  ../.github/workflows/macos.yml
  ../.github/workflows/windows-codeql.yml
  ../.github/workflows/windows.yml
)
