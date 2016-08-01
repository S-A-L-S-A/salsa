### CMake script for the salsaga library

### Resetting variables
SET( SALSA_USEFILE_GA_DEFS )
SET( SALSA_USEFILE_GA_INCLUDES )
SET( SALSA_USEFILE_GA_LINKDIRS )
SET( SALSA_USEFILE_GA_LIBRARIES )
SET( SALSA_USEFILE_GA_CFLAGS )
SET( SALSA_USEFILE_GA_LFLAGS )

### Find source and header files
FILE( GLOB_RECURSE SALSAGA_SRCS ./src/*.cpp )
FILE( GLOB_RECURSE SALSAGA_HDRS ./include/*.h )

### Add headers to SALSAUTILITIES sources for moc'ing up (not needed with Qt5)
IF( NOT SALSA_USE_QT5 )
	QT4_WRAP_CPP( SALSAGA_SRCS ${SALSAGA_HDRS} OPTIONS "-nw" )
ENDIF( NOT SALSA_USE_QT5 )

### Adding definitions
ADD_DEFINITIONS( ${SALSA_COMMON_DEFS} )
### Setting definitions for shared linkage
### This must not go into UseSALSA.cmake but it has to be defined only when BUILDING
ADD_DEFINITIONS( -DSALSA_GA_BUILDING_DLL )

### Setting directories with headers
INCLUDE_DIRECTORIES( ${SALSA_COMMON_INCLUDES} ./include ../utilities/include ../utilities ../configuration/include )

### Setting directories with libraries to link
LINK_DIRECTORIES( ${SALSA_COMMON_LINKDIRS} )

### Creating target
ADD_LIBRARY( salsaga SHARED ${SALSAGA_SRCS} ${SALSAGA_HDRS} )
SET_TARGET_PROPERTIES( salsaga PROPERTIES VERSION ${VER_MAJ}.${VER_MIN}.${VER_PAT} SOVERSION ${VER_MAJ}.${VER_MIN}  INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib )
ADD_DEPENDENCIES( salsaga salsautilities salsaconfiguration )

# Setting the Qt5 modules used by this library (if using Qt5)
IF( SALSA_USE_QT5 )
	QT5_USE_MODULES(salsaga Widgets Concurrent)
ENDIF( SALSA_USE_QT5 )

### Setting libraries to link
TARGET_LINK_LIBRARIES( salsaga ${SALSA_COMMON_LIBRARIES} salsautilities salsaconfiguration )

### Windows stuffs (suffix for debug libraries)
IF( WIN32 )
	SET_TARGET_PROPERTIES( salsaga PROPERTIES DEBUG_POSTFIX "d" )
ENDIF( WIN32 )

### Install configuration
INSTALL( TARGETS salsaga ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin )
IF( WIN32 )
	## Install PDB files necessary for debugging
	INSTALL( FILES ${CMAKE_BINARY_DIR}/ga/Debug/salsagad.pdb DESTINATION bin CONFIGURATIONS Debug )
ENDIF( WIN32 )
FILE( GLOB HDRS ./include/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/SALSA/ga )
FILE( GLOB HDRS ./include/core/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/SALSA/ga/core )
FILE( GLOB HDRS ./include/mutations/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/SALSA/ga/mutations )
FILE( GLOB HDRS ./include/crossovers/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/SALSA/ga/crossovers )
FILE( GLOB HDRS ./include/selections/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/SALSA/ga/selections )
FILE( GLOB HDRS ./include/reproductions/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/SALSA/ga/reproductions )
FILE( GLOB HDRS ./include/evaluations/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/SALSA/ga/evaluations )
FILE( GLOB HDRS ./include/gas/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/SALSA/ga/gas )
FILE( GLOB HDRS ./include/genotypes/*.h )
INSTALL( FILES ${HDRS} DESTINATION include/SALSA/ga/genotypes )

### Setting the value of varibles needed to generate UseSALSA.cmake
LIST( APPEND SALSA_USEFILE_GA_INCLUDES "\${SALSA_BASEDIR}/include/SALSA/ga" )
LIST( APPEND SALSA_USEFILE_GA_LINKDIRS "\${SALSA_BASEDIR}/lib" )
IF( WIN32 )
	LIST( APPEND SALSA_USEFILE_GA_LIBRARIES "optimized;salsaga" "debug;salsagad" )
ELSE( WIN32 )
	LIST( APPEND SALSA_USEFILE_GA_LIBRARIES salsaga )
ENDIF( WIN32 )

### Export variables to parent scope. Here we simply set variables to their value and add the PARENT_SCOPE
### directive.
SET( SALSA_USEFILE_GA_DEFS ${SALSA_USEFILE_GA_DEFS} PARENT_SCOPE )
SET( SALSA_USEFILE_GA_INCLUDES ${SALSA_USEFILE_GA_INCLUDES} PARENT_SCOPE )
SET( SALSA_USEFILE_GA_LINKDIRS ${SALSA_USEFILE_GA_LINKDIRS} PARENT_SCOPE )
SET( SALSA_USEFILE_GA_LIBRARIES ${SALSA_USEFILE_GA_LIBRARIES} PARENT_SCOPE )
SET( SALSA_USEFILE_GA_CFLAGS ${SALSA_USEFILE_GA_CFLAGS} PARENT_SCOPE )
SET( SALSA_USEFILE_GA_LFLAGS ${SALSA_USEFILE_GA_LFLAGS} PARENT_SCOPE )
