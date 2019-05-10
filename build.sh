#!/bin/sh
export JAVA_HOME
export JNI_OS
export MAGICK_INCLUDES=/usr/local/include/ImageMagick-7
export MAGICK_LIBS=/usr/local/lib
export MAGICK_WAND_LIBRARY
mkdir build
(cd build; cmake ..; make clean; cmake --build .)
