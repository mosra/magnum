#!/bin/bash
set -ev

git submodule update --init

# Corrade
git clone --depth 1 https://github.com/mosra/corrade.git
cd corrade

# Build native corrade-rc
mkdir build && cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps-native \
    -DCORRADE_WITH_INTERCONNECT=OFF \
    -DCORRADE_WITH_PLUGINMANAGER=OFF \
    -DCORRADE_WITH_TESTSUITE=OFF \
    -DCORRADE_WITH_UTILITY=OFF \
    -G Ninja
ninja install
cd ..

# Crosscompile Corrade
mkdir build-ios && cd build-ios
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../../toolchains/generic/iOS.cmake \
    -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk \
    -DCMAKE_OSX_ARCHITECTURES="x86_64" \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DCORRADE_BUILD_STATIC=ON \
    -DCORRADE_TESTSUITE_TARGET_XCTEST=ON \
    -DCORRADE_WITH_INTERCONNECT=OFF \
    -G Xcode
set -o pipefail && cmake --build . --config Release --target install -j$XCODE_JOBS | xcbeautify
cd ../..

# Crosscompile SDL. On 2022-14-02 curl says the certificate is expired, so
# ignore that.
# TODO use a CMake build instead
curl --insecure -O https://www.libsdl.org/release/SDL2-2.0.10.tar.gz
tar -xzvf SDL2-2.0.10.tar.gz
cd SDL2-2.0.10/Xcode-iOS/SDL
set -o pipefail && xcodebuild -sdk iphonesimulator -jobs $XCODE_JOBS -parallelizeTargets | xcbeautify
cp build/Release-iphonesimulator/libSDL2.a $HOME/deps/lib
mkdir -p $HOME/deps/include/SDL2
cp -R ../../include/* $HOME/deps/include/SDL2
cd ../../..

# Crosscompile Magnum
mkdir build-ios && cd build-ios
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../toolchains/generic/iOS.cmake \
    -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk \
    -DCMAKE_OSX_ARCHITECTURES="x86_64" \
    -DCMAKE_PREFIX_PATH="$HOME/deps" \
    -DCORRADE_RC_EXECUTABLE=$HOME/deps-native/bin/corrade-rc \
    -DCMAKE_INSTALL_PREFIX=$HOME/deps \
    -DMAGNUM_TARGET_GLES2=$TARGET_GLES2 \
    -DMAGNUM_WITH_AUDIO=ON \
    -DMAGNUM_WITH_VK=OFF \
    -DMAGNUM_WITH_SDL2APPLICATION=ON \
    -DMAGNUM_WITH_WINDOWLESSIOSAPPLICATION=ON \
    -DMAGNUM_WITH_EGLCONTEXT=ON \
    -DMAGNUM_WITH_OPENGLTESTER=ON \
    -DMAGNUM_WITH_ANYAUDIOIMPORTER=ON \
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
    -DMAGNUM_WITH_WAVAUDIOIMPORTER=ON \
    -DMAGNUM_BUILD_STATIC=ON \
    -DMAGNUM_BUILD_TESTS=ON \
    -DMAGNUM_BUILD_GL_TESTS=ON \
    -G Xcode
set -o pipefail && cmake --build . --config Release -j$XCODE_JOBS | xcbeautify

# TODO enable again once https://github.com/mosra/corrade/pull/176 is resolved
# TODO: find a better way to avoid
# Library not loaded: /System/Library/Frameworks/OpenGLES.framework/OpenGLES
# error
# DYLD_FALLBACK_LIBRARY_PATH="/Library/Developer/CoreSimulator/Profiles/Runtimes/iOS 12.4.simruntime/Contents/Resources/RuntimeRoot/System/Library/Frameworks/OpenGLES.framework" DYLD_FALLBACK_FRAMEWORK_PATH="/Library/Developer/CoreSimulator/Profiles/Runtimes/iOS 12.4.simruntime/Contents/Resources/RuntimeRoot/System/Library/Frameworks" CORRADE_TEST_COLOR=ON ctest -V -C Release -E "GLTest|GLBenchmark"

# Test install, after running the tests as for them it shouldn't be needed
set -o pipefail && cmake --build . --config Release --target install -j$XCODE_JOBS | xcbeautify
