# Script to compile the salsa experiments library

set(SALSAGA_SRCS
	src/evoalgo.cpp
	src/evorobotga.cpp
	src/evorobotgastatistics.cpp
	src/evorobotintgenotype.cpp
	src/galibinitializer.cpp)
set(SALSAGA_HDRS
	include/evoalgo.h
	include/evorobotga.h
	include/evorobotgastatistics.h
	include/evorobotintgenotype.h
	include/gaconfig.h
	include/gaevaluator.h
	include/gaexceptions.h
	include/genotypetophenotype.h)

add_library(salsaga SHARED ${SALSAGA_SRCS} ${SALSAGA_HDRS})
add_salsa_version(salsaga)

# Specifying the the include directories: they are used both here and  exported
# by this library (so that targets linking this one will automatically import
# the include directories declared here). CMAKE_SOURCE_DIR is needed for
# salsaversion.h
target_include_directories(salsaga PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
                           $<INSTALL_INTERFACE:include/salsa/ga>
                           $<INSTALL_INTERFACE:include/salsa>)

# Adding dependencies (they are also exported, so targets linking this one will
# automatically link libraries declared here)
target_link_libraries(salsaga salsautilities salsaconfiguration Qt5::Core Qt5::Concurrent Qt5::Widgets)

# Specifying the public headers of this target
set_property(TARGET salsaga PROPERTY PUBLIC_HEADER ${SALSAGA_HDRS})

# This definition is used only when building
target_compile_definitions(salsaga PRIVATE SALSA_GA_BUILDING_DLL)

# Installation paths
install(TARGETS salsaga
        EXPORT salsa
        ARCHIVE DESTINATION lib/
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/salsa/ga
        RUNTIME DESTINATION bin/)

# This directory contains unit tests for this library
add_subdirectory(test)
