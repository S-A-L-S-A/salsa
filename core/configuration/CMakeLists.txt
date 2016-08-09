# Script to compile the salsa configuration library

set(SALSACONFIGURATION_SRCS
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
set(SALSACONFIGURATION_HDRS
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

add_library(salsaconfiguration SHARED ${SALSACONFIGURATION_SRCS} ${SALSACONFIGURATION_HDRS})
add_salsa_version(salsaconfiguration)

# Specifying the the include directories: they are used both here and  exported
# by this library (so that targets linking this one will automatically import
# the include directories declared here). CMAKE_BINARY_DIR is needed for
# salsaversion.h. We only include it here because this library is used
# everywhere in SALSA, so other libraries will inherit these include directories
target_include_directories(salsaconfiguration PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
                           $<INSTALL_INTERFACE:include/salsa/configuration>
                           $<INSTALL_INTERFACE:include/salsa>)

# Adding dependencies (they are also exported, so targets linking this one will
# automatically link libraries declared here)
target_link_libraries(salsaconfiguration Qt5::Core Qt5::Xml Qt5::Concurrent Qt5::Widgets)

# Specifying the public headers of this target
set_property(TARGET salsaconfiguration PROPERTY PUBLIC_HEADER ${SALSACONFIGURATION_HDRS})

# Defines to use when compiling the library
target_compile_definitions(salsaconfiguration PRIVATE SALSA_CONF_BUILDING_DLL)

# Installation paths
install(TARGETS salsaconfiguration
        EXPORT salsa
        ARCHIVE DESTINATION lib/
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/salsa/configuration
        RUNTIME DESTINATION bin/)

# This directory contains unit tests for this library
add_subdirectory(test)
