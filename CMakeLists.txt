cmake_minimum_required(VERSION 3.5)

project (hawthorn)

add_definitions(-DUNIX_HOST)

add_executable(hawthorn hawthorn.c)