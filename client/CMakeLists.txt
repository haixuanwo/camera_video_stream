
add_executable(client
    client.cc
    ../common/codec.h
    cameraapp.h
    cameraapp.cc
)

target_link_libraries(client muduo_net muduo_base)
