# kate: indent-width 2;

class Magnum < Formula
  desc "C++11/C++14 and OpenGL 2D/3D graphics engine"
  homepage "http://magnum.graphics"
  head "git://github.com/mosra/magnum.git"

  depends_on "cmake"
  depends_on "corrade"
  depends_on "sdl2"

  def install
    system "mkdir build"
    cd "build" do
      system "cmake", "-DCMAKE_BUILD_TYPE=Release", "-DCMAKE_INSTALL_PREFIX=#{prefix}", "-DWITH_AUDIO=ON", "-DWITH_SDL2APPLICATION=ON", "-DWITH_WINDOWLESSCGLAPPLICATION=ON", "-DWITH_CGLCONTEXT=ON", "-DWITH_OPENGLTESTER=ON", "-DWITH_MAGNUMFONT=ON", "-DWITH_MAGNUMFONTCONVERTER=ON", "-DWITH_OBJIMPORTER=ON", "-DWITH_TGAIMAGECONVERTER=ON", "-DWITH_TGAIMPORTER=ON", "-DWITH_WAVAUDIOIMPORTER=ON", "-DWITH_DISTANCEFIELDCONVERTER=ON", "-DWITH_FONTCONVERTER=ON", "-DWITH_IMAGECONVERTER=ON", "-DWITH_MAGNUMINFO=ON", "-DWITH_AL_INFO=ON", ".."
      system "cmake", "--build", "."
      system "cmake", "--build", ".", "--target", "install"
    end
  end
end
