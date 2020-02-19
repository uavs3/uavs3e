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

#include "avx2.h"
#include "../sse/sse.h"

void affine_sobel_flt_hor_avx2(pel *pred, int i_pred, int *deriv, int i_deriv, int width, int height)
{
    int j, col, row;
    __m256i mm_pred[4];
    __m256i mm2x_pred[2];
    __m256i mm_intermediates[4];
    __m256i mm_deriv[2];
    __m128i m0, m1;
    assert(!(height % 2));
    assert(!(width % 4));
    /* Derivates of the rows and columns at the boundary are done at the end of this function */
    /* The value of col and row indicate the columns and rows for which the derivs have already been computed */
    for (col = 1; (col + 4) < width; col += 4) {
#if (BIT_DEPTH == 8)
        m0 = _mm_set_epi32(0, 0, *(int *)(pred + col + 1), *(int *)(pred + col - 1));
        m1 = _mm_set_epi32(0, 0, *(int *)(pred + col + 1 + i_pred), *(int *)(pred + col - 1 + i_pred));
        m0 = _mm_cvtepu8_epi16(m0);
        m1 = _mm_cvtepu8_epi16(m1);
#else
        m0 = _mm_set_epi64x(*(s64 *)(pred + col + 1), *(s64 *)(pred + col - 1));
        m1 = _mm_set_epi64x(*(s64 *)(pred + col + 1 + i_pred), *(s64 *)(pred + col - 1 + i_pred));
#endif
        mm_pred[0] = _mm256_cvtepi16_epi32(m0);
        mm_pred[1] = _mm256_cvtepi16_epi32(m1);
        for (row = 1; row < (height - 1); row += 2) {
#if (BIT_DEPTH == 8)
            m0 = _mm_set_epi32(0, 0, *(int *)(pred + col + 1 + (row + 1) * i_pred), *(int *)(pred + col - 1 + (row + 1) * i_pred));
            m1 = _mm_set_epi32(0, 0, *(int *)(pred + col + 1 + (row + 2) * i_pred), *(int *)(pred + col - 1 + (row + 2) * i_pred));
            m0 = _mm_cvtepu8_epi16(m0);
            m1 = _mm_cvtepu8_epi16(m1);
#else
            m0 = _mm_set_epi64x(*(s64 *)(pred + col + 1 + (row + 1) * i_pred), *(s64 *)(pred + col - 1 + (row + 1) * i_pred));
            m1 = _mm_set_epi64x(*(s64 *)(pred + col + 1 + (row + 2) * i_pred), *(s64 *)(pred + col - 1 + (row + 2) * i_pred));
#endif
            mm_pred[2] = _mm256_cvtepi16_epi32(m0);
            mm_pred[3] = _mm256_cvtepi16_epi32(m1);
            mm2x_pred[0] = _mm256_slli_epi32(mm_pred[1], 1);
            mm2x_pred[1] = _mm256_slli_epi32(mm_pred[2], 1);
            mm_intermediates[0] = _mm256_add_epi32(mm2x_pred[0], mm_pred[0]);
            mm_intermediates[2] = _mm256_add_epi32(mm2x_pred[1], mm_pred[1]);
            mm_intermediates[0] = _mm256_add_epi32(mm_intermediates[0], mm_pred[2]);
            mm_intermediates[2] = _mm256_add_epi32(mm_intermediates[2], mm_pred[3]);
            mm_pred[0] = mm_pred[2];
            mm_pred[1] = mm_pred[3];
            mm_intermediates[1] = _mm256_srli_si256(mm_intermediates[0], 8);
            mm_intermediates[3] = _mm256_srli_si256(mm_intermediates[2], 8);
            mm_deriv[0] = _mm256_sub_epi32(mm_intermediates[1], mm_intermediates[0]);
            mm_deriv[1] = _mm256_sub_epi32(mm_intermediates[3], mm_intermediates[2]);
            mm_deriv[0] = _mm256_permute4x64_epi64(mm_deriv[0], 0xd8);
            mm_deriv[1] = _mm256_permute4x64_epi64(mm_deriv[1], 0xd8);
            _mm_storeu_si128((__m128i *)(&deriv[col + (row + 0) * i_deriv]), _mm256_castsi256_si128(mm_deriv[0]));
            _mm_storeu_si128((__m128i *)(&deriv[col + (row + 1) * i_deriv]), _mm256_castsi256_si128(mm_deriv[1]));
        }
    }
    {
        __m128i mm_pred[4];
        __m128i mm2x_pred[2];
        __m128i mm_intermediates[4];
        __m128i mm_deriv[2];
#if (BIT_DEPTH == 8)
        mm_pred[0] = _mm_cvtepu8_epi16(_mm_cvtsi32_si128(*(int *)(pred + col - 1)));
        mm_pred[1] = _mm_cvtepu8_epi16(_mm_cvtsi32_si128(*(int *)(pred + col - 1 + i_pred)));
#else
        mm_pred[0] = _mm_loadl_epi64((const __m128i *)(&pred[0 * i_pred + col - 1]));
        mm_pred[1] = _mm_loadl_epi64((const __m128i *)(&pred[1 * i_pred + col - 1]));
#endif
        mm_pred[0] = _mm_cvtepi16_epi32(mm_pred[0]);
        mm_pred[1] = _mm_cvtepi16_epi32(mm_pred[1]);
        for (row = 1; row < (height - 1); row += 2) {
#if (BIT_DEPTH == 8)
            mm_pred[2] = _mm_cvtepu8_epi16(_mm_cvtsi32_si128(*(int *)(pred + col - 1 + (row + 1) * i_pred)));
            mm_pred[3] = _mm_cvtepu8_epi16(_mm_cvtsi32_si128(*(int *)(pred + col - 1 + (row + 2) * i_pred)));
#else
            mm_pred[2] = _mm_loadl_epi64((const __m128i *)(&pred[(row + 1) * i_pred + col - 1]));
            mm_pred[3] = _mm_loadl_epi64((const __m128i *)(&pred[(row + 2) * i_pred + col - 1]));
#endif
            mm_pred[2] = _mm_cvtepi16_epi32(mm_pred[2]);
            mm_pred[3] = _mm_cvtepi16_epi32(mm_pred[3]);
            mm2x_pred[0] = _mm_slli_epi32(mm_pred[1], 1);
            mm2x_pred[1] = _mm_slli_epi32(mm_pred[2], 1);
            mm_intermediates[0] = _mm_add_epi32(mm2x_pred[0], mm_pred[0]);
            mm_intermediates[2] = _mm_add_epi32(mm2x_pred[1], mm_pred[1]);
            mm_intermediates[0] = _mm_add_epi32(mm_intermediates[0], mm_pred[2]);
            mm_intermediates[2] = _mm_add_epi32(mm_intermediates[2], mm_pred[3]);
            mm_pred[0] = mm_pred[2];
            mm_pred[1] = mm_pred[3];
            mm_intermediates[1] = _mm_srli_si128(mm_intermediates[0], 8);
            mm_intermediates[3] = _mm_srli_si128(mm_intermediates[2], 8);
            mm_deriv[0] = _mm_sub_epi32(mm_intermediates[1], mm_intermediates[0]);
            mm_deriv[1] = _mm_sub_epi32(mm_intermediates[3], mm_intermediates[2]);
            _mm_storel_epi64((__m128i *)(&deriv[col + (row + 0) * i_deriv]), mm_deriv[0]);
            _mm_storel_epi64((__m128i *)(&deriv[col + (row + 1) * i_deriv]), mm_deriv[1]);
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

void affine_sobel_flt_ver_avx2(pel *pred, int i_pred, int *deriv, int i_deriv, int width, int height)
{
    int j, col, row;
    __m256i mm_pred[4];
    __m256i mm_intermediates[6];
    __m256i mm_derivate[2];
    __m128i m0, m1;
    assert(!(height % 2));
    assert(!(width % 4));
    /* Derivates of the rows and columns at the boundary are done at the end of this function */
    /* The value of col and row indicate the columns and rows for which the derivates have already been computed */
    for (col = 1; col < (width - 3); col += 4) {
#if (BIT_DEPTH == 8)
        m0 = _mm_set_epi32(0, 0, *(int *)(pred + col + 1), *(int *)(pred + col - 1));
        m1 = _mm_set_epi32(0, 0, *(int *)(pred + col + 1 + i_pred), *(int *)(pred + col - 1 + i_pred));
        m0 = _mm_cvtepu8_epi16(m0);
        m1 = _mm_cvtepu8_epi16(m1);
#else
        m0 = _mm_set_epi64x(*(s64 *)(pred + col + 1), *(s64 *)(pred + col - 1));
        m1 = _mm_set_epi64x(*(s64 *)(pred + col + 1 + i_pred), *(s64 *)(pred + col - 1 + i_pred));
#endif
        mm_pred[0] = _mm256_cvtepi16_epi32(m0);
        mm_pred[1] = _mm256_cvtepi16_epi32(m1);
        for (row = 1; row < (height - 1); row += 2) {
#if (BIT_DEPTH == 8)
            m0 = _mm_set_epi32(0, 0, *(int *)(pred + col + 1 + (row + 1) * i_pred), *(int *)(pred + col - 1 + (row + 1) * i_pred));
            m1 = _mm_set_epi32(0, 0, *(int *)(pred + col + 1 + (row + 2) * i_pred), *(int *)(pred + col - 1 + (row + 2) * i_pred));
            m0 = _mm_cvtepu8_epi16(m0);
            m1 = _mm_cvtepu8_epi16(m1);
#else
            m0 = _mm_set_epi64x(*(s64 *)(pred + col + 1 + (row + 1) * i_pred), *(s64 *)(pred + col - 1 + (row + 1) * i_pred));
            m1 = _mm_set_epi64x(*(s64 *)(pred + col + 1 + (row + 2) * i_pred), *(s64 *)(pred + col - 1 + (row + 2) * i_pred));
#endif
            mm_pred[2] = _mm256_cvtepi16_epi32(m0);
            mm_pred[3] = _mm256_cvtepi16_epi32(m1);
            mm_intermediates[0] = _mm256_sub_epi32(mm_pred[2], mm_pred[0]);
            mm_intermediates[3] = _mm256_sub_epi32(mm_pred[3], mm_pred[1]);
            mm_pred[0] = mm_pred[2];
            mm_pred[1] = mm_pred[3];
            mm_intermediates[1] = _mm256_srli_si256(mm_intermediates[0], 4);
            mm_intermediates[4] = _mm256_srli_si256(mm_intermediates[3], 4);
            mm_intermediates[2] = _mm256_srli_si256(mm_intermediates[0], 8);
            mm_intermediates[5] = _mm256_srli_si256(mm_intermediates[3], 8);
            mm_intermediates[1] = _mm256_slli_epi32(mm_intermediates[1], 1);
            mm_intermediates[4] = _mm256_slli_epi32(mm_intermediates[4], 1);
            mm_intermediates[0] = _mm256_add_epi32(mm_intermediates[0], mm_intermediates[2]);
            mm_intermediates[3] = _mm256_add_epi32(mm_intermediates[3], mm_intermediates[5]);
            mm_derivate[0] = _mm256_add_epi32(mm_intermediates[0], mm_intermediates[1]);
            mm_derivate[1] = _mm256_add_epi32(mm_intermediates[3], mm_intermediates[4]);
            mm_derivate[0] = _mm256_permute4x64_epi64(mm_derivate[0], 0xd8);
            mm_derivate[1] = _mm256_permute4x64_epi64(mm_derivate[1], 0xd8);
            _mm_storeu_si128((__m128i *)(&deriv[col + (row + 0) * i_deriv]), _mm256_castsi256_si128(mm_derivate[0]));
            _mm_storeu_si128((__m128i *)(&deriv[col + (row + 1) * i_deriv]), _mm256_castsi256_si128(mm_derivate[1]));
        }
    }
    {
        __m128i mm_pred[4];
        __m128i mm_intermediates[6];
        __m128i mm_derivate[2];

#if (BIT_DEPTH == 8)
        mm_pred[0] = _mm_cvtepu8_epi16(_mm_cvtsi32_si128(*(int *)(pred + col - 1)));
        mm_pred[1] = _mm_cvtepu8_epi16(_mm_cvtsi32_si128(*(int *)(pred + col - 1 + i_pred)));
#else
        mm_pred[0] = _mm_loadl_epi64((const __m128i *)(&pred[0 * i_pred + col - 1]));
        mm_pred[1] = _mm_loadl_epi64((const __m128i *)(&pred[1 * i_pred + col - 1]));
#endif
        mm_pred[0] = _mm_cvtepi16_epi32(mm_pred[0]);
        mm_pred[1] = _mm_cvtepi16_epi32(mm_pred[1]);
        for (row = 1; row < (height - 1); row += 2) {
#if (BIT_DEPTH == 8)
            mm_pred[2] = _mm_cvtepu8_epi16(_mm_cvtsi32_si128(*(int *)(pred + col - 1 + (row + 1) * i_pred)));
            mm_pred[3] = _mm_cvtepu8_epi16(_mm_cvtsi32_si128(*(int *)(pred + col - 1 + (row + 2) * i_pred)));
#else
            mm_pred[2] = _mm_loadl_epi64((const __m128i *)(&pred[(row + 1) * i_pred + col - 1]));
            mm_pred[3] = _mm_loadl_epi64((const __m128i *)(&pred[(row + 2) * i_pred + col - 1]));
#endif
            mm_pred[2] = _mm_cvtepi16_epi32(mm_pred[2]);
            mm_pred[3] = _mm_cvtepi16_epi32(mm_pred[3]);
            mm_intermediates[0] = _mm_sub_epi32(mm_pred[2], mm_pred[0]);
            mm_intermediates[3] = _mm_sub_epi32(mm_pred[3], mm_pred[1]);
            mm_pred[0] = mm_pred[2];
            mm_pred[1] = mm_pred[3];
            mm_intermediates[1] = _mm_srli_si128(mm_intermediates[0], 4);
            mm_intermediates[4] = _mm_srli_si128(mm_intermediates[3], 4);
            mm_intermediates[2] = _mm_srli_si128(mm_intermediates[0], 8);
            mm_intermediates[5] = _mm_srli_si128(mm_intermediates[3], 8);
            mm_intermediates[1] = _mm_slli_epi32(mm_intermediates[1], 1);
            mm_intermediates[4] = _mm_slli_epi32(mm_intermediates[4], 1);
            mm_intermediates[0] = _mm_add_epi32(mm_intermediates[0], mm_intermediates[2]);
            mm_intermediates[3] = _mm_add_epi32(mm_intermediates[3], mm_intermediates[5]);
            mm_derivate[0] = _mm_add_epi32(mm_intermediates[0], mm_intermediates[1]);
            mm_derivate[1] = _mm_add_epi32(mm_intermediates[3], mm_intermediates[4]);
            _mm_storel_epi64((__m128i *)(&deriv[col + (row + 0) * i_deriv]), mm_derivate[0]);
            _mm_storel_epi64((__m128i *)(&deriv[col + (row + 1) * i_deriv]), mm_derivate[1]);
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

void affine_coef_computer_avx2(s16 *resi, int i_resi, int(*deriv)[MAX_CU_DIM], int i_deriv, s64(*coef)[7], int width, int height, int vertex_num)
{
    int j, k;
    int idx1 = 0, idx2 = 0;
    if (width == 4) {
        __m128i mm_two, mm_four;
        __m128i mm_tmp[4];
        __m128i mm_intermediate[4];
        __m128i mm_idx_k, mm_idx_j[2];
        __m128i mm_residue[2];
        // Add directly to indexes to get new index
        mm_two = _mm_set1_epi32(2);
        mm_four = _mm_set1_epi32(4);

#define CALC_EQUAL_COEFF_8PXLS(x1,x2,y1,y2,tmp0,tmp1,tmp2,tmp3,inter0,inter1,inter2,inter3,load_location)      \
    {                                                                                                              \
        inter0 = _mm_mul_epi32(x1, y1);                                                                                \
        inter1 = _mm_mul_epi32(tmp0, tmp2);                                                                            \
        inter2 = _mm_mul_epi32(x2, y2);                                                                                \
        inter3 = _mm_mul_epi32(tmp1, tmp3);                                                                            \
        inter2 = _mm_add_epi64(inter0, inter2);                                                                        \
        inter3 = _mm_add_epi64(inter1, inter3);                                                                        \
        inter0 = _mm_loadl_epi64(load_location);                                                                       \
        inter3 = _mm_add_epi64(inter2, inter3);                                                                        \
        inter1 = _mm_srli_si128(inter3, 8);                                                                            \
        inter3 = _mm_add_epi64(inter1, inter3);                                                                        \
        inter3 = _mm_add_epi64(inter0, inter3);                                                                        \
    }

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
            idx1 = 0;
            idx2 = i_deriv;
            mm_idx_j[0] = _mm_set1_epi32(-2);
            mm_idx_j[1] = _mm_set1_epi32(-1);
            for (j = 0; j < height; j += 2) {
                mm_idx_j[0] = _mm_add_epi32(mm_idx_j[0], mm_two);
                mm_idx_j[1] = _mm_add_epi32(mm_idx_j[1], mm_two);
                mm_idx_k = _mm_set_epi32(3, 2, 1, 0);
                // 1st row
                mm_c[0] = _mm_loadu_si128((const __m128i *)&deriv[0][idx1]);
                mm_c[2] = _mm_loadu_si128((const __m128i *)&deriv[1][idx1]);
                // 2nd row
                mm_c[6] = _mm_loadu_si128((const __m128i *)&deriv[0][idx2]);
                mm_c[8] = _mm_loadu_si128((const __m128i *)&deriv[1][idx2]);
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

                idx1 += (i_deriv << 1);
                idx2 += (i_deriv << 1);

                // Calculate resi coefficients first
                mm_tmp[2] = _mm_srli_si128(mm_residue[0], 4);
                mm_tmp[3] = _mm_srli_si128(mm_residue[1], 4);
                // 1st row
                mm_tmp[0] = _mm_srli_si128(mm_c[0], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[6], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[6], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][6]);
                _mm_storel_epi64((__m128i *)&coef[1][6], mm_intermediate[3]);
                // 2nd row
                mm_tmp[0] = _mm_srli_si128(mm_c[1], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[7], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[7], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][6]);
                _mm_storel_epi64((__m128i *)&coef[2][6], mm_intermediate[3]);
                // 3rd row
                mm_tmp[0] = _mm_srli_si128(mm_c[2], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[8], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[2], mm_c[8], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[3][6]);
                _mm_storel_epi64((__m128i *)&coef[3][6], mm_intermediate[3]);
                // 4th row
                mm_tmp[0] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[9], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[3], mm_c[9], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[4][6]);
                _mm_storel_epi64((__m128i *)&coef[4][6], mm_intermediate[3]);
                // 5th row
                mm_tmp[0] = _mm_srli_si128(mm_c[4], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[10], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[4], mm_c[10], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[5][6]);
                _mm_storel_epi64((__m128i *)&coef[5][6], mm_intermediate[3]);
                // 6th row
                mm_tmp[0] = _mm_srli_si128(mm_c[5], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[11], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[5], mm_c[11], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[6][6]);
                _mm_storel_epi64((__m128i *)&coef[6][6], mm_intermediate[3]);
                //Start calculation of coefficient matrix
                // 1st row
                mm_tmp[0] = _mm_srli_si128(mm_c[0], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[6], 4);
                // 1st col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[6], mm_c[0], mm_c[6], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][0]);
                _mm_storel_epi64((__m128i *)&coef[1][0], mm_intermediate[3]);
                // 2nd col of row and 1st col of 2nd row
                mm_tmp[2] = _mm_srli_si128(mm_c[1], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[7], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[6], mm_c[1], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][1]);
                _mm_storel_epi64((__m128i *)&coef[1][1], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[2][0], mm_intermediate[3]);
                // 3rd col of row and 1st col of 3rd row
                mm_tmp[2] = _mm_srli_si128(mm_c[2], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[8], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[6], mm_c[2], mm_c[8], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][2]);
                _mm_storel_epi64((__m128i *)&coef[1][2], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[3][0], mm_intermediate[3]);
                // 4th col of row and 1st col of 4th row
                mm_tmp[2] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[9], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[6], mm_c[3], mm_c[9], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][3]);
                _mm_storel_epi64((__m128i *)&coef[1][3], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[4][0], mm_intermediate[3]);
                // 5th col of row and 1st col of the 5th row
                mm_tmp[2] = _mm_srli_si128(mm_c[4], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[10], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[6], mm_c[4], mm_c[10], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][4]);
                _mm_storel_epi64((__m128i *)&coef[1][4], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[5][0], mm_intermediate[3]);
                // 6th col of row and 1st col of the 6th row
                mm_tmp[2] = _mm_srli_si128(mm_c[5], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[11], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[6], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][5]);
                _mm_storel_epi64((__m128i *)&coef[1][5], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[6][0], mm_intermediate[3]);
                // 2nd row
                mm_tmp[0] = _mm_srli_si128(mm_c[1], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[7], 4);
                // 2nd col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[7], mm_c[1], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][1]);
                _mm_storel_epi64((__m128i *)&coef[2][1], mm_intermediate[3]);
                // 3rd col of row and 2nd col of 3rd row
                mm_tmp[2] = _mm_srli_si128(mm_c[2], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[8], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[7], mm_c[2], mm_c[8], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][2]);
                _mm_storel_epi64((__m128i *)&coef[2][2], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[3][1], mm_intermediate[3]);
                // 4th col of row and 2nd col of 4th row
                mm_tmp[2] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[9], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[7], mm_c[3], mm_c[9], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][3]);
                _mm_storel_epi64((__m128i *)&coef[2][3], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[4][1], mm_intermediate[3]);
                // 5th col of row and 1st col of the 5th row
                mm_tmp[2] = _mm_srli_si128(mm_c[4], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[10], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[7], mm_c[4], mm_c[10], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][4]);
                _mm_storel_epi64((__m128i *)&coef[2][4], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[5][1], mm_intermediate[3]);
                // 6th col of row and 1st col of the 6th row
                mm_tmp[2] = _mm_srli_si128(mm_c[5], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[11], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[7], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][5]);
                _mm_storel_epi64((__m128i *)&coef[2][5], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[6][1], mm_intermediate[3]);
                // 3rd row
                mm_tmp[0] = _mm_srli_si128(mm_c[2], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[8], 4);
                //3rd Col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[2], mm_c[8], mm_c[2], mm_c[8], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[3][2]);
                _mm_storel_epi64((__m128i *)&coef[3][2], mm_intermediate[3]);
                // 4th col of row and 3rd col of 4th row
                mm_tmp[2] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[9], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[2], mm_c[8], mm_c[3], mm_c[9], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[3][3]);
                _mm_storel_epi64((__m128i *)&coef[3][3], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[4][2], mm_intermediate[3]);
                // 5th col of row and 1st col of the 5th row
                mm_tmp[2] = _mm_srli_si128(mm_c[4], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[10], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[2], mm_c[8], mm_c[4], mm_c[10], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[3][4]);
                _mm_storel_epi64((__m128i *)&coef[3][4], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[5][2], mm_intermediate[3]);
                // 6th col of row and 1st col of the 6th row
                mm_tmp[2] = _mm_srli_si128(mm_c[5], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[11], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[2], mm_c[8], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[3][5]);
                _mm_storel_epi64((__m128i *)&coef[3][5], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[6][2], mm_intermediate[3]);
                // 4th row
                mm_tmp[0] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[9], 4);
                // 4th col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[3], mm_c[9], mm_c[3], mm_c[9], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[4][3]);
                _mm_storel_epi64((__m128i *)&coef[4][3], mm_intermediate[3]);
                // 5th col of row and 1st col of the 5th row
                mm_tmp[2] = _mm_srli_si128(mm_c[4], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[10], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[3], mm_c[9], mm_c[4], mm_c[10], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[4][4]);
                _mm_storel_epi64((__m128i *)&coef[4][4], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[5][3], mm_intermediate[3]);
                // 6th col of row and 1st col of the 6th row
                mm_tmp[2] = _mm_srli_si128(mm_c[5], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[11], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[3], mm_c[9], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[4][5]);
                _mm_storel_epi64((__m128i *)&coef[4][5], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[6][3], mm_intermediate[3]);
                // 5th row
                mm_tmp[0] = _mm_srli_si128(mm_c[4], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[10], 4);
                // 5th col of row and 1st col of the 5th row
                CALC_EQUAL_COEFF_8PXLS(mm_c[4], mm_c[10], mm_c[4], mm_c[10], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[5][4]);
                _mm_storel_epi64((__m128i *)&coef[5][4], mm_intermediate[3]);
                // 6th col of row and 1st col of the 6th row
                mm_tmp[2] = _mm_srli_si128(mm_c[5], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[11], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[4], mm_c[10], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[5][5]);
                _mm_storel_epi64((__m128i *)&coef[5][5], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[6][4], mm_intermediate[3]);
                // 6th row
                mm_tmp[0] = _mm_srli_si128(mm_c[5], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[11], 4);
                // 5th col of row and 1st col of the 5th row
                CALC_EQUAL_COEFF_8PXLS(mm_c[5], mm_c[11], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[6][5]);
                _mm_storel_epi64((__m128i *)&coef[6][5], mm_intermediate[3]);

            }
        }
        else {
            __m128i mm_c[8];
            //  mm_c - map
            //  C for 1st row of pixels
            //  mm_c[0] = iC[0][i] | iC[0][i+1] | iC[0][i+2] | iC[0][i+3]
            //  mm_c[1] = iC[1][i] | iC[1][i+1] | iC[1][i+2] | iC[1][i+3]
            //  mm_c[2] = iC[2][i] | iC[2][i+1] | iC[2][i+2] | iC[2][i+3]
            //  mm_c[3] = iC[3][i] | iC[3][i+1] | iC[3][i+2] | iC[3][i+3]
            //  C for 2nd row of pixels
            //  mm_c[4] = iC[0][i] | iC[0][i+1] | iC[0][i+2] | iC[0][i+3]
            //  mm_c[5] = iC[1][i] | iC[1][i+1] | iC[1][i+2] | iC[1][i+3]
            //  mm_c[6] = iC[2][i] | iC[2][i+1] | iC[2][i+2] | iC[2][i+3]
            //  mm_c[7] = iC[3][i] | iC[3][i+1] | iC[3][i+2] | iC[3][i+3]
            idx1 = 0;
            idx2 = i_deriv;
            mm_idx_j[0] = _mm_set1_epi32(-2);
            mm_idx_j[1] = _mm_set1_epi32(-1);
            for (j = 0; j < height; j += 2) {
                mm_idx_j[0] = _mm_add_epi32(mm_idx_j[0], mm_two);
                mm_idx_j[1] = _mm_add_epi32(mm_idx_j[1], mm_two);
                mm_idx_k = _mm_set_epi32(3, 2, 1, 0);

                mm_idx_k = _mm_add_epi32(mm_idx_k, mm_four);
                mm_c[0] = _mm_loadu_si128((const __m128i *)&deriv[0][idx1]);
                mm_c[2] = _mm_loadu_si128((const __m128i *)&deriv[1][idx1]);
                mm_c[4] = _mm_loadu_si128((const __m128i *)&deriv[0][idx2]);
                mm_c[6] = _mm_loadu_si128((const __m128i *)&deriv[1][idx2]);
                mm_c[1] = _mm_mullo_epi32(mm_idx_k, mm_c[0]);
                mm_c[3] = _mm_mullo_epi32(mm_idx_j[0], mm_c[0]);
                mm_c[5] = _mm_mullo_epi32(mm_idx_k, mm_c[4]);
                mm_c[7] = _mm_mullo_epi32(mm_idx_j[1], mm_c[4]);
                mm_residue[0] = _mm_loadl_epi64((const __m128i *)&resi[idx1]);
                mm_residue[1] = _mm_loadl_epi64((const __m128i *)&resi[idx2]);
                mm_tmp[0] = _mm_mullo_epi32(mm_idx_j[0], mm_c[2]);
                mm_tmp[1] = _mm_mullo_epi32(mm_idx_k, mm_c[2]);
                mm_tmp[2] = _mm_mullo_epi32(mm_idx_j[1], mm_c[6]);
                mm_tmp[3] = _mm_mullo_epi32(mm_idx_k, mm_c[6]);
                mm_residue[0] = _mm_cvtepi16_epi32(mm_residue[0]);
                mm_residue[1] = _mm_cvtepi16_epi32(mm_residue[1]);
                mm_c[1] = _mm_add_epi32(mm_c[1], mm_tmp[0]);
                mm_c[3] = _mm_sub_epi32(mm_c[3], mm_tmp[1]);
                mm_c[5] = _mm_add_epi32(mm_c[5], mm_tmp[2]);
                mm_c[7] = _mm_sub_epi32(mm_c[7], mm_tmp[3]);
                mm_residue[0] = _mm_slli_epi32(mm_residue[0], 3);
                mm_residue[1] = _mm_slli_epi32(mm_residue[1], 3);

                idx1 += (i_deriv << 1);
                idx2 += (i_deriv << 1);

                //Start calculation of coefficient matrix
                // 1st row
                mm_tmp[0] = _mm_srli_si128(mm_c[0], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[4], 4);
                // 1st col of row
                mm_intermediate[0] = _mm_mul_epi32(mm_c[0], mm_c[0]);
                mm_intermediate[1] = _mm_mul_epi32(mm_tmp[0], mm_tmp[0]);
                mm_intermediate[2] = _mm_mul_epi32(mm_c[4], mm_c[4]);
                mm_intermediate[3] = _mm_mul_epi32(mm_tmp[1], mm_tmp[1]);
                mm_intermediate[2] = _mm_add_epi64(mm_intermediate[0], mm_intermediate[2]);
                mm_intermediate[3] = _mm_add_epi64(mm_intermediate[1], mm_intermediate[3]);
                mm_intermediate[0] = _mm_loadl_epi64((const __m128i *)&coef[1][0]);
                mm_intermediate[3] = _mm_add_epi64(mm_intermediate[2], mm_intermediate[3]);
                mm_intermediate[1] = _mm_srli_si128(mm_intermediate[3], 8);
                mm_intermediate[3] = _mm_add_epi64(mm_intermediate[1], mm_intermediate[3]);
                mm_intermediate[3] = _mm_add_epi64(mm_intermediate[0], mm_intermediate[3]);
                //CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[4], mm_c[0], mm_c[4], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][0]);
                _mm_storel_epi64((__m128i *)&coef[1][0], mm_intermediate[3]);
                // 2nd col of row and 1st col of 2nd row
                mm_tmp[2] = _mm_srli_si128(mm_c[1], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[5], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[4], mm_c[1], mm_c[5], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][1]);
                _mm_storel_epi64((__m128i *)&coef[1][1], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[2][0], mm_intermediate[3]);
                // 3rd col of row and 1st col of 3rd row
                mm_tmp[2] = _mm_srli_si128(mm_c[2], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[6], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[4], mm_c[2], mm_c[6], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][2]);
                _mm_storel_epi64((__m128i *)&coef[1][2], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[3][0], mm_intermediate[3]);
                // 4th col of row and 1st col of 4th row
                mm_tmp[2] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[7], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[4], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][3]);
                _mm_storel_epi64((__m128i *)&coef[1][3], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[4][0], mm_intermediate[3]);
                // 5th col of row
                mm_tmp[2] = _mm_srli_si128(mm_residue[0], 4);
                mm_tmp[3] = _mm_srli_si128(mm_residue[1], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[0], mm_c[4], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][4]);
                _mm_storel_epi64((__m128i *)&coef[1][4], mm_intermediate[3]);
                // 2nd row
                mm_tmp[0] = _mm_srli_si128(mm_c[1], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[5], 4);
                // 2nd col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[5], mm_c[1], mm_c[5], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][1]);
                _mm_storel_epi64((__m128i *)&coef[2][1], mm_intermediate[3]);
                // 3rd col of row and 2nd col of 3rd row
                mm_tmp[2] = _mm_srli_si128(mm_c[2], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[6], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[5], mm_c[2], mm_c[6], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][2]);
                _mm_storel_epi64((__m128i *)&coef[2][2], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[3][1], mm_intermediate[3]);
                // 4th col of row and 2nd col of 4th row
                mm_tmp[2] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[7], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[5], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][3]);
                _mm_storel_epi64((__m128i *)&coef[2][3], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[4][1], mm_intermediate[3]);
                // 5th col of row
                mm_tmp[2] = _mm_srli_si128(mm_residue[0], 4);
                mm_tmp[3] = _mm_srli_si128(mm_residue[1], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[1], mm_c[5], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][4]);
                _mm_storel_epi64((__m128i *)&coef[2][4], mm_intermediate[3]);
                // 3rd row
                mm_tmp[0] = _mm_srli_si128(mm_c[2], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[6], 4);
                //3rd Col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[2], mm_c[6], mm_c[2], mm_c[6], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[3][2]);
                _mm_storel_epi64((__m128i *)&coef[3][2], mm_intermediate[3]);
                // 4th col of row and 3rd col of 4th row
                mm_tmp[2] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[3] = _mm_srli_si128(mm_c[7], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[2], mm_c[6], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[3][3]);
                _mm_storel_epi64((__m128i *)&coef[3][3], mm_intermediate[3]);
                _mm_storel_epi64((__m128i *)&coef[4][2], mm_intermediate[3]);
                // 5th col of row
                mm_tmp[2] = _mm_srli_si128(mm_residue[0], 4);
                mm_tmp[3] = _mm_srli_si128(mm_residue[1], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[2], mm_c[6], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[3][4]);
                _mm_storel_epi64((__m128i *)&coef[3][4], mm_intermediate[3]);
                // 4th row
                mm_tmp[0] = _mm_srli_si128(mm_c[3], 4);
                mm_tmp[1] = _mm_srli_si128(mm_c[7], 4);
                // 4th col of row
                CALC_EQUAL_COEFF_8PXLS(mm_c[3], mm_c[7], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[4][3]);
                _mm_storel_epi64((__m128i *)&coef[4][3], mm_intermediate[3]);
                // 5th col of row
                mm_tmp[2] = _mm_srli_si128(mm_residue[0], 4);
                mm_tmp[3] = _mm_srli_si128(mm_residue[1], 4);
                CALC_EQUAL_COEFF_8PXLS(mm_c[3], mm_c[7], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[4][4]);
                _mm_storel_epi64((__m128i *)&coef[4][4], mm_intermediate[3]);

            }
        }
#undef CALC_EQUAL_COEFF_8PXLS

        return;
    }

#define CALC_EQUAL_COEFF_16PXLS(x1,x2,y1,y2,tmp0,tmp1,tmp2,tmp3,inter0,inter1,inter2,inter3,load_location, m0, m1, m2)      \
    {                                                                                                          \
        inter0 = _mm256_mul_epi32(x1, y1);                                                                     \
        inter1 = _mm256_mul_epi32(tmp0, tmp2);                                                                 \
        inter2 = _mm256_mul_epi32(x2, y2);                                                                     \
        inter3 = _mm256_mul_epi32(tmp1, tmp3);                                                                 \
        inter2 = _mm256_add_epi64(inter0, inter2);                                                             \
        inter3 = _mm256_add_epi64(inter1, inter3);                                                             \
        m0 = _mm_loadl_epi64(load_location);                                                                   \
        inter3 = _mm256_add_epi64(inter2, inter3);                                                             \
        m1 = _mm_add_epi64(_mm256_castsi256_si128(inter3), _mm256_extracti128_si256(inter3, 1));               \
        m2 = _mm_srli_si128(m1, 8);                                                                            \
        m1 = _mm_add_epi64(m1, m2);                                                                            \
        m0 = _mm_add_epi64(m0, m1);                                                                            \
    }

    if (vertex_num == 3) {
        __m256i mm_two, mm_eight;
        __m256i mm_tmp[4];
        __m256i mm_intermediate[4];
        __m256i mm_idx_k, mm_idx_j[2];
        __m256i mm_residue[2];
        __m256i mm_c[12];
        __m128i m0, m1, m2;
        // Add directly to indexes to get new index
        mm_two = _mm256_set1_epi32(2);
        mm_eight = _mm256_set1_epi32(8);
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
        idx1 = -2 * i_deriv - 8;
        idx2 = -i_deriv - 8;
        mm_idx_j[0] = _mm256_set1_epi32(-2);
        mm_idx_j[1] = _mm256_set1_epi32(-1);
        for (j = 0; j < height; j += 2) {
            mm_idx_j[0] = _mm256_add_epi32(mm_idx_j[0], mm_two);
            mm_idx_j[1] = _mm256_add_epi32(mm_idx_j[1], mm_two);
            mm_idx_k = _mm256_set_epi32(-1, -2, -3, -4, -5, -6, -7, -8);
            idx1 += (i_deriv << 1);
            idx2 += (i_deriv << 1);
            for (k = 0; k < width; k += 8) {
                idx1 += 8;
                idx2 += 8;
                mm_idx_k = _mm256_add_epi32(mm_idx_k, mm_eight);
                // 1st row
                mm_c[0] = _mm256_loadu_si256((const __m256i *)&deriv[0][idx1]);
                mm_c[2] = _mm256_loadu_si256((const __m256i *)&deriv[1][idx1]);
                // 2nd row
                mm_c[6] = _mm256_loadu_si256((const __m256i *)&deriv[0][idx2]);
                mm_c[8] = _mm256_loadu_si256((const __m256i *)&deriv[1][idx2]);
                // 1st row
                mm_c[1] = _mm256_mullo_epi32(mm_idx_k, mm_c[0]);
                mm_c[3] = _mm256_mullo_epi32(mm_idx_k, mm_c[2]);
                mm_c[4] = _mm256_mullo_epi32(mm_idx_j[0], mm_c[0]);
                mm_c[5] = _mm256_mullo_epi32(mm_idx_j[0], mm_c[2]);
                // 2nd row
                mm_c[7]  = _mm256_mullo_epi32(mm_idx_k, mm_c[6]);
                mm_c[9]  = _mm256_mullo_epi32(mm_idx_k, mm_c[8]);
                mm_c[10] = _mm256_mullo_epi32(mm_idx_j[1], mm_c[6]);
                mm_c[11] = _mm256_mullo_epi32(mm_idx_j[1], mm_c[8]);
                // Residue
                m0 = _mm_loadu_si128((const __m128i *)&resi[idx1]);
                m1 = _mm_loadu_si128((const __m128i *)&resi[idx2]);
                mm_residue[0] = _mm256_cvtepi16_epi32(m0);
                mm_residue[1] = _mm256_cvtepi16_epi32(m1);
                mm_residue[0] = _mm256_slli_epi32(mm_residue[0], 3);
                mm_residue[1] = _mm256_slli_epi32(mm_residue[1], 3);
                // Calculate resi coefficients first
                mm_tmp[2] = _mm256_srli_si256(mm_residue[0], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_residue[1], 4);
                // 1st row
                mm_tmp[0] = _mm256_srli_si256(mm_c[0], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[6], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_16PXLS(mm_c[0], mm_c[6], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][6], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[1][6], m0);
                // 2nd row
                mm_tmp[0] = _mm256_srli_si256(mm_c[1], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[7], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_16PXLS(mm_c[1], mm_c[7], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][6], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[2][6], m0);
                // 3rd row
                mm_tmp[0] = _mm256_srli_si256(mm_c[2], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[8], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_16PXLS(mm_c[2], mm_c[8], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[3][6], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[3][6], m0);
                // 4th row
                mm_tmp[0] = _mm256_srli_si256(mm_c[3], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[9], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_16PXLS(mm_c[3], mm_c[9], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[4][6], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[4][6], m0);
                // 5th row
                mm_tmp[0] = _mm256_srli_si256(mm_c[4], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[10], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_16PXLS(mm_c[4], mm_c[10], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[5][6], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[5][6], m0);
                // 6th row
                mm_tmp[0] = _mm256_srli_si256(mm_c[5], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[11], 4);
                // 7th col of row
                CALC_EQUAL_COEFF_16PXLS(mm_c[5], mm_c[11], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[6][6], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[6][6], m0);
                //Start calculation of coefficient matrix
                // 1st row
                mm_tmp[0] = _mm256_srli_si256(mm_c[0], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[6], 4);
                // 1st col of row
                CALC_EQUAL_COEFF_16PXLS(mm_c[0], mm_c[6], mm_c[0], mm_c[6], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][0], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[1][0], m0);
                // 2nd col of row and 1st col of 2nd row
                mm_tmp[2] = _mm256_srli_si256(mm_c[1], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[7], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[0], mm_c[6], mm_c[1], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][1], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[1][1], m0);
                _mm_storel_epi64((__m128i *)&coef[2][0], m0);
                // 3rd col of row and 1st col of 3rd row
                mm_tmp[2] = _mm256_srli_si256(mm_c[2], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[8], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[0], mm_c[6], mm_c[2], mm_c[8], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][2], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[1][2], m0);
                _mm_storel_epi64((__m128i *)&coef[3][0], m0);
                // 4th col of row and 1st col of 4th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[3], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[9], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[0], mm_c[6], mm_c[3], mm_c[9], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][3], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[1][3], m0);
                _mm_storel_epi64((__m128i *)&coef[4][0], m0);
                // 5th col of row and 1st col of the 5th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[4], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[10], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[0], mm_c[6], mm_c[4], mm_c[10], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][4], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[1][4], m0);
                _mm_storel_epi64((__m128i *)&coef[5][0], m0);
                // 6th col of row and 1st col of the 6th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[5], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[11], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[0], mm_c[6], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][5], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[1][5], m0);
                _mm_storel_epi64((__m128i *)&coef[6][0], m0);
                // 2nd row
                mm_tmp[0] = _mm256_srli_si256(mm_c[1], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[7], 4);
                // 2nd col of row
                CALC_EQUAL_COEFF_16PXLS(mm_c[1], mm_c[7], mm_c[1], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][1], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[2][1], m0);
                // 3rd col of row and 2nd col of 3rd row
                mm_tmp[2] = _mm256_srli_si256(mm_c[2], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[8], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[1], mm_c[7], mm_c[2], mm_c[8], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][2], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[2][2], m0);
                _mm_storel_epi64((__m128i *)&coef[3][1], m0);
                // 4th col of row and 2nd col of 4th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[3], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[9], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[1], mm_c[7], mm_c[3], mm_c[9], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][3], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[2][3], m0);
                _mm_storel_epi64((__m128i *)&coef[4][1], m0);
                // 5th col of row and 1st col of the 5th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[4], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[10], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[1], mm_c[7], mm_c[4], mm_c[10], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][4], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[2][4], m0);
                _mm_storel_epi64((__m128i *)&coef[5][1], m0);
                // 6th col of row and 1st col of the 6th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[5], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[11], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[1], mm_c[7], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][5], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[2][5], m0);
                _mm_storel_epi64((__m128i *)&coef[6][1], m0);
                // 3rd row
                mm_tmp[0] = _mm256_srli_si256(mm_c[2], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[8], 4);
                //3rd Col of row
                CALC_EQUAL_COEFF_16PXLS(mm_c[2], mm_c[8], mm_c[2], mm_c[8], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[3][2], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[3][2], m0);
                // 4th col of row and 3rd col of 4th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[3], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[9], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[2], mm_c[8], mm_c[3], mm_c[9], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[3][3], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[3][3], m0);
                _mm_storel_epi64((__m128i *)&coef[4][2], m0);
                // 5th col of row and 1st col of the 5th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[4], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[10], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[2], mm_c[8], mm_c[4], mm_c[10], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[3][4], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[3][4], m0);
                _mm_storel_epi64((__m128i *)&coef[5][2], m0);
                // 6th col of row and 1st col of the 6th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[5], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[11], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[2], mm_c[8], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[3][5], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[3][5], m0);
                _mm_storel_epi64((__m128i *)&coef[6][2], m0);
                // 4th row
                mm_tmp[0] = _mm256_srli_si256(mm_c[3], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[9], 4);
                // 4th col of row
                CALC_EQUAL_COEFF_16PXLS(mm_c[3], mm_c[9], mm_c[3], mm_c[9], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[4][3], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[4][3], m0);
                // 5th col of row and 1st col of the 5th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[4], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[10], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[3], mm_c[9], mm_c[4], mm_c[10], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[4][4], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[4][4], m0);
                _mm_storel_epi64((__m128i *)&coef[5][3], m0);
                // 6th col of row and 1st col of the 6th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[5], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[11], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[3], mm_c[9], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[4][5], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[4][5], m0);
                _mm_storel_epi64((__m128i *)&coef[6][3], m0);
                // 5th row
                mm_tmp[0] = _mm256_srli_si256(mm_c[4], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[10], 4);
                // 5th col of row and 1st col of the 5th row
                CALC_EQUAL_COEFF_16PXLS(mm_c[4], mm_c[10], mm_c[4], mm_c[10], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[5][4], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[5][4], m0);
                // 6th col of row and 1st col of the 6th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[5], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[11], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[4], mm_c[10], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[5][5], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[5][5], m0);
                _mm_storel_epi64((__m128i *)&coef[6][4], m0);
                // 6th row
                mm_tmp[0] = _mm256_srli_si256(mm_c[5], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[11], 4);
                // 5th col of row and 1st col of the 5th row
                CALC_EQUAL_COEFF_16PXLS(mm_c[5], mm_c[11], mm_c[5], mm_c[11], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[6][5], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[6][5], m0);
            }
            idx1 -= (width);
            idx2 -= (width);
        }
    }
    else {
        __m256i mm_two, mm_eight;
        __m256i mm_tmp[4];
        __m256i mm_intermediate[4];
        __m256i mm_idx_k, mm_idx_j[2];
        __m256i mm_residue[2];
        __m128i m0, m1, m2;
        __m256i mm_c[8];
        //  mm_c - map
        //  C for 1st row of pixels
        //  mm_c[0] = iC[0][i] | iC[0][i+1] | iC[0][i+2] | iC[0][i+3]
        //  mm_c[1] = iC[1][i] | iC[1][i+1] | iC[1][i+2] | iC[1][i+3]
        //  mm_c[2] = iC[2][i] | iC[2][i+1] | iC[2][i+2] | iC[2][i+3]
        //  mm_c[3] = iC[3][i] | iC[3][i+1] | iC[3][i+2] | iC[3][i+3]
        //  C for 2nd row of pixels
        //  mm_c[4] = iC[0][i] | iC[0][i+1] | iC[0][i+2] | iC[0][i+3]
        //  mm_c[5] = iC[1][i] | iC[1][i+1] | iC[1][i+2] | iC[1][i+3]
        //  mm_c[6] = iC[2][i] | iC[2][i+1] | iC[2][i+2] | iC[2][i+3]
        //  mm_c[7] = iC[3][i] | iC[3][i+1] | iC[3][i+2] | iC[3][i+3]
        idx1 = -2 * i_deriv - 8;
        idx2 = -i_deriv - 8;
        mm_two = _mm256_set1_epi32(2);
        mm_eight = _mm256_set1_epi32(8);
        mm_idx_j[0] = _mm256_set1_epi32(-2);
        mm_idx_j[1] = _mm256_set1_epi32(-1);
        for (j = 0; j < height; j += 2) {
            mm_idx_j[0] = _mm256_add_epi32(mm_idx_j[0], mm_two);
            mm_idx_j[1] = _mm256_add_epi32(mm_idx_j[1], mm_two);
            mm_idx_k = _mm256_set_epi32(-1, -2, -3, -4, -5, -6, -7, -8);
            idx1 += (i_deriv << 1);
            idx2 += (i_deriv << 1);
            for (k = 0; k < width; k += 8) {
                idx1 += 8;
                idx2 += 8;
                mm_idx_k = _mm256_add_epi32(mm_idx_k, mm_eight);
                mm_c[0] = _mm256_loadu_si256((const __m256i *)&deriv[0][idx1]);
                mm_c[2] = _mm256_loadu_si256((const __m256i *)&deriv[1][idx1]);
                mm_c[4] = _mm256_loadu_si256((const __m256i *)&deriv[0][idx2]);
                mm_c[6] = _mm256_loadu_si256((const __m256i *)&deriv[1][idx2]);
                mm_c[1] = _mm256_mullo_epi32(mm_idx_k, mm_c[0]);
                mm_c[3] = _mm256_mullo_epi32(mm_idx_j[0], mm_c[0]);
                mm_c[5] = _mm256_mullo_epi32(mm_idx_k, mm_c[4]);
                mm_c[7] = _mm256_mullo_epi32(mm_idx_j[1], mm_c[4]);
                m0 = _mm_loadu_si128((const __m128i *)&resi[idx1]);
                m1 = _mm_loadu_si128((const __m128i *)&resi[idx2]);
                mm_tmp[0] = _mm256_mullo_epi32(mm_idx_j[0], mm_c[2]);
                mm_tmp[1] = _mm256_mullo_epi32(mm_idx_k, mm_c[2]);
                mm_tmp[2] = _mm256_mullo_epi32(mm_idx_j[1], mm_c[6]);
                mm_tmp[3] = _mm256_mullo_epi32(mm_idx_k, mm_c[6]);
                mm_residue[0] = _mm256_cvtepi16_epi32(m0);
                mm_residue[1] = _mm256_cvtepi16_epi32(m1);
                mm_c[1] = _mm256_add_epi32(mm_c[1], mm_tmp[0]);
                mm_c[3] = _mm256_sub_epi32(mm_c[3], mm_tmp[1]);
                mm_c[5] = _mm256_add_epi32(mm_c[5], mm_tmp[2]);
                mm_c[7] = _mm256_sub_epi32(mm_c[7], mm_tmp[3]);
                mm_residue[0] = _mm256_slli_epi32(mm_residue[0], 3);
                mm_residue[1] = _mm256_slli_epi32(mm_residue[1], 3);
                //Start calculation of coefficient matrix
                // 1st row
                mm_tmp[0] = _mm256_srli_si256(mm_c[0], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[4], 4);
                // 1st col of row
                CALC_EQUAL_COEFF_16PXLS(mm_c[0], mm_c[4], mm_c[0], mm_c[4], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][0], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[1][0], m0);
                // 2nd col of row and 1st col of 2nd row
                mm_tmp[2] = _mm256_srli_si256(mm_c[1], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[5], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[0], mm_c[4], mm_c[1], mm_c[5], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][1], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[1][1], m0);
                _mm_storel_epi64((__m128i *)&coef[2][0], m0);
                // 3rd col of row and 1st col of 3rd row
                mm_tmp[2] = _mm256_srli_si256(mm_c[2], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[6], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[0], mm_c[4], mm_c[2], mm_c[6], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][2], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[1][2], m0);
                _mm_storel_epi64((__m128i *)&coef[3][0], m0);
                // 4th col of row and 1st col of 4th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[3], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[7], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[0], mm_c[4], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][3], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[1][3], m0);
                _mm_storel_epi64((__m128i *)&coef[4][0], m0);
                // 5th col of row
                mm_tmp[2] = _mm256_srli_si256(mm_residue[0], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_residue[1], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[0], mm_c[4], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[1][4], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[1][4], m0);
                // 2nd row
                mm_tmp[0] = _mm256_srli_si256(mm_c[1], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[5], 4);
                // 2nd col of row
                CALC_EQUAL_COEFF_16PXLS(mm_c[1], mm_c[5], mm_c[1], mm_c[5], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][1], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[2][1], m0);
                // 3rd col of row and 2nd col of 3rd row
                mm_tmp[2] = _mm256_srli_si256(mm_c[2], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[6], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[1], mm_c[5], mm_c[2], mm_c[6], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][2], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[2][2], m0);
                _mm_storel_epi64((__m128i *)&coef[3][1], m0);
                // 4th col of row and 2nd col of 4th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[3], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[7], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[1], mm_c[5], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][3], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[2][3], m0);
                _mm_storel_epi64((__m128i *)&coef[4][1], m0);
                // 5th col of row
                mm_tmp[2] = _mm256_srli_si256(mm_residue[0], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_residue[1], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[1], mm_c[5], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[2][4], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[2][4], m0);
                // 3rd row
                mm_tmp[0] = _mm256_srli_si256(mm_c[2], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[6], 4);
                //3rd Col of row
                CALC_EQUAL_COEFF_16PXLS(mm_c[2], mm_c[6], mm_c[2], mm_c[6], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[3][2], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[3][2], m0);
                // 4th col of row and 3rd col of 4th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[3], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[7], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[2], mm_c[6], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[3][3], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[3][3], m0);
                _mm_storel_epi64((__m128i *)&coef[4][2], m0);
                // 5th col of row
                mm_tmp[2] = _mm256_srli_si256(mm_residue[0], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_residue[1], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[2], mm_c[6], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[3][4], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[3][4], m0);
                // 4th row
                mm_tmp[0] = _mm256_srli_si256(mm_c[3], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[7], 4);
                // 4th col of row
                CALC_EQUAL_COEFF_16PXLS(mm_c[3], mm_c[7], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[4][3], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[4][3], m0);
                // 5th col of row
                mm_tmp[2] = _mm256_srli_si256(mm_residue[0], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_residue[1], 4);
                CALC_EQUAL_COEFF_16PXLS(mm_c[3], mm_c[7], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], mm_intermediate[0], mm_intermediate[1], mm_intermediate[2], mm_intermediate[3], (const __m128i *)&coef[4][4], m0, m1, m2);
                _mm_storel_epi64((__m128i *)&coef[4][4], m0);
            }
            idx1 -= (width);
            idx2 -= (width);
        }
    }

#undef CALC_EQUAL_COEFF_16PXLS

}
