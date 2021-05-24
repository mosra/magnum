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

# Enabling only stuff that's directly affected by Vulkan (which means also
# parts of Platform, which need Trade for icon import in tests), disabling
# everything else. On Linux this uses a newer GCC because SwiftShader doesn't
# compile on 4.8; building of Vulkan Magnum code on GCC 4.8 is tested in the
# combined GL + Vulkan job.
#
# Not using CXXFLAGS in order to avoid affecting dependencies.
mkdir build && cd build
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    `# Vulkan Loader is custom-built on Mac` \
    -DCMAKE_PREFIX_PATH="$HOME/deps;$HOME/vulkan-loader" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_BUILD_TYPE=Debug \
    -DWITH_AUDIO=OFF \
    -DWITH_DEBUGTOOLS=ON \
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

export VK_ICD_FILENAMES=$HOME/swiftshader/share/vulkan/icd.d/vk_swiftshader_icd.json
export CORRADE_TEST_COLOR=ON

# Keep in sync with PKGBUILD, PKGBUILD-coverage and PKGBUILD-release
ctest -V
MAGNUM_VULKAN_VERSION=1.0 CORRADE_TEST_SKIP_BENCHMARKS=ON ctest -V -R VkTest
MAGNUM_DISABLE_EXTENSIONS="VK_KHR_get_physical_device_properties2 VK_KHR_get_memory_requirements2 VK_KHR_bind_memory2 VK_KHR_create_renderpass2 VK_KHR_copy_commands2 VK_KHR_maintenance1 VK_KHR_multiview VK_KHR_maintenance2" MAGNUM_VULKAN_VERSION=1.0 CORRADE_TEST_SKIP_BENCHMARKS=ON ctest -V -R VkTest

# Test install, after running the tests as for them it shouldn't be needed
ninja install
