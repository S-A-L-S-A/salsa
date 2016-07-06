# Script to compile the farsa worldsim library

# ==============================================================================
# ======= FORSE QUI METTERE SEMPRE COMPILAZIONE DA SORGENTI E MAI BINARIO ======
# ==============================================================================

set(NEWTON_FROM_SOURCE ON CACHE BOOL "If ON will compile the Newton Game Dynamics library from sources")

if (NEWTON_FROM_SOURCE)
	include(CMakeLists_sourceNewton.txt)
else ()
	message(FATAL_ERROR "Precompiled version of the Newton Game Dynamics library is not available yet")
endif ()

# This directory contains unit tests for this library
add_subdirectory(test)
