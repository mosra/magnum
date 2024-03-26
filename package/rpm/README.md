# Install next packages

## for usage
corrade should be installed

```
sudo dnf install openal-soft \
                    SDL2 \
                    freeglut-devel \
                    glfw
```

```
sudo zypper install openal-soft \
                    SDL2 \
                    freeglut-devel \
                    libglfw3
```
## for build and devel package
corrade-devel should be installed

```
sudo dnf install git gcc-c++ cmake make rpmdevtools \
                    openal-soft-devel \
                    SDL2-devel \
                    freeglut-devel \
                    glfw-devel
```

```
sudo zypper install git gcc-c++ cmake make rpmdevtools rpm-build \
                    openal-soft-devel \
                    libSDL2-devel \
                    freeglut-devel \
                    libglfw-devel
```


## on centos

### for glfw and glfw-devel add EPEL repo:
```
sudo dnf install --nogpgcheck https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm
```

## on openSUSE

### for SDL2 and libSDL2-devel add games repo:
```
sudo zypper addrepo https://download.opensuse.org/repositories/games/openSUSE_Tumbleweed/games.repo
sudo zypper refresh
```