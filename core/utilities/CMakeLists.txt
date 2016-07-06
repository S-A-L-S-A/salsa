# Script to compile the farsa utilities library

set(FARSAUTILITIES_SRCS
	src/dataexchange.cpp
	src/farsamiscutilities.cpp
	src/intervals.cpp
	src/logger.cpp
	src/optionparser.cpp
	src/randomgenerator.cpp
	src/utilitieslibinitializer.cpp
	src/workerthread.cpp)
set(FARSAUTILITIES_HDRS
	include/dataexchange.h
	include/dependencysorter.h
	include/farsamiscutilities.h
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

add_library(farsautilities SHARED ${FARSAUTILITIES_SRCS} ${FARSAUTILITIES_HDRS})
add_farsa_version(farsautilities)

# Specifying the the include directories: they are used both here and  exported
# by this library (so that targets linking this one will automatically import
# the include directories declared here). The "${CMAKE_CURRENT_SOURCE_DIR}" is
# to make Eigen available to other libraries
target_include_directories(farsautilities PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                           $<INSTALL_INTERFACE:include/farsa/utilities>)

# Adding dependencies (they are also exported, so targets linking this one will
# automatically link libraries declared here)
target_link_libraries(farsautilities farsaconfiguration Qt5::Core Qt5::Xml Qt5::Concurrent Qt5::Widgets)

# Specifying the public headers of this target
set_property(TARGET farsautilities PROPERTY PUBLIC_HEADER ${FARSAUTILITIES_HDRS})

# Defines to use when compiling the library
target_compile_definitions(farsautilities PRIVATE FARSA_UTIL_BUILDING_DLL)

# Checking if we have to use GSL
if (FARSA_USE_GSL)
	target_compile_definitions(farsautilities PUBLIC FARSA_USE_GSL)
	target_link_libraries(farsautilities ${GSL_LIBRARIES})
	target_include_directories(farsautilities PUBLIC ${GSL_INCLUDE_DIRS})
endif()

# Installation paths
install(TARGETS farsautilities
        EXPORT farsa
        ARCHIVE DESTINATION lib/
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/farsa/utilities
        RUNTIME DESTINATION bin/)

# Adding internal Eigen distribution
install(DIRECTORY Eigen DESTINATION include/farsa/utilities)
install(DIRECTORY unsupported DESTINATION include/farsa/utilities)

# This directory contains unit tests for this library
add_subdirectory(test)
