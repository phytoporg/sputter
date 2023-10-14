#!/usr/bin/env bash

# TODO: Add --clean flag

cmake -S . -bBuild; cmake --build build -j$(nproc)
