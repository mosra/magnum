# kate: indent-width 2;

class Magnum < Formula
  desc "C++11/C++14 graphics middleware for games and data visualization"
  homepage "http://magnum.graphics"
  head "git://github.com/mosra/magnum.git"

  depends_on "cmake"
  depends_on "corrade"
  depends_on "sdl2"
  depends_on "glfw"

  def install
    system "mkdir build"
    cd "build" do
      system "cmake", "-DCMAKE_BUILD_TYPE=Release", "-DCMAKE_INSTALL_PREFIX=#{prefix}", "-DMAGNUM_PLUGINS_DIR=#{HOMEBREW_PREFIX}/lib/magnum", "-DWITH_AUDIO=ON", "-DWITH_GLFWAPPLICATION=OFF", "-DWITH_SDL2APPLICATION=ON", "-DWITH_WINDOWLESSCGLAPPLICATION=ON", "-DWITH_CGLCONTEXT=ON", "-DWITH_OPENGLTESTER=ON", "-DWITH_ANYAUDIOIMPORTER=ON", "-DWITH_ANYIMAGECONVERTER=ON", "-DWITH_ANYIMAGEIMPORTER=ON", "-DWITH_ANYSCENEIMPORTER=ON", "-DWITH_MAGNUMFONT=ON", "-DWITH_MAGNUMFONTCONVERTER=ON", "-DWITH_OBJIMPORTER=ON", "-DWITH_TGAIMAGECONVERTER=ON", "-DWITH_TGAIMPORTER=ON", "-DWITH_WAVAUDIOIMPORTER=ON", "-DWITH_DISTANCEFIELDCONVERTER=ON", "-DWITH_FONTCONVERTER=ON", "-DWITH_IMAGECONVERTER=ON", "-DWITH_MAGNUMINFO=ON", "-DWITH_AL_INFO=ON", ".."
      system "cmake", "--build", "."
      system "cmake", "--build", ".", "--target", "install"
    end
  end
end
