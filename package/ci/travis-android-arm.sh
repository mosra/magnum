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
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCMAKE_PREFIX_PATH=$HOME/deps \
    -DCMAKE_FIND_ROOT_PATH=$HOME/deps \
    -DCMAKE_BUILD_TYPE=Release \
    -DTARGET_GLES2=$TARGET_GLES2 \
    -DWITH_AUDIO=OFF \
    -DWITH_VK=OFF \
    -DWITH_ANDROIDAPPLICATION=ON \
    -DWITH_EGLCONTEXT=ON \
    -DWITH_ANYAUDIOIMPORTER=OFF \
    -DWITH_ANYIMAGECONVERTER=ON \
    -DWITH_ANYIMAGEIMPORTER=ON \
    -DWITH_ANYSCENECONVERTER=ON \
    -DWITH_ANYSCENEIMPORTER=ON \
    -DWITH_ANYSHADERCONVERTER=ON \
    -DWITH_MAGNUMFONT=ON \
    -DWITH_MAGNUMFONTCONVERTER=ON \
    -DWITH_OBJIMPORTER=ON \
    -DWITH_TGAIMAGECONVERTER=ON \
    -DWITH_TGAIMPORTER=ON \
    -DWITH_WAVAUDIOIMPORTER=OFF \
    -DWITH_GL_INFO=ON \
    -DBUILD_TESTS=ON \
    -DBUILD_GL_TESTS=ON \
    -G Ninja
# Otherwise the job gets killed (probably because using too much memory)
ninja -j4

# Start simulator and run tests
echo no | android create avd --force -n test -t android-22 --abi armeabi-v7a
emulator -avd test -no-audio -no-window &
android-wait-for-emulator
CORRADE_TEST_COLOR=ON ctest -V -E GLTest

# Test install, after running the tests as for them it shouldn't be needed
ninja install
