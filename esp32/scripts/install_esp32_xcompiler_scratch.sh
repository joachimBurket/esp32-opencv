#!/bin/bash

# This script downloads the esp32 cross-compiler and compile it from scratch with crosstool-NG.
# It doesn't install esp-idf tools.
# Installation instructions from https://docs.espressif.com/projects/esp-idf/en/stable/get-started/linux-setup-scratch.html

set -e

SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"   # dir from where this script is run
CROSSTOOLNG_ESP_VERSION=2019r2

### install some dependencies ###
sudo apt-get install -y git wget libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-click python-cryptography python-future python-pyparsing python-pyelftools ninja-build ccache libffi-dev libssl-dev

sudo apt-get install -y gawk gperf grep gettext python python-dev automake bison flex texinfo help2man libtool libtool-bin make git


### Download and build cross-compilation toolchain ###
if [ -z "$1" ]
  then
    INSTALLDIR = $DEFAULTINSTALLDIR
else 
     INSTALLDIR = $1
fi

mkdir -p $INSTALLDIR && cd $INSTALLDIR

# download crosstool-NG and build it
git clone https://github.com/espressif/crosstool-NG.git
cd crosstool-NG
git checkout esp-$CROSSTOOLNG_ESP_VERSION
git submodule update --init
./bootstrap && ./configure --enable-local && make -j8

# build the toolchain (will be in esp/crosstool-NG/builds/xtensa-esp32-elf)
./ct-ng xtensa-esp32-elf
./ct-ng build
chmod -R u+w builds/xtensa-esp32-elf
