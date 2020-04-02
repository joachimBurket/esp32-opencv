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





