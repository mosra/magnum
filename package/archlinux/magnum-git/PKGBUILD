# Author: mosra <mosra@centrum.cz>
pkgname=magnum-git
pkgver=2020.06.r2855.g8538610fa
pkgrel=1
pkgdesc="C++11/C++14 graphics middleware for games and data visualization (Git version)"
arch=('i686' 'x86_64')
url="https://magnum.graphics"
license=('MIT')
depends=('corrade-git' 'openal' 'glfw' 'sdl2' 'vulkan-icd-loader')
makedepends=('cmake' 'git' 'ninja')
provides=('magnum')
conflicts=('magnum')
source=("git+https://github.com/mosra/magnum.git")
sha1sums=('SKIP')

pkgver() {
    cd "$srcdir/${pkgname%-git}"
    git describe --long | sed -r 's/([^-]*-g)/r\1/;s/-/./g;s/v//g'
}

build() {
    mkdir -p "$srcdir/build"
    cd "$srcdir/build"

    # Otherwise lib*Application.a and other static libraries get broken during
    # a strip, subsequently failing to link.
    #   https://archlinux.org/todo/lto-fat-objects/
    if [[ $CXXFLAGS == *"-flto"* ]]; then
        CXXFLAGS+=" -ffat-lto-objects"
    fi

    cmake "$srcdir/${pkgname%-git}" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DMAGNUM_WITH_AUDIO=ON \
        -DMAGNUM_WITH_VK=ON \
        -DMAGNUM_WITH_GLFWAPPLICATION=ON \
        -DMAGNUM_WITH_SDL2APPLICATION=ON \
        -DMAGNUM_WITH_GLXAPPLICATION=ON \
        -DMAGNUM_WITH_WINDOWLESSGLXAPPLICATION=ON \
        -DMAGNUM_WITH_GLXCONTEXT=ON \
        -DMAGNUM_WITH_OPENGLTESTER=ON \
        -DMAGNUM_WITH_ANYAUDIOIMPORTER=ON \
        -DMAGNUM_WITH_ANYIMAGECONVERTER=ON \
        -DMAGNUM_WITH_ANYIMAGEIMPORTER=ON \
        -DMAGNUM_WITH_ANYSCENECONVERTER=ON \
        -DMAGNUM_WITH_ANYSCENEIMPORTER=ON \
        -DMAGNUM_WITH_MAGNUMFONT=ON \
        -DMAGNUM_WITH_MAGNUMFONTCONVERTER=ON \
        -DMAGNUM_WITH_OBJIMPORTER=ON \
        -DMAGNUM_WITH_TGAIMAGECONVERTER=ON \
        -DMAGNUM_WITH_TGAIMPORTER=ON \
        -DMAGNUM_WITH_WAVAUDIOIMPORTER=ON \
        -DMAGNUM_WITH_DISTANCEFIELDCONVERTER=ON \
        -DMAGNUM_WITH_FONTCONVERTER=ON \
        -DMAGNUM_WITH_IMAGECONVERTER=ON \
        -DMAGNUM_WITH_SCENECONVERTER=ON \
        -DMAGNUM_WITH_SHADERCONVERTER=ON \
        -DMAGNUM_WITH_GL_INFO=ON \
        -DMAGNUM_WITH_VK_INFO=ON \
        -DMAGNUM_WITH_AL_INFO=ON \
        -G Ninja
    ninja
}

package() {
    cd "$srcdir/build"
    DESTDIR="$pkgdir/" ninja install
}
