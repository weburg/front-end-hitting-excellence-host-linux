# Front End Hitting Excellence Host on Linux (FEtHEr)

![Logo](https://www.weburg.com/fether/images/fetherlogo.png)

## A host application written to use FEtHEr and shared core

### Special requirements

These steps are needed beyond just running the CMAKE or relying on vcpkg.

#### GTK - to address vcpkg compile errors

```shell
sudo apt install autoconf autoconf-archive automake libtool m4 pkg-config
sudo apt install bison
sudo apt install libx11-dev libxft-dev libxext-dev libgles2-mesa-dev
sudo apt install gperf
sudo apt install libdrm-dev
sudo apt install libxrandr-dev
sudo apt install libxi-dev
sudo apt install libxcursor-dev
sudo apt install libxdamage-dev
sudo apt install libxinerama-dev
sudo apt install libxrender-dev libxfixes-dev libxcomposite-dev libxkbcommon-dev libxkbcommon-x11-dev
sudo apt install meson libegl1-mesa-dev libgl1-mesa-dev libwayland-dev wayland-protocols libfontconfig1-dev libfreetype6-dev
```

#### Icon and local app install for basic integration testing

```shell
cmake --install cmake-build-debug --prefix ~/.local
```