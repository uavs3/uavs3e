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

#define DEFINE_SAD(w) \
    u32 com_get_sad_##w(pel *p_org, int i_org, pel *p_pred, int i_pred, int height) { \
        int i;                                                                        \
        u32 uiSum = 0;                                                                \
        for (; height != 0; height--) {                                               \
            for (i = 0; i < w; i++) {                                                 \
                uiSum += abs(p_org[i] - p_pred[i]);                                   \
            }                                                                         \
            p_org += i_org;                                                           \
            p_pred += i_pred;                                                         \
        }                                                                             \
        uiSum;                                                                        \
        return uiSum;                                                                 \
    }

DEFINE_SAD(4)
DEFINE_SAD(8)
DEFINE_SAD(16)
DEFINE_SAD(32)
DEFINE_SAD(64)
DEFINE_SAD(128)

#define DEFINE_SSD(w) \
    u64 com_get_ssd_##w(pel *p_org, int i_org, pel *p_pred, int i_pred, int height) { \
        int i;                                                                        \
        u64 uiSum = 0;                                                                \
        for (; height != 0; height--) {                                               \
            for (i = 0; i < w; i++) {                                                 \
                int ssd = abs(p_org[i] - p_pred[i]);                                  \
                uiSum += (ssd * ssd);                                                 \
            }                                                                         \
            p_org += i_org;                                                           \
            p_pred += i_pred;                                                         \
        }                                                                             \
        return uiSum;                                                                 \
    }

DEFINE_SSD(4)
DEFINE_SSD(8)
DEFINE_SSD(16)
DEFINE_SSD(32)
DEFINE_SSD(64)
DEFINE_SSD(128)

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

u32 com_had(int w, int h, void *addr_org, void *addr_curr, int s_org, int s_cur, int bit_depth)
{
    pel *org = addr_org;
    pel *cur = addr_curr;
    int  x, y;
    int sum = 0;
    u32(*satd)(pel *p_org, int i_org, pel *p_pred, int i_pred);

    if (w == h) {
        if (w & 7) { // must be 4x4
            sum = uavs3e_funs_handle.cost_satd[0][0](org, s_org, cur, s_cur);
        } else if (w & 15) { // must be 8x8
            sum = uavs3e_funs_handle.cost_satd[1][1](org, s_org, cur, s_cur);
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
        }
    } else if (w > h) {
        if (h == 4) {
            if (w != 12) {
                satd = uavs3e_funs_handle.cost_satd[1][0];
                for (x = 0; x < w; x += 8) {
                    sum += satd(&org[x], s_org, &cur[x], s_cur);
                }
            } else {
                satd = uavs3e_funs_handle.cost_satd[0][0];
                sum += satd(&org[0], s_org, &cur[0], s_cur);
                sum += satd(&org[4], s_org, &cur[4], s_cur);
                sum += satd(&org[8], s_org, &cur[8], s_cur);
            }
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
        }
    } else {
        if (w == 4) {
            if (h != 12) {
                int  offset_org = s_org << 3;
                int  offset_cur = s_cur << 3;
                satd = uavs3e_funs_handle.cost_satd[0][1];

                for (y = 0; y < h; y += 8) {
                    sum += satd(org, s_org, cur, s_cur);
                    org += offset_org;
                    cur += offset_cur;
                }
            } else {
                int  offset_org = s_org << 2;
                int  offset_cur = s_cur << 2;
                satd = uavs3e_funs_handle.cost_satd[0][0];
                sum += satd(org, s_org, cur, s_cur);
                org += offset_org;
                cur += offset_cur;
                sum += satd(org, s_org, cur, s_cur);
                org += offset_org;
                cur += offset_cur;
                sum += satd(org, s_org, cur, s_cur);
            }
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
        }
    }
    return (sum >> (bit_depth - 8));
}

void uavs3e_funs_init_cost_c()
{
    uavs3e_funs_handle.cost_sad[0] = com_get_sad_4;
    uavs3e_funs_handle.cost_sad[1] = com_get_sad_8;
    uavs3e_funs_handle.cost_sad[2] = com_get_sad_16;
    uavs3e_funs_handle.cost_sad[3] = com_get_sad_32;
    uavs3e_funs_handle.cost_sad[4] = com_get_sad_64;
    uavs3e_funs_handle.cost_sad[5] = com_get_sad_128;

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
}