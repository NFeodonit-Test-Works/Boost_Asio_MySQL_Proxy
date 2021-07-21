# ****************************************************************************
#  Project:  LibCMaker
#  Purpose:  A CMake build scripts for build libraries with CMake
#  Author:   NikitaFeodonit, nfeodonit@yandex.com
# ****************************************************************************
#    Copyright (c) 2017-2021 NikitaFeodonit
#
#    This file is part of the LibCMaker project.
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published
#    by the Free Software Foundation, either version 3 of the License,
#    or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#    See the GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program. If not, see <http://www.gnu.org/licenses/>.
# ****************************************************************************

function(print_system_info)
  if(UNIX AND NOT APPLE AND NOT ANDROID)
    set(system_NAME "Linux")
  elseif(APPLE AND NOT IOS)
    set(system_NAME "macOS")
  elseif(WIN32)
    set(system_NAME "Windows")
  elseif(ANDROID)
    set(system_NAME "Android")
  elseif(IOS)
    set(system_NAME "iOS")
  endif()

  if(MINGW)
    set(system_NAME "${system_NAME}_MinGW")
  endif()

  set(compiler_NAME "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
  if(MSVC)
    set(compiler_NAME
      "${compiler_NAME} ${CMAKE_GENERATOR_PLATFORM} ${CMAKE_GENERATOR_TOOLSET}"
    )
  endif()

  message(STATUS "============================================================")
  message(STATUS "Host system:  ${CMAKE_HOST_SYSTEM}")
  message(STATUS "System:       ${system_NAME}, ${CMAKE_SYSTEM}")
  message(STATUS "Compiler:     ${compiler_NAME}")
  message(STATUS "CMake:        ${CMAKE_VERSION}")
  message(STATUS "Build type:   ${CMAKE_BUILD_TYPE}")
  message(STATUS "============================================================")
endfunction()
