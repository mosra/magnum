#!/bin/bash
set -ev

# https://www.msys2.org/news/#2020-06-29-new-packagers
# TODO: drop this once AppVeyor updates the images
curl -O http://repo.msys2.org/msys/x86_64/msys2-keyring-r21.b39fb11-1-any.pkg.tar.xz
curl -O http://repo.msys2.org/msys/x86_64/msys2-keyring-r21.b39fb11-1-any.pkg.tar.xz.sig
# The instructions say {.sig,} at the end, which is apparently wrong
pacman-key --verify msys2-keyring-r21.b39fb11-1-any.pkg.tar.xz.sig
pacman -U --noconfirm msys2-keyring-r21.b39fb11-1-any.pkg.tar.xz

pacman -Sy --noconfirm mingw-w64-x86_64-perl

# mingw lcov package is empty, so download and use it manually
# https://github.com/appveyor/ci/issues/1628
wget https://github.com/linux-test-project/lcov/archive/v1.13.tar.gz
tar -xzf v1.13.tar.gz

# Keep in sync with PKBUILD-coverage and travis.yml, please
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --directory . --capture --output-file coverage.info > /dev/null
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --extract coverage.info "*/src/Magnum*/*" --output-file coverage.info > /dev/null
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/src/MagnumExternal/*" --output-file coverage.info > /dev/null
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/Test/*" --output-file coverage.info > /dev/null
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/build/src/*" --output-file coverage.info > /dev/null
