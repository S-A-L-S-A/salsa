### CMake script for the salsanewga library

### Resetting variables
SET( SALSA_USEFILE_NEWGA_DEFS )
SET( SALSA_USEFILE_NEWGA_INCLUDES )
SET( SALSA_USEFILE_NEWGA_LINKDIRS )
SET( SALSA_USEFILE_NEWGA_LIBRARIES )
SET( SALSA_USEFILE_NEWGA_CFLAGS )
SET( SALSA_USEFILE_NEWGA_LFLAGS )

### Find source and header files
FILE( GLOB_RECURSE SALSANEWGA_SRCS ./src/*.cpp )
FILE( GLOB_RECURSE SALSANEWGA_HDRS ./include/*.h )

### Add headers to SALSANEWGA sources for moc'ing up (not needed with Qt5)
IF( NOT SALSA_USE_QT5 )
	QT4_WRAP_CPP( SALSANEWGA_SRCS ${SALSANEWGA_HDRS} OPTIONS "-nw" )
ENDIF( NOT SALSA_USE_QT5 )

### Adding definitions
ADD_DEFINITIONS( ${SALSA_COMMON_DEFS} )
### Setting definitions for shared linkage
### This must not go into UseSALSA.cmake but it has to be defined only when BUILDING
ADD_DEFINITIONS( -DSALSA_NEWGA_BUILDING_DLL )

### Setting directories with headers
INCLUDE_DIRECTORIES( ${SALSA_COMMON_INCLUDES} ./include ../utilities/include ../utilities ../configuration/include ../experiments/include ../experiments/evorobot/include ../worldsim/include ../worldsim/3rdParts )

### Setting directories with libraries to link
LINK_DIRECTORIES( ${SALSA_COMMON_LINKDIRS} )

### Creating target
ADD_LIBRARY( salsanewga SHARED ${SALSANEWGA_SRCS} ${SALSANEWGA_HDRS} )
SET_TARGET_PROPERTIES( salsanewga PROPERTIES VERSION ${VER_MAJ}.${VER_MIN}.${VER_PAT} SOVERSION ${VER_MAJ}.${VER_MIN}  INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib )
ADD_DEPENDENCIES( salsanewga salsautilities salsaconfiguration salsaexperiments )

# Setting the Qt5 modules used by this library (if using Qt5)
IF( SALSA_USE_QT5 )
	QT5_USE_MODULES(salsanewga Widgets Concurrent Xml OpenGL)
ENDIF( SALSA_USE_QT5 )

### Setting libraries to link
TARGET_LINK_LIBRARIES( salsanewga ${SALSA_COMMON_LIBRARIES} salsautilities salsaconfiguration salsaexperiments )

### Windows stuffs (suffix for debug libraries)
IF( WIN32 )
	SET_TARGET_PROPERTIES( salsanewga PROPERTIES DEBUG_POSTFIX "d" )
ENDIF( WIN32 )

### Install configuration
INSTALL( TARGETS salsanewga ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin )
IF( WIN32 )
	## Install PDB files necessary for debugging
	INSTALL( FILES ${CMAKE_BINARY_DIR}/newga/Debug/salsanewgad.pdb DESTINATION bin CONFIGURATIONS Debug )
ENDIF( WIN32 )
FILE( GLOB HDRS ./include/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/SALSA/newga )

### Setting the value of varibles needed to generate UseSALSA.cmake
LIST( APPEND SALSA_USEFILE_NEWGA_INCLUDES "\${SALSA_BASEDIR}/include/SALSA/newga" )
LIST( APPEND SALSA_USEFILE_NEWGA_LINKDIRS "\${SALSA_BASEDIR}/lib" )
IF( WIN32 )
	LIST( APPEND SALSA_USEFILE_NEWGA_LIBRARIES "optimized;salsanewga" "debug;salsanewgad" )
ELSE( WIN32 )
	LIST( APPEND SALSA_USEFILE_NEWGA_LIBRARIES salsanewga )
ENDIF( WIN32 )

### Export variables to parent scope. Here we simply set variables to their value and add the PARENT_SCOPE
### directive.
SET( SALSA_USEFILE_NEWGA_DEFS ${SALSA_USEFILE_NEWGA_DEFS} PARENT_SCOPE )
SET( SALSA_USEFILE_NEWGA_INCLUDES ${SALSA_USEFILE_NEWGA_INCLUDES} PARENT_SCOPE )
SET( SALSA_USEFILE_NEWGA_LINKDIRS ${SALSA_USEFILE_NEWGA_LINKDIRS} PARENT_SCOPE )
SET( SALSA_USEFILE_NEWGA_LIBRARIES ${SALSA_USEFILE_NEWGA_LIBRARIES} PARENT_SCOPE )
SET( SALSA_USEFILE_NEWGA_CFLAGS ${SALSA_USEFILE_NEWGA_CFLAGS} PARENT_SCOPE )
SET( SALSA_USEFILE_NEWGA_LFLAGS ${SALSA_USEFILE_NEWGA_LFLAGS} PARENT_SCOPE )
