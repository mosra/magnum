class Magnum < Formula
  desc "C++11 graphics middleware for games and data visualization"
  homepage "https://magnum.graphics"
  # git describe origin/master, except the `v` prefix
  version "2020.06-3494-gb85f0c8df"
  # Clone instead of getting an archive to have tags for version.h generation
  url "https://github.com/mosra/magnum.git", revision: "b85f0c8df"
  head "https://github.com/mosra/magnum.git"

  depends_on "cmake" => :build
  depends_on "corrade"
  depends_on "sdl2"
  depends_on "glfw"

  def install
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
        "-DMAGNUM_WITH_AUDIO=ON",
        "-DMAGNUM_WITH_GLFWAPPLICATION=ON",
        "-DMAGNUM_WITH_SDL2APPLICATION=ON",
        "-DMAGNUM_WITH_WINDOWLESSCGLAPPLICATION=ON",
        "-DMAGNUM_WITH_CGLCONTEXT=ON",
        "-DMAGNUM_WITH_OPENGLTESTER=ON",
        "-DMAGNUM_WITH_ANYAUDIOIMPORTER=ON",
        "-DMAGNUM_WITH_ANYIMAGECONVERTER=ON",
        "-DMAGNUM_WITH_ANYIMAGEIMPORTER=ON",
        "-DMAGNUM_WITH_ANYSCENECONVERTER=ON",
        "-DMAGNUM_WITH_ANYSCENEIMPORTER=ON",
        "-DMAGNUM_WITH_ANYSHADERCONVERTER=ON",
        "-DMAGNUM_WITH_MAGNUMFONT=ON",
        "-DMAGNUM_WITH_MAGNUMFONTCONVERTER=ON",
        "-DMAGNUM_WITH_OBJIMPORTER=ON",
        "-DMAGNUM_WITH_TGAIMAGECONVERTER=ON",
        "-DMAGNUM_WITH_TGAIMPORTER=ON",
        "-DMAGNUM_WITH_WAVAUDIOIMPORTER=ON",
        "-DMAGNUM_WITH_DISTANCEFIELDCONVERTER=ON",
        "-DMAGNUM_WITH_FONTCONVERTER=ON",
        "-DMAGNUM_WITH_IMAGECONVERTER=ON",
        "-DMAGNUM_WITH_SCENECONVERTER=ON",
        "-DMAGNUM_WITH_SHADERCONVERTER=ON",
        "-DMAGNUM_WITH_GL_INFO=ON",
        "-DMAGNUM_WITH_AL_INFO=ON",
        ".."
      system "cmake", "--build", "."
      system "cmake", "--build", ".", "--target", "install"
    end
  end
end
