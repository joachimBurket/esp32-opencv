#!/bin/bash

# This script downloads the esp32 toolchain and compile it.

# USAGE: ./install_esp32_toolchain.sh <path-to-install>
# Default installation is in $HOME/esp/

# Installation instructions from https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html

set -e

SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"   # dir from where this script is run
CROSSTOOLNGVERSION=2019r2
DEFAULTINSTALLDIR=$HOME/esp/

### install some dependencies ###
sudo apt-get install -y git wget libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-click python-cryptography python-future python-pyparsing python-pyelftools ninja-build ccache libffi-dev libssl-dev

sudo apt-get install -y gawk gperf grep gettext python python-dev automake bison flex texinfo help2man libtool libtool-bin make git


## get and install esp-idf tools ###
if [ -z "$1" ]
  then
    INSTALLDIR = $DEFAULTINSTALLDIR
else 
     INSTALLDIR = $1
fi

mkdir -p $INSTALLDIR && cd $INSTALLDIR
git clone --recursive https://github.com/espressif/esp-idf.git

cd esp-idf/
export IDF_TOOLS_PATH=$HOME/esp/
./install.sh
export IDF_PATH=$INSTALLDIR/esp-idf
. $INSTALLDIR/esp-idf/export.sh

# after that, the path modifications can be added to the terminal configuration file so that the tools are available after reboot
