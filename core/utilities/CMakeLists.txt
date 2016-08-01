# Script to compile the salsa utilities library

set(SALSAUTILITIES_SRCS
	src/dataexchange.cpp
	src/salsamiscutilities.cpp
	src/intervals.cpp
	src/logger.cpp
	src/optionparser.cpp
	src/randomgenerator.cpp
	src/utilitieslibinitializer.cpp
	src/workerthread.cpp)
set(SALSAUTILITIES_HDRS
	include/dataexchange.h
	include/dependencysorter.h
	include/salsamiscutilities.h
	include/flowcontrol.h
	include/helperresources.h
	include/intervals.h
	include/logger.h
	include/mathutils.h
	include/optionparser.h
	include/randomgenerator.h
	include/simpletimer.h
	include/updatetrigger.h
	include/utilitiesconfig.h
	include/utilitiesexceptions.h
	include/workerthread.h)

add_library(salsautilities SHARED ${SALSAUTILITIES_SRCS} ${SALSAUTILITIES_HDRS})
add_salsa_version(salsautilities)

# Specifying the the include directories: they are used both here and  exported
# by this library (so that targets linking this one will automatically import
# the include directories declared here). The "${CMAKE_CURRENT_SOURCE_DIR}" is
# to make Eigen available to other libraries
target_include_directories(salsautilities PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                           $<INSTALL_INTERFACE:include/salsa/utilities>)

# Adding dependencies (they are also exported, so targets linking this one will
# automatically link libraries declared here)
target_link_libraries(salsautilities salsaconfiguration Qt5::Core Qt5::Xml Qt5::Concurrent Qt5::Widgets)

# Specifying the public headers of this target
set_property(TARGET salsautilities PROPERTY PUBLIC_HEADER ${SALSAUTILITIES_HDRS})

# Defines to use when compiling the library
target_compile_definitions(salsautilities PRIVATE SALSA_UTIL_BUILDING_DLL)

# Checking if we have to use GSL
if (SALSA_USE_GSL)
	target_compile_definitions(salsautilities PUBLIC SALSA_USE_GSL)
	target_link_libraries(salsautilities ${GSL_LIBRARIES})
	target_include_directories(salsautilities PUBLIC ${GSL_INCLUDE_DIRS})
endif()

# Installation paths
install(TARGETS salsautilities
        EXPORT salsa
        ARCHIVE DESTINATION lib/
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/salsa/utilities
        RUNTIME DESTINATION bin/)

# Adding internal Eigen distribution
install(DIRECTORY Eigen DESTINATION include/salsa/utilities)
install(DIRECTORY unsupported DESTINATION include/salsa/utilities)

# This directory contains unit tests for this library
add_subdirectory(test)
