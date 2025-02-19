cmake_minimum_required(VERSION 3.10)

# Name the GLAD project
project(GLAD LANGUAGES C)

# We can specify the C standard (often GLAD can work with C99)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Create a static library named 'glad' from glad.c
add_library(GLAD STATIC
    src/glad.c
)

# Make the 'include' directory available as a public include path
target_include_directories(GLAD
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)