#! /bin/bash

set -euo pipefail
BUILD=build

cmake --version
cmake -S . -B $BUILD -DCMAKE_BUILD_TYPE=Debug -DPACKAGE_TESTS=ON -DCODE_COVERAGE=ON
cmake --build $BUILD -- -j $(nproc)

cd $BUILD
lcov -c -i -d . -o base_coverage.info
ctest -j $(nproc) --output-on-failure
lcov --directory . --capture --output-file test_coverage.info
lcov -a base_coverage.info -a test_coverage.info -o coverage.info
lcov --remove coverage.info '/usr/*' "${HOME}"'/.cache/*' '*/tests/*' '*/vendor/*' --output-file coverage.info
lcov --list coverage.info
if [ ${CI:-false} = true ]; then
	bash <(curl -s https://codecov.io/bash) -f coverage.info || echo "Codecov did not collect coverage reports"
else
	genhtml coverage.info --output-directory lcov
fi
