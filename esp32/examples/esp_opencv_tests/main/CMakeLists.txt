idf_component_register(
        SRCS
            main.cpp
            system.cpp
            report.cpp
        INCLUDE_DIRS
            .
            ./opencv
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

# create spiffs partition (named 'storage') from the ../spiffs_image directory
spiffs_create_partition_image(storage ../spiffs_images FLASH_IN_PROJECT)
