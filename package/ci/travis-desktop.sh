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
    -DBUILD_DEPRECATED=$BUILD_DEPRECATED \
    -DBUILD_STATIC=$BUILD_STATIC \
    -DCMAKE_BUILD_TYPE=$CONFIGURATION \
    -DWITH_INTERCONNECT=OFF \
    -G Ninja
ninja install
cd ../..

mkdir build && cd build
# Not using CXXFLAGS in order to avoid affecting dependencies
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_BUILD_TYPE=$CONFIGURATION \
    -DWITH_AUDIO=ON \
    -DWITH_VK=OFF \
    -DWITH_GLFWAPPLICATION=ON \
    -DWITH_SDL2APPLICATION=ON \
    -DWITH_WINDOWLESS${PLATFORM_GL_API}APPLICATION=ON \
    -DWITH_${PLATFORM_GL_API}CONTEXT=ON \
    -DWITH_OPENGLTESTER=ON \
    -DWITH_ANYAUDIOIMPORTER=ON \
    -DWITH_ANYIMAGECONVERTER=ON \
    -DWITH_ANYIMAGEIMPORTER=ON \
    -DWITH_ANYSCENECONVERTER=ON \
    -DWITH_ANYSCENEIMPORTER=ON \
    -DWITH_MAGNUMFONT=ON \
    -DWITH_MAGNUMFONTCONVERTER=ON \
    -DWITH_OBJIMPORTER=ON \
    -DWITH_TGAIMAGECONVERTER=ON \
    -DWITH_TGAIMPORTER=ON \
    -DWITH_WAVAUDIOIMPORTER=ON \
    -DWITH_DISTANCEFIELDCONVERTER=ON \
    -DWITH_FONTCONVERTER=ON \
    -DWITH_IMAGECONVERTER=ON \
    -DWITH_SCENECONVERTER=ON \
    -DWITH_GL_INFO=ON \
    -DWITH_AL_INFO=ON \
    -DBUILD_TESTS=ON \
    -DBUILD_GL_TESTS=ON \
    -DBUILD_DEPRECATED=$BUILD_DEPRECATED \
    -DBUILD_STATIC=$BUILD_STATIC \
    -DBUILD_PLUGINS_STATIC=$BUILD_STATIC \
    -G Ninja
# Otherwise the job gets killed (probably because using too much memory)
ninja -j4
ASAN_OPTIONS="color=always" LSAN_OPTIONS="color=always suppressions=$TRAVIS_BUILD_DIR/package/ci/leaksanitizer.conf" TSAN_OPTIONS="color=always" CORRADE_TEST_COLOR=ON ctest -V -E GLTest

# Test install, after running the tests as for them it shouldn't be needed
ninja install

# Verify also compilation of the documentation image generators
cd ..
mkdir build-doc-generated && cd build-doc-generated
cmake ../doc/generated \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_PREFIX_PATH=$HOME/deps \
    -DCMAKE_BUILD_TYPE=Debug \
    -G Ninja
ninja
