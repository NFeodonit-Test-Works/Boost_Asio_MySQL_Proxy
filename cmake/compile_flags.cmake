# ****************************************************************************
#   Project:  Feographia
#   Purpose:  The application to work with the biblical text
#   Author:   NikitaFeodonit, nfeodonit@yandex.com
# ****************************************************************************
#     Copyright (c) 2017-2020 NikitaFeodonit
#
#     This file is part of the Feographia project.
#
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published
#     by the Free Software Foundation, either version 3 of the License,
#     or (at your option) any later version.
#
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#     See the GNU General Public License for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with this program. If not, see <http://www.gnu.org/licenses/>.
# ****************************************************************************

# Used by LibCMaker_Boost.
set(CMAKE_CXX_STANDARD 17)

if(NOT BUILD_SHARED_LIBS)
  option(cmr_USE_STATIC_RUNTIME "cmr_USE_STATIC_RUNTIME" ON)
endif()

# Set compile flags.
if(MSVC)
  # Determine MSVC runtime library flag
  set(MSVC_LIB_USE "/MD")
  set(MSVC_LIB_REPLACE "/MT")
  if(cmr_USE_STATIC_RUNTIME AND NOT BUILD_SHARED_LIBS)
    set(MSVC_LIB_USE "/MT")
    set(MSVC_LIB_REPLACE "/MD")
  endif()
  # Set MSVC runtime flags for all configurations
  # See:
  # https://stackoverflow.com/a/20804336
  # https://stackoverflow.com/a/14172871
  foreach(cfg "" ${CMAKE_CONFIGURATION_TYPES})
    set(c_flag_var CMAKE_C_FLAGS)
    set(cxx_flag_var CMAKE_CXX_FLAGS)
    if(cfg)
      string(TOUPPER ${cfg} cfg_upper)
      set(c_flag_var   "${c_flag_var}_${cfg_upper}")
      set(cxx_flag_var "${cxx_flag_var}_${cfg_upper}")
    endif()
    if(${c_flag_var} MATCHES ${MSVC_LIB_REPLACE})
      string(REPLACE
        ${MSVC_LIB_REPLACE} ${MSVC_LIB_USE} ${c_flag_var} "${${c_flag_var}}"
      )
      set(${c_flag_var} ${${c_flag_var}} CACHE STRING
        "Flags used by the C compiler during ${cfg_upper} builds." FORCE
      )
    endif()
    if(${cxx_flag_var} MATCHES ${MSVC_LIB_REPLACE})
      string(REPLACE
        ${MSVC_LIB_REPLACE} ${MSVC_LIB_USE} ${cxx_flag_var} "${${cxx_flag_var}}"
      )
      set(${cxx_flag_var} ${${cxx_flag_var}} CACHE STRING
        "Flags used by the CXX compiler during ${cfg_upper} builds." FORCE
      )
    endif()
  endforeach()

elseif(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
  if(NOT ANDROID AND NOT IOS
      AND cmr_USE_STATIC_RUNTIME AND NOT BUILD_SHARED_LIBS)
    if(MINGW)
      set(STATIC_LINKER_FLAGS "-static")
    elseif(NOT ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND APPLE))
      set(STATIC_LINKER_FLAGS "-static-libgcc -static-libstdc++")
    endif()
    set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} ${STATIC_LINKER_FLAGS}"
    )
    set(CMAKE_SHARED_LINKER_FLAGS
      "${CMAKE_SHARED_LINKER_FLAGS} ${STATIC_LINKER_FLAGS}"
    )
  endif()
endif()

option(cmr_BUILD_MULTIPROC "cmr_BUILD_MULTIPROC" ON)

if(cmr_BUILD_MULTIPROC)
  # Enable /MP flag for Visual Studio 2008 and greater.
  if(MSVC AND MSVC_VERSION GREATER 1400)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP${cmr_BUILD_MULTIPROC_CNT}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP${cmr_BUILD_MULTIPROC_CNT}")
  endif()
endif()
