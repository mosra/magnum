#!/bin/sh

set -e

# Get version slug
version_hash=$(git describe --match "v*" | sed 's/^v//' | sed 's/-/./g')
echo "** repository hash: ${version_hash} ..."

# Create dir tree for rpmbuild in user dir
rpmdev-setuptree

# Archive repository
(cd ../.. && git archive --format=tar.gz --prefix=magnum-${version_hash}/ -o ~/rpmbuild/SOURCES/magnum-${version_hash}.tar.gz HEAD)
echo "** created archive: ~/rpmbuild/SOURCES/magnum-${version_hash}.tar.gz"
sleep 2

# Replace spec version
sed -i "s/Version:.\+/Version: ${version_hash}/g" magnum.spec
echo "** building package version: ${version_hash}"

# Check dependencies
sudo dnf builddep -y magnum.spec

# Build package
rpmbuild --define "debug_package %{nil}" --clean -ba magnum.spec

echo "** packages for magnum-${version_hash} complete:"
ls ~/rpmbuild/RPMS/$(uname -m)/magnum-${version_hash}*.rpm | cat
