#!/bin/bash
set -ev

pacman -Sy --noconfirm mingw-w64-x86_64-perl

# mingw lcov package is empty, so download and use it manually
# https://github.com/appveyor/ci/issues/1628
wget https://github.com/linux-test-project/lcov/archive/v1.13.tar.gz
tar -xzf v1.13.tar.gz

# Keep in sync with PKBUILD-coverage and travis.yml, please
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --directory . --capture --output-file coverage.info > /dev/null
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --extract coverage.info "*/src/Magnum*/*" --output-file coverage.info > /dev/null
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/src/MagnumExternal/*" --output-file coverage.info > /dev/null
# Deprecated, we don't care about coverage anymore
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/src/Magnum/Shapes/*" --output-file coverage.info > /dev/null
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/src/Magnum/DebugTools/ShapeRenderer*" --output-file coverage.info > /dev/null
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/src/Magnum/DebugTools/Implementation/*BoxRenderer*" --output-file coverage.info > /dev/null
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/src/Magnum/DebugTools/Implementation/AbstractShapeRenderer*" --output-file coverage.info > /dev/null
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/src/Magnum/DebugTools/Implementation/CapsuleRenderer*" --output-file coverage.info > /dev/null
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/src/Magnum/DebugTools/Implementation/CylinderRenderer*" --output-file coverage.info > /dev/null
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/src/Magnum/DebugTools/Implementation/LineSegmentRenderer*" --output-file coverage.info > /dev/null
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/src/Magnum/DebugTools/Implementation/PointRenderer*" --output-file coverage.info > /dev/null
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/src/Magnum/DebugTools/Implementation/SphereRenderer*" --output-file coverage.info > /dev/null
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/Test/*" --output-file coverage.info > /dev/null
lcov-1.13/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/build/src/*" --output-file coverage.info > /dev/null
