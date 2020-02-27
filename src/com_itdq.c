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

#include "com_modules.h"

static int get_inv_trans_shift(int log2_size, int type, int bit_depth)
{
    return ((type == 0) ? 5 : (15 + 5 - bit_depth));
}

/******************   DCT-2   ******************************************/

static avs3_always_inline void dct_butterfly_h4(s16 *src, s16 *dst, int line, int shift, int bit_depth)
{
    int j;
    int E[2], O[2];
    int add = 1 << (shift - 1);
    int max_tr_val = (1 << bit_depth) - 1;
    int min_tr_val = -(1 << bit_depth);

    for (j = 0; j < line; j++) {
        /* Utilizing symmetry properties to the maximum to minimize the number of multiplications */
        O[0] = com_tbl_tm4[DCT2][1][0] * src[1 * line + j] + com_tbl_tm4[DCT2][3][0] * src[3 * line + j];
        O[1] = com_tbl_tm4[DCT2][1][1] * src[1 * line + j] + com_tbl_tm4[DCT2][3][1] * src[3 * line + j];
        E[0] = com_tbl_tm4[DCT2][0][0] * src[0 * line + j] + com_tbl_tm4[DCT2][2][0] * src[2 * line + j];
        E[1] = com_tbl_tm4[DCT2][0][1] * src[0 * line + j] + com_tbl_tm4[DCT2][2][1] * src[2 * line + j];
        /* Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector */
        dst[j * 4 + 0] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (E[0] + O[0] + add) >> shift);
        dst[j * 4 + 1] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (E[1] + O[1] + add) >> shift);
        dst[j * 4 + 2] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (E[1] - O[1] + add) >> shift);
        dst[j * 4 + 3] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (E[0] - O[0] + add) >> shift);
    }
}

static avs3_always_inline void dct_butterfly_h8(s16 *src, s16 *dst, int line, int shift, int bit_depth)
{
    int j, k;
    int E[4], O[4];
    int EE[2], EO[2];
    int add = 1 << (shift - 1);
    int max_tr_val = (1 << bit_depth) - 1;
    int min_tr_val = -(1 << bit_depth);

    for (j = 0; j < line; j++) {
        /* Utilizing symmetry properties to the maximum to minimize the number of multiplications */
        for (k = 0; k < 4; k++) {
            O[k] = com_tbl_tm8[DCT2][1][k] * src[1 * line + j] + com_tbl_tm8[DCT2][3][k] * src[3 * line + j] + com_tbl_tm8[DCT2][5][k] * src[5 * line + j] + com_tbl_tm8[DCT2][7][k] * src[7 * line + j];
        }
        EO[0] = com_tbl_tm8[DCT2][2][0] * src[2 * line + j] + com_tbl_tm8[DCT2][6][0] * src[6 * line + j];
        EO[1] = com_tbl_tm8[DCT2][2][1] * src[2 * line + j] + com_tbl_tm8[DCT2][6][1] * src[6 * line + j];
        EE[0] = com_tbl_tm8[DCT2][0][0] * src[0 * line + j] + com_tbl_tm8[DCT2][4][0] * src[4 * line + j];
        EE[1] = com_tbl_tm8[DCT2][0][1] * src[0 * line + j] + com_tbl_tm8[DCT2][4][1] * src[4 * line + j];
        /* Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector */
        E[0] = EE[0] + EO[0];
        E[3] = EE[0] - EO[0];
        E[1] = EE[1] + EO[1];
        E[2] = EE[1] - EO[1];
        for (k = 0; k < 4; k++) {
            dst[j * 8 + k] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (E[k] + O[k] + add) >> shift);
            dst[j * 8 + k + 4] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (E[3 - k] - O[3 - k] + add) >> shift);
        }
    }
}

static avs3_always_inline void dct_butterfly_h16(s16 *src, s16 *dst, int line, int shift, int bit_depth)
{
    int j, k;
    int E[8], O[8];
    int EE[4], EO[4];
    int EEE[2], EEO[2];
    int add = 1 << (shift - 1);
    int max_tr_val = (1 << bit_depth) - 1;
    int min_tr_val = -(1 << bit_depth);

    for (j = 0; j < line; j++) {
        /* Utilizing symmetry properties to the maximum to minimize the number of multiplications */
        for (k = 0; k < 8; k++) {
            O[k] = com_tbl_tm16[DCT2][1][k] * src[1 * line + j] + com_tbl_tm16[DCT2][3][k] * src[3 * line + j] + com_tbl_tm16[DCT2][5][k] * src[5 * line + j] + com_tbl_tm16[DCT2][7][k] * src[7 * line + j] +
                   com_tbl_tm16[DCT2][9][k] * src[9 * line + j] + com_tbl_tm16[DCT2][11][k] * src[11 * line + j] + com_tbl_tm16[DCT2][13][k] * src[13 * line + j] + com_tbl_tm16[DCT2][15][k] * src[15 * line + j];
        }
        for (k = 0; k < 4; k++) {
            EO[k] = com_tbl_tm16[DCT2][2][k] * src[2 * line + j] + com_tbl_tm16[DCT2][6][k] * src[6 * line + j] + com_tbl_tm16[DCT2][10][k] * src[10 * line + j] + com_tbl_tm16[DCT2][14][k] * src[14 * line + j];
        }
        EEO[0] = com_tbl_tm16[DCT2][4][0] * src[4 * line + j] + com_tbl_tm16[DCT2][12][0] * src[12 * line + j];
        EEE[0] = com_tbl_tm16[DCT2][0][0] * src[0 * line + j] + com_tbl_tm16[DCT2][8][0] * src[8 * line + j];
        EEO[1] = com_tbl_tm16[DCT2][4][1] * src[4 * line + j] + com_tbl_tm16[DCT2][12][1] * src[12 * line + j];
        EEE[1] = com_tbl_tm16[DCT2][0][1] * src[0 * line + j] + com_tbl_tm16[DCT2][8][1] * src[8 * line + j];
        /* Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector */
        for (k = 0; k < 2; k++) {
            EE[k] = EEE[k] + EEO[k];
            EE[k + 2] = EEE[1 - k] - EEO[1 - k];
        }
        for (k = 0; k < 4; k++) {
            E[k] = EE[k] + EO[k];
            E[k + 4] = EE[3 - k] - EO[3 - k];
        }
        for (k = 0; k < 8; k++) {
            dst[j * 16 + k] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (E[k] + O[k] + add) >> shift);
            dst[j * 16 + k + 8] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (E[7 - k] - O[7 - k] + add) >> shift);
        }
    }
}

static avs3_always_inline void dct_butterfly_h32(s16 *src, s16 *dst, int line, int shift, int bit_depth)
{
    int j, k;
    int E[16], O[16];
    int EE[8], EO[8];
    int EEE[4], EEO[4];
    int EEEE[2], EEEO[2];
    int add = 1 << (shift - 1);
    int max_tr_val = (1 << bit_depth) - 1;
    int min_tr_val = -(1 << bit_depth);

    for (j = 0; j < line; j++) {
        for (k = 0; k < 16; k++) {
            O[k] = com_tbl_tm32[DCT2][1][k] * src[1 * line + j] + \
                   com_tbl_tm32[DCT2][3][k] * src[3 * line + j] + \
                   com_tbl_tm32[DCT2][5][k] * src[5 * line + j] + \
                   com_tbl_tm32[DCT2][7][k] * src[7 * line + j] + \
                   com_tbl_tm32[DCT2][9][k] * src[9 * line + j] + \
                   com_tbl_tm32[DCT2][11][k] * src[11 * line + j] + \
                   com_tbl_tm32[DCT2][13][k] * src[13 * line + j] + \
                   com_tbl_tm32[DCT2][15][k] * src[15 * line + j] + \
                   com_tbl_tm32[DCT2][17][k] * src[17 * line + j] + \
                   com_tbl_tm32[DCT2][19][k] * src[19 * line + j] + \
                   com_tbl_tm32[DCT2][21][k] * src[21 * line + j] + \
                   com_tbl_tm32[DCT2][23][k] * src[23 * line + j] + \
                   com_tbl_tm32[DCT2][25][k] * src[25 * line + j] + \
                   com_tbl_tm32[DCT2][27][k] * src[27 * line + j] + \
                   com_tbl_tm32[DCT2][29][k] * src[29 * line + j] + \
                   com_tbl_tm32[DCT2][31][k] * src[31 * line + j];
        }
        for (k = 0; k < 8; k++) {
            EO[k] = com_tbl_tm32[DCT2][2][k] * src[2 * line + j] + \
                    com_tbl_tm32[DCT2][6][k] * src[6 * line + j] + \
                    com_tbl_tm32[DCT2][10][k] * src[10 * line + j] + \
                    com_tbl_tm32[DCT2][14][k] * src[14 * line + j] + \
                    com_tbl_tm32[DCT2][18][k] * src[18 * line + j] + \
                    com_tbl_tm32[DCT2][22][k] * src[22 * line + j] + \
                    com_tbl_tm32[DCT2][26][k] * src[26 * line + j] + \
                    com_tbl_tm32[DCT2][30][k] * src[30 * line + j];
        }
        for (k = 0; k < 4; k++) {
            EEO[k] = com_tbl_tm32[DCT2][4][k] * src[4 * line + j] + \
                     com_tbl_tm32[DCT2][12][k] * src[12 * line + j] + \
                     com_tbl_tm32[DCT2][20][k] * src[20 * line + j] + \
                     com_tbl_tm32[DCT2][28][k] * src[28 * line + j];
        }
        EEEO[0] = com_tbl_tm32[DCT2][8][0] * src[8 * line + j] + com_tbl_tm32[DCT2][24][0] * src[24 * line + j];
        EEEO[1] = com_tbl_tm32[DCT2][8][1] * src[8 * line + j] + com_tbl_tm32[DCT2][24][1] * src[24 * line + j];
        EEEE[0] = com_tbl_tm32[DCT2][0][0] * src[0 * line + j] + com_tbl_tm32[DCT2][16][0] * src[16 * line + j];
        EEEE[1] = com_tbl_tm32[DCT2][0][1] * src[0 * line + j] + com_tbl_tm32[DCT2][16][1] * src[16 * line + j];
        EEE[0] = EEEE[0] + EEEO[0];
        EEE[3] = EEEE[0] - EEEO[0];
        EEE[1] = EEEE[1] + EEEO[1];
        EEE[2] = EEEE[1] - EEEO[1];
        for (k = 0; k < 4; k++) {
            EE[k] = EEE[k] + EEO[k];
            EE[k + 4] = EEE[3 - k] - EEO[3 - k];
        }
        for (k = 0; k < 8; k++) {
            E[k] = EE[k] + EO[k];
            E[k + 8] = EE[7 - k] - EO[7 - k];
        }
        for (k = 0; k < 16; k++) {
            dst[j * 32 + k] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (E[k] + O[k] + add) >> shift);
            dst[j * 32 + k + 16] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (E[15 - k] - O[15 - k] + add) >> shift);
        }
    }
}

static avs3_always_inline void dct_butterfly_h64(s16 *src, int i_src, s16 *dst, int line, int shift, int bit_depth)
{
    const int tx_size = 64;
    const s8 *tm = com_tbl_tm64[DCT2][0];
    int j, k;
    int E[32], O[32];
    int EE[16], EO[16];
    int EEE[8], EEO[8];
    int EEEE[4], EEEO[4];
    int add = 1 << (shift - 1);
    int max_tr_val = (1 << bit_depth) - 1;
    int min_tr_val = -(1 << bit_depth);

    for (j = 0; j < line; j++) {
        for (k = 0; k < 32; k++) {
            O[k] = tm[1 * 64 + k] * src[i_src] + tm[3 * 64 + k] * src[3 * i_src] + tm[5 * 64 + k] * src[5 * i_src] + tm[7 * 64 + k] * src[7 * i_src] +
                   tm[9 * 64 + k] * src[9 * i_src] + tm[11 * 64 + k] * src[11 * i_src] + tm[13 * 64 + k] * src[13 * i_src] + tm[15 * 64 + k] * src[15 * i_src] +
                   tm[17 * 64 + k] * src[17 * i_src] + tm[19 * 64 + k] * src[19 * i_src] + tm[21 * 64 + k] * src[21 * i_src] + tm[23 * 64 + k] * src[23 * i_src] +
                   tm[25 * 64 + k] * src[25 * i_src] + tm[27 * 64 + k] * src[27 * i_src] + tm[29 * 64 + k] * src[29 * i_src] + tm[31 * 64 + k] * src[31 * i_src];
        }
        for (k = 0; k < 16; k++) {
            EO[k] = tm[2 * 64 + k] * src[2 * i_src] + tm[6 * 64 + k] * src[6 * i_src] + tm[10 * 64 + k] * src[10 * i_src] + tm[14 * 64 + k] * src[14 * i_src] +
                    tm[18 * 64 + k] * src[18 * i_src] + tm[22 * 64 + k] * src[22 * i_src] + tm[26 * 64 + k] * src[26 * i_src] + tm[30 * 64 + k] * src[30 * i_src];
        }
        for (k = 0; k < 8; k++) {
            EEO[k] = tm[4 * 64 + k] * src[4 * i_src] + tm[12 * 64 + k] * src[12 * i_src] + tm[20 * 64 + k] * src[20 * i_src] + tm[28 * 64 + k] * src[28 * i_src];
        }
        for (k = 0; k < 4; k++) {
            EEEO[k] = tm[8 * 64 + k] * src[8 * i_src] + tm[24 * 64 + k] * src[24 * i_src];
        }

        EEEE[0] = tm[0 * 64 + 0] * src[0] + tm[16 * 64 + 0] * src[16 * i_src];
        EEEE[1] = tm[0 * 64 + 1] * src[0] + tm[16 * 64 + 1] * src[16 * i_src];
        EEEE[2] = tm[0 * 64 + 1] * src[0] + (-tm[16 * 64 + 1]) * src[16 * i_src];
        EEEE[3] = tm[0 * 64 + 0] * src[0] + (-tm[16 * 64 + 0]) * src[16 * i_src];

        for (k = 0; k < 4; k++) {
            EEE[k] = EEEE[k] + EEEO[k];
            EEE[k + 4] = EEEE[3 - k] - EEEO[3 - k];
        }
        for (k = 0; k < 8; k++) {
            EE[k] = EEE[k] + EEO[k];
            EE[k + 8] = EEE[7 - k] - EEO[7 - k];
        }
        for (k = 0; k < 16; k++) {
            E[k] = EE[k] + EO[k];
            E[k + 16] = EE[15 - k] - EO[15 - k];
        }
        for (k = 0; k < 32; k++) {
            dst[k] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (E[k] + O[k] + add) >> shift);
            dst[k + 32] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (E[31 - k] - O[31 - k] + add) >> shift);
        }
        src++;
        dst += tx_size;
    }
}

static void itrans_dct2_h4_w4(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[4 * 4];
    dct_butterfly_h4(src, tmp, 4, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h4(tmp, dst, 4, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h4_w8(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[4 * 8];
    dct_butterfly_h4(src, tmp, 8, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h8(tmp, dst, 4, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h4_w16(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[4 * 16];
    dct_butterfly_h4(src, tmp, 16, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h16(tmp, dst, 4, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h4_w32(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[4 * 32];
    dct_butterfly_h4(src, tmp, 32, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h32(tmp, dst, 4, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h8_w4(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[8 * 4];
    dct_butterfly_h8(src, tmp, 4, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h4(tmp, dst, 8, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h8_w8(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[8 * 8];
    dct_butterfly_h8(src, tmp, 8, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h8(tmp, dst, 8, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h8_w16(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[8 * 16];
    dct_butterfly_h8(src, tmp, 16, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h16(tmp, dst, 8, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h8_w32(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[8 * 32];
    dct_butterfly_h8(src, tmp, 32, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h32(tmp, dst, 8, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h8_w64(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[8 * 64];
    dct_butterfly_h8(src, tmp, 64, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h64(tmp, 8, dst, 8, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h16_w4(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[16 * 4];
    dct_butterfly_h16(src, tmp, 4, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h4(tmp, dst, 16, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h16_w8(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[16 * 8];
    dct_butterfly_h16(src, tmp, 8, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h8(tmp, dst, 16, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h16_w16(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[16 * 16];
    dct_butterfly_h16(src, tmp, 16, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h16(tmp, dst, 16, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h16_w32(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[16 * 32];
    dct_butterfly_h16(src, tmp, 32, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h32(tmp, dst, 16, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h16_w64(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[16 * 64];
    dct_butterfly_h16(src, tmp, 64, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h64(tmp, 16, dst, 16, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h32_w4(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[32 * 4];
    dct_butterfly_h32(src, tmp, 4, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h4(tmp, dst, 32, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h32_w8(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[32 * 8];
    dct_butterfly_h32(src, tmp, 8, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h8(tmp, dst, 32, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h32_w16(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[32 * 16];
    dct_butterfly_h32(src, tmp, 16, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h16(tmp, dst, 32, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h32_w32(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[32 * 32];
    dct_butterfly_h32(src, tmp, 32, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h32(tmp, dst, 32, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h32_w64(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[32 * 64];
    dct_butterfly_h32(src, tmp, 64, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h64(tmp, 32, dst, 32, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h64_w8(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[64 * 8];
    dct_butterfly_h64(src, 8, tmp, 8, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h8(tmp, dst, 64, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h64_w16(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[64 * 16];
    dct_butterfly_h64(src, 16, tmp, 16, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h16(tmp, dst, 64, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h64_w32(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[64 * 32];
    dct_butterfly_h64(src, 32, tmp, 32, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h32(tmp, dst, 64, 20 - bit_depth, bit_depth);
}

static void itrans_dct2_h64_w64(s16 *src, s16 *dst, int bit_depth)
{
    s16 tmp[64 * 64];
    dct_butterfly_h64(src, 64, tmp, 32, 5, MAX_TX_DYNAMIC_RANGE);
    dct_butterfly_h64(tmp, 64, dst, 64, 20 - bit_depth, bit_depth);
}


/******************   DCT-8   ******************************************/

static void itx_dct8_pb4(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val)  // input tmp, output block
{
    int i;
    int rnd_factor = 1 << (shift - 1);

    s8 *iT = com_tbl_tm4[DCT8][0];

    int c[4];
    const int  reducedLine = line;
    for (i = 0; i < reducedLine; i++) {
        // Intermediate Variables
        c[0] = coeff[0 * line] + coeff[3 * line];
        c[1] = coeff[2 * line] + coeff[0 * line];
        c[2] = coeff[3 * line] - coeff[2 * line];
        c[3] = iT[1] * coeff[1 * line];

        block[0] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (iT[3] * c[0] + iT[2] * c[1] + c[3] + rnd_factor) >> shift);
        block[1] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (iT[1] * (coeff[0 * line] - coeff[2 * line] - coeff[3 * line]) + rnd_factor) >> shift);
        block[2] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (iT[3] * c[2] + iT[2] * c[0] - c[3] + rnd_factor) >> shift);
        block[3] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (iT[3] * c[1] - iT[2] * c[2] - c[3] + rnd_factor) >> shift);

        block += 4;
        coeff++;
    }
}

static void itx_dct8_pb8(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val)  // input block, output coeff
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);
    const int uiTrSize = 8;
    s8 *iT = com_tbl_tm8[DCT8][0];
    const int  reducedLine = line;
    const int  cutoff = 8;

    for (i = 0; i < reducedLine; i++) {
        for (j = 0; j < uiTrSize; j++) {
            iSum = 0;
            for (k = 0; k < cutoff; k++) {
                iSum += coeff[k*line] * iT[k*uiTrSize + j];
            }
            block[j] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (int)(iSum + rnd_factor) >> shift);
        }
        block += uiTrSize;
        coeff++;
    }
}

static void itx_dct8_pb16(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val)  // input block, output coeff
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);
    const int uiTrSize = 16;
    s8 *iT = com_tbl_tm16[DCT8][0];
    const int  reducedLine = line;
    const int  cutoff = uiTrSize;

    for (i = 0; i < reducedLine; i++) {
        for (j = 0; j < uiTrSize; j++) {
            iSum = 0;
            for (k = 0; k < cutoff; k++) {
                iSum += coeff[k*line] * iT[k*uiTrSize + j];
            }
            block[j] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (int)(iSum + rnd_factor) >> shift);
        }
        block += uiTrSize;
        coeff++;
    }
}

static void itx_dct8_pb32(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val)  // input block, output coeff
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);
    const int uiTrSize = 32;
    s8 *iT = com_tbl_tm32[DCT8][0];
    const int  reducedLine = line;
    const int  cutoff = uiTrSize;

    for (i = 0; i < reducedLine; i++) {
        for (j = 0; j < uiTrSize; j++) {
            iSum = 0;
            for (k = 0; k < cutoff; k++) {
                iSum += coeff[k*line] * iT[k*uiTrSize + j];
            }
            block[j] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (int)(iSum + rnd_factor) >> shift);
        }
        block += uiTrSize;
        coeff++;
    }
}

static void itx_dct8_pb64(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val)  // input block, output coeff
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);
    const int uiTrSize = 64;
    s8 *iT = com_tbl_tm64[DCT8][0];
    const int  reducedLine = line;
    const int  cutoff = uiTrSize;

    for (i = 0; i < reducedLine; i++) {
        for (j = 0; j < uiTrSize; j++) {
            iSum = 0;
            for (k = 0; k < cutoff; k++) {
                iSum += coeff[k*line] * iT[k*uiTrSize + j];
            }
            block[j] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (int)(iSum + rnd_factor) >> shift);
        }
        block += uiTrSize;
        coeff++;
    }
}

/******************   DST-7   ******************************************/
static void itx_dst7_pb4(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val)  // input tmp, output block
{
    int i, c[4];
    int rnd_factor = 1 << (shift - 1);
    s8 *iT = com_tbl_tm4[DST7][0];
    const int  reducedLine = line;

    for (i = 0; i < reducedLine; i++) {
        // Intermediate Variables

        c[0] = coeff[0 * line] + coeff[2 * line];
        c[1] = coeff[2 * line] + coeff[3 * line];
        c[2] = coeff[0 * line] - coeff[3 * line];
        c[3] = iT[2] * coeff[1 * line];

        block[0] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (iT[0] * c[0] + iT[1] * c[1] + c[3] + rnd_factor) >> shift);
        block[1] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (iT[1] * c[2] - iT[0] * c[1] + c[3] + rnd_factor) >> shift);
        block[2] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (iT[2] * (coeff[0 * line] - coeff[2 * line] + coeff[3 * line]) + rnd_factor) >> shift);
        block[3] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (iT[1] * c[0] + iT[0] * c[2] - c[3] + rnd_factor) >> shift);

        block += 4;
        coeff++;
    }
}

static void itx_dst7_pb8(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val)  // input block, output coeff
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);
    const int uiTrSize = 8;
    s8 *iT = com_tbl_tm8[DST7][0];
    const int  reducedLine = line;
    const int  cutoff = uiTrSize;

    for (i = 0; i < reducedLine; i++) {
        for (j = 0; j < uiTrSize; j++) {
            iSum = 0;
            for (k = 0; k < cutoff; k++) {
                iSum += coeff[k*line] * iT[k*uiTrSize + j];
            }
            block[j] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (int)(iSum + rnd_factor) >> shift);
        }
        block += uiTrSize;
        coeff++;
    }
}

static void itx_dst7_pb16(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val)  // input block, output coeff
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);
    const int uiTrSize = 16;
    s8 *iT = com_tbl_tm16[DST7][0];
    const int  reducedLine = line;
    const int  cutoff = uiTrSize;

    for (i = 0; i < reducedLine; i++) {
        for (j = 0; j < uiTrSize; j++) {
            iSum = 0;
            for (k = 0; k < cutoff; k++) {
                iSum += coeff[k*line] * iT[k*uiTrSize + j];
            }
            block[j] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (int)(iSum + rnd_factor) >> shift);
        }
        block += uiTrSize;
        coeff++;
    }
}

static void itx_dst7_pb32(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val)  // input block, output coeff
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);
    const int uiTrSize = 32;
    s8 *iT = com_tbl_tm32[DST7][0];
    const int  reducedLine = line;
    const int  cutoff = uiTrSize;

    for (i = 0; i < reducedLine; i++) {
        for (j = 0; j < uiTrSize; j++) {
            iSum = 0;
            for (k = 0; k < cutoff; k++) {
                iSum += coeff[k*line] * iT[k*uiTrSize + j];
            }
            block[j] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (int)(iSum + rnd_factor) >> shift);
        }
        block += uiTrSize;
        coeff++;
    }
}

static void itx_dst7_pb64(s16 *coeff, s16 *block, int shift, int line, int max_tr_val, int min_tr_val)  // input block, output coeff
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);
    const int uiTrSize = 64;
    s8 *iT = com_tbl_tm64[DST7][0];
    const int  reducedLine = line;
    const int  cutoff = uiTrSize;
    for (i = 0; i < reducedLine; i++) {
        for (j = 0; j < uiTrSize; j++) {
            iSum = 0;
            for (k = 0; k < cutoff; k++) {
                iSum += coeff[k*line] * iT[k*uiTrSize + j];
            }
            block[j] = (s16)COM_CLIP3(min_tr_val, max_tr_val, (int)(iSum + rnd_factor) >> shift);
        }
        block += uiTrSize;
        coeff++;
    }
}

typedef void(*COM_ITX)(s16 *coef, s16 *t, int shift, int line, int max_tr_val, int min_tr_val);

static void xCTr_4_1d_Inv_Vert(s16 *src, int i_src, s16 *dst, int i_dst, int shift)
{
    int i, j, k, sum;
    int rnd_factor = shift == 0 ? 0 : 1 << (shift - 1);
    int tmpSrc[4][4];

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmpSrc[i][j] = src[i * i_src + j];
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            sum = rnd_factor;
            for (k = 0; k < 4; k++) {
                sum += com_tbl_c4_trans[k][i] * tmpSrc[k][j];
            }
            dst[i *i_dst + j] = (s16)COM_CLIP3(-32768, 32767, sum >> shift);
        }
    }
}

static void xCTr_4_1d_Inv_Hor(s16 *src, int i_src, s16 *dst, int i_dst, int shift, int bit_depth)
{
    int i, j, k, sum;
    int rnd_factor = shift == 0 ? 0 : 1 << (shift - 1);
    int tmpSrc[4][4];
    int min_pixel = -(1 << bit_depth);
    int max_pixel = (1 << bit_depth) - 1;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmpSrc[i][j] = src[i * i_src + j];
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            sum = rnd_factor;
            for (k = 0; k < 4; k++) {
                sum += com_tbl_c4_trans[k][i] * tmpSrc[j][k];
            }
            dst[j *i_dst + i] = (s16)COM_CLIP3(min_pixel, max_pixel, sum >> shift);
        }
    }
}

static void xTr2nd_8_1d_Inv_Vert(s16 *src, int i_src)
{
    int i, j, k, sum;
    int tmpSrc[4][4];
    int rnd_factor = 1 << (7 - 1);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmpSrc[i][j] = src[i * i_src + j];
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            sum = rnd_factor;
            for (k = 0; k < 4; k++) {
                sum += com_tbl_c8_trans[k][i] * tmpSrc[k][j];
            }
            src[i *i_src + j] = (s16)COM_CLIP3(-32768, 32767, sum >> 7);
        }
    }
}

static void xTr2nd_8_1d_Inv_Hor(s16 *src, int i_src)
{
    int i, j, k, sum;
    int tmpSrc[4][4];
    int rnd_factor = 1 << (7 - 1);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmpSrc[i][j] = src[i * i_src + j];
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            sum = rnd_factor;
            for (k = 0; k < 4; k++) {
                sum += com_tbl_c8_trans[k][i] * tmpSrc[j][k];
            }
            src[j *i_src + i] = (s16)COM_CLIP3(-32768, 32767, sum >> 7);
        }
    }
}

static void com_inv_trans(com_mode_t *mode, int plane, int blk_idx, s16 *coef_dq, s16 *resi, int cu_width_log2, int cu_height_log2, int bit_depth, int secT_Ver_Hor, int use_alt4x4Trans)
{
    ALIGNED_32(s16 coef_temp[MAX_TR_DIM]);

    int shift1 = get_inv_trans_shift(cu_width_log2,  0, bit_depth);
    int shift2 = get_inv_trans_shift(cu_height_log2, 1, bit_depth);

    if (use_alt4x4Trans && cu_width_log2 == 2 && cu_height_log2 == 2) {
        ALIGNED_32(s16 coef_temp2[16]);
        xCTr_4_1d_Inv_Vert(coef_dq, 4, coef_temp2, 4, shift1);
        xCTr_4_1d_Inv_Hor(coef_temp2, 4, resi, 4, shift2 + 2, bit_depth);
    } else {
        int nTrIdxHor = DCT2, nTrIdxVer = DCT2;
        int stride_tu = (1 << cu_width_log2);
        if (secT_Ver_Hor & 1) {
            xTr2nd_8_1d_Inv_Hor(coef_dq, stride_tu);
        }
        if (secT_Ver_Hor >> 1) {
            xTr2nd_8_1d_Inv_Vert(coef_dq, stride_tu);
        }
        if (plane == Y_C && mode->tb_part == SIZE_NxN) {
            int max_tr_val = (1 << MAX_TX_DYNAMIC_RANGE) - 1;
            int min_tr_val = -(1 << MAX_TX_DYNAMIC_RANGE);
            uavs3e_funs_handle.itrans_dct8_dst7[blk_idx >> 1][cu_height_log2 - 1](coef_dq, coef_temp, shift1, 1 << cu_width_log2, max_tr_val, min_tr_val);

            max_tr_val = (1 << bit_depth) - 1;
            min_tr_val = -(1 << bit_depth);
            uavs3e_funs_handle.itrans_dct8_dst7[blk_idx & 1][cu_width_log2 - 1](coef_temp, resi, shift2, 1 << cu_height_log2, max_tr_val, min_tr_val);
        } else {
            uavs3e_funs_handle.itrans_dct2[cu_height_log2 - 1][cu_width_log2 - 1](coef_dq, resi, bit_depth);
        }
    }
}

static tab_u32 tbl_dq_scale[80] = { // [64 + 16]
    32768, 36061, 38968, 42495, 46341, 50535, 55437, 60424,
    32932, 35734, 38968, 42495, 46177, 50535, 55109, 59933,
    65535, 35734, 38968, 42577, 46341, 50617, 55027, 60097,
    32809, 35734, 38968, 42454, 46382, 50576, 55109, 60056,
    65535, 35734, 38968, 42495, 46320, 50515, 55109, 60076,
    65535, 35744, 38968, 42495, 46341, 50535, 55099, 60087,
    65535, 35734, 38973, 42500, 46341, 50535, 55109, 60097,
    32771, 35734, 38965, 42497, 46341, 50535, 55109, 60099,
    32768, 36061, 38968, 42495, 46341, 50535, 55437, 60424,
    32932, 35734, 38968, 42495, 46177, 50535, 55109, 59933
};

static tab_u32 tbl_dq_shift[80] = { // [64 + 16]
    14, 14, 14, 14, 14, 14, 14, 14,  //15, 15, 15, 15, 15, 15, 15, 15,
    13, 13, 13, 13, 13, 13, 13, 13,  //14, 14, 14, 14, 14, 14, 14, 14,
    13, 12, 12, 12, 12, 12, 12, 12,  //14, 13, 13, 13, 13, 13, 13, 13,
    11, 11, 11, 11, 11, 11, 11, 11,  //12, 12, 12, 12, 12, 12, 12, 12,
    11, 10, 10, 10, 10, 10, 10, 10,  //12, 11, 11, 11, 11, 11, 11, 11,
    10,  9,  9,  9,  9,  9,  9,  9,  //11, 10, 10, 10, 10, 10, 10, 10,
    9,  8,  8,  8,  8,  8,  8,  8,  //10,  9,  9,  9,  9,  9,  9,  9,
    7,  7,  7,  7,  7,  7,  7,  7,  // 8,  8,  8,  8,  8,  8,  8,  8,
    6,  6,  6,  6,  6,  6,  6,  6,  // 7,  7,  7,  7,  7,  7,  7,  7,
    5,  5,  5,  5,  5,  5,  5,  5,  // 6,  6,  6,  6,  6,  6,  6,  6
};

static void com_dquant(s16 *coef, s16 *coef_out, u8 *wq_matrix[2], int log2_w, int log2_h, int scale, int shift, int bit_depth)
{
    int w = 1 << log2_w;
    int h = 1 << log2_h;
    int offset = (shift == 0) ? 0 : (1 << (shift - 1));
    int size = COM_MIN(h, 32) * w;

    if ((log2_w + log2_h) & 1) {
        for (int i = 0; i < size; i++) {
            int lev = (coef[i] * scale + offset) >> shift;
            lev = COM_CLIP(lev, -32768, 32767);
            coef_out[i] = (lev * 181 + 128) >> 8;
        }
    } else {
        for (int i = 0; i < size; i++) {
            int lev = (coef[i] * scale + offset) >> shift;
            coef_out[i] = COM_CLIP(lev, -32768, 32767);
        }
    }
}

static void com_dquant_wq(s16 *coef, s16 *coef_out, u8 *wq_matrix[2], int log2_w, int log2_h, int scale, int shift, int bit_depth)
{
    int i, j;
    int w = 1 << log2_w;
    int h = 1 << log2_h;
    int wq_width;
    int idx_shift;
    int idx_step;
    int refix = (log2_w + log2_h) & 1;
    int offset = (shift == 0) ? 0 : (1 << (shift - 1));
    u8 *wq;

    if (log2_w == 2 && log2_h == 2) {
        wq = wq_matrix[0];
        idx_shift = 0;
        idx_step = 1;
        wq_width = 4;
    }
    else {
        wq = wq_matrix[1];
        idx_shift = COM_MAX(log2_w, log2_h) - 3;
        idx_step = 1 << idx_shift;
        wq_width = 8;
    }

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            int weight = ((i | j) & 0xE0) ? 0 : wq[j >> idx_shift];

            int lev = (((((coef[j] * weight) >> 2) * (s64)scale) >> 4) + offset) >> shift;
            lev = COM_CLIP(lev, -32768, 32767);
            if (refix) {
                lev = (lev * 181 + 128) >> 8;
            }
            coef_out[j] = (s16)lev;
        }
        coef_out += w;
        coef += w;

        if ((i + 1) % idx_step == 0) {
            wq += wq_width;
        }
    }
}

void com_invqt(com_mode_t *mode, int plane, int blk_idx, s16 *coef, s16 *resi, u8 *wq[2], int log2_w, int log2_h, int qp, int bit_depth, int secT_Ver_Hor, int use_alt4x4Trans)
{
    ALIGNED_32(s16 coef_dq[MAX_CU_DIM]);
    int log2_size = (log2_w + log2_h) >> 1;
    int scale = tbl_dq_scale[qp];
    int shift = tbl_dq_shift[qp] - COM_GET_TRANS_SHIFT(bit_depth, log2_size) + 1; // +1 is used to compensate for the mismatching of shifts in quantization and inverse quantization
    uavs3e_funs_handle.dquant[!wq[0]](coef, coef_dq, wq, log2_w, log2_h, scale, shift, bit_depth);
    com_inv_trans(mode, plane, blk_idx, coef_dq, resi, log2_w, log2_h, bit_depth, secT_Ver_Hor, use_alt4x4Trans);
}

void com_invqt_inter_plane(com_mode_t *mode, int plane, s16 coef[MAX_CU_DIM], s16 resi[MAX_CU_DIM], u8 *wq[2], int cu_width_log2, int cu_height_log2, int qp, int bit_depth)
{
    int log2_tb_w, log2_tb_h, tb_size;
    int part_num = get_part_num(plane == Y_C ? mode->tb_part : SIZE_2Nx2N);
    get_tb_width_height_log2(cu_width_log2, cu_height_log2, plane == Y_C ? mode->tb_part : SIZE_2Nx2N, &log2_tb_w, &log2_tb_h);
    tb_size = 1 << (log2_tb_w + log2_tb_h);

    for (int i = 0; i < part_num; i++) {
        if (mode->num_nz[i][plane]) {
            com_invqt(mode, plane, i, coef + i * tb_size, resi + i * tb_size, wq, log2_tb_w, log2_tb_h, qp, bit_depth, 0, 0);
        }
    }
}

void com_invqt_inter_yuv(com_mode_t *mode, int tree_status, s16 coef[N_C][MAX_CU_DIM], s16 resi[N_C][MAX_CU_DIM], u8 *wq[2], int cu_width_log2, int cu_height_log2, int qp_y, int qp_u, int qp_v, int bit_depth)
{
    int start_comp = (tree_status == TREE_L || tree_status == TREE_LC) ? Y_C : U_C;
    int num_comp = tree_status == TREE_LC ? 3 : (tree_status == TREE_L ? 1 : 2);

    for (int i = start_comp; i < start_comp + num_comp; i++) {
        int plane_width_log2 = cu_width_log2 - (i != Y_C);
        int plane_height_log2 = cu_height_log2 - (i != Y_C);
        int qp = (i == Y_C ? qp_y : (i == U_C ? qp_u : qp_v));
        com_invqt_inter_plane(mode, i, coef[i], resi[i], wq, plane_width_log2, plane_height_log2, (u8)qp, bit_depth);
    }
}

static int quant_check(s16 *coef, int num, int shift, int threshold)
{
    num >>= 2;
    while (num--) {
        int lev10 = coef[0] << shift;
        int lev11 = coef[1] << shift;
        int lev12 = coef[2] << shift;
        int lev13 = coef[3] << shift;

        if (lev10 > threshold || lev10 < -threshold ||
            lev11 > threshold || lev11 < -threshold ||
            lev12 > threshold || lev12 < -threshold ||
            lev13 > threshold || lev13 < -threshold) {
            return 0; 
        }
        coef += 4;
    }
    return 1;
}

static int quant_rdoq(s16 *coef, int num, int q_value, int q_bits, s32 err_scale, int precision_bits, u32* abs_coef, s16* abs_level, s64 *uncoded_err)
{
    s32 q_bits_add = 1 << (q_bits - 1);
    int last_nz = -1;

    for (int i = 0; i < num; i++) {
        u32 level_double = COM_ABS16(coef[i]) * q_value;
        u32 max_abs_level = (u32)((level_double + q_bits_add) >> q_bits);

        if (max_abs_level) {
            s64 err = (level_double * (s64)err_scale) >> precision_bits;
            abs_coef[i] = level_double;
            uncoded_err[i] = err * err;
            last_nz = i;
        }
        abs_level[i] = (u16)max_abs_level;
    }

    return last_nz;
}

void uavs3e_funs_init_itrans_c()
{
    uavs3e_funs_handle.itrans_dct2[1][1] = itrans_dct2_h4_w4;
    uavs3e_funs_handle.itrans_dct2[1][2] = itrans_dct2_h4_w8;
    uavs3e_funs_handle.itrans_dct2[1][3] = itrans_dct2_h4_w16;
    uavs3e_funs_handle.itrans_dct2[1][4] = itrans_dct2_h4_w32;

    uavs3e_funs_handle.itrans_dct2[2][1] = itrans_dct2_h8_w4;
    uavs3e_funs_handle.itrans_dct2[2][2] = itrans_dct2_h8_w8;
    uavs3e_funs_handle.itrans_dct2[2][3] = itrans_dct2_h8_w16;
    uavs3e_funs_handle.itrans_dct2[2][4] = itrans_dct2_h8_w32;
    uavs3e_funs_handle.itrans_dct2[2][5] = itrans_dct2_h8_w64;

    uavs3e_funs_handle.itrans_dct2[3][1] = itrans_dct2_h16_w4;
    uavs3e_funs_handle.itrans_dct2[3][2] = itrans_dct2_h16_w8;
    uavs3e_funs_handle.itrans_dct2[3][3] = itrans_dct2_h16_w16;
    uavs3e_funs_handle.itrans_dct2[3][4] = itrans_dct2_h16_w32;
    uavs3e_funs_handle.itrans_dct2[3][5] = itrans_dct2_h16_w64;

    uavs3e_funs_handle.itrans_dct2[4][1] = itrans_dct2_h32_w4;
    uavs3e_funs_handle.itrans_dct2[4][2] = itrans_dct2_h32_w8;
    uavs3e_funs_handle.itrans_dct2[4][3] = itrans_dct2_h32_w16;
    uavs3e_funs_handle.itrans_dct2[4][4] = itrans_dct2_h32_w32;
    uavs3e_funs_handle.itrans_dct2[4][5] = itrans_dct2_h32_w64;

    uavs3e_funs_handle.itrans_dct2[5][2] = itrans_dct2_h64_w8;
    uavs3e_funs_handle.itrans_dct2[5][3] = itrans_dct2_h64_w16;
    uavs3e_funs_handle.itrans_dct2[5][4] = itrans_dct2_h64_w32;
    uavs3e_funs_handle.itrans_dct2[5][5] = itrans_dct2_h64_w64;

    uavs3e_funs_handle.itrans_dct8_dst7[0][1] = itx_dct8_pb4;
    uavs3e_funs_handle.itrans_dct8_dst7[0][2] = itx_dct8_pb8;
    uavs3e_funs_handle.itrans_dct8_dst7[0][3] = itx_dct8_pb16;
    uavs3e_funs_handle.itrans_dct8_dst7[0][4] = itx_dct8_pb32;
    uavs3e_funs_handle.itrans_dct8_dst7[0][5] = itx_dct8_pb64;

    uavs3e_funs_handle.itrans_dct8_dst7[1][1] = itx_dst7_pb4;
    uavs3e_funs_handle.itrans_dct8_dst7[1][2] = itx_dst7_pb8;
    uavs3e_funs_handle.itrans_dct8_dst7[1][3] = itx_dst7_pb16;
    uavs3e_funs_handle.itrans_dct8_dst7[1][4] = itx_dst7_pb32;
    uavs3e_funs_handle.itrans_dct8_dst7[1][5] = itx_dst7_pb64;

    uavs3e_funs_handle.dquant[0] = com_dquant_wq;
    uavs3e_funs_handle.dquant[1] = com_dquant;

    uavs3e_funs_handle.quant_check = quant_check;
    uavs3e_funs_handle.quant_rdoq  = quant_rdoq;

}
