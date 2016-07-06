# Script to compile the farsa experiments library

set(FARSAGA_SRCS
	src/evoalgo.cpp
	src/evorobotga.cpp
	src/evorobotgastatistics.cpp
	src/evorobotintgenotype.cpp
	src/galibinitializer.cpp)
set(FARSAGA_HDRS
	include/evoalgo.h
	include/evorobotga.h
	include/evorobotgastatistics.h
	include/evorobotintgenotype.h
	include/gaconfig.h
	include/gaevaluator.h
	include/gaexceptions.h
	include/genotypetophenotype.h)

add_library(farsaga SHARED ${FARSAGA_SRCS} ${FARSAGA_HDRS})
add_farsa_version(farsaga)

# Specifying the the include directories: they are used both here and  exported
# by this library (so that targets linking this one will automatically import
# the include directories declared here)
target_include_directories(farsaga PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include/farsa/ga>)

# Adding dependencies (they are also exported, so targets linking this one will
# automatically link libraries declared here)
target_link_libraries(farsaga farsautilities farsaconfiguration Qt5::Core Qt5::Concurrent Qt5::Widgets)

# Specifying the public headers of this target
set_property(TARGET farsaga PROPERTY PUBLIC_HEADER ${FARSAGA_HDRS})

# This definition is used only when building
target_compile_definitions(farsaga PRIVATE FARSA_GA_BUILDING_DLL)

# Installation paths
install(TARGETS farsaga
        EXPORT farsa
        ARCHIVE DESTINATION lib/
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/farsa/ga
        RUNTIME DESTINATION bin/)

# This directory contains unit tests for this library
add_subdirectory(test)
