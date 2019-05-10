#!/bin/sh
export JAVA_HOME=/usr/local/openjdk8
export JNI_OS=freebsd
export MAGICK_INCLUDES=/usr/local/include/ImageMagick-7
export MAGICK_LIBS=/usr/local/lib
export MAGICK_WAND_LIBRARY=MagickWand-7
ls -lah "$MAGICK_HOME/include"
(cd build; cmake ..; make clean; cmake --build .)
