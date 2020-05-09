#!/bin/bash

set -x
set -e
TF_VERSION=v2.2.0

# based on https://developer.ridgerun.com/wiki/index.php?title=How_to_Install_TensorFlow_Lite

if [ "$CI" == "" ] && [ -d /dev/shm ]; then
    TEMP_BASE=/dev/shm
else
    TEMP_BASE=/tmp
fi

cd $TEMP_BASE

git clone https://github.com/tensorflow/tensorflow.git
cd tensorflow
git checkout tags/$TF_VERSION -b $TF_VERSION

# Important - download a valid bazel version
BAZEL=`awk -F"'" '/_TF_MAX_BAZEL_VERSION = /{print $2}' configure.py`
BAZEL_INSTALLER=bazel-${BAZEL}-installer-linux-x86_64.sh

if [ "$TRAVIS_OS_NAME" = "osx" ]; then
    BAZEL_INSTALLER=bazel-${BAZEL}-installer-darwin-x86_64.sh
fi

mkdir bazel
cd bazel
wget -c https://github.com/bazelbuild/bazel/releases/download/${BAZEL}/$BAZEL_INSTALLER
chmod +x $BAZEL_INSTALLER
sudo ./$BAZEL_INSTALLER
cd ..

# Configure tensorflow - I usually accept all defaults
 echo | ./configure

cd tensorflow/lite/tools/make
./download_dependencies.sh
./build_lib.sh

cd $TEMP_BASE
