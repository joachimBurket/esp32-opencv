# Detailed build procedure

This doc details all the commands and modifications done to be able to compile and run OpenCV on the ESP32.



## CMake command:

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

* `-DCMAKE_BUILD_TYPE` can be set to `Debug` to simplify debugging, but uses more RAM 

* `-DBUILD_ZLIB` and `-DBUILD_PNG` are enabled to statically compile the libs instead of using dynamic libs of the system.

* `-DOPENCV_ENABLE_MEMALIGN` is disabled because the ESP32 doesn't support memory alignment for now (https://github.com/espressif/esp-idf/issues/4218)

* The toolchain file `toolchain-esp32.cmake` is taken from esp-idf github:

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
--     C++ Compiler:                ~/.espressif/tools/xtensa-esp32-elf/esp-2019r2-8.2.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-g++  (ver 8.2.0)
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
--   Python (for build):            ~/.espressif/python_env/idf4.2_py2.7_env/bin/python2.7
-- 
--   Install to:                    ~/esp32-opencv/build/install
-- -----------------------------------------------------------------
-- 
-- Configuring done
-- Generating done
-- Build files have been written to: ~/esp32-opencv/build
```



## Make command:

When the cmake is done, the compilation is started with:

```bash
make -j5
```

When the compilation has ended, the libs are in the `build/lib` folder.



## Modified files for OpenCV to compile and run:

To get the cmake configuration, make compilation and an ESP32 project compilation to work, some modifications on OpenCV files had to be done. They are listed below.

#### Configuration with CMake:

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

* *Having error `C++11 compiler must support std::atomic`*

  The esp32 only has hardware support for 32-bit atomic operations but not wider (https://github.com/espressif/esp-idf/issues/3163)

  **FIX:** 

  * Changed the file `cmake/checks/atomic_check.cpp` in OpenCV repo. The `std::atomic<long long>` was changed in `std::atomic<long>`, and also in file `modules/core/include/opencv2/core/utils/allocator_stats.impl.hpp` line 24.

* *CMake Error at `/usr/share/cmake-3.16/Modules/TestBigEndian.cmake:50`. The ESP32 is in little endian.*

  **FIX:** 

  * In the main `CMakeLists.txt`  file, the line 654 is changed : 

    ```cmake
    if(IOS OR ESP32)
    ```



#### Compilation with Make:


After these fixes, the command `make` is run, with some new errors:

* *alloc.cpp:31:16: error: 'posix_memalign' was not declared in this scope*

  **FIX:** Modify `alloc.cpp` 

  * When there is an `#if defined(ANDROID)`, add ` || defined(ESP32)` after, so that `malloc.h` is included and `memalign` is used
  
* *glob.cpp: #error "<dirent.h> not supported"*

  The `glob.cpp` file includes the file `xtensa-esp32-elf/sys-include/dirent.h`, which includes `xtensa-esp32-elf/sys-include/sys/dirent.h`, which causes this error. This is because the filesystem related functions are defined in the ESP-IDF repository (in `components/vfs/`). The `vfs/include/sys/dirent.h` header must therefore be included in `glob.cpp:135` instead of `<dirent.h>`:

  ```c++
  #if defined(ESP32)
  #include <sys/unistd.h>
  #include "esp_dirent.h"
  #include <sys/stat.h>
  const char dir_separators[] = "/";
  #else
  /* ... */
  ```
  
  This `esp_dirent.h` file is the `vfs/include/sys/dirent.h` copied into `modules/core/src/` in OpenCV sources.
  
* *system.cpp:1002:20: error: 'mkstemp' was not declared in this scope*

  **FIX:**

  * Tried to include `<stdlib.h>` where `mkstemp` is, but didn't worked, so just commented lines 1003 to 1007 for now

* *color.simd_helpers.hpp:148:5: error: insn does not satisfy its constraints*

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

    

* *histogram.cpp:1813:1: error: insn does not satisfy its constraints*

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



#### Compilation of an ESP-IDF project:

When the `make` command compiles successfully, the library was tested with an example. This led to some new errors:

* *parallel.cpp:949:58: undefined reference to sysconf*

  This error appeared while trying to use the `canny()` method of the imgproc module

  **Fix:** Modify `modules/core/src/parallel.cpp`

  * Change the line 947 in

    ```c++
    #if !defined(_WIN32) && !defined(__APPLE__) && !defined(ESP32)
    ```
  
    Which will bypass the unsupported call to `sysconf` that get the number of cpu

* *.dram0.bss will not fit in region dram0_0_seg ;  region `dram0_0_seg' overflowed by 13496 bytes*

  This error appeared while trying to add `cvtColor()` function

  **Fix:** 

  * The error says that `dram_0_0_seg` region (containing memory allocated statically at compile time) is overflowed. 

    The Linker Script `esp-idf/components/esp32/ld/esp32.ld` defines the memory layout. It tells the linker where to put the compiler outputs.

    * The region `dram_0_0_seg` has a size of `0x2c200` which corresponds to `~180kB`
    * It is said in this file that this dram segment *should* be `0x50000` (`~330kB`) and that extra DRAM is available in heap at runtime, but that due to static ROM memory usage at 176kB mark, the additional static memory cannot be used. (see also https://esp32.com/viewtopic.php?t=6699).
    * Also enabling `Bluetooth` and `Trace Memory` features in menuconfig will decrease the amount of RAM available

  * Because we can't have more static memory, some features had to be disabled:

    * Disabling the Bluetooth (~56kB) and Trace Memory in the menuconfig
    * Disabling some OpenCV features not mandatory and taking lot of memory. To find variables taking too much space, the `build/<project-name>.map`  file is useful (looking for big variables under the `.dram0.bss` section).
    * `imgproc/color_lab.cpp` has variables (trilinearLUTE, InvGammaTab and LabCbrtTab) taking ~88 kB. They are used for colorspace conversions in and from CIE LAB and XYZ images. These functions were removed. 
    
  * TODO: There are maybe other ways to save space in this segment:
  
    * Allow .bss segment placed in external memory 
      * This option of the menuconfig uses external RAM to store zero-initialized data from the lwIP, net80211, libpp and bluedroid libraries 
    *  Additional data can be moved from the internal BSS segment to external RAM by applying the macro `EXT_RAM_ATTR` to any static declaration (which is not initialized to a non-zero value).
    *  TODO: See what needs to be included and if it's possible to use this esp macro in OpenCV 
