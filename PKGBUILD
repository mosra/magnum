# Author: mosra <mosra@centrum.cz>
pkgname=magnum
pkgver=dev
pkgrel=1
pkgdesc="OpenGL 3 graphics engine"
arch=('i686' 'x86_64')
url="https://github.com/mosra/magnum"
license=('MIT')
depends=('corrade' 'glew')
makedepends=('cmake')
options=(!strip)
provides=('magnum-git')

build() {
    mkdir -p "$startdir/build"
    cd "$startdir/build/"

    # Disable optimization (saves A LOT of compilation time)
    newcxxflags=$(echo $CXXFLAGS | sed s/-O.//g | sed s/-D_FORTIFY_SOURCE=.//g)
    export CXXFLAGS="$newcxxflags"

    if [ "$CXX" = clang++ ] ; then
        newcxxflags=$(echo $CXXFLAGS | sed s/--param=ssp-buffer-size=4//g)
        export CXXFLAGS="$newcxxflags"
    fi

    cmake .. \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DBUILD_TESTS=TRUE
    make
}

check() {
    cd "$startdir/build"
    ctest --output-on-failure -E Benchmark
}

package() {
    cd "$startdir/build"
    make DESTDIR="$pkgdir/" install
}
