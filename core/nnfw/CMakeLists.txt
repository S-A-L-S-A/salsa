### CMake script for the farsannfw library

### Resetting variables
SET( FARSA_USEFILE_NNFW_DEFS )
SET( FARSA_USEFILE_NNFW_INCLUDES )
SET( FARSA_USEFILE_NNFW_LINKDIRS )
SET( FARSA_USEFILE_NNFW_LIBRARIES )
SET( FARSA_USEFILE_NNFW_CFLAGS )
SET( FARSA_USEFILE_NNFW_LFLAGS )

### Find source and header files
IF( USE_GRAPHVIZ )
	FILE( GLOB FARSANNFW_SRCS ./src/*.cpp ./ui/*.cpp )
	FILE( GLOB FARSANNFW_HDRS ./include/*.h ./ui/*.h )
ELSE( USE_GRAPHVIZ )
	FILE( GLOB FARSANNFW_SRCS ./src/*.cpp  )
	FILE( GLOB FARSANNFW_HDRS ./include/*.h )
ENDIF( USE_GRAPHVIZ )

### Add headers to FARSANNFW sources for moc'ing up (not needed with Qt5)
IF( NOT FARSA_USE_QT5 )
	QT4_WRAP_CPP( FARSANNFW_SRCS ${FARSANNFW_HDRS} OPTIONS "-nw" )
ENDIF( NOT FARSA_USE_QT5 )

### Adding definitions
ADD_DEFINITIONS( ${FARSA_COMMON_DEFS} -DEIGEN_NO_AUTOMATIC_RESIZING )
### Setting definitions for shared linkage
### This must not go into UseFARSA.cmake but it has to be defined only when BUILDING
ADD_DEFINITIONS( -DFARSA_NNFW_BUILDING_DLL )

### Setting directories with headers
INCLUDE_DIRECTORIES( ${FARSA_COMMON_INCLUDES} ./include ./ui ../utilities ../utilities/include ../configuration/include )

### Setting directories with libraries to link
LINK_DIRECTORIES( ${FARSA_COMMON_LINKDIRS} )

### Creating target
ADD_LIBRARY( farsannfw SHARED ${FARSANNFW_SRCS} ${FARSANNFW_HDRS} )
SET_TARGET_PROPERTIES( farsannfw PROPERTIES VERSION ${VER_MAJ}.${VER_MIN}.${VER_PAT} SOVERSION ${VER_MAJ}.${VER_MIN}  INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib )
ADD_DEPENDENCIES( farsannfw farsautilities farsaconfiguration )

# Setting the Qt5 modules used by this library (if using Qt5)
IF( FARSA_USE_QT5 )
	QT5_USE_MODULES(farsannfw Widgets Concurrent)
ENDIF( FARSA_USE_QT5 )

### Setting libraries to link
TARGET_LINK_LIBRARIES( farsannfw ${FARSA_COMMON_LIBRARIES} farsautilities farsaconfiguration )

### Windows stuffs (suffix for debug libraries)
IF( WIN32 )
	SET_TARGET_PROPERTIES( farsannfw PROPERTIES DEBUG_POSTFIX "d" )
ENDIF( WIN32 )

### Install configuration
INSTALL( TARGETS farsannfw ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin )
IF( WIN32 )
	## Install PDB files necessary for debugging
	INSTALL( FILES ${CMAKE_BINARY_DIR}/nnfw/Debug/farsannfwd.pdb DESTINATION bin CONFIGURATIONS Debug )
ENDIF( WIN32 )
INSTALL( FILES ${FARSANNFW_HDRS} DESTINATION include/FARSA/nnfw )

### Setting the value of variables needed to generate UseFARSA.cmake
LIST( APPEND FARSA_USEFILE_NNFW_INCLUDES "\${FARSA_BASEDIR}/include/FARSA/nnfw" )
LIST( APPEND FARSA_USEFILE_NNFW_LINKDIRS "\${FARSA_BASEDIR}/lib" )
IF( WIN32 )
	LIST( APPEND FARSA_USEFILE_NNFW_LIBRARIES "optimized;farsannfw" "debug;farsannfwd" )
ELSE( WIN32 )
	LIST( APPEND FARSA_USEFILE_NNFW_LIBRARIES farsannfw )
ENDIF( WIN32 )

### Export variables to parent scope. Here we simply set variables to their value and add the PARENT_SCOPE
### directive.
SET( FARSA_USEFILE_NNFW_DEFS ${FARSA_USEFILE_NNFW_DEFS} PARENT_SCOPE )
SET( FARSA_USEFILE_NNFW_INCLUDES ${FARSA_USEFILE_NNFW_INCLUDES} PARENT_SCOPE )
SET( FARSA_USEFILE_NNFW_LINKDIRS ${FARSA_USEFILE_NNFW_LINKDIRS} PARENT_SCOPE )
SET( FARSA_USEFILE_NNFW_LIBRARIES ${FARSA_USEFILE_NNFW_LIBRARIES} PARENT_SCOPE )
SET( FARSA_USEFILE_NNFW_CFLAGS ${FARSA_USEFILE_NNFW_CFLAGS} PARENT_SCOPE )
SET( FARSA_USEFILE_NNFW_LFLAGS ${FARSA_USEFILE_NNFW_LFLAGS} PARENT_SCOPE )
