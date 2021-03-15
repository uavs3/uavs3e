cmake_minimum_required(VERSION 2.8)

project(uAVS3)

add_subdirectory(uAVS3lib)

add_executable(uAVS3 ./utest.c)
add_executable(uAVS3_gop ./utest_gop.c)

target_link_libraries(uAVS3 uavs3e)
target_link_libraries(uAVS3_gop uavs3e)