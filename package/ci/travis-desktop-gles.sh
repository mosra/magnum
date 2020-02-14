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
    -DWITH_INTERCONNECT=OFF \
    -G Ninja
ninja install
cd ../..

mkdir build && cd build
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    -DCMAKE_PREFIX_PATH="$HOME/deps" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DEGL_LIBRARY=$HOME/swiftshader/libEGL.so \
    -DOPENGLES2_LIBRARY=$HOME/swiftshader/libGLESv2.so \
    -DOPENGLES3_LIBRARY=$HOME/swiftshader/libGLESv2.so \
    -DCMAKE_INSTALL_RPATH=$HOME/swiftshader \
    -DTARGET_GLES=ON \
    -DTARGET_GLES2=$TARGET_GLES2 \
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
    -DWITH_DISTANCEFIELDCONVERTER=OFF \
    -DWITH_FONTCONVERTER=OFF \
    -DWITH_IMAGECONVERTER=ON \
    -DWITH_SCENECONVERTER=ON \
    -DWITH_GL_INFO=ON \
    -DWITH_AL_INFO=ON \
    -DBUILD_TESTS=ON \
    -DBUILD_GL_TESTS=ON \
    -G Ninja
# Otherwise the job gets killed (probably because using too much memory)
ninja -j4
CORRADE_TEST_COLOR=ON ctest -V
if [ "$TARGET_GLES2" == "ON" ]; then CORRADE_TEST_COLOR=ON MAGNUM_DISABLE_EXTENSIONS="OES_vertex_array_object" ctest -V -R GLTest; fi
Debug/bin/magnum-al-info > /dev/null
Debug/bin/magnum-gl-info --limits > /dev/null

# Test install, after running the tests as for them it shouldn't be needed
ninja install
