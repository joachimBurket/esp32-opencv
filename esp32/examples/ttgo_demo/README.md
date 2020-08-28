# TTGO demo

This demo purpose is to see the cross-compiled OpenCV library working on the TTGO module (see https://github.com/Xinyuan-LilyGo/esp32-camera-screen), using an OV2640 camera and an LCD.

The demo gets an image from the camera, then depending on the DisplayMode (changing at regular interval), applies a Grayscale, Binary or Canny filter on it and finally displays it on the LCD.

![ttgo_demo](demo/ttgo_demo.gif)



The performances of the processing are approximately: 

* 6 fps with RGB565 image
* 6 fps with Grayscale image 
* 6 fps with Threshold transformation 
* 3 fps with Canny detection 



The size taken by the application is the following: 

```bash
Total sizes:
  DRAM .data size:   33536 bytes
  DRAM .bss  size:   74424 bytes
  Used static DRAM:  107960 bytes (  72776 available, 59.7% used)
  Used static IRAM:   79695 bytes (  51377 available, 60.8% used)
  Flash code: 1 096 883 bytes
  Flash rodata:  356 144 bytes
  Total image size:~1 640 682 bytes (.bin may be padded larger)
```

