add_library(gesh_setting
    src/setting.cpp
    src/settingInitializerHardcoded.cpp
    src/settingLayer.cpp
    src/settingStorageFile.cpp
    src/settingReaderFactory.cpp
    src/settingReaderFile.cpp
    src/settingRule.cpp
)

target_include_directories(gesh_setting PUBLIC inc)

target_link_libraries(gesh_setting PRIVATE
    gesh_common
)
