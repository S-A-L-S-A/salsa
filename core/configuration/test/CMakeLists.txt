# Compile the tests and adds a test target

# NOTE: To get the output of the failing test you should either call "make test"
# with the environmental variable CTEST_OUTPUT_ON_FAILURE set to 1 or call
# "ctest --output-on-failure" in the build directory (instead of "make test").

# Creating a library with helpers for test code
set(FARSACONFIGURATIONTESTLIBRARY_SRCS
	testhierarchy.cpp
	testutils.cpp)
set(FARSACONFIGURATIONTESTLIBRARY_HDRS
	testhierarchy.h
	testutils.h)

add_library(farsaconfigurationtest STATIC ${FARSACONFIGURATIONTESTLIBRARY_SRCS} ${FARSACONFIGURATIONTESTLIBRARY_HDRS})
target_link_libraries(farsaconfigurationtest farsaconfiguration Qt5::Test)

# A function to declare a test. The only argument is the name of the test. The
# source file of the test must have the same name of the test with a "_test.cpp"
# suffix. "_test" is appended also the the name of the test
function(addFarsaConfigurationTest testName)
	set(mangledTestName "${testName}_test")
	add_executable("${mangledTestName}" "${mangledTestName}.cpp")
	target_link_libraries("${mangledTestName}" farsaconfiguration farsaconfigurationtest Qt5::Test)
	add_test(NAME "${mangledTestName}" COMMAND "${mangledTestName}")
endfunction()

# Adding all tests
addFarsaConfigurationTest(functioncallssequencerecorder)
addFarsaConfigurationTest(configurationhelper)
addFarsaConfigurationTest(configurationkey)
addFarsaConfigurationTest(configurationnode)
addFarsaConfigurationTest(configurationmanager)
addFarsaConfigurationTest(parametersfileloadersaver)
addFarsaConfigurationTest(inifilesupport)
addFarsaConfigurationTest(typesdb)
addFarsaConfigurationTest(componentcreation)
addFarsaConfigurationTest(configurationobserver)
addFarsaConfigurationTest(resources)
addFarsaConfigurationTest(componentdescription)
addFarsaConfigurationTest(typesdbwithinterfaces)
addFarsaConfigurationTest(componentdescriptionwithdescriptors)
