/**************************************************************************************
 * Copyright (C) 2018-2019 uavs3e project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Open-Intelligence Open Source License V1.1.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Open-Intelligence Open Source License V1.1 for more details.
 *
 * You should have received a copy of the Open-Intelligence Open Source License V1.1
 * along with this program; if not, you can download it on:
 * http://www.aitisa.org.cn/uploadfile/2018/0910/20180910031548314.pdf
 *
 * For more information, contact us at rgwang@pkusz.edu.cn.
 **************************************************************************************/

#include "sse.h"

#if defined(_MSC_VER)
#pragma warning(disable: 4100)  // unreferenced formal parameter
#endif

static ALIGNED_16(tab_s8 tab_coeff_mode_3[4][16]) = {
    { 8 , 40, 56, 24, 8 , 40, 56, 24, 8 , 40, 56, 24, 8 , 40, 56, 24 },
    { 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16 },
    { 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8  },
    { 32, 64, 32, 0 , 32, 64, 32, 0 , 32, 64, 32, 0 , 32, 64, 32, 0  }
};
static tab_u8 tab_idx_mode_3[64] = { 2, 5, 8, 11, 13, 16, 19, 22, 24, 27, 30, 33, 35, 38, 41, 44, 46, 49, 52,
                                     55, 57, 60, 63, 66, 68, 71, 74, 77, 79, 82, 85, 88, 90, 93, 96, 99, 101, 104, 107, 110, 112, 115, 118,
                                     121, 123, 126, 129, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128
                                   };

static ALIGNED_16(tab_s16 tab_coeff_mode_3_10bit[4][4]) = {
    { 8, 40, 56, 24 },
    { 16, 48, 48, 16 },
    { 24, 56, 40, 8 },
    { 32, 64, 32, 0 }
};
static tab_u16 tab_idx_mode_3_10bit[64] = { 2, 5, 8, 11, 13, 16, 19, 22, 24, 27, 30, 33, 35, 38, 41, 44, 46, 49, 52,
        55, 57, 60, 63, 66, 68, 71, 74, 77, 79, 82, 85, 88, 90, 93, 96, 99, 101, 104, 107, 110, 112, 115, 118,
        121, 123, 126, 129, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128
                                          };

static ALIGNED_16(tab_s8 tab_coeff_mode_5[8][16]) = {
    { 20, 52, 44, 12, 20, 52, 44, 12, 20, 52, 44, 12, 20, 52, 44, 12 },
    { 8 , 40, 56, 24, 8 , 40, 56, 24, 8 , 40, 56, 24, 8 , 40, 56, 24 },
    { 28, 60, 36, 4 , 28, 60, 36, 4 , 28, 60, 36, 4 , 28, 60, 36, 4  },
    { 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16 },
    { 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28 },
    { 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8  },
    { 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20 },
    { 32, 64, 32, 0 , 32, 64, 32, 0 , 32, 64, 32, 0 , 32, 64, 32, 0  }
};
static tab_u8 tab_idx_mode_5[64] = {
    1, 2, 4, 5, 6, 8, 9, 11, 12, 13, 15, 16, 17, 19, 20, 22, 23, 24, 26, 27, 28, 30, 31,
    33, 34, 35, 37, 38, 39, 41, 42, 44, 45, 46, 48, 49, 50, 52, 53, 55, 56, 57, 59, 60,
    61, 63, 64, 66, 67, 68, 70, 71, 72, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 88
};

static ALIGNED_16(tab_s16 tab_coeff_mode_5_10bit[8][4]) = {
    { 20, 52, 44, 12 },
    { 8, 40, 56, 24 },
    { 28, 60, 36, 4 },
    { 16, 48, 48, 16 },
    { 4, 36, 60, 28 },
    { 24, 56, 40, 8 },
    { 12, 44, 52, 20 },
    { 32, 64, 32, 0 }
};
static tab_u16 tab_idx_mode_5_10bit[64] = {
    1, 2, 4, 5, 6, 8, 9, 11, 12, 13, 15, 16, 17, 19, 20, 22, 23, 24, 26, 27, 28, 30, 31,
    33, 34, 35, 37, 38, 39, 41, 42, 44, 45, 46, 48, 49, 50, 52, 53, 55, 56, 57, 59, 60,
    61, 63, 64, 66, 67, 68, 70, 71, 72, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 88
};

static ALIGNED_16(tab_s8 tab_coeff_mode_7[64][16]) = {
    { 9 , 41, 55, 23, 9 , 41, 55, 23, 9 , 41, 55, 23, 9 , 41, 55, 23 },
    { 18, 50, 46, 14, 18, 50, 46, 14, 18, 50, 46, 14, 18, 50, 46, 14 },
    { 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5  },
    { 3 , 35, 61, 29, 3 , 35, 61, 29, 3 , 35, 61, 29, 3 , 35, 61, 29 },
    { 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20 },
    { 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11 },
    { 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2  },
    { 6 , 38, 58, 26, 6 , 38, 58, 26, 6 , 38, 58, 26, 6 , 38, 58, 26 },
    { 15, 47, 49, 17, 15, 47, 49, 17, 15, 47, 49, 17, 15, 47, 49, 17 },
    { 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8  },
    { 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31 },
    { 9 , 41, 55, 23, 9 , 41, 55, 23, 9 , 41, 55, 23, 9 , 41, 55, 23 },
    { 18, 50, 46, 14, 18, 50, 46, 14, 18, 50, 46, 14, 18, 50, 46, 14 },
    { 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5  },
    { 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28 },
    { 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20 },
    { 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11 },
    { 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2  },
    { 7 , 39, 57, 25, 7 , 39, 57, 25, 7 , 39, 57, 25, 7 , 39, 57, 25 },
    { 15, 47, 49, 17, 15, 47, 49, 17, 15, 47, 49, 17, 15, 47, 49, 17 },
    { 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8  },
    { 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31 },
    { 10, 42, 54, 22, 10, 42, 54, 22, 10, 42, 54, 22, 10, 42, 54, 22 },
    { 18, 50, 46, 14, 18, 50, 46, 14, 18, 50, 46, 14, 18, 50, 46, 14 },
    { 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5  },
    { 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28 },
    { 13, 45, 51, 19, 13, 45, 51, 19, 13, 45, 51, 19, 13, 45, 51, 19 },
    { 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11 },
    { 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2  },
    { 7 , 39, 57, 25, 7 , 39, 57, 25, 7 , 39, 57, 25, 7 , 39, 57, 25 },
    { 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16 },
    { 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8  },
    { 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31 },
    { 10, 42, 54, 22, 10, 42, 54, 22, 10, 42, 54, 22, 10, 42, 54, 22 },
    { 19, 51, 45, 13, 19, 51, 45, 13, 19, 51, 45, 13, 19, 51, 45, 13 },
    { 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5  },
    { 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28 },
    { 13, 45, 51, 19, 13, 45, 51, 19, 13, 45, 51, 19, 13, 45, 51, 19 },
    { 22, 54, 42, 10, 22, 54, 42, 10, 22, 54, 42, 10, 22, 54, 42, 10 },
    { 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2  },
    { 7 , 39, 57, 25, 7 , 39, 57, 25, 7 , 39, 57, 25, 7 , 39, 57, 25 },
    { 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16 },
    { 25, 57, 39, 7 , 25, 57, 39, 7 , 25, 57, 39, 7 , 25, 57, 39, 7  },
    { 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31 },
    { 10, 42, 54, 22, 10, 42, 54, 22, 10, 42, 54, 22, 10, 42, 54, 22 },
    { 19, 51, 45, 13, 19, 51, 45, 13, 19, 51, 45, 13, 19, 51, 45, 13 },
    { 28, 60, 36, 4 , 28, 60, 36, 4 , 28, 60, 36, 4 , 28, 60, 36, 4  },
    { 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28 },
    { 13, 45, 51, 19, 13, 45, 51, 19, 13, 45, 51, 19, 13, 45, 51, 19 },
    { 22, 54, 42, 10, 22, 54, 42, 10, 22, 54, 42, 10, 22, 54, 42, 10 },
    { 31, 63, 33, 1 , 31, 63, 33, 1 , 31, 63, 33, 1 , 31, 63, 33, 1  },
    { 7 , 39, 57, 25, 7 , 39, 57, 25, 7 , 39, 57, 25, 7 , 39, 57, 25 },
    { 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16 },
    { 25, 57, 39, 7 , 25, 57, 39, 7 , 25, 57, 39, 7 , 25, 57, 39, 7  },
    { 2 , 34, 62, 30, 2 , 34, 62, 30, 2 , 34, 62, 30, 2 , 34, 62, 30 },
    { 10, 42, 54, 22, 10, 42, 54, 22, 10, 42, 54, 22, 10, 42, 54, 22 },
    { 19, 51, 45, 13, 19, 51, 45, 13, 19, 51, 45, 13, 19, 51, 45, 13 },
    { 28, 60, 36, 4 , 28, 60, 36, 4 , 28, 60, 36, 4 , 28, 60, 36, 4  },
    { 5 , 37, 59, 27, 5 , 37, 59, 27, 5 , 37, 59, 27, 5 , 37, 59, 27 },
    { 13, 45, 51, 19, 13, 45, 51, 19, 13, 45, 51, 19, 13, 45, 51, 19 },
    { 22, 54, 42, 10, 22, 54, 42, 10, 22, 54, 42, 10, 22, 54, 42, 10 },
    { 31, 63, 33, 1 , 31, 63, 33, 1 , 31, 63, 33, 1 , 31, 63, 33, 1  },
    { 8 , 40, 56, 24, 8 , 40, 56, 24, 8 , 40, 56, 24, 8 , 40, 56, 24 },
    { 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16 }
};
static tab_u8 tab_idx_mode_7[64] = {
    0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 7, 8, 9, 10, 10, 11, 12, 13, 13, 14, 15, 15, 16,
    17, 18, 18, 19, 20, 21, 21, 22, 23, 23, 24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 31,
    32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 39, 40, 41, 42, 42, 43, 44, 45, 45, 46
};

static ALIGNED_16(tab_s16 tab_coeff_mode_7_10bit[64][4]) = {
    { 9, 41, 55, 23 },
    { 18, 50, 46, 14 },
    { 27, 59, 37, 5 },
    { 3, 35, 61, 29 },
    { 12, 44, 52, 20 },
    { 21, 53, 43, 11 },
    { 30, 62, 34, 2 },
    { 6, 38, 58, 26 },
    { 15, 47, 49, 17 },
    { 24, 56, 40, 8 },
    { 1, 33, 63, 31 },
    { 9, 41, 55, 23 },
    { 18, 50, 46, 14 },
    { 27, 59, 37, 5 },
    { 4, 36, 60, 28 },
    { 12, 44, 52, 20 },
    { 21, 53, 43, 11 },
    { 30, 62, 34, 2 },
    { 7, 39, 57, 25 },
    { 15, 47, 49, 17 },
    { 24, 56, 40, 8 },
    { 1, 33, 63, 31 },
    { 10, 42, 54, 22 },
    { 18, 50, 46, 14 },
    { 27, 59, 37, 5 },
    { 4, 36, 60, 28 },
    { 13, 45, 51, 19 },
    { 21, 53, 43, 11 },
    { 30, 62, 34, 2 },
    { 7, 39, 57, 25 },
    { 16, 48, 48, 16 },
    { 24, 56, 40, 8 },
    { 1, 33, 63, 31 },
    { 10, 42, 54, 22 },
    { 19, 51, 45, 13 },
    { 27, 59, 37, 5 },
    { 4, 36, 60, 28 },
    { 13, 45, 51, 19 },
    { 22, 54, 42, 10 },
    { 30, 62, 34, 2 },
    { 7, 39, 57, 25 },
    { 16, 48, 48, 16 },
    { 25, 57, 39, 7 },
    { 1, 33, 63, 31 },
    { 10, 42, 54, 22 },
    { 19, 51, 45, 13 },
    { 28, 60, 36, 4 },
    { 4, 36, 60, 28 },
    { 13, 45, 51, 19 },
    { 22, 54, 42, 10 },
    { 31, 63, 33, 1 },
    { 7, 39, 57, 25 },
    { 16, 48, 48, 16 },
    { 25, 57, 39, 7 },
    { 2, 34, 62, 30 },
    { 10, 42, 54, 22 },
    { 19, 51, 45, 13 },
    { 28, 60, 36, 4 },
    { 5, 37, 59, 27 },
    { 13, 45, 51, 19 },
    { 22, 54, 42, 10 },
    { 31, 63, 33, 1 },
    { 8, 40, 56, 24 },
    { 16, 48, 48, 16 }
};
static tab_u16 tab_idx_mode_7_10bit[64] = {
    0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 7, 8, 9, 10, 10, 11, 12, 13, 13, 14, 15, 15, 16,
    17, 18, 18, 19, 20, 21, 21, 22, 23, 23, 24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 31,
    32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 39, 40, 41, 42, 42, 43, 44, 45, 45, 46
};

static ALIGNED_16(tab_s8 tab_coeff_mode_9[64][16]) = {
    { 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11 },
    { 9 , 41, 55, 23, 9 , 41, 55, 23, 9 , 41, 55, 23, 9 , 41, 55, 23 },
    { 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2  },
    { 18, 50, 46, 14, 18, 50, 46, 14, 18, 50, 46, 14, 18, 50, 46, 14 },
    { 6 , 38, 58, 26, 6 , 38, 58, 26, 6 , 38, 58, 26, 6 , 38, 58, 26 },
    { 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5  },
    { 15, 47, 49, 17, 15, 47, 49, 17, 15, 47, 49, 17, 15, 47, 49, 17 },
    { 3 , 35, 61, 29, 3 , 35, 61, 29, 3 , 35, 61, 29, 3 , 35, 61, 29 },
    { 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8  },
    { 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20 },
    { 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31 },
    { 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11 },
    { 9 , 41, 55, 23, 9 , 41, 55, 23, 9 , 41, 55, 23, 9 , 41, 55, 23 },
    { 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2  },
    { 18, 50, 46, 14, 18, 50, 46, 14, 18, 50, 46, 14, 18, 50, 46, 14 },
    { 6 , 38, 58, 26, 6 , 38, 58, 26, 6 , 38, 58, 26, 6 , 38, 58, 26 },
    { 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5  },
    { 15, 47, 49, 17, 15, 47, 49, 17, 15, 47, 49, 17, 15, 47, 49, 17 },
    { 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28 },
    { 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8  },
    { 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20 },
    { 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31 },
    { 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11 },
    { 9 , 41, 55, 23, 9 , 41, 55, 23, 9 , 41, 55, 23, 9 , 41, 55, 23 },
    { 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2  },
    { 18, 50, 46, 14, 18, 50, 46, 14, 18, 50, 46, 14, 18, 50, 46, 14 },
    { 7 , 39, 57, 25, 7 , 39, 57, 25, 7 , 39, 57, 25, 7 , 39, 57, 25 },
    { 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5  },
    { 15, 47, 49, 17, 15, 47, 49, 17, 15, 47, 49, 17, 15, 47, 49, 17 },
    { 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28 },
    { 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8  },
    { 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20 },
    { 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31 },
    { 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11 },
    { 10, 42, 54, 22, 10, 42, 54, 22, 10, 42, 54, 22, 10, 42, 54, 22 },
    { 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2  },
    { 18, 50, 46, 14, 18, 50, 46, 14, 18, 50, 46, 14, 18, 50, 46, 14 },
    { 7 , 39, 57, 25, 7 , 39, 57, 25, 7 , 39, 57, 25, 7 , 39, 57, 25 },
    { 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5  },
    { 15, 47, 49, 17, 15, 47, 49, 17, 15, 47, 49, 17, 15, 47, 49, 17 },
    { 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28 },
    { 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8  },
    { 13, 45, 51, 19, 13, 45, 51, 19, 13, 45, 51, 19, 13, 45, 51, 19 },
    { 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31 },
    { 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11 },
    { 10, 42, 54, 22, 10, 42, 54, 22, 10, 42, 54, 22, 10, 42, 54, 22 },
    { 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2  },
    { 18, 50, 46, 14, 18, 50, 46, 14, 18, 50, 46, 14, 18, 50, 46, 14 },
    { 7 , 39, 57, 25, 7 , 39, 57, 25, 7 , 39, 57, 25, 7 , 39, 57, 25 },
    { 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5  },
    { 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16 },
    { 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28 },
    { 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8  },
    { 13, 45, 51, 19, 13, 45, 51, 19, 13, 45, 51, 19, 13, 45, 51, 19 },
    { 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31, 1 , 33, 63, 31 },
    { 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11, 21, 53, 43, 11 },
    { 10, 42, 54, 22, 10, 42, 54, 22, 10, 42, 54, 22, 10, 42, 54, 22 },
    { 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2 , 30, 62, 34, 2  },
    { 19, 51, 45, 13, 19, 51, 45, 13, 19, 51, 45, 13, 19, 51, 45, 13 },
    { 7 , 39, 57, 25, 7 , 39, 57, 25, 7 , 39, 57, 25, 7 , 39, 57, 25 },
    { 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5 , 27, 59, 37, 5  },
    { 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16 },
    { 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28 },
    { 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8  }
};

static tab_u8 tab_idx_mode_9[64] = {
    0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 9, 9,
    9, 10, 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16,
    16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 21, 21, 21, 22, 22, 22, 23
};

static ALIGNED_16(tab_s16 tab_coeff_mode_9_10bit[64][4]) = {
    { 21, 53, 43, 11 },
    { 9, 41, 55, 23 },
    { 30, 62, 34, 2 },
    { 18, 50, 46, 14 },
    { 6, 38, 58, 26 },
    { 27, 59, 37, 5 },
    { 15, 47, 49, 17 },
    { 3, 35, 61, 29 },
    { 24, 56, 40, 8 },
    { 12, 44, 52, 20 },
    { 1, 33, 63, 31 },
    { 21, 53, 43, 11 },
    { 9, 41, 55, 23 },
    { 30, 62, 34, 2 },
    { 18, 50, 46, 14 },
    { 6, 38, 58, 26 },
    { 27, 59, 37, 5 },
    { 15, 47, 49, 17 },
    { 4, 36, 60, 28 },
    { 24, 56, 40, 8 },
    { 12, 44, 52, 20 },
    { 1, 33, 63, 31 },
    { 21, 53, 43, 11 },
    { 9, 41, 55, 23 },
    { 30, 62, 34, 2 },
    { 18, 50, 46, 14 },
    { 7, 39, 57, 25 },
    { 27, 59, 37, 5 },
    { 15, 47, 49, 17 },
    { 4, 36, 60, 28 },
    { 24, 56, 40, 8 },
    { 12, 44, 52, 20 },
    { 1, 33, 63, 31 },
    { 21, 53, 43, 11 },
    { 10, 42, 54, 22 },
    { 30, 62, 34, 2 },
    { 18, 50, 46, 14 },
    { 7, 39, 57, 25 },
    { 27, 59, 37, 5 },
    { 15, 47, 49, 17 },
    { 4, 36, 60, 28 },
    { 24, 56, 40, 8 },
    { 13, 45, 51, 19 },
    { 1, 33, 63, 31 },
    { 21, 53, 43, 11 },
    { 10, 42, 54, 22 },
    { 30, 62, 34, 2 },
    { 18, 50, 46, 14 },
    { 7, 39, 57, 25 },
    { 27, 59, 37, 5 },
    { 16, 48, 48, 16 },
    { 4, 36, 60, 28 },
    { 24, 56, 40, 8 },
    { 13, 45, 51, 19 },
    { 1, 33, 63, 31 },
    { 21, 53, 43, 11 },
    { 10, 42, 54, 22 },
    { 30, 62, 34, 2 },
    { 19, 51, 45, 13 },
    { 7, 39, 57, 25 },
    { 27, 59, 37, 5 },
    { 16, 48, 48, 16 },
    { 4, 36, 60, 28 },
    { 24, 56, 40, 8 }
};

static tab_u16 tab_idx_mode_9_10bit[64] = {
    0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 9, 9,
    9, 10, 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16,
    16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 21, 21, 21, 22, 22, 22, 23
};

static ALIGNED_16(tab_s8 tab_coeff_mode_11[8][16]) = {
    { 28, 60, 36, 4 , 28, 60, 36, 4 , 28, 60, 36, 4 , 28, 60, 36, 4 },
    { 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8 },
    { 20, 52, 44, 12, 20, 52, 44, 12, 20, 52, 44, 12, 20, 52, 44, 12 },
    { 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16 },
    { 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20 },
    { 8 , 40, 56, 24, 8 , 40, 56, 24, 8 , 40, 56, 24, 8 , 40, 56, 24 },
    { 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28 },
    { 32, 64, 32, 0 , 32, 64, 32, 0 , 32, 64, 32, 0 , 32, 64, 32, 0 }
};

static ALIGNED_16(tab_s16 tab_coeff_mode_11_10bit[8][16]) = {
    { 28, 60, 36, 4, 28, 60, 36, 4, 28, 60, 36, 4, 28, 60, 36, 4 },
    { 24, 56, 40, 8, 24, 56, 40, 8, 24, 56, 40, 8, 24, 56, 40, 8 },
    { 20, 52, 44, 12, 20, 52, 44, 12, 20, 52, 44, 12, 20, 52, 44, 12 },
    { 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16 },
    { 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20 },
    { 8, 40, 56, 24, 8, 40, 56, 24, 8, 40, 56, 24, 8, 40, 56, 24 },
    { 4, 36, 60, 28, 4, 36, 60, 28, 4, 36, 60, 28, 4, 36, 60, 28 },
    { 32, 64, 32, 0, 32, 64, 32, 0, 32, 64, 32, 0, 32, 64, 32, 0 }
};

static tab_s8 tbl_ipf_pred_param_sse[5][16] = {
    { 24,  6,  2,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0 }, //4x4, 24, 0.5
    { 44, 25, 14,  8,  4,  2,  1,  1,  0,  0, 0, 0, 0, 0, 0, 0 }, //8x8, 44-1.2
    { 40, 27, 19, 13,  9,  6,  4,  3,  2,  1, 0, 0, 0, 0, 0, 0 }, //16x16, 40-1.8
    { 36, 27, 21, 16, 12,  9,  7,  5,  4,  3, 0, 0, 0, 0, 0, 0 }, //32x32, 36-2.5
    { 52, 44, 37, 31, 26, 22, 18, 15, 13, 11, 0, 0, 0, 0, 0, 0 }, //64x64
};

void uavs3e_ipred_ipf_core_sse(pel *src, pel *dst, int i_dst, int ipm, int w, int h, int bit_depth)
{
    s32 filter_idx_hor = (s32)com_tbl_log2[w] - 2; //Block Size
    s32 filter_idx_ver = (s32)com_tbl_log2[h] - 2; //Block Size
    s32 ver_filter_range = COM_MIN(h, 10);
    s32 hor_filter_range = COM_MIN(w, 10);

    // TODO: tbl_ipf_pred_param doesn't support 128
    if (filter_idx_hor > 4) {
        filter_idx_hor = 4;
        hor_filter_range = 0; // don't use IPF at horizontal direction
    }
    if (filter_idx_ver > 4) {
        filter_idx_ver = 4;
        ver_filter_range = 0; // don't use IPF at vertical direction
    }

    tab_u8 *filter_hori_param = tbl_ipf_pred_param_sse[filter_idx_hor];
    tab_u8 *filter_vert_param = tbl_ipf_pred_param_sse[filter_idx_ver];

    if (IPD_DIA_L <= ipm && ipm <= IPD_DIA_R) { // vertical mode use left reference pixels, don't use top reference
        ver_filter_range = 0;
    }
    if (IPD_DIA_R < ipm) { // horizontal mode use top reference pixels, don't use left reference
        hor_filter_range = 0;
    }

    pel *p_top = src + 1;
    int row;

    if (w == 4) {
        if (hor_filter_range) {
            __m128i c_64_8 = _mm_set1_epi8(64);
            __m128i c_64 = _mm_set1_epi16(64);
            __m128i c_32 = _mm_set1_epi16(32);
            __m128i zero = _mm_setzero_si128();
            __m128i coef_left_8 = _mm_loadl_epi64((__m128i*)(filter_hori_param)); // coef_left
            __m128i pix_top = _mm_loadl_epi64((__m128i*)(p_top));
            for (row = 0; row < ver_filter_range; ++row) {
                __m128i coef_top_16 = _mm_set1_epi16(filter_vert_param[row]);
                __m128i coef_top_8 = _mm_set1_epi8((u8)filter_vert_param[row]);
                __m128i pix_left = _mm_set1_epi8((u8)src[-row - 1]);
                __m128i pix_cur = _mm_loadl_epi64((__m128i*)(dst));
                __m128i coef_cur, coef_left_16, pix_top_left, coef_top_left, val0, val1;

                coef_left_16 = _mm_unpacklo_epi8(coef_left_8, zero);
                coef_cur = _mm_subs_epi16(c_64, coef_top_16);
                coef_cur = _mm_subs_epi16(coef_cur, coef_left_16);

                pix_cur = _mm_unpacklo_epi8(pix_cur, zero);                 // 8bit->16bit
                pix_top_left = _mm_unpacklo_epi8(pix_top, pix_left);
                coef_top_left = _mm_unpacklo_epi8(coef_top_8, coef_left_8);

                val0 = _mm_mullo_epi16(pix_cur, coef_cur);
                val1 = _mm_maddubs_epi16(pix_top_left, coef_top_left);
                val0 = _mm_add_epi16(val0, val1);
                val0 = _mm_add_epi16(val0, c_32);
                val0 = _mm_srai_epi16(val0, 6);
                val0 = _mm_packus_epi16(val0, zero);

                ((int*)(dst))[0] = _mm_extract_epi32(val0, 0);
                dst += i_dst;
            }
            for (; row < h; ++row) {
                __m128i pix_left = _mm_set1_epi8((u8)src[-row - 1]);
                __m128i coef_cur, pix_cur, coef_left_cur, pix_left_cur, val;
                pix_cur = _mm_loadl_epi64((__m128i*)(dst));
                coef_cur = _mm_subs_epi8(c_64_8, coef_left_8);
                pix_left_cur = _mm_unpacklo_epi8(pix_left, pix_cur);
                coef_left_cur = _mm_unpacklo_epi8(coef_left_8, coef_cur);
                val = _mm_maddubs_epi16(pix_left_cur, coef_left_cur);
                val = _mm_add_epi16(val, c_32);
                val = _mm_srai_epi16(val, 6);
                val = _mm_packus_epi16(val, zero);
                ((int*)(dst))[0] = _mm_extract_epi32(val, 0);
                dst += i_dst;
            }
        }
        else {
            __m128i pix_top = _mm_loadl_epi64((__m128i*)(p_top));
            __m128i c_32 = _mm_set1_epi16(32);

            for (row = 0; row < ver_filter_range; ++row) {
                int tmp = 64 - filter_vert_param[row];
                __m128i coef_tmp = _mm_set1_epi8(tmp);
                __m128i coef_top = _mm_set1_epi8(filter_vert_param[row]);
                __m128i pix_cur, pix, coef;
                __m128i val;

                pix_cur = _mm_loadl_epi64((__m128i*)(dst));
                coef = _mm_unpacklo_epi8(coef_top, coef_tmp);
                pix = _mm_unpacklo_epi8(pix_top, pix_cur);

                val = _mm_maddubs_epi16(pix, coef);
                val = _mm_add_epi16(val, c_32);
                val = _mm_srai_epi16(val, 6);
                val = _mm_packus_epi16(val, val);

                ((int*)(dst))[0] = _mm_extract_epi32(val, 0);

                dst += i_dst;

            }
        }
    }
    else if (w == 8) { // w == 8
        if (w == hor_filter_range) {
            __m128i c_64_8 = _mm_set1_epi8(64);
            __m128i c_64 = _mm_set1_epi16(64);
            __m128i c_32 = _mm_set1_epi16(32);
            __m128i zero = _mm_setzero_si128();
            __m128i coef_left_8 = _mm_loadu_si128((__m128i*)(filter_hori_param)); // coef_left
            __m128i coef_left_16 = _mm_unpacklo_epi8(coef_left_8, zero);
            for (row = 0; row < ver_filter_range; ++row) {
                __m128i coef_top_16 = _mm_set1_epi16(filter_vert_param[row]);
                __m128i coef_top_8 = _mm_set1_epi8((u8)filter_vert_param[row]);
                __m128i pix_left = _mm_set1_epi8((u8)src[-row - 1]);
                __m128i pix_top = _mm_loadl_epi64((__m128i*)(p_top));
                __m128i pix_cur = _mm_loadl_epi64((__m128i*)(dst));
                __m128i coef_cur, pix_top_left, coef_top_left, val0, val1;

                coef_cur = _mm_subs_epi16(c_64, coef_top_16);
                coef_cur = _mm_subs_epi16(coef_cur, coef_left_16);

                pix_cur = _mm_unpacklo_epi8(pix_cur, zero);                 // 8bit->16bit
                pix_top_left = _mm_unpacklo_epi8(pix_top, pix_left);
                coef_top_left = _mm_unpacklo_epi8(coef_top_8, coef_left_8);

                val0 = _mm_mullo_epi16(pix_cur, coef_cur);
                val1 = _mm_maddubs_epi16(pix_top_left, coef_top_left);
                val0 = _mm_add_epi16(val0, val1);
                val0 = _mm_add_epi16(val0, c_32);
                val0 = _mm_srai_epi16(val0, 6);
                val0 = _mm_packus_epi16(val0, zero);

                _mm_storel_epi64((__m128i*)dst, val0);

                dst += i_dst;
            }
            for (; row < h; ++row) {
                __m128i pix_left = _mm_set1_epi8((u8)src[-row - 1]);
                __m128i coef_cur, pix_cur, coef_left_cur, pix_left_cur, val;

                pix_cur = _mm_loadl_epi64((__m128i*)(dst));
                coef_cur = _mm_subs_epi8(c_64_8, coef_left_8);
                pix_left_cur = _mm_unpacklo_epi8(pix_left, pix_cur);
                coef_left_cur = _mm_unpacklo_epi8(coef_left_8, coef_cur);
                val = _mm_maddubs_epi16(pix_left_cur, coef_left_cur);
                val = _mm_add_epi16(val, c_32);
                val = _mm_srai_epi16(val, 6);
                val = _mm_packus_epi16(val, zero);
                _mm_storel_epi64((__m128i*)dst, val);

                dst += i_dst;
            }
        }
        else {
            __m128i c_32 = _mm_set1_epi16(32);

            for (row = 0; row < ver_filter_range; ++row) {
                int tmp = 64 - filter_vert_param[row];
                __m128i coef_tmp = _mm_set1_epi8(tmp);
                __m128i coef_top = _mm_set1_epi8(filter_vert_param[row]);
                __m128i pix_top, pix_cur, pix, coef;
                __m128i val;

                coef = _mm_unpacklo_epi8(coef_top, coef_tmp);

                pix_cur = _mm_loadl_epi64((__m128i*)(dst));
                pix_top = _mm_loadl_epi64((__m128i*)(p_top));
                pix = _mm_unpacklo_epi8(pix_top, pix_cur);
                val = _mm_maddubs_epi16(pix, coef);
                val = _mm_add_epi16(val, c_32);
                val = _mm_srai_epi16(val, 6);
                val = _mm_packus_epi16(val, val);

                _mm_storel_epi64((__m128i*)dst, val);

                dst += i_dst;

            }
        }
    }
    else { // w >= 16
        if (hor_filter_range) {
            __m128i c_64 = _mm_set1_epi8(64);
            __m128i c_32 = _mm_set1_epi16(32);
            __m128i zero = _mm_setzero_si128();
            __m128i coef_left = _mm_loadu_si128((__m128i*)(filter_hori_param)); // coef_left
            int col;
            for (row = 0; row < ver_filter_range; ++row) {
                int tmp = 64 - filter_vert_param[row];
                __m128i coef_tmp = _mm_set1_epi8(tmp);
                __m128i coef_top = _mm_set1_epi8(filter_vert_param[row]);
                __m128i pix_left = _mm_set1_epi8((u8)src[-row - 1]);
                __m128i pix_top, pix_cur0, pix_cur1, coef_cur0, coef_cur1, pix0, pix1, coef0, coef1;
                __m128i val0, val1, val2, val3;

                pix_top = _mm_loadu_si128((__m128i*)(p_top));
                pix_cur0 = _mm_loadu_si128((__m128i*)(dst));

                coef_cur0 = _mm_subs_epi8(coef_tmp, coef_left);

                pix_cur1 = _mm_unpackhi_epi8(pix_cur0, zero);           // 8bit->16bit
                pix_cur0 = _mm_unpacklo_epi8(pix_cur0, zero);
                coef_cur1 = _mm_srli_si128(coef_cur0, 8);
                coef_cur0 = _mm_cvtepi8_epi16(coef_cur0);               // low 8 pixels
                coef_cur1 = _mm_cvtepi8_epi16(coef_cur1);
                pix0 = _mm_unpacklo_epi8(pix_top, pix_left);
                pix1 = _mm_unpackhi_epi8(pix_top, pix_left);
                coef0 = _mm_unpacklo_epi8(coef_top, coef_left);
                coef1 = _mm_unpackhi_epi8(coef_top, coef_left);

                val0 = _mm_mullo_epi16(pix_cur0, coef_cur0);
                val1 = _mm_mullo_epi16(pix_cur1, coef_cur1);
                val2 = _mm_maddubs_epi16(pix0, coef0);
                val3 = _mm_maddubs_epi16(pix1, coef1);
                val0 = _mm_add_epi16(val0, val2);
                val1 = _mm_add_epi16(val1, val3);
                val0 = _mm_add_epi16(val0, c_32);
                val1 = _mm_add_epi16(val1, c_32);
                val0 = _mm_srai_epi16(val0, 6);
                val1 = _mm_srai_epi16(val1, 6);
                val0 = _mm_packus_epi16(val0, val1);

                _mm_storeu_si128((__m128i*)dst, val0);

                for (col = 16; col < w; col += 16) {
                    pix_cur0 = _mm_loadu_si128((__m128i*)(dst + col));
                    pix_top = _mm_loadu_si128((__m128i*)(p_top + col));
                    pix0 = _mm_unpacklo_epi8(pix_top, pix_cur0);
                    pix1 = _mm_unpackhi_epi8(pix_top, pix_cur0);
                    coef0 = _mm_unpacklo_epi8(coef_top, coef_tmp);
                    coef1 = _mm_unpackhi_epi8(coef_top, coef_tmp);
                    val0 = _mm_maddubs_epi16(pix0, coef0);
                    val1 = _mm_maddubs_epi16(pix1, coef1);
                    val0 = _mm_add_epi16(val0, c_32);
                    val1 = _mm_add_epi16(val1, c_32);
                    val0 = _mm_srai_epi16(val0, 6);
                    val1 = _mm_srai_epi16(val1, 6);
                    val0 = _mm_packus_epi16(val0, val1);
                    _mm_storeu_si128((__m128i*)(dst + col), val0);
                }
                dst += i_dst;
            }
            for (; row < h; ++row) {
                __m128i pix_left = _mm_set1_epi8((u8)src[-row - 1]);
                __m128i coef_cur, pix_cur, coef0, coef1, pix0, pix1, val0, val1;

                pix_cur = _mm_loadu_si128((__m128i*)(dst));
                coef_cur = _mm_subs_epi8(c_64, coef_left);
                pix0 = _mm_unpacklo_epi8(pix_left, pix_cur);
                pix1 = _mm_unpackhi_epi8(pix_left, pix_cur);
                coef0 = _mm_unpacklo_epi8(coef_left, coef_cur);
                coef1 = _mm_unpackhi_epi8(coef_left, coef_cur);
                val0 = _mm_maddubs_epi16(pix0, coef0);
                val1 = _mm_maddubs_epi16(pix1, coef1);
                val0 = _mm_add_epi16(val0, c_32);
                val1 = _mm_add_epi16(val1, c_32);
                val0 = _mm_srai_epi16(val0, 6);
                val1 = _mm_srai_epi16(val1, 6);
                val0 = _mm_packus_epi16(val0, val1);
                _mm_storeu_si128((__m128i*)(dst), val0);

                dst += i_dst;
            }
        }
        else {
            __m128i c_32 = _mm_set1_epi16(32);
            int col;
            for (row = 0; row < ver_filter_range; ++row) {
                int tmp = 64 - filter_vert_param[row];
                __m128i coef_tmp = _mm_set1_epi8(tmp);
                __m128i coef_top = _mm_set1_epi8(filter_vert_param[row]);
                __m128i pix_top, pix_cur, pix0, pix1, coef0, coef1;
                __m128i val0, val1;

                coef0 = _mm_unpacklo_epi8(coef_top, coef_tmp);
                coef1 = _mm_unpackhi_epi8(coef_top, coef_tmp);

                for (col = 0; col < w; col += 16) {
                    pix_cur = _mm_loadu_si128((__m128i*)(dst + col));
                    pix_top = _mm_loadu_si128((__m128i*)(p_top + col));
                    pix0 = _mm_unpacklo_epi8(pix_top, pix_cur);
                    pix1 = _mm_unpackhi_epi8(pix_top, pix_cur);
                    val0 = _mm_maddubs_epi16(pix0, coef0);
                    val1 = _mm_maddubs_epi16(pix1, coef1);
                    val0 = _mm_add_epi16(val0, c_32);
                    val1 = _mm_add_epi16(val1, c_32);
                    val0 = _mm_srai_epi16(val0, 6);
                    val1 = _mm_srai_epi16(val1, 6);
                    val0 = _mm_packus_epi16(val0, val1);
                    _mm_storeu_si128((__m128i*)(dst + col), val0);
                }
                dst += i_dst;

            }
        }
    }
}

void uavs3e_ipred_ipf_core_s16_sse(pel *src, pel *dst, int i_dst, s16 *pred, int ipm, int w, int h, int bit_depth)
{
    s32 filter_idx_hor = (s32)com_tbl_log2[w] - 2; //Block Size
    s32 filter_idx_ver = (s32)com_tbl_log2[h] - 2; //Block Size
    s32 flt_range_ver = COM_MIN(h, 10);
    s32 flt_range_hor = COM_MIN(w, 10);

    int max_val = (1 << bit_depth) - 1;

    // TODO: tbl_ipf_pred_param doesn't support 128
    if (filter_idx_hor > 4) {
        filter_idx_hor = 4;
        flt_range_hor = 0; // don't use IPF at horizontal direction
    }
    if (filter_idx_ver > 4) {
        filter_idx_ver = 4;
        flt_range_ver = 0; // don't use IPF at vertical direction
    }

    tab_u8 *flt_hor_coef = tbl_ipf_pred_param_sse[filter_idx_hor];
    tab_u8 *flt_ver_coef = tbl_ipf_pred_param_sse[filter_idx_ver];

    if (IPD_DIA_L <= ipm && ipm <= IPD_DIA_R) { // vertical mode use left reference pixels, don't use top reference
        flt_range_ver = 0;
    }
    if (IPD_DIA_R < ipm) { // horizontal mode use top reference pixels, don't use left reference
        flt_range_hor = 0;
    }

    pel *p_top = src + 1;
    int row;
    __m128i c_32 = _mm_set1_epi16(32);
    __m128i zero = _mm_setzero_si128();
    if (w == 4) {
        if (flt_range_hor) {
            __m128i c_64 = _mm_set1_epi16(64);
            for (row = 0; row < flt_range_ver; ++row) {
                __m128i coef_top = _mm_set1_epi16(flt_ver_coef[row]);
                __m128i pix_left = _mm_set1_epi16(src[-row - 1]);
                __m128i coef_left = _mm_loadl_epi64((__m128i*)(flt_hor_coef)); // coef_left 8bit
                __m128i pix_top = _mm_loadl_epi64((__m128i*)(p_top));
                __m128i pix_cur = _mm_loadl_epi64((__m128i*)(pred));
                __m128i coef_cur, val0, val1, val2;

                coef_left = _mm_unpacklo_epi8(coef_left, zero);
                coef_cur = _mm_subs_epi16(c_64, coef_top);
                pix_top = _mm_unpacklo_epi8(pix_top, zero);                 // 8bit->16bit
                coef_cur = _mm_subs_epi16(coef_cur, coef_left);

                val0 = _mm_mullo_epi16(pix_cur, coef_cur);
                val1 = _mm_mullo_epi16(pix_left, coef_left);
                val2 = _mm_mullo_epi16(pix_top, coef_top);
                val0 = _mm_add_epi16(val0, val1);
                val2 = _mm_add_epi16(val2, c_32);
                val0 = _mm_add_epi16(val0, val2);
                val0 = _mm_srai_epi16(val0, 6);
                val0 = _mm_packus_epi16(val0, zero);

                ((int*)(dst))[0] = _mm_extract_epi32(val0, 0);
                pred += 4;
                dst += i_dst;
            }
            for (; row < h; ++row) {
                __m128i pix_left = _mm_set1_epi16(src[-row - 1]);
                __m128i coef_left = _mm_loadl_epi64((__m128i*)(flt_hor_coef)); // coef_left
                __m128i coef_cur, pix_cur, val0, val1;
                pix_cur = _mm_loadl_epi64((__m128i*)(pred));
                coef_left = _mm_unpacklo_epi8(coef_left, zero);
                coef_cur = _mm_subs_epi8(c_64, coef_left);
                val0 = _mm_mullo_epi16(pix_cur, coef_cur);
                val1 = _mm_mullo_epi16(pix_left, coef_left);
                val0 = _mm_add_epi16(val0, val1);
                val0 = _mm_add_epi16(val0, c_32);
                val0 = _mm_srai_epi16(val0, 6);
                val0 = _mm_packus_epi16(val0, zero);

                ((int*)(dst))[0] = _mm_extract_epi32(val0, 0);
                pred += 4;
                dst += i_dst;
            }
        }
        else {
            for (row = 0; row < flt_range_ver; ++row) {
                int tmp = 64 - flt_ver_coef[row];
                __m128i coef_cur = _mm_set1_epi16(tmp);
                __m128i coef_top = _mm_set1_epi16(flt_ver_coef[row]);
                __m128i pix_top, pix_cur;
                __m128i val0, val1;

                pix_top = _mm_loadl_epi64((__m128i*)(p_top));
                pix_cur = _mm_loadl_epi64((__m128i*)(pred));
                pix_top = _mm_unpacklo_epi8(pix_top, zero);

                val0 = _mm_mullo_epi16(pix_cur, coef_cur);
                val1 = _mm_mullo_epi16(pix_top, coef_top);
                val0 = _mm_add_epi16(val0, val1);
                val0 = _mm_add_epi16(val0, c_32);
                val0 = _mm_srai_epi16(val0, 6);
                val0 = _mm_packus_epi16(val0, zero);

                ((int*)(dst))[0] = _mm_extract_epi32(val0, 0);

                pred += 4;
                dst += i_dst;

            }

            for (; row < h; ++row) {
                __m128i pix = _mm_loadl_epi64((__m128i*)(pred));
                pix = _mm_packus_epi16(pix, zero);
                ((int*)(dst))[0] = _mm_extract_epi32(pix, 0);

                dst += i_dst;
                pred += 4;
            }
        }
    }
    else if (w == 8) { // w == 8
        if (flt_range_hor) {
            __m128i c_64 = _mm_set1_epi16(64);
            for (row = 0; row < flt_range_ver; ++row) {
                __m128i coef_top = _mm_set1_epi16(flt_ver_coef[row]);
                __m128i pix_left = _mm_set1_epi16(src[-row - 1]);
                __m128i coef_left = _mm_loadl_epi64((__m128i*)(flt_hor_coef)); // coef_left 8bit
                __m128i pix_top = _mm_loadl_epi64((__m128i*)(p_top));
                __m128i pix_cur = _mm_loadu_si128((__m128i*)(pred));
                __m128i coef_cur, val0, val1, val2;

                coef_left = _mm_unpacklo_epi8(coef_left, zero);
                coef_cur = _mm_subs_epi16(c_64, coef_top);
                pix_top = _mm_unpacklo_epi8(pix_top, zero);                 // 8bit->16bit
                coef_cur = _mm_subs_epi16(coef_cur, coef_left);

                val0 = _mm_mullo_epi16(pix_cur, coef_cur);
                val1 = _mm_mullo_epi16(pix_left, coef_left);
                val2 = _mm_mullo_epi16(pix_top, coef_top);
                val0 = _mm_add_epi16(val0, val1);
                val2 = _mm_add_epi16(val2, c_32);
                val0 = _mm_add_epi16(val0, val2);
                val0 = _mm_srai_epi16(val0, 6);
                val0 = _mm_packus_epi16(val0, zero);

                _mm_storel_epi64((__m128i*)dst, val0);
                dst += i_dst;
                pred += w;
            }
            for (; row < h; ++row) {
                __m128i pix_left = _mm_set1_epi16(src[-row - 1]);
                __m128i coef_left = _mm_loadl_epi64((__m128i*)(flt_hor_coef)); // coef_left
                __m128i coef_cur, pix_cur, val0, val1;

                pix_cur = _mm_loadu_si128((__m128i*)(pred));
                coef_left = _mm_unpacklo_epi8(coef_left, zero);
                coef_cur = _mm_subs_epi8(c_64, coef_left);
                val0 = _mm_mullo_epi16(pix_cur, coef_cur);
                val1 = _mm_mullo_epi16(pix_left, coef_left);
                val0 = _mm_add_epi16(val0, val1);
                val0 = _mm_add_epi16(val0, c_32);
                val0 = _mm_srai_epi16(val0, 6);
                val0 = _mm_packus_epi16(val0, zero);

                _mm_storel_epi64((__m128i*)dst, val0);
                dst += i_dst;
                pred += w;
            }
        }
        else {
            for (row = 0; row < flt_range_ver; ++row) {
                int tmp = 64 - flt_ver_coef[row];
                __m128i coef_cur = _mm_set1_epi16(tmp);
                __m128i coef_top = _mm_set1_epi16(flt_ver_coef[row]);
                __m128i pix_top, pix_cur;
                __m128i val0, val1;

                pix_top = _mm_loadl_epi64((__m128i*)(p_top));
                pix_cur = _mm_loadu_si128((__m128i*)(pred));
                pix_top = _mm_unpacklo_epi8(pix_top, zero);

                val0 = _mm_mullo_epi16(pix_cur, coef_cur);
                val1 = _mm_mullo_epi16(pix_top, coef_top);
                val0 = _mm_add_epi16(val0, val1);
                val0 = _mm_add_epi16(val0, c_32);
                val0 = _mm_srai_epi16(val0, 6);
                val0 = _mm_packus_epi16(val0, zero);

                _mm_storel_epi64((__m128i*)dst, val0);

                dst += i_dst;
                pred += w;
            }

            for (; row < h; ++row) {
                __m128i pix = _mm_loadu_si128((__m128i*)(pred));
                pix = _mm_packus_epi16(pix, zero);
                _mm_storel_epi64((__m128i*)dst, pix);

                dst += i_dst;
                pred += w;
            }
        }
    }
    else { // w >= 16
        if (flt_range_hor) {
            __m128i c_64 = _mm_set1_epi16(64);
            int col;
            for (row = 0; row < flt_range_ver; ++row) {
                int tmp = 64 - flt_ver_coef[row];
                __m128i coef_tmp_16 = _mm_set1_epi16(tmp);
                __m128i coef_top_16 = _mm_set1_epi16(flt_ver_coef[row]);
                __m128i coef_top_8 = _mm_set1_epi8(flt_ver_coef[row]);
                __m128i pix_left = _mm_set1_epi8((u8)src[-row - 1]);
                __m128i pix_top, pix_cur, coef_cur, coef_left_8, coef_left_16, pix0, coef0;
                __m128i val0, val1;

                // 0-8 3tap filter
                coef_left_8 = _mm_loadu_si128((__m128i*)(flt_hor_coef)); // coef_left
                pix_top = _mm_loadl_epi64((__m128i*)(p_top));
                pix_cur = _mm_loadu_si128((__m128i*)(pred));

                coef_left_16 = _mm_unpacklo_epi8(coef_left_8, zero);
                coef_cur = _mm_subs_epi16(coef_tmp_16, coef_left_16);

                pix0 = _mm_unpacklo_epi8(pix_top, pix_left);
                coef0 = _mm_unpacklo_epi8(coef_top_8, coef_left_8);

                val0 = _mm_mullo_epi16(pix_cur, coef_cur);
                val1 = _mm_maddubs_epi16(pix0, coef0);
                val0 = _mm_add_epi16(val0, val1);
                val0 = _mm_add_epi16(val0, c_32);
                val0 = _mm_srai_epi16(val0, 6);
                val0 = _mm_packus_epi16(val0, zero);

                _mm_storel_epi64((__m128i*)dst, val0);

                // 8-10 3tap filter, 10-15 2tap filter 
                coef_left_8 = _mm_loadu_si128((__m128i*)(flt_hor_coef + 8));
                pix_top = _mm_loadl_epi64((__m128i*)(p_top + 8));
                pix_cur = _mm_loadu_si128((__m128i*)(pred + 8));
                coef_left_16 = _mm_unpacklo_epi8(coef_left_8, zero);
                coef_cur = _mm_subs_epi16(coef_tmp_16, coef_left_16);

                pix0 = _mm_unpacklo_epi8(pix_top, pix_left);
                coef0 = _mm_unpacklo_epi8(coef_top_8, coef_left_8);

                val0 = _mm_mullo_epi16(pix_cur, coef_cur);
                val1 = _mm_maddubs_epi16(pix0, coef0);
                val0 = _mm_add_epi16(val0, val1);
                val0 = _mm_add_epi16(val0, c_32);
                val0 = _mm_srai_epi16(val0, 6);
                val0 = _mm_packus_epi16(val0, zero);
                _mm_storel_epi64((__m128i*)(dst + 8), val0);

                for (col = 16; col < w; col += 16) {
                    __m128i pix_cur2, pix_top2;
                    pix_top = _mm_loadu_si128((__m128i*)(p_top + col));
                    pix_cur = _mm_loadu_si128((__m128i*)(pred + col));
                    pix_cur2 = _mm_loadu_si128((__m128i*)(pred + col + 8));
                    pix_top2 = _mm_unpackhi_epi8(pix_top, zero);
                    pix_top = _mm_unpacklo_epi8(pix_top, zero);

                    pix_top = _mm_mullo_epi16(pix_top, coef_top_16);
                    pix_top2 = _mm_mullo_epi16(pix_top2, coef_top_16);
                    pix_cur = _mm_mullo_epi16(pix_cur, coef_tmp_16);
                    pix_cur2 = _mm_mullo_epi16(pix_cur2, coef_tmp_16);

                    val0 = _mm_adds_epi16(pix_top, pix_cur);
                    val1 = _mm_adds_epi16(pix_top2, pix_cur2);
                    val0 = _mm_add_epi16(val0, c_32);
                    val1 = _mm_add_epi16(val1, c_32);
                    val0 = _mm_srai_epi16(val0, 6);
                    val1 = _mm_srai_epi16(val1, 6);
                    val0 = _mm_packus_epi16(val0, val1);
                    _mm_storeu_si128((__m128i*)(dst + col), val0);
                }
                dst += i_dst;
                pred += w;
            }

            for (row = flt_range_ver; row < h; ++row) {
                __m128i pix_left0 = _mm_set1_epi16(src[-row - 1]), pix_left1;
                __m128i coef_left0, coef_left1, coef_cur0, coef_cur1;
                __m128i pix_cur0, pix_cur1, val0, val1;

                coef_left0 = _mm_loadu_si128((__m128i*)(flt_hor_coef)); // coef_left
                coef_left1 = _mm_unpackhi_epi8(coef_left0, zero);
                coef_left0 = _mm_unpacklo_epi8(coef_left0, zero);

                pix_cur0 = _mm_loadu_si128((__m128i*)(pred));
                pix_cur1 = _mm_loadu_si128((__m128i*)(pred + 8));
                coef_cur0 = _mm_subs_epi16(c_64, coef_left0);
                coef_cur1 = _mm_subs_epi16(c_64, coef_left1);
                pix_cur0 = _mm_mullo_epi16(pix_cur0, coef_cur0);
                pix_cur1 = _mm_mullo_epi16(pix_cur1, coef_cur1);
                pix_left1 = _mm_mullo_epi16(pix_left0, coef_left1);
                pix_left0 = _mm_mullo_epi16(pix_left0, coef_left0);
                val0 = _mm_adds_epi16(pix_cur0, pix_left0);
                val1 = _mm_adds_epi16(pix_cur1, pix_left1);
                val0 = _mm_adds_epi16(val0, c_32);
                val1 = _mm_adds_epi16(val1, c_32);
                val0 = _mm_srai_epi16(val0, 6);
                val1 = _mm_srai_epi16(val1, 6);
                val0 = _mm_packus_epi16(val0, val1);
                _mm_storeu_si128((__m128i*)(dst), val0);

                for (col = 16; col < w; col += 16) {
                    __m128i pix0 = _mm_loadu_si128((__m128i*)(pred + col));
                    __m128i pix1 = _mm_loadu_si128((__m128i*)(pred + col + 8));
                    pix0 = _mm_packus_epi16(pix0, pix1);
                    _mm_storeu_si128((__m128i*)(dst + col), pix0);
                }
                dst += i_dst;
                pred += w;
            }
        }
        else {
            int col;
            for (row = 0; row < flt_range_ver; ++row) {
                int tmp = 64 - flt_ver_coef[row];
                __m128i coef_cur = _mm_set1_epi16(tmp);
                __m128i coef_top = _mm_set1_epi16(flt_ver_coef[row]);
                __m128i pix_top0, pix_top1, pix_cur0, pix_cur1;
                __m128i val0, val1;

                for (col = 0; col < w; col += 16) {
                    pix_top0 = _mm_loadu_si128((__m128i*)(p_top + col));
                    pix_cur0 = _mm_loadu_si128((__m128i*)(pred + col));
                    pix_cur1 = _mm_loadu_si128((__m128i*)(pred + col + 8));
                    pix_top1 = _mm_unpackhi_epi8(pix_top0, zero);
                    pix_top0 = _mm_unpacklo_epi8(pix_top0, zero);

                    pix_cur0 = _mm_mullo_epi16(pix_cur0, coef_cur);
                    pix_cur1 = _mm_mullo_epi16(pix_cur1, coef_cur);
                    pix_top0 = _mm_mullo_epi16(pix_top0, coef_top);
                    pix_top1 = _mm_mullo_epi16(pix_top1, coef_top);

                    val0 = _mm_adds_epi16(pix_cur0, pix_top0);
                    val1 = _mm_adds_epi16(pix_cur1, pix_top1);
                    val0 = _mm_adds_epi16(val0, c_32);
                    val1 = _mm_adds_epi16(val1, c_32);
                    val0 = _mm_srai_epi16(val0, 6);
                    val1 = _mm_srai_epi16(val1, 6);
                    val0 = _mm_packus_epi16(val0, val1);
                    _mm_storeu_si128((__m128i*)(dst + col), val0);

                }

                pred += w;
                dst += i_dst;
            }

            for (; row < h; ++row) {
                for (col = 0; col < w; col += 16) {
                    __m128i pix0 = _mm_loadu_si128((__m128i*)(pred + col));
                    __m128i pix1 = _mm_loadu_si128((__m128i*)(pred + col + 8));
                    pix0 = _mm_packus_epi16(pix0, pix1);
                    _mm_storeu_si128((__m128i*)(dst + col), pix0);
                }

                dst += i_dst;
                pred += w;
            }

        }
    }
}

void uavs3e_ipred_ver_sse(pel *src, pel *dst, int i_dst, int width, int height)
{
    int y;
    pel *p_src = src;
    __m128i T1, T2, T3, T4;

    switch (width) {
    case 4: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            CP32(dst, p_src);
            CP32(dst + i_dst, p_src);
            CP32(dst + i_dst2, p_src);
            CP32(dst + i_dst3, p_src);
            dst += i_dst4;
        }
        break;
    }
    case 8:
        for (y = 0; y < height; y += 2) {
            CP64(dst, p_src);
            CP64(dst + i_dst, p_src);
            dst += i_dst << 1;
        }
        break;
    case 12:
        for (y = 0; y < height; y += 2) {
            CP64(dst, p_src);
            CP64(dst + i_dst, p_src);
            CP32(dst + 8, p_src + 8);
            CP32(dst + 8 + i_dst, p_src + 8);
            dst += i_dst << 1;
        }
        break;
    case 16:
        T1 = _mm_loadu_si128((__m128i *)p_src);
        for (y = 0; y < height; y++) {
            _mm_storeu_si128((__m128i *)(dst), T1);
            dst += i_dst;
        }
        break;
    case 24:
        T1 = _mm_loadu_si128((__m128i *)p_src);
        for (y = 0; y < height; y++) {
            _mm_storeu_si128((__m128i *)(dst), T1);
            CP64(dst + 16, p_src + 16);
            dst += i_dst;
        }
        break;
    case 32:
        T1 = _mm_loadu_si128((__m128i *)(p_src + 0));
        T2 = _mm_loadu_si128((__m128i *)(p_src + 16));
        for (y = 0; y < height; y++) {
            _mm_store_si128((__m128i *)(dst + 0), T1);
            _mm_store_si128((__m128i *)(dst + 16), T2);
            dst += i_dst;
        }
        break;
    case 48:
        T1 = _mm_loadu_si128((__m128i *)(p_src + 0));
        T2 = _mm_loadu_si128((__m128i *)(p_src + 16));
        T3 = _mm_loadu_si128((__m128i *)(p_src + 32));

        for (y = 0; y < height; y++) {
            _mm_store_si128((__m128i *)(dst + 0), T1);
            _mm_store_si128((__m128i *)(dst + 16), T2);
            _mm_store_si128((__m128i *)(dst + 32), T3);
            dst += i_dst;
        }
        break;

    case 64:
        T1 = _mm_loadu_si128((__m128i *)(p_src + 0));
        T2 = _mm_loadu_si128((__m128i *)(p_src + 16));
        T3 = _mm_loadu_si128((__m128i *)(p_src + 32));
        T4 = _mm_loadu_si128((__m128i *)(p_src + 48));
        for (y = 0; y < height; y++) {
            _mm_store_si128((__m128i *)(dst + 0), T1);
            _mm_store_si128((__m128i *)(dst + 16), T2);
            _mm_store_si128((__m128i *)(dst + 32), T3);
            _mm_store_si128((__m128i *)(dst + 48), T4);
            dst += i_dst;
        }
        break;
    default:
        com_assert(0);
        break;
    }
}

void uavs3e_ipred_hor_sse(pel *src, pel *dst, int i_dst, int width, int height)
{
    int y;
    switch (width) {
    case 4: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            M32(dst) = 0x01010101 * src[-y];
            M32(dst + i_dst) = 0x01010101 * src[-y - 1];
            M32(dst + i_dst2) = 0x01010101 * src[-y - 2];
            M32(dst + i_dst3) = 0x01010101 * src[-y - 3];
            dst += i_dst4;
        }
        break;
    }
    case 8: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            M64(dst) = 0x0101010101010101 * src[-y];
            M64(dst + i_dst) = 0x0101010101010101 * src[-y - 1];
            M64(dst + i_dst2) = 0x0101010101010101 * src[-y - 2];
            M64(dst + i_dst3) = 0x0101010101010101 * src[-y - 3];
            dst += i_dst4;
        }
        break;
    }
    case 12: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            M64(dst         ) = 0x0101010101010101 * src[-y];
            M64(dst + i_dst ) = 0x0101010101010101 * src[-y - 1];
            M64(dst + i_dst2) = 0x0101010101010101 * src[-y - 2];
            M64(dst + i_dst3) = 0x0101010101010101 * src[-y - 3];
            M32(dst + 8         ) = 0x01010101 * src[-y];
            M32(dst + 8 + i_dst ) = 0x01010101 * src[-y - 1];
            M32(dst + 8 + i_dst2) = 0x01010101 * src[-y - 2];
            M32(dst + 8 + i_dst3) = 0x01010101 * src[-y - 3];

            dst += i_dst4;
        }
        break;
    }
    case 16: {
        __m128i T0, T1, T2, T3;
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            T0 = _mm_set1_epi8((char)src[-y]);
            T1 = _mm_set1_epi8((char)src[-y - 1]);
            T2 = _mm_set1_epi8((char)src[-y - 2]);
            T3 = _mm_set1_epi8((char)src[-y - 3]);
            _mm_storeu_si128((__m128i *)(dst), T0);
            _mm_storeu_si128((__m128i *)(dst + i_dst), T1);
            _mm_storeu_si128((__m128i *)(dst + i_dst2), T2);
            _mm_storeu_si128((__m128i *)(dst + i_dst3), T3);
            dst += i_dst4;
        }
        break;
    }
    case 24: {
        __m128i T0, T1, T2, T3;
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            T0 = _mm_set1_epi8((char)src[-y]);
            T1 = _mm_set1_epi8((char)src[-y - 1]);
            T2 = _mm_set1_epi8((char)src[-y - 2]);
            T3 = _mm_set1_epi8((char)src[-y - 3]);
            _mm_storeu_si128((__m128i *)(dst), T0);
            _mm_storeu_si128((__m128i *)(dst + i_dst), T1);
            _mm_storeu_si128((__m128i *)(dst + i_dst2), T2);
            _mm_storeu_si128((__m128i *)(dst + i_dst3), T3);
            M64(dst + 16         ) = _mm_extract_epi64(T0, 0);
            M64(dst + 16 + i_dst ) = _mm_extract_epi64(T1, 0);
            M64(dst + 16 + i_dst2) = _mm_extract_epi64(T2, 0);
            M64(dst + 16 + i_dst3) = _mm_extract_epi64(T3, 0);
            dst += i_dst4;
        }
        break;
    }
    case 32: {
        __m128i T0, T1;
        int i_dst2 = i_dst << 1;
        for (y = 0; y < height; y += 2) {
            T0 = _mm_set1_epi8((char)src[-y]);
            T1 = _mm_set1_epi8((char)src[-y - 1]);
            _mm_store_si128((__m128i *)(dst), T0);
            _mm_store_si128((__m128i *)(dst + 16), T0);
            _mm_store_si128((__m128i *)(dst + i_dst), T1);
            _mm_store_si128((__m128i *)(dst + i_dst + 16), T1);
            dst += i_dst2;
        }
        break;
    }
    case 48: {
        __m128i T0, T1;
        int i_dst2 = i_dst << 1;
        for (y = 0; y < height; y += 2) {
            T0 = _mm_set1_epi8((char)src[-y]);
            T1 = _mm_set1_epi8((char)src[-y - 1]);
            _mm_store_si128((__m128i *)(dst), T0);
            _mm_store_si128((__m128i *)(dst + 16), T0);
            _mm_store_si128((__m128i *)(dst + 32), T0);
            _mm_store_si128((__m128i *)(dst + i_dst), T1);
            _mm_store_si128((__m128i *)(dst + i_dst + 16), T1);
            _mm_store_si128((__m128i *)(dst + i_dst + 32), T1);
            dst += i_dst2;
        }
        break;
    }

    case 64: {
        __m128i T;
        for (y = 0; y < height; y++) {
            T = _mm_set1_epi8((char)src[-y]);
            _mm_store_si128((__m128i *)(dst), T);
            _mm_store_si128((__m128i *)(dst + 16), T);
            _mm_store_si128((__m128i *)(dst + 32), T);
            _mm_store_si128((__m128i *)(dst + 48), T);
            dst += i_dst;
        }
        break;
    }
    default:
        com_assert(0);
        break;
    }
}

void uavs3e_ipred_dc_sse(pel *src, pel *dst, int i_dst, int width, int height, u16 avail_cu, int bit_depth)
{
    int  i, x, y;
    int  dc;
    pel  *p_src = src - 1;
    int left_avail = IS_AVAIL(avail_cu, AVAIL_LE);
    int above_avail = IS_AVAIL(avail_cu, AVAIL_UP);

    if (left_avail && above_avail) {
        int length = width + height + 1;
        __m128i sum = _mm_setzero_si128();
        __m128i val;

        p_src = src - height;

        for (i = 0; i < length - 7; i += 8) {
            val = _mm_cvtepu8_epi16(_mm_loadl_epi64((__m128i *)(p_src + i)));
            sum = _mm_add_epi16(sum, val);
        }
        if (i < length) {
            int left_pixels = length - i;
            __m128i mask = _mm_load_si128((__m128i *)(uavs3e_simd_mask[(left_pixels << 1) - 1]));
            val = _mm_cvtepu8_epi16(_mm_loadl_epi64((__m128i *)(p_src + i)));
            val = _mm_and_si128(val, mask);
            sum = _mm_add_epi16(sum, val);
        }
        sum = _mm_add_epi16(sum, _mm_srli_si128(sum, 8));
        sum = _mm_add_epi16(sum, _mm_srli_si128(sum, 4));
        sum = _mm_add_epi16(sum, _mm_srli_si128(sum, 2));

        dc = _mm_extract_epi16(sum, 0) + ((width + height) >> 1) - src[0];

        dc = (dc * (4096 / (width + height))) >> 12;

    } else if (left_avail) {
        dc = 0;
        for (y = 0; y < height; y++) {
            dc += p_src[-y];
        }
        dc += height / 2;
        dc >>= com_tbl_log2[height];
    } else {
        dc = 0;
        p_src = src + 1;
        if (above_avail) {
            for (x = 0; x < width; x++) {
                dc += p_src[x];
            }
            dc += width / 2;
            dc >>= com_tbl_log2[width];
        } else {
            dc = 1 << (bit_depth - 1);
        }
    }

    switch (width) {
    case 4: {
        u32 v32 = 0x01010101 * dc;
        for (y = 0; y < height; y++) {
            M32(dst) = v32;
            dst += i_dst;
        }
        break;
    }
    case 8: {
        u64 v64 = 0x0101010101010101 * dc;
        for (y = 0; y < height; y++) {
            M64(dst) = v64;
            dst += i_dst;
        }
        break;
    }
    case 16: {
        __m128i T = _mm_set1_epi8((s8)dc);
        for (y = 0; y < height; y++) {
            _mm_storeu_si128((__m128i *)(dst), T);
            dst += i_dst;
        }
        break;
    }
    case 32: {
        __m128i T = _mm_set1_epi8((s8)dc);
        for (y = 0; y < height; y++) {
            _mm_store_si128((__m128i *)(dst + 0), T);
            _mm_store_si128((__m128i *)(dst + 16), T);
            dst += i_dst;
        }
        break;
    }
    case 64: {
        __m128i T = _mm_set1_epi8((s8)dc);
        for (y = 0; y < height; y++) {
            _mm_store_si128((__m128i *)(dst + 0), T);
            _mm_store_si128((__m128i *)(dst + 16), T);
            _mm_store_si128((__m128i *)(dst + 32), T);
            _mm_store_si128((__m128i *)(dst + 48), T);
            dst += i_dst;
        }
        break;
    }
    default:
        com_assert(0);
        break;
    }
}

void uavs3e_ipred_plane_sse(pel *src, pel *dst, int i_dst, int width, int height, int bit_depth)
{
    pel  *p_src;
    int iH = 0;
    int iV = 0;
    int iA, iB, iC;
    int x, y;
    int iW2 = width >> 1;
    int iH2 = height >> 1;
    int ib_mult[5] = { 13, 17, 5, 11, 23 };
    int ib_shift[5] = { 7, 10, 11, 15, 19 };

    int im_h = ib_mult[com_tbl_log2[width] - 2];
    int is_h = ib_shift[com_tbl_log2[width] - 2];
    int im_v = ib_mult[com_tbl_log2[height] - 2];
    int is_v = ib_shift[com_tbl_log2[height] - 2];

    int iTmp;
    __m128i TC, TB, TA, T_Start, T, D, D1;

    p_src = src + iW2;
    for (x = 1; x < iW2 + 1; x++) {
        iH += x * (p_src[x] - p_src[-x]);
    }

    p_src = src - iH2;
    for (y = 1; y < iH2 + 1; y++) {
        iV += y * (p_src[-y] - p_src[y]);
    }

    iA = (src[-1 - (height - 1)] + src[1 + width - 1]) << 4;
    iB = ((iH << 5) * im_h + (1 << (is_h - 1))) >> is_h;
    iC = ((iV << 5) * im_v + (1 << (is_v - 1))) >> is_v;

    iTmp = iA - (iH2 - 1) * iC - (iW2 - 1) * iB + 16;

    TA = _mm_set1_epi16((s16)iTmp);
    TB = _mm_set1_epi16((s16)iB);
    TC = _mm_set1_epi16((s16)iC);

    T_Start = _mm_set_epi16(7, 6, 5, 4, 3, 2, 1, 0);
    T_Start = _mm_mullo_epi16(TB, T_Start);
    T_Start = _mm_add_epi16(T_Start, TA);

    TB = _mm_mullo_epi16(TB, _mm_set1_epi16(8));

    if (width <= 8) {
        for (y = 0; y < height; y++) {
            D = _mm_srai_epi16(T_Start, 5);
            D = _mm_packus_epi16(D, D);
            _mm_storel_epi64((__m128i *)dst, D);
            T_Start = _mm_add_epi16(T_Start, TC);
            dst += i_dst;
        }
    } else {
        for (y = 0; y < height; y++) {
            T = T_Start;
            for (x = 0; x < width; x += 16) {
                D = _mm_srai_epi16(T, 5);
                T = _mm_add_epi16(T, TB);
                D1 = _mm_srai_epi16(T, 5);
                T = _mm_add_epi16(T, TB);
                D = _mm_packus_epi16(D, D1);
                _mm_storeu_si128((__m128i *)(dst + x), D);
            }
            T_Start = _mm_add_epi16(T_Start, TC);
            dst += i_dst;
        }
    }

}

void uavs3e_ipred_bi_sse(pel *src, pel *dst, int i_dst, int width, int height, int bit_depth)
{
    int x, y;
    int ishift_x = com_tbl_log2[width];
    int ishift_y = com_tbl_log2[height];
    int ishift = COM_MIN(ishift_x, ishift_y);
    int ishift_xy = ishift_x + ishift_y + 1;
    int offset = 1 << (ishift_x + ishift_y);
    int wc, tbl_wc[6] = { -1, 21, 13, 7, 4, 2 };
    int a, b, c, w, val;

    __m128i T, T1, T2, T3, C1, C2, ADD;
    __m128i ZERO = _mm_setzero_si128();

    ALIGNED_16(s16 ref_up[MAX_CU_SIZE + 16]);
    ALIGNED_16(s16 ref_le[MAX_CU_SIZE + 16]);
    ALIGNED_16(s16 up[MAX_CU_SIZE + 16]);
    ALIGNED_16(s16 le[MAX_CU_SIZE + 16]);
    ALIGNED_16(s16 wy[MAX_CU_SIZE + 16]);

    for (y = 0; y < height; y++) {
        ref_le[y] = src[-1 - y];
    }

    wc = ishift_x > ishift_y ? ishift_x - ishift_y : ishift_y - ishift_x;
    com_assert(wc <= 5);
    wc = tbl_wc[wc];

    a = src[width];
    b = src[-height];

    if (width == height) {
        c = (a + b + 1) >> 1;
    } else {
        c = (((a << ishift_x) + (b << ishift_y)) * wc + (1 << (ishift + 5))) >> (ishift + 6);
    }

    w = (c << 1) - a - b;

    T = _mm_set1_epi16((s16)b);

    for (x = 0; x < width; x += 8) {
        T1 = _mm_loadl_epi64((__m128i *)(src + 1 + x));
        T1 = _mm_unpacklo_epi8(T1, ZERO);
        T2 = _mm_sub_epi16(T, T1);
        T1 = _mm_slli_epi16(T1, ishift_y);
        _mm_store_si128((__m128i *)(up + x), T2);
        _mm_store_si128((__m128i *)(ref_up + x), T1);
    }

    T = _mm_set1_epi16((s16)a);

    for (y = 0; y < height; y += 8) {
        T1 = _mm_load_si128((__m128i *)(ref_le + y));
        T2 = _mm_sub_epi16(T, T1);
        T1 = _mm_slli_epi16(T1, ishift_x);
        _mm_store_si128((__m128i *)(le + y), T2);
        _mm_store_si128((__m128i *)(ref_le + y), T1);
    }

    T = _mm_set1_epi16((s16)w);
    T = _mm_mullo_epi16(T, _mm_set_epi16(7, 6, 5, 4, 3, 2, 1, 0));
    T1 = _mm_set1_epi16((s16)(8 * w));

    for (y = 0; y < height; y += 8) {
        _mm_store_si128((__m128i *)(wy + y), T);
        T = _mm_add_epi16(T, T1);
    }

    C1 = _mm_set_epi32(3, 2, 1, 0);
    C2 = _mm_set1_epi32(4);

    if (width == 4) {
        __m128i m_up = _mm_loadl_epi64((__m128i *)up);
        T = _mm_loadl_epi64((__m128i *)ref_up);
        for (y = 0; y < height; y++) {
            int add = (le[y] << ishift_y) + wy[y];
            ADD = _mm_set1_epi32(add);
            ADD = _mm_mullo_epi32(C1, ADD);

            val = (ref_le[y] << ishift_y) + offset + (le[y] << ishift_y);

            ADD = _mm_add_epi32(ADD, _mm_set1_epi32(val));
            T = _mm_add_epi16(T, m_up);

            T1 = _mm_cvtepi16_epi32(T);
            T1 = _mm_slli_epi32(T1, ishift_x);

            T1 = _mm_add_epi32(T1, ADD);
            T1 = _mm_srai_epi32(T1, ishift_xy);

            T1 = _mm_packus_epi32(T1, T1);
            T1 = _mm_packus_epi16(T1, T1);

            M32(dst) = _mm_cvtsi128_si32(T1);

            dst += i_dst;
        }
    } else if (width == 8) {
        __m128i m_up = _mm_load_si128((__m128i *)up);
        T = _mm_load_si128((__m128i *)ref_up);
        for (y = 0; y < height; y++) {
            int add = (le[y] << ishift_y) + wy[y];
            ADD = _mm_set1_epi32(add);
            T3 = _mm_mullo_epi32(C2, ADD);
            ADD = _mm_mullo_epi32(C1, ADD);

            val = (ref_le[y] << ishift_y) + offset + (le[y] << ishift_y);

            ADD = _mm_add_epi32(ADD, _mm_set1_epi32(val));

            T = _mm_add_epi16(T, m_up);

            T1 = _mm_cvtepi16_epi32(T);
            T2 = _mm_cvtepi16_epi32(_mm_srli_si128(T, 8));
            T1 = _mm_slli_epi32(T1, ishift_x);
            T2 = _mm_slli_epi32(T2, ishift_x);

            T1 = _mm_add_epi32(T1, ADD);
            T1 = _mm_srai_epi32(T1, ishift_xy);
            ADD = _mm_add_epi32(ADD, T3);

            T2 = _mm_add_epi32(T2, ADD);
            T2 = _mm_srai_epi32(T2, ishift_xy);
            ADD = _mm_add_epi32(ADD, T3);

            T1 = _mm_packus_epi32(T1, T2);
            T1 = _mm_packus_epi16(T1, T1);

            _mm_storel_epi64((__m128i *)dst, T1);

            dst += i_dst;
        }
    } else {
        __m128i TT[16];
        __m128i PTT[16];
        for (x = 0; x < width; x += 8) {
            int idx = x >> 2;
            __m128i M0 = _mm_load_si128((__m128i *)(ref_up + x));
            __m128i M1 = _mm_load_si128((__m128i *)(up + x));
            TT[idx] = _mm_unpacklo_epi16(M0, ZERO);
            TT[idx + 1] = _mm_unpackhi_epi16(M0, ZERO);
            PTT[idx] = _mm_cvtepi16_epi32(M1);
            PTT[idx + 1] = _mm_cvtepi16_epi32(_mm_srli_si128(M1, 8));
        }
        for (y = 0; y < height; y++) {
            int add = (le[y] << ishift_y) + wy[y];
            ADD = _mm_set1_epi32(add);
            T3 = _mm_mullo_epi32(C2, ADD);
            ADD = _mm_mullo_epi32(C1, ADD);

            val = (ref_le[y] << ishift_y) + offset + (le[y] << ishift_y);

            ADD = _mm_add_epi32(ADD, _mm_set1_epi32(val));

            for (x = 0; x < width; x += 8) {
                int idx = x >> 2;
                TT[idx] = _mm_add_epi32(TT[idx], PTT[idx]);
                TT[idx + 1] = _mm_add_epi32(TT[idx + 1], PTT[idx + 1]);

                T1 = _mm_slli_epi32(TT[idx], ishift_x);
                T2 = _mm_slli_epi32(TT[idx + 1], ishift_x);

                T1 = _mm_add_epi32(T1, ADD);
                T1 = _mm_srai_epi32(T1, ishift_xy);
                ADD = _mm_add_epi32(ADD, T3);

                T2 = _mm_add_epi32(T2, ADD);
                T2 = _mm_srai_epi32(T2, ishift_xy);
                ADD = _mm_add_epi32(ADD, T3);

                T1 = _mm_packus_epi32(T1, T2);
                T1 = _mm_packus_epi16(T1, T1);

                _mm_storel_epi64((__m128i *)(dst + x), T1);
            }
            dst += i_dst;
        }
    }
}

void uavs3e_ipred_ang_x_3_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height)
{
    int i, j, k;
    int width2 = width << 1;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;

    if (width & 0x07) {
        for (j = 0; j < height; j++) {
            int real_width;
            int idx = tab_idx_mode_3[j];

            k = j & 0x03;
            real_width = COM_MIN(width, width2 - idx + 1);

            if (real_width <= 0) {
                pel val = (pel)((src[width2] * tab_coeff_mode_3[k][0] + src[width2 + 1] * tab_coeff_mode_3[k][1] + src[width2 + 2] * tab_coeff_mode_3[k][2] + src[width2 + 3] * tab_coeff_mode_3[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);
                _mm_storel_epi64((__m128i *)(dst), D0);
                dst += i_dst;
                j++;

                for (; j < height; j++) {
                    k = j & 0x03;
                    val = (pel)((src[width2] * tab_coeff_mode_3[k][0] + src[width2 + 1] * tab_coeff_mode_3[k][1] + src[width2 + 2] * tab_coeff_mode_3[k][2] + src[width2 + 3] * tab_coeff_mode_3[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i *)(dst), D0);
                    dst += i_dst;
                }
                break;
            } else {
                __m128i D0;
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_3[k]);

                S0 = _mm_loadl_epi64((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);

                D0 = _mm_hadds_epi16(t0, zero);
                D0 = _mm_add_epi16(D0, off);
                D0 = _mm_srli_epi16(D0, 7);

                D0 = _mm_packus_epi16(D0, zero);

                _mm_storel_epi64((__m128i *)(dst), D0);

                if (real_width < width) {
                    D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                    _mm_storel_epi64((__m128i *)(dst + real_width), D0);
                }
            }
            dst += i_dst;
        }
    } else if (width & 0x0f) {
        for (j = 0; j < height; j++) {
            int real_width;
            int idx = tab_idx_mode_3[j];

            k = j & 0x03;
            real_width = COM_MIN(width, width2 - idx + 1);

            if (real_width <= 0) {
                pel val = (pel)((src[width2] * tab_coeff_mode_3[k][0] + src[width2 + 1] * tab_coeff_mode_3[k][1] + src[width2 + 2] * tab_coeff_mode_3[k][2] + src[width2 + 3] * tab_coeff_mode_3[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);
                _mm_storel_epi64((__m128i *)(dst), D0);
                dst += i_dst;
                j++;

                for (; j < height; j++) {
                    k = j & 0x03;
                    val = (pel)((src[width2] * tab_coeff_mode_3[k][0] + src[width2 + 1] * tab_coeff_mode_3[k][1] + src[width2 + 2] * tab_coeff_mode_3[k][2] + src[width2 + 3] * tab_coeff_mode_3[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i *)(dst), D0);
                    dst += i_dst;
                }
                break;
            } else {
                __m128i D0;
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_3[k]);

                S0 = _mm_loadu_si128((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);
                t3 = _mm_unpackhi_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);
                t1 = _mm_maddubs_epi16(t3, c0);

                D0 = _mm_hadds_epi16(t0, t1);
                D0 = _mm_add_epi16(D0, off);
                D0 = _mm_srli_epi16(D0, 7);

                D0 = _mm_packus_epi16(D0, zero);

                _mm_storel_epi64((__m128i *)(dst), D0);
                //dst[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);

                if (real_width < width) {
                    D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                    _mm_storel_epi64((__m128i *)(dst + real_width), D0);
                }

            }

            dst += i_dst;
        }
    } else {
        for (j = 0; j < height; j++) {
            int real_width;
            int idx = tab_idx_mode_3[j];

            k = j & 0x03;
            real_width = COM_MIN(width, width2 - idx + 1);

            if (real_width <= 0) {
                pel val = (pel)((src[width2] * tab_coeff_mode_3[k][0] + src[width2 + 1] * tab_coeff_mode_3[k][1] + src[width2 + 2] * tab_coeff_mode_3[k][2] + src[width2 + 3] * tab_coeff_mode_3[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);

                for (i = 0; i < width; i += 16) {
                    _mm_storeu_si128((__m128i *)(dst + i), D0);
                }
                dst += i_dst;
                j++;

                for (; j < height; j++) {
                    k = j & 0x03;
                    val = (pel)((src[width2] * tab_coeff_mode_3[k][0] + src[width2 + 1] * tab_coeff_mode_3[k][1] + src[width2 + 2] * tab_coeff_mode_3[k][2] + src[width2 + 3] * tab_coeff_mode_3[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    for (i = 0; i < width; i += 16) {
                        _mm_storeu_si128((__m128i *)(dst + i), D0);
                    }
                    dst += i_dst;
                }
                break;
            } else {
                __m128i D0, D1;

                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_3[k]);
                for (i = 0; i < real_width; i += 16, idx += 16) {
                    S0 = _mm_loadu_si128((__m128i *)(src + idx));
                    S1 = _mm_loadu_si128((__m128i *)(src + idx + 1));
                    S2 = _mm_loadu_si128((__m128i *)(src + idx + 2));
                    S3 = _mm_loadu_si128((__m128i *)(src + idx + 3));

                    t0 = _mm_unpacklo_epi8(S0, S1);
                    t1 = _mm_unpacklo_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D0 = _mm_hadds_epi16(t0, t1);
                    D0 = _mm_add_epi16(D0, off);
                    D0 = _mm_srli_epi16(D0, 7);

                    t0 = _mm_unpackhi_epi8(S0, S1);
                    t1 = _mm_unpackhi_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D1 = _mm_hadds_epi16(t0, t1);
                    D1 = _mm_add_epi16(D1, off);
                    D1 = _mm_srli_epi16(D1, 7);

                    D0 = _mm_packus_epi16(D0, D1);

                    _mm_storeu_si128((__m128i *)(dst + i), D0);
                    //dst[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
                }

                if (real_width < width) {
                    D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                    for (i = real_width; i < width; i += 16) {
                        _mm_storeu_si128((__m128i *)(dst + i), D0);
                        //dst[i] = dst[real_width - 1];
                    }
                }

            }

            dst += i_dst;
        }
    }
}

void uavs3e_ipred_ang_x_5_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height)
{
    int i, j, k;
    int width2 = width << 1;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;

    if (width == 4) {
        for (j = 0; j < height; j++) {
            int real_width;
            int idx = tab_idx_mode_5[j];

            k = j & 0x07;
            real_width = COM_MIN(width, width2 - idx + 1);

            if (real_width <= 0) {
                pel val = (pel)((src[width2] * tab_coeff_mode_5[k][0] + src[width2 + 1] * tab_coeff_mode_5[k][1] + src[width2 + 2] * tab_coeff_mode_5[k][2] + src[width2 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);
                _mm_storel_epi64((__m128i *)(dst), D0);
                dst += i_dst;
                j++;

                for (; j < height; j++) {
                    k = j & 0x07;
                    val = (pel)((src[width2] * tab_coeff_mode_5[k][0] + src[width2 + 1] * tab_coeff_mode_5[k][1] + src[width2 + 2] * tab_coeff_mode_5[k][2] + src[width2 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i *)(dst), D0);
                    dst += i_dst;
                }
                break;
            } else {
                __m128i D0;
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_5[k]);

                S0 = _mm_loadl_epi64((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);

                D0 = _mm_hadds_epi16(t0, zero);
                D0 = _mm_add_epi16(D0, off);
                D0 = _mm_srli_epi16(D0, 7);

                D0 = _mm_packus_epi16(D0, zero);

                _mm_storel_epi64((__m128i *)(dst), D0);

                if (real_width < width) {
                    D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                    _mm_storel_epi64((__m128i *)(dst + real_width), D0);
                }
            }
            dst += i_dst;
        }
    } else if (width == 8) {
        for (j = 0; j < height; j++) {
            int real_width;
            int idx = tab_idx_mode_5[j];

            k = j & 0x07;
            real_width = COM_MIN(width, width2 - idx + 1);

            if (real_width <= 0) {
                pel val = (pel)((src[width2] * tab_coeff_mode_5[k][0] + src[width2 + 1] * tab_coeff_mode_5[k][1] + src[width2 + 2] * tab_coeff_mode_5[k][2] + src[width2 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);
                _mm_storel_epi64((__m128i *)(dst), D0);
                dst += i_dst;
                j++;

                for (; j < height; j++) {
                    k = j & 0x07;
                    val = (pel)((src[width2] * tab_coeff_mode_5[k][0] + src[width2 + 1] * tab_coeff_mode_5[k][1] + src[width2 + 2] * tab_coeff_mode_5[k][2] + src[width2 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i *)(dst), D0);
                    dst += i_dst;
                }
                break;
            } else {
                __m128i D0;
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_5[k]);

                S0 = _mm_loadu_si128((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);
                t3 = _mm_unpackhi_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);
                t1 = _mm_maddubs_epi16(t3, c0);

                D0 = _mm_hadds_epi16(t0, t1);
                D0 = _mm_add_epi16(D0, off);
                D0 = _mm_srli_epi16(D0, 7);

                D0 = _mm_packus_epi16(D0, zero);

                _mm_storel_epi64((__m128i *)(dst), D0);
                //dst[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);

                if (real_width < width) {
                    D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                    _mm_storel_epi64((__m128i *)(dst + real_width), D0);
                }

            }

            dst += i_dst;
        }
    } else {
        for (j = 0; j < height; j++) {
            int real_width;
            int idx = tab_idx_mode_5[j];
            k = j & 0x07;

            real_width = COM_MIN(width, width2 - idx + 1);

            if (real_width <= 0) {
                pel val = (pel)((src[width2] * tab_coeff_mode_5[k][0] + src[width2 + 1] * tab_coeff_mode_5[k][1] + src[width2 + 2] * tab_coeff_mode_5[k][2] + src[width2 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);

                for (i = 0; i < width; i += 16) {
                    _mm_storeu_si128((__m128i *)(dst + i), D0);
                }
                dst += i_dst;
                j++;

                for (; j < height; j++) {
                    k = j & 0x07;
                    val = (pel)((src[width2] * tab_coeff_mode_5[k][0] + src[width2 + 1] * tab_coeff_mode_5[k][1] + src[width2 + 2] * tab_coeff_mode_5[k][2] + src[width2 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    for (i = 0; i < width; i += 16) {
                        _mm_storeu_si128((__m128i *)(dst + i), D0);
                    }
                    dst += i_dst;
                }
                break;
            } else {
                __m128i D0, D1;

                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_5[k]);
                for (i = 0; i < real_width; i += 16, idx += 16) {
                    S0 = _mm_loadu_si128((__m128i *)(src + idx));
                    S1 = _mm_loadu_si128((__m128i *)(src + idx + 1));
                    S2 = _mm_loadu_si128((__m128i *)(src + idx + 2));
                    S3 = _mm_loadu_si128((__m128i *)(src + idx + 3));

                    t0 = _mm_unpacklo_epi8(S0, S1);
                    t1 = _mm_unpacklo_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D0 = _mm_hadds_epi16(t0, t1);
                    D0 = _mm_add_epi16(D0, off);
                    D0 = _mm_srli_epi16(D0, 7);

                    t0 = _mm_unpackhi_epi8(S0, S1);
                    t1 = _mm_unpackhi_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D1 = _mm_hadds_epi16(t0, t1);
                    D1 = _mm_add_epi16(D1, off);
                    D1 = _mm_srli_epi16(D1, 7);

                    D0 = _mm_packus_epi16(D0, D1);

                    _mm_storeu_si128((__m128i *)(dst + i), D0);
                    //dst[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
                }

                if (real_width < width) {
                    D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                    for (i = real_width; i < width; i += 16) {
                        _mm_storeu_si128((__m128i *)(dst + i), D0);
                        //dst[i] = dst[real_width - 1];
                    }
                }

            }

            dst += i_dst;
        }
    }
}

void uavs3e_ipred_ang_x_7_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height)
{
    int i, j;
    int width2 = width << 1;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;

    if (width >= height) {
        if (width & 0x07) {
            __m128i D0;
            int i_dst2 = i_dst << 1;

            for (j = 0; j < height; j += 2) {
                int idx = tab_idx_mode_7[j];
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_7[j]);

                S0 = _mm_loadl_epi64((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);

                idx = tab_idx_mode_7[j + 1];
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_7[j + 1]);
                S0 = _mm_loadl_epi64((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t1 = _mm_unpacklo_epi8(S0, S1);
                t2 = _mm_unpacklo_epi8(S2, S3);
                t1 = _mm_unpacklo_epi16(t1, t2);

                t1 = _mm_maddubs_epi16(t1, c0);

                D0 = _mm_hadds_epi16(t0, t1);
                D0 = _mm_add_epi16(D0, off);
                D0 = _mm_srli_epi16(D0, 7);
                D0 = _mm_packus_epi16(D0, zero);

                _mm_storel_epi64((__m128i *)dst, D0);
                _mm_storel_epi64((__m128i *)(dst + i_dst), _mm_srli_si128(D0, 4));
                //dst[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
                dst += i_dst2;
            }
        } else if (width & 0x0f) {
            __m128i D0;

            for (j = 0; j < height; j++) {
                int idx = tab_idx_mode_7[j];
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_7[j]);

                S0 = _mm_loadu_si128((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);
                t3 = _mm_unpackhi_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);
                t1 = _mm_maddubs_epi16(t3, c0);

                D0 = _mm_hadds_epi16(t0, t1);
                D0 = _mm_add_epi16(D0, off);
                D0 = _mm_srli_epi16(D0, 7);

                D0 = _mm_packus_epi16(D0, _mm_setzero_si128());

                _mm_storel_epi64((__m128i *)(dst), D0);
                //dst[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);

                dst += i_dst;
            }
        } else {
            for (j = 0; j < height; j++) {
                __m128i D0, D1;

                int idx = tab_idx_mode_7[j];
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_7[j]);

                for (i = 0; i < width; i += 16, idx += 16) {
                    S0 = _mm_loadu_si128((__m128i *)(src + idx));
                    S1 = _mm_loadu_si128((__m128i *)(src + idx + 1));
                    S2 = _mm_loadu_si128((__m128i *)(src + idx + 2));
                    S3 = _mm_loadu_si128((__m128i *)(src + idx + 3));

                    t0 = _mm_unpacklo_epi8(S0, S1);
                    t1 = _mm_unpacklo_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D0 = _mm_hadds_epi16(t0, t1);
                    D0 = _mm_add_epi16(D0, off);
                    D0 = _mm_srli_epi16(D0, 7);

                    t0 = _mm_unpackhi_epi8(S0, S1);
                    t1 = _mm_unpackhi_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D1 = _mm_hadds_epi16(t0, t1);
                    D1 = _mm_add_epi16(D1, off);
                    D1 = _mm_srli_epi16(D1, 7);

                    D0 = _mm_packus_epi16(D0, D1);

                    _mm_storeu_si128((__m128i *)(dst + i), D0);
                    //dst[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
                }

                dst += i_dst;
            }
        }
    } else {
        if (width & 0x07) {
            for (j = 0; j < height; j++) {
                int real_width;
                int idx = tab_idx_mode_7[j];

                real_width = COM_MIN(width, width2 - idx + 1);

                if (real_width <= 0) {
                    pel val = (pel)((src[width2] * tab_coeff_mode_7[j][0] + src[width2 + 1] * tab_coeff_mode_7[j][1] + src[width2 + 2] * tab_coeff_mode_7[j][2] + src[width2 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i *)(dst), D0);
                    dst += i_dst;
                    j++;

                    for (; j < height; j++) {
                        val = (pel)((src[width2] * tab_coeff_mode_7[j][0] + src[width2 + 1] * tab_coeff_mode_7[j][1] + src[width2 + 2] * tab_coeff_mode_7[j][2] + src[width2 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        _mm_storel_epi64((__m128i *)(dst), D0);
                        dst += i_dst;
                    }
                    break;
                } else {
                    __m128i D0;
                    c0 = _mm_load_si128((__m128i *)tab_coeff_mode_7[j]);

                    S0 = _mm_loadl_epi64((__m128i *)(src + idx));
                    S1 = _mm_srli_si128(S0, 1);
                    S2 = _mm_srli_si128(S0, 2);
                    S3 = _mm_srli_si128(S0, 3);

                    t0 = _mm_unpacklo_epi8(S0, S1);
                    t1 = _mm_unpacklo_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);

                    D0 = _mm_hadds_epi16(t0, zero);
                    D0 = _mm_add_epi16(D0, off);
                    D0 = _mm_srli_epi16(D0, 7);

                    D0 = _mm_packus_epi16(D0, zero);

                    _mm_storel_epi64((__m128i *)(dst), D0);

                    if (real_width < width) {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        _mm_storel_epi64((__m128i *)(dst + real_width), D0);
                    }
                }
                dst += i_dst;
            }
        } else if (width & 0x0f) {
            for (j = 0; j < height; j++) {
                int real_width;
                int idx = tab_idx_mode_7[j];

                real_width = COM_MIN(width, width2 - idx + 1);

                if (real_width <= 0) {
                    pel val = (pel)((src[width2] * tab_coeff_mode_7[j][0] + src[width2 + 1] * tab_coeff_mode_7[j][1] + src[width2 + 2] * tab_coeff_mode_7[j][2] + src[width2 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i *)(dst), D0);
                    dst += i_dst;
                    j++;

                    for (; j < height; j++) {
                        val = (pel)((src[width2] * tab_coeff_mode_7[j][0] + src[width2 + 1] * tab_coeff_mode_7[j][1] + src[width2 + 2] * tab_coeff_mode_7[j][2] + src[width2 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        _mm_storel_epi64((__m128i *)(dst), D0);
                        dst += i_dst;
                    }
                    break;
                } else {
                    __m128i D0;
                    c0 = _mm_load_si128((__m128i *)tab_coeff_mode_7[j]);

                    S0 = _mm_loadu_si128((__m128i *)(src + idx));
                    S1 = _mm_srli_si128(S0, 1);
                    S2 = _mm_srli_si128(S0, 2);
                    S3 = _mm_srli_si128(S0, 3);

                    t0 = _mm_unpacklo_epi8(S0, S1);
                    t1 = _mm_unpacklo_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D0 = _mm_hadds_epi16(t0, t1);
                    D0 = _mm_add_epi16(D0, off);
                    D0 = _mm_srli_epi16(D0, 7);

                    D0 = _mm_packus_epi16(D0, zero);

                    _mm_storel_epi64((__m128i *)(dst), D0);
                    //dst[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);

                    if (real_width < width) {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        _mm_storel_epi64((__m128i *)(dst + real_width), D0);
                    }

                }

                dst += i_dst;
            }
        } else {
            for (j = 0; j < height; j++) {
                int real_width;
                int idx = tab_idx_mode_7[j];

                real_width = COM_MIN(width, width2 - idx + 1);

                if (real_width <= 0) {
                    pel val = (pel)((src[width2] * tab_coeff_mode_7[j][0] + src[width2 + 1] * tab_coeff_mode_7[j][1] + src[width2 + 2] * tab_coeff_mode_7[j][2] + src[width2 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);

                    for (i = 0; i < width; i += 16) {
                        _mm_storeu_si128((__m128i *)(dst + i), D0);
                    }
                    dst += i_dst;
                    j++;

                    for (; j < height; j++) {
                        val = (pel)((src[width2] * tab_coeff_mode_7[j][0] + src[width2 + 1] * tab_coeff_mode_7[j][1] + src[width2 + 2] * tab_coeff_mode_7[j][2] + src[width2 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        for (i = 0; i < width; i += 16) {
                            _mm_storeu_si128((__m128i *)(dst + i), D0);
                        }
                        dst += i_dst;
                    }
                    break;
                } else {
                    __m128i D0, D1;

                    c0 = _mm_load_si128((__m128i *)tab_coeff_mode_7[j]);
                    for (i = 0; i < real_width; i += 16, idx += 16) {
                        S0 = _mm_loadu_si128((__m128i *)(src + idx));
                        S1 = _mm_loadu_si128((__m128i *)(src + idx + 1));
                        S2 = _mm_loadu_si128((__m128i *)(src + idx + 2));
                        S3 = _mm_loadu_si128((__m128i *)(src + idx + 3));

                        t0 = _mm_unpacklo_epi8(S0, S1);
                        t1 = _mm_unpacklo_epi8(S2, S3);
                        t2 = _mm_unpacklo_epi16(t0, t1);
                        t3 = _mm_unpackhi_epi16(t0, t1);

                        t0 = _mm_maddubs_epi16(t2, c0);
                        t1 = _mm_maddubs_epi16(t3, c0);

                        D0 = _mm_hadds_epi16(t0, t1);
                        D0 = _mm_add_epi16(D0, off);
                        D0 = _mm_srli_epi16(D0, 7);

                        t0 = _mm_unpackhi_epi8(S0, S1);
                        t1 = _mm_unpackhi_epi8(S2, S3);
                        t2 = _mm_unpacklo_epi16(t0, t1);
                        t3 = _mm_unpackhi_epi16(t0, t1);

                        t0 = _mm_maddubs_epi16(t2, c0);
                        t1 = _mm_maddubs_epi16(t3, c0);

                        D1 = _mm_hadds_epi16(t0, t1);
                        D1 = _mm_add_epi16(D1, off);
                        D1 = _mm_srli_epi16(D1, 7);

                        D0 = _mm_packus_epi16(D0, D1);

                        _mm_storeu_si128((__m128i *)(dst + i), D0);
                        //dst[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
                    }

                    if (real_width < width) {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        for (i = real_width; i < width; i += 16) {
                            _mm_storeu_si128((__m128i *)(dst + i), D0);
                            //dst[i] = dst[real_width - 1];
                        }
                    }

                }

                dst += i_dst;
            }
        }
    }
}

void uavs3e_ipred_ang_x_9_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height)
{
    int i, j;
    int width2 = width << 1;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;

    if (width >= height) {
        if (width & 0x07) {
            __m128i D0;
            int i_dst2 = i_dst << 1;

            for (j = 0; j < height; j += 2) {
                int idx = tab_idx_mode_9[j];
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_9[j]);

                S0 = _mm_loadl_epi64((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);

                idx = tab_idx_mode_9[j + 1];
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_9[j + 1]);
                S0 = _mm_loadl_epi64((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t1 = _mm_unpacklo_epi8(S0, S1);
                t2 = _mm_unpacklo_epi8(S2, S3);
                t1 = _mm_unpacklo_epi16(t1, t2);

                t1 = _mm_maddubs_epi16(t1, c0);

                D0 = _mm_hadds_epi16(t0, t1);
                D0 = _mm_add_epi16(D0, off);
                D0 = _mm_srli_epi16(D0, 7);
                D0 = _mm_packus_epi16(D0, zero);

                _mm_storel_epi64((__m128i *)dst, D0);
                _mm_storel_epi64((__m128i *)(dst + i_dst), _mm_srli_si128(D0, 4));
                dst += i_dst2;
            }
        } else if (width & 0x0f) {
            __m128i D0;

            for (j = 0; j < height; j++) {
                int idx = tab_idx_mode_9[j];
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_9[j]);

                S0 = _mm_loadu_si128((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);
                t3 = _mm_unpackhi_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);
                t1 = _mm_maddubs_epi16(t3, c0);

                D0 = _mm_hadds_epi16(t0, t1);
                D0 = _mm_add_epi16(D0, off);
                D0 = _mm_srli_epi16(D0, 7);

                D0 = _mm_packus_epi16(D0, _mm_setzero_si128());

                _mm_storel_epi64((__m128i *)(dst), D0);
                //dst[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);

                dst += i_dst;
            }
        } else {
            for (j = 0; j < height; j++) {
                __m128i D0, D1;

                int idx = tab_idx_mode_9[j];
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_9[j]);

                for (i = 0; i < width; i += 16, idx += 16) {
                    S0 = _mm_loadu_si128((__m128i *)(src + idx));
                    S1 = _mm_loadu_si128((__m128i *)(src + idx + 1));
                    S2 = _mm_loadu_si128((__m128i *)(src + idx + 2));
                    S3 = _mm_loadu_si128((__m128i *)(src + idx + 3));

                    t0 = _mm_unpacklo_epi8(S0, S1);
                    t1 = _mm_unpacklo_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D0 = _mm_hadds_epi16(t0, t1);
                    D0 = _mm_add_epi16(D0, off);
                    D0 = _mm_srli_epi16(D0, 7);

                    t0 = _mm_unpackhi_epi8(S0, S1);
                    t1 = _mm_unpackhi_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D1 = _mm_hadds_epi16(t0, t1);
                    D1 = _mm_add_epi16(D1, off);
                    D1 = _mm_srli_epi16(D1, 7);

                    D0 = _mm_packus_epi16(D0, D1);

                    _mm_storeu_si128((__m128i *)(dst + i), D0);
                    //dst[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
                }

                dst += i_dst;
            }
        }
    } else {
        if (width & 0x07) {
            for (j = 0; j < height; j++) {
                int real_width;
                int idx = tab_idx_mode_9[j];

                real_width = COM_MIN(width, width2 - idx + 1);

                if (real_width <= 0) {
                    pel val = (pel)((src[width2] * tab_coeff_mode_9[j][0] + src[width2 + 1] * tab_coeff_mode_9[j][1] + src[width2 + 2] * tab_coeff_mode_9[j][2] + src[width2 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i *)(dst), D0);
                    dst += i_dst;
                    j++;

                    for (; j < height; j++) {
                        val = (pel)((src[width2] * tab_coeff_mode_9[j][0] + src[width2 + 1] * tab_coeff_mode_9[j][1] + src[width2 + 2] * tab_coeff_mode_9[j][2] + src[width2 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        _mm_storel_epi64((__m128i *)(dst), D0);
                        dst += i_dst;
                    }
                    break;
                } else {
                    __m128i D0;
                    c0 = _mm_load_si128((__m128i *)tab_coeff_mode_9[j]);

                    S0 = _mm_loadl_epi64((__m128i *)(src + idx));
                    S1 = _mm_srli_si128(S0, 1);
                    S2 = _mm_srli_si128(S0, 2);
                    S3 = _mm_srli_si128(S0, 3);

                    t0 = _mm_unpacklo_epi8(S0, S1);
                    t1 = _mm_unpacklo_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);

                    D0 = _mm_hadds_epi16(t0, zero);
                    D0 = _mm_add_epi16(D0, off);
                    D0 = _mm_srli_epi16(D0, 7);

                    D0 = _mm_packus_epi16(D0, zero);

                    _mm_storel_epi64((__m128i *)(dst), D0);

                    if (real_width < width) {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        _mm_storel_epi64((__m128i *)(dst + real_width), D0);
                    }
                }
                dst += i_dst;
            }
        } else if (width & 0x0f) {
            for (j = 0; j < height; j++) {
                int real_width;
                int idx = tab_idx_mode_9[j];

                real_width = COM_MIN(width, width2 - idx + 1);

                if (real_width <= 0) {
                    pel val = (pel)((src[width2] * tab_coeff_mode_9[j][0] + src[width2 + 1] * tab_coeff_mode_9[j][1] + src[width2 + 2] * tab_coeff_mode_9[j][2] + src[width2 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i *)(dst), D0);
                    dst += i_dst;
                    j++;

                    for (; j < height; j++) {
                        val = (pel)((src[width2] * tab_coeff_mode_9[j][0] + src[width2 + 1] * tab_coeff_mode_9[j][1] + src[width2 + 2] * tab_coeff_mode_9[j][2] + src[width2 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        _mm_storel_epi64((__m128i *)(dst), D0);
                        dst += i_dst;
                    }
                    break;
                } else {
                    __m128i D0;
                    c0 = _mm_load_si128((__m128i *)tab_coeff_mode_9[j]);

                    S0 = _mm_loadu_si128((__m128i *)(src + idx));
                    S1 = _mm_srli_si128(S0, 1);
                    S2 = _mm_srli_si128(S0, 2);
                    S3 = _mm_srli_si128(S0, 3);

                    t0 = _mm_unpacklo_epi8(S0, S1);
                    t1 = _mm_unpacklo_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D0 = _mm_hadds_epi16(t0, t1);
                    D0 = _mm_add_epi16(D0, off);
                    D0 = _mm_srli_epi16(D0, 7);

                    D0 = _mm_packus_epi16(D0, zero);

                    _mm_storel_epi64((__m128i *)(dst), D0);
                    //dst[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);

                    if (real_width < width) {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        _mm_storel_epi64((__m128i *)(dst + real_width), D0);
                    }

                }

                dst += i_dst;
            }
        } else {
            for (j = 0; j < height; j++) {
                int real_width;
                int idx = tab_idx_mode_9[j];

                real_width = COM_MIN(width, width2 - idx + 1);

                if (real_width <= 0) {
                    pel val = (pel)((src[width2] * tab_coeff_mode_9[j][0] + src[width2 + 1] * tab_coeff_mode_9[j][1] + src[width2 + 2] * tab_coeff_mode_9[j][2] + src[width2 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);

                    for (i = 0; i < width; i += 16) {
                        _mm_storeu_si128((__m128i *)(dst + i), D0);
                    }
                    dst += i_dst;
                    j++;

                    for (; j < height; j++) {
                        val = (pel)((src[width2] * tab_coeff_mode_9[j][0] + src[width2 + 1] * tab_coeff_mode_9[j][1] + src[width2 + 2] * tab_coeff_mode_9[j][2] + src[width2 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        for (i = 0; i < width; i += 16) {
                            _mm_storeu_si128((__m128i *)(dst + i), D0);
                        }
                        dst += i_dst;
                    }
                    break;
                } else {
                    __m128i D0, D1;

                    c0 = _mm_load_si128((__m128i *)tab_coeff_mode_9[j]);
                    for (i = 0; i < real_width; i += 16, idx += 16) {
                        S0 = _mm_loadu_si128((__m128i *)(src + idx));
                        S1 = _mm_loadu_si128((__m128i *)(src + idx + 1));
                        S2 = _mm_loadu_si128((__m128i *)(src + idx + 2));
                        S3 = _mm_loadu_si128((__m128i *)(src + idx + 3));

                        t0 = _mm_unpacklo_epi8(S0, S1);
                        t1 = _mm_unpacklo_epi8(S2, S3);
                        t2 = _mm_unpacklo_epi16(t0, t1);
                        t3 = _mm_unpackhi_epi16(t0, t1);

                        t0 = _mm_maddubs_epi16(t2, c0);
                        t1 = _mm_maddubs_epi16(t3, c0);

                        D0 = _mm_hadds_epi16(t0, t1);
                        D0 = _mm_add_epi16(D0, off);
                        D0 = _mm_srli_epi16(D0, 7);

                        t0 = _mm_unpackhi_epi8(S0, S1);
                        t1 = _mm_unpackhi_epi8(S2, S3);
                        t2 = _mm_unpacklo_epi16(t0, t1);
                        t3 = _mm_unpackhi_epi16(t0, t1);

                        t0 = _mm_maddubs_epi16(t2, c0);
                        t1 = _mm_maddubs_epi16(t3, c0);

                        D1 = _mm_hadds_epi16(t0, t1);
                        D1 = _mm_add_epi16(D1, off);
                        D1 = _mm_srli_epi16(D1, 7);

                        D0 = _mm_packus_epi16(D0, D1);

                        _mm_storeu_si128((__m128i *)(dst + i), D0);
                        //dst[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
                    }

                    if (real_width < width) {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        for (i = real_width; i < width; i += 16) {
                            _mm_storeu_si128((__m128i *)(dst + i), D0);
                            //dst[i] = dst[real_width - 1];
                        }
                    }

                }

                dst += i_dst;
            }
        }
    }
}

void uavs3e_ipred_ang_x_11_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height)
{
    int i, j, idx;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;

    if (width & 0x07) {
        __m128i D0;
        int i_dst2 = i_dst << 1;

        for (j = 0; j < height; j += 2) {
            idx = (j + 1) >> 3;
            c0 = _mm_load_si128((__m128i *)tab_coeff_mode_11[j & 0x07]);

            S0 = _mm_loadl_epi64((__m128i *)(src + idx));
            S1 = _mm_srli_si128(S0, 1);
            S2 = _mm_srli_si128(S0, 2);
            S3 = _mm_srli_si128(S0, 3);

            t0 = _mm_unpacklo_epi8(S0, S1);
            t1 = _mm_unpacklo_epi8(S2, S3);
            t2 = _mm_unpacklo_epi16(t0, t1);

            t0 = _mm_maddubs_epi16(t2, c0);

            idx = (j + 2) >> 3;
            c0 = _mm_load_si128((__m128i *)tab_coeff_mode_11[(j + 1) & 0x07]);
            S0 = _mm_loadl_epi64((__m128i *)(src + idx));
            S1 = _mm_srli_si128(S0, 1);
            S2 = _mm_srli_si128(S0, 2);
            S3 = _mm_srli_si128(S0, 3);

            t1 = _mm_unpacklo_epi8(S0, S1);
            t2 = _mm_unpacklo_epi8(S2, S3);
            t1 = _mm_unpacklo_epi16(t1, t2);

            t1 = _mm_maddubs_epi16(t1, c0);

            D0 = _mm_hadds_epi16(t0, t1);
            D0 = _mm_add_epi16(D0, off);
            D0 = _mm_srli_epi16(D0, 7);
            D0 = _mm_packus_epi16(D0, zero);

            _mm_storel_epi64((__m128i *)dst, D0);
            _mm_storel_epi64((__m128i *)(dst + i_dst), _mm_srli_si128(D0, 4));
            //dst[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
            dst += i_dst2;
        }
    } else if (width & 0x0f) {
        __m128i D0;

        for (j = 0; j < height; j++) {
            idx = (j + 1) >> 3;
            c0 = _mm_load_si128((__m128i *)tab_coeff_mode_11[j & 0x07]);

            S0 = _mm_loadu_si128((__m128i *)(src + idx));
            S1 = _mm_srli_si128(S0, 1);
            S2 = _mm_srli_si128(S0, 2);
            S3 = _mm_srli_si128(S0, 3);

            t0 = _mm_unpacklo_epi8(S0, S1);
            t1 = _mm_unpacklo_epi8(S2, S3);
            t2 = _mm_unpacklo_epi16(t0, t1);
            t3 = _mm_unpackhi_epi16(t0, t1);

            t0 = _mm_maddubs_epi16(t2, c0);
            t1 = _mm_maddubs_epi16(t3, c0);

            D0 = _mm_hadds_epi16(t0, t1);
            D0 = _mm_add_epi16(D0, off);
            D0 = _mm_srli_epi16(D0, 7);

            D0 = _mm_packus_epi16(D0, _mm_setzero_si128());

            _mm_storel_epi64((__m128i *)(dst), D0);
            //dst[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);

            dst += i_dst;
        }
    } else {
        for (j = 0; j < height; j++) {
            __m128i D0, D1;

            idx = (j + 1) >> 3;
            c0 = _mm_load_si128((__m128i *)tab_coeff_mode_11[j & 0x07]);

            for (i = 0; i < width; i += 16, idx += 16) {
                S0 = _mm_loadu_si128((__m128i *)(src + idx));
                S1 = _mm_loadu_si128((__m128i *)(src + idx + 1));
                S2 = _mm_loadu_si128((__m128i *)(src + idx + 2));
                S3 = _mm_loadu_si128((__m128i *)(src + idx + 3));

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);
                t3 = _mm_unpackhi_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);
                t1 = _mm_maddubs_epi16(t3, c0);

                D0 = _mm_hadds_epi16(t0, t1);
                D0 = _mm_add_epi16(D0, off);
                D0 = _mm_srli_epi16(D0, 7);

                t0 = _mm_unpackhi_epi8(S0, S1);
                t1 = _mm_unpackhi_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);
                t3 = _mm_unpackhi_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);
                t1 = _mm_maddubs_epi16(t3, c0);

                D1 = _mm_hadds_epi16(t0, t1);
                D1 = _mm_add_epi16(D1, off);
                D1 = _mm_srli_epi16(D1, 7);

                D0 = _mm_packus_epi16(D0, D1);

                _mm_storeu_si128((__m128i *)(dst + i), D0);
                //dst[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
            }

            dst += i_dst;
        }
    }
}

void uavs3e_ipred_ang_x_4_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height)
{
    ALIGNED_16(pel first_line[64 + 128]);
    int line_size = width + (height - 1) * 2;
    int real_size = COM_MIN(line_size, width * 2 - 1);
    int height2 = height * 2;
    int i;
    __m128i zero = _mm_setzero_si128();
    __m128i offset = _mm_set1_epi16(2);

    src += 3;

    for (i = 0; i < real_size - 8; i += 16, src += 16) {
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);

        __m128i H0 = _mm_unpackhi_epi8(S0, zero);
        __m128i H1 = _mm_unpackhi_epi8(S1, zero);
        __m128i H2 = _mm_unpackhi_epi8(S2, zero);

        __m128i sum1 = _mm_add_epi16(L0, L1);
        __m128i sum2 = _mm_add_epi16(L1, L2);
        __m128i sum3 = _mm_add_epi16(H0, H1);
        __m128i sum4 = _mm_add_epi16(H1, H2);

        sum1 = _mm_add_epi16(sum1, sum2);
        sum3 = _mm_add_epi16(sum3, sum4);

        sum1 = _mm_add_epi16(sum1, offset);
        sum3 = _mm_add_epi16(sum3, offset);

        sum1 = _mm_srli_epi16(sum1, 2);
        sum3 = _mm_srli_epi16(sum3, 2);

        sum1 = _mm_packus_epi16(sum1, sum3);

        _mm_store_si128((__m128i *)&first_line[i], sum1);
    }

    if (i < real_size) {
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);

        __m128i sum1 = _mm_add_epi16(L0, L1);
        __m128i sum2 = _mm_add_epi16(L1, L2);

        sum1 = _mm_add_epi16(sum1, sum2);
        sum1 = _mm_add_epi16(sum1, offset);
        sum1 = _mm_srli_epi16(sum1, 2);

        sum1 = _mm_packus_epi16(sum1, sum1);
        _mm_storel_epi64((__m128i *)&first_line[i], sum1);
    }

    // padding
    for (i = real_size; i < line_size; i += 16) {
        __m128i pad = _mm_set1_epi8((char)first_line[real_size - 1]);
        _mm_storeu_si128((__m128i *)&first_line[i], pad);
    }

#define COPY_X4(w) { \
    for (i = 0; i < height2; i += 2) { \
        memcpy(dst, first_line + i, w * sizeof(pel)); \
        dst += i_dst; \
    } \
}
    switch (width) {
    case 4:
        COPY_X4(4)
            break;
    case 8:
        COPY_X4(8)
            break;
    case 12:
        COPY_X4(12)
            break;
    case 16:
        COPY_X4(16)
            break;
    case 24:
        COPY_X4(24)
            break;
    case 32:
        COPY_X4(32)
            break;
    case 48:
        COPY_X4(48)
            break;
    case 64:
        COPY_X4(64)
            break;
    }
}

void uavs3e_ipred_ang_x_6_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height)
{
    ALIGNED_16(pel first_line[64 + 64]);
    int line_size = width + height - 1;
    int real_size = COM_MIN(line_size, width * 2);
    int i;
    __m128i zero = _mm_setzero_si128();
    __m128i offset = _mm_set1_epi16(2);
    src += 2;

    for (i = 0; i < real_size - 8; i += 16, src += 16) {
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);

        __m128i H0 = _mm_unpackhi_epi8(S0, zero);
        __m128i H1 = _mm_unpackhi_epi8(S1, zero);
        __m128i H2 = _mm_unpackhi_epi8(S2, zero);

        __m128i sum1 = _mm_add_epi16(L0, L1);
        __m128i sum2 = _mm_add_epi16(L1, L2);
        __m128i sum3 = _mm_add_epi16(H0, H1);
        __m128i sum4 = _mm_add_epi16(H1, H2);

        sum1 = _mm_add_epi16(sum1, sum2);
        sum3 = _mm_add_epi16(sum3, sum4);

        sum1 = _mm_add_epi16(sum1, offset);
        sum3 = _mm_add_epi16(sum3, offset);

        sum1 = _mm_srli_epi16(sum1, 2);
        sum3 = _mm_srli_epi16(sum3, 2);

        sum1 = _mm_packus_epi16(sum1, sum3);

        _mm_store_si128((__m128i *)&first_line[i], sum1);
    }

    if (i < real_size) {
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);

        __m128i sum1 = _mm_add_epi16(L0, L1);
        __m128i sum2 = _mm_add_epi16(L1, L2);

        sum1 = _mm_add_epi16(sum1, sum2);
        sum1 = _mm_add_epi16(sum1, offset);
        sum1 = _mm_srli_epi16(sum1, 2);

        sum1 = _mm_packus_epi16(sum1, sum1);
        _mm_storel_epi64((__m128i *)&first_line[i], sum1);
    }

    // padding
    for (i = real_size; i < line_size; i += 16) {
        __m128i pad = _mm_set1_epi8((char)first_line[real_size - 1]);
        _mm_storeu_si128((__m128i *)&first_line[i], pad);
    }

    if (width > 16 || width == 4) {
        for (i = 0; i < height; i++) {
            memcpy(dst, first_line + i, width * sizeof(pel));
            dst += i_dst;
        }
    } else if (width == 16) {
        pel *dst1;

        for (i = 0; i < height; i += 4) {
            __m128i M = _mm_loadu_si128((__m128i *)&first_line[i]);
            dst1 = dst;
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 = dst + 8;
            M = _mm_loadu_si128((__m128i *)&first_line[i + 8]);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst += i_dst << 2;
        }
    } else {
        for (i = 0; i < height; i += 8) {
            __m128i M = _mm_loadu_si128((__m128i *)&first_line[i]);
            _mm_storel_epi64((__m128i *)dst, M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst, M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst, M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst, M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst, M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst, M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst, M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst, M);
            dst += i_dst;
        }
    }
}

void uavs3e_ipred_ang_x_8_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height)
{
    ALIGNED_16(pel first_line[2 *(64 + 48)]);
    int line_size = width + height / 2 - 1;
    int real_size = COM_MIN(line_size, width * 2 + 1);
    int i;
    __m128i pad1, pad2;
    int aligned_line_size = ((line_size + 31) >> 4) << 4;
    pel *pfirst[2];

    __m128i zero = _mm_setzero_si128();
    __m128i coeff = _mm_set1_epi16(3);
    __m128i offset1 = _mm_set1_epi16(4);
    __m128i offset2 = _mm_set1_epi16(2);
    int i_dst2 = i_dst * 2;

    pfirst[0] = first_line;
    pfirst[1] = first_line + aligned_line_size;

    for (i = 0; i < real_size - 8; i += 16, src += 16) {
        __m128i p01, p02, p11, p12;
        __m128i S0 = _mm_loadu_si128((__m128i *)(src));
        __m128i S3 = _mm_loadu_si128((__m128i *)(src + 3));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 2));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);
        __m128i L3 = _mm_unpacklo_epi8(S3, zero);

        __m128i H0 = _mm_unpackhi_epi8(S0, zero);
        __m128i H1 = _mm_unpackhi_epi8(S1, zero);
        __m128i H2 = _mm_unpackhi_epi8(S2, zero);
        __m128i H3 = _mm_unpackhi_epi8(S3, zero);

        p01 = _mm_add_epi16(L1, L2);
        p01 = _mm_mullo_epi16(p01, coeff);
        p02 = _mm_add_epi16(L0, L3);
        p02 = _mm_add_epi16(p02, offset1);
        p01 = _mm_add_epi16(p01, p02);
        p01 = _mm_srli_epi16(p01, 3);

        p11 = _mm_add_epi16(H1, H2);
        p11 = _mm_mullo_epi16(p11, coeff);
        p12 = _mm_add_epi16(H0, H3);
        p12 = _mm_add_epi16(p12, offset1);
        p11 = _mm_add_epi16(p11, p12);
        p11 = _mm_srli_epi16(p11, 3);

        p01 = _mm_packus_epi16(p01, p11);
        _mm_storeu_si128((__m128i *)&pfirst[0][i], p01);

        p01 = _mm_add_epi16(L1, L2);
        p02 = _mm_add_epi16(L2, L3);
        p11 = _mm_add_epi16(H1, H2);
        p12 = _mm_add_epi16(H2, H3);

        p01 = _mm_add_epi16(p01, p02);
        p11 = _mm_add_epi16(p11, p12);

        p01 = _mm_add_epi16(p01, offset2);
        p11 = _mm_add_epi16(p11, offset2);

        p01 = _mm_srli_epi16(p01, 2);
        p11 = _mm_srli_epi16(p11, 2);

        p01 = _mm_packus_epi16(p01, p11);
        _mm_storeu_si128((__m128i *)&pfirst[1][i], p01);
    }

    if (i < real_size) {
        __m128i p01, p02;
        __m128i S0 = _mm_loadu_si128((__m128i *)(src));
        __m128i S3 = _mm_loadu_si128((__m128i *)(src + 3));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 2));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);
        __m128i L3 = _mm_unpacklo_epi8(S3, zero);

        p01 = _mm_add_epi16(L1, L2);
        p01 = _mm_mullo_epi16(p01, coeff);
        p02 = _mm_add_epi16(L0, L3);
        p02 = _mm_add_epi16(p02, offset1);
        p01 = _mm_add_epi16(p01, p02);
        p01 = _mm_srli_epi16(p01, 3);

        p01 = _mm_packus_epi16(p01, p01);
        _mm_storel_epi64((__m128i *)&pfirst[0][i], p01);

        p01 = _mm_add_epi16(L1, L2);
        p02 = _mm_add_epi16(L2, L3);

        p01 = _mm_add_epi16(p01, p02);
        p01 = _mm_add_epi16(p01, offset2);
        p01 = _mm_srli_epi16(p01, 2);

        p01 = _mm_packus_epi16(p01, p01);
        _mm_storel_epi64((__m128i *)&pfirst[1][i], p01);
    }

    // padding
    if (real_size < line_size) {
        pfirst[1][real_size - 1] = pfirst[1][real_size - 2];

        pad1 = _mm_set1_epi8((char)pfirst[0][real_size - 1]);
        pad2 = _mm_set1_epi8((char)pfirst[1][real_size - 1]);
        for (i = real_size; i < line_size; i += 16) {
            _mm_storeu_si128((__m128i *)&pfirst[0][i], pad1);
            _mm_storeu_si128((__m128i *)&pfirst[1][i], pad2);
        }
    }

    height /= 2;

#define COPY_X8(w) { \
    for (i = 0; i < height; i++) { \
        memcpy(dst, pfirst[0] + i, w * sizeof(pel)); \
        memcpy(dst + i_dst, pfirst[1] + i, w * sizeof(pel)); \
        dst += i_dst2; \
    } \
}
    switch (width) {
    case 4:
        COPY_X8(4)
            break;
    case 8:
        COPY_X8(8)
            break;
    case 12:
        COPY_X8(12)
            break;
    case 16:
        COPY_X8(16)
            break;
    case 24:
        COPY_X8(24)
            break;
    case 32:
        COPY_X8(32)
            break;
    case 48:
        COPY_X8(48)
            break;
    case 64:
        COPY_X8(64)
            break;
    }
}

void uavs3e_ipred_ang_x_10_sse(pel *src, pel *dst, int i_dst, int mode, int width, int height)
{
    int i;
    pel *dst1 = dst;
    pel *dst2 = dst1 + i_dst;
    pel *dst3 = dst2 + i_dst;
    pel *dst4 = dst3 + i_dst;
    __m128i zero = _mm_setzero_si128();
    __m128i coeff2 = _mm_set1_epi16(2);
    __m128i coeff3 = _mm_set1_epi16(3);
    __m128i coeff4 = _mm_set1_epi16(4);
    __m128i coeff5 = _mm_set1_epi16(5);
    __m128i coeff7 = _mm_set1_epi16(7);
    __m128i coeff8 = _mm_set1_epi16(8);

    ALIGNED_16(pel first_line[4 *(64 + 32)]);
    int line_size = width + height / 4 - 1;
    int aligned_line_size = ((line_size + 31) >> 4) << 4;
    pel *pfirst[4];
    int i_dstx4 = i_dst << 2;

    pfirst[0] = first_line;
    pfirst[1] = first_line + aligned_line_size;
    pfirst[2] = first_line + aligned_line_size * 2;
    pfirst[3] = first_line + aligned_line_size * 3;

    for (i = 0; i < line_size - 8; i += 16, src += 16) {
        __m128i p00, p10, p20, p30;
        __m128i p01, p11, p21, p31;
        __m128i S0 = _mm_loadu_si128((__m128i *)(src));
        __m128i S3 = _mm_loadu_si128((__m128i *)(src + 3));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 2));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);
        __m128i L3 = _mm_unpacklo_epi8(S3, zero);

        __m128i H0 = _mm_unpackhi_epi8(S0, zero);
        __m128i H1 = _mm_unpackhi_epi8(S1, zero);
        __m128i H2 = _mm_unpackhi_epi8(S2, zero);
        __m128i H3 = _mm_unpackhi_epi8(S3, zero);

        p00 = _mm_mullo_epi16(L0, coeff3);
        p10 = _mm_mullo_epi16(L1, coeff7);
        p20 = _mm_mullo_epi16(L2, coeff5);
        p30 = _mm_add_epi16(L3, coeff8);
        p00 = _mm_add_epi16(p00, p30);
        p00 = _mm_add_epi16(p00, p10);
        p00 = _mm_add_epi16(p00, p20);
        p00 = _mm_srli_epi16(p00, 4);

        p01 = _mm_mullo_epi16(H0, coeff3);
        p11 = _mm_mullo_epi16(H1, coeff7);
        p21 = _mm_mullo_epi16(H2, coeff5);
        p31 = _mm_add_epi16(H3, coeff8);
        p01 = _mm_add_epi16(p01, p31);
        p01 = _mm_add_epi16(p01, p11);
        p01 = _mm_add_epi16(p01, p21);
        p01 = _mm_srli_epi16(p01, 4);

        p00 = _mm_packus_epi16(p00, p01);
        _mm_store_si128((__m128i *)&pfirst[0][i], p00);

        p00 = _mm_add_epi16(L1, L2);
        p00 = _mm_mullo_epi16(p00, coeff3);
        p10 = _mm_add_epi16(L0, L3);
        p10 = _mm_add_epi16(p10, coeff4);
        p00 = _mm_add_epi16(p10, p00);
        p00 = _mm_srli_epi16(p00, 3);

        p01 = _mm_add_epi16(H1, H2);
        p01 = _mm_mullo_epi16(p01, coeff3);
        p11 = _mm_add_epi16(H0, H3);
        p11 = _mm_add_epi16(p11, coeff4);
        p01 = _mm_add_epi16(p11, p01);
        p01 = _mm_srli_epi16(p01, 3);

        p00 = _mm_packus_epi16(p00, p01);
        _mm_store_si128((__m128i *)&pfirst[1][i], p00);

        p10 = _mm_mullo_epi16(L1, coeff5);
        p20 = _mm_mullo_epi16(L2, coeff7);
        p30 = _mm_mullo_epi16(L3, coeff3);
        p00 = _mm_add_epi16(L0, coeff8);
        p00 = _mm_add_epi16(p00, p10);
        p00 = _mm_add_epi16(p00, p20);
        p00 = _mm_add_epi16(p00, p30);
        p00 = _mm_srli_epi16(p00, 4);

        p11 = _mm_mullo_epi16(H1, coeff5);
        p21 = _mm_mullo_epi16(H2, coeff7);
        p31 = _mm_mullo_epi16(H3, coeff3);
        p01 = _mm_add_epi16(H0, coeff8);
        p01 = _mm_add_epi16(p01, p11);
        p01 = _mm_add_epi16(p01, p21);
        p01 = _mm_add_epi16(p01, p31);
        p01 = _mm_srli_epi16(p01, 4);

        p00 = _mm_packus_epi16(p00, p01);
        _mm_store_si128((__m128i *)&pfirst[2][i], p00);

        p00 = _mm_add_epi16(L1, L2);
        p10 = _mm_add_epi16(L2, L3);
        p00 = _mm_add_epi16(p00, p10);
        p00 = _mm_add_epi16(p00, coeff2);
        p00 = _mm_srli_epi16(p00, 2);

        p01 = _mm_add_epi16(H1, H2);
        p11 = _mm_add_epi16(H2, H3);
        p01 = _mm_add_epi16(p01, p11);
        p01 = _mm_add_epi16(p01, coeff2);
        p01 = _mm_srli_epi16(p01, 2);

        p00 = _mm_packus_epi16(p00, p01);
        _mm_store_si128((__m128i *)&pfirst[3][i], p00);
    }

    if (i < line_size) {
        __m128i p00, p10, p20, p30;
        __m128i S0 = _mm_loadu_si128((__m128i *)(src));
        __m128i S3 = _mm_loadu_si128((__m128i *)(src + 3));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 2));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);
        __m128i L3 = _mm_unpacklo_epi8(S3, zero);

        p00 = _mm_mullo_epi16(L0, coeff3);
        p10 = _mm_mullo_epi16(L1, coeff7);
        p20 = _mm_mullo_epi16(L2, coeff5);
        p30 = _mm_add_epi16(L3, coeff8);
        p00 = _mm_add_epi16(p00, p30);
        p00 = _mm_add_epi16(p00, p10);
        p00 = _mm_add_epi16(p00, p20);
        p00 = _mm_srli_epi16(p00, 4);

        p00 = _mm_packus_epi16(p00, p00);
        _mm_storel_epi64((__m128i *)&pfirst[0][i], p00);

        p00 = _mm_add_epi16(L1, L2);
        p00 = _mm_mullo_epi16(p00, coeff3);
        p10 = _mm_add_epi16(L0, L3);
        p10 = _mm_add_epi16(p10, coeff4);
        p00 = _mm_add_epi16(p10, p00);
        p00 = _mm_srli_epi16(p00, 3);

        p00 = _mm_packus_epi16(p00, p00);
        _mm_storel_epi64((__m128i *)&pfirst[1][i], p00);

        p10 = _mm_mullo_epi16(L1, coeff5);
        p20 = _mm_mullo_epi16(L2, coeff7);
        p30 = _mm_mullo_epi16(L3, coeff3);
        p00 = _mm_add_epi16(L0, coeff8);
        p00 = _mm_add_epi16(p00, p10);
        p00 = _mm_add_epi16(p00, p20);
        p00 = _mm_add_epi16(p00, p30);
        p00 = _mm_srli_epi16(p00, 4);

        p00 = _mm_packus_epi16(p00, p00);
        _mm_storel_epi64((__m128i *)&pfirst[2][i], p00);

        p00 = _mm_add_epi16(L1, L2);
        p10 = _mm_add_epi16(L2, L3);
        p00 = _mm_add_epi16(p00, p10);
        p00 = _mm_add_epi16(p00, coeff2);
        p00 = _mm_srli_epi16(p00, 2);

        p00 = _mm_packus_epi16(p00, p00);
        _mm_storel_epi64((__m128i *)&pfirst[3][i], p00);
    }

    height >>= 2;

    switch (width) {
    case 4:
        for (i = 0; i < height; i++) {
            CP32(dst1, pfirst[0] + i);
            dst1 += i_dstx4;
            CP32(dst2, pfirst[1] + i);
            dst2 += i_dstx4;
            CP32(dst3, pfirst[2] + i);
            dst3 += i_dstx4;
            CP32(dst4, pfirst[3] + i);
            dst4 += i_dstx4;
        }
        break;
    case 8:
        for (i = 0; i < height; i++) {
            CP64(dst1, pfirst[0] + i);
            dst1 += i_dstx4;
            CP64(dst2, pfirst[1] + i);
            dst2 += i_dstx4;
            CP64(dst3, pfirst[2] + i);
            dst3 += i_dstx4;
            CP64(dst4, pfirst[3] + i);
            dst4 += i_dstx4;
        }
        break;
    case 16:
        for (i = 0; i < height; i++) {
            CP128(dst1, pfirst[0] + i);
            dst1 += i_dstx4;
            CP128(dst2, pfirst[1] + i);
            dst2 += i_dstx4;
            CP128(dst3, pfirst[2] + i);
            dst3 += i_dstx4;
            CP128(dst4, pfirst[3] + i);
            dst4 += i_dstx4;
        }
        break;
    case 32:
        for (i = 0; i < height; i++) {
            memcpy(dst1, pfirst[0] + i, 32 * sizeof(pel));
            dst1 += i_dstx4;
            memcpy(dst2, pfirst[1] + i, 32 * sizeof(pel));
            dst2 += i_dstx4;
            memcpy(dst3, pfirst[2] + i, 32 * sizeof(pel));
            dst3 += i_dstx4;
            memcpy(dst4, pfirst[3] + i, 32 * sizeof(pel));
            dst4 += i_dstx4;
        }
        break;
    case 64:
        for (i = 0; i < height; i++) {
            memcpy(dst1, pfirst[0] + i, 64 * sizeof(pel));
            dst1 += i_dstx4;
            memcpy(dst2, pfirst[1] + i, 64 * sizeof(pel));
            dst2 += i_dstx4;
            memcpy(dst3, pfirst[2] + i, 64 * sizeof(pel));
            dst3 += i_dstx4;
            memcpy(dst4, pfirst[3] + i, 64 * sizeof(pel));
            dst4 += i_dstx4;
        }
        break;
    default:
        com_assert(0);
        break;
    }
}

static avs3_always_inline void transpose_4x4(pel *src, pel *dst, int i_dst)
{
    __m128i t0, t1, t2, t3;

    t0 = _mm_loadu_si128((__m128i *)src);
    t1 = _mm_srli_si128(t0, 4);
    t2 = _mm_srli_si128(t0, 8);
    t3 = _mm_srli_si128(t0, 12);

    t0 = _mm_unpacklo_epi8(t0, t1);
    t2 = _mm_unpacklo_epi8(t2, t3);

    t0 = _mm_unpacklo_epi16(t0, t2);

    ((u32 *)dst)[0] = _mm_extract_epi32(t0, 0); dst += i_dst;
    ((u32 *)dst)[0] = _mm_extract_epi32(t0, 1); dst += i_dst;
    ((u32 *)dst)[0] = _mm_extract_epi32(t0, 2); dst += i_dst;
    ((u32 *)dst)[0] = _mm_extract_epi32(t0, 3);
}
static avs3_always_inline void transpose_4x16(pel *dst, pel *dst1, int i_dst1)
{
    //  srcStride = dstStride = 16
    __m128i s0, s1, s2, s3;
    __m128i t0, t1, t2, t3;

    s0 = _mm_loadl_epi64((__m128i *)dst);
    s1 = _mm_loadl_epi64((__m128i *)(dst + 16));
    s2 = _mm_loadl_epi64((__m128i *)(dst + 32));
    s3 = _mm_loadl_epi64((__m128i *)(dst + 48));

    s0 = _mm_unpacklo_epi8(s0, s1);
    s2 = _mm_unpacklo_epi8(s2, s3);

    t0 = _mm_unpacklo_epi16(s0, s2);

    dst += 64;
    s0 = _mm_loadl_epi64((__m128i *)dst);
    s1 = _mm_loadl_epi64((__m128i *)(dst + 16));
    s2 = _mm_loadl_epi64((__m128i *)(dst + 32));
    s3 = _mm_loadl_epi64((__m128i *)(dst + 48));

    s0 = _mm_unpacklo_epi8(s0, s1);
    s2 = _mm_unpacklo_epi8(s2, s3);

    t1 = _mm_unpacklo_epi16(s0, s2);

    dst += 64;
    s0 = _mm_loadl_epi64((__m128i *)dst);
    s1 = _mm_loadl_epi64((__m128i *)(dst + 16));
    s2 = _mm_loadl_epi64((__m128i *)(dst + 32));
    s3 = _mm_loadl_epi64((__m128i *)(dst + 48));

    s0 = _mm_unpacklo_epi8(s0, s1);
    s2 = _mm_unpacklo_epi8(s2, s3);

    t2 = _mm_unpacklo_epi16(s0, s2);

    dst += 64;
    s0 = _mm_loadl_epi64((__m128i *)dst);
    s1 = _mm_loadl_epi64((__m128i *)(dst + 16));
    s2 = _mm_loadl_epi64((__m128i *)(dst + 32));
    s3 = _mm_loadl_epi64((__m128i *)(dst + 48));

    s0 = _mm_unpacklo_epi8(s0, s1);
    s2 = _mm_unpacklo_epi8(s2, s3);

    t3 = _mm_unpacklo_epi16(s0, s2);

    s0 = _mm_unpacklo_epi32(t0, t1);
    s1 = _mm_unpacklo_epi32(t2, t3);
    s2 = _mm_unpackhi_epi32(t0, t1);
    s3 = _mm_unpackhi_epi32(t2, t3);

    t0 = _mm_unpacklo_epi64(s0, s1);
    t1 = _mm_unpackhi_epi64(s0, s1);
    t2 = _mm_unpacklo_epi64(s2, s3);
    t3 = _mm_unpackhi_epi64(s2, s3);

    _mm_store_si128((__m128i *)dst1, t0);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, t1);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, t2);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, t3);
}

static avs3_always_inline void transpose_8x8_stride(pel *src, int i_src, pel *dst, int i_dst)
{
    __m128i t0, t1, t2, t3, t4, t5, t6, t7;
    pel *pDst = src;

    t0 = _mm_loadl_epi64((__m128i *)pDst);
    pDst += i_src;
    t1 = _mm_loadl_epi64((__m128i *)pDst);
    pDst += i_src;
    t2 = _mm_loadl_epi64((__m128i *)pDst);
    pDst += i_src;
    t3 = _mm_loadl_epi64((__m128i *)pDst);
    pDst += i_src;
    t4 = _mm_loadl_epi64((__m128i *)pDst);
    pDst += i_src;
    t5 = _mm_loadl_epi64((__m128i *)pDst);
    pDst += i_src;
    t6 = _mm_loadl_epi64((__m128i *)pDst);
    pDst += i_src;
    t7 = _mm_loadl_epi64((__m128i *)pDst);

    t0 = _mm_unpacklo_epi8(t0, t1);
    t2 = _mm_unpacklo_epi8(t2, t3);
    t4 = _mm_unpacklo_epi8(t4, t5);
    t6 = _mm_unpacklo_epi8(t6, t7);

    t1 = _mm_unpacklo_epi16(t0, t2);
    t3 = _mm_unpacklo_epi16(t4, t6);

    t5 = _mm_unpackhi_epi16(t0, t2);
    t7 = _mm_unpackhi_epi16(t4, t6);

    t0 = _mm_unpacklo_epi32(t1, t3);
    t2 = _mm_unpackhi_epi32(t1, t3);

    t4 = _mm_unpacklo_epi32(t5, t7);
    t6 = _mm_unpackhi_epi32(t5, t7);

    pDst = dst;
    _mm_storel_epi64((__m128i *)pDst, t0);
    pDst += i_dst;
    _mm_storel_epi64((__m128i *)pDst, _mm_srli_si128(t0, 8));
    pDst += i_dst;
    _mm_storel_epi64((__m128i *)pDst, t2);
    pDst += i_dst;
    _mm_storel_epi64((__m128i *)pDst, _mm_srli_si128(t2, 8));
    pDst += i_dst;
    _mm_storel_epi64((__m128i *)pDst, t4);
    pDst += i_dst;
    _mm_storel_epi64((__m128i *)pDst, _mm_srli_si128(t4, 8));
    pDst += i_dst;
    _mm_storel_epi64((__m128i *)pDst, t6);
    pDst += i_dst;
    _mm_storel_epi64((__m128i *)pDst, _mm_srli_si128(t6, 8));
}
static avs3_always_inline void transpose_8x8(pel *dst, pel *dst1, int i_dst1)
{
    __m128i t0, t1, t2, t3, t4, t5, t6, t7;

    t0 = _mm_load_si128((__m128i *)(dst));
    t1 = _mm_srli_si128(t0, 8);
    t2 = _mm_load_si128((__m128i *)(dst + 16));
    t3 = _mm_srli_si128(t2, 8);
    t4 = _mm_load_si128((__m128i *)(dst + 32));
    t5 = _mm_srli_si128(t4, 8);
    t6 = _mm_load_si128((__m128i *)(dst + 48));
    t7 = _mm_srli_si128(t6, 8);

    t0 = _mm_unpacklo_epi8(t0, t1);
    t2 = _mm_unpacklo_epi8(t2, t3);
    t4 = _mm_unpacklo_epi8(t4, t5);
    t6 = _mm_unpacklo_epi8(t6, t7);

    t1 = _mm_unpacklo_epi16(t0, t2);
    t3 = _mm_unpacklo_epi16(t4, t6);

    t5 = _mm_unpackhi_epi16(t0, t2);
    t7 = _mm_unpackhi_epi16(t4, t6);

    t0 = _mm_unpacklo_epi32(t1, t3);
    t2 = _mm_unpackhi_epi32(t1, t3);

    t4 = _mm_unpacklo_epi32(t5, t7);
    t6 = _mm_unpackhi_epi32(t5, t7);

    _mm_storel_epi64((__m128i *)dst1, t0);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t0, 8));
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, t2);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t2, 8));
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, t4);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t4, 8));
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, t6);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t6, 8));
}
static avs3_always_inline void transpose_8x32(pel *dst, pel *dst1, int i_dst1)
{
    transpose_8x8_stride(dst, 32, dst1, i_dst1);
    transpose_8x8_stride(dst + 8 * 32, 32, dst1 + 8, i_dst1);
    transpose_8x8_stride(dst + 16 * 32, 32, dst1 + 16, i_dst1);
    transpose_8x8_stride(dst + 24 * 32, 32, dst1 + 24, i_dst1);
}

static avs3_always_inline void transpose_16x16(pel *dst, pel *dst1, int i_dst1)
{
    __m128i r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15;
    __m128i t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15;

    r0 = _mm_load_si128((__m128i *)dst);
    r1 = _mm_load_si128((__m128i *)(dst + 16));
    r2 = _mm_load_si128((__m128i *)(dst + 16 * 2));
    r3 = _mm_load_si128((__m128i *)(dst + 16 * 3));
    r4 = _mm_load_si128((__m128i *)(dst + 16 * 4));
    r5 = _mm_load_si128((__m128i *)(dst + 16 * 5));
    r6 = _mm_load_si128((__m128i *)(dst + 16 * 6));
    r7 = _mm_load_si128((__m128i *)(dst + 16 * 7));
    r8 = _mm_load_si128((__m128i *)(dst + 16 * 8));
    r9 = _mm_load_si128((__m128i *)(dst + 16 * 9));
    r10 = _mm_load_si128((__m128i *)(dst + 16 * 10));
    r11 = _mm_load_si128((__m128i *)(dst + 16 * 11));
    r12 = _mm_load_si128((__m128i *)(dst + 16 * 12));
    r13 = _mm_load_si128((__m128i *)(dst + 16 * 13));
    r14 = _mm_load_si128((__m128i *)(dst + 16 * 14));
    r15 = _mm_load_si128((__m128i *)(dst + 16 * 15));

    t0 = _mm_unpacklo_epi8(r0, r1);
    t1 = _mm_unpacklo_epi8(r2, r3);
    t2 = _mm_unpacklo_epi8(r4, r5);
    t3 = _mm_unpacklo_epi8(r6, r7);
    t4 = _mm_unpacklo_epi8(r8, r9);
    t5 = _mm_unpacklo_epi8(r10, r11);
    t6 = _mm_unpacklo_epi8(r12, r13);
    t7 = _mm_unpacklo_epi8(r14, r15);
    t8 = _mm_unpackhi_epi8(r0, r1);
    t9 = _mm_unpackhi_epi8(r2, r3);
    t10 = _mm_unpackhi_epi8(r4, r5);
    t11 = _mm_unpackhi_epi8(r6, r7);
    t12 = _mm_unpackhi_epi8(r8, r9);
    t13 = _mm_unpackhi_epi8(r10, r11);
    t14 = _mm_unpackhi_epi8(r12, r13);
    t15 = _mm_unpackhi_epi8(r14, r15);

    r0 = _mm_unpacklo_epi16(t0, t1);
    r1 = _mm_unpacklo_epi16(t2, t3);
    r2 = _mm_unpacklo_epi16(t4, t5);
    r3 = _mm_unpacklo_epi16(t6, t7);
    r4 = _mm_unpackhi_epi16(t0, t1);
    r5 = _mm_unpackhi_epi16(t2, t3);
    r6 = _mm_unpackhi_epi16(t4, t5);
    r7 = _mm_unpackhi_epi16(t6, t7);
    r8 = _mm_unpacklo_epi16(t8, t9);
    r9 = _mm_unpacklo_epi16(t10, t11);
    r10 = _mm_unpacklo_epi16(t12, t13);
    r11 = _mm_unpacklo_epi16(t14, t15);
    r12 = _mm_unpackhi_epi16(t8, t9);
    r13 = _mm_unpackhi_epi16(t10, t11);
    r14 = _mm_unpackhi_epi16(t12, t13);
    r15 = _mm_unpackhi_epi16(t14, t15);

    t0 = _mm_unpacklo_epi32(r0, r1);
    t1 = _mm_unpacklo_epi32(r2, r3);
    t2 = _mm_unpackhi_epi32(r0, r1);
    t3 = _mm_unpackhi_epi32(r2, r3);
    t4 = _mm_unpacklo_epi32(r4, r5);
    t5 = _mm_unpacklo_epi32(r6, r7);
    t6 = _mm_unpackhi_epi32(r4, r5);
    t7 = _mm_unpackhi_epi32(r6, r7);
    t8 = _mm_unpacklo_epi32(r8, r9);
    t9 = _mm_unpacklo_epi32(r10, r11);
    t10 = _mm_unpackhi_epi32(r8, r9);
    t11 = _mm_unpackhi_epi32(r10, r11);
    t12 = _mm_unpacklo_epi32(r12, r13);
    t13 = _mm_unpacklo_epi32(r14, r15);
    t14 = _mm_unpackhi_epi32(r12, r13);
    t15 = _mm_unpackhi_epi32(r14, r15);

    r0 = _mm_unpacklo_epi64(t0, t1);
    r1 = _mm_unpackhi_epi64(t0, t1);
    r2 = _mm_unpacklo_epi64(t2, t3);
    r3 = _mm_unpackhi_epi64(t2, t3);
    r4 = _mm_unpacklo_epi64(t4, t5);
    r5 = _mm_unpackhi_epi64(t4, t5);
    r6 = _mm_unpacklo_epi64(t6, t7);
    r7 = _mm_unpackhi_epi64(t6, t7);
    r8 = _mm_unpacklo_epi64(t8, t9);
    r9 = _mm_unpackhi_epi64(t8, t9);
    r10 = _mm_unpacklo_epi64(t10, t11);
    r11 = _mm_unpackhi_epi64(t10, t11);
    r12 = _mm_unpacklo_epi64(t12, t13);
    r13 = _mm_unpackhi_epi64(t12, t13);
    r14 = _mm_unpacklo_epi64(t14, t15);
    r15 = _mm_unpackhi_epi64(t14, t15);

    _mm_store_si128((__m128i *)dst1, r0);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, r1);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, r2);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, r3);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, r4);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, r5);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, r6);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, r7);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, r8);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, r9);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, r10);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, r11);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, r12);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, r13);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, r14);
    dst1 += i_dst1;
    _mm_store_si128((__m128i *)dst1, r15);
}
static avs3_always_inline void transpose_16x4(pel *dst, pel *dst1, int i_dst1)
{
    __m128i r0, r1, r2, r3;
    __m128i t0, t1, t2, t3;

    r0 = _mm_load_si128((__m128i *)dst);
    r1 = _mm_load_si128((__m128i *)(dst + 16));
    r2 = _mm_load_si128((__m128i *)(dst + 32));
    r3 = _mm_load_si128((__m128i *)(dst + 48));

    t0 = _mm_unpacklo_epi8(r0, r1);
    t1 = _mm_unpacklo_epi8(r2, r3);
    t2 = _mm_unpackhi_epi8(r0, r1);
    t3 = _mm_unpackhi_epi8(r2, r3);

    r0 = _mm_unpacklo_epi16(t0, t1);
    r1 = _mm_unpackhi_epi16(t0, t1);
    r2 = _mm_unpacklo_epi16(t2, t3);
    r3 = _mm_unpackhi_epi16(t2, t3);

    ((u32 *)dst1)[0] = _mm_extract_epi32(r0, 0);
    dst1 += i_dst1;
    ((u32 *)dst1)[0] = _mm_extract_epi32(r0, 1);
    dst1 += i_dst1;
    ((u32 *)dst1)[0] = _mm_extract_epi32(r0, 2);
    dst1 += i_dst1;
    ((u32 *)dst1)[0] = _mm_extract_epi32(r0, 3);
    dst1 += i_dst1;
    ((u32 *)dst1)[0] = _mm_extract_epi32(r1, 0);
    dst1 += i_dst1;
    ((u32 *)dst1)[0] = _mm_extract_epi32(r1, 1);
    dst1 += i_dst1;
    ((u32 *)dst1)[0] = _mm_extract_epi32(r1, 2);
    dst1 += i_dst1;
    ((u32 *)dst1)[0] = _mm_extract_epi32(r1, 3);
    dst1 += i_dst1;
    ((u32 *)dst1)[0] = _mm_extract_epi32(r2, 0);
    dst1 += i_dst1;
    ((u32 *)dst1)[0] = _mm_extract_epi32(r2, 1);
    dst1 += i_dst1;
    ((u32 *)dst1)[0] = _mm_extract_epi32(r2, 2);
    dst1 += i_dst1;
    ((u32 *)dst1)[0] = _mm_extract_epi32(r2, 3);
    dst1 += i_dst1;
    ((u32 *)dst1)[0] = _mm_extract_epi32(r3, 0);
    dst1 += i_dst1;
    ((u32 *)dst1)[0] = _mm_extract_epi32(r3, 1);
    dst1 += i_dst1;
    ((u32 *)dst1)[0] = _mm_extract_epi32(r3, 2);
    dst1 += i_dst1;
    ((u32 *)dst1)[0] = _mm_extract_epi32(r3, 3);
}

static avs3_always_inline void transpose_32x32_stride(pel *src, int i_src, pel *dst, int i_dst)
{
    __m128i r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31;
    __m128i t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15;

    pel *pDst = src;

    //load the first and second 16x16 blocks
    r0 = _mm_load_si128((__m128i *)pDst);
    r16 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    r1 = _mm_load_si128((__m128i *)pDst);
    r17 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    r2 = _mm_load_si128((__m128i *)pDst);
    r18 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    r3 = _mm_load_si128((__m128i *)pDst);
    r19 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    r4 = _mm_load_si128((__m128i *)pDst);
    r20 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    r5 = _mm_load_si128((__m128i *)pDst);
    r21 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    r6 = _mm_load_si128((__m128i *)pDst);
    r22 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    r7 = _mm_load_si128((__m128i *)pDst);
    r23 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    r8 = _mm_load_si128((__m128i *)pDst);
    r24 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    r9 = _mm_load_si128((__m128i *)pDst);
    r25 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    r10 = _mm_load_si128((__m128i *)pDst);
    r26 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    r11 = _mm_load_si128((__m128i *)pDst);
    r27 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    r12 = _mm_load_si128((__m128i *)pDst);
    r28 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    r13 = _mm_load_si128((__m128i *)pDst);
    r29 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    r14 = _mm_load_si128((__m128i *)pDst);
    r30 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    r15 = _mm_load_si128((__m128i *)pDst);
    r31 = _mm_load_si128((__m128i *)(pDst + 16));

    //transpose the first block
    t0 = _mm_unpacklo_epi8(r0, r1);
    t1 = _mm_unpacklo_epi8(r2, r3);
    t2 = _mm_unpacklo_epi8(r4, r5);
    t3 = _mm_unpacklo_epi8(r6, r7);
    t4 = _mm_unpacklo_epi8(r8, r9);
    t5 = _mm_unpacklo_epi8(r10, r11);
    t6 = _mm_unpacklo_epi8(r12, r13);
    t7 = _mm_unpacklo_epi8(r14, r15);
    t8 = _mm_unpackhi_epi8(r0, r1);
    t9 = _mm_unpackhi_epi8(r2, r3);
    t10 = _mm_unpackhi_epi8(r4, r5);
    t11 = _mm_unpackhi_epi8(r6, r7);
    t12 = _mm_unpackhi_epi8(r8, r9);
    t13 = _mm_unpackhi_epi8(r10, r11);
    t14 = _mm_unpackhi_epi8(r12, r13);
    t15 = _mm_unpackhi_epi8(r14, r15);

    r0 = _mm_unpacklo_epi16(t0, t1);
    r1 = _mm_unpacklo_epi16(t2, t3);
    r2 = _mm_unpacklo_epi16(t4, t5);
    r3 = _mm_unpacklo_epi16(t6, t7);
    r4 = _mm_unpackhi_epi16(t0, t1);
    r5 = _mm_unpackhi_epi16(t2, t3);
    r6 = _mm_unpackhi_epi16(t4, t5);
    r7 = _mm_unpackhi_epi16(t6, t7);
    r8 = _mm_unpacklo_epi16(t8, t9);
    r9 = _mm_unpacklo_epi16(t10, t11);
    r10 = _mm_unpacklo_epi16(t12, t13);
    r11 = _mm_unpacklo_epi16(t14, t15);
    r12 = _mm_unpackhi_epi16(t8, t9);
    r13 = _mm_unpackhi_epi16(t10, t11);
    r14 = _mm_unpackhi_epi16(t12, t13);
    r15 = _mm_unpackhi_epi16(t14, t15);

    t0 = _mm_unpacklo_epi32(r0, r1);
    t1 = _mm_unpacklo_epi32(r2, r3);
    t2 = _mm_unpackhi_epi32(r0, r1);
    t3 = _mm_unpackhi_epi32(r2, r3);
    t4 = _mm_unpacklo_epi32(r4, r5);
    t5 = _mm_unpacklo_epi32(r6, r7);
    t6 = _mm_unpackhi_epi32(r4, r5);
    t7 = _mm_unpackhi_epi32(r6, r7);
    t8 = _mm_unpacklo_epi32(r8, r9);
    t9 = _mm_unpacklo_epi32(r10, r11);
    t10 = _mm_unpackhi_epi32(r8, r9);
    t11 = _mm_unpackhi_epi32(r10, r11);
    t12 = _mm_unpacklo_epi32(r12, r13);
    t13 = _mm_unpacklo_epi32(r14, r15);
    t14 = _mm_unpackhi_epi32(r12, r13);
    t15 = _mm_unpackhi_epi32(r14, r15);

    r0 = _mm_unpacklo_epi64(t0, t1);
    r1 = _mm_unpackhi_epi64(t0, t1);
    r2 = _mm_unpacklo_epi64(t2, t3);
    r3 = _mm_unpackhi_epi64(t2, t3);
    r4 = _mm_unpacklo_epi64(t4, t5);
    r5 = _mm_unpackhi_epi64(t4, t5);
    r6 = _mm_unpacklo_epi64(t6, t7);
    r7 = _mm_unpackhi_epi64(t6, t7);
    r8 = _mm_unpacklo_epi64(t8, t9);
    r9 = _mm_unpackhi_epi64(t8, t9);
    r10 = _mm_unpacklo_epi64(t10, t11);
    r11 = _mm_unpackhi_epi64(t10, t11);
    r12 = _mm_unpacklo_epi64(t12, t13);
    r13 = _mm_unpackhi_epi64(t12, t13);
    r14 = _mm_unpacklo_epi64(t14, t15);
    r15 = _mm_unpackhi_epi64(t14, t15);

    //write back the first block
    pDst = dst;
    _mm_store_si128((__m128i *)pDst, r0);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r1);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r2);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r3);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r4);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r5);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r6);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r7);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r8);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r9);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r10);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r11);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r12);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r13);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r14);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r15);

    //transpose the second block
    t0 = _mm_unpacklo_epi8(r16, r17);
    t1 = _mm_unpacklo_epi8(r18, r19);
    t2 = _mm_unpacklo_epi8(r20, r21);
    t3 = _mm_unpacklo_epi8(r22, r23);
    t4 = _mm_unpacklo_epi8(r24, r25);
    t5 = _mm_unpacklo_epi8(r26, r27);
    t6 = _mm_unpacklo_epi8(r28, r29);
    t7 = _mm_unpacklo_epi8(r30, r31);
    t8 = _mm_unpackhi_epi8(r16, r17);
    t9 = _mm_unpackhi_epi8(r18, r19);
    t10 = _mm_unpackhi_epi8(r20, r21);
    t11 = _mm_unpackhi_epi8(r22, r23);
    t12 = _mm_unpackhi_epi8(r24, r25);
    t13 = _mm_unpackhi_epi8(r26, r27);
    t14 = _mm_unpackhi_epi8(r28, r29);
    t15 = _mm_unpackhi_epi8(r30, r31);

    r0 = _mm_unpacklo_epi16(t0, t1);
    r1 = _mm_unpacklo_epi16(t2, t3);
    r2 = _mm_unpacklo_epi16(t4, t5);
    r3 = _mm_unpacklo_epi16(t6, t7);
    r4 = _mm_unpackhi_epi16(t0, t1);
    r5 = _mm_unpackhi_epi16(t2, t3);
    r6 = _mm_unpackhi_epi16(t4, t5);
    r7 = _mm_unpackhi_epi16(t6, t7);
    r8 = _mm_unpacklo_epi16(t8, t9);
    r9 = _mm_unpacklo_epi16(t10, t11);
    r10 = _mm_unpacklo_epi16(t12, t13);
    r11 = _mm_unpacklo_epi16(t14, t15);
    r12 = _mm_unpackhi_epi16(t8, t9);
    r13 = _mm_unpackhi_epi16(t10, t11);
    r14 = _mm_unpackhi_epi16(t12, t13);
    r15 = _mm_unpackhi_epi16(t14, t15);

    t0 = _mm_unpacklo_epi32(r0, r1);
    t1 = _mm_unpacklo_epi32(r2, r3);
    t2 = _mm_unpackhi_epi32(r0, r1);
    t3 = _mm_unpackhi_epi32(r2, r3);
    t4 = _mm_unpacklo_epi32(r4, r5);
    t5 = _mm_unpacklo_epi32(r6, r7);
    t6 = _mm_unpackhi_epi32(r4, r5);
    t7 = _mm_unpackhi_epi32(r6, r7);
    t8 = _mm_unpacklo_epi32(r8, r9);
    t9 = _mm_unpacklo_epi32(r10, r11);
    t10 = _mm_unpackhi_epi32(r8, r9);
    t11 = _mm_unpackhi_epi32(r10, r11);
    t12 = _mm_unpacklo_epi32(r12, r13);
    t13 = _mm_unpacklo_epi32(r14, r15);
    t14 = _mm_unpackhi_epi32(r12, r13);
    t15 = _mm_unpackhi_epi32(r14, r15);

    r0 = _mm_unpacklo_epi64(t0, t1);
    r1 = _mm_unpackhi_epi64(t0, t1);
    r2 = _mm_unpacklo_epi64(t2, t3);
    r3 = _mm_unpackhi_epi64(t2, t3);
    r4 = _mm_unpacklo_epi64(t4, t5);
    r5 = _mm_unpackhi_epi64(t4, t5);
    r6 = _mm_unpacklo_epi64(t6, t7);
    r7 = _mm_unpackhi_epi64(t6, t7);
    r8 = _mm_unpacklo_epi64(t8, t9);
    r9 = _mm_unpackhi_epi64(t8, t9);
    r10 = _mm_unpacklo_epi64(t10, t11);
    r11 = _mm_unpackhi_epi64(t10, t11);
    r12 = _mm_unpacklo_epi64(t12, t13);
    r13 = _mm_unpackhi_epi64(t12, t13);
    r14 = _mm_unpacklo_epi64(t14, t15);
    r15 = _mm_unpackhi_epi64(t14, t15);

    //load the third and fourth 16x16 blocks
    pDst = src + (i_src << 4);
    t0 = _mm_load_si128((__m128i *)pDst);
    r16 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    t1 = _mm_load_si128((__m128i *)pDst);
    r17 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    t2 = _mm_load_si128((__m128i *)pDst);
    r18 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    t3 = _mm_load_si128((__m128i *)pDst);
    r19 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    t4 = _mm_load_si128((__m128i *)pDst);
    r20 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    t5 = _mm_load_si128((__m128i *)pDst);
    r21 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    t6 = _mm_load_si128((__m128i *)pDst);
    r22 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    t7 = _mm_load_si128((__m128i *)pDst);
    r23 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    t8 = _mm_load_si128((__m128i *)pDst);
    r24 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    t9 = _mm_load_si128((__m128i *)pDst);
    r25 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    t10 = _mm_load_si128((__m128i *)pDst);
    r26 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    t11 = _mm_load_si128((__m128i *)pDst);
    r27 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    t12 = _mm_load_si128((__m128i *)pDst);
    r28 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    t13 = _mm_load_si128((__m128i *)pDst);
    r29 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    t14 = _mm_load_si128((__m128i *)pDst);
    r30 = _mm_load_si128((__m128i *)(pDst + 16));
    pDst += i_src;
    t15 = _mm_load_si128((__m128i *)pDst);
    r31 = _mm_load_si128((__m128i *)(pDst + 16));

    //store the transposition of the second block to the position of third block
    pDst = dst + (i_dst << 4);
    _mm_store_si128((__m128i *)pDst, r0);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r1);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r2);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r3);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r4);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r5);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r6);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r7);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r8);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r9);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r10);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r11);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r12);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r13);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r14);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r15);

    //transpose the third block
    r0 = _mm_unpacklo_epi8(t0, t1);
    r1 = _mm_unpacklo_epi8(t2, t3);
    r2 = _mm_unpacklo_epi8(t4, t5);
    r3 = _mm_unpacklo_epi8(t6, t7);
    r4 = _mm_unpacklo_epi8(t8, t9);
    r5 = _mm_unpacklo_epi8(t10, t11);
    r6 = _mm_unpacklo_epi8(t12, t13);
    r7 = _mm_unpacklo_epi8(t14, t15);
    r8 = _mm_unpackhi_epi8(t0, t1);
    r9 = _mm_unpackhi_epi8(t2, t3);
    r10 = _mm_unpackhi_epi8(t4, t5);
    r11 = _mm_unpackhi_epi8(t6, t7);
    r12 = _mm_unpackhi_epi8(t8, t9);
    r13 = _mm_unpackhi_epi8(t10, t11);
    r14 = _mm_unpackhi_epi8(t12, t13);
    r15 = _mm_unpackhi_epi8(t14, t15);

    t0 = _mm_unpacklo_epi16(r0, r1);
    t1 = _mm_unpacklo_epi16(r2, r3);
    t2 = _mm_unpacklo_epi16(r4, r5);
    t3 = _mm_unpacklo_epi16(r6, r7);
    t4 = _mm_unpackhi_epi16(r0, r1);
    t5 = _mm_unpackhi_epi16(r2, r3);
    t6 = _mm_unpackhi_epi16(r4, r5);
    t7 = _mm_unpackhi_epi16(r6, r7);
    t8 = _mm_unpacklo_epi16(r8, r9);
    t9 = _mm_unpacklo_epi16(r10, r11);
    t10 = _mm_unpacklo_epi16(r12, r13);
    t11 = _mm_unpacklo_epi16(r14, r15);
    t12 = _mm_unpackhi_epi16(r8, r9);
    t13 = _mm_unpackhi_epi16(r10, r11);
    t14 = _mm_unpackhi_epi16(r12, r13);
    t15 = _mm_unpackhi_epi16(r14, r15);

    r0 = _mm_unpacklo_epi32(t0, t1);
    r1 = _mm_unpacklo_epi32(t2, t3);
    r2 = _mm_unpackhi_epi32(t0, t1);
    r3 = _mm_unpackhi_epi32(t2, t3);
    r4 = _mm_unpacklo_epi32(t4, t5);
    r5 = _mm_unpacklo_epi32(t6, t7);
    r6 = _mm_unpackhi_epi32(t4, t5);
    r7 = _mm_unpackhi_epi32(t6, t7);
    r8 = _mm_unpacklo_epi32(t8, t9);
    r9 = _mm_unpacklo_epi32(t10, t11);
    r10 = _mm_unpackhi_epi32(t8, t9);
    r11 = _mm_unpackhi_epi32(t10, t11);
    r12 = _mm_unpacklo_epi32(t12, t13);
    r13 = _mm_unpacklo_epi32(t14, t15);
    r14 = _mm_unpackhi_epi32(t12, t13);
    r15 = _mm_unpackhi_epi32(t14, t15);

    t0 = _mm_unpacklo_epi64(r0, r1);
    t1 = _mm_unpackhi_epi64(r0, r1);
    t2 = _mm_unpacklo_epi64(r2, r3);
    t3 = _mm_unpackhi_epi64(r2, r3);
    t4 = _mm_unpacklo_epi64(r4, r5);
    t5 = _mm_unpackhi_epi64(r4, r5);
    t6 = _mm_unpacklo_epi64(r6, r7);
    t7 = _mm_unpackhi_epi64(r6, r7);
    t8 = _mm_unpacklo_epi64(r8, r9);
    t9 = _mm_unpackhi_epi64(r8, r9);
    t10 = _mm_unpacklo_epi64(r10, r11);
    t11 = _mm_unpackhi_epi64(r10, r11);
    t12 = _mm_unpacklo_epi64(r12, r13);
    t13 = _mm_unpackhi_epi64(r12, r13);
    t14 = _mm_unpacklo_epi64(r14, r15);
    t15 = _mm_unpackhi_epi64(r14, r15);

    //store the transposition of the third block to the position of second block
    pDst = dst + 16;
    _mm_store_si128((__m128i *)pDst, t0);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, t1);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, t2);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, t3);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, t4);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, t5);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, t6);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, t7);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, t8);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, t9);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, t10);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, t11);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, t12);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, t13);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, t14);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, t15);
    pDst += i_dst;

    //transpose the fourth block
    t0 = _mm_unpacklo_epi8(r16, r17);
    t1 = _mm_unpacklo_epi8(r18, r19);
    t2 = _mm_unpacklo_epi8(r20, r21);
    t3 = _mm_unpacklo_epi8(r22, r23);
    t4 = _mm_unpacklo_epi8(r24, r25);
    t5 = _mm_unpacklo_epi8(r26, r27);
    t6 = _mm_unpacklo_epi8(r28, r29);
    t7 = _mm_unpacklo_epi8(r30, r31);
    t8 = _mm_unpackhi_epi8(r16, r17);
    t9 = _mm_unpackhi_epi8(r18, r19);
    t10 = _mm_unpackhi_epi8(r20, r21);
    t11 = _mm_unpackhi_epi8(r22, r23);
    t12 = _mm_unpackhi_epi8(r24, r25);
    t13 = _mm_unpackhi_epi8(r26, r27);
    t14 = _mm_unpackhi_epi8(r28, r29);
    t15 = _mm_unpackhi_epi8(r30, r31);

    r0 = _mm_unpacklo_epi16(t0, t1);
    r1 = _mm_unpacklo_epi16(t2, t3);
    r2 = _mm_unpacklo_epi16(t4, t5);
    r3 = _mm_unpacklo_epi16(t6, t7);
    r4 = _mm_unpackhi_epi16(t0, t1);
    r5 = _mm_unpackhi_epi16(t2, t3);
    r6 = _mm_unpackhi_epi16(t4, t5);
    r7 = _mm_unpackhi_epi16(t6, t7);
    r8 = _mm_unpacklo_epi16(t8, t9);
    r9 = _mm_unpacklo_epi16(t10, t11);
    r10 = _mm_unpacklo_epi16(t12, t13);
    r11 = _mm_unpacklo_epi16(t14, t15);
    r12 = _mm_unpackhi_epi16(t8, t9);
    r13 = _mm_unpackhi_epi16(t10, t11);
    r14 = _mm_unpackhi_epi16(t12, t13);
    r15 = _mm_unpackhi_epi16(t14, t15);

    t0 = _mm_unpacklo_epi32(r0, r1);
    t1 = _mm_unpacklo_epi32(r2, r3);
    t2 = _mm_unpackhi_epi32(r0, r1);
    t3 = _mm_unpackhi_epi32(r2, r3);
    t4 = _mm_unpacklo_epi32(r4, r5);
    t5 = _mm_unpacklo_epi32(r6, r7);
    t6 = _mm_unpackhi_epi32(r4, r5);
    t7 = _mm_unpackhi_epi32(r6, r7);
    t8 = _mm_unpacklo_epi32(r8, r9);
    t9 = _mm_unpacklo_epi32(r10, r11);
    t10 = _mm_unpackhi_epi32(r8, r9);
    t11 = _mm_unpackhi_epi32(r10, r11);
    t12 = _mm_unpacklo_epi32(r12, r13);
    t13 = _mm_unpacklo_epi32(r14, r15);
    t14 = _mm_unpackhi_epi32(r12, r13);
    t15 = _mm_unpackhi_epi32(r14, r15);

    r0 = _mm_unpacklo_epi64(t0, t1);
    r1 = _mm_unpackhi_epi64(t0, t1);
    r2 = _mm_unpacklo_epi64(t2, t3);
    r3 = _mm_unpackhi_epi64(t2, t3);
    r4 = _mm_unpacklo_epi64(t4, t5);
    r5 = _mm_unpackhi_epi64(t4, t5);
    r6 = _mm_unpacklo_epi64(t6, t7);
    r7 = _mm_unpackhi_epi64(t6, t7);
    r8 = _mm_unpacklo_epi64(t8, t9);
    r9 = _mm_unpackhi_epi64(t8, t9);
    r10 = _mm_unpacklo_epi64(t10, t11);
    r11 = _mm_unpackhi_epi64(t10, t11);
    r12 = _mm_unpacklo_epi64(t12, t13);
    r13 = _mm_unpackhi_epi64(t12, t13);
    r14 = _mm_unpacklo_epi64(t14, t15);
    r15 = _mm_unpackhi_epi64(t14, t15);

    //write back the fourth block
    _mm_store_si128((__m128i *)pDst, r0);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r1);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r2);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r3);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r4);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r5);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r6);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r7);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r8);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r9);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r10);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r11);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r12);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r13);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r14);
    pDst += i_dst;
    _mm_store_si128((__m128i *)pDst, r15);
}
static avs3_always_inline void transpose_32x8(pel *dst, pel *dst1, int i_dst1)
{
    __m128i r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15;
    __m128i t0, t1, t2, t3, t4, t5, t6, t7;
    r0  = _mm_loadu_si128((__m128i *)dst);
    r8  = _mm_loadu_si128((__m128i *)(dst + 16));
    r1  = _mm_loadu_si128((__m128i *)(dst + 16 * 2));
    r9  = _mm_loadu_si128((__m128i *)(dst + 16 * 3));
    r2  = _mm_loadu_si128((__m128i *)(dst + 16 * 4));
    r10 = _mm_loadu_si128((__m128i *)(dst + 16 * 5));
    r3  = _mm_loadu_si128((__m128i *)(dst + 16 * 6));
    r11 = _mm_loadu_si128((__m128i *)(dst + 16 * 7));
    r4  = _mm_loadu_si128((__m128i *)(dst + 16 * 8));
    r12 = _mm_loadu_si128((__m128i *)(dst + 16 * 9));
    r5  = _mm_loadu_si128((__m128i *)(dst + 16 * 10));
    r13 = _mm_loadu_si128((__m128i *)(dst + 16 * 11));
    r6  = _mm_loadu_si128((__m128i *)(dst + 16 * 12));
    r14 = _mm_loadu_si128((__m128i *)(dst + 16 * 13));
    r7  = _mm_loadu_si128((__m128i *)(dst + 16 * 14));
    r15 = _mm_loadu_si128((__m128i *)(dst + 16 * 15));

    t0 = _mm_unpacklo_epi8(r0, r1);
    t1 = _mm_unpacklo_epi8(r2, r3);
    t2 = _mm_unpacklo_epi8(r4, r5);
    t3 = _mm_unpacklo_epi8(r6, r7);
    t4 = _mm_unpackhi_epi8(r0, r1);
    t5 = _mm_unpackhi_epi8(r2, r3);
    t6 = _mm_unpackhi_epi8(r4, r5);
    t7 = _mm_unpackhi_epi8(r6, r7);

    r0 = _mm_unpacklo_epi16(t0, t1);
    r1 = _mm_unpacklo_epi16(t2, t3);
    r2 = _mm_unpackhi_epi16(t0, t1);
    r3 = _mm_unpackhi_epi16(t2, t3);
    r4 = _mm_unpacklo_epi16(t4, t5);
    r5 = _mm_unpacklo_epi16(t6, t7);
    r6 = _mm_unpackhi_epi16(t4, t5);
    r7 = _mm_unpackhi_epi16(t6, t7);

    t0 = _mm_unpacklo_epi32(r0, r1);
    t1 = _mm_unpackhi_epi32(r0, r1);
    t2 = _mm_unpacklo_epi32(r2, r3);
    t3 = _mm_unpackhi_epi32(r2, r3);
    t4 = _mm_unpacklo_epi32(r4, r5);
    t5 = _mm_unpackhi_epi32(r4, r5);
    t6 = _mm_unpacklo_epi32(r6, r7);
    t7 = _mm_unpackhi_epi32(r6, r7);
    //write
    _mm_storeu_si128((__m128i *)dst1, t0);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t0, 8));
    dst1 += i_dst1;
    _mm_storeu_si128((__m128i *)dst1, t1);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t1, 8));
    dst1 += i_dst1;
    _mm_storeu_si128((__m128i *)dst1, t2);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t2, 8));
    dst1 += i_dst1;
    _mm_storeu_si128((__m128i *)dst1, t3);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t3, 8));
    dst1 += i_dst1;
    _mm_storeu_si128((__m128i *)dst1, t4);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t4, 8));
    dst1 += i_dst1;
    _mm_storeu_si128((__m128i *)dst1, t5);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t5, 8));
    dst1 += i_dst1;
    _mm_storeu_si128((__m128i *)dst1, t6);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t6, 8));
    dst1 += i_dst1;
    _mm_storeu_si128((__m128i *)dst1, t7);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t7, 8));
    dst1 += i_dst1;

    t0 = _mm_unpacklo_epi8(r8, r9);
    t1 = _mm_unpacklo_epi8(r10, r11);
    t2 = _mm_unpacklo_epi8(r12, r13);
    t3 = _mm_unpacklo_epi8(r14, r15);
    t4 = _mm_unpackhi_epi8(r8, r9);
    t5 = _mm_unpackhi_epi8(r10, r11);
    t6 = _mm_unpackhi_epi8(r12, r13);
    t7 = _mm_unpackhi_epi8(r14, r15);

    r0 = _mm_unpacklo_epi16(t0, t1);
    r1 = _mm_unpacklo_epi16(t2, t3);
    r2 = _mm_unpackhi_epi16(t0, t1);
    r3 = _mm_unpackhi_epi16(t2, t3);
    r4 = _mm_unpacklo_epi16(t4, t5);
    r5 = _mm_unpacklo_epi16(t6, t7);
    r6 = _mm_unpackhi_epi16(t4, t5);
    r7 = _mm_unpackhi_epi16(t6, t7);

    t0 = _mm_unpacklo_epi32(r0, r1);
    t1 = _mm_unpackhi_epi32(r0, r1);
    t2 = _mm_unpacklo_epi32(r2, r3);
    t3 = _mm_unpackhi_epi32(r2, r3);
    t4 = _mm_unpacklo_epi32(r4, r5);
    t5 = _mm_unpackhi_epi32(r4, r5);
    t6 = _mm_unpacklo_epi32(r6, r7);
    t7 = _mm_unpackhi_epi32(r6, r7);

    _mm_storeu_si128((__m128i *)dst1, t0);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t0, 8));
    dst1 += i_dst1;
    _mm_storeu_si128((__m128i *)dst1, t1);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t1, 8));
    dst1 += i_dst1;
    _mm_storeu_si128((__m128i *)dst1, t2);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t2, 8));
    dst1 += i_dst1;
    _mm_storeu_si128((__m128i *)dst1, t3);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t3, 8));
    dst1 += i_dst1;
    _mm_storeu_si128((__m128i *)dst1, t4);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t4, 8));
    dst1 += i_dst1;
    _mm_storeu_si128((__m128i *)dst1, t5);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t5, 8));
    dst1 += i_dst1;
    _mm_storeu_si128((__m128i *)dst1, t6);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t6, 8));
    dst1 += i_dst1;
    _mm_storeu_si128((__m128i *)dst1, t7);
    dst1 += i_dst1;
    _mm_storel_epi64((__m128i *)dst1, _mm_srli_si128(t7, 8));
}

static avs3_always_inline void transpose_64x64(pel *dst, pel *dst1, int i_dst1)
{
    transpose_32x32_stride(dst, 64, dst1, i_dst1);
    transpose_32x32_stride(dst + 32, 64, dst1 + (i_dst1 << 5), i_dst1);
    transpose_32x32_stride(dst + (64 << 5), 64, dst1 + 32, i_dst1);
    transpose_32x32_stride(dst + (64 << 5) + 32, 64, dst1 + (i_dst1 << 5) + 32, i_dst1);
}

void uavs3e_ipred_ang_y_25_sse(pel *src, pel *dst, int i_dst, int mode, int iWidth, int iHeight)
{
    int i, j, idx;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;
    ALIGNED_16(pel tmp1[64 * 64 + 16]);
    pel *tmp = tmp1;
    int height = iWidth;
    int width = iHeight;
    pel *pDst = tmp;

    int i_tmp = COM_MAX(height, width);
    int y;
    pel *trans_y = src + 144;
    for (y = 0; y < 2 * iHeight + 4; y++) {
        trans_y[y] = src[-y];
    }

    src += 144;
    if (width == 4) {
        __m128i D0;
        int i_tmp2 = i_tmp << 1;

        for (j = 0; j < height; j += 2) {
            idx = (j + 1) >> 3;
            c0 = _mm_load_si128((__m128i *)tab_coeff_mode_11[j & 0x07]);

            S0 = _mm_loadl_epi64((__m128i *)(src + idx));
            S1 = _mm_srli_si128(S0, 1);
            S2 = _mm_srli_si128(S0, 2);
            S3 = _mm_srli_si128(S0, 3);

            t0 = _mm_unpacklo_epi8(S0, S1);
            t1 = _mm_unpacklo_epi8(S2, S3);
            t2 = _mm_unpacklo_epi16(t0, t1);

            t0 = _mm_maddubs_epi16(t2, c0);

            idx = (j + 2) >> 3;
            c0 = _mm_load_si128((__m128i *)tab_coeff_mode_11[(j + 1) & 0x07]);
            S0 = _mm_loadl_epi64((__m128i *)(src + idx));
            S1 = _mm_srli_si128(S0, 1);
            S2 = _mm_srli_si128(S0, 2);
            S3 = _mm_srli_si128(S0, 3);

            t1 = _mm_unpacklo_epi8(S0, S1);
            t2 = _mm_unpacklo_epi8(S2, S3);
            t1 = _mm_unpacklo_epi16(t1, t2);

            t1 = _mm_maddubs_epi16(t1, c0);

            D0 = _mm_hadds_epi16(t0, t1);
            D0 = _mm_add_epi16(D0, off);
            D0 = _mm_srli_epi16(D0, 7);
            D0 = _mm_packus_epi16(D0, zero);

            _mm_storel_epi64((__m128i *)tmp, D0);
            _mm_storel_epi64((__m128i *)(tmp + i_tmp), _mm_srli_si128(D0, 4));
            //tmp[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
            tmp += i_tmp2;
        }
        if (height == 4) {
            transpose_4x4(pDst, dst, i_dst);
        } else {
            transpose_4x16(pDst, dst, i_dst);
        }
    } else if (width == 8) {
        __m128i D0;

        for (j = 0; j < height; j++) {
            idx = (j + 1) >> 3;
            c0 = _mm_load_si128((__m128i *)tab_coeff_mode_11[j & 0x07]);

            S0 = _mm_loadu_si128((__m128i *)(src + idx));
            S1 = _mm_srli_si128(S0, 1);
            S2 = _mm_srli_si128(S0, 2);
            S3 = _mm_srli_si128(S0, 3);

            t0 = _mm_unpacklo_epi8(S0, S1);
            t1 = _mm_unpacklo_epi8(S2, S3);
            t2 = _mm_unpacklo_epi16(t0, t1);
            t3 = _mm_unpackhi_epi16(t0, t1);

            t0 = _mm_maddubs_epi16(t2, c0);
            t1 = _mm_maddubs_epi16(t3, c0);

            D0 = _mm_hadds_epi16(t0, t1);
            D0 = _mm_add_epi16(D0, off);
            D0 = _mm_srli_epi16(D0, 7);

            D0 = _mm_packus_epi16(D0, _mm_setzero_si128());

            _mm_storel_epi64((__m128i *)(tmp), D0);
            //tmp[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);

            tmp += i_tmp;
        }
        if (height == 8) {
            transpose_8x8(pDst, dst, i_dst);
        } else {
            transpose_8x32(pDst, dst, i_dst);
        }
    } else {
        for (j = 0; j < height; j++) {
            __m128i D0, D1;

            idx = (j + 1) >> 3;
            c0 = _mm_load_si128((__m128i *)tab_coeff_mode_11[j & 0x07]);

            for (i = 0; i < width; i += 16, idx += 16) {
                S0 = _mm_loadu_si128((__m128i *)(src + idx));
                S1 = _mm_loadu_si128((__m128i *)(src + idx + 1));
                S2 = _mm_loadu_si128((__m128i *)(src + idx + 2));
                S3 = _mm_loadu_si128((__m128i *)(src + idx + 3));

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);
                t3 = _mm_unpackhi_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);
                t1 = _mm_maddubs_epi16(t3, c0);

                D0 = _mm_hadds_epi16(t0, t1);
                D0 = _mm_add_epi16(D0, off);
                D0 = _mm_srli_epi16(D0, 7);

                t0 = _mm_unpackhi_epi8(S0, S1);
                t1 = _mm_unpackhi_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);
                t3 = _mm_unpackhi_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);
                t1 = _mm_maddubs_epi16(t3, c0);

                D1 = _mm_hadds_epi16(t0, t1);
                D1 = _mm_add_epi16(D1, off);
                D1 = _mm_srli_epi16(D1, 7);

                D0 = _mm_packus_epi16(D0, D1);

                _mm_storeu_si128((__m128i *)(tmp + i), D0);
                //tmp[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
            }

            tmp += i_tmp;
        }
        if (width == 16) {
            if (height == 16) {
                transpose_16x16(pDst, dst, i_dst);
            } else {
                transpose_16x4(pDst, dst, i_dst);
            }
        } else if (width == 32) {
            if (height == 32) {
                transpose_32x32_stride(pDst, i_tmp, dst, i_dst);
            } else {
                transpose_32x8(pDst, dst, i_dst);
            }
        } else {
            transpose_64x64(pDst, dst, i_dst);
        }
    }
}

void uavs3e_ipred_ang_y_27_sse(pel *src, pel *dst, int i_dst, int mode, int iWidth, int iHeight)
{
    int i, j;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;
    ALIGNED_16(pel tmp1[64 * 64 + 16]);
    pel *tmp = tmp1;
    pel *pDst = tmp;
    int i_tmp = COM_MAX(iWidth, iHeight);
    int y;
    pel *trans_y = src + 144;
    for (y = 0; y < 2 * iHeight + 4; y++) {
        trans_y[y] = src[-y];
    }

    src += 144;
    if (iWidth == iHeight) {
        if (iHeight == 4) {
            __m128i D0;
            int i_tmp2 = i_tmp << 1;

            for (j = 0; j < iWidth; j += 2) {
                int idx = tab_idx_mode_9[j];
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_9[j]);

                S0 = _mm_loadl_epi64((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);

                idx = tab_idx_mode_9[j + 1];
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_9[j + 1]);
                S0 = _mm_loadl_epi64((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t1 = _mm_unpacklo_epi8(S0, S1);
                t2 = _mm_unpacklo_epi8(S2, S3);
                t1 = _mm_unpacklo_epi16(t1, t2);

                t1 = _mm_maddubs_epi16(t1, c0);

                D0 = _mm_hadds_epi16(t0, t1);
                D0 = _mm_add_epi16(D0, off);
                D0 = _mm_srli_epi16(D0, 7);
                D0 = _mm_packus_epi16(D0, zero);

                _mm_storel_epi64((__m128i *)tmp, D0);
                _mm_storel_epi64((__m128i *)(tmp + i_tmp), _mm_srli_si128(D0, 4));
                //tmp[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
                tmp += i_tmp2;
            }
            transpose_4x4(pDst, dst, i_dst);
        } else if (iHeight == 8) {
            __m128i D0;

            for (j = 0; j < iWidth; j++) {
                int idx = tab_idx_mode_9[j];
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_9[j]);

                S0 = _mm_loadu_si128((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);
                t3 = _mm_unpackhi_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);
                t1 = _mm_maddubs_epi16(t3, c0);

                D0 = _mm_hadds_epi16(t0, t1);
                D0 = _mm_add_epi16(D0, off);
                D0 = _mm_srli_epi16(D0, 7);

                D0 = _mm_packus_epi16(D0, _mm_setzero_si128());

                _mm_storel_epi64((__m128i *)(tmp), D0);
                //tmp[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);

                tmp += i_tmp;
            }
            transpose_8x8(pDst, dst, i_dst);
        } else {
            for (j = 0; j < iWidth; j++) {
                __m128i D0, D1;

                int idx = tab_idx_mode_9[j];
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_9[j]);

                for (i = 0; i < iHeight; i += 16, idx += 16) {
                    S0 = _mm_loadu_si128((__m128i *)(src + idx));
                    S1 = _mm_loadu_si128((__m128i *)(src + idx + 1));
                    S2 = _mm_loadu_si128((__m128i *)(src + idx + 2));
                    S3 = _mm_loadu_si128((__m128i *)(src + idx + 3));

                    t0 = _mm_unpacklo_epi8(S0, S1);
                    t1 = _mm_unpacklo_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D0 = _mm_hadds_epi16(t0, t1);
                    D0 = _mm_add_epi16(D0, off);
                    D0 = _mm_srli_epi16(D0, 7);

                    t0 = _mm_unpackhi_epi8(S0, S1);
                    t1 = _mm_unpackhi_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D1 = _mm_hadds_epi16(t0, t1);
                    D1 = _mm_add_epi16(D1, off);
                    D1 = _mm_srli_epi16(D1, 7);

                    D0 = _mm_packus_epi16(D0, D1);

                    _mm_storeu_si128((__m128i *)(tmp + i), D0);
                    //tmp[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
                }

                tmp += i_tmp;
            }
            if (iHeight == 16) {
                transpose_16x16(pDst, dst, i_dst);
            } else if (iHeight == 32) {
                transpose_32x32_stride(pDst, i_tmp, dst, i_dst);
            } else {
                transpose_64x64(pDst, dst, i_dst);
            }
        }
    } else {
        if (iHeight == 4) {
            for (j = 0; j < iWidth; j++) {
                int real_width;
                int idx = tab_idx_mode_9[j];

                real_width = COM_MIN(4, 8 - idx + 1);

                if (real_width <= 0) {
                    pel val = (pel)((src[8] * tab_coeff_mode_9[j][0] + src[9] * tab_coeff_mode_9[j][1] + src[10] * tab_coeff_mode_9[j][2] + src[11] * tab_coeff_mode_9[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i *)(tmp), D0);
                    tmp += i_tmp;
                    j++;

                    for (; j < iWidth; j++) {
                        val = (pel)((src[8] * tab_coeff_mode_9[j][0] + src[9] * tab_coeff_mode_9[j][1] + src[10] * tab_coeff_mode_9[j][2] + src[11] * tab_coeff_mode_9[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        _mm_storel_epi64((__m128i *)(tmp), D0);
                        tmp += i_tmp;
                    }
                    break;
                } else {
                    __m128i D0;
                    c0 = _mm_load_si128((__m128i *)tab_coeff_mode_9[j]);

                    S0 = _mm_loadl_epi64((__m128i *)(src + idx));
                    S1 = _mm_srli_si128(S0, 1);
                    S2 = _mm_srli_si128(S0, 2);
                    S3 = _mm_srli_si128(S0, 3);

                    t0 = _mm_unpacklo_epi8(S0, S1);
                    t1 = _mm_unpacklo_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);

                    D0 = _mm_hadds_epi16(t0, zero);
                    D0 = _mm_add_epi16(D0, off);
                    D0 = _mm_srli_epi16(D0, 7);

                    D0 = _mm_packus_epi16(D0, zero);

                    _mm_storel_epi64((__m128i *)(tmp), D0);

                    if (real_width < 4) {
                        D0 = _mm_set1_epi8((char)tmp[real_width - 1]);
                        _mm_storel_epi64((__m128i *)(tmp + real_width), D0);
                    }
                }
                tmp += i_tmp;
            }
            transpose_4x16(pDst, dst, i_dst);
        } else if (iHeight == 8) {
            for (j = 0; j < iWidth; j++) {
                int real_width;
                int idx = tab_idx_mode_9[j];

                real_width = COM_MIN(8, 16 - idx + 1);

                if (real_width <= 0) {
                    pel val = (pel)((src[16] * tab_coeff_mode_9[j][0] + src[16 + 1] * tab_coeff_mode_9[j][1] + src[16 + 2] * tab_coeff_mode_9[j][2] + src[16 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i *)(tmp), D0);
                    tmp += i_tmp;
                    j++;

                    for (; j < iHeight; j++) {
                        val = (pel)((src[16] * tab_coeff_mode_9[j][0] + src[16 + 1] * tab_coeff_mode_9[j][1] + src[16 + 2] * tab_coeff_mode_9[j][2] + src[16 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        _mm_storel_epi64((__m128i *)(tmp), D0);
                        tmp += i_tmp;
                    }
                    break;
                } else {
                    __m128i D0;
                    c0 = _mm_load_si128((__m128i *)tab_coeff_mode_9[j]);

                    S0 = _mm_loadu_si128((__m128i *)(src + idx));
                    S1 = _mm_srli_si128(S0, 1);
                    S2 = _mm_srli_si128(S0, 2);
                    S3 = _mm_srli_si128(S0, 3);

                    t0 = _mm_unpacklo_epi8(S0, S1);
                    t1 = _mm_unpacklo_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D0 = _mm_hadds_epi16(t0, t1);
                    D0 = _mm_add_epi16(D0, off);
                    D0 = _mm_srli_epi16(D0, 7);

                    D0 = _mm_packus_epi16(D0, zero);

                    _mm_storel_epi64((__m128i *)(tmp), D0);
                    //tmp[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);

                    if (real_width < 8) {
                        D0 = _mm_set1_epi8((char)tmp[real_width - 1]);
                        _mm_storel_epi64((__m128i *)(tmp + real_width), D0);
                    }

                }

                tmp += i_tmp;
            }
            transpose_8x32(pDst, dst, i_dst);
        } else {
            int iHeight2 = iHeight << 1;
            for (j = 0; j < iWidth; j++) {
                int real_width;
                int idx = tab_idx_mode_9[j];

                real_width = COM_MIN(iHeight, iHeight2 - idx + 1);

                if (real_width <= 0) {
                    pel val = (pel)((src[iHeight2] * tab_coeff_mode_9[j][0] + src[iHeight2 + 1] * tab_coeff_mode_9[j][1] + src[iHeight2 + 2] * tab_coeff_mode_9[j][2] + src[iHeight2 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);

                    for (i = 0; i < iHeight; i += 16) {
                        _mm_storeu_si128((__m128i *)(tmp + i), D0);
                    }
                    tmp += i_tmp;
                    j++;

                    for (; j < iWidth; j++) {
                        val = (pel)((src[iHeight2] * tab_coeff_mode_9[j][0] + src[iHeight2 + 1] * tab_coeff_mode_9[j][1] + src[iHeight2 + 2] * tab_coeff_mode_9[j][2] + src[iHeight2 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        for (i = 0; i < iHeight; i += 16) {
                            _mm_storeu_si128((__m128i *)(tmp + i), D0);
                        }
                        tmp += i_tmp;
                    }
                    break;
                } else {
                    __m128i D0, D1;

                    c0 = _mm_load_si128((__m128i *)tab_coeff_mode_9[j]);
                    for (i = 0; i < real_width; i += 16, idx += 16) {
                        S0 = _mm_loadu_si128((__m128i *)(src + idx));
                        S1 = _mm_loadu_si128((__m128i *)(src + idx + 1));
                        S2 = _mm_loadu_si128((__m128i *)(src + idx + 2));
                        S3 = _mm_loadu_si128((__m128i *)(src + idx + 3));

                        t0 = _mm_unpacklo_epi8(S0, S1);
                        t1 = _mm_unpacklo_epi8(S2, S3);
                        t2 = _mm_unpacklo_epi16(t0, t1);
                        t3 = _mm_unpackhi_epi16(t0, t1);

                        t0 = _mm_maddubs_epi16(t2, c0);
                        t1 = _mm_maddubs_epi16(t3, c0);

                        D0 = _mm_hadds_epi16(t0, t1);
                        D0 = _mm_add_epi16(D0, off);
                        D0 = _mm_srli_epi16(D0, 7);

                        t0 = _mm_unpackhi_epi8(S0, S1);
                        t1 = _mm_unpackhi_epi8(S2, S3);
                        t2 = _mm_unpacklo_epi16(t0, t1);
                        t3 = _mm_unpackhi_epi16(t0, t1);

                        t0 = _mm_maddubs_epi16(t2, c0);
                        t1 = _mm_maddubs_epi16(t3, c0);

                        D1 = _mm_hadds_epi16(t0, t1);
                        D1 = _mm_add_epi16(D1, off);
                        D1 = _mm_srli_epi16(D1, 7);

                        D0 = _mm_packus_epi16(D0, D1);

                        _mm_storeu_si128((__m128i *)(tmp + i), D0);
                        //tmp[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
                    }

                    if (real_width < iHeight) {
                        D0 = _mm_set1_epi8((char)tmp[real_width - 1]);
                        for (i = real_width; i < iHeight; i += 16) {
                            _mm_storeu_si128((__m128i *)(tmp + i), D0);
                            //tmp[i] = tmp[real_width - 1];
                        }
                    }

                }

                tmp += i_tmp;
            }
            if (iHeight == 16) {
                transpose_16x4(pDst, dst, i_dst);
            } else if (iHeight == 32) {
                transpose_32x8(pDst, dst, i_dst);
            }
        }
    }
}

void uavs3e_ipred_ang_y_29_sse(pel *src, pel *dst, int i_dst, int mode, int iWidth, int iHeight)
{
    int i, j;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;

    ALIGNED_16(pel tmp1[64 * 64 + 16]);
    pel *tmp = tmp1;
    pel *pDst = tmp;
    int i_tmp = COM_MAX(iWidth, iHeight);
    int y;
    pel *trans_y = src + 144;
    for (y = 0; y < 2 * iHeight + 4; y++) {
        trans_y[y] = src[-y];
    }

    src += 144;
    if (iWidth == iHeight) {
        if (iWidth == 4) {
            __m128i D0;
            int i_dst2 = i_tmp << 1;

            for (j = 0; j < iHeight; j += 2) {
                int idx = tab_idx_mode_7[j];
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_7[j]);

                S0 = _mm_loadl_epi64((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);

                idx = tab_idx_mode_7[j + 1];
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_7[j + 1]);
                S0 = _mm_loadl_epi64((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t1 = _mm_unpacklo_epi8(S0, S1);
                t2 = _mm_unpacklo_epi8(S2, S3);
                t1 = _mm_unpacklo_epi16(t1, t2);

                t1 = _mm_maddubs_epi16(t1, c0);

                D0 = _mm_hadds_epi16(t0, t1);
                D0 = _mm_add_epi16(D0, off);
                D0 = _mm_srli_epi16(D0, 7);
                D0 = _mm_packus_epi16(D0, zero);

                _mm_storel_epi64((__m128i *)tmp, D0);
                _mm_storel_epi64((__m128i *)(tmp + i_tmp), _mm_srli_si128(D0, 4));
                //tmp[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
                tmp += i_dst2;
            }
            transpose_4x4(pDst, dst, i_dst);
        } else if (iWidth == 8) {
            __m128i D0;

            for (j = 0; j < iHeight; j++) {
                int idx = tab_idx_mode_7[j];
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_7[j]);

                S0 = _mm_loadu_si128((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);
                t3 = _mm_unpackhi_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);
                t1 = _mm_maddubs_epi16(t3, c0);

                D0 = _mm_hadds_epi16(t0, t1);
                D0 = _mm_add_epi16(D0, off);
                D0 = _mm_srli_epi16(D0, 7);

                D0 = _mm_packus_epi16(D0, _mm_setzero_si128());

                _mm_storel_epi64((__m128i *)(tmp), D0);
                //tmp[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);

                tmp += i_tmp;
            }
            transpose_8x8(pDst, dst, i_dst);
        } else {
            for (j = 0; j < iHeight; j++) {
                __m128i D0, D1;

                int idx = tab_idx_mode_7[j];
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_7[j]);

                for (i = 0; i < iWidth; i += 16, idx += 16) {
                    S0 = _mm_loadu_si128((__m128i *)(src + idx));
                    S1 = _mm_loadu_si128((__m128i *)(src + idx + 1));
                    S2 = _mm_loadu_si128((__m128i *)(src + idx + 2));
                    S3 = _mm_loadu_si128((__m128i *)(src + idx + 3));

                    t0 = _mm_unpacklo_epi8(S0, S1);
                    t1 = _mm_unpacklo_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D0 = _mm_hadds_epi16(t0, t1);
                    D0 = _mm_add_epi16(D0, off);
                    D0 = _mm_srli_epi16(D0, 7);

                    t0 = _mm_unpackhi_epi8(S0, S1);
                    t1 = _mm_unpackhi_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D1 = _mm_hadds_epi16(t0, t1);
                    D1 = _mm_add_epi16(D1, off);
                    D1 = _mm_srli_epi16(D1, 7);

                    D0 = _mm_packus_epi16(D0, D1);

                    _mm_storeu_si128((__m128i *)(tmp + i), D0);
                    //tmp[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
                }

                tmp += i_tmp;
            }
            if (iWidth == 16) {
                transpose_16x16(pDst, dst, i_dst);
            } else if (iWidth == 32) {
                transpose_32x32_stride(pDst, i_tmp, dst, i_dst);
            } else {
                transpose_64x64(pDst, dst, i_dst);
            }
        }
    } else {
        int height = iWidth;
        int width = iHeight;
        if (width == 4) {
            for (j = 0; j < height; j++) {
                int real_width;
                int idx = tab_idx_mode_7[j];

                real_width = COM_MIN(4, 8 - idx + 1);

                if (real_width <= 0) {
                    pel val = (pel)((src[8] * tab_coeff_mode_7[j][0] + src[8 + 1] * tab_coeff_mode_7[j][1] + src[8 + 2] * tab_coeff_mode_7[j][2] + src[8 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i *)(tmp), D0);
                    tmp += i_tmp;
                    j++;

                    for (; j < height; j++) {
                        val = (pel)((src[8] * tab_coeff_mode_7[j][0] + src[8 + 1] * tab_coeff_mode_7[j][1] + src[8 + 2] * tab_coeff_mode_7[j][2] + src[8 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        _mm_storel_epi64((__m128i *)(tmp), D0);
                        tmp += i_tmp;
                    }
                    break;
                } else {
                    __m128i D0;
                    c0 = _mm_load_si128((__m128i *)tab_coeff_mode_7[j]);

                    S0 = _mm_loadl_epi64((__m128i *)(src + idx));
                    S1 = _mm_srli_si128(S0, 1);
                    S2 = _mm_srli_si128(S0, 2);
                    S3 = _mm_srli_si128(S0, 3);

                    t0 = _mm_unpacklo_epi8(S0, S1);
                    t1 = _mm_unpacklo_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);

                    D0 = _mm_hadds_epi16(t0, zero);
                    D0 = _mm_add_epi16(D0, off);
                    D0 = _mm_srli_epi16(D0, 7);

                    D0 = _mm_packus_epi16(D0, zero);

                    _mm_storel_epi64((__m128i *)(tmp), D0);

                    if (real_width < 4) {
                        D0 = _mm_set1_epi8((char)tmp[real_width - 1]);
                        _mm_storel_epi64((__m128i *)(tmp + real_width), D0);
                    }
                }
                tmp += i_tmp;
            }
            transpose_4x16(pDst, dst, i_dst);
        } else if (width == 8) {
            for (j = 0; j < height; j++) {
                int real_width;
                int idx = tab_idx_mode_7[j];

                real_width = COM_MIN(8, 16 - idx + 1);

                if (real_width <= 0) {
                    pel val = (pel)((src[16] * tab_coeff_mode_7[j][0] + src[16 + 1] * tab_coeff_mode_7[j][1] + src[16 + 2] * tab_coeff_mode_7[j][2] + src[16 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i *)(tmp), D0);
                    tmp += i_tmp;
                    j++;

                    for (; j < height; j++) {
                        val = (pel)((src[16] * tab_coeff_mode_7[j][0] + src[16 + 1] * tab_coeff_mode_7[j][1] + src[16 + 2] * tab_coeff_mode_7[j][2] + src[16 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        _mm_storel_epi64((__m128i *)(tmp), D0);
                        tmp += i_tmp;
                    }
                    break;
                } else {
                    __m128i D0;
                    c0 = _mm_load_si128((__m128i *)tab_coeff_mode_7[j]);

                    S0 = _mm_loadu_si128((__m128i *)(src + idx));
                    S1 = _mm_srli_si128(S0, 1);
                    S2 = _mm_srli_si128(S0, 2);
                    S3 = _mm_srli_si128(S0, 3);

                    t0 = _mm_unpacklo_epi8(S0, S1);
                    t1 = _mm_unpacklo_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D0 = _mm_hadds_epi16(t0, t1);
                    D0 = _mm_add_epi16(D0, off);
                    D0 = _mm_srli_epi16(D0, 7);

                    D0 = _mm_packus_epi16(D0, zero);

                    _mm_storel_epi64((__m128i *)(tmp), D0);
                    //tmp[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);

                    if (real_width < width) {
                        D0 = _mm_set1_epi8((char)tmp[real_width - 1]);
                        _mm_storel_epi64((__m128i *)(tmp + real_width), D0);
                    }

                }

                tmp += i_tmp;
            }
            transpose_8x32(pDst, dst, i_dst);
        } else {
            for (j = 0; j < height; j++) {
                int real_width;
                int idx = tab_idx_mode_7[j];
                int width2 = width << 1;

                real_width = COM_MIN(width, width2 - idx + 1);

                if (real_width <= 0) {
                    pel val = (pel)((src[width2] * tab_coeff_mode_7[j][0] + src[width2 + 1] * tab_coeff_mode_7[j][1] + src[width2 + 2] * tab_coeff_mode_7[j][2] + src[width2 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);

                    for (i = 0; i < width; i += 16) {
                        _mm_storeu_si128((__m128i *)(tmp + i), D0);
                    }
                    tmp += i_tmp;
                    j++;

                    for (; j < height; j++) {
                        val = (pel)((src[width2] * tab_coeff_mode_7[j][0] + src[width2 + 1] * tab_coeff_mode_7[j][1] + src[width2 + 2] * tab_coeff_mode_7[j][2] + src[width2 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        for (i = 0; i < width; i += 16) {
                            _mm_storeu_si128((__m128i *)(tmp + i), D0);
                        }
                        tmp += i_tmp;
                    }
                    break;
                } else {
                    __m128i D0, D1;

                    c0 = _mm_load_si128((__m128i *)tab_coeff_mode_7[j]);
                    for (i = 0; i < real_width; i += 16, idx += 16) {
                        S0 = _mm_loadu_si128((__m128i *)(src + idx));
                        S1 = _mm_loadu_si128((__m128i *)(src + idx + 1));
                        S2 = _mm_loadu_si128((__m128i *)(src + idx + 2));
                        S3 = _mm_loadu_si128((__m128i *)(src + idx + 3));

                        t0 = _mm_unpacklo_epi8(S0, S1);
                        t1 = _mm_unpacklo_epi8(S2, S3);
                        t2 = _mm_unpacklo_epi16(t0, t1);
                        t3 = _mm_unpackhi_epi16(t0, t1);

                        t0 = _mm_maddubs_epi16(t2, c0);
                        t1 = _mm_maddubs_epi16(t3, c0);

                        D0 = _mm_hadds_epi16(t0, t1);
                        D0 = _mm_add_epi16(D0, off);
                        D0 = _mm_srli_epi16(D0, 7);

                        t0 = _mm_unpackhi_epi8(S0, S1);
                        t1 = _mm_unpackhi_epi8(S2, S3);
                        t2 = _mm_unpacklo_epi16(t0, t1);
                        t3 = _mm_unpackhi_epi16(t0, t1);

                        t0 = _mm_maddubs_epi16(t2, c0);
                        t1 = _mm_maddubs_epi16(t3, c0);

                        D1 = _mm_hadds_epi16(t0, t1);
                        D1 = _mm_add_epi16(D1, off);
                        D1 = _mm_srli_epi16(D1, 7);

                        D0 = _mm_packus_epi16(D0, D1);

                        _mm_storeu_si128((__m128i *)(tmp + i), D0);
                        //tmp[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
                    }

                    if (real_width < width) {
                        D0 = _mm_set1_epi8((char)tmp[real_width - 1]);
                        for (i = real_width; i < width; i += 16) {
                            _mm_storeu_si128((__m128i *)(tmp + i), D0);
                            //tmp[i] = tmp[real_width - 1];
                        }
                    }

                }

                tmp += i_tmp;
            }
            if (width == 16) {
                transpose_16x4(pDst, dst, i_dst);
            } else if (width == 32) {
                transpose_32x8(pDst, dst, i_dst);
            }
        }
    }
}

void uavs3e_ipred_ang_y_31_sse(pel *src, pel *dst, int i_dst, int mode, int iWidth, int iHeight)
{
    int i, j, k;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;
    int width = iHeight;
    int height = iWidth;
    ALIGNED_16(pel tmp1[64 * 64 + 16]);
    pel *tmp = tmp1;
    pel *pDst = tmp;
    int i_tmp = COM_MAX(iWidth, iHeight);
    int y;
    pel *trans_y = src + 144;
    for (y = 0; y < 2 * iHeight + 4; y++) {
        trans_y[y] = src[-y];
    }

    src += 144;
    if (width == 4) {
        for (j = 0; j < height; j++) {
            int real_width;
            int idx = tab_idx_mode_5[j];

            k = j & 0x07;
            real_width = COM_MIN(4, 8 - idx + 1);

            if (real_width <= 0) {
                pel val = (pel)((src[8] * tab_coeff_mode_5[k][0] + src[8 + 1] * tab_coeff_mode_5[k][1] + src[8 + 2] * tab_coeff_mode_5[k][2] + src[8 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);
                _mm_storel_epi64((__m128i *)(tmp), D0);
                tmp += i_tmp;
                j++;

                for (; j < height; j++) {
                    k = j & 0x07;
                    val = (pel)((src[8] * tab_coeff_mode_5[k][0] + src[8 + 1] * tab_coeff_mode_5[k][1] + src[8 + 2] * tab_coeff_mode_5[k][2] + src[8 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i *)(tmp), D0);
                    tmp += i_tmp;
                }
                break;
            } else {
                __m128i D0;
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_5[k]);

                S0 = _mm_loadl_epi64((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);

                D0 = _mm_hadds_epi16(t0, zero);
                D0 = _mm_add_epi16(D0, off);
                D0 = _mm_srli_epi16(D0, 7);

                D0 = _mm_packus_epi16(D0, zero);

                _mm_storel_epi64((__m128i *)(tmp), D0);

                if (real_width < width) {
                    D0 = _mm_set1_epi8((char)tmp[real_width - 1]);
                    _mm_storel_epi64((__m128i *)(tmp + real_width), D0);
                }
            }
            tmp += i_tmp;
        }
        if (height == 4) {
            transpose_4x4(pDst, dst, i_dst);
        } else {
            transpose_4x16(pDst, dst, i_dst);
        }
    } else if (width == 8) {
        for (j = 0; j < height; j++) {
            int real_width;
            int idx = tab_idx_mode_5[j];

            k = j & 0x07;
            real_width = COM_MIN(8, 16 - idx + 1);

            if (real_width <= 0) {
                pel val = (pel)((src[16] * tab_coeff_mode_5[k][0] + src[16 + 1] * tab_coeff_mode_5[k][1] + src[16 + 2] * tab_coeff_mode_5[k][2] + src[16 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);
                _mm_storel_epi64((__m128i *)(tmp), D0);
                tmp += i_tmp;
                j++;

                for (; j < height; j++) {
                    k = j & 0x07;
                    val = (pel)((src[16] * tab_coeff_mode_5[k][0] + src[16 + 1] * tab_coeff_mode_5[k][1]  + src[16 + 2] * tab_coeff_mode_5[k][2] + src[16 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i *)(tmp), D0);
                    tmp += i_tmp;
                }
                break;
            } else {
                __m128i D0;
                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_5[k]);

                S0 = _mm_loadu_si128((__m128i *)(src + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);
                t3 = _mm_unpackhi_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);
                t1 = _mm_maddubs_epi16(t3, c0);

                D0 = _mm_hadds_epi16(t0, t1);
                D0 = _mm_add_epi16(D0, off);
                D0 = _mm_srli_epi16(D0, 7);

                D0 = _mm_packus_epi16(D0, zero);

                _mm_storel_epi64((__m128i *)(tmp), D0);
                //tmp[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);

                if (real_width < width) {
                    D0 = _mm_set1_epi8((char)tmp[real_width - 1]);
                    _mm_storel_epi64((__m128i *)(tmp + real_width), D0);
                }

            }

            tmp += i_tmp;
        }
        if (height == 8) {
            transpose_8x8(pDst, dst, i_dst);
        } else {
            transpose_8x32(pDst, dst, i_dst);
        }
    } else {
        int width2 = width << 1;
        for (j = 0; j < height; j++) {
            int real_width;
            int idx = tab_idx_mode_5[j];
            k = j & 0x07;

            real_width = COM_MIN(width, width2 - idx + 1);

            if (real_width <= 0) {
                pel val = (pel)((src[width2] * tab_coeff_mode_5[k][0] + src[width2 + 1] * tab_coeff_mode_5[k][1] + src[width2 + 2] * tab_coeff_mode_5[k][2] + src[width2 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);

                for (i = 0; i < width; i += 16) {
                    _mm_storeu_si128((__m128i *)(tmp + i), D0);
                }
                tmp += i_tmp;
                j++;

                for (; j < height; j++) {
                    k = j & 0x07;
                    val = (pel)((src[width2] * tab_coeff_mode_5[k][0] + src[width2 + 1] * tab_coeff_mode_5[k][1] + src[width2 + 2] * tab_coeff_mode_5[k][2] + src[width2 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    for (i = 0; i < width; i += 16) {
                        _mm_storeu_si128((__m128i *)(tmp + i), D0);
                    }
                    tmp += i_tmp;
                }
                break;
            } else {
                __m128i D0, D1;

                c0 = _mm_load_si128((__m128i *)tab_coeff_mode_5[k]);
                for (i = 0; i < real_width; i += 16, idx += 16) {
                    S0 = _mm_loadu_si128((__m128i *)(src + idx));
                    S1 = _mm_loadu_si128((__m128i *)(src + idx + 1));
                    S2 = _mm_loadu_si128((__m128i *)(src + idx + 2));
                    S3 = _mm_loadu_si128((__m128i *)(src + idx + 3));

                    t0 = _mm_unpacklo_epi8(S0, S1);
                    t1 = _mm_unpacklo_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D0 = _mm_hadds_epi16(t0, t1);
                    D0 = _mm_add_epi16(D0, off);
                    D0 = _mm_srli_epi16(D0, 7);

                    t0 = _mm_unpackhi_epi8(S0, S1);
                    t1 = _mm_unpackhi_epi8(S2, S3);
                    t2 = _mm_unpacklo_epi16(t0, t1);
                    t3 = _mm_unpackhi_epi16(t0, t1);

                    t0 = _mm_maddubs_epi16(t2, c0);
                    t1 = _mm_maddubs_epi16(t3, c0);

                    D1 = _mm_hadds_epi16(t0, t1);
                    D1 = _mm_add_epi16(D1, off);
                    D1 = _mm_srli_epi16(D1, 7);

                    D0 = _mm_packus_epi16(D0, D1);

                    _mm_storeu_si128((__m128i *)(tmp + i), D0);
                    //tmp[i] = (pel)((src[idx] * c1 + src[idx + 1] * c2 + src[idx + 2] * c3 + src[idx + 3] * c4 + 64) >> 7);
                }

                if (real_width < width) {
                    D0 = _mm_set1_epi8((char)tmp[real_width - 1]);
                    for (i = real_width; i < width; i += 16) {
                        _mm_storeu_si128((__m128i *)(tmp + i), D0);
                        //tmp[i] = tmp[real_width - 1];
                    }
                }

            }

            tmp += i_tmp;
        }
        if (width == 16) {
            if (height == 16) {
                transpose_16x16(pDst, dst, i_dst);
            } else {
                transpose_16x4(pDst, dst, i_dst);
            }
        } else if (width == 32) {
            if (height == 32) {
                transpose_32x32_stride(pDst, i_tmp, dst, i_dst);
            } else {
                transpose_32x8(pDst, dst, i_dst);
            }
        } else {
            transpose_64x64(pDst, dst, i_dst);
        }
    }

}

void uavs3e_ipred_ang_y_26_sse(pel *src, pel *dst, int i_dst, int mode, int iWidth, int iHeight)
{
    int i;

    if (iWidth != 4) {
        __m128i zero = _mm_setzero_si128();
        __m128i coeff2 = _mm_set1_epi16(2);
        __m128i coeff3 = _mm_set1_epi16(3);
        __m128i coeff4 = _mm_set1_epi16(4);
        __m128i coeff5 = _mm_set1_epi16(5);
        __m128i coeff7 = _mm_set1_epi16(7);
        __m128i coeff8 = _mm_set1_epi16(8);
        __m128i shuffle = _mm_setr_epi8(7, 15, 6, 14, 5, 13, 4, 12, 3, 11, 2, 10, 1, 9, 0, 8);

        ALIGNED_16(pel first_line[64 + 256]);
        int line_size = iWidth + (iHeight - 1) * 4;
        int iHeight4 = iHeight << 2;

        src -= 15;

        for (i = 0; i < line_size - 32; i += 64, src -= 16) {
            __m128i p00, p10, p20, p30;
            __m128i p01, p11, p21, p31;
            __m128i M1, M2, M3, M4, M5, M6, M7, M8;
            __m128i S0 = _mm_loadu_si128((__m128i *)(src));
            __m128i S3 = _mm_loadu_si128((__m128i *)(src - 3));
            __m128i S1 = _mm_loadu_si128((__m128i *)(src - 1));
            __m128i S2 = _mm_loadu_si128((__m128i *)(src - 2));

            __m128i L0 = _mm_unpacklo_epi8(S0, zero);
            __m128i L1 = _mm_unpacklo_epi8(S1, zero);
            __m128i L2 = _mm_unpacklo_epi8(S2, zero);
            __m128i L3 = _mm_unpacklo_epi8(S3, zero);

            __m128i H0 = _mm_unpackhi_epi8(S0, zero);
            __m128i H1 = _mm_unpackhi_epi8(S1, zero);
            __m128i H2 = _mm_unpackhi_epi8(S2, zero);
            __m128i H3 = _mm_unpackhi_epi8(S3, zero);

            p00 = _mm_mullo_epi16(L0, coeff3);
            p10 = _mm_mullo_epi16(L1, coeff7);
            p20 = _mm_mullo_epi16(L2, coeff5);
            p30 = _mm_add_epi16(L3, coeff8);
            p00 = _mm_add_epi16(p00, p30);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, p20);
            M1 = _mm_srli_epi16(p00, 4);

            p01 = _mm_mullo_epi16(H0, coeff3);
            p11 = _mm_mullo_epi16(H1, coeff7);
            p21 = _mm_mullo_epi16(H2, coeff5);
            p31 = _mm_add_epi16(H3, coeff8);
            p01 = _mm_add_epi16(p01, p31);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, p21);
            M2 = _mm_srli_epi16(p01, 4);


            p00 = _mm_add_epi16(L1, L2);
            p00 = _mm_mullo_epi16(p00, coeff3);
            p10 = _mm_add_epi16(L0, L3);
            p10 = _mm_add_epi16(p10, coeff4);
            p00 = _mm_add_epi16(p10, p00);
            M3 = _mm_srli_epi16(p00, 3);

            p01 = _mm_add_epi16(H1, H2);
            p01 = _mm_mullo_epi16(p01, coeff3);
            p11 = _mm_add_epi16(H0, H3);
            p11 = _mm_add_epi16(p11, coeff4);
            p01 = _mm_add_epi16(p11, p01);
            M4 = _mm_srli_epi16(p01, 3);


            p10 = _mm_mullo_epi16(L1, coeff5);
            p20 = _mm_mullo_epi16(L2, coeff7);
            p30 = _mm_mullo_epi16(L3, coeff3);
            p00 = _mm_add_epi16(L0, coeff8);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, p20);
            p00 = _mm_add_epi16(p00, p30);
            M5 = _mm_srli_epi16(p00, 4);

            p11 = _mm_mullo_epi16(H1, coeff5);
            p21 = _mm_mullo_epi16(H2, coeff7);
            p31 = _mm_mullo_epi16(H3, coeff3);
            p01 = _mm_add_epi16(H0, coeff8);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, p21);
            p01 = _mm_add_epi16(p01, p31);
            M6 = _mm_srli_epi16(p01, 4);


            p00 = _mm_add_epi16(L1, L2);
            p10 = _mm_add_epi16(L2, L3);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, coeff2);
            M7 = _mm_srli_epi16(p00, 2);

            p01 = _mm_add_epi16(H1, H2);
            p11 = _mm_add_epi16(H2, H3);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, coeff2);
            M8 = _mm_srli_epi16(p01, 2);

            M1 = _mm_packus_epi16(M1, M3);
            M5 = _mm_packus_epi16(M5, M7);
            M1 = _mm_shuffle_epi8(M1, shuffle);
            M5 = _mm_shuffle_epi8(M5, shuffle);

            M2 = _mm_packus_epi16(M2, M4);
            M6 = _mm_packus_epi16(M6, M8);
            M2 = _mm_shuffle_epi8(M2, shuffle);
            M6 = _mm_shuffle_epi8(M6, shuffle);

            M3 = _mm_unpacklo_epi16(M1, M5);
            M7 = _mm_unpackhi_epi16(M1, M5);
            M4 = _mm_unpacklo_epi16(M2, M6);
            M8 = _mm_unpackhi_epi16(M2, M6);

            _mm_store_si128((__m128i *)&first_line[i], M4);
            _mm_store_si128((__m128i *)&first_line[16 + i], M8);
            _mm_store_si128((__m128i *)&first_line[32 + i], M3);
            _mm_store_si128((__m128i *)&first_line[48 + i], M7);
        }

        if (i < line_size) {
            __m128i p01, p11, p21, p31;
            __m128i M2, M4, M6, M8;
            __m128i S0 = _mm_loadu_si128((__m128i *)(src));
            __m128i S3 = _mm_loadu_si128((__m128i *)(src - 3));
            __m128i S1 = _mm_loadu_si128((__m128i *)(src - 1));
            __m128i S2 = _mm_loadu_si128((__m128i *)(src - 2));

            __m128i H0 = _mm_unpackhi_epi8(S0, zero);
            __m128i H1 = _mm_unpackhi_epi8(S1, zero);
            __m128i H2 = _mm_unpackhi_epi8(S2, zero);
            __m128i H3 = _mm_unpackhi_epi8(S3, zero);

            p01 = _mm_mullo_epi16(H0, coeff3);
            p11 = _mm_mullo_epi16(H1, coeff7);
            p21 = _mm_mullo_epi16(H2, coeff5);
            p31 = _mm_add_epi16(H3, coeff8);
            p01 = _mm_add_epi16(p01, p31);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, p21);
            M2 = _mm_srli_epi16(p01, 4);

            p01 = _mm_add_epi16(H1, H2);
            p01 = _mm_mullo_epi16(p01, coeff3);
            p11 = _mm_add_epi16(H0, H3);
            p11 = _mm_add_epi16(p11, coeff4);
            p01 = _mm_add_epi16(p11, p01);
            M4 = _mm_srli_epi16(p01, 3);

            p11 = _mm_mullo_epi16(H1, coeff5);
            p21 = _mm_mullo_epi16(H2, coeff7);
            p31 = _mm_mullo_epi16(H3, coeff3);
            p01 = _mm_add_epi16(H0, coeff8);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, p21);
            p01 = _mm_add_epi16(p01, p31);
            M6 = _mm_srli_epi16(p01, 4);

            p01 = _mm_add_epi16(H1, H2);
            p11 = _mm_add_epi16(H2, H3);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, coeff2);
            M8 = _mm_srli_epi16(p01, 2);

            M2 = _mm_packus_epi16(M2, M4);
            M6 = _mm_packus_epi16(M6, M8);
            M2 = _mm_shuffle_epi8(M2, shuffle);
            M6 = _mm_shuffle_epi8(M6, shuffle);

            M4 = _mm_unpacklo_epi16(M2, M6);
            M8 = _mm_unpackhi_epi16(M2, M6);

            _mm_store_si128((__m128i *)&first_line[i], M4);
            _mm_store_si128((__m128i *)&first_line[16 + i], M8);
        }

        switch (iWidth) {
        case 4:
            for (i = 0; i < iHeight4; i += 4) {
                CP32(dst, first_line + i);
                dst += i_dst;
            }
            break;
        case 8:
            for (i = 0; i < iHeight4; i += 4) {
                CP64(dst, first_line + i);
                dst += i_dst;
            }
            break;
        case 16:
            for (i = 0; i < iHeight4; i += 4) {
                CP128(dst, first_line + i);
                dst += i_dst;
            }
            break;
        default:
            for (i = 0; i < iHeight4; i += 4) {
                memcpy(dst, first_line + i, iWidth * sizeof(pel));
                dst += i_dst;
            }
            break;
        }
    } else {
        __m128i zero = _mm_setzero_si128();
        __m128i coeff2 = _mm_set1_epi16(2);
        __m128i coeff3 = _mm_set1_epi16(3);
        __m128i coeff4 = _mm_set1_epi16(4);
        __m128i coeff5 = _mm_set1_epi16(5);
        __m128i coeff7 = _mm_set1_epi16(7);
        __m128i coeff8 = _mm_set1_epi16(8);
        __m128i shuffle = _mm_setr_epi8(7, 15, 6, 14, 5, 13, 4, 12, 3, 11, 2, 10, 1, 9, 0, 8);
        src -= 15;

        if (iHeight == 4) {
            __m128i p01, p11, p21, p31;
            __m128i M2, M4, M6, M8;
            __m128i S0 = _mm_loadu_si128((__m128i *)(src));
            __m128i S3 = _mm_loadu_si128((__m128i *)(src - 3));
            __m128i S1 = _mm_loadu_si128((__m128i *)(src - 1));
            __m128i S2 = _mm_loadu_si128((__m128i *)(src - 2));

            __m128i H0 = _mm_unpackhi_epi8(S0, zero);
            __m128i H1 = _mm_unpackhi_epi8(S1, zero);
            __m128i H2 = _mm_unpackhi_epi8(S2, zero);
            __m128i H3 = _mm_unpackhi_epi8(S3, zero);

            p01 = _mm_mullo_epi16(H0, coeff3);
            p11 = _mm_mullo_epi16(H1, coeff7);
            p21 = _mm_mullo_epi16(H2, coeff5);
            p31 = _mm_add_epi16(H3, coeff8);
            p01 = _mm_add_epi16(p01, p31);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, p21);
            M2 = _mm_srli_epi16(p01, 4);

            p01 = _mm_add_epi16(H1, H2);
            p01 = _mm_mullo_epi16(p01, coeff3);
            p11 = _mm_add_epi16(H0, H3);
            p11 = _mm_add_epi16(p11, coeff4);
            p01 = _mm_add_epi16(p11, p01);
            M4 = _mm_srli_epi16(p01, 3);

            p11 = _mm_mullo_epi16(H1, coeff5);
            p21 = _mm_mullo_epi16(H2, coeff7);
            p31 = _mm_mullo_epi16(H3, coeff3);
            p01 = _mm_add_epi16(H0, coeff8);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, p21);
            p01 = _mm_add_epi16(p01, p31);
            M6 = _mm_srli_epi16(p01, 4);

            p01 = _mm_add_epi16(H1, H2);
            p11 = _mm_add_epi16(H2, H3);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, coeff2);
            M8 = _mm_srli_epi16(p01, 2);

            M2 = _mm_packus_epi16(M2, M4);
            M6 = _mm_packus_epi16(M6, M8);
            M2 = _mm_shuffle_epi8(M2, shuffle);
            M6 = _mm_shuffle_epi8(M6, shuffle);

            M4 = _mm_unpacklo_epi16(M2, M6);

            ((int *)dst)[0] = _mm_cvtsi128_si32(M4);
            dst += i_dst;
            M4 = _mm_srli_si128(M4, 4);
            ((int *)dst)[0] = _mm_cvtsi128_si32(M4);
            dst += i_dst;
            M4 = _mm_srli_si128(M4, 4);
            ((int *)dst)[0] = _mm_cvtsi128_si32(M4);
            dst += i_dst;
            M4 = _mm_srli_si128(M4, 4);
            ((int *)dst)[0] = _mm_cvtsi128_si32(M4);
        } else {
            __m128i p00, p10, p20, p30;
            __m128i p01, p11, p21, p31;
            __m128i M1, M2, M3, M4, M5, M6, M7, M8;
            __m128i S0 = _mm_loadu_si128((__m128i *)(src));
            __m128i S3 = _mm_loadu_si128((__m128i *)(src - 3));
            __m128i S1 = _mm_loadu_si128((__m128i *)(src - 1));
            __m128i S2 = _mm_loadu_si128((__m128i *)(src - 2));

            __m128i L0 = _mm_unpacklo_epi8(S0, zero);
            __m128i L1 = _mm_unpacklo_epi8(S1, zero);
            __m128i L2 = _mm_unpacklo_epi8(S2, zero);
            __m128i L3 = _mm_unpacklo_epi8(S3, zero);

            __m128i H0 = _mm_unpackhi_epi8(S0, zero);
            __m128i H1 = _mm_unpackhi_epi8(S1, zero);
            __m128i H2 = _mm_unpackhi_epi8(S2, zero);
            __m128i H3 = _mm_unpackhi_epi8(S3, zero);

            p00 = _mm_mullo_epi16(L0, coeff3);
            p10 = _mm_mullo_epi16(L1, coeff7);
            p20 = _mm_mullo_epi16(L2, coeff5);
            p30 = _mm_add_epi16(L3, coeff8);
            p00 = _mm_add_epi16(p00, p30);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, p20);
            M1 = _mm_srli_epi16(p00, 4);

            p01 = _mm_mullo_epi16(H0, coeff3);
            p11 = _mm_mullo_epi16(H1, coeff7);
            p21 = _mm_mullo_epi16(H2, coeff5);
            p31 = _mm_add_epi16(H3, coeff8);
            p01 = _mm_add_epi16(p01, p31);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, p21);
            M2 = _mm_srli_epi16(p01, 4);


            p00 = _mm_add_epi16(L1, L2);
            p00 = _mm_mullo_epi16(p00, coeff3);
            p10 = _mm_add_epi16(L0, L3);
            p10 = _mm_add_epi16(p10, coeff4);
            p00 = _mm_add_epi16(p10, p00);
            M3 = _mm_srli_epi16(p00, 3);

            p01 = _mm_add_epi16(H1, H2);
            p01 = _mm_mullo_epi16(p01, coeff3);
            p11 = _mm_add_epi16(H0, H3);
            p11 = _mm_add_epi16(p11, coeff4);
            p01 = _mm_add_epi16(p11, p01);
            M4 = _mm_srli_epi16(p01, 3);


            p10 = _mm_mullo_epi16(L1, coeff5);
            p20 = _mm_mullo_epi16(L2, coeff7);
            p30 = _mm_mullo_epi16(L3, coeff3);
            p00 = _mm_add_epi16(L0, coeff8);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, p20);
            p00 = _mm_add_epi16(p00, p30);
            M5 = _mm_srli_epi16(p00, 4);

            p11 = _mm_mullo_epi16(H1, coeff5);
            p21 = _mm_mullo_epi16(H2, coeff7);
            p31 = _mm_mullo_epi16(H3, coeff3);
            p01 = _mm_add_epi16(H0, coeff8);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, p21);
            p01 = _mm_add_epi16(p01, p31);
            M6 = _mm_srli_epi16(p01, 4);


            p00 = _mm_add_epi16(L1, L2);
            p10 = _mm_add_epi16(L2, L3);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, coeff2);
            M7 = _mm_srli_epi16(p00, 2);

            p01 = _mm_add_epi16(H1, H2);
            p11 = _mm_add_epi16(H2, H3);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, coeff2);
            M8 = _mm_srli_epi16(p01, 2);

            M1 = _mm_packus_epi16(M1, M3);
            M5 = _mm_packus_epi16(M5, M7);
            M1 = _mm_shuffle_epi8(M1, shuffle);
            M5 = _mm_shuffle_epi8(M5, shuffle);

            M2 = _mm_packus_epi16(M2, M4);
            M6 = _mm_packus_epi16(M6, M8);
            M2 = _mm_shuffle_epi8(M2, shuffle);
            M6 = _mm_shuffle_epi8(M6, shuffle);

            M3 = _mm_unpacklo_epi16(M1, M5);
            M7 = _mm_unpackhi_epi16(M1, M5);
            M4 = _mm_unpacklo_epi16(M2, M6);
            M8 = _mm_unpackhi_epi16(M2, M6);

            ((int *)dst)[0] = _mm_cvtsi128_si32(M4);
            dst += i_dst;
            M4 = _mm_srli_si128(M4, 4);
            ((int *)dst)[0] = _mm_cvtsi128_si32(M4);
            dst += i_dst;
            M4 = _mm_srli_si128(M4, 4);
            ((int *)dst)[0] = _mm_cvtsi128_si32(M4);
            dst += i_dst;
            M4 = _mm_srli_si128(M4, 4);
            ((int *)dst)[0] = _mm_cvtsi128_si32(M4);
            dst += i_dst;
            ((int *)dst)[0] = _mm_cvtsi128_si32(M8);
            dst += i_dst;
            M8 = _mm_srli_si128(M8, 4);
            ((int *)dst)[0] = _mm_cvtsi128_si32(M8);
            dst += i_dst;
            M8 = _mm_srli_si128(M8, 4);
            ((int *)dst)[0] = _mm_cvtsi128_si32(M8);
            dst += i_dst;
            M8 = _mm_srli_si128(M8, 4);
            ((int *)dst)[0] = _mm_cvtsi128_si32(M8);
            dst += i_dst;
            ((int *)dst)[0] = _mm_cvtsi128_si32(M3);
            dst += i_dst;
            M3 = _mm_srli_si128(M3, 4);
            ((int *)dst)[0] = _mm_cvtsi128_si32(M3);
            dst += i_dst;
            M3 = _mm_srli_si128(M3, 4);
            ((int *)dst)[0] = _mm_cvtsi128_si32(M3);
            dst += i_dst;
            M3 = _mm_srli_si128(M3, 4);
            ((int *)dst)[0] = _mm_cvtsi128_si32(M3);
            dst += i_dst;
            ((int *)dst)[0] = _mm_cvtsi128_si32(M7);
            dst += i_dst;
            M7 = _mm_srli_si128(M7, 4);
            ((int *)dst)[0] = _mm_cvtsi128_si32(M7);
            dst += i_dst;
            M7 = _mm_srli_si128(M7, 4);
            ((int *)dst)[0] = _mm_cvtsi128_si32(M7);
            dst += i_dst;
            M7 = _mm_srli_si128(M7, 4);
            ((int *)dst)[0] = _mm_cvtsi128_si32(M7);
        }
    }
}

void uavs3e_ipred_ang_y_28_sse(pel *src, pel *dst, int i_dst, int mode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[64 + 128]);
    int line_size = iWidth + (iHeight - 1) * 2;
    int real_size = COM_MIN(line_size, iHeight * 4 + 1);
    int i;
    int iHeight2 = iHeight << 1;
    __m128i pad;
    __m128i coeff2 = _mm_set1_epi16(2);
    __m128i coeff3 = _mm_set1_epi16(3);
    __m128i coeff4 = _mm_set1_epi16(4);
    __m128i shuffle = _mm_setr_epi8(7, 15, 6, 14, 5, 13, 4, 12, 3, 11, 2, 10, 1, 9, 0, 8);
    __m128i zero = _mm_setzero_si128();

    src -= 15;

    for (i = 0; i < real_size - 16; i += 32, src -= 16) {
        __m128i p00, p10, p01, p11;
        __m128i S0 = _mm_loadu_si128((__m128i *)(src));
        __m128i S3 = _mm_loadu_si128((__m128i *)(src - 3));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src - 2));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);
        __m128i L3 = _mm_unpacklo_epi8(S3, zero);

        __m128i H0 = _mm_unpackhi_epi8(S0, zero);
        __m128i H1 = _mm_unpackhi_epi8(S1, zero);
        __m128i H2 = _mm_unpackhi_epi8(S2, zero);
        __m128i H3 = _mm_unpackhi_epi8(S3, zero);

        p00 = _mm_adds_epi16(L1, L2);
        p01 = _mm_add_epi16(L1, L2);
        p00 = _mm_mullo_epi16(p00, coeff3);
        p10 = _mm_adds_epi16(L0, L3);
        p11 = _mm_add_epi16(L2, L3);
        p10 = _mm_adds_epi16(p10, coeff4);
        p00 = _mm_adds_epi16(p00, p10);
        p01 = _mm_add_epi16(p01, p11);
        p01 = _mm_add_epi16(p01, coeff2);

        p00 = _mm_srli_epi16(p00, 3);
        p01 = _mm_srli_epi16(p01, 2);

        p00 = _mm_packus_epi16(p00, p01);
        p00 = _mm_shuffle_epi8(p00, shuffle);

        _mm_store_si128((__m128i *)&first_line[i + 16], p00);

        p00 = _mm_adds_epi16(H1, H2);
        p01 = _mm_add_epi16(H1, H2);
        p00 = _mm_mullo_epi16(p00, coeff3);
        p10 = _mm_adds_epi16(H0, H3);
        p11 = _mm_add_epi16(H2, H3);
        p10 = _mm_adds_epi16(p10, coeff4);
        p00 = _mm_adds_epi16(p00, p10);
        p01 = _mm_add_epi16(p01, p11);
        p01 = _mm_add_epi16(p01, coeff2);

        p00 = _mm_srli_epi16(p00, 3);
        p01 = _mm_srli_epi16(p01, 2);

        p00 = _mm_packus_epi16(p00, p01);
        p00 = _mm_shuffle_epi8(p00, shuffle);

        _mm_store_si128((__m128i *)&first_line[i], p00);
    }

    if (i < real_size) {
        __m128i p00, p10, p01, p11;
        __m128i S0 = _mm_loadu_si128((__m128i *)(src));
        __m128i S3 = _mm_loadu_si128((__m128i *)(src - 3));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src - 2));

        __m128i H0 = _mm_unpackhi_epi8(S0, zero);
        __m128i H1 = _mm_unpackhi_epi8(S1, zero);
        __m128i H2 = _mm_unpackhi_epi8(S2, zero);
        __m128i H3 = _mm_unpackhi_epi8(S3, zero);

        p00 = _mm_adds_epi16(H1, H2);
        p01 = _mm_add_epi16(H1, H2);
        p00 = _mm_mullo_epi16(p00, coeff3);
        p10 = _mm_adds_epi16(H0, H3);
        p11 = _mm_add_epi16(H2, H3);
        p10 = _mm_adds_epi16(p10, coeff4);
        p00 = _mm_adds_epi16(p00, p10);
        p01 = _mm_add_epi16(p01, p11);
        p01 = _mm_add_epi16(p01, coeff2);

        p00 = _mm_srli_epi16(p00, 3);
        p01 = _mm_srli_epi16(p01, 2);

        p00 = _mm_packus_epi16(p00, p01);
        p00 = _mm_shuffle_epi8(p00, shuffle);

        _mm_store_si128((__m128i *)&first_line[i], p00);
    }

    // padding
    if (real_size < line_size) {
        i = real_size + 1;
        first_line[i - 1] = first_line[i - 3];

        pad = _mm_set1_epi16(((short *)&first_line[i - 2])[0]);

        for (; i < line_size; i += 16) {
            _mm_storeu_si128((__m128i *)&first_line[i], pad);
        }
    }

#define COPY_Y28(w) { \
    for (i = 0; i < iHeight2; i += 2) { \
        memcpy(dst, first_line + i, w * sizeof(pel)); \
        dst += i_dst;\
    } \
}
    switch (iWidth) {
    case 4:
        COPY_Y28(4)
        break;
    case 8:
        COPY_Y28(8)
            break;
    case 12:
        COPY_Y28(12)
            break;
    case 16:
        COPY_Y28(16)
            break;
    case 24:
        COPY_Y28(24)
            break;
    case 32:
        COPY_Y28(32)
            break;
    case 48:
        COPY_Y28(48)
            break;
    case 64:
        COPY_Y28(64)
            break;
    }
}

void uavs3e_ipred_ang_y_30_sse(pel *src, pel *dst, int i_dst, int mode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[64 + 64]);
    int line_size = iWidth + iHeight - 1;
    int real_size = COM_MIN(line_size, iHeight * 2);
    int i;
    __m128i coeff2 = _mm_set1_epi16(2);
    __m128i shuffle = _mm_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
    __m128i zero = _mm_setzero_si128();

    src -= 17;

    for (i = 0; i < real_size - 8; i += 16, src -= 16) {
        __m128i p00, p10, p01, p11;
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);

        __m128i H0 = _mm_unpackhi_epi8(S0, zero);
        __m128i H1 = _mm_unpackhi_epi8(S1, zero);
        __m128i H2 = _mm_unpackhi_epi8(S2, zero);

        p00 = _mm_add_epi16(L0, L1);
        p10 = _mm_add_epi16(L1, L2);
        p01 = _mm_add_epi16(H0, H1);
        p11 = _mm_add_epi16(H1, H2);

        p00 = _mm_add_epi16(p00, p10);
        p01 = _mm_add_epi16(p01, p11);
        p00 = _mm_add_epi16(p00, coeff2);
        p01 = _mm_add_epi16(p01, coeff2);

        p00 = _mm_srli_epi16(p00, 2);
        p01 = _mm_srli_epi16(p01, 2);

        p00 = _mm_packus_epi16(p00, p01);
        p00 = _mm_shuffle_epi8(p00, shuffle);

        _mm_store_si128((__m128i *)&first_line[i], p00);
    }

    if (i < real_size) {
        __m128i p01, p11;
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i H0 = _mm_unpackhi_epi8(S0, zero);
        __m128i H1 = _mm_unpackhi_epi8(S1, zero);
        __m128i H2 = _mm_unpackhi_epi8(S2, zero);

        p01 = _mm_add_epi16(H0, H1);
        p11 = _mm_add_epi16(H1, H2);

        p01 = _mm_add_epi16(p01, p11);
        p01 = _mm_add_epi16(p01, coeff2);

        p01 = _mm_srli_epi16(p01, 2);

        p01 = _mm_packus_epi16(p01, p01);
        p01 = _mm_shuffle_epi8(p01, shuffle);

        _mm_store_si128((__m128i *)&first_line[i], p01);
    }
    // padding
    for (i = real_size; i < line_size; i += 16) {
        __m128i pad = _mm_set1_epi8((char)first_line[real_size - 1]);
        _mm_storeu_si128((__m128i *)&first_line[i], pad);
    }

    if (iWidth > 16) {
        for (i = 0; i < iHeight; i++) {
            memcpy(dst, first_line + i, iWidth * sizeof(pel));
            dst += i_dst;
        }
    } else if (iWidth == 16) {
        pel *dst1 = dst;
        pel *dst2;
        if (iHeight == 4) {
            __m128i M = _mm_load_si128((__m128i *)&first_line[0]);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 = dst + 8;
            M = _mm_loadu_si128((__m128i *)&first_line[8]);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
        } else {
            __m128i M = _mm_load_si128((__m128i *)&first_line[0]);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst2 = dst1 + i_dst;
            dst1 = dst + 8;
            M = _mm_loadu_si128((__m128i *)&first_line[8]);
            _mm_storel_epi64((__m128i *)dst1, M);
            _mm_storel_epi64((__m128i *)dst2, M);
            dst1 += i_dst;
            dst2 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            _mm_storel_epi64((__m128i *)dst2, M);
            dst1 += i_dst;
            dst2 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            _mm_storel_epi64((__m128i *)dst2, M);
            dst1 += i_dst;
            dst2 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            _mm_storel_epi64((__m128i *)dst2, M);
            dst1 += i_dst;
            dst2 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            _mm_storel_epi64((__m128i *)dst2, M);
            dst1 += i_dst;
            dst2 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            _mm_storel_epi64((__m128i *)dst2, M);
            dst1 += i_dst;
            dst2 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            _mm_storel_epi64((__m128i *)dst2, M);
            dst1 += i_dst;
            dst2 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            _mm_storel_epi64((__m128i *)dst2, M);
            dst1 += i_dst;
            M = _mm_loadu_si128((__m128i *)&first_line[16]);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
            dst1 += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst1, M);
        }
    } else if (iWidth == 8) {
        for (i = 0; i < iHeight; i += 8) {
            __m128i M = _mm_loadu_si128((__m128i *)&first_line[i]);
            _mm_storel_epi64((__m128i *)dst, M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst, M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst, M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst, M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst, M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst, M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst, M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            _mm_storel_epi64((__m128i *)dst, M);
            dst += i_dst;
        }
    } else {
        for (i = 0; i < iHeight; i += 4) {
            __m128i M = _mm_loadu_si128((__m128i *)&first_line[i]);
            ((int *)(dst))[0] = _mm_cvtsi128_si32(M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            ((int *)(dst))[0] = _mm_cvtsi128_si32(M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            ((int *)(dst))[0] = _mm_cvtsi128_si32(M);
            dst += i_dst;
            M = _mm_srli_si128(M, 1);
            ((int *)(dst))[0] = _mm_cvtsi128_si32(M);
            dst += i_dst;
        }
    }
}

void uavs3e_ipred_ang_y_32_sse(pel *src, pel *dst, int i_dst, int mode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[2 *(64 + 64)]);
    int line_size = iHeight / 2 + iWidth - 1;
    int real_size = COM_MIN(line_size, iHeight);
    int i;
    __m128i pad_val;
    int aligned_line_size = ((line_size + 63) >> 4) << 4;
    pel *pfirst[2];
    __m128i coeff2 = _mm_set1_epi16(2);
    __m128i zero = _mm_setzero_si128();
    __m128i shuffle1 = _mm_setr_epi8(15, 13, 11, 9, 7, 5, 3, 1, 14, 12, 10, 8, 6, 4, 2, 0);
    __m128i shuffle2 = _mm_setr_epi8(14, 12, 10, 8, 6, 4, 2, 0, 15, 13, 11, 9, 7, 5, 3, 1);
    int i_dst2 = i_dst * 2;

    pfirst[0] = first_line;
    pfirst[1] = first_line + aligned_line_size;

    src -= 18;

    for (i = 0; i < real_size - 4; i += 8, src -= 16) {
        __m128i p00, p01, p10, p11;
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);

        __m128i H0 = _mm_unpackhi_epi8(S0, zero);
        __m128i H1 = _mm_unpackhi_epi8(S1, zero);
        __m128i H2 = _mm_unpackhi_epi8(S2, zero);

        p00 = _mm_add_epi16(L0, L1);
        p01 = _mm_add_epi16(L1, L2);
        p00 = _mm_add_epi16(p00, coeff2);
        p00 = _mm_add_epi16(p00, p01);
        p00 = _mm_srli_epi16(p00, 2);

        p10 = _mm_add_epi16(H0, H1);
        p11 = _mm_add_epi16(H1, H2);
        p10 = _mm_add_epi16(p10, coeff2);
        p10 = _mm_add_epi16(p10, p11);
        p10 = _mm_srli_epi16(p10, 2);

        p00 = _mm_packus_epi16(p00, p10);
        p10 = _mm_shuffle_epi8(p00, shuffle2);
        p00 = _mm_shuffle_epi8(p00, shuffle1);
        _mm_storel_epi64((__m128i *)&pfirst[0][i], p00);
        _mm_storel_epi64((__m128i *)&pfirst[1][i], p10);
    }

    if (i < real_size) {
        __m128i p10, p11;
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i H0 = _mm_unpackhi_epi8(S0, zero);
        __m128i H1 = _mm_unpackhi_epi8(S1, zero);
        __m128i H2 = _mm_unpackhi_epi8(S2, zero);

        p10 = _mm_add_epi16(H0, H1);
        p11 = _mm_add_epi16(H1, H2);
        p10 = _mm_add_epi16(p10, coeff2);
        p10 = _mm_add_epi16(p10, p11);
        p10 = _mm_srli_epi16(p10, 2);

        p11 = _mm_packus_epi16(p10, p10);
        p10 = _mm_shuffle_epi8(p11, shuffle2);
        p11 = _mm_shuffle_epi8(p11, shuffle1);
        ((int *)&pfirst[0][i])[0] = _mm_cvtsi128_si32(p11);
        ((int *)&pfirst[1][i])[0] = _mm_cvtsi128_si32(p10);
    }

    // padding
    if (real_size < line_size) {
        pad_val = _mm_set1_epi8((char)pfirst[1][real_size - 1]);
        for (i = real_size; i < line_size; i++) {
            _mm_storeu_si128((__m128i *)&pfirst[0][i], pad_val);
            _mm_storeu_si128((__m128i *)&pfirst[1][i], pad_val);
        }
    }

    iHeight /= 2;

#define COPY_Y32(w) { \
    for (i = 0; i < iHeight; i++) { \
        memcpy(dst, pfirst[0] + i, w * sizeof(pel)); \
        memcpy(dst + i_dst, pfirst[1] + i, w * sizeof(pel)); \
        dst += i_dst2;\
    } \
}
    switch (iWidth) {
    case 4:
        COPY_Y32(4)
        break;
    case 8:
        COPY_Y32(8)
            break;
    case 12:
        COPY_Y32(12)
            break;
    case 16:
        COPY_Y32(16)
            break;
    case 24:
        COPY_Y32(24)
            break;
    case 32:
        COPY_Y32(32)
            break;
    case 48:
        COPY_Y32(48)
            break;
    case 64:
        COPY_Y32(64)
            break;
    }
}

void uavs3e_ipred_ang_xy_14_sse(pel *src, pel *dst, int i_dst, int mode, int iWidth, int iHeight)
{
    int i;
    __m128i coeff2 = _mm_set1_epi16(2);
    __m128i coeff3 = _mm_set1_epi16(3);
    __m128i coeff4 = _mm_set1_epi16(4);
    __m128i coeff5 = _mm_set1_epi16(5);
    __m128i coeff7 = _mm_set1_epi16(7);
    __m128i coeff8 = _mm_set1_epi16(8);
    __m128i zero = _mm_setzero_si128();
    if (iHeight != 4) {
        ALIGNED_16(pel first_line[4 *(64 + 32)]);
        int line_size = iWidth + iHeight / 4 - 1;
        int left_size = line_size - iWidth;
        int aligned_line_size = ((line_size + 31) >> 4) << 4;
        pel *pfirst[4];
        __m128i shuffle1 = _mm_setr_epi8(0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15);
        __m128i shuffle2 = _mm_setr_epi8(1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 0, 4, 8, 12);
        __m128i shuffle3 = _mm_setr_epi8(2, 6, 10, 14, 3, 7, 11, 15, 0, 4, 8, 12, 1, 5, 9, 13);
        __m128i shuffle4 = _mm_setr_epi8(3, 7, 11, 15, 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14);
        pel *src1 = src;

        pfirst[0] = first_line;
        pfirst[1] = first_line + aligned_line_size;
        pfirst[2] = first_line + aligned_line_size * 2;
        pfirst[3] = first_line + aligned_line_size * 3;

        src -= iHeight - 4;
        for (i = 0; i < left_size - 1; i += 4, src += 16) {
            __m128i p00, p01, p10, p11;
            __m128i p20, p30;
            __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
            __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
            __m128i S1 = _mm_loadu_si128((__m128i *)(src));

            __m128i L0 = _mm_unpacklo_epi8(S0, zero);
            __m128i L1 = _mm_unpacklo_epi8(S1, zero);
            __m128i L2 = _mm_unpacklo_epi8(S2, zero);

            __m128i H0 = _mm_unpackhi_epi8(S0, zero);
            __m128i H1 = _mm_unpackhi_epi8(S1, zero);
            __m128i H2 = _mm_unpackhi_epi8(S2, zero);

            p00 = _mm_add_epi16(L0, L1);
            p01 = _mm_add_epi16(L1, L2);
            p10 = _mm_add_epi16(H0, H1);
            p11 = _mm_add_epi16(H1, H2);

            p00 = _mm_add_epi16(p00, coeff2);
            p10 = _mm_add_epi16(p10, coeff2);
            p00 = _mm_add_epi16(p00, p01);
            p10 = _mm_add_epi16(p10, p11);

            p00 = _mm_srli_epi16(p00, 2);
            p10 = _mm_srli_epi16(p10, 2);

            p00 = _mm_packus_epi16(p00, p10);
            p10 = _mm_shuffle_epi8(p00, shuffle2);
            p20 = _mm_shuffle_epi8(p00, shuffle3);
            p30 = _mm_shuffle_epi8(p00, shuffle4);
            p00 = _mm_shuffle_epi8(p00, shuffle1);

            ((int *)&pfirst[0][i])[0] = _mm_cvtsi128_si32(p30);
            ((int *)&pfirst[1][i])[0] = _mm_cvtsi128_si32(p20);
            ((int *)&pfirst[2][i])[0] = _mm_cvtsi128_si32(p10);
            ((int *)&pfirst[3][i])[0] = _mm_cvtsi128_si32(p00);
        }

        if (i < left_size) { //使用c语言可能会更优
            __m128i p00, p01, p10;
            __m128i p20, p30;
            __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
            __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
            __m128i S1 = _mm_loadu_si128((__m128i *)(src));

            __m128i L0 = _mm_unpacklo_epi8(S0, zero);
            __m128i L1 = _mm_unpacklo_epi8(S1, zero);
            __m128i L2 = _mm_unpacklo_epi8(S2, zero);

            p00 = _mm_add_epi16(L0, L1);
            p01 = _mm_add_epi16(L1, L2);

            p00 = _mm_add_epi16(p00, coeff2);
            p00 = _mm_add_epi16(p00, p01);

            p00 = _mm_srli_epi16(p00, 2);

            p00 = _mm_packus_epi16(p00, p00);
            p10 = _mm_shuffle_epi8(p00, shuffle2);
            p20 = _mm_shuffle_epi8(p00, shuffle3);
            p30 = _mm_shuffle_epi8(p00, shuffle4);
            p00 = _mm_shuffle_epi8(p00, shuffle1);

            ((int *)&pfirst[0][i])[0] = _mm_cvtsi128_si32(p30);
            ((int *)&pfirst[1][i])[0] = _mm_cvtsi128_si32(p20);
            ((int *)&pfirst[2][i])[0] = _mm_cvtsi128_si32(p10);
            ((int *)&pfirst[3][i])[0] = _mm_cvtsi128_si32(p00);
        }

        src = src1;

        for (i = left_size; i < line_size; i += 16, src += 16) {
            __m128i p00, p10, p20, p30;
            __m128i p01, p11, p21, p31;
            __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
            __m128i S3 = _mm_loadu_si128((__m128i *)(src + 2));
            __m128i S1 = _mm_loadu_si128((__m128i *)(src));
            __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));

            __m128i L0 = _mm_unpacklo_epi8(S0, zero);
            __m128i L1 = _mm_unpacklo_epi8(S1, zero);
            __m128i L2 = _mm_unpacklo_epi8(S2, zero);
            __m128i L3 = _mm_unpacklo_epi8(S3, zero);

            __m128i H0 = _mm_unpackhi_epi8(S0, zero);
            __m128i H1 = _mm_unpackhi_epi8(S1, zero);
            __m128i H2 = _mm_unpackhi_epi8(S2, zero);
            __m128i H3 = _mm_unpackhi_epi8(S3, zero);

            p00 = _mm_mullo_epi16(L0, coeff3);
            p10 = _mm_mullo_epi16(L1, coeff7);
            p20 = _mm_mullo_epi16(L2, coeff5);
            p30 = _mm_add_epi16(L3, coeff8);
            p00 = _mm_add_epi16(p00, p30);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, p20);
            p00 = _mm_srli_epi16(p00, 4);

            p01 = _mm_mullo_epi16(H0, coeff3);
            p11 = _mm_mullo_epi16(H1, coeff7);
            p21 = _mm_mullo_epi16(H2, coeff5);
            p31 = _mm_add_epi16(H3, coeff8);
            p01 = _mm_add_epi16(p01, p31);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, p21);
            p01 = _mm_srli_epi16(p01, 4);

            p00 = _mm_packus_epi16(p00, p01);
            _mm_storeu_si128((__m128i *)&pfirst[2][i], p00);

            p00 = _mm_add_epi16(L1, L2);
            p00 = _mm_mullo_epi16(p00, coeff3);
            p10 = _mm_add_epi16(L0, L3);
            p10 = _mm_add_epi16(p10, coeff4);
            p00 = _mm_add_epi16(p10, p00);
            p00 = _mm_srli_epi16(p00, 3);

            p01 = _mm_add_epi16(H1, H2);
            p01 = _mm_mullo_epi16(p01, coeff3);
            p11 = _mm_add_epi16(H0, H3);
            p11 = _mm_add_epi16(p11, coeff4);
            p01 = _mm_add_epi16(p11, p01);
            p01 = _mm_srli_epi16(p01, 3);

            p00 = _mm_packus_epi16(p00, p01);
            _mm_storeu_si128((__m128i *)&pfirst[1][i], p00);

            p10 = _mm_mullo_epi16(L1, coeff5);
            p20 = _mm_mullo_epi16(L2, coeff7);
            p30 = _mm_mullo_epi16(L3, coeff3);
            p00 = _mm_add_epi16(L0, coeff8);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, p20);
            p00 = _mm_add_epi16(p00, p30);
            p00 = _mm_srli_epi16(p00, 4);

            p11 = _mm_mullo_epi16(H1, coeff5);
            p21 = _mm_mullo_epi16(H2, coeff7);
            p31 = _mm_mullo_epi16(H3, coeff3);
            p01 = _mm_add_epi16(H0, coeff8);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, p21);
            p01 = _mm_add_epi16(p01, p31);
            p01 = _mm_srli_epi16(p01, 4);

            p00 = _mm_packus_epi16(p00, p01);
            _mm_storeu_si128((__m128i *)&pfirst[0][i], p00);

            p00 = _mm_add_epi16(L0, L1);
            p10 = _mm_add_epi16(L1, L2);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, coeff2);
            p00 = _mm_srli_epi16(p00, 2);

            p01 = _mm_add_epi16(H0, H1);
            p11 = _mm_add_epi16(H1, H2);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, coeff2);
            p01 = _mm_srli_epi16(p01, 2);

            p00 = _mm_packus_epi16(p00, p01);
            _mm_storeu_si128((__m128i *)&pfirst[3][i], p00);
        }

        pfirst[0] += left_size;
        pfirst[1] += left_size;
        pfirst[2] += left_size;
        pfirst[3] += left_size;

        iHeight >>= 2;

        switch (iWidth) {
        case 4:
            for (i = 0; i < iHeight; i++) {
                CP32(dst, pfirst[0] - i);
                dst += i_dst;
                CP32(dst, pfirst[1] - i);
                dst += i_dst;
                CP32(dst, pfirst[2] - i);
                dst += i_dst;
                CP32(dst, pfirst[3] - i);
                dst += i_dst;
            }
            break;
        case 8:
            for (i = 0; i < iHeight; i++) {
                CP64(dst, pfirst[0] - i);
                dst += i_dst;
                CP64(dst, pfirst[1] - i);
                dst += i_dst;
                CP64(dst, pfirst[2] - i);
                dst += i_dst;
                CP64(dst, pfirst[3] - i);
                dst += i_dst;
            }
            break;
        case 16:
            for (i = 0; i < iHeight; i++) {
                CP128(dst, pfirst[0] - i);
                dst += i_dst;
                CP128(dst, pfirst[1] - i);
                dst += i_dst;
                CP128(dst, pfirst[2] - i);
                dst += i_dst;
                CP128(dst, pfirst[3] - i);
                dst += i_dst;
            }
            break;
        case 32:
        case 64:
            for (i = 0; i < iHeight; i++) {
                memcpy(dst, pfirst[0] - i, iWidth * sizeof(pel));
                dst += i_dst;
                memcpy(dst, pfirst[1] - i, iWidth * sizeof(pel));
                dst += i_dst;
                memcpy(dst, pfirst[2] - i, iWidth * sizeof(pel));
                dst += i_dst;
                memcpy(dst, pfirst[3] - i, iWidth * sizeof(pel));
                dst += i_dst;
            }
            break;
        default:
            com_assert(0);
            break;
        }
    } else {
        if (iWidth == 16) {
            pel *dst2 = dst + i_dst;
            pel *dst3 = dst2 + i_dst;
            pel *dst4 = dst3 + i_dst;
            __m128i p00, p10, p20, p30;
            __m128i p01, p11, p21, p31;
            __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
            __m128i S3 = _mm_loadu_si128((__m128i *)(src + 2));
            __m128i S1 = _mm_loadu_si128((__m128i *)(src));
            __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));

            __m128i L0 = _mm_unpacklo_epi8(S0, zero);
            __m128i L1 = _mm_unpacklo_epi8(S1, zero);
            __m128i L2 = _mm_unpacklo_epi8(S2, zero);
            __m128i L3 = _mm_unpacklo_epi8(S3, zero);

            __m128i H0 = _mm_unpackhi_epi8(S0, zero);
            __m128i H1 = _mm_unpackhi_epi8(S1, zero);
            __m128i H2 = _mm_unpackhi_epi8(S2, zero);
            __m128i H3 = _mm_unpackhi_epi8(S3, zero);

            p00 = _mm_mullo_epi16(L0, coeff3);
            p10 = _mm_mullo_epi16(L1, coeff7);
            p20 = _mm_mullo_epi16(L2, coeff5);
            p30 = _mm_add_epi16(L3, coeff8);
            p00 = _mm_add_epi16(p00, p30);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, p20);
            p00 = _mm_srli_epi16(p00, 4);

            p01 = _mm_mullo_epi16(H0, coeff3);
            p11 = _mm_mullo_epi16(H1, coeff7);
            p21 = _mm_mullo_epi16(H2, coeff5);
            p31 = _mm_add_epi16(H3, coeff8);
            p01 = _mm_add_epi16(p01, p31);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, p21);
            p01 = _mm_srli_epi16(p01, 4);

            p00 = _mm_packus_epi16(p00, p01);
            _mm_storeu_si128((__m128i *)dst3, p00);

            p00 = _mm_add_epi16(L1, L2);
            p00 = _mm_mullo_epi16(p00, coeff3);
            p10 = _mm_add_epi16(L0, L3);
            p10 = _mm_add_epi16(p10, coeff4);
            p00 = _mm_add_epi16(p10, p00);
            p00 = _mm_srli_epi16(p00, 3);

            p01 = _mm_add_epi16(H1, H2);
            p01 = _mm_mullo_epi16(p01, coeff3);
            p11 = _mm_add_epi16(H0, H3);
            p11 = _mm_add_epi16(p11, coeff4);
            p01 = _mm_add_epi16(p11, p01);
            p01 = _mm_srli_epi16(p01, 3);

            p00 = _mm_packus_epi16(p00, p01);
            _mm_storeu_si128((__m128i *)dst2, p00);

            p10 = _mm_mullo_epi16(L1, coeff5);
            p20 = _mm_mullo_epi16(L2, coeff7);
            p30 = _mm_mullo_epi16(L3, coeff3);
            p00 = _mm_add_epi16(L0, coeff8);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, p20);
            p00 = _mm_add_epi16(p00, p30);
            p00 = _mm_srli_epi16(p00, 4);

            p11 = _mm_mullo_epi16(H1, coeff5);
            p21 = _mm_mullo_epi16(H2, coeff7);
            p31 = _mm_mullo_epi16(H3, coeff3);
            p01 = _mm_add_epi16(H0, coeff8);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, p21);
            p01 = _mm_add_epi16(p01, p31);
            p01 = _mm_srli_epi16(p01, 4);

            p00 = _mm_packus_epi16(p00, p01);
            _mm_storeu_si128((__m128i *)dst, p00);

            p00 = _mm_add_epi16(L0, L1);
            p10 = _mm_add_epi16(L1, L2);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, coeff2);
            p00 = _mm_srli_epi16(p00, 2);

            p01 = _mm_add_epi16(H0, H1);
            p11 = _mm_add_epi16(H1, H2);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, coeff2);
            p01 = _mm_srli_epi16(p01, 2);

            p00 = _mm_packus_epi16(p00, p01);
            _mm_storeu_si128((__m128i *)dst4, p00);
        } else {
            pel *dst2 = dst + i_dst;
            pel *dst3 = dst2 + i_dst;
            pel *dst4 = dst3 + i_dst;
            __m128i p00, p10, p20, p30;
            __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
            __m128i S3 = _mm_loadu_si128((__m128i *)(src + 2));
            __m128i S1 = _mm_loadu_si128((__m128i *)(src));
            __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));

            __m128i L0 = _mm_unpacklo_epi8(S0, zero);
            __m128i L1 = _mm_unpacklo_epi8(S1, zero);
            __m128i L2 = _mm_unpacklo_epi8(S2, zero);
            __m128i L3 = _mm_unpacklo_epi8(S3, zero);

            p00 = _mm_mullo_epi16(L0, coeff3);
            p10 = _mm_mullo_epi16(L1, coeff7);
            p20 = _mm_mullo_epi16(L2, coeff5);
            p30 = _mm_add_epi16(L3, coeff8);
            p00 = _mm_add_epi16(p00, p30);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, p20);
            p00 = _mm_srli_epi16(p00, 4);

            p00 = _mm_packus_epi16(p00, p00);
            ((int *)dst3)[0] = _mm_cvtsi128_si32(p00);

            p00 = _mm_add_epi16(L1, L2);
            p00 = _mm_mullo_epi16(p00, coeff3);
            p10 = _mm_add_epi16(L0, L3);
            p10 = _mm_add_epi16(p10, coeff4);
            p00 = _mm_add_epi16(p10, p00);
            p00 = _mm_srli_epi16(p00, 3);

            p00 = _mm_packus_epi16(p00, p00);
            ((int *)dst2)[0] = _mm_cvtsi128_si32(p00);

            p10 = _mm_mullo_epi16(L1, coeff5);
            p20 = _mm_mullo_epi16(L2, coeff7);
            p30 = _mm_mullo_epi16(L3, coeff3);
            p00 = _mm_add_epi16(L0, coeff8);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, p20);
            p00 = _mm_add_epi16(p00, p30);
            p00 = _mm_srli_epi16(p00, 4);

            p00 = _mm_packus_epi16(p00, p00);
            ((int *)dst)[0] = _mm_cvtsi128_si32(p00);

            p00 = _mm_add_epi16(L0, L1);
            p10 = _mm_add_epi16(L1, L2);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, coeff2);
            p00 = _mm_srli_epi16(p00, 2);

            p00 = _mm_packus_epi16(p00, p00);
            ((int *)dst4)[0] = _mm_cvtsi128_si32(p00);
        }
    }
}

void uavs3e_ipred_ang_xy_16_sse(pel *src, pel *dst, int i_dst, int mode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[2 *(64 + 48)]);
    int line_size = iWidth + iHeight / 2 - 1;
    int left_size = line_size - iWidth;
    int aligned_line_size = ((line_size + 31) >> 4) << 4;
    pel *pfirst[2];
    __m128i zero = _mm_setzero_si128();
    __m128i coeff2 = _mm_set1_epi16(2);
    __m128i coeff3 = _mm_set1_epi16(3);
    __m128i coeff4 = _mm_set1_epi16(4);
    __m128i shuffle1 = _mm_setr_epi8(0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15);
    __m128i shuffle2 = _mm_setr_epi8(1, 3, 5, 7, 9, 11, 13, 15, 0, 2, 4, 6, 8, 10, 12, 14);
    int i;
    pel *src1;

    pfirst[0] = first_line;
    pfirst[1] = first_line + aligned_line_size;

    src -= iHeight - 2;

    src1 = src;

    for (i = 0; i < left_size - 4; i += 8, src += 16) {
        __m128i p00, p01, p10, p11;
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);

        __m128i H0 = _mm_unpackhi_epi8(S0, zero);
        __m128i H1 = _mm_unpackhi_epi8(S1, zero);
        __m128i H2 = _mm_unpackhi_epi8(S2, zero);

        p00 = _mm_add_epi16(L0, L1);
        p01 = _mm_add_epi16(L1, L2);
        p10 = _mm_add_epi16(H0, H1);
        p11 = _mm_add_epi16(H1, H2);

        p00 = _mm_add_epi16(p00, coeff2);
        p10 = _mm_add_epi16(p10, coeff2);

        p00 = _mm_add_epi16(p00, p01);
        p10 = _mm_add_epi16(p10, p11);

        p00 = _mm_srli_epi16(p00, 2);
        p10 = _mm_srli_epi16(p10, 2);
        p00 = _mm_packus_epi16(p00, p10);

        p10 = _mm_shuffle_epi8(p00, shuffle2);
        p00 = _mm_shuffle_epi8(p00, shuffle1);
        _mm_storel_epi64((__m128i *)&pfirst[1][i], p00);
        _mm_storel_epi64((__m128i *)&pfirst[0][i], p10);
    }

    if (i < left_size) {
        __m128i p00, p01;
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);

        p00 = _mm_add_epi16(L0, L1);
        p01 = _mm_add_epi16(L1, L2);
        p00 = _mm_add_epi16(p00, coeff2);
        p00 = _mm_add_epi16(p00, p01);
        p00 = _mm_srli_epi16(p00, 2);
        p00 = _mm_packus_epi16(p00, p00);

        p01 = _mm_shuffle_epi8(p00, shuffle2);
        p00 = _mm_shuffle_epi8(p00, shuffle1);
        ((int *)&pfirst[1][i])[0] = _mm_cvtsi128_si32(p00);
        ((int *)&pfirst[0][i])[0] = _mm_cvtsi128_si32(p01);
    }

    src = src1 + left_size + left_size;

    for (i = left_size; i < line_size; i += 16, src += 16) {
        __m128i p00, p01, p10, p11;
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S3 = _mm_loadu_si128((__m128i *)(src + 2));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);
        __m128i L3 = _mm_unpacklo_epi8(S3, zero);

        __m128i H0 = _mm_unpackhi_epi8(S0, zero);
        __m128i H1 = _mm_unpackhi_epi8(S1, zero);
        __m128i H2 = _mm_unpackhi_epi8(S2, zero);
        __m128i H3 = _mm_unpackhi_epi8(S3, zero);

        p00 = _mm_add_epi16(L1, L2);
        p10 = _mm_add_epi16(H1, H2);
        p00 = _mm_mullo_epi16(p00, coeff3);
        p10 = _mm_mullo_epi16(p10, coeff3);

        p01 = _mm_add_epi16(L0, L3);
        p11 = _mm_add_epi16(H0, H3);
        p00 = _mm_add_epi16(p00, coeff4);
        p10 = _mm_add_epi16(p10, coeff4);
        p00 = _mm_add_epi16(p00, p01);
        p10 = _mm_add_epi16(p10, p11);

        p00 = _mm_srli_epi16(p00, 3);
        p10 = _mm_srli_epi16(p10, 3);

        p00 = _mm_packus_epi16(p00, p10);
        _mm_storeu_si128((__m128i *)&pfirst[0][i], p00);

        p00 = _mm_add_epi16(L0, L1);
        p01 = _mm_add_epi16(L1, L2);
        p10 = _mm_add_epi16(H0, H1);
        p11 = _mm_add_epi16(H1, H2);

        p00 = _mm_add_epi16(p00, coeff2);
        p10 = _mm_add_epi16(p10, coeff2);

        p00 = _mm_add_epi16(p00, p01);
        p10 = _mm_add_epi16(p10, p11);

        p00 = _mm_srli_epi16(p00, 2);
        p10 = _mm_srli_epi16(p10, 2);

        p00 = _mm_packus_epi16(p00, p10);
        _mm_storeu_si128((__m128i *)&pfirst[1][i], p00);
    }

    pfirst[0] += left_size;
    pfirst[1] += left_size;

    iHeight >>= 1;

#define COPY_XY16(w) { \
    for (i = 0; i < iHeight; i++) { \
        memcpy(dst,         pfirst[0] - i, w * sizeof(pel)); \
        memcpy(dst + i_dst, pfirst[1] - i, w * sizeof(pel)); \
        dst += (i_dst << 1); \
    } \
}
    switch (iWidth) {
    case 4:
        COPY_XY16(4)
            break;
    case 8:
        COPY_XY16(8)
            break;
    case 12:
        COPY_XY16(12)
            break;
    case 16:
        COPY_XY16(16)
            break;
    case 24:
        COPY_XY16(24)
            break;
    case 32:
        COPY_XY16(32)
            break;
    case 48:
        COPY_XY16(48)
            break;
    case 64:
        COPY_XY16(64)
            break;
    }

}

void uavs3e_ipred_ang_xy_18_sse(pel *src, pel *dst, int i_dst, int mode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[64 + 64]);
    int line_size = iWidth + iHeight - 1;
    int i;
    pel *pfirst = first_line + iHeight - 1;
    __m128i coeff2 = _mm_set1_epi16(2);
    __m128i zero = _mm_setzero_si128();

    src -= iHeight - 1;

    for (i = 0; i < line_size - 8; i += 16, src += 16) {
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);

        __m128i H0 = _mm_unpackhi_epi8(S0, zero);
        __m128i H1 = _mm_unpackhi_epi8(S1, zero);
        __m128i H2 = _mm_unpackhi_epi8(S2, zero);

        __m128i sum1 = _mm_add_epi16(L0, L1);
        __m128i sum2 = _mm_add_epi16(L1, L2);
        __m128i sum3 = _mm_add_epi16(H0, H1);
        __m128i sum4 = _mm_add_epi16(H1, H2);

        sum1 = _mm_add_epi16(sum1, sum2);
        sum3 = _mm_add_epi16(sum3, sum4);

        sum1 = _mm_add_epi16(sum1, coeff2);
        sum3 = _mm_add_epi16(sum3, coeff2);

        sum1 = _mm_srli_epi16(sum1, 2);
        sum3 = _mm_srli_epi16(sum3, 2);

        sum1 = _mm_packus_epi16(sum1, sum3);

        _mm_store_si128((__m128i *)&first_line[i], sum1);
    }

    if (i < line_size) {
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);

        __m128i sum1 = _mm_add_epi16(L0, L1);
        __m128i sum2 = _mm_add_epi16(L1, L2);

        sum1 = _mm_add_epi16(sum1, sum2);
        sum1 = _mm_add_epi16(sum1, coeff2);
        sum1 = _mm_srli_epi16(sum1, 2);

        sum1 = _mm_packus_epi16(sum1, sum1);
        _mm_storel_epi64((__m128i *)&first_line[i], sum1);
    }

    switch (iWidth) {
    case 4:
        for (i = 0; i < iHeight; i++) {
            CP32(dst, pfirst--);
            dst += i_dst;
        }
        break;
    case 8:
        for (i = 0; i < iHeight; i++) {
            CP64(dst, pfirst--);
            dst += i_dst;
        }
        break;
    case 16:
        for (i = 0; i < iHeight; i++) {
            CP128(dst, pfirst--);
            dst += i_dst;
        }
        break;
    default:
        for (i = 0; i < iHeight; i++) {
            memcpy(dst, pfirst--, iWidth * sizeof(pel));
            dst += i_dst;
        }
        break;
        break;
    }

}

void uavs3e_ipred_ang_xy_20_sse(pel *src, pel *dst, int i_dst, int mode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[64 + 128]);
    int left_size = (iHeight - 1) * 2 + 1;
    int top_size = iWidth - 1;
    int line_size = left_size + top_size;
    int i;
    pel *pfirst = first_line + left_size - 1;
    __m128i zero = _mm_setzero_si128();
    __m128i coeff2 = _mm_set1_epi16(2);
    __m128i coeff3 = _mm_set1_epi16(3);
    __m128i coeff4 = _mm_set1_epi16(4);
    __m128i shuffle = _mm_setr_epi8(0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15);
    pel *src1 = src;
    src -= iHeight;

    for (i = 0; i < left_size - 16; i += 32, src += 16) {
        __m128i p00, p01, p10, p11;
        __m128i p20, p21, p30, p31;
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S3 = _mm_loadu_si128((__m128i *)(src + 2));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);
        __m128i L3 = _mm_unpacklo_epi8(S3, zero);

        __m128i H0 = _mm_unpackhi_epi8(S0, zero);
        __m128i H1 = _mm_unpackhi_epi8(S1, zero);
        __m128i H2 = _mm_unpackhi_epi8(S2, zero);
        __m128i H3 = _mm_unpackhi_epi8(S3, zero);

        p00 = _mm_add_epi16(L1, L2);
        p10 = _mm_add_epi16(H1, H2);
        p00 = _mm_mullo_epi16(p00, coeff3);
        p10 = _mm_mullo_epi16(p10, coeff3);

        p01 = _mm_add_epi16(L0, L3);
        p11 = _mm_add_epi16(H0, H3);
        p00 = _mm_add_epi16(p00, coeff4);
        p10 = _mm_add_epi16(p10, coeff4);
        p00 = _mm_add_epi16(p00, p01);
        p10 = _mm_add_epi16(p10, p11);

        p00 = _mm_srli_epi16(p00, 3);
        p10 = _mm_srli_epi16(p10, 3);

        p20 = _mm_add_epi16(L1, L2);
        p30 = _mm_add_epi16(H1, H2);
        p21 = _mm_add_epi16(L2, L3);
        p31 = _mm_add_epi16(H2, H3);
        p20 = _mm_add_epi16(p20, coeff2);
        p30 = _mm_add_epi16(p30, coeff2);
        p20 = _mm_add_epi16(p20, p21);
        p30 = _mm_add_epi16(p30, p31);

        p20 = _mm_srli_epi16(p20, 2);
        p30 = _mm_srli_epi16(p30, 2);

        p00 = _mm_packus_epi16(p00, p20);
        p10 = _mm_packus_epi16(p10, p30);

        p00 = _mm_shuffle_epi8(p00, shuffle);
        p10 = _mm_shuffle_epi8(p10, shuffle);
        _mm_store_si128((__m128i *)&first_line[i], p00);
        _mm_store_si128((__m128i *)&first_line[i + 16], p10);
    }

    if (i < left_size) {
        __m128i p00, p01;
        __m128i p20, p21;
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S3 = _mm_loadu_si128((__m128i *)(src + 2));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);
        __m128i L3 = _mm_unpacklo_epi8(S3, zero);

        p00 = _mm_add_epi16(L1, L2);
        p00 = _mm_mullo_epi16(p00, coeff3);

        p01 = _mm_add_epi16(L0, L3);
        p00 = _mm_add_epi16(p00, coeff4);
        p00 = _mm_add_epi16(p00, p01);

        p00 = _mm_srli_epi16(p00, 3);

        p20 = _mm_add_epi16(L1, L2);
        p21 = _mm_add_epi16(L2, L3);
        p20 = _mm_add_epi16(p20, coeff2);
        p20 = _mm_add_epi16(p20, p21);

        p20 = _mm_srli_epi16(p20, 2);

        p00 = _mm_packus_epi16(p00, p20);

        p00 = _mm_shuffle_epi8(p00, shuffle);
        _mm_store_si128((__m128i *)&first_line[i], p00);
    }

    src = src1;

    for (i = left_size; i < line_size - 8; i += 16, src += 16) {
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);

        __m128i H0 = _mm_unpackhi_epi8(S0, zero);
        __m128i H1 = _mm_unpackhi_epi8(S1, zero);
        __m128i H2 = _mm_unpackhi_epi8(S2, zero);

        __m128i sum1 = _mm_add_epi16(L0, L1);
        __m128i sum2 = _mm_add_epi16(L1, L2);
        __m128i sum3 = _mm_add_epi16(H0, H1);
        __m128i sum4 = _mm_add_epi16(H1, H2);

        sum1 = _mm_add_epi16(sum1, sum2);
        sum3 = _mm_add_epi16(sum3, sum4);

        sum1 = _mm_add_epi16(sum1, coeff2);
        sum3 = _mm_add_epi16(sum3, coeff2);

        sum1 = _mm_srli_epi16(sum1, 2);
        sum3 = _mm_srli_epi16(sum3, 2);

        sum1 = _mm_packus_epi16(sum1, sum3);

        _mm_storeu_si128((__m128i *)&first_line[i], sum1);
    }

    if (i < line_size) {
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i L0 = _mm_unpacklo_epi8(S0, zero);
        __m128i L1 = _mm_unpacklo_epi8(S1, zero);
        __m128i L2 = _mm_unpacklo_epi8(S2, zero);

        __m128i sum1 = _mm_add_epi16(L0, L1);
        __m128i sum2 = _mm_add_epi16(L1, L2);

        sum1 = _mm_add_epi16(sum1, sum2);
        sum1 = _mm_add_epi16(sum1, coeff2);
        sum1 = _mm_srli_epi16(sum1, 2);

        sum1 = _mm_packus_epi16(sum1, sum1);
        _mm_storel_epi64((__m128i *)&first_line[i], sum1);
    }

#define COPY_XY20(w) { \
    for (i = 0; i < iHeight; i++) { \
        memcpy(dst, pfirst, w * sizeof(pel)); \
        pfirst -= 2; \
        dst += i_dst;\
    } \
}
    switch (iWidth) {
    case 4:
        COPY_XY20(4)
            break;
    case 8:
        COPY_XY20(8)
            break;
    case 12:
        COPY_XY20(12)
            break;
    case 16:
        COPY_XY20(16)
            break;
    case 24:
        COPY_XY20(24)
            break;
    case 32:
        COPY_XY20(32)
            break;
    case 48:
        COPY_XY20(48)
            break;
    case 64:
        COPY_XY20(64)
            break;
    }
}

void uavs3e_ipred_ang_xy_22_sse(pel *src, pel *dst, int i_dst, int mode, int iWidth, int iHeight)
{
    int i;
    src -= iHeight;

    if (iWidth != 4) {
        ALIGNED_16(pel first_line[64 + 256]);
        int left_size = (iHeight - 1) * 4 + 3;
        int top_size = iWidth - 3;
        int line_size = left_size + top_size;
        pel *pfirst = first_line + left_size - 3;
        pel *src1 = src;

        __m128i zero = _mm_setzero_si128();
        __m128i coeff2 = _mm_set1_epi16(2);
        __m128i coeff3 = _mm_set1_epi16(3);
        __m128i coeff4 = _mm_set1_epi16(4);
        __m128i coeff5 = _mm_set1_epi16(5);
        __m128i coeff7 = _mm_set1_epi16(7);
        __m128i coeff8 = _mm_set1_epi16(8);
        __m128i shuffle = _mm_setr_epi8(0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15);

        for (i = 0; i < line_size - 32; i += 64, src += 16) {
            __m128i p00, p10, p20, p30;
            __m128i p01, p11, p21, p31;
            __m128i M1, M2, M3, M4, M5, M6, M7, M8;
            __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
            __m128i S3 = _mm_loadu_si128((__m128i *)(src + 2));
            __m128i S1 = _mm_loadu_si128((__m128i *)(src));
            __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));

            __m128i L0 = _mm_unpacklo_epi8(S0, zero);
            __m128i L1 = _mm_unpacklo_epi8(S1, zero);
            __m128i L2 = _mm_unpacklo_epi8(S2, zero);
            __m128i L3 = _mm_unpacklo_epi8(S3, zero);

            __m128i H0 = _mm_unpackhi_epi8(S0, zero);
            __m128i H1 = _mm_unpackhi_epi8(S1, zero);
            __m128i H2 = _mm_unpackhi_epi8(S2, zero);
            __m128i H3 = _mm_unpackhi_epi8(S3, zero);

            p00 = _mm_mullo_epi16(L0, coeff3);
            p10 = _mm_mullo_epi16(L1, coeff7);
            p20 = _mm_mullo_epi16(L2, coeff5);
            p30 = _mm_add_epi16(L3, coeff8);
            p00 = _mm_add_epi16(p00, p30);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, p20);
            M1 = _mm_srli_epi16(p00, 4);

            p01 = _mm_mullo_epi16(H0, coeff3);
            p11 = _mm_mullo_epi16(H1, coeff7);
            p21 = _mm_mullo_epi16(H2, coeff5);
            p31 = _mm_add_epi16(H3, coeff8);
            p01 = _mm_add_epi16(p01, p31);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, p21);
            M2 = _mm_srli_epi16(p01, 4);


            p00 = _mm_add_epi16(L1, L2);
            p00 = _mm_mullo_epi16(p00, coeff3);
            p10 = _mm_add_epi16(L0, L3);
            p10 = _mm_add_epi16(p10, coeff4);
            p00 = _mm_add_epi16(p10, p00);
            M3 = _mm_srli_epi16(p00, 3);

            p01 = _mm_add_epi16(H1, H2);
            p01 = _mm_mullo_epi16(p01, coeff3);
            p11 = _mm_add_epi16(H0, H3);
            p11 = _mm_add_epi16(p11, coeff4);
            p01 = _mm_add_epi16(p11, p01);
            M4 = _mm_srli_epi16(p01, 3);


            p10 = _mm_mullo_epi16(L1, coeff5);
            p20 = _mm_mullo_epi16(L2, coeff7);
            p30 = _mm_mullo_epi16(L3, coeff3);
            p00 = _mm_add_epi16(L0, coeff8);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, p20);
            p00 = _mm_add_epi16(p00, p30);
            M5 = _mm_srli_epi16(p00, 4);

            p11 = _mm_mullo_epi16(H1, coeff5);
            p21 = _mm_mullo_epi16(H2, coeff7);
            p31 = _mm_mullo_epi16(H3, coeff3);
            p01 = _mm_add_epi16(H0, coeff8);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, p21);
            p01 = _mm_add_epi16(p01, p31);
            M6 = _mm_srli_epi16(p01, 4);


            p00 = _mm_add_epi16(L1, L2);
            p10 = _mm_add_epi16(L2, L3);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, coeff2);
            M7 = _mm_srli_epi16(p00, 2);

            p01 = _mm_add_epi16(H1, H2);
            p11 = _mm_add_epi16(H2, H3);
            p01 = _mm_add_epi16(p01, p11);
            p01 = _mm_add_epi16(p01, coeff2);
            M8 = _mm_srli_epi16(p01, 2);

            M1 = _mm_packus_epi16(M1, M3);
            M5 = _mm_packus_epi16(M5, M7);
            M1 = _mm_shuffle_epi8(M1, shuffle);
            M5 = _mm_shuffle_epi8(M5, shuffle);

            M2 = _mm_packus_epi16(M2, M4);
            M6 = _mm_packus_epi16(M6, M8);
            M2 = _mm_shuffle_epi8(M2, shuffle);
            M6 = _mm_shuffle_epi8(M6, shuffle);

            M3 = _mm_unpacklo_epi16(M1, M5);
            M7 = _mm_unpackhi_epi16(M1, M5);
            M4 = _mm_unpacklo_epi16(M2, M6);
            M8 = _mm_unpackhi_epi16(M2, M6);

            _mm_store_si128((__m128i *)&first_line[i], M3);
            _mm_store_si128((__m128i *)&first_line[16 + i], M7);
            _mm_store_si128((__m128i *)&first_line[32 + i], M4);
            _mm_store_si128((__m128i *)&first_line[48 + i], M8);
        }

        if (i < left_size) {
            __m128i p00, p10, p20, p30;
            __m128i M1, M3, M5, M7;
            __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
            __m128i S3 = _mm_loadu_si128((__m128i *)(src + 2));
            __m128i S1 = _mm_loadu_si128((__m128i *)(src));
            __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));

            __m128i L0 = _mm_unpacklo_epi8(S0, zero);
            __m128i L1 = _mm_unpacklo_epi8(S1, zero);
            __m128i L2 = _mm_unpacklo_epi8(S2, zero);
            __m128i L3 = _mm_unpacklo_epi8(S3, zero);

            p00 = _mm_mullo_epi16(L0, coeff3);
            p10 = _mm_mullo_epi16(L1, coeff7);
            p20 = _mm_mullo_epi16(L2, coeff5);
            p30 = _mm_add_epi16(L3, coeff8);
            p00 = _mm_add_epi16(p00, p30);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, p20);
            M1 = _mm_srli_epi16(p00, 4);

            p00 = _mm_add_epi16(L1, L2);
            p00 = _mm_mullo_epi16(p00, coeff3);
            p10 = _mm_add_epi16(L0, L3);
            p10 = _mm_add_epi16(p10, coeff4);
            p00 = _mm_add_epi16(p10, p00);
            M3 = _mm_srli_epi16(p00, 3);

            p10 = _mm_mullo_epi16(L1, coeff5);
            p20 = _mm_mullo_epi16(L2, coeff7);
            p30 = _mm_mullo_epi16(L3, coeff3);
            p00 = _mm_add_epi16(L0, coeff8);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, p20);
            p00 = _mm_add_epi16(p00, p30);
            M5 = _mm_srli_epi16(p00, 4);

            p00 = _mm_add_epi16(L1, L2);
            p10 = _mm_add_epi16(L2, L3);
            p00 = _mm_add_epi16(p00, p10);
            p00 = _mm_add_epi16(p00, coeff2);
            M7 = _mm_srli_epi16(p00, 2);

            M1 = _mm_packus_epi16(M1, M3);
            M5 = _mm_packus_epi16(M5, M7);
            M1 = _mm_shuffle_epi8(M1, shuffle);
            M5 = _mm_shuffle_epi8(M5, shuffle);

            M3 = _mm_unpacklo_epi16(M1, M5);
            M7 = _mm_unpackhi_epi16(M1, M5);

            _mm_store_si128((__m128i *)&first_line[i], M3);
            _mm_store_si128((__m128i *)&first_line[16 + i], M7);
        }

        src = src1 + iHeight;

        for (i = left_size; i < line_size - 8; i += 16, src += 16) {
            __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
            __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
            __m128i S1 = _mm_loadu_si128((__m128i *)(src));

            __m128i L0 = _mm_unpacklo_epi8(S0, zero);
            __m128i L1 = _mm_unpacklo_epi8(S1, zero);
            __m128i L2 = _mm_unpacklo_epi8(S2, zero);

            __m128i H0 = _mm_unpackhi_epi8(S0, zero);
            __m128i H1 = _mm_unpackhi_epi8(S1, zero);
            __m128i H2 = _mm_unpackhi_epi8(S2, zero);

            __m128i sum1 = _mm_add_epi16(L0, L1);
            __m128i sum2 = _mm_add_epi16(L1, L2);
            __m128i sum3 = _mm_add_epi16(H0, H1);
            __m128i sum4 = _mm_add_epi16(H1, H2);

            sum1 = _mm_add_epi16(sum1, sum2);
            sum3 = _mm_add_epi16(sum3, sum4);

            sum1 = _mm_add_epi16(sum1, coeff2);
            sum3 = _mm_add_epi16(sum3, coeff2);

            sum1 = _mm_srli_epi16(sum1, 2);
            sum3 = _mm_srli_epi16(sum3, 2);

            sum1 = _mm_packus_epi16(sum1, sum3);

            _mm_storeu_si128((__m128i *)&first_line[i], sum1);
        }

        if (i < line_size) {
            __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
            __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
            __m128i S1 = _mm_loadu_si128((__m128i *)(src));

            __m128i L0 = _mm_unpacklo_epi8(S0, zero);
            __m128i L1 = _mm_unpacklo_epi8(S1, zero);
            __m128i L2 = _mm_unpacklo_epi8(S2, zero);

            __m128i sum1 = _mm_add_epi16(L0, L1);
            __m128i sum2 = _mm_add_epi16(L1, L2);

            sum1 = _mm_add_epi16(sum1, sum2);
            sum1 = _mm_add_epi16(sum1, coeff2);
            sum1 = _mm_srli_epi16(sum1, 2);

            sum1 = _mm_packus_epi16(sum1, sum1);
            _mm_storel_epi64((__m128i *)&first_line[i], sum1);
        }

        switch (iWidth) {
        case 8:
            while (iHeight--) {
                CP64(dst, pfirst);
                dst += i_dst;
                pfirst -= 4;
            }
            break;
        case 16:
            while (iHeight--) {
                CP128(dst, pfirst);
                dst += i_dst;
                pfirst -= 4;
            }
            break;
        case 32:
        case 64:
            while (iHeight--) {
                memcpy(dst, pfirst, iWidth * sizeof(pel));
                dst += i_dst;
                pfirst -= 4;
            }
            break;
        default:
            com_assert(0);
            break;
        }
    } else {
        dst += (iHeight - 1) * i_dst;
        for (i = 0; i < iHeight; i++, src++) {
            dst[0] = (src[-1] * 3 + src[0] * 7 + src[1] * 5 + src[2] + 8) >> 4;
            dst[1] = (src[-1] + (src[0] + src[1]) * 3 + src[2] + 4) >> 3;
            dst[2] = (src[-1] + src[0] * 5 + src[1] * 7 + src[2] * 3 + 8) >> 4;
            dst[3] = (src[0] + src[1] * 2 + src[2] + 2) >> 2;
            dst -= i_dst;
        }
    }
}

void uavs3e_ipred_ang_x_sse(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int offset;
    __m128i mAddOffset = _mm_set1_epi16(64);

    if (iWidth == 4) {
        __m128i mSwitch = _mm_setr_epi8(0, 1, 2, 3, 1, 2, 3, 4, 2, 3, 4, 5, 3, 4, 5, 6);

        for (int j = 0; j < iHeight; j += 2) {
            int offset2;
            pel *p1 = pSrc + getContextPixel(uiDirMode, 0, j + 1, &offset);
            pel *p2 = pSrc + getContextPixel(uiDirMode, 0, j + 2, &offset2);

            int c1 = 32 - offset;
            int c2 = 64 - offset;
            int c3 = 32 + offset;
            int c4 = offset;
            __m128i C1 = _mm_set1_epi32((c4 << 24) | (c3 << 16) | (c2 << 8) | c1);

            c1 = 32 - offset2;
            c2 = 64 - offset2;
            c3 = 32 + offset2;
            c4 = offset2;
            __m128i C2 = _mm_set1_epi32((c4 << 24) | (c3 << 16) | (c2 << 8) | c1);

            __m128i T1 = _mm_maddubs_epi16(_mm_shuffle_epi8(_mm_loadl_epi64((__m128i*)p1), mSwitch), C1);
            __m128i T2 = _mm_maddubs_epi16(_mm_shuffle_epi8(_mm_loadl_epi64((__m128i*)p2), mSwitch), C2);

            T1 = _mm_hadd_epi16(T1, T2);
            T1 = _mm_add_epi16(T1, mAddOffset);
            T1 = _mm_srai_epi16(T1, 7);
            T1 = _mm_packus_epi16(T1, T1);
            *(int*)dst = _mm_extract_epi32(T1, 0);
            dst += i_dst;
            *(int*)dst = _mm_extract_epi32(T1, 1);
            dst += i_dst;
        }
    }
    else if (iWidth == 12) {
        __m128i mSwitch1 = _mm_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
        __m128i mSwitch2 = _mm_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);

        for (int j = 0; j < iHeight; j++) {
            int c1, c2, c3, c4;
            pel *p = pSrc + getContextPixel(uiDirMode, 0, j + 1, &offset);

            c1 = 32 - offset;
            c2 = 64 - offset;
            c3 = 32 + offset;
            c4 = offset;

            int coef1 = (c2 << 8) | c1;
            int coef2 = (c4 << 8) | c3;

            __m128i C1 = _mm_set1_epi16(coef1);
            __m128i C2 = _mm_set1_epi16(coef2);

            __m128i mSrc = _mm_loadu_si128((__m128i*)(p));
            __m128i T0 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), C1);
            __m128i T1 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), C2);

            T0 = _mm_add_epi16(T0, T1);
            T0 = _mm_add_epi16(T0, mAddOffset);
            T0 = _mm_srai_epi16(T0, 7);
            T0 = _mm_packus_epi16(T0, T0);

            _mm_storel_epi64((__m128i*)dst, T0);

            mSrc = _mm_loadl_epi64((__m128i*)(p + 8));
            T0 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), C1);
            T1 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), C2);

            T0 = _mm_add_epi16(T0, T1);
            T0 = _mm_add_epi16(T0, mAddOffset);
            T0 = _mm_srai_epi16(T0, 7);
            T0 = _mm_packus_epi16(T0, T0);
           
            *(int*)(dst + 8) = _mm_extract_epi32(T0, 0);

            dst += i_dst;
        }

    } else { // x 8
        __m128i mSwitch1 = _mm_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
        __m128i mSwitch2 = _mm_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);

        for (int j = 0; j < iHeight; j++) {
            int c1, c2, c3, c4;
            pel *p = pSrc + getContextPixel(uiDirMode, 0, j + 1, &offset);

            c1 = 32 - offset;
            c2 = 64 - offset;
            c3 = 32 + offset;
            c4 = offset;

            int coef1 = (c2 << 8) | c1;
            int coef2 = (c4 << 8) | c3;

            __m128i C1 = _mm_set1_epi16(coef1);
            __m128i C2 = _mm_set1_epi16(coef2);

            for (int col = 0; col < iWidth; col += 8) {
                __m128i mSrc = _mm_loadu_si128((__m128i*)(p + col));
                __m128i T0 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), C1);
                __m128i T1 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), C2);

                T0 = _mm_add_epi16(T0, T1);
                T0 = _mm_add_epi16(T0, mAddOffset);
                T0 = _mm_srai_epi16(T0, 7);
                T0 = _mm_packus_epi16(T0, T0);

                _mm_storel_epi64((__m128i*)&dst[col], T0);
            }
            dst += i_dst;
        }
    }
}

#define TRANS_4x4(t0) {                  \
    __m128i t1 = _mm_srli_si128(t0,  4); \
    __m128i t2 = _mm_srli_si128(t0,  8); \
    __m128i t3 = _mm_srli_si128(t0, 12); \
    t0 = _mm_unpacklo_epi8 (t0, t1);     \
    t2 = _mm_unpacklo_epi8 (t2, t3);     \
    t0 = _mm_unpacklo_epi16(t0, t2);     \
}

#define TRANS_8x8(t0, t2, t4, t6) {      \
    __m128i t1, t3, t5, t7;              \
    t1 = _mm_srli_si128(t0, 8);          \
    t3 = _mm_srli_si128(t2, 8);          \
    t5 = _mm_srli_si128(t4, 8);          \
    t7 = _mm_srli_si128(t6, 8);          \
    t0 = _mm_unpacklo_epi8(t0, t1);      \
    t2 = _mm_unpacklo_epi8(t2, t3);      \
    t4 = _mm_unpacklo_epi8(t4, t5);      \
    t6 = _mm_unpacklo_epi8(t6, t7);      \
    t1 = _mm_unpacklo_epi16(t0, t2);     \
    t3 = _mm_unpacklo_epi16(t4, t6);     \
    t5 = _mm_unpackhi_epi16(t0, t2);     \
    t7 = _mm_unpackhi_epi16(t4, t6);     \
    t0 = _mm_unpacklo_epi32(t1, t3);     \
    t2 = _mm_unpackhi_epi32(t1, t3);     \
    t4 = _mm_unpacklo_epi32(t5, t7);     \
    t6 = _mm_unpackhi_epi32(t5, t7);     \
}

#define TRANS_8x4(t0, t2) {              \
    __m128i t1, t3;                      \
    t1 = _mm_srli_si128(t0, 8);          \
    t3 = _mm_srli_si128(t2, 8);          \
    t1 = _mm_unpacklo_epi8(t0, t1);      \
    t3 = _mm_unpacklo_epi8(t2, t3);      \
    t0 = _mm_unpacklo_epi16(t1, t3);     \
    t2 = _mm_unpackhi_epi16(t1, t3);     \
}

void uavs3e_ipred_ang_y_sse(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    __m128i mAddOffset = _mm_set1_epi16(64);

    if (iHeight == 4) {
        __m128i mSwitch = _mm_setr_epi8(3, 4, 5, 6, 2, 3, 4, 5, 1, 2, 3, 4, 0, 1, 2, 3);
 
        for (int i = 0; i < iWidth; i += 4) {
            int offset1, offset2, offset3, offset4;
            pel *p1 = pSrc - getContextPixel(uiDirMode, 1, i + 1, &offset1) - 6;
            pel *p2 = pSrc - getContextPixel(uiDirMode, 1, i + 2, &offset2) - 6;
            pel *p3 = pSrc - getContextPixel(uiDirMode, 1, i + 3, &offset3) - 6;
            pel *p4 = pSrc - getContextPixel(uiDirMode, 1, i + 4, &offset4) - 6;

            __m128i C1 = _mm_set1_epi32(((32 - offset1) << 24) | ((64 - offset1) << 16) | ((32 + offset1) << 8) | offset1);
            __m128i C2 = _mm_set1_epi32(((32 - offset2) << 24) | ((64 - offset2) << 16) | ((32 + offset2) << 8) | offset2);
            __m128i C3 = _mm_set1_epi32(((32 - offset3) << 24) | ((64 - offset3) << 16) | ((32 + offset3) << 8) | offset3);
            __m128i C4 = _mm_set1_epi32(((32 - offset4) << 24) | ((64 - offset4) << 16) | ((32 + offset4) << 8) | offset4);

            __m128i T1 = _mm_maddubs_epi16(_mm_shuffle_epi8(_mm_loadl_epi64((__m128i*)p1), mSwitch), C1);
            __m128i T2 = _mm_maddubs_epi16(_mm_shuffle_epi8(_mm_loadl_epi64((__m128i*)p2), mSwitch), C2);
            __m128i T3 = _mm_maddubs_epi16(_mm_shuffle_epi8(_mm_loadl_epi64((__m128i*)p3), mSwitch), C3);
            __m128i T4 = _mm_maddubs_epi16(_mm_shuffle_epi8(_mm_loadl_epi64((__m128i*)p4), mSwitch), C4);

            T1 = _mm_hadd_epi16(T1, T2); T1 = _mm_add_epi16(T1, mAddOffset); T1 = _mm_srai_epi16(T1, 7);
            T3 = _mm_hadd_epi16(T3, T4); T3 = _mm_add_epi16(T3, mAddOffset); T3 = _mm_srai_epi16(T3, 7);
            T1 = _mm_packus_epi16(T1, T3);

            TRANS_4x4(T1);

            pel *p = dst + i;

            *(int*)p = _mm_extract_epi32(T1, 0); p += i_dst;
            *(int*)p = _mm_extract_epi32(T1, 1); p += i_dst;
            *(int*)p = _mm_extract_epi32(T1, 2); p += i_dst;
            *(int*)p = _mm_extract_epi32(T1, 3); 
        }
    } else if (iHeight == 12) {
        for (int i = 0; i < iWidth; i++) {
            int offset;
            pel *p = pSrc - getContextPixel(uiDirMode, 1, i + 1, &offset);

            for (int j = 0; j < iHeight; j++, p--) {
                dst[j*i_dst] = (p[0] * (32 - offset) + p[-1] * (64 - offset) + p[-2] * (32 + offset) + p[-3] * offset + 64) >> 7;
            }
            dst++;
        }
    } else { // iHeight == 8x
        if (iWidth == 4) {
            __m128i mSwitch1 = _mm_setr_epi8(7, 8, 6, 7, 5, 6, 4, 5, 3, 4, 2, 3, 1, 2, 0, 1);
            __m128i mSwitch2 = _mm_setr_epi8(9, 10, 8, 9, 7, 8, 6, 7, 5, 6, 4, 5, 3, 4, 2, 3);

            int offset1, offset2, offset3, offset4;
            pel *p1 = pSrc - getContextPixel(uiDirMode, 1, 1, &offset1) - 10;
            pel *p2 = pSrc - getContextPixel(uiDirMode, 1, 2, &offset2) - 10;
            pel *p3 = pSrc - getContextPixel(uiDirMode, 1, 3, &offset3) - 10;
            pel *p4 = pSrc - getContextPixel(uiDirMode, 1, 4, &offset4) - 10;

            __m128i C1_1 = _mm_set1_epi16(((32 + offset1) << 8) | offset1);
            __m128i C2_1 = _mm_set1_epi16(((32 + offset2) << 8) | offset2);
            __m128i C3_1 = _mm_set1_epi16(((32 + offset3) << 8) | offset3);
            __m128i C4_1 = _mm_set1_epi16(((32 + offset4) << 8) | offset4);
            __m128i C1_2 = _mm_set1_epi16(((32 - offset1) << 8) | (64 - offset1));
            __m128i C2_2 = _mm_set1_epi16(((32 - offset2) << 8) | (64 - offset2));
            __m128i C3_2 = _mm_set1_epi16(((32 - offset3) << 8) | (64 - offset3));
            __m128i C4_2 = _mm_set1_epi16(((32 - offset4) << 8) | (64 - offset4));

            for (int j = 0; j < iHeight; j += 8) {
                __m128i mSrc1 = _mm_loadu_si128((__m128i*)p1);
                __m128i mSrc2 = _mm_loadu_si128((__m128i*)p2);
                __m128i mSrc3 = _mm_loadu_si128((__m128i*)p3);
                __m128i mSrc4 = _mm_loadu_si128((__m128i*)p4);

                __m128i T1 = _mm_add_epi16(_mm_maddubs_epi16(_mm_shuffle_epi8(mSrc1, mSwitch1), C1_1), _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc1, mSwitch2), C1_2));
                __m128i T2 = _mm_add_epi16(_mm_maddubs_epi16(_mm_shuffle_epi8(mSrc2, mSwitch1), C2_1), _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc2, mSwitch2), C2_2));
                __m128i T3 = _mm_add_epi16(_mm_maddubs_epi16(_mm_shuffle_epi8(mSrc3, mSwitch1), C3_1), _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc3, mSwitch2), C3_2));
                __m128i T4 = _mm_add_epi16(_mm_maddubs_epi16(_mm_shuffle_epi8(mSrc4, mSwitch1), C4_1), _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc4, mSwitch2), C4_2));

                T1 = _mm_srai_epi16(_mm_add_epi16(T1, mAddOffset), 7);
                T2 = _mm_srai_epi16(_mm_add_epi16(T2, mAddOffset), 7);
                T3 = _mm_srai_epi16(_mm_add_epi16(T3, mAddOffset), 7);
                T4 = _mm_srai_epi16(_mm_add_epi16(T4, mAddOffset), 7);

                T1 = _mm_packus_epi16(T1, T2);
                T3 = _mm_packus_epi16(T3, T4);

                TRANS_8x4(T1, T3);

                pel *p = dst + j * i_dst;
                *(int*)p = _mm_extract_epi32(T1, 0); p += i_dst;
                *(int*)p = _mm_extract_epi32(T1, 1); p += i_dst;
                *(int*)p = _mm_extract_epi32(T1, 2); p += i_dst;
                *(int*)p = _mm_extract_epi32(T1, 3); p += i_dst;
                *(int*)p = _mm_extract_epi32(T3, 0); p += i_dst;
                *(int*)p = _mm_extract_epi32(T3, 1); p += i_dst;
                *(int*)p = _mm_extract_epi32(T3, 2); p += i_dst;
                *(int*)p = _mm_extract_epi32(T3, 3);

                p1 -= 8;
                p2 -= 8;
                p3 -= 8;
                p4 -= 8;
            }
        } else if (iWidth == 12) {
            for (int i = 0; i < iWidth; i++) {
                int offset;
                pel *p = pSrc - getContextPixel(uiDirMode, 1, i + 1, &offset);

                for (int j = 0; j < iHeight; j++, p--) {
                    dst[j*i_dst] = (p[0] * (32 - offset) + p[-1] * (64 - offset) + p[-2] * (32 + offset) + p[-3] * offset + 64) >> 7;
                }
                dst++;
            }
        } else { // iWidth == 8x && iHeight == 8x
            __m128i mSwitch1 = _mm_setr_epi8(7,  8, 6, 7, 5, 6, 4, 5, 3, 4, 2, 3, 1, 2, 0, 1);
            __m128i mSwitch2 = _mm_setr_epi8(9, 10, 8, 9, 7, 8, 6, 7, 5, 6, 4, 5, 3, 4, 2, 3);

            for (int i = 0; i < iWidth; i+=8) {
                int offset1, offset2, offset3, offset4, offset5, offset6, offset7, offset8;
                pel *p1 = pSrc - getContextPixel(uiDirMode, 1, i + 1, &offset1) - 10;
                pel *p2 = pSrc - getContextPixel(uiDirMode, 1, i + 2, &offset2) - 10;
                pel *p3 = pSrc - getContextPixel(uiDirMode, 1, i + 3, &offset3) - 10;
                pel *p4 = pSrc - getContextPixel(uiDirMode, 1, i + 4, &offset4) - 10;
                pel *p5 = pSrc - getContextPixel(uiDirMode, 1, i + 5, &offset5) - 10;
                pel *p6 = pSrc - getContextPixel(uiDirMode, 1, i + 6, &offset6) - 10;
                pel *p7 = pSrc - getContextPixel(uiDirMode, 1, i + 7, &offset7) - 10;
                pel *p8 = pSrc - getContextPixel(uiDirMode, 1, i + 8, &offset8) - 10;

                __m128i C1_1 = _mm_set1_epi16(((32 + offset1) << 8) | offset1);
                __m128i C2_1 = _mm_set1_epi16(((32 + offset2) << 8) | offset2);
                __m128i C3_1 = _mm_set1_epi16(((32 + offset3) << 8) | offset3);
                __m128i C4_1 = _mm_set1_epi16(((32 + offset4) << 8) | offset4);
                __m128i C5_1 = _mm_set1_epi16(((32 + offset5) << 8) | offset5);
                __m128i C6_1 = _mm_set1_epi16(((32 + offset6) << 8) | offset6);
                __m128i C7_1 = _mm_set1_epi16(((32 + offset7) << 8) | offset7);
                __m128i C8_1 = _mm_set1_epi16(((32 + offset8) << 8) | offset8);
                __m128i C1_2 = _mm_set1_epi16(((32 - offset1) << 8) | (64 - offset1));
                __m128i C2_2 = _mm_set1_epi16(((32 - offset2) << 8) | (64 - offset2));
                __m128i C3_2 = _mm_set1_epi16(((32 - offset3) << 8) | (64 - offset3));
                __m128i C4_2 = _mm_set1_epi16(((32 - offset4) << 8) | (64 - offset4));
                __m128i C5_2 = _mm_set1_epi16(((32 - offset5) << 8) | (64 - offset5));
                __m128i C6_2 = _mm_set1_epi16(((32 - offset6) << 8) | (64 - offset6));
                __m128i C7_2 = _mm_set1_epi16(((32 - offset7) << 8) | (64 - offset7));
                __m128i C8_2 = _mm_set1_epi16(((32 - offset8) << 8) | (64 - offset8));

                for (int j = 0; j < iHeight; j += 8) {
                    __m128i mSrc1 = _mm_loadu_si128((__m128i*)p1);
                    __m128i mSrc2 = _mm_loadu_si128((__m128i*)p2);
                    __m128i mSrc3 = _mm_loadu_si128((__m128i*)p3);
                    __m128i mSrc4 = _mm_loadu_si128((__m128i*)p4);
                    __m128i mSrc5 = _mm_loadu_si128((__m128i*)p5);
                    __m128i mSrc6 = _mm_loadu_si128((__m128i*)p6);
                    __m128i mSrc7 = _mm_loadu_si128((__m128i*)p7);
                    __m128i mSrc8 = _mm_loadu_si128((__m128i*)p8);
         
                    __m128i T1 = _mm_add_epi16(_mm_maddubs_epi16(_mm_shuffle_epi8(mSrc1, mSwitch1), C1_1), _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc1, mSwitch2), C1_2));
                    __m128i T2 = _mm_add_epi16(_mm_maddubs_epi16(_mm_shuffle_epi8(mSrc2, mSwitch1), C2_1), _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc2, mSwitch2), C2_2));
                    __m128i T3 = _mm_add_epi16(_mm_maddubs_epi16(_mm_shuffle_epi8(mSrc3, mSwitch1), C3_1), _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc3, mSwitch2), C3_2));
                    __m128i T4 = _mm_add_epi16(_mm_maddubs_epi16(_mm_shuffle_epi8(mSrc4, mSwitch1), C4_1), _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc4, mSwitch2), C4_2));
                    __m128i T5 = _mm_add_epi16(_mm_maddubs_epi16(_mm_shuffle_epi8(mSrc5, mSwitch1), C5_1), _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc5, mSwitch2), C5_2));
                    __m128i T6 = _mm_add_epi16(_mm_maddubs_epi16(_mm_shuffle_epi8(mSrc6, mSwitch1), C6_1), _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc6, mSwitch2), C6_2));
                    __m128i T7 = _mm_add_epi16(_mm_maddubs_epi16(_mm_shuffle_epi8(mSrc7, mSwitch1), C7_1), _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc7, mSwitch2), C7_2));
                    __m128i T8 = _mm_add_epi16(_mm_maddubs_epi16(_mm_shuffle_epi8(mSrc8, mSwitch1), C8_1), _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc8, mSwitch2), C8_2));

                    T1 = _mm_srai_epi16(_mm_add_epi16(T1, mAddOffset), 7);
                    T2 = _mm_srai_epi16(_mm_add_epi16(T2, mAddOffset), 7);
                    T3 = _mm_srai_epi16(_mm_add_epi16(T3, mAddOffset), 7);
                    T4 = _mm_srai_epi16(_mm_add_epi16(T4, mAddOffset), 7);
                    T5 = _mm_srai_epi16(_mm_add_epi16(T5, mAddOffset), 7);
                    T6 = _mm_srai_epi16(_mm_add_epi16(T6, mAddOffset), 7);
                    T7 = _mm_srai_epi16(_mm_add_epi16(T7, mAddOffset), 7);
                    T8 = _mm_srai_epi16(_mm_add_epi16(T8, mAddOffset), 7);

                    T1 = _mm_packus_epi16(T1, T2);
                    T3 = _mm_packus_epi16(T3, T4);
                    T5 = _mm_packus_epi16(T5, T6);
                    T7 = _mm_packus_epi16(T7, T8);

                    TRANS_8x8(T1, T3, T5, T7);

                    pel *p = dst + j * i_dst;
                    _mm_storel_epi64((__m128i *)p, T1); p += i_dst; _mm_storel_epi64((__m128i *)p, _mm_srli_si128(T1, 8)); p += i_dst;
                    _mm_storel_epi64((__m128i *)p, T3); p += i_dst; _mm_storel_epi64((__m128i *)p, _mm_srli_si128(T3, 8)); p += i_dst;
                    _mm_storel_epi64((__m128i *)p, T5); p += i_dst; _mm_storel_epi64((__m128i *)p, _mm_srli_si128(T5, 8)); p += i_dst;
                    _mm_storel_epi64((__m128i *)p, T7); p += i_dst; _mm_storel_epi64((__m128i *)p, _mm_srli_si128(T7, 8));

                    p1 -= 8;
                    p2 -= 8;
                    p3 -= 8;
                    p4 -= 8;
                    p5 -= 8;
                    p6 -= 8;
                    p7 -= 8;
                    p8 -= 8;
                }
                dst += 8;
            }
        }
    }
}