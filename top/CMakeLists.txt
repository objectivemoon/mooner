CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(mooner C)

link_directories(
    /usr/local/lib
    ../lib
)

ADD_DEFINITIONS(-I/usr/local/include -I../include  -W -Wall -pedantic -g3 -std=gnu99)
FIND_LIBRARY(MOONTOOLS moontools ../lib)

file(GLOB source_files *.c)
add_executable(moontop ${source_files})

target_link_libraries(moontop ${MOONTOOLS})


