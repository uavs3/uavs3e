rd /s /q uAVS3lib
mkdir uAVS3lib

copy /y  .\\encoder_ra.cfg        .\\uAVS2lib\\
copy /y  .\\uAVS3lib_x64.lib      .\\uAVS2lib\\
copy /y  .\\utest_x64.exe         .\\uAVS2lib\\
copy /y ..\\utest.c               .\\uAVS2lib\\
copy /y ..\\uAVS3lib\\uAVS3lib.h  .\\uAVS2lib\\
