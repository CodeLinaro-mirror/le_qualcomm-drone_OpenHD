#!/bin/bash
#This file is the install instruction for the CHROOT build
#We're using cloudsmith-cli to upload the file in CHROOT

if [ -f /etc/os-release ]; then
    source /etc/os-release
    if [ "$ID" = "debian" ] || [ "$ID" = "ubuntu" ]; then
        build_deb
    elif [ "$ID" = "arch" ]; then
        build_arch
    else
        echo "Unknown distribution."
    fi
else
    echo "The /etc/os-release file does not exist."
fi


build_arch() {
    echo "Building for Arch Linux..."
    sudo pacman -Syu
    sudo pacman -S libusb libpcap libsodium libnl sdl2 libv4l git base-devel autoconf automake libtool python-pip ruby cmake gst-plugins-base boost boost-libs 
}

build_deb() {
sudo apt install -y python3-pip
sudo pip3 install --upgrade cloudsmith-cli
ls -a
./install_build_dep.sh rock5
sudo ./package.sh arm64 ${DISTRO} ${FLAVOR} || exit 1
mkdir -p /opt/out/
cp -v *.dep /opt/out/
echo "copied deb file"
echo "push to cloudsmith"
git describe --exact-match HEAD >/dev/null 2>&1
echo "Pushing the package to OpenHD 2.3 repository"
ls -a
}

API_KEY=$(cat cloudsmith_api_key.txt)
DISTRO=$(cat distro.txt)
FLAVOR=$(cat flavor.txt)
cloudsmith push deb --api-key "$API_KEY" openhd/openhd-2-3-evo/${DISTRO}/${FLAVOR} *.deb || exit 1

