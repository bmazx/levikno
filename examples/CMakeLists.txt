set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples)

file(COPY res DESTINATION .)

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(thirdparty/glfw)

# examples
set(LVN_EXAMPLE_SRC
    gmathtest.c
    logging.c
    sandbox.c
)

foreach(LVN_SRC ${LVN_EXAMPLE_SRC})
    get_filename_component(LVN_SRC_NAME ${LVN_SRC} NAME)
    string(REPLACE ".c" "" LVN_SRC_NAME ${LVN_SRC_NAME})

    add_executable(${LVN_SRC_NAME} ${LVN_SRC})
    target_link_libraries(${LVN_SRC_NAME} PRIVATE levikno lvngraphics glfw)
    target_include_directories(${LVN_SRC_NAME}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/include
            thirdparty/glfw/include
    )
endforeach()
