### CMake script for the farsaga library

### Resetting variables
SET( FARSA_USEFILE_GA_DEFS )
SET( FARSA_USEFILE_GA_INCLUDES )
SET( FARSA_USEFILE_GA_LINKDIRS )
SET( FARSA_USEFILE_GA_LIBRARIES )
SET( FARSA_USEFILE_GA_CFLAGS )
SET( FARSA_USEFILE_GA_LFLAGS )

### Find source and header files
FILE( GLOB_RECURSE FARSAGA_SRCS ./src/*.cpp )
FILE( GLOB_RECURSE FARSAGA_HDRS ./include/*.h )

### Add headers to FARSAUTILITIES sources for moc'ing up (not needed with Qt5)
IF( NOT FARSA_USE_QT5 )
	QT4_WRAP_CPP( FARSAGA_SRCS ${FARSAGA_HDRS} OPTIONS "-nw" )
ENDIF( NOT FARSA_USE_QT5 )

### Adding definitions
ADD_DEFINITIONS( ${FARSA_COMMON_DEFS} )
### Setting definitions for shared linkage
### This must not go into UseFARSA.cmake but it has to be defined only when BUILDING
ADD_DEFINITIONS( -DFARSA_GA_BUILDING_DLL )

### Setting directories with headers
INCLUDE_DIRECTORIES( ${FARSA_COMMON_INCLUDES} ./include ../utilities/include ../utilities ../configuration/include )

### Setting directories with libraries to link
LINK_DIRECTORIES( ${FARSA_COMMON_LINKDIRS} )

### Creating target
ADD_LIBRARY( farsaga SHARED ${FARSAGA_SRCS} ${FARSAGA_HDRS} )
SET_TARGET_PROPERTIES( farsaga PROPERTIES VERSION ${VER_MAJ}.${VER_MIN}.${VER_PAT} SOVERSION ${VER_MAJ}.${VER_MIN}  INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib )
ADD_DEPENDENCIES( farsaga farsautilities farsaconfiguration )

# Setting the Qt5 modules used by this library (if using Qt5)
IF( FARSA_USE_QT5 )
	QT5_USE_MODULES(farsaga Widgets Concurrent)
ENDIF( FARSA_USE_QT5 )

### Setting libraries to link
TARGET_LINK_LIBRARIES( farsaga ${FARSA_COMMON_LIBRARIES} farsautilities farsaconfiguration )

### Windows stuffs (suffix for debug libraries)
IF( WIN32 )
	SET_TARGET_PROPERTIES( farsaga PROPERTIES DEBUG_POSTFIX "d" )
ENDIF( WIN32 )

### Install configuration
INSTALL( TARGETS farsaga ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin )
IF( WIN32 )
	## Install PDB files necessary for debugging
	INSTALL( FILES ${CMAKE_BINARY_DIR}/ga/Debug/farsagad.pdb DESTINATION bin CONFIGURATIONS Debug )
ENDIF( WIN32 )
FILE( GLOB HDRS ./include/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/FARSA/ga )
FILE( GLOB HDRS ./include/core/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/FARSA/ga/core )
FILE( GLOB HDRS ./include/mutations/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/FARSA/ga/mutations )
FILE( GLOB HDRS ./include/crossovers/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/FARSA/ga/crossovers )
FILE( GLOB HDRS ./include/selections/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/FARSA/ga/selections )
FILE( GLOB HDRS ./include/reproductions/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/FARSA/ga/reproductions )
FILE( GLOB HDRS ./include/evaluations/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/FARSA/ga/evaluations )
FILE( GLOB HDRS ./include/gas/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/FARSA/ga/gas )
FILE( GLOB HDRS ./include/genotypes/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/FARSA/ga/genotypes )

### Setting the value of varibles needed to generate UseFARSA.cmake
LIST( APPEND FARSA_USEFILE_GA_INCLUDES "\${FARSA_BASEDIR}/include/FARSA/ga" )
LIST( APPEND FARSA_USEFILE_GA_LINKDIRS "\${FARSA_BASEDIR}/lib" )
IF( WIN32 )
	LIST( APPEND FARSA_USEFILE_GA_LIBRARIES "optimized;farsaga" "debug;farsagad" )
ELSE( WIN32 )
	LIST( APPEND FARSA_USEFILE_GA_LIBRARIES farsaga )
ENDIF( WIN32 )

### Export variables to parent scope. Here we simply set variables to their value and add the PARENT_SCOPE
### directive.
SET( FARSA_USEFILE_GA_DEFS ${FARSA_USEFILE_GA_DEFS} PARENT_SCOPE )
SET( FARSA_USEFILE_GA_INCLUDES ${FARSA_USEFILE_GA_INCLUDES} PARENT_SCOPE )
SET( FARSA_USEFILE_GA_LINKDIRS ${FARSA_USEFILE_GA_LINKDIRS} PARENT_SCOPE )
SET( FARSA_USEFILE_GA_LIBRARIES ${FARSA_USEFILE_GA_LIBRARIES} PARENT_SCOPE )
SET( FARSA_USEFILE_GA_CFLAGS ${FARSA_USEFILE_GA_CFLAGS} PARENT_SCOPE )
SET( FARSA_USEFILE_GA_LFLAGS ${FARSA_USEFILE_GA_LFLAGS} PARENT_SCOPE )
