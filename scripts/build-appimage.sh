#!/bin/bash

set -x
set -e

if [ "$CI" == "" ] && [ -d /dev/shm ]; then
    TEMP_BASE=/dev/shm
else
    TEMP_BASE=/tmp
fi

TMP_BUILD_DIR=$(mktemp -d -p "$TEMP_BASE" AppImageLauncher-build-XXXXXX)

# make sure to clean up build dir, even if errors occur
cleanup () {
    if [ -d "$TMP_BUILD_DIR" ]; then
        rm -rf "$TMP_BUILD_DIR"
    fi
}
trap cleanup EXIT

# store repo root as variable
REPO_ROOT=$(readlink -f $(dirname $(dirname $0)))
OLD_CWD=$(readlink -f .)

# switch to build dir
pushd "$TMP_BUILD_DIR"

# configure build files with CMake
# we need to explicitly set the install prefix, as CMake's default is /usr/local for some reason...
conan install $REPO_ROOT --build=missing
cmake "$REPO_ROOT" -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_TESTS=OFF

# build project and install files into AppDir
make -j$(nproc)
make install DESTDIR=AppDir

# now, build AppImage using linuxdeploy
# download linuxdeploy
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage

# make them executable
chmod +x linuxdeploy*.AppImage

# initialize AppDir, bundle shared libraries for photils-cli, use appimage plugin to bundle additional resources, and build AppImage, all in one single command
./linuxdeploy-x86_64.AppImage --appdir AppDir --output appimage

# move built AppImage back into original CWD
mv photils-cli*.AppImage $BUILD_DIR/photils-cli-$VERSION-linux-x86_64.AppImage
popd
