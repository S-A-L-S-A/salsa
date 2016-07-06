# Script to compile the farsa configuration library

set(FARSACONFIGURATION_SRCS
	src/component.cpp
	src/componentdescriptors.cpp
	src/componentresource.cpp
	src/configurationhelper.cpp
	src/configurationkey.cpp
	src/configurationlibinitializer.cpp
	src/configurationmanager.cpp
	src/configurationnode.cpp
	src/configurationobserver.cpp
	src/inifilesupport.cpp
	src/parametersfileloadersaver.cpp
	src/typesdb.cpp
	src/xmlfilesupport.cpp)
set(FARSACONFIGURATION_HDRS
	include/private/inifilesupport.h
	include/private/xmlfilesupport.h
	include/baseexception.h
	include/component.h
	include/componentandstatus.h
	include/componentdescriptors.h
	include/componentresource.h
	include/componentui.h
	include/configurationconfig.h
	include/configurationexceptions.h
	include/configurationhelper.h
	include/configurationkey.h
	include/configurationmanager.h
	include/configurationnode.h
	include/configurationobserver.h
	include/configurationwidget.h
	include/parametersfileloadersaver.h
	include/typesdb.h)

add_library(farsaconfiguration SHARED ${FARSACONFIGURATION_SRCS} ${FARSACONFIGURATION_HDRS})
add_farsa_version(farsaconfiguration)

# Specifying the the include directories: they are used both here and  exported
# by this library (so that targets linking this one will automatically import
# the include directories declared here)
target_include_directories(farsaconfiguration PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include/farsa/configuration>)

# Adding dependencies (they are also exported, so targets linking this one will
# automatically link libraries declared here)
target_link_libraries(farsaconfiguration Qt5::Core Qt5::Xml Qt5::Concurrent Qt5::Widgets)

# Specifying the public headers of this target
set_property(TARGET farsaconfiguration PROPERTY PUBLIC_HEADER ${FARSACONFIGURATION_HDRS})

# Defines to use when compiling the library
target_compile_definitions(farsaconfiguration PRIVATE FARSA_CONF_BUILDING_DLL)

# Installation paths
install(TARGETS farsaconfiguration
        EXPORT farsa
        ARCHIVE DESTINATION lib/
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/farsa/configuration
        RUNTIME DESTINATION bin/)

# This directory contains unit tests for this library
add_subdirectory(test)
