add_subdirectory(common)
add_subdirectory(dbus)
add_subdirectory(setting)

add_executable(${APP}
    main.cpp
    )

target_include_directories(${APP} PRIVATE ${CMAKE_BINARY_DIR}/generated)

target_link_libraries(${APP} PRIVATE
    gesh_dbus
    gesh_common
    gesh_setting
)
