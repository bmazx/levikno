cmake_minimum_required(VERSION 3.11)

set(CMAKE_C_STANDARD 99)

project(levikno)

# options
option(LVN_BUILD_EXAMPLES "Build example programs" TRUE)

# output dirs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# platform
if (WIN32)
    add_definitions(-DLVN_PLATFORM_WINDOWS)
elseif(APPLE)
    add_definitions(-DLVN_PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
    add_definitions(-DLVN_PLATFORM_LINUX)
else()
    add_definitions(-DLVN_PLATFORM_UNKNOWN)
endif()

# compiler options
if (MSVC)
    # warning level 4
    add_compile_options(/W4)
else()
    # additional warnings
    add_compile_options(-Wall -Wpedantic)
endif()


set(LVN_SRC
    include/levikno/levikno.h
    src/levikno.c
    src/levikno_internal.h
)

add_library(levikno STATIC
    ${LVN_SRC}
)

target_include_directories(levikno
    PRIVATE
        src
        include/levikno
)


# build definitions
target_compile_definitions(levikno PRIVATE
    $<$<CONFIG:Debug>:LVN_CONFIG_DEBUG>
    $<$<CONFIG:Release>:LVN_CONFIG_RELEASE>
)

# build examples
if(LVN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
