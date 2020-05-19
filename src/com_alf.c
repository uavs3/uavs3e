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


void com_alf_recon_coef(com_alf_pic_param_t *alfParam, int (*filterCoeff)[ALF_MAX_NUM_COEF])
{
    int g, sum, i, coeffPred;
    for (g = 0; g < alfParam->filters_per_group; g++) {
        sum = 0;
        for (i = 0; i < ALF_MAX_NUM_COEF - 1; i++) {
            sum += (2 * alfParam->coeffmulti[g][i]);
            filterCoeff[g][i] = alfParam->coeffmulti[g][i];
        }
        coeffPred = (1 << ALF_NUM_BIT_SHIFT) - sum;
        filterCoeff[g][ALF_MAX_NUM_COEF - 1] = coeffPred + alfParam->coeffmulti[g][ALF_MAX_NUM_COEF - 1];
    }
}


//(x,y): the pixel used for filtering
//(lcuPosX, lcuPosY): the LCU position within the picture
//startX: x start postion of the current filtering unit
//startY: y start postion of the current filtering unit
//endX: x end postion of the current filtering unit
//endY: y end postion of the current filtering unit
static int com_alf_check_boundary(int x, int y, int lcuPosX, int lcuPosY, int startX, int startY, int endX,
        int endY, int isAboveLeftAvail, int isLeftAvail, int isAboveRightAvail, int isRightAvail)
{
    int modifiedX;
    if (x < lcuPosX) {
        if (y < startY) {
            modifiedX = lcuPosX;
        } else if (y < lcuPosY) {
            if (isAboveLeftAvail) {
                modifiedX = x;
            } else {
                modifiedX = lcuPosX;
            }
        } else if (y <= endY) {
            if (isLeftAvail) {
                modifiedX = x;
            } else {
                modifiedX = lcuPosX;
            }
        } else {
            modifiedX = lcuPosX;
        }
    } else if (x <= endX) {
        modifiedX = x;
    } else {
        // x>endX
        if (y < startY) {
            modifiedX = endX;
        } else if (y < lcuPosY) {
            if (isAboveRightAvail) {
                modifiedX = x;
            } else {
                modifiedX = endX;
            }
        } else if (y <= endY) {
            if (isRightAvail) {
                modifiedX = x;
            } else {
                modifiedX = endX;
            }
        } else {
            modifiedX = endX;
        }
    }
    return modifiedX;
}

void com_alf_check_coef(int *filter, int filterLength)
{
    int i;
    int maxValueNonCenter;
    int minValueNonCenter;
    int maxValueCenter;
    int minValueCenter;
    maxValueNonCenter = 1 * (1 << ALF_NUM_BIT_SHIFT) - 1;
    minValueNonCenter = 0 - 1 * (1 << ALF_NUM_BIT_SHIFT);
    maxValueCenter = 2 * (1 << ALF_NUM_BIT_SHIFT) - 1;
    minValueCenter = 0;
    for (i = 0; i < filterLength - 1; i++) {
        filter[i] = COM_CLIP3(minValueNonCenter, maxValueNonCenter, filter[i]);
    }
    filter[filterLength - 1] = COM_CLIP3(minValueCenter, maxValueCenter, filter[filterLength - 1]);
}
void com_alf_copy_param(com_alf_pic_param_t *dst, com_alf_pic_param_t *src)
{
    int j;
    dst->alf_flag = src->alf_flag;
    dst->componentID = src->componentID;
    dst->filters_per_group = src->filters_per_group;
    switch (src->componentID) {
    case Y_C:
        for (j = 0; j < NO_VAR_BINS; j++) {
            memcpy(dst->coeffmulti[j], src->coeffmulti[j], ALF_MAX_NUM_COEF * sizeof(int));
        }
        memcpy(dst->filterPattern, src->filterPattern, NO_VAR_BINS * sizeof(int));
        break;
    case U_C:
    case V_C:
        for (j = 0; j < 1; j++) {
            memcpy(dst->coeffmulti[j], src->coeffmulti[j], ALF_MAX_NUM_COEF * sizeof(int));
        }
        break;
    default: {
        printf("Not a legal component ID\n");
        assert(0);
        exit(-1);
    }
    }
}

void com_alf_copy_frm(com_pic_t *pic_dst, com_pic_t *pic_src)
{
    int src_Stride = pic_src->stride_luma;
    int dst_Stride = pic_dst->stride_luma;
    pel *src = pic_src->y;
    pel *dst = pic_dst->y;

    for (int j = 0; j < pic_src->height_luma; j++) {
        memcpy(dst, src, pic_src->width_luma * sizeof(pel));
        pel *pl = dst, *pr = dst + pic_src->width_luma - 1;
        pl[-3] = pl[-2] = pl[-1] = pl[0];
        pr[ 3] = pr[ 2] = pr[ 1] = pr[0];
        dst += dst_Stride;
        src += src_Stride;
    }

    src_Stride = pic_src->stride_chroma;
    dst_Stride = pic_dst->stride_chroma;
    src = pic_src->u;
    dst = pic_dst->u;

    for (int j = 0; j < pic_src->height_chroma; j++) {
        memcpy(dst, src, pic_src->width_chroma * sizeof(pel));
        pel *pl = dst, *pr = dst + pic_src->width_chroma - 1;
        pl[-3] = pl[-2] = pl[-1] = pl[0];
        pr[ 3] = pr[ 2] = pr[ 1] = pr[0];
        dst += dst_Stride;
        src += src_Stride;
    }

    src = pic_src->v;
    dst = pic_dst->v;

    for (int j = 0; j < pic_src->height_chroma; j++) {
        memcpy(dst, src, pic_src->width_chroma * sizeof(pel));
        pel *pl = dst, *pr = dst + pic_src->width_chroma - 1;
        pl[-3] = pl[-2] = pl[-1] = pl[0];
        pr[ 3] = pr[ 2] = pr[ 1] = pr[0];
        dst += dst_Stride;
        src += src_Stride;
    }
}

void com_alf_buf_init(com_info_t *info, u8 *alf_var_map)
{
    int lcu_size = info->max_cuwh;
    int regionTable[NO_VAR_BINS] = { 0, 1, 4, 5, 15, 2, 3, 6, 14, 11, 10, 7, 13, 12,  9,  8 };
    int yIndex, xIndex;
    int yIndexOffset;
    int img_width_in_lcu = info->pic_width_in_lcu;
    int img_height_in_lcu = info->pic_height_in_lcu;
    int xInterval = ((img_width_in_lcu + 1) / 4 * lcu_size);
    int yInterval = ((img_height_in_lcu + 1) / 4 * lcu_size);

    for (int i = 0; i < img_height_in_lcu; i++) {
        yIndex = (yInterval == 0) ? (3) : (COM_MIN(3, (i * lcu_size) / yInterval));
        yIndexOffset = yIndex * 4;
        for (int j = 0; j < img_width_in_lcu; j++) {
            xIndex = (xInterval == 0) ? (3) : (COM_MIN(3, (j * lcu_size) / xInterval));
            alf_var_map[j] = regionTable[yIndexOffset + xIndex];
        }
        alf_var_map += img_width_in_lcu;
    }

}

static void alf_filter_block(pel *dst, int i_dst, pel *src, int i_src, int lcu_width, int lcu_height, int *coef, int sample_bit_depth)
{
    pel *imgPad1, *imgPad2, *imgPad3, *imgPad4, *imgPad5, *imgPad6;

    int i, j;
    int max_pel = (1 << sample_bit_depth) - 1;

    for (i = 0; i < lcu_height; i++) {
        imgPad1 = src + i_src;
        imgPad2 = src - i_src;
        imgPad3 = src + 2 * i_src;
        imgPad4 = src - 2 * i_src;
        imgPad5 = src + 3 * i_src;
        imgPad6 = src - 3 * i_src;
        if (i < 3) {
            if (i == 0) {
                imgPad4 = imgPad2 = src;
            } else if (i == 1) {
                imgPad4 = imgPad2;
            }
            imgPad6 = imgPad4;
        } else if (i > lcu_height - 4) {
            if (i == lcu_height - 1) {
                imgPad3 = imgPad1 = src;
            } else if (i == lcu_height - 2) {
                imgPad3 = imgPad1;
            }
            imgPad5 = imgPad3;
        }

        for (j = 0; j < lcu_width; j++) {
            int pixelInt;
            int xLeft = j - 1;
            int xRight = j + 1;

            pixelInt  = coef[0] * (imgPad5[j] + imgPad6[j]);
            pixelInt += coef[1] * (imgPad3[j] + imgPad4[j]);
            pixelInt += coef[2] * (imgPad1[xRight] + imgPad2[xLeft]);
            pixelInt += coef[3] * (imgPad1[j] + imgPad2[j]);
            pixelInt += coef[4] * (imgPad1[xLeft] + imgPad2[xRight]);
            pixelInt += coef[7] * (src[xRight] + src[xLeft]);

            xLeft = j - 2;
            xRight = j + 2;
            pixelInt += coef[6] * (src[xRight] + src[xLeft]);

            xLeft = j - 3;
            xRight = j + 3;
            pixelInt += coef[5] * (src[xRight] + src[xLeft]);

            pixelInt += coef[8] * (src[j]);

            pixelInt = (int)((pixelInt + 32) >> 6);

            dst[j] = COM_CLIP3(0, max_pel, pixelInt);

        }
        src += i_src;
        dst += i_dst;
    }
}

static void alf_filter_block_fix(pel *dst, int i_dst, pel *src, int i_src,  int lcu_width, int lcu_height, int *coef, int sample_bit_depth)
{
    pel *imgPad1, *imgPad2, *imgPad3, *imgPad4, *imgPad5, *imgPad6;
    int pixelInt;
    int max_pel = (1 << sample_bit_depth) - 1;

    if (src[0] != src[-1]) {
        imgPad1 = src + 1 * i_src;
        imgPad2 = src;
        imgPad3 = src + 2 * i_src;
        imgPad4 = src;
        imgPad5 = src + 3 * i_src;
        imgPad6 = src;

        pixelInt  = coef[0] * (imgPad5[ 0] + imgPad6[ 0]);
        pixelInt += coef[1] * (imgPad3[ 0] + imgPad4[ 0]);
        pixelInt += coef[2] * (imgPad1[ 1] + imgPad2[ 0]);
        pixelInt += coef[3] * (imgPad1[ 0] + imgPad2[ 0]);
        pixelInt += coef[4] * (imgPad1[-1] + imgPad2[ 1]);
        pixelInt += coef[7] * (src [ 1] + src [-1]);
        pixelInt += coef[6] * (src [ 2] + src [-2]);
        pixelInt += coef[5] * (src [ 3] + src [-3]);
        pixelInt += coef[8] * (src [ 0]);

        pixelInt = (int)((pixelInt + 32) >> 6);
        dst[0] = COM_CLIP3(0, max_pel, pixelInt);
    }

    src += lcu_width - 1;
    dst += lcu_width - 1;

    if (src[0] != src[1]) {
        imgPad1 = src + 1 * i_src;
        imgPad2 = src;
        imgPad3 = src + 2 * i_src;
        imgPad4 = src;
        imgPad5 = src + 3 * i_src;
        imgPad6 = src;

        pixelInt  = coef[0] * (imgPad5[ 0] + imgPad6[ 0]);
        pixelInt += coef[1] * (imgPad3[ 0] + imgPad4[ 0]);
        pixelInt += coef[2] * (imgPad1[ 1] + imgPad2[-1]);
        pixelInt += coef[3] * (imgPad1[ 0] + imgPad2[ 0]);
        pixelInt += coef[4] * (imgPad1[-1] + imgPad2[ 0]);
        pixelInt += coef[7] * (src [ 1] + src [-1]);
        pixelInt += coef[6] * (src [ 2] + src [-2]);
        pixelInt += coef[5] * (src [ 3] + src [-3]);
        pixelInt += coef[8] * (src [ 0]);

        pixelInt = (int)((pixelInt + 32) >> 6);
        dst[0] = COM_CLIP3(0, max_pel, pixelInt);
    }

    /* last line */
    src -= lcu_width - 1;
    dst -= lcu_width - 1;
    src += ((lcu_height - 1) * i_src);
    dst += ((lcu_height - 1) * i_dst);

    if (src[0] != src[-1]) {
        imgPad1 = src;
        imgPad2 = src - 1 * i_src;
        imgPad3 = src;
        imgPad4 = src - 2 * i_src;
        imgPad5 = src;
        imgPad6 = src - 3 * i_src;

        pixelInt  = coef[0] * (imgPad5[ 0] + imgPad6[ 0]);
        pixelInt += coef[1] * (imgPad3[ 0] + imgPad4[ 0]);
        pixelInt += coef[2] * (imgPad1[ 1] + imgPad2[-1]);
        pixelInt += coef[3] * (imgPad1[ 0] + imgPad2[ 0]);
        pixelInt += coef[4] * (imgPad1[ 0] + imgPad2[ 1]);
        pixelInt += coef[7] * (src [ 1] + src [-1]);
        pixelInt += coef[6] * (src [ 2] + src [-2]);
        pixelInt += coef[5] * (src [ 3] + src [-3]);
        pixelInt += coef[8] * (src [ 0]);

        pixelInt = (int)((pixelInt + 32) >> 6);
        dst[0] = COM_CLIP3(0, max_pel, pixelInt);
    }

    src += lcu_width - 1;
    dst += lcu_width - 1;

    if (src[0] != src[1]) {
        imgPad1 = src;
        imgPad2 = src - 1 * i_src;
        imgPad3 = src;
        imgPad4 = src - 2 * i_src;
        imgPad5 = src;
        imgPad6 = src - 3 * i_src;

        pixelInt  = coef[0] * (imgPad5[ 0] + imgPad6[ 0]);
        pixelInt += coef[1] * (imgPad3[ 0] + imgPad4[ 0]);
        pixelInt += coef[2] * (imgPad1[ 0] + imgPad2[-1]);
        pixelInt += coef[3] * (imgPad1[ 0] + imgPad2[ 0]);
        pixelInt += coef[4] * (imgPad1[-1] + imgPad2[ 1]);
        pixelInt += coef[7] * (src [ 1] + src [-1]);
        pixelInt += coef[6] * (src [ 2] + src [-2]);
        pixelInt += coef[5] * (src [ 3] + src [-3]);
        pixelInt += coef[8] * (src [ 0]);

        pixelInt = (int)((pixelInt + 32) >> 6);
        dst[0] = COM_CLIP3(0, max_pel, pixelInt);
    }
}

static void alf_calc(pel *imgOrg, int i_org, pel *imgPad, int stride, int xPos, int yPos, int width, int height, double eCorr[9][9], double yCorr[9], int isAboveAvail, int isBelowAvail)
{
    int xPosEnd = xPos + width;
    int startPosLuma = isAboveAvail ? (yPos - 4) : yPos;
    int endPosLuma = isBelowAvail ? (yPos + height - 4) : (yPos + height);

    int yUp, yBottom;
    int ELocal[ALF_MAX_NUM_COEF];
    pel *imgPad1, *imgPad2, *imgPad3, *imgPad4, *imgPad5, *imgPad6;
    int i, j, k, l, yLocal;

    imgPad += startPosLuma * stride;
    imgOrg += startPosLuma * i_org;

    for (i = startPosLuma; i < endPosLuma; i++) {
        yUp = COM_CLIP3(startPosLuma, endPosLuma - 1, i - 1);
        yBottom = COM_CLIP3(startPosLuma, endPosLuma - 1, i + 1);
        imgPad1 = imgPad + (yBottom - i) * stride;
        imgPad2 = imgPad + (yUp - i) * stride;

        yUp = COM_CLIP3(startPosLuma, endPosLuma - 1, i - 2);
        yBottom = COM_CLIP3(startPosLuma, endPosLuma - 1, i + 2);
        imgPad3 = imgPad + (yBottom - i) * stride;
        imgPad4 = imgPad + (yUp - i) * stride;

        yUp = COM_CLIP3(startPosLuma, endPosLuma - 1, i - 3);
        yBottom = COM_CLIP3(startPosLuma, endPosLuma - 1, i + 3);
        imgPad5 = imgPad + (yBottom - i) * stride;
        imgPad6 = imgPad + (yUp - i) * stride;

        for (j = xPos; j < xPosEnd; j++) {
            ELocal[0] = (imgPad5[j] + imgPad6[j]);
            ELocal[1] = (imgPad3[j] + imgPad4[j]);
            ELocal[2] = (imgPad1[j + 1] + imgPad2[j - 1]);
            ELocal[3] = (imgPad1[j] + imgPad2[j]);
            ELocal[4] = (imgPad1[j - 1] + imgPad2[j + 1]);
            ELocal[7] = (imgPad[j + 1] + imgPad[j - 1]);
            ELocal[6] = (imgPad[j + 2] + imgPad[j - 2]);
            ELocal[5] = (imgPad[j + 3] + imgPad[j - 3]);
            ELocal[8] = (imgPad[j]);

            yLocal = imgOrg[j];

            for (k = 0; k < ALF_MAX_NUM_COEF; k++) {
                for (l = k; l < ALF_MAX_NUM_COEF; l++) {
                    eCorr[k][l] += (double)(ELocal[k] * ELocal[l]);
                }
                yCorr[k] += (double)(ELocal[k] * yLocal);
            }
        }
        imgPad += stride;
        imgOrg += i_org;
    }

    for (k = 1; k < ALF_MAX_NUM_COEF; k++) {
        for (l = 0; l < k; l++) {
            eCorr[k][l] = eCorr[l][k];
        }
    }

}

void uavs3e_funs_init_alf_c()
{
    uavs3e_funs_handle.alf      = alf_filter_block;
    uavs3e_funs_handle.alf_fix  = alf_filter_block_fix;
    uavs3e_funs_handle.alf_calc = alf_calc;
}
