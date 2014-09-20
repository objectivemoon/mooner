CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(moonqrcode C)

link_directories(
    /usr/local/lib
)
ADD_DEFINITIONS(-std=gnu99)

file(GLOB source_files *.c)
add_executable(moonqrcode ${source_files})
