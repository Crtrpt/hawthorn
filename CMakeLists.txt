cmake_minimum_required(VERSION 3.5)

project (hawthorn)

aux_source_directory(. DIR_SRCS)

include_directories(cstdlib)

include_directories(platform)

add_definitions(-DUNIX_HOST)

add_executable(hawthorn hawthorn.c table.c lex.c parse.c expression.c heap.c type.c variable.c clibrary.c platform.c include.c debug.c cstdlib/stdio.c cstdlib/math.c cstdlib/string.c cstdlib/stdlib.c cstdlib/time.c cstdlib/errno.c cstdlib/ctype.c cstdlib/stdbool.c cstdlib/unistd.c ./platform/platform_unix.c ./platform/library_unix.c )

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -pedantic -g -DUNIX_HOST -DVER=\"0.0.0\"")
target_link_libraries(hawthorn m)







