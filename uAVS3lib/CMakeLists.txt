aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} DIR_UAVS3LIB_SRC)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/tools DIR_TOOL_SRC)

set(LIBNAME uavs3e)

include(GNUInstallDirs)
include(CheckCSourceRuns)

# check cpu
if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "AMD64" OR
   "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
    set(UAVS3E_TARGET_CPU "x86")
  elseif(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(UAVS3E_TARGET_CPU "x86_64")
  else()
    message(FATAL_ERROR
              " Unexpected pointer size ${CMAKE_SIZEOF_VOID_P} for ${CMAKE_SYSTEM_PROCESSOR}\n")
  endif()
elseif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "i386" OR
       "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86")
  set(UAVS3E_TARGET_CPU "x86")
elseif("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "aarch64" OR
       "${CMAKE_SYSTEM_PROCESSOR}" MATCHES "arm64")
  set(UAVS3E_TARGET_CPU "arm64")
elseif("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "^arm")
  set(UAVS3E_TARGET_CPU "armv7")
else()
  message(WARNING "unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}\n")
  set(UAVS3E_TARGET_CPU "generic")
endif()

if("${UAVS3E_TARGET_CPU}" MATCHES "x86" OR
   "${UAVS3E_TARGET_CPU}" MATCHES "x86_64")
    aux_source_directory(./x86 DIR_X86_SRC)
    list(APPEND DIR_UAVS3LIB_SRC ${DIR_X86_SRC})

    set(CMAKE_REQUIRED_FLAGS "-mavx2")

    check_c_source_runs([[
      #include <immintrin.h>

      __attribute__((noinline))
      static int test_avx2(void)
      {
          volatile __m256i a = _mm256_set1_epi32(1);
          volatile __m256i b = _mm256_set1_epi32(2);
          __m256i c = _mm256_add_epi32(a, b);
          return _mm256_extract_epi32(c, 0);
      }

      int main(void)
      {
          return test_avx2() == 3 ? 0 : 1;
      }
    ]] HAVE_AVX2)
elseif("${UAVS3E_TARGET_CPU}" MATCHES "arm64")
    aux_source_directory(./armv8 DIR_ARM64_SRC)
    set(ARM64_ASM_SRC
      armv8/cost_arm64.S
      armv8/deblock_arm64.S
      armv8/def_arm64.S
      armv8/inter_pred_arm64.S
      armv8/intra_pred_arm64.S
      armv8/pixel_arm64.S
      armv8/sao_kernel_arm64.S
      armv8/trans_dct_arm64.S
      armv8/block_arm64.S
    )
    list(APPEND DIR_UAVS3LIB_SRC ${DIR_ARM64_SRC} ${ARM64_ASM_SRC})
endif()

add_library(${LIBNAME} ${DIR_UAVS3LIB_SRC} ${DIR_TOOL_SRC})

set_target_properties(${LIBNAME} PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  C_STANDARD 99
  C_STANDARD_REQUIRED ON
)

target_compile_definitions(${LIBNAME} PRIVATE _GNU_SOURCE)
find_package(Threads REQUIRED)

if(CMAKE_C_COMPILER_ID MATCHES "Intel|IntelLLVM")
  target_compile_options(${LIBNAME} PRIVATE -O3 -march=native)
else()
  target_compile_options(${LIBNAME} PRIVATE -O3)
endif()

if(HAVE_AVX2)
  target_compile_options(${LIBNAME} PRIVATE -mavx2)
  target_compile_definitions(${LIBNAME} PRIVATE ENABLE_AVX2=1)
else()
  target_compile_definitions(${LIBNAME} PRIVATE ENABLE_AVX2=0)
endif()

# get version
set (CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
execute_process(COMMAND sh ${CONFIG_DIR}/version.sh ${CONFIG_DIR})

function(extract_version_string version_file version_string_out)
  file(STRINGS "${version_file}" uavs3e_version REGEX "VERSION_STR")
  string(REPLACE "#define VERSION_STR " "" uavs3e_version "${uavs3e_version}")
  string(REPLACE "\"" "" uavs3e_version "${uavs3e_version}")
  string(REPLACE " " "" uavs3e_version "${uavs3e_version}")
  set("${version_string_out}" "${uavs3e_version}" PARENT_SCOPE)
endfunction()

extract_version_string("${CONFIG_DIR}/version.h" uavs3e_version)
MESSAGE(STATUS "uavs3e version \t\t: ${uavs3e_version}")
MESSAGE(STATUS "Target CPU\t\t\t: ${UAVS3E_TARGET_CPU}")

# pkg-config
MESSAGE(STATUS "INSTALL_INCLUDEDIR \t\t: ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
MESSAGE(STATUS "INSTALL_LIBDIR \t\t: ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

configure_file("${CONFIG_DIR}/uavs3e.pc.in" "${CONFIG_DIR}/uavs3e.pc" @ONLY)
# set library
if(BUILD_SHARED_LIBS)
  MESSAGE(STATUS " BUILD_SHARED_LIBS \t\t: true ")
else()
  MESSAGE(STATUS " BUILD_SHARED_LIBS \t\t: false ")
endif()

if (NOT MSVC)
    target_link_libraries(${LIBNAME} PRIVATE m)
endif()

target_link_libraries(${LIBNAME} PRIVATE Threads::Threads)

# install
install(TARGETS ${LIBNAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/uAVS3lib.h
  ${CMAKE_CURRENT_SOURCE_DIR}/uAVS3lib_gop.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES
  "${CONFIG_DIR}/uavs3e.pc"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
)
