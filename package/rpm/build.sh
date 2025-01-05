#!/bin/sh

#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020, 2021, 2022, 2023, 2024, 2025
#             Vladimír Vondruš <mosra@centrum.cz>
#   Copyright © 2018, 2020, 2021, 2022, 2023, 2024
#             Igal Alkon <igal.alkon@gmail.com>
#   Copyright © 2021 1b00 <1b00@pm.me>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

set -e

# Get version slug
package_name=magnum
version_hash=$(git describe --match "v*" | sed 's/^v//' | sed 's/-/./g')
echo "** repository hash: ${version_hash} ..."

# Create dir tree for rpmbuild in user dir
rpmdev-setuptree

# Archive repository
(cd ../.. && git archive --format=tar.gz --prefix=${package_name}-${version_hash}/ -o ~/rpmbuild/SOURCES/${package_name}-${version_hash}.tar.gz HEAD)
echo "** created archive: ~/rpmbuild/SOURCES/${package_name}-${version_hash}.tar.gz"
sleep 2

# Replace spec version
sed -i "s/Version:.\+/Version: ${version_hash}/g" ${package_name}.spec
echo "** building package version: ${version_hash}"

# Check dependencies
sudo dnf builddep -y ${package_name}.spec

# Build package
rpmbuild --define "debug_package %{nil}" --clean -bb ${package_name}.spec

echo "** packages for ${package_name}-${version_hash} complete:"
ls ~/rpmbuild/RPMS/$(uname -m)/${package_name}-*${version_hash}*.rpm | cat
