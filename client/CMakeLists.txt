
add_executable(client
    client.cc
    ../common/codec.h
    ../common/common.h
    ../common/common.cc
    display_image.h
    display_image.cc
)

target_link_libraries(client
    muduo_net
    muduo_base
    opencv_core
    opencv_highgui
    opencv_imgcodecs
    opencv_imgproc
    opencv_videoio
    opencv_video
    dl
)
