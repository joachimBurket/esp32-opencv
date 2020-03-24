set(COMPONENT_SRCS "hello_opencv.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "." "./opencv/")
register_component()

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
