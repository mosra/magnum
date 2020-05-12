if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2019" call "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvarsall.bat" x64 || exit /b
set PATH=%APPVEYOR_BUILD_FOLDER%\deps\bin;%PATH%

rem Build Corrade
git clone --depth 1 git://github.com/mosra/corrade.git || exit /b
cd corrade || exit /b
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DWITH_INTERCONNECT=OFF ^
    -DUTILITY_USE_ANSI_COLORS=ON ^
    -G Ninja || exit /b
cmake --build . || exit /b
cmake --build . --target install || exit /b
cd .. && cd ..

rem Build the fastest Vulkan driver ever. See appveyor.yml for why Vulkan is
rem a separate build for now.
cl.exe /c package/ci/libvulkan.cpp || exit /b
lib.exe /OUT:%APPVEYOR_BUILD_FOLDER%/deps/lib/libvulkan.lib libvulkan.obj || exit /b

rem Enabling only stuff that's directly affected by Vulkan (which means also
rem parts of Platform, which need Trade for icon import in tests), disabling
rem everything else.
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCMAKE_PREFIX_PATH=%APPVEYOR_BUILD_FOLDER%/SDL ^
    -DVulkan_LIBRARY=%APPVEYOR_BUILD_FOLDER%/deps/lib/libvulkan.lib ^
    -DWITH_AUDIO=OFF ^
    -DWITH_DEBUGTOOLS=OFF ^
    -DWITH_GL=OFF ^
    -DWITH_MESHTOOLS=OFF ^
    -DWITH_PRIMITIVES=OFF ^
    -DWITH_SCENEGRAPH=OFF ^
    -DWITH_SHADERS=OFF ^
    -DWITH_TEXT=OFF ^
    -DWITH_TEXTURETOOLS=OFF ^
    -DWITH_TRADE=ON ^
    -DWITH_VK=ON ^
    -DWITH_ANYAUDIOIMPORTER=OFF ^
    -DWITH_ANYIMAGECONVERTER=OFF ^
    -DWITH_ANYIMAGEIMPORTER=OFF ^
    -DWITH_ANYSCENECONVERTER=ON ^
    -DWITH_ANYSCENEIMPORTER=OFF ^
    -DWITH_MAGNUMFONT=OFF ^
    -DWITH_MAGNUMFONTCONVERTER=OFF ^
    -DWITH_OBJIMPORTER=OFF ^
    -DWITH_TGAIMAGECONVERTER=OFF ^
    -DWITH_TGAIMPORTER=OFF ^
    -DWITH_WAVAUDIOIMPORTER=OFF ^
    -DWITH_DISTANCEFIELDCONVERTER=OFF ^
    -DWITH_FONTCONVERTER=OFF ^
    -DWITH_IMAGECONVERTER=OFF ^
    -DWITH_SCENECONVERTER=OFF ^
    -DWITH_GL_INFO=OFF ^
    -DWITH_VK_INFO=ON ^
    -DWITH_AL_INFO=OFF ^
    -DWITH_SDL2APPLICATION=ON ^
    -DWITH_GLFWAPPLICATION=ON ^
    -DBUILD_TESTS=ON ^
    -DBUILD_GL_TESTS=OFF ^
    -DBUILD_VK_TESTS=ON ^
    -G Ninja || exit /b
cmake --build . || exit /b

rem Test
set CORRADE_TEST_COLOR=ON
ctest -V -E "(GL|Vk)Test" || exit /b

rem Test install, after running the tests as for them it shouldn't be needed
cmake --build . --target install || exit /b
