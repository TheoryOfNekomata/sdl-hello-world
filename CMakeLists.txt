cmake_minimum_required(VERSION 3.31)
project(sdl_hello_world C)

set(CMAKE_C_STANDARD 23)

file(GLOB sources src/*.h src/*.c src/commands/*.c)

file(GLOB sources_7z thirdparty/lzma2409/include/lzma/*.h thirdparty/lzma2409/include/lzma/*.c)

add_executable(${PROJECT_NAME} thirdparty/sqlite/include/sqlite/sqlite3.c ${sources_7z} thirdparty/dirent/include/dirent/dirent.h ${sources})

target_include_directories(
    ${PROJECT_NAME} PRIVATE
        thirdparty/SDL3/include
        thirdparty/SDL3_image/include
        thirdparty/SDL3_ttf/include
        thirdparty/sqlite/include
        thirdparty/lzma2409/include
        thirdparty/dirent/include
)

target_link_directories(
    ${PROJECT_NAME} PRIVATE
        thirdparty/SDL3/lib/x64
        thirdparty/SDL3_image/lib/x64
        thirdparty/SDL3_ttf/lib/x64
)

target_link_libraries(${PROJECT_NAME} SDL3 SDL3_image SDL3_ttf)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/src/default.app.cfg"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${PROJECT_SOURCE_DIR}/src/schema.sql"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)

if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/thirdparty/SDL3/lib/x64/SDL3.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/thirdparty/SDL3_image/lib/x64/SDL3_image.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/thirdparty/SDL3_ttf/lib/x64/SDL3_ttf.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif ()

add_definitions(-DG00_APP_NAME="${PROJECT_NAME}")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_HOME_DIRECTORY}/assets"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)
