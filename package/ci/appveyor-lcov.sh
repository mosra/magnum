#!/bin/bash
set -ev

# This thing used to use MSYS's pacman to fetch lcov. However:
#
# - the lcov 1.13 package there was empty for some reason, so we had to
#   download the source on our own (https://github.com/appveyor/ci/issues/1628)
# - at some point, MSYS repos got new packagers and we had to update the
#   keyring by hand (https://www.msys2.org/news/#2020-06-29-new-packagers)
# - then, zstd-compressed packages happened, however the used version of pacman
#   didn't understand zstd yet, so for perl, which lcov depends on, we had to
#   manually download an archive that was still compressed with XZ (instead of
#   going the more complicated way of updating pacman itself first, in every CI
#   run)
# - and THEN, since 2021-11-20, the perl XZ packages were removed, causing a
#   404, but a rather weird one, since curl just downloaded the 404 page but
#   didn't treat that as an error
#
# Then I made a step back and laughed at the needless suffering, because
# AppVeyor ships Perl on its own and since we fetch our own lcov anyway, the
# MSYS insanity is not needed for ANYTHING AT ALL, in fact.

wget https://github.com/linux-test-project/lcov/archive/v1.15.tar.gz
tar -xzf v1.15.tar.gz

# Keep in sync with PKBUILD-coverage and circleci.yml, please
lcov-1.15/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --directory . --capture --output-file coverage.info > /dev/null
lcov-1.15/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --extract coverage.info "*/src/Magnum*/*" --output-file coverage.info > /dev/null
lcov-1.15/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/src/MagnumExternal/*" --output-file coverage.info > /dev/null
lcov-1.15/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/Test/*" --output-file coverage.info > /dev/null
lcov-1.15/bin/lcov --gcov-tool /c/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/bin/gcov --remove coverage.info "*/build/src/*" --output-file coverage.info > /dev/null
