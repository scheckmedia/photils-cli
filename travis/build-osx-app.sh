#!/bin/bash

set -x
set -e

if [ "$CI" == "" ] && [ -d /dev/shm ]; then
    TEMP_BASE=/dev/shm
else
    TEMP_BASE=/tmp
fi

TF_DIR=$TEMP_BASE/tensorflow
OCV_DIR=$TEMP_BASE/opencv/build

BUILD_DIR=$(mktemp -d 2>/dev/null || mktemp -d -t 'OSX-build-XXXXXX')
OUT=$BUILD_DIR/osx

# make sure to clean up build dir, even if errors occur
cleanup () {
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
}
trap cleanup EXIT

pushd "$BUILD_DIR"
cmake $TRAVIS_BUILD_DIR -D TF_DIR=$TF_DIR -D OpenCV_DIR=$OCV_DIR -DCMAKE_INSTALL_PREFIX=$OUT
make -j`nproc` && make install
cd $OUT
zip -r $TRAVIS_BUILD_DIR/releases/photils-cli-osx.zip *
cd $TEMP_BASE
