CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(fnbcollector C)

link_directories(
    /usr/local/lib
    ../lib
)

ADD_DEFINITIONS(-I/usr/local/include -I../include  -W -Wall -pedantic -g3)
FIND_LIBRARY(MOONTOOLS moontools ../lib)

file(GLOB source_files *.c)

ADD_EXECUTABLE(fnbcollector ${source_files})

TARGET_LINK_LIBRARIES(fnbcollector rabbitmq pcap ${MOONTOOLS} ldns pthread)

