aux_source_directory(. DIR_UAVS3E_SRC)
aux_source_directory(./sse DIR_X86_SRC)
aux_source_directory(./avx2 DIR_X86_256_SRC)

list(APPEND DIR_UAVS3E_SRC ${DIR_X86_CORE})

include_directories("../inc")

set_source_files_properties(${DIR_UAVS3E_SRC} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -fPIC -std=c99 -O3")
set_source_files_properties(${DIR_X86_SRC} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -fPIC -std=c99 -O3 -msse4.2")
set_source_files_properties(${DIR_X86_256_SRC} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS} -fPIC -std=c99 -O3 -mavx2")

add_library(uavs3e ${DIR_UAVS3E_SRC} ${DIR_X86_256_SRC} ${DIR_X86_SRC})

target_link_libraries(uavs3e m)
target_link_libraries(uavs3e pthread)

install(TARGETS uavs3e LIBRARY DESTINATION /usr/lib ARCHIVE DESTINATION /usr/lib)
install(FILES ../inc/uavs3e.h DESTINATION /usr/include/uavs3e/)
