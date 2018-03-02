#!/bin/bash
set -ev

# Corrade
git clone --depth 1 git://github.com/mosra/corrade.git
cd corrade
mkdir build && cd build
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_DEPRECATED=$BUILD_DEPRECATED \
    -DWITH_INTERCONNECT=OFF \
    -DWITH_PLUGINMANAGER=OFF \
    -G Ninja
ninja install
cd ../..

mkdir build && cd build
# Not using CXXFLAGS in order to avoid affecting dependencies
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_BUILD_TYPE=Debug \
    -DVulkan_LIBRARY=$HOME/libvulkan.so \
    -DWITH_AUDIO=OFF \
    -DWITH_DEBUGTOOLS=OFF \
    -DWITH_GL=OFF \
    -DWITH_MESHTOOLS=OFF \
    -DWITH_PRIMITIVES=OFF \
    -DWITH_SCENEGRAPH=ON \
    -DWITH_SHADERS=OFF \
    -DWITH_SHAPES=ON \
    -DWITH_TEXT=OFF \
    -DWITH_TEXTURETOOLS=OFF \
    -DWITH_TRADE=OFF \
    -DWITH_VK=ON \
    -DWITH_AL_INFO=OFF \
    -DWITH_VK_INFO=ON \
    -DBUILD_TESTS=ON \
    -DBUILD_VK_TESTS=ON \
    -DBUILD_DEPRECATED=$BUILD_DEPRECATED \
    -G Ninja
# Otherwise the job gets killed (probably because using too much memory)
ninja -j4
ASAN_OPTIONS="color=always" LSAN_OPTIONS="color=always suppressions=$TRAVIS_BUILD_DIR/package/ci/leaksanitizer.conf" CORRADE_TEST_COLOR=ON ctest -V -E GLTest
