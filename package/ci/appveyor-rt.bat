if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2017" call "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Auxiliary/Build/vcvarsall.bat" x64 || exit /b
if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2017" set GENERATOR=Visual Studio 15 2017
set PATH=%APPVEYOR_BUILD_FOLDER%\deps-native\bin;%PATH%

rem Build ANGLE. The repo is now just a README redirecting to googlesource.
rem I don't want to bother with this right now, so checking out last usable
rem version from 2017. TODO: fix when I can be bothered
git clone --depth 10 git://github.com/MSOpenTech/angle.git || exit /b
cd angle || exit /b
git checkout c61d0488abd9663e0d4d2450db7345baa2c0dfb6 || exit /b
cd winrt\10\src || exit /b
msbuild angle.sln /p:Configuration=Release || exit /b
cd ..\..\..\.. || exit /b

rem Build SDL
appveyor DownloadFile https://www.libsdl.org/release/SDL2-2.0.4.zip || exit /b
7z x SDL2-2.0.4.zip || exit /b
ren SDL2-2.0.4 SDL || exit /b
cd SDL/VisualC-WinRT/UWP_VS2015 || exit/b
msbuild /p:Configuration=Release || exit /b
cd ..\..\..

git clone --depth 1 git://github.com/mosra/corrade.git || exit /b
cd corrade || exit /b

rem Build native corrade-rc
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps-native ^
    -DWITH_INTERCONNECT=OFF ^
    -DWITH_PLUGINMANAGER=OFF ^
    -DWITH_TESTSUITE=OFF ^
    -DWITH_UTILITY=OFF ^
    -G Ninja || exit /b
cmake --build . --target install || exit /b
cd .. || exit /b

rem Crosscompile Corrade
mkdir build-rt && cd build-rt || exit /b
cmake .. ^
    -DCMAKE_SYSTEM_NAME=WindowsStore ^
    -DCMAKE_SYSTEM_VERSION=10.0 ^
    -DCORRADE_RC_EXECUTABLE=%APPVEYOR_BUILD_FOLDER%/deps-native/bin/corrade-rc.exe ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DWITH_INTERCONNECT=OFF ^
    -DBUILD_STATIC=ON ^
    -G "%GENERATOR%" -A x64 || exit /b
cmake --build . --config Release --target install -- /m /v:m || exit /b
cd .. || exit /b

cd .. || exit /b

rem Crosscompile
mkdir build-rt && cd build-rt || exit /b
cmake .. ^
    -DCMAKE_SYSTEM_NAME=WindowsStore ^
    -DCMAKE_SYSTEM_VERSION=10.0 ^
    -DCORRADE_RC_EXECUTABLE=%APPVEYOR_BUILD_FOLDER%/deps-native/bin/corrade-rc.exe ^
    -DCMAKE_PREFIX_PATH=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DEGL_LIBRARY=%APPVEYOR_BUILD_FOLDER%/angle/winrt/10/src/Release_x64/lib/libEGL.lib ^
    -DEGL_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/angle/include ^
    -DOPENGLES2_LIBRARY=%APPVEYOR_BUILD_FOLDER%/angle/winrt/10/src/Release_x64/lib/libGLESv2.lib ^
    -DOPENGLES2_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/angle/include ^
    -DOPENGLES3_LIBRARY=%APPVEYOR_BUILD_FOLDER%/angle/winrt/10/src/Release_x64/lib/libGLESv2.lib ^
    -DOPENGLES3_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/angle/include ^
    -DSDL2_LIBRARY_RELEASE=%APPVEYOR_BUILD_FOLDER%/SDL/VisualC-WinRT/UWP_VS2015/X64/Release/SDL-UWP/SDL2.lib ^
    -DSDL2_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/SDL/include ^
    -DWITH_AUDIO=OFF ^
    -DWITH_SDL2APPLICATION=ON ^
    -DWITH_ANYAUDIOIMPORTER=OFF ^
    -DWITH_ANYIMAGECONVERTER=ON ^
    -DWITH_ANYIMAGEIMPORTER=ON ^
    -DWITH_ANYSCENECONVERTER=ON ^
    -DWITH_ANYSCENEIMPORTER=ON ^
    -DWITH_MAGNUMFONT=ON ^
    -DWITH_MAGNUMFONTCONVERTER=ON ^
    -DWITH_OBJIMPORTER=ON ^
    -DWITH_TGAIMAGECONVERTER=ON ^
    -DWITH_TGAIMPORTER=ON ^
    -DWITH_WAVAUDIOIMPORTER=OFF ^
    -DTARGET_GLES2=%TARGET_GLES2% ^
    -DBUILD_TESTS=ON ^
    -DBUILD_STATIC=ON ^
    -G "%GENERATOR%" -A x64 || exit /b
cmake --build . --config Release -- /m /v:m || exit /b

rem Test install, after running the tests as for them it shouldn't be needed
cmake --build . --config Release --target install || exit /b
