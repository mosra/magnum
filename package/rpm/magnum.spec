Name:           magnum
Version:        v2020.06
Release:        bf4d59707%{?dist}
Summary:        Lightweight and modular C++11/C++14 graphics middleware for games and data visualization

License:        MIT
URL:            https://magnum.graphics/
Source0:        %{name}-%{version}-%{release}.tar.gz
Requires:       corrade
Requires:       SDL2
Requires:       freeglut
Requires:       glfw
Requires:       openal-soft

BuildRequires: gcc-c++
BuildRequires: cmake >= 3.4.0
BuildRequires: SDL2-devel
BuildRequires: freeglut-devel
BuildRequires: corrade-devel
BuildRequires: glfw-devel
BuildRequires: openal-soft-devel

%description
C++11/C++14 graphics middleware for games and data visualization

%package devel
Summary:        Lightweight and modular C++11/C++14 graphics middleware for games and data visualization
Requires:      corrade-devel
Requires:      %{name} = %{version}
Requires:      SDL2-devel
Requires:      freeglut-devel
Requires:      glfw-devel
Requires:      openal-soft-devel

%description devel
C++11/C++14 graphics middleware for games and data visualization

%global debug_package %{nil}

%prep
%autosetup


%build
mkdir build && cd build
cmake .. \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_AUDIO=ON \
    -DWITH_GLXAPPLICATION=ON \
    -DWITH_GLFWAPPLICATION=ON \
    -DWITH_SDL2APPLICATION=ON \
    -DWITH_WINDOWLESSGLXAPPLICATION=ON \
    -DWITH_GLXCONTEXT=ON \
    -DWITH_OPENGLTESTER=ON \
    -DWITH_ANYAUDIOIMPORTER=ON \
    -DWITH_ANYIMAGECONVERTER=ON \
    -DWITH_ANYIMAGEIMPORTER=ON \
    -DWITH_ANYSCENECONVERTER=ON \
    -DWITH_ANYSCENEIMPORTER=ON \
    -DWITH_ANYSHADERCONVERTER=ON \
    -DWITH_MAGNUMFONT=ON \
    -DWITH_MAGNUMFONTCONVERTER=ON \
    -DWITH_OBJIMPORTER=ON \
    -DWITH_TGAIMAGECONVERTER=ON \
    -DWITH_TGAIMPORTER=ON \
    -DWITH_WAVAUDIOIMPORTER=ON \
    -DWITH_DISTANCEFIELDCONVERTER=ON \
    -DWITH_IMAGECONVERTER=ON \
    -DWITH_SCENECONVERTER=ON \
    -DWITH_SHADERCONVERTER=ON \
    -DWITH_FONTCONVERTER=ON \
    -DWITH_GL_INFO=ON \
    -DWITH_AL_INFO=ON
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
cd build
make DESTDIR=$RPM_BUILD_ROOT install
strip $RPM_BUILD_ROOT/%{_prefix}/lib*/lib*.so.* $RPM_BUILD_ROOT/%{_prefix}/lib*/magnum/*/*.so $RPM_BUILD_ROOT/%{_prefix}/bin/*

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%{_prefix}/bin/*
%{_prefix}/lib*/magnum/*/*
%{_prefix}/lib*/lib*.so.*
%doc COPYING

%files devel
%{_prefix}/include/Magnum/*
%{_prefix}/include/MagnumExternal/*
%{_prefix}/include/MagnumPlugins/*
%{_prefix}/lib*/lib*.a
%{_prefix}/lib*/lib*.so
%{_prefix}/share/cmake*
%doc COPYING


%changelog
* Sun Oct 17 2021 1b00 <1b00@pm.me> - v2020.06-bf4d59707
- Initial release.
