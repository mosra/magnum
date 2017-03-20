#!/bin/bash
set -ev

git submodule update --init

# Corrade
git clone --depth 1 git://github.com/mosra/corrade.git
cd corrade

# Build native corrade-rc
mkdir build && cd build
cmake .. \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps-native \
    -DCMAKE_INSTALL_RPATH=$HOME/deps-native/lib \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_INTERCONNECT=OFF \
    -DWITH_PLUGINMANAGER=OFF \
    -DWITH_TESTSUITE=OFF
make -j install
cd ..

# Crosscompile Corrade
mkdir build-android-arm && cd build-android-arm
ANDROID_NDK=$TRAVIS_BUILD_DIR/android-ndk-r10e cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../toolchains/generic/Android-ARM.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DWITH_INTERCONNECT=OFF
make -j install
cd ..

cd ..

# Crosscompile
mkdir build-android-arm && cd build-android-arm
ANDROID_NDK=$TRAVIS_BUILD_DIR/android-ndk-r10e cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../toolchains/generic/Android-ARM.cmake \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DCMAKE_PREFIX_PATH=$HOME/deps \
    -DCMAKE_FIND_ROOT_PATH=$HOME/deps \
    -DCMAKE_BUILD_TYPE=Release \
    -DTARGET_GLES2=$TARGET_GLES2 \
    -DWITH_AUDIO=OFF \
    -DWITH_ANDROIDAPPLICATION=ON \
    -DWITH_EGLCONTEXT=ON \
    -DWITH_MAGNUMFONT=ON \
    -DWITH_MAGNUMFONTCONVERTER=ON \
    -DWITH_OBJIMPORTER=ON \
    -DWITH_TGAIMAGECONVERTER=ON \
    -DWITH_TGAIMPORTER=ON \
    -DWITH_WAVAUDIOIMPORTER=OFF \
    -DBUILD_TESTS=ON
# Otherwise the job gets killed (probably because using too much memory)
make -j4

# Start simulator and run tests
echo no | android create avd --force -n test -t android-19 --abi armeabi-v7a
emulator -avd test -no-audio -no-window &
android-wait-for-emulator
CORRADE_TEST_COLOR=ON ctest -V
