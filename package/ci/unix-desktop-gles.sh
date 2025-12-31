#!/bin/bash
set -ev

# Corrade
git clone --depth 1 --branch next https://github.com/mosra/corrade.git
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
    `# SwiftShader is used only on the Mac ES3 build. On Linux Mesa llvmpipe` \
    `# is used instead and SwiftShader is not even downloaded so this points` \
    `# to a non-existent location and does nothing.` \
    -DCMAKE_PREFIX_PATH="$HOME/deps;$HOME/swiftshader" \
    -DCMAKE_INSTALL_RPATH=$HOME/swiftshader/lib \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
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

export CORRADE_TEST_COLOR=ON
# Don't run any benchmarks. It doesn't make sense for GPU emulation, and
# CPU-side things are run in the usual desktop build already.
export CORRADE_TEST_SKIP_BENCHMARKS=ON

# Keep these in sync with package/archlinux/PKGBUILD-es*
ctest -V -E GLBenchmark
MAGNUM_DISABLE_EXTENSIONS="GL_OES_vertex_array_object GL_NV_framebuffer_multisample GL_NV_framebuffer_blit GL_EXT_robustness GL_EXT_draw_elements_base_vertex" ctest -V -R GLTest
MAGNUM_DISABLE_EXTENSIONS="GL_OES_vertex_array_object GL_NV_framebuffer_multisample GL_NV_framebuffer_blit GL_EXT_robustness GL_EXT_draw_elements_base_vertex GL_OES_draw_elements_base_vertex GL_ANGLE_base_vertex_base_instance" ctest -V -R GLTest
MAGNUM_DISABLE_EXTENSIONS="GL_OES_vertex_array_object GL_NV_framebuffer_multisample GL_NV_framebuffer_blit GL_EXT_robustness GL_EXT_draw_elements_base_vertex GL_OES_draw_elements_base_vertex GL_ANGLE_base_vertex_base_instance GL_EXT_multi_draw_arrays GL_ANGLE_multi_draw" ctest -V -R GLTest
MAGNUM_DISABLE_EXTENSIONS="GL_KHR_debug" ctest -V -R GLTest
Debug/bin/magnum-gl-info --extension-strings
Debug/bin/magnum-gl-info --limits

# Test install, after running the tests as for them it shouldn't be needed
ninja install
