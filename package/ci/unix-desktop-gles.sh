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
    -DCMAKE_BUILD_TYPE=Debug \
    -DCORRADE_WITH_INTERCONNECT=OFF \
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
    -DMAGNUM_TARGET_GLES=ON \
    -DMAGNUM_TARGET_GLES2=$TARGET_GLES2 \
    -DMAGNUM_WITH_AUDIO=OFF \
    -DMAGNUM_WITH_MATERIALTOOLS=OFF \
    -DMAGNUM_WITH_SCENETOOLS=OFF \
    -DMAGNUM_WITH_SHADERTOOLS=OFF \
    -DMAGNUM_WITH_VK=OFF \
    -DMAGNUM_WITH_GLFWAPPLICATION=ON \
    -DMAGNUM_WITH_SDL2APPLICATION=ON \
    -DMAGNUM_WITH_OPENGLTESTER=ON \
    `# Some plugins have to be enabled in order to test rendering output` \
    `# Converter plugins are needed by the DebugTools::screenshot() test` \
    -DMAGNUM_WITH_ANYAUDIOIMPORTER=OFF \
    -DMAGNUM_WITH_ANYIMAGECONVERTER=ON \
    -DMAGNUM_WITH_ANYIMAGEIMPORTER=ON \
    -DMAGNUM_WITH_ANYSCENECONVERTER=OFF \
    -DMAGNUM_WITH_ANYSCENEIMPORTER=OFF \
    -DMAGNUM_WITH_ANYSHADERCONVERTER=OFF \
    -DMAGNUM_WITH_MAGNUMFONT=ON \
    -DMAGNUM_WITH_MAGNUMFONTCONVERTER=ON \
    -DMAGNUM_WITH_OBJIMPORTER=OFF \
    -DMAGNUM_WITH_TGAIMAGECONVERTER=ON \
    -DMAGNUM_WITH_TGAIMPORTER=ON \
    -DMAGNUM_WITH_WAVAUDIOIMPORTER=OFF \
    -DMAGNUM_WITH_DISTANCEFIELDCONVERTER=$TARGET_GLES3 \
    -DMAGNUM_WITH_FONTCONVERTER=OFF \
    -DMAGNUM_WITH_IMAGECONVERTER=OFF \
    -DMAGNUM_WITH_SCENECONVERTER=OFF \
    -DMAGNUM_WITH_SHADERCONVERTER=OFF \
    -DMAGNUM_WITH_GL_INFO=ON \
    -DMAGNUM_WITH_AL_INFO=OFF \
    -DMAGNUM_BUILD_TESTS=ON \
    -DMAGNUM_BUILD_GL_TESTS=ON \
    $EXTRA_OPTS \
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
Debug/bin/magnum-gl-info --limits

# Test install, after running the tests as for them it shouldn't be needed
ninja install
