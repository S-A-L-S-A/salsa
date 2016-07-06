# Copyright: (C) 2010 Jan Woetzel
# Small modifications by Lorenzo Natale and Francesco Nori
# CopyPolicy: Use under LGPL or BSD license at your choice

# 
# Try to find GSL library
# Once run this will define: 
# 
# GSL_FOUND
# GSL_INCLUDE_DIR and GSL_INCLUDE_DIRS
# GSL_LIBRARIES
# GSL_LINK_DIRECTORIES
# GSLCBLAS_LIBRARY
# GSL_LIBRARY
##

# 
# 2004/05 Jan Woetzel, Friso, Daniel Grest 
# 2006 complete rewrite by Jan Woetzel
##
# www.mip.informatik.uni-kiel.de/
# --------------------------------
#
# Modified by macl
#

# Included in yarp by nat March 07.
# Some changes to account for different include dirs.
# 08/11/2011, Lorenzo Natale: added <prefix>/Release to library search path

# Debug and Relase libraries are now stored in different variables

##### check GSL_ROOT
IF (EXISTS "$ENV{GSL_ROOT}")

  SET(GSL_POSSIBLE_INCDIRS
	"$ENV{GSL_ROOT}/include"
	"$ENV{GSL_ROOT}")
  
  SET(GSL_POSSIBLE_LIBRARY_PATHS_RELEASE
	"$ENV{GSL_ROOT}/lib"
	"$ENV{GSL_ROOT}/Release")
  
  # Added lib/debug to possible paths, debug libraries are put there by the YARP package
  SET(GSL_POSSIBLE_LIBRARY_PATHS_DEBUG
	"$ENV{GSL_ROOT}/lib/debug"
	"$ENV{GSL_ROOT}/Debug"
	"$ENV{GSL_ROOT}/lib")
	
ENDIF (EXISTS "$ENV{GSL_ROOT}")

##### check GSL_DIR
IF (EXISTS "$ENV{GSL_DIR}")

  SET(GSL_POSSIBLE_INCDIRS
	"$ENV{GSL_DIR}/include"
	"$ENV{GSL_DIR}")
  
  SET(GSL_POSSIBLE_LIBRARY_PATHS_RELEASE
	"$ENV{GSL_DIR}/lib"
	"$ENV{GSL_DIR}/Release")
  
  # Added lib/debug to possible paths, debug libraries are put there by the YARP package
  SET(GSL_POSSIBLE_LIBRARY_PATHS_DEBUG
	"$ENV{GSL_DIR}/lib/debug"
	"$ENV{GSL_DIR}/Debug"
	"$ENV{GSL_DIR}/lib")
ENDIF (EXISTS "$ENV{GSL_DIR}")

IF (GSL_DIR)

  SET(GSL_POSSIBLE_INCDIRS
	"${GSL_DIR}/include"
	"${GSL_DIR}")
  
  SET(GSL_POSSIBLE_LIBRARY_PATHS_RELEASE
	"${GSL_DIR}/lib"
	"${GSL_DIR}/Release")
  
  # Added lib/debug to possible paths, debug libraries are put there by the YARP package
  SET(GSL_POSSIBLE_LIBRARY_PATHS_DEBUG
	"${GSL_DIR}/lib/debug"
	"${GSL_DIR}/Debug"
	"${GSL_DIR}/lib")
ENDIF (GSL_DIR)

FIND_PATH(GSL_BLAS_HEADER gsl/gsl_blas.h  
	${GSL_POSSIBLE_INCDIRS} 
	/usr/include
	/usr/local/include
	)

FIND_LIBRARY(GSL_LIBRARY_RELEASE
  NAMES libgsl libgsl.lib gsl
  PATHS ${GSL_POSSIBLE_LIBRARY_PATHS_RELEASE}
	/usr/lib
	/usr/local/lib
  DOC "Location of the gsl lib (relase)")

# We search gsl debug libraries in two steps: first we exclude paths in system variables (PATH and LIB)
# because otherwise cmake finds the release version (LIB contains C:\Program Files\robotology\gsl-1.14\lib),
# then, if the library hasn't been found, we repeat the search also in paths in system variables
FIND_LIBRARY(GSL_LIBRARY_DEBUG
  NAMES libgsld libgsld.lib gsld libgsl libgsl.lib gsl
  PATHS ${GSL_POSSIBLE_LIBRARY_PATHS_DEBUG}
	/usr/lib
	/usr/local/lib
  DOC "Location of the gsl lib (debug)"
  NO_SYSTEM_ENVIRONMENT_PATH)

FIND_LIBRARY(GSL_LIBRARY_DEBUG
  NAMES libgsld libgsld.lib gsld libgsl libgsl.lib gsl
  PATHS ${GSL_POSSIBLE_LIBRARY_PATHS_DEBUG}
	/usr/lib
	/usr/local/lib
  DOC "Location of the gsl lib (debug)")

FIND_LIBRARY(GSLCBLAS_LIBRARY_RELEASE
  NAMES libgslcblas libgslcblas.lib gslcblas
  PATHS ${GSL_POSSIBLE_LIBRARY_PATHS_RELEASE}
	/usr/lib
	/usr/local/lib
  DOC "Location of the gsl lib (release)")

# As for gsl we do the search in two steps
FIND_LIBRARY(GSLCBLAS_LIBRARY_DEBUG
  NAMES libgslcblasd libgslcblasd.lib gslcblasd libgslcblas libgslcblas.lib gslcblas
  PATHS ${GSL_POSSIBLE_LIBRARY_PATHS_DEBUG}
	/usr/lib
	/usr/local/lib
  DOC "Location of the gsl lib (debug)"
  NO_SYSTEM_ENVIRONMENT_PATH)

FIND_LIBRARY(GSLCBLAS_LIBRARY_DEBUG
  NAMES libgslcblasd libgslcblasd.lib gslcblasd libgslcblas libgslcblas.lib gslcblas
  PATHS ${GSL_POSSIBLE_LIBRARY_PATHS_DEBUG}
	/usr/lib
	/usr/local/lib
  DOC "Location of the gsl lib (debug)")

IF (GSLCBLAS_LIBRARY_DEBUG AND GSLCBLAS_LIBRARY_RELEASE AND GSL_LIBRARY_DEBUG AND GSL_LIBRARY_RELEASE AND GSL_BLAS_HEADER)
  SET(GSL_INCLUDE_DIR "${GSL_BLAS_HEADER}")
  SET(GSL_INCLUDE_DIRS "${GSL_BLAS_HEADER}")
  SET(GSL_LINK_DIRECTORIES "$ENV{GSL_ROOT}/lib")
  SET(GSL_LIBRARIES "optimized;${GSL_LIBRARY_RELEASE};debug;${GSL_LIBRARY_DEBUG}")
  SET(GSL_LIBRARIES "${GSL_LIBRARIES}" "optimized;${GSLCBLAS_LIBRARY_RELEASE};debug;${GSLCBLAS_LIBRARY_DEBUG}")
  SET(GSL_FOUND ON)
  SET(GSL_DIR "$ENV{GSL_ROOT}")
ENDIF (GSLCBLAS_LIBRARY_DEBUG AND GSLCBLAS_LIBRARY_RELEASE AND GSL_LIBRARY_DEBUG AND GSL_LIBRARY_RELEASE AND GSL_BLAS_HEADER)

SET (GSL_MARK)

IF (GSL_FOUND)
  # nothing to say
  IF (NOT GSL_DIR)
	MARK_AS_ADVANCED(GSL_DIR)
  ENDIF (NOT GSL_DIR)
ELSE (GSL_FOUND)
  SET (GSL_DIR "" CACHE PATH "Location of GSL")
  IF (GSL_FIND_REQUIRED OR GSL_DIR)
    SET (GSL_MARK CLEAR)
    MARK_AS_ADVANCED(
      ${GSL_MARK}
      GSL_INCLUDE_DIR
      GSL_LINK_DIRECTORIES
      GSL_LIBRARIES
      GSLCBLAS_LIBRARIES
      GSLCBLAS_LIBRARY_RELEASE
	  GSLCBLAS_LIBRARY_DEBUG
      GSL_BLAS_HEADER
      GSL_LIBRARY_RELEASE
	  GSL_LIBRARY_DEBUG
    )
    MESSAGE(FATAL_ERROR "GSL library or headers not found. "
            "Please search manually or set env. variable GSL_DIR to guide search." )
  ENDIF (GSL_FIND_REQUIRED OR GSL_DIR)  
ENDIF (GSL_FOUND)


MARK_AS_ADVANCED(
  ${GSL_MARK}
  GSL_INCLUDE_DIR
  GSL_INCLUDE_DIRS
  GSL_LINK_DIRECTORIES
  GSL_LIBRARIES
  GSLCBLAS_LIBRARIES
  GSLCBLAS_LIBRARY_RELEASE
  GSLCBLAS_LIBRARY_DEBUG
  GSLCBLAS_LIBRARY
  GSL_BLAS_HEADER
  GSL_LIBRARY
  GSL_LIBRARY_RELEASE
  GSL_LIBRARY_DEBUG
)

