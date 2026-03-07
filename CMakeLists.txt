
# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(kilo C)

# --- Compiler Flags and C Standard ---

# Set the standard compiler flags used in your Makefile
# Note: In CMake, you usually separate the standard/warning flags
# from the standard version flag.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")


# generates compile_commands.json  (i hope)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Specify the C standard (c99)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# --- Source Files ---

# Define the source files for your executable
set(KILO_SOURCES
    kilo.c
    terminal.c
    input.c
    output.c
    append_buffer.c
    file.c
    row_operations.c
)

# --- Build the Executable ---

# Create the 'kilo' executable from the source files
add_executable(kilo ${KILO_SOURCES})
