class Magnum < Formula
  desc "C++11/C++14 graphics middleware for games and data visualization"
  homepage "https://magnum.graphics"
  url "https://github.com/mosra/magnum/archive/v2020.06.tar.gz"
  # wget https://github.com/mosra/magnum/archive/v2020.06.tar.gz -O - | sha256sum
  sha256 "98dfe802e56614e4e6bf750d9b693de46a5ed0c6eb479b0268f1a20bf34268bf"
  head "https://github.com/mosra/magnum.git"

  depends_on "cmake" => :build
  depends_on "corrade"
  depends_on "sdl2"
  depends_on "glfw"

  def install
    # 2020.06 has the options unprefixed, current master has them prefixed.
    # Options not present in 2020.06 are prefixed always.
    option_prefix = build.head? ? 'MAGNUM_' : ''

    system "mkdir build"
    cd "build" do
      system "cmake",
        # Without this, the build of magnum-gl-info fails on CMake 3.30.3
        # because of unresolved references to CGL. The same problem affected
        # Bullet (https://github.com/bulletphysics/bullet3/issues/4659) and the
        # fix in https://github.com/Homebrew/homebrew-core/pull/189186 is to
        # add this, which ultimately results in `-DCMAKE_FIND_FRAMEWORK=FIRST`
        # being passed to CMake. No idea what's going on, the regular CI build
        # outside of Homebrew (although with CMake 3.26) doesn't need that.
        *std_cmake_args(find_framework: "FIRST"),
        # Without this, ARM builds will try to look for dependencies in
        # /usr/local/lib and /usr/lib (which are the default locations) instead
        # of /opt/homebrew/lib which is dedicated for ARM binaries. Please
        # complain to Homebrew about this insane non-obvious filesystem layout.
        "-DCMAKE_INSTALL_NAME_DIR:STRING=#{lib}",
        # Without this, it will try to look for plugins somewhere deep in
        # homebrew's Cellar in per-package directories instead of in the
        # location everything's symlinked to, thus finding only magnum's core
        # plugins and not those from the magnum-plugins package. Please
        # complain to Homebrew about this insane filesystem layout.
        "-DMAGNUM_PLUGINS_DIR=#{HOMEBREW_PREFIX}/lib/magnum",
        "-D#{option_prefix}WITH_AUDIO=ON",
        "-D#{option_prefix}WITH_GLFWAPPLICATION=ON",
        "-D#{option_prefix}WITH_SDL2APPLICATION=ON",
        "-D#{option_prefix}WITH_WINDOWLESSCGLAPPLICATION=ON",
        "-D#{option_prefix}WITH_CGLCONTEXT=ON",
        "-D#{option_prefix}WITH_OPENGLTESTER=ON",
        "-D#{option_prefix}WITH_ANYAUDIOIMPORTER=ON",
        "-D#{option_prefix}WITH_ANYIMAGECONVERTER=ON",
        "-D#{option_prefix}WITH_ANYIMAGEIMPORTER=ON",
        "-D#{option_prefix}WITH_ANYSCENECONVERTER=ON",
        "-D#{option_prefix}WITH_ANYSCENEIMPORTER=ON",
        "-DMAGNUM_WITH_ANYSHADERCONVERTER=ON",
        "-D#{option_prefix}WITH_MAGNUMFONT=ON",
        "-D#{option_prefix}WITH_MAGNUMFONTCONVERTER=ON",
        "-D#{option_prefix}WITH_OBJIMPORTER=ON",
        "-D#{option_prefix}WITH_TGAIMAGECONVERTER=ON",
        "-D#{option_prefix}WITH_TGAIMPORTER=ON",
        "-D#{option_prefix}WITH_WAVAUDIOIMPORTER=ON",
        "-D#{option_prefix}WITH_DISTANCEFIELDCONVERTER=ON",
        "-D#{option_prefix}WITH_FONTCONVERTER=ON",
        "-D#{option_prefix}WITH_IMAGECONVERTER=ON",
        "-D#{option_prefix}WITH_SCENECONVERTER=ON",
        "-DMAGNUM_WITH_SHADERCONVERTER=ON",
        "-D#{option_prefix}WITH_GL_INFO=ON",
        "-D#{option_prefix}WITH_AL_INFO=ON",
        ".."
      system "cmake", "--build", "."
      system "cmake", "--build", ".", "--target", "install"
    end
  end
end
