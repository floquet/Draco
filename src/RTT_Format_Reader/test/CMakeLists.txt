#-----------------------------*-cmake-*----------------------------------------#
# file   RTT_Format_Reader/test/CMakeLists.txt
# author Kelly Thompson <kgt@lanl.gov>
# date   2012 Aug 1
# brief  Generate build project files for RTT_Format_Reader/test.
# note   Copyright (C) 2016-2018, Los Alamos National Security, LLC.
#        All rights reserved.
#------------------------------------------------------------------------------#
project( RTT_Format_Reader_test CXX )

# ---------------------------------------------------------------------------- #
# Source files
# ---------------------------------------------------------------------------- #

file( GLOB test_sources *.cc )
file( GLOB mesh_input_files *.mesh.in )

# ---------------------------------------------------------------------------- #
# Build Unit tests
# ---------------------------------------------------------------------------- #

set(test_deps Lib_RTT_Format_Reader Lib_mesh_element Lib_dsxx )
add_scalar_tests(
   SOURCES "${test_sources}"
   DEPS    "${test_deps}" )

# ---------------------------------------------------------------------------- #
# Copy input files to test directory (binary_dir)
# ---------------------------------------------------------------------------- #

provide_aux_files(
   FILES    "${mesh_input_files}"
   SRC_EXT  ".mesh.in"
   DEST_EXT ".mesh" )

#------------------------------------------------------------------------------#
# End RTT_Format_Reader/test/CMakeLists.txt
#------------------------------------------------------------------------------#
