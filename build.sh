#! /bin/bash
cmake --version
cmake -S . -B build
cmake --build build
ctest
