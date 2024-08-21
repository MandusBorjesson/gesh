find_package(sdbus-c++ REQUIRED)
find_package(sdbus-c++-tools REQUIRED)

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

set(dbus_bus_options "system" "session")
set(DBUS_BUS "none" CACHE STRING "Select which DBus bus to use, avaliable options: [$dbus_bus_options]")
set_property(CACHE DBUS_BUS PROPERTY STRINGS ${dbus_bus_options})

if(${DBUS_BUS} STREQUAL "system")
    target_compile_options(gesh_dbus PUBLIC "-DDBUS_BUS_SYSTEM")
elseif(${DBUS_BUS} STREQUAL "session")
    target_compile_options(gesh_dbus PUBLIC "-DDBUS_BUS_SESSION")
else()
    message(FATAL_ERROR "DBUS_BUS must be set to one of: ${dbus_bus_options}")
endif ()

function(add_dbus_interface interface_type xml output)
  if(${interface_type} STREQUAL "SERVER")
    set(DBUS_IF_ARG "--adaptor")
  elseif(${interface_type} STREQUAL "CLIENT")
    set(DBUS_IF_ARG "--proxy")
  else()
    message( FATAL_ERROR "Unknown D-Bus interface type '${interface_type}', CMake will exit." )
  endif()

  set(OUT ${CMAKE_BINARY_DIR}/generated/${output})

  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/generated)

  execute_process(
    COMMAND sdbus-c++-xml2cpp ${xml} ${DBUS_IF_ARG}=${OUT} --verbose
    RESULT_VARIABLE ret
    OUTPUT_VARIABLE error_msg
    ERROR_QUIET
  )

  if(ret AND NOT ret EQUAL 0)
    message( FATAL_ERROR "Generating D-Bus interface failed: ${error_msg}")
  endif()

  if(NOT EXISTS "${OUT}")
    message( FATAL_ERROR "Generating D-Bus interface did not yield any files!")
  endif()

  message( "Generated ${interface_type} interface for xml: ${xml}, output: ${OUT}" )

endfunction()

add_dbus_interface(
    SERVER
    ${CMAKE_CURRENT_SOURCE_DIR}/api/owl.gesh.setting.xml
    dbus-setting.h
)

add_dbus_interface(
    SERVER
    ${CMAKE_CURRENT_SOURCE_DIR}/api/owl.gesh.management.xml
    dbus-management.h
)
