# Install next packages

## for usage
corrade should be installed

```
sudo dnf install openal-soft \
                    SDL2 \
                    freeglut-devel \
                    glfw
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


## on centos

### for glfw and glfw-devel add EPEL repo:
```
sudo dnf install --nogpgcheck https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm
```
