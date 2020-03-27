# Esp32 OpenCV tests

This example uses OpenCV library on the Esp32. Only the modules `core`, `imgproc` and `imgcodecs` are compiled. 



The goal of this example is to check which OpenCV features are working and to benchmark their computation time on the target.



The example tests :

* some matrices initialization, then basic image processing functions on these matrices. 
* Reading a 470x400 pixels PNG image from the SPIFFS filesystem of the Esp32. For now, only PNG files are supported. 
* Perform grayscale conversion and then binary threshold on the image, then write it on the spi flash
* Perform a canny edges detection on the image and the write it on the spi flash