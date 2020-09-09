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

# Enabling both GL and Vulkan but not running rendering tests for either --
# that's done by separate GLES and Vulkan jobs. For GL it's because there's no
# way to test desktop GL on SwiftShader, for Vulkan it's because:
# -  SwiftShader doesn't compile on GCC 4.8 and we *need* to test compile on
#    that,
# -  SwiftShader for macOS is built on 10.15 and we want to test on older
#    versions as well
# so we need two jobs to verify both compilation and rendering.
#
# Not using CXXFLAGS in order to avoid affecting dependencies.
mkdir build && cd build
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_BUILD_TYPE=$CONFIGURATION \
    -DWITH_AUDIO=ON \
    -DWITH_VK=ON \
    -DWITH_GLFWAPPLICATION=$BUILD_APPLICATIONS \
    -DWITH_SDL2APPLICATION=$BUILD_APPLICATIONS \
    -DWITH_WINDOWLESS${PLATFORM_GL_API}APPLICATION=ON \
    -DWITH_${PLATFORM_GL_API}CONTEXT=ON \
    -DWITH_OPENGLTESTER=ON \
    -DWITH_ANYAUDIOIMPORTER=ON \
    -DWITH_ANYIMAGECONVERTER=ON \
    -DWITH_ANYIMAGEIMPORTER=ON \
    -DWITH_ANYSCENECONVERTER=ON \
    -DWITH_ANYSCENEIMPORTER=ON \
    -DWITH_ANYSHADERCONVERTER=ON \
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
    -DWITH_SHADERCONVERTER=ON \
    -DWITH_GL_INFO=ON \
    -DWITH_VK_INFO=ON \
    -DWITH_AL_INFO=ON \
    -DBUILD_TESTS=ON \
    -DBUILD_GL_TESTS=ON \
    -DBUILD_VK_TESTS=ON \
    -DBUILD_DEPRECATED=$BUILD_DEPRECATED \
    -DBUILD_STATIC=$BUILD_STATIC \
    -DBUILD_PLUGINS_STATIC=$BUILD_STATIC \
    -G Ninja
ninja
ASAN_OPTIONS="color=always" LSAN_OPTIONS="color=always suppressions=$(pwd)/../package/ci/leaksanitizer.conf" TSAN_OPTIONS="color=always" CORRADE_TEST_COLOR=ON ctest -V -E "(GL|Vk)Test"

# Test install, after running the tests as for them it shouldn't be needed
ninja install

cd ..

# Verify also compilation of the documentation image generators, if apps are
# built
if [ "$BUILD_APPLICATIONS" != "OFF" ]; then
    mkdir build-doc-generated && cd build-doc-generated
    cmake ../doc/generated \
        -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
        -DCMAKE_PREFIX_PATH=$HOME/deps \
        -DCMAKE_BUILD_TYPE=Debug \
        -G Ninja
    ninja
fi
