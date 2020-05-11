#!/bin/bash

set -x
set -e
OCV_VERSION=4.3.0

if [ "$CI" == "" ] && [ -d /dev/shm ]; then
    TEMP_BASE=/dev/shm
else
    TEMP_BASE=/tmp
fi

cd $TEMP_BASE

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    sudo apt-get install -y build-essential;
    sudo apt-get install -y cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev libopenexr-dev;
    sudo apt-get install -y python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libdc1394-22-dev;
else
    alias nproc='sysctl -n hw.physicalcpu';
    brew install libpng libtiff openexr eigen tbb;
fi

git clone https://github.com/opencv/opencv.git
cd opencv
git checkout tags/$OCV_VERSION -b $OCV_VERSION
mkdir build && cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE -D BUILD_LIST=core,imgproc,imgcodecs,highgui -D BUILD_EXAMPLES=OFF -D BUILD_opencv_apps=OFF -D BUILD_DOCS=OFF -D BUILD_PERF_TESTS=OFF -D BUILD_TESTS=OFF -D CMAKE_INSTALL_PREFIX=/usr/local ..
make -j`nproc`

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    sudo make install
    sudo sh -c 'echo "/usr/local/lib" > /etc/ld.so.conf.d/opencv.conf'
    sudo ldconfig
    echo "OpenCV installed."
fi
