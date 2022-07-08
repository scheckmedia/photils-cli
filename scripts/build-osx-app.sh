#!/bin/bash

set -x
set -e

if [ "$CI" == "" ] && [ -d /dev/shm ]; then
    TEMP_BASE=/dev/shm
else
    TEMP_BASE=/tmp
fi

REPO_ROOT="$BUILD_DIR/../"
TMP_BUILD_DIR=$(mktemp -d 2>/dev/null || mktemp -d -t 'OSX-build-XXXXXX')
OUT=$TMP_BUILD_DIR/osx

# make sure to clean up build dir, even if errors occur
cleanup () {
    if [ -d "$TMP_BUILD_DIR" ]; then
        rm -rf "$TMP_BUILD_DIR"
    fi
}
trap cleanup EXIT

pushd "$TMP_BUILD_DIR"
conan install $REPO_ROOT --build=missing
cmake "$REPO_ROOT" -DBUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=$OUT
cmake --build . --target install
cd $OUT
mv photils-cli.app photils-cli-$VERSION.app
zip -r $BUILD_DIR/photils-cli-$VERSION-osx.zip *
cd $TEMP_BASE
