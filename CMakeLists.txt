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

cmake_minimum_required(VERSION 3.18)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/policy.cmake)
update_policy()

# Determine if this is built as a subproject (using add_subdirectory)
# or if it is the master project.
if (NOT DEFINED SQLITE_MASTER_PROJECT)
  set(SQLITE_MASTER_PROJECT OFF)
  if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(SQLITE_MASTER_PROJECT ON)
  endif()
endif()

project(sqlite_functions VERSION 0.1 DESCRIPTION "SQLite3 Functions" HOMEPAGE_URL "https://vxapps.com" LANGUAGES CXX)
include(cmake/env.cmake)

# Fetch Content Dependencies
include(${CMAKE}/fetch/modern.cpp.core.cmake)

# External Project Dependencies
include(${CMAKE}/external/zlib.cmake)

if(NOT ICU_FOUND)
  include(${CMAKE}/external/icu.cmake)
endif()

if(WIN32)
  include(${CMAKE}/fetch/jom.cmake)
endif()

if(NOT SQLite3_FOUND)
  include(${CMAKE}/external/sqlite3.cmake)
endif()

add_subdirectory(source)

if(SQLITE_MASTER_PROJECT)
  if(SQLITE_BUILD_EXAMPLES)
    add_subdirectory(examples)
  endif()
  if(SQLITE_BUILD_TESTS)
    include(${CMAKE}/external/googletest.cmake)
    include(GoogleTest)
    enable_testing()
    add_subdirectory(tests)
  endif()
endif()
