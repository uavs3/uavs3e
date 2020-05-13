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

#include "avx2.h"
#include "../sse/sse.h"

void affine_sobel_flt_hor_avx2(pel *pred, int i_pred, s16 *deriv, int i_deriv, int width, int height)
{
    int j, col, row;
    __m256i mm256_pred[4];
    __m256i mm256_pred2x[2];
    __m256i mm256_tmp[4];
    __m256i mm256_deriv[2];
    __m128i mm_pred[4];
    __m128i mm_pred2x[2];
    __m128i mm_tmp[4];
    __m128i mm_deriv[2];
    assert(!(height % 2));
    assert(!(width % 4));

    for (col = 1; (col + 8) < width; col += 8) {
        pel *p0 = pred + col;
        pel *p2 = p0 + (i_pred << 1);
        pel *p1 = p0 + i_pred;
        pel *p3 = p2 + i_pred;
#if (BIT_DEPTH == 8)
        mm256_pred[0] = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p0 + 3), *(s64*)(p0 - 1)));
        mm256_pred[1] = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p1 + 3), *(s64*)(p1 - 1)));
#else
        mm256_pred[0] = _mm256_set_m128i(_mm_loadu_si128((const __m128i *)(p0 + 3)), _mm_loadu_si128((const __m128i *)(p0 - 1)));
        mm256_pred[1] = _mm256_set_m128i(_mm_loadu_si128((const __m128i *)(p1 + 3)), _mm_loadu_si128((const __m128i *)(p1 - 1)));
#endif
        for (row = 1; row < (height - 1); row += 2) {
#if (BIT_DEPTH == 8)
            mm256_pred[2] = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p2 + 3), *(s64*)(p2 - 1)));
            mm256_pred[3] = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p3 + 3), *(s64*)(p3 - 1)));
#else
            mm256_pred[2] = _mm256_set_m128i(_mm_loadu_si128((const __m128i *)(p2 + 3)), _mm_loadu_si128((const __m128i *)(p2 - 1)));
            mm256_pred[3] = _mm256_set_m128i(_mm_loadu_si128((const __m128i *)(p3 + 3)), _mm_loadu_si128((const __m128i *)(p3 - 1)));
#endif
            mm256_pred2x[0] = _mm256_slli_epi16(mm256_pred[1], 1);
            mm256_pred2x[1] = _mm256_slli_epi16(mm256_pred[2], 1);
            mm256_tmp[0] = _mm256_add_epi16(mm256_pred2x[0], mm256_pred[0]);
            mm256_tmp[2] = _mm256_add_epi16(mm256_pred2x[1], mm256_pred[1]);
            mm256_tmp[0] = _mm256_add_epi16(mm256_tmp[0], mm256_pred[2]);
            mm256_tmp[2] = _mm256_add_epi16(mm256_tmp[2], mm256_pred[3]);
            mm256_pred[0] = mm256_pred[2];
            mm256_pred[1] = mm256_pred[3];
            mm256_tmp[1] = _mm256_srli_si256(mm256_tmp[0], 4);
            mm256_tmp[3] = _mm256_srli_si256(mm256_tmp[2], 4);
            mm256_deriv[0] = _mm256_sub_epi16(mm256_tmp[1], mm256_tmp[0]);
            mm256_deriv[1] = _mm256_sub_epi16(mm256_tmp[3], mm256_tmp[2]);
            p2 += i_pred << 1;
            p3 += i_pred << 1;
            mm256_deriv[0] = _mm256_permute4x64_epi64(mm256_deriv[0], 0xd8);
            mm256_deriv[1] = _mm256_permute4x64_epi64(mm256_deriv[1], 0xd8);
            _mm_storeu_si128((__m128i *)(&deriv[col + (row + 0) * i_deriv]), _mm256_castsi256_si128(mm256_deriv[0]));
            _mm_storeu_si128((__m128i *)(&deriv[col + (row + 1) * i_deriv]), _mm256_castsi256_si128(mm256_deriv[1]));
        }
    }
    if (col + 4 < width) {
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
            mm_pred2x[0] = _mm_slli_epi16(mm_pred[1], 1);
            mm_pred2x[1] = _mm_slli_epi16(mm_pred[2], 1);
            mm_tmp[0] = _mm_add_epi16(mm_pred2x[0], mm_pred[0]);
            mm_tmp[2] = _mm_add_epi16(mm_pred2x[1], mm_pred[1]);
            mm_tmp[0] = _mm_add_epi16(mm_tmp[0], mm_pred[2]);
            mm_tmp[2] = _mm_add_epi16(mm_tmp[2], mm_pred[3]);
            mm_pred[0] = mm_pred[2];
            mm_pred[1] = mm_pred[3];
            mm_tmp[1] = _mm_srli_si128(mm_tmp[0], 4);
            mm_tmp[3] = _mm_srli_si128(mm_tmp[2], 4);
            mm_deriv[0] = _mm_sub_epi16(mm_tmp[1], mm_tmp[0]);
            mm_deriv[1] = _mm_sub_epi16(mm_tmp[3], mm_tmp[2]);
            _mm_storel_epi64((__m128i *)(&deriv[col + (row + 0) * i_deriv]), mm_deriv[0]);
            _mm_storel_epi64((__m128i *)(&deriv[col + (row + 1) * i_deriv]), mm_deriv[1]);
        }
        col += 4;
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
            mm_pred2x[0] = _mm_slli_epi16(mm_pred[1], 1);
            mm_pred2x[1] = _mm_slli_epi16(mm_pred[2], 1);
            mm_tmp[0] = _mm_add_epi16(mm_pred2x[0], mm_pred[0]);
            mm_tmp[2] = _mm_add_epi16(mm_pred2x[1], mm_pred[1]);
            mm_tmp[0] = _mm_add_epi16(mm_tmp[0], mm_pred[2]);
            mm_tmp[2] = _mm_add_epi16(mm_tmp[2], mm_pred[3]);
            mm_pred[0] = mm_pred[2];
            mm_pred[1] = mm_pred[3];
            mm_tmp[1] = _mm_srli_si128(mm_tmp[0], 4);
            mm_tmp[3] = _mm_srli_si128(mm_tmp[2], 4);
            mm_deriv[0] = _mm_sub_epi16(mm_tmp[1], mm_tmp[0]);
            mm_deriv[1] = _mm_sub_epi16(mm_tmp[3], mm_tmp[2]);
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

void affine_sobel_flt_ver_avx2(pel *pred, int i_pred, s16 *deriv, int i_deriv, int width, int height)
{
    int j, col, row;
    __m256i mm256_pred[4];
    __m256i mm256_tmp[6];
    __m256i mm256_derivate[2];
    __m128i mm_pred[4];
    __m128i mm_tmp[6];
    __m128i mm_derivate[2];
    assert(!(height % 2));
    assert(!(width % 4));
    /* Derivates of the rows and columns at the boundary are done at the end of this function */
    /* The value of col and row indicate the columns and rows for which the derivates have already been computed */
    for (col = 1; col + 8 < width; col += 8) {
        pel *p0 = pred + col;
        pel *p2 = p0 + (i_pred << 1);
        pel *p1 = p0 + i_pred;
        pel *p3 = p2 + i_pred;
#if (BIT_DEPTH == 8)
        mm256_pred[0] = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p0 + 3), *(s64*)(p0 - 1)));
        mm256_pred[1] = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p1 + 3), *(s64*)(p1 - 1)));
#else
        mm256_pred[0] = _mm256_set_m128i(_mm_loadu_si128((const __m128i *)(p0 + 3)), _mm_loadu_si128((const __m128i *)(p0 - 1)));
        mm256_pred[1] = _mm256_set_m128i(_mm_loadu_si128((const __m128i *)(p1 + 3)), _mm_loadu_si128((const __m128i *)(p1 - 1)));
#endif
        for (row = 1; row < (height - 1); row += 2) {
#if (BIT_DEPTH == 8)
            mm256_pred[2] = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p2 + 3), *(s64*)(p2 - 1)));
            mm256_pred[3] = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p3 + 3), *(s64*)(p3 - 1)));
#else
            mm256_pred[2] = _mm256_set_m128i(_mm_loadu_si128((const __m128i *)(p2 + 3)), _mm_loadu_si128((const __m128i *)(p2 - 1)));
            mm256_pred[3] = _mm256_set_m128i(_mm_loadu_si128((const __m128i *)(p3 + 3)), _mm_loadu_si128((const __m128i *)(p3 - 1)));
#endif
            mm256_tmp[0] = _mm256_sub_epi16(mm256_pred[2], mm256_pred[0]);
            mm256_tmp[3] = _mm256_sub_epi16(mm256_pred[3], mm256_pred[1]);
            mm256_pred[0] = mm256_pred[2];
            mm256_pred[1] = mm256_pred[3];
            mm256_tmp[1] = _mm256_srli_si256(mm256_tmp[0], 2);
            mm256_tmp[4] = _mm256_srli_si256(mm256_tmp[3], 2);
            mm256_tmp[2] = _mm256_srli_si256(mm256_tmp[0], 4);
            mm256_tmp[5] = _mm256_srli_si256(mm256_tmp[3], 4);
            mm256_tmp[1] = _mm256_slli_epi16(mm256_tmp[1], 1);
            mm256_tmp[4] = _mm256_slli_epi16(mm256_tmp[4], 1);
            mm256_tmp[0] = _mm256_add_epi16(mm256_tmp[0], mm256_tmp[2]);
            mm256_tmp[3] = _mm256_add_epi16(mm256_tmp[3], mm256_tmp[5]);
            mm256_derivate[0] = _mm256_add_epi16(mm256_tmp[0], mm256_tmp[1]);
            mm256_derivate[1] = _mm256_add_epi16(mm256_tmp[3], mm256_tmp[4]);
            p2 += i_pred << 1;
            p3 += i_pred << 1;
            mm256_derivate[0] = _mm256_permute4x64_epi64(mm256_derivate[0], 0xd8);
            mm256_derivate[1] = _mm256_permute4x64_epi64(mm256_derivate[1], 0xd8);
            _mm_storeu_si128((__m128i *)(&deriv[col + (row + 0) * i_deriv]), _mm256_castsi256_si128(mm256_derivate[0]));
            _mm_storeu_si128((__m128i *)(&deriv[col + (row + 1) * i_deriv]), _mm256_castsi256_si128(mm256_derivate[1]));
        }
    }
    if (col + 4 < width) {
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
            mm_tmp[0] = _mm_sub_epi16(mm_pred[2], mm_pred[0]);
            mm_tmp[3] = _mm_sub_epi16(mm_pred[3], mm_pred[1]);
            mm_pred[0] = mm_pred[2];
            mm_pred[1] = mm_pred[3];
            mm_tmp[1] = _mm_srli_si128(mm_tmp[0], 2);
            mm_tmp[4] = _mm_srli_si128(mm_tmp[3], 2);
            mm_tmp[2] = _mm_srli_si128(mm_tmp[0], 4);
            mm_tmp[5] = _mm_srli_si128(mm_tmp[3], 4);
            mm_tmp[1] = _mm_slli_epi16(mm_tmp[1], 1);
            mm_tmp[4] = _mm_slli_epi16(mm_tmp[4], 1);
            mm_tmp[0] = _mm_add_epi16(mm_tmp[0], mm_tmp[2]);
            mm_tmp[3] = _mm_add_epi16(mm_tmp[3], mm_tmp[5]);
            mm_derivate[0] = _mm_add_epi16(mm_tmp[0], mm_tmp[1]);
            mm_derivate[1] = _mm_add_epi16(mm_tmp[3], mm_tmp[4]);
            _mm_storel_epi64((__m128i *)(&deriv[col + (row + 0) * i_deriv]), mm_derivate[0]);
            _mm_storel_epi64((__m128i *)(&deriv[col + (row + 1) * i_deriv]), mm_derivate[1]);
        }
        col += 4;
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
            mm_tmp[0] = _mm_sub_epi16(mm_pred[2], mm_pred[0]);
            mm_tmp[3] = _mm_sub_epi16(mm_pred[3], mm_pred[1]);
            mm_pred[0] = mm_pred[2];
            mm_pred[1] = mm_pred[3];
            mm_tmp[1] = _mm_srli_si128(mm_tmp[0], 2);
            mm_tmp[4] = _mm_srli_si128(mm_tmp[3], 2);
            mm_tmp[2] = _mm_srli_si128(mm_tmp[0], 4);
            mm_tmp[5] = _mm_srli_si128(mm_tmp[3], 4);
            mm_tmp[1] = _mm_slli_epi16(mm_tmp[1], 1);
            mm_tmp[4] = _mm_slli_epi16(mm_tmp[4], 1);
            mm_tmp[0] = _mm_add_epi16(mm_tmp[0], mm_tmp[2]);
            mm_tmp[3] = _mm_add_epi16(mm_tmp[3], mm_tmp[5]);
            mm_derivate[0] = _mm_add_epi16(mm_tmp[0], mm_tmp[1]);
            mm_derivate[1] = _mm_add_epi16(mm_tmp[3], mm_tmp[4]);
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

#if BIT_DEPTH == 8
void affine_coef_computer_avx2(s16 *resi, int i_resi, s16(*deriv)[MAX_CU_DIM], int i_deriv, s64(*coef)[5], int width, int height)
{
    int j, k;
    int idx1 = 0, idx2 = 0;
    if (width == 4) {
        __m128i mm_two, mm_four;
        __m128i mm_tmp[4];
        __m128i mm_intermediate[4];
        __m128i mm_idx_k, mm_idx_j[2];
        __m128i mm_residue[2];

        mm_two = _mm_set1_epi32(2);
        mm_four = _mm_set1_epi32(4);

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

#define CALC_AFFINE_COEF_16PXLS(x1,x2,y1,y2,tmp0,tmp1,tmp2,tmp3,dst)      \
    {                                                                                   \
        mm_intermediate[0] = _mm256_mul_epi32(x1, y1);                                  \
        mm_intermediate[1] = _mm256_mul_epi32(tmp0, tmp2);                              \
        mm_intermediate[2] = _mm256_mul_epi32(x2, y2);                                  \
        mm_intermediate[3] = _mm256_mul_epi32(tmp1, tmp3);                              \
        mm_intermediate[2] = _mm256_add_epi64(mm_intermediate[0], mm_intermediate[2]);  \
        mm_intermediate[3] = _mm256_add_epi64(mm_intermediate[1], mm_intermediate[3]);  \
        mm_intermediate[3] = _mm256_add_epi64(mm_intermediate[2], mm_intermediate[3]);  \
        dst = _mm_add_epi64(_mm256_castsi256_si128(mm_intermediate[3]), _mm256_extracti128_si256(mm_intermediate[3], 1)); \
    }

#define CALC_AFFINE_4COEFS_16PXLS_SRC16_DST32(s0, s1, s2, s3, s4, s5, s6, s7, dst) \
        mm_intermediate[0] = _mm256_madd_epi16(s0, s1);                   \
        mm_intermediate[1] = _mm256_madd_epi16(s2, s3);                   \
        mm_intermediate[2] = _mm256_madd_epi16(s4, s5);                   \
        mm_intermediate[3] = _mm256_madd_epi16(s6, s7);                   \
        mm_intermediate[0] = _mm256_add_epi32(mm_intermediate[0], mm_intermediate[1]);  \
        mm_intermediate[2] = _mm256_add_epi32(mm_intermediate[2], mm_intermediate[3]);  \
        mm_intermediate[0] = _mm256_hadd_epi32(mm_intermediate[0], mm_intermediate[2]); \
        dst = _mm256_hadd_epi32(mm_intermediate[0], mm_intermediate[0]);

    
    __m256i mm_two, mm_eight;
    __m256i mm_tmp[4];
    __m256i mm_intermediate[4];
    __m256i mm_idx_k, mm_idx_j[2];
    __m256i mm_residue[2];
    __m128i m0, m1, m2, m3, m4, m5;
    __m256i mm_c[8];
    __m256i mm_idx_k_s16, mm_idx_j_s16;
    __m256i mm_two_s16, mm_eight_s16;

    idx1 = 0;
    idx2 = i_deriv;
    mm_two = _mm256_set1_epi32(2);
    mm_eight = _mm256_set1_epi32(8);
    mm_two_s16 = _mm256_set1_epi16(2);
    mm_eight_s16 = _mm256_set1_epi16(8);
    mm_idx_j[0] = _mm256_set1_epi32(0);
    mm_idx_j[1] = _mm256_set1_epi32(1);
    mm_idx_j_s16 = _mm256_set_m128i(_mm_set1_epi16(1), _mm_set1_epi16(0));
    for (j = 0; j < height; j += 2) {
        mm_idx_k = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
        mm_idx_k_s16 = _mm256_set_epi16(7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0);
        for (k = 0; k < width; k += 8) {
            m0 = _mm_loadu_si128((const __m128i *)&deriv[0][idx1]);
            m1 = _mm_loadu_si128((const __m128i *)&deriv[1][idx1]);
            m2 = _mm_loadu_si128((const __m128i *)&deriv[0][idx2]);
            m3 = _mm_loadu_si128((const __m128i *)&deriv[1][idx2]);
            m4 = _mm_loadu_si128((const __m128i *)&resi[idx1]);
            m5 = _mm_loadu_si128((const __m128i *)&resi[idx2]);
            if (j < 16 && k < 16) {
                mm_c[0] = _mm256_set_m128i(m2, m0);
                mm_c[2] = _mm256_set_m128i(m3, m1);
                mm_c[4] = _mm256_set_m128i(m5, m4);
                
                mm_tmp[0] = _mm256_mullo_epi16(mm_idx_k_s16, mm_c[0]);
                mm_tmp[1] = _mm256_mullo_epi16(mm_idx_k_s16, mm_c[2]);
                mm_tmp[2] = _mm256_mullo_epi16(mm_idx_j_s16, mm_c[0]);
                mm_tmp[3] = _mm256_mullo_epi16(mm_idx_j_s16, mm_c[2]);
                mm_c[1] = _mm256_add_epi16(mm_tmp[0], mm_tmp[3]);
                mm_c[3] = _mm256_sub_epi16(mm_tmp[2], mm_tmp[1]);
                mm_tmp[0] = _mm256_madd_epi16(mm_c[0], mm_c[0]);    // (11 + 11 + 1)bits
                mm_tmp[1] = _mm256_madd_epi16(mm_c[0], mm_c[1]);    // (11 + 16 + 1)bits
                mm_tmp[2] = _mm256_madd_epi16(mm_c[0], mm_c[2]);    // (11 + 11 + 1)bits
                mm_tmp[3] = _mm256_madd_epi16(mm_c[0], mm_c[3]);    // (11 + 16 + 1)bits
                mm_tmp[0] = _mm256_hadd_epi32(mm_tmp[0], mm_tmp[1]);
                mm_tmp[2] = _mm256_hadd_epi32(mm_tmp[2], mm_tmp[3]);
                mm_tmp[0] = _mm256_hadd_epi32(mm_tmp[0], mm_tmp[2]);
                m0 = _mm_add_epi32(_mm256_castsi256_si128(mm_tmp[0]), _mm256_extracti128_si256(mm_tmp[0], 1));
                mm_tmp[2] = _mm256_loadu_si256((const __m256i*)&coef[1][0]);
                mm_tmp[3] = _mm256_cvtepi32_epi64(m0);
                mm_tmp[2] = _mm256_add_epi64(mm_tmp[2], mm_tmp[3]);
                _mm256_storeu_si256((__m256i*)&coef[1][0], mm_tmp[2]);

                mm_tmp[0] = _mm256_madd_epi16(mm_c[2], mm_c[1]);    // (11 + 16 + 1)bits
                mm_tmp[1] = _mm256_madd_epi16(mm_c[2], mm_c[2]);    // (11 + 11 + 1)bits
                mm_tmp[2] = _mm256_madd_epi16(mm_c[2], mm_c[3]);    // (11 + 16 + 1)bits
                mm_tmp[3] = _mm256_madd_epi16(mm_c[2], mm_c[4]);    // (11 + 9 + 1)bits
                mm_tmp[0] = _mm256_hadd_epi32(mm_tmp[0], mm_tmp[1]);
                mm_tmp[2] = _mm256_hadd_epi32(mm_tmp[2], mm_tmp[3]);
                mm_tmp[0] = _mm256_hadd_epi32(mm_tmp[0], mm_tmp[2]);
                m0 = _mm_add_epi32(_mm256_castsi256_si128(mm_tmp[0]), _mm256_extracti128_si256(mm_tmp[0], 1));
                mm_tmp[2] = _mm256_loadu_si256((const __m256i*)&coef[3][1]);
                mm_tmp[3] = _mm256_cvtepi32_epi64(m0);
                mm_tmp[2] = _mm256_add_epi64(mm_tmp[2], mm_tmp[3]);
                _mm256_storeu_si256((__m256i*)&coef[3][1], mm_tmp[2]);
                
                mm_tmp[0] = _mm256_madd_epi16(mm_c[0], mm_c[4]);
                mm_tmp[1] = _mm256_madd_epi16(mm_c[1], mm_c[4]);
                mm_tmp[2] = _mm256_madd_epi16(mm_c[3], mm_c[4]);
                mm_tmp[0] = _mm256_hadd_epi32(mm_tmp[0], mm_tmp[1]);
                mm_tmp[2] = _mm256_hadd_epi32(mm_tmp[2], mm_tmp[2]);
                mm_tmp[0] = _mm256_hadd_epi32(mm_tmp[0], mm_tmp[2]);
                coef[1][4] += _mm256_extract_epi32(mm_tmp[0], 0) + _mm256_extract_epi32(mm_tmp[0], 4);
                coef[2][4] += _mm256_extract_epi32(mm_tmp[0], 1) + _mm256_extract_epi32(mm_tmp[0], 5);
                coef[4][4] += _mm256_extract_epi32(mm_tmp[0], 2) + _mm256_extract_epi32(mm_tmp[0], 6);

                mm_c[5] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(mm_c[1], 1));
                mm_c[7] = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(mm_c[3], 1));
                mm_c[1] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(mm_c[1]));
                mm_c[3] = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(mm_c[3]));

                mm_tmp[0] = _mm256_srli_si256(mm_c[1], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[5], 4);
                CALC_AFFINE_COEF_16PXLS(mm_c[1], mm_c[5], mm_c[1], mm_c[5], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], m0);
                coef[2][1] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
                
                mm_tmp[2] = _mm256_srli_si256(mm_c[3], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[7], 4);
                CALC_AFFINE_COEF_16PXLS(mm_c[1], mm_c[5], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], m0);
                coef[2][3] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
                
                CALC_AFFINE_COEF_16PXLS(mm_c[3], mm_c[7], mm_c[3], mm_c[7], mm_tmp[2], mm_tmp[3], mm_tmp[2], mm_tmp[3], m0);
                coef[4][3] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);

                idx1 += 8;
                idx2 += 8;
                mm_idx_k = _mm256_add_epi32(mm_idx_k, mm_eight);
                mm_idx_k_s16 = _mm256_add_epi32(mm_idx_k_s16, mm_eight_s16);
            }
            else {
                mm_tmp[0] = _mm256_set_m128i(m1, m0);
                mm_tmp[1] = _mm256_set_m128i(m3, m2);
                mm_tmp[2] = _mm256_set_m128i(m4, m4);
                mm_tmp[3] = _mm256_set_m128i(m5, m5);

                mm_c[0] = _mm256_cvtepi16_epi32(m0);
                mm_c[2] = _mm256_cvtepi16_epi32(m1);
                mm_c[4] = _mm256_cvtepi16_epi32(m2);
                mm_c[6] = _mm256_cvtepi16_epi32(m3);

                m0 = _mm_madd_epi16(m0, m1);
                m2 = _mm_madd_epi16(m2, m3);
                m0 = _mm_add_epi32(m0, m2);
                m0 = _mm_hadd_epi32(m0, m0);

                CALC_AFFINE_4COEFS_16PXLS_SRC16_DST32(mm_tmp[0], mm_tmp[0], mm_tmp[1], mm_tmp[1], mm_tmp[0], mm_tmp[2], mm_tmp[1], mm_tmp[3], mm_intermediate[0])
                coef[1][0] += _mm256_extract_epi32(mm_intermediate[0], 0);
                coef[1][2] += _mm_extract_epi32(m0, 0) + _mm_extract_epi32(m0, 1);
                coef[1][4] += _mm256_extract_epi32(mm_intermediate[0], 1);
                coef[3][2] += _mm256_extract_epi32(mm_intermediate[0], 4);
                coef[3][4] += _mm256_extract_epi32(mm_intermediate[0], 5);

                mm_residue[0] = _mm256_cvtepi16_epi32(m4);
                mm_residue[1] = _mm256_cvtepi16_epi32(m5);

                mm_c[1] = _mm256_mullo_epi32(mm_idx_k, mm_c[0]);
                mm_c[3] = _mm256_mullo_epi32(mm_idx_j[0], mm_c[0]);
                mm_c[5] = _mm256_mullo_epi32(mm_idx_k, mm_c[4]);
                mm_c[7] = _mm256_mullo_epi32(mm_idx_j[1], mm_c[4]);
                mm_tmp[0] = _mm256_mullo_epi32(mm_idx_j[0], mm_c[2]);
                mm_tmp[1] = _mm256_mullo_epi32(mm_idx_k, mm_c[2]);
                mm_tmp[2] = _mm256_mullo_epi32(mm_idx_j[1], mm_c[6]);
                mm_tmp[3] = _mm256_mullo_epi32(mm_idx_k, mm_c[6]);
                mm_c[1] = _mm256_add_epi32(mm_c[1], mm_tmp[0]);
                mm_c[3] = _mm256_sub_epi32(mm_c[3], mm_tmp[1]);
                mm_c[5] = _mm256_add_epi32(mm_c[5], mm_tmp[2]);
                mm_c[7] = _mm256_sub_epi32(mm_c[7], mm_tmp[3]);

                // 1st row
                mm_tmp[0] = _mm256_srli_si256(mm_c[0], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[4], 4);
                mm_tmp[2] = _mm256_srli_si256(mm_c[1], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[5], 4);
                CALC_AFFINE_COEF_16PXLS(mm_c[0], mm_c[4], mm_c[1], mm_c[5], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], m0);
                coef[1][1] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
                // 4th col of row and 1st col of 4th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[3], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[7], 4);
                CALC_AFFINE_COEF_16PXLS(mm_c[0], mm_c[4], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], m0);
                coef[1][3] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
                mm_tmp[0] = _mm256_srli_si256(mm_c[1], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[5], 4);
                // 2nd col of row
                CALC_AFFINE_COEF_16PXLS(mm_c[1], mm_c[5], mm_c[1], mm_c[5], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], m0);
                coef[2][1] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
                // 3rd col of row and 2nd col of 3rd row
                mm_tmp[2] = _mm256_srli_si256(mm_c[2], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[6], 4);
                CALC_AFFINE_COEF_16PXLS(mm_c[1], mm_c[5], mm_c[2], mm_c[6], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], m0);
                coef[3][1] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
                // 4th col of row and 2nd col of 4th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[3], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[7], 4);
                CALC_AFFINE_COEF_16PXLS(mm_c[1], mm_c[5], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], m0);
                coef[2][3] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
                // 5th col of row
                mm_tmp[2] = _mm256_srli_si256(mm_residue[0], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_residue[1], 4);
                CALC_AFFINE_COEF_16PXLS(mm_c[1], mm_c[5], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], m0);
                coef[2][4] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
                // 3rd row
                mm_tmp[0] = _mm256_srli_si256(mm_c[2], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[6], 4);
                // 4th col of row and 3rd col of 4th row
                mm_tmp[2] = _mm256_srli_si256(mm_c[3], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_c[7], 4);
                CALC_AFFINE_COEF_16PXLS(mm_c[2], mm_c[6], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], m0);
                coef[3][3] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
                // 4th row
                mm_tmp[0] = _mm256_srli_si256(mm_c[3], 4);
                mm_tmp[1] = _mm256_srli_si256(mm_c[7], 4);
                // 4th col of row
                CALC_AFFINE_COEF_16PXLS(mm_c[3], mm_c[7], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], m0);
                coef[4][3] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
                // 5th col of row
                mm_tmp[2] = _mm256_srli_si256(mm_residue[0], 4);
                mm_tmp[3] = _mm256_srli_si256(mm_residue[1], 4);
                idx1 += 8;
                idx2 += 8;
                mm_idx_k = _mm256_add_epi32(mm_idx_k, mm_eight);
                CALC_AFFINE_COEF_16PXLS(mm_c[3], mm_c[7], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], m0);
                coef[4][4] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
            }
        }
        idx1 += (i_deriv << 1) - width;
        idx2 += (i_deriv << 1) - width;
        mm_idx_j[0] = _mm256_add_epi32(mm_idx_j[0], mm_two);
        mm_idx_j[1] = _mm256_add_epi32(mm_idx_j[1], mm_two);
        mm_idx_j_s16 = _mm256_add_epi32(mm_idx_j_s16, mm_two_s16);
    }
    coef[1][4] <<= 3;
    coef[2][0] = coef[1][1];
    coef[3][0] = coef[1][2];
    coef[4][0] = coef[1][3];
    coef[4][1] = coef[2][3];
    coef[2][4] <<= 3;
    coef[2][2] = coef[3][1];
    //coef[3][1] = coef[2][2];
    coef[4][2] = coef[3][3];
    coef[3][4] <<= 3;
    coef[4][4] <<= 3;

#undef CALC_AFFINE_COEF_16PXLS
#undef CALC_AFFINE_4COEFS_16PXLS_SRC16_DST32
}
#else
void affine_coef_computer_avx2(s16 *resi, int i_resi, s16(*deriv)[MAX_CU_DIM], int i_deriv, s64(*coef)[5], int width, int height)
{
    int j, k;
    int idx1 = 0, idx2 = 0;
    if (width == 4) {
        __m128i mm_two, mm_four;
        __m128i mm_tmp[4];
        __m128i mm_intermediate[4];
        __m128i mm_idx_k, mm_idx_j[2];
        __m128i mm_residue[2];

        mm_two = _mm_set1_epi32(2);
        mm_four = _mm_set1_epi32(4);

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

#define CALC_AFFINE_COEF_16PXLS(x1,x2,y1,y2,tmp0,tmp1,tmp2,tmp3,dst)      \
    {                                                                                   \
        mm_intermediate[0] = _mm256_mul_epi32(x1, y1);                                  \
        mm_intermediate[1] = _mm256_mul_epi32(tmp0, tmp2);                              \
        mm_intermediate[2] = _mm256_mul_epi32(x2, y2);                                  \
        mm_intermediate[3] = _mm256_mul_epi32(tmp1, tmp3);                              \
        mm_intermediate[2] = _mm256_add_epi64(mm_intermediate[0], mm_intermediate[2]);  \
        mm_intermediate[3] = _mm256_add_epi64(mm_intermediate[1], mm_intermediate[3]);  \
        mm_intermediate[3] = _mm256_add_epi64(mm_intermediate[2], mm_intermediate[3]);  \
        dst = _mm_add_epi64(_mm256_castsi256_si128(mm_intermediate[3]), _mm256_extracti128_si256(mm_intermediate[3], 1)); \
    }

#define CALC_AFFINE_4COEFS_16PXLS_SRC16_DST32(s0, s1, s2, s3, s4, s5, s6, s7, dst) \
        mm_intermediate[0] = _mm256_madd_epi16(s0, s1);                   \
        mm_intermediate[1] = _mm256_madd_epi16(s2, s3);                   \
        mm_intermediate[2] = _mm256_madd_epi16(s4, s5);                   \
        mm_intermediate[3] = _mm256_madd_epi16(s6, s7);                   \
        mm_intermediate[0] = _mm256_add_epi32(mm_intermediate[0], mm_intermediate[1]);  \
        mm_intermediate[2] = _mm256_add_epi32(mm_intermediate[2], mm_intermediate[3]);  \
        mm_intermediate[0] = _mm256_hadd_epi32(mm_intermediate[0], mm_intermediate[2]); \
        dst = _mm256_hadd_epi32(mm_intermediate[0], mm_intermediate[0]);

    
    __m256i mm_two, mm_eight;
    __m256i mm_tmp[4];
    __m256i mm_intermediate[4];
    __m256i mm_idx_k, mm_idx_j[2];
    __m256i mm_residue[2];
    __m128i m0, m1, m2, m3, m4, m5;
    __m256i mm_c[8];

    idx1 = 0;
    idx2 = i_deriv;
    mm_two = _mm256_set1_epi32(2);
    mm_eight = _mm256_set1_epi32(8);
    mm_idx_j[0] = _mm256_set1_epi32(0);
    mm_idx_j[1] = _mm256_set1_epi32(1);
    for (j = 0; j < height; j += 2) {
        mm_idx_k = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
        for (k = 0; k < width; k += 8) {
            m0 = _mm_loadu_si128((const __m128i *)&deriv[0][idx1]);
            m1 = _mm_loadu_si128((const __m128i *)&deriv[1][idx1]);
            m2 = _mm_loadu_si128((const __m128i *)&deriv[0][idx2]);
            m3 = _mm_loadu_si128((const __m128i *)&deriv[1][idx2]);
            m4 = _mm_loadu_si128((const __m128i *)&resi[idx1]);
            m5 = _mm_loadu_si128((const __m128i *)&resi[idx2]);

            mm_tmp[0] = _mm256_set_m128i(m1, m0);
            mm_tmp[1] = _mm256_set_m128i(m3, m2);
            mm_tmp[2] = _mm256_set_m128i(m4, m4);
            mm_tmp[3] = _mm256_set_m128i(m5, m5);

            mm_c[0] = _mm256_cvtepi16_epi32(m0);
            mm_c[2] = _mm256_cvtepi16_epi32(m1);
            mm_c[4] = _mm256_cvtepi16_epi32(m2);
            mm_c[6] = _mm256_cvtepi16_epi32(m3);

            m0 = _mm_madd_epi16(m0, m1);
            m2 = _mm_madd_epi16(m2, m3);
            m0 = _mm_add_epi32(m0, m2);
            m0 = _mm_hadd_epi32(m0, m0);

            CALC_AFFINE_4COEFS_16PXLS_SRC16_DST32(mm_tmp[0], mm_tmp[0], mm_tmp[1], mm_tmp[1], mm_tmp[0], mm_tmp[2], mm_tmp[1], mm_tmp[3], mm_intermediate[0])
                coef[1][0] += _mm256_extract_epi32(mm_intermediate[0], 0);
            coef[1][2] += _mm_extract_epi32(m0, 0) + _mm_extract_epi32(m0, 1);
            coef[1][4] += _mm256_extract_epi32(mm_intermediate[0], 1);
            coef[3][2] += _mm256_extract_epi32(mm_intermediate[0], 4);
            coef[3][4] += _mm256_extract_epi32(mm_intermediate[0], 5);

            mm_residue[0] = _mm256_cvtepi16_epi32(m4);
            mm_residue[1] = _mm256_cvtepi16_epi32(m5);

            mm_c[1] = _mm256_mullo_epi32(mm_idx_k, mm_c[0]);
            mm_c[3] = _mm256_mullo_epi32(mm_idx_j[0], mm_c[0]);
            mm_c[5] = _mm256_mullo_epi32(mm_idx_k, mm_c[4]);
            mm_c[7] = _mm256_mullo_epi32(mm_idx_j[1], mm_c[4]);
            mm_tmp[0] = _mm256_mullo_epi32(mm_idx_j[0], mm_c[2]);
            mm_tmp[1] = _mm256_mullo_epi32(mm_idx_k, mm_c[2]);
            mm_tmp[2] = _mm256_mullo_epi32(mm_idx_j[1], mm_c[6]);
            mm_tmp[3] = _mm256_mullo_epi32(mm_idx_k, mm_c[6]);
            mm_c[1] = _mm256_add_epi32(mm_c[1], mm_tmp[0]);
            mm_c[3] = _mm256_sub_epi32(mm_c[3], mm_tmp[1]);
            mm_c[5] = _mm256_add_epi32(mm_c[5], mm_tmp[2]);
            mm_c[7] = _mm256_sub_epi32(mm_c[7], mm_tmp[3]);

            // 1st row
            mm_tmp[0] = _mm256_srli_si256(mm_c[0], 4);
            mm_tmp[1] = _mm256_srli_si256(mm_c[4], 4);
            mm_tmp[2] = _mm256_srli_si256(mm_c[1], 4);
            mm_tmp[3] = _mm256_srli_si256(mm_c[5], 4);
            CALC_AFFINE_COEF_16PXLS(mm_c[0], mm_c[4], mm_c[1], mm_c[5], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], m0);
            coef[1][1] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
            // 4th col of row and 1st col of 4th row
            mm_tmp[2] = _mm256_srli_si256(mm_c[3], 4);
            mm_tmp[3] = _mm256_srli_si256(mm_c[7], 4);
            CALC_AFFINE_COEF_16PXLS(mm_c[0], mm_c[4], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], m0);
            coef[1][3] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
            mm_tmp[0] = _mm256_srli_si256(mm_c[1], 4);
            mm_tmp[1] = _mm256_srli_si256(mm_c[5], 4);
            // 2nd col of row
            CALC_AFFINE_COEF_16PXLS(mm_c[1], mm_c[5], mm_c[1], mm_c[5], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], m0);
            coef[2][1] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
            // 3rd col of row and 2nd col of 3rd row
            mm_tmp[2] = _mm256_srli_si256(mm_c[2], 4);
            mm_tmp[3] = _mm256_srli_si256(mm_c[6], 4);
            CALC_AFFINE_COEF_16PXLS(mm_c[1], mm_c[5], mm_c[2], mm_c[6], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], m0);
            coef[2][2] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
            // 4th col of row and 2nd col of 4th row
            mm_tmp[2] = _mm256_srli_si256(mm_c[3], 4);
            mm_tmp[3] = _mm256_srli_si256(mm_c[7], 4);
            CALC_AFFINE_COEF_16PXLS(mm_c[1], mm_c[5], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], m0);
            coef[2][3] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
            // 5th col of row
            mm_tmp[2] = _mm256_srli_si256(mm_residue[0], 4);
            mm_tmp[3] = _mm256_srli_si256(mm_residue[1], 4);
            CALC_AFFINE_COEF_16PXLS(mm_c[1], mm_c[5], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], m0);
            coef[2][4] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
            // 3rd row
            mm_tmp[0] = _mm256_srli_si256(mm_c[2], 4);
            mm_tmp[1] = _mm256_srli_si256(mm_c[6], 4);
            // 4th col of row and 3rd col of 4th row
            mm_tmp[2] = _mm256_srli_si256(mm_c[3], 4);
            mm_tmp[3] = _mm256_srli_si256(mm_c[7], 4);
            CALC_AFFINE_COEF_16PXLS(mm_c[2], mm_c[6], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], m0);
            coef[3][3] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
            // 4th row
            mm_tmp[0] = _mm256_srli_si256(mm_c[3], 4);
            mm_tmp[1] = _mm256_srli_si256(mm_c[7], 4);
            // 4th col of row
            CALC_AFFINE_COEF_16PXLS(mm_c[3], mm_c[7], mm_c[3], mm_c[7], mm_tmp[0], mm_tmp[1], mm_tmp[0], mm_tmp[1], m0);
            coef[4][3] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
            // 5th col of row
            mm_tmp[2] = _mm256_srli_si256(mm_residue[0], 4);
            mm_tmp[3] = _mm256_srli_si256(mm_residue[1], 4);
            idx1 += 8;
            idx2 += 8;
            mm_idx_k = _mm256_add_epi32(mm_idx_k, mm_eight);
            CALC_AFFINE_COEF_16PXLS(mm_c[3], mm_c[7], mm_residue[0], mm_residue[1], mm_tmp[0], mm_tmp[1], mm_tmp[2], mm_tmp[3], m0);
            coef[4][4] += _mm_extract_epi64(m0, 0) + _mm_extract_epi64(m0, 1);
        }
        idx1 += (i_deriv << 1) - width;
        idx2 += (i_deriv << 1) - width;
        mm_idx_j[0] = _mm256_add_epi32(mm_idx_j[0], mm_two);
        mm_idx_j[1] = _mm256_add_epi32(mm_idx_j[1], mm_two);
    }
    coef[1][4] <<= 3;
    coef[2][0] = coef[1][1];
    coef[3][0] = coef[1][2];
    coef[4][0] = coef[1][3];
    coef[3][1] = coef[2][2];
    coef[4][1] = coef[2][3];
    coef[2][4] <<= 3;
    coef[4][2] = coef[3][3];
    coef[3][4] <<= 3;
    coef[4][4] <<= 3;
    
#undef CALC_AFFINE_COEF_16PXLS
#undef CALC_AFFINE_4COEFS_16PXLS_SRC16_DST32
}
#endif
