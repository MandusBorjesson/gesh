#-------------------------------
# PROJECT INFORMATION
#-------------------------------

cmake_minimum_required(VERSION 3.13)

project(sdbus-c++ VERSION 1.3.0 LANGUAGES C CXX)

find_package(Threads REQUIRED)

#-------------------------------
# GENERAL COMPILER CONFIGURATION
#-------------------------------

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(cmake)

add_dbus_interface(
    SERVER
    ${PROJECT_SOURCE_DIR}/api/owl.gesh.setting.xml
    dbus-abstraction.h
)

add_subdirectory(src)
