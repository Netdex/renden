#! /bin/bash
cmake --version
cmake -S . -B build -DPACKAGE_TESTS=ON
cmake --build build
cd build
ctest

