#-----------------------------*-cmake-*----------------------------------------#
# file   roots/CMakeLists.txt
# author Kelly Thompson <kgt@lanl.gov>
# date   2012 Aug 1
# brief  Generate build project files for roots.
# note   Copyright (C) 2016-2018, Los Alamos National Security, LLC.
#        All rights reserved.
#------------------------------------------------------------------------------#
cmake_minimum_required(VERSION 3.9.0)
project( roots CXX )

# ---------------------------------------------------------------------------- #
# Source files
# ---------------------------------------------------------------------------- #

file( GLOB template_implementations *.t.hh *.i.hh )
file( GLOB sources *.cc )
file( GLOB headers *.hh )
list( REMOVE_ITEM headers ${template_implementations} )

# Make the header files available in the IDE.
if( MSVC_IDE OR ${CMAKE_GENERATOR} MATCHES Xcode )
   list( APPEND sources ${headers} )
 endif()

# ---------------------------------------------------------------------------- #
# Build package library
# ---------------------------------------------------------------------------- #

add_component_library(
   TARGET       Lib_roots
   TARGET_DEPS  Lib_linear
   LIBRARY_NAME ${PROJECT_NAME}
   SOURCES      "${sources}" )

# ---------------------------------------------------------------------------- #
# Installation instructions
# ---------------------------------------------------------------------------- #

install( TARGETS Lib_roots EXPORT draco-targets DESTINATION ${DBSCFGDIR}lib )
install( FILES ${headers} ${template_implementations} DESTINATION
  ${DBSCFGDIR}include/roots )

# ---------------------------------------------------------------------------- #
# Unit tests
# ---------------------------------------------------------------------------- #

if( BUILD_TESTING )
   add_subdirectory( test )
endif()

# ---------------------------------------------------------------------------- #
# Autodoc
# ---------------------------------------------------------------------------- #

process_autodoc_pages()

#------------------------------------------------------------------------------#
# End roots/CMakeLists.txt
#------------------------------------------------------------------------------#
