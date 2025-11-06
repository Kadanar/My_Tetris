cmake_minimum_required(VERSION 3.15)
project(My_Tetris)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройки для Visual Studio
if(MSVC)
    # Отключаем предупреждения о конфликтах библиотек
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /EHsc /MP")
    
    # Настройки для отладки
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Od")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")
endif()

# Путь к GLFW
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/libraries/glfw")

# Проверяем существование GLFW
if(NOT EXISTS "${GLFW_DIR}/include/GLFW/glfw3.h")
    message(FATAL_ERROR "GLFW not found at: ${GLFW_DIR}")
endif()

include_directories(${GLFW_DIR}/include)

# Явно перечисляем все исходные файлы
set(SOURCES
    src/main.cpp
    src/game/Tetromino.cpp
    src/game/GameBoard.cpp
    src/graphics/Renderer.cpp
)

# Включаем директории
include_directories(include)

# Создаем исполняемый файл
add_executable(My_Tetris ${SOURCES})

# Подключаем библиотеки
target_link_libraries(My_Tetris 
    "${GLFW_DIR}/lib/glfw3.lib"
    opengl32.lib
    gdi32.lib
)

# Настройки для отладки
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(My_Tetris PRIVATE DEBUG=1)
endif()

# Копируем DLL
if(EXISTS "${GLFW_DIR}/glfw3.dll")
    add_custom_command(TARGET My_Tetris POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${GLFW_DIR}/glfw3.dll"
        "$<TARGET_FILE_DIR:My_Tetris>"
        COMMENT "Copying GLFW DLL..."
    )
endif()

# Сообщения для отладки
message(STATUS "Source files: ${SOURCES}")
message(STATUS "GLFW path: ${GLFW_DIR}")