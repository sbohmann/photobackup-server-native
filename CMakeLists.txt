cmake_minimum_required(VERSION 3.13)
project(photobackup_server_native)

include_directories(
        $ENV{JAVA_HOME}/include
        $ENV{JAVA_HOME}/include/win32
        $ENV{MAGICK_HOME}/include
)

set(CMAKE_CXX_STANDARD 14)

add_library(photobackup_server_native SHARED heifToJpeg.cpp)
