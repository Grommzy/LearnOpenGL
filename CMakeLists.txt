cmake_minimum_required(VERSION 3.10)
project(OpenGL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

set(ENGINE_SRC
    src/main.cpp
    src/Shader.cpp
    src/Model.cpp
    src/Mesh.cpp
    # ...
)

add_subdirectory(external/glad      EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL       EXCLUDE_FROM_ALL)
add_subdirectory(external/STB_Image EXCLUDE_FROM_ALL)
add_subdirectory(external/assimp    EXCLUDE_FROM_ALL)   

add_library(EngineSrc STATIC ${ENGINE_SRC})
add_executable(OpenGL src/main.cpp)
target_link_libraries(OpenGL
    PUBLIC EngineSrc
    PUBLIC GLAD
    PUBLIC SDL3::SDL3
    PUBLIC STB_Image
    PUBLIC assimp::assimp
)

target_include_directories(EngineSrc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/STB_Image
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/external/assimp/include
    ${CMAKE_CURRENT_BINARY_DIR}/external/assimp/include # NOTE: Binary Directory
)