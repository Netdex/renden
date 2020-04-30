#! /bin/bash

set -euo pipefail

cmake --version
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DPACKAGE_TESTS=ON -DCODE_COVERAGE=ON
cmake --build build -- -j $(nproc)
cd build
lcov -c -i -d . -o base_coverage.info
ctest -j $(nproc) --output-on-failure
