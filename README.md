# uavs3e
 uavs3e is an opensource avs3 encoder, supports AVS3-P2 baseline profile. <br>
 features:
 1) AVS3-P2 baseline profile.
 2) supports to compile for Windows/Linux systems.
 3) optimized for SSE4/AVX2 chips.
 4) 10bit encoding on all supported platforms (without SIMD).

# license
  Copyright reserved by “Peking University Shenzhen Graduate School”, “Peng Cheng Laboratory”, and “Guangdong Bohua UHD Innovation Corporation” <br><br>
  This program is a free software. You can redistribute it and/or modify it under the terms of the BSD 4-clause license. <br>
  For more details, please view the file "COPYING" in the project.
  
# compile
  The default configuration only support 8bit decoding. <br>
  To support 10bit streams decoding, edit inc/com_api.h : #define BIT_DEPTH 10

## windows
Prerequisites:
  Visual Studio 2017

build:
  1. ./version.bat (to generate version.h)
  2. solution file: build/x86_windows/uavs3e.sln 

## linux
Prerequisites:
  1. gawk (http://www.gnu.org/software/gawk/)
  2. CMake (https://cmake.org) version 2.8 or higher
  
Build:
  1. mkdir build/linux
  2. cd build/linux && cmake ../..
  3. make && make install

  to build shared library, set BUILD_SHARED_LIBS=1 please.

# Run tests
## window/linux

  uavs3e [--config file] [-paramShort ParameterValue] [--paramLong ParameterValue]

     --config file    
             All Parameters are initially taken from the 'file', typically: "encode_RA.cfg".

     -paramShort ParameterValue
     --paramLong ParameterValue
             If -paramShort or --paramLong parameters are present, then the ParameterValue will override the default settings in the configuration file.


# Contributors
This program is originally developed by the team of Prof.Ronggang Wang (rgwang@pkusz.edu.cn) at Peking University Shenzhen Graduate School. <br>

* Main contributors:  
  * Zhenyu Wang (wangzhenyu@pkusz.edu.cn), Peking University Shenzhen Graduate School. 
  * Bingjie Han (hanbj@pkusz.edu.cn), Peking University Shenzhen Graduate School. 
  * Jiang Du, Peking University Shenzhen Graduate School. 
  * Kui Fan, Peking University Shenzhen Graduate School. 
  * Xi Xie, Peking University Shenzhen Graduate School. 
  * Shengyuan Wu, Peking University Shenzhen Graduate School. 
  * Tong Wu, Peking University Shenzhen Graduate School. 
  * Shiyi Liu, Peking University Shenzhen Graduate School. 
  * Jiayu Yang, Peking University Shenzhen Graduate School. 
  * Chunhui Yang, Peking University Shenzhen Graduate School. 
  * Jin Lin, Harbin Institute of Technology, Shenzhen.
  * Guisen Xu, Peking University Shenzhen Graduate School. 
  * Xufeng Li, Peking University Shenzhen Graduate School. 
  * Yangang Cai, Peking University Shenzhen Graduate School. 
  * Hao Lv, Peng Cheng Laboratory. 
  * Ronggang Wang (rgwang@pkusz.edu.cn), Peking University Shenzhen Graduate School
