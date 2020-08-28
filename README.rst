================
OpenCV for Esp32
================


This is a clone of OpenCV (from commit 8808aaccffaec43d5d276af493ff408d81d4593c), modified to be cross-compiled on the ESP32. This Readme explains how to cross-compile on the ESP32 and also some details on the steps done. 


Demo
====

A demo has been made using the **TTGO Camera Plus** module (https://github.com/Xinyuan-LilyGO/esp32-camera-screen). The board embedds an ESP32-DOWDQ6 with: 

- Xtensa dual core 32-bit LX6 uP, up to 600 MIPS
- 448 KB of ROM for booting and core functions
- 520 KB of SRAM for data and instructions cache
- 16 KB SRAM in RTC
- 8 MB of external SPI RAM
- 4 MB of external SPI Flash


The demo consists in getting an image from the camera, applying a simple transformation on it (Grayscale, Threshold or Canny edge detection), and then displaying it on the LCD.

.. image:: esp32/examples/ttgo_demo/demo/ttgo_demo.gif

The size taken by the application is the following: 

.. code:: shell


  Total sizes:
  DRAM .data size:   33536 bytes
  DRAM .bss  size:   74424 bytes
  Used static DRAM:  107960 bytes (  72776 available, 59.7% used)
  Used static IRAM:   79695 bytes (  51377 available, 60.8% used)
  Flash code: 1 096 883 bytes
  Flash rodata:  356 144 bytes
  Total image size:~1 640 682 bytes (.bin may be padded larger)



The demo code is located in `esp32/examples/ttgo_demo/`_.

.. _`esp32/examples/ttgo_demo/`: esp32/examples/ttgo_demo/




Benchmark
=========

Below is a summary of the OpenCV features tested on the ESP32 and the time they took (adding the heap/stack used could also be useful).

All measures are in **milliseconds**.

+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| Function name and arguments                    | BUILD_TYPE=Debug                                      | BUILD_TYPE=Release                                    |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
|                                                |     160x120 |     320x240 |     640x480 |    1024x768 |     160x120 |     320x240 |     640x480 |    1024x768 |
+================================================+=============+=============+=============+=============+=============+=============+=============+=============+
|                                                                                                        |                                                       |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| **Threshold**                                                                                          |                                                       |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| binaryThreshold                                |         4.5 |          18 |          69 |         175 |         2.5 |          11 |          42 |         107 |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| triangleThreshold                              |         8.0 |          32 |         124 |         315 |         3.9 |          17 |          66 |         168 |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| OTSUThreshold                                  |          11 |          35 |         127 |         318 |         6.5 |          20 |          69 |         171 |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| toZeroThreshold                                |         4.5 |          18 |          69 |         175 |         2.6 |          11 |          42 |         107 |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
|                                                                                                        |                                                       |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| **Blurring**                                                                                           |                                                       |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| GaussianBlur 3x3 kernel                        |          16 |          54 |         206 |         521 |         5.6 |          20 |          76 |         192 |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| medianBlur 3x3 kernel                          |         180 |         721 |        2883 |        7390 |          22 |          90 |         360 |         926 |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| bilateralFilter diameter=5                     |         132 |         509 |        2014 |        5079 |          51 |         190 |         743 |        1854 |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
|                                                                                                        |                                                       |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| **Morphological tranforms**                                                                            |                                                       |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| erode 5x5 kernel                               |          41 |         151 |         587 |        1494 |         6.2 |          22 |          84 |         214 |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| dilate 5x5 kernel                              |          41 |         151 |         588 |        1495 |         6.2 |          22 |          84 |         214 |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| open 5x5 kernel                                |          82 |         299 |        1164 |        2961 |          11 |          41 |         158 |         400 |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
|                                                                                                        |                                                       |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| **Resize image**                                                                                       |                                                       |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| resize linear interpolation                    |          10 |          40 |         150 |         378 |         3.8 |          16 |          59 |         147 |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| resize cubic interpolation                     |          21 |          75 |         287 |         728 |         6.5 |          27 |         108 |         275 |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
|                                                                                                        |                                                       |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| **Edge detection**                                                                                     |                                                       |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| Sobel                                          |          34 |         116 |         438 |        1129 |          14 |          50 |         187 |         497 |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| Canny                                          |          80 |         256 |         886 |         ERR |          32 |         108 |         375 |         ERR |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
|                                                                                                        |                                                       |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| **Hough tranformations**                                                                               |                                                       |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| HoughLines                                     |         392 |         897 |         ERR |         ERR |         314 |         686 |        2121 |         ERR |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+
| HoughLines probabilistic                       |         699 |        1652 |         ERR |         ERR |         603 |        1352 |        3765 |         ERR |
+------------------------------------------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+-------------+

The ``ERR`` fields means that the test hasn't pass (most of time due to OutOfMemory error).

The benchmark code can be found in `esp32/examples/esp_opencv_tests/`_.

.. _`esp32/examples/esp_opencv_tests/`: esp32/examples/esp_opencv_tests/

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

This script can be found in `esp32/scripts/install_esp_toolchain.sh`_.

.. _`esp32/scripts/install_esp_toolchain.sh`: esp32/scripts/install_esp32_toolchain.sh


OpenCV cross-compilation:
=========================

This is the interesting part. OpenCV is statically cross-compiled. There are 3 ways to get it. 

Faster way: 
-----------

The first way is to simply get the pre-built OpenCV library in `esp32/lib/`_ folder, and copy it into your project (see Compiling-esp-idf-project-using-opencv)

.. _`esp32/lib/`: esp32/lib/


Fast way:
---------

The second way is by using the script in build_opencv_for_esp32.sh_. This script automatically compiles OpenCV from this repository sources, and install the needed files into the desired project. It can be tweaked as needed to add and remove some parts (see `esp32/doc/build_configurations.md`_). 

.. _build_opencv_for_esp32.sh: esp32/scripts/build_opencv_for_esp32.sh

.. _`esp32/doc/build_configurations.md`: esp32/doc/build_configurations.md

The script has 2 arguments. The first is the path to the  ``toolchain-esp32.cmake`` (default is ``$HOME/esp/esp-idf/tools/cmake/toolchain-esp32.cmake``), and the second is the path where the OpenCV library is installed (default is in ``./esp32/lib``).

Detailed way:
-------------

The last way explains all the commands and modifications done to be able to compile and run OpenCV on the ESP32. The detailed procedure is in `esp32/doc/detailed_build_procedure.md`_.

.. _`esp32/doc/detailed_build_procedure.md`: esp32/doc/detailed_build_procedure.md



Compiling esp-idf project using OpenCV:
=======================================

When the OpenCV library is cross-compiled, we have in result ``*.a`` files located in ``build/lib`` folder. We now want to try to compile an example project using OpenCV on the esp32. A basic example of esp-idf project can be found in `esp32/examples/hello_opencv/`_. This project simply creates an OpenCV matrix, fill it with values and prints it on the console. It's only purpose is to test the installation. 

.. _`esp32/examples/hello_opencv/`: esp32/examples/hello_opencv/

Esp-idf environment uses cmake and is separated in components. Because OpenCV libs were compiled outside this example project, we use the pre-built library functionality of esp-idf (https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#using-prebuilt-libraries-with-components).

Here are the things done to add the OpenCV library to the project:

- Copy the ``opencv/`` folder (from `esp32/lib/`_) into your project's ``main/`` component's folder. It contains the generated libraries (``libade.a``, ``libopencv_core.a``, ``libopencv_imgproc.a`` and ``libopencv_imgcodecs.a``) and some needed header files (e.g. ``cvconfig.h``, ``opencv_modules.hpp``, ``core.hpp``, ``core/`` folder, etc..). 

.. _`esp32/lib/`: esp32/lib/

- Link the libraries to the project by modifying the ``CMakeList.txt`` of the ``main`` project's component as below :

  .. code:: cmake

    idf_component_register(
  	  SRC main.cpp
  	  INCLUDE_DIRS ./opencv
    )
  
    add_prebuilt_library(opencv_imgcodecs "opencv/libopencv_imgcodecs.a")
    add_prebuilt_library(libpng "opencv/3rdparty/liblibpng.a")
    add_prebuilt_library(libzlib "opencv/3rdparty/libzlib.a")
    add_prebuilt_library(opencv_imgproc "opencv/libopencv_imgproc.a")
    add_prebuilt_library(opencv_core "opencv/libopencv_core.a")
    add_prebuilt_library(ade "opencv/libade.a")

    set(IMGCODEC_DEP libpng libzlib)
    target_link_libraries(opencv_imgcodecs INTERFACE ${IMGCODEC_DEP})

    set(CORE_DEP libzlib)
    target_link_libraries(opencv_core INTERFACE ${CORE_DEP})

    set(OPENCV_DEP opencv_imgcodecs opencv_imgproc opencv_core)
    target_link_libraries(${COMPONENT_LIB} ${OPENCV_DEP})


- Finally, include the OpenCV headers needed into your source files. The ``EPS`` macro defined in FreeRTOS causes conflicts with the epsilon variable in OpenCV. This is why the macro must be undef before OpenCV is included:

  .. code:: c++
    #undef EPS
    #include "opencv2/core.hpp"
    #include "opencv2/imgproc.hpp"
    #include "opencv2/imgcodecs.hpp"
    #define EPS 192



Get project RAM and Flash usages
===================================

At compilation time:
--------------------

- The command below can be used to see the different segments sizes of the application :

  .. code shell

    $ xtensa-esp32-elf-size -d -A build/<project-name>.elf

- The file ``build/<project-name>.map`` is also very useful. It indicates the memory mapping of the variables and can be used to find big variables in the application. 


- The commands ``idf.py size``, ``idf.py size-files`` and ``idf.py size-components`` are very useful to see the memory segments usage. They show more precise information, and also per file usage. For instance with the `esp32/examples/hello_opencv/`_ project, the size used is : 

  .. _`esp32/examples/hello_opencv/`: esp32/examples/hello_opencv/

  .. code:: shell

    Total sizes:
      DRAM .data size:   21168 bytes
      DRAM .bss  size:   13280 bytes
      Used static DRAM:   34448 bytes ( 146288 available, 19.1% used)
      Used static IRAM:   61849 bytes (  69223 available, 47.2% used)
      Flash code:  843403 bytes
      Flash rodata:  246200 bytes
      Total image size:~1185900 bytes (.bin may be padded larger)

  And for the `esp32/examples/esp_opencv_tests/`_ project, the size used is:

  .. _`esp32/examples/esp_opencv_tests/`: esp32/examples/esp_opencv_tests/

  .. code:: shell

    Total sizes:
      DRAM .data size:   31812 bytes
      DRAM .bss  size:   14096 bytes
      Used static DRAM:   45908 bytes ( 134828 available, 25.4% used)
      Used static IRAM:   63741 bytes (  67331 available, 48.6% used)
      Flash code: 1373491 bytes
      Flash rodata:  347440 bytes
      Total image size:~1830580 bytes (.bin may be padded larger)

At run time:
------------

At startup, the application logs a summary of all heap available, e.g.: 

.. code:: shell

  I (252) heap_init: Initializing. RAM available for dynamic allocation:
  I (259) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
  I (265) heap_init: At 3FFB2EC8 len 0002D138 (180 KiB): DRAM
  I (272) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
  I (278) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
  I (284) heap_init: At 4008944C len 00016BB4 (90 KiB): IRAM

It is also possible to get heap and task stack information with the following functions:

.. code:: c++

  // Get the amount of stack (in Bytes) that remained unused when the task stack was at its greatest value
  ESP_LOGI(TAG, "task stack watermark: %d Bytes", uxTaskGetStackHighWaterMark(NULL));
  // Get the free heap in Bytes (may not be contiguous)
  ESP_LOGI(TAG, "heap left: %d Bytes", esp_get_free_heap_size());


DRAM region overflow
====================

Depending on which part of the OpenCV library is used, some big static variables can be present and the static DRAM can be overflowed. The following errors can appear: 

- dram overflow
  .. code:: shell

    .dram0.bss will not fit in region dram0_0_seg ;  region 'dram0_0_seg' overflowed by N bytes

- SHA256 digest overwrite
  .. code::shell
    
    A fatal error occurred: Contents of segment at SHA256 digest offset 0xb0 are not all zero. Refusing to overwrite.
      
The DRAM is the internal RAM section containing data. From the linker script ``esp-idf/components/esp32/ld/esp32.ld``, the ``dram_0_0_seg`` region has a size of ``0x2c200``, which corresponds to around ``180kB``. Due to some fixed RAM addresses used by the ESP32 ROM, there is a limit on the amount which can be statically allocated at compile time (see https://esp32.com/viewtopic.php?t=6699). To prevent this, there are some solutions:

- If not used, disable Bluetooth and Trace Memory features from the menuconfig. Bluetooth stack uses 64kB and Trace Memory 16kB or 32kB (see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/general-notes.html#dram-data-ram)

- In the menuconfig, the following options can also reduce internal DRAM usage: 

  - In Component Config -> ESP32-specific -> Support for external, SPI-connected RAM -> SPI RAM config, enable : 
  
    - "Try to allocate memories of WiFi and LWIP in SPIRAM firstly. If failed, allocate internal memory"
    
    - "Allow .bss segment placed in external memory"
    
- Search for big static array that could be stored in external RAM

  - In ``build/<project_name.map`` file of your project, look under the section ``.dram0.bss`` for big arrays
  
  - ``idf.py size-files`` command is also useful
  
  When big arrays are found, either apply the macro ``EXT_RAM_ATTR`` on them (only with option .bss segment placed in external memory enabled), or initialize them on the heap at runtime.
    


