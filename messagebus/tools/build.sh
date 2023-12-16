#!/bin/bash

DEST_DIR=./build

if [ -d $DEST_DIR ]; then
  rm -rf $DEST_DIR
  mkdir $DEST_DIR
else
  mkdir $DEST_DIR
fi

cd build
cmake ..
make
./main