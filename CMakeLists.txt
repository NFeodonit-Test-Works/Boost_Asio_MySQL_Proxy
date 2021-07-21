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

set(prj_options "${CMAKE_CURRENT_LIST_DIR}/.prj_options.cmake")
if(EXISTS ${prj_options})
  include(${prj_options})
endif()

if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  option(cmr_DEFAULT_CMAKE_INSTALL_PREFIX "cmr_DEFAULT_CMAKE_INSTALL_PREFIX" ON)
endif()

cmake_minimum_required(VERSION 3.9)

project(Boost_Asio_MySQL_Proxy CXX)

option(BUILD_SHARED_LIBS "BUILD_SHARED_LIBS" OFF)
option(CMAKE_VERBOSE_MAKEFILE "CMAKE_VERBOSE_MAKEFILE" OFF)
option(cmr_PRINT_DEBUG "cmr_PRINT_DEBUG" OFF)


#-----------------------------------------------------------------------
# Check compiler's version
#-----------------------------------------------------------------------

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
    AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0.0)
  message(FATAL_ERROR "GNU g++ compiler version 9+ is required.")
endif()

if((CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"
     OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
     AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0.0)
  message(FATAL_ERROR "Clang compiler version 9+ is required.")
endif()

if(MSVC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.15)
  message(FATAL_ERROR "MSVC compiler version 19.15+ is required.")
endif()



#-----------------------------------------------------------------------
# Boost_Asio_MySQL_Proxy project
#-----------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(print_system_info)
print_system_info()
include(compile_flags)
include(find_libraries)


#-----------------------------------------------------------------------
# Boost_Asio_MySQL_Proxy sources
#-----------------------------------------------------------------------

set(bamp_EXE_NAME "boost-asio-mysql-proxy")

add_executable(${bamp_EXE_NAME} "")
set_target_properties(${bamp_EXE_NAME} PROPERTIES
  CXX_STANDARD 17
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
  Boost::thread Boost::chrono
  Boost::system Boost::date_time Boost::atomic
)
