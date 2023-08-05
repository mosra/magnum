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

# Crosscompile
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
    -DMAGNUM_TARGET_GLES2=$TARGET_GLES2 \
    -DMAGNUM_WITH_AUDIO=OFF \
    -DMAGNUM_WITH_VK=OFF \
    -DMAGNUM_WITH_ANDROIDAPPLICATION=ON \
    -DMAGNUM_WITH_EGLCONTEXT=ON \
    -DMAGNUM_WITH_ANYAUDIOIMPORTER=OFF \
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
    -DMAGNUM_WITH_WAVAUDIOIMPORTER=OFF \
    -DMAGNUM_WITH_DISTANCEFIELDCONVERTER=ON \
    -DMAGNUM_WITH_FONTCONVERTER=ON \
    -DMAGNUM_WITH_IMAGECONVERTER=ON \
    -DMAGNUM_WITH_SCENECONVERTER=ON \
    -DMAGNUM_WITH_SHADERCONVERTER=ON \
    -DMAGNUM_WITH_GL_INFO=ON \
    -DMAGNUM_BUILD_TESTS=ON \
    -DMAGNUM_BUILD_GL_TESTS=ON \
    -G Ninja
ninja $NINJA_JOBS

# Wait for emulator to start (done in parallel to build) and run tests
circle-android wait-for-boot
# SwiftShader on the Android 29 image advertises itself as 4.0.0.1, however
# who the fuck knows, actually. The repository has no tags, so the only way
# to know the version is to check for changes to src/Common/Version.h (and
# then there's a SEPARATE Version.hpp for Vulkan!), and then use a crystal ball
# to figure out if the binary that reports such version is built from the
# commit that changed the version number or from the 100 commits after until
# the version got changed again.
#
# So there's the commit https://github.com/google/swiftshader/commit/c8fcfd82093bb2286e576ec76ef29c5480d35381,
# where it's APPARENT that GLES3 contexts were supported already in 3.3. Yet
# the cursed Android binary just doesn't allow those created, and any attempts
# to use EGL_CONFIG_CAVEAT lead to nothing. The fucked eglChooseConfig() just
# returns EGL_FALSE (and EGL_SUCCESS) always, which isn't even possible to
# happen from looking at the code.
#
# Then, Android has its own repository (fork? some stupid other something?) at
# https://android.googlesource.com/platform/external/swiftshader/ and it's full
# of tags and branches that even gitk blew up trying to visualize the damn
# thing. I tried looking at what version is used in platform-tools-29 or
# Android 10, it said 4.1 both times, which is totally fucked as it reports
# 4.0.0.1 to me in the Android 29 image!!!
#
# As a last resort, because the damn thing advertises itself as OpenGL ES 3.0
# in the version string, I tried to just create a GLES2 context and used it as
# version 3 instead. The result wasn't great, it was beyond awful, like if the
# driver was just random bits and crashes lumped together. Ugh. And of course,
# none of this reproducible with a local non-Android build. And then, THEN, the
# same Android image has a *perfectly fine* Vulkan SwiftShader binary that
# reports 1.1 and has just one, ONE bug, compared to what I use natively. I
# can't even.
if [[ "$TARGET_GLES2" == "OFF" ]]; then
    EXCLUDE_GL_TESTS="-E (GLTest|GLBenchmark)"
fi
# `adb push` uploads timeout quite often, and then CircleCI waits 10 minutes
# until it aborts the job due to no output. CTest can do timeouts on its own,
# but somehow the default is 10M seconds, which is quite a lot honestly.
# Instead set the timeout to 120 seconds (to account for GL tests), and try ten
# more times if it gets stuck. We're sending quite a few files for shader
# tests, so three isn't enough.
CORRADE_TEST_COLOR=ON ctest -V $EXCLUDE_GL_TESTS --timeout 120 --repeat after-timeout:10

# Test install, after running the tests as for them it shouldn't be needed
ninja install
