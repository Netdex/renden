#! /bin/bash

set -euo pipefail

cmake --version
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DPACKAGE_TESTS=ON -DCODE_COVERAGE=ON
cmake --build build -- -j $(nproc)
cd build
ctest -j $(nproc) --output-on-failure
