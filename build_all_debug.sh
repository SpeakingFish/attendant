#!/bin/bash

OUTPUT_DIR=build

if [[ -d "$OUTPUT_DIR" && ! $1 = "forced" ]] ; then
    echo Directory $OUTPUT_DIR is exists. Please delete it or type \'`basename $0` forced\'
else
    rm -fr $OUTPUT_DIR
    mkdir $OUTPUT_DIR
    cd $OUTPUT_DIR
    cmake -D CMAKE_BUILD_TYPE=Debug -D INSTALL_TYPE=Symlinks ..
    make --jobs=4
    cd -
fi
