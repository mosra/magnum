rem Workaround for CMake not wanting sh.exe on PATH for MinGW. AARGH.
set PATH=%PATH:C:\Program Files\Git\usr\bin;=%
set PATH=C:\mingw-w64\x86_64-7.2.0-posix-seh-rt_v5-rev1\mingw64\bin;%APPVEYOR_BUILD_FOLDER%\deps\bin;%PATH%
rem OpenAL DLL is renamed & copied to magnum's bin dir automatically by the
rem buildsystem, no need to do that here anymore

rem Build Corrade
git clone --depth 1 https://github.com/mosra/corrade.git || exit /b
cd corrade || exit /b
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_CXX_FLAGS="--coverage" ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCORRADE_WITH_INTERCONNECT=OFF ^
    -DCORRADE_UTILITY_USE_ANSI_COLORS=ON ^
    -G Ninja || exit /b
cmake --build . || exit /b
cmake --build . --target install || exit /b
cd .. && cd ..

rem Build
mkdir build && cd build || exit /b
cmake .. ^
    -DCMAKE_CXX_FLAGS="--coverage" ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_INSTALL_PREFIX=%APPVEYOR_BUILD_FOLDER%/deps ^
    -DCMAKE_PREFIX_PATH="%APPVEYOR_BUILD_FOLDER%/SDL;%APPVEYOR_BUILD_FOLDER%/openal" ^
    -DMAGNUM_WITH_AUDIO=ON ^
    -DMAGNUM_WITH_VK=ON ^
    -DMAGNUM_WITH_SDL2APPLICATION=ON ^
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
    -DMAGNUM_WITH_VK_INFO=ON ^
    -DMAGNUM_WITH_AL_INFO=ON ^
    -DMAGNUM_BUILD_TESTS=ON ^
    -DMAGNUM_BUILD_GL_TESTS=ON ^
    -DMAGNUM_BUILD_VK_TESTS=ON ^
    -G Ninja || exit /b
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

rem Coverage upload
cd %APPVEYOR_BUILD_FOLDER%
set GCOV=C:/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov.exe
rem Keep in sync with circleci.yml, appveyor-desktop.bat and PKBUILD-coverage,
rem please. The --source-dir needs to be present in order to circumvent
rem     Warning: "../foo" cannot be normalized because of "..", so skip it.
rem that happens with CMake before 3.21 that doesn't yet pass full paths to
rem Ninja: https://github.com/mozilla/grcov/issues/1182
grcov build -t lcov --source-dir %APPVEYOR_BUILD_FOLDER%/build --keep-only "*/src/Magnum*/*" --ignore "*/src/MagnumExternal/*" --ignore "*/Test/*" --ignore "*/build/src/*" -o coverage.info --excl-line LCOV_EXCL_LINE --excl-start LCOV_EXCL_START --excl-stop LCOV_EXCL_STOP  || exit /b
rem Official docs say "not needed for public repos", in reality not using the
rem token is "extremely flakey". What's best is that if the upload fails, the
rem damn thing exits with a success error code, and nobody cares:
rem https://github.com/codecov/codecov-circleci-orb/issues/139
rem https://community.codecov.com/t/commit-sha-does-not-match-circle-build/4266
codecov -f ./coverage.info -t 3b1a7cc0-8bfb-4cde-8d99-dc04572a43c3 -X gcov || exit /b
