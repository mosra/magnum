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
    -DCMAKE_PREFIX_PATH="$HOME/deps;$HOME/swiftshader" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_INSTALL_RPATH=$HOME/swiftshader/lib \
    -DTARGET_GLES=ON \
    -DTARGET_GLES2=$TARGET_GLES2 \
    -DWITH_AUDIO=OFF \
    -DWITH_VK=OFF \
    -DWITH_GLFWAPPLICATION=ON \
    -DWITH_SDL2APPLICATION=ON \
    -DWITH_WINDOWLESS${PLATFORM_GL_API}APPLICATION=ON \
    -DWITH_${PLATFORM_GL_API}CONTEXT=ON \
    -DWITH_OPENGLTESTER=ON \
    `# Some plugins have to be enabled in order to test rendering output` \
    `# Converter plugins are needed by the DebugTools::screenshot() test` \
    -DWITH_ANYAUDIOIMPORTER=OFF \
    -DWITH_ANYIMAGECONVERTER=ON \
    -DWITH_ANYIMAGEIMPORTER=ON \
    -DWITH_ANYSCENECONVERTER=OFF \
    -DWITH_ANYSCENEIMPORTER=OFF \
    -DWITH_ANYSHADERCONVERTER=OFF \
    -DWITH_MAGNUMFONT=ON \
    -DWITH_MAGNUMFONTCONVERTER=ON \
    -DWITH_OBJIMPORTER=OFF \
    -DWITH_TGAIMAGECONVERTER=ON \
    -DWITH_TGAIMPORTER=ON \
    -DWITH_WAVAUDIOIMPORTER=OFF \
    -DWITH_DISTANCEFIELDCONVERTER=OFF \
    -DWITH_FONTCONVERTER=OFF \
    -DWITH_IMAGECONVERTER=OFF \
    -DWITH_SCENECONVERTER=OFF \
    -DWITH_SHADERCONVERTER=OFF \
    -DWITH_GL_INFO=ON \
    -DWITH_AL_INFO=OFF \
    -DBUILD_TESTS=ON \
    -DBUILD_GL_TESTS=ON \
    -G Ninja
ninja $NINJA_JOBS

# Don't run any benchmarks. SwiftShader doesn't support
# EXT_disjoint_timer_query anyway and the CPU-side things are run in the usual
# desktop build already.
export CORRADE_TEST_SKIP_BENCHMARKS=ON

CORRADE_TEST_COLOR=ON ctest -V
MAGNUM_DISABLE_EXTENSIONS="GL_OES_vertex_array_object GL_NV_framebuffer_multisample GL_NV_framebuffer_blit GL_EXT_robustness GL_EXT_draw_elements_base_vertex" CORRADE_TEST_COLOR=ON ctest --output-on-failure -j5 -R GLTest
MAGNUM_DISABLE_EXTENSIONS="GL_OES_vertex_array_object GL_NV_framebuffer_multisample GL_NV_framebuffer_blit GL_EXT_robustness GL_EXT_draw_elements_base_vertex GL_OES_draw_elements_base_vertex GL_ANGLE_base_vertex_base_instance" CORRADE_TEST_COLOR=ON ctest --output-on-failure -j5 -R GLTest
MAGNUM_DISABLE_EXTENSIONS="GL_OES_vertex_array_object GL_NV_framebuffer_multisample GL_NV_framebuffer_blit GL_EXT_robustness GL_EXT_draw_elements_base_vertex GL_OES_draw_elements_base_vertex GL_ANGLE_base_vertex_base_instance GL_EXT_multi_draw_arrays GL_ANGLE_multi_draw" CORRADE_TEST_COLOR=ON ctest --output-on-failure -j5 -R GLTest
MAGNUM_DISABLE_EXTENSIONS="GL_KHR_debug" CORRADE_TEST_COLOR=ON ctest --output-on-failure -j5 -R GLTest
Debug/bin/magnum-gl-info --limits > /dev/null

# Test install, after running the tests as for them it shouldn't be needed
ninja install
