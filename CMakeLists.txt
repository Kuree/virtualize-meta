cmake_minimum_required(VERSION 3.0.0)

# here we specify that the project is C language only, so the default
# C compiler on the system will be used
project(Virtualize-Meta C)

add_library(parser parser.c parser.h)

add_executable(test test.c)
target_link_libraries(test parser)