set(LIBRARY_OUTPUT_PATH ../lib/${CMAKE_BUILD_TYPE})
FILE(GLOB_RECURSE source_files *.c)
ADD_LIBRARY(moontools STATIC ${source_files})
