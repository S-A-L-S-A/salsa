# Script to compile the salsa salsapluginhelper application

set(SALSAPLUGINHELPER_SRCS
	src/main.cpp)

add_executable(salsapluginhelper ${SALSAPLUGINHELPER_SRCS})
add_salsa_version(salsapluginhelper)

# Adding dependencies (they are also exported, so targets linking this one will
# automatically link libraries declared here)
target_link_libraries(salsapluginhelper salsalibpluginhelper Qt5::Core)

# Installation paths
install(TARGETS salsapluginhelper
        EXPORT salsa
        RUNTIME DESTINATION bin/)
