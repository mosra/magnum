# Copyright 1999-2014 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=5

EGIT_REPO_URI="git://github.com/mosra/magnum.git"

inherit cmake-utils git-r3

DESCRIPTION="C++11/C++14 and OpenGL 2D/3D graphics engine"
HOMEPAGE="http://magnum.graphics"

LICENSE="MIT"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""

RDEPEND="
	dev-libs/corrade
	media-libs/openal
	media-libs/libsdl2
"
DEPEND="${RDEPEND}"

src_configure() {
	local mycmakeargs=(
		-DCMAKE_INSTALL_PREFIX="${EPREFIX}/usr"
		-DCMAKE_BUILD_TYPE=Release
		-DWITH_AUDIO=ON
		-DWITH_GLXAPPLICATION=ON
		-DWITH_SDL2APPLICATION=ON
		-DWITH_WINDOWLESSGLXAPPLICATION=ON
		-DWITH_EGLCONTEXT=ON
		-DWITH_GLXCONTEXT=ON
		-DWITH_OPENGLTESTER=ON
		-DWITH_MAGNUMFONT=ON
		-DWITH_MAGNUMFONTCONVERTER=ON
		-DWITH_OBJIMPORTER=ON
		-DWITH_TGAIMAGECONVERTER=ON
		-DWITH_TGAIMPORTER=ON
		-DWITH_WAVAUDIOIMPORTER=ON
		-DWITH_DISTANCEFIELDCONVERTER=ON
		-DWITH_IMAGECONVERTER=ON
		-DWITH_FONTCONVERTER=ON
		-DWITH_MAGNUMINFO=ON
		-DWITH_AL_INFO=ON
	)
	cmake-utils_src_configure
}

# kate: replace-tabs off;
