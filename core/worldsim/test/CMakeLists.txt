# Compile the tests and adds a test target

# NOTE: To get the output of the failing test you should either call "make test"
# with the environmental variable CTEST_OUTPUT_ON_FAILURE set to 1 or call
# "ctest --output-on-failure" in the build directory (instead of "make test").

# A function to declare a test. The only argument is the name of the test. The
# source file of the test must have the same name of the test with a "_test.cpp"
# suffix. "_test" is appended also the the name of the test
function(addSalsaWorldsimTest testName)
	set(mangledTestName "${testName}_test")
	add_executable("${mangledTestName}" "${mangledTestName}.cpp")
	target_link_libraries("${mangledTestName}" salsaworldsim Qt5::Test)
	add_test(NAME "${mangledTestName}" COMMAND "${mangledTestName}")
endfunction()

# Adding all tests
addSalsaWorldsimTest(worldsimcreation)
