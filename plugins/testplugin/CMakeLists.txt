cmake_minimum_required(VERSION 2.8.12)

project(TestPlugin)

find_package(salsa REQUIRED)

set(SALSATESTPLUGIN_SRCS
	src/kheperadiscriminationexperiment.cpp)
set(SALSATESTPLUGIN_HDRS
	include/kheperadiscriminationexperiment.h)

add_salsaplugin(${PROJECT_NAME} ${SALSATESTPLUGIN_SRCS} ${SALSATESTPLUGIN_HDRS})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

salsa_plugin_install_conf(${PROJECT_NAME} "conf")
