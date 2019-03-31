# ****************************************************************************
#  Project:  Boost_Asio_MySQL_Proxy
#  Purpose:  Test project
#  Author:   NikitaFeodonit, nfeodonit@yandex.com
# ****************************************************************************
#    Copyright (c) 2019 NikitaFeodonit
#
#    This file is part of the Boost_Asio_MySQL_Proxy project.
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

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(Boost_Asio_MySQL_Proxy CXX)

# Used by LibCMaker_Boost for Boost building.
set(CMAKE_CXX_STANDARD 11)
if(MSVC)
  set(CMAKE_CXX_STANDARD 14)
endif()


#-----------------------------------------------------------------------
# LibCMaker_Boost
#-----------------------------------------------------------------------

option(cmr_PRINT_DEBUG "cmr_PRINT_DEBUG" OFF)


#-----------------------------------------------------------------------
# Compiler flags
#-----------------------------------------------------------------------

option(cmr_USE_MSVC_STATIC_RUNTIME "cmr_USE_MSVC_STATIC_RUNTIME" ON)

if(cmr_USE_MSVC_STATIC_RUNTIME AND MSVC AND NOT BUILD_SHARED_LIBS)
  # Set MSVC static runtime flags for all configurations.
  # See:
  # https://stackoverflow.com/a/20804336
  # https://stackoverflow.com/a/14172871
  foreach(cfg "" ${CMAKE_CONFIGURATION_TYPES})
    set(c_flag_var   CMAKE_C_FLAGS)
    set(cxx_flag_var CMAKE_CXX_FLAGS)
    if(cfg)
      string(TOUPPER ${cfg} cfg_upper)
      set(c_flag_var   "${c_flag_var}_${cfg_upper}")
      set(cxx_flag_var "${cxx_flag_var}_${cfg_upper}")
    endif()
    if(${c_flag_var} MATCHES "/MD")
      string(REPLACE "/MD" "/MT" ${c_flag_var} "${${c_flag_var}}")
      set(${c_flag_var} ${${c_flag_var}} CACHE STRING
        "Flags used by the C compiler during ${cfg_upper} builds." FORCE
      )
    endif()
    if(${cxx_flag_var} MATCHES "/MD")
      string(REPLACE "/MD" "/MT" ${cxx_flag_var} "${${cxx_flag_var}}")
      set(${cxx_flag_var} ${${cxx_flag_var}} CACHE STRING
        "Flags used by the CXX compiler during ${cfg_upper} builds." FORCE
      )
    endif()
  endforeach()
endif()


#-----------------------------------------------------------------------
# Configure the find_package()
#-----------------------------------------------------------------------

# Set CMake's search path for find_*() commands.
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_INSTALL_PREFIX}")


#-----------------------------------------------------------------------
# Set path vars
#-----------------------------------------------------------------------

set(LibCMaker_LIB_DIR "${CMAKE_CURRENT_LIST_DIR}/libs")
set(cmr_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")

if(NOT cmr_DOWNLOAD_DIR)
  set(cmr_DOWNLOAD_DIR "${PROJECT_BINARY_DIR}/download")
endif()
if(NOT cmr_UNPACKED_DIR)
  set(cmr_UNPACKED_DIR "${cmr_DOWNLOAD_DIR}/unpacked")
endif()


#-----------------------------------------------------------------------
# LibCMaker settings
#-----------------------------------------------------------------------

set(LibCMaker_DIR "${LibCMaker_LIB_DIR}/LibCMaker")
list(APPEND CMAKE_MODULE_PATH "${LibCMaker_DIR}/cmake")
include(cmr_find_package)


#-----------------------------------------------------------------------
# Download, configure, build, install and find the required libraries
#-----------------------------------------------------------------------

set(BOOST_lib_DIR "${CMAKE_CURRENT_LIST_DIR}/libs/LibCMaker_Boost")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/cmr_build_boost.cmake")


#-----------------------------------------------------------------------
# Boost_Asio_MySQL_Proxy
#-----------------------------------------------------------------------

set(bamp_EXE_NAME "boost-asio-mysql-proxy")

add_executable(${bamp_EXE_NAME} "")
set_target_properties(${bamp_EXE_NAME} PROPERTIES
  CXX_STANDARD 11
)

target_compile_definitions(${bamp_EXE_NAME} PRIVATE
  BOOST_ASIO_NO_DEPRECATED
#  PROXY_PACKET_DEBUG  # Set to turn the debug printers on.
)

target_include_directories(${bamp_EXE_NAME} PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/src"
)

target_sources(${bamp_EXE_NAME} PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/src/main.cpp"

  "${CMAKE_CURRENT_LIST_DIR}/src/connection.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/connection_manager.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/packet.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/packet_logger.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/server.cpp"

  "${CMAKE_CURRENT_LIST_DIR}/src/connection.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/connection_manager.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/packet.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/packet_logger.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/server.hpp"
)


#-----------------------------------------------------------------------
# Link to the libraries
#-----------------------------------------------------------------------

# Boost
# 'Boost::boost' is target for header-only dependencies.
# About 'Boost::disable_autolinking' see 'FindBoost.cmake'.
# TODO: Use the component autodetection from 'FindBoost.cmake'.
target_link_libraries(${bamp_EXE_NAME} PRIVATE
  Boost::disable_autolinking Boost::boost
  Boost::regex Boost::coroutine Boost::context Boost::thread Boost::chrono
  Boost::system Boost::date_time Boost::atomic
)
