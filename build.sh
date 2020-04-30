#! /bin/bash

set -euo pipefail

set -v
BUILD="build"
ROOT="$(pwd)"
BASE="$ROOT/src"
set +v

function install() {
	mkdir -p $HOME/.cache

	mkdir $HOME/.cache/cmake && \
		wget -qO- https://github.com/Kitware/CMake/releases/download/v3.17.0/cmake-3.17.0-Linux-x86_64.tar.gz | \
		tar -xzC $HOME/.cache/cmake --strip-components 1
	export PATH=$HOME/.cache/cmake/bin:$PATH

	mkdir $HOME/.cache/grcov && \
		curl -L https://github.com/mozilla/grcov/releases/latest/download/grcov-linux-x86_64.tar.bz2 | \
		tar -xjC $HOME/.cache/grcov
	export PATH=$HOME/.cache/grcov:$PATH
}

function build() {
	cmake --version
	cmake -S . -B $BUILD -DCMAKE_BUILD_TYPE=Debug -DPACKAGE_TESTS=ON -DCODE_COVERAGE=ON
	cmake --build $BUILD -- -j $(nproc)
}

function tests() {
	cd $BUILD
	ctest -j $(nproc) --output-on-failure
	cd $ROOT
	grcov $BUILD -s $ROOT -t lcov --llvm --branch --ignore-not-existing -o $BUILD/coverage.info \
		--ignore "/usr/*" \
		--ignore "vendor/*" \
		--ignore "tests/*" \
		--ignore "src/main.cpp"

	if [ ${CI:-false} = true ]; then
		bash <(curl -s https://codecov.io/bash) -f $BUILD/coverage.info || echo "Codecov did not collect coverage reports"
	else
		genhtml $BUILD/coverage.info --output-directory $BUILD/lcov
	fi
}

install
build
tests
