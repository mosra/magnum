#!/bin/bash
set -ev

# Corrade
git clone --depth 1 https://github.com/mosra/corrade.git
cd corrade
mkdir build && cd build
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_INSTALL_RPATH=$HOME/deps/lib \
    -DCMAKE_BUILD_TYPE=$CONFIGURATION \
    -DCORRADE_BUILD_DEPRECATED=$BUILD_DEPRECATED \
    -DCORRADE_BUILD_STATIC=$BUILD_STATIC \
    -DCORRADE_WITH_INTERCONNECT=OFF \
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
    -DMAGNUM_WITH_AUDIO=ON \
    -DMAGNUM_WITH_VK=ON \
    -DMAGNUM_WITH_GLFWAPPLICATION=$BUILD_APPLICATIONS \
    -DMAGNUM_WITH_SDL2APPLICATION=$BUILD_APPLICATIONS \
    -DMAGNUM_WITH_OPENGLTESTER=ON \
    -DMAGNUM_WITH_ANYAUDIOIMPORTER=ON \
    -DMAGNUM_WITH_ANYIMAGECONVERTER=ON \
    -DMAGNUM_WITH_ANYIMAGEIMPORTER=ON \
    -DMAGNUM_WITH_ANYSCENECONVERTER=ON \
    -DMAGNUM_WITH_ANYSCENEIMPORTER=ON \
    -DMAGNUM_WITH_ANYSHADERCONVERTER=ON \
    -DMAGNUM_WITH_MAGNUMFONT=ON \
    -DMAGNUM_WITH_MAGNUMFONTCONVERTER=ON \
    -DMAGNUM_WITH_OBJIMPORTER=ON \
    -DMAGNUM_WITH_TGAIMAGECONVERTER=ON \
    -DMAGNUM_WITH_TGAIMPORTER=ON \
    -DMAGNUM_WITH_WAVAUDIOIMPORTER=ON \
    -DMAGNUM_WITH_DISTANCEFIELDCONVERTER=ON \
    -DMAGNUM_WITH_FONTCONVERTER=ON \
    -DMAGNUM_WITH_IMAGECONVERTER=ON \
    -DMAGNUM_WITH_SCENECONVERTER=ON \
    -DMAGNUM_WITH_SHADERCONVERTER=ON \
    -DMAGNUM_WITH_GL_INFO=ON \
    -DMAGNUM_WITH_VK_INFO=ON \
    -DMAGNUM_WITH_AL_INFO=ON \
    -DMAGNUM_BUILD_TESTS=ON \
    -DMAGNUM_BUILD_GL_TESTS=ON \
    -DMAGNUM_BUILD_VK_TESTS=ON \
    -DMAGNUM_BUILD_DEPRECATED=$BUILD_DEPRECATED \
    -DMAGNUM_BUILD_STATIC=$BUILD_STATIC \
    -DMAGNUM_BUILD_PLUGINS_STATIC=$BUILD_STATIC \
    $EXTRA_OPTS \
    -G Ninja
ninja $NINJA_JOBS
ASAN_OPTIONS="color=always" LSAN_OPTIONS="color=always suppressions=$(pwd)/../package/ci/leaksanitizer.conf" TSAN_OPTIONS="color=always" CORRADE_TEST_COLOR=ON ctest -V -E "GLTest|GLBenchmark|VkTest"

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
