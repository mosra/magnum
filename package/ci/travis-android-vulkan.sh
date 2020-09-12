#!/bin/bash
set -ev

# Corrade
git clone --depth 1 git://github.com/mosra/corrade.git
cd corrade

# Build native corrade-rc
mkdir build && cd build
cmake .. \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps-native \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_INTERCONNECT=OFF \
    -DWITH_PLUGINMANAGER=OFF \
    -DWITH_TESTSUITE=OFF \
    -DWITH_UTILITY=OFF \
    -G Ninja
ninja install
cd ..

# Crosscompile Corrade
mkdir build-android-arm && cd build-android-arm
cmake .. \
    -DCMAKE_ANDROID_NDK=$TRAVIS_BUILD_DIR/android-ndk-r16b \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_SYSTEM_VERSION=22 \
    -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a \
    -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang \
    -DCMAKE_ANDROID_STL_TYPE=c++_static \
    -DCMAKE_BUILD_TYPE=Release \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DWITH_INTERCONNECT=OFF \
    -G Ninja
ninja install
cd ..

cd ..

# Generate debug keystore for APK signing
keytool -genkeypair -keystore $HOME/.android/debug.keystore -storepass android -alias androiddebugkey -keypass android -keyalg RSA -validity 10000 -dname CN=,OU=,O=,L=,S=,C=

# *Native* Vulkan requires Android 24, build-tools-25.0.2, android-24 and
# sys-img-armeabi-v7a-android-24 in android.components in the YML, but the
# fucking emulator never starts, which means we can't test shit:
# https://github.com/googlemaps/android-maps-utils/issues/371
# So don't even bother there, use some ass-old version that works, and link to
# our own library, which is the fastest Vulkan implementation IN THE WORLD.
$TRAVIS_BUILD_DIR/android-ndk-r16b/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ --target=armv7-none-linux-androideabi --gcc-toolchain=$TRAVIS_BUILD_DIR/android-ndk-r16b/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64 --sysroot=$TRAVIS_BUILD_DIR/android-ndk-r16b/platforms/android-22/arch-arm  -march=armv7-a -mthumb -mfpu=vfpv3-d16 -mfloat-abi=softfp -funwind-tables -no-canonical-prefixes -D__ANDROID_API__=22 -fexceptions -frtti -O3 -DNDEBUG  -Wl,--fix-cortex-a8  -fPIE -pie -Wl,--gc-sections -Wl,-z,nocopyreloc package/ci/libvulkan.cpp -std=c++11 -c -o vulkan.o
$TRAVIS_BUILD_DIR/android-ndk-r16b/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/arm-linux-androideabi/bin/ar rcs $HOME/libvulkan.a vulkan.o
$TRAVIS_BUILD_DIR/android-ndk-r16b/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/arm-linux-androideabi/bin/ranlib $HOME/libvulkan.a

# Crosscompile
mkdir build-android-arm && cd build-android-arm
cmake .. \
    -DANDROID_SDK=/usr/local/android-sdk \
    -DCMAKE_ANDROID_NDK=$TRAVIS_BUILD_DIR/android-ndk-r16b \
    -DCMAKE_SYSTEM_NAME=Android \
    -DCMAKE_SYSTEM_VERSION=22 \
    -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a \
    -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang \
    -DCMAKE_ANDROID_STL_TYPE=c++_static \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DVulkan_LIBRARY=$HOME/libvulkan.a \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_PREFIX_PATH=$HOME/deps \
    -DCMAKE_FIND_ROOT_PATH=$HOME/deps \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_AUDIO=OFF \
    -DWITH_DEBUGTOOLS=OFF \
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
    -DWITH_ANDROIDAPPLICATION=ON \
    -DBUILD_TESTS=ON \
    -DBUILD_GL_TESTS=OFF \
    -DBUILD_VK_TESTS=ON \
    -G Ninja
ninja

# Start simulator and run tests
echo no | android create avd --force -n test -t android-22 --abi armeabi-v7a
emulator -avd test -no-audio -no-window &
android-wait-for-emulator
CORRADE_TEST_COLOR=ON ctest -V -E VkTest

# Test install, after running the tests as for them it shouldn't be needed
ninja install
