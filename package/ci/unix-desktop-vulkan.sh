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
    -DWITH_PLUGINMANAGER=ON \
    -G Ninja
ninja install
cd ../..

# The fastest Vulkan driver ever. See travis.yml for why we have a separate
# Vulkan build.
g++ package/ci/libvulkan.cpp -std=c++11 -shared -o $HOME/libvulkan.so

# Enabling only stuff that's directly affected by Vulkan (which means also
# parts of Platform, which need Trade for icon import in tests), disabling
# everything else.
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
    -DWITH_SCENEGRAPH=OFF \
    -DWITH_SHADERS=OFF \
    -DWITH_TEXT=OFF \
    -DWITH_TEXTURETOOLS=OFF \
    -DWITH_TRADE=ON \
    -DWITH_VK=ON \
    -DWITH_AL_INFO=OFF \
    -DWITH_VK_INFO=ON \
    -DWITH_GL_INFO=OFF \
    -DWITH_ANYAUDIOIMPORTER=OFF \
    -DWITH_ANYIMAGECONVERTER=OFF \
    -DWITH_ANYIMAGEIMPORTER=OFF \
    -DWITH_ANYSCENECONVERTER=OFF \
    -DWITH_ANYSCENEIMPORTER=OFF \
    -DWITH_ANYSHADERCONVERTER=OFF \
    -DWITH_MAGNUMFONT=OFF \
    -DWITH_MAGNUMFONTCONVERTER=OFF \
    -DWITH_OBJIMPORTER=OFF \
    -DWITH_TGAIMAGECONVERTER=OFF \
    -DWITH_TGAIMPORTER=OFF \
    -DWITH_WAVAUDIOIMPORTER=OFF \
    -DWITH_DISTANCEFIELDCONVERTER=OFF \
    -DWITH_FONTCONVERTER=OFF \
    -DWITH_IMAGECONVERTER=OFF \
    -DWITH_SCENECONVERTER=OFF \
    -DWITH_SHADERCONVERTER=OFF \
    -DWITH_SDL2APPLICATION=ON \
    -DWITH_GLFWAPPLICATION=ON \
    -DBUILD_TESTS=ON \
    -DBUILD_GL_TESTS=OFF \
    -DBUILD_VK_TESTS=ON \
    -DBUILD_DEPRECATED=$BUILD_DEPRECATED \
    -G Ninja
ninja
ASAN_OPTIONS="color=always" LSAN_OPTIONS="color=always suppressions=$TRAVIS_BUILD_DIR/package/ci/leaksanitizer.conf" CORRADE_TEST_COLOR=ON ctest -V -E "(GL|Vk)Test"

# Test install, after running the tests as for them it shouldn't be needed
ninja install
