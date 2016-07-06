# Script to compile the farsa total99 application

set(FARSATOTAL99_SRCS
	src/batchinstancesmanager.cpp
	src/genericgroupeditor.cpp
	src/groupstreeeditor.cpp
	src/main.cpp
	src/parametereditorsfactory.cpp
	src/projectmanager.cpp
	src/viewersmanager.cpp)
set(FARSATOTAL99_HDRS
	include/batchinstancesmanager.h
	include/genericgroupeditor.h
	include/groupstreeeditor.h
	include/parametereditorsfactory.h
	include/projectmanager.h
	include/viewersmanager.h)

add_executable(total99 ${FARSATOTAL99_SRCS} ${FARSATOTAL99_HDRS})
add_farsa_version(total99)

# Specifying the the include directories: they are used both here and  exported
# by this library (so that targets linking this one will automatically import
# the include directories declared here)
target_include_directories(total99 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Adding dependencies (they are also exported, so targets linking this one will
# automatically link libraries declared here)
target_link_libraries(total99 farsautilities farsaconfiguration farsaexperiments Qt5::Core Qt5::Xml Qt5::Widgets Qt5::Concurrent Qt5::OpenGL)

# Installation paths
install(TARGETS total99
        EXPORT farsa
        RUNTIME DESTINATION bin/)

# Also installing base configuration, icons and templates
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/conf/" DESTINATION "${MISC_FILES_DESTINATION}/conf" PATTERN ".*" EXCLUDE)
