cmake_minimum_required(VERSION 2.8.12)

project(TestPlugin)

find_package(farsa REQUIRED)

set(FARSATESTPLUGIN_SRCS
	src/kheperadiscriminationexperiment.cpp)
set(FARSATESTPLUGIN_HDRS
	include/kheperadiscriminationexperiment.h)

add_farsaplugin(${PROJECT_NAME} ${FARSATESTPLUGIN_SRCS} ${FARSATESTPLUGIN_HDRS})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

farsa_plugin_install_conf(${PROJECT_NAME} "conf")
