# Optimization

This doc details some optimizations done for OpenCV to run faster on the ESP32.



Activating optimization
----------------------------

To activate the optimization for the ESP32, the CMake parameter `-DESP32_OPTIMIZATION=ON` must be enabled. Every optimization done will be disabled if this parameter is OFF.



## Floating point support

The ESP32 only have a single precision Floating Point Unit (no double precision). Therefore, OpenCV functions using double types are very slow.



### Matrix  multiplications

In files `core/matmul.dispatch.cp` and `core/matmul.simd.hpp`.

Results by multiplying 100x6 * 6x100 matrices:

1. Initial test : 60 ms
2. Changing `alpha` and `beta` from double to float in `GEMMsingleMult()` function: 12ms
3. Changing `alpha` and `beta` from double to float in `gemmImpl()` function: 4.6ms



Results by multiplying 150x100 * 100x150 matrices: 

1. Initial test: 2757ms
2. Changing double in `GEMMStore()` function: 888ms





## Esp-dsp library

The ESP32 processor has the following hardware: 

* 16/24-bit Instruction Set
* Support for FPU (Floating Point Unit) 
* Support for DSP instructions 
  * 32-bit integer multiplier
  * 32-bit integer divider 
  * 40-bit MAC (Multiply-Accumulate)



The *esp-dsp* library (https://github.com/espressif/esp-dsp) provides functions written in assembly to use this hardware.

This part describes which functions are used where in OpenCV for better performances. 

