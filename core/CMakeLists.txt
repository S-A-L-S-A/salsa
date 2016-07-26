# The minimum required version of CMake corresponds to the one in Ubuntu 14.04
# LTS
cmake_minimum_required(VERSION 2.8.12)

# CMake policies
cmake_policy(SET CMP0042 NEW)

# Version Information
set(SALSA_VER_MAJ 1)
set(SALSA_VER_MIN 0)
set(SALSA_VER_PAT 0)

project(salsa CXX)
set(CMAKE_COLOR_MAKEFILE ON)
# We set this to be able to load our script (e.g. for GSL)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

# Including the file with helpers
include(Helpers.cmake)

# A variable to select whether to display developer warnings when compiling or
# not
set(DISPLAY_DEVELOPER_WARNINGS OFF CACHE BOOL "If ON compile-time developer warnings will be shown")
mark_as_advanced(FORCE DISPLAY_DEVELOPER_WARNINGS)

# Enabling test globally
enable_testing()

# Adding the current directory to the include path and instructing CMake to run
# moc automatically when needed (this is needed by QT)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# QT packages we use
find_package(Qt5Test REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Svg REQUIRED)

# RPATH configuration. We always use RPATH (both when building and when
# installing). The following code snippet (with comments and minor changes) is
# taken from https://cmake.org/Wiki/CMake_RPATH_handling
# ==============================================================================
# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH OFF)
# when building, don't use the install RPATH already (but later on when
# installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# add the automatically determined parts of the RPATH which point to directories
# outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
# the RPATH to be used when installing, but only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
   set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()
# enable the installation of RPATH
set(CMAKE_SKIP_INSTALL_RPATH OFF)
# ==============================================================================

# General options
set(SALSA_USE_GSL OFF CACHE BOOL "If ON will use the GNU Scientific Library" )

# Checking if we have to use GSL
if(SALSA_USE_GSL)
	find_package(GSL REQUIRED)
endif()

# Definitions for platforms
if(UNIX)
	if(APPLE)
		add_definitions(-DSALSA_MAC)
	else()
		add_definitions(-DSALSA_LINUX)
	endif()
elseif(WIN32)
	add_definitions(-DSALSA_WIN)
endif()

# The variable containing the path relative to CMAKE_INSTALL_PREFIX for
# miscellaneous files. This is used by some subprojects, so defining it here
if(WIN32)
	set(MISC_FILES_DESTINATION .)
else()
	set(MISC_FILES_DESTINATION share/salsa)
endif()

# Adding all subdirectories
add_subdirectory(configuration)
add_subdirectory(utilities)
add_subdirectory(worldsim)
add_subdirectory(ga)
add_subdirectory(experiments)
add_subdirectory(libpluginhelper)
add_subdirectory(pluginhelper)
add_subdirectory(total99)

# Setting installation directory of the export
install(EXPORT salsa DESTINATION share/salsa FILE exportedSalsaTargets.cmake)

# Generating SALSAConfig.cmake
set(SALSA_USEFILE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(SALSA_USEFILE_INSTALL_MISC "${SALSA_USEFILE_INSTALL_PREFIX}/${MISC_FILES_DESTINATION}")
set(SALSA_TEST_PLUGIN_SCRIPT_PATH "${SALSA_USEFILE_INSTALL_MISC}/testPlugin.py")
set(SALSA_TOTAL99_EXECUTABLE_PATH "${SALSA_USEFILE_INSTALL_PREFIX}/bin/total99")
configure_file("salsaConfig.cmake.in" "${CMAKE_BINARY_DIR}/salsaConfig.cmake" @ONLY)

# Setting install destination for some files
install(FILES "GPL.txt" DESTINATION "${MISC_FILES_DESTINATION}")
install(FILES "COPYING" DESTINATION "${MISC_FILES_DESTINATION}")
install(FILES "INSTALL" DESTINATION "${MISC_FILES_DESTINATION}")
install(FILES "README" DESTINATION "${MISC_FILES_DESTINATION}")
install(FILES "${CMAKE_BINARY_DIR}/salsaConfig.cmake" DESTINATION "${MISC_FILES_DESTINATION}")
install(FILES "FindGSL.cmake" DESTINATION "${MISC_FILES_DESTINATION}")
install(FILES "tests/testPlugin.py" DESTINATION "${MISC_FILES_DESTINATION}")
