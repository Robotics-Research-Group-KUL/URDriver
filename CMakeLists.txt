#
# The find_package macro for Orocos-RTT works best with
# cmake >= 2.6.3
#


cmake_minimum_required(VERSION 2.6.3)

#
# This creates a standard cmake project. You may extend this file with
# any cmake macro you see fit.
#
project(URDriver)
add_definitions(-std=c++11)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE} -Wall")#this disable debug stuff
#set( CMAKE_VERBOSE_MAKEFILE on )


file(GLOB_RECURSE EXTRA_FILES *.ops *.lua *.py cpf/* )
add_custom_target(${PROJECT_NAME}_OTHER_FILES ALL WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} SOURCES ${EXTRA_FILES})

# Use 'source orocos_toolchain/env.sh' if the command below fails:
find_package(OROCOS-RTT QUIET)
if (NOT OROCOS-RTT_FOUND)
  message (FATAL_ERROR "\nCould not find Orocos. Please use the shell command\n 'source orocos_toolchain/env.sh' and then run cmake again.")
endif()
find_package(catkin REQUIRED)
find_package(rostest REQUIRED)
find_package(PkgConfig REQUIRED)


pkg_check_modules(LEXP REQUIRED libexplain)
include_directories(include ${LEXP_INCLUDE_DIRS})
# Defines the orocos_* cmake macros. See that file for additional
# documentation.
include( ${OROCOS-RTT_USE_FILE_PATH}/UseOROCOS-RTT.cmake )

# Rest of the configuration is in src/
add_subdirectory( src )





# add a target to generate API documentation with Doxygen
find_package(Doxygen)
#if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
#endif(DOXYGEN_FOUND)

orocos_generate_package()


#catkin_add_gtest(swapTestInt_utest test/swapTestInt_utest.cpp)
