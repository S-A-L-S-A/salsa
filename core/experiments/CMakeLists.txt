# Script to compile the salsa experiments library

set(SALSAEXPERIMENTS_SRCS
	evorobot/src/abstracttest.cpp
	evorobot/src/displaycontroller.cpp
	evorobot/src/embodiedagent.cpp
	evorobot/src/evodataviewer.cpp
	evorobot/src/evoga.cpp
	evorobot/src/evorobotcomponent.cpp
	evorobot/src/evorobotexperiment.cpp
	evorobot/src/evorobotviewer.cpp
	evorobot/src/holisticviewer.cpp
	evorobot/src/renderer2d.cpp
	evorobot/src/render2dgui.cpp
	evorobot/src/tests.cpp
	src/arena.cpp
	src/baseexperiment.cpp
	src/baseexperimentgui.cpp
	src/controller.cpp
	src/controllerinputoutput.cpp
	src/epuckmotors.cpp
	src/epucksensors.cpp
	src/evonet.cpp
	src/evonetiterator.cpp
	src/evonetui.cpp
	src/experimentslibinitializer.cpp
	src/kheperamotors.cpp
	src/kheperasensors.cpp
	src/marxbotmotors.cpp
	src/marxbotsensors.cpp
	src/motors.cpp
	src/musclepair.cpp
	src/neuralnetiterator.cpp
	src/noisydevice.cpp
	src/renderworldwrapperwidget.cpp
	src/robots.cpp
	src/sensors.cpp
	src/total99resources.cpp
	src/wheeledexperimenthelper.cpp)
set(SALSAEXPERIMENTS_HDRS
	evorobot/include/abstracttest.h
	evorobot/include/displaycontroller.h
	evorobot/include/embodiedagent.h
	evorobot/include/evodataviewer.h
	evorobot/include/evoga.h
	evorobot/include/evorobotcomponent.h
	evorobot/include/evorobotexperiment.h
	evorobot/include/evorobotviewer.h
	evorobot/include/holisticviewer.h
	evorobot/include/render2dgui.h
	evorobot/include/renderer2d.h
	evorobot/include/tests.h
	include/arena.h
	include/baseexperiment.h
	include/baseexperimentgui.h
	include/controller.h
	include/controllerinputoutput.h
	include/controlleriterator.h
	include/epuckmotors.h
	include/epucksensors.h
	include/evonet.h
	include/evonetiterator.h
	include/evonetui.h
	include/experimentsconfig.h
	include/salsaplugin.h
	include/kheperamotors.h
	include/kheperasensors.h
	include/marxbotmotors.h
	include/marxbotsensors.h
	include/motors.h
	include/musclepair.h
	include/neuralnetiterator.h
	include/noisydevice.h
	include/projector.h
	include/renderworldwrapperwidget.h
	include/robots.h
	include/sensors.h
	include/total99resources.h
	include/wheeledexperimenthelper.h)
set(SALSAEXPERIMENTS_QRCS
	evorobot/images/images.qrc)

qt5_add_resources(SALSAEXPERIMENTS_SRCS ${SALSAEXPERIMENTS_QRCS})

add_library(salsaexperiments SHARED ${SALSAEXPERIMENTS_SRCS} ${SALSAEXPERIMENTS_HDRS})
add_salsa_version(salsaexperiments)

# Specifying the the include directories: they are used both here and  exported
# by this library (so that targets linking this one will automatically import
# the include directories declared here). CMAKE_SOURCE_DIR is needed for
# salsaversion.h
target_include_directories(salsaexperiments PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/evorobot/include>
                           $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
                           $<INSTALL_INTERFACE:include/salsa/experiments>
                           $<INSTALL_INTERFACE:include/salsa>)

# Adding dependencies (they are also exported, so targets linking this one will
# automatically link libraries declared here)
target_link_libraries(salsaexperiments salsaworldsim salsaga salsautilities salsaconfiguration Qt5::Core Qt5::Xml Qt5::Concurrent Qt5::Widgets Qt5::OpenGL)

# Specifying the public headers of this target
set_property(TARGET salsaexperiments PROPERTY PUBLIC_HEADER ${SALSAEXPERIMENTS_HDRS})

# Defines to use when compiling the library
target_compile_definitions(salsaexperiments PRIVATE SALSA_EXPERIMENTS_BUILDING_DLL)

# Installation paths
install(TARGETS salsaexperiments
        EXPORT salsa
        ARCHIVE DESTINATION lib/
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/salsa/experiments
        RUNTIME DESTINATION bin/)

# This directory contains unit tests for this library
add_subdirectory(test)
