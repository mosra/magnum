#!/bin/bash
name=magnum

cd ../../
# get latest tag for empty param
if [[ -z ${1} ]];
then 
    version=$(git describe --abbrev=0);
else
    version="${1}"
fi
# try to get hash for input param
hash=$(git rev-parse --short ${version})
if [ "$?" -ne "0" ];
then
    echo "------------------------------------------------------------------"
    echo "Build FAILED"
    echo "USAGE: parameter should be either 'master' or any valid git tag|hash or no parameter for last tag";
    exit 1;
fi
# if hash provided as input param then name version as "hash"
if [ "${version}" == "${hash}" ];
then
    version="hash"
fi
# create the file tree you need to build RPM packages
rpmdev-setuptree
# create release string
release=${hash}$(rpmbuild --eval=%{?dist});
# create package name string
package_name=${name}-${version}-${release}
# archive sources as ${package_name}.tar.gz into rpm source dir
tarfile=$(rpmbuild --eval=%{_sourcedir})/${package_name}.tar
git archive --prefix=${name}-${version}/ -o ${tarfile} ${hash}
gzip -nf ${tarfile}
cd ./package/rpm
# modify Version and Release in rpm spec file
specfile=$(rpmbuild --eval=%{_specdir})/${package_name}.spec
cp ${name}.spec ${specfile}
sed -i "s/Version:.\+/Version:        ${version}/g" ${specfile}
sed -i "s/Release:.\+/Release:        ${hash}%{?dist}/g" ${specfile}
# build rpm into rpm build dir
rpmbuild -bb ${specfile}
# tell user what is the result of efforts
if [ "$?" -ne "0" ];
then
    echo "------------------------------------------------------------------"
    echo "Build of '${package_name}' FAILED"
    echo "Specfile: ${specfile}"
    echo "Get more info above"
    exit 1;
else
    echo "------------------------------------------------------------------"
    echo "Build of '${package_name}' SUCCEED"
    echo "USAGE: pass either 'master' or any valid git tag|hash or no parameter for last tag"
    echo "Specfile: ${specfile}"
    arch=$(rpmbuild --eval=%{_arch})
    rpmdir=$(rpmbuild --eval=%{_rpmdir})
    echo "To install them run next command:"
    echo "  $ sudo rpm -Uhv ${rpmdir}/${arch}/${name}*${version}-${release}.${arch}.rpm"
fi
