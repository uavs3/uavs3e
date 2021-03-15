#include "intra-prediction.h"
#include <assert.h>

#if defined(_MSC_VER) 
#pragma warning(disable: 4100)  // unreferenced formal parameter
#endif

static tab_char_t tab_auc_xy_flg[NUM_INTRA_PMODE] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    1, 1, 1, 1, 1,
    1, 1, 1
};

static tab_char_t tab_auc_sign[NUM_INTRA_PMODE] = {
    0, 0, 0, -1, -1,
    -1, -1, -1, -1, -1,
    -1, -1, 0, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 0,
    -1, -1, -1, -1, -1,
    -1, -1, -1
};

static tab_char_t tab_auc_dir_dxdy[2][NUM_INTRA_PMODE][2] = {
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

/////////////////////////////////////////////////////////////////////////////
/// function definition
/////////////////////////////////////////////////////////////////////////////
void xPredIntraVertAdi(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight)
{
    int x, y;
    pel_t *rpSrc = pSrc + 1;

    for (y = 0; y < iHeight; y++) {
        for (x = 0; x < iWidth; x++) {
            dst[x] = rpSrc[x];
        }
        dst += i_dst;
    }
}

void xPredIntraHorAdi(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight)
{
    int x, y;
    pel_t *rpSrc = pSrc - 1;

    for (y = 0; y < iHeight; y++) {
        for (x = 0; x < iWidth; x++) {
            dst[x] = rpSrc[-y];
        }
        dst += i_dst;
    }
}

void xPredIntraDCAdi(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bAboveAvail, int bLeftAvail, int bit_depth)
{
    int   x, y;
    int   iDCValue = 0;
    pel_t  *rpSrc = pSrc - 1;
    int h_bitsize = tab_log2[iHeight];
    int w_bitsize = tab_log2[iWidth];
    int half_height = iHeight / 2;
    int half_width = iWidth / 2;

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

    for (y = 0; y < iHeight; y++) {
        for (x = 0; x < iWidth; x++) {
            dst[x] = (pel_t)iDCValue;
        }
        dst += i_dst;
    }
}

void xPredIntraPlaneAdi(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bit_depth)
{
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

    int iTmp, iTmp2;
    int max_pixel = (1 << bit_depth) - 1;

    pel_t  *rpSrc = pSrc;

    rpSrc = pSrc + 1;
    rpSrc += (iW2 - 1);
    for (x = 1; x < iW2 + 1; x++) {
        iH += x * (rpSrc[x] - rpSrc[-x]);
    }

    rpSrc = pSrc - 1;
    rpSrc -= (iH2 - 1);

    for (y = 1; y < iH2 + 1; y++) {
        iV += y * (rpSrc[-y] - rpSrc[y]);
    }

    rpSrc = pSrc;

    iA = (rpSrc[-1 - (iHeight - 1)] + rpSrc[1 + iWidth - 1]) << 4;
    iB = ((iH << 5) * im_h + (1 << (is_h - 1))) >> is_h;
    iC = ((iV << 5) * im_v + (1 << (is_v - 1))) >> is_v;


    iTmp = iA - (iH2 - 1) * iC - (iW2 - 1) * iB + 16;
    for (y = 0; y < iHeight; y++) {
        iTmp2 = iTmp;
        for (x = 0; x < iWidth; x++) {
            dst[x] = avs3_pixel_clip(iTmp2 >> 5, max_pixel);
            iTmp2 += iB;
        }
        iTmp += iC;
        dst += i_dst;
    }
}

void xPredIntraBiAdi(pel_t *pSrc, pel_t *dst, int i_dst, int iWidth, int iHeight, int bit_depth)
{
    int x, y;
    int ishift_x = tab_log2[iWidth];
    int ishift_y = tab_log2[iHeight];
    int ishift = min(ishift_x, ishift_y);
    int ishift_xy = ishift_x + ishift_y + 1;
    int offset = 1 << (ishift_x + ishift_y);
    int a, b, c, w, tmp, val;
    int max_pixel = (1 << bit_depth) - 1;

    ALIGNED_16(i32s_t pTop [LCU_SIZE]);
    ALIGNED_16(i32s_t pLeft[LCU_SIZE]);
    ALIGNED_16(i32s_t pT   [LCU_SIZE]);
    ALIGNED_16(i32s_t pL   [LCU_SIZE]);
    ALIGNED_16(i32s_t wy   [LCU_SIZE]);

    for (x = 0; x < iWidth; x++) {
        pTop[x] = pSrc[1 + x];
    }
    for (y = 0; y < iHeight; y++) {
        pLeft[y] = pSrc[-1 - y];
    }
    a = pTop[iWidth - 1];
    b = pLeft[iHeight - 1];
    c = (iWidth == iHeight) ? (a + b + 1) >> 1 : (((a << ishift_x) + (b << ishift_y)) * 13 + (1 << (ishift + 5))) >> (ishift + 6);
    w = (c << 1) - a - b;

    for (x = 0; x < iWidth; x++) {
		pT[x] = (i32s_t)(b - pTop[x]);
        pTop[x] <<= ishift_y;
    }
    tmp = 0;
    for (y = 0; y < iHeight; y++) {
		pL[y] = (i32s_t)(a - pLeft[y]);
        pLeft[y] <<= ishift_x;
		wy[y] = (i32s_t)tmp;
        tmp += w;
    }
    for (y = 0; y < iHeight; y++) {
        int add = (pL[y] << ishift_y) + wy[y];
        
        val = (pLeft[y] << ishift_y) + offset + (pL[y] << ishift_y);

        for (x = 0; x < iWidth; x++, val += add) {
            pTop[x] += pT[x];
            dst[x] = avs3_pixel_clip(((pTop[x] << ishift_x) + val) >> ishift_xy, max_pixel);
        }
        dst += i_dst;
    }
}

static __inline int getContextPixel(int uiDirMode, int uiXYflag, int iTempD, int *offset)
{
    int imult  = tab_auc_dir_dxdy[uiXYflag][uiDirMode][0];
    int ishift = tab_auc_dir_dxdy[uiXYflag][uiDirMode][1];
    int iTempDn = iTempD * imult;

    *offset = ((iTempD * imult << 5) >> ishift) - ((iTempDn >> ishift) << 5);

    return iTempDn >> ishift;
}

void xPredIntraAngAdi_X(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    int offset;
    int iWidth2 = iWidth << 1;
    
    for (j = 0; j < iHeight; j++) {
        int c1, c2, c3, c4;
        int real_width;
        int idx = getContextPixel(uiDirMode, 0, j + 1, &offset);

        c1 = 32 - offset;
        c2 = 64 - offset;
        c3 = 32 + offset;
        c4 = offset;

        real_width = min(iWidth, iWidth2 - idx + 1);

        if (real_width <= 0) {
            pel_t val = (pel_t)((pSrc[iWidth2] * c1 + pSrc[iWidth2 + 1] * c2 + pSrc[iWidth2 + 2] * c3 + pSrc[iWidth2 + 3] * c4 + 64) >> 7);
            for (i = 0; i < iWidth; i++) {
                dst[i] = val;
            }
        } else {
            for (i = 0; i < real_width; i++, idx++) {
                dst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
            }
            for (; i < iWidth; i++) {
                dst[i] = dst[real_width - 1];
            }
        }

        dst += i_dst;
    }
}

void xPredIntraAngAdi_X_4(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    pel_t first_line[64 + 128];
    int line_size = iWidth + (iHeight - 1) * 2;
    int real_size = min(line_size, iWidth * 2 - 1);
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
        memcpy(dst, first_line + i, iWidth * sizeof(pel_t));
        dst += i_dst;
    }
}

void xPredIntraAngAdi_X_6(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    pel_t first_line[64 + 64];
    int line_size = iWidth + iHeight - 1;
    int real_size = min(line_size, iWidth * 2);
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
        memcpy(dst, first_line + i, iWidth * sizeof(pel_t));
        dst += i_dst;
    }
}

void xPredIntraAngAdi_X_8(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    pel_t first_line[2 * (64 + 32)];
    int line_size = iWidth + iHeight / 2 - 1;
    int real_size = min(line_size, iWidth * 2 + 1);
    int i;
    int pad1, pad2;
    int aligned_line_size = ((line_size + 15) >> 4) << 4;
    pel_t *pfirst[2];
    pfirst[0] = first_line;
    pfirst[1] = first_line + aligned_line_size;

    for (i = 0; i < real_size; i++, pSrc++) {
        pfirst[0][i] = (pSrc[0] + (pSrc[1] + pSrc[2]) * 3 + pSrc[3] + 4) >> 3;
        pfirst[1][i] = (           pSrc[1] + pSrc[2]  * 2 + pSrc[3] + 2) >> 2;
    }

    // padding
    if (real_size < line_size) {
        pfirst[1][real_size - 1] = pfirst[1][real_size - 2];

        pad1 = pfirst[0][real_size - 1];
        pad2 = pfirst[1][real_size - 1];
        for (; i < line_size; i++) {
            pfirst[0][i] = (pel_t)pad1;
            pfirst[1][i] = (pel_t)pad2;
        }
    }

    iHeight /= 2;

    for (i = 0; i < iHeight; i++) {
        memcpy(dst        , pfirst[0] + i, iWidth * sizeof(pel_t));
        memcpy(dst + i_dst, pfirst[1] + i, iWidth * sizeof(pel_t));
        dst += i_dst * 2;
    }
}

void xPredIntraAngAdi_X_10(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i;
    pel_t *dst1 = dst;
    pel_t *dst2 = dst1 + i_dst;
    pel_t *dst3 = dst2 + i_dst;
    pel_t *dst4 = dst3 + i_dst;

    if (iHeight != 4) {
        pel_t first_line[4 * (64 + 16)];
        int line_size = iWidth + (iHeight >> 2) - 1;
        int aligned_line_size = ((line_size + 15) >> 4) << 4;
        pel_t *pfirst[4];
        pfirst[0] = first_line;
        pfirst[1] = first_line + aligned_line_size;
        pfirst[2] = first_line + aligned_line_size * 2;
        pfirst[3] = first_line + aligned_line_size * 3;

        for (i = 0; i < line_size; i++, pSrc++) {
            pfirst[0][i] = (pSrc[0] * 3 +  pSrc[1] * 7 + pSrc[2]  * 5 + pSrc[3]     + 8) >> 4;
            pfirst[1][i] = (pSrc[0]     + (pSrc[1]     + pSrc[2]) * 3 + pSrc[3]     + 4) >> 3;
            pfirst[2][i] = (pSrc[0]     +  pSrc[1] * 5 + pSrc[2]  * 7 + pSrc[3] * 3 + 8) >> 4;
            pfirst[3][i] = (               pSrc[1]     + pSrc[2]  * 2 + pSrc[3]     + 2) >> 2;
        }

        iHeight /= 4;

        for (i = 0; i < iHeight; i++) {
            memcpy(dst1, pfirst[0] + i, iWidth * sizeof(pel_t));
            memcpy(dst2, pfirst[1] + i, iWidth * sizeof(pel_t));
            memcpy(dst3, pfirst[2] + i, iWidth * sizeof(pel_t));
            memcpy(dst4, pfirst[3] + i, iWidth * sizeof(pel_t));
            dst1 += i_dst * 4;
            dst2 += i_dst * 4;
            dst3 += i_dst * 4;
            dst4 += i_dst * 4;
        }
    } else {
        for (i = 0; i < iWidth; i++, pSrc++) {
            dst1[i] = (pSrc[0] * 3 +  pSrc[1] * 7 + pSrc[2]  * 5 + pSrc[3]     + 8) >> 4;
            dst2[i] = (pSrc[0]     + (pSrc[1]     + pSrc[2]) * 3 + pSrc[3]     + 4) >> 3;
            dst3[i] = (pSrc[0]     +  pSrc[1] * 5 + pSrc[2]  * 7 + pSrc[3] * 3 + 8) >> 4;
            dst4[i] = (               pSrc[1]     + pSrc[2]  * 2 + pSrc[3]     + 2) >> 2;
        }
    }
}

void xPredIntraAngAdi_Y(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    int offset;
    int iY;
    int iHeight2 = iHeight << 1;
    int offsets[64];
    int xsteps[64];
    
    for (i = 0; i < iWidth; i++) {
        xsteps[i] = getContextPixel(uiDirMode, 1, i + 1, &offsets[i]);
    }

    for (j = 0; j < iHeight; j++) {
        for (i = 0; i < iWidth; i++) {
            int idx;
            iY = j + xsteps[i];
            idx = max(-iHeight2, -iY);

            offset = offsets[i];
            dst[i] = (pel_t)((pSrc[idx] * (32 - offset) + pSrc[idx - 1] * (64 - offset) + pSrc[idx - 2] * (32 + offset) + pSrc[idx - 3] * offset + 64) >> 7);
        }

        dst += i_dst;
    }
}

 
#if 0
void xPredIntraAngAdi_Y_25(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    int offset;
    int iY;
    int iHeight2 = iHeight << 1;
    int offsets[64];
    int xsteps[64];
    pel_t dst_tmp[64 * 64];
    pel_t* pDst = dst_tmp;
    pSrc += 144;

    for (j = 0; j < iWidth; j++) {
        int c1, c2, c3, c4;
        int real_width;
        int idx = getContextPixel(uiDirMode, 1, j + 1, &offset);

        c1 = 32 - offset;
        c2 = 64 - offset;
        c3 = 32 + offset;
        c4 = offset;

        for (i = 0; i < iHeight; i++, idx++) {
            pDst[i] = (pel_t)((pSrc[idx] * c1 + pSrc[idx + 1] * c2 + pSrc[idx + 2] * c3 + pSrc[idx + 3] * c4 + 64) >> 7);
        }

        pDst += iHeight;
    }

    for (j = 0; j < iHeight; j++)
    {
        for (i = 0; i < iWidth; i++)
        {
            dst[j * iWidth + i] = dst_tmp[i * iHeight + j];
        }
    }

}
#endif


void xPredIntraAngAdi_Y_26(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i;

    if (iWidth != 4) {
        pel_t first_line[64 + 256];
        int line_size = iWidth + (iHeight - 1) * 4;
        int iHeight4 = iHeight << 2;
        for (i = 0; i < line_size; i += 4, pSrc--) {
            first_line[i    ] = (pSrc[0] * 3 +  pSrc[-1] * 7 + pSrc[-2]  * 5 + pSrc[-3]     + 8) >> 4;
            first_line[i + 1] = (pSrc[0]     + (pSrc[-1]     + pSrc[-2]) * 3 + pSrc[-3]     + 4) >> 3;
            first_line[i + 2] = (pSrc[0]     +  pSrc[-1] * 5 + pSrc[-2]  * 7 + pSrc[-3] * 3 + 8) >> 4;
            first_line[i + 3] = (               pSrc[-1]     + pSrc[-2]  * 2 + pSrc[-3]     + 2) >> 2;
        }

        for (i = 0; i < iHeight4; i += 4) {
            memcpy(dst, first_line + i, iWidth * sizeof(pel_t));
            dst += i_dst;
        }
    } else {
        for (i = 0; i < iHeight; i++, pSrc--) {
            dst[0] = (pSrc[0] * 3 +  pSrc[-1] * 7 + pSrc[-2]  * 5 + pSrc[-3]     + 8) >> 4;
            dst[1] = (pSrc[0]     + (pSrc[-1]     + pSrc[-2]) * 3 + pSrc[-3]     + 4) >> 3;
            dst[2] = (pSrc[0]     +  pSrc[-1] * 5 + pSrc[-2]  * 7 + pSrc[-3] * 3 + 8) >> 4;
            dst[3] = (               pSrc[-1]     + pSrc[-2]  * 2 + pSrc[-3]     + 2) >> 2;
            dst += i_dst;
        }
    }
}

void xPredIntraAngAdi_Y_28(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    pel_t first_line[64 + 128];
    int line_size = iWidth + (iHeight - 1) * 2;
    int real_size = min(line_size, iHeight * 4 + 1);
    int i;
    int iHeight2 = iHeight << 1;
    int pad1, pad2;

    for (i = 0; i < real_size; i += 2, pSrc--) {
        first_line[i    ] = (pSrc[0] + (pSrc[-1] + pSrc[-2]) * 3 + pSrc[-3] + 4) >> 3;
        first_line[i + 1] = (           pSrc[-1] + pSrc[-2]  * 2 + pSrc[-3] + 2) >> 2;
    }

    // padding
    if (real_size < line_size) {
        first_line[i - 1] = first_line[i - 3];
        
        pad1 = first_line[i - 2];
        pad2 = first_line[i - 1];

        for (; i < line_size; i += 2) {
            first_line[i    ] = (pel_t)pad1;
            first_line[i + 1] = (pel_t)pad2;
        }
    }

    for (i = 0; i < iHeight2; i += 2) {
        memcpy(dst, first_line + i, iWidth * sizeof(pel_t));
        dst += i_dst;
    }
}

void xPredIntraAngAdi_Y_30(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    pel_t first_line[64 + 64];
    int line_size = iWidth + iHeight - 1;
    int real_size = min(line_size, iHeight * 2);
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
        memcpy(dst, first_line + i, iWidth * sizeof(pel_t));
        dst += i_dst;
    }
}

void xPredIntraAngAdi_Y_32(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    pel_t first_line[2 * (32 + 64)];
    int line_size = iHeight / 2 + iWidth - 1;
    int real_size = min(line_size, iHeight);
    int i;
    pel_t pad_val;
    int aligned_line_size = ((line_size + 15) >> 4) << 4;
    pel_t *pfirst[2];
    pfirst[0] = first_line;
    pfirst[1] = first_line + aligned_line_size;

    pSrc -= 3;

    for (i = 0; i < real_size; i++, pSrc -= 2) {
        pfirst[0][i] = (pSrc[1] + pSrc[ 0] * 2 + pSrc[-1] + 2) >> 2;
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
        memcpy(dst        , pfirst[0] + i, iWidth * sizeof(pel_t));
        memcpy(dst + i_dst, pfirst[1] + i, iWidth * sizeof(pel_t));
        dst += i_dst * 2;
    }
}

void xPredIntraAngAdi_XY(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel_t *ref_pix;
    char_t coef[64 * 4];
    int offset;
    int xsteps[64];
    char_t *c = coef;
    for (i = 0; i < iWidth; i++, c += 4) {
        xsteps[i] = getContextPixel(uiDirMode, 1, i + 1, &offset);
        c[0] = (char_t)(32 - offset);
        c[1] = (char_t)(64 - offset);
        c[2] = (char_t)(32 + offset);
        c[3] = (char_t)(offset);
    }
    for (j = 0; j < iHeight; j++) {
        pel_t *base_src = pSrc - j;
        c = coef;
        for (i = 0; i < iWidth && xsteps[i] < j + 1; i++, c += 4) {
            ref_pix = base_src + xsteps[i];
            dst[i] = (pel_t)((ref_pix[-2] * c[0] + ref_pix[-1] * c[1] + ref_pix[0] * c[2] + ref_pix[1] * c[3] + 64) >> 7);
        }

        if (i < iWidth) {
            int c1, c2, c3, c4;
            int offsetx;
            ref_pix = pSrc + i - getContextPixel(uiDirMode, 0, j + 1, &offsetx);

            c1 = 32 - offsetx;
            c2 = 64 - offsetx;
            c3 = 32 + offsetx;
            c4 = offsetx;

            for (; i < iWidth; i++, ref_pix++) {
                dst[i] = (pel_t)((ref_pix[2] * c1 + ref_pix[1] * c2 + ref_pix[0] * c3 + ref_pix[-1] * c4 + 64) >> 7);
            }
        }
        
        dst += i_dst;
    }
}

static void xPredIntraAngAdi_XY_13(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel_t *rpSrc = pSrc;
    pel_t *dst_base = dst - 8 * i_dst - 1;
    int step1_height = min(iHeight, 7);

    for (j = 0; j < step1_height; j++) {
        int iTempD = j + 1;
        pel_t *p = pSrc - (iTempD >> 3);
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
        pel_t *px = pSrc + step1_width - (iTempD >> 3);

        step1_width = min(step1_width, iWidth);

        for (i = 0; i < step1_width; i++) {
            pel_t *py = rpSrc - j + ((i + 1) << 3);
            dst[i] = (py[-2] + (py[-1] << 1) + py[0] + 2) >> 2;
        }
        for (; i < iWidth; i++, px++) {
            dst[i] = (px[2] * a + px[1] * b + px[0] * c + px[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }
}

static void xPredIntraAngAdi_XY_14(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i;

    if (iHeight != 4) {
        ALIGNED_16(pel_t first_line[4 * (64 + 16)]);
        int line_size = iWidth + iHeight / 4 - 1;
        int left_size = line_size - iWidth;
        int aligned_line_size = ((line_size + 15) >> 4) << 4;
        pel_t *pfirst[4] = { first_line, first_line + aligned_line_size, first_line + aligned_line_size * 2, first_line + aligned_line_size * 3 };

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
            memcpy(dst, pfirst[0] - i, iWidth * sizeof(pel_t));
            dst += i_dst;
            memcpy(dst, pfirst[1] - i, iWidth * sizeof(pel_t));
            dst += i_dst;
            memcpy(dst, pfirst[2] - i, iWidth * sizeof(pel_t));
            dst += i_dst;
            memcpy(dst, pfirst[3] - i, iWidth * sizeof(pel_t));
            dst += i_dst;
        }
    }
    else {
        pel_t *dst1 = dst;
        pel_t *dst2 = dst1 + i_dst;
        pel_t *dst3 = dst2 + i_dst;
        pel_t *dst4 = dst3 + i_dst;

        for (i = 0; i < iWidth; i++, pSrc++) {
            dst1[i] = (pSrc[-1] + pSrc[0] * 5 + pSrc[1] * 7 + pSrc[2] * 3 + 8) >> 4;
            dst2[i] = (pSrc[-1] + (pSrc[0] + pSrc[1]) * 3 + pSrc[2] + 4) >> 3;
            dst3[i] = (pSrc[-1] * 3 + pSrc[0] * 7 + pSrc[1] * 5 + pSrc[2] + 8) >> 4;
            dst4[i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
        }
    }
}

static void xPredIntraAngAdi_XY_15(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel_t *rpSrc = pSrc;
    pel_t *dst_base = dst - 11 * i_dst - 4;
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
        pel_t *px = pSrc - (iTempD >> 8);
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
        pel_t *px = pSrc - (iTempD >> 8);

        setp1_width = min(setp1_width, iWidth);

        for (i = 0; i < setp1_width; i++) {
            pel_t *py = rpSrc - j + xsteps[i];
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

static void xPredIntraAngAdi_XY_16(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel_t first_line[2 * (64 + 32)]);
    int line_size = iWidth + iHeight / 2 - 1;
    int left_size = line_size - iWidth;
    int aligned_line_size = ((line_size + 15) >> 4) << 4;
    pel_t *pfirst[2] = { first_line, first_line + aligned_line_size };

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
        memcpy(dst, pfirst[0] - i, iWidth * sizeof(pel_t));
        memcpy(dst + i_dst, pfirst[1] - i, iWidth * sizeof(pel_t));
        dst += 2 * i_dst;
    }
}

static void xPredIntraAngAdi_XY_17(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel_t *rpSrc = pSrc;
    pel_t *dst_base = dst - 11 * i_dst - 8;
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
        pel_t *px = pSrc - (iTempD >> 7);
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
        pel_t *px = pSrc - (iTempD >> 7);
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;

        setp1_width = min(setp1_width, iWidth);

        for (i = 0; i < setp1_width; i++) {
            pel_t *py = rpSrc - j + xsteps[i];
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

static void xPredIntraAngAdi_XY_18(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel_t first_line[64 + 64]);
    int line_size = iWidth + iHeight - 1;
    int i;
    pel_t *pfirst = first_line + iHeight - 1;

    pSrc -= iHeight - 1;

    for (i = 0; i < line_size; i++, pSrc++) {
        first_line[i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
    }

    for (i = 0; i < iHeight; i++) {
        memcpy(dst, pfirst, iWidth * sizeof(pel_t));
        pfirst--;
        dst += i_dst;
    }
}

static void xPredIntraAngAdi_XY_19(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel_t *rpSrc = pSrc;
    pel_t *dst_base = dst - 8 * i_dst - 11;
    int xoffsets[64];
    int xsteps[64];
    int iTempD = 93;
    int step2_height = ((93 * iWidth) >> 7);

    for (i = 0; i < iWidth; i++, iTempD += 93) {
        xoffsets[i] = (iTempD >> 2) & 0x1f;
        xsteps[i] = iTempD >> 7;
    }

    iTempD = 11;

    step2_height = min(step2_height, iHeight);

    for (j = 0; j < step2_height; j++, iTempD += 11) {
        int step1_width = (int)(((j + 1) << 7) / 93.0 + 0.9999) - 1;
        int offsetx = (iTempD << 2) & 0x1f;
        pel_t *px = pSrc - (iTempD >> 3);
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;

        step1_width = min(step1_width, iWidth);

        for (i = 0; i < step1_width; i++) {
            pel_t *py = rpSrc - j + xsteps[i];
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
            pel_t *py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }
}

static void xPredIntraAngAdi_XY_20(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel_t first_line[64 + 128]);
    int left_size = (iHeight - 1) * 2 + 1;
    int top_size = iWidth - 1;
    int line_size = left_size + top_size;
    int i;
    pel_t *pfirst = first_line + left_size - 1;

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
        memcpy(dst, pfirst, iWidth * sizeof(pel_t));
        pfirst -= 2;
        dst += i_dst;
    }
}

static void xPredIntraAngAdi_XY_21(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel_t *rpSrc = pSrc;
    pel_t *dst_base = dst - 4 * i_dst - 11;
    int xoffsets[64];
    int xsteps[64];
    int iTempD = 93;
    int step2_height = (93 * iWidth) >> 8;

    for (i = 0; i < iWidth; i++, iTempD += 93) {
        xoffsets[i] = (iTempD >> 3) & 0x1f;
        xsteps[i] = iTempD >> 8;
    }

    iTempD = 11;
    step2_height = min(step2_height, iHeight);

    for (j = 0; j < step2_height; j++, iTempD += 11) {
        int step1_width = (int)(((j + 1) << 8) / 93.0 + 0.9999) - 1;
        int offsetx = (iTempD << 3) & 0x1f;
        pel_t *px = pSrc - (iTempD >> 2);
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;

        step1_width = min(step1_width, iWidth);

        for (i = 0; i < step1_width; i++) {
            pel_t *py = rpSrc - j + xsteps[i];
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
            pel_t *py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }
}

static void xPredIntraAngAdi_XY_22(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i;
    pSrc -= iHeight;

    if (iWidth != 4) {
        ALIGNED_16(pel_t first_line[64 + 256]);
        int left_size = (iHeight - 1) * 4 + 3;
        int top_size = iWidth - 3;
        int line_size = left_size + top_size;
        pel_t *pfirst = first_line + left_size - 3;
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
            memcpy(dst, pfirst, iWidth * sizeof(pel_t));
            dst += i_dst;
            pfirst -= 4;
        }
    }
    else {
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

static void xPredIntraAngAdi_XY_23(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel_t *rpSrc = pSrc;
    pel_t *dst_base = dst - i_dst - 8;
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
        pel_t *px = pSrc - iTempD;

        step1_width = min(step1_width, iWidth);

        for (i = 0; i < step1_width; i++) {
            pel_t *py = rpSrc - j + xsteps[i];
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
            pel_t *py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }
}

       
void intra_pred_pu_luma(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int bAbove, int bLeft, int bs_y, int bs_x, int bit_depth)
{
    switch (uiDirMode) {
    case VERT_PRED:  // Vertical
        g_funs_handle.intra_pred_ver(pSrc, dst, i_dst, bs_x, bs_y);
        break;
    case HOR_PRED:  // Horizontal
        g_funs_handle.intra_pred_hor(pSrc, dst, i_dst, bs_x, bs_y);
        break;
    case DC_PRED:  // DC
        g_funs_handle.intra_pred_dc(pSrc, dst, i_dst, bs_x, bs_y, bAbove, bLeft, bit_depth);
        break;
    case PLANE_PRED:  // Plane
        g_funs_handle.intra_pred_plane(pSrc, dst, i_dst, bs_x, bs_y, bit_depth);
        break;
    case BI_PRED:  // bi
        g_funs_handle.intra_pred_bi(pSrc, dst, i_dst, bs_x, bs_y, bit_depth);
        break;
    default:
        if (uiDirMode <= 11) {
            g_funs_handle.intra_pred_ang_x[uiDirMode - 3](pSrc, dst, i_dst, uiDirMode, bs_x, bs_y);
        } else if (uiDirMode < 25) {
            g_funs_handle.intra_pred_ang_xy[uiDirMode - 13](pSrc, dst, i_dst, uiDirMode, bs_x, bs_y);
        } else {
            g_funs_handle.intra_pred_ang_y[uiDirMode - 25](pSrc, dst, i_dst, uiDirMode, bs_x, bs_y);
        }
        break;
    }
}

void core_intra_get_chroma(pel_t *pSrc, pel_t *dst, int i_dst, int uiDirMode, int uiBitSize, int bAbove, int bLeft, int LumaMode, int bit_depth)
{
    int uiWidth = 1 << uiBitSize;
    int uiHeight = 1 << uiBitSize;

    if (uiDirMode == DM_PRED_C && (LumaMode == VERT_PRED || LumaMode == HOR_PRED || LumaMode == DC_PRED || LumaMode == BI_PRED)) {
        uiDirMode = LumaMode == VERT_PRED ? VERT_PRED_C : (LumaMode == HOR_PRED ? HOR_PRED_C : (LumaMode == DC_PRED ? DC_PRED_C : BI_PRED_C));
    }

    switch (uiDirMode) {
    case DM_PRED_C:  // DM
        switch (LumaMode) {
        case PLANE_PRED:  // Plane
            g_funs_handle.intra_pred_plane(pSrc, dst, i_dst, uiWidth, uiHeight, bit_depth);
            break;
        default:
            if (LumaMode <= 11) {
                g_funs_handle.intra_pred_ang_x[LumaMode - 3](pSrc, dst, i_dst, LumaMode, uiWidth, uiHeight);
            } else if (LumaMode < 25) {
                g_funs_handle.intra_pred_ang_xy[LumaMode - 13](pSrc, dst, i_dst, LumaMode, uiWidth, uiHeight);
            } else {
                g_funs_handle.intra_pred_ang_y[LumaMode - 25](pSrc, dst, i_dst, LumaMode, uiWidth, uiHeight);
            }
            break;
        }
        break;

    case DC_PRED_C:  // DC
        g_funs_handle.intra_pred_dc(pSrc, dst, i_dst, uiWidth, uiHeight, bAbove, bLeft, bit_depth);
        break;
    case HOR_PRED_C:  // Horizontal
        g_funs_handle.intra_pred_hor(pSrc, dst, i_dst, uiWidth, uiHeight);
        break;
    case VERT_PRED_C:  // Vertical
        g_funs_handle.intra_pred_ver(pSrc, dst, i_dst, uiWidth, uiHeight);
        break;
    case BI_PRED_C:  // Bilinear
        g_funs_handle.intra_pred_bi(pSrc, dst, i_dst, uiWidth, uiHeight, bit_depth);
        break;
    default:
        assert(0);
        break;

    }
}


void com_funs_init_intra_pred()
{
    int i;

    g_funs_handle.intra_pred_plane = xPredIntraPlaneAdi;
    g_funs_handle.intra_pred_hor   = xPredIntraHorAdi;
    g_funs_handle.intra_pred_ver   = xPredIntraVertAdi;
    g_funs_handle.intra_pred_dc    = xPredIntraDCAdi;
    g_funs_handle.intra_pred_bi    = xPredIntraBiAdi;

    for (i = 0; i < 9; i++) {
        g_funs_handle.intra_pred_ang_x[i] = xPredIntraAngAdi_X;
    }

    for (i = 0; i < 8; i++) {
        g_funs_handle.intra_pred_ang_y[i] = xPredIntraAngAdi_Y;
    }
    
    for (i = 0; i < 11; i++) {
        g_funs_handle.intra_pred_ang_xy[i] = xPredIntraAngAdi_XY;
    }

    g_funs_handle.intra_pred_ang_x[ 4 - ANG_X_OFFSET] = xPredIntraAngAdi_X_4;
    g_funs_handle.intra_pred_ang_x[ 6 - ANG_X_OFFSET] = xPredIntraAngAdi_X_6;
    g_funs_handle.intra_pred_ang_x[ 8 - ANG_X_OFFSET] = xPredIntraAngAdi_X_8;
    g_funs_handle.intra_pred_ang_x[10 - ANG_X_OFFSET] = xPredIntraAngAdi_X_10;

    g_funs_handle.intra_pred_ang_xy[13 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_13;
    g_funs_handle.intra_pred_ang_xy[15 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_15;
    g_funs_handle.intra_pred_ang_xy[17 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_17;
    g_funs_handle.intra_pred_ang_xy[19 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_19;
    g_funs_handle.intra_pred_ang_xy[21 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_21;
    g_funs_handle.intra_pred_ang_xy[23 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_23;

    g_funs_handle.intra_pred_ang_xy[14 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_14;
    g_funs_handle.intra_pred_ang_xy[16 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_16;
    g_funs_handle.intra_pred_ang_xy[18 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_18;
    g_funs_handle.intra_pred_ang_xy[20 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_20;
    g_funs_handle.intra_pred_ang_xy[22 - ANG_XY_OFFSET] = xPredIntraAngAdi_XY_22;

    g_funs_handle.intra_pred_ang_y[26 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_26;
    g_funs_handle.intra_pred_ang_y[28 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_28;
    g_funs_handle.intra_pred_ang_y[30 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_30;
    g_funs_handle.intra_pred_ang_y[32 - ANG_Y_OFFSET] = xPredIntraAngAdi_Y_32;

}