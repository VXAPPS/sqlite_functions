#
# Copyright (c) 2023 Florian Becker <fb@vxapps.com> (VX APPS).
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

if(ICU_FOUND)
  return()
endif()

include(FetchContent)

FetchContent_Declare(icu-data
  URL https://github.com/unicode-org/icu/releases/download/release-72-1/icu4c-72_1-data.zip
  URL_HASH SHA512=9c7a85d22bcd27ccb8503ce3a64a0f88c6c495d6c0619398ec3a693f156dadd22bbc5c2993730cf6f25b4869206562f875c35ba35c462c6d3420d5d4e8de4faf
  SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/icu-src/src/ICU/data
)

FetchContent_MakeAvailable(icu-data)

include(ExternalProject)

set(ICU_SRC ${CMAKE_BINARY_DIR}/_deps/icu-src)
set(ICU_INSTALL ${CMAKE_BINARY_DIR}/_deps/icu-install)
if (UNIX)
  set(ICU_I18N_LIBRARY ${ICU_INSTALL}/lib64/libicuin.a)
  set(ICU_UC_LIBRARY ${ICU_INSTALL}/lib64/libicuuc.a)
  set(ICU_INCLUDE_DIR ${ICU_INSTALL}/include)
else()
  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(ICU_ARCH )
  else()
    if(MSVC_CXX_ARCHITECTURE_ID MATCHES ARM64)
      set(ICU_ARCH ARM64)
    else()
      set(ICU_ARCH 64)
    endif()
  endif()
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(ICU_I18N_LIBRARY ${ICU_SRC}/src/ICU/lib${ICU_ARCH}/icuind.lib)
    set(ICU_UC_LIBRARY ${ICU_SRC}/src/ICU/lib${ICU_ARCH}/icuucd.lib)
    set(ICU_I18N_DLL ${ICU_SRC}/src/ICU/bin${ICU_ARCH}/icuin72d.dll)
    set(ICU_UC_DLL ${ICU_SRC}/src/ICU/bin${ICU_ARCH}/icuuc72d.dll)
  else()
    set(ICU_I18N_LIBRARY ${ICU_SRC}/src/ICU/lib${ICU_ARCH}/icuin.lib)
    set(ICU_UC_LIBRARY ${ICU_SRC}/src/ICU/lib${ICU_ARCH}/icuuc.lib)
    set(ICU_I18N_DLL ${ICU_SRC}/src/ICU/bin${ICU_ARCH}/icuin72.dll)
    set(ICU_UC_DLL ${ICU_SRC}/src/ICU/bin${ICU_ARCH}/icuuc72.dll)
  endif()
  set(ICU_DATA_DLL ${ICU_SRC}/src/ICU/bin${ICU_ARCH}/icudt72.dll)
  set(ICU_INCLUDE_DIR ${ICU_SRC}/src/ICU/include)
endif()

if(WIN32)
  set(ICU_CONFIGURE_COMMAND echo)
  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(ICU_BUILD_COMMAND msbuild ${ICU_SRC}/src/ICU/source/allinone/allinone.sln /p:Configuration=${CMAKE_BUILD_TYPE} /p:Platform=Win32 /p:SkipUWP=true /m:${CPU_COUNT})
  else()
    if(MSVC_CXX_ARCHITECTURE_ID STREQUAL ARM64)
      set(ICU_BUILD_COMMAND msbuild ${ICU_SRC}/src/ICU/source/allinone/allinone.sln /p:Configuration=${CMAKE_BUILD_TYPE} /p:Platform=ARM64 /p:SkipUWP=true /m:${CPU_COUNT})
    else()
      set(ICU_BUILD_COMMAND msbuild ${ICU_SRC}/src/ICU/source/allinone/allinone.sln /p:Configuration=${CMAKE_BUILD_TYPE} /p:Platform=x64 /p:SkipUWP=true /m:${CPU_COUNT})
    endif()
  endif()
  set(ICU_INSTALL_COMMAND echo)
else()
  set(ICU_CONFIGURE_COMMAND
        ${CMAKE_COMMAND} -E env
        CC=${CMAKE_C_COMPILER}
        CXX=${CMAKE_CXX_COMPILER}
#        CFLAGS=${ICU_CFLAGS}
        CXXFLAGS=${EXTRA_CXX_FLAGS}
        ${ICU_SRC}/src/ICU/source/configure
        --disable-shared
        --enable-static
        --disable-dyload
        --disable-extras
        --disable-tests
        --disable-samples
        --prefix=${ICU_INSTALL}
  )
  set(ICU_BUILD_COMMAND make -j${CPU_COUNT})
  set(ICU_INSTALL_COMMAND make install)
endif()

ExternalProject_Add(ICU
#  DEPENDS icu-data
  PREFIX ${ICU_SRC}
  URL https://github.com/unicode-org/icu/releases/download/release-72-1/icu4c-72_1-src.zip
  URL_HASH SHA512=d4bb1baed99674074f8af024dd159898eddaf4d71bc90f8d95b8448e96aac4b4e8358f755a516bfaf84baa34bf8657dc994459ef3bd72f54496b9ce2b0bd4636
  CONFIGURE_COMMAND ${ICU_CONFIGURE_COMMAND}
  BUILD_COMMAND ${ICU_BUILD_COMMAND}
  TEST_COMMAND ""
  INSTALL_COMMAND ${ICU_INSTALL_COMMAND}
  INSTALL_DIR ${ICU_INSTALL}
  BUILD_BYPRODUCTS ${ICU_I18N_LIBRARY} ${ICU_UC_LIBRARY}
  UPDATE_COMMAND ""
)

# We cannot use find_library because ExternalProject_Add() is performed at build time.
# And to please the property INTERFACE_INCLUDE_DIRECTORIES,
# we make the include directory in advance.
file(MAKE_DIRECTORY ${ICU_INCLUDE_DIR})

add_library(ICU::i18n STATIC IMPORTED GLOBAL)
set_property(TARGET ICU::i18n PROPERTY IMPORTED_LOCATION ${ICU_I18N_LIBRARY})
set_property(TARGET ICU::i18n PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${ICU_INCLUDE_DIR})
add_dependencies(ICU::i18n ICU)

add_library(ICU::uc STATIC IMPORTED GLOBAL)
set_property(TARGET ICU::uc PROPERTY IMPORTED_LOCATION ${ICU_UC_LIBRARY})
set_property(TARGET ICU::uc PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${ICU_INCLUDE_DIR})
add_dependencies(ICU::uc ICU)

if(WIN32)
  add_custom_command(TARGET ICU POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${ICU_DATA_DLL} ${CMAKE_BINARY_DIR}/tests)
  add_custom_command(TARGET ICU POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${ICU_I18N_DLL} ${CMAKE_BINARY_DIR}/tests)
  add_custom_command(TARGET ICU POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${ICU_UC_DLL} ${CMAKE_BINARY_DIR}/tests)
endif()
