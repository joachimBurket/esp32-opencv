# Detailed build procedure

The last way explains all the commands and modifications done to be able to compile and run OpenCV on the ESP32.



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
-DWITH_PTHREADS_PF=ON 					\
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



## Make command:

When the cmake is done, the compilation is started with:

```bash
make -j5
```

When the compilation has ended, the libs are in the `build/lib` folder.



## Compiling esp-idf project using OpenCV:

When the OpenCV library is cross-compiled, we have in result `*.a` files located in `build/lib` folder. We now want to try to compile an example project using OpenCV on the esp32. A basic example of esp-idf project can be found in [esp32/examples/esp_opencv_basic](esp32/examples/esp_opencv_basic). This project simply creates an OpenCV matrix, fill it with values and prints it on the console.

Esp-idf environment uses cmake and is separated in components. Because OpenCV libs were compiled outside this example project, we use the pre-built library functionality of esp-idf (https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html#using-prebuilt-libraries-with-components).

Here are the things done to add the OpenCV library to the project:

* Create a folder named `opencv/` into the `main/` component's folder
* Copy the generated libraries (`libade.a`, `libopencv_core.a`, `libopencv_imgproc.a` and `libopencv_imgcodecs.a`) into this directory
* Create the folder `opencv2/` into this directory, and copy into it the needed headers files :
  * `cvconfig.h`
  * `opencv_modules.hpp`
  * `opencv.hpp`
  * `core.hpp`
  * `imgproc.hpp`
  * `imgcodecs.hpp`
  * `core/` folder
  * `imgproc/` folder
  * `imgcodecs/` folder

* Link the libraries to the project by modifying the `CMakeList.txt` of the `main` project's component is as below :

  ```cmake
  idf_component_register(
  	SRC main.cpp
  	INCLUDE_DIRS ./opencv
  )
  
  # Be aware that the order of the librairies is important
  add_prebuilt_library(opencv_imgcodecs "opencv/libopencv_imgcodecs.a")
  add_prebuilt_library(libpng "opencv/3rdparty/liblibpng.a")
  add_prebuilt_library(libzlib "opencv/3rdparty/libzlib.a")
  add_prebuilt_library(opencv_imgproc "opencv/libopencv_imgproc.a")
  add_prebuilt_library(opencv_core "opencv/libopencv_core.a")
  add_prebuilt_library(ade "opencv/libade.a")
  
  target_link_libraries(${COMPONENT_LIB} PRIVATE opencv_imgcodecs)
  target_link_libraries(${COMPONENT_LIB} PRIVATE libpng)
  target_link_libraries(${COMPONENT_LIB} PRIVATE libzlib)
  target_link_libraries(${COMPONENT_LIB} PRIVATE opencv_imgproc)
  target_link_libraries(${COMPONENT_LIB} PRIVATE opencv_core)
  target_link_libraries(${COMPONENT_LIB} PRIVATE ade)
  ```

* Finally, include the OpenCV headers needed into your source files: 

  ```c++
  #include "opencv2/core.hpp"
  #include "opencv2/imgproc.hpp"
  #include "opencv2/imgcodecs.hpp"
  ```



## Modified files for OpenCV to compile and run:

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

The checks for system libraries (`libpthread`, `libdl`, `libm`, `librt`) are also activated by changing the line 607 of the `CMakeList.txt` file:

```cmake
if(UNIX OF ESP32)
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




After these fixes, the command `make` is run, with some new errors:

* *alloc.cpp:31:16: error: 'posix_memalign' was not declared in this scope*

  **FIX:** Modify `alloc.cpp` 

  * When there is an `#if defined(ANDROID)`, add ` || defined(ESP32)` after, so that `malloc.h` is included and `memalign` is used
  
* *#error "<dirent.h> not supported"*

  The ESP32 doesn't support directories (which are emulated with the filenames, like `mydir/mysubdir/myfile.c`).

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

<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< HEAD:README.md
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

To add loop parallelization on the 2 cores of the esp32, the following modifications are done:

* Changed cmake command with `-DWITH_PTHREADS_PF=ON` 
* Had to temporary add a pthread_cond usage in the main component for the link to work (otherwise had an *undefined reference to pthread_cond_* functions)
  * The pthread_cond implementation is in the esp-idf pthread component. Haven't found yet how to link it 
* In `parallel.cpp` function `getNumberOfCPUs`: 
  * For now, couldn't make `sysconf` function to work (to get the number of cpus), so returns 2 if ESP32 is defined
* In idf menuconfig: 
  * set pthread default stack size to 8192

These modifications haven't improved the performances on the benchmark for now.



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
========================================================================================================================================================================================================
    The Linker Script `esp-idf/components/esp32/ld/esp32.ld` defines the memory layout. It tells the linker where to put the compiler outputs.
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> master:esp32/doc/detailed_build_procedure.md

    * The region `dram_0_0_seg` has a size of `0x2c200` which corresponds to `~180kB`
    * It is said in this file that this dram segment *should* be `0x50000` (`~330kB`) and that extra DRAM is available in heap at runtime, but that due to static ROM memory usage at 176kB mark, the additional static memory cannot be used. (see also https://esp32.com/viewtopic.php?t=6699).
    * Also enabling `Bluetooth` and `Trace Memory` features in menuconfig will decrease the amount of RAM available

  * Because we can't have more static memory, some features had to be disabled:

    * Disabling the Bluetooth (~56kB) and Trace Memory in the menuconfig
    * Disabling some OpenCV features not mandatory and taking lot of memory. To find variables taking too much space, the `build/<project-name>.map`  file is useful (looking for big variables under the `.dram0.bss` section).
    * `imgproc/color_lab.cpp` has variables (trilinearLUTE, InvGammaTab and LabCbrtTab) taking ~88 kB. They are used for colorspace conversions in and from CIE LAB and XYZ images. These functions were removed. 
