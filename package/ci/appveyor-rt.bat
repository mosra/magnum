if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2017" call "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Auxiliary/Build/vcvarsall.bat" x64 || exit /b
if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2017" set GENERATOR=Visual Studio 15 2017
set PATH=%APPVEYOR_BUILD_FOLDER%\deps-native\bin;%PATH%

rem Build SDL
appveyor DownloadFile https://www.libsdl.org/release/SDL2-2.0.4.zip || exit /b
7z x SDL2-2.0.4.zip || exit /b
ren SDL2-2.0.4 SDL || exit /b
cd SDL/VisualC-WinRT/UWP_VS2015 || exit/b
msbuild /p:Configuration=Release || exit /b
cd ..\..\..

git clone --depth 1 https://github.com/mosra/corrade.git || exit /b
cd corrade || exit /b

rem Build native corrade-rc
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps-native ^
    -DCORRADE_WITH_INTERCONNECT=OFF ^
    -DCORRADE_WITH_PLUGINMANAGER=OFF ^
    -DCORRADE_WITH_TESTSUITE=OFF ^
    -DCORRADE_WITH_UTILITY=OFF ^
    -DCORRADE_BUILD_DEPRECATED=OFF ^
    -G Ninja || exit /b
cmake --build . --target install || exit /b
cd .. || exit /b

rem Crosscompile Corrade. /MP is apparently for more parallel builds than one
rem would get with /m or cmake --build . --parallel. FFS. And then it's not
rem possible to just -DCMAKE_CXX_FLAGS=/MP because that will unset all the
rem default flag the damn thing needs, and one has to do this magic instead.
rem https://discourse.cmake.org/t/strictly-appending-to-cmake-lang-flags/6478/9
mkdir build-rt && cd build-rt || exit /b
set CXXFLAGS=/MP6 && cmake .. -UCMAKE_CXX_FLAGS ^
    -DCMAKE_SYSTEM_NAME=WindowsStore ^
    -DCMAKE_SYSTEM_VERSION=10.0 ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCORRADE_RC_EXECUTABLE=%APPVEYOR_BUILD_FOLDER%/deps-native/bin/corrade-rc.exe ^
    -DCORRADE_WITH_INTERCONNECT=OFF ^
    -DCORRADE_BUILD_STATIC=ON ^
    -DCORRADE_BUILD_DEPRECATED=OFF ^
    -G "%GENERATOR%" -A x64 || exit /b
cmake --build . --config Release --target install -- /m:6 /p:CL_MPcount=6 /v:m || exit /b
cd .. || exit /b

cd .. || exit /b

rem Crosscompile. See above for the /MP mess.
mkdir build-rt && cd build-rt || exit /b
set CXXFLAGS=/MP6 && cmake .. -UCMAKE_CXX_FLAGS ^
    -DCMAKE_SYSTEM_NAME=WindowsStore ^
    -DCMAKE_SYSTEM_VERSION=10.0 ^
    -DCMAKE_PREFIX_PATH=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DSDL2_LIBRARY_RELEASE=%APPVEYOR_BUILD_FOLDER%/SDL/VisualC-WinRT/UWP_VS2015/X64/Release/SDL-UWP/SDL2.lib ^
    -DSDL2_INCLUDE_DIR=%APPVEYOR_BUILD_FOLDER%/SDL/include ^
    -DCORRADE_RC_EXECUTABLE=%APPVEYOR_BUILD_FOLDER%/deps-native/bin/corrade-rc.exe ^
    -DMAGNUM_WITH_AUDIO=OFF ^
    -DMAGNUM_WITH_SDL2APPLICATION=ON ^
    -DMAGNUM_WITH_ANYAUDIOIMPORTER=OFF ^
    -DMAGNUM_WITH_ANYIMAGECONVERTER=ON ^
    -DMAGNUM_WITH_ANYIMAGEIMPORTER=ON ^
    -DMAGNUM_WITH_ANYSCENECONVERTER=ON ^
    -DMAGNUM_WITH_ANYSCENEIMPORTER=ON ^
    -DMAGNUM_WITH_ANYSHADERCONVERTER=ON ^
    -DMAGNUM_WITH_MAGNUMFONT=ON ^
    -DMAGNUM_WITH_MAGNUMFONTCONVERTER=ON ^
    -DMAGNUM_WITH_OBJIMPORTER=ON ^
    -DMAGNUM_WITH_TGAIMAGECONVERTER=ON ^
    -DMAGNUM_WITH_TGAIMPORTER=ON ^
    -DMAGNUM_WITH_WAVAUDIOIMPORTER=OFF ^
    -DMAGNUM_TARGET_GLES2=%TARGET_GLES2% ^
    -DMAGNUM_BUILD_TESTS=ON ^
    -DMAGNUM_BUILD_STATIC=ON ^
    -DMAGNUM_BUILD_DEPRECATED=OFF ^
    -G "%GENERATOR%" -A x64 || exit /b
cmake --build . --config Release -- /m:6 /p:CL_MPcount=6 /v:m || exit /b

rem Test install, after running the tests as for them it shouldn't be needed
cmake --build . --config Release --target install || exit /b
