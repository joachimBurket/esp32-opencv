# TTGO demo

This demo purpose is to see the cross-compiled OpenCV library working on the TTGO module (see https://github.com/Xinyuan-LilyGo/esp32-camera-screen), using an OV2640 camera and an LCD.

The demo gets an image from the camera, then depending on the DisplayMode (changing at regular interval), applies a Grayscale, Binary or Canny filter on it and finally displays it on the LCD.

![ttgo_demo](demo/ttgo_demo.gif)



The performances of the processing are approximately: 

* 6 fps with RGB565 image
* 6 fps with Grayscale image 
* 6 fps with Threshold transformation 
* 3 fps with Canny detection 