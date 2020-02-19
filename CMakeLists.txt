cmake_minimum_required(VERSION 2.8)

project(uavs3e)

aux_source_directory(./test DIR_SRC_TEST)
set_source_files_properties(${DIR_SRC_TEST} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS}  -std=c99 -O3")

add_subdirectory(./src)
include_directories("./inc")

add_executable(uavs3enc ${DIR_SRC_TEST})

target_link_libraries(uavs3enc m)
target_link_libraries(uavs3enc uavs3e)
#target_link_libraries(uavs3enc dl)

