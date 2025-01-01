if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2022" call "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvarsall.bat" %PLATFORM% || exit /b
if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2019" call "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvarsall.bat" %PLATFORM% || exit /b
if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2017" call "C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Auxiliary/Build/vcvarsall.bat" %PLATFORM% || exit /b
if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2015" call "C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/vcvarsall.bat" %PLATFORM% || exit /b
set PATH=%APPVEYOR_BUILD_FOLDER%\deps\bin;%PATH%
rem OpenAL DLL is renamed & copied to magnum's bin dir automatically by the
rem buildsystem, no need to do that here anymore

rem - Need to explicitly specify a 64-bit target, otherwise CMake+Ninja can't
rem   figure that out -- https://gitlab.kitware.com/cmake/cmake/issues/16259
rem - For TestSuite we need to enable exceptions explicitly with /EH as these
rem   are currently disabled -- https://github.com/catchorg/Catch2/issues/1113
rem - The MSVC 2022 build has coverage enabled, for which the profile library
rem   has to be explicitly linked -- https://bugs.llvm.org/show_bug.cgi?id=40877,
rem   https://gitlab.kitware.com/cmake/cmake/-/issues/23437
if "%COMPILER%" == "msvc-clang" if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2022" set COMPILER_EXTRA=-DCMAKE_CXX_COMPILER="C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/bin/clang-cl.exe" -DCMAKE_LINKER="C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/bin/lld-link.exe" -DCMAKE_CXX_FLAGS="-m64 /EHsc --coverage" -DCMAKE_EXE_LINKER_FLAGS="-libpath:\"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/lib/clang/18/lib/windows\" \"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/lib/clang/18/lib/windows/clang_rt.profile-x86_64.lib\"" -DCMAKE_SHARED_LINKER_FLAGS="-libpath:\"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/lib/clang/18/lib/windows\" \"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/lib/clang/18/lib/windows/clang_rt.profile-x86_64.lib\""
if "%COMPILER%" == "msvc-clang" if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2019" set COMPILER_EXTRA=-DCMAKE_CXX_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/Llvm/bin/clang-cl.exe" -DCMAKE_LINKER="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/Llvm/bin/lld-link.exe" -DCMAKE_CXX_FLAGS="-m64 /EHsc"

rem Build Corrade
git clone --depth 1 https://github.com/mosra/corrade.git || exit /b
cd corrade || exit /b
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCORRADE_WITH_INTERCONNECT=OFF ^
    -DCORRADE_UTILITY_USE_ANSI_COLORS=ON ^
    -DCORRADE_BUILD_STATIC=%BUILD_STATIC% ^
    %COMPILER_EXTRA% -G Ninja || exit /b
cmake --build . || exit /b
cmake --build . --target install || exit /b
cd .. && cd ..

rem Build
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCMAKE_PREFIX_PATH="%APPVEYOR_BUILD_FOLDER%/SDL;%APPVEYOR_BUILD_FOLDER%/openal" ^
    -DMAGNUM_WITH_AUDIO=ON ^
    -DMAGNUM_WITH_VK=%ENABLE_VULKAN% ^
    -DMAGNUM_WITH_SDL2APPLICATION=ON ^
    -DMAGNUM_WITH_GLFWAPPLICATION=ON ^
    -DMAGNUM_WITH_WINDOWLESSWGLAPPLICATION=ON ^
    -DMAGNUM_WITH_WGLCONTEXT=ON ^
    -DMAGNUM_WITH_OPENGLTESTER=ON ^
    -DMAGNUM_WITH_ANYAUDIOIMPORTER=ON ^
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
    -DMAGNUM_WITH_WAVAUDIOIMPORTER=ON ^
    -DMAGNUM_WITH_DISTANCEFIELDCONVERTER=ON ^
    -DMAGNUM_WITH_FONTCONVERTER=ON ^
    -DMAGNUM_WITH_IMAGECONVERTER=ON ^
    -DMAGNUM_WITH_SCENECONVERTER=ON ^
    -DMAGNUM_WITH_SHADERCONVERTER=ON ^
    -DMAGNUM_WITH_GL_INFO=ON ^
    -DMAGNUM_WITH_VK_INFO=%ENABLE_VULKAN% ^
    -DMAGNUM_WITH_AL_INFO=ON ^
    -DMAGNUM_BUILD_TESTS=ON ^
    -DMAGNUM_BUILD_GL_TESTS=ON ^
    -DMAGNUM_BUILD_VK_TESTS=%ENABLE_VULKAN% ^
    -DMAGNUM_BUILD_STATIC=%BUILD_STATIC% ^
    -DMAGNUM_BUILD_PLUGINS_STATIC=%BUILD_STATIC% ^
    %COMPILER_EXTRA% -G Ninja || exit /b
cmake --build . || exit /b

rem Test
set CORRADE_TEST_COLOR=ON
rem On Windows, if an assertion or other issue happens, A DIALOG WINDOWS POPS
rem UP FROM THE CONSOLE. And then, for fucks sake, IT WAITS ENDLESSLY FOR YOU
rem TO CLOSE IT!! Such behavior is utterly stupid in a non-interactive setting
rem such as on this very CI, so I'm setting a timeout to 60 seconds to avoid
rem the CI job being stuck for an hour if an assertion happens. CTest's default
rem timeout is somehow 10M seconds, which is as useful as nothing at all.
ctest -V -E "GLTest|GLBenchmark|VkTest" --timeout 60 || exit /b

rem Test install, after running the tests as for them it shouldn't be needed
cmake --build . --target install || exit /b

cd %APPVEYOR_BUILD_FOLDER%

rem Gather and upload coverage on the clang-cl MSVC 2022 build. Keep in sync
rem with circleci.yml, appveyor-desktop-mingw.bat and PKBUILD-coverage, please.
if "%COMPILER%" == "msvc-clang" if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2022" grcov build -t lcov --keep-only "*/src/Magnum*/*" --ignore "*/src/MagnumExternal/*" --ignore "*/Test/*" --ignore "*/build/src/*" -o coverage.info --excl-line LCOV_EXCL_LINE --excl-start LCOV_EXCL_START --excl-stop LCOV_EXCL_STOP || exit /b
rem Official docs say "not needed for public repos", in reality not using the
rem token is "extremely flakey". What's best is that if the upload fails, the
rem damn thing exits with a success error code, and nobody cares:
rem https://github.com/codecov/codecov-circleci-orb/issues/139
rem https://community.codecov.com/t/commit-sha-does-not-match-circle-build/4266
if "%COMPILER%" == "msvc-clang" if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2022" codecov -f ./coverage.info -t 3b1a7cc0-8bfb-4cde-8d99-dc04572a43c3 -X gcov || exit /b
