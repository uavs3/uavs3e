cmake_minimum_required(VERSION 3.21)

project(uAVS3 LANGUAGES C ASM)
option(COMPILE_10BIT "Compile 10-bit encoder" OFF)

if(COMPILE_10BIT)
    add_compile_definitions(COMPILE_10BIT=1)
    message("-- compile 10bit")
else()
    add_compile_definitions(COMPILE_10BIT=0)
    message("-- compile 8bit")
endif()

add_subdirectory(uAVS3lib)

add_executable(uAVS3 utest.c)
add_executable(uAVS3_gop utest_gop.c)

target_link_libraries(uAVS3 PRIVATE uavs3e)
target_link_libraries(uAVS3_gop PRIVATE uavs3e)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(uAVS3 PRIVATE _GNU_SOURCE)
    target_compile_definitions(uAVS3_gop PRIVATE _GNU_SOURCE)
endif()

install(TARGETS uAVS3 uAVS3_gop
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)