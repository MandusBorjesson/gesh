#-------------------------------
# PROJECT INFORMATION
#-------------------------------

set(APP gesh)

cmake_minimum_required(VERSION 3.13)

project(${APP} VERSION 0.0.1 LANGUAGES CXX)

find_package(Threads REQUIRED)

#-------------------------------
# GENERAL COMPILER CONFIGURATION
#-------------------------------

set(CMAKE_CXX_STANDARD 17)

#-------------------------------
# GENERAL COMPILER CONFIGURATION
#-------------------------------

# Add command to generate version header...
add_custom_command(
    OUTPUT
        ${CMAKE_BINARY_DIR}/generated/version.h
        ALL
    COMMAND
        ${CMAKE_COMMAND} -D PROJECT_VERSION=${PROJECT_VERSION} -D IN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.h.in -D OUT_FILE=${CMAKE_BINARY_DIR}/generated/version.h -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Versions.cmake
    WORKING_DIRECTORY
        ${CMAKE_SOURCE_DIR}
)

# ...make sure it exists in filesystem (configuration WILL fail if it's not)...
file(WRITE ${CMAKE_BINARY_DIR}/generated/version.h)

# ...add custom target to rebuild version.hpp every time
add_custom_target(
    generate_version_header
        ALL
    COMMAND
        ${CMAKE_COMMAND} -D PROJECT_VERSION=${PROJECT_VERSION} -D IN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.h.in -D OUT_FILE=${CMAKE_BINARY_DIR}/generated/version.h -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Versions.cmake
    WORKING_DIRECTORY
        ${CMAKE_SOURCE_DIR}
)

add_subdirectory(cmake)

add_dbus_interface(
    SERVER
    ${PROJECT_SOURCE_DIR}/api/owl.gesh.setting.xml
    dbus-abstraction.h
)

add_subdirectory(src)
