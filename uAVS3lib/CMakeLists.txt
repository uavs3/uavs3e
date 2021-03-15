aux_source_directory(. DIR_UAVS3LIB_SRC)
aux_source_directory(./x86 DIR_X86_SRC)
aux_source_directory(./tools DIR_TOOL_SRC) 

list(APPEND DIR_UAVS3LIB_SRC ${DIR_X86_SRC})
list(APPEND DIR_UAVS3LIB_SRC ${DIR_TOOL_SRC})

IF (CC STREQUAL "icc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -ipo -O3 -std=c99 -march=native")
ELSE()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -O3 -std=c99 -mavx2")
ENDIF()

add_library(uavs3e ${DIR_UAVS3LIB_SRC})

target_link_libraries(uavs3e m)
target_link_libraries(uavs3e pthread)

install(TARGETS uavs3e LIBRARY DESTINATION /usr/lib ARCHIVE DESTINATION /usr/lib)
install(FILES uAVS3lib.h DESTINATION /usr/include)
install(FILES uAVS3lib_gop.h DESTINATION /usr/include)

