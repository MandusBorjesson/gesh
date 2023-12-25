
set(APP gesh)

add_executable(${APP}
    main.cpp
    setting.cpp
    settingInitializerHardcoded.cpp
    settingRule.cpp
    settingReaderFile.cpp
    ${CMAKE_BINARY_DIR}/generated/dbus-abstraction.h
    )
target_include_directories(${APP} PRIVATE ${CMAKE_BINARY_DIR}/generated)
target_link_libraries(${APP} sdbus-c++)

