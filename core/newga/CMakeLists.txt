### CMake script for the farsanewga library

### Resetting variables
SET( FARSA_USEFILE_NEWGA_DEFS )
SET( FARSA_USEFILE_NEWGA_INCLUDES )
SET( FARSA_USEFILE_NEWGA_LINKDIRS )
SET( FARSA_USEFILE_NEWGA_LIBRARIES )
SET( FARSA_USEFILE_NEWGA_CFLAGS )
SET( FARSA_USEFILE_NEWGA_LFLAGS )

### Find source and header files
FILE( GLOB_RECURSE FARSANEWGA_SRCS ./src/*.cpp )
FILE( GLOB_RECURSE FARSANEWGA_HDRS ./include/*.h )

### Add headers to FARSANEWGA sources for moc'ing up (not needed with Qt5)
IF( NOT FARSA_USE_QT5 )
	QT4_WRAP_CPP( FARSANEWGA_SRCS ${FARSANEWGA_HDRS} OPTIONS "-nw" )
ENDIF( NOT FARSA_USE_QT5 )

### Adding definitions
ADD_DEFINITIONS( ${FARSA_COMMON_DEFS} )
### Setting definitions for shared linkage
### This must not go into UseFARSA.cmake but it has to be defined only when BUILDING
ADD_DEFINITIONS( -DFARSA_NEWGA_BUILDING_DLL )

### Setting directories with headers
INCLUDE_DIRECTORIES( ${FARSA_COMMON_INCLUDES} ./include ../utilities/include ../utilities ../configuration/include ../experiments/include ../experiments/evorobot/include ../worldsim/include ../worldsim/3rdParts )

### Setting directories with libraries to link
LINK_DIRECTORIES( ${FARSA_COMMON_LINKDIRS} )

### Creating target
ADD_LIBRARY( farsanewga SHARED ${FARSANEWGA_SRCS} ${FARSANEWGA_HDRS} )
SET_TARGET_PROPERTIES( farsanewga PROPERTIES VERSION ${VER_MAJ}.${VER_MIN}.${VER_PAT} SOVERSION ${VER_MAJ}.${VER_MIN}  INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib )
ADD_DEPENDENCIES( farsanewga farsautilities farsaconfiguration farsaexperiments )

# Setting the Qt5 modules used by this library (if using Qt5)
IF( FARSA_USE_QT5 )
	QT5_USE_MODULES(farsanewga Widgets Concurrent Xml OpenGL)
ENDIF( FARSA_USE_QT5 )

### Setting libraries to link
TARGET_LINK_LIBRARIES( farsanewga ${FARSA_COMMON_LIBRARIES} farsautilities farsaconfiguration farsaexperiments )

### Windows stuffs (suffix for debug libraries)
IF( WIN32 )
	SET_TARGET_PROPERTIES( farsanewga PROPERTIES DEBUG_POSTFIX "d" )
ENDIF( WIN32 )

### Install configuration
INSTALL( TARGETS farsanewga ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin )
IF( WIN32 )
	## Install PDB files necessary for debugging
	INSTALL( FILES ${CMAKE_BINARY_DIR}/newga/Debug/farsanewgad.pdb DESTINATION bin CONFIGURATIONS Debug )
ENDIF( WIN32 )
FILE( GLOB HDRS ./include/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/FARSA/newga )

### Setting the value of varibles needed to generate UseFARSA.cmake
LIST( APPEND FARSA_USEFILE_NEWGA_INCLUDES "\${FARSA_BASEDIR}/include/FARSA/newga" )
LIST( APPEND FARSA_USEFILE_NEWGA_LINKDIRS "\${FARSA_BASEDIR}/lib" )
IF( WIN32 )
	LIST( APPEND FARSA_USEFILE_NEWGA_LIBRARIES "optimized;farsanewga" "debug;farsanewgad" )
ELSE( WIN32 )
	LIST( APPEND FARSA_USEFILE_NEWGA_LIBRARIES farsanewga )
ENDIF( WIN32 )

### Export variables to parent scope. Here we simply set variables to their value and add the PARENT_SCOPE
### directive.
SET( FARSA_USEFILE_NEWGA_DEFS ${FARSA_USEFILE_NEWGA_DEFS} PARENT_SCOPE )
SET( FARSA_USEFILE_NEWGA_INCLUDES ${FARSA_USEFILE_NEWGA_INCLUDES} PARENT_SCOPE )
SET( FARSA_USEFILE_NEWGA_LINKDIRS ${FARSA_USEFILE_NEWGA_LINKDIRS} PARENT_SCOPE )
SET( FARSA_USEFILE_NEWGA_LIBRARIES ${FARSA_USEFILE_NEWGA_LIBRARIES} PARENT_SCOPE )
SET( FARSA_USEFILE_NEWGA_CFLAGS ${FARSA_USEFILE_NEWGA_CFLAGS} PARENT_SCOPE )
SET( FARSA_USEFILE_NEWGA_LFLAGS ${FARSA_USEFILE_NEWGA_LFLAGS} PARENT_SCOPE )
