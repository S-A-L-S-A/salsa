# Script to compile the farsa farsapluginhelper application

set(FARSAPLUGINHELPER_SRCS
	src/main.cpp)

add_executable(farsapluginhelper ${FARSAPLUGINHELPER_SRCS})
add_farsa_version(farsapluginhelper)

# Adding dependencies (they are also exported, so targets linking this one will
# automatically link libraries declared here)
target_link_libraries(farsapluginhelper farsalibpluginhelper Qt5::Core)

# Installation paths
install(TARGETS farsapluginhelper
        EXPORT farsa
        RUNTIME DESTINATION bin/)
