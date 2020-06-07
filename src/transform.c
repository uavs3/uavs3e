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

#include "define.h"
#include <math.h>

/******************   DCT-2   ******************************************/

static void tx_dct2_pb2(s16 *src, s16 *dst, int line, int limit_line, int shift)
{
    int j;
    int E, O;
    int add = shift == 0 ? 0 : 1 << (shift - 1);
    for (j = 0; j < line; j++) {
        /* E and O */
        E = src[j*2+0] + src[j*2+1];
        O = src[j*2+0] - src[j*2+1];
        dst[0*line+j] = (s16)((com_tbl_tm2[DCT2][0][0] * E + add) >> shift);
        dst[1*line+j] = (s16)((com_tbl_tm2[DCT2][1][0] * O + add) >> shift);
    }
}

static void tx_dct2_pb4(s16 *src, s16 *dst, int line, int limit_line, int shift)
{
    int j;
    int E[2], O[2];
    int add = 1 << (shift - 1);
    for (j = 0; j < line; j++) {
        /* E and O */
        E[0] = src[j*4+0] + src[j*4+3];
        O[0] = src[j*4+0] - src[j*4+3];
        E[1] = src[j*4+1] + src[j*4+2];
        O[1] = src[j*4+1] - src[j*4+2];
        dst[0*line+j] = (s16)((com_tbl_tm4[DCT2][0][0] * E[0] + com_tbl_tm4[DCT2][0][1] * E[1] + add) >> shift);
        dst[2*line+j] = (s16)((com_tbl_tm4[DCT2][2][0] * E[0] + com_tbl_tm4[DCT2][2][1] * E[1] + add) >> shift);
        dst[1*line+j] = (s16)((com_tbl_tm4[DCT2][1][0] * O[0] + com_tbl_tm4[DCT2][1][1] * O[1] + add) >> shift);
        dst[3*line+j] = (s16)((com_tbl_tm4[DCT2][3][0] * O[0] + com_tbl_tm4[DCT2][3][1] * O[1] + add) >> shift);
    }
}

static void tx_dct2_pb8(s16 *src, s16 *dst, int line, int limit_line, int shift)
{
    int j, k;
    int E[4], O[4];
    int EE[2], EO[2];
    int add = 1 << (shift - 1);
    for (j = 0; j < limit_line; j++) {
        /* E and O*/
        for (k = 0; k < 4; k++) {
            E[k] = src[j*8+k] + src[j*8+7-k];
            O[k] = src[j*8+k] - src[j*8+7-k];
        }
        /* EE and EO */
        EE[0] = E[0] + E[3];
        EO[0] = E[0] - E[3];
        EE[1] = E[1] + E[2];
        EO[1] = E[1] - E[2];
        dst[0*line+j] = (s16)((com_tbl_tm8[DCT2][0][0] * EE[0] + com_tbl_tm8[DCT2][0][1] * EE[1] + add) >> shift);
        dst[4*line+j] = (s16)((com_tbl_tm8[DCT2][4][0] * EE[0] + com_tbl_tm8[DCT2][4][1] * EE[1] + add) >> shift);
        dst[2*line+j] = (s16)((com_tbl_tm8[DCT2][2][0] * EO[0] + com_tbl_tm8[DCT2][2][1] * EO[1] + add) >> shift);
        dst[6*line+j] = (s16)((com_tbl_tm8[DCT2][6][0] * EO[0] + com_tbl_tm8[DCT2][6][1] * EO[1] + add) >> shift);
        dst[1*line+j] = (s16)((com_tbl_tm8[DCT2][1][0] * O[0] + com_tbl_tm8[DCT2][1][1] * O[1] + com_tbl_tm8[DCT2][1][2] * O[2] + com_tbl_tm8[DCT2][1][3] * O[3] + add) >> shift);
        dst[3*line+j] = (s16)((com_tbl_tm8[DCT2][3][0] * O[0] + com_tbl_tm8[DCT2][3][1] * O[1] + com_tbl_tm8[DCT2][3][2] * O[2] + com_tbl_tm8[DCT2][3][3] * O[3] + add) >> shift);
        dst[5*line+j] = (s16)((com_tbl_tm8[DCT2][5][0] * O[0] + com_tbl_tm8[DCT2][5][1] * O[1] + com_tbl_tm8[DCT2][5][2] * O[2] + com_tbl_tm8[DCT2][5][3] * O[3] + add) >> shift);
        dst[7*line+j] = (s16)((com_tbl_tm8[DCT2][7][0] * O[0] + com_tbl_tm8[DCT2][7][1] * O[1] + com_tbl_tm8[DCT2][7][2] * O[2] + com_tbl_tm8[DCT2][7][3] * O[3] + add) >> shift);
    }
}

static void tx_dct2_pb16(s16 *src, s16 *dst, int line, int limit_line, int shift)
{
    int j, k;
    int E[8], O[8];
    int EE[4], EO[4];
    int EEE[2], EEO[2];
    int add = 1 << (shift - 1);
    for (j = 0; j < limit_line; j++) {
        /* E and O*/
        for (k = 0; k < 8; k++) {
            E[k] = src[j*16+k] + src[j*16+15-k];
            O[k] = src[j*16+k] - src[j*16+15-k];
        }
        /* EE and EO */
        for (k = 0; k < 4; k++) {
            EE[k] = E[k] + E[7-k];
            EO[k] = E[k] - E[7-k];
        }
        /* EEE and EEO */
        EEE[0] = EE[0] + EE[3];
        EEO[0] = EE[0] - EE[3];
        EEE[1] = EE[1] + EE[2];
        EEO[1] = EE[1] - EE[2];
        dst[ 0*line+j] = (s16)((com_tbl_tm16[DCT2][ 0][0] * EEE[0] + com_tbl_tm16[DCT2][ 0][1] * EEE[1] + add) >> shift);
        dst[ 8*line+j] = (s16)((com_tbl_tm16[DCT2][ 8][0] * EEE[0] + com_tbl_tm16[DCT2][ 8][1] * EEE[1] + add) >> shift);
        dst[ 4*line+j] = (s16)((com_tbl_tm16[DCT2][ 4][0] * EEO[0] + com_tbl_tm16[DCT2][ 4][1] * EEO[1] + add) >> shift);
        dst[12*line+j] = (s16)((com_tbl_tm16[DCT2][12][0] * EEO[0] + com_tbl_tm16[DCT2][12][1] * EEO[1] + add) >> shift);
        for (k = 2; k < 16; k += 4) {
            dst[k *line + j] = (s16)((com_tbl_tm16[DCT2][k][0] * EO[0] + com_tbl_tm16[DCT2][k][1] * EO[1] + com_tbl_tm16[DCT2][k][2] * EO[2] + com_tbl_tm16[DCT2][k][3] * EO[3] + add) >> shift);
        }
        for (k = 1; k < 16; k += 2) {
            dst[k *line + j] = (s16)((com_tbl_tm16[DCT2][k][0] * O[0] + com_tbl_tm16[DCT2][k][1] * O[1] + com_tbl_tm16[DCT2][k][2] * O[2] + com_tbl_tm16[DCT2][k][3] * O[3] +
                                      com_tbl_tm16[DCT2][k][4] * O[4] + com_tbl_tm16[DCT2][k][5] * O[5] + com_tbl_tm16[DCT2][k][6] * O[6] + com_tbl_tm16[DCT2][k][7] * O[7] + add) >> shift);
        }
    }
}

static void tx_dct2_pb32(s16 *src, s16 *dst, int line, int limit_line, int shift)
{
    int j, k;
    int E[16], O[16];
    int EE[8], EO[8];
    int EEE[4], EEO[4];
    int EEEE[2], EEEO[2];
    int add = 1 << (shift - 1);
    for (j = 0; j < limit_line; j++) {
        /* E and O*/
        for (k = 0; k < 16; k++) {
            E[k] = src[j*32+k] + src[j*32+31-k];
            O[k] = src[j*32+k] - src[j*32+31-k];
        }
        /* EE and EO */
        for (k = 0; k < 8; k++) {
            EE[k] = E[k] + E[15-k];
            EO[k] = E[k] - E[15-k];
        }
        /* EEE and EEO */
        for (k = 0; k < 4; k++) {
            EEE[k] = EE[k] + EE[7-k];
            EEO[k] = EE[k] - EE[7-k];
        }
        /* EEEE and EEEO */
        EEEE[0] = EEE[0] + EEE[3];
        EEEO[0] = EEE[0] - EEE[3];
        EEEE[1] = EEE[1] + EEE[2];
        EEEO[1] = EEE[1] - EEE[2];
        dst[ 0*line+j] = (s16)((com_tbl_tm32[DCT2][ 0][0] * EEEE[0] + com_tbl_tm32[DCT2][ 0][1] * EEEE[1] + add) >> shift);
        dst[16*line+j] = (s16)((com_tbl_tm32[DCT2][16][0] * EEEE[0] + com_tbl_tm32[DCT2][16][1] * EEEE[1] + add) >> shift);
        dst[ 8*line+j] = (s16)((com_tbl_tm32[DCT2][ 8][0] * EEEO[0] + com_tbl_tm32[DCT2][ 8][1] * EEEO[1] + add) >> shift);
        dst[24*line+j] = (s16)((com_tbl_tm32[DCT2][24][0] * EEEO[0] + com_tbl_tm32[DCT2][24][1] * EEEO[1] + add) >> shift);
        for (k = 4; k < 32; k += 8) {
            dst[k *line + j] = (s16)((com_tbl_tm32[DCT2][k][0] * EEO[0] + com_tbl_tm32[DCT2][k][1] * EEO[1] + com_tbl_tm32[DCT2][k][2] * EEO[2] + com_tbl_tm32[DCT2][k][3] * EEO[3] + add) >> shift);
        }
        for (k = 2; k < 32; k += 4) {
            dst[k *line + j] = (s16)((com_tbl_tm32[DCT2][k][0] * EO[0] + com_tbl_tm32[DCT2][k][1] * EO[1] + com_tbl_tm32[DCT2][k][2] * EO[2] + com_tbl_tm32[DCT2][k][3] * EO[3] +
                                      com_tbl_tm32[DCT2][k][4] * EO[4] + com_tbl_tm32[DCT2][k][5] * EO[5] + com_tbl_tm32[DCT2][k][6] * EO[6] + com_tbl_tm32[DCT2][k][7] * EO[7] + add) >> shift);
        }
        for (k = 1; k < 32; k += 2) {
            dst[k *line + j] = (s16)((com_tbl_tm32[DCT2][k][0] * O[0] + com_tbl_tm32[DCT2][k][1] * O[1] + com_tbl_tm32[DCT2][k][2] * O[2] + com_tbl_tm32[DCT2][k][3] * O[3] +
                                      com_tbl_tm32[DCT2][k][4] * O[4] + com_tbl_tm32[DCT2][k][5] * O[5] + com_tbl_tm32[DCT2][k][6] * O[6] + com_tbl_tm32[DCT2][k][7] * O[7] +
                                      com_tbl_tm32[DCT2][k][8] * O[8] + com_tbl_tm32[DCT2][k][9] * O[9] + com_tbl_tm32[DCT2][k][10] * O[10] + com_tbl_tm32[DCT2][k][11] * O[11] +
                                      com_tbl_tm32[DCT2][k][12] * O[12] + com_tbl_tm32[DCT2][k][13] * O[13] + com_tbl_tm32[DCT2][k][14] * O[14] + com_tbl_tm32[DCT2][k][15] * O[15] + add) >> shift);
        }
    }
}

static void tx_dct2_pb64(s16 *src, s16 *dst, int line, int limit_line, int shift)
{
    const int tx_size = 64;
    const s8 *tm = com_tbl_tm64[DCT2][0];
    int j, k;
    int E[32], O[32];
    int EE[16], EO[16];
    int EEE[8], EEO[8];
    int EEEE[4], EEEO[4];
    int EEEEE[2], EEEEO[2];
    int add = 1 << (shift - 1);
    for (j = 0; j < limit_line; j++) {
        for (k = 0; k < 32; k++) {
            E[k] = src[k] + src[63-k];
            O[k] = src[k] - src[63-k];
        }
        for (k = 0; k < 16; k++) {
            EE[k] = E[k] + E[31-k];
            EO[k] = E[k] - E[31-k];
        }
        for (k = 0; k < 8; k++) {
            EEE[k] = EE[k] + EE[15-k];
            EEO[k] = EE[k] - EE[15-k];
        }
        for (k = 0; k < 4; k++) {
            EEEE[k] = EEE[k] + EEE[7-k];
            EEEO[k] = EEE[k] - EEE[7-k];
        }
        EEEEE[0] = EEEE[0] + EEEE[3];
        EEEEO[0] = EEEE[0] - EEEE[3];
        EEEEE[1] = EEEE[1] + EEEE[2];
        EEEEO[1] = EEEE[1] - EEEE[2];

        dst[ 0       ] = (s16)((tm[ 0*64+0] * EEEEE[0] + tm[ 0*64+1] * EEEEE[1] + add) >> shift);
        dst[ 16*line ] = (s16)((tm[16*64+0] * EEEEO[0] + tm[16*64+1] * EEEEO[1] + add) >> shift);

        for (k = 8; k < 32; k += 16) {
            dst[ k *line ] = (s16)((tm[k*64+0] * EEEO[0] + tm[k*64+1] * EEEO[1] + tm[k*64+2] * EEEO[2] + tm[k*64+3] * EEEO[3] + add) >> shift);
        }
        for (k = 4; k < 32; k += 8) {
            dst[ k *line ] = (s16)((tm[k*64+0] * EEO[0] + tm[k*64+1] * EEO[1] + tm[k*64+2] * EEO[2] + tm[k*64+3] * EEO[3] +
                                    tm[k*64+4] * EEO[4] + tm[k*64+5] * EEO[5] + tm[k*64+6] * EEO[6] + tm[k*64+7] * EEO[7] + add) >> shift);
        }
        for (k = 2; k < 32; k += 4) {
            dst[ k *line ] = (s16)((tm[k*64+ 0] * EO[ 0] + tm[k*64+ 1] * EO[ 1] + tm[k*64+ 2] * EO[ 2] + tm[k*64+ 3] * EO[ 3] +
                                    tm[k*64+ 4] * EO[ 4] + tm[k*64+ 5] * EO[ 5] + tm[k*64+ 6] * EO[ 6] + tm[k*64+ 7] * EO[ 7] +
                                    tm[k*64+ 8] * EO[ 8] + tm[k*64+ 9] * EO[ 9] + tm[k*64+10] * EO[10] + tm[k*64+11] * EO[11] +
                                    tm[k*64+12] * EO[12] + tm[k*64+13] * EO[13] + tm[k*64+14] * EO[14] + tm[k*64+15] * EO[15] + add) >> shift);
        }
        for (k = 1; k < 32; k += 2) {
            dst[ k *line ] = (s16)((tm[k*64+ 0] * O[ 0] + tm[k*64+ 1] * O[ 1] + tm[k*64+ 2] * O[ 2] + tm[k*64+ 3] * O[ 3] +
                                    tm[k*64+ 4] * O[ 4] + tm[k*64+ 5] * O[ 5] + tm[k*64+ 6] * O[ 6] + tm[k*64+ 7] * O[ 7] +
                                    tm[k*64+ 8] * O[ 8] + tm[k*64+ 9] * O[ 9] + tm[k*64+10] * O[10] + tm[k*64+11] * O[11] +
                                    tm[k*64+12] * O[12] + tm[k*64+13] * O[13] + tm[k*64+14] * O[14] + tm[k*64+15] * O[15] +
                                    tm[k*64+16] * O[16] + tm[k*64+17] * O[17] + tm[k*64+18] * O[18] + tm[k*64+19] * O[19] +
                                    tm[k*64+20] * O[20] + tm[k*64+21] * O[21] + tm[k*64+22] * O[22] + tm[k*64+23] * O[23] +
                                    tm[k*64+24] * O[24] + tm[k*64+25] * O[25] + tm[k*64+26] * O[26] + tm[k*64+27] * O[27] +
                                    tm[k*64+28] * O[28] + tm[k*64+29] * O[29] + tm[k*64+30] * O[30] + tm[k*64+31] * O[31] + add) >> shift);
        }
        src += tx_size;
        dst ++;
    }
}

void trans_dct2_w4_h4(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[4*4]);
    tx_dct2_pb4(src, tmp, 4, 4, 0 + bit_depth - 8);
    tx_dct2_pb4(tmp, dst, 4, 4, 7);
}

void trans_dct2_w4_h8(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[4*8]);
    tx_dct2_pb4(src, tmp, 8, 8, 0 + bit_depth - 8);
    tx_dct2_pb8(tmp, dst, 4, 4, 8);
}

void trans_dct2_w4_h16(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[4*16]);
    tx_dct2_pb4(src, tmp, 16, 16, 0 + bit_depth - 8);
    tx_dct2_pb16(tmp, dst, 4, 4, 9);
}

void trans_dct2_w4_h32(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[4*32]);
    tx_dct2_pb4(src, tmp, 32, 32, 0 + bit_depth - 8);
    tx_dct2_pb32(tmp, dst, 4, 4, 10);
}

void trans_dct2_w8_h4(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8*4]);
    tx_dct2_pb8(src, tmp, 4, 4, 1 + bit_depth - 8);
    tx_dct2_pb4(tmp, dst, 8, 8, 7);
}

void trans_dct2_w8_h8(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8 * 8]);
    tx_dct2_pb8(src, tmp, 8, 8, 1 + bit_depth - 8);
    tx_dct2_pb8(tmp, dst, 8, 8, 8);
}

void trans_dct2_w8_h16(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8*16]);
    tx_dct2_pb8(src, tmp, 16, 16, 1 + bit_depth - 8);
    tx_dct2_pb16(tmp, dst, 8, 8, 9);
}

void trans_dct2_w8_h32(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8*32]);
    tx_dct2_pb8(src, tmp, 32, 32, 1 + bit_depth - 8);
    tx_dct2_pb32(tmp, dst, 8, 8, 10);
}

void trans_dct2_w8_h64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[8*64]);
    tx_dct2_pb8(src, tmp, 64, 64, 1 + bit_depth - 8);
    tx_dct2_pb64(tmp, dst, 8, 8, 11);
}

void trans_dct2_w16_h4(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16*4]);
    tx_dct2_pb16(src, tmp, 4, 4, 2 + bit_depth - 8);
    tx_dct2_pb4(tmp, dst, 16, 16, 7);
}

void trans_dct2_w16_h8(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16*8]);
    tx_dct2_pb16(src, tmp, 8, 8, 2 + bit_depth - 8);
    tx_dct2_pb8(tmp, dst, 16, 16, 8);
}

void trans_dct2_w16_h16(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16 * 16]);
    tx_dct2_pb16(src, tmp, 16, 16, 2 + bit_depth - 8);
    tx_dct2_pb16(tmp, dst, 16, 16, 9);
}

void trans_dct2_w16_h32(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16*32]);
    tx_dct2_pb16(src, tmp, 32, 32, 2 + bit_depth - 8);
    tx_dct2_pb32(tmp, dst, 16, 16, 10);
}

void trans_dct2_w16_h64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[16*64]);
    tx_dct2_pb16(src, tmp, 64, 64, 2 + bit_depth - 8);
    tx_dct2_pb64(tmp, dst, 16, 16, 11);
}

void trans_dct2_w32_h4(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32*4]);
    tx_dct2_pb32(src, tmp, 4, 4, 3 + bit_depth - 8);
    tx_dct2_pb4(tmp, dst, 32, 32, 7);
}

void trans_dct2_w32_h8(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32*8]);
    tx_dct2_pb32(src, tmp, 8, 8, 3 + bit_depth - 8);
    tx_dct2_pb8(tmp, dst, 32, 32, 8);
}

void trans_dct2_w32_h16(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32*16]);
    tx_dct2_pb32(src, tmp, 16, 16, 3 + bit_depth - 8);
    tx_dct2_pb16(tmp, dst, 32, 32, 9);
}

void trans_dct2_w32_h32(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32 * 32]);
    tx_dct2_pb32(src, tmp, 32, 32, 3 + bit_depth - 8);
    tx_dct2_pb32(tmp, dst, 32, 32, 10);
}

void trans_dct2_w32_h64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[32*64]);
    tx_dct2_pb32(src, tmp, 64, 64, 3 + bit_depth - 8);
    tx_dct2_pb64(tmp, dst, 32, 32, 11);
}

void trans_dct2_w64_h8(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[64*8]);
    tx_dct2_pb64(src, tmp, 8, 8, 4 + bit_depth - 8);
    tx_dct2_pb8(tmp, dst, 64, 32, 8);
}

void trans_dct2_w64_h16(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[64*16]);
    tx_dct2_pb64(src, tmp, 16, 16, 4 + bit_depth - 8);
    tx_dct2_pb16(tmp, dst, 64, 32, 9);
}

void trans_dct2_w64_h32(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[64*32]);
    tx_dct2_pb64(src, tmp, 32, 32, 4 + bit_depth - 8);
    tx_dct2_pb32(tmp, dst, 64, 32, 10);
}

void trans_dct2_w64_h64(s16 *src, s16 *dst, int bit_depth)
{
    ALIGNED_16(s16 tmp[64 * 64]);
    tx_dct2_pb64(src, tmp, 64, 64, 4 + bit_depth - 8);
    tx_dct2_pb64(tmp, dst, 64, 32, 11);
}

/******************   DCT-8   ******************************************/

static void tx_dct8_pb4(s16 *src, s16 *dst, int shift, int line)  // input src, output dst
{
    int i;
    int rnd_factor = 1 << (shift - 1);

    s8 *iT = com_tbl_tm4[DCT8][0];
    const int  reducedLine = line;
    for (i = 0; i < reducedLine; i++) {
        dst[0 * line] = (s16)(((iT[2] + iT[3]) * src[0] + iT[1] * src[1] + iT[2] * src[2] + iT[3] * src[3] + rnd_factor) >> shift);
        dst[1 * line] = (s16)((iT[1] * src[0] + (-iT[1]) * src[2] + (-iT[1]) * src[3] + rnd_factor) >> shift);
        dst[2 * line] = (s16)((iT[2] * src[0] + (-iT[1]) * src[1] + (-iT[3]) * src[2] + (iT[2] + iT[3]) * src[3] + rnd_factor) >> shift);
        dst[3 * line] = (s16)((iT[3] * src[0] + (-iT[1]) * src[1] + (iT[2] + iT[3]) * src[2] + (-iT[2]) * src[3] + rnd_factor) >> shift);

        src += 4;
        dst++;
    }
}

static void tx_dct8_pb8(s16 *src, s16 *dst, int shift, int line)  // input src, output dst
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);

    const int uiTrSize = 8;
    s8 *iT;
    s16 *pCoef;

    const int  reducedLine = line;
    const int  cutoff = uiTrSize;
    for (i = 0; i < reducedLine; i++) {
        pCoef = dst;
        iT = com_tbl_tm8[DCT8][0];

        for (j = 0; j < cutoff; j++) {
            iSum = 0;
            for (k = 0; k < uiTrSize; k++) {
                iSum += src[k] * iT[k];
            }
            pCoef[i] = (s16)((iSum + rnd_factor) >> shift);
            pCoef += line;
            iT += uiTrSize;
        }
        src += uiTrSize;
    }
}

static void tx_dct8_pb16(s16 *src, s16 *dst, int shift, int line)  // input src, output dst
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);

    const int uiTrSize = 16;
    s8 *iT;
    s16 *pCoef;

    const int  reducedLine = line;
    const int  cutoff = uiTrSize;
    for (i = 0; i < reducedLine; i++) {
        pCoef = dst;
        iT = com_tbl_tm16[DCT8][0];
        for (j = 0; j < cutoff; j++) {
            iSum = 0;
            for (k = 0; k < uiTrSize; k++) {
                iSum += src[k] * iT[k];
            }
            pCoef[i] = (s16)((iSum + rnd_factor) >> shift);
            pCoef += line;
            iT += uiTrSize;
        }
        src += uiTrSize;
    }
}

static void tx_dct8_pb32(s16 *src, s16 *dst, int shift, int line)  // input src, output dst
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);

    const int uiTrSize = 32;
    s8 *iT;
    s16 *pCoef;

    const int  reducedLine = line;
    const int  cutoff = uiTrSize;
    for (i = 0; i < reducedLine; i++) {
        pCoef = dst;
        iT = com_tbl_tm32[DCT8][0];
        for (j = 0; j < cutoff; j++) {
            iSum = 0;
            for (k = 0; k < uiTrSize; k++) {
                iSum += src[k] * iT[k];
            }
            pCoef[i] = (s16)((iSum + rnd_factor) >> shift);
            iT += uiTrSize;
            pCoef += line;
        }
        src += uiTrSize;
    }
}

static void tx_dct8_pb64(s16 *src, s16 *dst, int shift, int line)  // input src, output dst
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);

    const int uiTrSize = 64;
    s8 *iT;
    s16 *pCoef;

    const int  reducedLine = line;
    const int  cutoff = uiTrSize;
    for (i = 0; i < reducedLine; i++) {
        pCoef = dst;
        iT = com_tbl_tm64[DCT8][0];
        for (j = 0; j < cutoff; j++) {
            iSum = 0;
            for (k = 0; k < uiTrSize; k++) {
                iSum += src[k] * iT[k];
            }
            pCoef[i] = (s16)((iSum + rnd_factor) >> shift);
            iT += uiTrSize;
            pCoef += line;
        }
        src += uiTrSize;
    }
}

/******************   DST-7   ******************************************/

static void tx_dst7_pb4(s16 *src, s16 *dst, int shift, int line)  // input src, output dst
{
    int i;
    int rnd_factor = 1 << (shift - 1);

    s8 *iT = com_tbl_tm4[DST7][0];

    const int  reducedLine = line;
    for (i = 0; i < reducedLine; i++) {
        dst[0 * line] = (s16)((iT[0] * src[0] + iT[1] * src[1] + iT[2] * src[2] + (iT[0] + iT[1]) * src[3] + rnd_factor) >> shift);
        dst[1 * line] = (s16)((iT[2] * (src[0] + src[1] - src[3]) + rnd_factor) >> shift);
        dst[2 * line] = (s16)(((iT[0] + iT[1]) * src[0] + (-iT[0]) * src[1] + (-iT[2]) * src[2] + iT[1] * src[3] + rnd_factor) >> shift);
        dst[3 * line] = (s16)((iT[1] * src[0] + (-iT[0] - iT[1]) * src[1] + iT[2] * src[2] + (-iT[0]) * src[3] + rnd_factor) >> shift);

        src += 4;
        dst++;
    }
}

static void tx_dst7_pb8(s16 *src, s16 *dst, int shift, int line)  // input src, output dst
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);

    const int uiTrSize = 8;
    s8 *iT;
    s16 *pCoef;

    const int  reducedLine = line;
    const int  cutoff = uiTrSize;
    for (i = 0; i < reducedLine; i++) {
        pCoef = dst;
        iT = com_tbl_tm8[DST7][0];
        for (j = 0; j < cutoff; j++) {
            iSum = 0;
            for (k = 0; k < uiTrSize; k++) {
                iSum += src[k] * iT[k];
            }
            pCoef[i] = (s16)((iSum + rnd_factor) >> shift);
            pCoef += line;
            iT += uiTrSize;
        }
        src += uiTrSize;
    }
}

static void tx_dst7_pb16(s16 *src, s16 *dst, int shift, int line)  // input src, output dst
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);

    const int uiTrSize = 16;
    s8 *iT;
    s16 *pCoef;

    const int  reducedLine = line;
    const int  cutoff = uiTrSize;
    for (i = 0; i < reducedLine; i++) {
        pCoef = dst;
        iT = com_tbl_tm16[DST7][0];
        for (j = 0; j < cutoff; j++) {
            iSum = 0;
            for (k = 0; k < uiTrSize; k++) {
                iSum += src[k] * iT[k];
            }
            pCoef[i] = (s16)((iSum + rnd_factor) >> shift);
            pCoef += line;
            iT += uiTrSize;
        }
        src += uiTrSize;
    }
}

static void tx_dst7_pb32(s16 *src, s16 *dst, int shift, int line)  // input src, output dst
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);

    const int uiTrSize = 32;
    s8 *iT;
    s16 *pCoef;


    const int  reducedLine = line;
    const int  cutoff = uiTrSize;
    for (i = 0; i < reducedLine; i++) {
        pCoef = dst;
        iT = com_tbl_tm32[DST7][0];
        for (j = 0; j < cutoff; j++) {
            iSum = 0;
            for (k = 0; k < uiTrSize; k++) {
                iSum += src[k] * iT[k];
            }
            pCoef[i] = (s16)((iSum + rnd_factor) >> shift);
            iT += uiTrSize;
            pCoef += line;
        }
        src += uiTrSize;
    }
}

static void tx_dst7_pb64(s16 *src, s16 *dst, int shift, int line)  // input src, output dst
{
    int i, j, k, iSum;
    int rnd_factor = 1 << (shift - 1);

    const int uiTrSize = 64;
    s8 *iT;
    s16 *pCoef;

    const int  reducedLine = line;
    const int  cutoff = uiTrSize;
    for (i = 0; i < reducedLine; i++) {
        pCoef = dst;
        iT = com_tbl_tm64[DST7][0];
        for (j = 0; j < cutoff; j++) {
            iSum = 0;
            for (k = 0; k < uiTrSize; k++) {
                iSum += src[k] * iT[k];
            }
            pCoef[i] = (s16)((iSum + rnd_factor) >> shift);
            iT += uiTrSize;
            pCoef += line;
        }
        src += uiTrSize;
    }
}


typedef void (*COM_TX)(s16 *coef, s16 *t, int shift, int line);

static void xCTr_4_1d_Hor(s16 *src, int i_src, s16 *dst, int i_dst, int shift)
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
                sum += com_tbl_c4_trans[i][k] * tmpSrc[j][k];
            }
            dst[j *i_dst + i] = (s16)COM_CLIP3(-32768, 32767, sum >> shift);
        }
    }
}

static void xCTr_4_1d_Vert(s16 *src, int i_src, s16 *dst, int i_dst, int shift)
{
    int i, j, k, sum;
    int rnd_factor = shift == 0 ? 0 : 1 << (shift - 1);
    int tmpSrc[4][4];
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmpSrc[i][j] = src[i* i_src + j];
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            sum = rnd_factor;
            for (k = 0; k < 4; k++) {
                sum += com_tbl_c4_trans[i][k] * tmpSrc[k][j];
            }
            dst[i *i_dst + j] = (s16)COM_CLIP3(-32768, 32767, sum >> shift);
        }
    }
}

static void xTr2nd_8_1d_Hor(s16 *src, int i_src)
{
    int i, j, k, sum;
    int rnd_factor;
    int tmpSrc[4][4];

    rnd_factor = 1 << (7 - 1);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmpSrc[i][j] = src[i * i_src + j];
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            sum = rnd_factor;
            for (k = 0; k < 4; k++) {
                sum += com_tbl_c8_trans[i][k] * tmpSrc[j][k];
            }
            src[j *i_src + i] = (s16)COM_CLIP3(-32768, 32767, sum >> 7);
        }
    }
}

static void xTr2nd_8_1d_Vert(s16 *src, int i_src)
{
    int i, j, k, sum;
    int rnd_factor;
    int tmpSrc[4][4];

    rnd_factor = 1 << (7 - 1);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmpSrc[i][j] = src[i * i_src + j];
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            sum = rnd_factor;
            for (k = 0; k < 4; k++) {
                sum += com_tbl_c8_trans[i][k] * tmpSrc[k][j];
            }
            src[i *i_src + j] = (s16)COM_CLIP3(-32768, 32767, sum >> 7);
        }
    }
}

static int com_get_forward_trans_shift(int log2_size, int type, int bit_depth)
{
    assert(log2_size <= 6);
    return (type == 0) ? (log2_size + bit_depth - 10) : (log2_size + 5);
}

void transform(com_mode_t *mode, int plane, int blk_idx, s16 *coef, s16 *resi, int cu_width_log2, int cu_height_log2, int bit_depth, int secT_Ver_Hor, int use_alt4x4Trans)
{
    if (use_alt4x4Trans && cu_width_log2 == 2 && cu_height_log2 == 2) {
        s16 coef_temp2[16];
        int shift1 = com_get_forward_trans_shift(cu_width_log2, 0, bit_depth);
        int shift2 = com_get_forward_trans_shift(cu_height_log2, 1, bit_depth);
        xCTr_4_1d_Hor(resi, 4, coef_temp2, 4, shift1 + 1);
        xCTr_4_1d_Vert(coef_temp2, 4, coef, 4, shift2 + 1);
    } else {
        if (plane == Y_C && mode->tb_part == SIZE_NxN) {
            ALIGNED_32(s16 coef_temp[MAX_TR_DIM]);
            int shift1 = com_get_forward_trans_shift(cu_width_log2, 0, bit_depth);
            int shift2 = com_get_forward_trans_shift(cu_height_log2, 1, bit_depth);
            uavs3e_funs_handle.trans_dct8_dst7[blk_idx &  1][cu_width_log2  - 1](resi, coef_temp, shift1, 1 << cu_height_log2);
            uavs3e_funs_handle.trans_dct8_dst7[blk_idx >> 1][cu_height_log2 - 1](coef_temp, coef, shift2, 1 << cu_width_log2 );
        } else{
            uavs3e_funs_handle.trans_dct2[cu_width_log2 - 1][cu_height_log2 - 1](resi, coef, bit_depth);
        } 
        if (secT_Ver_Hor >> 1) {
            xTr2nd_8_1d_Vert(coef, 1 << cu_width_log2);
        }
        if (secT_Ver_Hor & 1) {
            xTr2nd_8_1d_Hor(coef, 1 << cu_width_log2);
        }
    }
}

void uavs3e_funs_init_trans_c()
{
    uavs3e_funs_handle.trans_dct2[1][1] = trans_dct2_w4_h4;
    uavs3e_funs_handle.trans_dct2[1][2] = trans_dct2_w4_h8;
    uavs3e_funs_handle.trans_dct2[1][3] = trans_dct2_w4_h16;
    uavs3e_funs_handle.trans_dct2[1][4] = trans_dct2_w4_h32;

    uavs3e_funs_handle.trans_dct2[2][1] = trans_dct2_w8_h4;
    uavs3e_funs_handle.trans_dct2[2][2] = trans_dct2_w8_h8;
    uavs3e_funs_handle.trans_dct2[2][3] = trans_dct2_w8_h16;
    uavs3e_funs_handle.trans_dct2[2][4] = trans_dct2_w8_h32;
    uavs3e_funs_handle.trans_dct2[2][5] = trans_dct2_w8_h64;

    uavs3e_funs_handle.trans_dct2[3][1] = trans_dct2_w16_h4;
    uavs3e_funs_handle.trans_dct2[3][2] = trans_dct2_w16_h8;
    uavs3e_funs_handle.trans_dct2[3][3] = trans_dct2_w16_h16;
    uavs3e_funs_handle.trans_dct2[3][4] = trans_dct2_w16_h32;
    uavs3e_funs_handle.trans_dct2[3][5] = trans_dct2_w16_h64;

    uavs3e_funs_handle.trans_dct2[4][1] = trans_dct2_w32_h4;
    uavs3e_funs_handle.trans_dct2[4][2] = trans_dct2_w32_h8;
    uavs3e_funs_handle.trans_dct2[4][3] = trans_dct2_w32_h16;
    uavs3e_funs_handle.trans_dct2[4][4] = trans_dct2_w32_h32;
    uavs3e_funs_handle.trans_dct2[4][5] = trans_dct2_w32_h64;

    uavs3e_funs_handle.trans_dct2[5][2] = trans_dct2_w64_h8;
    uavs3e_funs_handle.trans_dct2[5][3] = trans_dct2_w64_h16;
    uavs3e_funs_handle.trans_dct2[5][4] = trans_dct2_w64_h32;
    uavs3e_funs_handle.trans_dct2[5][5] = trans_dct2_w64_h64;

    uavs3e_funs_handle.trans_dct8_dst7[0][1] = tx_dct8_pb4;
    uavs3e_funs_handle.trans_dct8_dst7[0][2] = tx_dct8_pb8;
    uavs3e_funs_handle.trans_dct8_dst7[0][3] = tx_dct8_pb16;
    uavs3e_funs_handle.trans_dct8_dst7[0][4] = tx_dct8_pb32;
    uavs3e_funs_handle.trans_dct8_dst7[0][5] = tx_dct8_pb64;

    uavs3e_funs_handle.trans_dct8_dst7[1][1] = tx_dst7_pb4;
    uavs3e_funs_handle.trans_dct8_dst7[1][2] = tx_dst7_pb8;
    uavs3e_funs_handle.trans_dct8_dst7[1][3] = tx_dst7_pb16;
    uavs3e_funs_handle.trans_dct8_dst7[1][4] = tx_dst7_pb32;
    uavs3e_funs_handle.trans_dct8_dst7[1][5] = tx_dst7_pb64;
}

