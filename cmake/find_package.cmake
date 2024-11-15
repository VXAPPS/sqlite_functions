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

if (UNIX)
  set(CMAKE_THREAD_LIBS_INIT "-lpthread")
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(CMAKE_REQUIRED_FLAGS "-Wno-unreachable-code-return -Wno-zero-as-null-pointer-constant")
endif()
find_package(Threads REQUIRED)
unset(CMAKE_REQUIRED_FLAGS)

#find_program(BREW_PROGRAM "brew")
#set(HOMEBREW_PREFIX "brew --prefix")

if (NOT DEFINED ${HOMEBREW_PREFIX})
  if (DEFINED ENV{HOMEBREW_PREFIX} AND IS_DIRECTORY ENV{HOMEBREW_PREFIX})
    set(HOMEBREW_PREFIX ENV{HOMEBREW_PREFIX})
  elseif(IS_DIRECTORY /opt/homebrew)
    set(HOMEBREW_PREFIX /opt/homebrew)
  else()
    set(HOMEBREW_PREFIX /usr/local)
  endif()
endif()

if(UNIX AND NOT APPLE)
  set(ICU_ROOT /usr/lib/x86_64-linux-gnu/)
elseif(APPLE)
  set(ICU_ROOT ${HOMEBREW_PREFIX}/opt/icu4c/)
endif()
find_package(ICU 66 COMPONENTS i18n uc)

if(NOT WIN32)
  find_package(SQLite3)
endif()
