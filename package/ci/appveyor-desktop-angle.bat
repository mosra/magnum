call "C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/vcvarsall.bat" x64 || exit /b
set PATH=%APPVEYOR_BUILD_FOLDER%/openal/bin/Win64;%APPVEYOR_BUILD_FOLDER%\deps\bin;%PATH%

rem Build ANGLE
appveyor DownloadFile https://storage.googleapis.com/chrome-infra/depot_tools.zip || exit /b
mkdir depot_tools && cd depot_tools || exit /b
7z x ..\depot_tools.zip || exit /b
cd .. || exit /b
set PATH=%APPVEYOR_BUILD_FOLDER%\depot_tools;%PATH% || exit /b
set GYP_GENERATORS=ninja || exit /b
set GYP_DEFINES=windows_sdk_path="C:/Program Files (x86)/Windows Kits/8.1" || exit /b
git clone --depth 1 git://github.com/google/angle.git || exit /b
cd angle || exit /b
python scripts/bootstrap.py || exit /b
cmd /c "gclient sync && endlocal" || exit /b
ninja -C out/Release_x64 libEGL libGLESv2 || exit /b
mkdir ..\deps || exit /b
mkdir ..\deps\lib || exit /b
mkdir ..\deps\bin || exit /b
mkdir ..\deps\include || exit /b
copy out\Release_x64\d3dcompiler_47.dll ..\deps\bin\ || exit /b
copy out\Release_x64\libGLESv2.dll.lib ..\deps\lib\libGLESv2.lib || exit /b
copy out\Release_x64\libGLESv2.dll ..\deps\bin\ || exit /b
copy out\Release_x64\libEGL.dll.lib ..\deps\lib\libEGL.lib || exit /b
copy out\Release_x64\libEGL.dll ..\deps\bin\ || exit /b
xcopy /e include\* ..\deps\include\ || exit /b

rem Build Corrade
git clone --depth 1 git://github.com/mosra/corrade.git || exit /b
cd corrade || exit /b
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DWITH_INTERCONNECT=OFF ^
    -G Ninja || exit /b
cmake --build . || exit /b
cmake --build . --target install || exit /b
cd .. && cd ..

rem Build
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCMAKE_PREFIX_PATH="%APPVEYOR_BUILD_FOLDER%/openal" ^
    -DTARGET_GLES=ON ^
    -DTARGET_GLES2=%TARGET_GLES2% ^
    -DWITH_AUDIO=ON ^
    -DWITH_SDL2APPLICATION=OFF ^
    -DWITH_WINDOWLESSWINDOWSEGLAPPLICATION=ON ^
    -DWITH_EGLCONTEXT=ON ^
    -DWITH_OPENGLTESTER=ON ^
    -DWITH_MAGNUMFONT=ON ^
    -DWITH_MAGNUMFONTCONVERTER=ON ^
    -DWITH_OBJIMPORTER=ON ^
    -DWITH_TGAIMAGECONVERTER=ON ^
    -DWITH_TGAIMPORTER=ON ^
    -DWITH_WAVAUDIOIMPORTER=ON ^
    -DWITH_DISTANCEFIELDCONVERTER=OFF ^
    -DWITH_FONTCONVERTER=OFF ^
    -DWITH_IMAGECONVERTER=ON ^
    -DWITH_MAGNUMINFO=ON ^
    -DWITH_AL_INFO=ON ^
    -DBUILD_TESTS=ON ^
    -DBUILD_GL_TESTS=ON ^
    -G Ninja || exit /b
cmake --build . || exit /b
cmake --build . --target install || exit /b

rem Test
ctest -V -E GLTest || exit /b
