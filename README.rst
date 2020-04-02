OpenCV for Esp32
================

This is a clone of OpenCV (from commit 8808aaccffaec43d5d276af493ff408d81d4593c), modified to be cross-compiled on the ESP32. This Readme explains how to cross-compile on the ESP32 and also some details on the steps done. 



Hardware
********

The tests were done on the ESP32D0WDQ6 (revision 1):

- Xtensa dual core 32-bit LX6 uP, up to 600 MIPS
- 448 KB of ROM for booting and core functions
- 520 KB of SRAM for data and instructions cache
- 16 KB SRAM in RTC
- 8 MB of external SPI RAM
- 16 MB of external SPI Flash



Benchmark
*********

Below is a summary of the OpenCV features tested on the ESP32 and the time they took (adding the heap/stack used could also be useful).





Installing esp-idf toolchain
****************************

First thing to do is to install the toolchain for the esp32 (see https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html)
.. code-block:: shell
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

This script can be found in [esp32/scripts/install_esp32_toolchain.sh](esp32/scripts/install_esp32_toolchain.sh).



OpenCV cross-compilation:
*************************

This is the interesting part. OpenCV is statically cross-compiled. There are 3 ways to get it. 

Faster way: 
-----------

The first way is to simply get the pre-built OpenCV library in [esp32/lib](esp32/lib), and copy it into your project (see [this section](#Compiling-esp-idf-project-using-opencv))



Fast way:
---------

The second way is by using the script in [esp32/scripts/build_opencv_for_esp32.sh](esp32/scripts/build_opencv_for_esp32.sh). This script automatically compiles OpenCV from this repository sources, and install the needed files into the desired project. It can tweaked as needed to add and remove some parts. 

The script has 2 arguments. The first is the path to the  `toolchain-esp32.cmake` (default is `$HOME/esp/esp-idf/tools/cmake/toolchain-esp32.cmake`), and the second is the path where the OpenCV library is installed (default is in [esp32/lib](esp32/lib)).



Detailed way:
-------------

The last way explains all the commands and modifications done to be able to compile and run OpenCV on the ESP32. The detailed procedure is in [esp32/doc/detailed_build_procedure.md](esp32/doc/detailed_build_procedure.md).



Get project RAM and Flash usages
***********************************

At compilation time:
--------------------

- The command below can be used to see the different segments sizes of the application :
  .. code-block shell
    $ xtensa-esp32-elf-size -d -A build/<project-name>.elf

- The file `build/<project-name>.map` is also very useful. It indicates the memory mapping of the variables and can be used to find big variables in the application. 



At run time:
------------

.. code-block:: c++
  // Get the amount of stack (in Bytes) that remained unused when the task stack was at its greatest value
  ESP_LOGI(TAG, "task stack watermark: %d Bytes", uxTaskGetStackHighWaterMark(NULL));
  // Get the free heap in Bytes (may not be contiguous)
  ESP_LOGI(TAG, "heap left: %d Bytes", esp_get_free_heap_size());



Adding images codecs support
****************************

Things done to read/writes images in JPEG, PNG, etc..

PNG
"""

- Remove `-DWITH_PNG=OFF` and add `-DBUILD_PNG=ON` and `-DBUILD_ZLIB=ON` of the cmake command
  - => The lib `opencv_imgcodecs.a` build pass

The library is compiled in the `3rdparty/` folder. Copy this folder into the esp32 example project folder.



JPEG
""""

- Remove `-DWITH_JPEG=OFF` and add `-DBUILD_JPEG=ON` of the cmake command
  - => Problem at compilation time. TODO



Adding parallel support
***********************

TODO



Removing OpenCV unnecessary parts 
*********************************

Opencv is quite big, even when compiling only the core, imgproc and imgcodec modules. Because the ESP32 has limited resources, it is a good idea to remove some parts of opencv that are in most cases not used. 



TODO: List the modules functionalities and what is kept or not

Core module:
------------




Imgproc module:
---------------

Colorspaces
"""""""""""

Opencv supports multiple colorspaces (RGB, BGR, RGB565, RGBA, CIELAB, CIEXYZ, Luv, YUV, HSV, HLS, YCrCb, Bayer, Gray). All these colorspaces are not mandatory for an embedded system, so some are removed.

- `color_lab.cpp`: This file contains conversion for CIELAB and CIEXYZ (https://en.wikipedia.org/wiki/CIELAB_color_space). The conversion tables takes a lot of space in the .bss segment (~88kB) , which is already overflowing. Here are the steps done to disable this code:
  
  - Move `color_lab.cpp` to `color_lab.cpp.bak` 
  
  - In `color.hpp` disable :
  
    .. code-block:: c++
      // todo
    
  - In `color.cpp` disable:
  
    ..code-block:: c++
      // todo
  
- todo



