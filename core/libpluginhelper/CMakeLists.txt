# Script to compile the salsa library with the core functionalities of the
# pluginhelper executable

set(SALSALIBPLUGINHELPER_SRCS
	src/commandlineparser.cpp
	src/pluginhelper.cpp)
set(SALSALIBPLUGINHELPER_HDRS
	include/commandlineparser.h
	include/pluginhelper.h)

add_library(salsalibpluginhelper STATIC ${SALSALIBPLUGINHELPER_SRCS} ${SALSALIBPLUGINHELPER_HDRS})
add_salsa_version(salsalibpluginhelper)

# Specifying the the include directories: they are used both here and  exported
# by this library (so that targets linking this one will automatically import
# the include directories declared here)
target_include_directories(salsalibpluginhelper PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include/salsa/libpluginhelper>)

# Adding dependencies (they are also exported, so targets linking this one will
# automatically link libraries declared here)
target_link_libraries(salsalibpluginhelper salsautilities salsaconfiguration Qt5::Core Qt5::Xml)

# Turning AUTOMOC off for the the salsalibpluginhelper target as there are no
# classes to be parsed and it creates problems (there are files that contain
# Q_OBJECT in a string...)
set_target_properties(salsalibpluginhelper PROPERTIES AUTOMOC FALSE)

# This target has no plugin headers

# Installation paths
install(TARGETS salsalibpluginhelper
        EXPORT salsa
        ARCHIVE DESTINATION lib/
        LIBRARY DESTINATION lib/
        RUNTIME DESTINATION bin/)

# This directory contains unit tests for this library
add_subdirectory(test)
