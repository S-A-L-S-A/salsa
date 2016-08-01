### CMake script for the salsannfw library

### Resetting variables
SET( SALSA_USEFILE_NNFW_DEFS )
SET( SALSA_USEFILE_NNFW_INCLUDES )
SET( SALSA_USEFILE_NNFW_LINKDIRS )
SET( SALSA_USEFILE_NNFW_LIBRARIES )
SET( SALSA_USEFILE_NNFW_CFLAGS )
SET( SALSA_USEFILE_NNFW_LFLAGS )

### Find source and header files
IF( USE_GRAPHVIZ )
	FILE( GLOB SALSANNFW_SRCS ./src/*.cpp ./ui/*.cpp )
	FILE( GLOB SALSANNFW_HDRS ./include/*.h ./ui/*.h )
ELSE( USE_GRAPHVIZ )
	FILE( GLOB SALSANNFW_SRCS ./src/*.cpp  )
	FILE( GLOB SALSANNFW_HDRS ./include/*.h )
ENDIF( USE_GRAPHVIZ )

### Add headers to SALSANNFW sources for moc'ing up (not needed with Qt5)
IF( NOT SALSA_USE_QT5 )
	QT4_WRAP_CPP( SALSANNFW_SRCS ${SALSANNFW_HDRS} OPTIONS "-nw" )
ENDIF( NOT SALSA_USE_QT5 )

### Adding definitions
ADD_DEFINITIONS( ${SALSA_COMMON_DEFS} -DEIGEN_NO_AUTOMATIC_RESIZING )
### Setting definitions for shared linkage
### This must not go into UseSALSA.cmake but it has to be defined only when BUILDING
ADD_DEFINITIONS( -DSALSA_NNFW_BUILDING_DLL )

### Setting directories with headers
INCLUDE_DIRECTORIES( ${SALSA_COMMON_INCLUDES} ./include ./ui ../utilities ../utilities/include ../configuration/include )

### Setting directories with libraries to link
LINK_DIRECTORIES( ${SALSA_COMMON_LINKDIRS} )

### Creating target
ADD_LIBRARY( salsannfw SHARED ${SALSANNFW_SRCS} ${SALSANNFW_HDRS} )
SET_TARGET_PROPERTIES( salsannfw PROPERTIES VERSION ${VER_MAJ}.${VER_MIN}.${VER_PAT} SOVERSION ${VER_MAJ}.${VER_MIN}  INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib )
ADD_DEPENDENCIES( salsannfw salsautilities salsaconfiguration )

# Setting the Qt5 modules used by this library (if using Qt5)
IF( SALSA_USE_QT5 )
	QT5_USE_MODULES(salsannfw Widgets Concurrent)
ENDIF( SALSA_USE_QT5 )

### Setting libraries to link
TARGET_LINK_LIBRARIES( salsannfw ${SALSA_COMMON_LIBRARIES} salsautilities salsaconfiguration )

### Windows stuffs (suffix for debug libraries)
IF( WIN32 )
	SET_TARGET_PROPERTIES( salsannfw PROPERTIES DEBUG_POSTFIX "d" )
ENDIF( WIN32 )

### Install configuration
INSTALL( TARGETS salsannfw ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin )
IF( WIN32 )
	## Install PDB files necessary for debugging
	INSTALL( FILES ${CMAKE_BINARY_DIR}/nnfw/Debug/salsannfwd.pdb DESTINATION bin CONFIGURATIONS Debug )
ENDIF( WIN32 )
INSTALL( FILES ${SALSANNFW_HDRS} DESTINATION include/SALSA/nnfw )

### Setting the value of variables needed to generate UseSALSA.cmake
LIST( APPEND SALSA_USEFILE_NNFW_INCLUDES "\${SALSA_BASEDIR}/include/SALSA/nnfw" )
LIST( APPEND SALSA_USEFILE_NNFW_LINKDIRS "\${SALSA_BASEDIR}/lib" )
IF( WIN32 )
	LIST( APPEND SALSA_USEFILE_NNFW_LIBRARIES "optimized;salsannfw" "debug;salsannfwd" )
ELSE( WIN32 )
	LIST( APPEND SALSA_USEFILE_NNFW_LIBRARIES salsannfw )
ENDIF( WIN32 )

### Export variables to parent scope. Here we simply set variables to their value and add the PARENT_SCOPE
### directive.
SET( SALSA_USEFILE_NNFW_DEFS ${SALSA_USEFILE_NNFW_DEFS} PARENT_SCOPE )
SET( SALSA_USEFILE_NNFW_INCLUDES ${SALSA_USEFILE_NNFW_INCLUDES} PARENT_SCOPE )
SET( SALSA_USEFILE_NNFW_LINKDIRS ${SALSA_USEFILE_NNFW_LINKDIRS} PARENT_SCOPE )
SET( SALSA_USEFILE_NNFW_LIBRARIES ${SALSA_USEFILE_NNFW_LIBRARIES} PARENT_SCOPE )
SET( SALSA_USEFILE_NNFW_CFLAGS ${SALSA_USEFILE_NNFW_CFLAGS} PARENT_SCOPE )
SET( SALSA_USEFILE_NNFW_LFLAGS ${SALSA_USEFILE_NNFW_LFLAGS} PARENT_SCOPE )
