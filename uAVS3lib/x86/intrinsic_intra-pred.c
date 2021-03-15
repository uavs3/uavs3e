#include "intrinsic.h"

#if defined(_MSC_VER) 
#pragma warning(disable: 4100)  // unreferenced formal parameter
#endif

extern tab_char_t tab_log2[65];

static ALIGNED_16(tab_char_t tab_coeff_mode_3[4][16]) = {
        { 8 , 40, 56, 24, 8 , 40, 56, 24, 8 , 40, 56, 24, 8 , 40, 56, 24 },
        { 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16 },
        { 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8  },
        { 32, 64, 32, 0 , 32, 64, 32, 0 , 32, 64, 32, 0 , 32, 64, 32, 0  }
};
static tab_uchar_t tab_idx_mode_3[64] = { 2, 5, 8, 11, 13, 16, 19, 22, 24, 27, 30, 33, 35, 38, 41, 44, 46, 49, 52,
        55, 57, 60, 63, 66, 68, 71, 74, 77, 79, 82, 85, 88, 90, 93, 96, 99, 101, 104, 107, 110, 112, 115, 118, 
        121, 123, 126, 129, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 };

static ALIGNED_16(tab_i16s_t tab_coeff_mode_3_10bit[4][4]) = {
	{ 8, 40, 56, 24},
	{ 16, 48, 48, 16},
	{ 24, 56, 40, 8},
	{ 32, 64, 32, 0}
};
static tab_i16u_t tab_idx_mode_3_10bit[64] = { 2, 5, 8, 11, 13, 16, 19, 22, 24, 27, 30, 33, 35, 38, 41, 44, 46, 49, 52,
55, 57, 60, 63, 66, 68, 71, 74, 77, 79, 82, 85, 88, 90, 93, 96, 99, 101, 104, 107, 110, 112, 115, 118,
121, 123, 126, 129, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 };

static ALIGNED_16(tab_char_t tab_coeff_mode_5[8][16]) = {
        { 20, 52, 44, 12, 20, 52, 44, 12, 20, 52, 44, 12, 20, 52, 44, 12 },
        { 8 , 40, 56, 24, 8 , 40, 56, 24, 8 , 40, 56, 24, 8 , 40, 56, 24 },
        { 28, 60, 36, 4 , 28, 60, 36, 4 , 28, 60, 36, 4 , 28, 60, 36, 4  },
        { 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16 },
        { 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28 },
        { 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8  },
        { 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20 },
        { 32, 64, 32, 0 , 32, 64, 32, 0 , 32, 64, 32, 0 , 32, 64, 32, 0  }
};
static tab_uchar_t tab_idx_mode_5[64] = {
    1, 2, 4, 5, 6, 8, 9, 11, 12, 13, 15, 16, 17, 19, 20, 22, 23, 24, 26, 27, 28, 30, 31, 
    33, 34, 35, 37, 38, 39, 41, 42, 44, 45, 46, 48, 49, 50, 52, 53, 55, 56, 57, 59, 60, 
    61, 63, 64, 66, 67, 68, 70, 71, 72, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 88 };

static ALIGNED_16(tab_i16s_t tab_coeff_mode_5_10bit[8][4]) = {
	{ 20, 52, 44, 12 },
	{ 8, 40, 56, 24 },
	{ 28, 60, 36, 4},
	{ 16, 48, 48, 16},
	{ 4, 36, 60, 28},
	{ 24, 56, 40, 8},
	{ 12, 44, 52, 20},
	{ 32, 64, 32, 0}
};
static tab_i16u_t tab_idx_mode_5_10bit[64] = {
	1, 2, 4, 5, 6, 8, 9, 11, 12, 13, 15, 16, 17, 19, 20, 22, 23, 24, 26, 27, 28, 30, 31,
	33, 34, 35, 37, 38, 39, 41, 42, 44, 45, 46, 48, 49, 50, 52, 53, 55, 56, 57, 59, 60,
	61, 63, 64, 66, 67, 68, 70, 71, 72, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 88 };

static ALIGNED_16(tab_char_t tab_coeff_mode_7[64][16]) = {
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
static tab_uchar_t tab_idx_mode_7[64] = {
    0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 7, 8, 9, 10, 10, 11, 12, 13, 13, 14, 15, 15, 16, 
    17, 18, 18, 19, 20, 21, 21, 22, 23, 23,24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 31, 
    32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 39, 40, 41, 42, 42, 43, 44, 45, 45, 46
};

static ALIGNED_16(tab_i16s_t tab_coeff_mode_7_10bit[64][4]) = {
	{ 9, 41, 55, 23},
	{ 18, 50, 46, 14},
	{ 27, 59, 37, 5},
	{ 3, 35, 61, 29 },
	{ 12, 44, 52, 20},
	{ 21, 53, 43, 11 },
	{ 30, 62, 34, 2},
	{ 6, 38, 58, 26 },
	{ 15, 47, 49, 17 },
	{ 24, 56, 40, 8},
	{ 1, 33, 63, 31},
	{ 9, 41, 55, 23},
	{ 18, 50, 46, 14},
	{ 27, 59, 37, 5},
	{ 4, 36, 60, 28},
	{ 12, 44, 52, 20 },
	{ 21, 53, 43, 11 },
	{ 30, 62, 34, 2 },
	{ 7, 39, 57, 25 },
	{ 15, 47, 49, 17},
	{ 24, 56, 40, 8},
	{ 1, 33, 63, 31 },
	{ 10, 42, 54, 22},
	{ 18, 50, 46, 14},
	{ 27, 59, 37, 5},
	{ 4, 36, 60, 28 },
	{ 13, 45, 51, 19},
	{ 21, 53, 43, 11},
	{ 30, 62, 34, 2 },
	{ 7, 39, 57, 25 },
	{ 16, 48, 48, 16 },
	{ 24, 56, 40, 8 },
	{ 1, 33, 63, 31 },
	{ 10, 42, 54, 22 },
	{ 19, 51, 45, 13 },
	{ 27, 59, 37, 5 },
	{ 4, 36, 60, 28},
	{ 13, 45, 51, 19},
	{ 22, 54, 42, 10},
	{ 30, 62, 34, 2},
	{ 7, 39, 57, 25},
	{ 16, 48, 48, 16  },
	{ 25, 57, 39, 7},
	{ 1, 33, 63, 31},
	{ 10, 42, 54, 22 },
	{ 19, 51, 45, 13 },
	{ 28, 60, 36, 4},
	{ 4, 36, 60, 28},
	{ 13, 45, 51, 19},
	{ 22, 54, 42, 10},
	{ 31, 63, 33, 1 },
	{ 7, 39, 57, 25 },
	{ 16, 48, 48, 16},
	{ 25, 57, 39, 7},
	{ 2, 34, 62, 30 },
	{ 10, 42, 54, 22},
	{ 19, 51, 45, 13},
	{ 28, 60, 36, 4},
	{ 5, 37, 59, 27 },
	{ 13, 45, 51, 19},
	{ 22, 54, 42, 10},
	{ 31, 63, 33, 1 },
	{ 8, 40, 56, 24 },
	{ 16, 48, 48, 16 }
};
static tab_i16u_t tab_idx_mode_7_10bit[64] = {
	0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 7, 8, 9, 10, 10, 11, 12, 13, 13, 14, 15, 15, 16,
	17, 18, 18, 19, 20, 21, 21, 22, 23, 23, 24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 31,
	32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 39, 40, 41, 42, 42, 43, 44, 45, 45, 46
};

static ALIGNED_16(tab_char_t tab_coeff_mode_9[64][16]) = {
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
static tab_uchar_t tab_idx_mode_9[64] = {
    0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 5, 6, 6, 6, 7,7, 7, 8, 8, 9, 9, 
    9, 10, 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 
    16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 21, 21, 21, 22, 22, 22, 23};

static ALIGNED_16(tab_i16s_t tab_coeff_mode_9_10bit[64][4]) = {
	{ 21, 53, 43, 11},
	{ 9, 41, 55, 23},
	{ 30, 62, 34, 2},
	{ 18, 50, 46, 14},
	{ 6, 38, 58, 26 },
	{ 27, 59, 37, 5 },
	{ 15, 47, 49, 17 },
	{ 3, 35, 61, 29 },
	{ 24, 56, 40, 8},
	{ 12, 44, 52, 20},
	{ 1, 33, 63, 31},
	{ 21, 53, 43, 11 },
	{ 9, 41, 55, 23 },
	{ 30, 62, 34, 2 },
	{ 18, 50, 46, 14},
	{ 6, 38, 58, 26 },
	{ 27, 59, 37, 5 },
	{ 15, 47, 49, 17},
	{ 4, 36, 60, 28},
	{ 24, 56, 40, 8},
	{ 12, 44, 52, 20},
	{ 1, 33, 63, 31},
	{ 21, 53, 43, 11},
	{ 9, 41, 55, 23},
	{ 30, 62, 34, 2},
	{ 18, 50, 46, 14},
	{ 7, 39, 57, 25},
	{ 27, 59, 37, 5},
	{ 15, 47, 49, 17},
	{ 4, 36, 60, 28 },
	{ 24, 56, 40, 8 },
	{ 12, 44, 52, 20 },
	{ 1, 33, 63, 31 },
	{ 21, 53, 43, 11},
	{ 10, 42, 54, 22},
	{ 30, 62, 34, 2},
	{ 18, 50, 46, 14},
	{ 7, 39, 57, 25 },
	{ 27, 59, 37, 5 },
	{ 15, 47, 49, 17},
	{ 4, 36, 60, 28},
	{ 24, 56, 40, 8},
	{ 13, 45, 51, 19 },
	{ 1, 33, 63, 31 },
	{ 21, 53, 43, 11 },
	{ 10, 42, 54, 22 },
	{ 30, 62, 34, 2 },
	{ 18, 50, 46, 14 },
	{ 7, 39, 57, 25 },
	{ 27, 59, 37, 5 },
	{ 16, 48, 48, 16},
	{ 4, 36, 60, 28 },
	{ 24, 56, 40, 8 },
	{ 13, 45, 51, 19},
	{ 1, 33, 63, 31 },
	{ 21, 53, 43, 11 },
	{ 10, 42, 54, 22 },
	{ 30, 62, 34, 2},
	{ 19, 51, 45, 13},
	{ 7, 39, 57, 25 },
	{ 27, 59, 37, 5 },
	{ 16, 48, 48, 16 },
	{ 4, 36, 60, 28 },
	{ 24, 56, 40, 8 }
};
static tab_i16u_t tab_idx_mode_9_10bit[64] = {
	0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 9, 9,
	9, 10, 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16,
	16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 21, 21, 21, 22, 22, 22, 23 };

static ALIGNED_16(tab_char_t tab_coeff_mode_11[8][16]) = {
        { 28, 60, 36, 4 , 28, 60, 36, 4 , 28, 60, 36, 4 , 28, 60, 36, 4  },
        { 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8 , 24, 56, 40, 8  },
        { 20, 52, 44, 12, 20, 52, 44, 12, 20, 52, 44, 12, 20, 52, 44, 12 },
        { 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16 },
        { 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20 },
        { 8 , 40, 56, 24, 8 , 40, 56, 24, 8 , 40, 56, 24, 8 , 40, 56, 24 },
        { 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28, 4 , 36, 60, 28 },
        { 32, 64, 32, 0 , 32, 64, 32, 0 , 32, 64, 32, 0 , 32, 64, 32, 0  }
};
static ALIGNED_16(tab_i16s_t tab_coeff_mode_11_10bit[8][16]) = {
	{ 28, 60, 36, 4, 28, 60, 36, 4, 28, 60, 36, 4, 28, 60, 36, 4 },
	{ 24, 56, 40, 8, 24, 56, 40, 8, 24, 56, 40, 8, 24, 56, 40, 8 },
	{ 20, 52, 44, 12, 20, 52, 44, 12, 20, 52, 44, 12, 20, 52, 44, 12 },
	{ 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16, 16, 48, 48, 16 },
	{ 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20, 12, 44, 52, 20 },
	{ 8, 40, 56, 24, 8, 40, 56, 24, 8, 40, 56, 24, 8, 40, 56, 24 },
	{ 4, 36, 60, 28, 4, 36, 60, 28, 4, 36, 60, 28, 4, 36, 60, 28 },
	{ 32, 64, 32, 0, 32, 64, 32, 0, 32, 64, 32, 0, 32, 64, 32, 0 }
};

void xPredIntraAngAdi_X_3_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
    int i, j, k;
    int iWidth2 = iWidth << 1;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;

    if (iWidth & 0x07) {
        for (j = 0; j < iHeight; j++) {
            int real_width;
            int idx = tab_idx_mode_3[j];

            k = j & 0x03;
            real_width = min(iWidth, iWidth2 - idx + 1);

            if (real_width <= 0) {
                pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_3[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_3[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_3[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_3[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);
                _mm_storel_epi64((__m128i*)(dst), D0);
                dst += i_dst;
                j++;

                for (; j < iHeight; j++) {
                    k = j & 0x03;
                    val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_3[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_3[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_3[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_3[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i*)(dst), D0);
                    dst += i_dst;
                }
                break;
            }
            else {
                __m128i D0;
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_3[k]);

                S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
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

                _mm_storel_epi64((__m128i*)(dst), D0);

                if (real_width < iWidth)
                {
                    D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                    _mm_storel_epi64((__m128i*)(dst + real_width), D0);
                }
            }
            dst += i_dst;
        }
    } else if (iWidth & 0x0f) {
        for (j = 0; j < iHeight; j++) {
            int real_width;
            int idx = tab_idx_mode_3[j];

            k = j & 0x03;
            real_width = min(iWidth, iWidth2 - idx + 1);

            if (real_width <= 0) {
                pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_3[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_3[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_3[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_3[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);
                _mm_storel_epi64((__m128i*)(dst), D0);
                dst += i_dst;
                j++;

                for (; j < iHeight; j++) {
                    k = j & 0x03;
                    val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_3[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_3[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_3[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_3[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i*)(dst), D0);
                    dst += i_dst;
                }
                break;
            } else {
                __m128i D0;
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_3[k]);

                S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
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

                _mm_storel_epi64((__m128i*)(dst), D0);
                //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);

                if (real_width < iWidth)
                {
                    D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                    _mm_storel_epi64((__m128i*)(dst + real_width), D0);
                }

            }

            dst += i_dst;
        }
    } else {
        for (j = 0; j < iHeight; j++) {
            int real_width;
            int idx = tab_idx_mode_3[j];

            k = j & 0x03;
            real_width = min(iWidth, iWidth2 - idx + 1);

            if (real_width <= 0) {
                pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_3[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_3[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_3[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_3[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);

                for (i = 0; i < iWidth; i += 16) {
                    _mm_store_si128((__m128i*)(dst + i), D0);
                }
                dst += i_dst;
                j++;

                for (; j < iHeight; j++)
                {
                    k = j & 0x03;
                    val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_3[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_3[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_3[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_3[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    for (i = 0; i < iWidth; i += 16) {
                        _mm_store_si128((__m128i*)(dst + i), D0);
                    }
                    dst += i_dst;
                }
                break;
            } else {
                __m128i D0, D1;

                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_3[k]);
                for (i = 0; i < real_width; i += 16, idx += 16) {
                    S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
                    S1 = _mm_loadu_si128((__m128i*)(pSrc + idx + 1));
                    S2 = _mm_loadu_si128((__m128i*)(pSrc + idx + 2));
                    S3 = _mm_loadu_si128((__m128i*)(pSrc + idx + 3));

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

                    _mm_store_si128((__m128i*)(dst + i), D0);
                    //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
                }

                if (real_width < iWidth)
                {
                    D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                    for (i = real_width; i < iWidth; i += 16) {
                        _mm_storeu_si128((__m128i*)(dst + i), D0);
                        //dst[i] = dst[real_width - 1];
                    }
                }

            }

            dst += i_dst;
        }
    }
}

void xPredIntraAngAdi_X_5_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
    int i, j, k;
    int iWidth2 = iWidth << 1;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;

    if (iWidth == 4) {
        for (j = 0; j < iHeight; j++) {
            int real_width;
            int idx = tab_idx_mode_5[j];

            k = j & 0x07;
            real_width = min(iWidth, iWidth2 - idx + 1);

            if (real_width <= 0) {
                pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_5[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_5[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_5[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);
                _mm_storel_epi64((__m128i*)(dst), D0);
                dst += i_dst;
                j++;

                for (; j < iHeight; j++)
                {
                    k = j & 0x07;
                    val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_5[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_5[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_5[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i*)(dst), D0);
                    dst += i_dst;
                }
                break;
            } else {
                __m128i D0;
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_5[k]);

                S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
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

                _mm_storel_epi64((__m128i*)(dst), D0);

                if (real_width < iWidth) {
                    D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                    _mm_storel_epi64((__m128i*)(dst + real_width), D0);
                }
            }
            dst += i_dst;
        }
    }
    else if (iWidth == 8) {
        for (j = 0; j < iHeight; j++) {
            int real_width;
            int idx = tab_idx_mode_5[j];

            k = j & 0x07;
            real_width = min(iWidth, iWidth2 - idx + 1);

            if (real_width <= 0) {
                pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_5[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_5[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_5[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);
                _mm_storel_epi64((__m128i*)(dst), D0);
                dst += i_dst;
                j++;

                for (; j < iHeight; j++) {
                    k = j & 0x07;
                    val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_5[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_5[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_5[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i*)(dst), D0);
                    dst += i_dst;
                }
                break;
            } else {
                __m128i D0;
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_5[k]);

                S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
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

                _mm_storel_epi64((__m128i*)(dst), D0);
                //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);

                if (real_width < iWidth) {
                    D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                    _mm_storel_epi64((__m128i*)(dst + real_width), D0);
                }

            }

            dst += i_dst;
        }
    }
    else {
        for (j = 0; j < iHeight; j++) {
            int real_width;
            int idx = tab_idx_mode_5[j];
            k = j & 0x07;

            real_width = min(iWidth, iWidth2 - idx + 1);

            if (real_width <= 0) {
                pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_5[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_5[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_5[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);

                for (i = 0; i < iWidth; i += 16) {
                    _mm_store_si128((__m128i*)(dst + i), D0);
                }
                dst += i_dst;
                j++;

                for (; j < iHeight; j++) {
                    k = j & 0x07;
                    val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_5[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_5[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_5[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    for (i = 0; i < iWidth; i += 16) {
                        _mm_store_si128((__m128i*)(dst + i), D0);
                    }
                    dst += i_dst;
                }
                break;
            }
            else {
                __m128i D0, D1;

                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_5[k]);
                for (i = 0; i < real_width; i += 16, idx += 16) {
                    S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
                    S1 = _mm_loadu_si128((__m128i*)(pSrc + idx + 1));
                    S2 = _mm_loadu_si128((__m128i*)(pSrc + idx + 2));
                    S3 = _mm_loadu_si128((__m128i*)(pSrc + idx + 3));

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

                    _mm_store_si128((__m128i*)(dst + i), D0);
                    //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
                }

                if (real_width < iWidth)
                {
                    D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                    for (i = real_width; i < iWidth; i += 16) {
                        _mm_storeu_si128((__m128i*)(dst + i), D0);
                        //dst[i] = dst[real_width - 1];
                    }
                }

            }

            dst += i_dst;
        }
    }
}

void xPredIntraAngAdi_X_7_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
    int i, j;
    int iWidth2 = iWidth << 1;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;

    if (iWidth >= iHeight){
        if (iWidth & 0x07){
            __m128i D0;
            int i_dst2 = i_dst << 1;

            for (j = 0; j < iHeight; j += 2) {
                int idx = tab_idx_mode_7[j];
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_7[j]);

                S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);

                idx = tab_idx_mode_7[j + 1];
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_7[j + 1]);
                S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
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

                _mm_storel_epi64((__m128i*)dst, D0);
                _mm_storel_epi64((__m128i*)(dst + i_dst), _mm_srli_si128(D0, 4));
                //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
                dst += i_dst2;
            }
        }
        else if (iWidth & 0x0f) {
            __m128i D0;

            for (j = 0; j < iHeight; j++) {
                int idx = tab_idx_mode_7[j];
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_7[j]);

                S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
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

                _mm_storel_epi64((__m128i*)(dst), D0);
                //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);

                dst += i_dst;
            }
        }
        else {
            for (j = 0; j < iHeight; j++) {
                __m128i D0, D1;
               
                int idx = tab_idx_mode_7[j];
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_7[j]);

                for (i = 0; i < iWidth; i += 16, idx += 16) {
                    S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
                    S1 = _mm_loadu_si128((__m128i*)(pSrc + idx + 1));
                    S2 = _mm_loadu_si128((__m128i*)(pSrc + idx + 2));
                    S3 = _mm_loadu_si128((__m128i*)(pSrc + idx + 3));

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

                    _mm_store_si128((__m128i*)(dst + i), D0);
                    //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
                }

                dst += i_dst;
            }
        }
    }
    else{
        if (iWidth & 0x07) {
            for (j = 0; j < iHeight; j++) {
                int real_width;
                int idx = tab_idx_mode_7[j];

                real_width = min(iWidth, iWidth2 - idx + 1);

                if (real_width <= 0) {
                    pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_7[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_7[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_7[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i*)(dst), D0);
                    dst += i_dst;
                    j++;

                    for (; j < iHeight; j++)
                    {
                        val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_7[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_7[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_7[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        _mm_storel_epi64((__m128i*)(dst), D0);
                        dst += i_dst;
                    }
                    break;
                }
                else {
                    __m128i D0;
                    c0 = _mm_load_si128((__m128i*)tab_coeff_mode_7[j]);

                    S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
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

                    _mm_storel_epi64((__m128i*)(dst), D0);

                    if (real_width < iWidth)
                    {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        _mm_storel_epi64((__m128i*)(dst + real_width), D0);
                    }
                }
                dst += i_dst;
            }
        }
        else if (iWidth & 0x0f) {
            for (j = 0; j < iHeight; j++) {
                int real_width;
                int idx = tab_idx_mode_7[j];

                real_width = min(iWidth, iWidth2 - idx + 1);

                if (real_width <= 0) {
                    pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_7[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_7[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_7[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i*)(dst), D0);
                    dst += i_dst;
                    j++;

                    for (; j < iHeight; j++) {
                        val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_7[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_7[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_7[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        _mm_storel_epi64((__m128i*)(dst), D0);
                        dst += i_dst;
                    }
                    break;
                }
                else {
                    __m128i D0;
                    c0 = _mm_load_si128((__m128i*)tab_coeff_mode_7[j]);

                    S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
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

                    _mm_storel_epi64((__m128i*)(dst), D0);
                    //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
                    
                    if (real_width < iWidth)
                    {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        _mm_storel_epi64((__m128i*)(dst + real_width), D0);
                    }

                }

                dst += i_dst;
            }
        }
        else {
            for (j = 0; j < iHeight; j++) {
                int real_width;
                int idx = tab_idx_mode_7[j];

                real_width = min(iWidth, iWidth2 - idx + 1);

                if (real_width <= 0) {
                    pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_7[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_7[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_7[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);

                    for (i = 0; i < iWidth; i += 16) {
                        _mm_storeu_si128((__m128i*)(dst + i), D0);
                    }
                    dst += i_dst;
                    j++;

                    for (; j < iHeight; j++) {
                        val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_7[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_7[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_7[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        for (i = 0; i < iWidth; i += 16) {
                            _mm_storeu_si128((__m128i*)(dst + i), D0);
                        }
                        dst += i_dst;
                    }
                    break;
                }
                else {
                    __m128i D0, D1;
                    
                    c0 = _mm_load_si128((__m128i*)tab_coeff_mode_7[j]);
                    for (i = 0; i < real_width; i += 16, idx += 16) {
                        S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
                        S1 = _mm_loadu_si128((__m128i*)(pSrc + idx + 1));
                        S2 = _mm_loadu_si128((__m128i*)(pSrc + idx + 2));
                        S3 = _mm_loadu_si128((__m128i*)(pSrc + idx + 3));

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

                        _mm_store_si128((__m128i*)(dst + i), D0);
                        //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
                    }

                    if (real_width < iWidth)
                    {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        for (i = real_width; i < iWidth; i += 16) {
                            _mm_storeu_si128((__m128i*)(dst + i), D0);
                            //dst[i] = dst[real_width - 1];
                        }
                    }

                }

                dst += i_dst;
            }
        }
    }
}

void xPredIntraAngAdi_X_9_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
    int i, j;
    int iWidth2 = iWidth << 1;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;

    if (iWidth >= iHeight){
        if (iWidth & 0x07){
            __m128i D0;
            int i_dst2 = i_dst << 1;

            for (j = 0; j < iHeight; j += 2) {
                int idx = tab_idx_mode_9[j];
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_9[j]);

                S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);

                idx = tab_idx_mode_9[j + 1];
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_9[j + 1]);
                S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
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

                _mm_storel_epi64((__m128i*)dst, D0);
                _mm_storel_epi64((__m128i*)(dst + i_dst), _mm_srli_si128(D0, 4));
                //_mm_maskmoveu_si128(D0, mask, (char*)(dst + i_dst));
                //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
                dst += i_dst2;
            }
        }
        else if (iWidth & 0x0f) {
            __m128i D0;

            for (j = 0; j < iHeight; j++) {
                int idx = tab_idx_mode_9[j];
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_9[j]);

                S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
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

                _mm_storel_epi64((__m128i*)(dst), D0);
                //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);

                dst += i_dst;
            }
        }
        else {
            for (j = 0; j < iHeight; j++) {
                __m128i D0, D1;

                int idx = tab_idx_mode_9[j];
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_9[j]);

                for (i = 0; i < iWidth; i += 16, idx += 16) {
                    S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
                    S1 = _mm_loadu_si128((__m128i*)(pSrc + idx + 1));
                    S2 = _mm_loadu_si128((__m128i*)(pSrc + idx + 2));
                    S3 = _mm_loadu_si128((__m128i*)(pSrc + idx + 3));

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

                    _mm_store_si128((__m128i*)(dst + i), D0);
                    //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
                }

                dst += i_dst;
            }
        }
    }
    else{
        if (iWidth & 0x07) {
            for (j = 0; j < iHeight; j++) {
                int real_width;
                int idx = tab_idx_mode_9[j];

                real_width = min(iWidth, iWidth2 - idx + 1);

                if (real_width <= 0) {
                    pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_9[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_9[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_9[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i*)(dst), D0);
                    dst += i_dst;
                    j++;

                    for (; j < iHeight; j++)
                    {
                        val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_9[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_9[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_9[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        _mm_storel_epi64((__m128i*)(dst), D0);
                        dst += i_dst;
                    }
                    break;
                }
                else {
                    __m128i D0;
                    c0 = _mm_load_si128((__m128i*)tab_coeff_mode_9[j]);

                    S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
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

                    _mm_storel_epi64((__m128i*)(dst), D0);

                    if (real_width < iWidth)
                    {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        _mm_storel_epi64((__m128i*)(dst + real_width), D0);
                    }
                }
                dst += i_dst;
            }
        }
        else if (iWidth & 0x0f) {
            for (j = 0; j < iHeight; j++) {
                int real_width;
                int idx = tab_idx_mode_9[j];

                real_width = min(iWidth, iWidth2 - idx + 1);

                if (real_width <= 0) {
                    pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_9[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_9[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_9[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i*)(dst), D0);
                    dst += i_dst;
                    j++;

                    for (; j < iHeight; j++)
                    {
                        val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_9[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_9[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_9[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        _mm_storel_epi64((__m128i*)(dst), D0);
                        dst += i_dst;
                    }
                    break;
                }
                else {
                    __m128i D0;
                    c0 = _mm_load_si128((__m128i*)tab_coeff_mode_9[j]);

                    S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
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

                    _mm_storel_epi64((__m128i*)(dst), D0);
                    //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);

                    if (real_width < iWidth)
                    {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        _mm_storel_epi64((__m128i*)(dst + real_width), D0);
                    }

                }

                dst += i_dst;
            }
        }
        else {
            for (j = 0; j < iHeight; j++) {
                int real_width;
                int idx = tab_idx_mode_9[j];

                real_width = min(iWidth, iWidth2 - idx + 1);

                if (real_width <= 0) {
                    pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_9[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_9[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_9[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);

                    for (i = 0; i < iWidth; i += 16) {
                        _mm_storeu_si128((__m128i*)(dst + i), D0);
                    }
                    dst += i_dst;
                    j++;

                    for (; j < iHeight; j++)
                    {
                        val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_9[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_9[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_9[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        for (i = 0; i < iWidth; i += 16) {
                            _mm_storeu_si128((__m128i*)(dst + i), D0);
                        }
                        dst += i_dst;
                    }
                    break;
                }
                else {
                    __m128i D0, D1;

                    c0 = _mm_load_si128((__m128i*)tab_coeff_mode_9[j]);
                    for (i = 0; i < real_width; i += 16, idx += 16) {
                        S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
                        S1 = _mm_loadu_si128((__m128i*)(pSrc + idx + 1));
                        S2 = _mm_loadu_si128((__m128i*)(pSrc + idx + 2));
                        S3 = _mm_loadu_si128((__m128i*)(pSrc + idx + 3));

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

                        _mm_store_si128((__m128i*)(dst + i), D0);
                        //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
                    }

                    if (real_width < iWidth)
                    {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        for (i = real_width; i < iWidth; i += 16) {
                            _mm_storeu_si128((__m128i*)(dst + i), D0);
                            //dst[i] = dst[real_width - 1];
                        }
                    }

                }

                dst += i_dst;
            }
        }
    }
}

void xPredIntraAngAdi_X_11_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
    int i, j, idx;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;

    if (iWidth & 0x07){
        __m128i D0;
        int i_dst2 = i_dst << 1;

        for (j = 0; j < iHeight; j += 2) {
            idx = (j + 1) >> 3;
            c0 = _mm_load_si128((__m128i*)tab_coeff_mode_11[j & 0x07]);

            S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
            S1 = _mm_srli_si128(S0, 1);
            S2 = _mm_srli_si128(S0, 2);
            S3 = _mm_srli_si128(S0, 3);

            t0 = _mm_unpacklo_epi8(S0, S1);
            t1 = _mm_unpacklo_epi8(S2, S3);
            t2 = _mm_unpacklo_epi16(t0, t1);

            t0 = _mm_maddubs_epi16(t2, c0);

            idx = (j + 2) >> 3;
            c0 = _mm_load_si128((__m128i*)tab_coeff_mode_11[(j + 1) & 0x07]);
            S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
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

            _mm_storel_epi64((__m128i*)dst, D0);
            _mm_storel_epi64((__m128i*)(dst + i_dst), _mm_srli_si128(D0, 4));
            //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
            dst += i_dst2;
        }
    }
    else if (iWidth & 0x0f) {
        __m128i D0;

        for (j = 0; j < iHeight; j++) {
            idx = (j + 1) >> 3;
            c0 = _mm_load_si128((__m128i*)tab_coeff_mode_11[j & 0x07]);

            S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
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

            _mm_storel_epi64((__m128i*)(dst), D0);
            //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);

            dst += i_dst;
        }
    }
    else {
        for (j = 0; j < iHeight; j++) {
            __m128i D0, D1;

            idx = (j + 1) >> 3;
            c0 = _mm_load_si128((__m128i*)tab_coeff_mode_11[j & 0x07]);

            for (i = 0; i < iWidth; i += 16, idx += 16) {
                S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
                S1 = _mm_loadu_si128((__m128i*)(pSrc + idx + 1));
                S2 = _mm_loadu_si128((__m128i*)(pSrc + idx + 2));
                S3 = _mm_loadu_si128((__m128i*)(pSrc + idx + 3));

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

                _mm_store_si128((__m128i*)(dst + i), D0);
                //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
            }

            dst += i_dst;
        }
    }
}

void xPredIntraAngAdi_X_4_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[64 + 128]);
	int line_size = iWidth + (iHeight - 1) * 2;
	int real_size = min(line_size, iWidth * 2 - 1);
	int iHeight2 = iHeight * 2;
	int i;
	__m128i zero = _mm_setzero_si128();
	__m128i offset = _mm_set1_epi16(2);

	pSrc += 3;

	for (i = 0; i < real_size - 8; i += 16, pSrc += 16)
	{
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

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

		_mm_store_si128((__m128i*)&first_line[i], sum1);
	}

	if (i < real_size)
	{
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

		__m128i L0 = _mm_unpacklo_epi8(S0, zero);
		__m128i L1 = _mm_unpacklo_epi8(S1, zero);
		__m128i L2 = _mm_unpacklo_epi8(S2, zero);

		__m128i sum1 = _mm_add_epi16(L0, L1);
		__m128i sum2 = _mm_add_epi16(L1, L2);

		sum1 = _mm_add_epi16(sum1, sum2);
		sum1 = _mm_add_epi16(sum1, offset);
		sum1 = _mm_srli_epi16(sum1, 2);

		sum1 = _mm_packus_epi16(sum1, sum1);
		_mm_storel_epi64((__m128i*)&first_line[i], sum1);
	}

	// padding
	for (i = real_size; i < line_size; i += 16) {
        __m128i pad = _mm_set1_epi8((char)first_line[real_size - 1]);
		_mm_storeu_si128((__m128i*)&first_line[i], pad);
	}


	if (iWidth == iHeight || iWidth > 16)
	{
		for (i = 0; i < iHeight2; i += 2) {
			memcpy(dst, first_line + i, iWidth * sizeof(pel_t));
			dst += i_dst;
		}
	}
	else if (iWidth == 16)
	{
		pel_t *dst1 = dst;
		__m128i M = _mm_loadu_si128((__m128i*)&first_line[0]);
		_mm_storel_epi64((__m128i*)dst, M);
		dst += i_dst;
		M = _mm_srli_si128(M, 2);
		_mm_storel_epi64((__m128i*)dst, M);
		dst += i_dst;
		M = _mm_srli_si128(M, 2);
		_mm_storel_epi64((__m128i*)dst, M);
		dst += i_dst;
		M = _mm_srli_si128(M, 2);
		_mm_storel_epi64((__m128i*)dst, M);
		dst = dst1 + 8;
		M = _mm_loadu_si128((__m128i*)&first_line[8]);
		_mm_storel_epi64((__m128i*)dst, M);
		dst += i_dst;
		M = _mm_srli_si128(M, 2);
		_mm_storel_epi64((__m128i*)dst, M);
		dst += i_dst;
		M = _mm_srli_si128(M, 2);
		_mm_storel_epi64((__m128i*)dst, M);
		dst += i_dst;
		M = _mm_srli_si128(M, 2);
		_mm_storel_epi64((__m128i*)dst, M);
	}
	else if (iWidth == 8)
	{
		for (i = 0; i < iHeight2; i += 8)
		{
			__m128i M = _mm_loadu_si128((__m128i*)&first_line[i]);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 2);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 2);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 2);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
		}

	}
	else
	{
		for (i = 0; i < iHeight2; i += 8)
		{
			__m128i M = _mm_loadu_si128((__m128i*)&first_line[i]);
			((int*)(dst))[0] = _mm_cvtsi128_si32(M);
			dst += i_dst;
			M = _mm_srli_si128(M, 2);
			((int*)(dst))[0] = _mm_cvtsi128_si32(M);
			dst += i_dst;
			M = _mm_srli_si128(M, 2);
			((int*)(dst))[0] = _mm_cvtsi128_si32(M);
			dst += i_dst;
			M = _mm_srli_si128(M, 2);
			((int*)(dst))[0] = _mm_cvtsi128_si32(M);
			dst += i_dst;
		}
	}
}

void xPredIntraAngAdi_X_6_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[64 + 64]);
	int line_size = iWidth + iHeight - 1;
	int real_size = min(line_size, iWidth * 2);
	int i;
	__m128i zero = _mm_setzero_si128();
	__m128i offset = _mm_set1_epi16(2);
	pSrc += 2;

	for (i = 0; i < real_size - 8; i += 16, pSrc += 16)
	{
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

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

		_mm_store_si128((__m128i*)&first_line[i], sum1);
	}

	if (i < real_size)
	{
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

		__m128i L0 = _mm_unpacklo_epi8(S0, zero);
		__m128i L1 = _mm_unpacklo_epi8(S1, zero);
		__m128i L2 = _mm_unpacklo_epi8(S2, zero);

		__m128i sum1 = _mm_add_epi16(L0, L1);
		__m128i sum2 = _mm_add_epi16(L1, L2);

		sum1 = _mm_add_epi16(sum1, sum2);
		sum1 = _mm_add_epi16(sum1, offset);
		sum1 = _mm_srli_epi16(sum1, 2);

		sum1 = _mm_packus_epi16(sum1, sum1);
		_mm_storel_epi64((__m128i*)&first_line[i], sum1);
	}

	// padding
	for (i = real_size; i < line_size; i += 16) {
        __m128i pad = _mm_set1_epi8((char)first_line[real_size - 1]);
		_mm_storeu_si128((__m128i*)&first_line[i], pad);
	}

	if (iWidth > 16 || iWidth == 4)
	{
		for (i = 0; i < iHeight; i++) {
			memcpy(dst, first_line + i, iWidth * sizeof(pel_t));
			dst += i_dst;
		}
	}
	else if (iWidth == 16)
	{
		pel_t *dst1 = dst;
		pel_t *dst2;
		if (iHeight == 4)
		{
			__m128i M = _mm_loadu_si128((__m128i*)&first_line[0]);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 = dst + 8;
			M = _mm_loadu_si128((__m128i*)&first_line[8]);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
		}
		else
		{
			__m128i M = _mm_loadu_si128((__m128i*)&first_line[0]);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst2 = dst1 + i_dst;
			dst1 = dst + 8;
			M = _mm_loadu_si128((__m128i*)&first_line[8]);
			_mm_storel_epi64((__m128i*)dst1, M);
			_mm_storel_epi64((__m128i*)dst2, M);
			dst1 += i_dst;
			dst2 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			_mm_storel_epi64((__m128i*)dst2, M);
			dst1 += i_dst;
			dst2 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			_mm_storel_epi64((__m128i*)dst2, M);
			dst1 += i_dst;
			dst2 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			_mm_storel_epi64((__m128i*)dst2, M);
			dst1 += i_dst;
			dst2 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			_mm_storel_epi64((__m128i*)dst2, M);
			dst1 += i_dst;
			dst2 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			_mm_storel_epi64((__m128i*)dst2, M);
			dst1 += i_dst;
			dst2 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			_mm_storel_epi64((__m128i*)dst2, M);
			dst1 += i_dst;
			dst2 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			_mm_storel_epi64((__m128i*)dst2, M);
			dst1 += i_dst;
			M = _mm_loadu_si128((__m128i*)&first_line[16]);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
		}
	}
	else
	{
		for (i = 0; i < iHeight; i += 8)
		{
			__m128i M = _mm_loadu_si128((__m128i*)&first_line[i]);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
		}
	}
}

void xPredIntraAngAdi_X_8_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[2 * (64 + 48)]);
	int line_size = iWidth + iHeight / 2 - 1;
	int real_size = min(line_size, iWidth * 2 + 1);
	int i;
	__m128i pad1, pad2;
	int aligned_line_size = ((line_size + 31) >> 4) << 4;
    pel_t *pfirst[2];

	__m128i zero = _mm_setzero_si128();
	__m128i coeff = _mm_set1_epi16(3);
	__m128i offset1 = _mm_set1_epi16(4);
	__m128i offset2 = _mm_set1_epi16(2);
	int i_dst2 = i_dst * 2;

    pfirst[0] = first_line;
    pfirst[1] = first_line + aligned_line_size;

	for (i = 0; i < real_size - 8; i += 16, pSrc += 16)
	{
		__m128i p01, p02, p11, p12;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
		__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 3));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 2));

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
		_mm_store_si128((__m128i*)&pfirst[0][i], p01);

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
		_mm_store_si128((__m128i*)&pfirst[1][i], p01);
	}

	if (i < real_size)
	{
		__m128i p01, p02;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
		__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 3));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 2));

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
		_mm_storel_epi64((__m128i*)&pfirst[0][i], p01);

		p01 = _mm_add_epi16(L1, L2);
		p02 = _mm_add_epi16(L2, L3);

		p01 = _mm_add_epi16(p01, p02);
		p01 = _mm_add_epi16(p01, offset2);
		p01 = _mm_srli_epi16(p01, 2);

		p01 = _mm_packus_epi16(p01, p01);
		_mm_storel_epi64((__m128i*)&pfirst[1][i], p01);
	}

	// padding
	if (real_size < line_size) {
		pfirst[1][real_size - 1] = pfirst[1][real_size - 2];

        pad1 = _mm_set1_epi8((char)pfirst[0][real_size - 1]);
        pad2 = _mm_set1_epi8((char)pfirst[1][real_size - 1]);
		for (i = real_size; i < line_size; i += 16) {
			_mm_storeu_si128((__m128i*)&pfirst[0][i], pad1);
			_mm_storeu_si128((__m128i*)&pfirst[1][i], pad2);
		}
	}

	iHeight /= 2;

	if (iWidth != 8)
	{
		for (i = 0; i < iHeight; i++) {
			memcpy(dst, pfirst[0] + i, iWidth * sizeof(pel_t));
			memcpy(dst + i_dst, pfirst[1] + i, iWidth * sizeof(pel_t));
			dst += i_dst2;
		}
	}
	else
	{
		if (iHeight == 4)
		{
			__m128i M1 = _mm_loadu_si128((__m128i*)&pfirst[0][0]);
			__m128i M2 = _mm_loadu_si128((__m128i*)&pfirst[1][0]);
			_mm_storel_epi64((__m128i*)dst, M1);
			_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
			dst += i_dst2;
			M1 = _mm_srli_si128(M1, 1);
			M2 = _mm_srli_si128(M2, 1);
			_mm_storel_epi64((__m128i*)dst, M1);
			_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
			dst += i_dst2;
			M1 = _mm_srli_si128(M1, 1);
			M2 = _mm_srli_si128(M2, 1);
			_mm_storel_epi64((__m128i*)dst, M1);
			_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
			dst += i_dst2;
			M1 = _mm_srli_si128(M1, 1);
			M2 = _mm_srli_si128(M2, 1);
			_mm_storel_epi64((__m128i*)dst, M1);
			_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
		}
		else
		{
			for (i = 0; i < 16; i = i + 8)
			{
				__m128i M1 = _mm_loadu_si128((__m128i*)&pfirst[0][i]);
				__m128i M2 = _mm_loadu_si128((__m128i*)&pfirst[1][i]);
				_mm_storel_epi64((__m128i*)dst, M1);
				_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
				dst += i_dst2;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				_mm_storel_epi64((__m128i*)dst, M1);
				_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
				dst += i_dst2;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				_mm_storel_epi64((__m128i*)dst, M1);
				_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
				dst += i_dst2;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				_mm_storel_epi64((__m128i*)dst, M1);
				_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
				dst += i_dst2;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				_mm_storel_epi64((__m128i*)dst, M1);
				_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
				dst += i_dst2;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				_mm_storel_epi64((__m128i*)dst, M1);
				_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
				dst += i_dst2;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				_mm_storel_epi64((__m128i*)dst, M1);
				_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
				dst += i_dst2;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				_mm_storel_epi64((__m128i*)dst, M1);
				_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
				dst += i_dst2;
			}
		}
	}
}

void xPredIntraAngAdi_X_10_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	int i;
	pel_t *dst1 = dst;
	pel_t *dst2 = dst1 + i_dst;
	pel_t *dst3 = dst2 + i_dst;
	pel_t *dst4 = dst3 + i_dst;
	__m128i zero = _mm_setzero_si128();
	__m128i coeff2 = _mm_set1_epi16(2);
	__m128i coeff3 = _mm_set1_epi16(3);
	__m128i coeff4 = _mm_set1_epi16(4);
	__m128i coeff5 = _mm_set1_epi16(5);
	__m128i coeff7 = _mm_set1_epi16(7);
	__m128i coeff8 = _mm_set1_epi16(8);

	if (iHeight != 4) {
		ALIGNED_16(pel_t first_line[4 * (64 + 32)]);
		int line_size = iWidth + iHeight / 4 - 1;
		int aligned_line_size = ((line_size + 31) >> 4) << 4;
        pel_t *pfirst[4];

        pfirst[0] = first_line;
        pfirst[1] = first_line + aligned_line_size;
        pfirst[2] = first_line + aligned_line_size * 2;
        pfirst[3] = first_line + aligned_line_size * 3;

		for (i = 0; i < line_size - 8; i += 16, pSrc += 16)
		{
			__m128i p00, p10, p20, p30;
			__m128i p01, p11, p21, p31;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 3));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc + 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 2));

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
			_mm_store_si128((__m128i*)&pfirst[0][i], p00);

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
			_mm_store_si128((__m128i*)&pfirst[1][i], p00);

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
			_mm_store_si128((__m128i*)&pfirst[2][i], p00);

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
			_mm_store_si128((__m128i*)&pfirst[3][i], p00);
		}

		if (i < line_size)
		{
			__m128i p00, p10, p20, p30;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 3));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc + 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 2));

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
			_mm_storel_epi64((__m128i*)&pfirst[0][i], p00);

			p00 = _mm_add_epi16(L1, L2);
			p00 = _mm_mullo_epi16(p00, coeff3);
			p10 = _mm_add_epi16(L0, L3);
			p10 = _mm_add_epi16(p10, coeff4);
			p00 = _mm_add_epi16(p10, p00);
			p00 = _mm_srli_epi16(p00, 3);

			p00 = _mm_packus_epi16(p00, p00);
			_mm_storel_epi64((__m128i*)&pfirst[1][i], p00);

			p10 = _mm_mullo_epi16(L1, coeff5);
			p20 = _mm_mullo_epi16(L2, coeff7);
			p30 = _mm_mullo_epi16(L3, coeff3);
			p00 = _mm_add_epi16(L0, coeff8);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_srli_epi16(p00, 4);

			p00 = _mm_packus_epi16(p00, p00);
			_mm_storel_epi64((__m128i*)&pfirst[2][i], p00);

			p00 = _mm_add_epi16(L1, L2);
			p10 = _mm_add_epi16(L2, L3);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, coeff2);
			p00 = _mm_srli_epi16(p00, 2);

			p00 = _mm_packus_epi16(p00, p00);
			_mm_storel_epi64((__m128i*)&pfirst[3][i], p00);
		}

		iHeight >>= 2;

		if (iWidth != 8) {
            int i_dstx4 = i_dst << 2;
            switch (iWidth) {
            case 4:
                for (i = 0; i < iHeight; i++) {
                    CP32(dst1, pfirst[0] + i); dst1 += i_dstx4;
                    CP32(dst2, pfirst[1] + i); dst2 += i_dstx4;
                    CP32(dst3, pfirst[2] + i); dst3 += i_dstx4;
                    CP32(dst4, pfirst[3] + i); dst4 += i_dstx4;
                }
                break;
            case 16:
                for (i = 0; i < iHeight; i++) {
                    CP128(dst1, pfirst[0] + i); dst1 += i_dstx4;
                    CP128(dst2, pfirst[1] + i); dst2 += i_dstx4;
                    CP128(dst3, pfirst[2] + i); dst3 += i_dstx4;
                    CP128(dst4, pfirst[3] + i); dst4 += i_dstx4;
                }
                break;
            case 32:
                for (i = 0; i < iHeight; i++) {
                    memcpy(dst1, pfirst[0] + i, 32 * sizeof(pel_t)); dst1 += i_dstx4;
                    memcpy(dst2, pfirst[1] + i, 32 * sizeof(pel_t)); dst2 += i_dstx4;
                    memcpy(dst3, pfirst[2] + i, 32 * sizeof(pel_t)); dst3 += i_dstx4;
                    memcpy(dst4, pfirst[3] + i, 32 * sizeof(pel_t)); dst4 += i_dstx4;
                }
                break;
            case 64:
                for (i = 0; i < iHeight; i++) {
                    memcpy(dst1, pfirst[0] + i, 64 * sizeof(pel_t)); dst1 += i_dstx4;
                    memcpy(dst2, pfirst[1] + i, 64 * sizeof(pel_t)); dst2 += i_dstx4;
                    memcpy(dst3, pfirst[2] + i, 64 * sizeof(pel_t)); dst3 += i_dstx4;
                    memcpy(dst4, pfirst[3] + i, 64 * sizeof(pel_t)); dst4 += i_dstx4;
                }
                break;
            default:
                assert(0);
                break;
            }
			
		} else {
            if (iHeight == 2) {
                for (i = 0; i < iHeight; i++) {
                    CP64(dst1, pfirst[0] + i);
                    CP64(dst2, pfirst[1] + i);
                    CP64(dst3, pfirst[2] + i);
                    CP64(dst4, pfirst[3] + i);
                    dst1 = dst4 + i_dst;
                    dst2 = dst1 + i_dst;
                    dst3 = dst2 + i_dst;
                    dst4 = dst3 + i_dst;
                }
            } else {
				__m128i M1 = _mm_loadu_si128((__m128i*)&pfirst[0][0]);
				__m128i M2 = _mm_loadu_si128((__m128i*)&pfirst[1][0]);
				__m128i M3 = _mm_loadu_si128((__m128i*)&pfirst[2][0]);
				__m128i M4 = _mm_loadu_si128((__m128i*)&pfirst[3][0]);
				_mm_storel_epi64((__m128i*)dst1, M1);
				_mm_storel_epi64((__m128i*)dst2, M2);
				_mm_storel_epi64((__m128i*)dst3, M3);
				_mm_storel_epi64((__m128i*)dst4, M4);
				dst1 = dst4 + i_dst;
				dst2 = dst1 + i_dst;
				dst3 = dst2 + i_dst;
				dst4 = dst3 + i_dst;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				M3 = _mm_srli_si128(M3, 1);
				M4 = _mm_srli_si128(M4, 1);
				_mm_storel_epi64((__m128i*)dst1, M1);
				_mm_storel_epi64((__m128i*)dst2, M2);
				_mm_storel_epi64((__m128i*)dst3, M3);
				_mm_storel_epi64((__m128i*)dst4, M4);
				dst1 = dst4 + i_dst;
				dst2 = dst1 + i_dst;
				dst3 = dst2 + i_dst;
				dst4 = dst3 + i_dst;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				M3 = _mm_srli_si128(M3, 1);
				M4 = _mm_srli_si128(M4, 1);
				_mm_storel_epi64((__m128i*)dst1, M1);
				_mm_storel_epi64((__m128i*)dst2, M2);
				_mm_storel_epi64((__m128i*)dst3, M3);
				_mm_storel_epi64((__m128i*)dst4, M4);
				dst1 = dst4 + i_dst;
				dst2 = dst1 + i_dst;
				dst3 = dst2 + i_dst;
				dst4 = dst3 + i_dst;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				M3 = _mm_srli_si128(M3, 1);
				M4 = _mm_srli_si128(M4, 1);
				_mm_storel_epi64((__m128i*)dst1, M1);
				_mm_storel_epi64((__m128i*)dst2, M2);
				_mm_storel_epi64((__m128i*)dst3, M3);
				_mm_storel_epi64((__m128i*)dst4, M4);
				dst1 = dst4 + i_dst;
				dst2 = dst1 + i_dst;
				dst3 = dst2 + i_dst;
				dst4 = dst3 + i_dst;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				M3 = _mm_srli_si128(M3, 1);
				M4 = _mm_srli_si128(M4, 1);
				_mm_storel_epi64((__m128i*)dst1, M1);
				_mm_storel_epi64((__m128i*)dst2, M2);
				_mm_storel_epi64((__m128i*)dst3, M3);
				_mm_storel_epi64((__m128i*)dst4, M4);
				dst1 = dst4 + i_dst;
				dst2 = dst1 + i_dst;
				dst3 = dst2 + i_dst;
				dst4 = dst3 + i_dst;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				M3 = _mm_srli_si128(M3, 1);
				M4 = _mm_srli_si128(M4, 1);
				_mm_storel_epi64((__m128i*)dst1, M1);
				_mm_storel_epi64((__m128i*)dst2, M2);
				_mm_storel_epi64((__m128i*)dst3, M3);
				_mm_storel_epi64((__m128i*)dst4, M4);
				dst1 = dst4 + i_dst;
				dst2 = dst1 + i_dst;
				dst3 = dst2 + i_dst;
				dst4 = dst3 + i_dst;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				M3 = _mm_srli_si128(M3, 1);
				M4 = _mm_srli_si128(M4, 1);
				_mm_storel_epi64((__m128i*)dst1, M1);
				_mm_storel_epi64((__m128i*)dst2, M2);
				_mm_storel_epi64((__m128i*)dst3, M3);
				_mm_storel_epi64((__m128i*)dst4, M4);
				dst1 = dst4 + i_dst;
				dst2 = dst1 + i_dst;
				dst3 = dst2 + i_dst;
				dst4 = dst3 + i_dst;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				M3 = _mm_srli_si128(M3, 1);
				M4 = _mm_srli_si128(M4, 1);
				_mm_storel_epi64((__m128i*)dst1, M1);
				_mm_storel_epi64((__m128i*)dst2, M2);
				_mm_storel_epi64((__m128i*)dst3, M3);
				_mm_storel_epi64((__m128i*)dst4, M4);
			}
		}
	}
	else
	{
		if (iWidth == 16)
		{
			__m128i p00, p10, p20, p30;
			__m128i p01, p11, p21, p31;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 3));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc + 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 2));

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
			_mm_store_si128((__m128i*)dst1, p00);

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
			_mm_store_si128((__m128i*)dst2, p00);

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
			_mm_store_si128((__m128i*)dst3, p00);

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
			_mm_store_si128((__m128i*)dst4, p00);
		}
		else
		{
			__m128i p00, p10, p20, p30;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 3));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc + 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 2));

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
			((int*)(dst1))[0] = _mm_cvtsi128_si32(p00);

			p00 = _mm_add_epi16(L1, L2);
			p00 = _mm_mullo_epi16(p00, coeff3);
			p10 = _mm_add_epi16(L0, L3);
			p10 = _mm_add_epi16(p10, coeff4);
			p00 = _mm_add_epi16(p10, p00);
			p00 = _mm_srli_epi16(p00, 3);

			p00 = _mm_packus_epi16(p00, p00);
			((int*)(dst2))[0] = _mm_cvtsi128_si32(p00);

			p10 = _mm_mullo_epi16(L1, coeff5);
			p20 = _mm_mullo_epi16(L2, coeff7);
			p30 = _mm_mullo_epi16(L3, coeff3);
			p00 = _mm_add_epi16(L0, coeff8);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_srli_epi16(p00, 4);

			p00 = _mm_packus_epi16(p00, p00);
			((int*)(dst3))[0] = _mm_cvtsi128_si32(p00);

			p00 = _mm_add_epi16(L1, L2);
			p10 = _mm_add_epi16(L2, L3);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, coeff2);
			p00 = _mm_srli_epi16(p00, 2);

			p00 = _mm_packus_epi16(p00, p00);
			((int*)(dst4))[0] = _mm_cvtsi128_si32(p00);
		}
	}
}

static __inline void transpose_4x4(pel_t *dst){
    __m128i t0, t1, t2, t3;

    t0 = _mm_load_si128((__m128i *)dst);
    t1 = _mm_srli_si128(t0, 4);
    t2 = _mm_srli_si128(t0, 8);
    t3 = _mm_srli_si128(t0, 12);

    t0 = _mm_unpacklo_epi8(t0, t1);
    t2 = _mm_unpacklo_epi8(t2, t3);

    t0 = _mm_unpacklo_epi16(t0, t2);
    _mm_store_si128((__m128i *)dst, t0);
}
static __inline void transpose_4x16(pel_t *dst){
//  srcStride = dstStride = 16
    __m128i s0, s1, s2, s3;
    __m128i t0, t1, t2, t3;
    pel_t *pDst = dst;

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
    
    dst = pDst;
    _mm_store_si128((__m128i *)dst, t0);
    _mm_store_si128((__m128i *)(dst + 16), t1);
    _mm_store_si128((__m128i *)(dst + 32), t2);
    _mm_store_si128((__m128i *)(dst + 48), t3);
}

static __inline void transpose_8x8_stride(pel_t *src, int i_src, pel_t *dst, int i_dst){
    __m128i t0, t1, t2, t3, t4, t5, t6, t7;
    pel_t* pDst = src;

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
static __inline void transpose_8x8(pel_t *dst){
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

    _mm_store_si128((__m128i *)dst, t0);
    _mm_store_si128((__m128i *)(dst + 16), t2);
    _mm_store_si128((__m128i *)(dst + 32), t4);
    _mm_store_si128((__m128i *)(dst + 48), t6);
}
static __inline void transpose_8x32(pel_t *dst){
    transpose_8x8_stride(dst, 32, dst, 32);
    transpose_8x8_stride(dst + 8 * 32, 32, dst + 8, 32);
    transpose_8x8_stride(dst + 16 * 32, 32, dst + 16, 32);
    transpose_8x8_stride(dst + 24 * 32, 32, dst + 24, 32);
}

static __inline void transpose_16x16(pel_t *dst){
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

    _mm_store_si128((__m128i *)dst, r0);
    _mm_store_si128((__m128i *)(dst + 16), r1);
    _mm_store_si128((__m128i *)(dst + 16 * 2), r2);
    _mm_store_si128((__m128i *)(dst + 16 * 3), r3);
    _mm_store_si128((__m128i *)(dst + 16 * 4), r4);
    _mm_store_si128((__m128i *)(dst + 16 * 5), r5);
    _mm_store_si128((__m128i *)(dst + 16 * 6), r6);
    _mm_store_si128((__m128i *)(dst + 16 * 7), r7);
    _mm_store_si128((__m128i *)(dst + 16 * 8), r8);
    _mm_store_si128((__m128i *)(dst + 16 * 9), r9);
    _mm_store_si128((__m128i *)(dst + 16 * 10), r10);
    _mm_store_si128((__m128i *)(dst + 16 * 11), r11);
    _mm_store_si128((__m128i *)(dst + 16 * 12), r12);
    _mm_store_si128((__m128i *)(dst + 16 * 13), r13);
    _mm_store_si128((__m128i *)(dst + 16 * 14), r14);
    _mm_store_si128((__m128i *)(dst + 16 * 15), r15);
}
static __inline void transpose_16x4(pel_t *dst){
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

    _mm_store_si128((__m128i *)(dst), r0);
    _mm_store_si128((__m128i *)(dst + 16), r1);
    _mm_store_si128((__m128i *)(dst + 32), r2);
    _mm_store_si128((__m128i *)(dst + 48), r3);
}

static __inline void transpose_32x32_stride(pel_t *src, int i_src, pel_t *dst, int i_dst)
{
    __m128i r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31;
    __m128i t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15;

    pel_t *pDst = src;

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
static __inline void transpose_32x8(pel_t *dst){
    __m128i r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15;
    __m128i t0, t1, t2, t3, t4, t5, t6, t7;
    r0  = _mm_load_si128((__m128i *)dst);
    r8  = _mm_load_si128((__m128i *)(dst + 16));
    r1  = _mm_load_si128((__m128i *)(dst + 16*2));
    r9  = _mm_load_si128((__m128i *)(dst + 16*3));
    r2  = _mm_load_si128((__m128i *)(dst + 16*4));
    r10 = _mm_load_si128((__m128i *)(dst + 16*5));
    r3  = _mm_load_si128((__m128i *)(dst + 16*6));
    r11 = _mm_load_si128((__m128i *)(dst + 16*7));
    r4  = _mm_load_si128((__m128i *)(dst + 16*8));
    r12 = _mm_load_si128((__m128i *)(dst + 16*9));
    r5  = _mm_load_si128((__m128i *)(dst + 16*10));
    r13 = _mm_load_si128((__m128i *)(dst + 16*11));
    r6  = _mm_load_si128((__m128i *)(dst + 16*12));
    r14 = _mm_load_si128((__m128i *)(dst + 16*13));
    r7  = _mm_load_si128((__m128i *)(dst + 16*14));
    r15 = _mm_load_si128((__m128i *)(dst + 16*15));
    
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
    _mm_store_si128((__m128i *)(dst), t0);
    _mm_store_si128((__m128i *)(dst + 16), t1);
    _mm_store_si128((__m128i *)(dst + 16*2), t2);
    _mm_store_si128((__m128i *)(dst + 16*3), t3);
    _mm_store_si128((__m128i *)(dst + 16*4), t4);
    _mm_store_si128((__m128i *)(dst + 16*5), t5);
    _mm_store_si128((__m128i *)(dst + 16*6), t6);
    _mm_store_si128((__m128i *)(dst + 16*7), t7);

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

    dst += 16 * 8;
    _mm_store_si128((__m128i *)(dst), t0);
    _mm_store_si128((__m128i *)(dst + 16), t1);
    _mm_store_si128((__m128i *)(dst + 16 * 2), t2);
    _mm_store_si128((__m128i *)(dst + 16 * 3), t3);
    _mm_store_si128((__m128i *)(dst + 16 * 4), t4);
    _mm_store_si128((__m128i *)(dst + 16 * 5), t5);
    _mm_store_si128((__m128i *)(dst + 16 * 6), t6);
    _mm_store_si128((__m128i *)(dst + 16 * 7), t7);
}

static __inline void transpose_64x64(pel_t *dst){
    ALIGNED_16(pel_t tmp[32*32]);
    __m128i t0, t1;
    pel_t *pTmp = tmp;
    int i;
    transpose_32x32_stride(dst, 64, dst, 64);
    transpose_32x32_stride(dst + 32, 64, tmp, 32);
    transpose_32x32_stride(dst + (64 << 5), 64, dst + 32, 64);
    dst += (64 << 5);
    transpose_32x32_stride(dst + 32, 64, dst + 32, 64);
    for (i = 0; i < 32; i++)
    {
        t0 = _mm_load_si128((__m128i*)pTmp);
        t1 = _mm_load_si128((__m128i*)(pTmp + 16));
        _mm_store_si128((__m128i*)(dst), t0);
        _mm_store_si128((__m128i*)(dst + 16), t1);
        pTmp += 32;
        dst += 64;
    }
}

void xPredIntraAngAdi_Y_25_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
    int i, j, idx;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;
    int height = iWidth;
    int width = iHeight;
    pel_t *pDst = dst;

    i_dst = max(height, width);
    pSrc += 144;
    if (width == 4){
        __m128i D0;
        int i_dst2 = i_dst << 1;

        for (j = 0; j < height; j += 2) {
            idx = (j + 1) >> 3;
            c0 = _mm_load_si128((__m128i*)tab_coeff_mode_11[j & 0x07]);

            S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
            S1 = _mm_srli_si128(S0, 1);
            S2 = _mm_srli_si128(S0, 2);
            S3 = _mm_srli_si128(S0, 3);

            t0 = _mm_unpacklo_epi8(S0, S1);
            t1 = _mm_unpacklo_epi8(S2, S3);
            t2 = _mm_unpacklo_epi16(t0, t1);

            t0 = _mm_maddubs_epi16(t2, c0);

            idx = (j + 2) >> 3;
            c0 = _mm_load_si128((__m128i*)tab_coeff_mode_11[(j + 1) & 0x07]);
            S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
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

            _mm_storel_epi64((__m128i*)dst, D0);
            _mm_storel_epi64((__m128i*)(dst + i_dst), _mm_srli_si128(D0, 4));
            //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
            dst += i_dst2;
        }
        if (height == 4) {
            transpose_4x4(pDst);
        }
        else{
            transpose_4x16(pDst);
        }
    }
    else if (width == 8) {
        __m128i D0;

        for (j = 0; j < height; j++) {
            idx = (j + 1) >> 3;
            c0 = _mm_load_si128((__m128i*)tab_coeff_mode_11[j & 0x07]);

            S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
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

            _mm_storel_epi64((__m128i*)(dst), D0);
            //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);

            dst += i_dst;
        }
        if (height == 8) {
            transpose_8x8(pDst);
        }
        else{
            transpose_8x32(pDst);
        }
    }
    else{
        for (j = 0; j < height; j++) {
            __m128i D0, D1;

            idx = (j + 1) >> 3;
            c0 = _mm_load_si128((__m128i*)tab_coeff_mode_11[j & 0x07]);

            for (i = 0; i < width; i += 16, idx += 16) {
                S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
                S1 = _mm_loadu_si128((__m128i*)(pSrc + idx + 1));
                S2 = _mm_loadu_si128((__m128i*)(pSrc + idx + 2));
                S3 = _mm_loadu_si128((__m128i*)(pSrc + idx + 3));

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

                _mm_store_si128((__m128i*)(dst + i), D0);
                //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
            }

            dst += i_dst;
        }
        if (width == 16) {
            if (height == 16)
                transpose_16x16(pDst);
            else
                transpose_16x4(pDst);
        }
        else if (width == 32) {
            if (height == 32)
                transpose_32x32_stride(pDst, i_dst, pDst, i_dst);
            else
                transpose_32x8(pDst);
        }
        else{
            transpose_64x64(pDst);
        }
    }
}

void xPredIntraAngAdi_Y_27_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
    int i, j;
    pel_t *pDst = dst;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;

    pSrc += 144;
    if (iWidth == iHeight)
    {
        if (iHeight == 4){
            __m128i D0;
            int i_dst2 = i_dst << 1;

            for (j = 0; j < iWidth; j += 2) {
                int idx = tab_idx_mode_9[j];
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_9[j]);

                S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);

                idx = tab_idx_mode_9[j + 1];
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_9[j + 1]);
                S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
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

                _mm_storel_epi64((__m128i*)dst, D0);
                _mm_storel_epi64((__m128i*)(dst + i_dst), _mm_srli_si128(D0, 4));
                //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
                dst += i_dst2;
            }
            transpose_4x4(pDst);
        }
        else if (iHeight == 8) {
            __m128i D0;

            for (j = 0; j < iWidth; j++) {
                int idx = tab_idx_mode_9[j];
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_9[j]);

                S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
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

                _mm_storel_epi64((__m128i*)(dst), D0);
                //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);

                dst += i_dst;
            }
            transpose_8x8(pDst);
        }
        else {
            for (j = 0; j < iWidth; j++) {
                __m128i D0, D1;

                int idx = tab_idx_mode_9[j];
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_9[j]);

                for (i = 0; i < iHeight; i += 16, idx += 16) {
                    S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
                    S1 = _mm_loadu_si128((__m128i*)(pSrc + idx + 1));
                    S2 = _mm_loadu_si128((__m128i*)(pSrc + idx + 2));
                    S3 = _mm_loadu_si128((__m128i*)(pSrc + idx + 3));

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

                    _mm_store_si128((__m128i*)(dst + i), D0);
                    //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
                }

                dst += i_dst;
            }
            if (iHeight == 16) {
                transpose_16x16(pDst);
            }
            else if (iHeight == 32) {
                transpose_32x32_stride(pDst, i_dst, pDst, i_dst);
            }
            else{
                transpose_64x64(pDst);
            }
        }
    }
    else{
        i_dst = max(iWidth, iHeight);
        if (iHeight == 4) {
            for (j = 0; j < iWidth; j++) {
                int real_width;
                int idx = tab_idx_mode_9[j];

                real_width = min(4, 8 - idx + 1);

                if (real_width <= 0) {
                    pel_t val = (pel_t)((pSrc[8] * tab_coeff_mode_9[j][0] + pSrc[9] * tab_coeff_mode_9[j][1] + pSrc[10] * tab_coeff_mode_9[j][2] + pSrc[11] * tab_coeff_mode_9[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i*)(dst), D0);
                    dst += i_dst;
                    j++;

                    for (; j < iWidth; j++) {
                        val = (pel_t)((pSrc[8] * tab_coeff_mode_9[j][0] + pSrc[9] * tab_coeff_mode_9[j][1] + pSrc[10] * tab_coeff_mode_9[j][2] + pSrc[11] * tab_coeff_mode_9[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        _mm_storel_epi64((__m128i*)(dst), D0);
                        dst += i_dst;
                    }
                    break;
                }
                else {
                    __m128i D0;
                    c0 = _mm_load_si128((__m128i*)tab_coeff_mode_9[j]);

                    S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
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

                    _mm_storel_epi64((__m128i*)(dst), D0);

                    if (real_width < 4) {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        _mm_storel_epi64((__m128i*)(dst + real_width), D0);
                    }
                }
                dst += i_dst;
            }
            transpose_4x16(pDst);
        }
        else if (iHeight == 8) {
            for (j = 0; j < iWidth; j++) {
                int real_width;
                int idx = tab_idx_mode_9[j];

                real_width = min(8, 16 - idx + 1);

                if (real_width <= 0) {
                    pel_t val = (pel_t)((pSrc[16] * tab_coeff_mode_9[j][0] + pSrc[16 + 1] * tab_coeff_mode_9[j][1] + pSrc[16 + 2] * tab_coeff_mode_9[j][2] + pSrc[16 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i*)(dst), D0);
                    dst += i_dst;
                    j++;

                    for (; j < iHeight; j++)
                    {
                        val = (pel_t)((pSrc[16] * tab_coeff_mode_9[j][0] + pSrc[16 + 1] * tab_coeff_mode_9[j][1] + pSrc[16 + 2] * tab_coeff_mode_9[j][2] + pSrc[16 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        _mm_storel_epi64((__m128i*)(dst), D0);
                        dst += i_dst;
                    }
                    break;
                }
                else {
                    __m128i D0;
                    c0 = _mm_load_si128((__m128i*)tab_coeff_mode_9[j]);

                    S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
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

                    _mm_storel_epi64((__m128i*)(dst), D0);
                    //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);

                    if (real_width < 8) {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        _mm_storel_epi64((__m128i*)(dst + real_width), D0);
                    }

                }

                dst += i_dst;
            }
            transpose_8x32(pDst);
        }
        else {
            int iHeight2 = iHeight << 1;
            for (j = 0; j < iWidth; j++) {
                int real_width;
                int idx = tab_idx_mode_9[j];

                real_width = min(iHeight, iHeight2 - idx + 1);

                if (real_width <= 0) {
                    pel_t val = (pel_t)((pSrc[iHeight2] * tab_coeff_mode_9[j][0] + pSrc[iHeight2 + 1] * tab_coeff_mode_9[j][1] + pSrc[iHeight2 + 2] * tab_coeff_mode_9[j][2] + pSrc[iHeight2 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);

                    for (i = 0; i < iHeight; i += 16) {
                        _mm_store_si128((__m128i*)(dst + i), D0);
                    }
                    dst += i_dst;
                    j++;

                    for (; j < iWidth; j++)
                    {
                        val = (pel_t)((pSrc[iHeight2] * tab_coeff_mode_9[j][0] + pSrc[iHeight2 + 1] * tab_coeff_mode_9[j][1] + pSrc[iHeight2 + 2] * tab_coeff_mode_9[j][2] + pSrc[iHeight2 + 3] * tab_coeff_mode_9[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        for (i = 0; i < iHeight; i += 16) {
                            _mm_store_si128((__m128i*)(dst + i), D0);
                        }
                        dst += i_dst;
                    }
                    break;
                }
                else {
                    __m128i D0, D1;

                    c0 = _mm_load_si128((__m128i*)tab_coeff_mode_9[j]);
                    for (i = 0; i < real_width; i += 16, idx += 16) {
                        S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
                        S1 = _mm_loadu_si128((__m128i*)(pSrc + idx + 1));
                        S2 = _mm_loadu_si128((__m128i*)(pSrc + idx + 2));
                        S3 = _mm_loadu_si128((__m128i*)(pSrc + idx + 3));

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

                        _mm_store_si128((__m128i*)(dst + i), D0);
                        //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
                    }

                    if (real_width < iHeight)
                    {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        for (i = real_width; i < iHeight; i += 16) {
                            _mm_storeu_si128((__m128i*)(dst + i), D0);
                            //dst[i] = dst[real_width - 1];
                        }
                    }

                }

                dst += i_dst;
            }
            if (iHeight == 16) {
                transpose_16x4(pDst);
            }
            else if (iHeight == 32) {
                transpose_32x8(pDst);
            }
        }
    }
}

void xPredIntraAngAdi_Y_29_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
    int i, j;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;
    pel_t* pDst = dst;

    pSrc += 144;
    if (iWidth == iHeight){
        if (iWidth == 4){
            __m128i D0;
            int i_dst2 = i_dst << 1;

            for (j = 0; j < iHeight; j += 2) {
                int idx = tab_idx_mode_7[j];
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_7[j]);

                S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
                S1 = _mm_srli_si128(S0, 1);
                S2 = _mm_srli_si128(S0, 2);
                S3 = _mm_srli_si128(S0, 3);

                t0 = _mm_unpacklo_epi8(S0, S1);
                t1 = _mm_unpacklo_epi8(S2, S3);
                t2 = _mm_unpacklo_epi16(t0, t1);

                t0 = _mm_maddubs_epi16(t2, c0);

                idx = tab_idx_mode_7[j + 1];
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_7[j + 1]);
                S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
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

                _mm_storel_epi64((__m128i*)dst, D0);
                _mm_storel_epi64((__m128i*)(dst + i_dst), _mm_srli_si128(D0, 4));
                //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
                dst += i_dst2;
            }
            transpose_4x4(pDst);
        }
        else if (iWidth == 8) {
            __m128i D0;

            for (j = 0; j < iHeight; j++) {
                int idx = tab_idx_mode_7[j];
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_7[j]);

                S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
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

                _mm_storel_epi64((__m128i*)(dst), D0);
                //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);

                dst += i_dst;
            }
            transpose_8x8(pDst);
        }
        else {
            for (j = 0; j < iHeight; j++) {
                __m128i D0, D1;

                int idx = tab_idx_mode_7[j];
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_7[j]);

                for (i = 0; i < iWidth; i += 16, idx += 16) {
                    S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
                    S1 = _mm_loadu_si128((__m128i*)(pSrc + idx + 1));
                    S2 = _mm_loadu_si128((__m128i*)(pSrc + idx + 2));
                    S3 = _mm_loadu_si128((__m128i*)(pSrc + idx + 3));

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

                    _mm_store_si128((__m128i*)(dst + i), D0);
                    //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
                }

                dst += i_dst;
            }
            if (iWidth == 16) {
                transpose_16x16(pDst);
            }
            else if (iWidth == 32) {
                transpose_32x32_stride(pDst, i_dst, pDst, i_dst);
            }
            else{
                transpose_64x64(pDst);
            }
        }
    }
    else{
        int height = iWidth;
        int width = iHeight;
        i_dst = max(height, width);
        if (width == 4) {
            for (j = 0; j < height; j++) {
                int real_width;
                int idx = tab_idx_mode_7[j];

                real_width = min(4, 8 - idx + 1);

                if (real_width <= 0) {
                    pel_t val = (pel_t)((pSrc[8] * tab_coeff_mode_7[j][0] + pSrc[8 + 1] * tab_coeff_mode_7[j][1] + pSrc[8 + 2] * tab_coeff_mode_7[j][2] + pSrc[8 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i*)(dst), D0);
                    dst += i_dst;
                    j++;

                    for (; j < height; j++) {
                        val = (pel_t)((pSrc[8] * tab_coeff_mode_7[j][0] + pSrc[8 + 1] * tab_coeff_mode_7[j][1] + pSrc[8 + 2] * tab_coeff_mode_7[j][2] + pSrc[8 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        _mm_storel_epi64((__m128i*)(dst), D0);
                        dst += i_dst;
                    }
                    break;
                }
                else {
                    __m128i D0;
                    c0 = _mm_load_si128((__m128i*)tab_coeff_mode_7[j]);

                    S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
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

                    _mm_storel_epi64((__m128i*)(dst), D0);

                    if (real_width < 4) {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        _mm_storel_epi64((__m128i*)(dst + real_width), D0);
                    }
                }
                dst += i_dst;
            }
            transpose_4x16(pDst);
        }
        else if (width == 8) {
            for (j = 0; j < height; j++) {
                int real_width;
                int idx = tab_idx_mode_7[j];

                real_width = min(8, 16 - idx + 1);

                if (real_width <= 0) {
                    pel_t val = (pel_t)((pSrc[16] * tab_coeff_mode_7[j][0] + pSrc[16 + 1] * tab_coeff_mode_7[j][1] + pSrc[16 + 2] * tab_coeff_mode_7[j][2] + pSrc[16 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i*)(dst), D0);
                    dst += i_dst;
                    j++;

                    for (; j < height; j++) {
                        val = (pel_t)((pSrc[16] * tab_coeff_mode_7[j][0] + pSrc[16 + 1] * tab_coeff_mode_7[j][1] + pSrc[16 + 2] * tab_coeff_mode_7[j][2] + pSrc[16 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        _mm_storel_epi64((__m128i*)(dst), D0);
                        dst += i_dst;
                    }
                    break;
                }
                else {
                    __m128i D0;
                    c0 = _mm_load_si128((__m128i*)tab_coeff_mode_7[j]);

                    S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
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

                    _mm_storel_epi64((__m128i*)(dst), D0);
                    //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);

                    if (real_width < width) {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        _mm_storel_epi64((__m128i*)(dst + real_width), D0);
                    }

                }

                dst += i_dst;
            }
            transpose_8x32(pDst);
        }
        else {
            for (j = 0; j < height; j++) {
                int real_width;
                int idx = tab_idx_mode_7[j];
                int width2 = width << 1;

                real_width = min(width, width2 - idx + 1);

                if (real_width <= 0) {
                    pel_t val = (pel_t)((pSrc[width2] * tab_coeff_mode_7[j][0] + pSrc[width2 + 1] * tab_coeff_mode_7[j][1] + pSrc[width2 + 2] * tab_coeff_mode_7[j][2] + pSrc[width2 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                    __m128i D0 = _mm_set1_epi8((char)val);

                    for (i = 0; i < width; i += 16) {
                        _mm_store_si128((__m128i*)(dst + i), D0);
                    }
                    dst += i_dst;
                    j++;

                    for (; j < height; j++) {
                        val = (pel_t)((pSrc[width2] * tab_coeff_mode_7[j][0] + pSrc[width2 + 1] * tab_coeff_mode_7[j][1] + pSrc[width2 + 2] * tab_coeff_mode_7[j][2] + pSrc[width2 + 3] * tab_coeff_mode_7[j][3] + 64) >> 7);
                        D0 = _mm_set1_epi8((char)val);
                        for (i = 0; i < width; i += 16) {
                            _mm_store_si128((__m128i*)(dst + i), D0);
                        }
                        dst += i_dst;
                    }
                    break;
                }
                else {
                    __m128i D0, D1;

                    c0 = _mm_load_si128((__m128i*)tab_coeff_mode_7[j]);
                    for (i = 0; i < real_width; i += 16, idx += 16) {
                        S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
                        S1 = _mm_loadu_si128((__m128i*)(pSrc + idx + 1));
                        S2 = _mm_loadu_si128((__m128i*)(pSrc + idx + 2));
                        S3 = _mm_loadu_si128((__m128i*)(pSrc + idx + 3));

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

                        _mm_store_si128((__m128i*)(dst + i), D0);
                        //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
                    }

                    if (real_width < width) {
                        D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                        for (i = real_width; i < width; i += 16) {
                            _mm_storeu_si128((__m128i*)(dst + i), D0);
                            //dst[i] = dst[real_width - 1];
                        }
                    }

                }

                dst += i_dst;
            }
            if (width == 16)
                transpose_16x4(pDst);
            else if (width == 32)
                transpose_32x8(pDst);
        }
    }
}

void xPredIntraAngAdi_Y_31_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
    int i, j, k;
    __m128i zero = _mm_setzero_si128();
    __m128i off = _mm_set1_epi16(64);
    __m128i S0, S1, S2, S3;
    __m128i t0, t1, t2, t3;
    __m128i c0;
    pel_t* pDst = dst;
    int width = iHeight;
    int height = iWidth;
    i_dst = max(height, width);
    pSrc += 144;
    if (width == 4) {
        for (j = 0; j < height; j++) {
            int real_width;
            int idx = tab_idx_mode_5[j];

            k = j & 0x07;
            real_width = min(4, 8 - idx + 1);

            if (real_width <= 0) {
                pel_t val = (pel_t)((pSrc[8] * tab_coeff_mode_5[k][0] + pSrc[8 + 1] * tab_coeff_mode_5[k][1] + pSrc[8 + 2] * tab_coeff_mode_5[k][2] + pSrc[8 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);
                _mm_storel_epi64((__m128i*)(dst), D0);
                dst += i_dst;
                j++;

                for (; j < height; j++)
                {
                    k = j & 0x07;
                    val = (pel_t)((pSrc[8] * tab_coeff_mode_5[k][0] + pSrc[8 + 1] * tab_coeff_mode_5[k][1] + pSrc[8 + 2] * tab_coeff_mode_5[k][2] + pSrc[8 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i*)(dst), D0);
                    dst += i_dst;
                }
                break;
            }
            else {
                __m128i D0;
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_5[k]);

                S0 = _mm_loadl_epi64((__m128i*)(pSrc + idx));
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

                _mm_storel_epi64((__m128i*)(dst), D0);

                if (real_width < width)
                {
                    D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                    _mm_storel_epi64((__m128i*)(dst + real_width), D0);
                }
            }
            dst += i_dst;
        }
        if (height == 4)
            transpose_4x4(pDst);
        else
            transpose_4x16(pDst);
    }
    else if (width == 8) {
        for (j = 0; j < height; j++) {
            int real_width;
            int idx = tab_idx_mode_5[j];

            k = j & 0x07;
            real_width = min(8, 16 - idx + 1);

            if (real_width <= 0) {
                pel_t val = (pel_t)((pSrc[16] * tab_coeff_mode_5[k][0] + pSrc[16 + 1] * tab_coeff_mode_5[k][1] + pSrc[16 + 2] * tab_coeff_mode_5[k][2] + pSrc[16 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);
                _mm_storel_epi64((__m128i*)(dst), D0);
                dst += i_dst;
                j++;

                for (; j < height; j++) {
                    k = j & 0x07;
                    val = (pel_t)((pSrc[16] * tab_coeff_mode_5[k][0] + pSrc[16 + 1] * tab_coeff_mode_5[k][1]  + pSrc[16 + 2] * tab_coeff_mode_5[k][2] + pSrc[16 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    _mm_storel_epi64((__m128i*)(dst), D0);
                    dst += i_dst;
                }
                break;
            }
            else {
                __m128i D0;
                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_5[k]);

                S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
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

                _mm_storel_epi64((__m128i*)(dst), D0);
                //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);

                if (real_width < width)
                {
                    D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                    _mm_storel_epi64((__m128i*)(dst + real_width), D0);
                }

            }

            dst += i_dst;
        }
        if (height == 8)
            transpose_8x8(pDst);
        else
            transpose_8x32(pDst);
    }
    else {
        int width2 = width << 1;
        for (j = 0; j < height; j++) {
            int real_width;
            int idx = tab_idx_mode_5[j];
            k = j & 0x07;
            
            real_width = min(width, width2 - idx + 1);

            if (real_width <= 0) {
                pel_t val = (pel_t)((pSrc[width2] * tab_coeff_mode_5[k][0] + pSrc[width2 + 1] * tab_coeff_mode_5[k][1] + pSrc[width2 + 2] * tab_coeff_mode_5[k][2] + pSrc[width2 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                __m128i D0 = _mm_set1_epi8((char)val);

                for (i = 0; i < width; i += 16) {
                    _mm_store_si128((__m128i*)(dst + i), D0);
                }
                dst += i_dst;
                j++;

                for (; j < height; j++) {
                    k = j & 0x07;
                    val = (pel_t)((pSrc[width2] * tab_coeff_mode_5[k][0] + pSrc[width2 + 1] * tab_coeff_mode_5[k][1] + pSrc[width2 + 2] * tab_coeff_mode_5[k][2] + pSrc[width2 + 3] * tab_coeff_mode_5[k][3] + 64) >> 7);
                    D0 = _mm_set1_epi8((char)val);
                    for (i = 0; i < width; i += 16) {
                        _mm_store_si128((__m128i*)(dst + i), D0);
                    }
                    dst += i_dst;
                }
                break;
            }
            else {
                __m128i D0, D1;

                c0 = _mm_load_si128((__m128i*)tab_coeff_mode_5[k]);
                for (i = 0; i < real_width; i += 16, idx += 16) {
                    S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
                    S1 = _mm_loadu_si128((__m128i*)(pSrc + idx + 1));
                    S2 = _mm_loadu_si128((__m128i*)(pSrc + idx + 2));
                    S3 = _mm_loadu_si128((__m128i*)(pSrc + idx + 3));

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

                    _mm_store_si128((__m128i*)(dst + i), D0);
                    //dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
                }

                if (real_width < width)
                {
                    D0 = _mm_set1_epi8((char)dst[real_width - 1]);
                    for (i = real_width; i < width; i += 16) {
                        _mm_storeu_si128((__m128i*)(dst + i), D0);
                        //dst[i] = dst[real_width - 1];
                    }
                }

            }

            dst += i_dst;
        }
        if (width == 16) {
            if (height == 16)
                transpose_16x16(pDst);
            else
                transpose_16x4(pDst);
        }
        else if (width == 32) {
            if (height == 32)
                transpose_32x32_stride(pDst, i_dst, pDst, i_dst);
            else
                transpose_32x8(pDst);
        }
        else{
            transpose_64x64(pDst);
        }
    }
    
}

void xPredIntraAngAdi_Y_26_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
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

		ALIGNED_16(pel_t first_line[64 + 256]);
		int line_size = iWidth + (iHeight - 1) * 4;
		int iHeight4 = iHeight << 2;

        pSrc -= 15;

		for (i = 0; i < line_size - 32; i += 64, pSrc -= 16)
		{
			__m128i p00, p10, p20, p30;
			__m128i p01, p11, p21, p31;
			__m128i M1, M2, M3, M4, M5, M6, M7, M8;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc - 3));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc - 2));

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

			_mm_store_si128((__m128i*)&first_line[i], M4);
			_mm_store_si128((__m128i*)&first_line[16 + i], M8);
			_mm_store_si128((__m128i*)&first_line[32 + i], M3);
			_mm_store_si128((__m128i*)&first_line[48 + i], M7);
		}

		if (i < line_size)
		{
			__m128i p01, p11, p21, p31;
			__m128i M2, M4, M6, M8;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc - 3));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc - 2));

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

			_mm_store_si128((__m128i*)&first_line[i], M4);
			_mm_store_si128((__m128i*)&first_line[16 + i], M8);
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
                memcpy(dst, first_line + i, iWidth * sizeof(pel_t));
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
        pSrc -= 15;

		if (iHeight == 4) {
			__m128i p01, p11, p21, p31;
			__m128i M2, M4, M6, M8;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc - 3));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc - 2));

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

			((int*)dst)[0] = _mm_cvtsi128_si32(M4);
			dst += i_dst;
			M4 = _mm_srli_si128(M4, 4);
			((int*)dst)[0] = _mm_cvtsi128_si32(M4);
			dst += i_dst;
			M4 = _mm_srli_si128(M4, 4);
			((int*)dst)[0] = _mm_cvtsi128_si32(M4);
			dst += i_dst;
			M4 = _mm_srli_si128(M4, 4);
			((int*)dst)[0] = _mm_cvtsi128_si32(M4);
		}
		else
		{
			__m128i p00, p10, p20, p30;
			__m128i p01, p11, p21, p31;
			__m128i M1, M2, M3, M4, M5, M6, M7, M8;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc - 3));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc - 2));

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

			((int*)dst)[0] = _mm_cvtsi128_si32(M4);
			dst += i_dst;
			M4 = _mm_srli_si128(M4, 4);
			((int*)dst)[0] = _mm_cvtsi128_si32(M4);
			dst += i_dst;
			M4 = _mm_srli_si128(M4, 4);
			((int*)dst)[0] = _mm_cvtsi128_si32(M4);
			dst += i_dst;
			M4 = _mm_srli_si128(M4, 4);
			((int*)dst)[0] = _mm_cvtsi128_si32(M4);
			dst += i_dst;
			((int*)dst)[0] = _mm_cvtsi128_si32(M8);
			dst += i_dst;
			M8 = _mm_srli_si128(M8, 4);
			((int*)dst)[0] = _mm_cvtsi128_si32(M8);
			dst += i_dst;
			M8 = _mm_srli_si128(M8, 4);
			((int*)dst)[0] = _mm_cvtsi128_si32(M8);
			dst += i_dst;
			M8 = _mm_srli_si128(M8, 4);
			((int*)dst)[0] = _mm_cvtsi128_si32(M8);
			dst += i_dst;
			((int*)dst)[0] = _mm_cvtsi128_si32(M3);
			dst += i_dst;
			M3 = _mm_srli_si128(M3, 4);
			((int*)dst)[0] = _mm_cvtsi128_si32(M3);
			dst += i_dst;
			M3 = _mm_srli_si128(M3, 4);
			((int*)dst)[0] = _mm_cvtsi128_si32(M3);
			dst += i_dst;
			M3 = _mm_srli_si128(M3, 4);
			((int*)dst)[0] = _mm_cvtsi128_si32(M3);
			dst += i_dst;
			((int*)dst)[0] = _mm_cvtsi128_si32(M7);
			dst += i_dst;
			M7 = _mm_srli_si128(M7, 4);
			((int*)dst)[0] = _mm_cvtsi128_si32(M7);
			dst += i_dst;
			M7 = _mm_srli_si128(M7, 4);
			((int*)dst)[0] = _mm_cvtsi128_si32(M7);
			dst += i_dst;
			M7 = _mm_srli_si128(M7, 4);
			((int*)dst)[0] = _mm_cvtsi128_si32(M7);
		}
	}
}

void xPredIntraAngAdi_Y_28_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[64 + 128]);
	int line_size = iWidth + (iHeight - 1) * 2;
	int real_size = min(line_size, iHeight * 4 + 1);
	int i;
	int iHeight2 = iHeight << 1;
	__m128i pad;
	__m128i coeff2 = _mm_set1_epi16(2);
	__m128i coeff3 = _mm_set1_epi16(3);
	__m128i coeff4 = _mm_set1_epi16(4);
	__m128i shuffle = _mm_setr_epi8(7, 15, 6, 14, 5, 13, 4, 12, 3, 11, 2, 10, 1, 9, 0, 8);
	__m128i zero = _mm_setzero_si128();

	pSrc -= 15;

	for (i = 0; i < real_size - 16; i += 32, pSrc -= 16)
	{
		__m128i p00, p10, p01, p11;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
		__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc - 3));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc - 2));

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

		_mm_store_si128((__m128i*)&first_line[i + 16], p00);

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

		_mm_store_si128((__m128i*)&first_line[i], p00);
	}

	if (i < real_size)
	{
		__m128i p00, p10, p01, p11;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
		__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc - 3));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc - 2));

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

		_mm_store_si128((__m128i*)&first_line[i], p00);
	}

	// padding
	if (real_size < line_size) {
		i = real_size + 1;
		first_line[i - 1] = first_line[i - 3];

		pad = _mm_set1_epi16(((short*)&first_line[i - 2])[0]);

		for (; i < line_size; i += 16) {
			_mm_storeu_si128((__m128i*)&first_line[i], pad);
		}
	}

	if (iWidth >= 16)
	{
		for (i = 0; i < iHeight2; i += 2)
		{
			memcpy(dst, first_line + i, iWidth * sizeof(pel_t));
			dst += i_dst;
		}
	}
	else if (iWidth == 8)
	{
		for (i = 0; i < iHeight2; i += 8)
		{
			__m128i M = _mm_loadu_si128((__m128i*)&first_line[i]);
			_mm_storel_epi64((__m128i*)(dst), M);
			dst += i_dst;
			M = _mm_srli_si128(M, 2);
			_mm_storel_epi64((__m128i*)(dst), M);
			dst += i_dst;
			M = _mm_srli_si128(M, 2);
			_mm_storel_epi64((__m128i*)(dst), M);
			dst += i_dst;
			M = _mm_srli_si128(M, 2);
			_mm_storel_epi64((__m128i*)(dst), M);
			dst += i_dst;
		}
	}
	else
	{
		for (i = 0; i < iHeight2; i += 8)
		{
			__m128i M = _mm_loadu_si128((__m128i*)&first_line[i]);
			((int*)(dst))[0] = _mm_cvtsi128_si32(M);
			dst += i_dst;
			M = _mm_srli_si128(M, 2);
			((int*)(dst))[0] = _mm_cvtsi128_si32(M);
			dst += i_dst;
			M = _mm_srli_si128(M, 2);
			((int*)(dst))[0] = _mm_cvtsi128_si32(M);
			dst += i_dst;
			M = _mm_srli_si128(M, 2);
			((int*)(dst))[0] = _mm_cvtsi128_si32(M);
			dst += i_dst;
		}
	}
}

void xPredIntraAngAdi_Y_30_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[64 + 64]);
	int line_size = iWidth + iHeight - 1;
	int real_size = min(line_size, iHeight * 2);
	int i;
	__m128i coeff2 = _mm_set1_epi16(2);
	__m128i shuffle = _mm_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
	__m128i zero = _mm_setzero_si128();

	pSrc -= 17;

	for (i = 0; i < real_size - 8; i += 16, pSrc -= 16)
	{
		__m128i p00, p10, p01, p11;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

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

		_mm_store_si128((__m128i*)&first_line[i], p00);
	}

	if (i < real_size)
	{
		__m128i p01, p11;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

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

		_mm_store_si128((__m128i*)&first_line[i], p01);
	}
	// padding
	for (i = real_size; i < line_size; i += 16) {
        __m128i pad = _mm_set1_epi8((char)first_line[real_size - 1]);
		_mm_storeu_si128((__m128i*)&first_line[i], pad);
	}

	if (iWidth > 16) {
		for (i = 0; i < iHeight; i++) {
			memcpy(dst, first_line + i, iWidth * sizeof(pel_t));
			dst += i_dst;
		}
	} else if (iWidth == 16) {
		pel_t *dst1 = dst;
		pel_t *dst2;
		if (iHeight == 4) {
			__m128i M = _mm_loadu_si128((__m128i*)&first_line[0]);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 = dst + 8;
			M = _mm_loadu_si128((__m128i*)&first_line[8]);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
		} else {
			__m128i M = _mm_loadu_si128((__m128i*)&first_line[0]);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst2 = dst1 + i_dst;
			dst1 = dst + 8;
			M = _mm_loadu_si128((__m128i*)&first_line[8]);
			_mm_storel_epi64((__m128i*)dst1, M);
			_mm_storel_epi64((__m128i*)dst2, M);
			dst1 += i_dst;
			dst2 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			_mm_storel_epi64((__m128i*)dst2, M);
			dst1 += i_dst;
			dst2 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			_mm_storel_epi64((__m128i*)dst2, M);
			dst1 += i_dst;
			dst2 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			_mm_storel_epi64((__m128i*)dst2, M);
			dst1 += i_dst;
			dst2 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			_mm_storel_epi64((__m128i*)dst2, M);
			dst1 += i_dst;
			dst2 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			_mm_storel_epi64((__m128i*)dst2, M);
			dst1 += i_dst;
			dst2 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			_mm_storel_epi64((__m128i*)dst2, M);
			dst1 += i_dst;
			dst2 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			_mm_storel_epi64((__m128i*)dst2, M);
			dst1 += i_dst;
			M = _mm_loadu_si128((__m128i*)&first_line[16]);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
			dst1 += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst1, M);
		}
	}
	else if (iWidth == 8)
	{
		for (i = 0; i < iHeight; i += 8)
		{
			__m128i M = _mm_loadu_si128((__m128i*)&first_line[i]);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			_mm_storel_epi64((__m128i*)dst, M);
			dst += i_dst;
		}
	}
	else
	{
		for (i = 0; i < iHeight; i += 4)
		{
			__m128i M = _mm_loadu_si128((__m128i*)&first_line[i]);
			((int*)(dst))[0] = _mm_cvtsi128_si32(M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			((int*)(dst))[0] = _mm_cvtsi128_si32(M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			((int*)(dst))[0] = _mm_cvtsi128_si32(M);
			dst += i_dst;
			M = _mm_srli_si128(M, 1);
			((int*)(dst))[0] = _mm_cvtsi128_si32(M);
			dst += i_dst;
		}
	}
}

void xPredIntraAngAdi_Y_32_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[2 * (64 + 64)]);
	int line_size = iHeight / 2 + iWidth - 1;
	int real_size = min(line_size, iHeight);
	int i;
	__m128i pad_val;
	int aligned_line_size = ((line_size + 63) >> 4) << 4;
    pel_t *pfirst[2];
	__m128i coeff2 = _mm_set1_epi16(2);
	__m128i zero = _mm_setzero_si128();
	__m128i shuffle1 = _mm_setr_epi8(15, 13, 11, 9, 7, 5, 3, 1, 14, 12, 10, 8, 6, 4, 2, 0);
	__m128i shuffle2 = _mm_setr_epi8(14, 12, 10, 8, 6, 4, 2, 0, 15, 13, 11, 9, 7, 5, 3, 1);
	int i_dst2 = i_dst * 2;

    pfirst[0] = first_line;
    pfirst[1] = first_line + aligned_line_size;

	pSrc -= 18;

	for (i = 0; i < real_size - 4; i += 8, pSrc -= 16)
	{
		__m128i p00, p01, p10, p11;
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

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
		_mm_storel_epi64((__m128i*)&pfirst[0][i], p00);
		_mm_storel_epi64((__m128i*)&pfirst[1][i], p10);
	}

	if (i < real_size)
	{
		__m128i p10, p11;
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

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
		((int*)&pfirst[0][i])[0] = _mm_cvtsi128_si32(p11);
		((int*)&pfirst[1][i])[0] = _mm_cvtsi128_si32(p10);
	}

	// padding
	if (real_size < line_size)
	{
        pad_val = _mm_set1_epi8((char)pfirst[1][real_size - 1]);
		for (i = real_size; i < line_size; i++)
		{
			_mm_storeu_si128((__m128i*)&pfirst[0][i], pad_val);
			_mm_storeu_si128((__m128i*)&pfirst[1][i], pad_val);
		}
	}

	iHeight /= 2;

	if (iWidth >= 16 || iWidth == 4)
	{
		for (i = 0; i < iHeight; i++) {
			memcpy(dst, pfirst[0] + i, iWidth * sizeof(pel_t));
			memcpy(dst + i_dst, pfirst[1] + i, iWidth * sizeof(pel_t));
			dst += i_dst2;
		}
	}
	else
	{
		if (iHeight == 4)
		{
			__m128i M1 = _mm_loadu_si128((__m128i*)&pfirst[0][0]);
			__m128i M2 = _mm_loadu_si128((__m128i*)&pfirst[1][0]);
			_mm_storel_epi64((__m128i*)dst, M1);
			_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
			dst += i_dst2;
			M1 = _mm_srli_si128(M1, 1);
			M2 = _mm_srli_si128(M2, 1);
			_mm_storel_epi64((__m128i*)dst, M1);
			_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
			dst += i_dst2;
			M1 = _mm_srli_si128(M1, 1);
			M2 = _mm_srli_si128(M2, 1);
			_mm_storel_epi64((__m128i*)dst, M1);
			_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
			dst += i_dst2;
			M1 = _mm_srli_si128(M1, 1);
			M2 = _mm_srli_si128(M2, 1);
			_mm_storel_epi64((__m128i*)dst, M1);
			_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
		}
		else
		{
			for (i = 0; i < 16; i = i + 8)
			{
				__m128i M1 = _mm_loadu_si128((__m128i*)&pfirst[0][i]);
				__m128i M2 = _mm_loadu_si128((__m128i*)&pfirst[1][i]);
				_mm_storel_epi64((__m128i*)dst, M1);
				_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
				dst += i_dst2;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				_mm_storel_epi64((__m128i*)dst, M1);
				_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
				dst += i_dst2;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				_mm_storel_epi64((__m128i*)dst, M1);
				_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
				dst += i_dst2;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				_mm_storel_epi64((__m128i*)dst, M1);
				_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
				dst += i_dst2;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				_mm_storel_epi64((__m128i*)dst, M1);
				_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
				dst += i_dst2;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				_mm_storel_epi64((__m128i*)dst, M1);
				_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
				dst += i_dst2;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				_mm_storel_epi64((__m128i*)dst, M1);
				_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
				dst += i_dst2;
				M1 = _mm_srli_si128(M1, 1);
				M2 = _mm_srli_si128(M2, 1);
				_mm_storel_epi64((__m128i*)dst, M1);
				_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
				dst += i_dst2;
			}
		}
	}
}

void xPredIntraAngAdi_XY_14_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
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
		ALIGNED_16(pel_t first_line[4 * (64 + 32)]);
		int line_size = iWidth + iHeight / 4 - 1;
		int left_size = line_size - iWidth;
		int aligned_line_size = ((line_size + 31) >> 4) << 4;
        pel_t *pfirst[4];
		__m128i shuffle1 = _mm_setr_epi8(0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15);
		__m128i shuffle2 = _mm_setr_epi8(1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 0, 4, 8, 12);
		__m128i shuffle3 = _mm_setr_epi8(2, 6, 10, 14, 3, 7, 11, 15, 0, 4, 8, 12, 1, 5, 9, 13);
		__m128i shuffle4 = _mm_setr_epi8(3, 7, 11, 15, 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14);
		pel_t *pSrc1 = pSrc;

        pfirst[0] = first_line;
        pfirst[1] = first_line + aligned_line_size;
        pfirst[2] = first_line + aligned_line_size * 2;
        pfirst[3] = first_line + aligned_line_size * 3;

		pSrc -= iHeight - 4;
		for (i = 0; i < left_size - 1; i += 4, pSrc += 16)
		{
			__m128i p00, p01, p10, p11;
			__m128i p20, p30;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

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

			((int*)&pfirst[0][i])[0] = _mm_cvtsi128_si32(p30);
			((int*)&pfirst[1][i])[0] = _mm_cvtsi128_si32(p20);
			((int*)&pfirst[2][i])[0] = _mm_cvtsi128_si32(p10);
			((int*)&pfirst[3][i])[0] = _mm_cvtsi128_si32(p00);
		}

		if (i < left_size)  //使用c语言可能会更优
		{
			__m128i p00, p01, p10;
			__m128i p20, p30;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

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

			((int*)&pfirst[0][i])[0] = _mm_cvtsi128_si32(p30);
			((int*)&pfirst[1][i])[0] = _mm_cvtsi128_si32(p20);
			((int*)&pfirst[2][i])[0] = _mm_cvtsi128_si32(p10);
			((int*)&pfirst[3][i])[0] = _mm_cvtsi128_si32(p00);
		}

		pSrc = pSrc1;

		for (i = left_size; i < line_size; i+=16, pSrc+=16) {
			__m128i p00, p10, p20, p30;
			__m128i p01, p11, p21, p31;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 2));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));

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
			_mm_storeu_si128((__m128i*)&pfirst[2][i], p00);

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
			_mm_storeu_si128((__m128i*)&pfirst[1][i], p00);

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
			_mm_storeu_si128((__m128i*)&pfirst[0][i], p00);

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
			_mm_storeu_si128((__m128i*)&pfirst[3][i], p00);
		}

		pfirst[0] += left_size;
		pfirst[1] += left_size;
		pfirst[2] += left_size;
		pfirst[3] += left_size;

		iHeight >>= 2;

        switch (iWidth) {
        case 4:
            for (i = 0; i < iHeight; i++) {
                CP32(dst, pfirst[0] - i); dst += i_dst;
                CP32(dst, pfirst[1] - i); dst += i_dst;
                CP32(dst, pfirst[2] - i); dst += i_dst;
                CP32(dst, pfirst[3] - i); dst += i_dst;
            }
            break;
        case 8:
            for (i = 0; i < iHeight; i++) {
                CP64(dst, pfirst[0] - i); dst += i_dst;
                CP64(dst, pfirst[1] - i); dst += i_dst;
                CP64(dst, pfirst[2] - i); dst += i_dst;
                CP64(dst, pfirst[3] - i); dst += i_dst;
            }
            break;
        case 16:
            for (i = 0; i < iHeight; i++) {
                CP128(dst, pfirst[0] - i); dst += i_dst;
                CP128(dst, pfirst[1] - i); dst += i_dst;
                CP128(dst, pfirst[2] - i); dst += i_dst;
                CP128(dst, pfirst[3] - i); dst += i_dst;
            }
            break;
        case 32:
        case 64:
            for (i = 0; i < iHeight; i++) {
                memcpy(dst, pfirst[0] - i, iWidth * sizeof(pel_t)); dst += i_dst;
                memcpy(dst, pfirst[1] - i, iWidth * sizeof(pel_t)); dst += i_dst;
                memcpy(dst, pfirst[2] - i, iWidth * sizeof(pel_t)); dst += i_dst;
                memcpy(dst, pfirst[3] - i, iWidth * sizeof(pel_t)); dst += i_dst;
            }
            break;
        default:
            assert(0);
            break;
        }
	}
	else
	{
		if (iWidth == 16)
		{
			pel_t *dst2 = dst + i_dst;
			pel_t *dst3 = dst2 + i_dst;
			pel_t *dst4 = dst3 + i_dst;
			__m128i p00, p10, p20, p30;
			__m128i p01, p11, p21, p31;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 2));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));

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
			_mm_storeu_si128((__m128i*)dst3, p00);

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
			_mm_storeu_si128((__m128i*)dst2, p00);

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
			_mm_storeu_si128((__m128i*)dst, p00);

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
			_mm_storeu_si128((__m128i*)dst4, p00);
		}
		else
		{
			pel_t *dst2 = dst + i_dst;
			pel_t *dst3 = dst2 + i_dst;
			pel_t *dst4 = dst3 + i_dst;
			__m128i p00, p10, p20, p30;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 2));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));

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
			((int*)dst3)[0] = _mm_cvtsi128_si32(p00);

			p00 = _mm_add_epi16(L1, L2);
			p00 = _mm_mullo_epi16(p00, coeff3);
			p10 = _mm_add_epi16(L0, L3);
			p10 = _mm_add_epi16(p10, coeff4);
			p00 = _mm_add_epi16(p10, p00);
			p00 = _mm_srli_epi16(p00, 3);

			p00 = _mm_packus_epi16(p00, p00);
			((int*)dst2)[0] = _mm_cvtsi128_si32(p00);

			p10 = _mm_mullo_epi16(L1, coeff5);
			p20 = _mm_mullo_epi16(L2, coeff7);
			p30 = _mm_mullo_epi16(L3, coeff3);
			p00 = _mm_add_epi16(L0, coeff8);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_srli_epi16(p00, 4);

			p00 = _mm_packus_epi16(p00, p00);
			((int*)dst)[0] = _mm_cvtsi128_si32(p00);

			p00 = _mm_add_epi16(L0, L1);
			p10 = _mm_add_epi16(L1, L2);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, coeff2);
			p00 = _mm_srli_epi16(p00, 2);

			p00 = _mm_packus_epi16(p00, p00);
			((int*)dst4)[0] = _mm_cvtsi128_si32(p00);
		}
	}
}

void xPredIntraAngAdi_XY_16_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[2 * (64 + 48)]);
	int line_size = iWidth + iHeight / 2 - 1;
	int left_size = line_size - iWidth;
	int aligned_line_size = ((line_size + 31) >> 4) << 4;
    pel_t *pfirst[2];
	__m128i zero = _mm_setzero_si128();
	__m128i coeff2 = _mm_set1_epi16(2);
	__m128i coeff3 = _mm_set1_epi16(3);
	__m128i coeff4 = _mm_set1_epi16(4);
	__m128i shuffle1 = _mm_setr_epi8(0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15);
	__m128i shuffle2 = _mm_setr_epi8(1, 3, 5, 7, 9, 11, 13, 15, 0, 2, 4, 6, 8, 10, 12, 14);
	int i;
    pel_t *pSrc1;

    pfirst[0] = first_line;
    pfirst[1] = first_line + aligned_line_size;

	pSrc -= iHeight - 2;

	pSrc1 = pSrc;

	for (i = 0; i < left_size - 4; i += 8, pSrc += 16)
	{
		__m128i p00, p01, p10, p11;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

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
		_mm_storel_epi64((__m128i*)&pfirst[1][i], p00);
		_mm_storel_epi64((__m128i*)&pfirst[0][i], p10);
	}

	if (i < left_size)
	{
		__m128i p00, p01;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

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
		((int*)&pfirst[1][i])[0] = _mm_cvtsi128_si32(p00);
		((int*)&pfirst[0][i])[0] = _mm_cvtsi128_si32(p01);
	}

	pSrc = pSrc1 + left_size + left_size;

	for (i = left_size; i < line_size; i += 16, pSrc += 16)
	{
		__m128i p00, p01, p10, p11;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 2));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));

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
		_mm_storeu_si128((__m128i*)&pfirst[0][i], p00);

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
		_mm_storeu_si128((__m128i*)&pfirst[1][i], p00);
	}

	pfirst[0] += left_size;
	pfirst[1] += left_size;

	iHeight >>= 1;

    switch (iWidth) {
    case 4:
        for (i = 0; i < iHeight; i++) {
            CP32(dst,         pfirst[0] - i);
            CP32(dst + i_dst, pfirst[1] - i);
            dst += (i_dst << 1);
        }
        break;
    case 8:
        for (i = 0; i < iHeight; i++) {
            CP64(dst,         pfirst[0] - i);
            CP64(dst + i_dst, pfirst[1] - i);
            dst += (i_dst << 1);
        }
        break;
    case 16:
        for (i = 0; i < iHeight; i++) {
            CP128(dst,         pfirst[0] - i);
            CP128(dst + i_dst, pfirst[1] - i);
            dst += (i_dst << 1);
        }
        break;
    default:
        for (i = 0; i < iHeight; i++) {
            memcpy(dst,         pfirst[0] - i, iWidth * sizeof(pel_t));
            memcpy(dst + i_dst, pfirst[1] - i, iWidth * sizeof(pel_t));
            dst += (i_dst << 1);
        }
        break;
    }
	
}

void xPredIntraAngAdi_XY_18_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[64 + 64]);
	int line_size = iWidth + iHeight - 1;
	int i;
	pel_t *pfirst = first_line + iHeight - 1;
	__m128i coeff2 = _mm_set1_epi16(2);
	__m128i zero = _mm_setzero_si128();

	pSrc -= iHeight - 1;

	for (i = 0; i < line_size - 8; i += 16, pSrc += 16)
	{
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

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

		_mm_store_si128((__m128i*)&first_line[i], sum1);
	}

	if (i < line_size)
	{
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

		__m128i L0 = _mm_unpacklo_epi8(S0, zero);
		__m128i L1 = _mm_unpacklo_epi8(S1, zero);
		__m128i L2 = _mm_unpacklo_epi8(S2, zero);

		__m128i sum1 = _mm_add_epi16(L0, L1);
		__m128i sum2 = _mm_add_epi16(L1, L2);

		sum1 = _mm_add_epi16(sum1, sum2);
		sum1 = _mm_add_epi16(sum1, coeff2);
		sum1 = _mm_srli_epi16(sum1, 2);

		sum1 = _mm_packus_epi16(sum1, sum1);
		_mm_storel_epi64((__m128i*)&first_line[i], sum1);
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
            memcpy(dst, pfirst--, iWidth * sizeof(pel_t));
            dst += i_dst;
        }
        break;
        break;
    }
    
}

void xPredIntraAngAdi_XY_20_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[64 + 128]);
	int left_size = (iHeight - 1) * 2 + 1;
	int top_size = iWidth - 1;
	int line_size = left_size + top_size;
	int i;
	pel_t *pfirst = first_line + left_size - 1;
	__m128i zero = _mm_setzero_si128();
	__m128i coeff2 = _mm_set1_epi16(2);
	__m128i coeff3 = _mm_set1_epi16(3);
	__m128i coeff4 = _mm_set1_epi16(4);
	__m128i shuffle = _mm_setr_epi8(0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15);
	pel_t *pSrc1 = pSrc;
	pSrc -= iHeight;

	for (i = 0; i < left_size - 16; i += 32, pSrc += 16)
	{
		__m128i p00, p01, p10, p11;
		__m128i p20, p21, p30, p31;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 2));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

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
		_mm_store_si128((__m128i*)&first_line[i], p00);
		_mm_store_si128((__m128i*)&first_line[i + 16], p10);
	}

	if (i < left_size)
	{
		__m128i p00, p01;
		__m128i p20, p21;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 2));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

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
		_mm_store_si128((__m128i*)&first_line[i], p00);
	}

	pSrc = pSrc1;

	for (i = left_size; i < line_size - 8; i += 16, pSrc += 16)
	{
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

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

		_mm_storeu_si128((__m128i*)&first_line[i], sum1);
	}

	if (i < line_size)
	{
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

		__m128i L0 = _mm_unpacklo_epi8(S0, zero);
		__m128i L1 = _mm_unpacklo_epi8(S1, zero);
		__m128i L2 = _mm_unpacklo_epi8(S2, zero);

		__m128i sum1 = _mm_add_epi16(L0, L1);
		__m128i sum2 = _mm_add_epi16(L1, L2);

		sum1 = _mm_add_epi16(sum1, sum2);
		sum1 = _mm_add_epi16(sum1, coeff2);
		sum1 = _mm_srli_epi16(sum1, 2);

		sum1 = _mm_packus_epi16(sum1, sum1);
		_mm_storel_epi64((__m128i*)&first_line[i], sum1);
	}

	for (i = 0; i < iHeight; i++) {
		memcpy(dst, pfirst, iWidth * sizeof(pel_t));
		pfirst -= 2;
		dst += i_dst;
	}
}

void xPredIntraAngAdi_XY_22_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	int i;
	pSrc -= iHeight;

	if (iWidth != 4) {
		ALIGNED_16(pel_t first_line[64 + 256]);
		int left_size = (iHeight - 1) * 4 + 3;
		int top_size = iWidth - 3;
		int line_size = left_size + top_size;
		pel_t *pfirst = first_line + left_size - 3;
		pel_t *pSrc1 = pSrc;

		__m128i zero = _mm_setzero_si128();
		__m128i coeff2 = _mm_set1_epi16(2);
		__m128i coeff3 = _mm_set1_epi16(3);
		__m128i coeff4 = _mm_set1_epi16(4);
		__m128i coeff5 = _mm_set1_epi16(5);
		__m128i coeff7 = _mm_set1_epi16(7);
		__m128i coeff8 = _mm_set1_epi16(8);
		__m128i shuffle = _mm_setr_epi8(0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15);

		for (i = 0; i < line_size - 32; i += 64, pSrc += 16)
		{
			__m128i p00, p10, p20, p30;
			__m128i p01, p11, p21, p31;
			__m128i M1, M2, M3, M4, M5, M6, M7, M8;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 2));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));

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

			_mm_store_si128((__m128i*)&first_line[i], M3);
			_mm_store_si128((__m128i*)&first_line[16 + i], M7);
			_mm_store_si128((__m128i*)&first_line[32 + i], M4);
			_mm_store_si128((__m128i*)&first_line[48 + i], M8);
		}

		if (i < left_size)
		{
			__m128i p00, p10, p20, p30;
			__m128i M1, M3, M5, M7;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 2));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));

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

			_mm_store_si128((__m128i*)&first_line[i], M3);
			_mm_store_si128((__m128i*)&first_line[16 + i], M7);
		}

		pSrc = pSrc1 + iHeight;

		for (i = left_size; i < line_size - 8; i += 16, pSrc += 16)
		{
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

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

			_mm_storeu_si128((__m128i*)&first_line[i], sum1);
		}

		if (i < line_size)
		{
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

			__m128i L0 = _mm_unpacklo_epi8(S0, zero);
			__m128i L1 = _mm_unpacklo_epi8(S1, zero);
			__m128i L2 = _mm_unpacklo_epi8(S2, zero);

			__m128i sum1 = _mm_add_epi16(L0, L1);
			__m128i sum2 = _mm_add_epi16(L1, L2);

			sum1 = _mm_add_epi16(sum1, sum2);
			sum1 = _mm_add_epi16(sum1, coeff2);
			sum1 = _mm_srli_epi16(sum1, 2);

			sum1 = _mm_packus_epi16(sum1, sum1);
			_mm_storel_epi64((__m128i*)&first_line[i], sum1);
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
                memcpy(dst, pfirst, iWidth * sizeof(pel_t));
                dst += i_dst;
                pfirst -= 4;
            }
            break;
        default:
            assert(0);
            break;
        }
	} else {
		dst += (iHeight - 1) * i_dst;
		for (i = 0; i < iHeight; i++, pSrc++)
		{
			dst[0] = (pSrc[-1] * 3 + pSrc[0] * 7 + pSrc[1] * 5 + pSrc[2] + 8) >> 4;
			dst[1] = (pSrc[-1] + (pSrc[0] + pSrc[1]) * 3 + pSrc[2] + 4) >> 3;
			dst[2] = (pSrc[-1] + pSrc[0] * 5 + pSrc[1] * 7 + pSrc[2] * 3 + 8) >> 4;
			dst[3] = (pSrc[0] + pSrc[1] * 2 + pSrc[2] + 2) >> 2;
			dst -= i_dst;
		}
	}
}

void xPredIntraVertAdi_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight)
{
    int y;
    pel_t *rpSrc = pSrc + 1;
    __m128i T1, T2, T3, T4;

    switch (iWidth) {
    case 4:
        for (y = 0; y < iHeight; y += 2) {
            CP32(dst,         rpSrc);
            CP32(dst + i_dst, rpSrc);
            dst += i_dst << 1;
        }
        break;
    case 8:
        for (y = 0; y < iHeight; y++) {
            CP64(dst,         rpSrc);
            CP64(dst + i_dst, rpSrc);
            dst += i_dst << 1;
        }
        break;
    case 16:
        T1 = _mm_loadu_si128((__m128i*)rpSrc);
        for (y = 0; y < iHeight; y++) {
            _mm_storeu_si128((__m128i*)(dst), T1);
            dst += i_dst;
        }
        break;
    case 32:
        T1 = _mm_loadu_si128((__m128i*)(rpSrc +  0));
        T2 = _mm_loadu_si128((__m128i*)(rpSrc + 16));
        for (y = 0; y < iHeight; y++) {
            _mm_storeu_si128((__m128i*)(dst +  0), T1);
            _mm_storeu_si128((__m128i*)(dst + 16), T2);
            dst += i_dst;
        }
        break;
    case 64:
        T1 = _mm_loadu_si128((__m128i*)(rpSrc +  0));
        T2 = _mm_loadu_si128((__m128i*)(rpSrc + 16));
        T3 = _mm_loadu_si128((__m128i*)(rpSrc + 32));
        T4 = _mm_loadu_si128((__m128i*)(rpSrc + 48));
        for (y = 0; y < iHeight; y++) {
            _mm_storeu_si128((__m128i*)(dst +  0), T1);
            _mm_storeu_si128((__m128i*)(dst + 16), T2);
            _mm_storeu_si128((__m128i*)(dst + 32), T3);
            _mm_storeu_si128((__m128i*)(dst + 48), T4);
            dst += i_dst;
        }
        break;
    default:
        assert(0);
        break;
    }
}

void xPredIntraHorAdi_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight)
{
    int y;
    pel_t *rpSrc = pSrc - 1;
    __m128i T;

    switch (iWidth) {
    case 4:
        for (y = 0; y < iHeight; y++) {
            M32(dst) = 0x01010101 * rpSrc[-y];
            dst += i_dst;
        }
        break;
    case 8:
        for (y = 0; y < iHeight; y++) {
            M64(dst) = 0x0101010101010101 * rpSrc[-y];
            dst += i_dst;
        }
        break;
    case 16:
        for (y = 0; y < iHeight; y++) {
            T = _mm_set1_epi8((char)rpSrc[-y]);
            _mm_storeu_si128((__m128i*)(dst), T);
            dst += i_dst;
        }
        break;
    case 32:
        for (y = 0; y < iHeight; y++) {
            T = _mm_set1_epi8((char)rpSrc[-y]);
            _mm_storeu_si128((__m128i*)(dst +  0), T);
            _mm_storeu_si128((__m128i*)(dst + 16), T);
            dst += i_dst;
        }
        break;
    case 64:
        for (y = 0; y < iHeight; y++) {
            T = _mm_set1_epi8((char)rpSrc[-y]);
            _mm_storeu_si128((__m128i*)(dst +  0), T);
            _mm_storeu_si128((__m128i*)(dst + 16), T);
            _mm_storeu_si128((__m128i*)(dst + 32), T);
            _mm_storeu_si128((__m128i*)(dst + 48), T);
            dst += i_dst;
        }
        break;
    default:
        assert(0);
        break;
    }
}

void xPredIntraDCAdi_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bAboveAvail, int bLeftAvail, int bit_depth)
{
    int   x, y;
    int   iDCValue = 0;
    pel_t  *rpSrc = pSrc - 1;
    int h_bitsize = tab_log2[iHeight];
    int w_bitsize = tab_log2[iWidth];
    int half_height = iHeight >> 1;
    int half_width = iWidth >> 1;
    __m128i T;
    i32u_t v32;
    i64u_t v64;

    if (bLeftAvail) {
        for (y = 0; y < iHeight; y++) {
            iDCValue += rpSrc[-y];
        }

        rpSrc = pSrc + 1;
        if (bAboveAvail) {
            for (x = 0; x < iWidth; x++) {
                iDCValue += rpSrc[x];
            }

            iDCValue += ((iWidth + iHeight) >> 1);
            iDCValue = (iDCValue * (512 / (iWidth + iHeight))) >> 9;
        } else {
            iDCValue += half_height;
            iDCValue >>= h_bitsize;
        }
    } else {
        rpSrc = pSrc + 1;
        if (bAboveAvail) {
            for (x = 0; x < iWidth; x++) {
                iDCValue += rpSrc[x];
            }

            iDCValue += half_width;
            iDCValue >>= w_bitsize;
        } else {
            iDCValue = 1 << (bit_depth - 1);
        }
    }

    switch (iWidth) {
    case 4:
        v32 = 0x01010101 * iDCValue;
        for (y = 0; y < iHeight; y++) {
            M32(dst) = v32;
            dst += i_dst;
        }
        break;
    case 8:
        v64 = 0x0101010101010101 * iDCValue;
        for (y = 0; y < iHeight; y++) {
            M64(dst) = v64;
            dst += i_dst;
        }
        break;
    case 16:
        T = _mm_set1_epi8((char_t)iDCValue);
        for (y = 0; y < iHeight; y++) {
            _mm_storeu_si128((__m128i*)(dst), T);
            dst += i_dst;
        }
        break;
    case 32:
        T = _mm_set1_epi8((char_t)iDCValue);
        for (y = 0; y < iHeight; y++) {
            _mm_storeu_si128((__m128i*)(dst + 0), T);
            _mm_storeu_si128((__m128i*)(dst + 16), T);
            dst += i_dst;
        }
        break;
    case 64:
        T = _mm_set1_epi8((char_t)iDCValue);
        for (y = 0; y < iHeight; y++) {
            _mm_storeu_si128((__m128i*)(dst + 0), T);
            _mm_storeu_si128((__m128i*)(dst + 16), T);
            _mm_storeu_si128((__m128i*)(dst + 32), T);
            _mm_storeu_si128((__m128i*)(dst + 48), T);
            dst += i_dst;
        }
        break;
    default:
        assert(0);
        break;
    }
}

void xPredIntraPlaneAdi_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bit_depth)
{
    pel_t  *rpSrc;
    int iH = 0;
    int iV = 0;
    int iA, iB, iC;
    int x, y;
    int iW2 = iWidth >> 1;
    int iH2 = iHeight >> 1;
    int ib_mult[5] = {13, 17, 5, 11, 23};
    int ib_shift[5] = {7, 10, 11, 15, 19};

    int im_h = ib_mult[tab_log2[iWidth] - 2];
    int is_h = ib_shift[tab_log2[iWidth] - 2];
    int im_v = ib_mult[tab_log2[iHeight] - 2];
    int is_v = ib_shift[tab_log2[iHeight] - 2];

    int iTmp;
    __m128i TC, TB, TA, T_Start, T, D, D1;

    rpSrc = pSrc + iW2;
    for (x = 1; x < iW2 + 1; x++) {
        iH += x * (rpSrc[x] - rpSrc[-x]);
    }

    rpSrc = pSrc - iH2;
    for (y = 1; y < iH2 + 1; y++) {
        iV += y * (rpSrc[-y] - rpSrc[y]);
    }

    iA = (pSrc[-1 - (iHeight - 1)] + pSrc[1 + iWidth - 1]) << 4;
    iB = ((iH << 5) * im_h + (1 << (is_h - 1))) >> is_h;
    iC = ((iV << 5) * im_v + (1 << (is_v - 1))) >> is_v;

    iTmp = iA - (iH2 - 1) * iC - (iW2 - 1) * iB + 16;

    TA = _mm_set1_epi16((i16s_t)iTmp);
    TB = _mm_set1_epi16((i16s_t)iB);
    TC = _mm_set1_epi16((i16s_t)iC);
    
    T_Start = _mm_set_epi16(7, 6, 5, 4, 3, 2, 1, 0);
    T_Start = _mm_mullo_epi16(TB, T_Start);
    T_Start = _mm_add_epi16(T_Start, TA);

    TB = _mm_mullo_epi16(TB, _mm_set1_epi16(8));

    if (iWidth <= 8) {
        for (y = 0; y < iHeight; y++) {
            D = _mm_srai_epi16(T_Start, 5);
            D = _mm_packus_epi16(D, D);
            _mm_storel_epi64((__m128i*)dst, D);
            T_Start = _mm_add_epi16(T_Start, TC);
            dst += i_dst;
        }
    } else {
        for (y = 0; y < iHeight; y++) {
            T = T_Start;
            for (x = 0; x < iWidth; x += 16) {
                D  = _mm_srai_epi16(T, 5);
                T  = _mm_add_epi16(T, TB);
                D1 = _mm_srai_epi16(T, 5);
                T  = _mm_add_epi16(T, TB);
                D  = _mm_packus_epi16(D, D1);
                _mm_storeu_si128((__m128i*)(dst + x), D);
            }
            T_Start = _mm_add_epi16(T_Start, TC);
            dst += i_dst;
        }
    }
    
}

void xPredIntraBiAdi_sse128(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bit_depth)
{
    int x, y;
    int ishift_x = tab_log2[iWidth];
    int ishift_y = tab_log2[iHeight];
    int ishift = min(ishift_x, ishift_y);
    int ishift_xy = ishift_x + ishift_y + 1;
    int offset = 1 << (ishift_x + ishift_y);
    int a, b, c, w, val;
    pel_t *p;
    __m128i T, T1, T2, T3, C1, C2, ADD;
    __m128i ZERO = _mm_setzero_si128();

    ALIGNED_16(i16s_t pTop[LCU_SIZE + 16]);
    ALIGNED_16(i16s_t pLeft[LCU_SIZE + 16]);
    ALIGNED_16(i16s_t pT[LCU_SIZE + 16]);
    ALIGNED_16(i16s_t pL[LCU_SIZE + 16]);
    ALIGNED_16(i16s_t wy[LCU_SIZE + 16]);

    p = pSrc + 1;
    for (x = 0; x < iWidth; x += 16) {
        T  = _mm_loadu_si128((__m128i*)(p + x));
        T1 = _mm_unpacklo_epi8(T, ZERO);
        T2 = _mm_unpackhi_epi8(T, ZERO);
        _mm_store_si128((__m128i*)(pTop + x    ), T1);
        _mm_store_si128((__m128i*)(pTop + x + 8), T2);
    }
    for (y = 0; y < iHeight; y++) {
        pLeft[y] = pSrc[-1 - y];
    }

    a = pTop [iWidth  - 1];
    b = pLeft[iHeight - 1];

    if (iWidth == iHeight) {
        c = (a + b + 1) >> 1;
    } else {
        c = (((a << ishift_x) + (b << ishift_y)) * 13 + (1 << (ishift + 5))) >> (ishift + 6);
    }

    w = (c << 1) - a - b;

    T = _mm_set1_epi16((i16s_t)b);
    for (x = 0; x < iWidth; x += 8) {
        T1 = _mm_load_si128((__m128i*)(pTop + x));
        T2 = _mm_sub_epi16(T, T1);
        T1 = _mm_slli_epi16(T1, ishift_y);
        _mm_store_si128((__m128i*)(pT   + x), T2);
        _mm_store_si128((__m128i*)(pTop + x), T1);
    }

    T = _mm_set1_epi16((i16s_t)a);
    for (y = 0; y < iHeight; y += 8) {
        T1 = _mm_load_si128((__m128i*)(pLeft + y));
        T2 = _mm_sub_epi16(T, T1);
        T1 = _mm_slli_epi16(T1, ishift_x);
        _mm_store_si128((__m128i*)(pL    + y), T2);
        _mm_store_si128((__m128i*)(pLeft + y), T1);
    }

    T = _mm_set1_epi16((i16s_t)w);
    T = _mm_mullo_epi16(T, _mm_set_epi16(7, 6, 5, 4, 3, 2, 1, 0));
    T1 = _mm_set1_epi16((i16s_t)(8 * w));

    for (y = 0; y < iHeight; y += 8) {
        _mm_store_si128((__m128i*)(wy + y), T);
        T = _mm_add_epi16(T, T1);
    }

    C1 = _mm_set_epi32(3, 2, 1, 0);
    C2 = _mm_set1_epi32(4);

    if (iWidth == 4) {
        __m128i pTT = _mm_loadl_epi64((__m128i*)pT);
        T = _mm_loadl_epi64((__m128i*)pTop);
        for (y = 0; y < iHeight; y++) {
            int add = (pL[y] << ishift_y) + wy[y];
            ADD = _mm_set1_epi32(add);
            ADD = _mm_mullo_epi32(C1, ADD);

            val = (pLeft[y] << ishift_y) + offset + (pL[y] << ishift_y);

            ADD = _mm_add_epi32(ADD, _mm_set1_epi32(val));
            T = _mm_add_epi16(T, pTT);

            T1 = _mm_cvtepi16_epi32(T);
            T1 = _mm_slli_epi32(T1, ishift_x);

            T1 = _mm_add_epi32(T1, ADD);
            T1 = _mm_srai_epi32(T1, ishift_xy);

            T1 = _mm_packus_epi32(T1, T1);
            T1 = _mm_packus_epi16(T1, T1);

            M32(dst) = _mm_cvtsi128_si32(T1);

            dst += i_dst;
        }
    } else if (iWidth == 8) {
        __m128i pTT = _mm_load_si128((__m128i*)pT);
        T = _mm_load_si128((__m128i*)pTop);
        for (y = 0; y < iHeight; y++) {
            int add = (pL[y] << ishift_y) + wy[y];
            ADD = _mm_set1_epi32(add);
            T3 = _mm_mullo_epi32(C2, ADD);
            ADD = _mm_mullo_epi32(C1, ADD);

            val = (pLeft[y] << ishift_y) + offset + (pL[y] << ishift_y);

            ADD = _mm_add_epi32(ADD, _mm_set1_epi32(val));

            T = _mm_add_epi16(T, pTT);

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

            _mm_storel_epi64((__m128i*)dst, T1);

            dst += i_dst;
        }
    } else {
        __m128i TT[16];
        __m128i PTT[16];
        for (x = 0; x < iWidth; x += 8) {
            int idx = x >> 2;
            __m128i M0 = _mm_load_si128((__m128i*)(pTop + x));
            __m128i M1 = _mm_load_si128((__m128i*)(pT + x));
            TT[idx    ] = _mm_unpacklo_epi16(M0, ZERO);
            TT[idx + 1] = _mm_unpackhi_epi16(M0, ZERO);
            PTT[idx    ] = _mm_cvtepi16_epi32(M1);
            PTT[idx + 1] = _mm_cvtepi16_epi32(_mm_srli_si128(M1, 8));
        }
        for (y = 0; y < iHeight; y++) {
            int add = (pL[y] << ishift_y) + wy[y];
            ADD = _mm_set1_epi32(add);
            T3 = _mm_mullo_epi32(C2, ADD);
            ADD = _mm_mullo_epi32(C1, ADD);

            val = (pLeft[y] << ishift_y) + offset + (pL[y] << ishift_y);

            ADD = _mm_add_epi32(ADD, _mm_set1_epi32(val));

            for (x = 0; x < iWidth; x += 8) {
                int idx = x >> 2;
                TT[idx    ] = _mm_add_epi32(TT[idx    ], PTT[idx    ]);
                TT[idx + 1] = _mm_add_epi32(TT[idx + 1], PTT[idx + 1]);

                T1 = _mm_slli_epi32(TT[idx    ], ishift_x);
                T2 = _mm_slli_epi32(TT[idx + 1], ishift_x);

                T1 = _mm_add_epi32(T1, ADD);
                T1 = _mm_srai_epi32(T1, ishift_xy);
                ADD = _mm_add_epi32(ADD, T3);

                T2 = _mm_add_epi32(T2, ADD);
                T2 = _mm_srai_epi32(T2, ishift_xy);
                ADD = _mm_add_epi32(ADD, T3);

                T1 = _mm_packus_epi32(T1, T2);
                T1 = _mm_packus_epi16(T1, T1);

                _mm_storel_epi64((__m128i*)(dst + x), T1);
            }
            dst += i_dst;
        }
    }
}

void xPredIntraDCAdi_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bAboveAvail, int bLeftAvail, int bit_depth)
{
	int   x, y;
	int   iDCValue = 0;
	pel_t  *rpSrc = pSrc - 1;
	int h_bitsize = tab_log2[iHeight];
	int w_bitsize = tab_log2[iWidth];
	int half_height = iHeight >> 1;
	int half_width = iWidth >> 1;
	__m128i T;
	i64u_t v64;

	if (bLeftAvail) {
		for (y = 0; y < iHeight; y++) {
			iDCValue += rpSrc[-y];
		}

		rpSrc = pSrc + 1;
		if (bAboveAvail) {
			for (x = 0; x < iWidth; x++) {
				iDCValue += rpSrc[x];
			}

			iDCValue += ((iWidth + iHeight) >> 1);
			iDCValue = (iDCValue * (512 / (iWidth + iHeight))) >> 9;
		}
		else {
			iDCValue += half_height;
			iDCValue >>= h_bitsize;
		}
	}
	else {
		rpSrc = pSrc + 1;
		if (bAboveAvail) {
			for (x = 0; x < iWidth; x++) {
				iDCValue += rpSrc[x];
			}

			iDCValue += half_width;
			iDCValue >>= w_bitsize;
		}
		else {
			iDCValue = 1 << (bit_depth - 1);
		}
	}

	switch (iWidth) {
	case 4:
		v64 = 0x0001000100010001 * iDCValue;
		for (y = 0; y < iHeight; y++) {
			M64(dst) = v64;
			dst += i_dst;
		}
		break;
	case 8:
		T = _mm_set1_epi16((pel_t)iDCValue);
		for (y = 0; y < iHeight; y++) {
			_mm_storeu_si128((__m128i*)(dst), T);
			dst += i_dst;
		}
		break;
	case 16:
		T = _mm_set1_epi16((pel_t)iDCValue);
		for (y = 0; y < iHeight; y++) {
			_mm_storeu_si128((__m128i*)(dst + 0), T);
			_mm_storeu_si128((__m128i*)(dst + 8), T);
			dst += i_dst;
		}
		break;
	case 32:
		T = _mm_set1_epi16((pel_t)iDCValue);
		for (y = 0; y < iHeight; y++) {
			_mm_storeu_si128((__m128i*)(dst + 0), T);
			_mm_storeu_si128((__m128i*)(dst + 8), T);
			_mm_storeu_si128((__m128i*)(dst + 16), T);
			_mm_storeu_si128((__m128i*)(dst + 24), T);
			dst += i_dst;
		}
		break;
	case 64:
		T = _mm_set1_epi16((pel_t)iDCValue);
		for (y = 0; y < iHeight; y++) {
			_mm_storeu_si128((__m128i*)(dst + 0), T);
			_mm_storeu_si128((__m128i*)(dst + 8), T);
			_mm_storeu_si128((__m128i*)(dst + 16), T);
			_mm_storeu_si128((__m128i*)(dst + 24), T);
			_mm_storeu_si128((__m128i*)(dst + 32), T);
			_mm_storeu_si128((__m128i*)(dst + 40), T);
			_mm_storeu_si128((__m128i*)(dst + 48), T);
			_mm_storeu_si128((__m128i*)(dst + 56), T);
			dst += i_dst;
		}
		break;
	default:
		assert(0);
		break;
	}
}

void xPredIntraVertAdi_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight)
{
    int y;
    pel_t *rpSrc = pSrc + 1;
    __m128i T1, T2, T3, T4;
	__m128i M1, M2, M3, M4;

    switch (iWidth) {
    case 4:
        for (y = 0; y < iHeight; y += 2) {
            CP64(dst,         rpSrc);
            CP64(dst + i_dst, rpSrc);
            dst += i_dst << 1;
        }
        break;
    case 8:
		T1 = _mm_loadu_si128((__m128i*)rpSrc);
		for (y = 0; y < iHeight; y++) {
			_mm_storeu_si128((__m128i*)(dst), T1);
			dst += i_dst;
		}
        break;
    case 16:
		T1 = _mm_loadu_si128((__m128i*)(rpSrc + 0));
		T2 = _mm_loadu_si128((__m128i*)(rpSrc + 8));
		for (y = 0; y < iHeight; y++) {
			_mm_storeu_si128((__m128i*)(dst + 0), T1);
			_mm_storeu_si128((__m128i*)(dst + 8), T2);
			dst += i_dst;
		}
        break;
    case 32:
		T1 = _mm_loadu_si128((__m128i*)(rpSrc + 0));
		T2 = _mm_loadu_si128((__m128i*)(rpSrc + 8));
		T3 = _mm_loadu_si128((__m128i*)(rpSrc + 16));
		T4 = _mm_loadu_si128((__m128i*)(rpSrc + 24));
		for (y = 0; y < iHeight; y++) {
			_mm_storeu_si128((__m128i*)(dst + 0), T1);
			_mm_storeu_si128((__m128i*)(dst + 8), T2);
			_mm_storeu_si128((__m128i*)(dst + 16), T3);
			_mm_storeu_si128((__m128i*)(dst + 24), T4);
			dst += i_dst;
		}
        break;
    case 64:
		T1 = _mm_loadu_si128((__m128i*)(rpSrc + 0));
		T2 = _mm_loadu_si128((__m128i*)(rpSrc + 8));
		T3 = _mm_loadu_si128((__m128i*)(rpSrc + 16));
		T4 = _mm_loadu_si128((__m128i*)(rpSrc + 24));
		M1 = _mm_loadu_si128((__m128i*)(rpSrc + 32));
		M2 = _mm_loadu_si128((__m128i*)(rpSrc + 40));
		M3 = _mm_loadu_si128((__m128i*)(rpSrc + 48));
		M4 = _mm_loadu_si128((__m128i*)(rpSrc + 56));
		for (y = 0; y < iHeight; y++) {
			_mm_storeu_si128((__m128i*)(dst + 0), T1);
			_mm_storeu_si128((__m128i*)(dst + 8), T2);
			_mm_storeu_si128((__m128i*)(dst + 16), T3);
			_mm_storeu_si128((__m128i*)(dst + 24), T4);
			_mm_storeu_si128((__m128i*)(dst + 32), M1);
			_mm_storeu_si128((__m128i*)(dst + 40), M2);
			_mm_storeu_si128((__m128i*)(dst + 48), M3);
			_mm_storeu_si128((__m128i*)(dst + 56), M4);
			dst += i_dst;
		}
        break;
    default:
        assert(0);
        break;
    }
}

void xPredIntraHorAdi_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight)
{
    int y;
    pel_t *rpSrc = pSrc - 1;
    __m128i T;

    switch (iWidth) {
    case 4:
        for (y = 0; y < iHeight; y++) {
			M64(dst) = 0x0001000100010001 * rpSrc[-y];
			dst += i_dst;
        }
        break;
    case 8:
		for (y = 0; y < iHeight; y++) {
			T = _mm_set1_epi16((pel_t)rpSrc[-y]);
			_mm_storeu_si128((__m128i*)(dst), T);
			dst += i_dst;
		}
        break;
    case 16:
		for (y = 0; y < iHeight; y++) {
			T = _mm_set1_epi16((pel_t)rpSrc[-y]);
			_mm_storeu_si128((__m128i*)(dst + 0), T);
			_mm_storeu_si128((__m128i*)(dst + 8), T);
			dst += i_dst;
		}
        break;
    case 32:
        for (y = 0; y < iHeight; y++) {
			T = _mm_set1_epi16((pel_t)rpSrc[-y]);
            _mm_storeu_si128((__m128i*)(dst +  0), T);
            _mm_storeu_si128((__m128i*)(dst +  8), T);
            _mm_storeu_si128((__m128i*)(dst + 16), T);
            _mm_storeu_si128((__m128i*)(dst + 24), T);
            dst += i_dst;
        }
        break;
    case 64:
        for (y = 0; y < iHeight; y++) {
			T = _mm_set1_epi16((pel_t)rpSrc[-y]);
            _mm_storeu_si128((__m128i*)(dst +  0), T);
            _mm_storeu_si128((__m128i*)(dst +  8), T);
            _mm_storeu_si128((__m128i*)(dst + 16), T);
            _mm_storeu_si128((__m128i*)(dst + 24), T);
			_mm_storeu_si128((__m128i*)(dst + 32), T);
			_mm_storeu_si128((__m128i*)(dst + 40), T);
			_mm_storeu_si128((__m128i*)(dst + 48), T);
			_mm_storeu_si128((__m128i*)(dst + 56), T);
            dst += i_dst;
        }
        break;
    default:
        assert(0);
        break;
    }
}

void xPredIntraPlaneAdi_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bit_depth)
{
	pel_t  *rpSrc;
	int iH = 0;
	int iV = 0;
	int iA, iB, iC;
	int x, y;
	int iW2 = iWidth >> 1;
	int iH2 = iHeight >> 1;
	int ib_mult[5] = { 13, 17, 5, 11, 23 };
	int ib_shift[5] = { 7, 10, 11, 15, 19 };
	int max_pixel = (1 << bit_depth) - 1;
	__m128i max_val = _mm_set1_epi16((pel_t)max_pixel);

	int im_h = ib_mult[tab_log2[iWidth] - 2];
	int is_h = ib_shift[tab_log2[iWidth] - 2];
	int im_v = ib_mult[tab_log2[iHeight] - 2];
	int is_v = ib_shift[tab_log2[iHeight] - 2];

	int iTmp;
	__m128i TC, TB, TA, T_Start, T, D, D1;

	rpSrc = pSrc + iW2;
	for (x = 1; x < iW2 + 1; x++) {
		iH += x * (rpSrc[x] - rpSrc[-x]);
	}

	rpSrc = pSrc - iH2;
	for (y = 1; y < iH2 + 1; y++) {
		iV += y * (rpSrc[-y] - rpSrc[y]);
	}

	iA = (pSrc[-1 - (iHeight - 1)] + pSrc[1 + iWidth - 1]) << 4;
	iB = ((iH << 5) * im_h + (1 << (is_h - 1))) >> is_h;
	iC = ((iV << 5) * im_v + (1 << (is_v - 1))) >> is_v;

	iTmp = iA - (iH2 - 1) * iC - (iW2 - 1) * iB + 16;

	TA = _mm_set1_epi32((i16s_t)iTmp);
	TB = _mm_set1_epi32((i16s_t)iB);
	TC = _mm_set1_epi32((i16s_t)iC);

	T_Start = _mm_set_epi32(3, 2, 1, 0);
	T_Start = _mm_mullo_epi32(TB, T_Start);
	T_Start = _mm_add_epi32(T_Start, TA);

	TB = _mm_slli_epi32(TB, 2);

	if (iWidth <= 4) {
		for (y = 0; y < iHeight; y++) {
			D = _mm_srai_epi32(T_Start, 5);
			D = _mm_packus_epi32(D, D);
			D = _mm_min_epu16(D, max_val);
			_mm_storel_epi64((__m128i*)dst, D);
			T_Start = _mm_add_epi32(T_Start, TC);
			dst += i_dst;
		}
	}
	else
	{
		for (y = 0; y < iHeight; y++) {
			T = T_Start;
			for (x = 0; x < iWidth; x += 8) {
				D = _mm_srai_epi32(T, 5);
				T = _mm_add_epi32(T, TB);
				D1 = _mm_srai_epi32(T, 5);
				T = _mm_add_epi32(T, TB);
				D = _mm_packus_epi32(D, D1);
				D = _mm_min_epu16(D, max_val);
				_mm_storeu_si128((__m128i*)(dst + x), D);
			}
			T_Start = _mm_add_epi32(T_Start, TC);
			dst += i_dst;
		}
	}
}

void xPredIntraBiAdi_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bit_depth)
{
	int x, y;
	int ishift_x = tab_log2[iWidth];
	int ishift_y = tab_log2[iHeight];
	int ishift = min(ishift_x, ishift_y);
	int ishift_xy = ishift_x + ishift_y + 1;
	int offset = 1 << (ishift_x + ishift_y);
	int a, b, c, w, val;
	pel_t *p;
	__m128i T, T1, T2, T3, C1, C2, ADD;
	__m128i ZERO = _mm_setzero_si128();
	__m128i shuff = _mm_setr_epi8(14, 15, 12, 13, 10, 11, 8, 9, 6, 7, 4, 5, 2, 3, 0, 1);
	int max_pixel = (1 << bit_depth) - 1;
	__m128i max_val = _mm_set1_epi16(max_pixel);

	ALIGNED_16(i16s_t pTop[LCU_SIZE + 16]);
	ALIGNED_16(i16s_t pLeft[LCU_SIZE + 16]);
	ALIGNED_16(i16s_t pT[LCU_SIZE + 16]);
	ALIGNED_16(i16s_t pL[LCU_SIZE + 16]);
	ALIGNED_16(i16s_t wy[LCU_SIZE + 16]);

	a = pSrc[iWidth];
	b = pSrc[-iHeight];

	c = (iWidth == iHeight) ? (a + b + 1) >> 1 : (((a << ishift_x) + (b << ishift_y)) * 13 + (1 << (ishift + 5))) >> (ishift + 6);
	w = (c << 1) - a - b;

	T = _mm_set1_epi16((i16s_t)b);
    p = pSrc + 1;

	for (x = 0; x < iWidth; x += 8) {
        T1 = _mm_loadu_si128((__m128i*)(p + x));
		T2 = _mm_sub_epi16(T, T1);
		T1 = _mm_slli_epi16(T1, ishift_y);
		_mm_store_si128((__m128i*)(pT + x), T2);
		_mm_store_si128((__m128i*)(pTop + x), T1);
	}

	T = _mm_set1_epi16((i16s_t)a);
    p = pSrc - 8;

	for (y = 0; y < iHeight; y += 8) {
		T1 = _mm_loadu_si128((__m128i*)(p - y));
        T1 = _mm_shuffle_epi8(T1, shuff);
		T2 = _mm_sub_epi16(T, T1);
		T1 = _mm_slli_epi16(T1, ishift_x);
		_mm_store_si128((__m128i*)(pL + y), T2);
		_mm_store_si128((__m128i*)(pLeft + y), T1);
	}

	T = _mm_set1_epi16((i16s_t)w);
	T = _mm_mullo_epi16(T, _mm_set_epi16(7, 6, 5, 4, 3, 2, 1, 0));
	T1 = _mm_set1_epi16((i16s_t)(8 * w));

	for (y = 0; y < iHeight; y += 8) {
		_mm_store_si128((__m128i*)(wy + y), T);
		T = _mm_add_epi16(T, T1);
	}

	C1 = _mm_set_epi32(3, 2, 1, 0);
	C2 = _mm_set1_epi32(4);

	if (iWidth == 4) {
		__m128i pTT = _mm_loadl_epi64((__m128i*)pT);
		T = _mm_loadl_epi64((__m128i*)pTop);
		for (y = 0; y < iHeight; y++) {
			int add = (pL[y] << ishift_y) + wy[y];
			ADD = _mm_set1_epi32(add);
			ADD = _mm_mullo_epi32(C1, ADD);

			val = (pLeft[y] << ishift_y) + offset + (pL[y] << ishift_y);

			ADD = _mm_add_epi32(ADD, _mm_set1_epi32(val));
			T = _mm_add_epi16(T, pTT);

			T1 = _mm_cvtepi16_epi32(T);
			T1 = _mm_slli_epi32(T1, ishift_x);

			T1 = _mm_add_epi32(T1, ADD);
			T1 = _mm_srai_epi32(T1, ishift_xy);

			T1 = _mm_packus_epi32(T1, T1);
			T1 = _mm_min_epu16(T1, max_val);
			_mm_storel_epi64((__m128i*)dst, T1);

			dst += i_dst;
		}
	}
	else if (iWidth == 8) {
		__m128i pTT = _mm_load_si128((__m128i*)pT);
		T = _mm_load_si128((__m128i*)pTop);
		for (y = 0; y < iHeight; y++) {
			int add = (pL[y] << ishift_y) + wy[y];
			ADD = _mm_set1_epi32(add);
			T3 = _mm_mullo_epi32(C2, ADD);
			ADD = _mm_mullo_epi32(C1, ADD);

			val = (pLeft[y] << ishift_y) + offset + (pL[y] << ishift_y);

			ADD = _mm_add_epi32(ADD, _mm_set1_epi32(val));

			T = _mm_add_epi16(T, pTT);

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
			T1 = _mm_min_epu16(T1, max_val);
			_mm_storeu_si128((__m128i*)dst, T1);

			dst += i_dst;
		}
	}
	else {
		__m128i TT[16];
		__m128i PTT[16];
		for (x = 0; x < iWidth; x += 8) {
			int idx = x >> 2;
			__m128i M0 = _mm_load_si128((__m128i*)(pTop + x));
			__m128i M1 = _mm_load_si128((__m128i*)(pT + x));
			TT[idx] = _mm_unpacklo_epi16(M0, ZERO);
			TT[idx + 1] = _mm_unpackhi_epi16(M0, ZERO);
			PTT[idx] = _mm_cvtepi16_epi32(M1);
			PTT[idx + 1] = _mm_cvtepi16_epi32(_mm_srli_si128(M1, 8));
		}
		for (y = 0; y < iHeight; y++) {
			int add = (pL[y] << ishift_y) + wy[y];
			ADD = _mm_set1_epi32(add);
			T3 = _mm_mullo_epi32(C2, ADD);
			ADD = _mm_mullo_epi32(C1, ADD);

			val = ((i16u_t)pLeft[y] << ishift_y) + offset + (pL[y] << ishift_y);

			ADD = _mm_add_epi32(ADD, _mm_set1_epi32(val));

			for (x = 0; x < iWidth; x += 8) {
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
				T1 = _mm_min_epu16(T1, max_val);
				_mm_storeu_si128((__m128i*)(dst + x), T1);
			}
			dst += i_dst;
		}
	}
}

void xPredIntraAngAdi_X_4_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[64 + 128]);
	int line_size = iWidth + (iHeight - 1) * 2;
	int real_size = min(line_size, iWidth * 2 - 1);
	int iHeight2 = iHeight * 2;
	int i;
	__m128i offset = _mm_set1_epi16(2);

	pSrc += 3;

	for (i = 0; i < real_size; i += 8, pSrc += 8)
	{
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

		__m128i sum1 = _mm_add_epi16(S0, S1);
		__m128i sum2 = _mm_add_epi16(S1, S2);
		sum1 = _mm_add_epi16(sum1, sum2);

		sum1 = _mm_add_epi16(sum1, offset);
		sum1 = _mm_srli_epi16(sum1, 2);

		_mm_store_si128((__m128i*)&first_line[i], sum1);
	}

	// padding
	for (i = real_size; i < line_size; i += 8) {
		__m128i pad = _mm_set1_epi16((pel_t)first_line[real_size - 1]);
		_mm_storeu_si128((__m128i*)&first_line[i], pad);
	}

	for (i = 0; i < iHeight2; i += 2) {
		memcpy(dst, first_line + i, iWidth * sizeof(pel_t));
		dst += i_dst;
	}
}

void xPredIntraAngAdi_X_6_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[64 + 64]);
	int line_size = iWidth + iHeight - 1;
	int real_size = min(line_size, iWidth * 2);
	int i;
	__m128i offset = _mm_set1_epi16(2);
	pSrc += 2;

	for (i = 0; i < real_size; i += 8, pSrc += 8)
	{
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

		__m128i sum1 = _mm_add_epi16(S0, S1);
		__m128i sum2 = _mm_add_epi16(S1, S2);
		sum1 = _mm_add_epi16(sum1, sum2);

		sum1 = _mm_add_epi16(sum1, offset);
		sum1 = _mm_srli_epi16(sum1, 2);

		_mm_store_si128((__m128i*)&first_line[i], sum1);
	}


	// padding
	for (i = real_size; i < line_size; i += 8) {
		__m128i pad = _mm_set1_epi16((pel_t)first_line[real_size - 1]);
		_mm_storeu_si128((__m128i*)&first_line[i], pad);
	}

	for (i = 0; i < iHeight; i++) {
		memcpy(dst, first_line + i, iWidth * sizeof(pel_t));
		dst += i_dst;
	}

}

void xPredIntraAngAdi_X_8_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[2 * (64 + 48)]);
	int line_size = iWidth + iHeight / 2 - 1;
	int real_size = min(line_size, iWidth * 2 + 1);
	int i;
	__m128i pad1, pad2;
	int aligned_line_size = ((line_size + 31) >> 4) << 4;
	pel_t *pfirst[2];

	__m128i coeff = _mm_set1_epi16(3);
	__m128i offset1 = _mm_set1_epi16(4);
	__m128i offset2 = _mm_set1_epi16(2);
	int i_dst2 = i_dst * 2;

	pfirst[0] = first_line;
	pfirst[1] = first_line + aligned_line_size;

	for (i = 0; i < real_size; i += 8, pSrc += 8)
	{
		__m128i p01, p02, p11;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
		__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 3));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 2));

		p11 = _mm_add_epi16(S1, S2);
		p01 = _mm_mullo_epi16(p11, coeff);
		p02 = _mm_add_epi16(S0, S3);
		p02 = _mm_add_epi16(p02, offset1);
		p01 = _mm_add_epi16(p01, p02);
		p01 = _mm_srli_epi16(p01, 3);

		_mm_store_si128((__m128i*)&pfirst[0][i], p01);

		p02 = _mm_add_epi16(S2, S3);
		p01 = _mm_add_epi16(p11, p02);

		p01 = _mm_add_epi16(p01, offset2);
		p01 = _mm_srli_epi16(p01, 2);

		_mm_store_si128((__m128i*)&pfirst[1][i], p01);
	}

	// padding
	if (real_size < line_size) {
		pfirst[1][real_size - 1] = pfirst[1][real_size - 2];

		pad1 = _mm_set1_epi16((pel_t)pfirst[0][real_size - 1]);
		pad2 = _mm_set1_epi16((pel_t)pfirst[1][real_size - 1]);
		for (i = real_size; i < line_size; i += 8) {
			_mm_storeu_si128((__m128i*)&pfirst[0][i], pad1);
			_mm_storeu_si128((__m128i*)&pfirst[1][i], pad2);
		}
	}

	iHeight /= 2;

	for (i = 0; i < iHeight; i++) {
		memcpy(dst, pfirst[0] + i, iWidth * sizeof(pel_t));
		memcpy(dst + i_dst, pfirst[1] + i, iWidth * sizeof(pel_t));
		dst += i_dst * 2;
	}
}

void xPredIntraAngAdi_X_10_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	int i;
	pel_t *dst1 = dst;
	pel_t *dst2 = dst1 + i_dst;
	pel_t *dst3 = dst2 + i_dst;
	pel_t *dst4 = dst3 + i_dst;
	__m128i coeff2 = _mm_set1_epi16(2);
	__m128i coeff3 = _mm_set1_epi16(3);
	__m128i coeff4 = _mm_set1_epi16(4);
	__m128i coeff5 = _mm_set1_epi16(5);
	__m128i coeff7 = _mm_set1_epi16(7);
	__m128i coeff8 = _mm_set1_epi16(8);

	if (iHeight != 4) {
		ALIGNED_16(pel_t first_line[4 * (64 + 32)]);
		int line_size = iWidth + iHeight / 4 - 1;
		int aligned_line_size = ((line_size + 31) >> 4) << 4;
		pel_t *pfirst[4];

		pfirst[0] = first_line;
		pfirst[1] = first_line + aligned_line_size;
		pfirst[2] = first_line + aligned_line_size * 2;
		pfirst[3] = first_line + aligned_line_size * 3;

		for (i = 0; i < line_size; i += 8, pSrc += 8)
		{
			__m128i p00, p10, p20, p30;
			__m128i p01;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 3));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc + 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 2));

			p00 = _mm_mullo_epi16(S0, coeff3);
			p10 = _mm_mullo_epi16(S1, coeff7);
			p20 = _mm_mullo_epi16(S2, coeff5);
			p30 = _mm_add_epi16(S3, coeff8);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_srli_epi16(p00, 4);

			_mm_store_si128((__m128i*)&pfirst[0][i], p00);

			p01 = _mm_add_epi16(S1, S2);
			p00 = _mm_mullo_epi16(p01, coeff3);
			p10 = _mm_add_epi16(S0, S3);
			p10 = _mm_add_epi16(p10, coeff4);
			p00 = _mm_add_epi16(p10, p00);
			p00 = _mm_srli_epi16(p00, 3);

			_mm_store_si128((__m128i*)&pfirst[1][i], p00);

			p10 = _mm_mullo_epi16(S1, coeff5);
			p20 = _mm_mullo_epi16(S2, coeff7);
			p30 = _mm_mullo_epi16(S3, coeff3);
			p00 = _mm_add_epi16(S0, coeff8);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_srli_epi16(p00, 4);

			_mm_store_si128((__m128i*)&pfirst[2][i], p00);

			p10 = _mm_add_epi16(S2, S3);
			p00 = _mm_add_epi16(p01, p10);
			p00 = _mm_add_epi16(p00, coeff2);
			p00 = _mm_srli_epi16(p00, 2);

			_mm_store_si128((__m128i*)&pfirst[3][i], p00);
		}

		iHeight >>= 2;


		int i_dstx4 = i_dst << 2;
		switch (iWidth) {
		case 4:
			for (i = 0; i < iHeight; i++) {
				CP64(dst1, pfirst[0] + i); dst1 += i_dstx4;
				CP64(dst2, pfirst[1] + i); dst2 += i_dstx4;
				CP64(dst3, pfirst[2] + i); dst3 += i_dstx4;
				CP64(dst4, pfirst[3] + i); dst4 += i_dstx4;
			}
			break;
		case 8:
			for (i = 0; i < iHeight; i++) {
				CP128(dst1, pfirst[0] + i); dst1 += i_dstx4;
				CP128(dst2, pfirst[1] + i); dst2 += i_dstx4;
				CP128(dst3, pfirst[2] + i); dst3 += i_dstx4;
				CP128(dst4, pfirst[3] + i); dst4 += i_dstx4;
			}
			break;
		case 16:
			for (i = 0; i < iHeight; i++) {
				memcpy(dst1, pfirst[0] + i, 32 * sizeof(pel_t)); dst1 += i_dstx4;
				memcpy(dst2, pfirst[1] + i, 32 * sizeof(pel_t)); dst2 += i_dstx4;
				memcpy(dst3, pfirst[2] + i, 32 * sizeof(pel_t)); dst3 += i_dstx4;
				memcpy(dst4, pfirst[3] + i, 32 * sizeof(pel_t)); dst4 += i_dstx4;
			}
			break;
		case 32:
			for (i = 0; i < iHeight; i++) {
				memcpy(dst1, pfirst[0] + i, 32 * sizeof(pel_t)); dst1 += i_dstx4;
				memcpy(dst2, pfirst[1] + i, 32 * sizeof(pel_t)); dst2 += i_dstx4;
				memcpy(dst3, pfirst[2] + i, 32 * sizeof(pel_t)); dst3 += i_dstx4;
				memcpy(dst4, pfirst[3] + i, 32 * sizeof(pel_t)); dst4 += i_dstx4;
			}
			break;
		case 64:
			for (i = 0; i < iHeight; i++) {
				memcpy(dst1, pfirst[0] + i, 64 * sizeof(pel_t)); dst1 += i_dstx4;
				memcpy(dst2, pfirst[1] + i, 64 * sizeof(pel_t)); dst2 += i_dstx4;
				memcpy(dst3, pfirst[2] + i, 64 * sizeof(pel_t)); dst3 += i_dstx4;
				memcpy(dst4, pfirst[3] + i, 64 * sizeof(pel_t)); dst4 += i_dstx4;
			}
			break;
		default:
			assert(0);
			break;
		}

	}
	else
	{
		if (iWidth == 16)
		{
			__m128i p00, p10, p20, p30;
			__m128i p01;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 3));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc + 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 2));

			p00 = _mm_mullo_epi16(S0, coeff3);
			p10 = _mm_mullo_epi16(S1, coeff7);
			p20 = _mm_mullo_epi16(S2, coeff5);
			p30 = _mm_add_epi16(S3, coeff8);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_srli_epi16(p00, 4);

			_mm_store_si128((__m128i*)dst1, p00);

			p01 = _mm_add_epi16(S1, S2);
			p00 = _mm_mullo_epi16(p01, coeff3);
			p10 = _mm_add_epi16(S0, S3);
			p10 = _mm_add_epi16(p10, coeff4);
			p00 = _mm_add_epi16(p10, p00);
			p00 = _mm_srli_epi16(p00, 3);

			_mm_store_si128((__m128i*)dst2, p00);

			p10 = _mm_mullo_epi16(S1, coeff5);
			p20 = _mm_mullo_epi16(S2, coeff7);
			p30 = _mm_mullo_epi16(S3, coeff3);
			p00 = _mm_add_epi16(S0, coeff8);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_srli_epi16(p00, 4);

			_mm_store_si128((__m128i*)dst3, p00);

			p10 = _mm_add_epi16(S2, S3);
			p00 = _mm_add_epi16(p01, p10);
			p00 = _mm_add_epi16(p00, coeff2);
			p00 = _mm_srli_epi16(p00, 2);

			_mm_store_si128((__m128i*)dst4, p00);

			pSrc += 8;
			S0 = _mm_loadu_si128((__m128i*)(pSrc));
			S3 = _mm_loadu_si128((__m128i*)(pSrc + 3));
			S1 = _mm_loadu_si128((__m128i*)(pSrc + 1));
			S2 = _mm_loadu_si128((__m128i*)(pSrc + 2));

			p00 = _mm_mullo_epi16(S0, coeff3);
			p10 = _mm_mullo_epi16(S1, coeff7);
			p20 = _mm_mullo_epi16(S2, coeff5);
			p30 = _mm_add_epi16(S3, coeff8);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_srli_epi16(p00, 4);

			_mm_store_si128((__m128i*)(dst1+8), p00);

			p00 = _mm_add_epi16(S1, S2);
			p00 = _mm_mullo_epi16(p00, coeff3);
			p10 = _mm_add_epi16(S0, S3);
			p10 = _mm_add_epi16(p10, coeff4);
			p00 = _mm_add_epi16(p10, p00);
			p00 = _mm_srli_epi16(p00, 3);

			_mm_store_si128((__m128i*)(dst2+8), p00);

			p10 = _mm_mullo_epi16(S1, coeff5);
			p20 = _mm_mullo_epi16(S2, coeff7);
			p30 = _mm_mullo_epi16(S3, coeff3);
			p00 = _mm_add_epi16(S0, coeff8);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_srli_epi16(p00, 4);

			_mm_store_si128((__m128i*)(dst3+8), p00);

			p00 = _mm_add_epi16(S1, S2);
			p10 = _mm_add_epi16(S2, S3);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, coeff2);
			p00 = _mm_srli_epi16(p00, 2);

			_mm_store_si128((__m128i*)(dst4+8), p00);

		}
		else
		{
			__m128i p00, p10, p20, p30;
			__m128i p01;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 3));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc + 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 2));

			p00 = _mm_mullo_epi16(S0, coeff3);
			p10 = _mm_mullo_epi16(S1, coeff7);
			p20 = _mm_mullo_epi16(S2, coeff5);
			p30 = _mm_add_epi16(S3, coeff8);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_srli_epi16(p00, 4);

			_mm_storel_epi64((__m128i*)dst1, p00);

			p01 = _mm_add_epi16(S1, S2);
			p00 = _mm_mullo_epi16(p01, coeff3);
			p10 = _mm_add_epi16(S0, S3);
			p10 = _mm_add_epi16(p10, coeff4);
			p00 = _mm_add_epi16(p10, p00);
			p00 = _mm_srli_epi16(p00, 3);

			_mm_storel_epi64((__m128i*)dst2, p00);

			p10 = _mm_mullo_epi16(S1, coeff5);
			p20 = _mm_mullo_epi16(S2, coeff7);
			p30 = _mm_mullo_epi16(S3, coeff3);
			p00 = _mm_add_epi16(S0, coeff8);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_srli_epi16(p00, 4);

			_mm_storel_epi64((__m128i*)dst3, p00);

			p10 = _mm_add_epi16(S2, S3);
			p00 = _mm_add_epi16(p01, p10);
			p00 = _mm_add_epi16(p00, coeff2);
			p00 = _mm_srli_epi16(p00, 2);

			_mm_storel_epi64((__m128i*)dst4, p00);
		}
	}
}

void xPredIntraAngAdi_Y_26_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	int i;

	if (iWidth != 4) {
		__m128i coeff2 = _mm_set1_epi16(2);
		__m128i coeff3 = _mm_set1_epi16(3);
		__m128i coeff4 = _mm_set1_epi16(4);
		__m128i coeff5 = _mm_set1_epi16(5);
		__m128i coeff7 = _mm_set1_epi16(7);
		__m128i coeff8 = _mm_set1_epi16(8);
		__m128i shuffle = _mm_setr_epi8(8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7);

		ALIGNED_16(pel_t first_line[64 + 256]);
		int line_size = iWidth + (iHeight - 1) * 4;
		int iHeight4 = iHeight << 2;

		pSrc -= 7;

		for (i = 0; i < line_size; i += 32, pSrc -= 8)
		{
			__m128i p00, p10, p20, p30;
			__m128i M1, M2, M3, M4, M5, M6, M7, M8;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc - 3));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc - 2));

			p00 = _mm_mullo_epi16(S0, coeff3);
			p10 = _mm_mullo_epi16(S1, coeff7);
			p20 = _mm_mullo_epi16(S2, coeff5);
			p30 = _mm_add_epi16(S3, coeff8);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			M1 = _mm_srli_epi16(p00, 4);

			p00 = _mm_add_epi16(S1, S2);
			p00 = _mm_mullo_epi16(p00, coeff3);
			p10 = _mm_add_epi16(S0, S3);
			p10 = _mm_add_epi16(p10, coeff4);
			p00 = _mm_add_epi16(p10, p00);
			M2 = _mm_srli_epi16(p00, 3);

			p10 = _mm_mullo_epi16(S1, coeff5);
			p20 = _mm_mullo_epi16(S2, coeff7);
			p30 = _mm_mullo_epi16(S3, coeff3);
			p00 = _mm_add_epi16(S0, coeff8);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_add_epi16(p00, p30);
			M3 = _mm_srli_epi16(p00, 4);

			p00 = _mm_add_epi16(S1, S2);
			p10 = _mm_add_epi16(S2, S3);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, coeff2);
			M4 = _mm_srli_epi16(p00, 2);

			M5 = _mm_unpacklo_epi16(M1, M2);
			M6 = _mm_unpackhi_epi16(M1, M2);
			M7 = _mm_unpacklo_epi16(M3, M4);
			M8 = _mm_unpackhi_epi16(M3, M4);

			M1 = _mm_unpacklo_epi32(M5, M7);
			M2 = _mm_unpackhi_epi32(M5, M7);
			M3 = _mm_unpacklo_epi32(M6, M8);
			M4 = _mm_unpackhi_epi32(M6, M8);

			M1 = _mm_shuffle_epi8(M1, shuffle);
			M2 = _mm_shuffle_epi8(M2, shuffle);
			M3 = _mm_shuffle_epi8(M3, shuffle);
			M4 = _mm_shuffle_epi8(M4, shuffle);

			_mm_store_si128((__m128i*)&first_line[i], M4);
			_mm_store_si128((__m128i*)&first_line[8 + i], M3);
			_mm_store_si128((__m128i*)&first_line[16 + i], M2);
			_mm_store_si128((__m128i*)&first_line[24 + i], M1);
		}

		switch (iWidth) {
		case 4:
			for (i = 0; i < iHeight4; i += 4) {
				CP64(dst, first_line + i);
				dst += i_dst;
			}
			break;
		case 8:
			for (i = 0; i < iHeight4; i += 4) {
				CP128(dst, first_line + i);
				dst += i_dst;
			}
			break;
		default:
			for (i = 0; i < iHeight4; i += 4) {
				memcpy(dst, first_line + i, iWidth * sizeof(pel_t));
				dst += i_dst;
			}
			break;
		}
	}
	else {
		__m128i coeff2 = _mm_set1_epi16(2);
		__m128i coeff3 = _mm_set1_epi16(3);
		__m128i coeff4 = _mm_set1_epi16(4);
		__m128i coeff5 = _mm_set1_epi16(5);
		__m128i coeff7 = _mm_set1_epi16(7);
		__m128i coeff8 = _mm_set1_epi16(8);

		pSrc -= 7;

		if (iHeight == 4) {
			__m128i p00, p10, p20, p30;
			__m128i M1, M2, M3, M4, M6, M8;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc - 3));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc - 2));

			p00 = _mm_mullo_epi16(S0, coeff3);
			p10 = _mm_mullo_epi16(S1, coeff7);
			p20 = _mm_mullo_epi16(S2, coeff5);
			p30 = _mm_add_epi16(S3, coeff8);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			M1 = _mm_srli_epi16(p00, 4);

			p00 = _mm_add_epi16(S1, S2);
			p00 = _mm_mullo_epi16(p00, coeff3);
			p10 = _mm_add_epi16(S0, S3);
			p10 = _mm_add_epi16(p10, coeff4);
			p00 = _mm_add_epi16(p10, p00);
			M2 = _mm_srli_epi16(p00, 3);

			p10 = _mm_mullo_epi16(S1, coeff5);
			p20 = _mm_mullo_epi16(S2, coeff7);
			p30 = _mm_mullo_epi16(S3, coeff3);
			p00 = _mm_add_epi16(S0, coeff8);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_add_epi16(p00, p30);
			M3 = _mm_srli_epi16(p00, 4);

			p00 = _mm_add_epi16(S1, S2);
			p10 = _mm_add_epi16(S2, S3);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, coeff2);
			M4 = _mm_srli_epi16(p00, 2);

			M6 = _mm_unpackhi_epi16(M1, M2);
			M8 = _mm_unpackhi_epi16(M3, M4);

			M3 = _mm_unpacklo_epi32(M6, M8);
			M4 = _mm_unpackhi_epi32(M6, M8);

			_mm_storel_epi64((__m128i*)(dst + i_dst), M4);
			M4 = _mm_srli_si128(M4, 8);
			_mm_storel_epi64((__m128i*)dst, M4);
			dst += (i_dst << 1);
			_mm_storel_epi64((__m128i*)(dst + i_dst), M3);
			M3 = _mm_srli_si128(M3, 8);
			_mm_storel_epi64((__m128i*)dst, M3);
		}
		else
		{
			__m128i p00, p10, p20, p30;
			__m128i M1, M2, M3, M4, M5, M6, M7, M8;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc - 3));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc - 2));
			__m128i shuffle = _mm_setr_epi8(8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7);

			p00 = _mm_mullo_epi16(S0, coeff3);
			p10 = _mm_mullo_epi16(S1, coeff7);
			p20 = _mm_mullo_epi16(S2, coeff5);
			p30 = _mm_add_epi16(S3, coeff8);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			M1 = _mm_srli_epi16(p00, 4);

			p00 = _mm_add_epi16(S1, S2);
			p00 = _mm_mullo_epi16(p00, coeff3);
			p10 = _mm_add_epi16(S0, S3);
			p10 = _mm_add_epi16(p10, coeff4);
			p00 = _mm_add_epi16(p10, p00);
			M2 = _mm_srli_epi16(p00, 3);

			p10 = _mm_mullo_epi16(S1, coeff5);
			p20 = _mm_mullo_epi16(S2, coeff7);
			p30 = _mm_mullo_epi16(S3, coeff3);
			p00 = _mm_add_epi16(S0, coeff8);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_add_epi16(p00, p30);
			M3 = _mm_srli_epi16(p00, 4);

			p00 = _mm_add_epi16(S1, S2);
			p10 = _mm_add_epi16(S2, S3);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, coeff2);
			M4 = _mm_srli_epi16(p00, 2);

			M5 = _mm_unpacklo_epi16(M1, M2);
			M6 = _mm_unpackhi_epi16(M1, M2);
			M7 = _mm_unpacklo_epi16(M3, M4);
			M8 = _mm_unpackhi_epi16(M3, M4);

			M1 = _mm_unpacklo_epi32(M5, M7);
			M2 = _mm_unpackhi_epi32(M5, M7);
			M3 = _mm_unpacklo_epi32(M6, M8);
			M4 = _mm_unpackhi_epi32(M6, M8);

			int i_dst2 = i_dst << 1;
			_mm_storel_epi64((__m128i*)(dst + i_dst), M4);
			M4 = _mm_srli_si128(M4, 8);
			_mm_storel_epi64((__m128i*)dst, M4);
			dst += i_dst2;
			_mm_storel_epi64((__m128i*)(dst + i_dst), M3);
			M3 = _mm_srli_si128(M3, 8);
			_mm_storel_epi64((__m128i*)dst, M3);
			dst += i_dst2;
			_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
			M2 = _mm_srli_si128(M2, 8);
			_mm_storel_epi64((__m128i*)dst, M2);
			dst += i_dst2;
			_mm_storel_epi64((__m128i*)(dst + i_dst), M1);
			M1 = _mm_srli_si128(M1, 8);
			_mm_storel_epi64((__m128i*)dst, M1);
			dst += i_dst2;

			pSrc -= 8;
			S0 = _mm_loadu_si128((__m128i*)(pSrc));
			S3 = _mm_loadu_si128((__m128i*)(pSrc - 3));
			S1 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			S2 = _mm_loadu_si128((__m128i*)(pSrc - 2));
			shuffle = _mm_setr_epi8(8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7);

			p00 = _mm_mullo_epi16(S0, coeff3);
			p10 = _mm_mullo_epi16(S1, coeff7);
			p20 = _mm_mullo_epi16(S2, coeff5);
			p30 = _mm_add_epi16(S3, coeff8);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			M1 = _mm_srli_epi16(p00, 4);

			p00 = _mm_add_epi16(S1, S2);
			p00 = _mm_mullo_epi16(p00, coeff3);
			p10 = _mm_add_epi16(S0, S3);
			p10 = _mm_add_epi16(p10, coeff4);
			p00 = _mm_add_epi16(p10, p00);
			M2 = _mm_srli_epi16(p00, 3);

			p10 = _mm_mullo_epi16(S1, coeff5);
			p20 = _mm_mullo_epi16(S2, coeff7);
			p30 = _mm_mullo_epi16(S3, coeff3);
			p00 = _mm_add_epi16(S0, coeff8);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_add_epi16(p00, p30);
			M3 = _mm_srli_epi16(p00, 4);

			p00 = _mm_add_epi16(S1, S2);
			p10 = _mm_add_epi16(S2, S3);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, coeff2);
			M4 = _mm_srli_epi16(p00, 2);

			M5 = _mm_unpacklo_epi16(M1, M2);
			M6 = _mm_unpackhi_epi16(M1, M2);
			M7 = _mm_unpacklo_epi16(M3, M4);
			M8 = _mm_unpackhi_epi16(M3, M4);

			M1 = _mm_unpacklo_epi32(M5, M7);
			M2 = _mm_unpackhi_epi32(M5, M7);
			M3 = _mm_unpacklo_epi32(M6, M8);
			M4 = _mm_unpackhi_epi32(M6, M8);

			_mm_storel_epi64((__m128i*)(dst + i_dst), M4);
			M4 = _mm_srli_si128(M4, 8);
			_mm_storel_epi64((__m128i*)dst, M4);
			dst += i_dst2;
			_mm_storel_epi64((__m128i*)(dst + i_dst), M3);
			M3 = _mm_srli_si128(M3, 8);
			_mm_storel_epi64((__m128i*)dst, M3);
			dst += i_dst2;
			_mm_storel_epi64((__m128i*)(dst + i_dst), M2);
			M2 = _mm_srli_si128(M2, 8);
			_mm_storel_epi64((__m128i*)dst, M2);
			dst += i_dst2;
			_mm_storel_epi64((__m128i*)(dst + i_dst), M1);
			M1 = _mm_srli_si128(M1, 8);
			_mm_storel_epi64((__m128i*)dst, M1);

		}
	}
}

void xPredIntraAngAdi_Y_28_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[64 + 128]);
	int line_size = iWidth + (iHeight - 1) * 2;
	int real_size = min(line_size, iHeight * 4 + 1);
	int i;
	int iHeight2 = iHeight << 1;
	__m128i pad;
	__m128i coeff2 = _mm_set1_epi16(2);
	__m128i coeff3 = _mm_set1_epi16(3);
	__m128i coeff4 = _mm_set1_epi16(4);
	__m128i shuffle = _mm_setr_epi8(12, 13, 14, 15, 8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3);

	pSrc -= 7;

	for (i = 0; i < real_size; i += 16, pSrc -= 8)
	{
		__m128i p00, p10, p01, p11;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc));
		__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc - 3));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc - 2));

		p01 = _mm_add_epi16(S1, S2);
		p00 = _mm_mullo_epi16(p01, coeff3);
		p10 = _mm_add_epi16(S0, S3);
		p00 = _mm_adds_epi16(p00, coeff4);
		p00 = _mm_adds_epi16(p00, p10);

		p11 = _mm_add_epi16(S2, S3);
		p01 = _mm_add_epi16(p01, p11);
		p01 = _mm_add_epi16(p01, coeff2);

		p00 = _mm_srli_epi16(p00, 3);
		p01 = _mm_srli_epi16(p01, 2);

		p10 = _mm_unpacklo_epi16(p00, p01);
		p11 = _mm_unpackhi_epi16(p00, p01);

		p10 = _mm_shuffle_epi8(p10, shuffle);
		p11 = _mm_shuffle_epi8(p11, shuffle);

		_mm_store_si128((__m128i*)&first_line[i], p11);
		_mm_store_si128((__m128i*)&first_line[i + 8], p10);
	}


	// padding
	if (real_size < line_size) {
		i = real_size + 1;
		first_line[i - 1] = first_line[i - 3];

		pad = _mm_set1_epi32(((i32u_t*)&first_line[i - 2])[0]);

		for (; i < line_size; i += 8) {
			_mm_storeu_si128((__m128i*)&first_line[i], pad);
		}
	}

	for (i = 0; i < iHeight2; i += 2) {
		memcpy(dst, first_line + i, iWidth * sizeof(pel_t));
		dst += i_dst;
	}
}

void xPredIntraAngAdi_Y_30_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[64 + 64]);
	int line_size = iWidth + iHeight - 1;
	int real_size = min(line_size, iHeight * 2);
	int i;
	__m128i coeff2 = _mm_set1_epi16(2);
	__m128i shuffle = _mm_setr_epi8(14, 15, 12, 13, 10, 11, 8, 9, 6, 7, 4, 5, 2, 3, 0, 1);

	pSrc -= 9;

	for (i = 0; i < real_size; i += 8, pSrc -= 8)
	{
		__m128i p00, p10;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

		p00 = _mm_add_epi16(S0, S1);
		p10 = _mm_add_epi16(S1, S2);

		p00 = _mm_add_epi16(p00, p10);
		p00 = _mm_add_epi16(p00, coeff2);
		p00 = _mm_srli_epi16(p00, 2);

		p00 = _mm_shuffle_epi8(p00, shuffle);

		_mm_store_si128((__m128i*)&first_line[i], p00);
	}

	// padding
	for (i = real_size; i < line_size; i += 8) {
		__m128i pad = _mm_set1_epi16((pel_t)first_line[real_size - 1]);
		_mm_storeu_si128((__m128i*)&first_line[i], pad);
	}

	for (i = 0; i < iHeight; i++) {
		memcpy(dst, first_line + i, iWidth * sizeof(pel_t));
		dst += i_dst;
	}

}

void xPredIntraAngAdi_Y_32_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[2 * (64 + 64)]);
	int line_size = iHeight / 2 + iWidth - 1;
	int real_size = min(line_size, iHeight);
	int i;
	__m128i pad_val;
	int aligned_line_size = ((line_size + 63) >> 4) << 4;
	pel_t *pfirst[2];
	__m128i coeff2 = _mm_set1_epi16(2);
	__m128i shuffle = _mm_setr_epi8(14, 15, 10, 11, 6, 7, 2, 3, 12, 13, 8, 9, 4, 5, 0, 1);
	int i_dst2 = i_dst * 2;

	pfirst[0] = first_line;
	pfirst[1] = first_line + aligned_line_size;

	pSrc -= 10;

	for (i = 0; i < real_size - 4; i += 8, pSrc -= 8)
	{
		__m128i p00, p01, p10, p11;
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

		p00 = _mm_add_epi16(S0, S1);
		p01 = _mm_add_epi16(S1, S2);
		p00 = _mm_add_epi16(p00, coeff2);
		p00 = _mm_add_epi16(p00, p01);
		p00 = _mm_srli_epi16(p00, 2);

		pSrc -= 8;
		S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		S1 = _mm_loadu_si128((__m128i*)(pSrc));

		p10 = _mm_add_epi16(S0, S1);
		p11 = _mm_add_epi16(S1, S2);
		p10 = _mm_add_epi16(p10, coeff2);
		p10 = _mm_add_epi16(p10, p11);
		p10 = _mm_srli_epi16(p10, 2);

		p00 = _mm_shuffle_epi8(p00, shuffle);
		p10 = _mm_shuffle_epi8(p10, shuffle);

		p01 = _mm_unpacklo_epi64(p00, p10);
		p11 = _mm_unpackhi_epi64(p00, p10);

		_mm_store_si128((__m128i*)&pfirst[0][i], p01);
		_mm_store_si128((__m128i*)&pfirst[1][i], p11);
	}

	if (i < real_size)
	{
		__m128i p00, p01;
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

		p00 = _mm_add_epi16(S0, S1);
		p01 = _mm_add_epi16(S1, S2);
		p00 = _mm_add_epi16(p00, coeff2);
		p00 = _mm_add_epi16(p00, p01);
		p00 = _mm_srli_epi16(p00, 2);

		p00 = _mm_shuffle_epi8(p00, shuffle);

		_mm_storel_epi64((__m128i*)&pfirst[0][i], p00);
		p00 = _mm_srli_si128(p00, 8);
		_mm_storel_epi64((__m128i*)&pfirst[1][i], p00);
	}

	// padding
	if (real_size < line_size)
	{
		pad_val = _mm_set1_epi16((pel_t)pfirst[1][real_size - 1]);
		for (i = real_size; i < line_size; i+=8)
		{
			_mm_storeu_si128((__m128i*)&pfirst[0][i], pad_val);
			_mm_storeu_si128((__m128i*)&pfirst[1][i], pad_val);
		}
	}

	iHeight /= 2;

	for (i = 0; i < iHeight; i++) {
		memcpy(dst, pfirst[0] + i, iWidth * sizeof(pel_t));
		memcpy(dst + i_dst, pfirst[1] + i, iWidth * sizeof(pel_t));
		dst += i_dst2;
	}
}

void xPredIntraAngAdi_XY_14_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	int i;
	__m128i coeff2 = _mm_set1_epi16(2);
	__m128i coeff3 = _mm_set1_epi16(3);
	__m128i coeff4 = _mm_set1_epi16(4);
	__m128i coeff5 = _mm_set1_epi16(5);
	__m128i coeff7 = _mm_set1_epi16(7);
	__m128i coeff8 = _mm_set1_epi16(8);

	if (iHeight != 4) {
		ALIGNED_16(pel_t first_line[4 * (64 + 32)]);
		int line_size = iWidth + iHeight / 4 - 1;
		int left_size = line_size - iWidth;
		int aligned_line_size = ((line_size + 31) >> 4) << 4;
		pel_t *pfirst[4];
		__m128i shuffle = _mm_setr_epi8(0, 1, 8, 9, 2, 3, 10, 11, 4, 5, 12, 13, 6, 7, 14, 15);
		pel_t *pSrc1 = pSrc;

		pfirst[0] = first_line;
		pfirst[1] = first_line + aligned_line_size;
		pfirst[2] = first_line + aligned_line_size * 2;
		pfirst[3] = first_line + aligned_line_size * 3;

		pSrc -= iHeight - 4;
		for (i = 0; i < left_size; i += 2, pSrc += 8)
		{
			__m128i p00, p01;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

			p00 = _mm_add_epi16(S0, S1);
			p01 = _mm_add_epi16(S1, S2);
			p00 = _mm_add_epi16(p00, coeff2);
			p00 = _mm_add_epi16(p00, p01);
			p00 = _mm_srli_epi16(p00, 2);

			p00 = _mm_shuffle_epi8(p00, shuffle);

			((int*)&pfirst[0][i])[0] = _mm_extract_epi32(p00, 3);
			((int*)&pfirst[1][i])[0] = _mm_extract_epi32(p00, 2);
			((int*)&pfirst[2][i])[0] = _mm_extract_epi32(p00, 1);
			((int*)&pfirst[3][i])[0] = _mm_extract_epi32(p00, 0);
		}

		pSrc = pSrc1;

		for (i = left_size; i < line_size; i += 8, pSrc += 8) {
			__m128i p00, p10, p20, p30;
			__m128i p01;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 2));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));

			p00 = _mm_mullo_epi16(S0, coeff3);
			p10 = _mm_mullo_epi16(S1, coeff7);
			p20 = _mm_mullo_epi16(S2, coeff5);
			p30 = _mm_add_epi16(S3, coeff8);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_srli_epi16(p00, 4);

			_mm_storeu_si128((__m128i*)&pfirst[2][i], p00);

			p01 = _mm_add_epi16(S1, S2);
			p00 = _mm_mullo_epi16(p01, coeff3);
			p10 = _mm_add_epi16(S0, S3);
			p10 = _mm_add_epi16(p10, coeff4);
			p00 = _mm_add_epi16(p10, p00);
			p00 = _mm_srli_epi16(p00, 3);

			_mm_storeu_si128((__m128i*)&pfirst[1][i], p00);

			p10 = _mm_mullo_epi16(S1, coeff5);
			p20 = _mm_mullo_epi16(S2, coeff7);
			p30 = _mm_mullo_epi16(S3, coeff3);
			p00 = _mm_add_epi16(S0, coeff8);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_srli_epi16(p00, 4);

			_mm_storeu_si128((__m128i*)&pfirst[0][i], p00);

			p00 = _mm_add_epi16(S0, S1);
			p00 = _mm_add_epi16(p00, p01);
			p00 = _mm_add_epi16(p00, coeff2);
			p00 = _mm_srli_epi16(p00, 2);

			_mm_storeu_si128((__m128i*)&pfirst[3][i], p00);
		}

		pfirst[0] += left_size;
		pfirst[1] += left_size;
		pfirst[2] += left_size;
		pfirst[3] += left_size;

		iHeight >>= 2;

		switch (iWidth) {
		case 4:
			for (i = 0; i < iHeight; i++) {
				CP64(dst, pfirst[0] - i); dst += i_dst;
				CP64(dst, pfirst[1] - i); dst += i_dst;
				CP64(dst, pfirst[2] - i); dst += i_dst;
				CP64(dst, pfirst[3] - i); dst += i_dst;
			}
			break;
		case 8:
			for (i = 0; i < iHeight; i++) {
				CP128(dst, pfirst[0] - i); dst += i_dst;
				CP128(dst, pfirst[1] - i); dst += i_dst;
				CP128(dst, pfirst[2] - i); dst += i_dst;
				CP128(dst, pfirst[3] - i); dst += i_dst;
			}
			break;
		case 16:
		case 32:
		case 64:
			for (i = 0; i < iHeight; i++) {
				memcpy(dst, pfirst[0] - i, iWidth * sizeof(pel_t)); dst += i_dst;
				memcpy(dst, pfirst[1] - i, iWidth * sizeof(pel_t)); dst += i_dst;
				memcpy(dst, pfirst[2] - i, iWidth * sizeof(pel_t)); dst += i_dst;
				memcpy(dst, pfirst[3] - i, iWidth * sizeof(pel_t)); dst += i_dst;
			}
			break;
		default:
			assert(0);
			break;
		}
	}
	else
	{
		if (iWidth == 16)
		{
			pel_t *dst2 = dst + i_dst;
			pel_t *dst3 = dst2 + i_dst;
			pel_t *dst4 = dst3 + i_dst;
			__m128i p00, p10, p20, p30;
			__m128i p01;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 2));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));

			p00 = _mm_mullo_epi16(S0, coeff3);
			p10 = _mm_mullo_epi16(S1, coeff7);
			p20 = _mm_mullo_epi16(S2, coeff5);
			p30 = _mm_add_epi16(S3, coeff8);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_srli_epi16(p00, 4);

			_mm_storeu_si128((__m128i*)dst3, p00);

			p01 = _mm_add_epi16(S1, S2);
			p00 = _mm_mullo_epi16(p01, coeff3);
			p10 = _mm_add_epi16(S0, S3);
			p10 = _mm_add_epi16(p10, coeff4);
			p00 = _mm_add_epi16(p10, p00);
			p00 = _mm_srli_epi16(p00, 3);

			_mm_storeu_si128((__m128i*)dst2, p00);

			p10 = _mm_mullo_epi16(S1, coeff5);
			p20 = _mm_mullo_epi16(S2, coeff7);
			p30 = _mm_mullo_epi16(S3, coeff3);
			p00 = _mm_add_epi16(S0, coeff8);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_srli_epi16(p00, 4);

			_mm_storeu_si128((__m128i*)dst, p00);

			p00 = _mm_add_epi16(S0, S1);
			p00 = _mm_add_epi16(p00, p01);
			p00 = _mm_add_epi16(p00, coeff2);
			p00 = _mm_srli_epi16(p00, 2);

			_mm_storeu_si128((__m128i*)dst4, p00);

			pSrc += 8;
			S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			S3 = _mm_loadu_si128((__m128i*)(pSrc + 2));
			S1 = _mm_loadu_si128((__m128i*)(pSrc));
			S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));

			p00 = _mm_mullo_epi16(S0, coeff3);
			p10 = _mm_mullo_epi16(S1, coeff7);
			p20 = _mm_mullo_epi16(S2, coeff5);
			p30 = _mm_add_epi16(S3, coeff8);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_srli_epi16(p00, 4);

			_mm_storeu_si128((__m128i*)(dst3 + 8), p00);

			p01 = _mm_add_epi16(S1, S2);
			p00 = _mm_mullo_epi16(p01, coeff3);
			p10 = _mm_add_epi16(S0, S3);
			p10 = _mm_add_epi16(p10, coeff4);
			p00 = _mm_add_epi16(p10, p00);
			p00 = _mm_srli_epi16(p00, 3);

			_mm_storeu_si128((__m128i*)(dst2 + 8), p00);

			p10 = _mm_mullo_epi16(S1, coeff5);
			p20 = _mm_mullo_epi16(S2, coeff7);
			p30 = _mm_mullo_epi16(S3, coeff3);
			p00 = _mm_add_epi16(S0, coeff8);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_srli_epi16(p00, 4);

			_mm_storeu_si128((__m128i*)(dst+8), p00);

			p00 = _mm_add_epi16(S0, S1);
			p00 = _mm_add_epi16(p00, p01);
			p00 = _mm_add_epi16(p00, coeff2);
			p00 = _mm_srli_epi16(p00, 2);

			_mm_storeu_si128((__m128i*)(dst4+8), p00);

		}
		else
		{
			pel_t *dst2 = dst + i_dst;
			pel_t *dst3 = dst2 + i_dst;
			pel_t *dst4 = dst3 + i_dst;
			__m128i p00, p10, p20, p30;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 2));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));

			p00 = _mm_mullo_epi16(S0, coeff3);
			p10 = _mm_mullo_epi16(S1, coeff7);
			p20 = _mm_mullo_epi16(S2, coeff5);
			p30 = _mm_add_epi16(S3, coeff8);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_srli_epi16(p00, 4);

			_mm_storel_epi64((__m128i*)dst3, p00);

			p00 = _mm_add_epi16(S1, S2);
			p00 = _mm_mullo_epi16(p00, coeff3);
			p10 = _mm_add_epi16(S0, S3);
			p10 = _mm_add_epi16(p10, coeff4);
			p00 = _mm_add_epi16(p10, p00);
			p00 = _mm_srli_epi16(p00, 3);

			_mm_storel_epi64((__m128i*)dst2, p00);

			p10 = _mm_mullo_epi16(S1, coeff5);
			p20 = _mm_mullo_epi16(S2, coeff7);
			p30 = _mm_mullo_epi16(S3, coeff3);
			p00 = _mm_add_epi16(S0, coeff8);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_srli_epi16(p00, 4);

			_mm_storel_epi64((__m128i*)dst, p00);

			p00 = _mm_add_epi16(S0, S1);
			p10 = _mm_add_epi16(S1, S2);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, coeff2);
			p00 = _mm_srli_epi16(p00, 2);

			_mm_storel_epi64((__m128i*)dst4, p00);
		}
	}
}

void xPredIntraAngAdi_XY_16_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[2 * (64 + 48)]);
	int line_size = iWidth + iHeight / 2 - 1;
	int left_size = line_size - iWidth;
	int aligned_line_size = ((line_size + 31) >> 4) << 4;
	pel_t *pfirst[2];

	__m128i coeff2 = _mm_set1_epi16(2);
	__m128i coeff3 = _mm_set1_epi16(3);
	__m128i coeff4 = _mm_set1_epi16(4);
	__m128i shuffle = _mm_setr_epi8(0, 1, 4, 5, 8, 9, 12, 13, 2, 3, 6, 7, 10, 11, 14, 15);

	int i;
	pel_t *pSrc1;

	pfirst[0] = first_line;
	pfirst[1] = first_line + aligned_line_size;

	pSrc -= iHeight - 2;

	pSrc1 = pSrc;

	for (i = 0; i < left_size; i += 4, pSrc += 8)
	{
		__m128i p00, p01;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

		p00 = _mm_add_epi16(S0, S1);
		p01 = _mm_add_epi16(S1, S2);
		p00 = _mm_add_epi16(p00, coeff2);
		p00 = _mm_add_epi16(p00, p01);
		p00 = _mm_srli_epi16(p00, 2);

		p00 = _mm_shuffle_epi8(p00, shuffle);
		_mm_storel_epi64((__m128i*)&pfirst[1][i], p00);
		p00 = _mm_srli_si128(p00, 8);
		_mm_storel_epi64((__m128i*)&pfirst[0][i], p00);
	}

	pSrc = pSrc1 + left_size + left_size;

	for (i = left_size; i < line_size; i += 8, pSrc += 8)
	{
		__m128i p00, p01, p10;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 2));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));

		p10 = _mm_add_epi16(S1, S2);
		p00 = _mm_mullo_epi16(p10, coeff3);

		p01 = _mm_add_epi16(S0, S3);
		p00 = _mm_add_epi16(p00, coeff4);

		p00 = _mm_add_epi16(p00, p01);
		p00 = _mm_srli_epi16(p00, 3);

		_mm_storeu_si128((__m128i*)&pfirst[0][i], p00);

		p00 = _mm_add_epi16(S0, S1);
		p00 = _mm_add_epi16(p00, coeff2);
		p00 = _mm_add_epi16(p00, p10);
		p00 = _mm_srli_epi16(p00, 2);

		_mm_storeu_si128((__m128i*)&pfirst[1][i], p00);
	}

	pfirst[0] += left_size;
	pfirst[1] += left_size;

	iHeight >>= 1;

	switch (iWidth) {
	case 4:
		for (i = 0; i < iHeight; i++) {
			CP64(dst, pfirst[0] - i);
			CP64(dst + i_dst, pfirst[1] - i);
			dst += (i_dst << 1);
		}
		break;
	case 8:
		for (i = 0; i < iHeight; i++) {
			CP128(dst, pfirst[0] - i);
			CP128(dst + i_dst, pfirst[1] - i);
			dst += (i_dst << 1);
		}
		break;
	default:
		for (i = 0; i < iHeight; i++) {
			memcpy(dst, pfirst[0] - i, iWidth * sizeof(pel_t));
			memcpy(dst + i_dst, pfirst[1] - i, iWidth * sizeof(pel_t));
			dst += (i_dst << 1);
		}
		break;
	}

}

void xPredIntraAngAdi_XY_18_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[64 + 64]);
	int line_size = iWidth + iHeight - 1;
	int i;
	pel_t *pfirst = first_line + iHeight - 1;
	__m128i coeff2 = _mm_set1_epi16(2);

	pSrc -= iHeight - 1;

	for (i = 0; i < line_size; i += 8, pSrc += 8)
	{
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

		__m128i sum1 = _mm_add_epi16(S0, S1);
		__m128i sum2 = _mm_add_epi16(S1, S2);

		sum1 = _mm_add_epi16(sum1, sum2);
		sum1 = _mm_add_epi16(sum1, coeff2);
		sum1 = _mm_srli_epi16(sum1, 2);

		_mm_store_si128((__m128i*)&first_line[i], sum1);
	}

	switch (iWidth) {
	case 4:
		for (i = 0; i < iHeight; i++) {
			CP64(dst, pfirst--);
			dst += i_dst;
		}
		break;
	case 8:
		for (i = 0; i < iHeight; i++) {
			CP128(dst, pfirst--);
			dst += i_dst;
		}
		break;
	default:
		for (i = 0; i < iHeight; i++) {
			memcpy(dst, pfirst--, iWidth * sizeof(pel_t));
			dst += i_dst;
		}
		break;
		break;
	}

}

void xPredIntraAngAdi_XY_20_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	ALIGNED_16(pel_t first_line[64 + 128]);
	int left_size = (iHeight - 1) * 2 + 1;
	int top_size = iWidth - 1;
	int line_size = left_size + top_size;
	int i;
	pel_t *pfirst = first_line + left_size - 1;

	__m128i coeff2 = _mm_set1_epi16(2);
	__m128i coeff3 = _mm_set1_epi16(3);
	__m128i coeff4 = _mm_set1_epi16(4);

	pel_t *pSrc1 = pSrc;
	pSrc -= iHeight;

	for (i = 0; i < left_size; i += 16, pSrc += 8)
	{
		__m128i p00, p01, p10;
		__m128i p20, p21;
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 2));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

		p10 = _mm_add_epi16(S1, S2);
		p00 = _mm_mullo_epi16(p10, coeff3);

		p01 = _mm_add_epi16(S0, S3);
		p00 = _mm_add_epi16(p00, coeff4);
		p00 = _mm_add_epi16(p00, p01);
		p00 = _mm_srli_epi16(p00, 3);

		p21 = _mm_add_epi16(S2, S3);
		p20 = _mm_add_epi16(p10, coeff2);
		p20 = _mm_add_epi16(p20, p21);
		p20 = _mm_srli_epi16(p20, 2);

		p10 = _mm_unpacklo_epi16(p00, p20);
		p00 = _mm_unpackhi_epi16(p00, p20);

		_mm_store_si128((__m128i*)&first_line[i], p10);
		_mm_store_si128((__m128i*)&first_line[i + 8], p00);
	}

	pSrc = pSrc1;

	for (i = left_size; i < line_size; i += 8, pSrc += 8)
	{
		__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
		__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
		__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

		__m128i sum1 = _mm_add_epi16(S0, S1);
		__m128i sum2 = _mm_add_epi16(S1, S2);

		sum1 = _mm_add_epi16(sum1, sum2);
		sum1 = _mm_add_epi16(sum1, coeff2);
		sum1 = _mm_srli_epi16(sum1, 2);

		_mm_storeu_si128((__m128i*)&first_line[i], sum1);
	}

	for (i = 0; i < iHeight; i++) {
		memcpy(dst, pfirst, iWidth * sizeof(pel_t));
		pfirst -= 2;
		dst += i_dst;
	}
}

void xPredIntraAngAdi_XY_22_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
	int i;
	pSrc -= iHeight;

	if (iWidth != 4) {
		ALIGNED_16(pel_t first_line[64 + 256]);
		int left_size = (iHeight - 1) * 4 + 3;
		int top_size = iWidth - 3;
		int line_size = left_size + top_size;
		pel_t *pfirst = first_line + left_size - 3;
		pel_t *pSrc1 = pSrc;

		__m128i coeff2 = _mm_set1_epi16(2);
		__m128i coeff3 = _mm_set1_epi16(3);
		__m128i coeff4 = _mm_set1_epi16(4);
		__m128i coeff5 = _mm_set1_epi16(5);
		__m128i coeff7 = _mm_set1_epi16(7);
		__m128i coeff8 = _mm_set1_epi16(8);

		for (i = 0; i < line_size; i += 32, pSrc += 8)
		{
			__m128i p00, p10, p20, p30;
			__m128i M1, M2, M3, M4, M5, M6, M7, M8;
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S3 = _mm_loadu_si128((__m128i*)(pSrc + 2));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));

			p00 = _mm_mullo_epi16(S0, coeff3);
			p10 = _mm_mullo_epi16(S1, coeff7);
			p20 = _mm_mullo_epi16(S2, coeff5);
			p30 = _mm_add_epi16(S3, coeff8);
			p00 = _mm_add_epi16(p00, p30);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			M1 = _mm_srli_epi16(p00, 4);

			p00 = _mm_add_epi16(S1, S2);
			p00 = _mm_mullo_epi16(p00, coeff3);
			p10 = _mm_add_epi16(S0, S3);
			p10 = _mm_add_epi16(p10, coeff4);
			p00 = _mm_add_epi16(p10, p00);
			M2 = _mm_srli_epi16(p00, 3);

			p10 = _mm_mullo_epi16(S1, coeff5);
			p20 = _mm_mullo_epi16(S2, coeff7);
			p30 = _mm_mullo_epi16(S3, coeff3);
			p00 = _mm_add_epi16(S0, coeff8);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, p20);
			p00 = _mm_add_epi16(p00, p30);
			M3 = _mm_srli_epi16(p00, 4);

			p00 = _mm_add_epi16(S1, S2);
			p10 = _mm_add_epi16(S2, S3);
			p00 = _mm_add_epi16(p00, p10);
			p00 = _mm_add_epi16(p00, coeff2);
			M4 = _mm_srli_epi16(p00, 2);

			M5 = _mm_unpacklo_epi16(M1, M2);
			M6 = _mm_unpackhi_epi16(M1, M2);
			M7 = _mm_unpacklo_epi16(M3, M4);
			M8 = _mm_unpackhi_epi16(M3, M4);

			M1 = _mm_unpacklo_epi32(M5, M7);
			M2 = _mm_unpackhi_epi32(M5, M7);
			M3 = _mm_unpacklo_epi32(M6, M8);
			M4 = _mm_unpackhi_epi32(M6, M8);

			_mm_store_si128((__m128i*)&first_line[i], M1);
			_mm_store_si128((__m128i*)&first_line[8 + i], M2);
			_mm_store_si128((__m128i*)&first_line[16 + i], M3);
			_mm_store_si128((__m128i*)&first_line[24 + i], M4);
		}

		pSrc = pSrc1 + iHeight;

		for (i = left_size; i < line_size; i += 8, pSrc += 8)
		{
			__m128i S0 = _mm_loadu_si128((__m128i*)(pSrc - 1));
			__m128i S2 = _mm_loadu_si128((__m128i*)(pSrc + 1));
			__m128i S1 = _mm_loadu_si128((__m128i*)(pSrc));

			__m128i sum1 = _mm_add_epi16(S0, S1);
			__m128i sum2 = _mm_add_epi16(S1, S2);

			sum1 = _mm_add_epi16(sum1, sum2);
			sum1 = _mm_add_epi16(sum1, coeff2);
			sum1 = _mm_srli_epi16(sum1, 2);

			_mm_storeu_si128((__m128i*)&first_line[i], sum1);
		}

		switch (iWidth) {
		case 8:
			while (iHeight--) {
				CP128(dst, pfirst);
				dst += i_dst;
				pfirst -= 4;
			}
			break;
		case 16:
		case 32:
		case 64:
			while (iHeight--) {
				memcpy(dst, pfirst, iWidth * sizeof(pel_t));
				dst += i_dst;
				pfirst -= 4;
			}
			break;
		default:
			assert(0);
			break;
		}
	}
	else {
		dst += (iHeight - 1) * i_dst;
		for (i = 0; i < iHeight; i++, pSrc++)
		{
			dst[0] = (pSrc[-1] * 3 + pSrc[0] * 7 + pSrc[1] * 5 + pSrc[2] + 8) >> 4;
			dst[1] = (pSrc[-1] + (pSrc[0] + pSrc[1]) * 3 + pSrc[2] + 4) >> 3;
			dst[2] = (pSrc[-1] + pSrc[0] * 5 + pSrc[1] * 7 + pSrc[2] * 3 + 8) >> 4;
			dst[3] = (pSrc[0] + pSrc[1] * 2 + pSrc[2] + 2) >> 2;
			dst -= i_dst;
		}
	}
}

void xPredIntraAngAdi_X_3_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
	int i, j, k;
	int iWidth2 = iWidth << 1;
	__m128i zero = _mm_setzero_si128();
	__m128i off = _mm_set1_epi32(64);
	__m128i S0, S1, S2, S3;
	__m128i t0, t1, t2, t3;
	__m128i c0, c1;

	if (iWidth & 0x07) {
		for (j = 0; j < iHeight; j++) {
			int real_width;
			int idx = tab_idx_mode_3_10bit[j];

			k = j & 0x03;
			real_width = min(iWidth, iWidth2 - idx + 1);

			if (real_width <= 0) {
				pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_3_10bit[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_3_10bit[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_3_10bit[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_3_10bit[k][3] + 64) >> 7);
				__m128i D0 = _mm_set1_epi16((pel_t)val);
				_mm_storel_epi64((__m128i*)(dst), D0);
				dst += i_dst;
				j++;

				for (; j < iHeight; j++) {
					k = j & 0x03;
					val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_3_10bit[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_3_10bit[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_3_10bit[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_3_10bit[k][3] + 64) >> 7);
					D0 = _mm_set1_epi16((pel_t)val);
					_mm_storel_epi64((__m128i*)(dst), D0);
					dst += i_dst;
				}
				break;
			}
			else {
				__m128i D0;
				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_3_10bit[k])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_3_10bit[k])[1]);

				S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
				S1 = _mm_srli_si128(S0, 2);
				S2 = _mm_srli_si128(S0, 4);
				S3 = _mm_srli_si128(S0, 6);

				t0 = _mm_unpacklo_epi16(S0, S1);
				t1 = _mm_unpacklo_epi16(S2, S3);

				t0 = _mm_madd_epi16(t0, c0);
				t1 = _mm_madd_epi16(t1, c1);

				t0 = _mm_add_epi32(t0, t1);

				D0 = _mm_add_epi32(t0, off);
				D0 = _mm_srli_epi32(D0, 7);

				D0 = _mm_packus_epi32(D0, zero);

				_mm_storel_epi64((__m128i*)(dst), D0);

				if (real_width < iWidth)
				{
					D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
					_mm_storel_epi64((__m128i*)(dst + real_width), D0);
				}
			}
			dst += i_dst;
		}
	}
	else {
		for (j = 0; j < iHeight; j++) {
			int real_width;
			int idx = tab_idx_mode_3_10bit[j];

			k = j & 0x03;
			real_width = min(iWidth, iWidth2 - idx + 1);

			if (real_width <= 0) {
				pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_3_10bit[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_3_10bit[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_3_10bit[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_3_10bit[k][3] + 64) >> 7);
				__m128i D0 = _mm_set1_epi16((pel_t)val);

				for (i = 0; i < iWidth; i += 8) {
					_mm_storeu_si128((__m128i*)(dst + i), D0);
				}
				dst += i_dst;
				j++;

				for (; j < iHeight; j++)
				{
					k = j & 0x03;
					val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_3_10bit[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_3_10bit[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_3_10bit[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_3_10bit[k][3] + 64) >> 7);
					D0 = _mm_set1_epi16((pel_t)val);
					for (i = 0; i < iWidth; i += 8) {
						_mm_storeu_si128((__m128i*)(dst + i), D0);
					}
					dst += i_dst;
				}
				break;
			}
			else {
				__m128i D0, D1;
				
				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_3_10bit[k])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_3_10bit[k])[1]);

				for (i = 0; i < real_width; i += 8, idx += 8) {
					pel_t *pSrc1 = pSrc + idx;
					S0 = _mm_loadu_si128((__m128i*)pSrc1);
					S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
					S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
					S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

					t0 = _mm_unpacklo_epi16(S0, S1);
					t1 = _mm_unpacklo_epi16(S2, S3);
					t2 = _mm_unpackhi_epi16(S0, S1);
					t3 = _mm_unpackhi_epi16(S2, S3);
					
					t0 = _mm_madd_epi16(t0, c0);
					t1 = _mm_madd_epi16(t1, c1);
					t2 = _mm_madd_epi16(t2, c0);
					t3 = _mm_madd_epi16(t3, c1);

					t0 = _mm_add_epi32(t0, t1);
					t2 = _mm_add_epi32(t2, t3);

					D0 = _mm_add_epi32(t0, off);
					D0 = _mm_srli_epi32(D0, 7);

					D1 = _mm_add_epi32(t2, off);
					D1 = _mm_srli_epi32(D1, 7);

					D0 = _mm_packus_epi32(D0, D1);

					_mm_storeu_si128((__m128i*)(dst + i), D0);
					//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
				}

				if (real_width < iWidth)
				{
					D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
					for (i = real_width; i < iWidth; i += 8) {
						_mm_storeu_si128((__m128i*)(dst + i), D0);
						//dst[i] = dst[real_width - 1];
					}
				}

			}

			dst += i_dst;
		}
	}
}

void xPredIntraAngAdi_X_5_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
	int i, j, k;
	int iWidth2 = iWidth << 1;
	__m128i zero = _mm_setzero_si128();
	__m128i off = _mm_set1_epi32(64);
	__m128i S0, S1, S2, S3;
	__m128i t0, t1, t2, t3;
	__m128i c0, c1;

	if (iWidth == 4) {
		for (j = 0; j < iHeight; j++) {
			int real_width;
			int idx = tab_idx_mode_5_10bit[j];

			k = j & 0x07;
			real_width = min(iWidth, iWidth2 - idx + 1);

			if (real_width <= 0) {
				pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_5_10bit[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_5_10bit[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_5_10bit[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_5_10bit[k][3] + 64) >> 7);
				__m128i D0 = _mm_set1_epi16((pel_t)val);
				_mm_storel_epi64((__m128i*)(dst), D0);
				dst += i_dst;
				j++;

				for (; j < iHeight; j++)
				{
					k = j & 0x07;
					val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_5_10bit[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_5_10bit[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_5_10bit[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_5_10bit[k][3] + 64) >> 7);
					D0 = _mm_set1_epi16((pel_t)val);
					_mm_storel_epi64((__m128i*)(dst), D0);
					dst += i_dst;
				}
				break;
			}
			else {
				__m128i D0;
				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_5_10bit[k])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_5_10bit[k])[1]);

				S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
				S1 = _mm_srli_si128(S0, 2);
				S2 = _mm_srli_si128(S0, 4);
				S3 = _mm_srli_si128(S0, 6);

				t0 = _mm_unpacklo_epi16(S0, S1);
				t1 = _mm_unpacklo_epi16(S2, S3);

				t0 = _mm_madd_epi16(t0, c0);
				t1 = _mm_madd_epi16(t1, c1);

				t0 = _mm_add_epi32(t0, t1);

				D0 = _mm_add_epi32(t0, off);
				D0 = _mm_srli_epi32(D0, 7);

				D0 = _mm_packus_epi32(D0, zero);

				_mm_storel_epi64((__m128i*)(dst), D0);

				if (real_width < iWidth)
				{
					D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
					_mm_storel_epi64((__m128i*)(dst + real_width), D0);
				}
			}
			dst += i_dst;
		}
	}
	else {
		for (j = 0; j < iHeight; j++) {
			int real_width;
			int idx = tab_idx_mode_5_10bit[j];
			k = j & 0x07;

			real_width = min(iWidth, iWidth2 - idx + 1);

			if (real_width <= 0) {
				pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_5_10bit[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_5_10bit[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_5_10bit[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_5_10bit[k][3] + 64) >> 7);
				__m128i D0 = _mm_set1_epi16((pel_t)val);

				for (i = 0; i < iWidth; i += 8) {
					_mm_store_si128((__m128i*)(dst + i), D0);
				}
				dst += i_dst;
				j++;

				for (; j < iHeight; j++) {
					k = j & 0x07;
					val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_5_10bit[k][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_5_10bit[k][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_5_10bit[k][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_5_10bit[k][3] + 64) >> 7);
					D0 = _mm_set1_epi16((pel_t)val);
					for (i = 0; i < iWidth; i += 8) {
						_mm_store_si128((__m128i*)(dst + i), D0);
					}
					dst += i_dst;
				}
				break;
			}
			else {
				__m128i D0, D1;

				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_5_10bit[k])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_5_10bit[k])[1]);

				for (i = 0; i < real_width; i += 8, idx += 8) {
					pel_t *pSrc1 = pSrc + idx;
					S0 = _mm_loadu_si128((__m128i*)pSrc1);
					S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
					S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
					S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

					t0 = _mm_unpacklo_epi16(S0, S1);
					t1 = _mm_unpacklo_epi16(S2, S3);
					t2 = _mm_unpackhi_epi16(S0, S1);
					t3 = _mm_unpackhi_epi16(S2, S3);

					t0 = _mm_madd_epi16(t0, c0);
					t1 = _mm_madd_epi16(t1, c1);
					t2 = _mm_madd_epi16(t2, c0);
					t3 = _mm_madd_epi16(t3, c1);

					t0 = _mm_add_epi32(t0, t1);
					t2 = _mm_add_epi32(t2, t3);

					D0 = _mm_add_epi32(t0, off);
					D0 = _mm_srli_epi32(D0, 7);

					D1 = _mm_add_epi32(t2, off);
					D1 = _mm_srli_epi32(D1, 7);

					D0 = _mm_packus_epi32(D0, D1);

					_mm_storeu_si128((__m128i*)(dst + i), D0);
					//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
				}

				if (real_width < iWidth)
				{
					D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
					for (i = real_width; i < iWidth; i += 8) {
						_mm_storeu_si128((__m128i*)(dst + i), D0);
						//dst[i] = dst[real_width - 1];
					}
				}

			}

			dst += i_dst;
		}
	}
}

void xPredIntraAngAdi_X_7_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
	int i, j;
	int iWidth2 = iWidth << 1;
	__m128i zero = _mm_setzero_si128();
	__m128i off = _mm_set1_epi32(64);
	__m128i S0, S1, S2, S3;
	__m128i t0, t1, t2, t3;
	__m128i c0, c1;

	if (iWidth >= iHeight){
		if (iWidth & 0x07){
			__m128i D0;

			for (j = 0; j < iHeight; j ++) {
				int idx = tab_idx_mode_7_10bit[j];
				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[1]);

				S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
				S1 = _mm_srli_si128(S0, 2);
				S2 = _mm_srli_si128(S0, 4);
				S3 = _mm_srli_si128(S0, 6);

				t0 = _mm_unpacklo_epi16(S0, S1);
				t1 = _mm_unpacklo_epi16(S2, S3);

				t0 = _mm_madd_epi16(t0, c0);
				t1 = _mm_madd_epi16(t1, c1);

				t0 = _mm_add_epi32(t0, t1);

				D0 = _mm_add_epi32(t0, off);
				D0 = _mm_srli_epi32(D0, 7);

				D0 = _mm_packus_epi32(D0, zero);

				_mm_storel_epi64((__m128i*)(dst), D0);

				//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
				dst += i_dst;
			}
		}
		else {
			for (j = 0; j < iHeight; j++) {
				__m128i D0, D1;

				int idx = tab_idx_mode_7_10bit[j];
				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[1]);

				for (i = 0; i < iWidth; i += 8, idx += 8) {
					pel_t *pSrc1 = pSrc + idx;
					S0 = _mm_loadu_si128((__m128i*)pSrc1);
					S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
					S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
					S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

					t0 = _mm_unpacklo_epi16(S0, S1);
					t1 = _mm_unpacklo_epi16(S2, S3);
					t2 = _mm_unpackhi_epi16(S0, S1);
					t3 = _mm_unpackhi_epi16(S2, S3);

					t0 = _mm_madd_epi16(t0, c0);
					t1 = _mm_madd_epi16(t1, c1);
					t2 = _mm_madd_epi16(t2, c0);
					t3 = _mm_madd_epi16(t3, c1);

					t0 = _mm_add_epi32(t0, t1);
					t2 = _mm_add_epi32(t2, t3);

					D0 = _mm_add_epi32(t0, off);
					D0 = _mm_srli_epi32(D0, 7);

					D1 = _mm_add_epi32(t2, off);
					D1 = _mm_srli_epi32(D1, 7);

					D0 = _mm_packus_epi32(D0, D1);

					_mm_storeu_si128((__m128i*)(dst + i), D0);
					//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
				}

				dst += i_dst;
			}
		}
	}
	else{
		if (iWidth & 0x07) {
			for (j = 0; j < iHeight; j++) {
				int real_width;
				int idx = tab_idx_mode_7_10bit[j];

				real_width = min(iWidth, iWidth2 - idx + 1);

				if (real_width <= 0) {
					pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_7_10bit[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_7_10bit[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_7_10bit[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_7_10bit[j][3] + 64) >> 7);
					__m128i D0 = _mm_set1_epi16((pel_t)val);
					_mm_storel_epi64((__m128i*)(dst), D0);
					dst += i_dst;
					j++;

					for (; j < iHeight; j++)
					{
						val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_7_10bit[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_7_10bit[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_7_10bit[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_7_10bit[j][3] + 64) >> 7);
						D0 = _mm_set1_epi16((pel_t)val);
						_mm_storel_epi64((__m128i*)(dst), D0);
						dst += i_dst;
					}
					break;
				}
				else {
					__m128i D0;
					c0 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[0]);
					c1 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[1]);

					S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
					S1 = _mm_srli_si128(S0, 2);
					S2 = _mm_srli_si128(S0, 4);
					S3 = _mm_srli_si128(S0, 6);

					t0 = _mm_unpacklo_epi16(S0, S1);
					t1 = _mm_unpacklo_epi16(S2, S3);

					t0 = _mm_madd_epi16(t0, c0);
					t1 = _mm_madd_epi16(t1, c1);

					t0 = _mm_add_epi32(t0, t1);

					D0 = _mm_add_epi32(t0, off);
					D0 = _mm_srli_epi32(D0, 7);

					D0 = _mm_packus_epi32(D0, zero);

					_mm_storel_epi64((__m128i*)(dst), D0);

					if (real_width < iWidth)
					{
						D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
						_mm_storel_epi64((__m128i*)(dst + real_width), D0);
					}
				}
				dst += i_dst;
			}
		}
		else {
			for (j = 0; j < iHeight; j++) {
				int real_width;
				int idx = tab_idx_mode_7_10bit[j];

				real_width = min(iWidth, iWidth2 - idx + 1);

				if (real_width <= 0) {
					pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_7_10bit[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_7_10bit[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_7_10bit[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_7_10bit[j][3] + 64) >> 7);
					__m128i D0 = _mm_set1_epi16((pel_t)val);

					for (i = 0; i < iWidth; i += 8) {
						_mm_storeu_si128((__m128i*)(dst + i), D0);
					}
					dst += i_dst;
					j++;

					for (; j < iHeight; j++) {
						val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_7_10bit[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_7_10bit[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_7_10bit[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_7_10bit[j][3] + 64) >> 7);
						D0 = _mm_set1_epi16((pel_t)val);
						for (i = 0; i < iWidth; i += 8) {
							_mm_storeu_si128((__m128i*)(dst + i), D0);
						}
						dst += i_dst;
					}
					break;
				}
				else {
					__m128i D0, D1;

					c0 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[0]);
					c1 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[1]);

					for (i = 0; i < real_width; i += 8, idx += 8) {
						pel_t *pSrc1 = pSrc + idx;
						S0 = _mm_loadu_si128((__m128i*)pSrc1);
						S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
						S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
						S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

						t0 = _mm_unpacklo_epi16(S0, S1);
						t1 = _mm_unpacklo_epi16(S2, S3);
						t2 = _mm_unpackhi_epi16(S0, S1);
						t3 = _mm_unpackhi_epi16(S2, S3);

						t0 = _mm_madd_epi16(t0, c0);
						t1 = _mm_madd_epi16(t1, c1);
						t2 = _mm_madd_epi16(t2, c0);
						t3 = _mm_madd_epi16(t3, c1);

						t0 = _mm_add_epi32(t0, t1);
						t2 = _mm_add_epi32(t2, t3);

						D0 = _mm_add_epi32(t0, off);
						D0 = _mm_srli_epi32(D0, 7);

						D1 = _mm_add_epi32(t2, off);
						D1 = _mm_srli_epi32(D1, 7);

						D0 = _mm_packus_epi32(D0, D1);

						_mm_storeu_si128((__m128i*)(dst + i), D0);
						//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
					}

					if (real_width < iWidth)
					{
						D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
						for (i = real_width; i < iWidth; i += 8) {
							_mm_storeu_si128((__m128i*)(dst + i), D0);
							//dst[i] = dst[real_width - 1];
						}
					}

				}

				dst += i_dst;
			}
		}
	}
}


void xPredIntraAngAdi_X_9_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
	int i, j;
	int iWidth2 = iWidth << 1;
	__m128i zero = _mm_setzero_si128();
	__m128i off = _mm_set1_epi32(64);
	__m128i S0, S1, S2, S3;
	__m128i t0, t1, t2, t3;
	__m128i c0, c1;

	if (iWidth >= iHeight){
		if (iWidth & 0x07){
			__m128i D0;

			for (j = 0; j < iHeight; j ++) {
				int idx = tab_idx_mode_9_10bit[j];
				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[1]);

				S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
				S1 = _mm_srli_si128(S0, 2);
				S2 = _mm_srli_si128(S0, 4);
				S3 = _mm_srli_si128(S0, 6);

				t0 = _mm_unpacklo_epi16(S0, S1);
				t1 = _mm_unpacklo_epi16(S2, S3);

				t0 = _mm_madd_epi16(t0, c0);
				t1 = _mm_madd_epi16(t1, c1);

				t0 = _mm_add_epi32(t0, t1);

				D0 = _mm_add_epi32(t0, off);
				D0 = _mm_srli_epi32(D0, 7);

				D0 = _mm_packus_epi32(D0, zero);

				_mm_storel_epi64((__m128i*)(dst), D0);
				//_mm_maskmoveu_si128(D0, mask, (char*)(dst + i_dst));
				//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
				dst += i_dst;
			}
		}
		else {
			for (j = 0; j < iHeight; j++) {
				__m128i D0, D1;

				int idx = tab_idx_mode_9_10bit[j];
				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[1]);

				for (i = 0; i < iWidth; i += 8, idx += 8) {
					pel_t *pSrc1 = pSrc + idx;
					S0 = _mm_loadu_si128((__m128i*)pSrc1);
					S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
					S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
					S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

					t0 = _mm_unpacklo_epi16(S0, S1);
					t1 = _mm_unpacklo_epi16(S2, S3);
					t2 = _mm_unpackhi_epi16(S0, S1);
					t3 = _mm_unpackhi_epi16(S2, S3);

					t0 = _mm_madd_epi16(t0, c0);
					t1 = _mm_madd_epi16(t1, c1);
					t2 = _mm_madd_epi16(t2, c0);
					t3 = _mm_madd_epi16(t3, c1);

					t0 = _mm_add_epi32(t0, t1);
					t2 = _mm_add_epi32(t2, t3);

					D0 = _mm_add_epi32(t0, off);
					D0 = _mm_srli_epi32(D0, 7);

					D1 = _mm_add_epi32(t2, off);
					D1 = _mm_srli_epi32(D1, 7);

					D0 = _mm_packus_epi32(D0, D1);

					_mm_storeu_si128((__m128i*)(dst + i), D0);
					//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
				}

				dst += i_dst;
			}
		}
	}
	else{
		if (iWidth & 0x07) {
			for (j = 0; j < iHeight; j++) {
				int real_width;
				int idx = tab_idx_mode_9_10bit[j];

				real_width = min(iWidth, iWidth2 - idx + 1);

				if (real_width <= 0) {
					pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_9_10bit[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_9_10bit[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_9_10bit[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_9_10bit[j][3] + 64) >> 7);
					__m128i D0 = _mm_set1_epi16((pel_t)val);
					_mm_storel_epi64((__m128i*)(dst), D0);
					dst += i_dst;
					j++;

					for (; j < iHeight; j++)
					{
						val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_9_10bit[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_9_10bit[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_9_10bit[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_9_10bit[j][3] + 64) >> 7);
						D0 = _mm_set1_epi16((pel_t)val);
						_mm_storel_epi64((__m128i*)(dst), D0);
						dst += i_dst;
					}
					break;
				}
				else {
					__m128i D0;
					c0 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[0]);
					c1 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[1]);

					S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
					S1 = _mm_srli_si128(S0, 2);
					S2 = _mm_srli_si128(S0, 4);
					S3 = _mm_srli_si128(S0, 6);

					t0 = _mm_unpacklo_epi16(S0, S1);
					t1 = _mm_unpacklo_epi16(S2, S3);

					t0 = _mm_madd_epi16(t0, c0);
					t1 = _mm_madd_epi16(t1, c1);

					t0 = _mm_add_epi32(t0, t1);

					D0 = _mm_add_epi32(t0, off);
					D0 = _mm_srli_epi32(D0, 7);

					D0 = _mm_packus_epi32(D0, zero);

					_mm_storel_epi64((__m128i*)(dst), D0);

					if (real_width < iWidth)
					{
						D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
						_mm_storel_epi64((__m128i*)(dst + real_width), D0);
					}
				}
				dst += i_dst;
			}
		}
		else {
			for (j = 0; j < iHeight; j++) {
				int real_width;
				int idx = tab_idx_mode_9_10bit[j];

				real_width = min(iWidth, iWidth2 - idx + 1);

				if (real_width <= 0) {
					pel_t val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_9_10bit[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_9_10bit[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_9_10bit[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_9_10bit[j][3] + 64) >> 7);
					__m128i D0 = _mm_set1_epi16((pel_t)val);

					for (i = 0; i < iWidth; i += 16) {
						_mm_storeu_si128((__m128i*)(dst + i), D0);
					}
					dst += i_dst;
					j++;

					for (; j < iHeight; j++)
					{
						val = (pel_t)((pSrc[iWidth2] * tab_coeff_mode_9_10bit[j][0] + pSrc[iWidth2 + 1] * tab_coeff_mode_9_10bit[j][1] + pSrc[iWidth2 + 2] * tab_coeff_mode_9_10bit[j][2] + pSrc[iWidth2 + 3] * tab_coeff_mode_9_10bit[j][3] + 64) >> 7);
						D0 = _mm_set1_epi16((pel_t)val);
						for (i = 0; i < iWidth; i += 16) {
							_mm_storeu_si128((__m128i*)(dst + i), D0);
						}
						dst += i_dst;
					}
					break;
				}
				else {
					__m128i D0, D1;

					c0 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[0]);
					c1 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[1]);

					for (i = 0; i < real_width; i += 8, idx += 8) {
						pel_t *pSrc1 = pSrc + idx;
						S0 = _mm_loadu_si128((__m128i*)pSrc1);
						S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
						S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
						S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

						t0 = _mm_unpacklo_epi16(S0, S1);
						t1 = _mm_unpacklo_epi16(S2, S3);
						t2 = _mm_unpackhi_epi16(S0, S1);
						t3 = _mm_unpackhi_epi16(S2, S3);

						t0 = _mm_madd_epi16(t0, c0);
						t1 = _mm_madd_epi16(t1, c1);
						t2 = _mm_madd_epi16(t2, c0);
						t3 = _mm_madd_epi16(t3, c1);

						t0 = _mm_add_epi32(t0, t1);
						t2 = _mm_add_epi32(t2, t3);

						D0 = _mm_add_epi32(t0, off);
						D0 = _mm_srli_epi32(D0, 7);

						D1 = _mm_add_epi32(t2, off);
						D1 = _mm_srli_epi32(D1, 7);

						D0 = _mm_packus_epi32(D0, D1);

						_mm_storeu_si128((__m128i*)(dst + i), D0);
						//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
					}

					if (real_width < iWidth)
					{
						D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
						for (i = real_width; i < iWidth; i += 8) {
							_mm_storeu_si128((__m128i*)(dst + i), D0);
							//dst[i] = dst[real_width - 1];
						}
					}

				}

				dst += i_dst;
			}
		}
	}
}

void xPredIntraAngAdi_X_11_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
	int i, j, idx;
	__m128i zero = _mm_setzero_si128();
	__m128i off = _mm_set1_epi32(64);
	__m128i S0, S1, S2, S3;
	__m128i t0, t1, t2, t3;
	__m128i c0, c1;

	if (iWidth & 0x07){
		__m128i D0;

		for (j = 0; j < iHeight; j ++) {
			idx = (j + 1) >> 3;
			c0 = _mm_set1_epi32(((int*)tab_coeff_mode_11_10bit[j & 0x07])[0]);
			c1 = _mm_set1_epi32(((int*)tab_coeff_mode_11_10bit[j & 0x07])[1]);

			S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
			S1 = _mm_srli_si128(S0, 2);
			S2 = _mm_srli_si128(S0, 4);
			S3 = _mm_srli_si128(S0, 6);

			t0 = _mm_unpacklo_epi16(S0, S1);
			t1 = _mm_unpacklo_epi16(S2, S3);

			t0 = _mm_madd_epi16(t0, c0);
			t1 = _mm_madd_epi16(t1, c1);
			
			t0 = _mm_add_epi32(t0, t1);

			D0 = _mm_add_epi32(t0, off);
			D0 = _mm_srli_epi32(D0, 7);

			D0 = _mm_packus_epi32(D0, zero);

			_mm_storel_epi64((__m128i*)(dst), D0);
			//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
			dst += i_dst;
		}
	}
	else {
		for (j = 0; j < iHeight; j++) {
			__m128i D0, D1;

			idx = (j + 1) >> 3;
			c0 = _mm_set1_epi32(((int*)tab_coeff_mode_11_10bit[j & 0x07])[0]);
			c1 = _mm_set1_epi32(((int*)tab_coeff_mode_11_10bit[j & 0x07])[1]);

			for (i = 0; i < iWidth; i += 8, idx += 8) {
				pel_t *pSrc1 = pSrc + idx;
				S0 = _mm_loadu_si128((__m128i*)pSrc1);
				S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
				S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
				S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

				t0 = _mm_unpacklo_epi16(S0, S1);
				t1 = _mm_unpacklo_epi16(S2, S3);
				t2 = _mm_unpackhi_epi16(S0, S1);
				t3 = _mm_unpackhi_epi16(S2, S3);

				t0 = _mm_madd_epi16(t0, c0);
				t1 = _mm_madd_epi16(t1, c1);
				t2 = _mm_madd_epi16(t2, c0);
				t3 = _mm_madd_epi16(t3, c1);

				t0 = _mm_add_epi32(t0, t1);
				t2 = _mm_add_epi32(t2, t3);

				D0 = _mm_add_epi32(t0, off);
				D0 = _mm_srli_epi32(D0, 7);

				D1 = _mm_add_epi32(t2, off);
				D1 = _mm_srli_epi32(D1, 7);

				D0 = _mm_packus_epi32(D0, D1);

				_mm_storeu_si128((__m128i*)(dst + i), D0);
				//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
			}

			dst += i_dst;
		}
	}
}

static __inline void transpose_4x4_10bit(pel_t *dst){
	__m128i t0, t1;
	__m128i m0, m1;
	pel_t *dst1 = dst + 8;

	t0 = _mm_load_si128((__m128i *)dst);
	t1 = _mm_load_si128((__m128i *)dst1);

	m0 = _mm_unpacklo_epi16(t0, t1);
	m1 = _mm_unpackhi_epi16(t0, t1);

	t0 = _mm_unpacklo_epi16(m0, m1);
	t1 = _mm_unpackhi_epi16(m0, m1);

	_mm_store_si128((__m128i *)dst, t0);
	_mm_store_si128((__m128i *)dst1, t1);
}

static __inline void transpose_4x16_10bit(pel_t *dst){
	//  srcStride = dstStride = 16
	__m128i s0, s1, s2, s3;
	__m128i t0, t1, t2, t3, t4, t5, t6, t7;
	pel_t *pDst = dst;

	s0 = _mm_loadl_epi64((__m128i *)dst);
	s1 = _mm_loadl_epi64((__m128i *)(dst + 16));
	s2 = _mm_loadl_epi64((__m128i *)(dst + 32));
	s3 = _mm_loadl_epi64((__m128i *)(dst + 48));

	s0 = _mm_unpacklo_epi16(s0, s1);
	s2 = _mm_unpacklo_epi16(s2, s3);

	t0 = _mm_unpacklo_epi32(s0, s2);
	t1 = _mm_unpackhi_epi32(s0, s2);

	dst += 64;
	s0 = _mm_loadl_epi64((__m128i *)dst);
	s1 = _mm_loadl_epi64((__m128i *)(dst + 16));
	s2 = _mm_loadl_epi64((__m128i *)(dst + 32));
	s3 = _mm_loadl_epi64((__m128i *)(dst + 48));

	s0 = _mm_unpacklo_epi16(s0, s1);
	s2 = _mm_unpacklo_epi16(s2, s3);

	t2 = _mm_unpacklo_epi32(s0, s2);
	t3 = _mm_unpackhi_epi32(s0, s2);

	dst += 64;	s0 = _mm_loadl_epi64((__m128i *)dst);
	s1 = _mm_loadl_epi64((__m128i *)(dst + 16));
	s2 = _mm_loadl_epi64((__m128i *)(dst + 32));
	s3 = _mm_loadl_epi64((__m128i *)(dst + 48));

	s0 = _mm_unpacklo_epi16(s0, s1);
	s2 = _mm_unpacklo_epi16(s2, s3);

	t4 = _mm_unpacklo_epi32(s0, s2);
	t5 = _mm_unpackhi_epi32(s0, s2);

	dst += 64;	s0 = _mm_loadl_epi64((__m128i *)dst);
	s1 = _mm_loadl_epi64((__m128i *)(dst + 16));
	s2 = _mm_loadl_epi64((__m128i *)(dst + 32));
	s3 = _mm_loadl_epi64((__m128i *)(dst + 48));

	s0 = _mm_unpacklo_epi16(s0, s1);
	s2 = _mm_unpacklo_epi16(s2, s3);

	t6 = _mm_unpacklo_epi32(s0, s2);
	t7 = _mm_unpackhi_epi32(s0, s2);

	s0 = _mm_unpacklo_epi64(t0, t2);
	s1 = _mm_unpacklo_epi64(t4, t6);
	s2 = _mm_unpackhi_epi64(t0, t2);
	s3 = _mm_unpackhi_epi64(t4, t6);

	dst = pDst;
	_mm_store_si128((__m128i *)dst, s0);
	_mm_store_si128((__m128i *)(dst + 8), s1);
	_mm_store_si128((__m128i *)(dst + 16), s2);
	_mm_store_si128((__m128i *)(dst + 24), s3);

	s0 = _mm_unpacklo_epi64(t1, t3);
	s1 = _mm_unpacklo_epi64(t5, t7);
	s2 = _mm_unpackhi_epi64(t1, t3);
	s3 = _mm_unpackhi_epi64(t5, t7);

	dst += 32;
	_mm_store_si128((__m128i *)dst, s0);
	_mm_store_si128((__m128i *)(dst + 8), s1);
	_mm_store_si128((__m128i *)(dst + 16), s2);
	_mm_store_si128((__m128i *)(dst + 24), s3);
}

static __inline void transpose_8x8_10bit(pel_t *dst){
	__m128i t0, t1, t2, t3, t4, t5, t6, t7;
	__m128i m0, m1, m2, m3, m4, m5, m6, m7;

	t0 = _mm_load_si128((__m128i *)(dst));
	t1 = _mm_load_si128((__m128i *)(dst + 8));
	t2 = _mm_load_si128((__m128i *)(dst + 16));
	t3 = _mm_load_si128((__m128i *)(dst + 24));
	t4 = _mm_load_si128((__m128i *)(dst + 32));
	t5 = _mm_load_si128((__m128i *)(dst + 40));
	t6 = _mm_load_si128((__m128i *)(dst + 48));
	t7 = _mm_load_si128((__m128i *)(dst + 56));

	m0 = _mm_unpacklo_epi16(t0, t1);
	m1 = _mm_unpackhi_epi16(t0, t1);
	m2 = _mm_unpacklo_epi16(t2, t3);
	m3 = _mm_unpackhi_epi16(t2, t3);
	m4 = _mm_unpacklo_epi16(t4, t5);
	m5 = _mm_unpackhi_epi16(t4, t5);
	m6 = _mm_unpacklo_epi16(t6, t7);
	m7 = _mm_unpackhi_epi16(t6, t7);

	t0 = _mm_unpacklo_epi32(m0, m2);
	t1 = _mm_unpackhi_epi32(m0, m2);
	t2 = _mm_unpacklo_epi32(m1, m3);
	t3 = _mm_unpackhi_epi32(m1, m3);
	t4 = _mm_unpacklo_epi32(m4, m6);
	t5 = _mm_unpackhi_epi32(m4, m6);
	t6 = _mm_unpacklo_epi32(m5, m7);
	t7 = _mm_unpackhi_epi32(m5, m7);

	m0 = _mm_unpacklo_epi64(t0, t4);
	m1 = _mm_unpackhi_epi64(t0, t4);
	m2 = _mm_unpacklo_epi64(t2, t6);
	m3 = _mm_unpackhi_epi64(t2, t6);
	m4 = _mm_unpacklo_epi64(t1, t5);
	m5 = _mm_unpackhi_epi64(t1, t5);
	m6 = _mm_unpacklo_epi64(t3, t7);
	m7 = _mm_unpackhi_epi64(t3, t7);

	_mm_store_si128((__m128i *)dst, m0);
	_mm_store_si128((__m128i *)(dst + 8), m1);
	_mm_store_si128((__m128i *)(dst + 16), m4);
	_mm_store_si128((__m128i *)(dst + 24), m5);
	_mm_store_si128((__m128i *)(dst + 32), m2);
	_mm_store_si128((__m128i *)(dst + 40), m3);
	_mm_store_si128((__m128i *)(dst + 48), m6);
	_mm_store_si128((__m128i *)(dst + 56), m7);
}

static __inline void transpose_8x8_stride_10bit(pel_t *src, int i_src, pel_t *dst, int i_dst){
	__m128i t0, t1, t2, t3, t4, t5, t6, t7;
	__m128i m0, m1, m2, m3, m4, m5, m6, m7;

	t0 = _mm_load_si128((__m128i *)src);
	src += i_src;
	t1 = _mm_load_si128((__m128i *)src);
	src += i_src;
	t2 = _mm_load_si128((__m128i *)src);
	src += i_src;
	t3 = _mm_load_si128((__m128i *)src);
	src += i_src;
	t4 = _mm_load_si128((__m128i *)src);
	src += i_src;
	t5 = _mm_load_si128((__m128i *)src);
	src += i_src;
	t6 = _mm_load_si128((__m128i *)src);
	src += i_src;
	t7 = _mm_load_si128((__m128i *)src);

	m0 = _mm_unpacklo_epi16(t0, t1);
	m1 = _mm_unpackhi_epi16(t0, t1);
	m2 = _mm_unpacklo_epi16(t2, t3);
	m3 = _mm_unpackhi_epi16(t2, t3);
	m4 = _mm_unpacklo_epi16(t4, t5);
	m5 = _mm_unpackhi_epi16(t4, t5);
	m6 = _mm_unpacklo_epi16(t6, t7);
	m7 = _mm_unpackhi_epi16(t6, t7);

	t0 = _mm_unpacklo_epi32(m0, m2);
	t1 = _mm_unpackhi_epi32(m0, m2);
	t2 = _mm_unpacklo_epi32(m1, m3);
	t3 = _mm_unpackhi_epi32(m1, m3);
	t4 = _mm_unpacklo_epi32(m4, m6);
	t5 = _mm_unpackhi_epi32(m4, m6);
	t6 = _mm_unpacklo_epi32(m5, m7);
	t7 = _mm_unpackhi_epi32(m5, m7);

	m0 = _mm_unpacklo_epi64(t0, t4);
	m1 = _mm_unpackhi_epi64(t0, t4);
	m2 = _mm_unpacklo_epi64(t2, t6);
	m3 = _mm_unpackhi_epi64(t2, t6);
	m4 = _mm_unpacklo_epi64(t1, t5);
	m5 = _mm_unpackhi_epi64(t1, t5);
	m6 = _mm_unpacklo_epi64(t3, t7);
	m7 = _mm_unpackhi_epi64(t3, t7);

	_mm_store_si128((__m128i *)dst, m0);
	dst += i_dst;
	_mm_store_si128((__m128i *)dst, m1);
	dst += i_dst;
	_mm_store_si128((__m128i *)dst, m4);
	dst += i_dst;
	_mm_store_si128((__m128i *)dst, m5);
	dst += i_dst;
	_mm_store_si128((__m128i *)dst, m2);
	dst += i_dst;
	_mm_store_si128((__m128i *)dst, m3);
	dst += i_dst;
	_mm_store_si128((__m128i *)dst, m6);
	dst += i_dst;
	_mm_store_si128((__m128i *)dst, m7);
}

static __inline void transpose_8x8_stride_x2_10bit(pel_t *src, int i_src, pel_t *dst, int i_dst)//two 8x8 transpose
{
	__m128i t0, t1, t2, t3, t4, t5, t6, t7;
	__m128i m0, m1, m2, m3, m4, m5, m6, m7;
	pel_t * src1, *dst1;
	src1 = src;
	dst1 = dst;

	t0 = _mm_load_si128((__m128i *)src);
	src += i_src;
	t1 = _mm_load_si128((__m128i *)src);
	src += i_src;
	t2 = _mm_load_si128((__m128i *)src);
	src += i_src;
	t3 = _mm_load_si128((__m128i *)src);
	src += i_src;
	t4 = _mm_load_si128((__m128i *)src);
	src += i_src;
	t5 = _mm_load_si128((__m128i *)src);
	src += i_src;
	t6 = _mm_load_si128((__m128i *)src);
	src += i_src;
	t7 = _mm_load_si128((__m128i *)src);

	m0 = _mm_unpacklo_epi16(t0, t1);
	m1 = _mm_unpackhi_epi16(t0, t1);
	m2 = _mm_unpacklo_epi16(t2, t3);
	m3 = _mm_unpackhi_epi16(t2, t3);
	m4 = _mm_unpacklo_epi16(t4, t5);
	m5 = _mm_unpackhi_epi16(t4, t5);
	m6 = _mm_unpacklo_epi16(t6, t7);
	m7 = _mm_unpackhi_epi16(t6, t7);

	t0 = _mm_unpacklo_epi32(m0, m2);
	t1 = _mm_unpackhi_epi32(m0, m2);
	t2 = _mm_unpacklo_epi32(m1, m3);
	t3 = _mm_unpackhi_epi32(m1, m3);
	t4 = _mm_unpacklo_epi32(m4, m6);
	t5 = _mm_unpackhi_epi32(m4, m6);
	t6 = _mm_unpacklo_epi32(m5, m7);
	t7 = _mm_unpackhi_epi32(m5, m7);

	m0 = _mm_unpacklo_epi64(t0, t4);
	m1 = _mm_unpackhi_epi64(t0, t4);
	m2 = _mm_unpacklo_epi64(t2, t6);
	m3 = _mm_unpackhi_epi64(t2, t6);
	m4 = _mm_unpacklo_epi64(t1, t5);
	m5 = _mm_unpackhi_epi64(t1, t5);
	m6 = _mm_unpacklo_epi64(t3, t7);
	m7 = _mm_unpackhi_epi64(t3, t7);

	t0 = _mm_load_si128((__m128i *)dst);
	dst += i_dst;
	t1 = _mm_load_si128((__m128i *)dst);
	dst += i_dst;
	t2 = _mm_load_si128((__m128i *)dst);
	dst += i_dst;
	t3 = _mm_load_si128((__m128i *)dst);
	dst += i_dst;
	t4 = _mm_load_si128((__m128i *)dst);
	dst += i_dst;
	t5 = _mm_load_si128((__m128i *)dst);
	dst += i_dst;
	t6 = _mm_load_si128((__m128i *)dst);
	dst += i_dst;
	t7 = _mm_load_si128((__m128i *)dst);

	_mm_store_si128((__m128i *)dst1, m0);
	dst1 += i_dst;
	_mm_store_si128((__m128i *)dst1, m1);
	dst1 += i_dst;
	_mm_store_si128((__m128i *)dst1, m4);
	dst1 += i_dst;
	_mm_store_si128((__m128i *)dst1, m5);
	dst1 += i_dst;
	_mm_store_si128((__m128i *)dst1, m2);
	dst1 += i_dst;
	_mm_store_si128((__m128i *)dst1, m3);
	dst1 += i_dst;
	_mm_store_si128((__m128i *)dst1, m6);
	dst1 += i_dst;
	_mm_store_si128((__m128i *)dst1, m7);

	m0 = _mm_unpacklo_epi16(t0, t1);
	m1 = _mm_unpackhi_epi16(t0, t1);
	m2 = _mm_unpacklo_epi16(t2, t3);
	m3 = _mm_unpackhi_epi16(t2, t3);
	m4 = _mm_unpacklo_epi16(t4, t5);
	m5 = _mm_unpackhi_epi16(t4, t5);
	m6 = _mm_unpacklo_epi16(t6, t7);
	m7 = _mm_unpackhi_epi16(t6, t7);

	t0 = _mm_unpacklo_epi32(m0, m2);
	t1 = _mm_unpackhi_epi32(m0, m2);
	t2 = _mm_unpacklo_epi32(m1, m3);
	t3 = _mm_unpackhi_epi32(m1, m3);
	t4 = _mm_unpacklo_epi32(m4, m6);
	t5 = _mm_unpackhi_epi32(m4, m6);
	t6 = _mm_unpacklo_epi32(m5, m7);
	t7 = _mm_unpackhi_epi32(m5, m7);

	m0 = _mm_unpacklo_epi64(t0, t4);
	m1 = _mm_unpackhi_epi64(t0, t4);
	m2 = _mm_unpacklo_epi64(t2, t6);
	m3 = _mm_unpackhi_epi64(t2, t6);
	m4 = _mm_unpacklo_epi64(t1, t5);
	m5 = _mm_unpackhi_epi64(t1, t5);
	m6 = _mm_unpacklo_epi64(t3, t7);
	m7 = _mm_unpackhi_epi64(t3, t7);

	_mm_store_si128((__m128i *)src1, m0);
	src1 += i_src;
	_mm_store_si128((__m128i *)src1, m1);
	src1 += i_src;
	_mm_store_si128((__m128i *)src1, m4);
	src1 += i_src;
	_mm_store_si128((__m128i *)src1, m5);
	src1 += i_src;
	_mm_store_si128((__m128i *)src1, m2);
	src1 += i_src;
	_mm_store_si128((__m128i *)src1, m3);
	src1 += i_src;
	_mm_store_si128((__m128i *)src1, m6);
	src1 += i_src;
	_mm_store_si128((__m128i *)src1, m7);
}

static __inline void transpose_16x16_10bit(pel_t *dst){
	transpose_8x8_stride_10bit(dst, 16, dst, 16);
	transpose_8x8_stride_x2_10bit(dst + 8, 16, dst + 128, 16);
	transpose_8x8_stride_10bit(dst + 136, 16, dst + 136, 16);
}

static __inline void transpose_16x4_10bit(pel_t *dst){
	__m128i r0, r1, r2, r3, r4, r5, r6, r7;
	__m128i t0, t1, t2, t3, t4, t5, t6, t7;

	r0 = _mm_load_si128((__m128i *)dst);
	r1 = _mm_load_si128((__m128i *)(dst + 8));
	r2 = _mm_load_si128((__m128i *)(dst + 16));
	r3 = _mm_load_si128((__m128i *)(dst + 24));
	r4 = _mm_load_si128((__m128i *)(dst + 32));
	r5 = _mm_load_si128((__m128i *)(dst + 40));
	r6 = _mm_load_si128((__m128i *)(dst + 48));
	r7 = _mm_load_si128((__m128i *)(dst + 56));

	t0 = _mm_unpacklo_epi16(r0, r2);
	t1 = _mm_unpacklo_epi16(r4, r6);
	t2 = _mm_unpackhi_epi16(r0, r2);
	t3 = _mm_unpackhi_epi16(r4, r6);
	t4 = _mm_unpacklo_epi16(r1, r3);
	t5 = _mm_unpacklo_epi16(r5, r7);
	t6 = _mm_unpackhi_epi16(r1, r3);
	t7 = _mm_unpackhi_epi16(r5, r7);

	r0 = _mm_unpacklo_epi32(t0, t1);
	r1 = _mm_unpackhi_epi32(t0, t1);
	r2 = _mm_unpacklo_epi32(t2, t3);
	r3 = _mm_unpackhi_epi32(t2, t3);
	r4 = _mm_unpacklo_epi32(t4, t5);
	r5 = _mm_unpackhi_epi32(t4, t5);
	r6 = _mm_unpacklo_epi32(t6, t7);
	r7 = _mm_unpackhi_epi32(t6, t7);

	_mm_store_si128((__m128i *)(dst), r0);
	_mm_store_si128((__m128i *)(dst + 8), r1);
	_mm_store_si128((__m128i *)(dst + 16), r2);
	_mm_store_si128((__m128i *)(dst + 24), r3);
	_mm_store_si128((__m128i *)(dst + 32), r4);
	_mm_store_si128((__m128i *)(dst + 40), r5);
	_mm_store_si128((__m128i *)(dst + 48), r6);
	_mm_store_si128((__m128i *)(dst + 56), r7);
}

static __inline void transpose_32x8_10bit(pel_t *dst){
	__m128i t0, t1, t2, t3, t4, t5, t6, t7;
	__m128i r0, r1, r2, r3, r4, r5, r6, r7;
	__m128i n0, n1, n2, n3, n4, n5, n6, n7;
	__m128i l0, l1, l2, l3, l4, l5, l6, l7;
	__m128i m0, m1, m2, m3, m4, m5, m6, m7;
	pel_t *dst1 = dst;

	t0 = _mm_load_si128((__m128i *)(dst));
	t1 = _mm_load_si128((__m128i *)(dst + 32));
	t2 = _mm_load_si128((__m128i *)(dst + 64));
	t3 = _mm_load_si128((__m128i *)(dst + 96));
	t4 = _mm_load_si128((__m128i *)(dst + 128));
	t5 = _mm_load_si128((__m128i *)(dst + 160));
	t6 = _mm_load_si128((__m128i *)(dst + 192));
	t7 = _mm_load_si128((__m128i *)(dst + 224));

	dst += 8;
	r0 = _mm_load_si128((__m128i *)(dst));
	r1 = _mm_load_si128((__m128i *)(dst + 32));
	r2 = _mm_load_si128((__m128i *)(dst + 64));
	r3 = _mm_load_si128((__m128i *)(dst + 96));
	r4 = _mm_load_si128((__m128i *)(dst + 128));
	r5 = _mm_load_si128((__m128i *)(dst + 160));
	r6 = _mm_load_si128((__m128i *)(dst + 192));
	r7 = _mm_load_si128((__m128i *)(dst + 224));

	dst += 8;
	n0 = _mm_load_si128((__m128i *)(dst));
	n1 = _mm_load_si128((__m128i *)(dst + 32));
	n2 = _mm_load_si128((__m128i *)(dst + 64));
	n3 = _mm_load_si128((__m128i *)(dst + 96));
	n4 = _mm_load_si128((__m128i *)(dst + 128));
	n5 = _mm_load_si128((__m128i *)(dst + 160));
	n6 = _mm_load_si128((__m128i *)(dst + 192));
	n7 = _mm_load_si128((__m128i *)(dst + 224));

	dst += 8;
	l0 = _mm_load_si128((__m128i *)(dst));
	l1 = _mm_load_si128((__m128i *)(dst + 32));
	l2 = _mm_load_si128((__m128i *)(dst + 64));
	l3 = _mm_load_si128((__m128i *)(dst + 96));
	l4 = _mm_load_si128((__m128i *)(dst + 128));
	l5 = _mm_load_si128((__m128i *)(dst + 160));
	l6 = _mm_load_si128((__m128i *)(dst + 192));
	l7 = _mm_load_si128((__m128i *)(dst + 224));

	m0 = _mm_unpacklo_epi16(t0, t1);
	m1 = _mm_unpackhi_epi16(t0, t1);
	m2 = _mm_unpacklo_epi16(t2, t3);
	m3 = _mm_unpackhi_epi16(t2, t3);
	m4 = _mm_unpacklo_epi16(t4, t5);
	m5 = _mm_unpackhi_epi16(t4, t5);
	m6 = _mm_unpacklo_epi16(t6, t7);
	m7 = _mm_unpackhi_epi16(t6, t7);

	t0 = _mm_unpacklo_epi32(m0, m2);
	t1 = _mm_unpackhi_epi32(m0, m2);
	t2 = _mm_unpacklo_epi32(m1, m3);
	t3 = _mm_unpackhi_epi32(m1, m3);
	t4 = _mm_unpacklo_epi32(m4, m6);
	t5 = _mm_unpackhi_epi32(m4, m6);
	t6 = _mm_unpacklo_epi32(m5, m7);
	t7 = _mm_unpackhi_epi32(m5, m7);

	m0 = _mm_unpacklo_epi64(t0, t4);
	m1 = _mm_unpackhi_epi64(t0, t4);
	m2 = _mm_unpacklo_epi64(t2, t6);
	m3 = _mm_unpackhi_epi64(t2, t6);
	m4 = _mm_unpacklo_epi64(t1, t5);
	m5 = _mm_unpackhi_epi64(t1, t5);
	m6 = _mm_unpacklo_epi64(t3, t7);
	m7 = _mm_unpackhi_epi64(t3, t7);

	_mm_store_si128((__m128i *)dst1, m0);
	_mm_store_si128((__m128i *)(dst1 + 8), m1);
	_mm_store_si128((__m128i *)(dst1 + 16), m4);
	_mm_store_si128((__m128i *)(dst1 + 24), m5);
	_mm_store_si128((__m128i *)(dst1 + 32), m2);
	_mm_store_si128((__m128i *)(dst1 + 40), m3);
	_mm_store_si128((__m128i *)(dst1 + 48), m6);
	_mm_store_si128((__m128i *)(dst1 + 56), m7);

	m0 = _mm_unpacklo_epi16(r0, r1);
	m1 = _mm_unpackhi_epi16(r0, r1);
	m2 = _mm_unpacklo_epi16(r2, r3);
	m3 = _mm_unpackhi_epi16(r2, r3);
	m4 = _mm_unpacklo_epi16(r4, r5);
	m5 = _mm_unpackhi_epi16(r4, r5);
	m6 = _mm_unpacklo_epi16(r6, r7);
	m7 = _mm_unpackhi_epi16(r6, r7);

	t0 = _mm_unpacklo_epi32(m0, m2);
	t1 = _mm_unpackhi_epi32(m0, m2);
	t2 = _mm_unpacklo_epi32(m1, m3);
	t3 = _mm_unpackhi_epi32(m1, m3);
	t4 = _mm_unpacklo_epi32(m4, m6);
	t5 = _mm_unpackhi_epi32(m4, m6);
	t6 = _mm_unpacklo_epi32(m5, m7);
	t7 = _mm_unpackhi_epi32(m5, m7);

	m0 = _mm_unpacklo_epi64(t0, t4);
	m1 = _mm_unpackhi_epi64(t0, t4);
	m2 = _mm_unpacklo_epi64(t2, t6);
	m3 = _mm_unpackhi_epi64(t2, t6);
	m4 = _mm_unpacklo_epi64(t1, t5);
	m5 = _mm_unpackhi_epi64(t1, t5);
	m6 = _mm_unpacklo_epi64(t3, t7);
	m7 = _mm_unpackhi_epi64(t3, t7);

	dst1 += 64;
	_mm_store_si128((__m128i *)dst1, m0);
	_mm_store_si128((__m128i *)(dst1 + 8), m1);
	_mm_store_si128((__m128i *)(dst1 + 16), m4);
	_mm_store_si128((__m128i *)(dst1 + 24), m5);
	_mm_store_si128((__m128i *)(dst1 + 32), m2);
	_mm_store_si128((__m128i *)(dst1 + 40), m3);
	_mm_store_si128((__m128i *)(dst1 + 48), m6);
	_mm_store_si128((__m128i *)(dst1 + 56), m7);

	m0 = _mm_unpacklo_epi16(n0, n1);
	m1 = _mm_unpackhi_epi16(n0, n1);
	m2 = _mm_unpacklo_epi16(n2, n3);
	m3 = _mm_unpackhi_epi16(n2, n3);
	m4 = _mm_unpacklo_epi16(n4, n5);
	m5 = _mm_unpackhi_epi16(n4, n5);
	m6 = _mm_unpacklo_epi16(n6, n7);
	m7 = _mm_unpackhi_epi16(n6, n7);

	t0 = _mm_unpacklo_epi32(m0, m2);
	t1 = _mm_unpackhi_epi32(m0, m2);
	t2 = _mm_unpacklo_epi32(m1, m3);
	t3 = _mm_unpackhi_epi32(m1, m3);
	t4 = _mm_unpacklo_epi32(m4, m6);
	t5 = _mm_unpackhi_epi32(m4, m6);
	t6 = _mm_unpacklo_epi32(m5, m7);
	t7 = _mm_unpackhi_epi32(m5, m7);

	m0 = _mm_unpacklo_epi64(t0, t4);
	m1 = _mm_unpackhi_epi64(t0, t4);
	m2 = _mm_unpacklo_epi64(t2, t6);
	m3 = _mm_unpackhi_epi64(t2, t6);
	m4 = _mm_unpacklo_epi64(t1, t5);
	m5 = _mm_unpackhi_epi64(t1, t5);
	m6 = _mm_unpacklo_epi64(t3, t7);
	m7 = _mm_unpackhi_epi64(t3, t7);

	dst1 += 64;
	_mm_store_si128((__m128i *)dst1, m0);
	_mm_store_si128((__m128i *)(dst1 + 8), m1);
	_mm_store_si128((__m128i *)(dst1 + 16), m4);
	_mm_store_si128((__m128i *)(dst1 + 24), m5);
	_mm_store_si128((__m128i *)(dst1 + 32), m2);
	_mm_store_si128((__m128i *)(dst1 + 40), m3);
	_mm_store_si128((__m128i *)(dst1 + 48), m6);
	_mm_store_si128((__m128i *)(dst1 + 56), m7);

	m0 = _mm_unpacklo_epi16(l0, l1);
	m1 = _mm_unpackhi_epi16(l0, l1);
	m2 = _mm_unpacklo_epi16(l2, l3);
	m3 = _mm_unpackhi_epi16(l2, l3);
	m4 = _mm_unpacklo_epi16(l4, l5);
	m5 = _mm_unpackhi_epi16(l4, l5);
	m6 = _mm_unpacklo_epi16(l6, l7);
	m7 = _mm_unpackhi_epi16(l6, l7);

	t0 = _mm_unpacklo_epi32(m0, m2);
	t1 = _mm_unpackhi_epi32(m0, m2);
	t2 = _mm_unpacklo_epi32(m1, m3);
	t3 = _mm_unpackhi_epi32(m1, m3);
	t4 = _mm_unpacklo_epi32(m4, m6);
	t5 = _mm_unpackhi_epi32(m4, m6);
	t6 = _mm_unpacklo_epi32(m5, m7);
	t7 = _mm_unpackhi_epi32(m5, m7);

	m0 = _mm_unpacklo_epi64(t0, t4);
	m1 = _mm_unpackhi_epi64(t0, t4);
	m2 = _mm_unpacklo_epi64(t2, t6);
	m3 = _mm_unpackhi_epi64(t2, t6);
	m4 = _mm_unpacklo_epi64(t1, t5);
	m5 = _mm_unpackhi_epi64(t1, t5);
	m6 = _mm_unpacklo_epi64(t3, t7);
	m7 = _mm_unpackhi_epi64(t3, t7);

	dst1 += 64;
	_mm_store_si128((__m128i *)dst1, m0);
	_mm_store_si128((__m128i *)(dst1 + 8), m1);
	_mm_store_si128((__m128i *)(dst1 + 16), m4);
	_mm_store_si128((__m128i *)(dst1 + 24), m5);
	_mm_store_si128((__m128i *)(dst1 + 32), m2);
	_mm_store_si128((__m128i *)(dst1 + 40), m3);
	_mm_store_si128((__m128i *)(dst1 + 48), m6);
	_mm_store_si128((__m128i *)(dst1 + 56), m7);

}

static __inline void transpose_8x32_10bit(pel_t *dst){
	transpose_8x8_stride_10bit(dst, 32, dst, 32);
	transpose_8x8_stride_10bit(dst + 8 * 32, 32, dst + 8, 32);
	transpose_8x8_stride_10bit(dst + 16 * 32, 32, dst + 16, 32);
	transpose_8x8_stride_10bit(dst + 24 * 32, 32, dst + 24, 32);
}

static __inline void transpose_32x32_10bit(pel_t *dst){
	transpose_8x8_stride_10bit(dst, 32, dst, 32);
	transpose_8x8_stride_10bit(dst + 264, 32, dst + 264, 32);
	transpose_8x8_stride_10bit(dst + 528, 32, dst + 528, 32);
	transpose_8x8_stride_10bit(dst + 792, 32, dst + 792, 32);

	transpose_8x8_stride_x2_10bit(dst + 8, 32, dst + 256, 32);
	transpose_8x8_stride_x2_10bit(dst + 16, 32, dst + 512, 32);
	transpose_8x8_stride_x2_10bit(dst + 24, 32, dst + 768, 32);
	transpose_8x8_stride_x2_10bit(dst + 272, 32, dst + 520, 32);
	transpose_8x8_stride_x2_10bit(dst + 280, 32, dst + 776, 32);
	transpose_8x8_stride_x2_10bit(dst + 536, 32, dst + 784, 32);
}

static __inline void transpose_64x64_10bit(pel_t *dst){
	transpose_8x8_stride_10bit(dst, 64, dst, 64);
	transpose_8x8_stride_10bit(dst + 520, 64, dst + 520, 64);
	transpose_8x8_stride_10bit(dst + 1040, 64, dst + 1040, 64);
	transpose_8x8_stride_10bit(dst + 1560, 64, dst + 1560, 64);
	transpose_8x8_stride_10bit(dst + 2080, 64, dst + 2080, 64);
	transpose_8x8_stride_10bit(dst + 2600, 64, dst + 2600, 64);
	transpose_8x8_stride_10bit(dst + 3120, 64, dst + 3120, 64);
	transpose_8x8_stride_10bit(dst + 3640, 64, dst + 3640, 64);

	transpose_8x8_stride_x2_10bit(dst + 8, 64, dst + 512, 64);
	transpose_8x8_stride_x2_10bit(dst + 16, 64, dst + 1024, 64);
	transpose_8x8_stride_x2_10bit(dst + 24, 64, dst + 1536, 64);
	transpose_8x8_stride_x2_10bit(dst + 32, 64, dst + 2048, 64);
	transpose_8x8_stride_x2_10bit(dst + 40, 64, dst + 2560, 64);
	transpose_8x8_stride_x2_10bit(dst + 48, 64, dst + 3072, 64);
	transpose_8x8_stride_x2_10bit(dst + 56, 64, dst + 3584, 64);

	transpose_8x8_stride_x2_10bit(dst + 528, 64, dst + 1032, 64);
	transpose_8x8_stride_x2_10bit(dst + 536, 64, dst + 1544, 64);
	transpose_8x8_stride_x2_10bit(dst + 544, 64, dst + 2056, 64);
	transpose_8x8_stride_x2_10bit(dst + 552, 64, dst + 2568, 64);
	transpose_8x8_stride_x2_10bit(dst + 560, 64, dst + 3080, 64);
	transpose_8x8_stride_x2_10bit(dst + 568, 64, dst + 3592, 64);

	transpose_8x8_stride_x2_10bit(dst + 1048, 64, dst + 1552, 64);
	transpose_8x8_stride_x2_10bit(dst + 1056, 64, dst + 2064, 64);
	transpose_8x8_stride_x2_10bit(dst + 1064, 64, dst + 2576, 64);
	transpose_8x8_stride_x2_10bit(dst + 1072, 64, dst + 3088, 64);
	transpose_8x8_stride_x2_10bit(dst + 1080, 64, dst + 3600, 64);

	transpose_8x8_stride_x2_10bit(dst + 1568, 64, dst + 2072, 64);
	transpose_8x8_stride_x2_10bit(dst + 1576, 64, dst + 2584, 64);
	transpose_8x8_stride_x2_10bit(dst + 1584, 64, dst + 3096, 64);
	transpose_8x8_stride_x2_10bit(dst + 1592, 64, dst + 3608, 64);

	transpose_8x8_stride_x2_10bit(dst + 2088, 64, dst + 2592, 64);
	transpose_8x8_stride_x2_10bit(dst + 2096, 64, dst + 3104, 64);
	transpose_8x8_stride_x2_10bit(dst + 2104, 64, dst + 3616, 64);

	transpose_8x8_stride_x2_10bit(dst + 2608, 64, dst + 3112, 64);
	transpose_8x8_stride_x2_10bit(dst + 2616, 64, dst + 3624, 64);

	transpose_8x8_stride_x2_10bit(dst + 3128, 64, dst + 3632, 64);
}

void xPredIntraAngAdi_Y_25_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
	int i, j, idx;
	__m128i zero = _mm_setzero_si128();
	__m128i off = _mm_set1_epi32(64);
	__m128i S0, S1, S2, S3;
	__m128i t0, t1, t2, t3;
	__m128i c0, c1;
	int height = iWidth;
	int width = iHeight;
	pel_t *pDst = dst;

	i_dst = max(height, width);
	pSrc += 144;
	if (width == 4){
		__m128i D0;

		for (j = 0; j < height; j ++) {
			idx = (j + 1) >> 3;
			c0 = _mm_set1_epi32(((int*)tab_coeff_mode_11_10bit[j & 0x07])[0]);
			c1 = _mm_set1_epi32(((int*)tab_coeff_mode_11_10bit[j & 0x07])[1]);

			S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
			S1 = _mm_srli_si128(S0, 2);
			S2 = _mm_srli_si128(S0, 4);
			S3 = _mm_srli_si128(S0, 6);

			t0 = _mm_unpacklo_epi16(S0, S1);
			t1 = _mm_unpacklo_epi16(S2, S3);

			t0 = _mm_madd_epi16(t0, c0);
			t1 = _mm_madd_epi16(t1, c1);

			t0 = _mm_add_epi32(t0, t1);

			D0 = _mm_add_epi32(t0, off);
			D0 = _mm_srli_epi32(D0, 7);

			D0 = _mm_packus_epi32(D0, zero);

			_mm_storel_epi64((__m128i*)(dst), D0);
			//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
			dst += i_dst;
		}
		if (height == 4) {
			transpose_4x4_10bit(pDst);
		}
		else{
			transpose_4x16_10bit(pDst);
		}
	}
	else if (width == 8) {
		__m128i D0, D1;

		for (j = 0; j < height; j++) {
			pel_t * pSrc1;
			idx = (j + 1) >> 3;
			c0 = _mm_set1_epi32(((int*)tab_coeff_mode_11_10bit[j & 0x07])[0]);
			c1 = _mm_set1_epi32(((int*)tab_coeff_mode_11_10bit[j & 0x07])[1]);

			pSrc1 = pSrc + idx;
			S0 = _mm_loadu_si128((__m128i*)pSrc1);
			S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
			S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
			S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

			t0 = _mm_unpacklo_epi16(S0, S1);
			t1 = _mm_unpacklo_epi16(S2, S3);
			t2 = _mm_unpackhi_epi16(S0, S1);
			t3 = _mm_unpackhi_epi16(S2, S3);

			t0 = _mm_madd_epi16(t0, c0);
			t1 = _mm_madd_epi16(t1, c1);
			t2 = _mm_madd_epi16(t2, c0);
			t3 = _mm_madd_epi16(t3, c1);

			t0 = _mm_add_epi32(t0, t1);
			t2 = _mm_add_epi32(t2, t3);

			D0 = _mm_add_epi32(t0, off);
			D0 = _mm_srli_epi32(D0, 7);

			D1 = _mm_add_epi32(t2, off);
			D1 = _mm_srli_epi32(D1, 7);

			D0 = _mm_packus_epi32(D0, D1);

			_mm_storeu_si128((__m128i*)(dst), D0);
			//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);

			dst += i_dst;
		}
		if (height == 8) {
			transpose_8x8_10bit(pDst);
		}
		else{
			transpose_8x32_10bit(pDst);
		}
	}
	else{
		for (j = 0; j < height; j++) {
			__m128i D0, D1;

			idx = (j + 1) >> 3;
			c0 = _mm_set1_epi32(((int*)tab_coeff_mode_11_10bit[j & 0x07])[0]);
			c1 = _mm_set1_epi32(((int*)tab_coeff_mode_11_10bit[j & 0x07])[1]);

			for (i = 0; i < width; i += 8, idx += 8) {
				pel_t *pSrc1 = pSrc + idx;
				S0 = _mm_loadu_si128((__m128i*)pSrc1);
				S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
				S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
				S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

				t0 = _mm_unpacklo_epi16(S0, S1);
				t1 = _mm_unpacklo_epi16(S2, S3);
				t2 = _mm_unpackhi_epi16(S0, S1);
				t3 = _mm_unpackhi_epi16(S2, S3);

				t0 = _mm_madd_epi16(t0, c0);
				t1 = _mm_madd_epi16(t1, c1);
				t2 = _mm_madd_epi16(t2, c0);
				t3 = _mm_madd_epi16(t3, c1);

				t0 = _mm_add_epi32(t0, t1);
				t2 = _mm_add_epi32(t2, t3);

				D0 = _mm_add_epi32(t0, off);
				D0 = _mm_srli_epi32(D0, 7);

				D1 = _mm_add_epi32(t2, off);
				D1 = _mm_srli_epi32(D1, 7);

				D0 = _mm_packus_epi32(D0, D1);

				_mm_storeu_si128((__m128i*)(dst + i), D0);
				//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
			}

			dst += i_dst;
		}
		if (width == 16) {
			if (height == 16)
				transpose_16x16_10bit(pDst);
			else
				transpose_16x4_10bit(pDst);
		}
		else if (width == 32) {
			if (height == 32)
				transpose_32x32_10bit(pDst);
			else
				transpose_32x8_10bit(pDst);
		}
		else{
			transpose_64x64_10bit(pDst);
		}
	}
}

void xPredIntraAngAdi_Y_27_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
	int i, j;
	pel_t *pDst = dst;
	__m128i zero = _mm_setzero_si128();
	__m128i off = _mm_set1_epi32(64);
	__m128i S0, S1, S2, S3;
	__m128i t0, t1, t2, t3;
	__m128i c0, c1;

	pSrc += 144;
	if (iWidth == iHeight)
	{
		if (iHeight == 4){
			__m128i D0;

			for (j = 0; j < iWidth; j ++) {
				int idx = tab_idx_mode_9_10bit[j];
				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[1]);

				S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
				S1 = _mm_srli_si128(S0, 2);
				S2 = _mm_srli_si128(S0, 4);
				S3 = _mm_srli_si128(S0, 6);

				t0 = _mm_unpacklo_epi16(S0, S1);
				t1 = _mm_unpacklo_epi16(S2, S3);

				t0 = _mm_madd_epi16(t0, c0);
				t1 = _mm_madd_epi16(t1, c1);

				t0 = _mm_add_epi32(t0, t1);

				D0 = _mm_add_epi32(t0, off);
				D0 = _mm_srli_epi32(D0, 7);

				D0 = _mm_packus_epi32(D0, zero);

				_mm_storel_epi64((__m128i*)(dst), D0);
				//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
				dst += i_dst;
			}
			transpose_4x4_10bit(pDst);
		}
		else if (iHeight == 8) {
			__m128i D0, D1;
			pel_t * pSrc1;

			for (j = 0; j < iWidth; j++) {
				int idx = tab_idx_mode_9_10bit[j];
				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[1]);

				pSrc1 = pSrc + idx;
				S0 = _mm_loadu_si128((__m128i*)pSrc1);
				S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
				S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
				S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

				t0 = _mm_unpacklo_epi16(S0, S1);
				t1 = _mm_unpacklo_epi16(S2, S3);
				t2 = _mm_unpackhi_epi16(S0, S1);
				t3 = _mm_unpackhi_epi16(S2, S3);

				t0 = _mm_madd_epi16(t0, c0);
				t1 = _mm_madd_epi16(t1, c1);
				t2 = _mm_madd_epi16(t2, c0);
				t3 = _mm_madd_epi16(t3, c1);

				t0 = _mm_add_epi32(t0, t1);
				t2 = _mm_add_epi32(t2, t3);

				D0 = _mm_add_epi32(t0, off);
				D0 = _mm_srli_epi32(D0, 7);

				D1 = _mm_add_epi32(t2, off);
				D1 = _mm_srli_epi32(D1, 7);

				D0 = _mm_packus_epi32(D0, D1);

				_mm_storeu_si128((__m128i*)(dst), D0);
				//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);

				dst += i_dst;
			}
			transpose_8x8_10bit(pDst);
		}
		else {
			for (j = 0; j < iWidth; j++) {
				__m128i D0, D1;

				int idx = tab_idx_mode_9_10bit[j];
				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[1]);

				for (i = 0; i < iWidth; i += 8, idx += 8) {
					pel_t *pSrc1 = pSrc + idx;
					S0 = _mm_loadu_si128((__m128i*)pSrc1);
					S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
					S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
					S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

					t0 = _mm_unpacklo_epi16(S0, S1);
					t1 = _mm_unpacklo_epi16(S2, S3);
					t2 = _mm_unpackhi_epi16(S0, S1);
					t3 = _mm_unpackhi_epi16(S2, S3);

					t0 = _mm_madd_epi16(t0, c0);
					t1 = _mm_madd_epi16(t1, c1);
					t2 = _mm_madd_epi16(t2, c0);
					t3 = _mm_madd_epi16(t3, c1);

					t0 = _mm_add_epi32(t0, t1);
					t2 = _mm_add_epi32(t2, t3);

					D0 = _mm_add_epi32(t0, off);
					D0 = _mm_srli_epi32(D0, 7);

					D1 = _mm_add_epi32(t2, off);
					D1 = _mm_srli_epi32(D1, 7);

					D0 = _mm_packus_epi32(D0, D1);

					_mm_storeu_si128((__m128i*)(dst + i), D0);
					//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
				}

				dst += i_dst;
			}
			if (iHeight == 16) {
				transpose_16x16_10bit(pDst);
			}
			else if (iHeight == 32) {
				transpose_32x32_10bit(pDst);
			}
			else{
				transpose_64x64_10bit(pDst);
			}
		}
	}
	else{
		i_dst = max(iWidth, iHeight);
		if (iHeight == 4) {
			for (j = 0; j < iWidth; j++) {
				int real_width;
				int idx = tab_idx_mode_9_10bit[j];

				real_width = min(4, 8 - idx + 1);

				if (real_width <= 0) {
					pel_t val = (pel_t)((pSrc[8] * tab_coeff_mode_9_10bit[j][0] + pSrc[9] * tab_coeff_mode_9_10bit[j][1] + pSrc[10] * tab_coeff_mode_9_10bit[j][2] + pSrc[11] * tab_coeff_mode_9_10bit[j][3] + 64) >> 7);
					__m128i D0 = _mm_set1_epi16((pel_t)val);
					_mm_storel_epi64((__m128i*)(dst), D0);
					dst += i_dst;
					j++;

					for (; j < iWidth; j++) {
						val = (pel_t)((pSrc[8] * tab_coeff_mode_9_10bit[j][0] + pSrc[9] * tab_coeff_mode_9_10bit[j][1] + pSrc[10] * tab_coeff_mode_9_10bit[j][2] + pSrc[11] * tab_coeff_mode_9_10bit[j][3] + 64) >> 7);
						D0 = _mm_set1_epi16((pel_t)val);
						_mm_storel_epi64((__m128i*)(dst), D0);
						dst += i_dst;
					}
					break;
				}
				else {
					__m128i D0;
					c0 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[0]);
					c1 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[1]);

					S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
					S1 = _mm_srli_si128(S0, 2);
					S2 = _mm_srli_si128(S0, 4);
					S3 = _mm_srli_si128(S0, 6);

					t0 = _mm_unpacklo_epi16(S0, S1);
					t1 = _mm_unpacklo_epi16(S2, S3);

					t0 = _mm_madd_epi16(t0, c0);
					t1 = _mm_madd_epi16(t1, c1);

					t0 = _mm_add_epi32(t0, t1);

					D0 = _mm_add_epi32(t0, off);
					D0 = _mm_srli_epi32(D0, 7);

					D0 = _mm_packus_epi32(D0, zero);

					_mm_storel_epi64((__m128i*)(dst), D0);

					if (real_width < iWidth)
					{
						D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
						_mm_storel_epi64((__m128i*)(dst + real_width), D0);
					}
				}
				dst += i_dst;
			}
			transpose_4x16_10bit(pDst);
		}
		else if (iHeight == 8) {
			for (j = 0; j < iWidth; j++) {
				int real_width;
				int idx = tab_idx_mode_9_10bit[j];

				real_width = min(8, 16 - idx + 1);

				if (real_width <= 0) {
					pel_t val = (pel_t)((pSrc[16] * tab_coeff_mode_9_10bit[j][0] + pSrc[16 + 1] * tab_coeff_mode_9_10bit[j][1] + pSrc[16 + 2] * tab_coeff_mode_9_10bit[j][2] + pSrc[16 + 3] * tab_coeff_mode_9_10bit[j][3] + 64) >> 7);
					__m128i D0 = _mm_set1_epi16((pel_t)val);
					_mm_store_si128((__m128i*)(dst), D0);
					dst += i_dst;
					j++;

					for (; j < iHeight; j++)
					{
						val = (pel_t)((pSrc[16] * tab_coeff_mode_9_10bit[j][0] + pSrc[16 + 1] * tab_coeff_mode_9_10bit[j][1] + pSrc[16 + 2] * tab_coeff_mode_9_10bit[j][2] + pSrc[16 + 3] * tab_coeff_mode_9_10bit[j][3] + 64) >> 7);
						D0 = _mm_set1_epi16((pel_t)val);
						_mm_store_si128((__m128i*)(dst), D0);
						dst += i_dst;
					}
					break;
				}
				else {
					__m128i D0, D1;
					pel_t *pSrc1 = pSrc + idx;
					c0 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[0]);
					c1 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[1]);

					S0 = _mm_loadu_si128((__m128i*)pSrc1);
					S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
					S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
					S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

					t0 = _mm_unpacklo_epi16(S0, S1);
					t1 = _mm_unpacklo_epi16(S2, S3);
					t2 = _mm_unpackhi_epi16(S0, S1);
					t3 = _mm_unpackhi_epi16(S2, S3);

					t0 = _mm_madd_epi16(t0, c0);
					t1 = _mm_madd_epi16(t1, c1);
					t2 = _mm_madd_epi16(t2, c0);
					t3 = _mm_madd_epi16(t3, c1);

					t0 = _mm_add_epi32(t0, t1);
					t2 = _mm_add_epi32(t2, t3);

					D0 = _mm_add_epi32(t0, off);
					D0 = _mm_srli_epi32(D0, 7);

					D1 = _mm_add_epi32(t2, off);
					D1 = _mm_srli_epi32(D1, 7);

					D0 = _mm_packus_epi32(D0, D1);

					_mm_storeu_si128((__m128i*)(dst), D0);
					//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);


					if (real_width < 8)
					{
						D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
						_mm_storeu_si128((__m128i*)(dst + real_width), D0);
						//dst[i] = dst[real_width - 1];
					}

				}

				dst += i_dst;
			}
			transpose_8x32_10bit(pDst);
		}
		else {
			int iHeight2 = iHeight << 1;
			for (j = 0; j < iWidth; j++) {
				int real_width;
				int idx = tab_idx_mode_9_10bit[j];

				real_width = min(iHeight, iHeight2 - idx + 1);

				if (real_width <= 0) {
					pel_t val = (pel_t)((pSrc[iHeight2] * tab_coeff_mode_9_10bit[j][0] + pSrc[iHeight2 + 1] * tab_coeff_mode_9_10bit[j][1] + pSrc[iHeight2 + 2] * tab_coeff_mode_9_10bit[j][2] + pSrc[iHeight2 + 3] * tab_coeff_mode_9_10bit[j][3] + 64) >> 7);
					__m128i D0 = _mm_set1_epi16((pel_t)val);

					for (i = 0; i < iHeight; i += 8) {
						_mm_store_si128((__m128i*)(dst + i), D0);
					}
					dst += i_dst;
					j++;

					for (; j < iWidth; j++)
					{
						val = (pel_t)((pSrc[iHeight2] * tab_coeff_mode_9_10bit[j][0] + pSrc[iHeight2 + 1] * tab_coeff_mode_9_10bit[j][1] + pSrc[iHeight2 + 2] * tab_coeff_mode_9_10bit[j][2] + pSrc[iHeight2 + 3] * tab_coeff_mode_9_10bit[j][3] + 64) >> 7);
						D0 = _mm_set1_epi16((pel_t)val);
						for (i = 0; i < iHeight; i += 8) {
							_mm_store_si128((__m128i*)(dst + i), D0);
						}
						dst += i_dst;
					}
					break;
				}
				else {
					__m128i D0, D1;

					c0 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[0]);
					c1 = _mm_set1_epi32(((int*)tab_coeff_mode_9_10bit[j])[1]);

					for (i = 0; i < real_width; i += 8, idx += 8) {
						pel_t *pSrc1 = pSrc + idx;
						S0 = _mm_loadu_si128((__m128i*)pSrc1);
						S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
						S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
						S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

						t0 = _mm_unpacklo_epi16(S0, S1);
						t1 = _mm_unpacklo_epi16(S2, S3);
						t2 = _mm_unpackhi_epi16(S0, S1);
						t3 = _mm_unpackhi_epi16(S2, S3);

						t0 = _mm_madd_epi16(t0, c0);
						t1 = _mm_madd_epi16(t1, c1);
						t2 = _mm_madd_epi16(t2, c0);
						t3 = _mm_madd_epi16(t3, c1);

						t0 = _mm_add_epi32(t0, t1);
						t2 = _mm_add_epi32(t2, t3);

						D0 = _mm_add_epi32(t0, off);
						D0 = _mm_srli_epi32(D0, 7);

						D1 = _mm_add_epi32(t2, off);
						D1 = _mm_srli_epi32(D1, 7);

						D0 = _mm_packus_epi32(D0, D1);

						_mm_storeu_si128((__m128i*)(dst + i), D0);
						//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
					}

					if (real_width < iHeight)
					{
						D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
						for (i = real_width; i < iHeight; i += 8) {
							_mm_storeu_si128((__m128i*)(dst + i), D0);
							//dst[i] = dst[real_width - 1];
						}
					}

				}

				dst += i_dst;
			}
			if (iHeight == 16) {
				transpose_16x4_10bit(pDst);
			}
			else if (iHeight == 32) {
				transpose_32x8_10bit(pDst);
			}
		}
	}
}

void xPredIntraAngAdi_Y_29_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
	int i, j;
	__m128i zero = _mm_setzero_si128();
	__m128i off = _mm_set1_epi32(64);
	__m128i S0, S1, S2, S3;
	__m128i t0, t1, t2, t3;
	__m128i c0, c1;
	pel_t* pDst = dst;

	pSrc += 144;
	if (iWidth == iHeight){
		if (iWidth == 4){
			__m128i D0;

			for (j = 0; j < iHeight; j ++) {
				int idx = tab_idx_mode_7_10bit[j];
				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[1]);

				S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
				S1 = _mm_srli_si128(S0, 2);
				S2 = _mm_srli_si128(S0, 4);
				S3 = _mm_srli_si128(S0, 6);

				t0 = _mm_unpacklo_epi16(S0, S1);
				t1 = _mm_unpacklo_epi16(S2, S3);

				t0 = _mm_madd_epi16(t0, c0);
				t1 = _mm_madd_epi16(t1, c1);

				t0 = _mm_add_epi32(t0, t1);

				D0 = _mm_add_epi32(t0, off);
				D0 = _mm_srli_epi32(D0, 7);

				D0 = _mm_packus_epi32(D0, zero);

				_mm_storel_epi64((__m128i*)(dst), D0);
				//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
				dst += i_dst;
			}
			transpose_4x4_10bit(pDst);
		}
		else if (iWidth == 8) {
			__m128i D0, D1;
			pel_t*pSrc1;
			for (j = 0; j < iHeight; j++) {
				int idx = tab_idx_mode_7_10bit[j];
				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[1]);

				pSrc1 = pSrc + idx;
				S0 = _mm_loadu_si128((__m128i*)pSrc1);
				S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
				S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
				S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

				t0 = _mm_unpacklo_epi16(S0, S1);
				t1 = _mm_unpacklo_epi16(S2, S3);
				t2 = _mm_unpackhi_epi16(S0, S1);
				t3 = _mm_unpackhi_epi16(S2, S3);

				t0 = _mm_madd_epi16(t0, c0);
				t1 = _mm_madd_epi16(t1, c1);
				t2 = _mm_madd_epi16(t2, c0);
				t3 = _mm_madd_epi16(t3, c1);

				t0 = _mm_add_epi32(t0, t1);
				t2 = _mm_add_epi32(t2, t3);

				D0 = _mm_add_epi32(t0, off);
				D0 = _mm_srli_epi32(D0, 7);

				D1 = _mm_add_epi32(t2, off);
				D1 = _mm_srli_epi32(D1, 7);

				D0 = _mm_packus_epi32(D0, D1);

				_mm_storeu_si128((__m128i*)(dst), D0);
				//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);

				dst += i_dst;
			}
			transpose_8x8_10bit(pDst);
		}
		else {
			for (j = 0; j < iHeight; j++) {
				__m128i D0, D1;

				int idx = tab_idx_mode_7_10bit[j];
				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[1]);

				for (i = 0; i < iWidth; i += 8, idx += 8) {
					pel_t *pSrc1 = pSrc + idx;
					S0 = _mm_loadu_si128((__m128i*)pSrc1);
					S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
					S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
					S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

					t0 = _mm_unpacklo_epi16(S0, S1);
					t1 = _mm_unpacklo_epi16(S2, S3);
					t2 = _mm_unpackhi_epi16(S0, S1);
					t3 = _mm_unpackhi_epi16(S2, S3);

					t0 = _mm_madd_epi16(t0, c0);
					t1 = _mm_madd_epi16(t1, c1);
					t2 = _mm_madd_epi16(t2, c0);
					t3 = _mm_madd_epi16(t3, c1);

					t0 = _mm_add_epi32(t0, t1);
					t2 = _mm_add_epi32(t2, t3);

					D0 = _mm_add_epi32(t0, off);
					D0 = _mm_srli_epi32(D0, 7);

					D1 = _mm_add_epi32(t2, off);
					D1 = _mm_srli_epi32(D1, 7);

					D0 = _mm_packus_epi32(D0, D1);

					_mm_storeu_si128((__m128i*)(dst + i), D0);
					//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
				}
				dst += i_dst;
			}
			if (iWidth == 16) {
				transpose_16x16_10bit(pDst);
			}
			else if (iWidth == 32) {
				transpose_32x32_10bit(pDst);
			}
			else{
				transpose_64x64_10bit(pDst);
			}
		}
	}
	else{
		int height = iWidth;
		int width = iHeight;
		i_dst = max(height, width);
		if (width == 4) {
			for (j = 0; j < height; j++) {
				int real_width;
				int idx = tab_idx_mode_7_10bit[j];

				real_width = min(4, 8 - idx + 1);

				if (real_width <= 0) {
					pel_t val = (pel_t)((pSrc[8] * tab_coeff_mode_7_10bit[j][0] + pSrc[8 + 1] * tab_coeff_mode_7_10bit[j][1] + pSrc[8 + 2] * tab_coeff_mode_7_10bit[j][2] + pSrc[8 + 3] * tab_coeff_mode_7_10bit[j][3] + 64) >> 7);
					__m128i D0 = _mm_set1_epi16((pel_t)val);
					_mm_storel_epi64((__m128i*)(dst), D0);
					dst += i_dst;
					j++;

					for (; j < height; j++) {
						val = (pel_t)((pSrc[8] * tab_coeff_mode_7_10bit[j][0] + pSrc[8 + 1] * tab_coeff_mode_7_10bit[j][1] + pSrc[8 + 2] * tab_coeff_mode_7_10bit[j][2] + pSrc[8 + 3] * tab_coeff_mode_7_10bit[j][3] + 64) >> 7);
						D0 = _mm_set1_epi16((pel_t)val);
						_mm_storel_epi64((__m128i*)(dst), D0);
						dst += i_dst;
					}
					break;
				}
				else {
					__m128i D0;
					c0 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[0]);
					c1 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[1]);

					S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
					S1 = _mm_srli_si128(S0, 2);
					S2 = _mm_srli_si128(S0, 4);
					S3 = _mm_srli_si128(S0, 6);

					t0 = _mm_unpacklo_epi16(S0, S1);
					t1 = _mm_unpacklo_epi16(S2, S3);

					t0 = _mm_madd_epi16(t0, c0);
					t1 = _mm_madd_epi16(t1, c1);

					t0 = _mm_add_epi32(t0, t1);

					D0 = _mm_add_epi32(t0, off);
					D0 = _mm_srli_epi32(D0, 7);

					D0 = _mm_packus_epi32(D0, zero);

					_mm_storel_epi64((__m128i*)(dst), D0);

					if (real_width < iWidth)
					{
						D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
						_mm_storel_epi64((__m128i*)(dst + real_width), D0);
					}
				}
				dst += i_dst;
			}
			transpose_4x16_10bit(pDst);
		}
		else if (width == 8) {
			for (j = 0; j < height; j++) {
				int real_width;
				int idx = tab_idx_mode_7_10bit[j];

				real_width = min(8, 16 - idx + 1);

				if (real_width <= 0) {
					pel_t val = (pel_t)((pSrc[16] * tab_coeff_mode_7_10bit[j][0] + pSrc[16 + 1] * tab_coeff_mode_7_10bit[j][1] + pSrc[16 + 2] * tab_coeff_mode_7_10bit[j][2] + pSrc[16 + 3] * tab_coeff_mode_7_10bit[j][3] + 64) >> 7);
					__m128i D0 = _mm_set1_epi16((pel_t)val);
					_mm_store_si128((__m128i*)(dst), D0);
					dst += i_dst;
					j++;

					for (; j < height; j++) {
						val = (pel_t)((pSrc[16] * tab_coeff_mode_7_10bit[j][0] + pSrc[16 + 1] * tab_coeff_mode_7_10bit[j][1] + pSrc[16 + 2] * tab_coeff_mode_7_10bit[j][2] + pSrc[16 + 3] * tab_coeff_mode_7_10bit[j][3] + 64) >> 7);
						D0 = _mm_set1_epi16((pel_t)val);
						_mm_store_si128((__m128i*)(dst), D0);
						dst += i_dst;
					}
					break;
				}
				else {
					__m128i D0, D1;
					pel_t *pSrc1 = pSrc + idx;
					c0 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[0]);
					c1 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[1]);

					S0 = _mm_loadu_si128((__m128i*)pSrc1);
					S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
					S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
					S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

					t0 = _mm_unpacklo_epi16(S0, S1);
					t1 = _mm_unpacklo_epi16(S2, S3);
					t2 = _mm_unpackhi_epi16(S0, S1);
					t3 = _mm_unpackhi_epi16(S2, S3);

					t0 = _mm_madd_epi16(t0, c0);
					t1 = _mm_madd_epi16(t1, c1);
					t2 = _mm_madd_epi16(t2, c0);
					t3 = _mm_madd_epi16(t3, c1);

					t0 = _mm_add_epi32(t0, t1);
					t2 = _mm_add_epi32(t2, t3);

					D0 = _mm_add_epi32(t0, off);
					D0 = _mm_srli_epi32(D0, 7);

					D1 = _mm_add_epi32(t2, off);
					D1 = _mm_srli_epi32(D1, 7);

					D0 = _mm_packus_epi32(D0, D1);

					_mm_storeu_si128((__m128i*)(dst), D0);
					//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);


					if (real_width < 8)
					{
						D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
						_mm_storeu_si128((__m128i*)(dst + real_width), D0);
						//dst[i] = dst[real_width - 1];
					}


				}

				dst += i_dst;
			}
			transpose_8x32_10bit(pDst);
		}
		else {
			for (j = 0; j < height; j++) {
				int real_width;
				int idx = tab_idx_mode_7_10bit[j];
				int width2 = width << 1;

				real_width = min(width, width2 - idx + 1);

				if (real_width <= 0) {
					pel_t val = (pel_t)((pSrc[width2] * tab_coeff_mode_7_10bit[j][0] + pSrc[width2 + 1] * tab_coeff_mode_7_10bit[j][1] + pSrc[width2 + 2] * tab_coeff_mode_7_10bit[j][2] + pSrc[width2 + 3] * tab_coeff_mode_7_10bit[j][3] + 64) >> 7);
					__m128i D0 = _mm_set1_epi16((pel_t)val);

					for (i = 0; i < width; i += 8) {
						_mm_store_si128((__m128i*)(dst + i), D0);
					}
					dst += i_dst;
					j++;

					for (; j < height; j++) {
						val = (pel_t)((pSrc[width2] * tab_coeff_mode_7_10bit[j][0] + pSrc[width2 + 1] * tab_coeff_mode_7_10bit[j][1] + pSrc[width2 + 2] * tab_coeff_mode_7_10bit[j][2] + pSrc[width2 + 3] * tab_coeff_mode_7_10bit[j][3] + 64) >> 7);
						D0 = _mm_set1_epi16((pel_t)val);
						for (i = 0; i < width; i += 8) {
							_mm_store_si128((__m128i*)(dst + i), D0);
						}
						dst += i_dst;
					}
					break;
				}
				else {
					__m128i D0, D1;

					c0 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[0]);
					c1 = _mm_set1_epi32(((int*)tab_coeff_mode_7_10bit[j])[1]);

					for (i = 0; i < real_width; i += 8, idx += 8) {
						pel_t *pSrc1 = pSrc + idx;
						S0 = _mm_loadu_si128((__m128i*)pSrc1);
						S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
						S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
						S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

						t0 = _mm_unpacklo_epi16(S0, S1);
						t1 = _mm_unpacklo_epi16(S2, S3);
						t2 = _mm_unpackhi_epi16(S0, S1);
						t3 = _mm_unpackhi_epi16(S2, S3);

						t0 = _mm_madd_epi16(t0, c0);
						t1 = _mm_madd_epi16(t1, c1);
						t2 = _mm_madd_epi16(t2, c0);
						t3 = _mm_madd_epi16(t3, c1);

						t0 = _mm_add_epi32(t0, t1);
						t2 = _mm_add_epi32(t2, t3);

						D0 = _mm_add_epi32(t0, off);
						D0 = _mm_srli_epi32(D0, 7);

						D1 = _mm_add_epi32(t2, off);
						D1 = _mm_srli_epi32(D1, 7);

						D0 = _mm_packus_epi32(D0, D1);

						_mm_storeu_si128((__m128i*)(dst + i), D0);
						//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
					}

					if (real_width < iHeight)
					{
						D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
						for (i = real_width; i < iHeight; i += 8) {
							_mm_storeu_si128((__m128i*)(dst + i), D0);
							//dst[i] = dst[real_width - 1];
						}
					}

				}

				dst += i_dst;
			}
			if (width == 16)
				transpose_16x4_10bit(pDst);
			else if (width == 32)
				transpose_32x8_10bit(pDst);
		}
	}
}


void xPredIntraAngAdi_Y_31_sse128_10bit(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight){
	int i, j, k;
	__m128i zero = _mm_setzero_si128();
	__m128i off = _mm_set1_epi32(64);
	__m128i S0, S1, S2, S3;
	__m128i t0, t1, t2, t3;
	__m128i c0, c1;
	pel_t* pDst = dst;
	int width = iHeight;
	int height = iWidth;
	i_dst = max(height, width);
	pSrc += 144;
	if (width == 4) {
		for (j = 0; j < height; j++) {
			int real_width;
			int idx = tab_idx_mode_5_10bit[j];

			k = j & 0x07;
			real_width = min(4, 8 - idx + 1);

			if (real_width <= 0) {
				pel_t val = (pel_t)((pSrc[8] * tab_coeff_mode_5_10bit[k][0] + pSrc[8 + 1] * tab_coeff_mode_5_10bit[k][1] + pSrc[8 + 2] * tab_coeff_mode_5_10bit[k][2] + pSrc[8 + 3] * tab_coeff_mode_5_10bit[k][3] + 64) >> 7);
				__m128i D0 = _mm_set1_epi16((pel_t)val);
				_mm_storel_epi64((__m128i*)(dst), D0);
				dst += i_dst;
				j++;

				for (; j < height; j++)
				{
					k = j & 0x07;
					val = (pel_t)((pSrc[8] * tab_coeff_mode_5_10bit[k][0] + pSrc[8 + 1] * tab_coeff_mode_5_10bit[k][1] + pSrc[8 + 2] * tab_coeff_mode_5_10bit[k][2] + pSrc[8 + 3] * tab_coeff_mode_5_10bit[k][3] + 64) >> 7);
					D0 = _mm_set1_epi16((pel_t)val);
					_mm_storel_epi64((__m128i*)(dst), D0);
					dst += i_dst;
				}
				break;
			}
			else {
				__m128i D0;
				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_5_10bit[k])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_5_10bit[k])[1]);

				S0 = _mm_loadu_si128((__m128i*)(pSrc + idx));
				S1 = _mm_srli_si128(S0, 2);
				S2 = _mm_srli_si128(S0, 4);
				S3 = _mm_srli_si128(S0, 6);

				t0 = _mm_unpacklo_epi16(S0, S1);
				t1 = _mm_unpacklo_epi16(S2, S3);

				t0 = _mm_madd_epi16(t0, c0);
				t1 = _mm_madd_epi16(t1, c1);

				t0 = _mm_add_epi32(t0, t1);

				D0 = _mm_add_epi32(t0, off);
				D0 = _mm_srli_epi32(D0, 7);

				D0 = _mm_packus_epi32(D0, zero);

				_mm_storel_epi64((__m128i*)(dst), D0);

				if (real_width < iWidth)
				{
					D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
					_mm_storel_epi64((__m128i*)(dst + real_width), D0);
				}
			}
			dst += i_dst;
		}
		if (height == 4)
			transpose_4x4_10bit(pDst);
		else
			transpose_4x16_10bit(pDst);
	}
	else if (width == 8) {
		for (j = 0; j < height; j++) {
			int real_width;
			int idx = tab_idx_mode_5_10bit[j];

			k = j & 0x07;
			real_width = min(8, 16 - idx + 1);

			if (real_width <= 0) {
				pel_t val = (pel_t)((pSrc[16] * tab_coeff_mode_5_10bit[k][0] + pSrc[16 + 1] * tab_coeff_mode_5_10bit[k][1] + pSrc[16 + 2] * tab_coeff_mode_5_10bit[k][2] + pSrc[16 + 3] * tab_coeff_mode_5_10bit[k][3] + 64) >> 7);
				__m128i D0 = _mm_set1_epi16((pel_t)val);
				_mm_store_si128((__m128i*)(dst), D0);
				dst += i_dst;
				j++;

				for (; j < height; j++) {
					k = j & 0x07;
					val = (pel_t)((pSrc[16] * tab_coeff_mode_5_10bit[k][0] + pSrc[16 + 1] * tab_coeff_mode_5_10bit[k][1] + pSrc[16 + 2] * tab_coeff_mode_5_10bit[k][2] + pSrc[16 + 3] * tab_coeff_mode_5_10bit[k][3] + 64) >> 7);
					D0 = _mm_set1_epi16((pel_t)val);
					_mm_store_si128((__m128i*)(dst), D0);
					dst += i_dst;
				}
				break;
			}
			else {
				__m128i D0, D1;
				pel_t * pSrc1 = pSrc + idx;
				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_5_10bit[k])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_5_10bit[k])[1]);

				S0 = _mm_loadu_si128((__m128i*)pSrc1);
				S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
				S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
				S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

				t0 = _mm_unpacklo_epi16(S0, S1);
				t1 = _mm_unpacklo_epi16(S2, S3);
				t2 = _mm_unpackhi_epi16(S0, S1);
				t3 = _mm_unpackhi_epi16(S2, S3);

				t0 = _mm_madd_epi16(t0, c0);
				t1 = _mm_madd_epi16(t1, c1);
				t2 = _mm_madd_epi16(t2, c0);
				t3 = _mm_madd_epi16(t3, c1);

				t0 = _mm_add_epi32(t0, t1);
				t2 = _mm_add_epi32(t2, t3);

				D0 = _mm_add_epi32(t0, off);
				D0 = _mm_srli_epi32(D0, 7);

				D1 = _mm_add_epi32(t2, off);
				D1 = _mm_srli_epi32(D1, 7);

				D0 = _mm_packus_epi32(D0, D1);

				_mm_storeu_si128((__m128i*)(dst), D0);
				//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);


				if (real_width < 8)
				{
					D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
					_mm_storeu_si128((__m128i*)(dst + real_width), D0);
					//dst[i] = dst[real_width - 1];
				}

			}

			dst += i_dst;
		}
		if (height == 8)
			transpose_8x8_10bit(pDst);
		else
			transpose_8x32_10bit(pDst);
	}
	else {
		int width2 = width << 1;
		for (j = 0; j < height; j++) {
			int real_width;
			int idx = tab_idx_mode_5_10bit[j];
			k = j & 0x07;

			real_width = min(width, width2 - idx + 1);

			if (real_width <= 0) {
				pel_t val = (pel_t)((pSrc[width2] * tab_coeff_mode_5_10bit[k][0] + pSrc[width2 + 1] * tab_coeff_mode_5_10bit[k][1] + pSrc[width2 + 2] * tab_coeff_mode_5_10bit[k][2] + pSrc[width2 + 3] * tab_coeff_mode_5_10bit[k][3] + 64) >> 7);
				__m128i D0 = _mm_set1_epi16((pel_t)val);

				for (i = 0; i < width; i += 8) {
					_mm_store_si128((__m128i*)(dst + i), D0);
				}
				dst += i_dst;
				j++;

				for (; j < height; j++) {
					k = j & 0x07;
					val = (pel_t)((pSrc[width2] * tab_coeff_mode_5_10bit[k][0] + pSrc[width2 + 1] * tab_coeff_mode_5_10bit[k][1] + pSrc[width2 + 2] * tab_coeff_mode_5_10bit[k][2] + pSrc[width2 + 3] * tab_coeff_mode_5_10bit[k][3] + 64) >> 7);
					D0 = _mm_set1_epi16((pel_t)val);
					for (i = 0; i < width; i += 8) {
						_mm_store_si128((__m128i*)(dst + i), D0);
					}
					dst += i_dst;
				}
				break;
			}
			else {
				__m128i D0, D1;

				c0 = _mm_set1_epi32(((int*)tab_coeff_mode_5_10bit[k])[0]);
				c1 = _mm_set1_epi32(((int*)tab_coeff_mode_5_10bit[k])[1]);

				for (i = 0; i < real_width; i += 8, idx += 8) {
					pel_t *pSrc1 = pSrc + idx;
					S0 = _mm_loadu_si128((__m128i*)pSrc1);
					S3 = _mm_loadu_si128((__m128i*)(pSrc1 + 3));
					S1 = _mm_loadu_si128((__m128i*)(pSrc1 + 1));
					S2 = _mm_loadu_si128((__m128i*)(pSrc1 + 2));

					t0 = _mm_unpacklo_epi16(S0, S1);
					t1 = _mm_unpacklo_epi16(S2, S3);
					t2 = _mm_unpackhi_epi16(S0, S1);
					t3 = _mm_unpackhi_epi16(S2, S3);

					t0 = _mm_madd_epi16(t0, c0);
					t1 = _mm_madd_epi16(t1, c1);
					t2 = _mm_madd_epi16(t2, c0);
					t3 = _mm_madd_epi16(t3, c1);

					t0 = _mm_add_epi32(t0, t1);
					t2 = _mm_add_epi32(t2, t3);

					D0 = _mm_add_epi32(t0, off);
					D0 = _mm_srli_epi32(D0, 7);

					D1 = _mm_add_epi32(t2, off);
					D1 = _mm_srli_epi32(D1, 7);

					D0 = _mm_packus_epi32(D0, D1);

					_mm_storeu_si128((__m128i*)(dst + i), D0);
					//dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
				}

				if (real_width < iHeight)
				{
					D0 = _mm_set1_epi16((pel_t)dst[real_width - 1]);
					for (i = real_width; i < iHeight; i += 8) {
						_mm_storeu_si128((__m128i*)(dst + i), D0);
						//dst[i] = dst[real_width - 1];
					}
				}

			}

			dst += i_dst;
		}
		if (width == 16) {
			if (height == 16)
				transpose_16x16_10bit(pDst);
			else
				transpose_16x4_10bit(pDst);
		}
		else if (width == 32) {
			if (height == 32)
				transpose_32x32_10bit(pDst);
			else
				transpose_32x8_10bit(pDst);
		}
		else{
			transpose_64x64_10bit(pDst);
		}
	}

}



