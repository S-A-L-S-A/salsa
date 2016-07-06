# Script to compile the farsa version of QGLViewer

# Adding OpenGL
find_package(OpenGL REQUIRED)

set(FARSAQGLVIEWER_SRCS
	camera.cpp
	constraint.cpp
	frame.cpp
	keyFrameInterpolator.cpp
	manipulatedCameraFrame.cpp
	manipulatedFrame.cpp
	mouseGrabber.cpp
	qglviewer.cpp
	quaternion.cpp
	saveSnapshot.cpp
	vec.cpp
	VRender/BackFaceCullingOptimizer.cpp
	VRender/BSPSortMethod.cpp
	VRender/EPSExporter.cpp
	VRender/Exporter.cpp
	VRender/FIGExporter.cpp
	VRender/gpc.cpp
	VRender/NVector3.cpp
	VRender/ParserGL.cpp
	VRender/Primitive.cpp
	VRender/PrimitivePositioning.cpp
	VRender/TopologicalSortMethod.cpp
	VRender/Vector2.cpp
	VRender/Vector3.cpp
	VRender/VisibilityOptimizer.cpp
	VRender/VRender.cpp)
set(FARSAQGLVIEWER_HDRS
	camera.h
	config.h
	constraint.h
	domUtils.h
	frame.h
	keyFrameInterpolator.h
	manipulatedCameraFrame.h
	manipulatedFrame.h
	mouseGrabber.h
	qglviewer.h
	quaternion.h
	vec.h
	VRender/AxisAlignedBox.h
	VRender/Exporter.h
	VRender/gpc.h
	VRender/NVector3.h
	VRender/Optimizer.h
	VRender/ParserGL.h
	VRender/Primitive.h
	VRender/PrimitivePositioning.h
	VRender/SortMethod.h
	VRender/Types.h
	VRender/Vector2.h
	VRender/Vector3.h
	VRender/VRender.h)

# Adding resources and ui files
qt5_wrap_ui(FARSAQGLVIEWER_SRCS ImageInterface.ui)
qt5_wrap_ui(FARSAQGLVIEWER_SRCS VRenderInterface.ui)

add_library(farsaqglviewer SHARED ${FARSAQGLVIEWER_SRCS} ${FARSAQGLVIEWER_HDRS})
add_farsa_version(farsaqglviewer)

# Specifying the the include directories: they are used both here and  exported
# by this library (so that targets linking this one will automatically import
# the include directories declared here)
target_include_directories(farsaqglviewer PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/VRender>
                           $<INSTALL_INTERFACE:include/farsa/worldsim/qglviewer>)

# Adding dependencies (they are also exported, so targets linking this one will
# automatically link libraries declared here)
target_link_libraries(farsaqglviewer Qt5::Core Qt5::Xml Qt5::Concurrent Qt5::Widgets Qt5::OpenGL ${OPENGL_LIBRARIES})

# Defines to use. The first ones are used only when compiling the library, the
# second one both when compiling the library and targets using the library
target_compile_definitions(farsaqglviewer PRIVATE CREATE_QGLVIEWER_DLL)
target_compile_definitions(farsaqglviewer PUBLIC NO_VECTORIAL_RENDER)

# Specifying the public headers of this target
set_property(TARGET farsaqglviewer PROPERTY PUBLIC_HEADER ${FARSAQGLVIEWER_HDRS})

# Installation paths
install(TARGETS farsaqglviewer
        EXPORT farsa
        ARCHIVE DESTINATION lib/
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/farsa/worldsim/qglviewer
        RUNTIME DESTINATION bin/)

