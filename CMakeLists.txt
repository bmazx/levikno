cmake_minimum_required(VERSION 3.11)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

project(levikno)

# options
option(LVN_BUILD_EXAMPLES "Build example programs" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(LVN_BUILD_VULKAN "Build shared libraries" ON)
option(LVN_BUILD_GLSLANG "Build shared libraries" ON)

set(LVN_LIB_TYPE "${LVN_LIB_TYPE}" CACHE STRING
    "Library type override for levikno (SHARED, STATIC, OBJECT, or empty to follow BUILD_SHARED_LIBS)")

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
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wpedantic)
endif()

# library type
set(LVN_LIB_TYPE )

# core
set(LVN_SRC
    include/levikno/levikno.h
    src/levikno.c
    src/levikno_internal.h
    src/lvn_platform.c
)

add_library(levikno ${LVN_LIB_TYPE} ${LVN_SRC})

target_include_directories(levikno
    PRIVATE
        include/levikno
)

# graphics

set(LVN_GRAPHICS_SRC
    include/levikno/lvn_graphics.h
    src/lvn_graphics.c
    src/lvn_graphics_internal.h
)

# vulkanSDK
if (LVN_BUILD_VULKAN)
    set(MIN_VULKAN_VERSION 1.2)
    find_package(Vulkan ${MIN_VULKAN_VERSION})

    if(Vulkan_FOUND)
        message("Vulkan SDK found, including vulkan support")
        add_definitions(-DLVN_INCLUDE_VULKAN)

        list(APPEND LVN_GRAPHICS_SRC
            src/api/lvn_impl_vk.c
            src/api/lvn_impl_vk.h
            src/api/lvn_impl_vk_backends.h
        )

        # find glslang
        find_package(glslang CONFIG)

        if (LVN_BUILD_GLSLANG)
            if (TARGET glslang::glslang AND TARGET glslang::SPIRV)
                message("glslang and SPIRV found, including glslang support")
                add_definitions(-DLVN_INCLUDE_GLSLANG)
            else()
                message("cannot find glslang, skipping glslang support")
            endif()
        endif()

    else()
        message("cannot find Vulkan SDK, skipping vulkan support")
    endif()
endif()

add_library(lvngraphics ${LVN_LIB_TYPE} ${LVN_GRAPHICS_SRC})

target_include_directories(lvngraphics
    PRIVATE
        include/levikno
        src/api
        src
)


# build definitions
target_compile_definitions(lvngraphics PRIVATE
    $<$<CONFIG:Debug>:LVN_CONFIG_DEBUG>
    $<$<CONFIG:Release>:LVN_CONFIG_RELEASE>
)

# build examples
if(LVN_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
