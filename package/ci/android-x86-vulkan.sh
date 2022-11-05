#!/bin/bash
set -ev

git submodule update --init

# Corrade
git clone --depth 1 https://github.com/mosra/corrade.git
cd corrade

# Build native corrade-rc
mkdir build && cd build
cmake .. \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps-native \
    -DCMAKE_BUILD_TYPE=Release \
    -DCORRADE_WITH_INTERCONNECT=OFF \
    -DCORRADE_WITH_PLUGINMANAGER=OFF \
    -DCORRADE_WITH_TESTSUITE=OFF \
    -DCORRADE_WITH_UTILITY=OFF \
    -G Ninja
ninja install
cd ..

# Crosscompile Corrade
mkdir build-android-x86 && cd build-android-x86
cmake .. \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_SYSTEM_VERSION=29 \
    -DCMAKE_ANDROID_ARCH_ABI=x86 \
    -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang \
    -DCMAKE_ANDROID_STL_TYPE=c++_static \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DCORRADE_WITH_INTERCONNECT=OFF \
    -G Ninja
ninja install
cd ..

cd ..

# Generate debug keystore for APK signing
keytool -genkeypair -keystore $HOME/.android/debug.keystore -storepass android -alias androiddebugkey -keypass android -keyalg RSA -validity 10000 -dname CN=,OU=,O=,L=,S=,C=

# Crosscompile. Enabling only stuff that's directly affected by Vulkan (which
# means also parts of Platform, which need Trade for icon import in tests),
# disabling everything else. Should be consistent with unix-desktop-vulkan.sh,
# except for features not available on Android like Sdl2 or GlfwApplication.
mkdir build-android-x86 && cd build-android-x86
cmake .. \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_SYSTEM_VERSION=29 \
    -DCMAKE_ANDROID_ARCH_ABI=x86 \
    -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang \
    -DCMAKE_ANDROID_STL_TYPE=c++_static \
    -DCMAKE_FIND_ROOT_PATH="/opt/android/sdk/ndk/21.4.7075529/toolchains/llvm/prebuilt/linux-x86_64/sysroot;$HOME/deps" \
    -DCMAKE_FIND_LIBRARY_CUSTOM_LIB_SUFFIX=/i686-linux-android/29 \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_PREFIX_PATH=$HOME/deps \
    -DCMAKE_BUILD_TYPE=Release \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DMAGNUM_WITH_AUDIO=OFF \
    `# Needed by VkMeshVkTest, together with TgaImporter and AnyImageImporter` \
    -DMAGNUM_WITH_DEBUGTOOLS=ON \
    -DMAGNUM_WITH_GL=OFF \
    -DMAGNUM_WITH_MATERIALTOOLS=OFF \
    -DMAGNUM_WITH_MESHTOOLS=OFF \
    -DMAGNUM_WITH_PRIMITIVES=OFF \
    -DMAGNUM_WITH_SCENEGRAPH=OFF \
    -DMAGNUM_WITH_SCENETOOLS=OFF \
    -DMAGNUM_WITH_SHADERTOOLS=OFF \
    -DMAGNUM_WITH_SHADERS=OFF \
    -DMAGNUM_WITH_TEXT=OFF \
    -DMAGNUM_WITH_TEXTURETOOLS=OFF \
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
    `# TODO: enable back when it can be GL-less` \
    -DMAGNUM_WITH_ANDROIDAPPLICATION=OFF \
    -DMAGNUM_BUILD_TESTS=ON \
    -DMAGNUM_BUILD_GL_TESTS=OFF \
    -DMAGNUM_BUILD_VK_TESTS=ON \
    -G Ninja
ninja $NINJA_JOBS

# Wait for emulator to start (done in parallel to build) and run tests
circle-android wait-for-boot
# `adb push` uploads timeout quite often, and then CircleCI waits 10 minutes
# until it aborts the job due to no output. CTest can do timeouts on its own,
# but somehow the default is 10M seconds, which is quite a lot honestly.
# Instead set the timeout to 15 seconds which should be enough even for very
# heavy future benchmarks, and try two more times if it gets stuck.
CORRADE_TEST_COLOR=ON ctest -V --timeout 15 --repeat after-timeout:3

# Test install, after running the tests as for them it shouldn't be needed
ninja install
