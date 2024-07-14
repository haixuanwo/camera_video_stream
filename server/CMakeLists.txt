
add_executable(server
    server.cc
    ../common/codec.h
    cameraapp.h
    cameraapp.cc
)

target_link_libraries(server
    muduo_net
    muduo_base
)
