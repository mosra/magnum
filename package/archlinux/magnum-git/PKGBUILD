# Author: mosra <mosra@centrum.cz>
pkgname=magnum-git
pkgver=snapshot.2015.05.r1216.gcd70bc8f
pkgrel=1
pkgdesc="C++11/C++14 and OpenGL 2D/3D graphics engine (Git version)"
arch=('i686' 'x86_64')
url="http://mosra.cz/blog/magnum.php"
license=('MIT')
depends=('corrade-git' 'openal' 'sdl2')
makedepends=('cmake' 'git')
provides=('magnum')
conflicts=('magnum')
source=("git+git://github.com/mosra/magnum.git")
sha1sums=('SKIP')

pkgver() {
    cd "$srcdir/${pkgname%-git}"
    git describe --long | sed -r 's/([^-]*-g)/r\1/;s/-/./g'
}

build() {
    mkdir -p "$srcdir/build"
    cd "$srcdir/build"

    cmake "$srcdir/${pkgname%-git}" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DWITH_AUDIO=ON \
        -DWITH_SDL2APPLICATION=ON \
        -DWITH_GLXAPPLICATION=ON \
        -DWITH_WINDOWLESSGLXAPPLICATION=ON \
        -DWITH_GLXCONTEXT=ON \
        -DWITH_OPENGLTESTER=ON \
        -DWITH_MAGNUMFONT=ON \
        -DWITH_MAGNUMFONTCONVERTER=ON \
        -DWITH_OBJIMPORTER=ON \
        -DWITH_TGAIMAGECONVERTER=ON \
        -DWITH_TGAIMPORTER=ON \
        -DWITH_WAVAUDIOIMPORTER=ON \
        -DWITH_DISTANCEFIELDCONVERTER=ON \
        -DWITH_FONTCONVERTER=ON \
        -DWITH_IMAGECONVERTER=ON \
        -DWITH_MAGNUMINFO=ON \
        -DWITH_AL_INFO=ON
    make
}

package() {
    cd "$srcdir/build"
    make DESTDIR="$pkgdir/" install
}
