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
    -DCORRADE_BUILD_DEPRECATED=$BUILD_DEPRECATED \
    -DCORRADE_WITH_INTERCONNECT=OFF \
    -DCORRADE_WITH_PLUGINMANAGER=ON \
    -G Ninja
ninja install
cd ../..

# Enabling only libraries that have opt-in GL or Vulkan functionality (and thus
# should be tested to compile w/ MAGNUM_TARGET_GL disabled), plus dependencies
# of what's directly affected by Vulkan (which means also Trade for icon import
# in Platform tests), disabling everything else. On Linux this uses a newer GCC
# because SwiftShader doesn't compile on 4.8; building of Vulkan Magnum code on
# GCC 4.8 is tested in the combined GL + Vulkan job.
#
# Not using CXXFLAGS in order to avoid affecting dependencies.
mkdir build && cd build
cmake .. \
    -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
    `# Vulkan Loader is custom-built on Mac` \
    -DCMAKE_PREFIX_PATH="$HOME/deps;$HOME/vulkan-loader" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_BUILD_TYPE=Debug \
    -DMAGNUM_WITH_AUDIO=OFF \
    `# Needed by VkImageVkTest, and by VkMeshVkTest, together with` \
    `# TgaImporter and AnyImageImporter` \
    -DMAGNUM_WITH_DEBUGTOOLS=ON \
    -DMAGNUM_WITH_GL=OFF \
    -DMAGNUM_WITH_MATERIALTOOLS=OFF \
    -DMAGNUM_WITH_MESHTOOLS=ON \
    `# Needed by MeshTools tests` \
    -DMAGNUM_WITH_PRIMITIVES=ON \
    -DMAGNUM_WITH_SCENEGRAPH=OFF \
    -DMAGNUM_WITH_SCENETOOLS=OFF \
    -DMAGNUM_WITH_SHADERS=ON \
    -DMAGNUM_WITH_TEXT=ON \
    -DMAGNUM_WITH_TEXTURETOOLS=ON \
    -DMAGNUM_WITH_TRADE=ON \
    -DMAGNUM_WITH_VK=ON \
    -DMAGNUM_WITH_AL_INFO=OFF \
    -DMAGNUM_WITH_VK_INFO=ON \
    -DMAGNUM_WITH_GL_INFO=OFF \
    -DMAGNUM_WITH_ANYAUDIOIMPORTER=OFF \
    -DMAGNUM_WITH_ANYIMAGECONVERTER=OFF \
    -DMAGNUM_WITH_ANYIMAGEIMPORTER=ON \
    -DMAGNUM_WITH_ANYSCENECONVERTER=OFF \
    -DMAGNUM_WITH_ANYSCENEIMPORTER=OFF \
    -DMAGNUM_WITH_ANYSHADERCONVERTER=OFF \
    -DMAGNUM_WITH_MAGNUMFONT=OFF \
    -DMAGNUM_WITH_MAGNUMFONTCONVERTER=OFF \
    -DMAGNUM_WITH_OBJIMPORTER=OFF \
    -DMAGNUM_WITH_TGAIMAGECONVERTER=OFF \
    -DMAGNUM_WITH_TGAIMPORTER=ON \
    -DMAGNUM_WITH_WAVAUDIOIMPORTER=OFF \
    -DMAGNUM_WITH_DISTANCEFIELDCONVERTER=OFF \
    -DMAGNUM_WITH_FONTCONVERTER=OFF \
    -DMAGNUM_WITH_IMAGECONVERTER=OFF \
    -DMAGNUM_WITH_SCENECONVERTER=OFF \
    -DMAGNUM_WITH_SHADERCONVERTER=OFF \
    -DMAGNUM_WITH_SDL2APPLICATION=ON \
    -DMAGNUM_WITH_GLFWAPPLICATION=ON \
    -DMAGNUM_BUILD_TESTS=ON \
    -DMAGNUM_BUILD_GL_TESTS=OFF \
    -DMAGNUM_BUILD_VK_TESTS=ON \
    -DMAGNUM_BUILD_DEPRECATED=$BUILD_DEPRECATED \
    -G Ninja

ninja $NINJA_JOBS

# The GL library shouldn't get built by accident
if ls Debug/lib/libMagnumGL*; then
    echo "The MagnumGL library was built even though it shouldn't"
    false
fi

export VK_ICD_FILENAMES=$HOME/swiftshader/share/vulkan/icd.d/vk_swiftshader_icd.json
export CORRADE_TEST_COLOR=ON

# Keep in sync with PKGBUILD, PKGBUILD-coverage and PKGBUILD-release
ctest -V
MAGNUM_VULKAN_VERSION=1.0 CORRADE_TEST_SKIP_BENCHMARKS=ON ctest -V -R VkTest
MAGNUM_DISABLE_EXTENSIONS="VK_KHR_get_physical_device_properties2 VK_KHR_get_memory_requirements2 VK_KHR_bind_memory2 VK_KHR_create_renderpass2 VK_KHR_copy_commands2 VK_KHR_maintenance1 VK_KHR_multiview VK_KHR_maintenance2" MAGNUM_VULKAN_VERSION=1.0 CORRADE_TEST_SKIP_BENCHMARKS=ON ctest -V -R VkTest

# Test install, after running the tests as for them it shouldn't be needed
ninja install
