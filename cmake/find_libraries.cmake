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

#-----------------------------------------------------------------------
# Set path vars
#-----------------------------------------------------------------------

if(UNIX AND NOT APPLE AND NOT ANDROID)
  set(system_NAME "linux")
elseif(APPLE AND NOT IOS)
  set(system_NAME "macos")
elseif(WIN32)
  set(system_NAME "windows")
elseif(ANDROID)
  set(system_NAME "android")
elseif(IOS)
  set(system_NAME "ios")
endif()

if(MINGW)
  set(system_NAME "${system_NAME}_MinGW")
endif()

set(compiler_NAME "${CMAKE_CXX_COMPILER_ID}_${CMAKE_CXX_COMPILER_VERSION}")
if(MSVC)
  set(compiler_NAME
    "${compiler_NAME}_${CMAKE_GENERATOR_PLATFORM}_${CMAKE_GENERATOR_TOOLSET}"
  )
endif()

set(platform_NAME "${system_NAME}_${compiler_NAME}_${CMAKE_BUILD_TYPE}")

set(libs_DIR "${PROJECT_SOURCE_DIR}/libs")
set(build_libs_DIR "${PROJECT_BINARY_DIR}/build_libs")
set(cmr_LIBCMAKER_WORK_DIR "${PROJECT_SOURCE_DIR}/.libcmaker")

if(NOT cmr_DOWNLOAD_DIR)
  set(cmr_DOWNLOAD_DIR "${PROJECT_SOURCE_DIR}/.downloads")
endif()

if(NOT cmr_UNPACKED_DIR)
  set(cmr_UNPACKED_DIR
    "${cmr_DOWNLOAD_DIR}/.unpacked${platform_DIR}${compiler_DIR}"
  )
endif()

if(NOT cmr_BUILD_DIR)
  set(cmr_BUILD_DIR "${build_libs_DIR}/LibCMaker")
endif()

if(NOT cmr_HOST_UNPACKED_DIR)
  set(cmr_HOST_UNPACKED_DIR "${cmr_UNPACKED_DIR}/.host_tools_sources")
endif()
if(NOT cmr_HOST_BUILD_DIR)
  set(cmr_HOST_BUILD_DIR "${cmr_BUILD_DIR}/.build_host_tools")
endif()

if(cmr_DEFAULT_CMAKE_INSTALL_PREFIX)
  set(cmr_INSTALL_DIR "${cmr_LIBCMAKER_WORK_DIR}/${platform_NAME}/install")
else()
  set(cmr_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
endif()


#-----------------------------------------------------------------------
# Configure to find_package()
#-----------------------------------------------------------------------

# Set CMake's search path for find_*() commands.
list(APPEND CMAKE_PREFIX_PATH "${cmr_INSTALL_DIR}")

if(ANDROID OR IOS)
  list(APPEND CMAKE_FIND_ROOT_PATH "${cmr_INSTALL_DIR}")
endif()


#-----------------------------------------------------------------------
# LibCMaker settings
#-----------------------------------------------------------------------

set(LibCMaker_LIB_DIR "${libs_DIR}")
set(LibCMaker_DIR "${LibCMaker_LIB_DIR}/LibCMaker")

list(APPEND CMAKE_MODULE_PATH
  "${LibCMaker_DIR}/cmake"
)

include(cmr_find_package)


#-----------------------------------------------------------------------
# Download, configure, build, install and find the required libraries
#-----------------------------------------------------------------------

option(BOOST_WITHOUT_ICU "Disable Unicode/ICU support in Regex" ON)

#set(USE_BOOST ON)  # TODO: rename to USE_BOOST_FILESYSTEM
set(BOOST_lib_COMPONENTS
  thread chrono system date_time atomic
  CACHE STRING "BOOST_lib_COMPONENTS"
)
include(${LibCMaker_LIB_DIR}/LibCMaker_Boost/cmr_build_boost.cmake)
