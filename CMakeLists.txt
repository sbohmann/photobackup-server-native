cmake_minimum_required(VERSION 3.13)
project(photobackup_server_native)

add_compile_definitions(
        MAGICKCORE_QUANTUM_DEPTH=16
        MAGICKCORE_HDRI_ENABLE=true
)

include_directories(
        SYSTEM $ENV{JAVA_HOME}/include
        SYSTEM $ENV{JAVA_HOME}/include/win32
        SYSTEM $ENV{MAGICK_INCLUDES}
)

link_directories($ENV{MAGICK_HOME}/lib)

link_libraries(CORE_RL_Magick++_ CORE_RL_MagickWand_)

set(CMAKE_CXX_STANDARD 14)

add_library(photobackup_server_native SHARED heifToJpeg.cpp main.cpp)
add_executable(test main.cpp)
