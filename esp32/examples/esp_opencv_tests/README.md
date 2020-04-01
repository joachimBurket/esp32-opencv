# Esp32 OpenCV tests

This example uses OpenCV library on the Esp32. Only the modules `core`, `imgproc` and `imgcodecs` are compiled. 

The goal of this example is to check which OpenCV features are working and to benchmark their computation time on the target. 

The code reads PNG image of different size from the SPIFFS filesystem of the Esp32 (for now, only PNG files are supported). It then performs operations on each image and report the results in an array into the console. 



The results obtained with  can be seen in [benchmark/bm_concat.rst](benchmark/bm_concat.rst). It was tested on the following hardware:

* ESP32D0WDQ6 (revision 1)
* 8 MB of external SPI RAM
* 16 MB of external SPI Flash

