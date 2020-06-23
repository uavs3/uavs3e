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

#include "com_modules.h"

#define DEFINE_SAD(w) \
static u32 com_get_sad_##w(pel *p_org, int i_org, pel *p_pred, int i_pred, int height) { \
    u32 uiSum = 0;                                                                       \
    while (height--) {                                                                   \
        for (int i = 0; i < w; i++) {                                                    \
            uiSum += abs(p_org[i] - p_pred[i]);                                          \
        }                                                                                \
        p_org  += i_org;                                                                 \
        p_pred += i_pred;                                                                \
    }                                                                                    \
    return uiSum;                                                                        \
}

DEFINE_SAD(4)
DEFINE_SAD(8)
DEFINE_SAD(16)
DEFINE_SAD(32)
DEFINE_SAD(64)
DEFINE_SAD(128)

#define DEFINE_SAD_X3(w) \
static void com_get_sad_x3_##w(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2,    \
                                                      int i_pred, u32 sad[3], int height) {  \
    u32 uiSum0 = 0, uiSum1 = 0, uiSum2 = 0;                                                  \
    while (height--) {                                                                       \
        for (int i = 0; i < w; i++) {                                                        \
            uiSum0 += abs(p_org[i] - pred0[i]);                                              \
            uiSum1 += abs(p_org[i] - pred1[i]);                                              \
            uiSum2 += abs(p_org[i] - pred2[i]);                                              \
        }                                                                                    \
        p_org += i_org;                                                                      \
        pred0 += i_pred;                                                                     \
        pred1 += i_pred;                                                                     \
        pred2 += i_pred;                                                                     \
    }                                                                                        \
    sad[0] = uiSum0;                                                                         \
    sad[1] = uiSum1;                                                                         \
    sad[2] = uiSum2;                                                                         \
}

DEFINE_SAD_X3(4)
DEFINE_SAD_X3(8)
DEFINE_SAD_X3(16)
DEFINE_SAD_X3(32)
DEFINE_SAD_X3(64)
DEFINE_SAD_X3(128)

#define DEFINE_SAD_X4(w) \
static void com_get_sad_x4_##w(pel *p_org, int i_org, pel *pred0, pel *pred1, pel *pred2, pel *pred3,    \
                                                                  int i_pred, u32 sad[4], int height) {  \
    u32 uiSum0 = 0, uiSum1 = 0, uiSum2 = 0, uiSum3 = 0;                                                  \
    while (height--) {                                                                                   \
        for (int i = 0; i < w; i++) {                                                                    \
            uiSum0 += abs(p_org[i] - pred0[i]);                                                          \
            uiSum1 += abs(p_org[i] - pred1[i]);                                                          \
            uiSum2 += abs(p_org[i] - pred2[i]);                                                          \
            uiSum3 += abs(p_org[i] - pred3[i]);                                                          \
        }                                                                                                \
        p_org += i_org;                                                                                  \
        pred0 += i_pred;                                                                                 \
        pred1 += i_pred;                                                                                 \
        pred2 += i_pred;                                                                                 \
        pred3 += i_pred;                                                                                 \
    }                                                                                                    \
    sad[0] = uiSum0;                                                                                     \
    sad[1] = uiSum1;                                                                                     \
    sad[2] = uiSum2;                                                                                     \
    sad[3] = uiSum3;                                                                                     \
}

DEFINE_SAD_X4(4)
DEFINE_SAD_X4(8)
DEFINE_SAD_X4(16)
DEFINE_SAD_X4(32)
DEFINE_SAD_X4(64)
DEFINE_SAD_X4(128)

#define DEFINE_SSD(w) \
static u64 com_get_ssd_##w(pel *p_org, int i_org, pel *p_pred, int i_pred, int height) { \
    int i;                                                                               \
    u64 uiSum = 0;                                                                       \
    for (; height != 0; height--) {                                                      \
        for (i = 0; i < w; i++) {                                                        \
            int ssd = abs(p_org[i] - p_pred[i]);                                         \
            uiSum += (ssd * ssd);                                                        \
        }                                                                                \
        p_org += i_org;                                                                  \
        p_pred += i_pred;                                                                \
    }                                                                                    \
    return uiSum;                                                                        \
}

DEFINE_SSD(4)
DEFINE_SSD(8)
DEFINE_SSD(16)
DEFINE_SSD(32)
DEFINE_SSD(64)
DEFINE_SSD(128)

#define DEFINE_VAR(w)                                                             \
static u64 com_get_var_##w(pel *p_org, int i_org) {                               \
    int i, height = w;                                                            \
    u64 uiSum = 0, uiSSD = 0;                                                     \
    for (; height != 0; height--) {                                               \
        for (i = 0; i < w; i++) {                                                 \
            uiSum += p_org[i];                                                    \
            uiSSD += (p_org[i] * p_org[i]);                                       \
        }                                                                         \
        p_org += i_org;                                                           \
    }                                                                             \
    return uiSSD - ((uiSum * uiSum) >> (CONV_LOG2(w) * 2));                       \
}

DEFINE_VAR(4)
DEFINE_VAR(8)
DEFINE_VAR(16)
DEFINE_VAR(32)
DEFINE_VAR(64)
DEFINE_VAR(128)

static u64 com_var_cost(const pel* pix, int i_pix, int width, int height)
{
    u64 sum = 0, sqr = 0;
    int x, y;

    for (y = 0; y < width; y++) {
        for (x = 0; x < height; x++) {
            sum += pix[x];
            sqr += pix[x] * pix[x];
        }
        pix += i_pix;
    }
    return sqr - (sum * sum >> 8);
}


u32 com_had_4x4(pel *org, int s_org, pel *cur, int s_cur)
{
    int k;
    int satd = 0;
    int diff[16], m[16], d[16];
    for (k = 0; k < 16; k += 4) {
        diff[k + 0] = org[0] - cur[0];
        diff[k + 1] = org[1] - cur[1];
        diff[k + 2] = org[2] - cur[2];
        diff[k + 3] = org[3] - cur[3];
        cur += s_cur;
        org += s_org;
    }
    m[0] = diff[0] + diff[12];
    m[1] = diff[1] + diff[13];
    m[2] = diff[2] + diff[14];
    m[3] = diff[3] + diff[15];
    m[4] = diff[4] + diff[8];
    m[5] = diff[5] + diff[9];
    m[6] = diff[6] + diff[10];
    m[7] = diff[7] + diff[11];

    m[8] = diff[4] - diff[8];
    m[9] = diff[5] - diff[9];
    m[10] = diff[6] - diff[10];
    m[11] = diff[7] - diff[11];
    m[12] = diff[0] - diff[12];
    m[13] = diff[1] - diff[13];
    m[14] = diff[2] - diff[14];
    m[15] = diff[3] - diff[15];

    d[0] = m[0] + m[4];
    d[1] = m[1] + m[5];
    d[2] = m[2] + m[6];
    d[3] = m[3] + m[7];
    d[4] = m[8] + m[12];
    d[5] = m[9] + m[13];
    d[6] = m[10] + m[14];
    d[7] = m[11] + m[15];
    d[8] = m[0] - m[4];
    d[9] = m[1] - m[5];
    d[10] = m[2] - m[6];
    d[11] = m[3] - m[7];
    d[12] = m[12] - m[8];
    d[13] = m[13] - m[9];
    d[14] = m[14] - m[10];
    d[15] = m[15] - m[11];

    m[0] = d[0] + d[3];
    m[1] = d[1] + d[2];
    m[2] = d[1] - d[2];
    m[3] = d[0] - d[3];
    m[4] = d[4] + d[7];
    m[5] = d[5] + d[6];
    m[6] = d[5] - d[6];
    m[7] = d[4] - d[7];
    m[8] = d[8] + d[11];
    m[9] = d[9] + d[10];
    m[10] = d[9] - d[10];
    m[11] = d[8] - d[11];
    m[12] = d[12] + d[15];
    m[13] = d[13] + d[14];
    m[14] = d[13] - d[14];
    m[15] = d[12] - d[15];
    d[0] = m[0] + m[1];
    d[1] = m[0] - m[1];
    d[2] = m[2] + m[3];
    d[3] = m[3] - m[2];
    d[4] = m[4] + m[5];
    d[5] = m[4] - m[5];
    d[6] = m[6] + m[7];
    d[7] = m[7] - m[6];
    d[8] = m[8] + m[9];
    d[9] = m[8] - m[9];
    d[10] = m[10] + m[11];
    d[11] = m[11] - m[10];
    d[12] = m[12] + m[13];
    d[13] = m[12] - m[13];
    d[14] = m[14] + m[15];
    d[15] = m[15] - m[14];
    for (k = 0; k < 16; k++) {
        satd += COM_ABS(d[k]);
    }
    satd = ((satd + 1) >> 1);
    return satd;
}

u32 com_had_8x8(pel *org, int s_org, pel *cur, int s_cur)
{
    int k, i, j, jj;
    int satd = 0;
    int diff[64], m1[8][8], m2[8][8], m3[8][8];
    for (k = 0; k < 64; k += 8) {
        diff[k + 0] = org[0] - cur[0];
        diff[k + 1] = org[1] - cur[1];
        diff[k + 2] = org[2] - cur[2];
        diff[k + 3] = org[3] - cur[3];
        diff[k + 4] = org[4] - cur[4];
        diff[k + 5] = org[5] - cur[5];
        diff[k + 6] = org[6] - cur[6];
        diff[k + 7] = org[7] - cur[7];
        cur += s_cur;
        org += s_org;
    }
    /* horizontal */
    for (j = 0; j < 8; j++) {
        jj = j << 3;
        m2[j][0] = diff[jj] + diff[jj + 4];
        m2[j][1] = diff[jj + 1] + diff[jj + 5];
        m2[j][2] = diff[jj + 2] + diff[jj + 6];
        m2[j][3] = diff[jj + 3] + diff[jj + 7];
        m2[j][4] = diff[jj] - diff[jj + 4];
        m2[j][5] = diff[jj + 1] - diff[jj + 5];
        m2[j][6] = diff[jj + 2] - diff[jj + 6];
        m2[j][7] = diff[jj + 3] - diff[jj + 7];
        m1[j][0] = m2[j][0] + m2[j][2];
        m1[j][1] = m2[j][1] + m2[j][3];
        m1[j][2] = m2[j][0] - m2[j][2];
        m1[j][3] = m2[j][1] - m2[j][3];
        m1[j][4] = m2[j][4] + m2[j][6];
        m1[j][5] = m2[j][5] + m2[j][7];
        m1[j][6] = m2[j][4] - m2[j][6];
        m1[j][7] = m2[j][5] - m2[j][7];
        m2[j][0] = m1[j][0] + m1[j][1];
        m2[j][1] = m1[j][0] - m1[j][1];
        m2[j][2] = m1[j][2] + m1[j][3];
        m2[j][3] = m1[j][2] - m1[j][3];
        m2[j][4] = m1[j][4] + m1[j][5];
        m2[j][5] = m1[j][4] - m1[j][5];
        m2[j][6] = m1[j][6] + m1[j][7];
        m2[j][7] = m1[j][6] - m1[j][7];
    }
    /* vertical */
    for (i = 0; i < 8; i++) {
        m3[0][i] = m2[0][i] + m2[4][i];
        m3[1][i] = m2[1][i] + m2[5][i];
        m3[2][i] = m2[2][i] + m2[6][i];
        m3[3][i] = m2[3][i] + m2[7][i];
        m3[4][i] = m2[0][i] - m2[4][i];
        m3[5][i] = m2[1][i] - m2[5][i];
        m3[6][i] = m2[2][i] - m2[6][i];
        m3[7][i] = m2[3][i] - m2[7][i];
        m1[0][i] = m3[0][i] + m3[2][i];
        m1[1][i] = m3[1][i] + m3[3][i];
        m1[2][i] = m3[0][i] - m3[2][i];
        m1[3][i] = m3[1][i] - m3[3][i];
        m1[4][i] = m3[4][i] + m3[6][i];
        m1[5][i] = m3[5][i] + m3[7][i];
        m1[6][i] = m3[4][i] - m3[6][i];
        m1[7][i] = m3[5][i] - m3[7][i];
        m2[0][i] = m1[0][i] + m1[1][i];
        m2[1][i] = m1[0][i] - m1[1][i];
        m2[2][i] = m1[2][i] + m1[3][i];
        m2[3][i] = m1[2][i] - m1[3][i];
        m2[4][i] = m1[4][i] + m1[5][i];
        m2[5][i] = m1[4][i] - m1[5][i];
        m2[6][i] = m1[6][i] + m1[7][i];
        m2[7][i] = m1[6][i] - m1[7][i];
    }
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            satd += COM_ABS(m2[i][j]);
        }
    }
    satd = ((satd + 2) >> 2);
    return satd;
}

u32 com_had_16x8(pel *org, int s_org, pel *cur, int s_cur)
{
    int k, i, j, jj;
    int satd = 0;
    int diff[128], m1[8][16], m2[8][16];
    for (k = 0; k < 128; k += 16) {
        diff[k + 0] = org[0] - cur[0];
        diff[k + 1] = org[1] - cur[1];
        diff[k + 2] = org[2] - cur[2];
        diff[k + 3] = org[3] - cur[3];
        diff[k + 4] = org[4] - cur[4];
        diff[k + 5] = org[5] - cur[5];
        diff[k + 6] = org[6] - cur[6];
        diff[k + 7] = org[7] - cur[7];
        diff[k + 8] = org[8] - cur[8];
        diff[k + 9] = org[9] - cur[9];
        diff[k + 10] = org[10] - cur[10];
        diff[k + 11] = org[11] - cur[11];
        diff[k + 12] = org[12] - cur[12];
        diff[k + 13] = org[13] - cur[13];
        diff[k + 14] = org[14] - cur[14];
        diff[k + 15] = org[15] - cur[15];
        cur += s_cur;
        org += s_org;
    }
    for (j = 0; j < 8; j++) {
        jj = j << 4;
        m2[j][0] = diff[jj] + diff[jj + 8];
        m2[j][1] = diff[jj + 1] + diff[jj + 9];
        m2[j][2] = diff[jj + 2] + diff[jj + 10];
        m2[j][3] = diff[jj + 3] + diff[jj + 11];
        m2[j][4] = diff[jj + 4] + diff[jj + 12];
        m2[j][5] = diff[jj + 5] + diff[jj + 13];
        m2[j][6] = diff[jj + 6] + diff[jj + 14];
        m2[j][7] = diff[jj + 7] + diff[jj + 15];
        m2[j][8] = diff[jj] - diff[jj + 8];
        m2[j][9] = diff[jj + 1] - diff[jj + 9];
        m2[j][10] = diff[jj + 2] - diff[jj + 10];
        m2[j][11] = diff[jj + 3] - diff[jj + 11];
        m2[j][12] = diff[jj + 4] - diff[jj + 12];
        m2[j][13] = diff[jj + 5] - diff[jj + 13];
        m2[j][14] = diff[jj + 6] - diff[jj + 14];
        m2[j][15] = diff[jj + 7] - diff[jj + 15];
        m1[j][0] = m2[j][0] + m2[j][4];
        m1[j][1] = m2[j][1] + m2[j][5];
        m1[j][2] = m2[j][2] + m2[j][6];
        m1[j][3] = m2[j][3] + m2[j][7];
        m1[j][4] = m2[j][0] - m2[j][4];
        m1[j][5] = m2[j][1] - m2[j][5];
        m1[j][6] = m2[j][2] - m2[j][6];
        m1[j][7] = m2[j][3] - m2[j][7];
        m1[j][8] = m2[j][8] + m2[j][12];
        m1[j][9] = m2[j][9] + m2[j][13];
        m1[j][10] = m2[j][10] + m2[j][14];
        m1[j][11] = m2[j][11] + m2[j][15];
        m1[j][12] = m2[j][8] - m2[j][12];
        m1[j][13] = m2[j][9] - m2[j][13];
        m1[j][14] = m2[j][10] - m2[j][14];
        m1[j][15] = m2[j][11] - m2[j][15];
        m2[j][0] = m1[j][0] + m1[j][2];
        m2[j][1] = m1[j][1] + m1[j][3];
        m2[j][2] = m1[j][0] - m1[j][2];
        m2[j][3] = m1[j][1] - m1[j][3];
        m2[j][4] = m1[j][4] + m1[j][6];
        m2[j][5] = m1[j][5] + m1[j][7];
        m2[j][6] = m1[j][4] - m1[j][6];
        m2[j][7] = m1[j][5] - m1[j][7];
        m2[j][8] = m1[j][8] + m1[j][10];
        m2[j][9] = m1[j][9] + m1[j][11];
        m2[j][10] = m1[j][8] - m1[j][10];
        m2[j][11] = m1[j][9] - m1[j][11];
        m2[j][12] = m1[j][12] + m1[j][14];
        m2[j][13] = m1[j][13] + m1[j][15];
        m2[j][14] = m1[j][12] - m1[j][14];
        m2[j][15] = m1[j][13] - m1[j][15];
        m1[j][0] = m2[j][0] + m2[j][1];
        m1[j][1] = m2[j][0] - m2[j][1];
        m1[j][2] = m2[j][2] + m2[j][3];
        m1[j][3] = m2[j][2] - m2[j][3];
        m1[j][4] = m2[j][4] + m2[j][5];
        m1[j][5] = m2[j][4] - m2[j][5];
        m1[j][6] = m2[j][6] + m2[j][7];
        m1[j][7] = m2[j][6] - m2[j][7];
        m1[j][8] = m2[j][8] + m2[j][9];
        m1[j][9] = m2[j][8] - m2[j][9];
        m1[j][10] = m2[j][10] + m2[j][11];
        m1[j][11] = m2[j][10] - m2[j][11];
        m1[j][12] = m2[j][12] + m2[j][13];
        m1[j][13] = m2[j][12] - m2[j][13];
        m1[j][14] = m2[j][14] + m2[j][15];
        m1[j][15] = m2[j][14] - m2[j][15];
    }
    for (i = 0; i < 16; i++) {
        m2[0][i] = m1[0][i] + m1[4][i];
        m2[1][i] = m1[1][i] + m1[5][i];
        m2[2][i] = m1[2][i] + m1[6][i];
        m2[3][i] = m1[3][i] + m1[7][i];
        m2[4][i] = m1[0][i] - m1[4][i];
        m2[5][i] = m1[1][i] - m1[5][i];
        m2[6][i] = m1[2][i] - m1[6][i];
        m2[7][i] = m1[3][i] - m1[7][i];
        m1[0][i] = m2[0][i] + m2[2][i];
        m1[1][i] = m2[1][i] + m2[3][i];
        m1[2][i] = m2[0][i] - m2[2][i];
        m1[3][i] = m2[1][i] - m2[3][i];
        m1[4][i] = m2[4][i] + m2[6][i];
        m1[5][i] = m2[5][i] + m2[7][i];
        m1[6][i] = m2[4][i] - m2[6][i];
        m1[7][i] = m2[5][i] - m2[7][i];
        m2[0][i] = m1[0][i] + m1[1][i];
        m2[1][i] = m1[0][i] - m1[1][i];
        m2[2][i] = m1[2][i] + m1[3][i];
        m2[3][i] = m1[2][i] - m1[3][i];
        m2[4][i] = m1[4][i] + m1[5][i];
        m2[5][i] = m1[4][i] - m1[5][i];
        m2[6][i] = m1[6][i] + m1[7][i];
        m2[7][i] = m1[6][i] - m1[7][i];
    }
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 16; j++) {
            satd += COM_ABS(m2[i][j]);
        }
    }
    satd = (int)(satd / com_tbl_sqrt[1] * 2.0);
    return satd;
}

u32 com_had_8x16(pel *org, int s_org, pel *cur, int s_cur)
{
    int k, i, j, jj;
    int satd = 0;
    int diff[128], m1[16][8], m2[16][8];
    for (k = 0; k < 128; k += 8) {
        diff[k + 0] = org[0] - cur[0];
        diff[k + 1] = org[1] - cur[1];
        diff[k + 2] = org[2] - cur[2];
        diff[k + 3] = org[3] - cur[3];
        diff[k + 4] = org[4] - cur[4];
        diff[k + 5] = org[5] - cur[5];
        diff[k + 6] = org[6] - cur[6];
        diff[k + 7] = org[7] - cur[7];
        cur += s_cur;
        org += s_org;
    }
    for (j = 0; j < 16; j++) {
        jj = j << 3;
        m2[j][0] = diff[jj] + diff[jj + 4];
        m2[j][1] = diff[jj + 1] + diff[jj + 5];
        m2[j][2] = diff[jj + 2] + diff[jj + 6];
        m2[j][3] = diff[jj + 3] + diff[jj + 7];
        m2[j][4] = diff[jj] - diff[jj + 4];
        m2[j][5] = diff[jj + 1] - diff[jj + 5];
        m2[j][6] = diff[jj + 2] - diff[jj + 6];
        m2[j][7] = diff[jj + 3] - diff[jj + 7];
        m1[j][0] = m2[j][0] + m2[j][2];
        m1[j][1] = m2[j][1] + m2[j][3];
        m1[j][2] = m2[j][0] - m2[j][2];
        m1[j][3] = m2[j][1] - m2[j][3];
        m1[j][4] = m2[j][4] + m2[j][6];
        m1[j][5] = m2[j][5] + m2[j][7];
        m1[j][6] = m2[j][4] - m2[j][6];
        m1[j][7] = m2[j][5] - m2[j][7];
        m2[j][0] = m1[j][0] + m1[j][1];
        m2[j][1] = m1[j][0] - m1[j][1];
        m2[j][2] = m1[j][2] + m1[j][3];
        m2[j][3] = m1[j][2] - m1[j][3];
        m2[j][4] = m1[j][4] + m1[j][5];
        m2[j][5] = m1[j][4] - m1[j][5];
        m2[j][6] = m1[j][6] + m1[j][7];
        m2[j][7] = m1[j][6] - m1[j][7];
    }
    for (i = 0; i < 8; i++) {
        m1[0][i] = m2[0][i] + m2[8][i];
        m1[1][i] = m2[1][i] + m2[9][i];
        m1[2][i] = m2[2][i] + m2[10][i];
        m1[3][i] = m2[3][i] + m2[11][i];
        m1[4][i] = m2[4][i] + m2[12][i];
        m1[5][i] = m2[5][i] + m2[13][i];
        m1[6][i] = m2[6][i] + m2[14][i];
        m1[7][i] = m2[7][i] + m2[15][i];
        m1[8][i] = m2[0][i] - m2[8][i];
        m1[9][i] = m2[1][i] - m2[9][i];
        m1[10][i] = m2[2][i] - m2[10][i];
        m1[11][i] = m2[3][i] - m2[11][i];
        m1[12][i] = m2[4][i] - m2[12][i];
        m1[13][i] = m2[5][i] - m2[13][i];
        m1[14][i] = m2[6][i] - m2[14][i];
        m1[15][i] = m2[7][i] - m2[15][i];
        m2[0][i] = m1[0][i] + m1[4][i];
        m2[1][i] = m1[1][i] + m1[5][i];
        m2[2][i] = m1[2][i] + m1[6][i];
        m2[3][i] = m1[3][i] + m1[7][i];
        m2[4][i] = m1[0][i] - m1[4][i];
        m2[5][i] = m1[1][i] - m1[5][i];
        m2[6][i] = m1[2][i] - m1[6][i];
        m2[7][i] = m1[3][i] - m1[7][i];
        m2[8][i] = m1[8][i] + m1[12][i];
        m2[9][i] = m1[9][i] + m1[13][i];
        m2[10][i] = m1[10][i] + m1[14][i];
        m2[11][i] = m1[11][i] + m1[15][i];
        m2[12][i] = m1[8][i] - m1[12][i];
        m2[13][i] = m1[9][i] - m1[13][i];
        m2[14][i] = m1[10][i] - m1[14][i];
        m2[15][i] = m1[11][i] - m1[15][i];
        m1[0][i] = m2[0][i] + m2[2][i];
        m1[1][i] = m2[1][i] + m2[3][i];
        m1[2][i] = m2[0][i] - m2[2][i];
        m1[3][i] = m2[1][i] - m2[3][i];
        m1[4][i] = m2[4][i] + m2[6][i];
        m1[5][i] = m2[5][i] + m2[7][i];
        m1[6][i] = m2[4][i] - m2[6][i];
        m1[7][i] = m2[5][i] - m2[7][i];
        m1[8][i] = m2[8][i] + m2[10][i];
        m1[9][i] = m2[9][i] + m2[11][i];
        m1[10][i] = m2[8][i] - m2[10][i];
        m1[11][i] = m2[9][i] - m2[11][i];
        m1[12][i] = m2[12][i] + m2[14][i];
        m1[13][i] = m2[13][i] + m2[15][i];
        m1[14][i] = m2[12][i] - m2[14][i];
        m1[15][i] = m2[13][i] - m2[15][i];
        m2[0][i] = m1[0][i] + m1[1][i];
        m2[1][i] = m1[0][i] - m1[1][i];
        m2[2][i] = m1[2][i] + m1[3][i];
        m2[3][i] = m1[2][i] - m1[3][i];
        m2[4][i] = m1[4][i] + m1[5][i];
        m2[5][i] = m1[4][i] - m1[5][i];
        m2[6][i] = m1[6][i] + m1[7][i];
        m2[7][i] = m1[6][i] - m1[7][i];
        m2[8][i] = m1[8][i] + m1[9][i];
        m2[9][i] = m1[8][i] - m1[9][i];
        m2[10][i] = m1[10][i] + m1[11][i];
        m2[11][i] = m1[10][i] - m1[11][i];
        m2[12][i] = m1[12][i] + m1[13][i];
        m2[13][i] = m1[12][i] - m1[13][i];
        m2[14][i] = m1[14][i] + m1[15][i];
        m2[15][i] = m1[14][i] - m1[15][i];
    }
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 8; j++) {
            satd += COM_ABS(m2[i][j]);
        }
    }
    satd = (int)(satd / com_tbl_sqrt[1] * 2.0);
    return satd;
}

u32 com_had_8x4(pel *org, int s_org, pel *cur, int s_cur)
{
    int k, i, j, jj;
    int satd = 0;
    int diff[32], m1[4][8], m2[4][8];
    for (k = 0; k < 32; k += 8) {
        diff[k + 0] = org[0] - cur[0];
        diff[k + 1] = org[1] - cur[1];
        diff[k + 2] = org[2] - cur[2];
        diff[k + 3] = org[3] - cur[3];
        diff[k + 4] = org[4] - cur[4];
        diff[k + 5] = org[5] - cur[5];
        diff[k + 6] = org[6] - cur[6];
        diff[k + 7] = org[7] - cur[7];
        cur += s_cur;
        org += s_org;
    }
    for (j = 0; j < 4; j++) {
        jj = j << 3;
        m2[j][0] = diff[jj] + diff[jj + 4];
        m2[j][1] = diff[jj + 1] + diff[jj + 5];
        m2[j][2] = diff[jj + 2] + diff[jj + 6];
        m2[j][3] = diff[jj + 3] + diff[jj + 7];
        m2[j][4] = diff[jj] - diff[jj + 4];
        m2[j][5] = diff[jj + 1] - diff[jj + 5];
        m2[j][6] = diff[jj + 2] - diff[jj + 6];
        m2[j][7] = diff[jj + 3] - diff[jj + 7];
        m1[j][0] = m2[j][0] + m2[j][2];
        m1[j][1] = m2[j][1] + m2[j][3];
        m1[j][2] = m2[j][0] - m2[j][2];
        m1[j][3] = m2[j][1] - m2[j][3];
        m1[j][4] = m2[j][4] + m2[j][6];
        m1[j][5] = m2[j][5] + m2[j][7];
        m1[j][6] = m2[j][4] - m2[j][6];
        m1[j][7] = m2[j][5] - m2[j][7];
        m2[j][0] = m1[j][0] + m1[j][1];
        m2[j][1] = m1[j][0] - m1[j][1];
        m2[j][2] = m1[j][2] + m1[j][3];
        m2[j][3] = m1[j][2] - m1[j][3];
        m2[j][4] = m1[j][4] + m1[j][5];
        m2[j][5] = m1[j][4] - m1[j][5];
        m2[j][6] = m1[j][6] + m1[j][7];
        m2[j][7] = m1[j][6] - m1[j][7];
    }
    for (i = 0; i < 8; i++) {
        m1[0][i] = m2[0][i] + m2[2][i];
        m1[1][i] = m2[1][i] + m2[3][i];
        m1[2][i] = m2[0][i] - m2[2][i];
        m1[3][i] = m2[1][i] - m2[3][i];
        m2[0][i] = m1[0][i] + m1[1][i];
        m2[1][i] = m1[0][i] - m1[1][i];
        m2[2][i] = m1[2][i] + m1[3][i];
        m2[3][i] = m1[2][i] - m1[3][i];
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            satd += COM_ABS(m2[i][j]);
        }
    }
    satd = (int)(satd / com_tbl_sqrt[0] * 2.0);
    return satd;
}

u32 com_had_4x8(pel *org, int s_org, pel *cur, int s_cur)
{
    int k, i, j, jj;
    int satd = 0;
    int diff[32], m1[8][4], m2[8][4];
    for (k = 0; k < 32; k += 4) {
        diff[k + 0] = org[0] - cur[0];
        diff[k + 1] = org[1] - cur[1];
        diff[k + 2] = org[2] - cur[2];
        diff[k + 3] = org[3] - cur[3];
        cur += s_cur;
        org += s_org;
    }
    for (j = 0; j < 8; j++) {
        jj = j << 2;
        m2[j][0] = diff[jj] + diff[jj + 2];
        m2[j][1] = diff[jj + 1] + diff[jj + 3];
        m2[j][2] = diff[jj] - diff[jj + 2];
        m2[j][3] = diff[jj + 1] - diff[jj + 3];
        m1[j][0] = m2[j][0] + m2[j][1];
        m1[j][1] = m2[j][0] - m2[j][1];
        m1[j][2] = m2[j][2] + m2[j][3];
        m1[j][3] = m2[j][2] - m2[j][3];
    }
    for (i = 0; i < 4; i++) {
        m2[0][i] = m1[0][i] + m1[4][i];
        m2[1][i] = m1[1][i] + m1[5][i];
        m2[2][i] = m1[2][i] + m1[6][i];
        m2[3][i] = m1[3][i] + m1[7][i];
        m2[4][i] = m1[0][i] - m1[4][i];
        m2[5][i] = m1[1][i] - m1[5][i];
        m2[6][i] = m1[2][i] - m1[6][i];
        m2[7][i] = m1[3][i] - m1[7][i];
        m1[0][i] = m2[0][i] + m2[2][i];
        m1[1][i] = m2[1][i] + m2[3][i];
        m1[2][i] = m2[0][i] - m2[2][i];
        m1[3][i] = m2[1][i] - m2[3][i];
        m1[4][i] = m2[4][i] + m2[6][i];
        m1[5][i] = m2[5][i] + m2[7][i];
        m1[6][i] = m2[4][i] - m2[6][i];
        m1[7][i] = m2[5][i] - m2[7][i];
        m2[0][i] = m1[0][i] + m1[1][i];
        m2[1][i] = m1[0][i] - m1[1][i];
        m2[2][i] = m1[2][i] + m1[3][i];
        m2[3][i] = m1[2][i] - m1[3][i];
        m2[4][i] = m1[4][i] + m1[5][i];
        m2[5][i] = m1[4][i] - m1[5][i];
        m2[6][i] = m1[6][i] + m1[7][i];
        m2[7][i] = m1[6][i] - m1[7][i];
    }
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 4; j++) {
            satd += COM_ABS(m2[i][j]);
        }
    }
    satd = (int)(satd / com_tbl_sqrt[0] * 2.0);
    return satd;
}

u32 com_had(int w, int h, pel *org, int s_org, pel *cur, int s_cur, int bit_depth)
{
    int  x, y;
    int sum = 0;
    u32(*satd)(pel *p_org, int i_org, pel *p_pred, int i_pred);
    int shift = bit_depth - 8;

    if (w == h) {
        if (w & 7) { // must be 4x4
            return uavs3e_funs_handle.cost_satd[0][0](org, s_org, cur, s_cur) >> shift;
        } else if (w & 15) { // must be 8x8
            return uavs3e_funs_handle.cost_satd[1][1](org, s_org, cur, s_cur) >> shift;
        } else {
            int  offset_org = s_org << 3;
            int  offset_cur = s_cur << 3;
            satd = uavs3e_funs_handle.cost_satd[1][1];

            for (y = 0; y < h; y += 8) {
                for (x = 0; x < w; x += 8) {
                    sum += satd(&org[x], s_org, &cur[x], s_cur);
                }
                org += offset_org;
                cur += offset_cur;
            }
            return sum >> shift;
        }
    } else if (w > h) {
        if (h == 4) {
            satd = uavs3e_funs_handle.cost_satd[1][0];
            for (x = 0; x < w; x += 8) {
                sum += satd(&org[x], s_org, &cur[x], s_cur);
            }
            return sum >> shift;
        } else if (h & 7) {
            int  offset_org = s_org << 2;
            int  offset_cur = s_cur << 2;
            satd = uavs3e_funs_handle.cost_satd[1][0];

            for (y = 0; y < h; y += 4) {
                for (x = 0; x < w; x += 8) {
                    sum += satd(&org[x], s_org, &cur[x], s_cur);
                }
                org += offset_org;
                cur += offset_cur;
            }
            return sum >> shift;
        } else {
            int  offset_org = s_org << 3;
            int  offset_cur = s_cur << 3;
            satd = uavs3e_funs_handle.cost_satd[2][1];

            for (y = 0; y < h; y += 8) {
                for (x = 0; x < w; x += 16) {
                    sum += satd(&org[x], s_org, &cur[x], s_cur);
                }
                org += offset_org;
                cur += offset_cur;
            }
            return sum >> shift;
        }
    } else {
        if (w == 4) {
            int  offset_org = s_org << 3;
            int  offset_cur = s_cur << 3;
            satd = uavs3e_funs_handle.cost_satd[0][1];

            for (y = 0; y < h; y += 8) {
                sum += satd(org, s_org, cur, s_cur);
                org += offset_org;
                cur += offset_cur;
            }
            return sum >> shift;
        } else if (w & 7) {
            int  offset_org = s_org << 3;
            int  offset_cur = s_cur << 3;
            satd = uavs3e_funs_handle.cost_satd[0][1];

            for (y = 0; y < h; y += 8) {
                sum += satd(org,     s_org, cur,     s_cur);
                sum += satd(org + 4, s_org, cur + 4, s_cur);
                sum += satd(org + 8, s_org, cur + 8, s_cur);
                org += offset_org;
                cur += offset_cur;
            }
            return sum >> shift;
        } else {
            int  offset_org = s_org << 4;
            int  offset_cur = s_cur << 4;
            satd = uavs3e_funs_handle.cost_satd[1][2];

            for (y = 0; y < h; y += 16) {
                for (x = 0; x < w; x += 8) {
                    sum += satd(&org[x], s_org, &cur[x], s_cur);
                }
                org += offset_org;
                cur += offset_cur;
            }
            return sum >> shift;
        }
    }
}

static void ssim_4x4x2_core(const pel *pix1, int stride1, const pel *pix2, int stride2, int sums[2][4])
{
    int x, y, z;
    for (z = 0; z < 2; z++) {
        int s1 = 0, s2 = 0, ss = 0, s12 = 0;
        for (y = 0; y < 4; y++) {
            for (x = 0; x < 4; x++) {
                int a = pix1[x + y * stride1];
                int b = pix2[x + y * stride2];
                s1 += a;
                s2 += b;
                ss += a * a;
                ss += b * b;
                s12 += a * b;
            }
        }
        sums[z][0] = s1;
        sums[z][1] = s2;
        sums[z][2] = ss;
        sums[z][3] = s12;
        pix1 += 4;
        pix2 += 4;
    }
}

static float ssim_end1(int s1, int s2, int ss, int s12, float ssim_c1, float ssim_c2)
{
    float fs1 = (float)s1;
    float fs2 = (float)s2;
    float fss = (float)ss;
    float fs12 = (float)s12;
    float vars = (float)(fss * 64 - fs1 * fs1 - fs2 * fs2);
    float covar = (float)(fs12 * 64 - fs1 * fs2);
    return (float)(2 * fs1 * fs2 + ssim_c1) * (float)(2 * covar + ssim_c2) / ((float)(fs1 * fs1 + fs2 * fs2 + ssim_c1) * (float)(vars + ssim_c2));
}

static float ssim_end4(int sum0[5][4], int sum1[5][4], int width, float ssim_c1, float ssim_c2)
{
    float ssim = 0.0;
    int i;
    for (i = 0; i < width; i++)
        ssim += ssim_end1(sum0[i][0] + sum0[i + 1][0] + sum1[i][0] + sum1[i + 1][0],
            sum0[i][1] + sum0[i + 1][1] + sum1[i][1] + sum1[i + 1][1],
            sum0[i][2] + sum0[i + 1][2] + sum1[i][2] + sum1[i + 1][2],
            sum0[i][3] + sum0[i + 1][3] + sum1[i][3] + sum1[i + 1][3], ssim_c1, ssim_c2);
    return ssim;
}

float com_ssim_img_plane(pel *pix1, int stride1, pel *pix2, int stride2, int width, int height, int *cnt, int bit_depth)
{
#define MAX_PIC_WIDTH (8192*2)

    static int buf[2 * (MAX_PIC_WIDTH / 4 + 3)][4];
    int x, y, z = 0;
    float ssim = 0.0;
    int(*sum0)[4] = buf;
    int(*sum1)[4] = sum0 + (width >> 2) + 3;

    width >>= 2;
    height >>= 2;

    /* Maximum value for 10-bit is: ss*64 = (2^10-1)^2*16*4*64 = 4286582784, which will overflow in some cases.
    * s1*s1, s2*s2, and s1*s2 also obtain this value for edge cases: ((2^10-1)*16*4)^2 = 4286582784.
    * Maximum value for 9-bit is: ss*64 = (2^9-1)^2*16*4*64 = 1069551616, which will not overflow. */
    int pixel_max = bit_depth == 8 ? 255 : 1023;
    float ssim_c1 = (float)(.01 * .01 * pixel_max * pixel_max * 64);
    float ssim_c2 = (float)(.03 * .03 * pixel_max * pixel_max * 64 * 63);

#define XCHG(type,a,b) do{ type t = a; a = b; b = t; } while(0)

    for (y = 1; y < height; y++) {
        for (; z <= y; z++) {
            XCHG(void*, sum0, sum1);
            for (x = 0; x < width; x += 2)
                uavs3e_funs_handle.ssim_4x4x2_core(&pix1[4 * (x + z * stride1)], stride1, &pix2[4 * (x + z * stride2)], stride2, &sum0[x]);
        }
        for (x = 0; x < width - 1; x += 4)
            ssim += uavs3e_funs_handle.ssim_end4(sum0 + x, sum1 + x, COM_MIN(4, width - x - 1), ssim_c1, ssim_c2);
    }
    *cnt = (height - 1) * (width - 1);
    return ssim;
}

void uavs3e_funs_init_cost_c()
{
    uavs3e_funs_handle.cost_sad[0] = com_get_sad_4;
    uavs3e_funs_handle.cost_sad[1] = com_get_sad_8;
    uavs3e_funs_handle.cost_sad[2] = com_get_sad_16;
    uavs3e_funs_handle.cost_sad[3] = com_get_sad_32;
    uavs3e_funs_handle.cost_sad[4] = com_get_sad_64;
    uavs3e_funs_handle.cost_sad[5] = com_get_sad_128;

    uavs3e_funs_handle.cost_sad_x3[0] = com_get_sad_x3_4;
    uavs3e_funs_handle.cost_sad_x3[1] = com_get_sad_x3_8;
    uavs3e_funs_handle.cost_sad_x3[2] = com_get_sad_x3_16;
    uavs3e_funs_handle.cost_sad_x3[3] = com_get_sad_x3_32;
    uavs3e_funs_handle.cost_sad_x3[4] = com_get_sad_x3_64;
    uavs3e_funs_handle.cost_sad_x3[5] = com_get_sad_x3_128;

    uavs3e_funs_handle.cost_sad_x4[0] = com_get_sad_x4_4;
    uavs3e_funs_handle.cost_sad_x4[1] = com_get_sad_x4_8;
    uavs3e_funs_handle.cost_sad_x4[2] = com_get_sad_x4_16;
    uavs3e_funs_handle.cost_sad_x4[3] = com_get_sad_x4_32;
    uavs3e_funs_handle.cost_sad_x4[4] = com_get_sad_x4_64;
    uavs3e_funs_handle.cost_sad_x4[5] = com_get_sad_x4_128;

    uavs3e_funs_handle.cost_ssd[0] = com_get_ssd_4;
    uavs3e_funs_handle.cost_ssd[1] = com_get_ssd_8;
    uavs3e_funs_handle.cost_ssd[2] = com_get_ssd_16;
    uavs3e_funs_handle.cost_ssd[3] = com_get_ssd_32;
    uavs3e_funs_handle.cost_ssd[4] = com_get_ssd_64;
    uavs3e_funs_handle.cost_ssd[5] = com_get_ssd_128;

    uavs3e_funs_handle.cost_satd[0][0] = com_had_4x4;
    uavs3e_funs_handle.cost_satd[1][0] = com_had_8x4;
    uavs3e_funs_handle.cost_satd[0][1] = com_had_4x8;
    uavs3e_funs_handle.cost_satd[1][1] = com_had_8x8;
    uavs3e_funs_handle.cost_satd[2][1] = com_had_16x8;
    uavs3e_funs_handle.cost_satd[1][2] = com_had_8x16;

    uavs3e_funs_handle.cost_var[0] = com_get_var_4;
    uavs3e_funs_handle.cost_var[1] = com_get_var_8;
    uavs3e_funs_handle.cost_var[2] = com_get_var_16;
    uavs3e_funs_handle.cost_var[3] = com_get_var_32;
    uavs3e_funs_handle.cost_var[4] = com_get_var_64;
    uavs3e_funs_handle.cost_var[5] = com_get_var_128;

    uavs3e_funs_handle.ssim_4x4x2_core = ssim_4x4x2_core;
    uavs3e_funs_handle.ssim_end4 = ssim_end4;
}