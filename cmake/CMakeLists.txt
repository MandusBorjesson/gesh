
find_package(sdbus-c++ REQUIRED)
find_package(sdbus-c++-tools REQUIRED)
set(DBUS_DEPS SDBusCpp::sdbus-c++)


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
