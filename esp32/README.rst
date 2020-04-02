================
OpenCV for Esp32
================


This is a clone of OpenCV (from commit 8808aaccffaec43d5d276af493ff408d81d4593c), modified to be cross-compiled on the ESP32. This Readme explains how to cross-compile on the ESP32 and also some details on the steps done. 



Hardware
========

The tests were done on the ESP32D0WDQ6 (revision 1):

- Xtensa dual core 32-bit LX6 uP, up to 600 MIPS
- 448 KB of ROM for booting and core functions
- 520 KB of SRAM for data and instructions cache
- 16 KB SRAM in RTC
- 8 MB of external SPI RAM
- 16 MB of external SPI Flash



Benchmark
=========

Below is a summary of the OpenCV features tested on the ESP32 and the time they took (adding the heap/stack used could also be useful).



Installing esp-idf toolchain
============================

First thing to do is to install the toolchain for the esp32 (see https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html)
.. code:: shell
  ### install some dependencies ###
  sudo apt-get install -y git wget libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-click python-cryptography python-future python-pyparsing python-pyelftools ninja-build ccache libffi-dev libssl-dev

  sudo apt-get install -y gawk gperf grep gettext python python-dev automake bison flex texinfo help2man libtool libtool-bin make git

  mkdir -p $INSTALLDIR && cd $INSTALLDIR
  git clone --recursive https://github.com/espressif/esp-idf.git

  cd esp-idf/
  export IDF_TOOLS_PATH=$INSTALLDIR
  ./install.sh
  export IDF_PATH=$INSTALLDIR/esp-idf
  . $INSTALLDIR/esp-idf/export.sh

This script can be found in esp32/scripts/install_esp32_toolchain.sh.


OpenCV cross-compilation:
=========================

This is the interesting part. OpenCV is statically cross-compiled. There are 3 ways to get it. 

Faster way: 
-----------

The first way is to simply get the pre-built OpenCV library in esp32/lib, and copy it into your project (see Compiling-esp-idf-project-using-opencv)



Fast way:
---------

The second way is by using the script in esp32/scripts/build_opencv_for_esp32.sh. This script automatically compiles OpenCV from this repository sources, and install the needed files into the desired project. It can tweaked as needed to add and remove some parts. 

