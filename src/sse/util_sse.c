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


#define CALC_EQUAL_COEFF_8PXLS(x1,x2,y1,y2,tmp0,tmp1,tmp2,tmp3,dst)     \
    {                                                                                               \
        mm_intermediate[0] = _mm_mul_epi32(x1, y1);                                                 \
        mm_intermediate[1] = _mm_mul_epi32(tmp0, tmp2);                                             \
        mm_intermediate[2] = _mm_mul_epi32(x2, y2);                                                 \
        mm_intermediate[3] = _mm_mul_epi32(tmp1, tmp3);                                             \
        mm_intermediate[0] = _mm_add_epi64(mm_intermediate[0], mm_intermediate[2]);                 \
        mm_intermediate[1] = _mm_add_epi64(mm_intermediate[1], mm_intermediate[3]);                 \
        dst = _mm_add_epi64(mm_intermediate[0], mm_intermediate[1]);                                \
    }

void affine_sobel_flt_hor_sse(pel *pred, int i_pred, s16 *deriv, int i_deriv, int width, int height)
{
    int j, col, row;
    __m128i mm_pred[4];
    __m128i mm2x_pred[2];
    __m128i mm_intermediates[4];
    __m128i mm_deriv[2];
    assert(!(height % 2));
    assert(!(width % 4));
    /* Derivates of the rows and columns at the boundary are done at the end of this function */
    /* The value of col and row indicate the columns and rows for which the derivs have already been computed */
    for (col = 1; (col + 4) < width; col += 4) {
#if (BIT_DEPTH == 8)
        mm_pred[0] = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i *)(&pred[0 * i_pred + col - 1])));
        mm_pred[1] = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i *)(&pred[1 * i_pred + col - 1])));
#else
        mm_pred[0] = _mm_loadu_si128((const __m128i *)(&pred[0 * i_pred + col - 1]));
        mm_pred[1] = _mm_loadu_si128((const __m128i *)(&pred[1 * i_pred + col - 1]));
#endif
        for (row = 1; row < (height - 1); row += 2) {
#if (BIT_DEPTH == 8)
            mm_pred[2] = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i *)(&pred[(row + 1) * i_pred + col - 1])));
            mm_pred[3] = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i *)(&pred[(row + 2) * i_pred + col - 1])));
#else
            mm_pred[2] = _mm_loadu_si128((const __m128i *)(&pred[(row + 1) * i_pred + col - 1]));
            mm_pred[3] = _mm_loadu_si128((const __m128i *)(&pred[(row + 2) * i_pred + col - 1]));
#endif
            mm2x_pred[0] = _mm_slli_epi16(mm_pred[1], 1);
            mm2x_pred[1] = _mm_slli_epi16(mm_pred[2], 1);
            mm_intermediates[0] = _mm_add_epi16(mm2x_pred[0], mm_pred[0]);
            mm_intermediates[2] = _mm_add_epi16(mm2x_pred[1], mm_pred[1]);
            mm_intermediates[0] = _mm_add_epi16(mm_intermediates[0], mm_pred[2]);
            mm_intermediates[2] = _mm_add_epi16(mm_intermediates[2], mm_pred[3]);
            mm_pred[0] = mm_pred[2];
            mm_pred[1] = mm_pred[3];
            mm_intermediates[1] = _mm_srli_si128(mm_intermediates[0], 4);
            mm_intermediates[3] = _mm_srli_si128(mm_intermediates[2], 4);
            mm_deriv[0] = _mm_sub_epi16(mm_intermediates[1], mm_intermediates[0]);
            mm_deriv[1] = _mm_sub_epi16(mm_intermediates[3], mm_intermediates[2]);
            _mm_storel_epi64((__m128i *)(&deriv[col + (row + 0) * i_deriv]), mm_deriv[0]);
            _mm_storel_epi64((__m128i *)(&deriv[col + (row + 1) * i_deriv]), mm_deriv[1]);
        }
    }
    {
#if (BIT_DEPTH == 8)
        mm_pred[0] = _mm_cvtepu8_epi16(_mm_cvtsi32_si128(*(int *)(pred + col - 1)));
        mm_pred[1] = _mm_cvtepu8_epi16(_mm_cvtsi32_si128(*(int *)(pred + col - 1 + i_pred)));
#else
        mm_pred[0] = _mm_loadl_epi64((const __m128i *)(&pred[0 * i_pred + col - 1]));
        mm_pred[1] = _mm_loadl_epi64((const __m128i *)(&pred[1 * i_pred + col - 1]));
#endif
        for (row = 1; row < (height - 1); row += 2) {
#if (BIT_DEPTH == 8)
            mm_pred[2] = _mm_cvtepu8_epi16(_mm_cvtsi32_si128(*(int *)(pred + col - 1 + (row + 1) * i_pred)));
            mm_pred[3] = _mm_cvtepu8_epi16(_mm_cvtsi32_si128(*(int *)(pred + col - 1 + (row + 2) * i_pred)));
#else
            mm_pred[2] = _mm_loadl_epi64((const __m128i *)(&pred[(row + 1) * i_pred + col - 1]));
            mm_pred[3] = _mm_loadl_epi64((const __m128i *)(&pred[(row + 2) * i_pred + col - 1]));
#endif
            mm2x_pred[0] = _mm_slli_epi16(mm_pred[1], 1);
            mm2x_pred[1] = _mm_slli_epi16(mm_pred[2], 1);
            mm_intermediates[0] = _mm_add_epi16(mm2x_pred[0], mm_pred[0]);
            mm_intermediates[2] = _mm_add_epi16(mm2x_pred[1], mm_pred[1]);
            mm_intermediates[0] = _mm_add_epi16(mm_intermediates[0], mm_pred[2]);
            mm_intermediates[2] = _mm_add_epi16(mm_intermediates[2], mm_pred[3]);
            mm_pred[0] = mm_pred[2];
            mm_pred[1] = mm_pred[3];
            mm_intermediates[1] = _mm_srli_si128(mm_intermediates[0], 4);
            mm_intermediates[3] = _mm_srli_si128(mm_intermediates[2], 4);
            mm_deriv[0] = _mm_sub_epi16(mm_intermediates[1], mm_intermediates[0]);
            mm_deriv[1] = _mm_sub_epi16(mm_intermediates[3], mm_intermediates[2]);
            *(s32*)(&deriv[col + (row + 0) * i_deriv]) = _mm_extract_epi32(mm_deriv[0], 0);
            *(s32*)(&deriv[col + (row + 1) * i_deriv]) = _mm_extract_epi32(mm_deriv[1], 0);
        }
    }
    for (j = 1; j < (height - 1); j++) {
        deriv[j *i_deriv] = deriv[j * i_deriv + 1];
        deriv[j * i_deriv + (width - 1)] = deriv[j * i_deriv + (width - 2)];
    }
    com_mcpy
    (
        deriv,
        deriv + i_deriv,
        width * sizeof(deriv[0])
    );
    com_mcpy
    (
        deriv + (height - 1) * i_deriv,
        deriv + (height - 2) * i_deriv,
        width * sizeof(deriv[0])
    );
}

void affine_sobel_flt_ver_sse(pel *pred, int i_pred, s16 *deriv, int i_deriv, int width, int height)
{
    int j, col, row;
    __m128i mm_pred[4];
    __m128i mm_intermediates[6];
    __m128i mm_derivate[2];
    assert(!(height % 2));
    assert(!(width % 4));

    for (col = 1; col + 4 < width; col += 4) {
#if (BIT_DEPTH == 8)
        mm_pred[0] = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i *)(&pred[0 * i_pred + col - 1])));
        mm_pred[1] = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i *)(&pred[1 * i_pred + col - 1])));
#else
        mm_pred[0] = _mm_loadu_si128((const __m128i *)(&pred[0 * i_pred + col - 1]));
        mm_pred[1] = _mm_loadu_si128((const __m128i *)(&pred[1 * i_pred + col - 1]));
#endif
        for (row = 1; row < (height - 1); row += 2) {
#if (BIT_DEPTH == 8)
            mm_pred[2] = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i *)(&pred[(row + 1) * i_pred + col - 1])));
            mm_pred[3] = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i *)(&pred[(row + 2) * i_pred + col - 1])));
#else
            mm_pred[2] = _mm_loadu_si128((const __m128i *)(&pred[(row + 1) * i_pred + col - 1]));
            mm_pred[3] = _mm_loadu_si128((const __m128i *)(&pred[(row + 2) * i_pred + col - 1]));
#endif
            mm_intermediates[0] = _mm_sub_epi16(mm_pred[2], mm_pred[0]);
            mm_intermediates[3] = _mm_sub_epi16(mm_pred[3], mm_pred[1]);
            mm_pred[0] = mm_pred[2];
            mm_pred[1] = mm_pred[3];
            mm_intermediates[1] = _mm_srli_si128(mm_intermediates[0], 2);
            mm_intermediates[4] = _mm_srli_si128(mm_intermediates[3], 2);
            mm_intermediates[2] = _mm_srli_si128(mm_intermediates[0], 4);
            mm_intermediates[5] = _mm_srli_si128(mm_intermediates[3], 4);
            mm_intermediates[1] = _mm_slli_epi16(mm_intermediates[1], 1);
            mm_intermediates[4] = _mm_slli_epi16(mm_intermediates[4], 1);
            mm_intermediates[0] = _mm_add_epi16(mm_intermediates[0], mm_intermediates[2]);
            mm_intermediates[3] = _mm_add_epi16(mm_intermediates[3], mm_intermediates[5]);
            mm_derivate[0] = _mm_add_epi16(mm_intermediates[0], mm_intermediates[1]);
            mm_derivate[1] = _mm_add_epi16(mm_intermediates[3], mm_intermediates[4]);
            _mm_storel_epi64((__m128i *)(&deriv[col + (row + 0) * i_deriv]), mm_derivate[0]);
            _mm_storel_epi64((__m128i *)(&deriv[col + (row + 1) * i_deriv]), mm_derivate[1]);
        }
    }
    {
#if (BIT_DEPTH == 8)
        mm_pred[0] = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i *)(&pred[0 * i_pred + col - 1])));
        mm_pred[1] = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i *)(&pred[1 * i_pred + col - 1])));
#else
        mm_pred[0] = _mm_loadu_si128((const __m128i *)(&pred[0 * i_pred + col - 1]));
        mm_pred[1] = _mm_loadu_si128((const __m128i *)(&pred[1 * i_pred + col - 1]));
#endif
        for (row = 1; row < (height - 1); row += 2) {
#if (BIT_DEPTH == 8)
            mm_pred[2] = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i *)(&pred[(row + 1) * i_pred + col - 1])));
            mm_pred[3] = _mm_cvtepu8_epi16(_mm_loadl_epi64((const __m128i *)(&pred[(row + 2) * i_pred + col - 1])));
#else
            mm_pred[2] = _mm_loadu_si128((const __m128i *)(&pred[(row + 1) * i_pred + col - 1]));
            mm_pred[3] = _mm_loadu_si128((const __m128i *)(&pred[(row + 2) * i_pred + col - 1]));
#endif
            mm_intermediates[0] = _mm_sub_epi16(mm_pred[2], mm_pred[0]);
            mm_intermediates[3] = _mm_sub_epi16(mm_pred[3], mm_pred[1]);
            mm_pred[0] = mm_pred[2];
            mm_pred[1] = mm_pred[3];
            mm_intermediates[1] = _mm_srli_si128(mm_intermediates[0], 2);
            mm_intermediates[4] = _mm_srli_si128(mm_intermediates[3], 2);
            mm_intermediates[2] = _mm_srli_si128(mm_intermediates[0], 4);
            mm_intermediates[5] = _mm_srli_si128(mm_intermediates[3], 4);
            mm_intermediates[1] = _mm_slli_epi16(mm_intermediates[1], 1);
            mm_intermediates[4] = _mm_slli_epi16(mm_intermediates[4], 1);
            mm_intermediates[0] = _mm_add_epi16(mm_intermediates[0], mm_intermediates[2]);
            mm_intermediates[3] = _mm_add_epi16(mm_intermediates[3], mm_intermediates[5]);
            mm_derivate[0] = _mm_add_epi16(mm_intermediates[0], mm_intermediates[1]);
            mm_derivate[1] = _mm_add_epi16(mm_intermediates[3], mm_intermediates[4]);
            *(s32*)(&deriv[col + (row + 0) * i_deriv]) = _mm_extract_epi32(mm_derivate[0], 0);
            *(s32*)(&deriv[col + (row + 1) * i_deriv]) = _mm_extract_epi32(mm_derivate[1], 0);
        }
    }
    for (j = 1; j < (height - 1); j++) {
        deriv[j *i_deriv] = deriv[j * i_deriv + 1];
        deriv[j * i_deriv + (width - 1)] = deriv[j * i_deriv + (width - 2)];
    }
    com_mcpy
    (
        deriv,
        deriv + i_deriv,
        width * sizeof(deriv[0])
    );
    com_mcpy
    (
        deriv + (height - 1) * i_deriv,
        deriv + (height - 2) * i_deriv,
        width * sizeof(deriv[0])
    );

}

// deriv: 11 bits signed integer
void affine_coef_computer_sse(s16 *resi, int i_resi, s16(*deriv)[MAX_CU_DIM], int i_deriv, s64(*coef)[7], int width, int height, int vertex_num)
{
    int j, k;
    int idx1 = 0, idx2 = 0;
    __m128i mm_two, mm_four;
    __m128i mm_tmp[4];
    __m128i mm_intermediate[4];
    __m128i mm_idx_k, mm_idx_j[2];
    __m128i mm_residue[2];

    mm_two = _mm_set1_epi32(2);
    mm_four = _mm_set1_epi32(4);
    if (vertex_num == 3) {
        __m128i mm_c[12];
        //  mm_c - map
        //  C for 1st row of pixels
        //  mm_c[0] = iC[0][i] | iC[0][i+1] | iC[0][i+2] | iC[0][i+3]
        //  mm_c[1] = iC[1][i] | iC[1][i+1] | iC[1][i+2] | iC[1][i+3]
        //  mm_c[2] = iC[2][i] | iC[2][i+1] | iC[2][i+2] | iC[2][i+3]
        //  mm_c[3] = iC[3][i] | iC[3][i+1] | iC[3][i+2] | iC[3][i+3]
        //  mm_c[4] = iC[4][i] | iC[4][i+1] | iC[4][i+2] | iC[4][i+3]
        //  mm_c[5] = iC[5][i] | iC[5][i+1] | iC[5][i+2] | iC[5][i+3]
        //  C for 2nd row of pixels
        //  mm_c[6] = iC[6][i] | iC[6][i+1] | iC[6][i+2] | iC[6][i+3]
        //  mm_c[7] = iC[7][i] | iC[7][i+1] | iC[7][i+2] | iC[7][i+3]
        //  mm_c[8] = iC[8][i] | iC[8][i+1] | iC[8][i+2] | iC[8][i+3]
        //  mm_c[9] = iC[9][i] | iC[9][i+1] | iC[9][i+2] | iC[9][i+3]
        //  mm_c[10] = iC[10][i] | iC[10][i+1] | iC[10][i+2] | iC[10][i+3]
        //  mm_c[11] = iC[11][i] | iC[11][i+1] | iC[11][i+2] | iC[11][i+3]
        idx1 = -2 * i_deriv - 4;
        idx2 = -i_deriv - 4;
        mm_idx_j[0] = _mm_set1_epi32(-2);
        mm_idx_j[1] = _mm_set1_epi32(-1);
        for (j = 0; j < height; j += 2) {
            mm_idx_j[0] = _mm_add_epi32(mm_idx_j[0], mm_two);
            mm_idx_j[1] = _mm_add_epi32(mm_idx_j[1], mm_two);
            mm_idx_k = _mm_set_epi32(-1, -2, -3, -4);
            idx1 += (i_deriv << 1);
            idx2 += (i_deriv << 1);
            for (k = 0; k < width; k += 4) {
                idx1 += 4;
                idx2 += 4;
                mm_idx_k = _mm_add_epi32(mm_idx_k, mm_four);
                // 1st row
                mm_c[0] = _mm_loadl_epi64((const __m128i *)&deriv[0][idx1]);
                mm_c[2] = _mm_loadl_epi64((const __m128i *)&deriv[1][idx1]);
                mm_c[0] = _mm_cvtepi16_epi32(mm_c[0]);
                mm_c[2] = _mm_cvtepi16_epi32(mm_c[2]);
                // 2nd row
                mm_c[6] = _mm_loadl_epi64((const __m128i *)&deriv[0][idx2]);
                mm_c[8] = _mm_loadl_epi64((const __m128i *)&deriv[1][idx2]);
                mm_c[6] = _mm_cvtepi16_epi32(mm_c[6]);
                mm_c[8] = _mm_cvtepi16_epi32(mm_c[8]);
                // 1st row
                mm_c[1] = _mm_mullo_epi32(mm_idx_k, mm_c[0]);
                mm_c[3] = _mm_mullo_epi32(mm_idx_k, mm_c[2]);
                mm_c[4] = _mm_mullo_epi32(mm_idx_j[0], mm_c[0]);
                mm_c[5] = _mm_mullo_epi32(mm_idx_j[0], mm_c[2]);
                // 2nd row
                mm_c[7] = _mm_mullo_epi32(mm_idx_k, mm_c[6]);
                mm_c[9] = _mm_mullo_epi32(mm_idx_k, mm_c[8]);
                mm_c[10] = _mm_mullo_epi32(mm_idx_j[1], mm_c[6]);
                mm_c[11] = _mm_mullo_epi32(mm_idx_j[1], mm_c[8]);
                // Residue
                mm_residue[0] = _mm_loadl_epi64((const __m128i *)&resi[idx1]);
                mm_residue[1] = _mm_loadl_epi64((const __m128i *)&resi[idx2]);
                mm_residue[0] = _mm_cvtepi16_epi32(mm_residue[0]);
                mm_residue[1] = _mm_cvtepi16_epi32(mm_residue[1]);
                mm_residue[0] = _mm_slli_epi32(mm_residue[0], 3);
                mm_residue[1] = _mm_slli_epi32(mm_residue[1], 3);
                // Calculate resi coefficients first
                mm_tmp[2] = _mm_srli_si128(mm_residue[0], 4);
                mm_tmp[3] = _mm_srli_si128(mm_residue[1], 4);
                // 1st row
                mm_tmp[0] = _mm_srli_si128(mm_c[0], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[6], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[6], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[1][6] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 2nd row
                mm_tmp[0] = _mm_srli_si128(mm_c[1], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[7], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[7], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[2][6] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 3rd row
                mm_tmp[0] = _mm_srli_si128(mm_c[2], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[8], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[2], mm_c[8], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[3][6] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 4th row
                mm_tmp[0] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[9], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[3], mm_c[9], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[4][6] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 5th row
                mm_tmp[0] = _mm_srli_si128(mm_c[4], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[10], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[4], mm_c[10], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[5][6] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 6th row
                mm_tmp[0] = _mm_srli_si128(mm_c[5], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[11], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[5], mm_c[11], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[6][6] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                //Start calculation of coefficient matrix
                // 1st row
                mm_tmp[0] = _mm_srli_si128(mm_c[0], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[6], 4);
                // 1st col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[6], mm_c[0], mm_c[6], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0]);
                coef[1][0] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 2nd col of row and 1st col of 2nd row
                mm_tmp[2] = _mm_srli_si128(mm_c[1], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[7], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[6], mm_c[1], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[1][1] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 3rd col of row and 1st col of 3rd row
                mm_tmp[2] = _mm_srli_si128(mm_c[2], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[8], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[6], mm_c[2], mm_c[8], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[1][2] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 4th col of row and 1st col of 4th row
                mm_tmp[2] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[9], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[6], mm_c[3], mm_c[9], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[1][3] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 5th col of row and 1st col of the 5th row
                mm_tmp[2] = _mm_srli_si128(mm_c[4], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[10], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[6], mm_c[4], mm_c[10], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[1][4] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 6th col of row and 1st col of the 6th row
                mm_tmp[2] = _mm_srli_si128(mm_c[5], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[11], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[6], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[1][5] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 2nd row
                mm_tmp[0] = _mm_srli_si128(mm_c[1], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[7], 4);
                // 2nd col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[7], mm_c[1], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0]);
                coef[2][1] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 3rd col of row and 2nd col of 3rd row
                mm_tmp[2] = _mm_srli_si128(mm_c[2], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[8], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[7], mm_c[2], mm_c[8], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[2][2] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 4th col of row and 2nd col of 4th row
                mm_tmp[2] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[9], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[7], mm_c[3], mm_c[9], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[2][3] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 5th col of row and 1st col of the 5th row
                mm_tmp[2] = _mm_srli_si128(mm_c[4], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[10], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[7], mm_c[4], mm_c[10], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[2][4] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 6th col of row and 1st col of the 6th row
                mm_tmp[2] = _mm_srli_si128(mm_c[5], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[11], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[7], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[2][5] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 3rd row
                mm_tmp[0] = _mm_srli_si128(mm_c[2], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[8], 4);
                //3rd Col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[2], mm_c[8], mm_c[2], mm_c[8], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0]);
                coef[3][2] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 4th col of row and 3rd col of 4th row
                mm_tmp[2] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[9], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[2], mm_c[8], mm_c[3], mm_c[9], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[3][3] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 5th col of row and 1st col of the 5th row
                mm_tmp[2] = _mm_srli_si128(mm_c[4], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[10], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[2], mm_c[8], mm_c[4], mm_c[10], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[3][4] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 6th col of row and 1st col of the 6th row
                mm_tmp[2] = _mm_srli_si128(mm_c[5], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[11], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[2], mm_c[8], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[3][5] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 4th row
                mm_tmp[0] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[9], 4);
                // 4th col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[3], mm_c[9], mm_c[3], mm_c[9], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0]);
                coef[4][3] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 5th col of row and 1st col of the 5th row
                mm_tmp[2] = _mm_srli_si128(mm_c[4], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[10], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[3], mm_c[9], mm_c[4], mm_c[10], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[4][4] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 6th col of row and 1st col of the 6th row
                mm_tmp[2] = _mm_srli_si128(mm_c[5], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[11], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[3], mm_c[9], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[4][5] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 5th row
                mm_tmp[0] = _mm_srli_si128(mm_c[4], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[10], 4);
                // 5th col of row and 1st col of the 5th row
                CALC_EQUAL_COEFF_8PXLS(mm_c[4], mm_c[10], mm_c[4], mm_c[10], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0]);
                coef[5][4] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 6th col of row and 1st col of the 6th row
                mm_tmp[2] = _mm_srli_si128(mm_c[5], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[11], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[4], mm_c[10], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[5][5] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 6th row
                mm_tmp[0] = _mm_srli_si128(mm_c[5], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[11], 4);
                // 5th col of row and 1st col of the 5th row
                CALC_EQUAL_COEFF_8PXLS(mm_c[5], mm_c[11], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0]);
                coef[6][5] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
            }
            idx1 -= (width);
            idx2 -= (width);
        }
        coef[2][0] = coef[1][1];
        coef[3][0] = coef[1][2];
        coef[4][0] = coef[1][3];
        coef[5][0] = coef[1][4];
        coef[6][0] = coef[1][5];
        coef[3][1] = coef[2][2];
        coef[4][1] = coef[2][3];
        coef[5][1] = coef[2][4];
        coef[6][1] = coef[2][5];
        coef[4][2] = coef[3][3];
        coef[5][2] = coef[3][4];
        coef[6][2] = coef[3][5];
        coef[5][3] = coef[4][4];
        coef[6][3] = coef[4][5];
        coef[6][4] = coef[5][5];
    }
    else {
        __m128i mm_c[12];
        __m128i mm_four_s16 = _mm_set1_epi16(4);
        __m128i mm_two_s16 = _mm_set1_epi16(2);
        __m128i mm_idx_jk[2], mm_idx_kj[2];
        __m128i zero = _mm_setzero_si128();

        idx1 = 0;
        idx2 = i_deriv;
        mm_idx_j[0] = _mm_set1_epi16(0);
        mm_idx_j[1] = _mm_set1_epi16(1);
        for (j = 0; j < height; j += 2) {
            mm_idx_k = _mm_set_epi16(0, 0, 0, 0, 3, 2, 1, 0);
            for (k = 0; k < width; k += 4) {
                mm_c[0] = _mm_loadl_epi64((const __m128i *)&deriv[0][idx1]);
                mm_c[2] = _mm_loadl_epi64((const __m128i *)&deriv[1][idx1]);
                mm_c[4] = _mm_loadl_epi64((const __m128i *)&deriv[0][idx2]);
                mm_c[6] = _mm_loadl_epi64((const __m128i *)&deriv[1][idx2]);
                mm_tmp[0] = _mm_subs_epi16(zero, mm_idx_k);
                mm_idx_kj[0] = _mm_unpacklo_epi16(mm_idx_k, mm_idx_j[0]);
                mm_idx_kj[1] = _mm_unpacklo_epi16(mm_idx_k, mm_idx_j[1]);
                mm_idx_jk[0] = _mm_unpacklo_epi16(mm_idx_j[0], mm_tmp[0]);
                mm_idx_jk[1] = _mm_unpacklo_epi16(mm_idx_j[1], mm_tmp[0]);
                mm_tmp[1] = _mm_unpacklo_epi16(mm_c[0], mm_c[2]);
                mm_tmp[2] = _mm_unpacklo_epi16(mm_c[4], mm_c[6]);
                mm_c[1] = _mm_madd_epi16(mm_idx_kj[0], mm_tmp[1]);
                mm_c[3] = _mm_madd_epi16(mm_idx_jk[0], mm_tmp[1]);
                mm_c[5] = _mm_madd_epi16(mm_idx_kj[1], mm_tmp[2]);
                mm_c[7] = _mm_madd_epi16(mm_idx_jk[1], mm_tmp[2]);

                mm_residue[0] = _mm_loadl_epi64((const __m128i *)&resi[idx1]);
                mm_residue[1] = _mm_loadl_epi64((const __m128i *)&resi[idx2]);
                mm_c[8] = _mm_unpacklo_epi64(mm_c[0], mm_c[2]);
                mm_c[9] = _mm_unpacklo_epi64(mm_c[2], mm_c[0]);
                mm_c[10] = _mm_unpacklo_epi64(mm_c[4], mm_c[6]);
                mm_c[11] = _mm_unpacklo_epi64(mm_c[6], mm_c[4]);
                mm_c[0] = _mm_cvtepi16_epi32(mm_c[0]);
                mm_c[2] = _mm_cvtepi16_epi32(mm_c[2]);
                mm_c[4] = _mm_cvtepi16_epi32(mm_c[4]);
                mm_c[6] = _mm_cvtepi16_epi32(mm_c[6]);

                mm_tmp[0] = _mm_madd_epi16(mm_c[8], mm_c[8]);
                mm_tmp[1] = _mm_madd_epi16(mm_c[8], mm_c[9]);
                mm_tmp[2] = _mm_madd_epi16(mm_c[10], mm_c[10]);
                mm_tmp[3] = _mm_madd_epi16(mm_c[10], mm_c[11]);
                mm_tmp[0] = _mm_add_epi32(mm_tmp[0], mm_tmp[2]);
                mm_tmp[1] = _mm_add_epi32(mm_tmp[1], mm_tmp[3]);
                mm_tmp[0] = _mm_hadd_epi32(mm_tmp[0], mm_tmp[1]);
                coef[1][0] += _mm_extract_epi32(mm_tmp[0], 0);
                coef[3][2] += _mm_extract_epi32(mm_tmp[0], 1);
                coef[1][2] += _mm_extract_epi32(mm_tmp[0], 2);

                mm_c[9] = _mm_unpacklo_epi64(mm_residue[0], mm_residue[0]);
                mm_c[11] = _mm_unpacklo_epi64(mm_residue[1], mm_residue[1]);
                mm_tmp[0] = _mm_madd_epi16(mm_c[8], mm_c[9]);
                mm_tmp[1] = _mm_madd_epi16(mm_c[10], mm_c[11]);
                mm_tmp[0] = _mm_add_epi32(mm_tmp[0], mm_tmp[1]);
                mm_tmp[0] = _mm_hadd_epi32(mm_tmp[0], mm_tmp[0]);
                coef[1][4] += _mm_extract_epi32(mm_tmp[0], 0);
                coef[3][4] += _mm_extract_epi32(mm_tmp[0], 1);

                mm_residue[0] = _mm_cvtepi16_epi32(mm_residue[0]);
                mm_residue[1] = _mm_cvtepi16_epi32(mm_residue[1]);

                idx1 += 4;
                idx2 += 4;
                mm_idx_k = _mm_add_epi16(mm_idx_k, mm_four_s16);

                // 1st row
                mm_tmp[0] = _mm_srli_si128(mm_c[0], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[4], 4);
                mm_tmp[2] = _mm_srli_si128(mm_c[1], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[5], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[4], mm_c[1], mm_c[5], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[1][1] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);

                mm_tmp[2] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[7], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[4], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[1][3] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);

                // 2nd row
                mm_tmp[0] = _mm_srli_si128(mm_c[1], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[5], 4);
                // 2nd col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[5], mm_c[1], mm_c[5], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0]);
                coef[2][1] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 3rd col of row and 2nd col of 3rd row
                mm_tmp[2] = _mm_srli_si128(mm_c[2], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[6], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[5], mm_c[2], mm_c[6], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[2][2] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 4th col of row and 2nd col of 4th row
                mm_tmp[2] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[7], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[5], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[2][3] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 5th col of row
                mm_tmp[2] = _mm_srli_si128(mm_residue[0], 4);
                mm_tmp[3] = _mm_srli_si128(mm_residue[1], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[5], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[2][4] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 3rd row
                mm_tmp[0] = _mm_srli_si128(mm_c[2], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[6], 4);
                mm_tmp[2] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[7], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[2], mm_c[6], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[3][3] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);

                // 4th row
                mm_tmp[0] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[7], 4);
                // 4th col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[3], mm_c[7], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0]);
                coef[4][3] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
                // 5th col of row
                mm_tmp[2] = _mm_srli_si128(mm_residue[0], 4);
                mm_tmp[3] = _mm_srli_si128(mm_residue[1], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[3], mm_c[7], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0]);
                coef[4][4] += _mm_extract_epi64(mm_intermediate[0], 0) + _mm_extract_epi64(mm_intermediate[0], 1);
            }
            idx1 += (i_deriv << 1) - width;
            idx2 += (i_deriv << 1) - width;
            mm_idx_j[0] = _mm_add_epi16(mm_idx_j[0], mm_two_s16);
            mm_idx_j[1] = _mm_add_epi16(mm_idx_j[1], mm_two_s16);
        }
        coef[1][4] <<= 3;
        coef[2][4] <<= 3;
        coef[3][4] <<= 3;
        coef[4][4] <<= 3;
        coef[2][0] = coef[1][1];
        coef[3][0] = coef[1][2];
        coef[4][0] = coef[1][3];
        coef[3][1] = coef[2][2];
        coef[4][1] = coef[2][3];
        coef[4][2] = coef[3][3];
    }
}
