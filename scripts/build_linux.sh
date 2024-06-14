#!/usr/bin/env bash

BUILD_TYPE=Release
BUILD_DIR=build
CLEAN_BUILD=0

for param in "$@"; do
    if [ "$param" == "--debug" ]; then
        BUILD_TYPE=Debug
        BUILD_DIR=build-debug
    elif [ "$param" == "--clean" ]; then
        CLEAN_BUILD=1
    else
        echo -e "Unrecognized parameter: $param"
    fi
done

if [ $CLEAN_BUILD -gt 0 ]; then
    echo "Deleting $BUILD_DIR for clean build"
    rm -rf $BUILD_DIR
fi

cmake -S . -B$BUILD_DIR; cmake --build $BUILD_DIR -j$(nproc)
