#!/bin/bash

source "$PWD/.ci/config.sh" ${1:-build}

echo "---- build_check.sh ----"
echo "BUILD_DIR: $BUILD_DIR"
echo "--------------------------"

cmake --build $BUILD_DIR -j --target cut

if [ -f ./bin/cut ]; then
	echo "OK"
else
	echo "NOT OK"
	exit 1
fi