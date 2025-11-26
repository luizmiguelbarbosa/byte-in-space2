cmake_minimum_required(VERSION 3.22)
project(byte_in_space2 C)

set(CMAKE_C_STANDARD 99)

# =============== RAYLIB MULTIPLATFORM ===============
# Detect OS and choose correct Raylib folder
if (WIN32)
    set(RAYLIB_DIR "${CMAKE_SOURCE_DIR}/external/raylib_windows")
    set(RAYLIB_LIB raylib)
elseif (APPLE)
    set(RAYLIB_DIR "${CMAKE_SOURCE_DIR}/external/raylib_macos")
    set(RAYLIB_LIB raylib)
elseif (UNIX)
    set(RAYLIB_DIR "${CMAKE_SOURCE_DIR}/external/raylib_linux")
    set(RAYLIB_LIB raylib)
endif()

set(RAYLIB_INCLUDE_DIR "${RAYLIB_DIR}/include")
set(RAYLIB_LIB_DIR "${RAYLIB_DIR}/lib")

include_directories(
        ${RAYLIB_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)

# Source files
file(GLOB PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/src/*.c")

add_executable(byte_in_space2 ${PROJECT_SOURCES}
        src/game_state.c
        include/game_state.h
        src/shop.c
        include/shop.h
        src/cutscene.c
        include/cutscene.h)

# Link directories
target_link_directories(byte_in_space2 PRIVATE ${RAYLIB_LIB_DIR})

# Core Raylib library
target_link_libraries(byte_in_space2 ${RAYLIB_LIB})

# =============== PLATFORM-SPECIFIC SYSTEM LIBRARIES ===============
if (WIN32)
    target_link_libraries(byte_in_space2 winmm gdi32 user32 shell32)
elseif (APPLE)
    target_link_libraries(byte_in_space2 "-framework Cocoa" "-framework OpenGL" "-framework IOKit")
elseif (UNIX)
    target_link_libraries(byte_in_space2 m pthread dl GL X11)
endif()

# =============== COPY ASSETS TO BUILD FOLDER ===============
add_custom_command(TARGET byte_in_space2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:byte_in_space2>/assets"
)

# =============== COPY RAYLIB RUNTIME LIBS ===============
if (WIN32)
    add_custom_command(TARGET byte_in_space2 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${RAYLIB_LIB_DIR}/raylib.dll"
            "$<TARGET_FILE_DIR:byte_in_space2>/raylib.dll"
    )
elseif (APPLE)
    add_custom_command(TARGET byte_in_space2 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${RAYLIB_LIB_DIR}/libraylib.dylib"
            "$<TARGET_FILE_DIR:byte_in_space2>/libraylib.dylib"
    )
elseif (UNIX)
    add_custom_command(TARGET byte_in_space2 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${RAYLIB_LIB_DIR}/libraylib.so.5.0.0"
            "$<TARGET_FILE_DIR:byte_in_space2>/libraylib.so.5.0.0"
    )
endif()
