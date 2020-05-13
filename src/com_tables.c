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
 *    This product includes software developed by the <organization>.
 * 4. Neither the name of the <organization> nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * For more information, contact us at rgwang@pkusz.edu.cn.
 **************************************************************************************/

#include "com_type.h"
#include <math.h>



#if FS_SAME_SIZE_PER_X_CTU
tab_u32 com_tbl_target_size_list[27][2] = {
    {  4,   4 },
    {  4,   8 },
    {  4,  16 },
    {  4,  32 },
    {  8,   4 },
    {  8,   8 },
    {  8,  16 },
    {  8,  32 },
    {  8,  64 },
    { 16,   4 },
    { 16,   8 },
    { 16,  16 },
    { 16,  32 },
    { 16,  64 },
    { 32,   4 },
    { 32,   8 },
    { 32,  16 },
    { 32,  32 },
    { 32,  64 },
    { 64,   8 },
    { 64,  16 },
    { 64,  32 },
    { 64,  64 },
    { 64, 128 },
    {128,  64 },
    {128, 128 },
    { -1,  -1 }, //end of all possible CU sizes
};
#endif

tab_s8 com_tbl_log2[257] = {
    /* 0, 1 */
    -1, 0,
    /* 2, 3 */
    1, -1,
    /* 4 ~ 7 */
    2, -1, -1, -1,
    /* 8 ~ 15 */
    3, -1, -1, -1, -1, -1, -1, -1,
    /* 16 ~ 31 */
    4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 31 ~ 63 */
    5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 64 ~ 127 */
    6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 128 ~ 255 */
    7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* 256 */
    8
};

s8 com_tbl_tm2[NUM_TRANS_TYPE][2][2];
s8 com_tbl_tm4[NUM_TRANS_TYPE][4][4];
s8 com_tbl_tm8[NUM_TRANS_TYPE][8][8];
s8 com_tbl_tm16[NUM_TRANS_TYPE][16][16];
s8 com_tbl_tm32[NUM_TRANS_TYPE][32][32];
s8 com_tbl_tm64[NUM_TRANS_TYPE][64][64];

tab_s8 com_tbl_c4_trans[4][4] = {
    { 34,    58,    72,     81, },
    { 77,    69,    -7,    -75, },
    { 79,   -33,   -75,     58, },
    { 55,   -84,    73,    -28, }
};

tab_s8 com_tbl_c8_trans[4][4] = {
    { 123,   -35,    -8,    -3, },
    { -32,  -120,    30,    10, },
    { 14,    25,   123,   -22, },
    { 8,    13,    19,   126, },
};

u16 *com_tbl_scan[MAX_CU_LOG2][MAX_CU_LOG2];

tab_s8 com_tbl_mc_l_coeff_hp[16][8] = {
    {  0, 0,   0, 64,  0,   0,  0,  0 },
    {  0, 1,  -3, 63,  4,  -2,  1,  0 },
    { -1, 2,  -5, 62,  8,  -3,  1,  0 },
    { -1, 3,  -8, 60, 13,  -4,  1,  0 },
    { -1, 4, -10, 58, 17,  -5,  1,  0 },
    { -1, 4, -11, 52, 26,  -8,  3, -1 },
    { -1, 3,  -9, 47, 31, -10,  4, -1 },
    { -1, 4, -11, 45, 34, -10,  4, -1 },
    { -1, 4, -11, 40, 40, -11,  4, -1 },
    { -1, 4, -10, 34, 45, -11,  4, -1 },
    { -1, 4, -10, 31, 47,  -9,  3, -1 },
    { -1, 3,  -8, 26, 52, -11,  4, -1 },
    {  0, 1,  -5, 17, 58, -10,  4, -1 },
    {  0, 1,  -4, 13, 60,  -8,  3, -1 },
    {  0, 1,  -3,  8, 62,  -5,  2, -1 },
    {  0, 1,  -2,  4, 63,  -3,  1,  0 }
};

tab_s8 com_tbl_mc_l_coeff[4][8] = {
    { 0,  0,  0, 64,  0,  0,  0,  0},
    { -1, 4, -10, 57, 19,  -7, 3, -1},
    { -1, 4, -11, 40, 40, -11, 4, -1},
    { -1, 3,  -7, 19, 57, -10, 4, -1}
};

tab_s8 com_tbl_mc_c_coeff_hp[32][4] = {
    { 0, 64, 0, 0 },
    { -1, 63,  2,  0 },
    { -2, 62,  4,  0 },
    { -2, 60,  7,  -1 },
    { -2, 58, 10, -2 },
    { -3, 57, 12, -2 },
    { -4, 56, 14, -2 },
    { -4, 55, 15, -2 },
    { -4, 54, 16, -2 },
    { -5, 53, 18, -2 },
    { -6, 52, 20, -2 },
    { -6, 49, 24, -3 },
    { -6, 46, 28, -4 },
    { -5, 44, 29, -4 },
    { -4, 42, 30, -4 },
    { -4, 39, 33, -4 },
    { -4, 36, 36, -4 },
    { -4, 33, 39, -4 },
    { -4, 30, 42, -4 },
    { -4, 29, 44, -5 },
    { -4, 28, 46, -6 },
    { -3, 24, 49, -6 },
    { -2, 20, 52, -6 },
    { -2, 18, 53, -5 },
    { -2, 16, 54, -4 },
    { -2, 15, 55, -4 },
    { -2, 14, 56, -4 },
    { -2, 12, 57, -3 },
    { -2, 10, 58, -2 },
    { -1,  7, 60, -2 },
    { 0,  4, 62, -2 },
    { 0,  2, 63, -1 },
};

tab_s8 com_tbl_mc_c_coeff[8][4] = {
    { 0, 64, 0, 0 },
    { -4, 62, 6, 0 },
    { -6, 56, 15, -1 },
    { -5, 47, 25, -3 },
    { -4, 36, 36, -4 },
    { -3, 25, 47, -5 },
    { -1, 15, 56, -6 },
    { 0, 6, 62, -4 }
};

tab_u8 com_tbl_qp_chroma_ajudst[64] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 42, 43, 43, 44, 44, 45, 45,
    46, 46, 47, 47, 48, 48, 48, 49, 49, 49,
    50, 50, 50, 51
};

tab_u8 com_tbl_qp_chroma_adjust_enc[64] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 33, 34, 35, 36, 37, 38,
    39, 40, 41, 42, 43, 44, 44, 44, 45, 46,
    47, 48, 48, 49, 50, 51, 52, 53, 54, 55,
    56, 57, 58, 59
};

void com_dct_coef_create()
{
    int i, c = 2;
    const double PI = 3.14159265358979323846;

    for (i = 0; i < 6; i++) {
        s8 *iT = NULL;
        const double s = sqrt((double)c) * 32;
        switch (i) {
        case 0:
            iT = com_tbl_tm2[0][0];
            break;
        case 1:
            iT = com_tbl_tm4[0][0];
            break;
        case 2:
            iT = com_tbl_tm8[0][0];
            break;
        case 3:
            iT = com_tbl_tm16[0][0];
            break;
        case 4:
            iT = com_tbl_tm32[0][0];
            break;
        case 5:
            iT = com_tbl_tm64[0][0];
            break;
        default:
            exit(0);
            break;
        }

        for (int k = 0; k < c; k++) {
            for (int n = 0; n < c; n++) {
                double w0, v;

                // DCT-II
                w0 = k == 0 ? sqrt(0.5) : 1;
                v = cos(PI * (n + 0.5) * k / c) * w0 * sqrt(2.0 / c);
                iT[DCT2 *c *c + k *c + n] = (s8)(s * v + (v > 0 ? 0.5 : -0.5));

                // DCT-VIII
                v = cos(PI * (k + 0.5) * (n + 0.5) / (c + 0.5)) * sqrt(2.0 / (c + 0.5));
                iT[DCT8 *c *c + k *c + n] = (s8)(s * v + (v > 0 ? 0.5 : -0.5));

                // DST-VII
                v = sin(PI * (k + 0.5) * (n + 1) / (c + 0.5)) * sqrt(2.0 / (c + 0.5));
                iT[DST7 *c *c + k *c + n] = (s8)(s * v + (v > 0 ? 0.5 : -0.5));

            }
        }
        c <<= 1;
    }

    com_tbl_sqrt[0] = sqrt(4.0 * 8.0);
    com_tbl_sqrt[1] = sqrt(16.0 * 8.0);
}

double com_tbl_sqrt[2];

tab_u32 com_tbl_wq_default_param[2][6] = {
    { 67, 71, 71, 80, 80, 106 },
    { 64, 49, 53, 58, 58,  64 }
};

tab_u8 com_tbl_wq_default_matrix_4x4[16] = {
    64, 64, 64, 68, 64, 64, 68, 72, 64, 68, 76, 80, 72, 76, 84, 96
};

tab_u8 com_tbl_wq_default_matrix_8x8[64] = {
    64,  64,  64,  64,  68,  68,  72,  76,
    64,  64,  64,  68,  72,  76,  84,  92,
    64,  64,  68,  72,  76,  80,  88, 100,
    64,  68,  72,  80,  84,  92, 100, 112,
    68,  72,  80,  84,  92, 104, 112, 128,
    76,  80,  84,  92, 104, 116, 132, 152,
    96, 100, 104, 116, 124, 140, 164, 188,
    104, 108, 116, 128, 152, 172, 192, 216
};

tab_u8 com_tbl_wq_model_8x8[4][64] = {
    //   l a b c d h
    //   0 1 2 3 4 5
    {
        // Mode 0
        0, 0, 0, 4, 4, 4, 5, 5,
        0, 0, 3, 3, 3, 3, 5, 5,
        0, 3, 2, 2, 1, 1, 5, 5,
        4, 3, 2, 2, 1, 5, 5, 5,
        4, 3, 1, 1, 5, 5, 5, 5,
        4, 3, 1, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5
    }, {
        // Mode 1
        0, 0, 0, 4, 4, 4, 5, 5,
        0, 0, 4, 4, 4, 4, 5, 5,
        0, 3, 2, 2, 2, 1, 5, 5,
        3, 3, 2, 2, 1, 5, 5, 5,
        3, 3, 2, 1, 5, 5, 5, 5,
        3, 3, 1, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5
    }, {
        // Mode 2
        0, 0, 0, 4, 4, 3, 5, 5,
        0, 0, 4, 4, 3, 2, 5, 5,
        0, 4, 4, 3, 2, 1, 5, 5,
        4, 4, 3, 2, 1, 5, 5, 5,
        4, 3, 2, 1, 5, 5, 5, 5,
        3, 2, 1, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5
    }, {
        // Mode 3
        0, 0, 0, 3, 2, 1, 5, 5,
        0, 0, 4, 3, 2, 1, 5, 5,
        0, 4, 4, 3, 2, 1, 5, 5,
        3, 3, 3, 3, 2, 5, 5, 5,
        2, 2, 2, 2, 5, 5, 5, 5,
        1, 1, 1, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5
    }
};

tab_u8 com_tbl_wq_model_4x4[4][16] = {
    //   l a b c d h
    //   0 1 2 3 4 5
    {
        // Mode 0
        0, 4, 3, 5,
        4, 2, 1, 5,
        3, 1, 1, 5,
        5, 5, 5, 5
    }, {
        // Mode 1
        0, 4, 4, 5,
        3, 2, 2, 5,
        3, 2, 1, 5,
        5, 5, 5, 5
    }, {

        // Mode 2
        0, 4, 3, 5,
        4, 3, 2, 5,
        3, 2, 1, 5,
        5, 5, 5, 5
    }, {
        // Mode 3
        0, 3, 1, 5,
        3, 4, 2, 5,
        1, 2, 2, 5,
        5, 5, 5, 5
    }
};

tab_u8 com_tbl_part_num[8] = {
    1, // SIZE_2Nx2N
    2, // SIZE_2NxnU
    2, // SIZE_2NxnD
    2, // SIZE_nLx2N
    2, // SIZE_nRx2N
    4, // SIZE_NxN
    4, // SIZE_2NxhN
    4  // SIZE_hNx2N
};

const part_size_t com_tbl_tb_part[8] = {
    SIZE_2Nx2N, // SIZE_2Nx2N
    SIZE_2NxhN, // SIZE_2NxnU
    SIZE_2NxhN, // SIZE_2NxnD
    SIZE_hNx2N, // SIZE_nLx2N
    SIZE_hNx2N, // SIZE_nRx2N
    SIZE_NxN,   // SIZE_NxN
    SIZE_2NxhN, // SIZE_2NxhN
    SIZE_hNx2N  // SIZE_hNx2N
};


tab_s8 tab_auc_dir_dxdy[2][IPD_CNT][2] = {
    {
        // dx/dy
        {0, 0}, {0, 0}, {0, 0}, {11, 2}, {2, 0},
        {11, 3}, {1, 0}, {93, 7}, {1, 1}, {93, 8},
        {1, 2}, {1, 3}, {0, 0}, {1, 3}, {1, 2},
        {93, 8}, {1, 1}, {93, 7}, {1, 0}, {11, 3},
        {2, 0}, {11, 2}, {4, 0}, {8, 0}, {0, 0},
        {8, 0}, {4, 0}, {11, 2}, {2, 0}, {11, 3},
        {1, 0}, {93, 7}, {1, 1},
    },
    {
        // dy/dx
        {0, 0}, {0, 0}, {0, 0}, {93, 8}, {1, 1},
        {93, 7}, {1, 0}, {11, 3}, {2, 0}, {11, 2},
        {4, 0}, {8, 0}, {0, 0}, {8, 0}, {4, 0},
        {11, 2}, {2, 0}, {11, 3}, {1, 0}, {93, 7},
        {1, 1}, {93, 8}, {1, 2}, {1, 3}, {0, 0},
        {1, 3}, {1, 2}, {93, 8}, {1, 1}, {93, 7},
        {1, 0}, {11, 3}, {2, 0}
    }
};
