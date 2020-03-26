# OpenCV for Esp32

This is a clone of OpenCV (from commit 8808aaccffaec43d5d276af493ff408d81d4593c), modified to be cross-compiled on the ESP32. This Readme explains how to cross-compile on the ESP32 and also some details on the steps done. 



## Hardware

The tests were done on the ESP32D0WDQ6 (revision 1):

* Xtensa dual core 32-bit LX6 uP, up to 600 MIPS
* 448 KB of ROM for booting and core functions
* 520 KB of SRAM for data and instructions cache
* 16 KB SRAM in RTC
* 8 MB of external SPI RAM
* 16 MB of external SPI Flash



## What works

TODO: Resume what works



## Installing esp-idf toolchain

First thing to do is to install the toolchain for the esp32 (see https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html)

```bash
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
```

This script can be found in [esp32/scripts/install_esp32_toolchain.sh](esp32/scripts/install_esp32_toolchain.sh).



## OpenCV cross-compilation:

This is the interesting part. OpenCV is statically cross-compiled. There are 3 ways to get it. 

### Faster way: 

The first way is to simply get the already compiled library in [esp32/lib](esp32/lib), and copy it into your project (see [this section](#Compiling-esp-idf-project-using-opencv))



### Fast way:

The second way is by using the script in [esp32/scripts/build_opencv_for_esp32.sh](esp32/scripts/build_opencv_for_esp32.sh). This script automatically compiles OpenCV from this repository sources, and install the needed files into the desired project. It can tweaked as needed to add and remove some parts. 

The script has 2 arguments. The first is the path to the  `toolchain-esp32.cmake` (default is `$HOME/esp/esp-idf/tools/cmake/toolchain-esp32.cmake`), and the second is the path where the OpenCV library is installed (default is in [esp32/lib](esp32/lib)).



### Detailed way:

The last way explains all the commands and modifications done to be able to compile and run OpenCV on the ESP32.



#### CMake command:

The following cmake command is launched in the `build/` directory.

```bash
cmake 									\
-DCMAKE_BUILD_TYPE=Release				\
-DESP32=ON								\
-DBUILD_SHARED_LIBS=OFF 				\
-DCV_DISABLE_OPTIMIZATION=OFF 			\
-DWITH_IPP=OFF 							\
-DWITH_TBB=OFF 							\
-DWITH_OPENMP=OFF 						\
-DWITH_PTHREADS_PF=OFF 					\
-DWITH_QUIRC=OFF 						\
-DWITH_1394=OFF 						\
-DWITH_CUDA=OFF 						\
-DWITH_OPENCL=OFF 						\
-DWITH_OPENCLAMDFFT=OFF 				\
-DWITH_OPENCLAMDBLAS=OFF 				\
-DWITH_VA_INTEL=OFF 					\
-DWITH_EIGEN=OFF						\
										\
-DWITH_GSTREAMER=OFF 					\
-DWITH_GTK=OFF 							\
-DWITH_JASPER=OFF 						\
-DWITH_JPEG=OFF 						\
-DWITH_WEBP=OFF 						\
-DBUILD_ZLIB=ON							\
-DBUILD_PNG=ON 							\
-DWITH_TIFF=OFF 						\
-DWITH_V4L=OFF 							\
-DWITH_LAPACK=OFF						\
-DWITH_ITT=OFF 							\
-DWITH_PROTOBUF=OFF 					\
-DWITH_IMGCODEC_HDR=OFF 				\
-DWITH_IMGCODEC_SUNRASTER=OFF 			\
-DWITH_IMGCODEC_PXM=OFF 				\
-DWITH_IMGCODEC_PFM=OFF 				\
										\
-DBUILD_LIST=core,imgproc,imgcodecs		\
-DBUILD_JAVA=OFF						\
-DBUILD_opencv_python=OFF 				\
-DBUILD_opencv_java=OFF 				\
										\
-DBUILD_opencv_apps=OFF					\
-DBUILD_PACKAGE=OFF						\
-DBUILD_PERF_TESTS=OFF					\
-DBUILD_TESTS=OFF						\
-DCV_ENABLE_INTRINSICS=OFF				\
-DCV_TRACE=OFF							\
-DOPENCV_ENABLE_MEMALIGN=OFF			\
										\
-DCMAKE_TOOLCHAIN_FILE=~/esp/esp-idf/tools/cmake/toolchain-esp32.cmake \
..
```



The toolchain file `toolchain-esp32.cmake` is taken from esp-idf github:

```cmake
set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER xtensa-esp32-elf-gcc)
set(CMAKE_CXX_COMPILER xtensa-esp32-elf-g++)
set(CMAKE_ASM_COMPILER xtensa-esp32-elf-gcc)

set(CMAKE_C_FLAGS "-mlongcalls -Wno-frame-address" CACHE STRING "C Compiler Base Flags")
set(CMAKE_CXX_FLAGS "-mlongcalls -Wno-frame-address" CACHE STRING "C++ Compiler Base Flags")

# Can be removed after gcc 5.2.0 support is removed (ref GCC_NOT_5_2_0)
set(CMAKE_EXE_LINKER_FLAGS "-nostdlib" CACHE STRING "Linker Base Flags")
```



When the `cmake` command works, the following summary is given:

```bash
-- General configuration for OpenCV 4.2.0-dev =====================================
--   Version control:               4.2.0-375-g8808aaccff-dirty
-- 
--   Platform:
--     Timestamp:                   2020-03-23T09:54:18Z
--     Host:                        Linux 4.19.10-041910-generic x86_64
--     Target:                      Generic
--     CMake:                       3.16.4
--     CMake generator:             Unix Makefiles
--     CMake build tool:            /usr/bin/make
--     Configuration:               Release
-- 
--   CPU/HW features:
--     Baseline:
--       requested:                 DETECT
-- 
--   C/C++:
--     Built as dynamic libs?:      NO
--     C++ standard:                11
--     C++ Compiler:                /home/joachim/.espressif/tools/xtensa-esp32-elf/esp-2019r2-8.2.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-g++  (ver 8.2.0)
--     C++ flags (Release):         -mlongcalls -Wno-frame-address   -fsigned-char -W -Wall -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wundef -Winit-self -Wpointer-arith -Wshadow -Wsign-promo -Wuninitialized -Winit-self -Wsuggest-override -Wno-delete-non-virtual-dtor -Wno-comment -Wimplicit-fallthrough=3 -Wno-strict-overflow -fdiagnostics-show-option -fomit-frame-pointer -ffunction-sections -fdata-sections  -fvisibility=hidden -fvisibility-inlines-hidden -O3 -DNDEBUG  -DNDEBUG
--     C++ flags (Debug):           -mlongcalls -Wno-frame-address   -fsigned-char -W -Wall -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wundef -Winit-self -Wpointer-arith -Wshadow -Wsign-promo -Wuninitialized -Winit-self -Wsuggest-override -Wno-delete-non-virtual-dtor -Wno-comment -Wimplicit-fallthrough=3 -Wno-strict-overflow -fdiagnostics-show-option -fomit-frame-pointer -ffunction-sections -fdata-sections  -fvisibility=hidden -fvisibility-inlines-hidden -g  -O0 -DDEBUG -D_DEBUG
--     C Compiler:                  /home/joachim/.espressif/tools/xtensa-esp32-elf/esp-2019r2-8.2.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc
--     C flags (Release):           -mlongcalls -Wno-frame-address   -fsigned-char -W -Wall -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wundef -Winit-self -Wpointer-arith -Wshadow -Wuninitialized -Winit-self -Wno-comment -Wimplicit-fallthrough=3 -Wno-strict-overflow -fdiagnostics-show-option -fomit-frame-pointer -ffunction-sections -fdata-sections  -fvisibility=hidden -O3 -DNDEBUG  -DNDEBUG
--     C flags (Debug):             -mlongcalls -Wno-frame-address   -fsigned-char -W -Wall -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wundef -Winit-self -Wpointer-arith -Wshadow -Wuninitialized -Winit-self -Wno-comment -Wimplicit-fallthrough=3 -Wno-strict-overflow -fdiagnostics-show-option -fomit-frame-pointer -ffunction-sections -fdata-sections  -fvisibility=hidden -g  -O0 -DDEBUG -D_DEBUG
--     Linker flags (Release):      -Wl,--gc-sections  
--     Linker flags (Debug):        -Wl,--gc-sections  
--     ccache:                      NO
--     Precompiled headers:         NO
--     Extra dependencies:
--     3rdparty dependencies:       zlib libpng
-- 
--   OpenCV modules:
--     To be built:                 core imgcodecs imgproc
--     Disabled:                    world
--     Disabled by dependency:      calib3d features2d flann gapi highgui java_bindings_generator ml objdetect photo python_bindings_generator python_tests stitching video videoio
--     Unavailable:                 dnn java js python2 python3 ts
--     Applications:                -
--     Documentation:               NO
--     Non-free algorithms:         NO
-- 
--   GUI: 
-- 
--   Media I/O: 
--     ZLib:                        build (ver 1.2.11)
--     PNG:                         build (ver 1.6.37)
--     HDR:                         NO
--     SUNRASTER:                   NO
--     PXM:                         NO
--     PFM:                         NO
-- 
--   Video I/O:
--     FFMPEG:                      NO
--       avcodec:                   NO
--       avformat:                  NO
--       avutil:                    NO
--       swscale:                   NO
--       avresample:                NO
-- 
--   Parallel framework:            none
-- 
--   Other third-party libraries:
--     Custom HAL:                  NO
-- 
--   Python (for build):            /home/joachim/.espressif/python_env/idf4.2_py2.7_env/bin/python2.7
-- 
--   Install to:                    /home/joachim/Documents/HES/02_MSE/22_master_thesis/esp32-opencv/build/install
-- -----------------------------------------------------------------
-- 
-- Configuring done
-- Generating done
-- Build files have been written to: /home/joachim/Documents/HES/02_MSE/22_master_thesis/esp32-opencv/build
```



#### Make command:

When the cmake is done, the compilation is started with:

```bash
make -j5
```

When the compilation has ended, the libs are in the `build/lib` folder.



#### Compiling esp-idf project using OpenCV:

When the OpenCV library is cross-compiled, we have in result `*.a` files located in `build/lib` folder. We now want to try to compile an example project using OpenCV on the esp32. A basic example of esp-idf project can be found in [esp32/examples/esp_opencv_basic](esp32/examples/esp_opencv_basic). This project simply creates an OpenCV matrix, fill it with values and prints it on the console.

Esp-idf environment uses cmake and is separated in components. Because OpenCV lib were compiled outside this example project, we use the pre-built library functionality of esp-idf (https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#using-prebuilt-libraries-with-components).

Here are the things done to add the OpenCV library to the project:

* Create a folder named `opencv/` into the `main/` component's folder
* Copy the generated libraries (`libade.a`, `libopencv_core.a`, `libopencv_imgproc.a` and `libopencv_imgcodecs.a`) into this directory
* Create the folder `opencv2/` into this directory, and copy into it the needed headers :
  * `cvconfig.h`
  * `core.hpp`
  * `imgproc.hpp`
  * `opencv.hpp`
  * `opencv_modules.hpp`
  * `core/` folder
  * `imgproc/` folder



So the `CMakeList.txt` of the `main` project's component is as below :

```cmake
set(COMPONENT_SRCS "hello_opencv.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "." "./opencv/")
set(COMPONENT_NAME "main")
register_component()

# Be aware that the order of the librairies is important
add_prebuilt_library(opencv_imgcodecs "opencv/libopencv_imgcodecs.a")
add_prebuilt_library(opencv_imgproc "opencv/libopencv_imgproc.a")
add_prebuilt_library(opencv_core "opencv/libopencv_core.a")
add_prebuilt_library(ade "opencv/libade.a")

target_link_libraries(${COMPONENT_LIB} PRIVATE opencv_imgcodecs)
target_link_libraries(${COMPONENT_LIB} PRIVATE opencv_imgproc)
target_link_libraries(${COMPONENT_LIB} PRIVATE opencv_core)
target_link_libraries(${COMPONENT_LIB} PRIVATE ade)
```



#### Modified files for OpenCV to compile and run:

To get the cmake configuration and make compilation to work, some modifications on OpenCV files had to be done. They are listed below.



A `cmake` option `ESP32` is added to the `CmakeLists.txt` : 

```cmake
OCV_OPTION(ESP32 "Compilation for esp32 target" OFF)
```

To enable it, add the `-DESP32=ON` in the `cmake` command. This will then be used to enable/disable some parts of the codes of the OpenCV project. 

A preprocessor definition is also added in the `CMakeLists.txt` file, line 494:

```cmake
if(ESP32)
  message(STATUS "Enabled ESP32 target specific build")
  add_definitions(-DESP32)
endif()
```



The `cmake` command is run, leading to the following errors:

* Having error `C++11 compiler must support std::atomic`. The esp32 only has hardware support for 32-bit atomic operations but not wider (https://github.com/espressif/esp-idf/issues/3163)

  **FIX:** 

  * Changed the file `cmake/checks/atomic_check.cpp` in OpenCV repo. The `std::atomic<long long>` was changed in `std::atomic<long>`, and also in file `modules/core/include/opencv2/core/utils/allocator_stats.impl.hpp` line 24.

* CMake Error at `/usr/share/cmake-3.16/Modules/TestBigEndian.cmake:50`. The ESP32 is in little endian.

  **FIX:** 

  * In the main `CMakeLists.txt`  file, the line 654 is changed : 

    ```cmake
    if(IOS OR ESP32)
    ```




After these fixes, the command `make` is run, with some new errors:

* alloc.cpp:31:16: error: 'posix_memalign' was not declared in this scope

  **FIX:** Modify `alloc.cpp` 

  * When there is an `#if defined(ANDROID)`, add ` || defined(ESP32)` after, so that `malloc.h` is included and `memalign` is used
  
* #error "<dirent.h> not supported". The ESP32 doesn't support directories (which are emulated with the filenames, like `mydir/mysubdir/myfile.c`).

  **FIX:** Modify `modules/core/src/glob.cpp`

  * Add the following code after line 136:

    ```c++
    #if defined(ESP32)
    #include <sys/stat.h>
    const char dir_separators[] = "/";
    
    namespace {
        struct dirent {
            const char *d_name;
        };
    
        struct DIR {
            dirent ent;
    
            DIR() { }
            ~DIR()
            {
                if (ent.d_name)
                    delete[] ent.d_name;
            }
        };
    
        DIR* opendir(const char* path)
        {
            DIR* dir = new DIR;
            dir->ent.d_name = 0;
            // TODO implement (point the first file starting with 'path' in its name)
            return dir;
        }
    
        dirent* readdir(DIR* dir)
        {
            // TODO: implement (point to the next file with 'path' in its name)
            return &(dir->ent);
        }
    
        void closedir(DIR* dir)
        {
            delete dir;
        }
    }
    ```
  
    The function are not implemented yet. Must be implemented if files reading/writing in SPIFFS needed.

* system.cpp:1002:20: error: 'mkstemp' was not declared in this scope

  **FIX:**

  * Tried to include `<stdlib.h>` where `mkstemp` is, but didn't worked, so just commented lines 1003 to 1007 for now

* color.simd_helpers.hpp:148:5: error: insn does not satisfy its constraints

  **FIX:**

  * Disable optimization only for this function:

    ```c++
    #pragma GCC push_options
    #pragma GCC optimize ("-O0")
        virtual void operator()(const Range& range) const CV_OVERRIDE
        {
            // functon content ...
        }
    #pragma GCC pop_options
    ```

    

* histogram.cpp:1813:1: error: insn does not satisfy its constraints

  **FIX:** 

  * Disable optimization only for this function:

    ```c++
    #pragma GCC push_options
    #pragma GCC optimize ("-O0")
    void cv::calcBackProject( const Mat* images, int nimages, const int* channels,
                              const SparseMat& hist, OutputArray _backProject,
                              const float** ranges, double scale, bool uniform )
    {
        // functon content ...
    }
    #pragma GCC pop_options
    ```



When the `make` command compiles successfully, the library was tested with an example. This led to some new errors:

* parallel.cpp:949:58: undefined reference to sysconf

  This error appeared while trying to use the `canny()` method of the imgproc module

  **Fix:** Modify `modules/core/src/parallel.cpp`

  * Change the line 947 in

    ```c++
    #if !defined(_WIN32) && !defined(__APPLE__) && !defined(ESP32)
    ```
  
    Which will bypass the unsupported call to `sysconf` that get the number of cpu

* .dram0.bss will not fit in region dram0_0_seg ;  region `dram0_0_seg' overflowed by 13496 bytes

  This error appeared while trying to add `cvtColor()` function

  **Fix:** 

  * The error says that BSS section (uninitialized variables) is too big. 

  * There are Linker Script that can be used to define the placements of the code into the memory but couldn't find how to increase dram_0_0_seg size. Some say that it's not possible for now because section after dram0_0_seg is static. If not, will need to find too big variables and put them on the heap..

    See `esp-idf/components/esp32/ld/esp32.ld` file

  * Disabling the bluetooth in the `menuconfig` saves 56'156 Bytes, but doesn't work though

  * Searching in `build/<project-name>.map` for the `.dram0.bss` section, and looking at the opencv bss variables that are big

    * `imgproc/color_lab.cpp` has variables (trilinearLUTE, InvGammaTab and LabCbrtTab) taking 88kB, which is big
    * It is deactivated (see [this section](#removing-opencv-unnecessary-parts))



## Get project RAM and Flash usages

### At compilation time:

* The command below can be used to see the different segments sizes of the application :

  ```bash
  $ xtensa-esp32-elf-size -d -A build/<project-name>.elf
  ```

* The file `build/<project-name>.map` is also very useful. It indicates the memory mapping of the variables and can be used to find big variables in the application. 



### At run time:

```c++
ESP_LOGI(TAG, "task stack left: %d Bytes", uxTaskGetStackHighWaterMark(NULL));
ESP_LOGI(TAG, "heap left: %d Bytes", esp_get_free_heap_size());
```



## Adding images codecs support

Things done to read/writes images in JPEG, PNG, etc..

#### PNG

* Remove `-DWITH_PNG=OFF` and add `-DBUILD_PNG=ON` and `-DBUILD_ZLIB=ON` of the cmake command
  * => The lib `opencv_imgcodecs.a` build pass

The library is compiled in the `3rdparty/` folder. Copy this folder into the esp32 example project folder.



#### JPEG

* Remove `-DWITH_JPEG=OFF` and add `-DBUILD_JPEG=ON` of the cmake command
  * => Problem at compilation time. TODO



## Adding parallel support

TODO



## Removing OpenCV unnecessary parts 

Opencv is quite big, even when compiling only the core, imgproc and imgcodec modules. Because the ESP32 has limited resources, it is a good idea to remove some parts of opencv that are in most cases not used. 



TODO: List the modules functionalities and what is kept or not

### Core module:





### Imgproc module:

#### Colorspaces

Opencv supports multiple colorspaces (RGB, BGR, RGB565, RGBA, CIELAB, CIEXYZ, Luv, YUV, HSV, HLS, YCrCb, Bayer, Gray). All these colorspaces are not mandatory for an embedded system, so some are removed.

* `color_lab.cpp`: This file contains conversion for CIELAB and CIEXYZ (https://en.wikipedia.org/wiki/CIELAB_color_space). The conversion tables takes a lot of space in the .bss segment (~88kB) , which is already overflowing. Here are the steps done to disable this code:
  
  * Move `color_lab.cpp` to `color_lab.cpp.bak` 
  
  * In `color.hpp` disable :
  
    ```c++
    
    ```
  
  * In `color.cpp` disable:
  
    ```c++
    
    ```
  
* todo



