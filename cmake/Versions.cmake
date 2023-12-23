# Run this with CMake in "script mode" (-P flag) at build time

# in case Git is not available, we default to "unknown"
set(GIT_HASH "unknown")
set(GIT_DIRTY "")

# find Git and if available set GIT_HASH variable
find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND git log -1 --pretty=format:%h
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    execute_process(
        COMMAND git diff-index --quiet HEAD --
        ERROR_QUIET
        RESULT_VARIABLE GIT_SUBMOD_RESULT
    )
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        set(GIT_DIRTY "-dirty")
    endif()
endif()

set(BUILD_VERSION "${GIT_HASH}${GIT_DIRTY}")

execute_process(
    COMMAND date "+%Y-%m-%d %H:%M:%S"
    OUTPUT_VARIABLE BUILD_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

message(STATUS "Build version is ${BUILD_VERSION}")
message(STATUS "Build date is ${BUILD_DATE}")

# generate file version.hpp based on version.hpp.in
configure_file(
    ${IN_FILE}
    ${OUT_FILE}
    @ONLY
)
