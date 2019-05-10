cmake_minimum_required(VERSION 3.0)
project(photobackup_server_native)

add_definitions(
        -DMAGICKCORE_QUANTUM_DEPTH=16
        -DMAGICKCORE_HDRI_ENABLE=true
)

include_directories(
        SYSTEM $ENV{JAVA_HOME}/include
        SYSTEM $ENV{JAVA_HOME}/include/$ENV{JNI_OS}
        SYSTEM $ENV{MAGICK_INCLUDES}
)

link_directories($ENV{MAGICK_LIBS})

link_libraries($ENV{MAGICK_WAND_LIBRARY})

message("including $ENV{MAGICK_INCLUDES}, libdirs [$ENV{MAGICK_LIBS}], libs[$ENV{MAGICK_WAND_LIBRARY}]")

set(CMAKE_CXX_STANDARD 14)

add_library(photobackup_server_native SHARED imageMagick.cpp)
add_executable(demo main.cpp)
