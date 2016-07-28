# Compile the tests and adds a test target

# NOTE: To get the output of the failing test you should either call "make test"
# with the environmental variable CTEST_OUTPUT_ON_FAILURE set to 1 or call
# "ctest --output-on-failure" in the build directory (instead of "make test").

# Creating a library with helpers for test code
set(SALSACONFIGURATIONTESTLIBRARY_SRCS
	testhierarchy.cpp
	testutils.cpp)
set(SALSACONFIGURATIONTESTLIBRARY_HDRS
	testhierarchy.h
	testutils.h)

add_library(salsaconfigurationtest STATIC ${SALSACONFIGURATIONTESTLIBRARY_SRCS} ${SALSACONFIGURATIONTESTLIBRARY_HDRS})
target_link_libraries(salsaconfigurationtest salsaconfiguration Qt5::Test)

# A function to declare a test. The only argument is the name of the test. The
# source file of the test must have the same name of the test with a "_test.cpp"
# suffix. "_test" is appended also the the name of the test
function(addSalsaConfigurationTest testName)
	set(mangledTestName "${testName}_test")
	add_executable("${mangledTestName}" "${mangledTestName}.cpp")
	target_link_libraries("${mangledTestName}" salsaconfiguration salsaconfigurationtest Qt5::Test)
	add_test(NAME "${mangledTestName}" COMMAND "${mangledTestName}")
endfunction()

# Adding all tests
addSalsaConfigurationTest(functioncallssequencerecorder)
addSalsaConfigurationTest(configurationhelper)
addSalsaConfigurationTest(configurationkey)
addSalsaConfigurationTest(configurationnode)
addSalsaConfigurationTest(configurationmanager)
addSalsaConfigurationTest(parametersfileloadersaver)
addSalsaConfigurationTest(inifilesupport)
addSalsaConfigurationTest(typesdb)
addSalsaConfigurationTest(componentcreation)
addSalsaConfigurationTest(configurationobserver)
addSalsaConfigurationTest(resources)
addSalsaConfigurationTest(componentdescription)
addSalsaConfigurationTest(typesdbwithinterfaces)
addSalsaConfigurationTest(componentdescriptionwithdescriptors)
