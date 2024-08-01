add_subdirectory(common)
add_subdirectory(setting)

if(BUILD_TEST)
    add_executable(${APP}
        setting/tests/test_general.cpp
        setting/tests/test_access.cpp
        setting/tests/test_rules.cpp
        )
    target_link_libraries(${APP} PRIVATE GTest::gtest_main)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/e39786088138f2749d64e9e90e0f9902daa77c40.zip
        )
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(${APP})

else()
    add_subdirectory(dbus)
    add_executable(${APP} main.cpp)
    target_link_libraries(${APP} PRIVATE gesh_dbus)
endif()

target_include_directories(${APP} PRIVATE ${CMAKE_BINARY_DIR}/generated)

target_link_libraries(${APP} PRIVATE
    gesh_common
    gesh_setting
)
