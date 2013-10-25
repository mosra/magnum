# Author: mosra <mosra@centrum.cz>
pkgname=magnum
pkgver=dev
pkgrel=1
pkgdesc="C++11 and OpenGL 2D/3D graphics engine"
arch=('i686' 'x86_64')
url="https://github.com/mosra/magnum"
license=('MIT')
depends=('corrade' 'openal' 'freeglut' 'sdl2')
makedepends=('cmake' 'ninja')
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
        -DWITH_GLUTAPPLICATION=ON \
        -DWITH_GLXAPPLICATION=ON \
        -DWITH_SDL2APPLICATION=ON \
        -DBUILD_TESTS=TRUE \
        -G Ninja
    ninja
}

check() {
    cd "$startdir/build"
    ctest --output-on-failure
}

package() {
    cd "$startdir/build"
    DESTDIR="$pkgdir/" ninja install
}
