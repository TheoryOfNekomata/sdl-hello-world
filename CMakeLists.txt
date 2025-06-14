cmake_minimum_required(VERSION 3.31)
project(sdl_hello_world C)

set(CMAKE_C_STANDARD 23)

add_executable(${PROJECT_NAME} src/main.c)

target_include_directories(${PROJECT_NAME} PRIVATE thirdparty/SDL3-3.2.16/include)
target_link_directories(${PROJECT_NAME} PRIVATE thirdparty/SDL3-3.2.16/lib/x64)

target_link_libraries(${PROJECT_NAME} SDL3)

if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/thirdparty/SDL3-3.2.16/lib/x64/SDL3.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif ()
