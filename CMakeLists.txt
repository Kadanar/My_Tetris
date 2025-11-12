cmake_minimum_required(VERSION 3.15)
project(My_Tetris)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler settings
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /EHsc")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# GLFW path
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/libraries/glfw")

# Check GLFW exists
if(NOT EXISTS "${GLFW_DIR}/include/GLFW/glfw3.h")
    message(FATAL_ERROR "GLFW not found at: ${GLFW_DIR}")
endif()

# Include directories
include_directories(include ${GLFW_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/graphics/Renderer.cpp
    src/game/GameBoard.cpp
    src/game/Tetromino.cpp
    src/audio/AudioManager.cpp
    src/menu/MenuSystem.cpp
    src/db/Database.cpp
)

# Create executable
add_executable(My_Tetris ${SOURCES})

# Copy assets
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Platform-specific linking
if(WIN32)
    target_link_libraries(My_Tetris 
        "${GLFW_DIR}/lib/glfw3.lib"
        opengl32.lib
        gdi32.lib
        winmm.lib
        odbc32.lib
    )
    
    # Copy DLL
    if(EXISTS "${GLFW_DIR}/glfw3.dll")
        add_custom_command(TARGET My_Tetris POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${GLFW_DIR}/glfw3.dll"
            "$<TARGET_FILE_DIR:My_Tetris>"
        )
    endif()
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(My_Tetris OpenGL::GL ${GLFW_DIR}/lib/libglfw3.a)
endif()