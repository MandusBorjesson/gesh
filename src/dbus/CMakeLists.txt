add_library(gesh_dbus
    src/dbus.cpp
)

target_include_directories(gesh_dbus PUBLIC inc)

target_include_directories(gesh_dbus PRIVATE ${CMAKE_BINARY_DIR}/generated)

target_link_libraries(gesh_dbus PRIVATE
    gesh_common
    gesh_setting
    sdbus-c++
)
