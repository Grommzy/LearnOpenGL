cmake_minimum_required(VERSION 3.10)

# Name the GLAD project
project(STB_Image LANGUAGES C)

# We can specify the C standard (often GLAD can work with C99)
set(CMAKE_C_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Create a static library named 'glad' from glad.c
add_library(STB_Image STATIC
    stb_image.cpp
)