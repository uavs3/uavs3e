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

void com_intra_get_nbr(int x, int y, int width, int height, pel *src, int s_src, pel *top, int lcu_size, u16 avail_cu, pel nb[N_C][INTRA_NEIB_SIZE], int scup, com_scu_t *map_scu, int i_scu, int bit_depth, int ch_type)
{
    int  i;
    int  width_in_scu  = (ch_type == Y_C) ? (width >> MIN_CU_LOG2)  : (width >> (MIN_CU_LOG2 - 1));
    int  height_in_scu = (ch_type == Y_C) ? (height >> MIN_CU_LOG2) : (height >> (MIN_CU_LOG2 - 1));
    int  unit_size = (ch_type == Y_C) ? MIN_CU_SIZE : (MIN_CU_SIZE >> 1);
    int  x_scu = PEL2SCU(ch_type == Y_C ? x : x << 1);
    int  y_scu = PEL2SCU(ch_type == Y_C ? y : y << 1);
    pel *srcT = (y % lcu_size == 0) ? top : src - s_src;
    pel *srcTL = srcT - 1;
    pel *srcL = src - 1;
    pel *dst = nb[ch_type] + INTRA_NEIB_MID;
    pel *left  = dst - 1;
    pel *up    = dst + 1;

    int pad_le_in_scu = height_in_scu;
    int pad_up_in_scu = width_in_scu;
    int default_val = 1 << (bit_depth - 1);
    int pad_range = COM_MAX(width, height) * 4 + 4;

    if (IS_AVAIL(avail_cu, AVAIL_UP)) {
        com_scu_t *map_up_scu = map_scu + scup - i_scu + width_in_scu;

        com_mcpy(up, srcT, width * sizeof(pel));
        up += width, srcT += width;

        for (i = 0; i < pad_up_in_scu; i++, up += unit_size, srcT += unit_size, map_up_scu++) {
            if (map_up_scu->coded) {
                com_mcpy(up, srcT, unit_size * sizeof(pel));
            } else {
                break;
            }
        }
        com_mset_pel(up, up[-1], pad_range - (width + i * unit_size));
    } else {
        com_mset_pel(up, default_val, pad_range);
    }

    if (IS_AVAIL(avail_cu, AVAIL_LE)) {
        com_scu_t *map_left_scu = map_scu + scup - 1 + height_in_scu * i_scu;
        pel *src = srcL;

        for (i = 0; i < height; ++i) {
            *left-- = *src;
            src += s_src;
        }
        for (i = 0; i < pad_le_in_scu; i++, map_left_scu += i_scu) {
            if (map_left_scu->coded) {
                int j;
                for (j = 0; j < unit_size; ++j) {
                    *left-- = *src;
                    src += s_src;
                }
            } else {
                break;
            }
        }
        int pads = pad_range - height - i * unit_size;
        com_mset_pel(left - pads + 1, left[1], pads);
    } else {
        com_mset_pel(left - pad_range + 1, default_val, pad_range);
    }

    if (IS_AVAIL(avail_cu, AVAIL_UP_LE)) {
        dst[0] = srcTL[0];
    } else if (IS_AVAIL(avail_cu, AVAIL_UP)) {
        dst[0] = srcTL[1];
    } else if (IS_AVAIL(avail_cu, AVAIL_LE)) {
        dst[0] = srcL[0];
    } else {
        dst[0] = default_val;
    }
}

static void ipred_hor(pel *src, pel *dst, int i_dst, int w, int h)
{
    for (int i = 0; i < h; i++) {
        com_mset_pel(dst, src[-i], w);
        dst += i_dst;
    }
}

static void ipred_vert(pel *src, pel *dst, int i_dst, int w, int h)
{
    while (h--) {
        memcpy(dst, src, w * sizeof(pel));
        dst += i_dst;
    }
}

static void ipred_dc(pel *src, pel *dst, int i_dst, int w, int h, u16 avail_cu, int bit_depth)
{
    assert(com_tbl_log2[w] >= 2);
    assert(com_tbl_log2[h] >= 2);

    int dc = 0;
    int i, j;
    if (IS_AVAIL(avail_cu, AVAIL_LE)) {
        for (i = 0; i < h; i++) {
            dc += src[-i-1];
        }
        if (IS_AVAIL(avail_cu, AVAIL_UP)) {
            for (j = 0; j < w; j++) {
                dc += src[j+1];
            }
            dc = (dc + ((w + h) >> 1)) * (4096 / (w + h)) >> 12;
        } else {
            dc = (dc + (h >> 1)) >> com_tbl_log2[h];
        }
    } else if (IS_AVAIL(avail_cu, AVAIL_UP)) {
        for (j = 0; j < w; j++) {
            dc += src[j+1];
        }
        dc = (dc + (w >> 1)) >> com_tbl_log2[w];
    } else {
        dc = 1 << (bit_depth - 1);
    }

    while (h--) {
        com_mset_pel(dst, dc, w);
        dst += i_dst;
    }
}

static void ipred_plane(pel *src, pel *dst, int i_dst, int w, int h, int bit_depth)
{
    assert(com_tbl_log2[w] >= 2);
    assert(com_tbl_log2[h] >= 2);

    pel *rsrc;
    int  coef_h = 0, coef_v = 0;
    int  a, b, c, x, y;
    int  w2 = w >> 1;
    int  h2 = h >> 1;
    int  ib_mult[5]  = { 13, 17, 5, 11, 23 };
    int  ib_shift[5] = { 7, 10, 11, 15, 19 };
    int  idx_w = com_tbl_log2[w] - 2;
    int  idx_h = com_tbl_log2[h] - 2;
    int  im_h, is_h, im_v, is_v, temp, temp2;
    int  max_pel = (1 << bit_depth) - 1;
    int  val;

    im_h = ib_mult[idx_w];
    is_h = ib_shift[idx_w];
    im_v = ib_mult[idx_h];
    is_v = ib_shift[idx_h];
    rsrc = src + w2;

    for (x = 1; x < w2 + 1; x++) {
        coef_h += x * (rsrc[x] - rsrc[-x]);
    }
    rsrc = src - h2;
    for (y = 1; y < h2 + 1; y++) {
        coef_v += y * (rsrc[-y] - rsrc[y]);
    }
    a = (src[-h] + src[w]) << 4;
    b = ((coef_h << 5) * im_h + (1 << (is_h - 1))) >> is_h;
    c = ((coef_v << 5) * im_v + (1 << (is_v - 1))) >> is_v;
    temp = a - (h2 - 1) * c - (w2 - 1) * b + 16;

    for (y = 0; y < h; y++) {
        temp2 = temp;
        for (x = 0; x < w; x++) {
            val = temp2 >> 5;
            dst[x] = (pel)COM_CLIP3(0, max_pel, val);
            temp2 += b;
        }
        temp += c;
        dst += i_dst;
    }
}

static void ipred_plane_ipf(pel *src, s16 *dst, int w, int h)
{
    assert(com_tbl_log2[w] >= 2);
    assert(com_tbl_log2[h] >= 2);

    pel *rsrc;
    int  coef_h = 0, coef_v = 0;
    int  a, b, c, x, y;
    int  w2 = w >> 1;
    int  h2 = h >> 1;
    int  ib_mult[5] = { 13, 17, 5, 11, 23 };
    int  ib_shift[5] = { 7, 10, 11, 15, 19 };
    int  idx_w = com_tbl_log2[w] - 2;
    int  idx_h = com_tbl_log2[h] - 2;
    int  im_h, is_h, im_v, is_v, temp, temp2;
    im_h = ib_mult[idx_w];
    is_h = ib_shift[idx_w];
    im_v = ib_mult[idx_h];
    is_v = ib_shift[idx_h];
    rsrc = src + w2;

    for (x = 1; x < w2 + 1; x++) {
        coef_h += x * (rsrc[x] - rsrc[-x]);
    }
    rsrc = src - h2;
    for (y = 1; y < h2 + 1; y++) {
        coef_v += y * (rsrc[-y] - rsrc[y]);
    }
    a = (src[-h] + src[w]) << 4;
    b = ((coef_h << 5) * im_h + (1 << (is_h - 1))) >> is_h;
    c = ((coef_v << 5) * im_v + (1 << (is_v - 1))) >> is_v;
    temp = a - (h2 - 1) * c - (w2 - 1) * b + 16;

    for (y = 0; y < h; y++) {
        temp2 = temp;
        for (x = 0; x < w; x++) {
            dst[x] = (s16)(temp2 >> 5);
            temp2 += b;
        }
        temp += c;
        dst += w;
    }
}

static void ipred_bi(pel *src, pel *dst, int i_dst, int w, int h, int bit_depth)
{
    assert(com_tbl_log2[w] >= 2);
    assert(com_tbl_log2[h] >= 2);

    int x, y;
    int ishift_x = com_tbl_log2[w];
    int ishift_y = com_tbl_log2[h];
    int ishift = COM_MIN(ishift_x, ishift_y);
    int ishift_xy = ishift_x + ishift_y + 1;
    int offset = 1 << (ishift_x + ishift_y);
    int a, b, c, wt, wxy, tmp;
    int predx;
    int ref_up[MAX_CU_SIZE], ref_le[MAX_CU_SIZE], up[MAX_CU_SIZE], le[MAX_CU_SIZE], wy[MAX_CU_SIZE];
    int wc, tbl_wc[6] = { -1, 21, 13, 7, 4, 2};
    int max_pel = (1 << bit_depth) - 1;
    int val;

    wc = ishift_x > ishift_y ? ishift_x - ishift_y : ishift_y - ishift_x;
    com_assert(wc <= 5);

    wc = tbl_wc[wc];
    for (x = 0; x < w; x++) {
        ref_up[x] = src[x + 1];
    }
    for (y = 0; y < h; y++) {
        ref_le[y] = src[-y - 1];
    }

    a = src[w];
    b = src[-h];
    c = (w == h) ? (a + b + 1) >> 1 : (((a << ishift_x) + (b << ishift_y)) * wc + (1 << (ishift + 5))) >> (ishift + 6);
    wt = (c << 1) - a - b;

    for (x = 0; x < w; x++) {
        up[x] = b - ref_up[x];
        ref_up[x] <<= ishift_y;
    }
    tmp = 0;
    for (y = 0; y < h; y++) {
        le[y] = a - ref_le[y];
        ref_le[y] <<= ishift_x;
        wy[y] = tmp;
        tmp += wt;
    }
    for (y = 0; y < h; y++) {
        predx = ref_le[y];
        wxy = 0;
        for (x = 0; x < w; x++) {
            predx += le[y];
            ref_up[x] += up[x];
            val = ((predx << ishift_y) + (ref_up[x] << ishift_x) + wxy + offset) >> ishift_xy;
            dst[x] = (pel)COM_CLIP3(0, max_pel, val);
            wxy += wy[y];
        }
        dst += i_dst;
    }
}

static void ipred_bi_ipf(pel *src, s16 *dst, int w, int h)
{
    assert(com_tbl_log2[w] >= 2);
    assert(com_tbl_log2[h] >= 2);

    int x, y;
    int ishift_x = com_tbl_log2[w];
    int ishift_y = com_tbl_log2[h];
    int ishift = COM_MIN(ishift_x, ishift_y);
    int ishift_xy = ishift_x + ishift_y + 1;
    int offset = 1 << (ishift_x + ishift_y);
    int a, b, c, wt, wxy, tmp;
    int predx;
    int ref_up[MAX_CU_SIZE], ref_le[MAX_CU_SIZE], up[MAX_CU_SIZE], le[MAX_CU_SIZE], wy[MAX_CU_SIZE];
    int wc, tbl_wc[6] = { -1, 21, 13, 7, 4, 2 };
    wc = ishift_x > ishift_y ? ishift_x - ishift_y : ishift_y - ishift_x;
    com_assert(wc <= 5);

    wc = tbl_wc[wc];
    for (x = 0; x < w; x++) {
        ref_up[x] = src[x + 1];
    }
    for (y = 0; y < h; y++) {
        ref_le[y] = src[-y - 1];
    }

    a = src[w];
    b = src[-h];
    c = (w == h) ? (a + b + 1) >> 1 : (((a << ishift_x) + (b << ishift_y)) * wc + (1 << (ishift + 5))) >> (ishift + 6);
    wt = (c << 1) - a - b;

    for (x = 0; x < w; x++) {
        up[x] = b - ref_up[x];
        ref_up[x] <<= ishift_y;
    }
    tmp = 0;
    for (y = 0; y < h; y++) {
        le[y] = a - ref_le[y];
        ref_le[y] <<= ishift_x;
        wy[y] = tmp;
        tmp += wt;
    }
    for (y = 0; y < h; y++) {
        predx = ref_le[y];
        wxy = 0;
        for (x = 0; x < w; x++) {
            predx += le[y];
            ref_up[x] += up[x];
            dst[x] = (s16)(((predx << ishift_y) + (ref_up[x] << ishift_x) + wxy + offset) >> ishift_xy);
            wxy += wy[y];
        }
        dst += w;
    }
}

static void tspcm_down_sample(int uiCWidth, int uiCHeight, int bitDept, pel *pSrc, int uiSrcStride, pel *pDst, int uiDstStride)
{
    int maxResult = (1 << bitDept) - 1;
    int tempValue;

    for (int j = 0; j < uiCHeight; j++) {
        for (int i = 0; i < uiCWidth; i++) {
            if (i == 0) {
                tempValue = (pSrc[2 * i] + pSrc[2 * i + uiSrcStride] + 1) >> 1;
            } else {
                tempValue = (pSrc[2 * i] * 2 + pSrc[2 * i + 1] + pSrc[2 * i - 1]
                             + pSrc[2 * i + uiSrcStride] * 2
                             + pSrc[2 * i + uiSrcStride + 1]
                             + pSrc[2 * i + uiSrcStride - 1]
                             + 4) >> 3;
            }

            if (tempValue > maxResult || tempValue < 0) {
                printf("\n TSCPM clip error");
            }
            pDst[i] = tempValue;
        }
        pDst += uiDstStride;
        pSrc += uiSrcStride * 2;
    }
}

static pel avs3_always_inline tspcm_get_luma_border(int uiIdx, int bAbovePixel, int uiCWidth, int uiCHeight, int bAboveAvail, int bLeftAvail, pel *luma)
{
    pel *pSrc = NULL;
    pel dstPoint = -1;

    if (bAbovePixel) {
        if (bAboveAvail) {
            pSrc = luma + 1;
            int i = uiIdx << 1;
            if (i < (uiCWidth << 1)) {
                if (i == 0 && !bLeftAvail) {
                    dstPoint = (3 * pSrc[i] + pSrc[i + 1] + 2) >> 2;
                } else {
                    dstPoint = (2 * pSrc[i] + pSrc[i - 1] + pSrc[i + 1] + 2) >> 2;
                }
            }
        }
    } else {
        if (bLeftAvail) {
            pSrc = luma - 1;
            int j = uiIdx << 1;
            if (j < (uiCHeight << 1)) {
                dstPoint = (pSrc[-j] + pSrc[-j - 1] + 1) >> 1;
            }
        }
    }
    com_assert(dstPoint >= 0);
    return dstPoint;
}

#define xGetSrcPixel(idx, bAbovePixel)  tspcm_get_luma_border((idx), (bAbovePixel), uiCWidth, uiCHeight, bAboveAvaillable, bLeftAvaillable, luma)
#define xExchange(a, b, type) {type exTmp; exTmp = (a); (a) = (b); (b) = exTmp;}

static tab_s32 com_tbl_ai_tscpm_div[64] = {
    65536, 32768, 21845, 16384, 13107, 10922, 9362, 8192, 7281, 6553, 5957, 5461, 5041, 4681, 4369, 4096, 3855, 3640,
    3449,  3276,  3120,  2978,  2849,  2730,  2621, 2520, 2427, 2340, 2259, 2184, 2114, 2048, 1985, 1927, 1872, 1820,
    1771,  1724,  1680,  1638,  1598,  1560,  1524, 1489, 1456, 1424, 1394, 1365, 1337, 1310, 1285, 1260, 1236, 1213,
    1191,  1170,  1149,  1129,  1110,  1092,  1074, 1057, 1040, 1024,
};

static void tspcm_get_param(int compID, int *a, int *b, int *iShift, int bAbove, int bLeft, int uiCWidth, int uiCHeight, int bitDept, pel *src, pel *luma)
{
    int bAboveAvaillable = bAbove;
    int bLeftAvaillable = bLeft;
    pel *pCur = NULL;

    unsigned uiInternalBitDepth = bitDept;

    int minDim = bLeftAvaillable && bAboveAvaillable ? COM_MIN(uiCHeight, uiCWidth) : (bLeftAvaillable ? uiCHeight : uiCWidth);
    int numSteps = minDim;
    int yMax = 0;
    int xMax = COM_INT_MIN;
    int yMin = 0;
    int xMin = COM_INT_MAX;

    // four points start
    int iRefPointLuma[4] = { -1, -1, -1, -1 };
    int iRefPointChroma[4] = { -1, -1, -1, -1 };
    int yMax_sec = 0;
    int xMax_sec = COM_INT_MIN;
    int yMin_sec = 0;
    int xMin_sec = COM_INT_MAX;

    if (bAboveAvaillable) {
        pCur = src + 1;
        int idx = ((numSteps - 1) * uiCWidth) / minDim;
        iRefPointLuma[0] = xGetSrcPixel(0, 1); // pSrc[0];
        iRefPointChroma[0] = pCur[0];
        iRefPointLuma[1] = xGetSrcPixel(idx, 1); // pSrc[idx];
        iRefPointChroma[1] = pCur[idx];
        // using 4 points when only one border
        if (!bLeftAvaillable && uiCWidth >= 4) {
            int uiStep = uiCWidth >> 2;
            for (int i = 0; i < 4; i++) {
                iRefPointLuma[i] = xGetSrcPixel(i * uiStep, 1); // pSrc[i * uiStep];
                iRefPointChroma[i] = pCur[i * uiStep];
            }
        }
    }
    if (bLeftAvaillable) {
        pCur = src - 1;
        int idx = ((numSteps - 1) * uiCHeight) / minDim;
        iRefPointLuma[2] = xGetSrcPixel(0, 0); // pSrc[0];
        iRefPointChroma[2] = pCur[0];
        iRefPointLuma[3] = xGetSrcPixel(idx, 0); // pSrc[idx * iSrcStride];
        iRefPointChroma[3] = pCur[-idx];
        // using 4 points when only one border
        if (!bAboveAvaillable && uiCHeight >= 4) {
            int uiStep = uiCHeight >> 2;
            for (int i = 0; i < 4; i++) {
                iRefPointLuma[i] = xGetSrcPixel(i * uiStep, 0); // pSrc[i * uiStep * iSrcStride];
                iRefPointChroma[i] = pCur[-i * uiStep];
            }
        }
    }

    if ((bAboveAvaillable &&  bLeftAvaillable)
        || (bAboveAvaillable && !bLeftAvaillable  && uiCWidth >= 4)
        || (bLeftAvaillable && !bAboveAvaillable && uiCHeight >= 4)) {
        int minGrpIdx[2] = { 0, 2 };
        int maxGrpIdx[2] = { 1, 3 };
        int *tmpMinGrp = minGrpIdx;
        int *tmpMaxGrp = maxGrpIdx;
        if (iRefPointLuma[tmpMinGrp[0]] > iRefPointLuma[tmpMinGrp[1]]) {
            xExchange(tmpMinGrp[0], tmpMinGrp[1], int);
        }
        if (iRefPointLuma[tmpMaxGrp[0]] > iRefPointLuma[tmpMaxGrp[1]]) {
            xExchange(tmpMaxGrp[0], tmpMaxGrp[1], int);
        }
        if (iRefPointLuma[tmpMinGrp[0]] > iRefPointLuma[tmpMaxGrp[1]]) {
            xExchange(tmpMinGrp,    tmpMaxGrp,    int *);
        }
        if (iRefPointLuma[tmpMinGrp[1]] > iRefPointLuma[tmpMaxGrp[0]]) {
            xExchange(tmpMinGrp[1], tmpMaxGrp[0], int);
        }

        assert(iRefPointLuma[tmpMaxGrp[0]] >= iRefPointLuma[tmpMinGrp[0]]);
        assert(iRefPointLuma[tmpMaxGrp[0]] >= iRefPointLuma[tmpMinGrp[1]]);
        assert(iRefPointLuma[tmpMaxGrp[1]] >= iRefPointLuma[tmpMinGrp[0]]);
        assert(iRefPointLuma[tmpMaxGrp[1]] >= iRefPointLuma[tmpMinGrp[1]]);

        xMin = (iRefPointLuma  [tmpMinGrp[0]] + iRefPointLuma  [tmpMinGrp[1]] + 1) >> 1;
        yMin = (iRefPointChroma[tmpMinGrp[0]] + iRefPointChroma[tmpMinGrp[1]] + 1) >> 1;

        xMax = (iRefPointLuma  [tmpMaxGrp[0]] + iRefPointLuma  [tmpMaxGrp[1]] + 1) >> 1;
        yMax = (iRefPointChroma[tmpMaxGrp[0]] + iRefPointChroma[tmpMaxGrp[1]] + 1) >> 1;
    } else if (bAboveAvaillable) {
        for (int k = 0; k < 2; k++) {
            if (iRefPointLuma[k] > xMax) {
                xMax = iRefPointLuma[k];
                yMax = iRefPointChroma[k];
            }
            if (iRefPointLuma[k] < xMin) {
                xMin = iRefPointLuma[k];
                yMin = iRefPointChroma[k];
            }
        }
    } else if (bLeftAvaillable) {
        for (int k = 2; k < 4; k++) {
            if (iRefPointLuma[k] > xMax) {
                xMax = iRefPointLuma[k];
                yMax = iRefPointChroma[k];
            }
            if (iRefPointLuma[k] < xMin) {
                xMin = iRefPointLuma[k];
                yMin = iRefPointChroma[k];
            }
        }
    }
    // four points end

    if (bLeftAvaillable || bAboveAvaillable) {
        *a = 0;
        *iShift = 16;
        int diff = xMax - xMin;
        int add = 0;
        int shift = 0;
        if (diff > 64) {
            shift = (uiInternalBitDepth > 8) ? uiInternalBitDepth - 6 : 2;
            add = shift ? 1 << (shift - 1) : 0;
            diff = (diff + add) >> shift;

            if (uiInternalBitDepth == 10) {
                assert(shift == 4 && add == 8); // for default 10bit
            }
        }

        if (diff > 0) {
            *a = ((yMax - yMin) * com_tbl_ai_tscpm_div[diff - 1] + add) >> shift;
        }
        *b = yMin - (((s64)(*a) * xMin) >> (*iShift));
    }
    if (!bLeftAvaillable && !bAboveAvaillable) {
        *a = 0;
        *b = 1 << (uiInternalBitDepth - 1);
        *iShift = 0;
        return;
    }
}

static void tspcm_linear_transform(pel *pSrc, int iSrcStride, pel *pDst, int iDstStride, int a, int iShift, int b, int uiWidth, int uiHeight, int bitDept)
{
    int maxResult = (1 << bitDept) - 1;

    for (int j = 0; j < uiHeight; j++) {
        for (int i = 0; i < uiWidth; i++) {
            int tempValue = (((s64)a * pSrc[i]) >> (iShift >= 0 ? iShift : 0)) + b;
            pDst[i] = COM_CLIP3(0, maxResult, tempValue);
        }
        pDst += iDstStride;
        pSrc += iSrcStride;
    }
}

static void ipred_tscpm(int compID, pel *piPred, pel *piRecoY, int uiStrideY, int uiWidth, int uiHeight, int bAbove, int bLeft, int bitDept, pel *src, pel *luma_src)
{
    int a, b, shift;   // parameters of Linear Model : a, b, shift
    tspcm_get_param(compID, &a, &b, &shift, bAbove, bLeft, uiWidth, uiHeight, bitDept, src, luma_src);

    pel *lumaRec = piRecoY;
    int lumaRecStride = uiStrideY;
    pel upCbCrRec[MAX_CU_SIZE * MAX_CU_SIZE];
    int upCbCrStride = MAX_CU_SIZE;
    tspcm_linear_transform(lumaRec, lumaRecStride, upCbCrRec, upCbCrStride,
                         a, shift, b, (uiWidth << 1), (uiHeight << 1), bitDept);

    int uiStride = uiWidth;
    tspcm_down_sample(uiWidth, uiHeight, bitDept, upCbCrRec, upCbCrStride, piPred, uiStride);
}

static tab_u8 tbl_ipf_pred_param[5][10] = {
    { 24,  6,  2,  0,  0,  0,  0,  0,  0,  0 }, //4x4, 24, 0.5
    { 44, 25, 14,  8,  4,  2,  1,  1,  0,  0 }, //8x8, 44-1.2
    { 40, 27, 19, 13,  9,  6,  4,  3,  2,  1 }, //16x16, 40-1.8
    { 36, 27, 21, 16, 12,  9,  7,  5,  4,  3 }, //32x32, 36-2.5
    { 52, 44, 37, 31, 26, 22, 18, 15, 13, 11 }, //64x64
};

static void ipf_core(pel *src, pel *dst, int i_dst, int ipm, int w, int h, int bit_depth)
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

    tab_u8 *filter_hori_param = tbl_ipf_pred_param[filter_idx_hor];
    tab_u8 *filter_vert_param = tbl_ipf_pred_param[filter_idx_ver];

    if (IPD_DIA_L <= ipm && ipm <= IPD_DIA_R) { // vertical mode use left reference pixels, don't use top reference
        ver_filter_range = 0;
    }
    if (IPD_DIA_R < ipm) { // horizontal mode use top reference pixels, don't use left reference
        hor_filter_range = 0;
    }

    pel * p_top = src + 1;
    int row;

    for (row = 0; row < ver_filter_range; row++) {
        s32 coeff_top = filter_vert_param[row];
        int col;
        for (col = 0; col < hor_filter_range; col++) {
            s32 coeff_left = filter_hori_param[col];
            s32 coeff_cur = 64 - coeff_left - coeff_top;
            dst[col] = (coeff_left * src[-row - 1] + coeff_top * p_top[col] + coeff_cur * dst[col] + 32) >> 6;
        }
        for (; col < w; col++) {
            s32 coeff_cur = 64 - coeff_top;
            dst[col] = (coeff_top * p_top[col] + coeff_cur * dst[col] + 32) >> 6;
        }
        dst += i_dst;
    }
    for (; row < h; row++) {
        for (s32 col = 0; col < hor_filter_range; col++) {
            s32 coeff_left = filter_hori_param[col];
            s32 coeff_cur = 64 - coeff_left;
            dst[col] = (coeff_left * src[-row - 1] + coeff_cur * dst[col] + 32) >> 6;
        }
        dst += i_dst;
    }
}

static void ipf_core_s16(pel *src, pel *dst, int i_dst, s16 *pred, int ipm, int w, int h, int bit_depth)
{
    s32 filter_idx_hor = (s32)com_tbl_log2[w] - 2; //Block Size
    s32 filter_idx_ver = (s32)com_tbl_log2[h] - 2; //Block Size
    s32 ver_filter_range = COM_MIN(h, 10);
    s32 hor_filter_range = COM_MIN(w, 10);

    int max_val = (1 << bit_depth) - 1;

    // TODO: tbl_ipf_pred_param doesn't support 128
    if (filter_idx_hor > 4) {
        filter_idx_hor = 4;
        hor_filter_range = 0; // don't use IPF at horizontal direction
    }
    if (filter_idx_ver > 4) {
        filter_idx_ver = 4;
        ver_filter_range = 0; // don't use IPF at vertical direction
    }

    tab_u8 *filter_hori_param = tbl_ipf_pred_param[filter_idx_hor];
    tab_u8 *filter_vert_param = tbl_ipf_pred_param[filter_idx_ver];

    if (IPD_DIA_L <= ipm && ipm <= IPD_DIA_R) { // vertical mode use left reference pixels, don't use top reference
        ver_filter_range = 0;
    }
    if (IPD_DIA_R < ipm) { // horizontal mode use top reference pixels, don't use left reference
        hor_filter_range = 0;
    }
    pel *p_top = src + 1;
    int row;

    for (row = 0; row < ver_filter_range; ++row) {
        s32 coeff_top = filter_vert_param[row];
        int col;
        for (col = 0; col < hor_filter_range; col++) {
            s32 coeff_left = filter_hori_param[col];
            s32 coeff_cur = 64 - coeff_left - coeff_top;
            s32 sample_val = (coeff_left * src[-row - 1] + coeff_top * p_top[col] + coeff_cur * pred[col] + 32) >> 6;
            dst[col] = COM_CLIP3(0, max_val, sample_val);
        }
        for (; col < w; col++) {
            s32 coeff_cur = 64 - coeff_top;
            s32 sample_val = (coeff_top * p_top[col] + coeff_cur * pred[col] + 32) >> 6;
            dst[col] = COM_CLIP3(0, max_val, sample_val);
        }
        dst += i_dst;
        pred += w;
    }
    for (; row < h; ++row) {
        int col;
        for (col = 0; col < hor_filter_range; col++) {
            s32 coeff_left = filter_hori_param[col];
            s32 coeff_cur = 64 - coeff_left;
            s32 sample_val = (coeff_left * src[-row - 1] + coeff_cur * pred[col] + 32) >> 6;
            dst[col] = COM_CLIP3(0, max_val, sample_val);
        }
        for (; col < w; col++) {
            dst[col] = COM_CLIP3(0, max_val, pred[col]);
        }
        dst += i_dst;
        pred += w;
    }
}

void com_intra_pred(pel *src, pel *dst, int ipm, int w, int h, int bit_depth, u16 avail_cu, u8 ipf_flag)
{
    assert(w <= 64 && h <= 64);

    if (ipm != IPD_PLN && ipm != IPD_BI) {
        switch (ipm) {
        case IPD_VER:
            uavs3e_funs_handle.intra_pred_ver(src + 1, dst, w, w, h);
            break;
        case IPD_HOR:
            uavs3e_funs_handle.intra_pred_hor(src - 1, dst, w, w, h);
            break;
        case IPD_DC:
            uavs3e_funs_handle.intra_pred_dc(src, dst, w, w, h, avail_cu, bit_depth);
            break;
        default:
            uavs3e_funs_handle.intra_pred_ang[ipm](src, dst, w, ipm, w, h);
            break;
        }
        if (ipf_flag) {
            uavs3e_funs_handle.intra_pred_ipf_core(src, dst, w, ipm, w, h, bit_depth);
        }
    } else {
        if (ipf_flag) {
            ALIGNED_32(s16 tmp_buf[MAX_CU_DIM]);

            switch (ipm) {
            case IPD_PLN:
                uavs3e_funs_handle.intra_pred_plane_ipf(src, (s16 *)tmp_buf, w, h);
                break;
            case IPD_BI:
                uavs3e_funs_handle.intra_pred_bi_ipf(src, (s16 *)tmp_buf, w, h);
                break;
            }
            uavs3e_funs_handle.intra_pred_ipf_core_s16(src, dst, w, (s16 *)tmp_buf, ipm, w, h, bit_depth);
        } else {
            switch (ipm) {
            case IPD_PLN:
                uavs3e_funs_handle.intra_pred_plane(src, dst, w, w, h, bit_depth);
                break;
            case IPD_BI:
                uavs3e_funs_handle.intra_pred_bi(src, dst, w, w, h, bit_depth);
                break;
            }
        }
    }
}

void com_intra_pred_chroma(pel *dst, int ipm_c, int ipm, int w, int h, int bit_depth, u16 avail_cu, int chType,  pel *piRecoY, int uiStrideY, pel nb[N_C][INTRA_NEIB_SIZE])
{
    int bAbove = IS_AVAIL(avail_cu, AVAIL_UP);
    int bLeft = IS_AVAIL(avail_cu, AVAIL_LE);
    pel *src = nb[chType] + INTRA_NEIB_MID;

    if (ipm_c == IPD_DM_C && COM_IPRED_CHK_CONV(ipm)) {
        ipm_c = COM_IPRED_CONV_L2C(ipm);
    }
    switch (ipm_c) {
    case IPD_DM_C:
        switch (ipm) {
        case IPD_PLN:
            uavs3e_funs_handle.intra_pred_plane(src, dst, w, w, h, bit_depth);
            break;
        default:
            uavs3e_funs_handle.intra_pred_ang[ipm](src, dst, w, ipm, w, h);
            break;
        }
        break;
    case IPD_DC_C:
        uavs3e_funs_handle.intra_pred_dc(src, dst, w, w, h, avail_cu, bit_depth);
        break;
    case IPD_HOR_C:
        uavs3e_funs_handle.intra_pred_hor(src - 1, dst, w, w, h);
        break;
    case IPD_VER_C:
        uavs3e_funs_handle.intra_pred_ver(src + 1, dst, w, w, h);
        break;
    case IPD_BI_C:
        uavs3e_funs_handle.intra_pred_bi(src, dst, w, w, h, bit_depth);
        break;
    case IPD_TSCPM_C: {
        ipred_tscpm(chType, dst, piRecoY, uiStrideY, w, h, bAbove, bLeft, bit_depth, src, nb[Y_C] + INTRA_NEIB_MID);
        break;
    }
    default:
        printf("\n illegal chroma intra prediction mode\n");
        break;
    }
}

void com_intra_get_mpm(int x_scu, int y_scu, com_scu_t *map_scu, s8 *map_ipm, int scup, int i_scu, u8 mpm[2])
{
    u8 ipm_l = IPD_DC, ipm_u = IPD_DC;
    int valid_l = 0, valid_u = 0;

    if (COM_IS_INTRA_SCU(map_scu[scup - 1])) {
        ipm_l = map_ipm[scup - 1];
        valid_l = 1;
    }
    if (COM_IS_INTRA_SCU(map_scu[scup - i_scu])) {
        ipm_u = map_ipm[scup - i_scu];
        valid_u = 1;
    }
    mpm[0] = COM_MIN(ipm_l, ipm_u);
    mpm[1] = COM_MAX(ipm_l, ipm_u);
    if (mpm[0] == mpm[1]) {
        mpm[0] = IPD_DC;
        mpm[1] = (mpm[1] == IPD_DC) ? IPD_BI : mpm[1];
    }
}

static void ipred_ang_x(pel *src, pel *dst, int i_dst, int mode, int width, int height)
{
    int i, j;
    int offset;
    int width2 = width << 1;

    for (j = 0; j < height; j++) {
        int c1, c2, c3, c4;
        int idx = getContextPixel(mode, 0, j + 1, &offset);
        pel *p = src + idx;
        int pred_width = COM_MIN(width, width2 - idx + 1);

        c1 = 32 - offset;
        c2 = 64 - offset;
        c3 = 32 + offset;
        c4 = offset;

        for (i = 0; i < pred_width; i++, p++) {
            dst[i] = (p[0] * c1 + p[1] * c2 + p[2] * c3 + p[3] * c4 + 64) >> 7;
        }
        if (pred_width <= 0) {
            dst[0] = (src[width2] * c1 + src[width2 + 1] * c2 + src[width2 + 2] * c3 + src[width2 + 3] * c4 + 64) >> 7;
            pred_width = 1;
        }
        for (; i < width; i++) {
            dst[i] = dst[pred_width - 1];
        }
        dst += i_dst;
    }
}

static void ipred_ang_x_4(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[64 + 128]);
    int line_size = iWidth + (iHeight - 1) * 2;
    int real_size = COM_MIN(line_size, iWidth * 2 - 1);
    int iHeight2 = iHeight * 2;
    int i;

    pSrc += 3;

    for (i = 0; i < real_size; i++, pSrc++) {
        first_line[i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
    }

    // padding
    for (; i < line_size; i++) {
        first_line[i] = first_line[real_size - 1];
    }

    for (i = 0; i < iHeight2; i += 2) {
        memcpy(dst, first_line + i, iWidth * sizeof(pel));
        dst += i_dst;
    }
}

static void ipred_ang_x_6(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[64 + 64]);
    int line_size = iWidth + iHeight - 1;
    int real_size = COM_MIN(line_size, iWidth * 2);
    int i;

    pSrc += 2;

    for (i = 0; i < real_size; i++, pSrc++) {
        first_line[i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
    }

    // padding
    for (; i < line_size; i++) {
        first_line[i] = first_line[real_size - 1];
    }

    for (i = 0; i < iHeight; i++) {
        memcpy(dst, first_line + i, iWidth * sizeof(pel));
        dst += i_dst;
    }
}

static void ipred_ang_x_8(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[2 *(64 + 32)]);
    int line_size = iWidth + iHeight / 2 - 1;
    int real_size = COM_MIN(line_size, iWidth * 2 + 1);
    int i;
    int pad1, pad2;
    int aligned_line_size = ((line_size + 15) >> 4) << 4;
    pel *pfirst[2] = { first_line, first_line + aligned_line_size };

    for (i = 0; i < real_size; i++, pSrc++) {
        pfirst[0][i] = (pSrc[0] + (pSrc[1] + pSrc[2]) * 3 + pSrc[3] + 4) >> 3;
        pfirst[1][i] = (pSrc[1] + pSrc[2] * 2 + pSrc[3] + 2) >> 2;
    }

    // padding
    if (real_size < line_size) {
        pfirst[1][real_size - 1] = pfirst[1][real_size - 2];

        pad1 = pfirst[0][real_size - 1];
        pad2 = pfirst[1][real_size - 1];
        for (; i < line_size; i++) {
            pfirst[0][i] = pad1;
            pfirst[1][i] = pad2;
        }
    }

    iHeight /= 2;

    for (i = 0; i < iHeight; i++) {
        memcpy(dst, pfirst[0] + i, iWidth * sizeof(pel));
        memcpy(dst + i_dst, pfirst[1] + i, iWidth * sizeof(pel));
        dst += i_dst * 2;
    }
}

static void ipred_ang_x_10(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i;
    pel *dst1 = dst;
    pel *dst2 = dst1 + i_dst;
    pel *dst3 = dst2 + i_dst;
    pel *dst4 = dst3 + i_dst;

    if (iHeight != 4) {
        ALIGNED_16(pel first_line[4 *(64 + 16)]);
        int line_size = iWidth + iHeight / 4 - 1;
        int aligned_line_size = ((line_size + 15) >> 4) << 4;
        pel *pfirst[4] = { first_line, first_line + aligned_line_size, first_line + aligned_line_size * 2, first_line + aligned_line_size * 3 };

        for (i = 0; i < line_size; i++, pSrc++) {
            pfirst[0][i] = (pSrc[0] * 3 + pSrc[1] * 7 + pSrc[2] * 5 + pSrc[3] + 8) >> 4;
            pfirst[1][i] = (pSrc[0] + (pSrc[1] + pSrc[2]) * 3 + pSrc[3] + 4) >> 3;
            pfirst[2][i] = (pSrc[0] + pSrc[1] * 5 + pSrc[2] * 7 + pSrc[3] * 3 + 8) >> 4;
            pfirst[3][i] = (pSrc[1] + pSrc[2] * 2 + pSrc[3] + 2) >> 2;
        }

        iHeight /= 4;

        for (i = 0; i < iHeight; i++) {
            memcpy(dst1, pfirst[0] + i, iWidth * sizeof(pel));
            memcpy(dst2, pfirst[1] + i, iWidth * sizeof(pel));
            memcpy(dst3, pfirst[2] + i, iWidth * sizeof(pel));
            memcpy(dst4, pfirst[3] + i, iWidth * sizeof(pel));
            dst1 += i_dst * 4;
            dst2 += i_dst * 4;
            dst3 += i_dst * 4;
            dst4 += i_dst * 4;
        }
    } else {
        for (i = 0; i < iWidth; i++, pSrc++) {
            dst1[i] = (pSrc[0] * 3 + pSrc[1] * 7 + pSrc[2] * 5 + pSrc[3] + 8) >> 4;
            dst2[i] = (pSrc[0] + (pSrc[1] + pSrc[2]) * 3 + pSrc[3] + 4) >> 3;
            dst3[i] = (pSrc[0] + pSrc[1] * 5 + pSrc[2] * 7 + pSrc[3] * 3 + 8) >> 4;
            dst4[i] = (pSrc[1] + pSrc[2] * 2 + pSrc[3] + 2) >> 2;
        }
    }
}

static void xPredIntraAngAdi_Y(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    int offset;
    int offsets[64];
    int xsteps[64];
    int iHeight2 = iHeight << 1;

    for (i = 0; i < iWidth; i++) {
        xsteps[i] = getContextPixel(uiDirMode, 1, i + 1, &offsets[i]);
    }

    for (j = 0; j < iHeight; j++) {
        for (i = 0; i < iWidth; i++) {
            int idx = -j - xsteps[i];
            idx = COM_MAX(-iHeight2, idx);

            offset = offsets[i];
            dst[i] = (pSrc[idx] * (32 - offset) + pSrc[idx - 1] * (64 - offset) + pSrc[idx - 2] * (32 + offset) + pSrc[idx - 3] * offset + 64) >> 7;
        }
        dst += i_dst;
    }
}

static void xPredIntraAngAdi_Y_26(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i;

    if (iWidth != 4) {
        ALIGNED_16(pel first_line[64 + 256]);
        int line_size = iWidth + (iHeight - 1) * 4;
        int iHeight4 = iHeight << 2;
        for (i = 0; i < line_size; i += 4, pSrc--) {
            first_line[i] = (pSrc[0] * 3 + pSrc[-1] * 7 + pSrc[-2] * 5 + pSrc[-3] + 8) >> 4;
            first_line[i + 1] = (pSrc[0] + (pSrc[-1] + pSrc[-2]) * 3 + pSrc[-3] + 4) >> 3;
            first_line[i + 2] = (pSrc[0] + pSrc[-1] * 5 + pSrc[-2] * 7 + pSrc[-3] * 3 + 8) >> 4;
            first_line[i + 3] = (pSrc[-1] + pSrc[-2] * 2 + pSrc[-3] + 2) >> 2;
        }

        for (i = 0; i < iHeight4; i += 4) {
            memcpy(dst, first_line + i, iWidth * sizeof(pel));
            dst += i_dst;
        }
    } else {
        for (i = 0; i < iHeight; i++, pSrc--) {
            dst[0] = (pSrc[0] * 3 + pSrc[-1] * 7 + pSrc[-2] * 5 + pSrc[-3] + 8) >> 4;
            dst[1] = (pSrc[0] + (pSrc[-1] + pSrc[-2]) * 3 + pSrc[-3] + 4) >> 3;
            dst[2] = (pSrc[0] + pSrc[-1] * 5 + pSrc[-2] * 7 + pSrc[-3] * 3 + 8) >> 4;
            dst[3] = (pSrc[-1] + pSrc[-2] * 2 + pSrc[-3] + 2) >> 2;
            dst += i_dst;
        }
    }
}

static void xPredIntraAngAdi_Y_28(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[64 + 128]);
    int line_size = iWidth + (iHeight - 1) * 2;
    int real_size = COM_MIN(line_size, iHeight * 4 + 1);
    int i;
    int iHeight2 = iHeight << 1;
    int pad1, pad2;

    for (i = 0; i < real_size; i += 2, pSrc--) {
        first_line[i] = (pSrc[0] + (pSrc[-1] + pSrc[-2]) * 3 + pSrc[-3] + 4) >> 3;
        first_line[i + 1] = (pSrc[-1] + pSrc[-2] * 2 + pSrc[-3] + 2) >> 2;
    }

    // padding
    if (real_size < line_size) {
        first_line[i - 1] = first_line[i - 3];

        pad1 = first_line[i - 2];
        pad2 = first_line[i - 1];

        for (; i < line_size; i += 2) {
            first_line[i] = pad1;
            first_line[i + 1] = pad2;
        }
    }

    for (i = 0; i < iHeight2; i += 2) {
        memcpy(dst, first_line + i, iWidth * sizeof(pel));
        dst += i_dst;
    }
}

static void xPredIntraAngAdi_Y_30(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[64 + 64]);
    int line_size = iWidth + iHeight - 1;
    int real_size = COM_MIN(line_size, iHeight * 2);
    int i;

    pSrc -= 2;

    for (i = 0; i < real_size; i++, pSrc--) {
        first_line[i] = (pSrc[1] + pSrc[0] * 2 + pSrc[-1] + 2) >> 2;
    }

    // padding
    for (; i < line_size; i++) {
        first_line[i] = first_line[real_size - 1];
    }

    for (i = 0; i < iHeight; i++) {
        memcpy(dst, first_line + i, iWidth * sizeof(pel));
        dst += i_dst;
    }
}

static void xPredIntraAngAdi_Y_32(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[2 *(32 + 64)]);
    int line_size = iHeight / 2 + iWidth - 1;
    int real_size = COM_MIN(line_size, iHeight);
    int i;
    pel pad_val;
    int aligned_line_size = ((line_size + 15) >> 4) << 4;
    pel *pfirst[2] = { first_line, first_line + aligned_line_size };

    pSrc -= 3;

    for (i = 0; i < real_size; i++, pSrc -= 2) {
        pfirst[0][i] = (pSrc[1] + pSrc[0] * 2 + pSrc[-1] + 2) >> 2;
        pfirst[1][i] = (pSrc[0] + pSrc[-1] * 2 + pSrc[-2] + 2) >> 2;
    }

    // padding
    pad_val = pfirst[1][i - 1];
    for (; i < line_size; i++) {
        pfirst[0][i] = pad_val;
        pfirst[1][i] = pad_val;
    }

    iHeight /= 2;
    for (i = 0; i < iHeight; i++) {
        memcpy(dst, pfirst[0] + i, iWidth * sizeof(pel));
        memcpy(dst + i_dst, pfirst[1] + i, iWidth * sizeof(pel));
        dst += i_dst * 2;
    }
}

static void ipred_ang_xy(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    int offsetx, offsety;
    pel *rpSrc = pSrc;
    int xoffsets[64];
    int xsteps[64];

    for (i = 0; i < iWidth; i++) {
        xsteps[i] = getContextPixel(uiDirMode, 1, i + 1, &xoffsets[i]);
    }

    for (j = 0; j < iHeight; j++) {
        pel *px = pSrc - getContextPixel(uiDirMode, 0, j + 1, &offsetx);

        for (i = 0; i < iWidth; i++, px++) {
            int iYy = j - xsteps[i];
            if (iYy <= -1) {
                dst[i] = (px[2] * (32 - offsetx) + px[1] * (64 - offsetx) + px[0] * (32 + offsetx) + px[-1] * offsetx + 64) >> 7;
            } else {
                offsety = xoffsets[i];
                dst[i] = (rpSrc[-iYy - 2] * (32 - offsety) + rpSrc[-iYy - 1] * (64 - offsety) + rpSrc[-iYy] * (32 + offsety) + rpSrc[-iYy + 1] * offsety + 64) >> 7;
            }
        }
        dst += i_dst;
    }
}

static void ipred_ang_xy_13(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel *rpSrc = pSrc;
    pel *dst_base = dst - 8 * i_dst - 1;
    int step1_height = COM_MIN(iHeight, 7);

    for (j = 0; j < step1_height; j++) {
        int iTempD = j + 1;
        pel *p = pSrc - (iTempD >> 3);
        int offsetx = (iTempD << 2) & 0x1f;
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;

        for (i = 0; i < iWidth; i++, p++) {
            dst[i] = (p[2] * a + p[1] * b + p[0] * c + p[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }

    for (; j < iHeight; j++) {
        int iTempD = j + 1;
        int step1_width = (int)(((j + 1) / 8.0 + 0.9999)) - 1;
        int offsetx = (iTempD << 2) & 0x1f;
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;
        pel *px = pSrc + step1_width - (iTempD >> 3);

        step1_width = COM_MIN(step1_width, iWidth);

        for (i = 0; i < step1_width; i++) {
            pel *py = rpSrc - j + ((i + 1) << 3);
            dst[i] = (py[-2] + (py[-1] << 1) + py[0] + 2) >> 2;
        }
        for (; i < iWidth; i++, px++) {
            dst[i] = (px[2] * a + px[1] * b + px[0] * c + px[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }
}

static void ipred_ang_xy_14(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i;

    if (iHeight != 4) {
        ALIGNED_16(pel first_line[4 *(64 + 16)]);
        int line_size = iWidth + iHeight / 4 - 1;
        int left_size = line_size - iWidth;
        int aligned_line_size = ((line_size + 15) >> 4) << 4;
        pel *pfirst[4] = { first_line, first_line + aligned_line_size, first_line + aligned_line_size * 2, first_line + aligned_line_size * 3 };

        pSrc -= iHeight - 4;
        for (i = 0; i < left_size; i++, pSrc += 4) {
            pfirst[0][i] = (pSrc[2] + pSrc[3] * 2 + pSrc[4] + 2) >> 2;
            pfirst[1][i] = (pSrc[1] + pSrc[2] * 2 + pSrc[3] + 2) >> 2;
            pfirst[2][i] = (pSrc[0] + pSrc[1] * 2 + pSrc[2] + 2) >> 2;
            pfirst[3][i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
        }

        for (; i < line_size; i++, pSrc++) {
            pfirst[0][i] = (pSrc[-1] + pSrc[0] * 5 + pSrc[1] * 7 + pSrc[2] * 3 + 8) >> 4;
            pfirst[1][i] = (pSrc[-1] + (pSrc[0] + pSrc[1]) * 3 + pSrc[2] + 4) >> 3;
            pfirst[2][i] = (pSrc[-1] * 3 + pSrc[0] * 7 + pSrc[1] * 5 + pSrc[2] + 8) >> 4;
            pfirst[3][i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
        }

        pfirst[0] += left_size;
        pfirst[1] += left_size;
        pfirst[2] += left_size;
        pfirst[3] += left_size;

        iHeight /= 4;

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
    } else {
        pel *dst1 = dst;
        pel *dst2 = dst1 + i_dst;
        pel *dst3 = dst2 + i_dst;
        pel *dst4 = dst3 + i_dst;

        for (i = 0; i < iWidth; i++, pSrc++) {
            dst1[i] = (pSrc[-1] + pSrc[0] * 5 + pSrc[1] * 7 + pSrc[2] * 3 + 8) >> 4;
            dst2[i] = (pSrc[-1] + (pSrc[0] + pSrc[1]) * 3 + pSrc[2] + 4) >> 3;
            dst3[i] = (pSrc[-1] * 3 + pSrc[0] * 7 + pSrc[1] * 5 + pSrc[2] + 8) >> 4;
            dst4[i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
        }
    }
}

static void ipred_ang_xy_15(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel *rpSrc = pSrc;
    pel *dst_base = dst - 11 * i_dst - 4;
    int xoffsets[64];
    int xsteps[64];
    int iTempD = 11;

    for (i = 0; i < iWidth; i++, iTempD += 11) {
        xoffsets[i] = (iTempD << 3) & 0x1f;
        xsteps[i] = (iTempD >> 2);
    }

    iTempD = 93;

    for (j = 0; j < 2; j++, iTempD += 93) {
        int offsetx = (iTempD >> 3) & 0x1f;
        pel *px = pSrc - (iTempD >> 8);
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;
        for (i = 0; i < iWidth; i++, px++) {
            dst[i] = (px[2] * a + px[1] * b + px[0] * c + px[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }

    for (; j < iHeight; j++, iTempD += 93) {
        int setp1_width = (int)(((j + 1) << 2) / 11.0 + 0.9999) - 1;
        int offsetx = (iTempD >> 3) & 0x1f;
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;
        pel *px = pSrc - (iTempD >> 8);

        setp1_width = COM_MIN(setp1_width, iWidth);

        for (i = 0; i < setp1_width; i++) {
            pel *py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }

        px += i;

        for (; i < iWidth; i++, px++) {
            dst[i] = (px[2] * a + px[1] * b + px[0] * c + px[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }
}

static void ipred_ang_xy_16(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[2 *(64 + 32)]);
    int line_size = iWidth + iHeight / 2 - 1;
    int left_size = line_size - iWidth;
    int aligned_line_size = ((line_size + 15) >> 4) << 4;
    pel *pfirst[2] = { first_line, first_line + aligned_line_size };

    int i;

    pSrc -= iHeight - 2;

    for (i = 0; i < left_size; i++, pSrc += 2) {
        pfirst[0][i] = (pSrc[0] + pSrc[1] * 2 + pSrc[2] + 2) >> 2;
        pfirst[1][i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
    }

    for (; i < line_size; i++, pSrc++) {
        pfirst[0][i] = (pSrc[-1] + (pSrc[0] + pSrc[1]) * 3 + pSrc[2] + 4) >> 3;
        pfirst[1][i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
    }

    pfirst[0] += left_size;
    pfirst[1] += left_size;

    iHeight /= 2;

    for (i = 0; i < iHeight; i++) {
        memcpy(dst, pfirst[0] - i, iWidth * sizeof(pel));
        memcpy(dst + i_dst, pfirst[1] - i, iWidth * sizeof(pel));
        dst += 2 * i_dst;
    }
}

static void ipred_ang_xy_17(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel *rpSrc = pSrc;
    pel *dst_base = dst - 11 * i_dst - 8;
    int xoffsets[64];
    int xsteps[64];
    int iTempD = 11;

    for (i = 0; i < iWidth; i++, iTempD += 11) {
        xoffsets[i] = (iTempD << 2) & 0x1f;
        xsteps[i] = iTempD >> 3;
    }

    iTempD = 93;

    {
        int offsetx = (iTempD >> 2) & 0x1f;
        pel *px = pSrc - (iTempD >> 7);
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;
        for (i = 0; i < iWidth; i++, px++) {
            dst[i] = (px[2] * a + px[1] * b + px[0] * c + px[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
        iTempD += 93;
    }

    for (j = 1; j < iHeight; j++, iTempD += 93) {
        int setp1_width = (int)(((j + 1) << 3) / 11.0 + 0.9999) - 1;
        int offsetx = (iTempD >> 2) & 0x1f;
        pel *px = pSrc - (iTempD >> 7);
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;

        setp1_width = COM_MIN(setp1_width, iWidth);

        for (i = 0; i < setp1_width; i++) {
            pel *py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }
        px += i;
        for (; i < iWidth; i++, px++) {
            dst[i] = (px[2] * a + px[1] * b + px[0] * c + px[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }
}

static void ipred_ang_xy_18(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[64 + 64]);
    int line_size = iWidth + iHeight - 1;
    int i;
    pel *pfirst = first_line + iHeight - 1;

    pSrc -= iHeight - 1;

    for (i = 0; i < line_size; i++, pSrc++) {
        first_line[i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
    }

    for (i = 0; i < iHeight; i++) {
        memcpy(dst, pfirst, iWidth * sizeof(pel));
        pfirst--;
        dst += i_dst;
    }
}

static void ipred_ang_xy_19(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel *rpSrc = pSrc;
    pel *dst_base = dst - 8 * i_dst - 11;
    int xoffsets[64];
    int xsteps[64];
    int iTempD = 93;
    int step2_height = ((93 * iWidth) >> 7);

    for (i = 0; i < iWidth; i++, iTempD += 93) {
        xoffsets[i] = (iTempD >> 2) & 0x1f;
        xsteps[i] = iTempD >> 7;
    }

    iTempD = 11;

    step2_height = COM_MIN(step2_height, iHeight);

    for (j = 0; j < step2_height; j++, iTempD += 11) {
        int step1_width = (int)(((j + 1) << 7) / 93.0 + 0.9999) - 1;
        int offsetx = (iTempD << 2) & 0x1f;
        pel *px = pSrc - (iTempD >> 3);
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;

        step1_width = COM_MIN(step1_width, iWidth);

        for (i = 0; i < step1_width; i++) {
            pel *py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }
        px += i;

        for (; i < iWidth; i++, px++) {
            dst[i] = (px[2] * a + px[1] * b + px[0] * c + px[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }

    for (; j < iHeight; j++) {
        for (i = 0; i < iWidth; i++) {
            pel *py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }
}

static void ipred_ang_xy_20(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[64 + 128]);
    int left_size = (iHeight - 1) * 2 + 1;
    int top_size = iWidth - 1;
    int line_size = left_size + top_size;
    int i;
    pel *pfirst = first_line + left_size - 1;

    pSrc -= iHeight;

    for (i = 0; i < left_size; i += 2, pSrc++) {
        first_line[i] = (pSrc[-1] + (pSrc[0] + pSrc[1]) * 3 + pSrc[2] + 4) >> 3;
        first_line[i + 1] = (pSrc[0] + pSrc[1] * 2 + pSrc[2] + 2) >> 2;
    }
    i--;

    for (; i < line_size; i++, pSrc++) {
        first_line[i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
    }

    for (i = 0; i < iHeight; i++) {
        memcpy(dst, pfirst, iWidth * sizeof(pel));
        pfirst -= 2;
        dst += i_dst;
    }
}

static void ipred_ang_xy_21(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel *rpSrc = pSrc;
    pel *dst_base = dst - 4 * i_dst - 11;
    int xoffsets[64];
    int xsteps[64];
    int iTempD = 93;
    int step2_height = (93 * iWidth) >> 8;

    for (i = 0; i < iWidth; i++, iTempD += 93) {
        xoffsets[i] = (iTempD >> 3) & 0x1f;
        xsteps[i] = iTempD >> 8;
    }

    iTempD = 11;
    step2_height = COM_MIN(step2_height, iHeight);

    for (j = 0; j < step2_height; j++, iTempD += 11) {
        int step1_width = (int)(((j + 1) << 8) / 93.0 + 0.9999) - 1;
        int offsetx = (iTempD << 3) & 0x1f;
        pel *px = pSrc - (iTempD >> 2);
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;

        step1_width = COM_MIN(step1_width, iWidth);

        for (i = 0; i < step1_width; i++) {
            pel *py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }

        px += i;

        for (; i < iWidth; i++, px++) {
            dst[i] = (px[2] * a + px[1] * b + px[0] * c + px[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }

    for (; j < iHeight; j++) {
        for (i = 0; i < iWidth; i++) {
            pel *py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }
}

static void ipred_ang_xy_22(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i;
    pSrc -= iHeight;

    if (iWidth != 4) {
        ALIGNED_16(pel first_line[64 + 256]);
        int left_size = (iHeight - 1) * 4 + 3;
        int top_size = iWidth - 3;
        int line_size = left_size + top_size;
        pel *pfirst = first_line + left_size - 3;
        for (i = 0; i < left_size; i += 4, pSrc++) {
            first_line[i] = (pSrc[-1] * 3 + pSrc[0] * 7 + pSrc[1] * 5 + pSrc[2] + 8) >> 4;
            first_line[i + 1] = (pSrc[-1] + (pSrc[0] + pSrc[1]) * 3 + pSrc[2] + 4) >> 3;
            first_line[i + 2] = (pSrc[-1] + pSrc[0] * 5 + pSrc[1] * 7 + pSrc[2] * 3 + 8) >> 4;
            first_line[i + 3] = (pSrc[0] + pSrc[1] * 2 + pSrc[2] + 2) >> 2;
        }
        i--;

        for (; i < line_size; i++, pSrc++) {
            first_line[i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
        }

        for (i = 0; i < iHeight; i++) {
            memcpy(dst, pfirst, iWidth * sizeof(pel));
            dst += i_dst;
            pfirst -= 4;
        }
    } else {
        dst += (iHeight - 1) * i_dst;
        for (i = 0; i < iHeight; i++, pSrc++) {
            dst[0] = (pSrc[-1] * 3 + pSrc[0] * 7 + pSrc[1] * 5 + pSrc[2] + 8) >> 4;
            dst[1] = (pSrc[-1] + (pSrc[0] + pSrc[1]) * 3 + pSrc[2] + 4) >> 3;
            dst[2] = (pSrc[-1] + pSrc[0] * 5 + pSrc[1] * 7 + pSrc[2] * 3 + 8) >> 4;
            dst[3] = (pSrc[0] + pSrc[1] * 2 + pSrc[2] + 2) >> 2;
            dst -= i_dst;
        }
    }
}

static void ipred_ang_xy_23(pel *pSrc, pel *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel *rpSrc = pSrc;
    pel *dst_base = dst - i_dst - 8;
    int xoffsets[64];
    int xsteps[64];
    int iTempD = 1;
    int step2_height = (iWidth >> 3);

    for (i = 0; i < iWidth; i++, iTempD++) {
        xoffsets[i] = (iTempD << 2) & 0x1f;
        xsteps[i] = iTempD >> 3;
    }

    iTempD = 8;

    for (j = 0; j < step2_height; j++, iTempD += 8) {
        int step1_width = ((j + 1) << 3) - 1;
        pel *px = pSrc - iTempD;

        step1_width = COM_MIN(step1_width, iWidth);

        for (i = 0; i < step1_width; i++) {
            pel *py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }
        px += i;
        for (; i < iWidth; i++, px++) {
            dst[i] = (px[2] + (px[1] << 1) + px[0] + 2) >> 2;
        }
        dst_base += i_dst;
        dst += i_dst;
    }

    for (; j < iHeight; j++) {
        for (i = 0; i < iWidth; i++) {
            pel *py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }
}

void uavs3e_funs_init_intra_pred_c()
{
    int i;

    for (i = IPD_BI + 1; i < IPD_VER; i++) {
        uavs3e_funs_handle.intra_pred_ang[i] = ipred_ang_x;
    }
    for (i = IPD_VER; i < IPD_HOR; i++) {
        uavs3e_funs_handle.intra_pred_ang[i] = ipred_ang_xy;
    }
    for (i = IPD_HOR + 1; i < IPD_CNT; i++) {
        uavs3e_funs_handle.intra_pred_ang[i] = xPredIntraAngAdi_Y;
    }

    uavs3e_funs_handle.intra_pred_ang[4] = ipred_ang_x_4;
    uavs3e_funs_handle.intra_pred_ang[6] = ipred_ang_x_6;
    uavs3e_funs_handle.intra_pred_ang[8] = ipred_ang_x_8;
    uavs3e_funs_handle.intra_pred_ang[10] = ipred_ang_x_10;

    uavs3e_funs_handle.intra_pred_ang[13] = ipred_ang_xy_13;
    uavs3e_funs_handle.intra_pred_ang[15] = ipred_ang_xy_15;
    uavs3e_funs_handle.intra_pred_ang[17] = ipred_ang_xy_17;
    uavs3e_funs_handle.intra_pred_ang[19] = ipred_ang_xy_19;
    uavs3e_funs_handle.intra_pred_ang[21] = ipred_ang_xy_21;
    uavs3e_funs_handle.intra_pred_ang[23] = ipred_ang_xy_23;

    uavs3e_funs_handle.intra_pred_ang[14] = ipred_ang_xy_14;
    uavs3e_funs_handle.intra_pred_ang[16] = ipred_ang_xy_16;
    uavs3e_funs_handle.intra_pred_ang[18] = ipred_ang_xy_18;
    uavs3e_funs_handle.intra_pred_ang[20] = ipred_ang_xy_20;
    uavs3e_funs_handle.intra_pred_ang[22] = ipred_ang_xy_22;

    uavs3e_funs_handle.intra_pred_ang[26] = xPredIntraAngAdi_Y_26;
    uavs3e_funs_handle.intra_pred_ang[28] = xPredIntraAngAdi_Y_28;
    uavs3e_funs_handle.intra_pred_ang[30] = xPredIntraAngAdi_Y_30;
    uavs3e_funs_handle.intra_pred_ang[32] = xPredIntraAngAdi_Y_32;

    uavs3e_funs_handle.intra_pred_dc        = ipred_dc;
    uavs3e_funs_handle.intra_pred_bi        = ipred_bi;
    uavs3e_funs_handle.intra_pred_plane     = ipred_plane;
    uavs3e_funs_handle.intra_pred_hor       = ipred_hor;
    uavs3e_funs_handle.intra_pred_ver       = ipred_vert;
    uavs3e_funs_handle.intra_pred_bi_ipf    = ipred_bi_ipf;
    uavs3e_funs_handle.intra_pred_plane_ipf = ipred_plane_ipf;
    uavs3e_funs_handle.intra_pred_ipf_core  = ipf_core;
    uavs3e_funs_handle.intra_pred_ipf_core_s16 = ipf_core_s16;
}
