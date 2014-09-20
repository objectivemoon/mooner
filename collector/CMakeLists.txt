CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(moontop C)

link_directories(
    /usr/local/lib
)

ADD_DEFINITIONS(-I/usr/local/include -I../include  -W -Wall -pedantic -g3 -std=gnu99)

file(GLOB source_files *.c)
add_executable(mooner ${source_files})

target_link_libraries(mooner rabbitmq pcap ldns pthread)
#dmalloc)


