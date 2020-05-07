# Build configurations

This doc details some build configurations possible. 



Images codecs support
----------------------------

To add/remove image codecs support, the following modifications are done.

### PNG

- Remove ``-DWITH_PNG=OFF`` and add ``-DBUILD_PNG=ON`` and ``-DBUILD_ZLIB=ON`` from the cmake command

  - The lib ``opencv_imgcodecs.a`` build pass

The library is compiled in the ``3rdparty/`` folder. Copy this folder into the esp32 example project folder.



### JPEG

- Remove ``-DWITH_JPEG=OFF`` and add ``-DBUILD_JPEG=ON`` of the cmake command

  - Problem at compilation time. Doesn't support JPEG for now (TODO).



## Additional OpenCV modules

In addition to the `core`, `imgproc` and `imgcodecs` modules, the following were also tested: 

* `features2d`: The cross-compilation didn't cause issues. The following functionalities were used:

  * AKAZE features detector/descriptor
  * Matching method

* `calib3d`: The cross-compilation didn't cause issues. The following functionalities were used:

  * undistortPoints method
  * findHomography method
  * projectPoints method

  These methods use `double` precision operations. Because the ESP32 only have a single precision Floating Point Unit (FPU), the operations are very slow. (TODO: have modified the methods to use only `float`, but must now add them to this repo).



Parallel support
-----------------------

TODO

