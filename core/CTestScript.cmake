# This script is run by CTest on test machines. It performs a compilation and
# installation of FARSA on the host followed by tests for all plugins. The
# plugins for which tests are run are those having a CTestScript.cmake file. The
# results are submitted to CDash. This script doesn't perform the update of the
# repository as that is done externally. When using this script, pass the host
# as argument (either Linux, Windows or MacOSX). For example:
# 	ctest -S /mnt/sharedRepos/farsa/farsa/CTestScript.cmake,Linux

# The build name is the same on all sites
SET( CTEST_BUILD_NAME "FARSA" )
# The build configuration to use is always Release
SET( CTEST_BUILD_CONFIGURATION "Release" )
# Not using memcheck nor code coverage, for the moment
SET( WITH_MEMCHECK FALSE )
SET( WITH_COVERAGE FALSE )
# Setting the site name
SITE_NAME( CTEST_SITE )

# Setting properties that are site-dependent
IF( CTEST_SCRIPT_ARG STREQUAL "Linux" )
	# Setting source and binary dir
	SET( CTEST_SOURCE_DIRECTORY "/mnt/sharedRepos/farsa/farsa" )
	SET( CTEST_BINARY_DIRECTORY "/home/nursery/build-farsa" )
	SET( FARSA_INSTALLATION_DIRECTORY "/usr/local" )

	# Setting the generator to use
	SET( CTEST_CMAKE_GENERATOR "Unix Makefiles" )
	# Setting the build options
	SET( CTEST_BUILD_OPTIONS "\"-DCMAKE_CXX_FLAGS=-Wall -Wextra\" \"-DCMAKE_C_FLAGS=-Wall -Wextra\" -DDISPLAY_DEVELOPER_WARNINGS=OFF -DEXPERIMENTAL_NEWTON_FROMSOURCE=ON -DFARSA_USE_QT5=OFF -DUSE_GRAPHVIZ=OFF -DUSE_GSL=ON \"-DCMAKE_INSTALL_PREFIX=${FARSA_INSTALLATION_DIRECTORY}\"" )
ELSEIF( CTEST_SCRIPT_ARG STREQUAL "Windows" )
	# Setting source and binary dir
	SET( CTEST_SOURCE_DIRECTORY "E:\\farsa\\farsa" )
	SET( CTEST_BINARY_DIRECTORY "C:\\farsa\\build-farsa" )
	SET( FARSA_INSTALLATION_DIRECTORY "C:/farsa/install-farsa" )

	# Setting the generator to use
	SET( CTEST_CMAKE_GENERATOR "Visual Studio 10" )
	# Setting the build options
	SET( CTEST_BUILD_OPTIONS "-DDISPLAY_DEVELOPER_WARNINGS=OFF -DEXPERIMENTAL_NEWTON_FROMSOURCE=ON -DFARSA_USE_QT5=OFF -DUSE_GRAPHVIZ=OFF -DUSE_GSL=ON \"-DCMAKE_INSTALL_PREFIX=${FARSA_INSTALLATION_DIRECTORY}\"" )
ELSEIF( CTEST_SCRIPT_ARG STREQUAL "MacOSX" )
	MESSAGE( FATAL_ERROR "TODO MacOSX" )
ELSE( CTEST_SCRIPT_ARG STREQUAL "Linux" )
	MESSAGE( FATAL_ERROR "Unknown system" )
ENDIF( CTEST_SCRIPT_ARG STREQUAL "Linux" )

# Clearing the build directory
CTEST_EMPTY_BINARY_DIRECTORY( ${CTEST_BINARY_DIRECTORY} )

# Also removing the installation directory
FILE( REMOVE_RECURSE "${FARSA_INSTALLATION_DIRECTORY}" )

# Generating the configure command
SET( CTEST_CONFIGURE_COMMAND "${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=${CTEST_BUILD_CONFIGURATION} -DBUILD_TESTING:BOOL=ON ${CTEST_BUILD_OPTIONS} \"-G${CTEST_CMAKE_GENERATOR}\" \"${CTEST_SOURCE_DIRECTORY}\"")

# The steps to execute. We skip the update (done externally)
CTEST_START( "Nightly" )
CTEST_CONFIGURE()
CTEST_BUILD()
CTEST_TEST()
IF ( WITH_COVERAGE AND CTEST_COVERAGE_COMMAND )
	CTEST_COVERAGE()
ENDIF ( WITH_COVERAGE AND CTEST_COVERAGE_COMMAND )
IF ( WITH_MEMCHECK AND CTEST_MEMORYCHECK_COMMAND )
	CTEST_MEMCHECK()
ENDIF ( WITH_MEMCHECK AND CTEST_MEMORYCHECK_COMMAND )
CTEST_SUBMIT()

# Now that we have finished FARSA tests, we can test plugins. First of all
# getting the list of CTestScript.cmake files. We need to put iCub and OpenCL
# plugin before anything else, so we remove it from the list and add it back
# after sorting in the first position
FILE( GLOB_RECURSE PLUGINS_TEST_SCRIPTS "${CTEST_SCRIPT_DIRECTORY}/../farsaPlugins/CTestScript.cmake" )
LIST( REMOVE_ITEM PLUGINS_TEST_SCRIPTS "${CTEST_SCRIPT_DIRECTORY}/../farsaPlugins/iCub/CTestScript.cmake" )
LIST( REMOVE_ITEM PLUGINS_TEST_SCRIPTS "${CTEST_SCRIPT_DIRECTORY}/../farsaPlugins/OpenCL/CTestScript.cmake" )

# Sorting the list alphabetically because plugins which depend on other plugins come after alphabetically
LIST( SORT PLUGINS_TEST_SCRIPTS )

# Adding back iCub and OpenCL in the first positions
LIST( INSERT PLUGINS_TEST_SCRIPTS 0 "${CTEST_SCRIPT_DIRECTORY}/../farsaPlugins/iCub/CTestScript.cmake" )
LIST( INSERT PLUGINS_TEST_SCRIPTS 0 "${CTEST_SCRIPT_DIRECTORY}/../farsaPlugins/OpenCL/CTestScript.cmake" )

# Now executing each file
FOREACH( CUR_TEST_SCRIPT IN LISTS PLUGINS_TEST_SCRIPTS )
	CTEST_RUN_SCRIPT( "${CUR_TEST_SCRIPT},${CTEST_SCRIPT_ARG}" )
ENDFOREACH( CUR_TEST_SCRIPT )
