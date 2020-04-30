#! /bin/bash
set -euo pipefail

set -v
CC=${CC:-clang}
CXX=${CXX:-clang++}
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
	cmake --build $BUILD -j $(nproc)
}

function tests() {
	cd $BUILD
	ctest -j $(nproc) --output-on-failure
	
	cd $ROOT
	if [ $CC = clang ]; then
		GRCOV_FLAGS=--llvm
	else
		GRCOV_FLAGS=
	fi
	
	if [ ${CI:-false} = true ]; then
		GRCOV_TYPE=lcov
		GRCOV_OUTPUT="$BUILD/coverage.info"
	else
		GRCOV_TYPE=html
		GRCOV_OUTPUT="$BUILD/coverage/"
	fi
	
	grcov $BUILD -s $ROOT -t $GRCOV_TYPE $GRCOV_FLAGS --branch --ignore-not-existing -o $GRCOV_OUTPUT \
		--ignore "/usr/*" \
		--ignore "vendor/*" \
		--ignore "tests/*" \
		--ignore "src/main.cpp"
	
	if [ ${CI:-false} = true ]; then
		bash <(curl -s https://codecov.io/bash) -f $GRCOV_OUTPUT || echo "Codecov did not collect coverage reports"
	fi
}

install
build
tests
