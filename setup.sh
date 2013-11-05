#!/bin/bash -x
#
# This scripts sets up the Arduino libraries directory

ARDUINO_LIB="../libraries"

cd `dirname $0`

for library in `ls`; do
  if [ -d "$library" ]; then
    ln -s $library $ARDUINO_LIB/$library
  fi
done
