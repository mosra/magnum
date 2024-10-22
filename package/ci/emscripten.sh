#!/bin/bash
set -ev

git submodule update --init

# Crosscompile Corrade
git clone --depth 1 https://github.com/mosra/corrade.git
cd corrade
mkdir build-emscripten && cd build-emscripten
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="../../toolchains/generic/Emscripten-wasm.cmake" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCORRADE_WITH_INTERCONNECT=OFF \
    $EXTRA_OPTS \
    -G Ninja
ninja install
cd ..

cd ..

# Crosscompile
mkdir build-emscripten && cd build-emscripten
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="../toolchains/generic/Emscripten-wasm.cmake" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_FIND_ROOT_PATH=$HOME/deps \
    -DMAGNUM_WITH_AUDIO=ON \
    -DMAGNUM_WITH_VK=OFF \
    -DMAGNUM_WITH_EMSCRIPTENAPPLICATION=ON \
    -DMAGNUM_WITH_SDL2APPLICATION=ON \
    -DMAGNUM_WITH_WINDOWLESSEGLAPPLICATION=ON \
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
    -DMAGNUM_WITH_DISTANCEFIELDCONVERTER=OFF \
    -DMAGNUM_WITH_FONTCONVERTER=OFF \
    -DMAGNUM_WITH_IMAGECONVERTER=ON \
    -DMAGNUM_WITH_SCENECONVERTER=ON \
    -DMAGNUM_WITH_SHADERCONVERTER=ON \
    -DMAGNUM_WITH_GL_INFO=ON \
    -DMAGNUM_WITH_AL_INFO=ON \
    -DMAGNUM_BUILD_TESTS=ON \
    -DMAGNUM_BUILD_AL_TESTS=ON \
    -DMAGNUM_BUILD_GL_TESTS=ON \
    -DMAGNUM_TARGET_GLES2=$TARGET_GLES2 \
    $EXTRA_OPTS \
    -G Ninja
ninja $NINJA_JOBS

# Test
CORRADE_TEST_COLOR=ON ctest -V -E "GLTest|GLBenchmark|ALTest"

# Test install, after running the tests as for them it shouldn't be needed
ninja install
