#!/bin/sh

# get version slug
version_hash=$(git describe --match "v*" | sed 's/^v//' | sed 's/-/./g')
echo "** repository hash: ${version_hash} ..."

# create dir tree for rpmbuild in user dir
rpmdev-setuptree

# archive reository
(cd ../.. && git archive --format=tar.gz --prefix=magnum-${version_hash}/ -o ~/rpmbuild/SOURCES/magnum-${version_hash}.tar.gz HEAD)
echo "** created archive: ~/rpmbuild/SOURCES/magnum-${version_hash}.tar.gz"
sleep 2

# replace spec version
spec_file="magnum.spec"
sed -i "s/Version:.\+/Version: ${version_hash}/g" ${spec_file}
echo "** building package version: ${version_hash}"

# check dependencies
sudo dnf builddep -y ${spec_file}

# build package
rpmbuild --define "debug_package %{nil}" --clean -ba ${spec_file}
