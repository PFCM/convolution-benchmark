cmake_minimum_required(VERSION 3.0)

project(convolutions)

include_directories(./include ./src /usr/local/include)
find_package(gflags REQUIRED)

find_path(LIBSNDFILE_INCLUDE_DIRS sndfile.h)
find_library(LIBSNDFILE_LIBRARIES sndfile)
include_directories(${LIBSNDFILE_INCLUDE_DIRS})

find_library(GFLAGS_LIBRARIES gflags)

set(LIBS
    ${LIBSNDFILE_LIBRARIES}
    ${GFLAGS_LIBRARIES})

file(GLOB SOURCES src/*.cpp)
add_executable(convolutions ${SOURCES})
target_link_libraries(convolutions ${LIBS})
