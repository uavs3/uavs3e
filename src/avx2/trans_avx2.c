/**************************************************************************************
 * Copyright (c) 2018-2020 ["Peking University Shenzhen Graduate School",
 *   "Peng Cheng Laboratory", and "Guangdong Bohua UHD Innovation Corporation"]
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes the software uAVS3d developed by
 *    Peking University Shenzhen Graduate School, Peng Cheng Laboratory
 *    and Guangdong Bohua UHD Innovation Corporation.
 * 4. Neither the name of the organizations (Peking University Shenzhen Graduate School,
 *    Peng Cheng Laboratory and Guangdong Bohua UHD Innovation Corporation) nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * For more information, contact us at rgwang@pkusz.edu.cn.
 **************************************************************************************/

#include "avx2.h"
#include "../sse/sse.h"
#include "com_system.h"

ALIGNED_32(static const s16 tab_dct2_2nd_shuffle_256i[][16]) = {
    // 16bit: 0-7, 3-0 7-4
    { 0x0100, 0x0302, 0x0504, 0x0706, 0x0908, 0x0B0A, 0x0D0C, 0x0F0E, 0x0706, 0x0504, 0x0302, 0x0100, 0x0F0E, 0x0D0C, 0x0B0A, 0x0908 },  // 0
    // 32bit: 3-0, 3-0
    { 0x0D0C, 0x0F0E, 0x0908, 0x0B0A, 0x0504, 0x0706, 0x0100, 0x0302, 0x0D0C, 0x0F0E, 0x0908, 0x0B0A, 0x0504, 0x0706, 0x0100, 0x0302 },  // 1
    // 32bit: 0, 3, 1, 2, 0, 3, 1, 2
    { 0x0100, 0x0302, 0x0D0C, 0x0F0E, 0x0504, 0x0706, 0x0908, 0x0B0A, 0x0100, 0x0302, 0x0D0C, 0x0F0E, 0x0504, 0x0706, 0x0908, 0x0B0A },  // 2
    // 16bit: 7-0, 7-0
    { 0x0F0E, 0x0D0C, 0x0B0A, 0x0908, 0x0706, 0x0504, 0x0302, 0x0100, 0x0F0E, 0x0D0C, 0x0B0A, 0x0908, 0x0706, 0x0504, 0x0302, 0x0100 }
};
ALIGNED_32(static const s16 tab_dct2_1st_shuffle_256i[][16]) = {
    // 16bit: 7-0, 7-0
    { 0x0F0E, 0x0D0C, 0x0B0A, 0x0908, 0x0706, 0x0504, 0x0302, 0x0100, 0x0F0E, 0x0D0C, 0x0B0A, 0x0908, 0x0706, 0x0504, 0x0302, 0x0100 },
    // 16bit: 0, 7, 1, 6, 2, 5, 3, 4, 0, 7, 1, 6, 2, 5, 3, 4
    { 0x0100, 0x0F0E, 0x0302, 0x0D0C, 0x0504, 0x0B0A, 0x0706, 0x0908, 0x0100, 0x0F0E, 0x0302, 0x0D0C, 0x0504, 0x0B0A, 0x0706, 0x0908 },
    // 16bit: 0, 3, 1, 2, 4, 7, 5, 6, 0, 3, 1, 2, 4, 7, 5, 6
    { 0x0100, 0x0706, 0x0302, 0x0504, 0x0908, 0x0F0E, 0x0B0A, 0x0D0C, 0x0100, 0x0706, 0x0302, 0x0504, 0x0908, 0x0F0E, 0x0B0A, 0x0D0C }
};
ALIGNED_32(static const s16 tab_dct2_1st_coeffs_256i[][16]) = {
    // 
    { 44, 38, 25, 9, 44, 38, 25, 9, 44, 38, 25, 9, 44, 38, 25, 9 },
    { 38, -9, -44, -25, 38, -9, -44, -25, 38, -9, -44, -25, 38, -9, -44, -25 },
    { 25, -44, 9, 38, 25, -44, 9, 38, 25, -44, 9, 38, 25, -44, 9, 38 },
    { 9, -25, 38, -44, 9, -25, 38, -44, 9, -25, 38, -44, 9, -25, 38, -44 },
    // 
    { 45, 43, 40, 35, 29, 21, 13, 4, 45, 43, 40, 35, 29, 21, 13, 4 },
    { 43, 29, 4, -21, -40, -45, -35, -13, 43, 29, 4, -21, -40, -45, -35, -13 },
    { 40, 4, -35, -43, -13, 29, 45, 21, 40, 4, -35, -43, -13, 29, 45, 21 },
    { 35, -21, -43, 4, 45, 13, -40, -29, 35, -21, -43, 4, 45, 13, -40, -29 },
    { 29, -40, -13, 45, -4, -43, 21, 35, 29, -40, -13, 45, -4, -43, 21, 35 },
    { 21, -45, 29, 13, -43, 35, 4, -40, 21, -45, 29, 13, -43, 35, 4, -40 },
    { 13, -35, 45, -40, 21, 4, -29, 43, 13, -35, 45, -40, 21, 4, -29, 43 },
    { 4, -13, 21, -29, 35, -40, 43, -45, 4, -13, 21, -29, 35, -40, 43, -45 },
    // 
    { 45, 45, 44, 43, 41, 39, 36, 34       , 30, 27, 23, 19, 15, 11, 7, 2 },
    { 45, 41, 34, 23, 11, -2, -15, -27     , -36, -43, -45, -44, -39, -30, -19, -7 },
    { 44, 34, 15, -7, -27, -41, -45, -39   , -23, -2, 19, 36, 45, 43, 30, 11 },
    { 43, 23, -7, -34, -45, -36, -11, 19   , 41, 44, 27, -2, -30, -45, -39, -15 },
    { 41, 11, -27, -45, -30, 7, 39, 43     , 15, -23, -45, -34, 2, 36, 44, 19 },
    { 39, -2, -41, -36, 7, 43, 34, -11     , -44, -30, 15, 45, 27, -19, -45, -23 },
    { 36, -15, -45, -11, 39, 34, -19, -45  , -7, 41, 30, -23, -44, -2, 43, 27 },
    { 34, -27, -39, 19, 43, -11, -45, 2    , 45, 7, -44, -15, 41, 23, -36, -30 },
    { 30, -36, -23, 41, 15, -44, -7, 45    , -2, -45, 11, 43, -19, -39, 27, 34 },
    { 27, -43, -2, 44, -23, -30, 41, 7     , -45, 19, 34, -39, -11, 45, -15, -36 },
    { 23, -45, 19, 27, -45, 15, 30, -44    , 11, 34, -43, 7, 36, -41, 2, 39 },
    { 19, -44, 36, -2, -34, 45, -23, -15   , 43, -39, 7, 30, -45, 27, 11, -41 },
    { 15, -39, 45, -30, 2, 27, -44, 41     , -19, -11, 36, -45, 34, -7, -23, 43 },
    { 11, -30, 43, -45, 36, -19, -2, 23    , -39, 45, -41, 27, -7, -15, 34, -44 },
    { 7, -19, 30, -39, 44, -45, 43, -36    , 27, -15, 2, 11, -23, 34, -41, 45 },
    { 2, -7, 11, -15, 19, -23, 27, -30     , 34, -36, 39, -41, 43, -44, 45, -45 },
    //
    { 45, 45, 45, 45, 44, 44, 43, 42, 41, 40, 39, 38, 37, 36, 34, 33 },
    { 31, 30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 3, 1 },
    { 45, 44, 42, 39, 36, 31, 26, 20, 14, 8, 1, -6, -12, -18, -24, -30 },
    { -34, -38, -41, -44, -45, -45, -45, -43, -40, -37, -33, -28, -22, -16, -10, -3 },
    { 45, 42, 37, 30, 20, 10, -1, -12, -22, -31, -38, -43, -45, -45, -41, -36 },
    { -28, -18, -8, 3, 14, 24, 33, 39, 44, 45, 44, 40, 34, 26, 16, 6 },
    { 45, 39, 30, 16, 1, -14, -28, -38, -44, -45, -40, -31, -18, -3, 12, 26 },
    { 37, 44, 45, 41, 33, 20, 6, -10, -24, -36, -43, -45, -42, -34, -22, -8 },
    { 44, 36, 20, 1, -18, -34, -44, -45, -37, -22, -3, 16, 33, 43, 45, 38 },
    { 24, 6, -14, -31, -42, -45, -39, -26, -8, 12, 30, 41, 45, 40, 28, 10 },
    { 44, 31, 10, -14, -34, -45, -42, -28, -6, 18, 37, 45, 40, 24, 1, -22 },
    { -39, -45, -38, -20, 3, 26, 41, 45, 36, 16, -8, -30, -43, -44, -33, -12 },
    { 43, 26, -1, -28, -44, -42, -24, 3, 30, 44, 41, 22, -6, -31, -45, -40 },
    { -20, 8, 33, 45, 39, 18, -10, -34, -45, -38, -16, 12, 36, 45, 37, 14 },
    { 42, 20, -12, -38, -45, -28, 3, 33, 45, 34, 6, -26, -44, -39, -14, 18 },
    { 41, 43, 22, -10, -37, -45, -30, 1, 31, 45, 36, 8, -24, -44, -40, -16 },
    { 41, 14, -22, -44, -37, -6, 30, 45, 31, -3, -36, -45, -24, 12, 40, 42 },
    { 16, -20, -44, -38, -8, 28, 45, 33, -1, -34, -45, -26, 10, 39, 43, 18 },
    { 40, 8, -31, -45, -22, 18, 44, 34, -3, -38, -42, -12, 28, 45, 26, -14 },
    { -43, -37, -1, 36, 44, 16, -24, -45, -30, 10, 41, 39, 6, -33, -45, -20 },
    { 39, 1, -38, -40, -3, 37, 41, 6, -36, -42, -8, 34, 43, 10, -33, -44 },
    { -12, 31, 44, 14, -30, -45, -16, 28, 45, 18, -26, -45, -20, 24, 45, 22 },
    { 38, -6, -43, -31, 16, 45, 22, -26, -45, -12, 34, 41, 1, -40, -36, 10 },
    { 44, 28, -20, -45, -18, 30, 44, 8, -37, -39, 3, 42, 33, -14, -45, -24 },
    { 37, -12, -45, -18, 33, 40, -6, -44, -24, 28, 43, 1, -42, -30, 22, 45 },
    { 8, -39, -34, 16, 45, 14, -36, -38, 10, 45, 20, -31, -41, 3, 44, 26 },
    { 36, -18, -45, -3, 43, 24, -31, -39, 12, 45, 10, -40, -30, 26, 42, -6 },
    { -45, -16, 37, 34, -20, -44, -1, 44, 22, -33, -38, 14, 45, 8, -41, -28 },
    { 34, -24, -41, 12, 45, 1, -45, -14, 40, 26, -33, -36, 22, 42, -10, -45 },
    { -3, 44, 16, -39, -28, 31, 37, -20, -43, 8, 45, 6, -44, -18, 38, 30 },
    { 33, -30, -36, 26, 38, -22, -40, 18, 42, -14, -44, 10, 45, -6, -45, 1 },
    { 45, 3, -45, -8, 44, 12, -43, -16, 41, 20, -39, -24, 37, 28, -34, -31 }
};
ALIGNED_32(static const s32 tab_dct2_pb16_coeffs_256i[][8]) = {
    // EO:
    { 44, 38, 25, 9, 44, 38, 25, 9 },
    { 38, -9, -44, -25, 38, -9, -44, -25 },
    { 25, -44, 9, 38, 25, -44, 9, 38 },
    { 9, -25, 38, -44, 9, -25, 38, -44 },
    // O:
    { 45, 43, 40, 35, 45, 43, 40, 35 },
    { 29, 21, 13, 4, 29, 21, 13, 4 },
    { 43, 29, 4, -21, 43, 29, 4, -21 }, 
    { -40, -45, -35, -13, -40, -45, -35, -13 },
    { 40, 4, -35, -43, 40, 4, -35, -43 },
    { -13, 29, 45, 21, -13, 29, 45, 21 },
    { 35, -21, -43, 4, 35, -21, -43, 4 },
    { 45, 13, -40, -29, 45, 13, -40, -29 },
    { 29, -40, -13, 45, 29, -40, -13, 45 },
    { -4, -43, 21, 35, -4, -43, 21, 35 },
    { 21, -45, 29, 13, 21, -45, 29, 13 },
    { -43, 35, 4, -40, -43, 35, 4, -40 },
    { 13, -35, 45, -40, 13, -35, 45, -40 },
    { 21, 4, -29, 43, 21, 4, -29, 43 },
    { 4, -13, 21, -29, 4, -13, 21, -29 }, 
    { 35, -40, 43, -45, 35, -40, 43, -45 }
};
ALIGNED_32(static const s32 tab_dct2_pb32_coeffs_256i[][8]) = {
    // EEO:
    { 44, 38, 25, 9, 44, 38, 25, 9 },
    { 38, -9, -44, -25, 38, -9, -44, -25 },
    { 25, -44, 9, 38, 25, -44, 9, 38 },
    { 9, -25, 38, -44, 9, -25, 38, -44 },
    // EO:
    { 45, 43, 40, 35, 45, 43, 40, 35 },
    { 29, 21, 13, 4, 29, 21, 13, 4 },
    { 43, 29, 4, -21, 43, 29, 4, -21 },
    { -40, -45, -35, -13, -40, -45, -35, -13 },
    { 40, 4, -35, -43, 40, 4, -35, -43 },
    { -13, 29, 45, 21, -13, 29, 45, 21 },
    { 35, -21, -43, 4, 35, -21, -43, 4 },
    { 45, 13, -40, -29, 45, 13, -40, -29 },
    { 29, -40, -13, 45, 29, -40, -13, 45 },
    { -4, -43, 21, 35, -4, -43, 21, 35 },
    { 21, -45, 29, 13, 21, -45, 29, 13 },
    { -43, 35, 4, -40, -43, 35, 4, -40 },
    { 13, -35, 45, -40, 13, -35, 45, -40 },
    { 21, 4, -29, 43, 21, 4, -29, 43 },
    { 4, -13, 21, -29, 4, -13, 21, -29 },
    { 35, -40, 43, -45, 35, -40, 43, -45 },
    // O:
    { 45, 45, 44, 43, 41, 39, 36, 34 },
    { 30, 27, 23, 19, 15, 11, 7, 2 },
    { 45, 41, 34, 23, 11, -2, -15, -27 },
    { -36, -43, -45, -44, -39, -30, -19, -7 },
    { 44, 34, 15, -7, -27, -41, -45, -39 },
    { -23, -2, 19, 36, 45, 43, 30, 11 },
    { 43, 23, -7, -34, -45, -36, -11, 19 },
    { 41, 44, 27, -2, -30, -45, -39, -15 },
    { 41, 11, -27, -45, -30, 7, 39, 43 },
    { 15, -23, -45, -34, 2, 36, 44, 19 },
    { 39, -2, -41, -36, 7, 43, 34, -11 },
    { -44, -30, 15, 45, 27, -19, -45, -23 },
    { 36, -15, -45, -11, 39, 34, -19, -45 },
    { -7, 41, 30, -23, -44, -2, 43, 27 },
    { 34, -27, -39, 19, 43, -11, -45, 2 },
    { 45, 7, -44, -15, 41, 23, -36, -30 },
    { 30, -36, -23, 41, 15, -44, -7, 45 },
    { -2, -45, 11, 43, -19, -39, 27, 34 },
    { 27, -43, -2, 44, -23, -30, 41, 7 },
    { -45, 19, 34, -39, -11, 45, -15, -36 },
    { 23, -45, 19, 27, -45, 15, 30, -44 },
    { 11, 34, -43, 7, 36, -41, 2, 39 },
    { 19, -44, 36, -2, -34, 45, -23, -15 },
    { 43, -39, 7, 30, -45, 27, 11, -41 },
    { 15, -39, 45, -30, 2, 27, -44, 41 },
    { -19, -11, 36, -45, 34, -7, -23, 43 },
    { 11, -30, 43, -45, 36, -19, -2, 23 },
    { -39, 45, -41, 27, -7, -15, 34, -44 },
    { 7, -19, 30, -39, 44, -45, 43, -36 },
    { 27, -15, 2, 11, -23, 34, -41, 45 },
    { 2, -7, 11, -15, 19, -23, 27, -30 },
    { 34, -36, 39, -41, 43, -44, 45, -45 },
};
ALIGNED_32(static const s32 tab_dct2_pb64_coeffs_256i[][8]) = {
    // EEEO:
    { 44, 38, 25, 9, 44, 38, 25, 9 },
    { 38, -9, -44, -25, 38, -9, -44, -25 },
    // EEO:
    { 45, 43, 40, 35, 45, 43, 40, 35 },
    { 29, 21, 13, 4, 29, 21, 13, 4 },
    { 43, 29, 4, -21, 43, 29, 4, -21 },
    { -40, -45, -35, -13, -40, -45, -35, -13 },
    { 40, 4, -35, -43, 40, 4, -35, -43 },
    { -13, 29, 45, 21, -13, 29, 45, 21 },
    { 35, -21, -43, 4, 35, -21, -43, 4 },
    { 45, 13, -40, -29, 45, 13, -40, -29 },
    // EO:
    { 45, 45, 44, 43, 45, 45, 44, 43 },
    { 41, 39, 36, 34, 41, 39, 36, 34 },
    { 30, 27, 23, 19, 30, 27, 23, 19 },
    { 15, 11, 7, 2, 15, 11, 7, 2 },
    { 45, 41, 34, 23, 45, 41, 34, 23 },
    { 11, -2, -15, -27, 11, -2, -15, -27 },
    { -36, -43, -45, -44, -36, -43, -45, -44 },
    { -39, -30, -19, -7, -39, -30, -19, -7 },
    { 44, 34, 15, -7, 44, 34, 15, -7 },
    { -27, -41, -45, -39, -27, -41, -45, -39 },
    { -23, -2, 19, 36, -23, -2, 19, 36 },
    { 45, 43, 30, 11, 45, 43, 30, 11 },
    { 43, 23, -7, -34, 43, 23, -7, -34 },
    { -45, -36, -11, 19, -45, -36, -11, 19 },
    { 41, 44, 27, -2, 41, 44, 27, -2 },
    { -30, -45, -39, -15, -30, -45, -39, -15 },
    { 41, 11, -27, -45, 41, 11, -27, -45 },
    { -30, 7, 39, 43, -30, 7, 39, 43 },
    { 15, -23, -45, -34, 15, -23, -45, -34 },
    { 2, 36, 44, 19, 2, 36, 44, 19 },
    { 39, -2, -41, -36, 39, -2, -41, -36 },
    { 7, 43, 34, -11, 7, 43, 34, -11 },
    { -44, -30, 15, 45, -44, -30, 15, 45 },
    { 27, -19, -45, -23, 27, -19, -45, -23 },
    { 36, -15, -45, -11, 36, -15, -45, -11 },
    { 39, 34, -19, -45, 39, 34, -19, -45 },
    { -7, 41, 30, -23, -7, 41, 30, -23 },
    { -44, -2, 43, 27, -44, -2, 43, 27 },
    { 34, -27, -39, 19, 34, -27, -39, 19 },
    { 43, -11, -45, 2, 43, -11, -45, 2 },
    { 45, 7, -44, -15, 45, 7, -44, -15 },
    { 41, 23, -36, -30, 41, 23, -36, -30 },
    // O:
    { 45, 45, 45, 45, 44, 44, 43, 42 },
    { 41, 40, 39, 38, 37, 36, 34, 33 },
    { 31, 30, 28, 26, 24, 22, 20, 18 },
    { 16, 14, 12, 10, 8, 6, 3, 1 },
    { 45, 44, 42, 39, 36, 31, 26, 20 },
    { 14, 8, 1, -6, -12, -18, -24, -30 },
    { -34, -38, -41, -44, -45, -45, -45, -43 },
    { -40, -37, -33, -28, -22, -16, -10, -3 },
    { 45, 42, 37, 30, 20, 10, -1, -12 },
    { -22, -31, -38, -43, -45, -45, -41, -36 },
    { -28, -18, -8, 3, 14, 24, 33, 39 },
    { 44, 45, 44, 40, 34, 26, 16, 6 },
    { 45, 39, 30, 16, 1, -14, -28, -38 },
    { -44, -45, -40, -31, -18, -3, 12, 26 },
    { 37, 44, 45, 41, 33, 20, 6, -10 },
    { -24, -36, -43, -45, -42, -34, -22, -8 },
    { 44, 36, 20, 1, -18, -34, -44, -45 },
    { -37, -22, -3, 16, 33, 43, 45, 38 },
    { 24, 6, -14, -31, -42, -45, -39, -26 },
    { -8, 12, 30, 41, 45, 40, 28, 10 },
    { 44, 31, 10, -14, -34, -45, -42, -28 },
    { -6, 18, 37, 45, 40, 24, 1, -22 },
    { -39, -45, -38, -20, 3, 26, 41, 45 },
    { 36, 16, -8, -30, -43, -44, -33, -12 },
    { 43, 26, -1, -28, -44, -42, -24, 3 },
    { 30, 44, 41, 22, -6, -31, -45, -40 },
    { -20, 8, 33, 45, 39, 18, -10, -34 },
    { -45, -38, -16, 12, 36, 45, 37, 14 },
    { 42, 20, -12, -38, -45, -28, 3, 33 },
    { 45, 34, 6, -26, -44, -39, -14, 18 },
    { 41, 43, 22, -10, -37, -45, -30, 1 },
    { 31, 45, 36, 8, -24, -44, -40, -16 },
    { 41, 14, -22, -44, -37, -6, 30, 45 },
    { 31, -3, -36, -45, -24, 12, 40, 42 },
    { 16, -20, -44, -38, -8, 28, 45, 33 },
    { -1, -34, -45, -26, 10, 39, 43, 18 },
    { 40, 8, -31, -45, -22, 18, 44, 34 },
    { -3, -38, -42, -12, 28, 45, 26, -14 },
    { -43, -37, -1, 36, 44, 16, -24, -45 },
    { -30, 10, 41, 39, 6, -33, -45, -20 },
    { 39, 1, -38, -40, -3, 37, 41, 6 },
    { -36, -42, -8, 34, 43, 10, -33, -44 },
    { -12, 31, 44, 14, -30, -45, -16, 28 },
    { 45, 18, -26, -45, -20, 24, 45, 22 },
    { 38, -6, -43, -31, 16, 45, 22, -26 },
    { -45, -12, 34, 41, 1, -40, -36, 10 },
    { 44, 28, -20, -45, -18, 30, 44, 8 },
    { -37, -39, 3, 42, 33, -14, -45, -24 },
    { 37, -12, -45, -18, 33, 40, -6, -44 },
    { -24, 28, 43, 1, -42, -30, 22, 45 },
    { 8, -39, -34, 16, 45, 14, -36, -38 },
    { 10, 45, 20, -31, -41, 3, 44, 26 },
    { 36, -18, -45, -3, 43, 24, -31, -39 },
    { 12, 45, 10, -40, -30, 26, 42, -6 },
    { -45, -16, 37, 34, -20, -44, -1, 44 },
    { 22, -33, -38, 14, 45, 8, -41, -28 },
    { 34, -24, -41, 12, 45, 1, -45, -14 },
    { 40, 26, -33, -36, 22, 42, -10, -45 },
    { -3, 44, 16, -39, -28, 31, 37, -20 },
    { -43, 8, 45, 6, -44, -18, 38, 30 },
    { 33, -30, -36, 26, 38, -22, -40, 18 },
    { 42, -14, -44, 10, 45, -6, -45, 1 },
    { 45, 3, -45, -8, 44, 12, -43, -16 },
    { 41, 20, -39, -24, 37, 28, -34, -31 }
};


static void tx_dct2_pb4_1st_avx2(s16 *src, s16 *dst, int line, int limit_line, int shift)
{
#if (BIT_DEPTH > 8)
    __m256i add = _mm256_set1_epi32(1 << (shift - 1));
#endif
    if (line > 4)
    {
        int j;
        __m256i s0, s1;
        __m256i tab0;
        __m256i e0, o0;
        __m256i v0, v1, v2, v3;
        __m256i d0, d1;
        const __m256i coeff_p32_p32 = _mm256_set1_epi32(0x00200020);    // E
        const __m256i coeff_p32_n32 = _mm256_set1_epi32(0xFFE00020);
        const __m256i coeff_p42_p17 = _mm256_set1_epi32(0x0011002A);    // O
        const __m256i coeff_p17_n42 = _mm256_set1_epi32(0xFFD60011);

        tab0 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[2]);  // 16bit: 0, 3, 1, 2, 4, 7, 5, 6, 0, 3, 1, 2, 4, 7, 5, 6

        for (j = 0; j < limit_line; j += 8)
        {
            s0 = _mm256_loadu2_m128i((const __m128i*)&src[4 * 4], (const __m128i*)&src[0]);  // src[0][0-4], src[1][0-4], src[4][0-4], src[5][0-4]
            s1 = _mm256_loadu2_m128i((const __m128i*)&src[6 * 4], (const __m128i*)&src[8]);  // src[2][0-4], src[3][0-4], src[6][0-4], src[7][0-4]

            s0 = _mm256_shuffle_epi8(s0, tab0);                         // src[0][0, 3, 1, 2], src[1][0, 3, 1, 2], src[4][0, 3, 1, 2], src[5][0, 3, 1, 2]
            s1 = _mm256_shuffle_epi8(s1, tab0);

            src += 4 * 8;

            e0 = _mm256_hadd_epi16(s0, s1);                             // e[0][0, 1], e[1][0, 1], e[2][0, 1], e[3][0, 1], e[4][0, 1], e[5][0, 1], e[6][0, 1], e[7][0, 1]
            o0 = _mm256_hsub_epi16(s0, s1);

            v0 = _mm256_madd_epi16(e0, coeff_p32_p32);                  // dst[0][0-7]                   
            v1 = _mm256_madd_epi16(e0, coeff_p32_n32);                  // dst[2][0-7]
            v2 = _mm256_madd_epi16(o0, coeff_p42_p17);                  // dst[1][0-7]
            v3 = _mm256_madd_epi16(o0, coeff_p17_n42);                  // dst[3][0-7]

#if (BIT_DEPTH > 8)
            if (shift > 0) {
                v0 = _mm256_add_epi32(v0, add);
                v1 = _mm256_add_epi32(v1, add);
                v2 = _mm256_add_epi32(v2, add);
                v3 = _mm256_add_epi32(v3, add);

                v0 = _mm256_srai_epi32(v0, shift);
                v1 = _mm256_srai_epi32(v1, shift);
                v2 = _mm256_srai_epi32(v2, shift);
                v3 = _mm256_srai_epi32(v3, shift);
            }
#endif
            d0 = _mm256_packs_epi32(v0, v1);
            d1 = _mm256_packs_epi32(v2, v3);

            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);

            _mm_store_si128((__m128i*)dst, _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 2 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 3 * line), _mm256_extracti128_si256(d1, 1));

            dst += 8;
        }
    }
    else {//line=4	
        __m256i s0;
        __m256i v0, v1;
        __m256i d0, d1;
        __m256i tab0;
        const __m256i coeff0 = _mm256_set_epi16(17, 42, 17, 42, 17, 42, 17, 42, 32, 32, 32, 32, 32, 32, 32, 32); 
        const __m256i coeff1 = _mm256_set_epi16(-42, 17, -42, 17, -42, 17, -42, 17, -32, 32, -32, 32, -32, 32, -32, 32); 
        __m256i sign = _mm256_set_epi16(-1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1);

        tab0 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[2]);  // 16bit: 0, 3, 1, 2, 4, 7, 5, 6, 0, 3, 1, 2, 4, 7, 5, 6

        s0 = _mm256_loadu_si256((__m256i*)(src));           // src[0][0-4], src[1][0-4], src[2][0-4], src[3][0-4]

        v0 = _mm256_shuffle_epi8(s0, tab0);                 // src[0][0, 3, 1, 2], src[1][0, 3, 1, 2], src[2][0, 3, 1, 2], src[3][0, 3, 1, 2]
        v1 = _mm256_sign_epi16(v0, sign);

        v0 = _mm256_hadd_epi16(v0, v1);                     // e[0][0, 1], e[1][0, 1], o[0][0, 1], o[1][0, 1], e[2][0, 1], e[3][0, 1], o[2][0, 1], o[3][0, 1]
        v0 = _mm256_permute4x64_epi64(v0, 0xd8);            // e[0-3][0, 1], o[0-3][0, 1]

        d0 = _mm256_madd_epi16(v0, coeff0);                 // dst[0][0-4], dst[1][0-4]
        d1 = _mm256_madd_epi16(v0, coeff1);                 // dst[2][0-4], dst[3][0-4]

#if (BIT_DEPTH > 8)
        if (shift > 0) {
            d0 = _mm256_add_epi32(d0, add);
            d1 = _mm256_add_epi32(d1, add);

            d0 = _mm256_srai_epi32(d0, shift);
            d1 = _mm256_srai_epi32(d1, shift);
        }
#endif

        d0 = _mm256_packs_epi32(d0, d1);
        d0 = _mm256_permute4x64_epi64(d0, 0xd8);

        _mm256_storeu_si256((__m256i*)dst, d0);
    }
}

static void tx_dct2_pb4_2nd_avx2(s16 *src, s16 *dst, int line, int limit_line, int shift)
{
    if (line > 4)
    {
        int j;
        __m256i s0, s1;
        __m256i v0, v1, v2, v3, v4, v5, v6, v7;
        __m256i d0, d1;
        const __m256i coeff0 = _mm256_set1_epi16(32);    // E
        const __m256i coeff1 = _mm256_set_epi16(32, -32, -32, 32, 32, -32, -32, 32, 32, -32, -32, 32, 32, -32, -32, 32);
        const __m256i coeff2 = _mm256_set_epi16(-42, -17, 17, 42, -42, -17, 17, 42, -42, -17, 17, 42, -42, -17, 17, 42);    // O
        const __m256i coeff3 = _mm256_set_epi16(-17, 42, -42, 17, -17, 42, -42, 17, -17, 42, -42, 17, -17, 42, -42, 17);
        __m256i add = _mm256_set1_epi32(1 << (shift - 1));

        for (j = 0; j < limit_line; j += 8)
        {
            s0 = _mm256_loadu2_m128i((const __m128i*)&src[4 * 4], (const __m128i*)&src[0]);  // src[0][0-4], src[1][0-4], src[4][0-4], src[5][0-4]
            s1 = _mm256_loadu2_m128i((const __m128i*)&src[6 * 4], (const __m128i*)&src[8]);  // src[2][0-4], src[3][0-4], src[6][0-4], src[7][0-4]

            src += 4 * 8;

            v0 = _mm256_madd_epi16(s0, coeff0);                     
            v1 = _mm256_madd_epi16(s0, coeff1);  
            v2 = _mm256_madd_epi16(s0, coeff2);  
            v3 = _mm256_madd_epi16(s0, coeff3);  
            v4 = _mm256_madd_epi16(s1, coeff0);
            v5 = _mm256_madd_epi16(s1, coeff1);
            v6 = _mm256_madd_epi16(s1, coeff2);
            v7 = _mm256_madd_epi16(s1, coeff3);

            v0 = _mm256_hadd_epi32(v0, v4);             // dst[0][0-7]
            v1 = _mm256_hadd_epi32(v1, v5);             // dst[2][0-7]
            v2 = _mm256_hadd_epi32(v2, v6);             // dst[1][0-7]
            v3 = _mm256_hadd_epi32(v3, v7);             // dst[3][0-7]

            v0 = _mm256_add_epi32(v0, add);
            v1 = _mm256_add_epi32(v1, add);
            v2 = _mm256_add_epi32(v2, add);
            v3 = _mm256_add_epi32(v3, add);

            v0 = _mm256_srai_epi32(v0, shift);
            v1 = _mm256_srai_epi32(v1, shift);
            v2 = _mm256_srai_epi32(v2, shift);
            v3 = _mm256_srai_epi32(v3, shift);

            d0 = _mm256_packs_epi32(v0, v1);
            d1 = _mm256_packs_epi32(v2, v3);

            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);

            _mm_store_si128((__m128i*)dst, _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 2 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 3 * line), _mm256_extracti128_si256(d1, 1));

            dst += 8;
        }
    }
    else {//line=4	
        __m256i d0;
        __m256i v0, v1, v2, v3;
        __m256i s0;
        const __m256i coeff0 = _mm256_set1_epi16(32);       // E
        const __m256i coeff1 = _mm256_set_epi16(32, -32, -32, 32, 32, -32, -32, 32, 32, -32, -32, 32, 32, -32, -32, 32);
        const __m256i coeff2 = _mm256_set_epi16(-42, -17, 17, 42, -42, -17, 17, 42, -42, -17, 17, 42, -42, -17, 17, 42);    // O
        const __m256i coeff3 = _mm256_set_epi16(-17, 42, -42, 17, -17, 42, -42, 17, -17, 42, -42, 17, -17, 42, -42, 17);
        __m256i add = _mm256_set1_epi32(1 << (shift - 1));

        s0 = _mm256_loadu_si256((__m256i*)(src));           // src[0][0-4], src[1][0-4], src[2][0-4], src[3][0-4]

        v0 = _mm256_madd_epi16(s0, coeff0);
        v1 = _mm256_madd_epi16(s0, coeff1);
        v2 = _mm256_madd_epi16(s0, coeff2);
        v3 = _mm256_madd_epi16(s0, coeff3);

        v0 = _mm256_hadd_epi32(v0, v1);                     // dst[0][0-1], dst[2][0-1], dst[0][2-3], dst[2][2-3]
        v1 = _mm256_hadd_epi32(v2, v3);                     // dst[1][0-1], dst[3][0-1], dst[1][2-3], dst[3][2-3]

        v0 = _mm256_permute4x64_epi64(v0, 0xd8);            // dst[0][0-3], dst[2][0-3]
        v1 = _mm256_permute4x64_epi64(v1, 0xd8);

        v0 = _mm256_add_epi32(v0, add);
        v1 = _mm256_add_epi32(v1, add);

        v0 = _mm256_srai_epi32(v0, shift);
        v1 = _mm256_srai_epi32(v1, shift);

        d0 = _mm256_packs_epi32(v0, v1);

        _mm256_storeu_si256((__m256i*)dst, d0);
    }
}

static void tx_dct2_pb8_1st_avx2(s16 *src, s16 *dst, int line, int limit_line, int shift)
{
    if (line > 4)
    {
        int j;
        __m256i s00, s01, s02, s03;
        __m256i tab0, tab1;
        __m256i e0, e1, o0, o1;
        __m256i ee0, eo0;
        __m256i v0, v1, v2, v3, v4, v5, v6, v7;
        __m256i d0, d1;
        __m256i add = _mm256_set1_epi32(1 << (shift - 1));
        __m256i coeffs[4];
        const __m256i coeff_p32_p32 = _mm256_set1_epi32(0x00200020); // EEE
        const __m256i coeff_p32_n32 = _mm256_set1_epi32(0xFFE00020);
        const __m256i coeff_p42_p17 = _mm256_set1_epi32(0x0011002A); // EEO
        const __m256i coeff_p17_n42 = _mm256_set1_epi32(0xFFD60011);

        tab0 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[1]);  // 16bit: 0, 7, 1, 6, 2, 5, 3, 4, 0, 7, 1, 6, 2, 5, 3, 4
        tab1 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[2]);  // 16bit: 0, 3, 1, 2, 4, 7, 5, 6, 0, 3, 1, 2, 4, 7, 5, 6

        for (j = 0; j < 4; j++) {
            coeffs[j] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[j]);
        }

        for (j = 0; j < limit_line; j += 8)
        {
            s00 = _mm256_loadu2_m128i((const __m128i*)&src[4 * 8], (const __m128i*)&src[0]);    // src[0][0-7], src[4][0-7]
            s01 = _mm256_loadu2_m128i((const __m128i*)&src[5 * 8], (const __m128i*)&src[8]);    // src[1][0-7], src[5][0-7]
            s02 = _mm256_loadu2_m128i((const __m128i*)&src[6 * 8], (const __m128i*)&src[16]);
            s03 = _mm256_loadu2_m128i((const __m128i*)&src[7 * 8], (const __m128i*)&src[24]);

            s00 = _mm256_shuffle_epi8(s00, tab0);                           // src[0][0, 7, 1, 6, 2, 5, 3, 4], src[4][0, 7, 1, 6, 2, 5, 3, 4]
            s01 = _mm256_shuffle_epi8(s01, tab0);
            s02 = _mm256_shuffle_epi8(s02, tab0);
            s03 = _mm256_shuffle_epi8(s03, tab0);

            src += 8 * 8;

            e0 = _mm256_hadd_epi16(s00, s01);                               // e[0][0-3], e[1][0-3], e[4][0-3], e[5][0-3]
            o0 = _mm256_hsub_epi16(s00, s01);                               // o[0][0-3], o[1][0-3], o[4][0-3], o[5][0-3]
            e1 = _mm256_hadd_epi16(s02, s03);   
            o1 = _mm256_hsub_epi16(s02, s03);   

            e0 = _mm256_shuffle_epi8(e0, tab1);                             // ee[0][0, 3, 1, 2], ee[1][0, 3, 1, 2], ee[4][0, 3, 1, 2], ee[5][0, 3, 1, 2]
            e1 = _mm256_shuffle_epi8(e1, tab1);

            ee0 = _mm256_hadd_epi16(e0, e1);                                // eee[0][0, 1], eee[1][0, 1], eee[2][0, 1], eee[3][0, 1], eee[4][0, 1], eee[5][0, 1], eee[6][0, 1], eee[7][0, 1]
            eo0 = _mm256_hsub_epi16(e0, e1);

            v0 = _mm256_madd_epi16(ee0, coeff_p32_p32);                    // dst[0][0-7]                   
            v1 = _mm256_madd_epi16(ee0, coeff_p32_n32);                    // dst[4][0-7]
            v2 = _mm256_madd_epi16(eo0, coeff_p42_p17);                    // dst[2][0-7]
            v3 = _mm256_madd_epi16(eo0, coeff_p17_n42);                    // dst[6][0-7]

            v0 = _mm256_add_epi32(v0, add);
            v1 = _mm256_add_epi32(v1, add);
            v2 = _mm256_add_epi32(v2, add);
            v3 = _mm256_add_epi32(v3, add);

            v0 = _mm256_srai_epi32(v0, shift);
            v1 = _mm256_srai_epi32(v1, shift);
            v2 = _mm256_srai_epi32(v2, shift);
            v3 = _mm256_srai_epi32(v3, shift);

            d0 = _mm256_packs_epi32(v0, v1);
            d1 = _mm256_packs_epi32(v2, v3);

            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);

            _mm_store_si128((__m128i*)dst, _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 2 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 4 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 6 * line), _mm256_extracti128_si256(d1, 1));

            v0 = _mm256_madd_epi16(o0, coeffs[0]);
            v1 = _mm256_madd_epi16(o1, coeffs[0]);
            v2 = _mm256_madd_epi16(o0, coeffs[1]);
            v3 = _mm256_madd_epi16(o1, coeffs[1]);
            v4 = _mm256_madd_epi16(o0, coeffs[2]);
            v5 = _mm256_madd_epi16(o1, coeffs[2]);
            v6 = _mm256_madd_epi16(o0, coeffs[3]);
            v7 = _mm256_madd_epi16(o1, coeffs[3]);

            v0 = _mm256_hadd_epi32(v0, v1);                 // dst[1][0-7]
            v2 = _mm256_hadd_epi32(v2, v3);                 // dst[3][0-7]
            v4 = _mm256_hadd_epi32(v4, v5);                 // dst[5][0-7]
            v6 = _mm256_hadd_epi32(v6, v7);                 // dst[7][0-7]

            v0 = _mm256_add_epi32(v0, add);
            v1 = _mm256_add_epi32(v2, add);
            v2 = _mm256_add_epi32(v4, add);
            v3 = _mm256_add_epi32(v6, add);

            v0 = _mm256_srai_epi32(v0, shift);
            v1 = _mm256_srai_epi32(v1, shift);
            v2 = _mm256_srai_epi32(v2, shift);
            v3 = _mm256_srai_epi32(v3, shift);

            d0 = _mm256_packs_epi32(v0, v1);
            d1 = _mm256_packs_epi32(v2, v3);

            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);

            _mm_store_si128((__m128i*)(dst + line), _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 3 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 5 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 7 * line), _mm256_extracti128_si256(d1, 1));

            dst += 8;
        }
    }
    else {//line=4	
        int j;
        __m256i s00, s01;
        __m256i tab0, tab1;
        __m256i e0, o0;
        __m256i v0, v1, v2, v3;
        __m256i d0, d1, d2, d3;
        __m256i add = _mm256_set1_epi32(1 << (shift - 1));
        __m256i coeffs[4];
        const __m256i coeff0 = _mm256_set_epi16(17, 42, 17, 42, 17, 42, 17, 42, 32, 32, 32, 32, 32, 32, 32, 32);
        const __m256i coeff1 = _mm256_set_epi16(-42, 17, -42, 17, -42, 17, -42, 17, -32, 32, -32, 32, -32, 32, -32, 32);
        __m256i sign = _mm256_set_epi16(-1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1);

        tab0 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[1]);  // 16bit: 0, 7, 1, 6, 2, 5, 3, 4, 0, 7, 1, 6, 2, 5, 3, 4
        tab1 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[2]);  // 16bit: 0, 3, 1, 2, 4, 7, 5, 6, 0, 3, 1, 2, 4, 7, 5, 6

        for (j = 0; j < 4; j++) {
            coeffs[j] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[j]);
        }

        s00 = _mm256_loadu2_m128i((const __m128i*)&src[16], (const __m128i*)&src[0]);    // src[0][0-7], src[2][0-7]
        s01 = _mm256_loadu2_m128i((const __m128i*)&src[24], (const __m128i*)&src[8]);    // src[1][0-7], src[3][0-7]

        s00 = _mm256_shuffle_epi8(s00, tab0);                           // src[0][0, 7, 1, 6, 2, 5, 3, 4], src[2][0, 7, 1, 6, 2, 5, 3, 4]
        s01 = _mm256_shuffle_epi8(s01, tab0);

        e0 = _mm256_hadd_epi16(s00, s01);                                // e[0][0-3], e[1][0-3], e[2][0-3], e[3][0-3]
        o0 = _mm256_hsub_epi16(s00, s01);                                // o[0][0-3], o[1][0-3], o[2][0-3], o[3][0-3]

        v0 = _mm256_shuffle_epi8(e0, tab1);                             // e[0][0, 3, 1, 2], e[1][0, 3, 1, 2], e[4][0, 3, 1, 2], e[5][0, 3, 1, 2]
        v1 = _mm256_sign_epi16(v0, sign);

        v0 = _mm256_hadd_epi16(v0, v1);                                 // ee[0][0, 1], ee[1][0, 1], eo[0][0, 1], eo[1][0, 1], ee[2][0, 1], ee[3][0, 1], eo[2][0, 1], eo[3][0, 1]
        v0 = _mm256_permute4x64_epi64(v0, 0xd8);                        // ee[0-3][0, 1], eo[0-3][0, 1]

        d0 = _mm256_madd_epi16(v0, coeff0);                             // dst[0][0-3], dst[2][0-3]
        d1 = _mm256_madd_epi16(v0, coeff1);                             // dst[4][0-3], dst[6][0-3]

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);

        d0 = _mm256_srai_epi32(d0, shift);
        d1 = _mm256_srai_epi32(d1, shift);

        d0 = _mm256_packs_epi32(d0, d1);                                // dst[0][0-3], dst[4][0-3], dst[2][0-3], dst[6][0-3]

        d0 = _mm256_permute4x64_epi64(d0, 0xd8);

        v0 = _mm256_madd_epi16(o0, coeffs[0]);
        v1 = _mm256_madd_epi16(o0, coeffs[1]);
        v2 = _mm256_madd_epi16(o0, coeffs[2]);
        v3 = _mm256_madd_epi16(o0, coeffs[3]);

        v0 = _mm256_hadd_epi32(v0, v1);                                 // dst[1][0-1], dst[3][0-1], dst[1][2-3], dst[3][2-3]
        v2 = _mm256_hadd_epi32(v2, v3);

        v0 = _mm256_permute4x64_epi64(v0, 0xd8);                        // dst[1][0-3], dst[3][0-3]
        v1 = _mm256_permute4x64_epi64(v2, 0xd8);

        v0 = _mm256_add_epi32(v0, add);
        v1 = _mm256_add_epi32(v1, add);

        v0 = _mm256_srai_epi32(v0, shift);
        v1 = _mm256_srai_epi32(v1, shift);

        d1 = _mm256_packs_epi32(v0, v1);

        d1 = _mm256_permute4x64_epi64(d1, 0xd8);                        // dst[1][0-3], dst[3][0-3], dst[5][0-3], dst[7][0-3]

        d2 = _mm256_unpacklo_epi64(d0, d1);                             // dst[0][0-3], dst[1][0-3], dst[4][0-3], dst[5][0-3]
        d3 = _mm256_unpackhi_epi64(d0, d1);

        _mm_store_si128((__m128i*)(dst), _mm256_castsi256_si128(d2));
        _mm_store_si128((__m128i*)(dst + 8), _mm256_castsi256_si128(d3));
        _mm_store_si128((__m128i*)(dst + 16), _mm256_extracti128_si256(d2, 1));
        _mm_store_si128((__m128i*)(dst + 24), _mm256_extracti128_si256(d3, 1));
    }
}

static void tx_dct2_pb8_2nd_avx2(s16 *src, s16 *dst, int line, int limit_line, int shift)
{
    __m256i v0, v1, v2, v3, v4, v5, v6, v7;
    __m256i d0, d1, d2, d3;
    const __m256i coeff0 = _mm256_set1_epi16(32);    // EE
    const __m256i coeff1 = _mm256_set_epi16(32, -32, -32, 32, 32, -32, -32, 32, 32, -32, -32, 32, 32, -32, -32, 32);
    const __m256i coeff2 = _mm256_set_epi16(42, 17, -17, -42, -42, -17, 17, 42, 42, 17, -17, -42, -42, -17, 17, 42);    // EO
    const __m256i coeff3 = _mm256_set_epi16(17, -42, 42, -17, -17, 42, -42, 17, 17, -42, 42, -17, -17, 42, -42, 17);
    const __m256i coeff4 = _mm256_set_epi16(-44, -38, -25, -9, 9, 25, 38, 44, -44, -38, -25, -9, 9, 25, 38, 44);    // O
    const __m256i coeff5 = _mm256_set_epi16(-38, 9, 44, 25, -25, -44, -9, 38, -38, 9, 44, 25, -25, -44, -9, 38);
    const __m256i coeff6 = _mm256_set_epi16(-25, 44, -9, -38, 38, 9, -44, 25, -25, 44, -9, -38, 38, 9, -44, 25); 
    const __m256i coeff7 = _mm256_set_epi16(-9, 25, -38, 44, -44, 38, -25, 9, -9, 25, -38, 44, -44, 38, -25, 9);
    __m256i add = _mm256_set1_epi32(1 << (shift - 1));

    if (line > 4)
    {
        int j;
        __m256i s0, s1, s2, s3;

        for (j = 0; j < limit_line; j += 8)
        {
            s0 = _mm256_loadu2_m128i((const __m128i*)&src[4 * 8], (const __m128i*)&src[0]);    // src[0][0-7], src[4][0-7]
            s1 = _mm256_loadu2_m128i((const __m128i*)&src[5 * 8], (const __m128i*)&src[8]);    // src[1][0-7], src[5][0-7]
            s2 = _mm256_loadu2_m128i((const __m128i*)&src[6 * 8], (const __m128i*)&src[16]);
            s3 = _mm256_loadu2_m128i((const __m128i*)&src[7 * 8], (const __m128i*)&src[24]);

            src += 8 * 8;

#define CALCU_2x8(c0, c1, d0, d1) \
            v0 = _mm256_madd_epi16(s0, c0); \
            v1 = _mm256_madd_epi16(s1, c0); \
            v2 = _mm256_madd_epi16(s2, c0); \
            v3 = _mm256_madd_epi16(s3, c0); \
            v4 = _mm256_madd_epi16(s0, c1); \
            v5 = _mm256_madd_epi16(s1, c1); \
            v6 = _mm256_madd_epi16(s2, c1); \
            v7 = _mm256_madd_epi16(s3, c1); \
            v0 = _mm256_hadd_epi32(v0, v1); \
            v2 = _mm256_hadd_epi32(v2, v3); \
            v4 = _mm256_hadd_epi32(v4, v5); \
            v6 = _mm256_hadd_epi32(v6, v7); \
            d0 = _mm256_hadd_epi32(v0, v2); \
            d1 = _mm256_hadd_epi32(v4, v6)

            CALCU_2x8(coeff0, coeff4, d0, d1);
            CALCU_2x8(coeff2, coeff5, d2, d3);

            d0 = _mm256_add_epi32(d0, add);
            d1 = _mm256_add_epi32(d1, add);
            d2 = _mm256_add_epi32(d2, add);
            d3 = _mm256_add_epi32(d3, add);

            d0 = _mm256_srai_epi32(d0, shift);      // dst[0][0-7]
            d1 = _mm256_srai_epi32(d1, shift);      // dst[1][0-7]
            d2 = _mm256_srai_epi32(d2, shift);
            d3 = _mm256_srai_epi32(d3, shift);

            d0 = _mm256_packs_epi32(d0, d1);
            d1 = _mm256_packs_epi32(d2, d3);

            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);

            _mm_store_si128((__m128i*)dst, _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 1 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 2 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 3 * line), _mm256_extracti128_si256(d1, 1));

            CALCU_2x8(coeff1, coeff6, d0, d1);
            CALCU_2x8(coeff3, coeff7, d2, d3);
#undef CALCU_2x8

            d0 = _mm256_add_epi32(d0, add);
            d1 = _mm256_add_epi32(d1, add);
            d2 = _mm256_add_epi32(d2, add);
            d3 = _mm256_add_epi32(d3, add);

            d0 = _mm256_srai_epi32(d0, shift);      // dst[4][0-7]
            d1 = _mm256_srai_epi32(d1, shift);      // dst[5][0-7]
            d2 = _mm256_srai_epi32(d2, shift);
            d3 = _mm256_srai_epi32(d3, shift);

            d0 = _mm256_packs_epi32(d0, d1);
            d1 = _mm256_packs_epi32(d2, d3);

            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);

            _mm_store_si128((__m128i*)(dst + 4 * line), _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 5 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 6 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 7 * line), _mm256_extracti128_si256(d1, 1));

            dst += 8;
        }
    }
    else {//line=4	
        __m256i s0, s1;

        s0 = _mm256_loadu2_m128i((const __m128i*)&src[2 * 8], (const __m128i*)&src[0]);    // src[0][0-7], src[2][0-7]
        s1 = _mm256_loadu2_m128i((const __m128i*)&src[3 * 8], (const __m128i*)&src[8]);    // src[1][0-7], src[3][0-7]

#define CALCU_2x8(c0, c1, c2, c3, d0, d1) \
            v0 = _mm256_madd_epi16(s0, c0); \
            v1 = _mm256_madd_epi16(s1, c0); \
            v2 = _mm256_madd_epi16(s0, c1); \
            v3 = _mm256_madd_epi16(s1, c1); \
            v4 = _mm256_madd_epi16(s0, c2); \
            v5 = _mm256_madd_epi16(s1, c2); \
            v6 = _mm256_madd_epi16(s0, c3); \
            v7 = _mm256_madd_epi16(s1, c3); \
            v0 = _mm256_hadd_epi32(v0, v1); \
            v2 = _mm256_hadd_epi32(v2, v3); \
            v4 = _mm256_hadd_epi32(v4, v5); \
            v6 = _mm256_hadd_epi32(v6, v7); \
            d0 = _mm256_hadd_epi32(v0, v2); \
            d1 = _mm256_hadd_epi32(v4, v6); \
            d0 = _mm256_permute4x64_epi64(d0, 0xd8); \
            d1 = _mm256_permute4x64_epi64(d1, 0xd8)

        CALCU_2x8(coeff0, coeff4, coeff2, coeff5, d0, d1);
        CALCU_2x8(coeff1, coeff6, coeff3, coeff7, d2, d3);

        d0 = _mm256_add_epi32(d0, add);                     // dst[0][0-3], dst[1][0-3]
        d1 = _mm256_add_epi32(d1, add);                     // dst[2][0-3], dst[3][0-3]
        d2 = _mm256_add_epi32(d2, add);
        d3 = _mm256_add_epi32(d3, add);

        d0 = _mm256_srai_epi32(d0, shift);      
        d1 = _mm256_srai_epi32(d1, shift);      
        d2 = _mm256_srai_epi32(d2, shift);
        d3 = _mm256_srai_epi32(d3, shift);

        d0 = _mm256_packs_epi32(d0, d1);
        d1 = _mm256_packs_epi32(d2, d3);

        d0 = _mm256_permute4x64_epi64(d0, 0xd8);
        d1 = _mm256_permute4x64_epi64(d1, 0xd8);

        _mm256_storeu_si256((__m256i*)dst, d0);
        _mm256_storeu_si256((__m256i*)(dst + 16), d1);
    }
}

static void tx_dct2_pb16_1st_avx2(s16 *src, s16 *dst, int line, int limit_line, int shift)
{
    if (line > 4)
    {
        int j;
        __m256i s00, s01, s02, s03, s04, s05, s06, s07;
        __m256i tab0, tab1, tab2;
        __m256i e0, e1, e2, e3;
        __m256i o0, o1, o2, o3;
        __m256i ee0, ee1, eo0, eo1;
        __m256i eee0, eeo0;
        __m256i v0, v1, v2, v3, v4, v5, v6, v7;
        __m256i d0, d1, d2, d3, d4, d5, d6, d7;
        __m256i add = _mm256_set1_epi32(1 << (shift - 1));
        __m256i coeffs[8];
        const __m256i coeff_p32_p32 = _mm256_set1_epi32(0x00200020); // EEE
        const __m256i coeff_p32_n32 = _mm256_set1_epi32(0xFFE00020);
        const __m256i coeff_p42_p17 = _mm256_set1_epi32(0x0011002A); // EEO
        const __m256i coeff_p17_n42 = _mm256_set1_epi32(0xFFD60011);

        tab0 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[0]);  // 16bit: 7-0, 7-0
        tab1 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[1]);  // 16bit: 0, 7, 1, 6, 2, 5, 3, 4, 0, 7, 1, 6, 2, 5, 3, 4
        tab2 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[2]);  // 16bit: 0, 3, 1, 2, 4, 7, 5, 6, 0, 3, 1, 2, 4, 7, 5, 6

        for (j = 0; j < limit_line; j += 8)
        {
            s00 = _mm256_loadu2_m128i((const __m128i*)&src[4 * 16], (const __m128i*)&src[0]);           // src[0][0-7], src[4][0-7]
            s01 = _mm256_loadu2_m128i((const __m128i*)&src[4 * 16 + 8], (const __m128i*)&src[8]);       // src[0][8-15], src[4][8-15]
            s02 = _mm256_loadu2_m128i((const __m128i*)&src[5 * 16], (const __m128i*)&src[16]);
            s03 = _mm256_loadu2_m128i((const __m128i*)&src[5 * 16 + 8], (const __m128i*)&src[16 + 8]);
            s04 = _mm256_loadu2_m128i((const __m128i*)&src[6 * 16], (const __m128i*)&src[16 * 2]);
            s05 = _mm256_loadu2_m128i((const __m128i*)&src[6 * 16 + 8], (const __m128i*)&src[16 * 2 + 8]);
            s06 = _mm256_loadu2_m128i((const __m128i*)&src[7 * 16], (const __m128i*)&src[16 * 3]);
            s07 = _mm256_loadu2_m128i((const __m128i*)&src[7 * 16 + 8], (const __m128i*)&src[16 * 3 + 8]);

            s01 = _mm256_shuffle_epi8(s01, tab0);                           // src[0][15-8], src[4][15-8]
            s03 = _mm256_shuffle_epi8(s03, tab0);
            s05 = _mm256_shuffle_epi8(s05, tab0);
            s07 = _mm256_shuffle_epi8(s07, tab0);

            src += 16 * 8;

            e0 = _mm256_add_epi16(s00, s01);                                // e[0][0-7], e[4][0-7]
            o0 = _mm256_sub_epi16(s00, s01);                                // o[0][0-7], o[4][0-7]
            e1 = _mm256_add_epi16(s02, s03);                                // e[1][0-7], e[5][0-7]
            o1 = _mm256_sub_epi16(s02, s03);                                // o[1][0-7], o[5][0-7]
            e2 = _mm256_add_epi16(s04, s05);
            o2 = _mm256_sub_epi16(s04, s05);
            e3 = _mm256_add_epi16(s06, s07);
            o3 = _mm256_sub_epi16(s06, s07);

            e0 = _mm256_shuffle_epi8(e0, tab1);                             // e[0][0, 7, 1, 6, 2, 5, 3, 4], e[4][0, 7, 1, 6, 2, 5, 3, 4]
            e1 = _mm256_shuffle_epi8(e1, tab1);
            e2 = _mm256_shuffle_epi8(e2, tab1);
            e3 = _mm256_shuffle_epi8(e3, tab1);

            ee0 = _mm256_hadd_epi16(e0, e1);                                // ee[0][0-3], ee[1][0-3], ee[4][0-3], ee[5][0-3]
            eo0 = _mm256_hsub_epi16(e0, e1);                                // eo[0][0-3], eo[1][0-3], eo[4][0-3], eo[5][0-3]
            ee1 = _mm256_hadd_epi16(e2, e3);
            eo1 = _mm256_hsub_epi16(e2, e3);

            ee0 = _mm256_shuffle_epi8(ee0, tab2);                           // ee[0][0, 3, 1, 2], ee[1][0, 3, 1, 2], ee[4][0, 3, 1, 2], ee[5][0, 3, 1, 2]
            ee1 = _mm256_shuffle_epi8(ee1, tab2);

            eee0 = _mm256_hadd_epi16(ee0, ee1);                             // eee[0][0, 1], eee[1][0, 1], eee[2][0, 1], eee[3][0, 1], eee[4][0, 1], eee[5][0, 1], eee[6][0, 1], eee[7][0, 1]
            eeo0 = _mm256_hsub_epi16(ee0, ee1);

            v0 = _mm256_madd_epi16(eee0, coeff_p32_p32);                    // dst[0][0-7]                   
            v1 = _mm256_madd_epi16(eee0, coeff_p32_n32);                    // dst[8][0-7]
            v2 = _mm256_madd_epi16(eeo0, coeff_p42_p17);                    // dst[4][0-7]
            v3 = _mm256_madd_epi16(eeo0, coeff_p17_n42);                    // dst[12][0-7]

            v0 = _mm256_add_epi32(v0, add);
            v1 = _mm256_add_epi32(v1, add);
            v2 = _mm256_add_epi32(v2, add);
            v3 = _mm256_add_epi32(v3, add);

            v0 = _mm256_srai_epi32(v0, shift);
            v1 = _mm256_srai_epi32(v1, shift);
            v2 = _mm256_srai_epi32(v2, shift);
            v3 = _mm256_srai_epi32(v3, shift);

            coeffs[0] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[0]);
            coeffs[1] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[1]);
            coeffs[2] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[2]);
            coeffs[3] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[3]);

            d0 = _mm256_packs_epi32(v0, v1);
            d1 = _mm256_packs_epi32(v2, v3);

            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);

            _mm_store_si128((__m128i*)dst, _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 4 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 8 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 12 * line), _mm256_extracti128_si256(d1, 1));

            v0 = _mm256_madd_epi16(eo0, coeffs[0]);
            v1 = _mm256_madd_epi16(eo1, coeffs[0]);
            v2 = _mm256_madd_epi16(eo0, coeffs[1]);
            v3 = _mm256_madd_epi16(eo1, coeffs[1]);
            v4 = _mm256_madd_epi16(eo0, coeffs[2]);
            v5 = _mm256_madd_epi16(eo1, coeffs[2]);
            v6 = _mm256_madd_epi16(eo0, coeffs[3]);
            v7 = _mm256_madd_epi16(eo1, coeffs[3]);

            v0 = _mm256_hadd_epi32(v0, v1);                 // dst[2][0-7]
            v2 = _mm256_hadd_epi32(v2, v3);                 // dst[6][0-7]
            v4 = _mm256_hadd_epi32(v4, v5);                 // dst[10][0-7]
            v6 = _mm256_hadd_epi32(v6, v7);                 // dst[14][0-7]

            v0 = _mm256_add_epi32(v0, add);
            v1 = _mm256_add_epi32(v2, add);
            v2 = _mm256_add_epi32(v4, add);
            v3 = _mm256_add_epi32(v6, add);

            v0 = _mm256_srai_epi32(v0, shift);
            v1 = _mm256_srai_epi32(v1, shift);
            v2 = _mm256_srai_epi32(v2, shift);
            v3 = _mm256_srai_epi32(v3, shift);

            d0 = _mm256_packs_epi32(v0, v1);
            d1 = _mm256_packs_epi32(v2, v3);

            coeffs[0] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[4]);
            coeffs[1] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[5]);
            coeffs[2] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[6]);
            coeffs[3] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[7]);
            coeffs[4] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[8]);
            coeffs[5] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[9]);
            coeffs[6] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[10]);
            coeffs[7] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[11]);

            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);

            _mm_store_si128((__m128i*)(dst + 2 * line), _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 6 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 10 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 14 * line), _mm256_extracti128_si256(d1, 1));

#define CALCU_O(coeff0, coeff1, d0, d1) \
            v0 = _mm256_madd_epi16(o0, coeff0); \
            v1 = _mm256_madd_epi16(o1, coeff0); \
            v2 = _mm256_madd_epi16(o2, coeff0); \
            v3 = _mm256_madd_epi16(o3, coeff0); \
            v4 = _mm256_madd_epi16(o0, coeff1); \
            v5 = _mm256_madd_epi16(o1, coeff1); \
            v6 = _mm256_madd_epi16(o2, coeff1); \
            v7 = _mm256_madd_epi16(o3, coeff1); \
            v0 = _mm256_hadd_epi32(v0, v1); \
            v2 = _mm256_hadd_epi32(v2, v3); \
            v4 = _mm256_hadd_epi32(v4, v5); \
            v6 = _mm256_hadd_epi32(v6, v7); \
            d0 = _mm256_hadd_epi32(v0, v2); \
            d1 = _mm256_hadd_epi32(v4, v6)

            CALCU_O(coeffs[0], coeffs[1], d0, d1);
            CALCU_O(coeffs[2], coeffs[3], d2, d3);
            CALCU_O(coeffs[4], coeffs[5], d4, d5);
            CALCU_O(coeffs[6], coeffs[7], d6, d7);

#undef CALCU_O

            d0 = _mm256_add_epi32(d0, add);
            d1 = _mm256_add_epi32(d1, add);
            d2 = _mm256_add_epi32(d2, add);
            d3 = _mm256_add_epi32(d3, add);
            d4 = _mm256_add_epi32(d4, add);
            d5 = _mm256_add_epi32(d5, add);
            d6 = _mm256_add_epi32(d6, add);
            d7 = _mm256_add_epi32(d7, add);

            d0 = _mm256_srai_epi32(d0, shift);
            d1 = _mm256_srai_epi32(d1, shift);
            d2 = _mm256_srai_epi32(d2, shift);
            d3 = _mm256_srai_epi32(d3, shift);
            d4 = _mm256_srai_epi32(d4, shift);
            d5 = _mm256_srai_epi32(d5, shift);
            d6 = _mm256_srai_epi32(d6, shift);
            d7 = _mm256_srai_epi32(d7, shift);

            d0 = _mm256_packs_epi32(d0, d1);
            d1 = _mm256_packs_epi32(d2, d3);
            d2 = _mm256_packs_epi32(d4, d5);
            d3 = _mm256_packs_epi32(d6, d7);
            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);
            d2 = _mm256_permute4x64_epi64(d2, 0xd8);
            d3 = _mm256_permute4x64_epi64(d3, 0xd8);

            _mm_store_si128((__m128i*)(dst + line), _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 3 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 5 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 7 * line), _mm256_extracti128_si256(d1, 1));
            _mm_store_si128((__m128i*)(dst + 9 * line), _mm256_castsi256_si128(d2));
            _mm_store_si128((__m128i*)(dst + 11 * line), _mm256_extracti128_si256(d2, 1));
            _mm_store_si128((__m128i*)(dst + 13 * line), _mm256_castsi256_si128(d3));
            _mm_store_si128((__m128i*)(dst + 15 * line), _mm256_extracti128_si256(d3, 1));

            dst += 8;
        }
    }
    else {//line=4	
        __m256i s00, s01, s02, s03;
        __m256i tab0, tab1, tab2;
        __m256i e0, e1;
        __m256i o0, o1;
        __m256i ee0, eo0;
        __m256i v0, v1, v2, v3, v4, v5, v6, v7;
        __m256i d0, d1, d2, d3;
        __m256i add = _mm256_set1_epi32(1 << (shift - 1));
        __m256i coeffs[8];
        const __m256i coeff0 = _mm256_set_epi16(17, 42, 17, 42, 17, 42, 17, 42, 32, 32, 32, 32, 32, 32, 32, 32);
        const __m256i coeff1 = _mm256_set_epi16(-42, 17, -42, 17, -42, 17, -42, 17, -32, 32, -32, 32, -32, 32, -32, 32);
        __m256i sign = _mm256_set_epi16(-1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1);
        __m128i m0, m1, m2, m3;

        tab0 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[0]);  // 16bit: 7-0, 7-0
        tab1 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[1]);  // 16bit: 0, 7, 1, 6, 2, 5, 3, 4, 0, 7, 1, 6, 2, 5, 3, 4
        tab2 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[2]);  // 16bit: 0, 3, 1, 2, 4, 7, 5, 6, 0, 3, 1, 2, 4, 7, 5, 6

        s00 = _mm256_loadu2_m128i((const __m128i*)&src[2 * 16], (const __m128i*)&src[0]);           // src[0][0-7], src[4][0-7]
        s01 = _mm256_loadu2_m128i((const __m128i*)&src[2 * 16 + 8], (const __m128i*)&src[8]);       // src[0][8-15], src[4][8-15]
        s02 = _mm256_loadu2_m128i((const __m128i*)&src[3 * 16], (const __m128i*)&src[16]);
        s03 = _mm256_loadu2_m128i((const __m128i*)&src[3 * 16 + 8], (const __m128i*)&src[16 + 8]);

        s01 = _mm256_shuffle_epi8(s01, tab0);                           // src[0][15-8], src[4][15-8]
        s03 = _mm256_shuffle_epi8(s03, tab0);

        e0 = _mm256_add_epi16(s00, s01);                                // e[0][0-7], e[2][0-7]
        o0 = _mm256_sub_epi16(s00, s01);                                // o[0][0-7], o[2][0-7]
        e1 = _mm256_add_epi16(s02, s03);                                // e[1][0-7], e[3][0-7]
        o1 = _mm256_sub_epi16(s02, s03);                                // o[1][0-7], o[3][0-7]

        e0 = _mm256_shuffle_epi8(e0, tab1);                             // e[0][0, 7, 1, 6, 2, 5, 3, 4], e[2][0, 7, 1, 6, 2, 5, 3, 4]
        e1 = _mm256_shuffle_epi8(e1, tab1);

        ee0 = _mm256_hadd_epi16(e0, e1);                                // ee[0][0-3], ee[1][0-3], ee[2][0-3], ee[3][0-3]
        eo0 = _mm256_hsub_epi16(e0, e1);                                // eo[0][0-3], eo[1][0-3], eo[2][0-3], eo[3][0-3]

        ee0 = _mm256_shuffle_epi8(ee0, tab2);                           // ee[0][0, 3, 1, 2], ee[1][0, 3, 1, 2], ee[2][0, 3, 1, 2], ee[3][0, 3, 1, 2]
        v0 = _mm256_sign_epi16(ee0, sign);

        v0 = _mm256_hadd_epi16(ee0, v0);        
        v0 = _mm256_permute4x64_epi64(v0, 0xd8);                        // eee[0][0, 1], eee[1][0, 1], eee[2][0, 1], eee[3][0, 1], eeo[0][0, 1], eeo[1][0, 1], eeo[2][0, 1], eeo[3][0, 1]

        v2 = _mm256_madd_epi16(v0, coeff0);                             // dst[0][0-3], dst[4][0-3]              
        v3 = _mm256_madd_epi16(v0, coeff1);                             // dst[8][0-3], dst[12][0-3]

        v0 = _mm256_add_epi32(v2, add);
        v1 = _mm256_add_epi32(v3, add);

        v0 = _mm256_srai_epi32(v0, shift);
        v1 = _mm256_srai_epi32(v1, shift);

        coeffs[0] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[0]);
        coeffs[1] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[1]);
        coeffs[2] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[2]);
        coeffs[3] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[3]);

        d0 = _mm256_packs_epi32(v0, v1);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        _mm_storel_epi64((__m128i*)dst, m0);
        _mm_storel_epi64((__m128i*)(dst + 4 * 4), m1);
        _mm_storel_epi64((__m128i*)(dst + 8 * 4), m2);
        _mm_storel_epi64((__m128i*)(dst + 12 * 4), m3);

        v0 = _mm256_madd_epi16(eo0, coeffs[0]);
        v1 = _mm256_madd_epi16(eo0, coeffs[1]);
        v2 = _mm256_madd_epi16(eo0, coeffs[2]);
        v3 = _mm256_madd_epi16(eo0, coeffs[3]);

        v0 = _mm256_hadd_epi32(v0, v1);         
        v2 = _mm256_hadd_epi32(v2, v3);         

        v0 = _mm256_permute4x64_epi64(v0, 0xd8);        // dst[2][0-3], dst[6][0-3]
        v2 = _mm256_permute4x64_epi64(v2, 0xd8);        // dst[10][0-3], dst[14][0-3]

        v0 = _mm256_add_epi32(v0, add);
        v1 = _mm256_add_epi32(v2, add);

        v0 = _mm256_srai_epi32(v0, shift);
        v1 = _mm256_srai_epi32(v1, shift);

        d0 = _mm256_packs_epi32(v0, v1);

        coeffs[0] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[4]);
        coeffs[1] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[5]);
        coeffs[2] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[6]);
        coeffs[3] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[7]);
        coeffs[4] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[8]);
        coeffs[5] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[9]);
        coeffs[6] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[10]);
        coeffs[7] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[11]);

        m0 = _mm256_castsi256_si128(d0);                // dst[2]
        m1 = _mm256_extracti128_si256(d0, 1);           // dst[6]
        m2 = _mm_srli_si128(m0, 8);                     // dst[10]
        m3 = _mm_srli_si128(m1, 8);

        _mm_storel_epi64((__m128i*)(dst + 2 * 4), m0);
        _mm_storel_epi64((__m128i*)(dst + 6 * 4), m1);
        _mm_storel_epi64((__m128i*)(dst + 10 * 4), m2);
        _mm_storel_epi64((__m128i*)(dst + 14 * 4), m3);

#define CALCU_O(coeff0, coeff1, coeff2, coeff3, d0, d1) \
            v0 = _mm256_madd_epi16(o0, coeff0); \
            v1 = _mm256_madd_epi16(o1, coeff0); \
            v2 = _mm256_madd_epi16(o0, coeff1); \
            v3 = _mm256_madd_epi16(o1, coeff1); \
            v4 = _mm256_madd_epi16(o0, coeff2); \
            v5 = _mm256_madd_epi16(o1, coeff2); \
            v6 = _mm256_madd_epi16(o0, coeff3); \
            v7 = _mm256_madd_epi16(o1, coeff3); \
            v0 = _mm256_hadd_epi32(v0, v1); \
            v2 = _mm256_hadd_epi32(v2, v3); \
            v4 = _mm256_hadd_epi32(v4, v5); \
            v6 = _mm256_hadd_epi32(v6, v7); \
            d0 = _mm256_hadd_epi32(v0, v2); \
            d1 = _mm256_hadd_epi32(v4, v6); \
            d0 = _mm256_permute4x64_epi64(d0, 0xd8); \
            d1 = _mm256_permute4x64_epi64(d1, 0xd8); 

        CALCU_O(coeffs[0], coeffs[1], coeffs[2], coeffs[3], d0, d1);
        CALCU_O(coeffs[4], coeffs[5], coeffs[6], coeffs[7], d2, d3);

#undef CALCU_O

        d0 = _mm256_add_epi32(d0, add);         // dst[1][0-3], dst[3][0-3]
        d1 = _mm256_add_epi32(d1, add);
        d2 = _mm256_add_epi32(d2, add);
        d3 = _mm256_add_epi32(d3, add);

        d0 = _mm256_srai_epi32(d0, shift);
        d1 = _mm256_srai_epi32(d1, shift);
        d2 = _mm256_srai_epi32(d2, shift);
        d3 = _mm256_srai_epi32(d3, shift);

        d0 = _mm256_packs_epi32(d0, d1);
        d1 = _mm256_packs_epi32(d2, d3);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        _mm_storel_epi64((__m128i*)(dst + 1 * 4), m0);
        _mm_storel_epi64((__m128i*)(dst + 3 * 4), m1);
        _mm_storel_epi64((__m128i*)(dst + 5 * 4), m2);
        _mm_storel_epi64((__m128i*)(dst + 7 * 4), m3);

        m0 = _mm256_castsi256_si128(d1);
        m1 = _mm256_extracti128_si256(d1, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        _mm_storel_epi64((__m128i*)(dst + 9 * 4), m0);
        _mm_storel_epi64((__m128i*)(dst + 11 * 4), m1);
        _mm_storel_epi64((__m128i*)(dst + 13 * 4), m2);
        _mm_storel_epi64((__m128i*)(dst + 15 * 4), m3);
    }
}

static void tx_dct2_pb16_2nd_avx2(s16 *src, s16 *dst, int line, int limit_line, int shift)
{
    if (line > 4)
    {
        int j;
        __m256i s00, s01, s02, s03, s04, s05, s06, s07;
        __m256i v0, v1, v2, v3, v4, v5, v6, v7;
        __m256i d0, d1, d2, d3, d4, d5, d6, d7;
        __m256i add = _mm256_set1_epi32(1 << (shift - 1));
        __m256i coeffs[8];

        for (j = 0; j < limit_line; j += 8)
        {
            s00 = _mm256_loadu_si256((__m256i*)(src));            // src[0][0-15]
            s01 = _mm256_loadu_si256((__m256i*)(src + 16));
            s02 = _mm256_loadu_si256((__m256i*)(src + 16 * 2));
            s03 = _mm256_loadu_si256((__m256i*)(src + 16 * 3));
            s04 = _mm256_loadu_si256((__m256i*)(src + 16 * 4));
            s05 = _mm256_loadu_si256((__m256i*)(src + 16 * 5));
            s06 = _mm256_loadu_si256((__m256i*)(src + 16 * 6));
            s07 = _mm256_loadu_si256((__m256i*)(src + 16 * 7));

            coeffs[0] = _mm256_set1_epi16(32);
            coeffs[1] = _mm256_set_epi16(-45, -43, -40, -35, -29, -21, -13, -4, 4, 13, 21, 29, 35, 40, 43, 45);
            coeffs[2] = _mm256_set_epi16(44, 38, 25, 9, -9, -25, -38, -44, -44, -38, -25, -9, 9, 25, 38, 44);
            coeffs[3] = _mm256_set_epi16(-43, -29, -4, 21, 40, 45, 35, 13, -13, -35, -45, -40, -21, 4, 29, 43);
            coeffs[4] = _mm256_set_epi16(42, 17, -17, -42, -42, -17, 17, 42, 42, 17, -17, -42, -42, -17, 17, 42);
            coeffs[5] = _mm256_set_epi16(-40, -4, 35, 43, 13, -29, -45, -21, 21, 45, 29, -13, -43, -35, 4, 40);
            coeffs[6] = _mm256_set_epi16(38, -9, -44, -25, 25, 44, 9, -38, -38, 9, 44, 25, -25, -44, -9, 38);
            coeffs[7] = _mm256_set_epi16(-35, 21, 43, -4, -45, -13, 40, 29, -29, -40, 13, 45, 4, -43, -21, 35);

            src += 16 * 8;

#define CALCU_LINE(coeff0, dst) \
            v0 = _mm256_madd_epi16(s00, coeff0);          \
            v1 = _mm256_madd_epi16(s01, coeff0);          \
            v2 = _mm256_madd_epi16(s02, coeff0);          \
            v3 = _mm256_madd_epi16(s03, coeff0);          \
            v4 = _mm256_madd_epi16(s04, coeff0);          \
            v5 = _mm256_madd_epi16(s05, coeff0);          \
            v6 = _mm256_madd_epi16(s06, coeff0);          \
            v7 = _mm256_madd_epi16(s07, coeff0);          \
            v0 = _mm256_hadd_epi32(v0, v1);               \
            v2 = _mm256_hadd_epi32(v2, v3);               \
            v4 = _mm256_hadd_epi32(v4, v5);               \
            v6 = _mm256_hadd_epi32(v6, v7);               \
            v0 = _mm256_hadd_epi32(v0, v2);               \
            v4 = _mm256_hadd_epi32(v4, v6);               \
            v1 = _mm256_permute2x128_si256(v0, v4, 0x20); \
            v2 = _mm256_permute2x128_si256(v0, v4, 0x31); \
            dst = _mm256_add_epi32(v1, v2)

            CALCU_LINE(coeffs[0], d0);
            CALCU_LINE(coeffs[1], d1);
            CALCU_LINE(coeffs[2], d2);
            CALCU_LINE(coeffs[3], d3);
            CALCU_LINE(coeffs[4], d4);
            CALCU_LINE(coeffs[5], d5);
            CALCU_LINE(coeffs[6], d6);
            CALCU_LINE(coeffs[7], d7);

            d0 = _mm256_add_epi32(d0, add);
            d1 = _mm256_add_epi32(d1, add);
            d2 = _mm256_add_epi32(d2, add);
            d3 = _mm256_add_epi32(d3, add);
            d4 = _mm256_add_epi32(d4, add);
            d5 = _mm256_add_epi32(d5, add);
            d6 = _mm256_add_epi32(d6, add);
            d7 = _mm256_add_epi32(d7, add);

            d0 = _mm256_srai_epi32(d0, shift);
            d1 = _mm256_srai_epi32(d1, shift);
            d2 = _mm256_srai_epi32(d2, shift);
            d3 = _mm256_srai_epi32(d3, shift);
            d4 = _mm256_srai_epi32(d4, shift);
            d5 = _mm256_srai_epi32(d5, shift);
            d6 = _mm256_srai_epi32(d6, shift);
            d7 = _mm256_srai_epi32(d7, shift);

            d0 = _mm256_packs_epi32(d0, d1);
            d1 = _mm256_packs_epi32(d2, d3);
            d2 = _mm256_packs_epi32(d4, d5);
            d3 = _mm256_packs_epi32(d6, d7);
            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);
            d2 = _mm256_permute4x64_epi64(d2, 0xd8);
            d3 = _mm256_permute4x64_epi64(d3, 0xd8);

            coeffs[0] = _mm256_set_epi16(32, -32, -32, 32, 32, -32, -32, 32, 32, -32, -32, 32, 32, -32, -32, 32);
            coeffs[1] = _mm256_set_epi16(-29, 40, 13, -45, 4, 43, -21, -35, 35, 21, -43, -4, 45, -13, -40, 29);
            coeffs[2] = _mm256_set_epi16(25, -44, 9, 38, -38, -9, 44, -25, -25, 44, -9, -38, 38, 9, -44, 25);
            coeffs[3] = _mm256_set_epi16(-21, 45, -29, -13, 43, -35, -4, 40, -40, 4, 35, -43, 13, 29, -45, 21);
            coeffs[4] = _mm256_set_epi16(17, -42, 42, -17, -17, 42, -42, 17, 17, -42, 42, -17, -17, 42, -42, 17);
            coeffs[5] = _mm256_set_epi16(-13, 35, -45, 40, -21, -4, 29, -43, 43, -29, 4, 21, -40, 45, -35, 13);
            coeffs[6] = _mm256_set_epi16(9, -25, 38, -44, 44, -38, 25, -9, -9, 25, -38, 44, -44, 38, -25, 9);
            coeffs[7] = _mm256_set_epi16(-4, 13, -21, 29, -35, 40, -43, 45, -45, 43, -40, 35, -29, 21, -13, 4);

            _mm_store_si128((__m128i*)(dst), _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 1 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 2 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 3 * line), _mm256_extracti128_si256(d1, 1));
            _mm_store_si128((__m128i*)(dst + 4 * line), _mm256_castsi256_si128(d2));
            _mm_store_si128((__m128i*)(dst + 5 * line), _mm256_extracti128_si256(d2, 1));
            _mm_store_si128((__m128i*)(dst + 6 * line), _mm256_castsi256_si128(d3));
            _mm_store_si128((__m128i*)(dst + 7 * line), _mm256_extracti128_si256(d3, 1));

            CALCU_LINE(coeffs[0], d0);
            CALCU_LINE(coeffs[1], d1);
            CALCU_LINE(coeffs[2], d2);
            CALCU_LINE(coeffs[3], d3);
            CALCU_LINE(coeffs[4], d4);
            CALCU_LINE(coeffs[5], d5);
            CALCU_LINE(coeffs[6], d6);
            CALCU_LINE(coeffs[7], d7);
#undef CALCU_LINE

            d0 = _mm256_add_epi32(d0, add);
            d1 = _mm256_add_epi32(d1, add);
            d2 = _mm256_add_epi32(d2, add);
            d3 = _mm256_add_epi32(d3, add);
            d4 = _mm256_add_epi32(d4, add);
            d5 = _mm256_add_epi32(d5, add);
            d6 = _mm256_add_epi32(d6, add);
            d7 = _mm256_add_epi32(d7, add);

            d0 = _mm256_srai_epi32(d0, shift);
            d1 = _mm256_srai_epi32(d1, shift);
            d2 = _mm256_srai_epi32(d2, shift);
            d3 = _mm256_srai_epi32(d3, shift);
            d4 = _mm256_srai_epi32(d4, shift);
            d5 = _mm256_srai_epi32(d5, shift);
            d6 = _mm256_srai_epi32(d6, shift);
            d7 = _mm256_srai_epi32(d7, shift);

            d0 = _mm256_packs_epi32(d0, d1);
            d1 = _mm256_packs_epi32(d2, d3);
            d2 = _mm256_packs_epi32(d4, d5);
            d3 = _mm256_packs_epi32(d6, d7);
            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);
            d2 = _mm256_permute4x64_epi64(d2, 0xd8);
            d3 = _mm256_permute4x64_epi64(d3, 0xd8);

            _mm_store_si128((__m128i*)(dst + 8 * line), _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 9 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 10 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 11 * line), _mm256_extracti128_si256(d1, 1));
            _mm_store_si128((__m128i*)(dst + 12 * line), _mm256_castsi256_si128(d2));
            _mm_store_si128((__m128i*)(dst + 13 * line), _mm256_extracti128_si256(d2, 1));
            _mm_store_si128((__m128i*)(dst + 14 * line), _mm256_castsi256_si128(d3));
            _mm_store_si128((__m128i*)(dst + 15 * line), _mm256_extracti128_si256(d3, 1));

            dst += 8;
        }
    }
    else {//line=4	
        __m256i s00, s01, s02, s03;
        __m256i v0, v1, v2, v3, v4, v5, v6, v7;
        __m256i d0, d1, d2, d3;
        __m256i add = _mm256_set1_epi32(1 << (shift - 1));
        __m256i coeffs[8];
        s00 = _mm256_loadu_si256((__m256i*)(src));            // src[0][0-15]
        s01 = _mm256_loadu_si256((__m256i*)(src + 16));
        s02 = _mm256_loadu_si256((__m256i*)(src + 16 * 2));
        s03 = _mm256_loadu_si256((__m256i*)(src + 16 * 3));

        coeffs[0] = _mm256_set1_epi16(32);
        coeffs[1] = _mm256_set_epi16(-45, -43, -40, -35, -29, -21, -13, -4, 4, 13, 21, 29, 35, 40, 43, 45);
        coeffs[2] = _mm256_set_epi16(44, 38, 25, 9, -9, -25, -38, -44, -44, -38, -25, -9, 9, 25, 38, 44);
        coeffs[3] = _mm256_set_epi16(-43, -29, -4, 21, 40, 45, 35, 13, -13, -35, -45, -40, -21, 4, 29, 43);
        coeffs[4] = _mm256_set_epi16(42, 17, -17, -42, -42, -17, 17, 42, 42, 17, -17, -42, -42, -17, 17, 42);
        coeffs[5] = _mm256_set_epi16(-40, -4, 35, 43, 13, -29, -45, -21, 21, 45, 29, -13, -43, -35, 4, 40);
        coeffs[6] = _mm256_set_epi16(38, -9, -44, -25, 25, 44, 9, -38, -38, 9, 44, 25, -25, -44, -9, 38);
        coeffs[7] = _mm256_set_epi16(-35, 21, 43, -4, -45, -13, 40, 29, -29, -40, 13, 45, 4, -43, -21, 35);

        src += 16 * 8;

#define CALCU_LINE(coeff0, coeff1, dst) \
            v0 = _mm256_madd_epi16(s00, coeff0);          \
            v1 = _mm256_madd_epi16(s01, coeff0);          \
            v2 = _mm256_madd_epi16(s02, coeff0);          \
            v3 = _mm256_madd_epi16(s03, coeff0);          \
            v4 = _mm256_madd_epi16(s00, coeff1);          \
            v5 = _mm256_madd_epi16(s01, coeff1);          \
            v6 = _mm256_madd_epi16(s02, coeff1);          \
            v7 = _mm256_madd_epi16(s03, coeff1);          \
            v0 = _mm256_hadd_epi32(v0, v1);               \
            v2 = _mm256_hadd_epi32(v2, v3);               \
            v4 = _mm256_hadd_epi32(v4, v5);               \
            v6 = _mm256_hadd_epi32(v6, v7);               \
            v0 = _mm256_hadd_epi32(v0, v2);               \
            v4 = _mm256_hadd_epi32(v4, v6);               \
            v1 = _mm256_permute2x128_si256(v0, v4, 0x20); \
            v2 = _mm256_permute2x128_si256(v0, v4, 0x31); \
            dst = _mm256_add_epi32(v1, v2)

        CALCU_LINE(coeffs[0], coeffs[1], d0);
        CALCU_LINE(coeffs[2], coeffs[3], d1);
        CALCU_LINE(coeffs[4], coeffs[5], d2);
        CALCU_LINE(coeffs[6], coeffs[7], d3);

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);
        d2 = _mm256_add_epi32(d2, add);
        d3 = _mm256_add_epi32(d3, add);

        d0 = _mm256_srai_epi32(d0, shift);
        d1 = _mm256_srai_epi32(d1, shift);
        d2 = _mm256_srai_epi32(d2, shift);
        d3 = _mm256_srai_epi32(d3, shift);

        d0 = _mm256_packs_epi32(d0, d1);
        d1 = _mm256_packs_epi32(d2, d3);
        d0 = _mm256_permute4x64_epi64(d0, 0xd8);
        d1 = _mm256_permute4x64_epi64(d1, 0xd8);

        coeffs[0] = _mm256_set_epi16(32, -32, -32, 32, 32, -32, -32, 32, 32, -32, -32, 32, 32, -32, -32, 32);
        coeffs[1] = _mm256_set_epi16(-29, 40, 13, -45, 4, 43, -21, -35, 35, 21, -43, -4, 45, -13, -40, 29);
        coeffs[2] = _mm256_set_epi16(25, -44, 9, 38, -38, -9, 44, -25, -25, 44, -9, -38, 38, 9, -44, 25);
        coeffs[3] = _mm256_set_epi16(-21, 45, -29, -13, 43, -35, -4, 40, -40, 4, 35, -43, 13, 29, -45, 21);
        coeffs[4] = _mm256_set_epi16(17, -42, 42, -17, -17, 42, -42, 17, 17, -42, 42, -17, -17, 42, -42, 17);
        coeffs[5] = _mm256_set_epi16(-13, 35, -45, 40, -21, -4, 29, -43, 43, -29, 4, 21, -40, 45, -35, 13);
        coeffs[6] = _mm256_set_epi16(9, -25, 38, -44, 44, -38, 25, -9, -9, 25, -38, 44, -44, 38, -25, 9);
        coeffs[7] = _mm256_set_epi16(-4, 13, -21, 29, -35, 40, -43, 45, -45, 43, -40, 35, -29, 21, -13, 4);

        _mm256_storeu_si256((__m256i*)(dst), d0);
        _mm256_storeu_si256((__m256i*)(dst + 16), d1);

        CALCU_LINE(coeffs[0], coeffs[1], d0);
        CALCU_LINE(coeffs[2], coeffs[3], d1);
        CALCU_LINE(coeffs[4], coeffs[5], d2);
        CALCU_LINE(coeffs[6], coeffs[7], d3);
#undef CALCU_LINE

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);
        d2 = _mm256_add_epi32(d2, add);
        d3 = _mm256_add_epi32(d3, add);

        d0 = _mm256_srai_epi32(d0, shift);
        d1 = _mm256_srai_epi32(d1, shift);
        d2 = _mm256_srai_epi32(d2, shift);
        d3 = _mm256_srai_epi32(d3, shift);

        d0 = _mm256_packs_epi32(d0, d1);
        d1 = _mm256_packs_epi32(d2, d3);
        d0 = _mm256_permute4x64_epi64(d0, 0xd8);
        d1 = _mm256_permute4x64_epi64(d1, 0xd8);

        _mm256_storeu_si256((__m256i*)(dst + 32), d0);
        _mm256_storeu_si256((__m256i*)(dst + 48), d1);
    }
}

static void tx_dct2_pb32_1st_avx2(s16 *src, s16 *dst, int line, int limit_line, int shift)
{
    if (line > 4)
    {
        int i, j;
        __m256i s[16];
        __m256i t[8];
        __m256i tab0, tab1, tab2;
        __m256i e[8], o[8], ee[4], eo[4];
        __m256i eee[2], eeo[2];
        __m256i eeee[2], eeeo[2];
        __m256i v[18];
        __m256i d0, d1, d2, d3, d4, d5, d6, d7;
        __m256i add = _mm256_set1_epi32(1 << (shift - 1));
        __m256i coeffs[28];
        const __m256i coeff_p32_p32 = _mm256_set1_epi32(0x00200020); // EEEE
        const __m256i coeff_p32_n32 = _mm256_set1_epi32(0xFFE00020);
        const __m256i coeff_p42_p17 = _mm256_set1_epi32(0x0011002A); // EEEO
        const __m256i coeff_p17_n42 = _mm256_set1_epi32(0xFFD60011);

        tab0 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[0]);   // 16bit: 7-0, 7-0
        tab1 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[1]);   // 16bit: 0, 7, 1, 6, 2, 5, 3, 4, 0, 7, 1, 6, 2, 5, 3, 4
        tab2 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[2]);   // 16bit: 0, 3, 1, 2, 4, 7, 5, 6, 0, 3, 1, 2, 4, 7, 5, 6

        for (j = 0; j < limit_line; j += 8)
        {
            s[0] = _mm256_loadu_si256((__m256i*)(src));                     // src[0][0-15]
            s[1] = _mm256_loadu_si256((__m256i*)(src + 16 * 1));            // src[0][16-31]
            s[2] = _mm256_loadu_si256((__m256i*)(src + 16 * 2));
            s[3] = _mm256_loadu_si256((__m256i*)(src + 16 * 3));
            s[4] = _mm256_loadu_si256((__m256i*)(src + 16 * 4));
            s[5] = _mm256_loadu_si256((__m256i*)(src + 16 * 5));
            s[6] = _mm256_loadu_si256((__m256i*)(src + 16 * 6));
            s[7] = _mm256_loadu_si256((__m256i*)(src + 16 * 7));
            s[8] = _mm256_loadu_si256((__m256i*)(src + 16 * 8));            // src[4][0-15]
            s[9] = _mm256_loadu_si256((__m256i*)(src + 16 * 9));            // src[4][16-31]
            s[10] = _mm256_loadu_si256((__m256i*)(src + 16 * 10));
            s[11] = _mm256_loadu_si256((__m256i*)(src + 16 * 11));
            s[12] = _mm256_loadu_si256((__m256i*)(src + 16 * 12));
            s[13] = _mm256_loadu_si256((__m256i*)(src + 16 * 13));
            s[14] = _mm256_loadu_si256((__m256i*)(src + 16 * 14));
            s[15] = _mm256_loadu_si256((__m256i*)(src + 16 * 15));

            s[1 ] = _mm256_shuffle_epi8(s[1 ], tab0);                       // src[0][23-16], src[0][31-24]
            s[3 ] = _mm256_shuffle_epi8(s[3 ], tab0);                       // src[1][23-16], src[1][31-24]
            s[5 ] = _mm256_shuffle_epi8(s[5 ], tab0);
            s[7 ] = _mm256_shuffle_epi8(s[7 ], tab0);
            s[9 ] = _mm256_shuffle_epi8(s[9 ], tab0);
            s[11] = _mm256_shuffle_epi8(s[11], tab0);
            s[13] = _mm256_shuffle_epi8(s[13], tab0);
            s[15] = _mm256_shuffle_epi8(s[15], tab0);

            s[1 ] = _mm256_permute4x64_epi64(s[1 ], 0x4e);                  // src[0][31-16]
            s[3 ] = _mm256_permute4x64_epi64(s[3 ], 0x4e);                  // src[1][31-16]
            s[5 ] = _mm256_permute4x64_epi64(s[5 ], 0x4e);
            s[7 ] = _mm256_permute4x64_epi64(s[7 ], 0x4e);
            s[9 ] = _mm256_permute4x64_epi64(s[9 ], 0x4e);
            s[11] = _mm256_permute4x64_epi64(s[11], 0x4e);
            s[13] = _mm256_permute4x64_epi64(s[13], 0x4e);
            s[15] = _mm256_permute4x64_epi64(s[15], 0x4e);

            src += 32 * 8;

            e[0] = _mm256_add_epi16(s[0], s[1]);                            // e[0][0-16]
            o[0] = _mm256_sub_epi16(s[0], s[1]);                            // o[0][0-16]
            e[1] = _mm256_add_epi16(s[2], s[3]);                            // e[1][0-16]
            o[1] = _mm256_sub_epi16(s[2], s[3]);                            // o[1][0-16]
            e[2] = _mm256_add_epi16(s[4], s[5]);
            o[2] = _mm256_sub_epi16(s[4], s[5]);
            e[3] = _mm256_add_epi16(s[6], s[7]);
            o[3] = _mm256_sub_epi16(s[6], s[7]);
            e[4] = _mm256_add_epi16(s[8], s[9]);
            o[4] = _mm256_sub_epi16(s[8], s[9]);
            e[5] = _mm256_add_epi16(s[10], s[11]);
            o[5] = _mm256_sub_epi16(s[10], s[11]);
            e[6] = _mm256_add_epi16(s[12], s[13]);
            o[6] = _mm256_sub_epi16(s[12], s[13]);
            e[7] = _mm256_add_epi16(s[14], s[15]);
            o[7] = _mm256_sub_epi16(s[14], s[15]);

            t[0] = _mm256_permute2x128_si256(e[0], e[4], 0x20);             // e[0][0-7], e[4][0-7]
            t[1] = _mm256_permute2x128_si256(e[0], e[4], 0x31);             // e[0][8-15], e[4][8-15]
            t[2] = _mm256_permute2x128_si256(e[1], e[5], 0x20);             // e[1][0-7], e[5][0-7]
            t[3] = _mm256_permute2x128_si256(e[1], e[5], 0x31);             // e[1][8-15], e[5][8-15]
            t[4] = _mm256_permute2x128_si256(e[2], e[6], 0x20);
            t[5] = _mm256_permute2x128_si256(e[2], e[6], 0x31);
            t[6] = _mm256_permute2x128_si256(e[3], e[7], 0x20);
            t[7] = _mm256_permute2x128_si256(e[3], e[7], 0x31);

            t[1] = _mm256_shuffle_epi8(t[1], tab0);                         // e[0][15-8], e[4][15-8]
            t[3] = _mm256_shuffle_epi8(t[3], tab0);                         // e[1][15-8], e[5][15-8]
            t[5] = _mm256_shuffle_epi8(t[5], tab0);
            t[7] = _mm256_shuffle_epi8(t[7], tab0);

            ee[0] = _mm256_add_epi16(t[0], t[1]);                           // ee[0][0-7], ee[4][0-7]
            eo[0] = _mm256_sub_epi16(t[0], t[1]);                           // eo[0][0-7], eo[4][0-7]
            ee[1] = _mm256_add_epi16(t[2], t[3]);                           // ee[1][0-7], ee[5][0-7]
            eo[1] = _mm256_sub_epi16(t[2], t[3]);                           // eo[1][0-7], eo[5][0-7]
            ee[2] = _mm256_add_epi16(t[4], t[5]);
            eo[2] = _mm256_sub_epi16(t[4], t[5]);
            ee[3] = _mm256_add_epi16(t[6], t[7]);
            eo[3] = _mm256_sub_epi16(t[6], t[7]);

            ee[0] = _mm256_shuffle_epi8(ee[0], tab1);                       // ee[0][0, 7, 1, 6, 2, 5, 3, 4], ee[4][0, 7, 1, 6, 2, 5, 3, 4]
            ee[1] = _mm256_shuffle_epi8(ee[1], tab1);
            ee[2] = _mm256_shuffle_epi8(ee[2], tab1);
            ee[3] = _mm256_shuffle_epi8(ee[3], tab1);

            eee[0] = _mm256_hadd_epi16(ee[0], ee[1]);                       // eee[0][0-3], eee[1][0-3], eee[4][0-3], eee[5][0-3]
            eeo[0] = _mm256_hsub_epi16(ee[0], ee[1]);
            eee[1] = _mm256_hadd_epi16(ee[2], ee[3]);                       // eee[2][0-3], eee[3][0-3], eee[6][0-3], eee[7][0-3]
            eeo[1] = _mm256_hsub_epi16(ee[2], ee[3]);

            eee[0] = _mm256_shuffle_epi8(eee[0], tab2);                     // eee[0][0, 3, 1, 2], eee[1][0, 3, 1, 2], eee[4][0, 3, 1, 2], eee[5][0, 3, 1, 2]
            eee[1] = _mm256_shuffle_epi8(eee[1], tab2);

            eeee[0] = _mm256_hadd_epi16(eee[0], eee[1]);                    // eeee[0-7][0, 1]
            eeeo[0] = _mm256_hsub_epi16(eee[0], eee[1]);

            coeffs[0] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[0]);
            coeffs[1] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[1]);
            coeffs[2] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[2]);
            coeffs[3] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[3]);

            v[0] = _mm256_madd_epi16(eeee[0], coeff_p32_p32);               // dst[0][0-7]
            v[1] = _mm256_madd_epi16(eeee[0], coeff_p32_n32);               // dst[16][0-7]
            v[2] = _mm256_madd_epi16(eeeo[0], coeff_p42_p17);               // dst[8][0-7]
            v[3] = _mm256_madd_epi16(eeeo[0], coeff_p17_n42);               // dst[24][0-7]

            v[0] = _mm256_add_epi32(v[0], add);
            v[1] = _mm256_add_epi32(v[1], add);
            v[2] = _mm256_add_epi32(v[2], add);
            v[3] = _mm256_add_epi32(v[3], add);

            v[0] = _mm256_srai_epi32(v[0], shift);
            v[1] = _mm256_srai_epi32(v[1], shift);
            v[2] = _mm256_srai_epi32(v[2], shift);
            v[3] = _mm256_srai_epi32(v[3], shift);

            d0 = _mm256_packs_epi32(v[0], v[1]);
            d1 = _mm256_packs_epi32(v[2], v[3]);

            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);

            _mm_store_si128((__m128i*)dst, _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 8 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 16 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 24 * line), _mm256_extracti128_si256(d1, 1));

            v[0] = _mm256_madd_epi16(eeo[0], coeffs[0]);
            v[1] = _mm256_madd_epi16(eeo[1], coeffs[0]);
            v[2] = _mm256_madd_epi16(eeo[0], coeffs[1]);
            v[3] = _mm256_madd_epi16(eeo[1], coeffs[1]);
            v[4] = _mm256_madd_epi16(eeo[0], coeffs[2]);
            v[5] = _mm256_madd_epi16(eeo[1], coeffs[2]);
            v[6] = _mm256_madd_epi16(eeo[0], coeffs[3]);
            v[7] = _mm256_madd_epi16(eeo[1], coeffs[3]);

            v[0] = _mm256_hadd_epi32(v[0], v[1]);               // dst[2][0-7]
            v[2] = _mm256_hadd_epi32(v[2], v[3]);               // dst[6][0-7]
            v[4] = _mm256_hadd_epi32(v[4], v[5]);               // dst[10][0-7]
            v[6] = _mm256_hadd_epi32(v[6], v[7]);               // dst[14][0-7]

            v[0] = _mm256_add_epi32(v[0], add);
            v[2] = _mm256_add_epi32(v[2], add);
            v[4] = _mm256_add_epi32(v[4], add);
            v[6] = _mm256_add_epi32(v[6], add);

            v[0] = _mm256_srai_epi32(v[0], shift);
            v[2] = _mm256_srai_epi32(v[2], shift);
            v[4] = _mm256_srai_epi32(v[4], shift);
            v[6] = _mm256_srai_epi32(v[6], shift);

            for (i = 4; i < 12; i++)
                coeffs[i] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[i]);

            d0 = _mm256_packs_epi32(v[0], v[2]);
            d1 = _mm256_packs_epi32(v[4], v[6]);

            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);

            _mm_store_si128((__m128i*)(dst + 4 * line), _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 12 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 20 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 28 * line), _mm256_extracti128_si256(d1, 1));

#define CALCU_EO(coeff0, coeff1, d0, d1) \
            v[0] = _mm256_madd_epi16(eo[0], coeff0); \
            v[1] = _mm256_madd_epi16(eo[1], coeff0); \
            v[2] = _mm256_madd_epi16(eo[2], coeff0); \
            v[3] = _mm256_madd_epi16(eo[3], coeff0); \
            v[4] = _mm256_madd_epi16(eo[0], coeff1); \
            v[5] = _mm256_madd_epi16(eo[1], coeff1); \
            v[6] = _mm256_madd_epi16(eo[2], coeff1); \
            v[7] = _mm256_madd_epi16(eo[3], coeff1); \
            v[0] = _mm256_hadd_epi32(v[0], v[1]); \
            v[2] = _mm256_hadd_epi32(v[2], v[3]); \
            v[4] = _mm256_hadd_epi32(v[4], v[5]); \
            v[6] = _mm256_hadd_epi32(v[6], v[7]); \
            d0 = _mm256_hadd_epi32(v[0], v[2]); \
            d1 = _mm256_hadd_epi32(v[4], v[6])

            CALCU_EO(coeffs[4], coeffs[5], d0, d1);
            CALCU_EO(coeffs[6], coeffs[7], d2, d3);
            CALCU_EO(coeffs[8], coeffs[9], d4, d5);
            CALCU_EO(coeffs[10], coeffs[11], d6, d7);

#undef CALCU_EO

            for (i = 12; i < 20; i++)
                coeffs[i] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[i]);

            d0 = _mm256_add_epi32(d0, add);
            d1 = _mm256_add_epi32(d1, add);
            d2 = _mm256_add_epi32(d2, add);
            d3 = _mm256_add_epi32(d3, add);
            d4 = _mm256_add_epi32(d4, add);
            d5 = _mm256_add_epi32(d5, add);
            d6 = _mm256_add_epi32(d6, add);
            d7 = _mm256_add_epi32(d7, add);

            d0 = _mm256_srai_epi32(d0, shift);
            d1 = _mm256_srai_epi32(d1, shift);
            d2 = _mm256_srai_epi32(d2, shift);
            d3 = _mm256_srai_epi32(d3, shift);
            d4 = _mm256_srai_epi32(d4, shift);
            d5 = _mm256_srai_epi32(d5, shift);
            d6 = _mm256_srai_epi32(d6, shift);
            d7 = _mm256_srai_epi32(d7, shift);

            d0 = _mm256_packs_epi32(d0, d1);
            d1 = _mm256_packs_epi32(d2, d3);
            d2 = _mm256_packs_epi32(d4, d5);
            d3 = _mm256_packs_epi32(d6, d7);
            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);
            d2 = _mm256_permute4x64_epi64(d2, 0xd8);
            d3 = _mm256_permute4x64_epi64(d3, 0xd8);

            _mm_store_si128((__m128i*)(dst + 2 * line), _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 6 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 10 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 14 * line), _mm256_extracti128_si256(d1, 1));
            _mm_store_si128((__m128i*)(dst + 18 * line), _mm256_castsi256_si128(d2));
            _mm_store_si128((__m128i*)(dst + 22 * line), _mm256_extracti128_si256(d2, 1));
            _mm_store_si128((__m128i*)(dst + 26 * line), _mm256_castsi256_si128(d3));
            _mm_store_si128((__m128i*)(dst + 30 * line), _mm256_extracti128_si256(d3, 1));

#define CALCU_O(coeff, dst) \
            v[0 ] = _mm256_madd_epi16(o[0], coeff); \
            v[1 ] = _mm256_madd_epi16(o[1], coeff); \
            v[2 ] = _mm256_madd_epi16(o[2], coeff); \
            v[3 ] = _mm256_madd_epi16(o[3], coeff); \
            v[4 ] = _mm256_madd_epi16(o[4], coeff); \
            v[5 ] = _mm256_madd_epi16(o[5], coeff); \
            v[6 ] = _mm256_madd_epi16(o[6], coeff); \
            v[7 ] = _mm256_madd_epi16(o[7], coeff); \
            v[0 ] = _mm256_hadd_epi32(v[0], v[1]); \
            v[2 ] = _mm256_hadd_epi32(v[2], v[3]); \
            v[4 ] = _mm256_hadd_epi32(v[4], v[5]); \
            v[6 ] = _mm256_hadd_epi32(v[6], v[7]); \
            v[0 ] = _mm256_hadd_epi32(v[0], v[2]); \
            v[4 ] = _mm256_hadd_epi32(v[4], v[6]); \
            v[2 ] = _mm256_permute2x128_si256(v[0], v[4], 0x20); \
            v[3 ] = _mm256_permute2x128_si256(v[0], v[4], 0x31); \
            dst = _mm256_add_epi32(v[2], v[3])

            CALCU_O(coeffs[12], d0);
            CALCU_O(coeffs[13], d1);
            CALCU_O(coeffs[14], d2);
            CALCU_O(coeffs[15], d3);
            CALCU_O(coeffs[16], d4);
            CALCU_O(coeffs[17], d5);
            CALCU_O(coeffs[18], d6);
            CALCU_O(coeffs[19], d7);

            d0 = _mm256_add_epi32(d0, add);
            d1 = _mm256_add_epi32(d1, add);
            d2 = _mm256_add_epi32(d2, add);
            d3 = _mm256_add_epi32(d3, add);
            d4 = _mm256_add_epi32(d4, add);
            d5 = _mm256_add_epi32(d5, add);
            d6 = _mm256_add_epi32(d6, add);
            d7 = _mm256_add_epi32(d7, add);

            for (i = 20; i < 28; i++)
                coeffs[i] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[i]);

            d0 = _mm256_srai_epi32(d0, shift);
            d1 = _mm256_srai_epi32(d1, shift);
            d2 = _mm256_srai_epi32(d2, shift);
            d3 = _mm256_srai_epi32(d3, shift);
            d4 = _mm256_srai_epi32(d4, shift);
            d5 = _mm256_srai_epi32(d5, shift);
            d6 = _mm256_srai_epi32(d6, shift);
            d7 = _mm256_srai_epi32(d7, shift);

            d0 = _mm256_packs_epi32(d0, d1);
            d1 = _mm256_packs_epi32(d2, d3);
            d2 = _mm256_packs_epi32(d4, d5);
            d3 = _mm256_packs_epi32(d6, d7);
            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);
            d2 = _mm256_permute4x64_epi64(d2, 0xd8);
            d3 = _mm256_permute4x64_epi64(d3, 0xd8);

            _mm_store_si128((__m128i*)(dst + line), _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 3 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 5 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 7 * line), _mm256_extracti128_si256(d1, 1));
            _mm_store_si128((__m128i*)(dst + 9 * line), _mm256_castsi256_si128(d2));
            _mm_store_si128((__m128i*)(dst + 11 * line), _mm256_extracti128_si256(d2, 1));
            _mm_store_si128((__m128i*)(dst + 13 * line), _mm256_castsi256_si128(d3));
            _mm_store_si128((__m128i*)(dst + 15 * line), _mm256_extracti128_si256(d3, 1));

            CALCU_O(coeffs[20], d0);
            CALCU_O(coeffs[21], d1);
            CALCU_O(coeffs[22], d2);
            CALCU_O(coeffs[23], d3);
            CALCU_O(coeffs[24], d4);
            CALCU_O(coeffs[25], d5);
            CALCU_O(coeffs[26], d6);
            CALCU_O(coeffs[27], d7);

#undef CALCU_O

            d0 = _mm256_add_epi32(d0, add);
            d1 = _mm256_add_epi32(d1, add);
            d2 = _mm256_add_epi32(d2, add);
            d3 = _mm256_add_epi32(d3, add);
            d4 = _mm256_add_epi32(d4, add);
            d5 = _mm256_add_epi32(d5, add);
            d6 = _mm256_add_epi32(d6, add);
            d7 = _mm256_add_epi32(d7, add);

            d0 = _mm256_srai_epi32(d0, shift);
            d1 = _mm256_srai_epi32(d1, shift);
            d2 = _mm256_srai_epi32(d2, shift);
            d3 = _mm256_srai_epi32(d3, shift);
            d4 = _mm256_srai_epi32(d4, shift);
            d5 = _mm256_srai_epi32(d5, shift);
            d6 = _mm256_srai_epi32(d6, shift);
            d7 = _mm256_srai_epi32(d7, shift);

            d0 = _mm256_packs_epi32(d0, d1);
            d1 = _mm256_packs_epi32(d2, d3);
            d2 = _mm256_packs_epi32(d4, d5);
            d3 = _mm256_packs_epi32(d6, d7);
            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);
            d2 = _mm256_permute4x64_epi64(d2, 0xd8);
            d3 = _mm256_permute4x64_epi64(d3, 0xd8);

            _mm_store_si128((__m128i*)(dst + 17 * line), _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 19 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 21 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 23 * line), _mm256_extracti128_si256(d1, 1));
            _mm_store_si128((__m128i*)(dst + 25 * line), _mm256_castsi256_si128(d2));
            _mm_store_si128((__m128i*)(dst + 27 * line), _mm256_extracti128_si256(d2, 1));
            _mm_store_si128((__m128i*)(dst + 29 * line), _mm256_castsi256_si128(d3));
            _mm_store_si128((__m128i*)(dst + 31 * line), _mm256_extracti128_si256(d3, 1));

            dst += 8;
        }
    }
    else {//line=4	
        int i;
        __m256i s[16];
        __m256i t[8];
        __m256i tab0, tab1, tab2;
        __m256i e[8], o[8], ee[4], eo[4];
        __m256i eee, eeo;
        __m128i eeee, eeeo;
        __m256i v[8];
        __m256i d0, d1, d2, d3;
        __m256i add = _mm256_set1_epi32(1 << (shift - 1));
        __m256i coeffs[28];
        __m128i m0, m1, m2, m3, m4, m5, m6, m7;
        const __m128i coeff_p32_p32 = _mm_set1_epi32(0x00200020); // EEEE
        const __m128i coeff_p32_n32 = _mm_set1_epi32(0xFFE00020);
        const __m128i coeff_p42_p17 = _mm_set1_epi32(0x0011002A); // EEEO
        const __m128i coeff_p17_n42 = _mm_set1_epi32(0xFFD60011);

        tab0 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[0]);  // 16bit: 7-0, 7-0
        tab1 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[1]);  // 16bit: 0, 7, 1, 6, 2, 5, 3, 4, 0, 7, 1, 6, 2, 5, 3, 4
        tab2 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[2]);  // 16bit: 0, 3, 1, 2, 4, 7, 5, 6, 0, 3, 1, 2, 4, 7, 5, 6

        s[0] = _mm256_loadu_si256((__m256i*)(src));                   // src[0][0-15]
        s[1] = _mm256_loadu_si256((__m256i*)(src + 16 * 1));          // src[0][16-31]
        s[2] = _mm256_loadu_si256((__m256i*)(src + 16 * 2));
        s[3] = _mm256_loadu_si256((__m256i*)(src + 16 * 3));
        s[4] = _mm256_loadu_si256((__m256i*)(src + 16 * 4));
        s[5] = _mm256_loadu_si256((__m256i*)(src + 16 * 5));
        s[6] = _mm256_loadu_si256((__m256i*)(src + 16 * 6));
        s[7] = _mm256_loadu_si256((__m256i*)(src + 16 * 7));

        s[1] = _mm256_shuffle_epi8(s[1], tab0);                      // src[0][23-16], src[0][31-24]
        s[3] = _mm256_shuffle_epi8(s[3], tab0);                      // src[1][23-16], src[1][31-24]
        s[5] = _mm256_shuffle_epi8(s[5], tab0);
        s[7] = _mm256_shuffle_epi8(s[7], tab0);

        s[1] = _mm256_permute4x64_epi64(s[1], 0x4e);                 // src[0][31-16]
        s[3] = _mm256_permute4x64_epi64(s[3], 0x4e);                 // src[1][31-16]
        s[5] = _mm256_permute4x64_epi64(s[5], 0x4e);
        s[7] = _mm256_permute4x64_epi64(s[7], 0x4e);

        e[0] = _mm256_add_epi16(s[0], s[1]);                        // e[0][0-15]
        o[0] = _mm256_sub_epi16(s[0], s[1]);                        // o[0][0-15]
        e[1] = _mm256_add_epi16(s[2], s[3]);                        // e[1][0-15]
        o[1] = _mm256_sub_epi16(s[2], s[3]);                        // o[1][0-15]
        e[2] = _mm256_add_epi16(s[4], s[5]);                        // e[2][0-15]
        o[2] = _mm256_sub_epi16(s[4], s[5]);                        // o[2][0-15]
        e[3] = _mm256_add_epi16(s[6], s[7]);                        // e[3][0-15]
        o[3] = _mm256_sub_epi16(s[6], s[7]);                        // o[3][0-15]

        t[0] = _mm256_permute2x128_si256(e[0], e[2], 0x20);         // e[0][0-7], e[2][0-7]
        t[1] = _mm256_permute2x128_si256(e[0], e[2], 0x31);         // e[0][8-15], e[2][8-15]
        t[2] = _mm256_permute2x128_si256(e[1], e[3], 0x20);         // e[1][0-7], e[3][0-7]
        t[3] = _mm256_permute2x128_si256(e[1], e[3], 0x31);         // e[1][8-15], e[3][8-15]

        t[1] = _mm256_shuffle_epi8(t[1], tab0);                     // e[0][15-8], e[2][15-8]
        t[3] = _mm256_shuffle_epi8(t[3], tab0);                     // e[1][15-8], e[3][15-8]

        ee[0] = _mm256_add_epi16(t[0], t[1]);                       // ee[0][0-7], ee[2][0-7]
        eo[0] = _mm256_sub_epi16(t[0], t[1]);                       // eo[0][0-7], eo[2][0-7]
        ee[1] = _mm256_add_epi16(t[2], t[3]);                       // ee[1][0-7], ee[3][0-7]
        eo[1] = _mm256_sub_epi16(t[2], t[3]);                       // eo[1][0-7], eo[3][0-7]

        ee[0] = _mm256_shuffle_epi8(ee[0], tab1);                   // ee[0][0, 7, 1, 6, 2, 5, 3, 4], ee[2][0, 7, 1, 6, 2, 5, 3, 4]
        ee[1] = _mm256_shuffle_epi8(ee[1], tab1);                   // ee[1][0, 7, 1, 6, 2, 5, 3, 4], ee[3][0, 7, 1, 6, 2, 5, 3, 4]

        eee = _mm256_hadd_epi16(ee[0], ee[1]);                      // eee[0][0-3], eee[1][0-3], eee[2][0-3], eee[3][0-3]
        eeo = _mm256_hsub_epi16(ee[0], ee[1]);                      // eeo[0][0-3], eeo[1][0-3], eeo[2][0-3], eeo[3][0-3]

        eee = _mm256_shuffle_epi8(eee, tab2);                       // eee[0-3][0, 3, 1, 2]

        m0 = _mm256_castsi256_si128(eee);
        m1 = _mm256_extracti128_si256(eee, 1);
        eeee = _mm_hadd_epi16(m0, m1);                              // eeee[0][0, 1], eeee[1][0, 1], eeee[2][0, 1], eeee[3][0, 1]
        eeeo = _mm_hsub_epi16(m0, m1);

        m0 = _mm_madd_epi16(eeee, coeff_p32_p32);                   // dst[0][0-3]
        m1 = _mm_madd_epi16(eeee, coeff_p32_n32);                   // dst[16][0-3]
        m2 = _mm_madd_epi16(eeeo, coeff_p42_p17);                   // dst[8][0-3]
        m3 = _mm_madd_epi16(eeeo, coeff_p17_n42);                   // dst[24][0-3]

        v[0] = _mm256_set_m128i(m1, m0);                            // dst[0][0-4], dst[16][0-4]
        v[2] = _mm256_set_m128i(m3, m2);                            // dst[8][0-4], dst[24][0-4]

        v[0] = _mm256_add_epi32(v[0], add);
        v[2] = _mm256_add_epi32(v[2], add);

        v[0] = _mm256_srai_epi32(v[0], shift);
        v[2] = _mm256_srai_epi32(v[2], shift);

        d0 = _mm256_packs_epi32(v[0], v[2]);                        // dst[0][0-4], dst[8][0-4], dst[16][0-4], dst[24][0-4]

        coeffs[0] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[0]);
        coeffs[1] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[1]);
        coeffs[2] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[2]);
        coeffs[3] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[3]);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        _mm_storel_epi64((__m128i*)(dst), m0);
        _mm_storel_epi64((__m128i*)(dst + 8 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 16 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 24 * line), m3);

        v[0] = _mm256_madd_epi16(eeo, coeffs[0]);
        v[1] = _mm256_madd_epi16(eeo, coeffs[1]);
        v[2] = _mm256_madd_epi16(eeo, coeffs[2]);
        v[3] = _mm256_madd_epi16(eeo, coeffs[3]);
        
        v[0] = _mm256_hadd_epi32(v[0], v[1]);                       // dst[4][0-1], dst[12][0-1], dst[4][2-3], dst[12][2-3]
        v[2] = _mm256_hadd_epi32(v[2], v[3]);                       // dst[20][0-1], dst[28][0-1], dst[20][2-3], dst[28][2-3]

        v[0] = _mm256_add_epi32(v[0], add);
        v[2] = _mm256_add_epi32(v[2], add);

        v[0] = _mm256_srai_epi32(v[0], shift);
        v[2] = _mm256_srai_epi32(v[2], shift);

        v[0] = _mm256_permute4x64_epi64(v[0], 0xd8);                // dst[4][0-3], dst[12][0-3]
        v[2] = _mm256_permute4x64_epi64(v[2], 0xd8);

        d0 = _mm256_packs_epi32(v[0], v[2]);                        // dst[4][0-3], dst[20][0-3], dst[12][0-3], dst[28][0-3]

        for (i = 4; i < 12; i++)
            coeffs[i] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[i]);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);

        _mm_storel_epi64((__m128i*)(dst + 4 * line), m0);
        _mm_storel_epi64((__m128i*)(dst + 12 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 20 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 28 * line), m3);

#define CALCU_EO(coeff0, coeff1, d0) \
        v[0] = _mm256_madd_epi16(eo[0], coeff0); \
        v[1] = _mm256_madd_epi16(eo[1], coeff0); \
        v[2] = _mm256_madd_epi16(eo[0], coeff1); \
        v[3] = _mm256_madd_epi16(eo[1], coeff1); \
        v[0] = _mm256_hadd_epi32(v[0], v[1]); \
        v[2] = _mm256_hadd_epi32(v[2], v[3]); \
        d0 = _mm256_hadd_epi32(v[0], v[2])

        CALCU_EO(coeffs[4], coeffs[5], d0);
        CALCU_EO(coeffs[6], coeffs[7], d1);
        CALCU_EO(coeffs[8], coeffs[9], d2);
        CALCU_EO(coeffs[10], coeffs[11], d3);

        d0 = _mm256_permute4x64_epi64(d0, 0xd8);                    // dst[2][0-4], dst[6][0-4]
        d1 = _mm256_permute4x64_epi64(d1, 0xd8);
        d2 = _mm256_permute4x64_epi64(d2, 0xd8);
        d3 = _mm256_permute4x64_epi64(d3, 0xd8);

#undef CALCU_EO

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);
        d2 = _mm256_add_epi32(d2, add);
        d3 = _mm256_add_epi32(d3, add);

        d0 = _mm256_srai_epi32(d0, shift);
        d1 = _mm256_srai_epi32(d1, shift);
        d2 = _mm256_srai_epi32(d2, shift);
        d3 = _mm256_srai_epi32(d3, shift);

        d0 = _mm256_packs_epi32(d0, d1);
        d1 = _mm256_packs_epi32(d2, d3);

        for (i = 12; i < 28; i++)
            coeffs[i] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[i]);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        m4 = _mm256_castsi256_si128(d1);
        m5 = _mm256_extracti128_si256(d1, 1);
        m6 = _mm_srli_si128(m4, 8);
        m7 = _mm_srli_si128(m5, 8);

        _mm_storel_epi64((__m128i*)(dst + 2 * line), m0);
        _mm_storel_epi64((__m128i*)(dst + 6 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 10 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 14 * line), m3);
        _mm_storel_epi64((__m128i*)(dst + 18 * line), m4);
        _mm_storel_epi64((__m128i*)(dst + 22 * line), m5);
        _mm_storel_epi64((__m128i*)(dst + 26 * line), m6);
        _mm_storel_epi64((__m128i*)(dst + 30 * line), m7);

#define CALCU_O(coeff0, coeff1, dst) \
        v[0] = _mm256_madd_epi16(o[0], coeff0); \
        v[1] = _mm256_madd_epi16(o[1], coeff0); \
        v[2] = _mm256_madd_epi16(o[2], coeff0); \
        v[3] = _mm256_madd_epi16(o[3], coeff0); \
        v[4] = _mm256_madd_epi16(o[0], coeff1); \
        v[5] = _mm256_madd_epi16(o[1], coeff1); \
        v[6] = _mm256_madd_epi16(o[2], coeff1); \
        v[7] = _mm256_madd_epi16(o[3], coeff1); \
        v[0] = _mm256_hadd_epi32(v[0], v[1]); \
        v[2] = _mm256_hadd_epi32(v[2], v[3]); \
        v[4] = _mm256_hadd_epi32(v[4], v[5]); \
        v[6] = _mm256_hadd_epi32(v[6], v[7]); \
        v[0] = _mm256_hadd_epi32(v[0], v[2]); \
        v[4] = _mm256_hadd_epi32(v[4], v[6]); \
        v[2] = _mm256_permute2x128_si256(v[0], v[4], 0x20); \
        v[3] = _mm256_permute2x128_si256(v[0], v[4], 0x31); \
        dst = _mm256_add_epi32(v[2], v[3]);                        // dst[0][0-3], dst[1][0-3]

        CALCU_O(coeffs[12], coeffs[13], d0);
        CALCU_O(coeffs[14], coeffs[15], d1);
        CALCU_O(coeffs[16], coeffs[17], d2);
        CALCU_O(coeffs[18], coeffs[19], d3);

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);
        d2 = _mm256_add_epi32(d2, add);
        d3 = _mm256_add_epi32(d3, add);

        d0 = _mm256_srai_epi32(d0, shift);                          // dst[1][0-3], dst[3][0-3]
        d1 = _mm256_srai_epi32(d1, shift);                          // dst[5][0-3], dst[7][0-3]
        d2 = _mm256_srai_epi32(d2, shift);
        d3 = _mm256_srai_epi32(d3, shift);

        d0 = _mm256_packs_epi32(d0, d1);
        d1 = _mm256_packs_epi32(d2, d3);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        m4 = _mm256_castsi256_si128(d1);
        m5 = _mm256_extracti128_si256(d1, 1);
        m6 = _mm_srli_si128(m4, 8);
        m7 = _mm_srli_si128(m5, 8);

        _mm_storel_epi64((__m128i*)(dst + 1 * line), m0);
        _mm_storel_epi64((__m128i*)(dst + 3 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 5 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 7 * line), m3);
        _mm_storel_epi64((__m128i*)(dst + 9 * line), m4);
        _mm_storel_epi64((__m128i*)(dst + 11 * line), m5);
        _mm_storel_epi64((__m128i*)(dst + 13 * line), m6);
        _mm_storel_epi64((__m128i*)(dst + 15 * line), m7);

        CALCU_O(coeffs[20], coeffs[21], d0);
        CALCU_O(coeffs[22], coeffs[23], d1);
        CALCU_O(coeffs[24], coeffs[25], d2);
        CALCU_O(coeffs[26], coeffs[27], d3);

#undef CALCU_O

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);
        d2 = _mm256_add_epi32(d2, add);
        d3 = _mm256_add_epi32(d3, add);

        d0 = _mm256_srai_epi32(d0, shift);                          // dst[17][0-3], dst[19][0-3]
        d1 = _mm256_srai_epi32(d1, shift);                          // dst[21][0-3], dst[23][0-3]
        d2 = _mm256_srai_epi32(d2, shift);
        d3 = _mm256_srai_epi32(d3, shift);

        d0 = _mm256_packs_epi32(d0, d1);
        d1 = _mm256_packs_epi32(d2, d3);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        m4 = _mm256_castsi256_si128(d1);
        m5 = _mm256_extracti128_si256(d1, 1);
        m6 = _mm_srli_si128(m4, 8);
        m7 = _mm_srli_si128(m5, 8);

        _mm_storel_epi64((__m128i*)(dst + 17 * line), m0);
        _mm_storel_epi64((__m128i*)(dst + 19 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 21 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 23 * line), m3);
        _mm_storel_epi64((__m128i*)(dst + 25 * line), m4);
        _mm_storel_epi64((__m128i*)(dst + 27 * line), m5);
        _mm_storel_epi64((__m128i*)(dst + 29 * line), m6);
        _mm_storel_epi64((__m128i*)(dst + 31 * line), m7);
    }
}

static void tx_dct2_pb32_2nd_avx2(s16 *src, s16 *dst, int line, int limit_line, int shift)
{
    if (line > 4)
    {
        int i, j;
        __m256i s[32];
        __m256i t[16];
        __m256i tab0, tab1, tab2;
        __m256i e[16], o[16], ee[8], eo[8];
        __m256i eee[4], eeo[4];
        __m256i eeee[2], eeeo[2];
        __m256i v[18];
        __m256i d0, d1, d2, d3, d4, d5, d6, d7;
        __m256i add = _mm256_set1_epi32(1 << (shift - 1));
        __m256i coeffs[52];
        const __m256i coeff_p32_p32 = _mm256_set1_epi64x(0x0000002000000020); // EEEE
        const __m256i coeff_p32_n32 = _mm256_set1_epi64x(0xFFFFFFE000000020);
        const __m256i coeff_p42_p17 = _mm256_set1_epi64x(0x000000110000002A); // EEEO
        const __m256i coeff_p17_n42 = _mm256_set1_epi64x(0xFFFFFFD600000011);

        tab0 = _mm256_loadu_si256((__m256i*)tab_dct2_2nd_shuffle_256i[3]);  // 16bit: 7-0, 7-0
        tab1 = _mm256_loadu_si256((__m256i*)tab_dct2_2nd_shuffle_256i[1]);  // 32bit: 3-0, 3-0
        tab2 = _mm256_loadu_si256((__m256i*)tab_dct2_2nd_shuffle_256i[2]);  // 32bit: 0, 3, 1, 2, 0, 3, 1, 2

        for (j = 0; j < limit_line; j += 8)
        {
            s[ 0] = _mm256_loadu_si256((__m256i*)(src));                    // src[0][0-15]
            s[ 1] = _mm256_loadu_si256((__m256i*)(src + 16 *  1));          // src[0][16-31]
            s[ 2] = _mm256_loadu_si256((__m256i*)(src + 16 *  2));
            s[ 3] = _mm256_loadu_si256((__m256i*)(src + 16 *  3));
            s[ 4] = _mm256_loadu_si256((__m256i*)(src + 16 *  4));
            s[ 5] = _mm256_loadu_si256((__m256i*)(src + 16 *  5));
            s[ 6] = _mm256_loadu_si256((__m256i*)(src + 16 *  6));
            s[ 7] = _mm256_loadu_si256((__m256i*)(src + 16 *  7));
            s[ 8] = _mm256_loadu_si256((__m256i*)(src + 16 *  8));          // src[4][0-15]
            s[ 9] = _mm256_loadu_si256((__m256i*)(src + 16 *  9));          // src[4][16-31]
            s[10] = _mm256_loadu_si256((__m256i*)(src + 16 * 10));
            s[11] = _mm256_loadu_si256((__m256i*)(src + 16 * 11));
            s[12] = _mm256_loadu_si256((__m256i*)(src + 16 * 12));
            s[13] = _mm256_loadu_si256((__m256i*)(src + 16 * 13));
            s[14] = _mm256_loadu_si256((__m256i*)(src + 16 * 14));
            s[15] = _mm256_loadu_si256((__m256i*)(src + 16 * 15));

            t[0] = _mm256_shuffle_epi8(s[ 1], tab0);                        // src[0][23-16], src[0][31-24]
            t[1] = _mm256_shuffle_epi8(s[ 3], tab0);                        // src[1][23-16], src[1][31-24]
            t[2] = _mm256_shuffle_epi8(s[ 5], tab0);
            t[3] = _mm256_shuffle_epi8(s[ 7], tab0);
            t[4] = _mm256_shuffle_epi8(s[ 9], tab0);
            t[5] = _mm256_shuffle_epi8(s[11], tab0);
            t[6] = _mm256_shuffle_epi8(s[13], tab0);
            t[7] = _mm256_shuffle_epi8(s[15], tab0);

            src += 32 * 8;

            s[ 1] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[0], 1));   // src[0][7-15]
            s[ 0] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[0]));        // src[0][0-7]
            s[ 3] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[2], 1));   // src[1][7-15]
            s[ 2] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[2]));        // src[1][0-7]
            s[ 5] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[4], 1));
            s[ 4] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[4]));
            s[ 7] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[6], 1));
            s[ 6] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[6]));
            s[ 9] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[8], 1)); 
            s[ 8] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[8]));     
            s[11] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[10], 1));  
            s[10] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[10]));    
            s[13] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[12], 1));  
            s[12] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[12]));
            s[15] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[14], 1));
            s[14] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[14]));

            s[16] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[0], 1));   // src[0][31-24]
            s[17] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[0]));        // src[0][23-16]
            s[18] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[1], 1));   // src[1][31-24]
            s[19] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[1]));        // src[1][23-16]
            s[20] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[2], 1));
            s[21] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[2]));
            s[22] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[3], 1));
            s[23] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[3]));
            s[24] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[4], 1)); 
            s[25] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[4]));    
            s[26] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[5], 1));   
            s[27] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[5]));  
            s[28] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[6], 1));    
            s[29] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[6]));
            s[30] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[7], 1));
            s[31] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[7]));

            e[ 0] = _mm256_add_epi32(s[ 0], s[16]);                         // e[0][0-7]
            o[ 0] = _mm256_sub_epi32(s[ 0], s[16]);                         // o[0][0-7]
            e[ 1] = _mm256_add_epi32(s[ 1], s[17]);                         // e[0][8-15]
            o[ 1] = _mm256_sub_epi32(s[ 1], s[17]);                         // o[0][8-15]
            e[ 2] = _mm256_add_epi32(s[ 2], s[18]);                         // e[1][0-7]
            o[ 2] = _mm256_sub_epi32(s[ 2], s[18]);                         // o[1][0-7]
            e[ 3] = _mm256_add_epi32(s[ 3], s[19]);                         // e[1][8-15]
            o[ 3] = _mm256_sub_epi32(s[ 3], s[19]);                         // o[1][8-15]
            e[ 4] = _mm256_add_epi32(s[ 4], s[20]);
            o[ 4] = _mm256_sub_epi32(s[ 4], s[20]);
            e[ 5] = _mm256_add_epi32(s[ 5], s[21]);
            o[ 5] = _mm256_sub_epi32(s[ 5], s[21]);
            e[ 6] = _mm256_add_epi32(s[ 6], s[22]);
            o[ 6] = _mm256_sub_epi32(s[ 6], s[22]);
            e[ 7] = _mm256_add_epi32(s[ 7], s[23]);
            o[ 7] = _mm256_sub_epi32(s[ 7], s[23]);
            e[ 8] = _mm256_add_epi32(s[ 8], s[24]);                         // e[4][0-7]
            o[ 8] = _mm256_sub_epi32(s[ 8], s[24]);
            e[ 9] = _mm256_add_epi32(s[ 9], s[25]);                         // e[4][8-15]
            o[ 9] = _mm256_sub_epi32(s[ 9], s[25]);
            e[10] = _mm256_add_epi32(s[10], s[26]);
            o[10] = _mm256_sub_epi32(s[10], s[26]);
            e[11] = _mm256_add_epi32(s[11], s[27]);
            o[11] = _mm256_sub_epi32(s[11], s[27]);
            e[12] = _mm256_add_epi32(s[12], s[28]);
            o[12] = _mm256_sub_epi32(s[12], s[28]);
            e[13] = _mm256_add_epi32(s[13], s[29]);
            o[13] = _mm256_sub_epi32(s[13], s[29]);
            e[14] = _mm256_add_epi32(s[14], s[30]);
            o[14] = _mm256_sub_epi32(s[14], s[30]);
            e[15] = _mm256_add_epi32(s[15], s[31]);
            o[15] = _mm256_sub_epi32(s[15], s[31]);

            t[ 0] = _mm256_permute2x128_si256(e[ 0], e[ 8], 0x20);          // e[0][0-3], e[4][0-3]
            t[ 1] = _mm256_permute2x128_si256(e[ 0], e[ 8], 0x31);          // e[0][4-7], e[4][4-7]
            t[ 2] = _mm256_permute2x128_si256(e[ 1], e[ 9], 0x20);          // e[0][8-11], e[4][8-11]
            t[ 3] = _mm256_permute2x128_si256(e[ 1], e[ 9], 0x31);          // e[0][12-15], e[4][12-15]
            t[ 4] = _mm256_permute2x128_si256(e[ 2], e[10], 0x20);
            t[ 5] = _mm256_permute2x128_si256(e[ 2], e[10], 0x31);
            t[ 6] = _mm256_permute2x128_si256(e[ 3], e[11], 0x20);
            t[ 7] = _mm256_permute2x128_si256(e[ 3], e[11], 0x31);
            t[ 8] = _mm256_permute2x128_si256(e[ 4], e[12], 0x20);
            t[ 9] = _mm256_permute2x128_si256(e[ 4], e[12], 0x31);
            t[10] = _mm256_permute2x128_si256(e[ 5], e[13], 0x20);
            t[11] = _mm256_permute2x128_si256(e[ 5], e[13], 0x31);
            t[12] = _mm256_permute2x128_si256(e[ 6], e[14], 0x20);
            t[13] = _mm256_permute2x128_si256(e[ 6], e[14], 0x31);
            t[14] = _mm256_permute2x128_si256(e[ 7], e[15], 0x20);
            t[15] = _mm256_permute2x128_si256(e[ 7], e[15], 0x31);

            t[ 2] = _mm256_shuffle_epi8(t[ 2], tab1);                       // e[0][11-8], e[4][11-8]
            t[ 3] = _mm256_shuffle_epi8(t[ 3], tab1);                       // e[0][15-12], e[4][15-12]
            t[ 6] = _mm256_shuffle_epi8(t[ 6], tab1);
            t[ 7] = _mm256_shuffle_epi8(t[ 7], tab1);
            t[10] = _mm256_shuffle_epi8(t[10], tab1);                       // e[2][11-8], e[6][11-8]
            t[11] = _mm256_shuffle_epi8(t[11], tab1);
            t[14] = _mm256_shuffle_epi8(t[14], tab1);
            t[15] = _mm256_shuffle_epi8(t[15], tab1);

            ee[0] = _mm256_add_epi32(t[ 0], t[ 3]);                         // ee[0][0-3], ee[4][0-3]
            eo[0] = _mm256_sub_epi32(t[ 0], t[ 3]);                         // eo[0][0-3], eo[4][0-3]
            ee[1] = _mm256_add_epi32(t[ 1], t[ 2]);                         // ee[0][4-7], ee[4][4-7]
            eo[1] = _mm256_sub_epi32(t[ 1], t[ 2]);                         // eo[0][4-7], eo[4][4-7]
            ee[2] = _mm256_add_epi32(t[ 4], t[ 7]);
            eo[2] = _mm256_sub_epi32(t[ 4], t[ 7]);
            ee[3] = _mm256_add_epi32(t[ 5], t[ 6]);
            eo[3] = _mm256_sub_epi32(t[ 5], t[ 6]);
            ee[4] = _mm256_add_epi32(t[ 8], t[11]);
            eo[4] = _mm256_sub_epi32(t[ 8], t[11]);
            ee[5] = _mm256_add_epi32(t[ 9], t[10]);
            eo[5] = _mm256_sub_epi32(t[ 9], t[10]);
            ee[6] = _mm256_add_epi32(t[12], t[15]);
            eo[6] = _mm256_sub_epi32(t[12], t[15]);
            ee[7] = _mm256_add_epi32(t[13], t[14]);
            eo[7] = _mm256_sub_epi32(t[13], t[14]);

            ee[1] = _mm256_shuffle_epi8(ee[1], tab1);                       // ee[0][7-4], ee[4][7-4]
            ee[3] = _mm256_shuffle_epi8(ee[3], tab1);
            ee[5] = _mm256_shuffle_epi8(ee[5], tab1);
            ee[7] = _mm256_shuffle_epi8(ee[7], tab1);

            eee[0] = _mm256_add_epi32(ee[0], ee[1]);                        // eee[0][0-3], eee[4][0-3]
            eeo[0] = _mm256_sub_epi32(ee[0], ee[1]);                        // eeo[0][0-3], eeo[4][0-3]
            eee[1] = _mm256_add_epi32(ee[2], ee[3]);
            eeo[1] = _mm256_sub_epi32(ee[2], ee[3]);
            eee[2] = _mm256_add_epi32(ee[4], ee[5]);
            eeo[2] = _mm256_sub_epi32(ee[4], ee[5]);
            eee[3] = _mm256_add_epi32(ee[6], ee[7]);
            eeo[3] = _mm256_sub_epi32(ee[6], ee[7]);

            eee[0] = _mm256_shuffle_epi8(eee[0], tab2);                     // eee[0][0, 3, 1, 2], eee[4][0, 3, 1, 2]
            eee[1] = _mm256_shuffle_epi8(eee[1], tab2);
            eee[2] = _mm256_shuffle_epi8(eee[2], tab2);
            eee[3] = _mm256_shuffle_epi8(eee[3], tab2);

            eeee[0] = _mm256_hadd_epi32(eee[0], eee[1]);                    // eeee[0][0, 1], eeee[1][0, 1], eeee[4][0, 1], eeee[5][0, 1]
            eeeo[0] = _mm256_hsub_epi32(eee[0], eee[1]);
            eeee[1] = _mm256_hadd_epi32(eee[2], eee[3]);                    // eeee[2][0, 1], eeee[3][0, 1], eeee[6][0, 1], eeee[7][0, 1]
            eeeo[1] = _mm256_hsub_epi32(eee[2], eee[3]);

            coeffs[0] = _mm256_loadu_si256((__m256i*)tab_dct2_pb32_coeffs_256i[0]);
            coeffs[1] = _mm256_loadu_si256((__m256i*)tab_dct2_pb32_coeffs_256i[1]);
            coeffs[2] = _mm256_loadu_si256((__m256i*)tab_dct2_pb32_coeffs_256i[2]);
            coeffs[3] = _mm256_loadu_si256((__m256i*)tab_dct2_pb32_coeffs_256i[3]);

            v[0] = _mm256_mullo_epi32(eeee[0], coeff_p32_p32);
            v[1] = _mm256_mullo_epi32(eeee[1], coeff_p32_p32);
            v[2] = _mm256_mullo_epi32(eeee[0], coeff_p32_n32);
            v[3] = _mm256_mullo_epi32(eeee[1], coeff_p32_n32);
            v[4] = _mm256_mullo_epi32(eeeo[0], coeff_p42_p17);
            v[5] = _mm256_mullo_epi32(eeeo[1], coeff_p42_p17);
            v[6] = _mm256_mullo_epi32(eeeo[0], coeff_p17_n42);
            v[7] = _mm256_mullo_epi32(eeeo[1], coeff_p17_n42);

            v[0] = _mm256_hadd_epi32(v[0], v[1]);                           // dst[0][0-7]
            v[2] = _mm256_hadd_epi32(v[2], v[3]);                           // dst[16][0-7]
            v[4] = _mm256_hadd_epi32(v[4], v[5]);                           // dst[8][0-7]
            v[6] = _mm256_hadd_epi32(v[6], v[7]);                           // dst[24][0-7]

            v[0] = _mm256_add_epi32(v[0], add);
            v[2] = _mm256_add_epi32(v[2], add);
            v[4] = _mm256_add_epi32(v[4], add);
            v[6] = _mm256_add_epi32(v[6], add);

            v[0] = _mm256_srai_epi32(v[0], shift);
            v[2] = _mm256_srai_epi32(v[2], shift);
            v[4] = _mm256_srai_epi32(v[4], shift);
            v[6] = _mm256_srai_epi32(v[6], shift);

            d0 = _mm256_packs_epi32(v[0], v[2]);
            d1 = _mm256_packs_epi32(v[4], v[6]);

            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);

            _mm_store_si128((__m128i*)dst, _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 8 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 16 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 24 * line), _mm256_extracti128_si256(d1, 1));

            v[0] = _mm256_mullo_epi32(eeo[0], coeffs[0]);
            v[1] = _mm256_mullo_epi32(eeo[1], coeffs[0]);
            v[2] = _mm256_mullo_epi32(eeo[2], coeffs[0]);
            v[3] = _mm256_mullo_epi32(eeo[3], coeffs[0]);
            v[4] = _mm256_mullo_epi32(eeo[0], coeffs[1]);
            v[5] = _mm256_mullo_epi32(eeo[1], coeffs[1]);
            v[6] = _mm256_mullo_epi32(eeo[2], coeffs[1]);
            v[7] = _mm256_mullo_epi32(eeo[3], coeffs[1]);

            v[0] = _mm256_hadd_epi32(v[0], v[1]);
            v[2] = _mm256_hadd_epi32(v[2], v[3]);
            v[4] = _mm256_hadd_epi32(v[4], v[5]);
            v[6] = _mm256_hadd_epi32(v[6], v[7]);
            v[8] = _mm256_hadd_epi32(v[0], v[2]);               // dst[2][0-7]
            v[9] = _mm256_hadd_epi32(v[4], v[6]);               // dst[6][0-7]

            v[0] = _mm256_mullo_epi32(eeo[0], coeffs[2]);
            v[1] = _mm256_mullo_epi32(eeo[1], coeffs[2]);
            v[2] = _mm256_mullo_epi32(eeo[2], coeffs[2]);
            v[3] = _mm256_mullo_epi32(eeo[3], coeffs[2]);
            v[4] = _mm256_mullo_epi32(eeo[0], coeffs[3]);
            v[5] = _mm256_mullo_epi32(eeo[1], coeffs[3]);
            v[6] = _mm256_mullo_epi32(eeo[2], coeffs[3]);
            v[7] = _mm256_mullo_epi32(eeo[3], coeffs[3]);

            v[0] = _mm256_hadd_epi32(v[0], v[1]);
            v[2] = _mm256_hadd_epi32(v[2], v[3]);
            v[4] = _mm256_hadd_epi32(v[4], v[5]);
            v[6] = _mm256_hadd_epi32(v[6], v[7]);
            v[0] = _mm256_hadd_epi32(v[0], v[2]);               // dst[10][0-7]
            v[1] = _mm256_hadd_epi32(v[4], v[6]);               // dst[14][0-7]

            v[2] = _mm256_add_epi32(v[8], add);
            v[3] = _mm256_add_epi32(v[9], add);
            v[4] = _mm256_add_epi32(v[0], add);
            v[5] = _mm256_add_epi32(v[1], add);

            v[2] = _mm256_srai_epi32(v[2], shift);
            v[3] = _mm256_srai_epi32(v[3], shift);
            v[4] = _mm256_srai_epi32(v[4], shift);
            v[5] = _mm256_srai_epi32(v[5], shift);

            for (i = 4; i < 20; i++)
                coeffs[i] = _mm256_loadu_si256((__m256i*)tab_dct2_pb32_coeffs_256i[i]);

            d0 = _mm256_packs_epi32(v[2], v[3]);
            d1 = _mm256_packs_epi32(v[4], v[5]);

            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);

            _mm_store_si128((__m128i*)(dst + 4 * line), _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 12 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 20 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 28 * line), _mm256_extracti128_si256(d1, 1));

#define CALCU_EO(coeff0, coeff1, dst) \
            v[0] = _mm256_mullo_epi32(eo[0], coeff0); \
            v[1] = _mm256_mullo_epi32(eo[1], coeff1); \
            v[2] = _mm256_mullo_epi32(eo[2], coeff0); \
            v[3] = _mm256_mullo_epi32(eo[3], coeff1); \
            v[4] = _mm256_mullo_epi32(eo[4], coeff0); \
            v[5] = _mm256_mullo_epi32(eo[5], coeff1); \
            v[6] = _mm256_mullo_epi32(eo[6], coeff0); \
            v[7] = _mm256_mullo_epi32(eo[7], coeff1); \
            v[0] = _mm256_hadd_epi32(v[0], v[1]); \
            v[2] = _mm256_hadd_epi32(v[2], v[3]); \
            v[4] = _mm256_hadd_epi32(v[4], v[5]); \
            v[6] = _mm256_hadd_epi32(v[6], v[7]); \
            v[0] = _mm256_hadd_epi32(v[0], v[2]); \
            v[4] = _mm256_hadd_epi32(v[4], v[6]); \
            dst = _mm256_hadd_epi32(v[0], v[4])

            CALCU_EO(coeffs[4], coeffs[5], d0);
            CALCU_EO(coeffs[6], coeffs[7], d1);
            CALCU_EO(coeffs[8], coeffs[9], d2);
            CALCU_EO(coeffs[10], coeffs[11], d3);
            CALCU_EO(coeffs[12], coeffs[13], d4);
            CALCU_EO(coeffs[14], coeffs[15], d5);
            CALCU_EO(coeffs[16], coeffs[17], d6);
            CALCU_EO(coeffs[18], coeffs[19], d7);

#undef CALCU_EO
            for (i = 20; i < 36; i++)
                coeffs[i] = _mm256_loadu_si256((__m256i*)tab_dct2_pb32_coeffs_256i[i]);

            d0 = _mm256_add_epi32(d0, add);
            d1 = _mm256_add_epi32(d1, add);
            d2 = _mm256_add_epi32(d2, add);
            d3 = _mm256_add_epi32(d3, add);
            d4 = _mm256_add_epi32(d4, add);
            d5 = _mm256_add_epi32(d5, add);
            d6 = _mm256_add_epi32(d6, add);
            d7 = _mm256_add_epi32(d7, add);

            d0 = _mm256_srai_epi32(d0, shift);
            d1 = _mm256_srai_epi32(d1, shift);
            d2 = _mm256_srai_epi32(d2, shift);
            d3 = _mm256_srai_epi32(d3, shift);
            d4 = _mm256_srai_epi32(d4, shift);
            d5 = _mm256_srai_epi32(d5, shift);
            d6 = _mm256_srai_epi32(d6, shift);
            d7 = _mm256_srai_epi32(d7, shift);

            d0 = _mm256_packs_epi32(d0, d1);
            d1 = _mm256_packs_epi32(d2, d3);
            d2 = _mm256_packs_epi32(d4, d5);
            d3 = _mm256_packs_epi32(d6, d7);
            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);
            d2 = _mm256_permute4x64_epi64(d2, 0xd8);
            d3 = _mm256_permute4x64_epi64(d3, 0xd8);

            _mm_store_si128((__m128i*)(dst + 2 * line), _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 6 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 10 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 14 * line), _mm256_extracti128_si256(d1, 1));
            _mm_store_si128((__m128i*)(dst + 18 * line), _mm256_castsi256_si128(d2));
            _mm_store_si128((__m128i*)(dst + 22 * line), _mm256_extracti128_si256(d2, 1));
            _mm_store_si128((__m128i*)(dst + 26 * line), _mm256_castsi256_si128(d3));
            _mm_store_si128((__m128i*)(dst + 30 * line), _mm256_extracti128_si256(d3, 1));

#define CALCU_O(coeff0, coeff1, dst) \
            v[0 ] = _mm256_mullo_epi32(o[0], coeff0); \
            v[1 ] = _mm256_mullo_epi32(o[1], coeff1); \
            v[2 ] = _mm256_mullo_epi32(o[2], coeff0); \
            v[3 ] = _mm256_mullo_epi32(o[3], coeff1); \
            v[4 ] = _mm256_mullo_epi32(o[4], coeff0); \
            v[5 ] = _mm256_mullo_epi32(o[5], coeff1); \
            v[6 ] = _mm256_mullo_epi32(o[6], coeff0); \
            v[7 ] = _mm256_mullo_epi32(o[7], coeff1); \
            v[8 ] = _mm256_mullo_epi32(o[8], coeff0); \
            v[9 ] = _mm256_mullo_epi32(o[9], coeff1); \
            v[10] = _mm256_mullo_epi32(o[10], coeff0); \
            v[11] = _mm256_mullo_epi32(o[11], coeff1); \
            v[12] = _mm256_mullo_epi32(o[12], coeff0); \
            v[13] = _mm256_mullo_epi32(o[13], coeff1); \
            v[14] = _mm256_mullo_epi32(o[14], coeff0); \
            v[15] = _mm256_mullo_epi32(o[15], coeff1); \
            v[0 ] = _mm256_add_epi32(v[0], v[1]); \
            v[2 ] = _mm256_add_epi32(v[2], v[3]); \
            v[4 ] = _mm256_add_epi32(v[4], v[5]); \
            v[6 ] = _mm256_add_epi32(v[6], v[7]); \
            v[8 ] = _mm256_add_epi32(v[8 ], v[9 ]); \
            v[10] = _mm256_add_epi32(v[10], v[11]); \
            v[12] = _mm256_add_epi32(v[12], v[13]); \
            v[14] = _mm256_add_epi32(v[14], v[15]); \
            v[0 ] = _mm256_hadd_epi32(v[0], v[2]); \
            v[4 ] = _mm256_hadd_epi32(v[4], v[6]); \
            v[8 ] = _mm256_hadd_epi32(v[8], v[10]); \
            v[12] = _mm256_hadd_epi32(v[12], v[14]); \
            v[0 ] = _mm256_hadd_epi32(v[0], v[4]); \
            v[8 ] = _mm256_hadd_epi32(v[8], v[12]); \
            v[2 ] = _mm256_permute2x128_si256(v[0], v[8], 0x20); \
            v[3 ] = _mm256_permute2x128_si256(v[0], v[8], 0x31); \
            dst = _mm256_add_epi32(v[2], v[3])

            CALCU_O(coeffs[20], coeffs[21], d0);
            CALCU_O(coeffs[22], coeffs[23], d1);
            CALCU_O(coeffs[24], coeffs[25], d2);
            CALCU_O(coeffs[26], coeffs[27], d3);
            CALCU_O(coeffs[28], coeffs[29], d4);
            CALCU_O(coeffs[30], coeffs[31], d5);
            CALCU_O(coeffs[32], coeffs[33], d6);
            CALCU_O(coeffs[34], coeffs[35], d7);

            d0 = _mm256_add_epi32(d0, add);
            d1 = _mm256_add_epi32(d1, add);
            d2 = _mm256_add_epi32(d2, add);
            d3 = _mm256_add_epi32(d3, add);
            d4 = _mm256_add_epi32(d4, add);
            d5 = _mm256_add_epi32(d5, add);
            d6 = _mm256_add_epi32(d6, add);
            d7 = _mm256_add_epi32(d7, add);

            for (i = 36; i < 52; i++)
                coeffs[i] = _mm256_loadu_si256((__m256i*)tab_dct2_pb32_coeffs_256i[i]);

            d0 = _mm256_srai_epi32(d0, shift);
            d1 = _mm256_srai_epi32(d1, shift);
            d2 = _mm256_srai_epi32(d2, shift);
            d3 = _mm256_srai_epi32(d3, shift);
            d4 = _mm256_srai_epi32(d4, shift);
            d5 = _mm256_srai_epi32(d5, shift);
            d6 = _mm256_srai_epi32(d6, shift);
            d7 = _mm256_srai_epi32(d7, shift);

            d0 = _mm256_packs_epi32(d0, d1);
            d1 = _mm256_packs_epi32(d2, d3);
            d2 = _mm256_packs_epi32(d4, d5);
            d3 = _mm256_packs_epi32(d6, d7);
            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);
            d2 = _mm256_permute4x64_epi64(d2, 0xd8);
            d3 = _mm256_permute4x64_epi64(d3, 0xd8);

            _mm_store_si128((__m128i*)(dst + line), _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 3 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 5 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 7 * line), _mm256_extracti128_si256(d1, 1));
            _mm_store_si128((__m128i*)(dst + 9 * line), _mm256_castsi256_si128(d2));
            _mm_store_si128((__m128i*)(dst + 11 * line), _mm256_extracti128_si256(d2, 1));
            _mm_store_si128((__m128i*)(dst + 13 * line), _mm256_castsi256_si128(d3));
            _mm_store_si128((__m128i*)(dst + 15 * line), _mm256_extracti128_si256(d3, 1));

            CALCU_O(coeffs[36], coeffs[37], d0);
            CALCU_O(coeffs[38], coeffs[39], d1);
            CALCU_O(coeffs[40], coeffs[41], d2);
            CALCU_O(coeffs[42], coeffs[43], d3);
            CALCU_O(coeffs[44], coeffs[45], d4);
            CALCU_O(coeffs[46], coeffs[47], d5);
            CALCU_O(coeffs[48], coeffs[49], d6);
            CALCU_O(coeffs[50], coeffs[51], d7);

#undef CALCU_O

            d0 = _mm256_add_epi32(d0, add);
            d1 = _mm256_add_epi32(d1, add);
            d2 = _mm256_add_epi32(d2, add);
            d3 = _mm256_add_epi32(d3, add);
            d4 = _mm256_add_epi32(d4, add);
            d5 = _mm256_add_epi32(d5, add);
            d6 = _mm256_add_epi32(d6, add);
            d7 = _mm256_add_epi32(d7, add);

            d0 = _mm256_srai_epi32(d0, shift);
            d1 = _mm256_srai_epi32(d1, shift);
            d2 = _mm256_srai_epi32(d2, shift);
            d3 = _mm256_srai_epi32(d3, shift);
            d4 = _mm256_srai_epi32(d4, shift);
            d5 = _mm256_srai_epi32(d5, shift);
            d6 = _mm256_srai_epi32(d6, shift);
            d7 = _mm256_srai_epi32(d7, shift);

            d0 = _mm256_packs_epi32(d0, d1);
            d1 = _mm256_packs_epi32(d2, d3);
            d2 = _mm256_packs_epi32(d4, d5);
            d3 = _mm256_packs_epi32(d6, d7);
            d0 = _mm256_permute4x64_epi64(d0, 0xd8);
            d1 = _mm256_permute4x64_epi64(d1, 0xd8);
            d2 = _mm256_permute4x64_epi64(d2, 0xd8);
            d3 = _mm256_permute4x64_epi64(d3, 0xd8);

            _mm_store_si128((__m128i*)(dst + 17 * line), _mm256_castsi256_si128(d0));
            _mm_store_si128((__m128i*)(dst + 19 * line), _mm256_extracti128_si256(d0, 1));
            _mm_store_si128((__m128i*)(dst + 21 * line), _mm256_castsi256_si128(d1));
            _mm_store_si128((__m128i*)(dst + 23 * line), _mm256_extracti128_si256(d1, 1));
            _mm_store_si128((__m128i*)(dst + 25 * line), _mm256_castsi256_si128(d2));
            _mm_store_si128((__m128i*)(dst + 27 * line), _mm256_extracti128_si256(d2, 1));
            _mm_store_si128((__m128i*)(dst + 29 * line), _mm256_castsi256_si128(d3));
            _mm_store_si128((__m128i*)(dst + 31 * line), _mm256_extracti128_si256(d3, 1));

            dst += 8;
        }
    }
    else {//line=4	
        int i;
        __m256i s[16];
        __m256i t[8];
        __m256i tab0, tab1, tab2;
        __m256i e[8], o[8], ee[4], eo[4];
        __m256i eee[2], eeo[2];
        __m256i eeee, eeeo;
        __m256i v[8];
        __m256i d0, d1, d2, d3, d4, d5, d6, d7;
        __m256i add = _mm256_set1_epi32(1 << (shift - 1));
        __m256i coeffs[52];
        __m128i m0, m1, m2, m3, m4, m5, m6, m7;
        const __m256i coeff_p32_p32 = _mm256_set1_epi64x(0x0000002000000020); // EEEE
        const __m256i coeff_p32_n32 = _mm256_set1_epi64x(0xFFFFFFE000000020);
        const __m256i coeff_p42_p17 = _mm256_set1_epi64x(0x000000110000002A); // EEEO
        const __m256i coeff_p17_n42 = _mm256_set1_epi64x(0xFFFFFFD600000011);

        tab0 = _mm256_loadu_si256((__m256i*)tab_dct2_2nd_shuffle_256i[3]);  // 16bit: 7-0, 7-0
        tab1 = _mm256_loadu_si256((__m256i*)tab_dct2_2nd_shuffle_256i[1]);  // 32bit: 3-0, 3-0
        tab2 = _mm256_loadu_si256((__m256i*)tab_dct2_2nd_shuffle_256i[2]);  // 32bit: 0, 3, 1, 2, 0, 3, 1, 2

        s[0] = _mm256_loadu_si256((__m256i*)(src));                     // src[0][0-15]
        s[1] = _mm256_loadu_si256((__m256i*)(src + 16 * 1));            // src[0][16-31]
        s[2] = _mm256_loadu_si256((__m256i*)(src + 16 * 2));
        s[3] = _mm256_loadu_si256((__m256i*)(src + 16 * 3));
        s[4] = _mm256_loadu_si256((__m256i*)(src + 16 * 4));
        s[5] = _mm256_loadu_si256((__m256i*)(src + 16 * 5));
        s[6] = _mm256_loadu_si256((__m256i*)(src + 16 * 6));
        s[7] = _mm256_loadu_si256((__m256i*)(src + 16 * 7));

        t[0] = _mm256_shuffle_epi8(s[1], tab0);                        // src[0][23-16], src[0][31-24]
        t[1] = _mm256_shuffle_epi8(s[3], tab0);                        // src[1][23-16], src[1][31-24]
        t[2] = _mm256_shuffle_epi8(s[5], tab0);
        t[3] = _mm256_shuffle_epi8(s[7], tab0);

        s[1] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[0], 1));   // src[0][7-15]
        s[0] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[0]));        // src[0][0-7]
        s[3] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[2], 1));   // src[1][7-15]
        s[2] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[2]));        // src[1][0-7]
        s[5] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[4], 1));
        s[4] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[4]));
        s[7] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[6], 1));
        s[6] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[6]));

        s[ 8] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[0], 1));   // src[0][31-24]
        s[ 9] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[0]));        // src[0][23-16]
        s[10] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[1], 1));   // src[1][31-24]
        s[11] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[1]));        // src[1][23-16]
        s[12] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[2], 1));
        s[13] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[2]));
        s[14] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[3], 1));
        s[15] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[3]));

        e[0] = _mm256_add_epi32(s[0], s[8]);                         // e[0][0-7]
        o[0] = _mm256_sub_epi32(s[0], s[8]);                         // o[0][0-7]
        e[1] = _mm256_add_epi32(s[1], s[9]);                         // e[0][8-15]
        o[1] = _mm256_sub_epi32(s[1], s[9]);                         // o[0][8-15]
        e[2] = _mm256_add_epi32(s[2], s[10]);                         // e[1][0-7]
        o[2] = _mm256_sub_epi32(s[2], s[10]);                         // o[1][0-7]
        e[3] = _mm256_add_epi32(s[3], s[11]);                         // e[1][8-15]
        o[3] = _mm256_sub_epi32(s[3], s[11]);                         // o[1][8-15]
        e[4] = _mm256_add_epi32(s[4], s[12]);
        o[4] = _mm256_sub_epi32(s[4], s[12]);
        e[5] = _mm256_add_epi32(s[5], s[13]);
        o[5] = _mm256_sub_epi32(s[5], s[13]);
        e[6] = _mm256_add_epi32(s[6], s[14]);
        o[6] = _mm256_sub_epi32(s[6], s[14]);
        e[7] = _mm256_add_epi32(s[7], s[15]);
        o[7] = _mm256_sub_epi32(s[7], s[15]);

        t[0] = _mm256_permute2x128_si256(e[0], e[4], 0x20);          // e[0][0-3], e[2][0-3]
        t[1] = _mm256_permute2x128_si256(e[0], e[4], 0x31);          // e[0][4-7], e[2][4-7]
        t[2] = _mm256_permute2x128_si256(e[1], e[5], 0x20);          // e[0][8-11], e[2][8-11]
        t[3] = _mm256_permute2x128_si256(e[1], e[5], 0x31);          // e[0][12-15], e[2][12-15]
        t[4] = _mm256_permute2x128_si256(e[2], e[6], 0x20);
        t[5] = _mm256_permute2x128_si256(e[2], e[6], 0x31);
        t[6] = _mm256_permute2x128_si256(e[3], e[7], 0x20);
        t[7] = _mm256_permute2x128_si256(e[3], e[7], 0x31);

        t[2] = _mm256_shuffle_epi8(t[2], tab1);                     // e[0][11-8], e[2][11-8]
        t[3] = _mm256_shuffle_epi8(t[3], tab1);                     // e[0][15-12], e[2][15-12]
        t[6] = _mm256_shuffle_epi8(t[6], tab1);
        t[7] = _mm256_shuffle_epi8(t[7], tab1);

        ee[0] = _mm256_add_epi32(t[0], t[3]);                       // ee[0][0-3], ee[2][0-3]
        eo[0] = _mm256_sub_epi32(t[0], t[3]);                       // eo[0][0-3], eo[2][0-3]
        ee[1] = _mm256_add_epi32(t[1], t[2]);                       // ee[0][4-7], ee[2][4-7]
        eo[1] = _mm256_sub_epi32(t[1], t[2]);                       // eo[0][4-7], eo[2][4-7]
        ee[2] = _mm256_add_epi32(t[4], t[7]);
        eo[2] = _mm256_sub_epi32(t[4], t[7]);
        ee[3] = _mm256_add_epi32(t[5], t[6]);
        eo[3] = _mm256_sub_epi32(t[5], t[6]);

        ee[1] = _mm256_shuffle_epi8(ee[1], tab1);                   // ee[0][7-4], ee[2][7-4]
        ee[3] = _mm256_shuffle_epi8(ee[3], tab1);                   // ee[1][7-4], ee[3][7-4]

        eee[0] = _mm256_add_epi32(ee[0], ee[1]);                    // eee[0][0-3], eee[2][0-3]
        eeo[0] = _mm256_sub_epi32(ee[0], ee[1]);                    // eeo[0][0-3], eeo[2][0-3]
        eee[1] = _mm256_add_epi32(ee[2], ee[3]);
        eeo[1] = _mm256_sub_epi32(ee[2], ee[3]);

        eee[0] = _mm256_shuffle_epi8(eee[0], tab2);                 // eee[0][0, 3, 1, 2], eee[2][0, 3, 1, 2]
        eee[1] = _mm256_shuffle_epi8(eee[1], tab2);

        eeee = _mm256_hadd_epi32(eee[0], eee[1]);                // eeee[0][0, 1], eeee[1][0, 1], eeee[2][0, 1], eeee[3][0, 1]
        eeeo = _mm256_hsub_epi32(eee[0], eee[1]);

        v[0] = _mm256_mullo_epi32(eeee, coeff_p32_p32);
        v[1] = _mm256_mullo_epi32(eeee, coeff_p32_n32);
        v[2] = _mm256_mullo_epi32(eeeo, coeff_p42_p17);
        v[3] = _mm256_mullo_epi32(eeeo, coeff_p17_n42);

        v[0] = _mm256_hadd_epi32(v[0], v[1]);                       // dst[0][0-1], dst[16][0-1], dst[0][2-3], dst[16][2-3]
        v[2] = _mm256_hadd_epi32(v[2], v[3]);                       // dst[8][0-1], dst[24][0-1], dst[8][2-3], dst[24][2-3]

        v[0] = _mm256_permute4x64_epi64(v[0], 0xd8);                // dst[0][0-4], dst[16][0-4]
        v[2] = _mm256_permute4x64_epi64(v[2], 0xd8);                // dst[8][0-4], dst[24][0-4]

        v[0] = _mm256_add_epi32(v[0], add);
        v[2] = _mm256_add_epi32(v[2], add);

        v[0] = _mm256_srai_epi32(v[0], shift);
        v[2] = _mm256_srai_epi32(v[2], shift);

        d0 = _mm256_packs_epi32(v[0], v[2]);                        // dst[0][0-4], dst[8][0-4], dst[16][0-4], dst[24][0-4]

        coeffs[0] = _mm256_loadu_si256((__m256i*)tab_dct2_pb32_coeffs_256i[0]);
        coeffs[1] = _mm256_loadu_si256((__m256i*)tab_dct2_pb32_coeffs_256i[1]);
        coeffs[2] = _mm256_loadu_si256((__m256i*)tab_dct2_pb32_coeffs_256i[2]);
        coeffs[3] = _mm256_loadu_si256((__m256i*)tab_dct2_pb32_coeffs_256i[3]);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        _mm_storel_epi64((__m128i*)(dst), m0);
        _mm_storel_epi64((__m128i*)(dst + 8 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 16 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 24 * line), m3);

        v[0] = _mm256_mullo_epi32(eeo[0], coeffs[0]);
        v[1] = _mm256_mullo_epi32(eeo[1], coeffs[0]);
        v[2] = _mm256_mullo_epi32(eeo[0], coeffs[1]);
        v[3] = _mm256_mullo_epi32(eeo[1], coeffs[1]);

        v[0] = _mm256_hadd_epi32(v[0], v[1]);
        v[2] = _mm256_hadd_epi32(v[2], v[3]);
        v[4] = _mm256_hadd_epi32(v[0], v[2]);                       // dst[4][0-1], dst[12][0-1], dst[4][2-3], dst[12][2-3]

        v[0] = _mm256_mullo_epi32(eeo[0], coeffs[2]);
        v[1] = _mm256_mullo_epi32(eeo[1], coeffs[2]);
        v[2] = _mm256_mullo_epi32(eeo[0], coeffs[3]);
        v[3] = _mm256_mullo_epi32(eeo[1], coeffs[3]);

        v[0] = _mm256_hadd_epi32(v[0], v[1]);
        v[2] = _mm256_hadd_epi32(v[2], v[3]);
        v[0] = _mm256_hadd_epi32(v[0], v[2]);                       // dst[20][0-1], dst[28][0-1], dst[20][2-3], dst[28][2-3]

        v[2] = _mm256_add_epi32(v[4], add);
        v[3] = _mm256_add_epi32(v[0], add);

        v[0] = _mm256_srai_epi32(v[2], shift);
        v[1] = _mm256_srai_epi32(v[3], shift);

        v[0] = _mm256_permute4x64_epi64(v[0], 0xd8);                // dst[4][0-3], dst[12][0-3]
        v[1] = _mm256_permute4x64_epi64(v[1], 0xd8);

        d0 = _mm256_packs_epi32(v[0], v[1]);                        // dst[4][0-3], dst[20][0-3], dst[12][0-3], dst[28][0-3]

        for (i = 4; i < 20; i++)
            coeffs[i] = _mm256_loadu_si256((__m256i*)tab_dct2_pb32_coeffs_256i[i]);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);

        _mm_storel_epi64((__m128i*)(dst + 4 * line), m0);
        _mm_storel_epi64((__m128i*)(dst + 12 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 20 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 28 * line), m3);

#define CALCU_EO(coeff0, coeff1, dst) \
        v[0] = _mm256_mullo_epi32(eo[0], coeff0); \
        v[1] = _mm256_mullo_epi32(eo[1], coeff1); \
        v[2] = _mm256_mullo_epi32(eo[2], coeff0); \
        v[3] = _mm256_mullo_epi32(eo[3], coeff1); \
        v[0] = _mm256_hadd_epi32(v[0], v[1]); \
        v[2] = _mm256_hadd_epi32(v[2], v[3]); \
        dst = _mm256_hadd_epi32(v[0], v[2])

        CALCU_EO(coeffs[4], coeffs[5], d0);
        CALCU_EO(coeffs[6], coeffs[7], d1);
        CALCU_EO(coeffs[8], coeffs[9], d2);
        CALCU_EO(coeffs[10], coeffs[11], d3);
        CALCU_EO(coeffs[12], coeffs[13], d4);
        CALCU_EO(coeffs[14], coeffs[15], d5);
        CALCU_EO(coeffs[16], coeffs[17], d6);
        CALCU_EO(coeffs[18], coeffs[19], d7);

        d0 = _mm256_hadd_epi32(d0, d1);
        d2 = _mm256_hadd_epi32(d2, d3);
        d4 = _mm256_hadd_epi32(d4, d5);
        d6 = _mm256_hadd_epi32(d6, d7);

        d0 = _mm256_permute4x64_epi64(d0, 0xd8);                    // dst[2][0-4], dst[6][0-4]
        d1 = _mm256_permute4x64_epi64(d2, 0xd8);
        d2 = _mm256_permute4x64_epi64(d4, 0xd8);
        d3 = _mm256_permute4x64_epi64(d6, 0xd8);

#undef CALCU_EO

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);
        d2 = _mm256_add_epi32(d2, add);
        d3 = _mm256_add_epi32(d3, add);

        d0 = _mm256_srai_epi32(d0, shift);
        d1 = _mm256_srai_epi32(d1, shift);
        d2 = _mm256_srai_epi32(d2, shift);
        d3 = _mm256_srai_epi32(d3, shift);

        d0 = _mm256_packs_epi32(d0, d1);
        d1 = _mm256_packs_epi32(d2, d3);

        for (i = 20; i < 36; i++)
            coeffs[i] = _mm256_loadu_si256((__m256i*)tab_dct2_pb32_coeffs_256i[i]);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        m4 = _mm256_castsi256_si128(d1);
        m5 = _mm256_extracti128_si256(d1, 1);
        m6 = _mm_srli_si128(m4, 8);
        m7 = _mm_srli_si128(m5, 8);

        _mm_storel_epi64((__m128i*)(dst + 2 * line), m0);
        _mm_storel_epi64((__m128i*)(dst + 6 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 10 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 14 * line), m3);
        _mm_storel_epi64((__m128i*)(dst + 18 * line), m4);
        _mm_storel_epi64((__m128i*)(dst + 22 * line), m5);
        _mm_storel_epi64((__m128i*)(dst + 26 * line), m6);
        _mm_storel_epi64((__m128i*)(dst + 30 * line), m7);

#define CALCU_O(coeff0, coeff1, dst) \
        v[0] = _mm256_mullo_epi32(o[0], coeff0); \
        v[1] = _mm256_mullo_epi32(o[1], coeff1); \
        v[2] = _mm256_mullo_epi32(o[2], coeff0); \
        v[3] = _mm256_mullo_epi32(o[3], coeff1); \
        v[4] = _mm256_mullo_epi32(o[4], coeff0); \
        v[5] = _mm256_mullo_epi32(o[5], coeff1); \
        v[6] = _mm256_mullo_epi32(o[6], coeff0); \
        v[7] = _mm256_mullo_epi32(o[7], coeff1); \
        v[0] = _mm256_add_epi32(v[0], v[1]); \
        v[2] = _mm256_add_epi32(v[2], v[3]); \
        v[4] = _mm256_add_epi32(v[4], v[5]); \
        v[6] = _mm256_add_epi32(v[6], v[7]); \
        v[0] = _mm256_hadd_epi32(v[0], v[2]); \
        v[4] = _mm256_hadd_epi32(v[4], v[6]); \
        dst = _mm256_hadd_epi32(v[0], v[4]);                        // a[0-3][0], b[0-3][1] (dst[0-3] = a[0-3][0] + b[0-3][1])

        CALCU_O(coeffs[20], coeffs[21], d0);
        CALCU_O(coeffs[22], coeffs[23], d1);
        CALCU_O(coeffs[24], coeffs[25], d2);
        CALCU_O(coeffs[26], coeffs[27], d3);
        CALCU_O(coeffs[28], coeffs[29], d4);
        CALCU_O(coeffs[30], coeffs[31], d5);
        CALCU_O(coeffs[32], coeffs[33], d6);
        CALCU_O(coeffs[34], coeffs[35], d7);

        t[0] = _mm256_permute2x128_si256(d0, d1, 0x20);
        t[1] = _mm256_permute2x128_si256(d0, d1, 0x31);
        t[2] = _mm256_permute2x128_si256(d2, d3, 0x20);
        t[3] = _mm256_permute2x128_si256(d2, d3, 0x31);
        t[4] = _mm256_permute2x128_si256(d4, d5, 0x20);
        t[5] = _mm256_permute2x128_si256(d4, d5, 0x31);
        t[6] = _mm256_permute2x128_si256(d6, d7, 0x20);
        t[7] = _mm256_permute2x128_si256(d6, d7, 0x31);

        d0 = _mm256_add_epi32(t[0], t[1]);
        d1 = _mm256_add_epi32(t[2], t[3]);
        d2 = _mm256_add_epi32(t[4], t[5]);
        d3 = _mm256_add_epi32(t[6], t[7]);

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);
        d2 = _mm256_add_epi32(d2, add);
        d3 = _mm256_add_epi32(d3, add);

        d0 = _mm256_srai_epi32(d0, shift);                          // dst[1][0-3], dst[3][0-3]
        d1 = _mm256_srai_epi32(d1, shift);                          // dst[5][0-3], dst[7][0-3]
        d2 = _mm256_srai_epi32(d2, shift);
        d3 = _mm256_srai_epi32(d3, shift);

        d0 = _mm256_packs_epi32(d0, d1);
        d1 = _mm256_packs_epi32(d2, d3);

        for (i = 36; i < 52; i++)
            coeffs[i] = _mm256_loadu_si256((__m256i*)tab_dct2_pb32_coeffs_256i[i]);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        m4 = _mm256_castsi256_si128(d1);
        m5 = _mm256_extracti128_si256(d1, 1);
        m6 = _mm_srli_si128(m4, 8);
        m7 = _mm_srli_si128(m5, 8);

        _mm_storel_epi64((__m128i*)(dst + 1 * line), m0);
        _mm_storel_epi64((__m128i*)(dst + 3 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 5 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 7 * line), m3);
        _mm_storel_epi64((__m128i*)(dst + 9 * line), m4);
        _mm_storel_epi64((__m128i*)(dst + 11 * line), m5);
        _mm_storel_epi64((__m128i*)(dst + 13 * line), m6);
        _mm_storel_epi64((__m128i*)(dst + 15 * line), m7);

        CALCU_O(coeffs[36], coeffs[37], d0);
        CALCU_O(coeffs[38], coeffs[39], d1);
        CALCU_O(coeffs[40], coeffs[41], d2);
        CALCU_O(coeffs[42], coeffs[43], d3);
        CALCU_O(coeffs[44], coeffs[45], d4);
        CALCU_O(coeffs[46], coeffs[47], d5);
        CALCU_O(coeffs[48], coeffs[49], d6);
        CALCU_O(coeffs[50], coeffs[51], d7);

#undef CALCU_O

        t[0] = _mm256_permute2x128_si256(d0, d1, 0x20);
        t[1] = _mm256_permute2x128_si256(d0, d1, 0x31);
        t[2] = _mm256_permute2x128_si256(d2, d3, 0x20);
        t[3] = _mm256_permute2x128_si256(d2, d3, 0x31);
        t[4] = _mm256_permute2x128_si256(d4, d5, 0x20);
        t[5] = _mm256_permute2x128_si256(d4, d5, 0x31);
        t[6] = _mm256_permute2x128_si256(d6, d7, 0x20);
        t[7] = _mm256_permute2x128_si256(d6, d7, 0x31);

        d0 = _mm256_add_epi32(t[0], t[1]);
        d1 = _mm256_add_epi32(t[2], t[3]);
        d2 = _mm256_add_epi32(t[4], t[5]);
        d3 = _mm256_add_epi32(t[6], t[7]);

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);
        d2 = _mm256_add_epi32(d2, add);
        d3 = _mm256_add_epi32(d3, add);

        d0 = _mm256_srai_epi32(d0, shift);                          // dst[17][0-3], dst[19][0-3]
        d1 = _mm256_srai_epi32(d1, shift);                          // dst[21][0-3], dst[23][0-3]
        d2 = _mm256_srai_epi32(d2, shift);
        d3 = _mm256_srai_epi32(d3, shift);

        d0 = _mm256_packs_epi32(d0, d1);
        d1 = _mm256_packs_epi32(d2, d3);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        m4 = _mm256_castsi256_si128(d1);
        m5 = _mm256_extracti128_si256(d1, 1);
        m6 = _mm_srli_si128(m4, 8);
        m7 = _mm_srli_si128(m5, 8);

        _mm_storel_epi64((__m128i*)(dst + 17 * line), m0);
        _mm_storel_epi64((__m128i*)(dst + 19 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 21 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 23 * line), m3);
        _mm_storel_epi64((__m128i*)(dst + 25 * line), m4);
        _mm_storel_epi64((__m128i*)(dst + 27 * line), m5);
        _mm_storel_epi64((__m128i*)(dst + 29 * line), m6);
        _mm_storel_epi64((__m128i*)(dst + 31 * line), m7);
    }
}

static void tx_dct2_pb64_1st_avx2(s16 *src, s16 *dst, int line, int limit_line, int shift)
{
    int i, j;
    __m256i s[16];
    __m256i t[16];
    __m256i tab0, tab1, tab2;
    __m256i e[8], o[8], ee[4], eo[4];
    __m256i eee[2], eeo[2];
    __m256i eeee, eeeo;
    __m128i eeeee, eeeeo;
    __m256i v[16];
    __m256i d0, d1, d2, d3;
    __m256i add = _mm256_set1_epi32(1 << (shift - 1));
    __m256i coeffs[106];
    __m128i m0, m1, m2, m3, m4, m5, m6, m7;
    const __m128i coeff_p32_p32 = _mm_set1_epi32(0x00200020); // EEEE
    const __m128i coeff_p42_p17 = _mm_set1_epi32(0x0011002A); // EEEO

    tab0 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[0]);  // 16bit: 7-0, 7-0
    tab1 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[1]);  // 16bit: 0, 7, 1, 6, 2, 5, 3, 4, 0, 7, 1, 6, 2, 5, 3, 4
    tab2 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[2]);  // 16bit: 0, 3, 1, 2, 4, 7, 5, 6, 0, 3, 1, 2, 4, 7, 5, 6

    for (j = 0; j < limit_line; j += 4) {
        s[0] = _mm256_loadu_si256((__m256i*)(src));                 // src[0][0-15]
        s[1] = _mm256_loadu_si256((__m256i*)(src + 16 * 1));        // src[0][16-31]
        s[2] = _mm256_loadu_si256((__m256i*)(src + 16 * 2));        // src[0][32-47]
        s[3] = _mm256_loadu_si256((__m256i*)(src + 16 * 3));        // src[0][48-63]
        s[4] = _mm256_loadu_si256((__m256i*)(src + 16 * 4));
        s[5] = _mm256_loadu_si256((__m256i*)(src + 16 * 5));
        s[6] = _mm256_loadu_si256((__m256i*)(src + 16 * 6));
        s[7] = _mm256_loadu_si256((__m256i*)(src + 16 * 7));
        s[8] = _mm256_loadu_si256((__m256i*)(src + 16 * 8));        // src[2][0-15]
        s[9] = _mm256_loadu_si256((__m256i*)(src + 16 * 9));        // src[2][16-31]
        s[10] = _mm256_loadu_si256((__m256i*)(src + 16 * 10));
        s[11] = _mm256_loadu_si256((__m256i*)(src + 16 * 11));
        s[12] = _mm256_loadu_si256((__m256i*)(src + 16 * 12));
        s[13] = _mm256_loadu_si256((__m256i*)(src + 16 * 13));
        s[14] = _mm256_loadu_si256((__m256i*)(src + 16 * 14));
        s[15] = _mm256_loadu_si256((__m256i*)(src + 16 * 15));

        s[2 ] = _mm256_shuffle_epi8(s[2 ], tab0);                   // src[0][39-32], src[0][47-40]
        s[3 ] = _mm256_shuffle_epi8(s[3 ], tab0);                   // src[0][55-48], src[0][63-56]
        s[6 ] = _mm256_shuffle_epi8(s[6 ], tab0);
        s[7 ] = _mm256_shuffle_epi8(s[7 ], tab0);
        s[10] = _mm256_shuffle_epi8(s[10], tab0);                   // src[2][39-32], src[2][47-40]
        s[11] = _mm256_shuffle_epi8(s[11], tab0);                   // src[2][55-48], src[2][63-48]
        s[14] = _mm256_shuffle_epi8(s[14], tab0);
        s[15] = _mm256_shuffle_epi8(s[15], tab0);

        s[2 ] = _mm256_permute4x64_epi64(s[2 ], 0x4e);              // src[0][47-32]
        s[3 ] = _mm256_permute4x64_epi64(s[3 ], 0x4e);              // src[0][63-48]
        s[6 ] = _mm256_permute4x64_epi64(s[6 ], 0x4e);
        s[7 ] = _mm256_permute4x64_epi64(s[7 ], 0x4e);
        s[10] = _mm256_permute4x64_epi64(s[10], 0x4e);
        s[11] = _mm256_permute4x64_epi64(s[11], 0x4e);
        s[14] = _mm256_permute4x64_epi64(s[14], 0x4e);
        s[15] = _mm256_permute4x64_epi64(s[15], 0x4e);

        src += 64 * 4;

        e[0] = _mm256_add_epi16(s[0], s[3]);                        // e[0][0-15]
        o[0] = _mm256_sub_epi16(s[0], s[3]);                        // o[0][0-15]
        e[1] = _mm256_add_epi16(s[1], s[2]);                        // e[0][16-31]
        o[1] = _mm256_sub_epi16(s[1], s[2]);
        e[2] = _mm256_add_epi16(s[4], s[7]);
        o[2] = _mm256_sub_epi16(s[4], s[7]);
        e[3] = _mm256_add_epi16(s[5], s[6]);
        o[3] = _mm256_sub_epi16(s[5], s[6]);
        e[4] = _mm256_add_epi16(s[8], s[11]);
        o[4] = _mm256_sub_epi16(s[8], s[11]);
        e[5] = _mm256_add_epi16(s[9], s[10]);
        o[5] = _mm256_sub_epi16(s[9], s[10]);
        e[6] = _mm256_add_epi16(s[12], s[15]);
        o[6] = _mm256_sub_epi16(s[12], s[15]);
        e[7] = _mm256_add_epi16(s[13], s[14]);
        o[7] = _mm256_sub_epi16(s[13], s[14]);

        e[1] = _mm256_shuffle_epi8(e[1], tab0);                     // e[0][23-16], e[0][31-24]
        e[3] = _mm256_shuffle_epi8(e[3], tab0);                     // e[1][23-16], e[1][31-24]
        e[5] = _mm256_shuffle_epi8(e[5], tab0);
        e[7] = _mm256_shuffle_epi8(e[7], tab0);

        e[1] = _mm256_permute4x64_epi64(e[1], 0x4e);                // e[0][31-16]
        e[3] = _mm256_permute4x64_epi64(e[3], 0x4e);
        e[5] = _mm256_permute4x64_epi64(e[5], 0x4e);
        e[7] = _mm256_permute4x64_epi64(e[7], 0x4e);

        ee[0] = _mm256_add_epi16(e[0], e[1]);                       // ee[0][0-15]
        eo[0] = _mm256_sub_epi16(e[0], e[1]);                       // eo[0][0-15]
        ee[1] = _mm256_add_epi16(e[2], e[3]);                       // ee[1][0-15]
        eo[1] = _mm256_sub_epi16(e[2], e[3]);                       // eo[1][0-15]
        ee[2] = _mm256_add_epi16(e[4], e[5]);
        eo[2] = _mm256_sub_epi16(e[4], e[5]);
        ee[3] = _mm256_add_epi16(e[6], e[7]);
        eo[3] = _mm256_sub_epi16(e[6], e[7]);

        t[0] = _mm256_permute2x128_si256(ee[0], ee[2], 0x20);       // ee[0][0-7], ee[2][0-7] 
        t[1] = _mm256_permute2x128_si256(ee[0], ee[2], 0x31);       // ee[0][8-15], ee[2][8-15]
        t[2] = _mm256_permute2x128_si256(ee[1], ee[3], 0x20);       // ee[1][0-7], ee[3][0-7] 
        t[3] = _mm256_permute2x128_si256(ee[1], ee[3], 0x31);       // ee[1][8-15], ee[3][8-15]

        t[1] = _mm256_shuffle_epi8(t[1], tab0);                     // ee[0][15-8], ee[2][15-8]
        t[3] = _mm256_shuffle_epi8(t[3], tab0);                     // ee[1][15-8], ee[3][15-8]

        eee[0] = _mm256_add_epi16(t[0], t[1]);                      // eee[0][0-7], eee[2][0-7]
        eeo[0] = _mm256_sub_epi16(t[0], t[1]);                      // eeo[0][0-7], eeo[2][0-7]
        eee[1] = _mm256_add_epi16(t[2], t[3]);
        eeo[1] = _mm256_sub_epi16(t[2], t[3]);

        eee[0] = _mm256_shuffle_epi8(eee[0], tab1);                 // eee[0][0, 7, 1, 6, 2, 5, 3, 4, 0], eee[2][0, 7, 1, 6, 2, 5, 3, 4, 0]
        eee[1] = _mm256_shuffle_epi8(eee[1], tab1);

        eeee = _mm256_hadd_epi16(eee[0], eee[1]);                    // eeee[0][0, 3], eeee[1][0, 3], eeee[2][0, 3], eeee[3][0, 3]
        eeeo = _mm256_hsub_epi16(eee[0], eee[1]);

        eeee = _mm256_shuffle_epi8(eeee, tab2);                     // eeee[0][0, 3, 1, 2], eeee[1][0, 3, 1, 2], eeee[2][0, 3, 1, 2], eeee[3][0, 3, 1, 2]

        m0 = _mm256_castsi256_si128(eeee);
        m1 = _mm256_extracti128_si256(eeee, 1);
        eeeee = _mm_hadd_epi16(m0, m1);                             // eeee[0][0, 1], eeee[1][0, 1], eeee[2][0, 1], eeee[3][0, 1]
        eeeeo = _mm_hsub_epi16(m0, m1);

        coeffs[0] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[0]);
        coeffs[1] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[1]);

        m0 = _mm_madd_epi16(eeeee, coeff_p32_p32);
        m1 = _mm_madd_epi16(eeeeo, coeff_p42_p17);

        v[4] = _mm256_set_m128i(m1, m0);                            // dst[0][0-3], dst[16][0-3]

        v[0] = _mm256_madd_epi16(eeeo, coeffs[0]);
        v[2] = _mm256_madd_epi16(eeeo, coeffs[1]);

        v[0] = _mm256_hadd_epi32(v[0], v[2]);                       // dst[8][0-1], dst[24][0-1], dst[8][2-3], dst[24][2-3]

        v[0] = _mm256_permute4x64_epi64(v[0], 0xd8);                // dst[8][0-3], dst[24][0-3]
        v[4] = _mm256_add_epi32(v[4], add);
        v[0] = _mm256_add_epi32(v[0], add);
        v[4] = _mm256_srai_epi32(v[4], shift);
        v[0] = _mm256_srai_epi32(v[0], shift);

        d0 = _mm256_packs_epi32(v[4], v[0]);                        // dst[0][0-3], dst[8][0-3], dst[16][0-3], dst[24][0-3]

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);

        for (i = 4; i < 8; i++)
            coeffs[i - 2] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[i]);

        _mm_storel_epi64((__m128i*)(dst), m0);
        _mm_storel_epi64((__m128i*)(dst + 8 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 16 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 24 * line), m3);

#define CALCU_EEO(coeff0, coeff1, dst) \
        v[0] = _mm256_madd_epi16(eeo[0], coeff0); \
        v[1] = _mm256_madd_epi16(eeo[1], coeff0); \
        v[2] = _mm256_madd_epi16(eeo[0], coeff1); \
        v[3] = _mm256_madd_epi16(eeo[1], coeff1); \
        v[0] = _mm256_hadd_epi32(v[0], v[1]); \
        v[2] = _mm256_hadd_epi32(v[2], v[3]); \
        dst = _mm256_hadd_epi32(v[0], v[2])

        CALCU_EEO(coeffs[2], coeffs[3], d0);
        CALCU_EEO(coeffs[4], coeffs[5], d1);

        d0 = _mm256_permute4x64_epi64(d0, 0xd8);                    // dst[4][0-4], dst[12][0-4]
        d1 = _mm256_permute4x64_epi64(d1, 0xd8);

#undef CALCU_EEO

        for (i = 12; i < 20; i++)
            coeffs[i-12] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[i]);

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);

        d0 = _mm256_srai_epi32(d0, shift);
        d1 = _mm256_srai_epi32(d1, shift);

        d0 = _mm256_packs_epi32(d0, d1);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);

        _mm_storel_epi64((__m128i*)(dst + 4 * line), m0);
        _mm_storel_epi64((__m128i*)(dst + 12 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 20 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 28 * line), m3);

#define CALCU_EO(coeff0, coeff1, dst) \
        v[0] = _mm256_madd_epi16(eo[0], coeff0); \
        v[1] = _mm256_madd_epi16(eo[1], coeff0); \
        v[2] = _mm256_madd_epi16(eo[2], coeff0); \
        v[3] = _mm256_madd_epi16(eo[3], coeff0); \
        v[4] = _mm256_madd_epi16(eo[0], coeff1); \
        v[5] = _mm256_madd_epi16(eo[1], coeff1); \
        v[6] = _mm256_madd_epi16(eo[2], coeff1); \
        v[7] = _mm256_madd_epi16(eo[3], coeff1); \
        v[0] = _mm256_hadd_epi32(v[0], v[1]); \
        v[2] = _mm256_hadd_epi32(v[2], v[3]); \
        v[4] = _mm256_hadd_epi32(v[4], v[5]); \
        v[6] = _mm256_hadd_epi32(v[6], v[7]); \
        v[0] = _mm256_hadd_epi32(v[0], v[2]); \
        v[4] = _mm256_hadd_epi32(v[4], v[6]); \
        v[2] = _mm256_permute2x128_si256(v[0], v[4], 0x20); \
        v[3] = _mm256_permute2x128_si256(v[0], v[4], 0x31); \
        dst = _mm256_add_epi32(v[2], v[3]);                     

        v[0] = _mm256_madd_epi16(eo[0], coeffs[0]); 
        v[1] = _mm256_madd_epi16(eo[1], coeffs[0]); 
        v[2] = _mm256_madd_epi16(eo[2], coeffs[0]); 
        v[3] = _mm256_madd_epi16(eo[3], coeffs[0]); 
        v[4] = _mm256_madd_epi16(eo[0], coeffs[1]); 
        v[5] = _mm256_madd_epi16(eo[1], coeffs[1]); 
        v[6] = _mm256_madd_epi16(eo[2], coeffs[1]); 
        v[7] = _mm256_madd_epi16(eo[3], coeffs[1]); 
        v[0] = _mm256_hadd_epi32(v[0], v[1]); 
        v[2] = _mm256_hadd_epi32(v[2], v[3]); 
        v[4] = _mm256_hadd_epi32(v[4], v[5]); 
        v[6] = _mm256_hadd_epi32(v[6], v[7]); 
        v[0] = _mm256_hadd_epi32(v[0], v[2]); 
        v[4] = _mm256_hadd_epi32(v[4], v[6]); 
        v[2] = _mm256_permute2x128_si256(v[0], v[4], 0x20); 
        v[3] = _mm256_permute2x128_si256(v[0], v[4], 0x31); 
        d0 = _mm256_add_epi32(v[2], v[3]);

        //CALCU_EO(coeffs[0], coeffs[1], d0);                       // dst[2][0-3], dst[6][0-3]
        CALCU_EO(coeffs[2], coeffs[3], d1);
        CALCU_EO(coeffs[4], coeffs[5], d2);
        CALCU_EO(coeffs[6], coeffs[7], d3);

#undef CALCU_EO

        for (i = 28; i < 44; i++)
            coeffs[i - 28] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[i]);

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);
        d2 = _mm256_add_epi32(d2, add);
        d3 = _mm256_add_epi32(d3, add);

        d0 = _mm256_srai_epi32(d0, shift);                          // dst[2][0-3], dst[6][0-3]
        d1 = _mm256_srai_epi32(d1, shift);                          // dst[10][0-3], dst[14][0-3]
        d2 = _mm256_srai_epi32(d2, shift);
        d3 = _mm256_srai_epi32(d3, shift);

        d0 = _mm256_packs_epi32(d0, d1);
        d1 = _mm256_packs_epi32(d2, d3);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        m4 = _mm256_castsi256_si128(d1);
        m5 = _mm256_extracti128_si256(d1, 1);
        m6 = _mm_srli_si128(m4, 8);
        m7 = _mm_srli_si128(m5, 8);

        _mm_storel_epi64((__m128i*)(dst + 2 * line), m0);
        _mm_storel_epi64((__m128i*)(dst + 6 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 10 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 14 * line), m3);
        _mm_storel_epi64((__m128i*)(dst + 18 * line), m4);
        _mm_storel_epi64((__m128i*)(dst + 22 * line), m5);
        _mm_storel_epi64((__m128i*)(dst + 26 * line), m6);
        _mm_storel_epi64((__m128i*)(dst + 30 * line), m7);

#define CALCU_O(coeff0, coeff1, coeff2, coeff3, d) \
        v[0 ] = _mm256_madd_epi16(o[0], coeff0);   \
        v[1 ] = _mm256_madd_epi16(o[1], coeff1);   \
        v[2 ] = _mm256_madd_epi16(o[2], coeff0);   \
        v[3 ] = _mm256_madd_epi16(o[3], coeff1);   \
        v[4 ] = _mm256_madd_epi16(o[4], coeff0);   \
        v[5 ] = _mm256_madd_epi16(o[5], coeff1);   \
        v[6 ] = _mm256_madd_epi16(o[6], coeff0);   \
        v[7 ] = _mm256_madd_epi16(o[7], coeff1);   \
        v[8 ] = _mm256_madd_epi16(o[0], coeff2);   \
        v[9 ] = _mm256_madd_epi16(o[1], coeff3);   \
        v[10] = _mm256_madd_epi16(o[2], coeff2);   \
        v[11] = _mm256_madd_epi16(o[3], coeff3);   \
        v[12] = _mm256_madd_epi16(o[4], coeff2);   \
        v[13] = _mm256_madd_epi16(o[5], coeff3);   \
        v[14] = _mm256_madd_epi16(o[6], coeff2);   \
        v[15] = _mm256_madd_epi16(o[7], coeff3);   \
        v[0 ] = _mm256_add_epi32(v[0], v[1]);      \
        v[2 ] = _mm256_add_epi32(v[2], v[3]);      \
        v[4 ] = _mm256_add_epi32(v[4], v[5]);      \
        v[6 ] = _mm256_add_epi32(v[6], v[7]);      \
        v[8 ] = _mm256_add_epi32(v[8 ], v[9]);     \
        v[10] = _mm256_add_epi32(v[10], v[11]);    \
        v[12] = _mm256_add_epi32(v[12], v[13]);    \
        v[14] = _mm256_add_epi32(v[14], v[15]);    \
        v[0 ] = _mm256_hadd_epi32(v[0], v[2]);     \
        v[1 ] = _mm256_hadd_epi32(v[4], v[6]);     \
        v[2 ] = _mm256_hadd_epi32(v[8], v[10]);    \
        v[3 ] = _mm256_hadd_epi32(v[12], v[14]);   \
        v[0 ] = _mm256_hadd_epi32(v[0], v[1]);     \
        v[2 ] = _mm256_hadd_epi32(v[2], v[3]);     \
        v[1] = _mm256_permute2x128_si256(v[0], v[2], 0x20); \
        v[3] = _mm256_permute2x128_si256(v[0], v[2], 0x31); \
        d = _mm256_add_epi32(v[1], v[3]); 

        CALCU_O(coeffs[0], coeffs[1], coeffs[2], coeffs[3], d0);
        CALCU_O(coeffs[4], coeffs[5], coeffs[6], coeffs[7], d1);
        CALCU_O(coeffs[8], coeffs[9], coeffs[10], coeffs[11], d2);
        CALCU_O(coeffs[12], coeffs[13], coeffs[14], coeffs[15], d3);

        for (i = 44; i < 60; i++)
            coeffs[i - 44] = _mm256_loadu_si256((__m256i*)tab_dct2_1st_coeffs_256i[i]);

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);
        d2 = _mm256_add_epi32(d2, add);
        d3 = _mm256_add_epi32(d3, add);

        d0 = _mm256_srai_epi32(d0, shift);                          // dst[1][0-3], dst[3][0-3]
        d1 = _mm256_srai_epi32(d1, shift);                          // dst[5][0-3], dst[7][0-3]
        d2 = _mm256_srai_epi32(d2, shift);
        d3 = _mm256_srai_epi32(d3, shift);

        d0 = _mm256_packs_epi32(d0, d1);
        d1 = _mm256_packs_epi32(d2, d3);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        m4 = _mm256_castsi256_si128(d1);
        m5 = _mm256_extracti128_si256(d1, 1);
        m6 = _mm_srli_si128(m4, 8);
        m7 = _mm_srli_si128(m5, 8);

        _mm_storel_epi64((__m128i*)(dst + 1 * line), m0);
        _mm_storel_epi64((__m128i*)(dst + 3 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 5 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 7 * line), m3);
        _mm_storel_epi64((__m128i*)(dst + 9 * line), m4);
        _mm_storel_epi64((__m128i*)(dst + 11 * line), m5);
        _mm_storel_epi64((__m128i*)(dst + 13 * line), m6);
        _mm_storel_epi64((__m128i*)(dst + 15 * line), m7);

        CALCU_O(coeffs[0], coeffs[1], coeffs[2], coeffs[3], d0);
        CALCU_O(coeffs[4], coeffs[5], coeffs[6], coeffs[7], d1);
        CALCU_O(coeffs[8], coeffs[9], coeffs[10], coeffs[11], d2);
        CALCU_O(coeffs[12], coeffs[13], coeffs[14], coeffs[15], d3);

#undef CALCU_O

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);
        d2 = _mm256_add_epi32(d2, add);
        d3 = _mm256_add_epi32(d3, add);

        d0 = _mm256_srai_epi32(d0, shift);                          // dst[17][0-3], dst[19][0-3]
        d1 = _mm256_srai_epi32(d1, shift);                          // dst[21][0-3], dst[23][0-3]
        d2 = _mm256_srai_epi32(d2, shift);
        d3 = _mm256_srai_epi32(d3, shift);

        d0 = _mm256_packs_epi32(d0, d1);
        d1 = _mm256_packs_epi32(d2, d3);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        m4 = _mm256_castsi256_si128(d1);
        m5 = _mm256_extracti128_si256(d1, 1);
        m6 = _mm_srli_si128(m4, 8);
        m7 = _mm_srli_si128(m5, 8);

        _mm_storel_epi64((__m128i*)(dst + 17 * line), m0);
        _mm_storel_epi64((__m128i*)(dst + 19 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 21 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 23 * line), m3);
        _mm_storel_epi64((__m128i*)(dst + 25 * line), m4);
        _mm_storel_epi64((__m128i*)(dst + 27 * line), m5);
        _mm_storel_epi64((__m128i*)(dst + 29 * line), m6);
        _mm_storel_epi64((__m128i*)(dst + 31 * line), m7);

        dst += 4;
    }
}

static void tx_dct2_pb64_2nd_avx2(s16 *src, s16 *dst, int line, int limit_line, int shift)
{
    int i, j;
    __m256i s[32];
    __m256i t[16];
    __m256i tab0, tab1, tab2;
    __m256i e[16], o[16], ee[8], eo[8];
    __m256i eee[4], eeo[4];
    __m256i eeee[2], eeeo[2], eeeee, eeeeo;
    __m256i v[16];
    __m256i d0, d1, d2, d3, d4, d5, d6, d7;
    __m256i add = _mm256_set1_epi32(1 << (shift - 1));
    __m256i coeffs[106];
    __m128i m0, m1, m2, m3, m4, m5, m6, m7;
    const __m256i coeff_p32_p32 = _mm256_set1_epi64x(0x0000002000000020); // EEEE
    const __m256i coeff_p42_p17 = _mm256_set1_epi64x(0x000000110000002A); // EEEO

    tab0 = _mm256_loadu_si256((__m256i*)tab_dct2_2nd_shuffle_256i[3]);  // 16bit: 7-0, 7-0
    tab1 = _mm256_loadu_si256((__m256i*)tab_dct2_2nd_shuffle_256i[1]);  // 32bit: 3-0, 3-0
    tab2 = _mm256_loadu_si256((__m256i*)tab_dct2_2nd_shuffle_256i[2]);  // 32bit: 0, 3, 1, 2, 0, 3, 1, 2

    for (j = 0; j < 106; j++) {
        coeffs[j] = _mm256_loadu_si256((__m256i*)tab_dct2_pb64_coeffs_256i[j]);
    }

    for (j = 0; j < limit_line; j += 4) {
        s[0] = _mm256_loadu_si256((__m256i*)(src));                     // src[0][0-15]
        s[1] = _mm256_loadu_si256((__m256i*)(src + 16 * 1));            // src[0][16-31]
        s[2] = _mm256_loadu_si256((__m256i*)(src + 16 * 2));            // src[0][32-47]
        s[3] = _mm256_loadu_si256((__m256i*)(src + 16 * 3));            // src[0][48-63]
        s[4] = _mm256_loadu_si256((__m256i*)(src + 16 * 4));
        s[5] = _mm256_loadu_si256((__m256i*)(src + 16 * 5));
        s[6] = _mm256_loadu_si256((__m256i*)(src + 16 * 6));
        s[7] = _mm256_loadu_si256((__m256i*)(src + 16 * 7));
        s[8] = _mm256_loadu_si256((__m256i*)(src + 16 * 8));            // src[2][0-15]
        s[9] = _mm256_loadu_si256((__m256i*)(src + 16 * 9));            // src[2][16-31]
        s[10] = _mm256_loadu_si256((__m256i*)(src + 16 * 10));
        s[11] = _mm256_loadu_si256((__m256i*)(src + 16 * 11));
        s[12] = _mm256_loadu_si256((__m256i*)(src + 16 * 12));
        s[13] = _mm256_loadu_si256((__m256i*)(src + 16 * 13));
        s[14] = _mm256_loadu_si256((__m256i*)(src + 16 * 14));
        s[15] = _mm256_loadu_si256((__m256i*)(src + 16 * 15));

        t[0] = _mm256_shuffle_epi8(s[2], tab0);                         // src[0][39-32], src[0][47-40]
        t[1] = _mm256_shuffle_epi8(s[3], tab0);                         // src[0][55-48], src[0][63-56]
        t[2] = _mm256_shuffle_epi8(s[6], tab0);
        t[3] = _mm256_shuffle_epi8(s[7], tab0);
        t[4] = _mm256_shuffle_epi8(s[10], tab0);                        // src[2][39-32], src[2][47-40]
        t[5] = _mm256_shuffle_epi8(s[11], tab0);                        // src[2][55-48], src[2][63-48]
        t[6] = _mm256_shuffle_epi8(s[14], tab0);
        t[7] = _mm256_shuffle_epi8(s[15], tab0);

        s[3] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[1], 1));    // src[0][24-31]
        s[2] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[1]));         // src[0][16-23]
        s[1] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[0], 1));    // src[0][7-15]
        s[0] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[0]));         // src[0][0-7]
        s[7] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[5], 1));    // src[1][24-31]
        s[6] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[5]));
        s[5] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[4], 1));
        s[4] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[4]));
        s[11] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[9], 1));   // src[2][24-31]
        s[10] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[9]));        // src[2][16-23]
        s[9] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[8], 1));    // src[2][7-15]
        s[8] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[8]));         // src[2][0-7]
        s[15] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[13], 1));  // src[3][24-31]
        s[14] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[13]));
        s[13] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(s[12], 1));
        s[12] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(s[12]));

        s[18] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[0], 1));   // src[0][63-56]
        s[19] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[0]));        // src[0][55-48]
        s[16] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[1], 1));   // src[0][47-40]
        s[17] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[1]));        // src[0][39-32]
        s[22] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[2], 1));
        s[23] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[2]));
        s[20] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[3], 1));
        s[21] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[3]));
        s[26] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[4], 1));   // src[2][63-56]
        s[27] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[4]));        // src[2][55-48]
        s[24] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[5], 1));   // src[2][47-40]
        s[25] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[5]));        // src[2][39-32]
        s[30] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[6], 1));
        s[31] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[6]));
        s[28] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(t[7], 1));
        s[29] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(t[7]));

        src += 64 * 4;

        for (i = 0; i < 16; i++) {
            e[i] = _mm256_add_epi32(s[i], s[16 + i]);               // e[0-3]: e[0-31]
            o[i] = _mm256_sub_epi32(s[i], s[16 + i]);
        }

        for (i = 0; i < 8; i++) {
            // t[0]: e[0][0-3], e[2][0-3] 
            // t[1]: e[0][4-7], e[2][4-7]
            // t[2]: e[0][8-11], e[2][8-11]
            // t[3]: e[0][12-15], e[2][12-15]
            // t[4]: e[0][16-19], e[2][16-19] 
            // t[5]: ---
            // t[6]: ---
            // t[7]: e[0][28-31], e[2][28-31] 
            t[i * 2] = _mm256_permute2x128_si256(e[i], e[i + 8], 0x20);
            t[i * 2 + 1] = _mm256_permute2x128_si256(e[i], e[i + 8], 0x31);
        }

        t[4] = _mm256_shuffle_epi8(t[4], tab1);                     // e[0][19-16], e[2][19-16]
        t[5] = _mm256_shuffle_epi8(t[5], tab1);                     // e[0][23-20], e[2][23-20]
        t[6] = _mm256_shuffle_epi8(t[6], tab1);
        t[7] = _mm256_shuffle_epi8(t[7], tab1);
        t[12] = _mm256_shuffle_epi8(t[12], tab1);                   // e[1][19-16], e[3][19-16]
        t[13] = _mm256_shuffle_epi8(t[13], tab1);                   // e[1][23-20], e[3][23-20]
        t[14] = _mm256_shuffle_epi8(t[14], tab1);
        t[15] = _mm256_shuffle_epi8(t[15], tab1);

        for (i = 0; i < 8; i += 4) {
            int i2 = i * 2;
            ee[i] = _mm256_add_epi32(t[i2], t[i2 + 7]);             // ee[0][0-3], ee[2][0-3]
            eo[i] = _mm256_sub_epi32(t[i2], t[i2 + 7]);             // eo[0][0-3], eo[2][0-3]
            ee[i + 1] = _mm256_add_epi32(t[i2 + 1], t[i2 + 6]);     // ee[0][4-7], ee[2][4-7]
            eo[i + 1] = _mm256_sub_epi32(t[i2 + 1], t[i2 + 6]);     // eo[0][4-7], eo[2][4-7]
            ee[i + 2] = _mm256_add_epi32(t[i2 + 2], t[i2 + 5]);
            eo[i + 2] = _mm256_sub_epi32(t[i2 + 2], t[i2 + 5]);
            ee[i + 3] = _mm256_add_epi32(t[i2 + 3], t[i2 + 4]);
            eo[i + 3] = _mm256_sub_epi32(t[i2 + 3], t[i2 + 4]);
        }

        ee[2] = _mm256_shuffle_epi8(ee[2], tab1);                   // ee[0][11-8], ee[2][11-8]
        ee[3] = _mm256_shuffle_epi8(ee[3], tab1);                   // ee[0][15-12], ee[2][15-12]
        ee[6] = _mm256_shuffle_epi8(ee[6], tab1);                   // ee[1][11-8], ee[3][11-8]
        ee[7] = _mm256_shuffle_epi8(ee[7], tab1);                   // ee[1][15-12], ee[3][15-12]

        eee[0] = _mm256_add_epi32(ee[0], ee[3]);                    // eee[0][0-3], eee[2][0-3]
        eeo[0] = _mm256_sub_epi32(ee[0], ee[3]);                    // eeo[0][0-3], eeo[2][0-3]
        eee[1] = _mm256_add_epi32(ee[1], ee[2]);
        eeo[1] = _mm256_sub_epi32(ee[1], ee[2]);
        eee[2] = _mm256_add_epi32(ee[4], ee[7]);                    // eee[1][0-3], eee[3][0-3]
        eeo[2] = _mm256_sub_epi32(ee[4], ee[7]);                    // eeo[1][0-3], eeo[3][0-3]
        eee[3] = _mm256_add_epi32(ee[5], ee[6]);
        eeo[3] = _mm256_sub_epi32(ee[5], ee[6]);

        eee[1] = _mm256_shuffle_epi8(eee[1], tab1);                 // eee[0][7-4], eee[2][7-4]
        eee[3] = _mm256_shuffle_epi8(eee[3], tab1);

        eeee[0] = _mm256_add_epi32(eee[0], eee[1]);                 // eeee[0][0, 3], eeee[2][0, 3]
        eeeo[0] = _mm256_sub_epi32(eee[0], eee[1]);
        eeee[1] = _mm256_add_epi32(eee[2], eee[3]);                 // eeee[1][0, 3], eeee[3][0, 3]
        eeeo[1] = _mm256_sub_epi32(eee[2], eee[3]);

        eeee[0] = _mm256_shuffle_epi8(eeee[0], tab2);               // eeee[0][0, 3, 1, 2], eeee[2][0, 3, 1, 2]
        eeee[1] = _mm256_shuffle_epi8(eeee[1], tab2);

        eeeee = _mm256_hadd_epi32(eeee[0], eeee[1]);                // eeeee[0][0, 1], eeeee[1][0, 1], eeeee[2][0, 1], eeeee[3][0, 1]
        eeeeo = _mm256_hsub_epi32(eeee[0], eeee[1]);

        v[0] = _mm256_mullo_epi32(eeeee, coeff_p32_p32);
        v[1] = _mm256_mullo_epi32(eeeeo, coeff_p42_p17);

        v[4] = _mm256_hadd_epi32(v[0], v[1]);                       // dst[0][0-1], dst[16][0-1], dst[0][2-3], dst[16][2-3]

        v[0] = _mm256_mullo_epi32(eeeo[0], coeffs[0]);
        v[1] = _mm256_mullo_epi32(eeeo[1], coeffs[0]);
        v[2] = _mm256_mullo_epi32(eeeo[0], coeffs[1]);
        v[3] = _mm256_mullo_epi32(eeeo[1], coeffs[1]);

        v[0] = _mm256_hadd_epi32(v[0], v[1]);
        v[2] = _mm256_hadd_epi32(v[2], v[3]);
        v[0] = _mm256_hadd_epi32(v[0], v[2]);                       // dst[8][0-1], dst[24][0-1], dst[8][2-3], dst[24][2-3]

        v[4] = _mm256_permute4x64_epi64(v[4], 0xd8);                // dst[0][0-3], dst[16][0-3]
        v[0] = _mm256_permute4x64_epi64(v[0], 0xd8);                // dst[8][0-3], dst[24][0-3]
        v[4] = _mm256_add_epi32(v[4], add);
        v[0] = _mm256_add_epi32(v[0], add);
        v[4] = _mm256_srai_epi32(v[4], shift);
        v[0] = _mm256_srai_epi32(v[0], shift);

        d0 = _mm256_packs_epi32(v[4], v[0]);                        // dst[0][0-3], dst[8][0-3], dst[16][0-3], dst[24][0-3]

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);

        _mm_storel_epi64((__m128i*)(dst), m0);
        _mm_storel_epi64((__m128i*)(dst + 8 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 16 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 24 * line), m3);

#define CALCU_EEO(coeff0, coeff1, dst) \
        v[0] = _mm256_mullo_epi32(eeo[0], coeff0); \
        v[1] = _mm256_mullo_epi32(eeo[1], coeff1); \
        v[2] = _mm256_mullo_epi32(eeo[2], coeff0); \
        v[3] = _mm256_mullo_epi32(eeo[3], coeff1); \
        v[0] = _mm256_hadd_epi32(v[0], v[1]); \
        v[2] = _mm256_hadd_epi32(v[2], v[3]); \
        dst = _mm256_hadd_epi32(v[0], v[2])

        CALCU_EEO(coeffs[2], coeffs[3], d0);
        CALCU_EEO(coeffs[4], coeffs[5], d1);
        CALCU_EEO(coeffs[6], coeffs[7], d2);
        CALCU_EEO(coeffs[8], coeffs[9], d3);

        d0 = _mm256_hadd_epi32(d0, d1);
        d2 = _mm256_hadd_epi32(d2, d3);

        d0 = _mm256_permute4x64_epi64(d0, 0xd8);                    // dst[4][0-4], dst[12][0-4]
        d1 = _mm256_permute4x64_epi64(d2, 0xd8);

#undef CALCU_EEO

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);

        d0 = _mm256_srai_epi32(d0, shift);
        d1 = _mm256_srai_epi32(d1, shift);

        d0 = _mm256_packs_epi32(d0, d1);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);

        _mm_storel_epi64((__m128i*)(dst + 4 * line), m0);
        _mm_storel_epi64((__m128i*)(dst + 12 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 20 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 28 * line), m3);

#define CALCU_EO(coeff0, coeff1, coeff2, coeff3, dst) \
        v[0] = _mm256_mullo_epi32(eo[0], coeff0); \
        v[1] = _mm256_mullo_epi32(eo[1], coeff1); \
        v[2] = _mm256_mullo_epi32(eo[2], coeff2); \
        v[3] = _mm256_mullo_epi32(eo[3], coeff3); \
        v[4] = _mm256_mullo_epi32(eo[4], coeff0); \
        v[5] = _mm256_mullo_epi32(eo[5], coeff1); \
        v[6] = _mm256_mullo_epi32(eo[6], coeff2); \
        v[7] = _mm256_mullo_epi32(eo[7], coeff3); \
        v[0] = _mm256_add_epi32(v[0], v[1]); \
        v[2] = _mm256_add_epi32(v[2], v[3]); \
        v[4] = _mm256_add_epi32(v[4], v[5]); \
        v[6] = _mm256_add_epi32(v[6], v[7]); \
        v[0] = _mm256_add_epi32(v[0], v[2]); \
        v[4] = _mm256_add_epi32(v[4], v[6]); \
        dst = _mm256_hadd_epi32(v[0], v[4]);                        // t[0][0-1], t[1][0-1], t[2][0-1], t[3][0-1] (dst[i] = t[i][0]+t[i][1])

        CALCU_EO(coeffs[10], coeffs[11], coeffs[12], coeffs[13], d0);
        CALCU_EO(coeffs[14], coeffs[15], coeffs[16], coeffs[17], d1);
        CALCU_EO(coeffs[18], coeffs[19], coeffs[20], coeffs[21], d2);
        CALCU_EO(coeffs[22], coeffs[23], coeffs[24], coeffs[25], d3);
        CALCU_EO(coeffs[26], coeffs[27], coeffs[28], coeffs[29], d4);
        CALCU_EO(coeffs[30], coeffs[31], coeffs[32], coeffs[33], d5);
        CALCU_EO(coeffs[34], coeffs[35], coeffs[36], coeffs[37], d6);
        CALCU_EO(coeffs[38], coeffs[39], coeffs[40], coeffs[41], d7);

#undef CALCU_EO
        d0 = _mm256_hadd_epi32(d0, d1);                             // dst[2][0-1], dst[6][0-1], dst[2][2-3], dst[6][2-3]
        d1 = _mm256_hadd_epi32(d2, d3);
        d2 = _mm256_hadd_epi32(d4, d5);
        d3 = _mm256_hadd_epi32(d6, d7);

        d0 = _mm256_permute4x64_epi64(d0, 0xd8);
        d1 = _mm256_permute4x64_epi64(d1, 0xd8);
        d2 = _mm256_permute4x64_epi64(d2, 0xd8);
        d3 = _mm256_permute4x64_epi64(d3, 0xd8);

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);
        d2 = _mm256_add_epi32(d2, add);
        d3 = _mm256_add_epi32(d3, add);

        d0 = _mm256_srai_epi32(d0, shift);                          // dst[2][0-3], dst[6][0-3]
        d1 = _mm256_srai_epi32(d1, shift);                          // dst[10][0-3], dst[14][0-3]
        d2 = _mm256_srai_epi32(d2, shift);
        d3 = _mm256_srai_epi32(d3, shift);

        d0 = _mm256_packs_epi32(d0, d1);
        d1 = _mm256_packs_epi32(d2, d3);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        m4 = _mm256_castsi256_si128(d1);
        m5 = _mm256_extracti128_si256(d1, 1);
        m6 = _mm_srli_si128(m4, 8);
        m7 = _mm_srli_si128(m5, 8);

        _mm_storel_epi64((__m128i*)(dst + 2 * line), m0);
        _mm_storel_epi64((__m128i*)(dst + 6 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 10 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 14 * line), m3);
        _mm_storel_epi64((__m128i*)(dst + 18 * line), m4);
        _mm_storel_epi64((__m128i*)(dst + 22 * line), m5);
        _mm_storel_epi64((__m128i*)(dst + 26 * line), m6);
        _mm_storel_epi64((__m128i*)(dst + 30 * line), m7);

#define CALCU_O(coeff0, coeff1, coeff2, coeff3, d) \
        v[0 ] = _mm256_mullo_epi32(o[0], coeff0);  \
        v[1 ] = _mm256_mullo_epi32(o[1], coeff1);  \
        v[2 ] = _mm256_mullo_epi32(o[2], coeff2);  \
        v[3 ] = _mm256_mullo_epi32(o[3], coeff3);  \
        v[4 ] = _mm256_mullo_epi32(o[4], coeff0);  \
        v[5 ] = _mm256_mullo_epi32(o[5], coeff1);  \
        v[6 ] = _mm256_mullo_epi32(o[6], coeff2);  \
        v[7 ] = _mm256_mullo_epi32(o[7], coeff3);  \
        v[8 ] = _mm256_mullo_epi32(o[8 ], coeff0); \
        v[9 ] = _mm256_mullo_epi32(o[9 ], coeff1); \
        v[10] = _mm256_mullo_epi32(o[10], coeff2); \
        v[11] = _mm256_mullo_epi32(o[11], coeff3); \
        v[12] = _mm256_mullo_epi32(o[12], coeff0); \
        v[13] = _mm256_mullo_epi32(o[13], coeff1); \
        v[14] = _mm256_mullo_epi32(o[14], coeff2); \
        v[15] = _mm256_mullo_epi32(o[15], coeff3); \
        v[0 ] = _mm256_add_epi32(v[0], v[1]);      \
        v[2 ] = _mm256_add_epi32(v[2], v[3]);      \
        v[4 ] = _mm256_add_epi32(v[4], v[5]);      \
        v[6 ] = _mm256_add_epi32(v[6], v[7]);      \
        v[8 ] = _mm256_add_epi32(v[8 ], v[9]);     \
        v[10] = _mm256_add_epi32(v[10], v[11]);    \
        v[12] = _mm256_add_epi32(v[12], v[13]);    \
        v[14] = _mm256_add_epi32(v[14], v[15]);    \
        v[0 ] = _mm256_add_epi32(v[0], v[2]);      \
        v[1 ] = _mm256_add_epi32(v[4], v[6]);      \
        v[2 ] = _mm256_add_epi32(v[8], v[10]);     \
        v[3 ] = _mm256_add_epi32(v[12], v[14]);    \
        v[0 ] = _mm256_hadd_epi32(v[0], v[1]);     \
        v[2 ] = _mm256_hadd_epi32(v[2], v[3]);     \
        d = _mm256_hadd_epi32(v[0], v[2]);                        // t[0][0-3], t[1][0-3] (dst[0-3] = t[0][0-3] + t[1][0-3])

        CALCU_O(coeffs[42], coeffs[43], coeffs[44], coeffs[45], d0);
        CALCU_O(coeffs[46], coeffs[47], coeffs[48], coeffs[49], d1);
        CALCU_O(coeffs[50], coeffs[51], coeffs[52], coeffs[53], d2);
        CALCU_O(coeffs[54], coeffs[55], coeffs[56], coeffs[57], d3);
        CALCU_O(coeffs[58], coeffs[59], coeffs[60], coeffs[61], d4);
        CALCU_O(coeffs[62], coeffs[63], coeffs[64], coeffs[65], d5);
        CALCU_O(coeffs[66], coeffs[67], coeffs[68], coeffs[69], d6);
        CALCU_O(coeffs[70], coeffs[71], coeffs[72], coeffs[73], d7);

        t[0] = _mm256_permute2x128_si256(d0, d1, 0x20);
        t[1] = _mm256_permute2x128_si256(d0, d1, 0x31);
        t[2] = _mm256_permute2x128_si256(d2, d3, 0x20);
        t[3] = _mm256_permute2x128_si256(d2, d3, 0x31);
        t[4] = _mm256_permute2x128_si256(d4, d5, 0x20);
        t[5] = _mm256_permute2x128_si256(d4, d5, 0x31);
        t[6] = _mm256_permute2x128_si256(d6, d7, 0x20);
        t[7] = _mm256_permute2x128_si256(d6, d7, 0x31);

        d0 = _mm256_add_epi32(t[0], t[1]);
        d1 = _mm256_add_epi32(t[2], t[3]);
        d2 = _mm256_add_epi32(t[4], t[5]);
        d3 = _mm256_add_epi32(t[6], t[7]);

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);
        d2 = _mm256_add_epi32(d2, add);
        d3 = _mm256_add_epi32(d3, add);

        d0 = _mm256_srai_epi32(d0, shift);                          // dst[1][0-3], dst[3][0-3]
        d1 = _mm256_srai_epi32(d1, shift);                          // dst[5][0-3], dst[7][0-3]
        d2 = _mm256_srai_epi32(d2, shift);
        d3 = _mm256_srai_epi32(d3, shift);

        d0 = _mm256_packs_epi32(d0, d1);
        d1 = _mm256_packs_epi32(d2, d3);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        m4 = _mm256_castsi256_si128(d1);
        m5 = _mm256_extracti128_si256(d1, 1);
        m6 = _mm_srli_si128(m4, 8);
        m7 = _mm_srli_si128(m5, 8);

        _mm_storel_epi64((__m128i*)(dst + 1 * line), m0);
        _mm_storel_epi64((__m128i*)(dst + 3 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 5 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 7 * line), m3);
        _mm_storel_epi64((__m128i*)(dst + 9 * line), m4);
        _mm_storel_epi64((__m128i*)(dst + 11 * line), m5);
        _mm_storel_epi64((__m128i*)(dst + 13 * line), m6);
        _mm_storel_epi64((__m128i*)(dst + 15 * line), m7);

        CALCU_O(coeffs[74], coeffs[75], coeffs[76], coeffs[77], d0);
        CALCU_O(coeffs[78], coeffs[79], coeffs[80], coeffs[81], d1);
        CALCU_O(coeffs[82], coeffs[83], coeffs[84], coeffs[85], d2);
        CALCU_O(coeffs[86], coeffs[87], coeffs[88], coeffs[89], d3);
        CALCU_O(coeffs[90], coeffs[91], coeffs[92], coeffs[93], d4);
        CALCU_O(coeffs[94], coeffs[95], coeffs[96], coeffs[97], d5);
        CALCU_O(coeffs[98], coeffs[99], coeffs[100], coeffs[101], d6);
        CALCU_O(coeffs[102], coeffs[103], coeffs[104], coeffs[105], d7);

#undef CALCU_O

        t[0] = _mm256_permute2x128_si256(d0, d1, 0x20);
        t[1] = _mm256_permute2x128_si256(d0, d1, 0x31);
        t[2] = _mm256_permute2x128_si256(d2, d3, 0x20);
        t[3] = _mm256_permute2x128_si256(d2, d3, 0x31);
        t[4] = _mm256_permute2x128_si256(d4, d5, 0x20);
        t[5] = _mm256_permute2x128_si256(d4, d5, 0x31);
        t[6] = _mm256_permute2x128_si256(d6, d7, 0x20);
        t[7] = _mm256_permute2x128_si256(d6, d7, 0x31);

        d0 = _mm256_add_epi32(t[0], t[1]);
        d1 = _mm256_add_epi32(t[2], t[3]);
        d2 = _mm256_add_epi32(t[4], t[5]);
        d3 = _mm256_add_epi32(t[6], t[7]);

        d0 = _mm256_add_epi32(d0, add);
        d1 = _mm256_add_epi32(d1, add);
        d2 = _mm256_add_epi32(d2, add);
        d3 = _mm256_add_epi32(d3, add);

        d0 = _mm256_srai_epi32(d0, shift);                          // dst[17][0-3], dst[19][0-3]
        d1 = _mm256_srai_epi32(d1, shift);                          // dst[21][0-3], dst[23][0-3]
        d2 = _mm256_srai_epi32(d2, shift);
        d3 = _mm256_srai_epi32(d3, shift);

        d0 = _mm256_packs_epi32(d0, d1);
        d1 = _mm256_packs_epi32(d2, d3);

        m0 = _mm256_castsi256_si128(d0);
        m1 = _mm256_extracti128_si256(d0, 1);
        m2 = _mm_srli_si128(m0, 8);
        m3 = _mm_srli_si128(m1, 8);
        m4 = _mm256_castsi256_si128(d1);
        m5 = _mm256_extracti128_si256(d1, 1);
        m6 = _mm_srli_si128(m4, 8);
        m7 = _mm_srli_si128(m5, 8);

        _mm_storel_epi64((__m128i*)(dst + 17 * line), m0);
        _mm_storel_epi64((__m128i*)(dst + 19 * line), m1);
        _mm_storel_epi64((__m128i*)(dst + 21 * line), m2);
        _mm_storel_epi64((__m128i*)(dst + 23 * line), m3);
        _mm_storel_epi64((__m128i*)(dst + 25 * line), m4);
        _mm_storel_epi64((__m128i*)(dst + 27 * line), m5);
        _mm_storel_epi64((__m128i*)(dst + 29 * line), m6);
        _mm_storel_epi64((__m128i*)(dst + 31 * line), m7);

        dst += 4;
    }
}

void uavs3e_trans_dct2_w4_h4_avx2_(s16 *src, s16 *dst, int bit_depth)
{
    __m256i s0;
    __m256i v0, v1, v2, v3;
    __m256i d0, d1;
    __m256i tab0;
    const __m256i coeff0 = _mm256_set_epi16(17, 42, 17, 42, 17, 42, 17, 42, 32, 32, 32, 32, 32, 32, 32, 32);
    const __m256i coeff1 = _mm256_set_epi16(-42, 17, -42, 17, -42, 17, -42, 17, -32, 32, -32, 32, -32, 32, -32, 32);
    const __m256i coeff2 = _mm256_set1_epi16(32);       // E
    const __m256i coeff3 = _mm256_set_epi16(32, -32, -32, 32, 32, -32, -32, 32, 32, -32, -32, 32, 32, -32, -32, 32);
    const __m256i coeff4 = _mm256_set_epi16(-42, -17, 17, 42, -42, -17, 17, 42, -42, -17, 17, 42, -42, -17, 17, 42);    // O
    const __m256i coeff5 = _mm256_set_epi16(-17, 42, -42, 17, -17, 42, -42, 17, -17, 42, -42, 17, -17, 42, -42, 17);
    __m256i add = _mm256_set1_epi32(1 << (7 - 1));
    __m256i sign = _mm256_set_epi16(-1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1);

    tab0 = _mm256_loadu_si256((__m256i*)tab_dct2_1st_shuffle_256i[2]);  // 16bit: 0, 3, 1, 2, 4, 7, 5, 6, 0, 3, 1, 2, 4, 7, 5, 6

    s0 = _mm256_loadu_si256((__m256i*)(src));           // src[0][0-4], src[1][0-4], src[2][0-4], src[3][0-4]

    v0 = _mm256_shuffle_epi8(s0, tab0);                 // src[0][0, 3, 1, 2], src[1][0, 3, 1, 2], src[2][0, 3, 1, 2], src[3][0, 3, 1, 2]
    v1 = _mm256_sign_epi16(v0, sign);

    v0 = _mm256_hadd_epi16(v0, v1);                     // e[0][0, 1], e[1][0, 1], o[0][0, 1], o[1][0, 1], e[2][0, 1], e[3][0, 1], o[2][0, 1], o[3][0, 1]
    v0 = _mm256_permute4x64_epi64(v0, 0xd8);            // e[0-3][0, 1], o[0-3][0, 1]

    d0 = _mm256_madd_epi16(v0, coeff0);                 // dst[0][0-4], dst[1][0-4]
    d1 = _mm256_madd_epi16(v0, coeff1);                 // dst[2][0-4], dst[3][0-4]

    d0 = _mm256_packs_epi32(d0, d1);

    s0 = _mm256_permute4x64_epi64(d0, 0xd8);

    v0 = _mm256_madd_epi16(s0, coeff2);
    v1 = _mm256_madd_epi16(s0, coeff3);
    v2 = _mm256_madd_epi16(s0, coeff4);
    v3 = _mm256_madd_epi16(s0, coeff5);

    v0 = _mm256_hadd_epi32(v0, v1);                     // dst[0][0-1], dst[2][0-1], dst[0][2-3], dst[2][2-3]
    v1 = _mm256_hadd_epi32(v2, v3);                     // dst[1][0-1], dst[3][0-1], dst[1][2-3], dst[3][2-3]

    v0 = _mm256_permute4x64_epi64(v0, 0xd8);            // dst[0][0-3], dst[2][0-3]
    v1 = _mm256_permute4x64_epi64(v1, 0xd8);

    v0 = _mm256_add_epi32(v0, add);
    v1 = _mm256_add_epi32(v1, add);

    v0 = _mm256_srai_epi32(v0, 7);
    v1 = _mm256_srai_epi32(v1, 7);

    d0 = _mm256_packs_epi32(v0, v1);

    _mm256_storeu_si256((__m256i*)dst, d0);
}

void uavs3e_trans_dct2_w4_h4_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[4 * 4]);
    tx_dct2_pb4_1st_avx2(src, tmp, 4, 4, 0 + bit_depth - 8);
    tx_dct2_pb4_2nd_avx2(tmp, dst, 4, 4, 7);
}

void uavs3e_trans_dct2_w4_h8_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[4 * 8]);
    tx_dct2_pb4_1st_avx2(src, tmp, 8, 8, 0 + bit_depth - 8);
    tx_dct2_pb8_2nd_avx2(tmp, dst, 4, 4, 8);
}

void uavs3e_trans_dct2_w4_h16_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[4 * 16]);
    tx_dct2_pb4_1st_avx2(src, tmp, 16, 16, 0 + bit_depth - 8);
    tx_dct2_pb16_2nd_avx2(tmp, dst, 4, 4, 9);
}

void uavs3e_trans_dct2_w4_h32_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[4 * 32]);
    tx_dct2_pb4_1st_avx2(src, tmp, 32, 32, 0 + bit_depth - 8);
    tx_dct2_pb32_2nd_avx2(tmp, dst, 4, 4, 10);
}

void uavs3e_trans_dct2_w8_h4_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8 * 4]);
    tx_dct2_pb8_1st_avx2(src, tmp, 4, 4, 1 + bit_depth - 8);
    tx_dct2_pb4_2nd_avx2(tmp, dst, 8, 8, 7);
}

void uavs3e_trans_dct2_w8_h8_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8 * 8]);
    tx_dct2_pb8_1st_avx2(src, tmp, 8, 8, 1 + bit_depth - 8);
    tx_dct2_pb8_2nd_avx2(tmp, dst, 8, 8, 8);
}

void uavs3e_trans_dct2_w8_h16_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8 * 16]);
    tx_dct2_pb8_1st_avx2(src, tmp, 16, 16, 1 + bit_depth - 8);
    tx_dct2_pb16_2nd_avx2(tmp, dst, 8, 8, 9);
}

void uavs3e_trans_dct2_w8_h32_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8 * 32]);
    tx_dct2_pb8_1st_avx2(src, tmp, 32, 32, 1 + bit_depth - 8);
    tx_dct2_pb32_2nd_avx2(tmp, dst, 8, 8, 10);
}

void uavs3e_trans_dct2_w8_h64_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8 * 64]);
    tx_dct2_pb8_1st_avx2(src, tmp, 64, 64, 1 + bit_depth - 8);
    tx_dct2_pb64_2nd_avx2(tmp, dst, 8, 8, 11);
}

void uavs3e_trans_dct2_w16_h4_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16 * 4]);
    tx_dct2_pb16_1st_avx2(src, tmp, 4, 4, 2 + bit_depth - 8);
    tx_dct2_pb4_2nd_avx2(tmp, dst, 16, 16, 7);
}

void uavs3e_trans_dct2_w16_h8_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16 * 8]);
    tx_dct2_pb16_1st_avx2(src, tmp, 8, 8, 2 + bit_depth - 8);
    tx_dct2_pb8_2nd_avx2(tmp, dst, 16, 16, 8);
}

void uavs3e_trans_dct2_w16_h16_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16 * 16]);
    tx_dct2_pb16_1st_avx2(src, tmp, 16, 16, 2 + bit_depth - 8);
    tx_dct2_pb16_2nd_avx2(tmp, dst, 16, 16, 9);
}

void uavs3e_trans_dct2_w16_h32_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16 * 32]);
    tx_dct2_pb16_1st_avx2(src, tmp, 32, 32, 2 + bit_depth - 8);
    tx_dct2_pb32_2nd_avx2(tmp, dst, 16, 16, 10);
}

void uavs3e_trans_dct2_w16_h64_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16 * 64]);
    tx_dct2_pb16_1st_avx2(src, tmp, 64, 64, 2 + bit_depth - 8);
    tx_dct2_pb64_2nd_avx2(tmp, dst, 16, 16, 11);
}

void uavs3e_trans_dct2_w32_h4_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32 * 4]);
    tx_dct2_pb32_1st_avx2(src, tmp, 4, 4, 3 + bit_depth - 8);
    tx_dct2_pb4_2nd_avx2(tmp, dst, 32, 32, 7);
}

void uavs3e_trans_dct2_w32_h8_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32 * 8]);
    tx_dct2_pb32_1st_avx2(src, tmp, 8, 8, 3 + bit_depth - 8);
    tx_dct2_pb8_2nd_avx2(tmp, dst, 32, 32, 8);
}

void uavs3e_trans_dct2_w32_h16_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32 * 16]);
    tx_dct2_pb32_1st_avx2(src, tmp, 16, 16, 3 + bit_depth - 8);
    tx_dct2_pb16_2nd_avx2(tmp, dst, 32, 32, 9);
}

void uavs3e_trans_dct2_w32_h32_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32 * 32]);
    tx_dct2_pb32_1st_avx2(src, tmp, 32, 32, 3 + bit_depth - 8);
    tx_dct2_pb32_2nd_avx2(tmp, dst, 32, 32, 10);
}

void uavs3e_trans_dct2_w32_h64_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32 * 64]);
    tx_dct2_pb32_1st_avx2(src, tmp, 64, 64, 3 + bit_depth - 8);
    tx_dct2_pb64_2nd_avx2(tmp, dst, 32, 32, 11);
}

void uavs3e_trans_dct2_w64_h8_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[64 * 8]);
    tx_dct2_pb64_1st_avx2(src, tmp, 8, 8, 4 + bit_depth - 8);
    tx_dct2_pb8_2nd_avx2(tmp, dst, 64, 32, 8);
}

void uavs3e_trans_dct2_w64_h16_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[64 * 16]);
    tx_dct2_pb64_1st_avx2(src, tmp, 16, 16, 4 + bit_depth - 8);
    tx_dct2_pb16_2nd_avx2(tmp, dst, 64, 32, 9);
}

void uavs3e_trans_dct2_w64_h32_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[64 * 32]);
    tx_dct2_pb64_1st_avx2(src, tmp, 32, 32, 4 + bit_depth - 8);
    tx_dct2_pb32_2nd_avx2(tmp, dst, 64, 32, 10);
}

void uavs3e_trans_dct2_w64_h64_avx2(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[64 * 64]);
    tx_dct2_pb64_1st_avx2(src, tmp, 64, 64, 4 + bit_depth - 8);
    tx_dct2_pb64_2nd_avx2(tmp, dst, 64, 32, 11);
}


ALIGNED_32(static const s16 tab_dct8_pb4_coeffs_256i[][16]) = {
    // iT[2]+iT[3], iT[1], iT[2], iT[3]
    { 42, 37, 27, 15, 42, 37, 27, 15, 42, 37, 27, 15, 42, 37, 27, 15 },
    // iT[1], 0, -iT[1], -iT[1]
    { 37, 0, -37, -37, 37, 0, -37, -37, 37, 0, -37, -37, 37, 0, -37, -37 },
    // iT[2], -iT[1], -iT[3], iT[2]+iT[3]
    { 27, -37, -15, 42, 27, -37, -15, 42, 27, -37, -15, 42, 27, -37, -15, 42 },
    // iT[3], -iT[1], iT[2]+iT[3], -iT[2]
    { 15, -37, 42, -27, 15, -37, 42, -27, 15, -37, 42, -27, 15, -37, 42, -27 }
};

ALIGNED_32(static const s16 tab_dct8_pb8_coeffs_256i[][16]) = {
    { 44, 42, 39, 35, 30, 23, 16, 8, 44, 42, 39, 35, 30, 23, 16, 8 },
    { 42, 30, 8, -16, -35, -44, -39, -23, 42, 30, 8, -16, -35, -44, -39, -23 },
    { 39, 8, -30, -44, -23, 16, 42, 35, 39, 8, -30, -44, -23, 16, 42, 35 },
    { 35, -16, -44, -8, 39, 30, -23, -42, 35, -16, -44, -8, 39, 30, -23, -42 },
    { 30, -35, -23, 39, 16, -42, -8, 44, 30, -35, -23, 39, 16, -42, -8, 44 },
    { 23, -44, 16, 30, -42, 8, 35, -39, 23, -44, 16, 30, -42, 8, 35, -39 },
    { 16, -39, 42, -23, -8, 35, -44, 30, 16, -39, 42, -23, -8, 35, -44, 30 },
    { 8, -23, 35, -42, 44, -39, 30, -16, 8, -23, 35, -42, 44, -39, 30, -16 }
};

ALIGNED_32(static const s16 tab_dct8_pb16_coeffs_256i[][16]) = {
    { 45, 44, 43, 42, 41, 39, 36, 34, 31, 28, 24, 20, 17, 13, 8, 4 },
    { 44, 41, 34, 24, 13, 0, -13, -24, -34, -41, -44, -44, -41, -34, -24, -13 },
    { 43, 34, 17, -4, -24, -39, -45, -41, -28, -8, 13, 31, 42, 44, 36, 20 },
    { 42, 24, -4, -31, -44, -39, -17, 13, 36, 45, 34, 8, -20, -41, -43, -28 },
    { 41, 13, -24, -44, -34, 0, 34, 44, 24, -13, -41, -41, -13, 24, 44, 34 },
    { 39, 0, -39, -39, 0, 39, 39, 0, -39, -39, 0, 39, 39, 0, -39, -39 },
    { 36, -13, -45, -17, 34, 39, -8, -44, -20, 31, 41, -4, -43, -24, 28, 42 },
    { 34, -24, -41, 13, 44, 0, -44, -13, 41, 24, -34, -34, 24, 41, -13, -44 },
    { 31, -34, -28, 36, 24, -39, -20, 41, 17, -42, -13, 43, 8, -44, -4, 45 },
    { 28, -41, -8, 45, -13, -39, 31, 24, -42, -4, 44, -17, -36, 34, 20, -43 },
    { 24, -44, 13, 34, -41, 0, 41, -34, -13, 44, -24, -24, 44, -13, -34, 41 },
    { 20, -44, 31, 8, -41, 39, -4, -34, 43, -17, -24, 45, -28, -13, 42, -36 },
    { 17, -41, 42, -20, -13, 39, -43, 24, 8, -36, 44, -28, -4, 34, -45, 31 },
    { 13, -34, 44, -41, 24, 0, -24, 41, -44, 34, -13, -13, 34, -44, 41, -24 },
    { 8, -24, 36, -43, 44, -39, 28, -13, -4, 20, -34, 42, -45, 41, -31, 17 },
    { 4, -13, 20, -28, 34, -39, 42, -44, 45, -43, 41, -36, 31, -24, 17, -8 }
};

ALIGNED_32(static const s16 tab_dst7_pb4_coeffs_256i[][16]) = {
    // iT[0], iT[1], iT[2], iT[0]+iT[1]
    { 15, 27, 37, 42, 15, 27, 37, 42, 15, 27, 37, 42, 15, 27, 37, 42 },
    // iT[2], iT[2], 0, -iT[1]
    { 37, 37, 0, -37, 37, 37, 0, -37, 37, 37, 0, -37, 37, 37, 0, -37 },
    // iT[0]+iT[1], -iT[0], -iT[2], iT[1]
    { 42, -15, -37, 27, 42, -15, -37, 27, 42, -15, -37, 27, 42, -15, -37, 27 },
    // iT[1], -iT[0]-iT[1], iT[2], -iT[0]
    { 27, -42, 37, -15, 27, -42, 37, -15, 27, -42, 37, -15, 27, -42, 37, -15 }
};

ALIGNED_32(static const s16 tab_dst7_pb8_coeffs_256i[][16]) = {
    { 8, 16, 23, 30, 35, 39, 42, 44, 8, 16, 23, 30, 35, 39, 42, 44 },
    { 23, 39, 44, 35, 16, -8, -30, -42, 23, 39, 44, 35, 16, -8, -30, -42 },
    { 35, 42, 16, -23, -44, -30, 8, 39, 35, 42, 16, -23, -44, -30, 8, 39 },
    { 42, 23, -30, -39, 8, 44, 16, -35, 42, 23, -30, -39, 8, 44, 16, -35 },
    { 44, -8, -42, 16, 39, -23, -35, 30, 44, -8, -42, 16, 39, -23, -35, 30 },
    { 39, -35, -8, 42, -30, -16, 44, -23, 39, -35, -8, 42, -30, -16, 44, -23 },
    { 30, -44, 35, -8, -23, 42, -39, 16, 30, -44, 35, -8, -23, 42, -39, 16 },
    { 16, -30, 39, -44, 42, -35, 23, -8, 16, -30, 39, -44, 42, -35, 23, -8 }
};

ALIGNED_32(static const s16 tab_dst7_pb16_coeffs_256i[][16]) = {
    { 4, 8, 13, 17, 20, 24, 28, 31, 34, 36, 39, 41, 42, 43, 44, 45, },
    { 13, 24, 34, 41, 44, 44, 41, 34, 24, 13, 0, -13, -24, -34, -41, -44, },
    { 20, 36, 44, 42, 31, 13, -8, -28, -41, -45, -39, -24, -4, 17, 34, 43, },
    { 28, 43, 41, 20, -8, -34, -45, -36, -13, 17, 39, 44, 31, 4, -24, -42, },
    { 34, 44, 24, -13, -41, -41, -13, 24, 44, 34, 0, -34, -44, -24, 13, 41, },
    { 39, 39, 0, -39, -39, 0, 39, 39, 0, -39, -39, 0, 39, 39, 0, -39, },
    { 42, 28, -24, -43, -4, 41, 31, -20, -44, -8, 39, 34, -17, -45, -13, 36, },
    { 44, 13, -41, -24, 34, 34, -24, -41, 13, 44, 0, -44, -13, 41, 24, -34, },
    { 45, -4, -44, 8, 43, -13, -42, 17, 41, -20, -39, 24, 36, -28, -34, 31, },
    { 43, -20, -34, 36, 17, -44, 4, 42, -24, -31, 39, 13, -45, 8, 41, -28, },
    { 41, -34, -13, 44, -24, -24, 44, -13, -34, 41, 0, -41, 34, 13, -44, 24, },
    { 36, -42, 13, 28, -45, 24, 17, -43, 34, 4, -39, 41, -8, -31, 44, -20, },
    { 31, -45, 34, -4, -28, 44, -36, 8, 24, -43, 39, -13, -20, 42, -41, 17, },
    { 24, -41, 44, -34, 13, 13, -34, 44, -41, 24, 0, -24, 41, -44, 34, -13, },
    { 17, -31, 41, -45, 42, -34, 20, -4, -13, 28, -39, 44, -43, 36, -24, 8, },
    { 8, -17, 24, -31, 36, -41, 43, -45, 44, -42, 39, -34, 28, -20, 13, -4, }
};

void trans_dct8_pb4_avx2(s16 *src, s16 *dst, int shift, int line)
{
    int i = 0;
    __m256i s0;
    __m256i c0, c1, c2, c3;
    __m256i v0, v1, v2, v3;
    __m256i off;
    __m128i d0, d1, d2, d3;

    if (shift) {
        off = _mm256_set1_epi32(1 << (shift - 1));
    } else {
        off = _mm256_set1_epi32(0);
    }

    c0 = _mm256_loadu_si256((__m256i*)(tab_dct8_pb4_coeffs_256i[0]));
    c1 = _mm256_loadu_si256((__m256i*)(tab_dct8_pb4_coeffs_256i[1]));
    c2 = _mm256_loadu_si256((__m256i*)(tab_dct8_pb4_coeffs_256i[2]));
    c3 = _mm256_loadu_si256((__m256i*)(tab_dct8_pb4_coeffs_256i[3]));

    for (i; i < line; i += 4)
    {
        s0 = _mm256_loadu_si256((__m256i*)(src));   // src[0][0-3], src[1][0-3], src[2][0-3], src[3][0-3]

        v0 = _mm256_madd_epi16(s0, c0);
        v1 = _mm256_madd_epi16(s0, c1);
        v2 = _mm256_madd_epi16(s0, c2);
        v3 = _mm256_madd_epi16(s0, c3);

        v0 = _mm256_hadd_epi32(v0, v1);
        v2 = _mm256_hadd_epi32(v2, v3);

        v0 = _mm256_add_epi32(v0, off);
        v2 = _mm256_add_epi32(v2, off);

        v0 = _mm256_srai_epi32(v0, shift);          // dst[0][0-1], dst[1][0-1], dst[0][2-3], dst[1][2-3]
        v2 = _mm256_srai_epi32(v2, shift);

        v0 = _mm256_packs_epi32(v0, v2);            // dst[0][0-1], dst[1][0-1], dst[2][0-1], dst[3][0-1], dst[0][2-3], dst[1][2-3], dst[2][2-3], dst[3][2-3]

        d0 = _mm256_castsi256_si128(v0);
        d1 = _mm256_extracti128_si256(v0, 1);

        d2 = _mm_unpacklo_epi32(d0, d1);            // dst[0][0-3], dst[1][0-3]
        d3 = _mm_unpackhi_epi32(d0, d1);
        d0 = _mm_srli_si128(d2, 8);
        d1 = _mm_srli_si128(d3, 8);
        _mm_storel_epi64((__m128i*)(dst), d2);
        _mm_storel_epi64((__m128i*)(dst + line), d0);
        _mm_storel_epi64((__m128i*)(dst + line * 2), d3);
        _mm_storel_epi64((__m128i*)(dst + line * 3), d1);

        src += 16;
        dst += 4;
    }
}

void trans_dct8_pb8_avx2(s16 *src, s16 *dst, int shift, int line)
{
    __m256i s0, s1;
    __m256i c[8];
    __m256i v[16];
    __m256i off;
    __m128i d0, d1, d2, d3, d4, d5, d6, d7;
    int i, j;

    off = _mm256_set1_epi32(1 << (shift - 1));

    for (i = 0; i < 8; i++) {
        c[i] = _mm256_loadu_si256((__m256i*)(tab_dct8_pb8_coeffs_256i[i]));
    }

    for (i = 0; i < line; i += 4)
    {
        s0 = _mm256_loadu2_m128i((__m128i *)(src + 16), (__m128i *)(src));      // src[0][0-7], src[2][0-7]
        s1 = _mm256_loadu2_m128i((__m128i *)(src + 24), (__m128i *)(src + 8));  // src[1][0-7], src[3][0-7]

        for (j = 0; j < 8; j++) {
            v[j] = _mm256_madd_epi16(s0, c[j]);
            v[j + 8] = _mm256_madd_epi16(s1, c[j]);
        }

        v[0] = _mm256_hadd_epi32(v[0], v[8]);
        v[1] = _mm256_hadd_epi32(v[1], v[9]);
        v[2] = _mm256_hadd_epi32(v[2], v[10]);
        v[3] = _mm256_hadd_epi32(v[3], v[11]);
        v[4] = _mm256_hadd_epi32(v[4], v[12]);
        v[5] = _mm256_hadd_epi32(v[5], v[13]);
        v[6] = _mm256_hadd_epi32(v[6], v[14]);
        v[7] = _mm256_hadd_epi32(v[7], v[15]);

        v[0] = _mm256_hadd_epi32(v[0], v[1]);
        v[2] = _mm256_hadd_epi32(v[2], v[3]);
        v[4] = _mm256_hadd_epi32(v[4], v[5]);
        v[6] = _mm256_hadd_epi32(v[6], v[7]);

        v[0] = _mm256_add_epi32(v[0], off);
        v[2] = _mm256_add_epi32(v[2], off);
        v[4] = _mm256_add_epi32(v[4], off);
        v[6] = _mm256_add_epi32(v[6], off);

        v[0] = _mm256_srai_epi32(v[0], shift);      // dst[0][0-1], dst[1][0-1], dst[0][2-3], dst[1][2-3]
        v[2] = _mm256_srai_epi32(v[2], shift);
        v[4] = _mm256_srai_epi32(v[4], shift); 
        v[6] = _mm256_srai_epi32(v[6], shift);

        v[0] = _mm256_packs_epi32(v[0], v[2]);      // dst[0][0-1], dst[1][0-1], dst[2][0-1], dst[3][0-1], dst[0][2-3], dst[1][2-3], dst[2][2-3], dst[3][2-3]
        v[4] = _mm256_packs_epi32(v[4], v[6]);

        d4 = _mm256_castsi256_si128(v[0]);
        d5 = _mm256_extracti128_si256(v[0], 1);
        d6 = _mm256_castsi256_si128(v[4]);
        d7 = _mm256_extracti128_si256(v[4], 1);

        d0 = _mm_unpacklo_epi32(d4, d5);            // dst[0][0-3], dst[1][0-3]
        d1 = _mm_unpackhi_epi32(d4, d5);
        d2 = _mm_unpacklo_epi32(d6, d7);            // dst[4][0-3], dst[5][0-3]
        d3 = _mm_unpackhi_epi32(d6, d7);
        d4 = _mm_srli_si128(d0, 8);
        d5 = _mm_srli_si128(d1, 8);
        d6 = _mm_srli_si128(d2, 8);
        d7 = _mm_srli_si128(d3, 8);
        _mm_storel_epi64((__m128i*)(dst), d0);
        _mm_storel_epi64((__m128i*)(dst + line), d4);
        _mm_storel_epi64((__m128i*)(dst + line * 2), d1);
        _mm_storel_epi64((__m128i*)(dst + line * 3), d5);
        _mm_storel_epi64((__m128i*)(dst + line * 4), d2);
        _mm_storel_epi64((__m128i*)(dst + line * 5), d6);
        _mm_storel_epi64((__m128i*)(dst + line * 6), d3);
        _mm_storel_epi64((__m128i*)(dst + line * 7), d7);

        src += 8*4;
        dst += 4;
    }
}

void trans_dct8_pb16_avx2(s16 *src, s16 *dst, int shift, int line)
{
    __m256i s0, s1, s2, s3;
    __m256i c[16];
    __m256i v[32], t[8];
    __m256i off;
    __m128i d0, d1, d2, d3, d4, d5, d6, d7;
    int i, j, k;

    off = _mm256_set1_epi32(1 << (shift - 1));

    for (i = 0; i < 16; i++) {
        c[i] = _mm256_loadu_si256((__m256i*)(tab_dct8_pb16_coeffs_256i[i]));
    }

    for (i = 0; i < line; i += 4)
    {
        s16* pdst = dst;

        s0 = _mm256_loadu_si256((__m256i *)(src));          // src[0][0-15]
        s1 = _mm256_loadu_si256((__m256i *)(src + 16));     // src[1][0-15]
        s2 = _mm256_loadu_si256((__m256i *)(src + 32));
        s3 = _mm256_loadu_si256((__m256i *)(src + 48));

        for (j = 0; j < 2; j++) {
            for (k = 0; k < 8; k++) {
                int cid = k + j * 8;
                v[k] = _mm256_madd_epi16(s0, c[cid]);
                v[k + 8] = _mm256_madd_epi16(s1, c[cid]);
                v[k + 16] = _mm256_madd_epi16(s2, c[cid]);
                v[k + 24] = _mm256_madd_epi16(s3, c[cid]);
            }

            v[0] = _mm256_hadd_epi32(v[0], v[8]);
            v[1] = _mm256_hadd_epi32(v[1], v[9]);
            v[2] = _mm256_hadd_epi32(v[2], v[10]);
            v[3] = _mm256_hadd_epi32(v[3], v[11]);
            v[4] = _mm256_hadd_epi32(v[4], v[12]);
            v[5] = _mm256_hadd_epi32(v[5], v[13]);
            v[6] = _mm256_hadd_epi32(v[6], v[14]);
            v[7] = _mm256_hadd_epi32(v[7], v[15]);

            v[16] = _mm256_hadd_epi32(v[16], v[24]);
            v[17] = _mm256_hadd_epi32(v[17], v[25]);
            v[18] = _mm256_hadd_epi32(v[18], v[26]);
            v[19] = _mm256_hadd_epi32(v[19], v[27]);
            v[20] = _mm256_hadd_epi32(v[20], v[28]);
            v[21] = _mm256_hadd_epi32(v[21], v[29]);
            v[22] = _mm256_hadd_epi32(v[22], v[30]);
            v[23] = _mm256_hadd_epi32(v[23], v[31]);

            v[0] = _mm256_hadd_epi32(v[0], v[1]);
            v[2] = _mm256_hadd_epi32(v[2], v[3]);
            v[4] = _mm256_hadd_epi32(v[4], v[5]);
            v[6] = _mm256_hadd_epi32(v[6], v[7]);

            v[16] = _mm256_hadd_epi32(v[16], v[17]);
            v[18] = _mm256_hadd_epi32(v[18], v[19]);
            v[20] = _mm256_hadd_epi32(v[20], v[21]);
            v[22] = _mm256_hadd_epi32(v[22], v[23]);

            t[0] = _mm256_permute2x128_si256(v[0], v[16], 0x20);
            t[1] = _mm256_permute2x128_si256(v[0], v[16], 0x31);
            t[2] = _mm256_permute2x128_si256(v[2], v[18], 0x20);
            t[3] = _mm256_permute2x128_si256(v[2], v[18], 0x31);
            t[4] = _mm256_permute2x128_si256(v[4], v[20], 0x20);
            t[5] = _mm256_permute2x128_si256(v[4], v[20], 0x31);
            t[6] = _mm256_permute2x128_si256(v[6], v[22], 0x20);
            t[7] = _mm256_permute2x128_si256(v[6], v[22], 0x31);

            v[0] = _mm256_add_epi32(t[0], t[1]);
            v[1] = _mm256_add_epi32(t[2], t[3]);
            v[2] = _mm256_add_epi32(t[4], t[5]);
            v[3] = _mm256_add_epi32(t[6], t[7]);

            v[0] = _mm256_add_epi32(v[0], off);
            v[1] = _mm256_add_epi32(v[1], off);
            v[2] = _mm256_add_epi32(v[2], off);
            v[3] = _mm256_add_epi32(v[3], off);

            v[0] = _mm256_srai_epi32(v[0], shift);      // dst[0][0-1], dst[1][0-1], dst[0][2-3], dst[1][2-3]
            v[1] = _mm256_srai_epi32(v[1], shift);
            v[2] = _mm256_srai_epi32(v[2], shift);
            v[3] = _mm256_srai_epi32(v[3], shift);

            v[0] = _mm256_packs_epi32(v[0], v[1]);      // dst[0][0-1], dst[1][0-1], dst[2][0-1], dst[3][0-1], dst[0][2-3], dst[1][2-3], dst[2][2-3], dst[3][2-3]
            v[2] = _mm256_packs_epi32(v[2], v[3]);

            d4 = _mm256_castsi256_si128(v[0]);
            d5 = _mm256_extracti128_si256(v[0], 1);
            d6 = _mm256_castsi256_si128(v[2]);
            d7 = _mm256_extracti128_si256(v[2], 1);

            d0 = _mm_unpacklo_epi32(d4, d5);            // dst[0][0-3], dst[1][0-3]
            d1 = _mm_unpackhi_epi32(d4, d5);
            d2 = _mm_unpacklo_epi32(d6, d7);            // dst[4][0-3], dst[5][0-3]
            d3 = _mm_unpackhi_epi32(d6, d7);
            d4 = _mm_srli_si128(d0, 8);
            d5 = _mm_srli_si128(d1, 8);
            d6 = _mm_srli_si128(d2, 8);
            d7 = _mm_srli_si128(d3, 8);
            _mm_storel_epi64((__m128i*)(pdst), d0);
            _mm_storel_epi64((__m128i*)(pdst + line), d4);
            _mm_storel_epi64((__m128i*)(pdst + line * 2), d1);
            _mm_storel_epi64((__m128i*)(pdst + line * 3), d5);
            _mm_storel_epi64((__m128i*)(pdst + line * 4), d2);
            _mm_storel_epi64((__m128i*)(pdst + line * 5), d6);
            _mm_storel_epi64((__m128i*)(pdst + line * 6), d3);
            _mm_storel_epi64((__m128i*)(pdst + line * 7), d7);

            pdst += line * 8;
        }
        src += 16*4;
        dst += 4;
    }
}

void trans_dst7_pb4_avx2(s16 *src, s16 *dst, int shift, int line)
{
    int i = 0;
    __m256i s0;
    __m256i c0, c1, c2, c3;
    __m256i v0, v1, v2, v3;
    __m256i off;
    __m128i d0, d1, d2, d3;

    if (shift) {
        off = _mm256_set1_epi32(1 << (shift - 1));
    }
    else {
        off = _mm256_set1_epi32(0);
    }

    c0 = _mm256_loadu_si256((__m256i*)(tab_dst7_pb4_coeffs_256i[0]));
    c1 = _mm256_loadu_si256((__m256i*)(tab_dst7_pb4_coeffs_256i[1]));
    c2 = _mm256_loadu_si256((__m256i*)(tab_dst7_pb4_coeffs_256i[2]));
    c3 = _mm256_loadu_si256((__m256i*)(tab_dst7_pb4_coeffs_256i[3]));

    for (i; i < line; i += 4)
    {
        s0 = _mm256_loadu_si256((__m256i*)(src));   // src[0][0-3], src[1][0-3], src[2][0-3], src[3][0-3]

        v0 = _mm256_madd_epi16(s0, c0);
        v1 = _mm256_madd_epi16(s0, c1);
        v2 = _mm256_madd_epi16(s0, c2);
        v3 = _mm256_madd_epi16(s0, c3);

        v0 = _mm256_hadd_epi32(v0, v1);
        v2 = _mm256_hadd_epi32(v2, v3);

        v0 = _mm256_add_epi32(v0, off);
        v2 = _mm256_add_epi32(v2, off);

        v0 = _mm256_srai_epi32(v0, shift);          // dst[0][0-1], dst[1][0-1], dst[0][2-3], dst[1][2-3]
        v2 = _mm256_srai_epi32(v2, shift);

        v0 = _mm256_packs_epi32(v0, v2);            // dst[0][0-1], dst[1][0-1], dst[2][0-1], dst[3][0-1], dst[0][2-3], dst[1][2-3], dst[2][2-3], dst[3][2-3]

        d0 = _mm256_castsi256_si128(v0);
        d1 = _mm256_extracti128_si256(v0, 1);

        d2 = _mm_unpacklo_epi32(d0, d1);            // dst[0][0-3], dst[1][0-3]
        d3 = _mm_unpackhi_epi32(d0, d1);
        d0 = _mm_srli_si128(d2, 8);
        d1 = _mm_srli_si128(d3, 8);
        _mm_storel_epi64((__m128i*)(dst), d2);
        _mm_storel_epi64((__m128i*)(dst + line), d0);
        _mm_storel_epi64((__m128i*)(dst + line * 2), d3);
        _mm_storel_epi64((__m128i*)(dst + line * 3), d1);

        src += 16;
        dst += 4;
    }
}

void trans_dst7_pb8_avx2(s16 *src, s16 *dst, int shift, int line)
{
    __m256i s0, s1;
    __m256i c[8];
    __m256i v[16];
    __m256i off;
    __m128i d0, d1, d2, d3, d4, d5, d6, d7;
    int i, j;

    off = _mm256_set1_epi32(1 << (shift - 1));

    for (i = 0; i < 8; i++) {
        c[i] = _mm256_loadu_si256((__m256i*)(tab_dst7_pb8_coeffs_256i[i]));
    }

    for (i = 0; i < line; i += 4)
    {
        s0 = _mm256_loadu2_m128i((__m128i *)(src + 16), (__m128i *)(src));      // src[0][0-7], src[2][0-7]
        s1 = _mm256_loadu2_m128i((__m128i *)(src + 24), (__m128i *)(src + 8));  // src[1][0-7], src[3][0-7]

        for (j = 0; j < 8; j++) {
            v[j] = _mm256_madd_epi16(s0, c[j]);
            v[j + 8] = _mm256_madd_epi16(s1, c[j]);
        }

        v[0] = _mm256_hadd_epi32(v[0], v[8]);
        v[1] = _mm256_hadd_epi32(v[1], v[9]);
        v[2] = _mm256_hadd_epi32(v[2], v[10]);
        v[3] = _mm256_hadd_epi32(v[3], v[11]);
        v[4] = _mm256_hadd_epi32(v[4], v[12]);
        v[5] = _mm256_hadd_epi32(v[5], v[13]);
        v[6] = _mm256_hadd_epi32(v[6], v[14]);
        v[7] = _mm256_hadd_epi32(v[7], v[15]);

        v[0] = _mm256_hadd_epi32(v[0], v[1]);
        v[2] = _mm256_hadd_epi32(v[2], v[3]);
        v[4] = _mm256_hadd_epi32(v[4], v[5]);
        v[6] = _mm256_hadd_epi32(v[6], v[7]);

        v[0] = _mm256_add_epi32(v[0], off);
        v[2] = _mm256_add_epi32(v[2], off);
        v[4] = _mm256_add_epi32(v[4], off);
        v[6] = _mm256_add_epi32(v[6], off);

        v[0] = _mm256_srai_epi32(v[0], shift);      // dst[0][0-1], dst[1][0-1], dst[0][2-3], dst[1][2-3]
        v[2] = _mm256_srai_epi32(v[2], shift);
        v[4] = _mm256_srai_epi32(v[4], shift);
        v[6] = _mm256_srai_epi32(v[6], shift);

        v[0] = _mm256_packs_epi32(v[0], v[2]);      // dst[0][0-1], dst[1][0-1], dst[2][0-1], dst[3][0-1], dst[0][2-3], dst[1][2-3], dst[2][2-3], dst[3][2-3]
        v[4] = _mm256_packs_epi32(v[4], v[6]);

        d4 = _mm256_castsi256_si128(v[0]);
        d5 = _mm256_extracti128_si256(v[0], 1);
        d6 = _mm256_castsi256_si128(v[4]);
        d7 = _mm256_extracti128_si256(v[4], 1);

        d0 = _mm_unpacklo_epi32(d4, d5);            // dst[0][0-3], dst[1][0-3]
        d1 = _mm_unpackhi_epi32(d4, d5);
        d2 = _mm_unpacklo_epi32(d6, d7);            // dst[4][0-3], dst[5][0-3]
        d3 = _mm_unpackhi_epi32(d6, d7);
        d4 = _mm_srli_si128(d0, 8);
        d5 = _mm_srli_si128(d1, 8);
        d6 = _mm_srli_si128(d2, 8);
        d7 = _mm_srli_si128(d3, 8);
        _mm_storel_epi64((__m128i*)(dst), d0);
        _mm_storel_epi64((__m128i*)(dst + line), d4);
        _mm_storel_epi64((__m128i*)(dst + line * 2), d1);
        _mm_storel_epi64((__m128i*)(dst + line * 3), d5);
        _mm_storel_epi64((__m128i*)(dst + line * 4), d2);
        _mm_storel_epi64((__m128i*)(dst + line * 5), d6);
        _mm_storel_epi64((__m128i*)(dst + line * 6), d3);
        _mm_storel_epi64((__m128i*)(dst + line * 7), d7);

        src += 8 * 4;
        dst += 4;
    }
}

void trans_dst7_pb16_avx2(s16 *src, s16 *dst, int shift, int line)
{
    __m256i s0, s1, s2, s3;
    __m256i c[16];
    __m256i v[32], t[8];
    __m256i off;
    __m128i d0, d1, d2, d3, d4, d5, d6, d7;
    int i, j, k;

    for (i = 0; i < 16; i++) {
        c[i] = _mm256_loadu_si256((__m256i*)(tab_dst7_pb16_coeffs_256i[i]));
    }

    off = _mm256_set1_epi32(1 << (shift - 1));

    for (i = 0; i < line; i += 4)
    {
        s16* pdst = dst;

        s0 = _mm256_loadu_si256((__m256i *)(src));          // src[0][0-15]
        s1 = _mm256_loadu_si256((__m256i *)(src + 16));     // src[1][0-15]
        s2 = _mm256_loadu_si256((__m256i *)(src + 32));
        s3 = _mm256_loadu_si256((__m256i *)(src + 48));

        for (j = 0; j < 2; j++) {
            for (k = 0; k < 8; k++) {
                int cid = k + j * 8;
                v[k] = _mm256_madd_epi16(s0, c[cid]);
                v[k + 8] = _mm256_madd_epi16(s1, c[cid]);
                v[k + 16] = _mm256_madd_epi16(s2, c[cid]);
                v[k + 24] = _mm256_madd_epi16(s3, c[cid]);
            }

            v[0] = _mm256_hadd_epi32(v[0], v[8]);
            v[1] = _mm256_hadd_epi32(v[1], v[9]);
            v[2] = _mm256_hadd_epi32(v[2], v[10]);
            v[3] = _mm256_hadd_epi32(v[3], v[11]);
            v[4] = _mm256_hadd_epi32(v[4], v[12]);
            v[5] = _mm256_hadd_epi32(v[5], v[13]);
            v[6] = _mm256_hadd_epi32(v[6], v[14]);
            v[7] = _mm256_hadd_epi32(v[7], v[15]);

            v[16] = _mm256_hadd_epi32(v[16], v[24]);
            v[17] = _mm256_hadd_epi32(v[17], v[25]);
            v[18] = _mm256_hadd_epi32(v[18], v[26]);
            v[19] = _mm256_hadd_epi32(v[19], v[27]);
            v[20] = _mm256_hadd_epi32(v[20], v[28]);
            v[21] = _mm256_hadd_epi32(v[21], v[29]);
            v[22] = _mm256_hadd_epi32(v[22], v[30]);
            v[23] = _mm256_hadd_epi32(v[23], v[31]);

            v[0] = _mm256_hadd_epi32(v[0], v[1]);
            v[2] = _mm256_hadd_epi32(v[2], v[3]);
            v[4] = _mm256_hadd_epi32(v[4], v[5]);
            v[6] = _mm256_hadd_epi32(v[6], v[7]);

            v[16] = _mm256_hadd_epi32(v[16], v[17]);
            v[18] = _mm256_hadd_epi32(v[18], v[19]);
            v[20] = _mm256_hadd_epi32(v[20], v[21]);
            v[22] = _mm256_hadd_epi32(v[22], v[23]);

            t[0] = _mm256_permute2x128_si256(v[0], v[16], 0x20);
            t[1] = _mm256_permute2x128_si256(v[0], v[16], 0x31);
            t[2] = _mm256_permute2x128_si256(v[2], v[18], 0x20);
            t[3] = _mm256_permute2x128_si256(v[2], v[18], 0x31);
            t[4] = _mm256_permute2x128_si256(v[4], v[20], 0x20);
            t[5] = _mm256_permute2x128_si256(v[4], v[20], 0x31);
            t[6] = _mm256_permute2x128_si256(v[6], v[22], 0x20);
            t[7] = _mm256_permute2x128_si256(v[6], v[22], 0x31);

            v[0] = _mm256_add_epi32(t[0], t[1]);
            v[1] = _mm256_add_epi32(t[2], t[3]);
            v[2] = _mm256_add_epi32(t[4], t[5]);
            v[3] = _mm256_add_epi32(t[6], t[7]);

            v[0] = _mm256_add_epi32(v[0], off);
            v[1] = _mm256_add_epi32(v[1], off);
            v[2] = _mm256_add_epi32(v[2], off);
            v[3] = _mm256_add_epi32(v[3], off);

            v[0] = _mm256_srai_epi32(v[0], shift);      // dst[0][0-1], dst[1][0-1], dst[0][2-3], dst[1][2-3]
            v[1] = _mm256_srai_epi32(v[1], shift);
            v[2] = _mm256_srai_epi32(v[2], shift);
            v[3] = _mm256_srai_epi32(v[3], shift);

            v[0] = _mm256_packs_epi32(v[0], v[1]);      // dst[0][0-1], dst[1][0-1], dst[2][0-1], dst[3][0-1], dst[0][2-3], dst[1][2-3], dst[2][2-3], dst[3][2-3]
            v[2] = _mm256_packs_epi32(v[2], v[3]);

            d4 = _mm256_castsi256_si128(v[0]);
            d5 = _mm256_extracti128_si256(v[0], 1);
            d6 = _mm256_castsi256_si128(v[2]);
            d7 = _mm256_extracti128_si256(v[2], 1);

            d0 = _mm_unpacklo_epi32(d4, d5);            // dst[0][0-3], dst[1][0-3]
            d1 = _mm_unpackhi_epi32(d4, d5);
            d2 = _mm_unpacklo_epi32(d6, d7);            // dst[4][0-3], dst[5][0-3]
            d3 = _mm_unpackhi_epi32(d6, d7);
            d4 = _mm_srli_si128(d0, 8);
            d5 = _mm_srli_si128(d1, 8);
            d6 = _mm_srli_si128(d2, 8);
            d7 = _mm_srli_si128(d3, 8);
            _mm_storel_epi64((__m128i*)(pdst), d0);
            _mm_storel_epi64((__m128i*)(pdst + line), d4);
            _mm_storel_epi64((__m128i*)(pdst + line * 2), d1);
            _mm_storel_epi64((__m128i*)(pdst + line * 3), d5);
            _mm_storel_epi64((__m128i*)(pdst + line * 4), d2);
            _mm_storel_epi64((__m128i*)(pdst + line * 5), d6);
            _mm_storel_epi64((__m128i*)(pdst + line * 6), d3);
            _mm_storel_epi64((__m128i*)(pdst + line * 7), d7);

            pdst += line * 8;
        }
        src += 16 * 4;
        dst += 4;
    }
}
