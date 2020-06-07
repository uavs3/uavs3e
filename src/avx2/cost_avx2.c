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

#include "avx2.h"

#if (BIT_DEPTH == 8)
u32 uavs3e_get_sad_16_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m256i sum = _mm256_setzero_si256();
    __m256i O0, O1, P0, P1, S0, S1;
    int i_org2 = i_org << 1;
    int i_org3 = i_org2 + i_org;
    int i_pred2 = i_pred << 1;
    int i_pred3 = i_pred2 + i_pred;
    height >>= 2;

    while (height--) {
        O0 = _mm256_loadu2_m128i((__m128i *)p_org, (__m128i *)(p_org + i_org));
        O1 = _mm256_loadu2_m128i((__m128i *)(p_org + i_org2), (__m128i *)(p_org + i_org3));
        P0 = _mm256_loadu2_m128i((__m128i *)p_pred, (__m128i *)(p_pred + i_pred));
        P1 = _mm256_loadu2_m128i((__m128i *)(p_pred + i_pred2), (__m128i *)(p_pred + i_pred3));
        S0 = _mm256_sad_epu8(O0, P0);
        S1 = _mm256_sad_epu8(O1, P1);
        sum = _mm256_add_epi64(sum, S0);
        sum = _mm256_add_epi64(sum, S1);
        p_org += i_org << 2;
        p_pred += i_pred << 2;
    }
    return ((u32)_mm256_extract_epi32(sum, 0)) + 
           ((u32)_mm256_extract_epi32(sum, 2)) + 
           ((u32)_mm256_extract_epi32(sum, 4)) + 
           ((u32)_mm256_extract_epi32(sum, 6));
}

#define CAL_SAD_W32(d) { \
        __m256i A = _mm256_loadu_si256((__m256i*)(p_org  + d * 32)); \
        __m256i B = _mm256_loadu_si256((__m256i*)(p_pred + d * 32)); \
        __m256i C = _mm256_sad_epu8(A, B); \
        sum = _mm256_add_epi32(sum, C); \
    }

u32 uavs3e_get_sad_32_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m256i sum = _mm256_setzero_si256();
    __m256i O0, O1, P0, P1, S0, S1;
    height >>= 1;

    while (height--) {
        O0 = _mm256_loadu_si256((__m256i*)(p_org));
        O1 = _mm256_loadu_si256((__m256i*)(p_org + i_org));
        P0 = _mm256_loadu_si256((__m256i*)(p_pred));
        P1 = _mm256_loadu_si256((__m256i*)(p_pred + i_pred));
        S0 = _mm256_sad_epu8(O0, P0);
        S1 = _mm256_sad_epu8(O1, P1);
        p_org += i_org << 1;
        p_pred += i_pred << 1;
        sum = _mm256_add_epi64(sum, S0);
        sum = _mm256_add_epi64(sum, S1);
    }
    return ((u32)_mm256_extract_epi32(sum, 0)) + 
           ((u32)_mm256_extract_epi32(sum, 2)) + 
           ((u32)_mm256_extract_epi32(sum, 4)) + 
           ((u32)_mm256_extract_epi32(sum, 6));
}

u32 uavs3e_get_sad_64_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m256i sum = _mm256_setzero_si256();

    while (height--) {
        CAL_SAD_W32(0);
        CAL_SAD_W32(1);
        p_org += i_org;
        p_pred += i_pred;
    }
    return ((u32)_mm256_extract_epi32(sum, 0)) + 
           ((u32)_mm256_extract_epi32(sum, 2)) + 
           ((u32)_mm256_extract_epi32(sum, 4)) + 
           ((u32)_mm256_extract_epi32(sum, 6));
}

u32 uavs3e_get_sad_128_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m256i sum = _mm256_setzero_si256();

    while (height--) {
        CAL_SAD_W32(0);
        CAL_SAD_W32(1);
        CAL_SAD_W32(2);
        CAL_SAD_W32(3);
        p_org += i_org;
        p_pred += i_pred;
    }
    return ((u32)_mm256_extract_epi32(sum, 0)) + 
           ((u32)_mm256_extract_epi32(sum, 2)) + 
           ((u32)_mm256_extract_epi32(sum, 4)) + 
           ((u32)_mm256_extract_epi32(sum, 6));
}

void uavs3e_get_sad_x3_16_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height)
{
    __m256i sum0, sum1, sum2;
    __m256i O0, P0, P1, P2, S0, S1, S2;
    int i_org2 = i_org << 1;
    int i_pred2 = i_pred << 1;

    height >>= 1;
    sum0 = sum1 = sum2 = _mm256_setzero_si256();

    while (height--) {
        O0 = _mm256_loadu2_m128i((__m128i *)p_org, (__m128i *)(p_org + i_org));
        P0 = _mm256_loadu2_m128i((__m128i *)p_pred0, (__m128i *)(p_pred0 + i_pred));
        P1 = _mm256_loadu2_m128i((__m128i *)(p_pred1), (__m128i *)(p_pred1 + i_pred));
        P2 = _mm256_loadu2_m128i((__m128i *)(p_pred2), (__m128i *)(p_pred2 + i_pred));
        S0 = _mm256_sad_epu8(O0, P0);
        S1 = _mm256_sad_epu8(O0, P1);
        S2 = _mm256_sad_epu8(O0, P2);
        sum0 = _mm256_add_epi64(sum0, S0);
        sum1 = _mm256_add_epi64(sum1, S1);
        sum2 = _mm256_add_epi64(sum2, S2);
        p_org += i_org2;
        p_pred0 += i_pred2;
        p_pred1 += i_pred2;
        p_pred2 += i_pred2;
    }
    sad[0] = ((u32)_mm256_extract_epi32(sum0, 0)) + ((u32)_mm256_extract_epi32(sum0, 2)) + ((u32)_mm256_extract_epi32(sum0, 4)) + ((u32)_mm256_extract_epi32(sum0, 6));
    sad[1] = ((u32)_mm256_extract_epi32(sum1, 0)) + ((u32)_mm256_extract_epi32(sum1, 2)) + ((u32)_mm256_extract_epi32(sum1, 4)) + ((u32)_mm256_extract_epi32(sum1, 6));
    sad[2] = ((u32)_mm256_extract_epi32(sum2, 0)) + ((u32)_mm256_extract_epi32(sum2, 2)) + ((u32)_mm256_extract_epi32(sum2, 4)) + ((u32)_mm256_extract_epi32(sum2, 6));
}

#define CAL_SAD_X3_W32(d) { \
        __m256i o = _mm256_loadu_si256((__m256i*)(p_org  + d * 32)); \
        __m256i p0 = _mm256_loadu_si256((__m256i*)(p_pred0 + d * 32)); \
        __m256i p1 = _mm256_loadu_si256((__m256i*)(p_pred1 + d * 32)); \
        __m256i p2 = _mm256_loadu_si256((__m256i*)(p_pred2 + d * 32)); \
        __m256i t0 = _mm256_sad_epu8(o, p0); \
        __m256i t1 = _mm256_sad_epu8(o, p1); \
        __m256i t2 = _mm256_sad_epu8(o, p2); \
        sum0 = _mm256_add_epi32(sum0, t0); \
        sum1 = _mm256_add_epi32(sum1, t1); \
        sum2 = _mm256_add_epi32(sum2, t2); \
    }

void uavs3e_get_sad_x3_32_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height)
{
    __m256i sum0, sum1, sum2;
    
    sum0 = sum1 = sum2 = _mm256_setzero_si256();

    while (height--) {
        CAL_SAD_X3_W32(0);
        p_org += i_org;
        p_pred0 += i_pred;
        p_pred1 += i_pred;
        p_pred2 += i_pred;
    }
    sad[0] = ((u32)_mm256_extract_epi32(sum0, 0)) + ((u32)_mm256_extract_epi32(sum0, 2)) + ((u32)_mm256_extract_epi32(sum0, 4)) + ((u32)_mm256_extract_epi32(sum0, 6));
    sad[1] = ((u32)_mm256_extract_epi32(sum1, 0)) + ((u32)_mm256_extract_epi32(sum1, 2)) + ((u32)_mm256_extract_epi32(sum1, 4)) + ((u32)_mm256_extract_epi32(sum1, 6));
    sad[2] = ((u32)_mm256_extract_epi32(sum2, 0)) + ((u32)_mm256_extract_epi32(sum2, 2)) + ((u32)_mm256_extract_epi32(sum2, 4)) + ((u32)_mm256_extract_epi32(sum2, 6));
}

void uavs3e_get_sad_x3_64_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height)
{
    __m256i sum0, sum1, sum2;

    sum0 = sum1 = sum2 = _mm256_setzero_si256();

    while (height--) {
        CAL_SAD_X3_W32(0);
        CAL_SAD_X3_W32(1);
        p_org += i_org;
        p_pred0 += i_pred;
        p_pred1 += i_pred;
        p_pred2 += i_pred;
    }
    sad[0] = ((u32)_mm256_extract_epi32(sum0, 0)) + ((u32)_mm256_extract_epi32(sum0, 2)) + ((u32)_mm256_extract_epi32(sum0, 4)) + ((u32)_mm256_extract_epi32(sum0, 6));
    sad[1] = ((u32)_mm256_extract_epi32(sum1, 0)) + ((u32)_mm256_extract_epi32(sum1, 2)) + ((u32)_mm256_extract_epi32(sum1, 4)) + ((u32)_mm256_extract_epi32(sum1, 6));
    sad[2] = ((u32)_mm256_extract_epi32(sum2, 0)) + ((u32)_mm256_extract_epi32(sum2, 2)) + ((u32)_mm256_extract_epi32(sum2, 4)) + ((u32)_mm256_extract_epi32(sum2, 6));
}

void uavs3e_get_sad_x3_128_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height)
{
    __m256i sum0, sum1, sum2;

    sum0 = sum1 = sum2 = _mm256_setzero_si256();

    while (height--) {
        CAL_SAD_X3_W32(0);
        CAL_SAD_X3_W32(1);
        CAL_SAD_X3_W32(2);
        CAL_SAD_X3_W32(3);
        p_org += i_org;
        p_pred0 += i_pred;
        p_pred1 += i_pred;
        p_pred2 += i_pred;
    }
    sad[0] = ((u32)_mm256_extract_epi32(sum0, 0)) + ((u32)_mm256_extract_epi32(sum0, 2)) + ((u32)_mm256_extract_epi32(sum0, 4)) + ((u32)_mm256_extract_epi32(sum0, 6));
    sad[1] = ((u32)_mm256_extract_epi32(sum1, 0)) + ((u32)_mm256_extract_epi32(sum1, 2)) + ((u32)_mm256_extract_epi32(sum1, 4)) + ((u32)_mm256_extract_epi32(sum1, 6));
    sad[2] = ((u32)_mm256_extract_epi32(sum2, 0)) + ((u32)_mm256_extract_epi32(sum2, 2)) + ((u32)_mm256_extract_epi32(sum2, 4)) + ((u32)_mm256_extract_epi32(sum2, 6));
}

void uavs3e_get_sad_x4_4_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[4], int height)
{
    __m256i sum0, sum1;
    __m256i o, p0, p1, t0, t1;
    __m128i m0;
    int i_orgx2  = i_org << 1;
    int i_orgx3  = i_orgx2 + i_org;
    int i_predx2 = i_pred << 1;
    int i_predx3 = i_predx2 + i_pred;
    int i_predx4 = i_pred << 2;
    height >>= 2;
    sum0 = sum1 = _mm256_setzero_si256();

    while (1) {
        int o0 = *(int *)(p_org);
        int o1 = *(int *)(p_org + i_org);
        int o2 = *(int *)(p_org + i_orgx2);
        int o3 = *(int *)(p_org + i_orgx3);

        o = _mm256_set_epi32(o0, o1, o2, o3, o0, o1, o2, o3);

        p0 = _mm256_set_epi32(*(int *)(p_pred1), *(int *)(p_pred1 + i_pred), *(int *)(p_pred1 + i_predx2), *(int *)(p_pred1 + i_predx3),
                              *(int *)(p_pred0), *(int *)(p_pred0 + i_pred), *(int *)(p_pred0 + i_predx2), *(int *)(p_pred0 + i_predx3));
        p1 = _mm256_set_epi32(*(int *)(p_pred3), *(int *)(p_pred3 + i_pred), *(int *)(p_pred3 + i_predx2), *(int *)(p_pred3 + i_predx3),
                              *(int *)(p_pred2), *(int *)(p_pred2 + i_pred), *(int *)(p_pred2 + i_predx2), *(int *)(p_pred2 + i_predx3));
       
        height--;

        t0 = _mm256_sad_epu8(o, p0);
        t1 = _mm256_sad_epu8(o, p1);

        sum0 = _mm256_add_epi64(sum0, t0);
        sum1 = _mm256_add_epi64(sum1, t1);

        if (height == 0) {
            goto End;
        }
        p_org   += i_org << 2;
        p_pred0 += i_predx4;
        p_pred1 += i_predx4;
        p_pred2 += i_predx4;
        p_pred3 += i_predx4;
    }

End:
    sum0 = _mm256_hadd_epi32(sum0, sum1);
    sum0 = _mm256_permute4x64_epi64(sum0, 0xd8);
    m0 = _mm_hadd_epi32(_mm256_castsi256_si128(sum0), _mm256_extracti128_si256(sum0, 1));
    _mm_storeu_si128((__m128i*)sad, m0);
}

void uavs3e_get_sad_x4_8_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[4], int height)
{
    __m256i sum0, sum1;
    __m256i O0, P0, P1, S0, S1;
    __m128i m0;
    int i_org2  = i_org  << 1;
    int i_pred2 = i_pred << 1;

    height >>= 1;
    sum0 = sum1 = _mm256_setzero_si256();

    while (1) {
        s64 t0 = *(s64 *)p_org, t1 = *(s64 *)(p_org + i_org);
        O0 = _mm256_set_epi64x(t0, t1, t0, t1);

        P0 = _mm256_set_epi64x(*(s64 *)(p_pred1), *(s64 *)(p_pred1 + i_pred), *(s64 *)(p_pred0), *(s64 *)(p_pred0 + i_pred));
        P1 = _mm256_set_epi64x(*(s64 *)(p_pred3), *(s64 *)(p_pred3 + i_pred), *(s64 *)(p_pred2), *(s64 *)(p_pred2 + i_pred));

        S0 = _mm256_sad_epu8(O0, P0);
        S1 = _mm256_sad_epu8(O0, P1);

        height--;

        sum0 = _mm256_add_epi64(sum0, S0);
        sum1 = _mm256_add_epi64(sum1, S1);

        if (height == 0) {
            goto End;
        }
        p_org   += i_org2;
        p_pred0 += i_pred2;
        p_pred1 += i_pred2;
        p_pred2 += i_pred2;
        p_pred3 += i_pred2;
    }
End:
    sum0 = _mm256_hadd_epi32(sum0, sum1);
    sum0 = _mm256_permute4x64_epi64(sum0, 0xd8);
    m0 = _mm_hadd_epi32(_mm256_castsi256_si128(sum0), _mm256_extracti128_si256(sum0, 1));
    _mm_storeu_si128((__m128i*)sad, m0);
}

void uavs3e_get_sad_x4_16_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[4], int height)
{
    __m256i sum0, sum1, sum2, sum3;
    __m256i O0, P0, P1, P2, P3, S0, S1, S2, S3;
    int i_org2 = i_org << 1;
    int i_pred2 = i_pred << 1;

    height >>= 1;
    sum0 = sum1 = sum2 = sum3 = _mm256_setzero_si256();

    while (height--) {
        O0 = _mm256_loadu2_m128i((__m128i *)p_org, (__m128i *)(p_org + i_org));
        P0 = _mm256_loadu2_m128i((__m128i *)p_pred0, (__m128i *)(p_pred0 + i_pred));
        P1 = _mm256_loadu2_m128i((__m128i *)(p_pred1), (__m128i *)(p_pred1 + i_pred));
        P2 = _mm256_loadu2_m128i((__m128i *)(p_pred2), (__m128i *)(p_pred2 + i_pred));
        P3 = _mm256_loadu2_m128i((__m128i *)(p_pred3), (__m128i *)(p_pred3 + i_pred));

        S0 = _mm256_sad_epu8(O0, P0);
        S1 = _mm256_sad_epu8(O0, P1);
        S2 = _mm256_sad_epu8(O0, P2);
        S3 = _mm256_sad_epu8(O0, P3);

        sum0 = _mm256_add_epi64(sum0, S0);
        sum1 = _mm256_add_epi64(sum1, S1);
        sum2 = _mm256_add_epi64(sum2, S2);
        sum3 = _mm256_add_epi64(sum3, S3);

        p_org += i_org2;
        p_pred0 += i_pred2;
        p_pred1 += i_pred2;
        p_pred2 += i_pred2;
        p_pred3 += i_pred2;
    }

    sad[0] = ((u32)_mm256_extract_epi32(sum0, 0)) + ((u32)_mm256_extract_epi32(sum0, 2)) + ((u32)_mm256_extract_epi32(sum0, 4)) + ((u32)_mm256_extract_epi32(sum0, 6));
    sad[1] = ((u32)_mm256_extract_epi32(sum1, 0)) + ((u32)_mm256_extract_epi32(sum1, 2)) + ((u32)_mm256_extract_epi32(sum1, 4)) + ((u32)_mm256_extract_epi32(sum1, 6));
    sad[2] = ((u32)_mm256_extract_epi32(sum2, 0)) + ((u32)_mm256_extract_epi32(sum2, 2)) + ((u32)_mm256_extract_epi32(sum2, 4)) + ((u32)_mm256_extract_epi32(sum2, 6));
    sad[3] = ((u32)_mm256_extract_epi32(sum3, 0)) + ((u32)_mm256_extract_epi32(sum3, 2)) + ((u32)_mm256_extract_epi32(sum3, 4)) + ((u32)_mm256_extract_epi32(sum3, 6));
}

#define CAL_SAD_X4_W32(d) { \
        __m256i o  = _mm256_loadu_si256((__m256i*)(p_org  + d * 32)); \
        __m256i p0 = _mm256_loadu_si256((__m256i*)(p_pred0 + d * 32)); \
        __m256i p1 = _mm256_loadu_si256((__m256i*)(p_pred1 + d * 32)); \
        __m256i p2 = _mm256_loadu_si256((__m256i*)(p_pred2 + d * 32)); \
        __m256i p3 = _mm256_loadu_si256((__m256i*)(p_pred3 + d * 32)); \
        __m256i t0 = _mm256_sad_epu8(o, p0); \
        __m256i t1 = _mm256_sad_epu8(o, p1); \
        __m256i t2 = _mm256_sad_epu8(o, p2); \
        __m256i t3 = _mm256_sad_epu8(o, p3); \
        sum0 = _mm256_add_epi32(sum0, t0); \
        sum1 = _mm256_add_epi32(sum1, t1); \
        sum2 = _mm256_add_epi32(sum2, t2); \
        sum3 = _mm256_add_epi32(sum3, t3); \
    }

void uavs3e_get_sad_x4_32_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[4], int height)
{
    __m256i sum0, sum1, sum2, sum3;

    sum0 = sum1 = sum2 = sum3 = _mm256_setzero_si256();

    while (height--) {
        CAL_SAD_X4_W32(0);
        p_org += i_org;
        p_pred0 += i_pred;
        p_pred1 += i_pred;
        p_pred2 += i_pred;
        p_pred3 += i_pred;
    }
    sad[0] = ((u32)_mm256_extract_epi32(sum0, 0)) + ((u32)_mm256_extract_epi32(sum0, 2)) + ((u32)_mm256_extract_epi32(sum0, 4)) + ((u32)_mm256_extract_epi32(sum0, 6));
    sad[1] = ((u32)_mm256_extract_epi32(sum1, 0)) + ((u32)_mm256_extract_epi32(sum1, 2)) + ((u32)_mm256_extract_epi32(sum1, 4)) + ((u32)_mm256_extract_epi32(sum1, 6));
    sad[2] = ((u32)_mm256_extract_epi32(sum2, 0)) + ((u32)_mm256_extract_epi32(sum2, 2)) + ((u32)_mm256_extract_epi32(sum2, 4)) + ((u32)_mm256_extract_epi32(sum2, 6));
    sad[3] = ((u32)_mm256_extract_epi32(sum3, 0)) + ((u32)_mm256_extract_epi32(sum3, 2)) + ((u32)_mm256_extract_epi32(sum3, 4)) + ((u32)_mm256_extract_epi32(sum3, 6));
}

void uavs3e_get_sad_x4_64_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[4], int height)
{
    __m256i sum0, sum1, sum2, sum3;

    sum0 = sum1 = sum2 = sum3 = _mm256_setzero_si256();

    while (height--) {
        CAL_SAD_X4_W32(0);
        CAL_SAD_X4_W32(1);
        p_org += i_org;
        p_pred0 += i_pred;
        p_pred1 += i_pred;
        p_pred2 += i_pred;
        p_pred3 += i_pred;
    }
    sad[0] = ((u32)_mm256_extract_epi32(sum0, 0)) + ((u32)_mm256_extract_epi32(sum0, 2)) + ((u32)_mm256_extract_epi32(sum0, 4)) + ((u32)_mm256_extract_epi32(sum0, 6));
    sad[1] = ((u32)_mm256_extract_epi32(sum1, 0)) + ((u32)_mm256_extract_epi32(sum1, 2)) + ((u32)_mm256_extract_epi32(sum1, 4)) + ((u32)_mm256_extract_epi32(sum1, 6));
    sad[2] = ((u32)_mm256_extract_epi32(sum2, 0)) + ((u32)_mm256_extract_epi32(sum2, 2)) + ((u32)_mm256_extract_epi32(sum2, 4)) + ((u32)_mm256_extract_epi32(sum2, 6));
    sad[3] = ((u32)_mm256_extract_epi32(sum3, 0)) + ((u32)_mm256_extract_epi32(sum3, 2)) + ((u32)_mm256_extract_epi32(sum3, 4)) + ((u32)_mm256_extract_epi32(sum3, 6));
}

void uavs3e_get_sad_x4_128_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[4], int height)
{
    __m256i sum0, sum1, sum2, sum3;

    sum0 = sum1 = sum2 = sum3 = _mm256_setzero_si256();

    while (height--) {
        CAL_SAD_X4_W32(0);
        CAL_SAD_X4_W32(1);
        CAL_SAD_X4_W32(2);
        CAL_SAD_X4_W32(3);
        p_org += i_org;
        p_pred0 += i_pred;
        p_pred1 += i_pred;
        p_pred2 += i_pred;
        p_pred3 += i_pred;
    }
    sad[0] = ((u32)_mm256_extract_epi32(sum0, 0)) + ((u32)_mm256_extract_epi32(sum0, 2)) + ((u32)_mm256_extract_epi32(sum0, 4)) + ((u32)_mm256_extract_epi32(sum0, 6));
    sad[1] = ((u32)_mm256_extract_epi32(sum1, 0)) + ((u32)_mm256_extract_epi32(sum1, 2)) + ((u32)_mm256_extract_epi32(sum1, 4)) + ((u32)_mm256_extract_epi32(sum1, 6));
    sad[2] = ((u32)_mm256_extract_epi32(sum2, 0)) + ((u32)_mm256_extract_epi32(sum2, 2)) + ((u32)_mm256_extract_epi32(sum2, 4)) + ((u32)_mm256_extract_epi32(sum2, 6));
    sad[3] = ((u32)_mm256_extract_epi32(sum3, 0)) + ((u32)_mm256_extract_epi32(sum3, 2)) + ((u32)_mm256_extract_epi32(sum3, 4)) + ((u32)_mm256_extract_epi32(sum3, 6));
}

u64 uavs3e_get_ssd_4_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m256i sum = _mm256_setzero_si256();
    int i_orgx2  = i_org  << 1;
    int i_orgx3  = i_orgx2 + i_org;
    int i_predx2 = i_pred << 1;
    int i_predx3 = i_predx2 + i_pred;

    height >>= 2;
    while (height--) {
        __m128i A = _mm_setr_epi32(*(int*)(p_org), *(int*)(p_org + i_org), *(int*)(p_org + i_orgx2), *(int*)(p_org + i_orgx3));
        __m128i B = _mm_setr_epi32(*(int*)(p_pred), *(int*)(p_pred + i_pred), *(int*)(p_pred + i_predx2), *(int*)(p_pred + i_predx3));
        __m256i C = _mm256_sub_epi16(_mm256_cvtepu8_epi16(A), _mm256_cvtepu8_epi16(B));
        __m256i D = _mm256_madd_epi16(C, C);
        sum = _mm256_add_epi32(sum, D);

        p_org  += i_org << 2;
        p_pred += i_pred << 2;
    }
    sum = _mm256_hadd_epi32(sum, sum);
    sum = _mm256_hadd_epi32(sum, sum);
    return ((u32)_mm256_extract_epi32(sum, 0)) + ((u32)_mm256_extract_epi32(sum, 4));
}

u64 uavs3e_get_ssd_8_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m256i sum = _mm256_setzero_si256();

    height >>= 1;
    while (height--) {
        __m128i A = _mm_insert_epi64(_mm_loadl_epi64((__m128i*)(p_org )), *(u64*)(p_org  + i_org ), 1);
        __m128i B = _mm_insert_epi64(_mm_loadl_epi64((__m128i*)(p_pred)), *(u64*)(p_pred + i_pred), 1);
        __m256i C = _mm256_sub_epi16(_mm256_cvtepu8_epi16(A), _mm256_cvtepu8_epi16(B));
        __m256i D = _mm256_madd_epi16(C, C); 
        sum = _mm256_add_epi32(sum, D); 

        p_org  += i_org  << 1;
        p_pred += i_pred << 1;
    }
    sum = _mm256_hadd_epi32(sum, sum);
    sum = _mm256_hadd_epi32(sum, sum);
    return ((u32)_mm256_extract_epi32(sum, 0)) + ((u32)_mm256_extract_epi32(sum, 4));
}

#define CAL_SSD_W16(d) { \
        __m256i A = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)(p_org  + d * 16))); \
        __m256i B = _mm256_cvtepu8_epi16(_mm_loadu_si128((__m128i*)(p_pred  + d * 16))); \
        __m256i C = _mm256_sub_epi16(A, B); \
        __m256i D = _mm256_madd_epi16(C, C); \
        sum = _mm256_add_epi32(sum, D); \
    }

u64 uavs3e_get_ssd_16_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m256i sum = _mm256_setzero_si256();

    while (height--) {
        CAL_SSD_W16(0);
        p_org += i_org;
        p_pred += i_pred;
    }
    sum = _mm256_hadd_epi32(sum, sum);
    sum = _mm256_hadd_epi32(sum, sum);
    return ((u32)_mm256_extract_epi32(sum, 0)) + ((u32)_mm256_extract_epi32(sum, 4));
}

u64 uavs3e_get_ssd_32_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m256i sum = _mm256_setzero_si256();

    while (height--) {
        CAL_SSD_W16(0);
        CAL_SSD_W16(1);
        p_org += i_org;
        p_pred += i_pred;
    }
    sum = _mm256_hadd_epi32(sum, sum);
    sum = _mm256_hadd_epi32(sum, sum);
    return ((u32)_mm256_extract_epi32(sum, 0)) + ((u32)_mm256_extract_epi32(sum, 4));
}

u64 uavs3e_get_ssd_64_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    u64 total = 0;

    while (height) {
        __m256i sum = _mm256_setzero_si256();
        int h = COM_MIN(height, 64);
        height -= h;

        while (h--) {
            CAL_SSD_W16(0);
            CAL_SSD_W16(1);
            CAL_SSD_W16(2);
            CAL_SSD_W16(3);
            p_org += i_org;
            p_pred += i_pred;
        }

        sum = _mm256_hadd_epi32(sum, sum);
        sum = _mm256_hadd_epi32(sum, sum);
        total += ((u32)_mm256_extract_epi32(sum, 0)) + ((u32)_mm256_extract_epi32(sum, 4));
    }
    return total;
}

u64 uavs3e_get_ssd_128_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    u64 total = 0;

    while (height) {
        __m256i sum = _mm256_setzero_si256();
        int h = COM_MIN(height, 32);
        height -= h;

        while (h--) {
            CAL_SSD_W16(0);
            CAL_SSD_W16(1);
            CAL_SSD_W16(2);
            CAL_SSD_W16(3);
            CAL_SSD_W16(4);
            CAL_SSD_W16(5);
            CAL_SSD_W16(6);
            CAL_SSD_W16(7);
            p_org += i_org;
            p_pred += i_pred;
        }

        sum = _mm256_hadd_epi32(sum, sum);
        sum = _mm256_hadd_epi32(sum, sum);
        total += ((u32)_mm256_extract_epi32(sum, 0)) + ((u32)_mm256_extract_epi32(sum, 4));
    }
    return total;
}

u32 uavs3e_had_4x8_avx2(pel *org, int s_org, pel *cur, int s_cur)
{
    int k;
    __m256i m1[4], m2[8];
    __m256i sum;
    int satd = 0;
    
    for (k = 0; k < 4; k++) {
        __m256i r0 = _mm256_cvtepu8_epi16(_mm_set_epi32(0, *(s32*)(org + s_org), 0, *(s32*)(org)));
        __m256i r1 = _mm256_cvtepu8_epi16(_mm_set_epi32(0, *(s32*)(cur + s_cur), 0, *(s32*)(cur)));
        m2[k] = _mm256_sub_epi16(r0, r1);
        org += s_org << 1;
        cur += s_cur << 1;
    }
    // vertical
    m1[0] = _mm256_add_epi16(m2[0], m2[2]);
    m1[1] = _mm256_add_epi16(m2[1], m2[3]);
    m1[2] = _mm256_sub_epi16(m2[0], m2[2]);
    m1[3] = _mm256_sub_epi16(m2[1], m2[3]);
    m2[0] = _mm256_add_epi16(m1[0], m1[1]);
    m2[1] = _mm256_sub_epi16(m1[0], m1[1]);
    m2[2] = _mm256_add_epi16(m1[2], m1[3]);
    m2[3] = _mm256_sub_epi16(m1[2], m1[3]);
    m2[4] = _mm256_permute2x128_si256(m2[0], m2[1], 0x02);
    m2[5] = _mm256_permute2x128_si256(m2[0], m2[1], 0x13);
    m2[6] = _mm256_permute2x128_si256(m2[2], m2[3], 0x02);
    m2[7] = _mm256_permute2x128_si256(m2[2], m2[3], 0x13);
    m1[0] = _mm256_add_epi16(m2[4], m2[5]);     // 02
    m1[1] = _mm256_sub_epi16(m2[4], m2[5]);     // 13
    m1[2] = _mm256_add_epi16(m2[6], m2[7]);
    m1[3] = _mm256_sub_epi16(m2[6], m2[7]);

    // horizontal
    // transpose
    m2[0] = _mm256_unpacklo_epi16(m1[0], m1[1]);
    m2[1] = _mm256_unpacklo_epi16(m1[2], m1[3]);
    m2[2] = _mm256_permute2x128_si256(m2[0], m2[1], 0x02);
    m2[3] = _mm256_permute2x128_si256(m2[0], m2[1], 0x13);
    m1[0] = _mm256_unpacklo_epi32(m2[2], m2[3]);
    m1[1] = _mm256_unpackhi_epi32(m2[2], m2[3]);
    m1[2] = _mm256_permute2x128_si256(m1[0], m1[1], 0x02);
    m1[3] = _mm256_permute2x128_si256(m1[0], m1[1], 0x13);
    m2[0] = _mm256_unpacklo_epi64(m1[2], m1[3]);
    m2[1] = _mm256_unpackhi_epi64(m1[2], m1[3]);
    m2[2] = _mm256_permute2x128_si256(m2[0], m2[1], 0x02);
    m2[3] = _mm256_permute2x128_si256(m2[0], m2[1], 0x13);

    m1[0] = _mm256_add_epi16(m2[2], m2[3]);
    m1[1] = _mm256_sub_epi16(m2[2], m2[3]);
    m1[2] = _mm256_permute2x128_si256(m1[0], m1[1], 0x02);
    m1[3] = _mm256_permute2x128_si256(m1[0], m1[1], 0x13);

    m2[0] = _mm256_add_epi16(m1[2], m1[3]);
    m2[1] = _mm256_sub_epi16(m1[2], m1[3]);
    m2[0] = _mm256_abs_epi16(m2[0]);
    m2[1] = _mm256_abs_epi16(m2[1]);

    sum = _mm256_add_epi16(m2[0], m2[1]);
    {
        __m256i C = _mm256_set1_epi16(1);
        sum = _mm256_madd_epi16(sum, C);
        sum = _mm256_hadd_epi32(sum, sum);
        sum = _mm256_hadd_epi32(sum, sum);
    }
    
    satd = (int)((_mm256_extract_epi32(sum, 0) + _mm256_extract_epi32(sum, 4)) / com_tbl_sqrt[0] * 2);
    return satd;
}

u32 uavs3e_had_8x4_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred)
{
    __m256i T01, T23;
    __m256i M01, M23;
    __m256i N01, N23;
    __m256i sign = _mm256_set_epi16(1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1);
   
    #define HOR_LINE256(i, j) { \
        __m256i T0, T1, T2, T3, T4, T5, T6; \
        T0 = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p_org + i_org), *(s64*)p_org)); p_org  += (i_org  << 1);\
        T1 = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p_pred + i_pred), *(s64*)p_pred)); p_pred += (i_pred << 1);\
        T4 = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p_org + i_org), *(s64*)p_org)); p_org += (i_org << 1); \
        T5 = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p_pred + i_pred), *(s64*)p_pred)); p_pred += (i_pred << 1); \
        T0 = _mm256_sub_epi16(T0, T1); \
        T4 = _mm256_sub_epi16(T4, T5); \
        T1 = _mm256_unpacklo_epi64(T0, T4); \
        T5 = _mm256_unpackhi_epi64(T0, T4); \
        T2 = _mm256_add_epi16(T1, T5); \
        T3 = _mm256_sub_epi16(T1, T5); \
        T0 = _mm256_unpacklo_epi32(T2, T3); \
        T4 = _mm256_unpackhi_epi32(T2, T3); \
        T1 = _mm256_unpackhi_epi32(T0, T0); \
        T2 = _mm256_unpacklo_epi32(T0, T0); \
        T5 = _mm256_unpackhi_epi32(T4, T4); \
        T6 = _mm256_unpacklo_epi32(T4, T4); \
        T1 = _mm256_sign_epi16(T1, sign); \
        T5 = _mm256_sign_epi16(T5, sign); \
        T0 = _mm256_add_epi16(T2, T1); \
        T4 = _mm256_add_epi16(T6, T5); \
        T1 = _mm256_hadd_epi16(T0, T4); \
        T2 = _mm256_hsub_epi16(T0, T4); \
        M##i = _mm256_unpacklo_epi16(T1, T2); \
        M##j = _mm256_unpackhi_epi16(T1, T2); \
    }

    HOR_LINE256(01, 23)

#undef HOR_LINE256

    // vertical

    T01 = _mm256_add_epi16(M01, M23);
    T23 = _mm256_sub_epi16(M01, M23);
    
    N01 = _mm256_permute2x128_si256(T01, T23, 0x02);
    N23 = _mm256_permute2x128_si256(T01, T23, 0x13);

    T01 = _mm256_abs_epi16(_mm256_add_epi16(N01, N23));
    T23 = _mm256_abs_epi16(_mm256_sub_epi16(N01, N23));
 
    T01 = _mm256_add_epi16(T01, T23);
    
    T23 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(T01));
    T01 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(T01, 1));

    T01 = _mm256_add_epi32(T01, T23);

    T01 = _mm256_hadd_epi32(T01, T01);
    T01 = _mm256_hadd_epi32(T01, T01);

    return (u32)((_mm256_extract_epi32(T01, 0) + _mm256_extract_epi32(T01, 4)) / com_tbl_sqrt[0] * 2.0);
}

u32 uavs3e_had_8x8_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred)
{
    __m256i T01, T23, T45, T67;
    __m256i M01, M23, M45, M67;
    __m256i N01, N23, N45, N67;
    __m256i sign = _mm256_set_epi16(1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1);
   
    #define HOR_LINE256(i, j) { \
        __m256i T0, T1, T2, T3, T4, T5, T6; \
        T0 = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p_org + i_org), *(s64*)p_org)); p_org  += (i_org  << 1);\
        T1 = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p_pred + i_pred), *(s64*)p_pred)); p_pred += (i_pred << 1);\
        T4 = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p_org + i_org), *(s64*)p_org)); p_org += (i_org << 1); \
        T5 = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p_pred + i_pred), *(s64*)p_pred)); p_pred += (i_pred << 1); \
        T0 = _mm256_sub_epi16(T0, T1); \
        T4 = _mm256_sub_epi16(T4, T5); \
        T1 = _mm256_unpacklo_epi64(T0, T4); \
        T5 = _mm256_unpackhi_epi64(T0, T4); \
        T2 = _mm256_add_epi16(T1, T5); \
        T3 = _mm256_sub_epi16(T1, T5); \
        T0 = _mm256_unpacklo_epi32(T2, T3); \
        T4 = _mm256_unpackhi_epi32(T2, T3); \
        T1 = _mm256_unpackhi_epi32(T0, T0); \
        T2 = _mm256_unpacklo_epi32(T0, T0); \
        T5 = _mm256_unpackhi_epi32(T4, T4); \
        T6 = _mm256_unpacklo_epi32(T4, T4); \
        T1 = _mm256_sign_epi16(T1, sign); \
        T5 = _mm256_sign_epi16(T5, sign); \
        T0 = _mm256_add_epi16(T2, T1); \
        T4 = _mm256_add_epi16(T6, T5); \
        T1 = _mm256_hadd_epi16(T0, T4); \
        T2 = _mm256_hsub_epi16(T0, T4); \
        M##i = _mm256_unpacklo_epi16(T1, T2); \
        M##j = _mm256_unpackhi_epi16(T1, T2); \
    }

    HOR_LINE256(01, 23)
    HOR_LINE256(45, 67)

#undef HOR_LINE256

    // vertical
    T01 = _mm256_add_epi16(M01, M45);
    T23 = _mm256_add_epi16(M23, M67);
    T45 = _mm256_sub_epi16(M01, M45);
    T67 = _mm256_sub_epi16(M23, M67);
    
    M01 = _mm256_add_epi16(T01, T23);
    M23 = _mm256_sub_epi16(T01, T23);
    M45 = _mm256_add_epi16(T45, T67);
    M67 = _mm256_sub_epi16(T45, T67);
    
    N01 = _mm256_permute2x128_si256(M01, M23, 0x02);
    N23 = _mm256_permute2x128_si256(M01, M23, 0x13);
    N45 = _mm256_permute2x128_si256(M45, M67, 0x02);
    N67 = _mm256_permute2x128_si256(M45, M67, 0x13);

    T01 = _mm256_abs_epi16(_mm256_add_epi16(N01, N23));
    T23 = _mm256_abs_epi16(_mm256_sub_epi16(N01, N23));
    T45 = _mm256_abs_epi16(_mm256_add_epi16(N45, N67));
    T67 = _mm256_abs_epi16(_mm256_sub_epi16(N45, N67));
 
    {
        __m256i C = _mm256_set1_epi16(1);
        T01 = _mm256_madd_epi16(T01, C);
        T23 = _mm256_madd_epi16(T23, C);
        T45 = _mm256_madd_epi16(T45, C);
        T67 = _mm256_madd_epi16(T67, C);

        T01 = _mm256_add_epi32(T01, T23);
        T45 = _mm256_add_epi32(T45, T67);
        T01 = _mm256_add_epi32(T01, T45);

        T01 = _mm256_hadd_epi32(T01, T01);
        T01 = _mm256_hadd_epi32(T01, T01);
    }
    return ((_mm256_extract_epi32(T01, 0) + _mm256_extract_epi32(T01, 4)) + 2) >> 2;
}

u32 uavs3e_had_16x8_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred)
{
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;
    __m256i sign = _mm256_set_epi16(1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1);

#define HOR_LINE(i, j) { \
    T0 = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p_org  + i_org ), *(s64*)p_org )); \
    T1 = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p_pred + i_pred), *(s64*)p_pred)); \
    T2 = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p_org  + i_org  + 8), *(s64*)(p_org  + 8))); p_org  += i_org  << 1; \
    T3 = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p_pred + i_pred + 8), *(s64*)(p_pred + 8))); p_pred += i_pred << 1;\
    T1 = _mm256_sub_epi16(T0, T1); \
    T5 = _mm256_sub_epi16(T2, T3); \
    T0 = _mm256_add_epi16(T1, T5); \
    T4 = _mm256_sub_epi16(T1, T5); \
    T1 = _mm256_unpacklo_epi64(T0, T4); \
    T5 = _mm256_unpackhi_epi64(T0, T4); \
    T2 = _mm256_add_epi16(T1, T5); \
    T3 = _mm256_sub_epi16(T1, T5); \
    T0 = _mm256_unpacklo_epi32(T2, T3); \
    T4 = _mm256_unpackhi_epi32(T2, T3); \
    T1 = _mm256_unpackhi_epi32(T0, T0); \
    T2 = _mm256_unpacklo_epi32(T0, T0); \
    T5 = _mm256_unpackhi_epi32(T4, T4); \
    T6 = _mm256_unpacklo_epi32(T4, T4); \
    T1 = _mm256_sign_epi16(T1, sign); \
    T5 = _mm256_sign_epi16(T5, sign); \
    T0 = _mm256_add_epi16(T2, T1); \
    T4 = _mm256_add_epi16(T6, T5); \
    T1 = _mm256_hadd_epi16(T0, T4); \
    T2 = _mm256_hsub_epi16(T0, T4); \
    T0 = _mm256_unpacklo_epi16(T1, T2); \
    T4 = _mm256_unpackhi_epi16(T1, T2); \
    M##i = _mm256_permute2x128_si256(T0, T4, 0x02); \
    M##j = _mm256_permute2x128_si256(T0, T4, 0x13); \
}
    HOR_LINE(0, 1);
    HOR_LINE(2, 3);
    HOR_LINE(4, 5);
    HOR_LINE(6, 7);

#undef HOR_LINE

    // vertical
    T0 = _mm256_add_epi16(M0, M4); 
    T1 = _mm256_add_epi16(M1, M5); 
    T2 = _mm256_add_epi16(M2, M6); 
    T3 = _mm256_add_epi16(M3, M7); 
    T4 = _mm256_sub_epi16(M0, M4); 
    T5 = _mm256_sub_epi16(M1, M5); 
    T6 = _mm256_sub_epi16(M2, M6); 
    T7 = _mm256_sub_epi16(M3, M7); 
            
    M0 = _mm256_add_epi16(T0, T2); 
    M1 = _mm256_add_epi16(T1, T3); 
    M2 = _mm256_sub_epi16(T0, T2); 
    M3 = _mm256_sub_epi16(T1, T3); 
    M4 = _mm256_add_epi16(T4, T6); 
    M5 = _mm256_add_epi16(T5, T7); 
    M6 = _mm256_sub_epi16(T4, T6); 
    M7 = _mm256_sub_epi16(T5, T7); 

    T0 = _mm256_abs_epi16(_mm256_add_epi16(M0, M1)); 
    T1 = _mm256_abs_epi16(_mm256_sub_epi16(M0, M1)); 
    T2 = _mm256_abs_epi16(_mm256_add_epi16(M2, M3)); 
    T3 = _mm256_abs_epi16(_mm256_sub_epi16(M2, M3)); 
    T4 = _mm256_abs_epi16(_mm256_add_epi16(M4, M5)); 
    T5 = _mm256_abs_epi16(_mm256_sub_epi16(M4, M5)); 
    T6 = _mm256_abs_epi16(_mm256_add_epi16(M6, M7)); 
    T7 = _mm256_abs_epi16(_mm256_sub_epi16(M6, M7)); 
    {
        __m256i C = _mm256_set1_epi16(1);
        T0 = _mm256_madd_epi16(T0, C);
        T1 = _mm256_madd_epi16(T1, C);
        T2 = _mm256_madd_epi16(T2, C);
        T3 = _mm256_madd_epi16(T3, C);
        T4 = _mm256_madd_epi16(T4, C);
        T5 = _mm256_madd_epi16(T5, C);
        T6 = _mm256_madd_epi16(T6, C);
        T7 = _mm256_madd_epi16(T7, C);

        T0 = _mm256_add_epi32(T0, T1);
        T2 = _mm256_add_epi32(T2, T3);
        T4 = _mm256_add_epi32(T4, T5);
        T6 = _mm256_add_epi32(T6, T7);

        T0 = _mm256_add_epi32(T0, T2);
        T4 = _mm256_add_epi32(T4, T6);

        M0 = _mm256_add_epi32(T0, T4);

        M0 = _mm256_hadd_epi32(M0, M0);
        M0 = _mm256_hadd_epi32(M0, M0);
    }
    return (u32)((_mm256_extract_epi32(M0, 0) + _mm256_extract_epi32(M0, 4)) / com_tbl_sqrt[1] * 2);
}

u32 uavs3e_had_8x16_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred)
{
    __m256i T01, T23, T45, T67, T89, T1011, T1213, T1415;
    __m256i M01, M23, M45, M67, M89, M1011, M1213, M1415;
    __m256i N01, N23, N45, N67, N89, N1011, N1213, N1415;
    __m256i sign = _mm256_set_epi16(1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1);
   
    #define HOR_LINE256(i, j) { \
        __m256i T0, T1, T2, T3, T4, T5, T6; \
        T0 = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p_org + i_org), *(s64*)p_org)); p_org  += (i_org  << 1);\
        T1 = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p_pred + i_pred), *(s64*)p_pred)); p_pred += (i_pred << 1);\
        T4 = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p_org + i_org), *(s64*)p_org)); p_org += (i_org << 1); \
        T5 = _mm256_cvtepu8_epi16(_mm_set_epi64x(*(s64*)(p_pred + i_pred), *(s64*)p_pred)); p_pred += (i_pred << 1); \
        T0 = _mm256_sub_epi16(T0, T1); \
        T4 = _mm256_sub_epi16(T4, T5); \
        T1 = _mm256_unpacklo_epi64(T0, T4); \
        T5 = _mm256_unpackhi_epi64(T0, T4); \
        T2 = _mm256_add_epi16(T1, T5); \
        T3 = _mm256_sub_epi16(T1, T5); \
        T0 = _mm256_unpacklo_epi32(T2, T3); \
        T4 = _mm256_unpackhi_epi32(T2, T3); \
        T1 = _mm256_unpackhi_epi32(T0, T0); \
        T2 = _mm256_unpacklo_epi32(T0, T0); \
        T5 = _mm256_unpackhi_epi32(T4, T4); \
        T6 = _mm256_unpacklo_epi32(T4, T4); \
        T1 = _mm256_sign_epi16(T1, sign); \
        T5 = _mm256_sign_epi16(T5, sign); \
        T0 = _mm256_add_epi16(T2, T1); \
        T4 = _mm256_add_epi16(T6, T5); \
        T1 = _mm256_hadd_epi16(T0, T4); \
        T2 = _mm256_hsub_epi16(T0, T4); \
        M##i = _mm256_unpacklo_epi16(T1, T2); \
        M##j = _mm256_unpackhi_epi16(T1, T2); \
    }

    HOR_LINE256(01, 23)
    HOR_LINE256(45, 67)
    HOR_LINE256(89, 1011)
    HOR_LINE256(1213, 1415)

#undef HOR_LINE256

    // vertical
    N01   = _mm256_add_epi16(M01, M89  );
    N23   = _mm256_add_epi16(M23, M1011);
    N45   = _mm256_add_epi16(M45, M1213);
    N67   = _mm256_add_epi16(M67, M1415);
    N89   = _mm256_sub_epi16(M01, M89  );
    N1011 = _mm256_sub_epi16(M23, M1011);
    N1213 = _mm256_sub_epi16(M45, M1213);
    N1415 = _mm256_sub_epi16(M67, M1415);

    T01   = _mm256_add_epi16(N01, N45);
    T23   = _mm256_add_epi16(N23, N67);
    T45   = _mm256_sub_epi16(N01, N45);
    T67   = _mm256_sub_epi16(N23, N67);
    T89   = _mm256_add_epi16(N89,   N1213);
    T1011 = _mm256_add_epi16(N1011, N1415);
    T1213 = _mm256_sub_epi16(N89,   N1213);
    T1415 = _mm256_sub_epi16(N1011, N1415);

    M01   = _mm256_add_epi16(T01, T23);
    M23   = _mm256_sub_epi16(T01, T23);
    M45   = _mm256_add_epi16(T45, T67);
    M67   = _mm256_sub_epi16(T45, T67);
    M89   = _mm256_add_epi16(T89, T1011);
    M1011 = _mm256_sub_epi16(T89, T1011);
    M1213 = _mm256_add_epi16(T1213, T1415);
    M1415 = _mm256_sub_epi16(T1213, T1415);

    N01   = _mm256_permute2x128_si256(M01, M23, 0x02);
    N23   = _mm256_permute2x128_si256(M01, M23, 0x13);
    N45   = _mm256_permute2x128_si256(M45, M67, 0x02);
    N67   = _mm256_permute2x128_si256(M45, M67, 0x13);
    N89   = _mm256_permute2x128_si256(M89, M1011, 0x02);
    N1011 = _mm256_permute2x128_si256(M89, M1011, 0x13);
    N1213 = _mm256_permute2x128_si256(M1213, M1415, 0x02);
    N1415 = _mm256_permute2x128_si256(M1213, M1415, 0x13);

    T01   = _mm256_abs_epi16(_mm256_add_epi16(N01, N23));
    T23   = _mm256_abs_epi16(_mm256_sub_epi16(N01, N23));
    T45   = _mm256_abs_epi16(_mm256_add_epi16(N45, N67));
    T67   = _mm256_abs_epi16(_mm256_sub_epi16(N45, N67));
    T89   = _mm256_abs_epi16(_mm256_add_epi16(N89, N1011));
    T1011 = _mm256_abs_epi16(_mm256_sub_epi16(N89, N1011));
    T1213 = _mm256_abs_epi16(_mm256_add_epi16(N1213, N1415));
    T1415 = _mm256_abs_epi16(_mm256_sub_epi16(N1213, N1415));

    {
        __m256i C = _mm256_set1_epi16(1);
        T01   = _mm256_madd_epi16(T01  , C);
        T23   = _mm256_madd_epi16(T23  , C);
        T45   = _mm256_madd_epi16(T45  , C);
        T67   = _mm256_madd_epi16(T67  , C);
        T89   = _mm256_madd_epi16(T89  , C);
        T1011 = _mm256_madd_epi16(T1011, C);
        T1213 = _mm256_madd_epi16(T1213, C);
        T1415 = _mm256_madd_epi16(T1415, C);

        T01 = _mm256_add_epi32(T01, T23);
        T23 = _mm256_add_epi32(T45, T67);
        T45 = _mm256_add_epi32(T89, T1011);
        T67 = _mm256_add_epi32(T1213, T1415);

        T01 = _mm256_add_epi32(T01, T23);
        T45 = _mm256_add_epi32(T45, T67);

        T01 = _mm256_add_epi32(T01, T45);

        T01 = _mm256_hadd_epi32(T01, T01);
        T01 = _mm256_hadd_epi32(T01, T01);
    }

    return (u32)((_mm256_extract_epi32(T01, 0) + _mm256_extract_epi32(T01, 4)) / com_tbl_sqrt[1] * 2.0);
}

#elif (BIT_DEPTH == 10)

u32 uavs3e_get_sad_4_avx2(pel* p_org, int i_org, pel* p_pred, int i_pred, int height)
{
    __m256i sum;
    __m256i zero = _mm256_setzero_si256();
    __m256i o, p, t0, t1;
    int i_org2 = i_org << 1;
    int i_org3 = i_org2 + i_org;
    int i_pred2 = i_pred << 1;
    int i_pred3 = i_pred2 + i_pred;

    sum = zero;
    height >>= 2;

    while (height--) {
        o = _mm256_set_epi64x(*(s64*)(p_org + i_org3), *(s64*)(p_org + i_org2), *(s64*)(p_org + i_org), *(s64*)p_org);
        p = _mm256_set_epi64x(*(s64*)(p_pred + i_pred3), *(s64*)(p_pred + i_pred2), *(s64*)(p_pred + i_pred), *(s64*)p_pred);
        t0 = _mm256_sub_epi16(o, p);
        t0 = _mm256_abs_epi16(t0);
        sum = _mm256_add_epi16(sum, t0);
        p_org += i_org << 2;
        p_pred += i_pred << 2;
    }
    t0 = _mm256_unpacklo_epi16(sum, zero);
    t1 = _mm256_unpackhi_epi16(sum, zero);
    sum = _mm256_add_epi32(t0, t1);
    sum = _mm256_hadd_epi32(sum, sum);
    sum = _mm256_hadd_epi32(sum, sum);
    return ((u32)_mm256_extract_epi32(sum, 0)) + ((u32)_mm256_extract_epi32(sum, 4));
}

u32 uavs3e_get_sad_8_avx2(pel* p_org, int i_org, pel* p_pred, int i_pred, int height)
{
    __m256i sum;;
    __m256i zero = _mm256_setzero_si256();
    __m256i O0, O1, P0, P1, S0, S1;
    int i_org2 = i_org << 1;
    int i_org3 = i_org2 + i_org;
    int i_pred2 = i_pred << 1;
    int i_pred3 = i_pred2 + i_pred;

    sum = zero;
    height >>= 2;

    while (height--) {
        O0 = _mm256_loadu2_m128i((__m128i*)(p_org + i_org), (__m128i*)p_org);
        O1 = _mm256_loadu2_m128i((__m128i*)(p_org + i_org3), (__m128i*)(p_org + i_org2));
        P0 = _mm256_loadu2_m128i((__m128i*)(p_pred + i_pred), (__m128i*)p_pred);
        P1 = _mm256_loadu2_m128i((__m128i*)(p_pred + i_pred3), (__m128i*)(p_pred + i_pred2));
        S0 = _mm256_sub_epi16(O0, P0);
        S1 = _mm256_sub_epi16(O1, P1);
        S0 = _mm256_abs_epi16(S0);
        S1 = _mm256_abs_epi16(S1);
        sum = _mm256_add_epi16(sum, S0);
        sum = _mm256_add_epi16(sum, S1);
        p_org += i_org << 2;
        p_pred += i_pred << 2;
    }
    S0 = _mm256_unpacklo_epi16(sum, zero);
    S1 = _mm256_unpackhi_epi16(sum, zero);
    sum = _mm256_add_epi32(S0, S1);
    sum = _mm256_hadd_epi32(sum, sum);
    sum = _mm256_hadd_epi32(sum, sum);
    return ((u32)_mm256_extract_epi32(sum, 0)) + ((u32)_mm256_extract_epi32(sum, 4));
}

u32 uavs3e_get_sad_16_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m256i sum0, sum1;;
    __m256i zero = _mm256_setzero_si256();
    __m256i O0, O1, P0, P1, S0, S1, S2, S3;

    sum0 = sum1 = zero;
    height >>= 1;

    while (height--) {
        O0 = _mm256_loadu_si256((__m256i *)p_org);
        O1 = _mm256_loadu_si256((__m256i *)(p_org + i_org));
        P0 = _mm256_loadu_si256((__m256i *)p_pred);
        P1 = _mm256_loadu_si256((__m256i *)(p_pred + i_pred));
        S0 = _mm256_sub_epi16(O0, P0);
        S1 = _mm256_sub_epi16(O1, P1);
        S0 = _mm256_abs_epi16(S0);
        S1 = _mm256_abs_epi16(S1);
        sum0 = _mm256_add_epi16(sum0, S0);
        sum1 = _mm256_add_epi16(sum1, S1);
        p_org += i_org << 1;
        p_pred += i_pred << 1;
    }
    S0 = _mm256_unpacklo_epi16(sum0, zero);
    S1 = _mm256_unpackhi_epi16(sum0, zero);
    S2 = _mm256_unpacklo_epi16(sum1, zero);
    S3 = _mm256_unpackhi_epi16(sum1, zero);
    sum0 = _mm256_add_epi32(S0, S1);
    sum1 = _mm256_add_epi32(S2, S3);
    sum0 = _mm256_add_epi32(sum0, sum1);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    return ((u32)_mm256_extract_epi32(sum0, 0)) + ((u32)_mm256_extract_epi32(sum0, 4));
}

u32 uavs3e_get_sad_32_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m256i zero = _mm256_setzero_si256();
    __m256i o0, o1, o2, o3;
    __m256i p0, p1, p2, p3;
    __m256i t0, t1, t2, t3;
    __m256i sum0, sum1, sum2, sum3;

    height >>= 1;
    sum0 = sum1 = sum2 = sum3 = zero;
    while (height--) {
        o0 = _mm256_loadu_si256((__m256i*)(p_org));
        o1 = _mm256_loadu_si256((__m256i*)(p_org + 16));
        o2 = _mm256_loadu_si256((__m256i*)(p_org + i_org));
        o3 = _mm256_loadu_si256((__m256i*)(p_org + i_org + 16));
        p0 = _mm256_loadu_si256((__m256i*)(p_pred));
        p1 = _mm256_loadu_si256((__m256i*)(p_pred + 16));
        p2 = _mm256_loadu_si256((__m256i*)(p_pred + i_pred));
        p3 = _mm256_loadu_si256((__m256i*)(p_pred + i_pred + 16));
        t0 = _mm256_sub_epi16(o0, p0);
        t1 = _mm256_sub_epi16(o1, p1);
        t2 = _mm256_sub_epi16(o2, p2);
        t3 = _mm256_sub_epi16(o3, p3);
        t0 = _mm256_abs_epi16(t0);
        t1 = _mm256_abs_epi16(t1);
        t2 = _mm256_abs_epi16(t2);
        t3 = _mm256_abs_epi16(t3);
        p_org += i_org << 1;
        p_pred += i_pred << 1;
        sum0 = _mm256_add_epi16(sum0, t0);
        sum1 = _mm256_add_epi16(sum1, t1);
        sum2 = _mm256_add_epi16(sum2, t2);
        sum3 = _mm256_add_epi16(sum3, t3);
    }
    t0 = _mm256_unpacklo_epi16(sum0, zero);
    t1 = _mm256_unpackhi_epi16(sum0, zero);
    t2 = _mm256_unpacklo_epi16(sum1, zero);
    t3 = _mm256_unpackhi_epi16(sum1, zero);
    sum0 = _mm256_add_epi32(t0, t1);
    sum1 = _mm256_add_epi32(t2, t3);
    t0 = _mm256_unpacklo_epi16(sum2, zero);
    t1 = _mm256_unpackhi_epi16(sum2, zero);
    t2 = _mm256_unpacklo_epi16(sum3, zero);
    t3 = _mm256_unpackhi_epi16(sum3, zero);
    sum2 = _mm256_add_epi32(t0, t1);
    sum3 = _mm256_add_epi32(t2, t3);
    sum0 = _mm256_add_epi32(sum0, sum1);
    sum2 = _mm256_add_epi32(sum2, sum3);
    sum0 = _mm256_add_epi32(sum0, sum2);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    return ((u32)_mm256_extract_epi32(sum0, 0)) + ((u32)_mm256_extract_epi32(sum0, 4));
}

#define CAL_SAD_W32_10BIT(i, sum0, sum1, sum2, sum3)  \
        o0 = _mm256_loadu_si256((__m256i*)(p_org + i*32));                \
        o1 = _mm256_loadu_si256((__m256i*)(p_org + i*32 + 16));           \
        o2 = _mm256_loadu_si256((__m256i*)(p_org + i*32 + i_org));        \
        o3 = _mm256_loadu_si256((__m256i*)(p_org + i*32 + i_org + 16));   \
        p0 = _mm256_loadu_si256((__m256i*)(p_pred + i*32));               \
        p1 = _mm256_loadu_si256((__m256i*)(p_pred + i*32 + 16));          \
        p2 = _mm256_loadu_si256((__m256i*)(p_pred + i*32 + i_pred));      \
        p3 = _mm256_loadu_si256((__m256i*)(p_pred + i*32 + i_pred + 16)); \
        t0 = _mm256_sub_epi16(o0, p0);                             \
        t1 = _mm256_sub_epi16(o1, p1);                             \
        t2 = _mm256_sub_epi16(o2, p2);                             \
        t3 = _mm256_sub_epi16(o3, p3);                             \
        t0 = _mm256_abs_epi16(t0);                                 \
        t1 = _mm256_abs_epi16(t1);                                 \
        t2 = _mm256_abs_epi16(t2);                                 \
        t3 = _mm256_abs_epi16(t3);                                 \
        sum0 = _mm256_add_epi16(sum0, t0);                         \
        sum1 = _mm256_add_epi16(sum1, t1);                         \
        sum2 = _mm256_add_epi16(sum2, t2);                         \
        sum3 = _mm256_add_epi16(sum3, t3)

#define CAL_SAD_SUM_10BIT(sum0, sum1) \
        t0 = _mm256_unpacklo_epi16(sum0, zero); \
        t1 = _mm256_unpackhi_epi16(sum0, zero); \
        t2 = _mm256_unpacklo_epi16(sum1, zero); \
        t3 = _mm256_unpackhi_epi16(sum1, zero); \
        sum0 = _mm256_add_epi32(t0, t1);        \
        sum1 = _mm256_add_epi32(t2, t3);

u32 uavs3e_get_sad_64_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m256i zero = _mm256_setzero_si256();
    __m256i o0, o1, o2, o3;
    __m256i p0, p1, p2, p3;
    __m256i t0, t1, t2, t3;
    __m256i sum[8];
    int i;
    int i_org2 = i_org << 1;
    int i_pred2 = i_pred << 1;

    height >>= 1;
    for (i = 0; i < 8; i++) {
        sum[i] = zero;
    }

    while (height--) {
        CAL_SAD_W32_10BIT(0, sum[0], sum[1], sum[2], sum[3]);
        CAL_SAD_W32_10BIT(1, sum[4], sum[5], sum[6], sum[7]);
        p_org += i_org2;
        p_pred += i_pred2;
    }
    CAL_SAD_SUM_10BIT(sum[0], sum[1]);
    CAL_SAD_SUM_10BIT(sum[2], sum[3]);
    sum[0] = _mm256_add_epi32(sum[0], sum[1]);
    sum[2] = _mm256_add_epi32(sum[2], sum[3]);

    CAL_SAD_SUM_10BIT(sum[4], sum[5]);
    CAL_SAD_SUM_10BIT(sum[6], sum[7]);
    sum[4] = _mm256_add_epi32(sum[4], sum[5]);
    sum[6] = _mm256_add_epi32(sum[6], sum[7]);

    sum[0] = _mm256_add_epi32(sum[0], sum[2]);
    sum[4] = _mm256_add_epi32(sum[4], sum[6]);
    sum[0] = _mm256_add_epi32(sum[0], sum[4]);

    sum[0] = _mm256_hadd_epi32(sum[0], sum[0]);
    sum[0] = _mm256_hadd_epi32(sum[0], sum[0]);
    return ((u32)_mm256_extract_epi32(sum[0], 0)) + ((u32)_mm256_extract_epi32(sum[0], 4));
}

u32 uavs3e_get_sad_128_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m256i zero = _mm256_setzero_si256();
    __m256i sum[16];
    __m256i o0, o1, o2, o3;
    __m256i p0, p1, p2, p3;
    __m256i t0, t1, t2, t3;
    int i;
    for (i = 0; i < 16; i++) {
        sum[i] = zero;
    }

    while (height--) {
        CAL_SAD_W32_10BIT(0, sum[0], sum[1], sum[2], sum[3]);
        CAL_SAD_W32_10BIT(1, sum[4], sum[5], sum[6], sum[7]);
        CAL_SAD_W32_10BIT(2, sum[8], sum[9], sum[10], sum[11]);
        CAL_SAD_W32_10BIT(3, sum[12], sum[13], sum[14], sum[15]);
        p_org += i_org;
        p_pred += i_pred;
    }
    CAL_SAD_SUM_10BIT(sum[0], sum[1]);
    CAL_SAD_SUM_10BIT(sum[2], sum[3]);
    sum[0] = _mm256_add_epi32(sum[0], sum[1]);
    sum[2] = _mm256_add_epi32(sum[2], sum[3]);

    CAL_SAD_SUM_10BIT(sum[4], sum[5]);
    CAL_SAD_SUM_10BIT(sum[6], sum[7]);
    sum[4] = _mm256_add_epi32(sum[4], sum[5]);
    sum[6] = _mm256_add_epi32(sum[6], sum[7]);

    sum[0] = _mm256_add_epi32(sum[0], sum[2]);
    sum[4] = _mm256_add_epi32(sum[4], sum[6]);

    CAL_SAD_SUM_10BIT(sum[8], sum[9]);
    CAL_SAD_SUM_10BIT(sum[10], sum[11]);
    sum[8] = _mm256_add_epi32(sum[8], sum[9]);
    sum[10] = _mm256_add_epi32(sum[10], sum[11]);

    CAL_SAD_SUM_10BIT(sum[12], sum[13]);
    CAL_SAD_SUM_10BIT(sum[14], sum[15]);
    sum[12] = _mm256_add_epi32(sum[12], sum[13]);
    sum[14] = _mm256_add_epi32(sum[14], sum[15]);

    sum[8] = _mm256_add_epi32(sum[8], sum[10]);
    sum[12] = _mm256_add_epi32(sum[12], sum[14]);

    sum[0] = _mm256_add_epi32(sum[0], sum[4]);
    sum[8] = _mm256_add_epi32(sum[8], sum[12]);
    sum[0] = _mm256_add_epi32(sum[0], sum[8]);

    sum[0] = _mm256_hadd_epi32(sum[0], sum[0]);
    sum[0] = _mm256_hadd_epi32(sum[0], sum[0]);
    return ((u32)_mm256_extract_epi32(sum[0], 0)) + ((u32)_mm256_extract_epi32(sum[0], 4));
}
#undef CAL_SAD_W32_10BIT
#undef CAL_SAD_SUM_10BIT

void uavs3e_get_sad_x3_4_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height)
{
    __m256i sum0, sum1, sum2;
    __m256i zero = _mm256_setzero_si256();
    __m256i o, p0, p1, p2, t0, t1, t2;
    int i_org2 = i_org << 1;
    int i_org3 = i_org2 + i_org;
    int i_pred2 = i_pred << 1;
    int i_pred3 = i_pred2 + i_pred;

    sum0 = sum1 = sum2 = zero;
    height >>= 2;

    while (height--) {
        o = _mm256_set_epi64x(*(s64*)(p_org + i_org3), *(s64*)(p_org + i_org2), *(s64*)(p_org + i_org), *(s64*)p_org);
        p0 = _mm256_set_epi64x(*(s64*)(p_pred0 + i_pred3), *(s64*)(p_pred0 + i_pred2), *(s64*)(p_pred0 + i_pred), *(s64*)p_pred0);
        p1 = _mm256_set_epi64x(*(s64*)(p_pred1 + i_pred3), *(s64*)(p_pred1 + i_pred2), *(s64*)(p_pred1 + i_pred), *(s64*)p_pred1);
        p2 = _mm256_set_epi64x(*(s64*)(p_pred2 + i_pred3), *(s64*)(p_pred2 + i_pred2), *(s64*)(p_pred2 + i_pred), *(s64*)p_pred2);
        t0 = _mm256_sub_epi16(o, p0);
        t1 = _mm256_sub_epi16(o, p1);
        t2 = _mm256_sub_epi16(o, p2);
        t0 = _mm256_abs_epi16(t0);
        t1 = _mm256_abs_epi16(t1);
        t2 = _mm256_abs_epi16(t2);
        sum0 = _mm256_add_epi16(sum0, t0);
        sum1 = _mm256_add_epi16(sum1, t1);
        sum2 = _mm256_add_epi16(sum2, t2);
        p_org += i_org << 2;
        p_pred0 += i_pred << 2;
        p_pred1 += i_pred << 2;
        p_pred2 += i_pred << 2;
    }
    sum0 = _mm256_hadd_epi16(sum0, sum1);
    sum2 = _mm256_hadd_epi16(sum2, sum2);
    sum0 = _mm256_hadd_epi16(sum0, sum2);   //00 01 10 11 20 21 00 00 02 03 12 13 22 23 00 00

    t0 = _mm256_unpacklo_epi16(sum0, zero);
    t1 = _mm256_unpackhi_epi16(sum0, zero);

    sum0 = _mm256_hadd_epi32(t0, t1);       // 00 10 20 00 01 11 21 00

    sad[0] = ((u32)_mm256_extract_epi32(sum0, 0)) + ((u32)_mm256_extract_epi32(sum0, 4));
    sad[1] = ((u32)_mm256_extract_epi32(sum0, 1)) + ((u32)_mm256_extract_epi32(sum0, 5));
    sad[2] = ((u32)_mm256_extract_epi32(sum0, 2)) + ((u32)_mm256_extract_epi32(sum0, 6));
}

void uavs3e_get_sad_x3_8_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height)
{
    __m256i sum0, sum1, sum2;
    __m256i zero = _mm256_setzero_si256();
    __m256i o, p0, p1, p2, t0, t1, t2, t3;
    int i_org2 = i_org << 1;
    int i_pred2 = i_pred << 1;

    sum0 = sum1 = sum2 = zero;
    height >>= 1;

    while (height--) {
        o = _mm256_loadu2_m128i((__m128i*)(p_org + i_org), (__m128i*)p_org);
        p0 = _mm256_loadu2_m128i((__m128i*)(p_pred0 + i_pred), (__m128i*)p_pred0);
        p1 = _mm256_loadu2_m128i((__m128i*)(p_pred1 + i_pred), (__m128i*)(p_pred1));
        p2 = _mm256_loadu2_m128i((__m128i*)(p_pred2 + i_pred), (__m128i*)(p_pred2));
        t0 = _mm256_sub_epi16(o, p0);
        t1 = _mm256_sub_epi16(o, p1);
        t2 = _mm256_sub_epi16(o, p2);
        t0 = _mm256_abs_epi16(t0);
        t1 = _mm256_abs_epi16(t1);
        t2 = _mm256_abs_epi16(t2);
        sum0 = _mm256_add_epi16(sum0, t0);
        sum1 = _mm256_add_epi16(sum1, t1);
        sum2 = _mm256_add_epi16(sum2, t2);
        p_org += i_org2;
        p_pred0 += i_pred2;
        p_pred1 += i_pred2;
        p_pred2 += i_pred2;
    }
    t0 = _mm256_unpacklo_epi16(sum0, zero);
    t1 = _mm256_unpackhi_epi16(sum0, zero);
    t2 = _mm256_unpacklo_epi16(sum1, zero);
    t3 = _mm256_unpackhi_epi16(sum1, zero);
    sum0 = _mm256_add_epi32(t0, t1);
    sum1 = _mm256_add_epi32(t2, t3);
    t0 = _mm256_unpacklo_epi16(sum2, zero);
    t1 = _mm256_unpackhi_epi16(sum2, zero);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    sum1 = _mm256_hadd_epi32(sum1, sum1);
    sum2 = _mm256_add_epi32(t0, t1);
    sum0 = _mm256_hadd_epi32(sum0, sum0);
    sum1 = _mm256_hadd_epi32(sum1, sum1);
    sum2 = _mm256_hadd_epi32(sum2, sum2);
    sum2 = _mm256_hadd_epi32(sum2, sum2);
    sad[0] = ((u32)_mm256_extract_epi32(sum0, 0)) + ((u32)_mm256_extract_epi32(sum0, 4));
    sad[1] = ((u32)_mm256_extract_epi32(sum1, 0)) + ((u32)_mm256_extract_epi32(sum1, 4));
    sad[2] = ((u32)_mm256_extract_epi32(sum2, 0)) + ((u32)_mm256_extract_epi32(sum2, 4));
}

void uavs3e_get_sad_x3_16_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height)
{
    __m256i sum[6];
    __m256i zero = _mm256_setzero_si256();
    __m256i O[2], P[6], S[6];
    int i;
    int i_orgx2 = i_org << 1;
    int i_predx2 = i_pred << 1;
    for (i = 0; i < 6; i++) {
        sum[i] = zero;
    }
    height >>= 1;

    while (height--) {
        O[0] = _mm256_loadu_si256((__m256i *)p_org);
        O[1] = _mm256_loadu_si256((__m256i *)(p_org + i_org));
        P[0] = _mm256_loadu_si256((__m256i *)p_pred0);
        P[1] = _mm256_loadu_si256((__m256i *)(p_pred0 + i_pred));
        P[2] = _mm256_loadu_si256((__m256i *)p_pred1);
        P[3] = _mm256_loadu_si256((__m256i *)(p_pred1 + i_pred));
        P[4] = _mm256_loadu_si256((__m256i *)p_pred2);
        P[5] = _mm256_loadu_si256((__m256i *)(p_pred2 + i_pred));
        for (i = 0; i < 6; i += 2) {
            S[i+0] = _mm256_sub_epi16(O[0], P[i+0]);
            S[i+1] = _mm256_sub_epi16(O[1], P[i+1]);
            S[i+0] = _mm256_abs_epi16(S[i+0]);
            S[i+1] = _mm256_abs_epi16(S[i+1]);
            sum[i+0] = _mm256_add_epi16(sum[i+0], S[i+0]);
            sum[i+1] = _mm256_add_epi16(sum[i+1], S[i+1]);
        }
        p_org += i_orgx2;
        p_pred0 += i_predx2;
        p_pred1 += i_predx2;
        p_pred2 += i_predx2;
    }
    for (i = 0; i < 6; i += 2) {
        P[0] = _mm256_unpacklo_epi16(sum[i+0], zero);
        P[1] = _mm256_unpackhi_epi16(sum[i+0], zero);
        P[2] = _mm256_unpacklo_epi16(sum[i+1], zero);
        P[3] = _mm256_unpackhi_epi16(sum[i+1], zero);
        sum[i+0] = _mm256_add_epi32(P[0], P[1]);
        sum[i+1] = _mm256_add_epi32(P[2], P[3]);
        sum[i] = _mm256_add_epi32(sum[i], sum[i+1]);
    }
    sum[0] = _mm256_hadd_epi32(sum[0], sum[2]);
    sum[4] = _mm256_hadd_epi32(sum[4], sum[4]);
    sum[0] = _mm256_hadd_epi32(sum[0], sum[4]);

    sad[0] = ((u32)_mm256_extract_epi32(sum[0], 0)) + ((u32)_mm256_extract_epi32(sum[0], 4));
    sad[1] = ((u32)_mm256_extract_epi32(sum[0], 1)) + ((u32)_mm256_extract_epi32(sum[0], 5));
    sad[2] = ((u32)_mm256_extract_epi32(sum[0], 2)) + ((u32)_mm256_extract_epi32(sum[0], 6));
}

void uavs3e_get_sad_x3_32_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height)
{
    __m256i sum0, sum1, sum2;
    __m256i zero = _mm256_setzero_si256();
    __m256i o, p0, p1, p2, t0, t1, t2, t3, t4, t5;

    sum0 = sum1 = sum2 = zero;

    while (height--) {
        o  = _mm256_loadu_si256((__m256i *)p_org);
        p0 = _mm256_loadu_si256((__m256i *)p_pred0);
        p1 = _mm256_loadu_si256((__m256i *)p_pred1);
        p2 = _mm256_loadu_si256((__m256i *)p_pred2);
        t0 = _mm256_sub_epi16(o, p0);
        t1 = _mm256_sub_epi16(o, p1);
        t2 = _mm256_sub_epi16(o, p2);
        t0 = _mm256_abs_epi16(t0);
        t1 = _mm256_abs_epi16(t1);
        t2 = _mm256_abs_epi16(t2);

        o  = _mm256_loadu_si256((__m256i *)(p_org + 16));
        p0 = _mm256_loadu_si256((__m256i *)(p_pred0 + 16));
        p1 = _mm256_loadu_si256((__m256i *)(p_pred1 + 16));
        p2 = _mm256_loadu_si256((__m256i *)(p_pred2 + 16));
        t3 = _mm256_sub_epi16(o, p0);
        t4 = _mm256_sub_epi16(o, p1);
        t5 = _mm256_sub_epi16(o, p2);
        t3 = _mm256_abs_epi16(t3);
        t4 = _mm256_abs_epi16(t4);
        t5 = _mm256_abs_epi16(t5);

        t0 = _mm256_add_epi16(t0, t3);
        t1 = _mm256_add_epi16(t1, t4);
        t2 = _mm256_add_epi16(t2, t5);
        t3 = _mm256_unpacklo_epi16(t0, zero);
        t4 = _mm256_unpackhi_epi16(t0, zero);
        t5 = _mm256_unpacklo_epi16(t1, zero);
        t0 = _mm256_unpackhi_epi16(t1, zero);
        t1 = _mm256_unpacklo_epi16(t2, zero);
        t2 = _mm256_unpackhi_epi16(t2, zero);

        t3 = _mm256_add_epi32(t3, t4);
        t0 = _mm256_add_epi32(t5, t0);
        t1 = _mm256_add_epi32(t1, t2);

        sum0 = _mm256_add_epi32(sum0, t3);
        sum1 = _mm256_add_epi32(sum1, t0);
        sum2 = _mm256_add_epi32(sum2, t1);

        p_org += i_org;
        p_pred0 += i_pred;
        p_pred1 += i_pred;
        p_pred2 += i_pred;
    }
    sum0 = _mm256_hadd_epi32(sum0, sum1);
    sum2 = _mm256_hadd_epi32(sum2, sum2);
    sum0 = _mm256_hadd_epi32(sum0, sum2);

    sad[0] = ((u32)_mm256_extract_epi32(sum0, 0)) + ((u32)_mm256_extract_epi32(sum0, 4));
    sad[1] = ((u32)_mm256_extract_epi32(sum0, 1)) + ((u32)_mm256_extract_epi32(sum0, 5));
    sad[2] = ((u32)_mm256_extract_epi32(sum0, 2)) + ((u32)_mm256_extract_epi32(sum0, 6));
}

#define CAL_SUB_ABS_X3_10BIT(i, t0, t1, t2) \
        o = _mm256_loadu_si256((__m256i *)(p_org + 16*i));    \
        p0 = _mm256_loadu_si256((__m256i *)(p_pred0 + 16*i)); \
        p1 = _mm256_loadu_si256((__m256i *)(p_pred1 + 16*i)); \
        p2 = _mm256_loadu_si256((__m256i *)(p_pred2 + 16*i)); \
        t0 = _mm256_sub_epi16(o, p0);                       \
        t1 = _mm256_sub_epi16(o, p1);                       \
        t2 = _mm256_sub_epi16(o, p2);                       \
        t0 = _mm256_abs_epi16(t0);                          \
        t1 = _mm256_abs_epi16(t1);                          \
        t2 = _mm256_abs_epi16(t2) 

void uavs3e_get_sad_x3_64_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height)
{
    __m256i sum0, sum1, sum2;
    __m256i zero = _mm256_setzero_si256();
    __m256i o, p0, p1, p2, t0, t1, t2, t3, t4, t5;

    sum0 = sum1 = sum2 = zero;

    while (height--) {
        CAL_SUB_ABS_X3_10BIT(0, t0, t1, t2);
        CAL_SUB_ABS_X3_10BIT(1, t3, t4, t5);

        t0 = _mm256_add_epi16(t0, t3);
        t1 = _mm256_add_epi16(t1, t4);
        t2 = _mm256_add_epi16(t2, t5);

        CAL_SUB_ABS_X3_10BIT(2, t3, t4, t5);
        t0 = _mm256_add_epi16(t0, t3);
        t1 = _mm256_add_epi16(t1, t4);
        t2 = _mm256_add_epi16(t2, t5);

        CAL_SUB_ABS_X3_10BIT(3, t3, t4, t5);
        t0 = _mm256_add_epi16(t0, t3);
        t1 = _mm256_add_epi16(t1, t4);
        t2 = _mm256_add_epi16(t2, t5);

        t3 = _mm256_unpacklo_epi16(t0, zero);
        t4 = _mm256_unpackhi_epi16(t0, zero);
        t5 = _mm256_unpacklo_epi16(t1, zero);
        t0 = _mm256_unpackhi_epi16(t1, zero);
        t1 = _mm256_unpacklo_epi16(t2, zero);
        t2 = _mm256_unpackhi_epi16(t2, zero);

        t3 = _mm256_add_epi32(t3, t4);
        t0 = _mm256_add_epi32(t5, t0);
        t1 = _mm256_add_epi32(t1, t2);

        sum0 = _mm256_add_epi32(sum0, t3);
        sum1 = _mm256_add_epi32(sum1, t0);
        sum2 = _mm256_add_epi32(sum2, t1);

        p_org += i_org;
        p_pred0 += i_pred;
        p_pred1 += i_pred;
        p_pred2 += i_pred;
    }
    sum0 = _mm256_hadd_epi32(sum0, sum1);
    sum2 = _mm256_hadd_epi32(sum2, sum2);
    sum0 = _mm256_hadd_epi32(sum0, sum2);

    sad[0] = ((u32)_mm256_extract_epi32(sum0, 0)) + ((u32)_mm256_extract_epi32(sum0, 4));
    sad[1] = ((u32)_mm256_extract_epi32(sum0, 1)) + ((u32)_mm256_extract_epi32(sum0, 5));
    sad[2] = ((u32)_mm256_extract_epi32(sum0, 2)) + ((u32)_mm256_extract_epi32(sum0, 6));
}

void uavs3e_get_sad_x3_128_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height)
{
    __m256i sum0, sum1, sum2;
    __m256i zero = _mm256_setzero_si256();
    __m256i o, p0, p1, p2, t0, t1, t2, t3, t4, t5;
    int i;

    sum0 = sum1 = sum2 = zero;

    while (height--) {
        CAL_SUB_ABS_X3_10BIT(0, t0, t1, t2);

        for (i = 1; i < 8; i++) {
            CAL_SUB_ABS_X3_10BIT(i, t3, t4, t5);

            t0 = _mm256_add_epi16(t0, t3);
            t1 = _mm256_add_epi16(t1, t4);
            t2 = _mm256_add_epi16(t2, t5);
        }

        t3 = _mm256_unpacklo_epi16(t0, zero);
        t4 = _mm256_unpackhi_epi16(t0, zero);
        t5 = _mm256_unpacklo_epi16(t1, zero);
        t0 = _mm256_unpackhi_epi16(t1, zero);
        t1 = _mm256_unpacklo_epi16(t2, zero);
        t2 = _mm256_unpackhi_epi16(t2, zero);

        t3 = _mm256_add_epi32(t3, t4);
        t0 = _mm256_add_epi32(t5, t0);
        t1 = _mm256_add_epi32(t1, t2);

        sum0 = _mm256_add_epi32(sum0, t3);
        sum1 = _mm256_add_epi32(sum1, t0);
        sum2 = _mm256_add_epi32(sum2, t1);

        p_org += i_org;
        p_pred0 += i_pred;
        p_pred1 += i_pred;
        p_pred2 += i_pred;
    }
    sum0 = _mm256_hadd_epi32(sum0, sum1);
    sum2 = _mm256_hadd_epi32(sum2, sum2);
    sum0 = _mm256_hadd_epi32(sum0, sum2);

    sad[0] = ((u32)_mm256_extract_epi32(sum0, 0)) + ((u32)_mm256_extract_epi32(sum0, 4));
    sad[1] = ((u32)_mm256_extract_epi32(sum0, 1)) + ((u32)_mm256_extract_epi32(sum0, 5));
    sad[2] = ((u32)_mm256_extract_epi32(sum0, 2)) + ((u32)_mm256_extract_epi32(sum0, 6));
}
#undef CAL_SUB_ABS_X3_10BIT

void uavs3e_get_sad_x4_4_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[4], int height)
{
    __m256i sum0, sum1, sum2, sum3;
    __m256i zero = _mm256_setzero_si256();
    __m256i o, p0, p1, p2, p3, t0, t1, t2, t3;
    __m128i m0;
    int i_org2 = i_org << 1;
    int i_org3 = i_org2 + i_org;
    int i_pred2 = i_pred << 1;
    int i_pred3 = i_pred2 + i_pred;
    int i_pred4 = i_pred << 2;

    sum0 = sum1 = sum2 = sum3 = zero;
    height >>= 2;

    while (height--) {
        o = _mm256_set_epi64x(*(s64*)(p_org + i_org3), *(s64*)(p_org + i_org2), *(s64*)(p_org + i_org), *(s64*)p_org);
        p0 = _mm256_set_epi64x(*(s64*)(p_pred0 + i_pred3), *(s64*)(p_pred0 + i_pred2), *(s64*)(p_pred0 + i_pred), *(s64*)p_pred0);
        p1 = _mm256_set_epi64x(*(s64*)(p_pred1 + i_pred3), *(s64*)(p_pred1 + i_pred2), *(s64*)(p_pred1 + i_pred), *(s64*)p_pred1);
        p2 = _mm256_set_epi64x(*(s64*)(p_pred2 + i_pred3), *(s64*)(p_pred2 + i_pred2), *(s64*)(p_pred2 + i_pred), *(s64*)p_pred2);
        p3 = _mm256_set_epi64x(*(s64*)(p_pred3 + i_pred3), *(s64*)(p_pred3 + i_pred2), *(s64*)(p_pred3 + i_pred), *(s64*)p_pred3);
        t0 = _mm256_sub_epi16(o, p0);
        t1 = _mm256_sub_epi16(o, p1);
        t2 = _mm256_sub_epi16(o, p2);
        t3 = _mm256_sub_epi16(o, p3);
        t0 = _mm256_abs_epi16(t0);
        t1 = _mm256_abs_epi16(t1);
        t2 = _mm256_abs_epi16(t2);
        t3 = _mm256_abs_epi16(t3);
        sum0 = _mm256_add_epi16(sum0, t0);
        sum1 = _mm256_add_epi16(sum1, t1);
        sum2 = _mm256_add_epi16(sum2, t2);
        sum3 = _mm256_add_epi16(sum3, t3);
        p_org += i_org << 2;
        p_pred0 += i_pred4;
        p_pred1 += i_pred4;
        p_pred2 += i_pred4;
        p_pred3 += i_pred4;
    }
    sum0 = _mm256_hadd_epi16(sum0, sum1);
    sum2 = _mm256_hadd_epi16(sum2, sum3);
    sum0 = _mm256_hadd_epi16(sum0, sum2);   //00 01 10 11 20 21 30 31 02 03 12 13 22 23 32 33

    t0 = _mm256_unpacklo_epi16(sum0, zero);
    t1 = _mm256_unpackhi_epi16(sum0, zero);

    sum0 = _mm256_hadd_epi32(t0, t1);       // 00 10 20 30 01 11 21 31

    m0 = _mm_add_epi32(_mm256_castsi256_si128(sum0), _mm256_extracti128_si256(sum0, 1));
    _mm_storeu_si128((__m128i*)sad, m0);
}

void uavs3e_get_sad_x4_8_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[4], int height)
{
    __m256i sum0, sum1, sum2, sum3;
    __m256i zero = _mm256_setzero_si256();
    __m256i o, p0, p1, p2, p3, t0, t1, t2, t3;
    __m128i m0;
    int i_org2 = i_org << 1;
    int i_pred2 = i_pred << 1;

    sum0 = sum1 = sum2 = sum3 = zero;
    height >>= 1;

    while (height--) {
        o = _mm256_loadu2_m128i((__m128i*)(p_org + i_org), (__m128i*)p_org);
        p0 = _mm256_loadu2_m128i((__m128i*)(p_pred0 + i_pred), (__m128i*)p_pred0);
        p1 = _mm256_loadu2_m128i((__m128i*)(p_pred1 + i_pred), (__m128i*)(p_pred1));
        p2 = _mm256_loadu2_m128i((__m128i*)(p_pred2 + i_pred), (__m128i*)(p_pred2));
        p3 = _mm256_loadu2_m128i((__m128i*)(p_pred3 + i_pred), (__m128i*)(p_pred3));
        t0 = _mm256_sub_epi16(o, p0);
        t1 = _mm256_sub_epi16(o, p1);
        t2 = _mm256_sub_epi16(o, p2);
        t3 = _mm256_sub_epi16(o, p3);
        t0 = _mm256_abs_epi16(t0);
        t1 = _mm256_abs_epi16(t1);
        t2 = _mm256_abs_epi16(t2);
        t3 = _mm256_abs_epi16(t3);
        sum0 = _mm256_add_epi16(sum0, t0);
        sum1 = _mm256_add_epi16(sum1, t1);
        sum2 = _mm256_add_epi16(sum2, t2);
        sum3 = _mm256_add_epi16(sum3, t3);
        p_org += i_org2;
        p_pred0 += i_pred2;
        p_pred1 += i_pred2;
        p_pred2 += i_pred2;
        p_pred3 += i_pred2;
    }
    sum0 = _mm256_hadd_epi16(sum0, sum1);
    sum2 = _mm256_hadd_epi16(sum2, sum3);
    t0 = _mm256_unpacklo_epi16(sum0, zero);
    t1 = _mm256_unpackhi_epi16(sum0, zero);
    t2 = _mm256_unpacklo_epi16(sum2, zero);
    t3 = _mm256_unpackhi_epi16(sum2, zero);
    sum0 = _mm256_hadd_epi32(t0, t1);
    sum1 = _mm256_hadd_epi32(t2, t3);
    sum0 = _mm256_hadd_epi32(sum0, sum1);
    m0 = _mm_add_epi32(_mm256_castsi256_si128(sum0), _mm256_extracti128_si256(sum0, 1));
    _mm_storeu_si128((__m128i*)sad, m0);
}

void uavs3e_get_sad_x4_16_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[4], int height)
{
    __m256i sum[8];
    __m256i zero = _mm256_setzero_si256();
    __m256i O[2], P[8], S[8];
    __m128i m0;
    int i;
    int i_orgx2 = i_org << 1;
    int i_predx2 = i_pred << 1;
    for (i = 0; i < 8; i++) {
        sum[i] = zero;
    }
    height >>= 1;

    while (height--) {
        O[0] = _mm256_loadu_si256((__m256i *)p_org);
        O[1] = _mm256_loadu_si256((__m256i *)(p_org + i_org));
        P[0] = _mm256_loadu_si256((__m256i *)p_pred0);
        P[1] = _mm256_loadu_si256((__m256i *)(p_pred0 + i_pred));
        P[2] = _mm256_loadu_si256((__m256i *)p_pred1);
        P[3] = _mm256_loadu_si256((__m256i *)(p_pred1 + i_pred));
        P[4] = _mm256_loadu_si256((__m256i *)p_pred2);
        P[5] = _mm256_loadu_si256((__m256i *)(p_pred2 + i_pred));
        P[6] = _mm256_loadu_si256((__m256i *)p_pred3);
        P[7] = _mm256_loadu_si256((__m256i *)(p_pred3 + i_pred));
        for (i = 0; i < 8; i += 2) {
            S[i + 0] = _mm256_sub_epi16(O[0], P[i + 0]);
            S[i + 1] = _mm256_sub_epi16(O[1], P[i + 1]);
            S[i + 0] = _mm256_abs_epi16(S[i + 0]);
            S[i + 1] = _mm256_abs_epi16(S[i + 1]);
            sum[i + 0] = _mm256_add_epi16(sum[i + 0], S[i + 0]);
            sum[i + 1] = _mm256_add_epi16(sum[i + 1], S[i + 1]);
        }
        p_org += i_orgx2;
        p_pred0 += i_predx2;
        p_pred1 += i_predx2;
        p_pred2 += i_predx2;
        p_pred3 += i_predx2;
    }
    for (i = 0; i < 8; i += 2) {
        P[0] = _mm256_unpacklo_epi16(sum[i + 0], zero);
        P[1] = _mm256_unpackhi_epi16(sum[i + 0], zero);
        P[2] = _mm256_unpacklo_epi16(sum[i + 1], zero);
        P[3] = _mm256_unpackhi_epi16(sum[i + 1], zero);
        sum[i + 0] = _mm256_add_epi32(P[0], P[1]);
        sum[i + 1] = _mm256_add_epi32(P[2], P[3]);
        sum[i] = _mm256_add_epi32(sum[i], sum[i + 1]);
    }
    sum[0] = _mm256_hadd_epi32(sum[0], sum[2]);
    sum[4] = _mm256_hadd_epi32(sum[4], sum[6]);
    sum[0] = _mm256_hadd_epi32(sum[0], sum[4]);

    m0 = _mm_add_epi32(_mm256_castsi256_si128(sum[0]), _mm256_extracti128_si256(sum[0], 1));
    _mm_storeu_si128((__m128i*)sad, m0);
}

void uavs3e_get_sad_x4_32_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[4], int height)
{
    __m256i sum0, sum1;
    __m256i zero = _mm256_setzero_si256();
    __m256i o0, o1, p0, p1, p2, p3, t0, t1, t2, t3;
    __m128i m0;

    sum0 = sum1 = zero;

    while (height--) {
        o0 = _mm256_loadu2_m128i((__m128i*)(p_org), (__m128i*)p_org);
        o1 = _mm256_loadu2_m128i((__m128i*)(p_org + 8), (__m128i*)(p_org + 8));
        p0 = _mm256_loadu2_m128i((__m128i *)p_pred1, (__m128i *)p_pred0);
        p1 = _mm256_loadu2_m128i((__m128i *)p_pred3, (__m128i *)p_pred2);
        p2 = _mm256_loadu2_m128i((__m128i *)(p_pred1 + 8), (__m128i *)(p_pred0 + 8));
        p3 = _mm256_loadu2_m128i((__m128i *)(p_pred3 + 8), (__m128i *)(p_pred2 + 8));
        t0 = _mm256_sub_epi16(o0, p0);
        t1 = _mm256_sub_epi16(o0, p1);
        t0 = _mm256_abs_epi16(t0);
        t1 = _mm256_abs_epi16(t1);

        t2 = _mm256_sub_epi16(o1, p2);
        t3 = _mm256_sub_epi16(o1, p3);
        t2 = _mm256_abs_epi16(t2);
        t3 = _mm256_abs_epi16(t3);

        o0 = _mm256_loadu2_m128i((__m128i*)(p_org + 16), (__m128i*)(p_org + 16));
        o1 = _mm256_loadu2_m128i((__m128i*)(p_org + 24), (__m128i*)(p_org + 24));
        p0 = _mm256_loadu2_m128i((__m128i *)(p_pred1 + 16), (__m128i *)(p_pred0 + 16));
        p1 = _mm256_loadu2_m128i((__m128i *)(p_pred3 + 16), (__m128i *)(p_pred2 + 16));
        p2 = _mm256_loadu2_m128i((__m128i *)(p_pred1 + 24), (__m128i *)(p_pred0 + 24));
        p3 = _mm256_loadu2_m128i((__m128i *)(p_pred3 + 24), (__m128i *)(p_pred2 + 24));
        t0 = _mm256_add_epi16(t0, t2);
        t1 = _mm256_add_epi16(t1, t3);

        t2 = _mm256_sub_epi16(o0, p0);
        t3 = _mm256_sub_epi16(o0, p1);
        t2 = _mm256_abs_epi16(t2);
        t3 = _mm256_abs_epi16(t3);
        t0 = _mm256_add_epi16(t0, t2);
        t1 = _mm256_add_epi16(t1, t3);

        t2 = _mm256_sub_epi16(o1, p2);
        t3 = _mm256_sub_epi16(o1, p3);
        t2 = _mm256_abs_epi16(t2);
        t3 = _mm256_abs_epi16(t3);
        t0 = _mm256_add_epi16(t0, t2);
        t1 = _mm256_add_epi16(t1, t3);

        t2 = _mm256_unpacklo_epi16(t0, zero);
        t3 = _mm256_unpackhi_epi16(t0, zero);
        t0 = _mm256_unpacklo_epi16(t1, zero);
        t1 = _mm256_unpackhi_epi16(t1, zero);

        t2 = _mm256_add_epi32(t2, t3);
        t0 = _mm256_add_epi32(t0, t1);

        sum0 = _mm256_add_epi32(sum0, t2);
        sum1 = _mm256_add_epi32(sum1, t0);

        p_org += i_org;
        p_pred0 += i_pred;
        p_pred1 += i_pred;
        p_pred2 += i_pred;
        p_pred3 += i_pred;
    }
    sum0 = _mm256_permute4x64_epi64(sum0, 0xd8);
    sum1 = _mm256_permute4x64_epi64(sum1, 0xd8);
    sum0 = _mm256_hadd_epi32(sum0, sum1);
    m0 = _mm_add_epi32(_mm256_castsi256_si128(sum0), _mm256_extracti128_si256(sum0, 1));
    _mm_storeu_si128((__m128i*)sad, m0);
}

#define CAL_SUB_ABS_X4_10BIT(i, t0, t1) \
        o = _mm256_loadu2_m128i((__m128i*)(p_org + 8*i), (__m128i*)(p_org + 8*i));    \
        p0 = _mm256_loadu2_m128i((__m128i *)(p_pred1 + 8*i), (__m128i *)(p_pred0 + 8*i)); \
        p1 = _mm256_loadu2_m128i((__m128i *)(p_pred3 + 8*i), (__m128i *)(p_pred2 + 8*i)); \
        t0 = _mm256_sub_epi16(o, p0);                       \
        t1 = _mm256_sub_epi16(o, p1);                       \
        t0 = _mm256_abs_epi16(t0);                          \
        t1 = _mm256_abs_epi16(t1) 

void uavs3e_get_sad_x4_64_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[4], int height)
{
    __m256i sum0, sum1;
    __m256i zero = _mm256_setzero_si256();
    __m256i o, p0, p1, t0, t1, t2, t3;
    __m128i m0;
    int i;

    sum0 = sum1 = zero;

    while (height--) {
        CAL_SUB_ABS_X4_10BIT(0, t0, t1);
        for (i = 1; i < 8; i++) {
            CAL_SUB_ABS_X4_10BIT(i, t2, t3);

            t0 = _mm256_add_epi16(t0, t2);
            t1 = _mm256_add_epi16(t1, t3);
        }

        t2 = _mm256_unpacklo_epi16(t0, zero);
        t3 = _mm256_unpackhi_epi16(t0, zero);
        t0 = _mm256_unpacklo_epi16(t1, zero);
        t1 = _mm256_unpackhi_epi16(t1, zero);

        t2 = _mm256_add_epi32(t2, t3);
        t0 = _mm256_add_epi32(t0, t1);

        sum0 = _mm256_add_epi32(sum0, t2);
        sum1 = _mm256_add_epi32(sum1, t0);

        p_org += i_org;
        p_pred0 += i_pred;
        p_pred1 += i_pred;
        p_pred2 += i_pred;
        p_pred3 += i_pred;
    }
    sum0 = _mm256_permute4x64_epi64(sum0, 0xd8);
    sum1 = _mm256_permute4x64_epi64(sum1, 0xd8);
    sum0 = _mm256_hadd_epi32(sum0, sum1);
    m0 = _mm_add_epi32(_mm256_castsi256_si128(sum0), _mm256_extracti128_si256(sum0, 1));
    _mm_storeu_si128((__m128i*)sad, m0);
}

void uavs3e_get_sad_x4_128_avx2(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, pel *p_pred3, int i_pred, u32 sad[4], int height)
{
    __m256i sum0, sum1;
    __m256i zero = _mm256_setzero_si256();
    __m256i o, p0, p1, t0, t1, t2, t3;
    __m128i m0;
    int i;

    sum0 = sum1 = zero;

    while (height--) {
        CAL_SUB_ABS_X4_10BIT(0, t0, t1);

        for (i = 1; i < 16; i++) {
            CAL_SUB_ABS_X4_10BIT(i, t2, t3);

            t0 = _mm256_add_epi16(t0, t2);
            t1 = _mm256_add_epi16(t1, t3);
        }

        t2 = _mm256_unpacklo_epi16(t0, zero);
        t3 = _mm256_unpackhi_epi16(t0, zero);
        t0 = _mm256_unpacklo_epi16(t1, zero);
        t1 = _mm256_unpackhi_epi16(t1, zero);

        t2 = _mm256_add_epi32(t2, t3);
        t0 = _mm256_add_epi32(t0, t1);

        sum0 = _mm256_add_epi32(sum0, t2);
        sum1 = _mm256_add_epi32(sum1, t0);

        p_org += i_org;
        p_pred0 += i_pred;
        p_pred1 += i_pred;
        p_pred2 += i_pred;
        p_pred3 += i_pred;
    }
    sum0 = _mm256_permute4x64_epi64(sum0, 0xd8);
    sum1 = _mm256_permute4x64_epi64(sum1, 0xd8);
    sum0 = _mm256_hadd_epi32(sum0, sum1);
    m0 = _mm_add_epi32(_mm256_castsi256_si128(sum0), _mm256_extracti128_si256(sum0, 1));
    _mm_storeu_si128((__m128i*)sad, m0);
}
#undef CAL_SUB_ABS_X4_10BIT

u64 uavs3e_get_ssd_4_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m256i sum = _mm256_setzero_si256();
    int i_orgx2 = i_org << 1;
    int i_orgx3 = i_orgx2 + i_org;
    int i_predx2 = i_pred << 1;
    int i_predx3 = i_predx2 + i_pred;

    height >>= 2;
    while (height--) {
        __m256i A = _mm256_set_epi64x(*(s64*)(p_org), *(s64*)(p_org + i_org), *(s64*)(p_org + i_orgx2), *(s64*)(p_org + i_orgx3));
        __m256i B = _mm256_set_epi64x(*(s64*)(p_pred), *(s64*)(p_pred + i_pred), *(s64*)(p_pred + i_predx2), *(s64*)(p_pred + i_predx3));
        __m256i C = _mm256_sub_epi16(A, B);
        __m256i D = _mm256_madd_epi16(C, C);
        sum = _mm256_add_epi32(sum, D);

        p_org += i_org << 2;
        p_pred += i_pred << 2;
    }
    sum = _mm256_hadd_epi32(sum, sum);
    sum = _mm256_hadd_epi32(sum, sum);
    return ((u32)_mm256_extract_epi32(sum, 0)) + ((u32)_mm256_extract_epi32(sum, 4));
}

u64 uavs3e_get_ssd_8_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m256i sum = _mm256_setzero_si256();
    __m256i o0, o1, p0, p1, t0, t1;
    int i_org2 = i_org << 1;
    int i_org3 = i_org2 + i_org;
    int i_pred2 = i_pred << 1;
    int i_pred3 = i_pred2 + i_pred;

    height >>= 2;
    while (height--) {
        o0 = _mm256_loadu2_m128i((__m128i*)(p_org + i_org), (__m128i*)p_org);
        o1 = _mm256_loadu2_m128i((__m128i*)(p_org + i_org3), (__m128i*)(p_org + i_org2));
        p0 = _mm256_loadu2_m128i((__m128i*)(p_pred + i_pred), (__m128i*)p_pred);
        p1 = _mm256_loadu2_m128i((__m128i*)(p_pred + i_pred3), (__m128i*)(p_pred + i_pred2));
        t0 = _mm256_sub_epi16(o0, p0);
        t1 = _mm256_sub_epi16(o1, p1);
        t0 = _mm256_madd_epi16(t0, t0);
        t1 = _mm256_madd_epi16(t1, t1);
        sum = _mm256_add_epi32(sum, t0);
        sum = _mm256_add_epi32(sum, t1);

        p_org += i_org << 2;
        p_pred += i_pred << 2;
    }
    sum = _mm256_hadd_epi32(sum, sum);
    sum = _mm256_hadd_epi32(sum, sum);
    return ((u32)_mm256_extract_epi32(sum, 0)) + ((u32)_mm256_extract_epi32(sum, 4));
}

u64 uavs3e_get_ssd_16_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m256i sum = _mm256_setzero_si256();
    __m256i o0, o1, p0, p1, t0, t1;

    height >>= 1;
    while (height--) {
        o0 = _mm256_loadu_si256((__m256i*)p_org);
        o1 = _mm256_loadu_si256((__m256i*)(p_org + i_org));
        p0 = _mm256_loadu_si256((__m256i*)p_pred);
        p1 = _mm256_loadu_si256((__m256i*)(p_pred + i_pred));

        t0 = _mm256_sub_epi16(o0, p0);
        t1 = _mm256_sub_epi16(o1, p1);
        t0 = _mm256_madd_epi16(t0, t0);
        t1 = _mm256_madd_epi16(t1, t1);
        sum = _mm256_add_epi32(sum, t0);
        sum = _mm256_add_epi32(sum, t1);

        p_org += i_org << 1;
        p_pred += i_pred << 1;
    }
    sum = _mm256_hadd_epi32(sum, sum);
    sum = _mm256_hadd_epi32(sum, sum);
    return ((u32)_mm256_extract_epi32(sum, 0)) + ((u32)_mm256_extract_epi32(sum, 4));
}

u64 uavs3e_get_ssd_32_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m256i zero = _mm256_setzero_si256();
    __m256i sum = zero;
    __m256i o0, o1, o2, o3, p0, p1, p2, p3;
    __m256i t0, t1, t2, t3;

    height >>= 1;
    while (height--) {
        o0 = _mm256_loadu_si256((__m256i*)p_org);
        o1 = _mm256_loadu_si256((__m256i*)(p_org + 16));
        o2 = _mm256_loadu_si256((__m256i*)(p_org + i_org));
        o3 = _mm256_loadu_si256((__m256i*)(p_org + i_org + 16));
        p0 = _mm256_loadu_si256((__m256i*)p_pred);
        p1 = _mm256_loadu_si256((__m256i*)(p_pred + 16));
        p2 = _mm256_loadu_si256((__m256i*)(p_pred + i_pred));
        p3 = _mm256_loadu_si256((__m256i*)(p_pred + i_pred + 16));

        t0 = _mm256_sub_epi16(o0, p0);
        t1 = _mm256_sub_epi16(o1, p1);
        t2 = _mm256_sub_epi16(o2, p2);
        t3 = _mm256_sub_epi16(o3, p3);
        t0 = _mm256_madd_epi16(t0, t0);
        t1 = _mm256_madd_epi16(t1, t1);
        t2 = _mm256_madd_epi16(t2, t2);
        t3 = _mm256_madd_epi16(t3, t3);

        t0 = _mm256_add_epi32(t0, t1);
        t2 = _mm256_add_epi32(t2, t3);

        sum = _mm256_add_epi32(sum, t0);
        sum = _mm256_add_epi32(sum, t2);

        p_org += i_org << 1;
        p_pred += i_pred << 1;
    }

    t0 = _mm256_unpacklo_epi32(sum, zero);
    t1 = _mm256_unpackhi_epi32(sum, zero);
    sum = _mm256_add_epi64(t0, t1);
    return ((u64)_mm256_extract_epi64(sum, 0)) + ((u64)_mm256_extract_epi64(sum, 1)) + ((u64)_mm256_extract_epi64(sum, 2)) + ((u64)_mm256_extract_epi64(sum, 3));
}

u64 uavs3e_get_ssd_64_avx2(pel* p_org, int i_org, pel* p_pred, int i_pred, int height)
{
    __m256i zero = _mm256_setzero_si256();
    __m256i sum = zero;
    __m256i o0, o1, o2, o3, p0, p1, p2, p3;
    __m256i t0, t1, t2, t3;

    while (height--) {
        o0 = _mm256_loadu_si256((__m256i*)p_org);
        o1 = _mm256_loadu_si256((__m256i*)(p_org + 16));
        o2 = _mm256_loadu_si256((__m256i*)(p_org + 32));
        o3 = _mm256_loadu_si256((__m256i*)(p_org + 48));
        p0 = _mm256_loadu_si256((__m256i*)p_pred);
        p1 = _mm256_loadu_si256((__m256i*)(p_pred + 16));
        p2 = _mm256_loadu_si256((__m256i*)(p_pred + 32));
        p3 = _mm256_loadu_si256((__m256i*)(p_pred + 48));

        t0 = _mm256_sub_epi16(o0, p0);
        t1 = _mm256_sub_epi16(o1, p1);
        t2 = _mm256_sub_epi16(o2, p2);
        t3 = _mm256_sub_epi16(o3, p3);
        t0 = _mm256_madd_epi16(t0, t0);
        t1 = _mm256_madd_epi16(t1, t1);
        t2 = _mm256_madd_epi16(t2, t2);
        t3 = _mm256_madd_epi16(t3, t3);

        t0 = _mm256_add_epi32(t0, t1);
        t2 = _mm256_add_epi32(t2, t3);

        p_org += i_org;
        p_pred += i_pred;

        sum = _mm256_add_epi32(sum, t0);
        sum = _mm256_add_epi32(sum, t2);
    }

    t0 = _mm256_unpacklo_epi32(sum, zero);
    t1 = _mm256_unpackhi_epi32(sum, zero);
    sum = _mm256_add_epi64(t0, t1);
    return ((u64)_mm256_extract_epi64(sum, 0)) + ((u64)_mm256_extract_epi64(sum, 1)) + ((u64)_mm256_extract_epi64(sum, 2)) + ((u64)_mm256_extract_epi64(sum, 3));
}

u64 uavs3e_get_ssd_128_avx2(pel* p_org, int i_org, pel* p_pred, int i_pred, int height)
{
    __m256i zero = _mm256_setzero_si256();
    __m256i sum = zero;
    __m256i o0, o1, o2, o3, p0, p1, p2, p3;
    __m256i t0, t1, t2, t3;

    while (height--) {
        o0 = _mm256_loadu_si256((__m256i*)p_org);
        o1 = _mm256_loadu_si256((__m256i*)(p_org + 16));
        o2 = _mm256_loadu_si256((__m256i*)(p_org + 32));
        o3 = _mm256_loadu_si256((__m256i*)(p_org + 48));
        p0 = _mm256_loadu_si256((__m256i*)p_pred);
        p1 = _mm256_loadu_si256((__m256i*)(p_pred + 16));
        p2 = _mm256_loadu_si256((__m256i*)(p_pred + 32));
        p3 = _mm256_loadu_si256((__m256i*)(p_pred + 48));

        t0 = _mm256_sub_epi16(o0, p0);
        t1 = _mm256_sub_epi16(o1, p1);
        t2 = _mm256_sub_epi16(o2, p2);
        t3 = _mm256_sub_epi16(o3, p3);

        o0 = _mm256_loadu_si256((__m256i*)(p_org + 64));
        o1 = _mm256_loadu_si256((__m256i*)(p_org + 80));
        o2 = _mm256_loadu_si256((__m256i*)(p_org + 96));
        o3 = _mm256_loadu_si256((__m256i*)(p_org + 112));
        p0 = _mm256_loadu_si256((__m256i*)(p_pred + 64));
        p1 = _mm256_loadu_si256((__m256i*)(p_pred + 80));
        p2 = _mm256_loadu_si256((__m256i*)(p_pred + 96));
        p3 = _mm256_loadu_si256((__m256i*)(p_pred + 112));

        t0 = _mm256_madd_epi16(t0, t0);
        t1 = _mm256_madd_epi16(t1, t1);
        t2 = _mm256_madd_epi16(t2, t2);
        t3 = _mm256_madd_epi16(t3, t3);

        t0 = _mm256_add_epi32(t0, t1);
        t2 = _mm256_add_epi32(t2, t3);

        sum = _mm256_add_epi32(sum, t0);
        sum = _mm256_add_epi32(sum, t2);

        t0 = _mm256_sub_epi16(o0, p0);
        t1 = _mm256_sub_epi16(o1, p1);
        t2 = _mm256_sub_epi16(o2, p2);
        t3 = _mm256_sub_epi16(o3, p3);

        t0 = _mm256_madd_epi16(t0, t0);
        t1 = _mm256_madd_epi16(t1, t1);
        t2 = _mm256_madd_epi16(t2, t2);
        t3 = _mm256_madd_epi16(t3, t3);

        t0 = _mm256_add_epi32(t0, t1);
        t2 = _mm256_add_epi32(t2, t3);

        sum = _mm256_add_epi32(sum, t0);
        sum = _mm256_add_epi32(sum, t2);

        p_org += i_org;
        p_pred += i_pred;
    }
    t0 = _mm256_unpacklo_epi32(sum, zero);
    t1 = _mm256_unpackhi_epi32(sum, zero);
    sum = _mm256_add_epi64(t0, t1);
    return ((u64)_mm256_extract_epi64(sum, 0)) + ((u64)_mm256_extract_epi64(sum, 1)) + ((u64)_mm256_extract_epi64(sum, 2)) + ((u64)_mm256_extract_epi64(sum, 3));
}


u32 uavs3e_had_4x8_avx2(pel *org, int s_org, pel *cur, int s_cur)
{
    int k;
    __m256i m1[4], m2[8];
    __m256i sum;
    int satd = 0;

    for (k = 0; k < 4; k++) {
        __m256i r0 = _mm256_set_m128i(_mm_loadl_epi64((const __m128i*)(org + s_org)), _mm_loadl_epi64((const __m128i*)(org)));
        __m256i r1 = _mm256_set_m128i(_mm_loadl_epi64((const __m128i*)(cur + s_cur)), _mm_loadl_epi64((const __m128i*)(cur)));
        m2[k] = _mm256_sub_epi16(r0, r1);
        org += s_org << 1;
        cur += s_cur << 1;
    }
    // vertical
    m1[0] = _mm256_add_epi16(m2[0], m2[2]);
    m1[1] = _mm256_add_epi16(m2[1], m2[3]);
    m1[2] = _mm256_sub_epi16(m2[0], m2[2]);
    m1[3] = _mm256_sub_epi16(m2[1], m2[3]);
    m2[0] = _mm256_add_epi16(m1[0], m1[1]);
    m2[1] = _mm256_sub_epi16(m1[0], m1[1]);
    m2[2] = _mm256_add_epi16(m1[2], m1[3]);
    m2[3] = _mm256_sub_epi16(m1[2], m1[3]);
    m2[4] = _mm256_permute2x128_si256(m2[0], m2[1], 0x02);
    m2[5] = _mm256_permute2x128_si256(m2[0], m2[1], 0x13);
    m2[6] = _mm256_permute2x128_si256(m2[2], m2[3], 0x02);
    m2[7] = _mm256_permute2x128_si256(m2[2], m2[3], 0x13);
    m1[0] = _mm256_add_epi16(m2[4], m2[5]);     // 02
    m1[1] = _mm256_sub_epi16(m2[4], m2[5]);     // 13
    m1[2] = _mm256_add_epi16(m2[6], m2[7]);
    m1[3] = _mm256_sub_epi16(m2[6], m2[7]);

    // horizontal
    // transpose
    m2[0] = _mm256_unpacklo_epi16(m1[0], m1[1]);
    m2[1] = _mm256_unpacklo_epi16(m1[2], m1[3]);
    m2[2] = _mm256_permute2x128_si256(m2[0], m2[1], 0x02);
    m2[3] = _mm256_permute2x128_si256(m2[0], m2[1], 0x13);
    m1[0] = _mm256_unpacklo_epi32(m2[2], m2[3]);
    m1[1] = _mm256_unpackhi_epi32(m2[2], m2[3]);
    m1[2] = _mm256_permute2x128_si256(m1[0], m1[1], 0x02);
    m1[3] = _mm256_permute2x128_si256(m1[0], m1[1], 0x13);
    m2[0] = _mm256_unpacklo_epi64(m1[2], m1[3]);
    m2[1] = _mm256_unpackhi_epi64(m1[2], m1[3]);
    m2[2] = _mm256_permute2x128_si256(m2[0], m2[1], 0x02);
    m2[3] = _mm256_permute2x128_si256(m2[0], m2[1], 0x13);

    m1[0] = _mm256_add_epi16(m2[2], m2[3]);
    m1[1] = _mm256_sub_epi16(m2[2], m2[3]);
    m1[2] = _mm256_permute2x128_si256(m1[0], m1[1], 0x02);
    m1[3] = _mm256_permute2x128_si256(m1[0], m1[1], 0x13);

    m2[0] = _mm256_add_epi16(m1[2], m1[3]);
    m2[1] = _mm256_sub_epi16(m1[2], m1[3]);
    m2[0] = _mm256_abs_epi16(m2[0]);
    m2[1] = _mm256_abs_epi16(m2[1]);

    {
        __m256i C = _mm256_set1_epi16(1);
        m2[0] = _mm256_madd_epi16(m2[0], C);
        m2[1] = _mm256_madd_epi16(m2[1], C);
        sum = _mm256_add_epi32(m2[0], m2[1]);
        sum = _mm256_hadd_epi32(sum, sum);
        sum = _mm256_hadd_epi32(sum, sum);
    }

    satd = (int)((_mm256_extract_epi32(sum, 0) + _mm256_extract_epi32(sum, 4)) / com_tbl_sqrt[0] * 2);
    return satd;
}

u32 uavs3e_had_8x4_avx2(pel *p_org, int i_org, pel *p_pred, int i_pred)
{
    __m256i T01, T23;
    __m256i M01, M23;
    __m256i N01, N23;
    __m256i sign = _mm256_set_epi16(1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1);

#define HOR_LINE256(i, j) { \
        __m256i T0, T1, T2, T3, T4, T5, T6; \
        T0 = _mm256_loadu2_m128i((__m128i*)(p_org + i_org), (__m128i*)p_org); p_org  += (i_org  << 1);\
        T1 = _mm256_loadu2_m128i((__m128i*)(p_pred + i_pred), (__m128i*)p_pred); p_pred += (i_pred << 1);\
        T4 = _mm256_loadu2_m128i((__m128i*)(p_org + i_org), (__m128i*)p_org); p_org += (i_org << 1); \
        T5 = _mm256_loadu2_m128i((__m128i*)(p_pred + i_pred), (__m128i*)p_pred); p_pred += (i_pred << 1); \
        T0 = _mm256_sub_epi16(T0, T1); \
        T4 = _mm256_sub_epi16(T4, T5); \
        T1 = _mm256_unpacklo_epi64(T0, T4); \
        T5 = _mm256_unpackhi_epi64(T0, T4); \
        T2 = _mm256_add_epi16(T1, T5); \
        T3 = _mm256_sub_epi16(T1, T5); \
        T0 = _mm256_unpacklo_epi32(T2, T3); \
        T4 = _mm256_unpackhi_epi32(T2, T3); \
        T1 = _mm256_unpackhi_epi32(T0, T0); \
        T2 = _mm256_unpacklo_epi32(T0, T0); \
        T5 = _mm256_unpackhi_epi32(T4, T4); \
        T6 = _mm256_unpacklo_epi32(T4, T4); \
        T1 = _mm256_sign_epi16(T1, sign); \
        T5 = _mm256_sign_epi16(T5, sign); \
        T0 = _mm256_add_epi16(T2, T1); \
        T4 = _mm256_add_epi16(T6, T5); \
        T1 = _mm256_hadd_epi16(T0, T4); \
        T2 = _mm256_hsub_epi16(T0, T4); \
        M##i = _mm256_unpacklo_epi16(T1, T2); \
        M##j = _mm256_unpackhi_epi16(T1, T2); \
    }

    HOR_LINE256(01, 23)

#undef HOR_LINE256

    // vertical
    T01 = _mm256_add_epi16(M01, M23);
    T23 = _mm256_sub_epi16(M01, M23);

    N01 = _mm256_permute2x128_si256(T01, T23, 0x02);
    N23 = _mm256_permute2x128_si256(T01, T23, 0x13);

    T01 = _mm256_abs_epi16(_mm256_add_epi16(N01, N23));
    T23 = _mm256_abs_epi16(_mm256_sub_epi16(N01, N23));

    T01 = _mm256_add_epi16(T01, T23);

    T23 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(T01));
    T01 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(T01, 1));

    T01 = _mm256_add_epi32(T01, T23);

    T01 = _mm256_hadd_epi32(T01, T01);
    T01 = _mm256_hadd_epi32(T01, T01);

    return (u32)((_mm256_extract_epi32(T01, 0) + _mm256_extract_epi32(T01, 4)) / com_tbl_sqrt[0] * 2.0);
}

u32 uavs3e_had_8x8_avx2(pel* p_org, int i_org, pel* p_pred, int i_pred)
{
    __m256i T01, T23, T45, T67;
    __m256i M01, M23, M45, M67;
    __m256i N01, N23, N45, N67;
    __m256i sign = _mm256_set_epi16(1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1);

#define HOR_LINE256(i, j) { \
        __m256i T0, T1, T2, T3, T4, T5, T6; \
        T0 = _mm256_loadu2_m128i((__m128i*)(p_org + i_org), (__m128i*)p_org); p_org  += (i_org  << 1);\
        T1 = _mm256_loadu2_m128i((__m128i*)(p_pred + i_pred), (__m128i*)p_pred); p_pred += (i_pred << 1);\
        T4 = _mm256_loadu2_m128i((__m128i*)(p_org + i_org), (__m128i*)p_org); p_org += (i_org << 1); \
        T5 = _mm256_loadu2_m128i((__m128i*)(p_pred + i_pred), (__m128i*)p_pred); p_pred += (i_pred << 1); \
        T0 = _mm256_sub_epi16(T0, T1); \
        T4 = _mm256_sub_epi16(T4, T5); \
        T1 = _mm256_unpacklo_epi64(T0, T4); \
        T5 = _mm256_unpackhi_epi64(T0, T4); \
        T2 = _mm256_add_epi16(T1, T5); \
        T3 = _mm256_sub_epi16(T1, T5); \
        T0 = _mm256_unpacklo_epi32(T2, T3); \
        T4 = _mm256_unpackhi_epi32(T2, T3); \
        T1 = _mm256_unpackhi_epi32(T0, T0); \
        T2 = _mm256_unpacklo_epi32(T0, T0); \
        T5 = _mm256_unpackhi_epi32(T4, T4); \
        T6 = _mm256_unpacklo_epi32(T4, T4); \
        T1 = _mm256_sign_epi16(T1, sign); \
        T5 = _mm256_sign_epi16(T5, sign); \
        T0 = _mm256_add_epi16(T2, T1); \
        T4 = _mm256_add_epi16(T6, T5); \
        T1 = _mm256_hadd_epi16(T0, T4); \
        T2 = _mm256_hsub_epi16(T0, T4); \
        M##i = _mm256_unpacklo_epi16(T1, T2); \
        M##j = _mm256_unpackhi_epi16(T1, T2); \
    }

    HOR_LINE256(01, 23)
    HOR_LINE256(45, 67)

#undef HOR_LINE256

    // vertical
    T01 = _mm256_add_epi16(M01, M45);
    T23 = _mm256_add_epi16(M23, M67);
    T45 = _mm256_sub_epi16(M01, M45);
    T67 = _mm256_sub_epi16(M23, M67);

    M01 = _mm256_add_epi16(T01, T23);
    M23 = _mm256_sub_epi16(T01, T23);
    M45 = _mm256_add_epi16(T45, T67);
    M67 = _mm256_sub_epi16(T45, T67);

    N01 = _mm256_permute2x128_si256(M01, M23, 0x02);
    N23 = _mm256_permute2x128_si256(M01, M23, 0x13);
    N45 = _mm256_permute2x128_si256(M45, M67, 0x02);
    N67 = _mm256_permute2x128_si256(M45, M67, 0x13);

    {
        __m256i C0 = _mm256_set1_epi16(1);
        __m256i C1 = _mm256_set_epi16(-1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1);
        __m256i T0, T1, T2, T3;
        T0 = _mm256_unpacklo_epi16(N01, N23);
        T1 = _mm256_unpackhi_epi16(N01, N23);
        T2 = _mm256_unpacklo_epi16(N45, N67);
        T3 = _mm256_unpackhi_epi16(N45, N67);
        T01 = _mm256_madd_epi16(T0, C0);
        T23 = _mm256_madd_epi16(T0, C1);
        T45 = _mm256_madd_epi16(T1, C0);
        T67 = _mm256_madd_epi16(T1, C1);
        T01 = _mm256_abs_epi32(T01);
        T23 = _mm256_abs_epi32(T23);
        T45 = _mm256_abs_epi32(T45);
        T67 = _mm256_abs_epi32(T67);

        N01 = _mm256_madd_epi16(T2, C0);
        N23 = _mm256_madd_epi16(T2, C1);
        N45 = _mm256_madd_epi16(T3, C0);
        N67 = _mm256_madd_epi16(T3, C1);
        N01 = _mm256_abs_epi32(N01);
        N23 = _mm256_abs_epi32(N23);
        N45 = _mm256_abs_epi32(N45);
        N67 = _mm256_abs_epi32(N67);

        T01 = _mm256_add_epi32(T01, N01);
        T23 = _mm256_add_epi32(T23, N23);
        T45 = _mm256_add_epi32(T45, N45);
        T67 = _mm256_add_epi32(T67, N67);

        T01 = _mm256_add_epi32(T01, T23);
        T45 = _mm256_add_epi32(T45, T67);
        T01 = _mm256_add_epi32(T01, T45);

        T01 = _mm256_hadd_epi32(T01, T01);
        T01 = _mm256_hadd_epi32(T01, T01);
    }
    return ((_mm256_extract_epi32(T01, 0) + _mm256_extract_epi32(T01, 4)) + 2) >> 2;
}

u32 uavs3e_had_16x8_avx2(pel* p_org, int i_org, pel* p_pred, int i_pred)
{
    __m256i T0, T1, T2, T3, T4, T5, T6, T7;
    __m256i M0, M1, M2, M3, M4, M5, M6, M7;
    __m256i sign = _mm256_set_epi16(1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1);

#define HOR_LINE(i, j) { \
    T0 = _mm256_loadu2_m128i((__m128i*)(p_org  + i_org ), (__m128i*)p_org ); \
    T1 = _mm256_loadu2_m128i((__m128i*)(p_pred + i_pred), (__m128i*)p_pred); \
    T2 = _mm256_loadu2_m128i((__m128i*)(p_org  + i_org  + 8), (__m128i*)(p_org  + 8)); p_org  += i_org  << 1; \
    T3 = _mm256_loadu2_m128i((__m128i*)(p_pred + i_pred + 8), (__m128i*)(p_pred + 8)); p_pred += i_pred << 1;\
    T1 = _mm256_sub_epi16(T0, T1); \
    T5 = _mm256_sub_epi16(T2, T3); \
    T0 = _mm256_add_epi16(T1, T5); \
    T4 = _mm256_sub_epi16(T1, T5); \
    T1 = _mm256_unpacklo_epi64(T0, T4); \
    T5 = _mm256_unpackhi_epi64(T0, T4); \
    T2 = _mm256_add_epi16(T1, T5); \
    T3 = _mm256_sub_epi16(T1, T5); \
    T0 = _mm256_unpacklo_epi32(T2, T3); \
    T4 = _mm256_unpackhi_epi32(T2, T3); \
    T1 = _mm256_unpackhi_epi32(T0, T0); \
    T2 = _mm256_unpacklo_epi32(T0, T0); \
    T5 = _mm256_unpackhi_epi32(T4, T4); \
    T6 = _mm256_unpacklo_epi32(T4, T4); \
    T1 = _mm256_sign_epi16(T1, sign); \
    T5 = _mm256_sign_epi16(T5, sign); \
    T0 = _mm256_add_epi16(T2, T1); \
    T4 = _mm256_add_epi16(T6, T5); \
    T1 = _mm256_hadd_epi16(T0, T4); \
    T2 = _mm256_hsub_epi16(T0, T4); \
    T0 = _mm256_unpacklo_epi16(T1, T2); \
    T4 = _mm256_unpackhi_epi16(T1, T2); \
    M##i = _mm256_permute2x128_si256(T0, T4, 0x02); \
    M##j = _mm256_permute2x128_si256(T0, T4, 0x13); \
}
    HOR_LINE(0, 1);
    HOR_LINE(2, 3);
    HOR_LINE(4, 5);
    HOR_LINE(6, 7);

#undef HOR_LINE

    // vertical
    T0 = _mm256_add_epi16(M0, M4);
    T1 = _mm256_add_epi16(M1, M5);
    T2 = _mm256_add_epi16(M2, M6);
    T3 = _mm256_add_epi16(M3, M7);
    T4 = _mm256_sub_epi16(M0, M4);
    T5 = _mm256_sub_epi16(M1, M5);
    T6 = _mm256_sub_epi16(M2, M6);
    T7 = _mm256_sub_epi16(M3, M7);

    {
        __m256i C0 = _mm256_set1_epi16(1);
        __m256i C1 = _mm256_set_epi16(-1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1);
        __m256i N0, N1, N2, N3, N4, N5, N6, N7;
        M0 = _mm256_unpacklo_epi16(T0, T2);
        M1 = _mm256_unpackhi_epi16(T0, T2);
        M2 = _mm256_unpacklo_epi16(T1, T3);
        M3 = _mm256_unpackhi_epi16(T1, T3);
        N0 = _mm256_madd_epi16(M0, C0);         // T0 + T2
        N1 = _mm256_madd_epi16(M0, C1);         // T0 - T2
        N2 = _mm256_madd_epi16(M1, C0);
        N3 = _mm256_madd_epi16(M1, C1);
        N4 = _mm256_madd_epi16(M2, C0);         // T1 + T3
        N5 = _mm256_madd_epi16(M2, C1);         // T1 - T3
        N6 = _mm256_madd_epi16(M3, C0);
        N7 = _mm256_madd_epi16(M3, C1);

        M0 = _mm256_add_epi32(N0, N4);
        M1 = _mm256_sub_epi32(N0, N4);
        M2 = _mm256_add_epi32(N2, N6);
        M3 = _mm256_sub_epi32(N2, N6);
        M4 = _mm256_add_epi32(N1, N5);
        M5 = _mm256_sub_epi32(N1, N5);
        M6 = _mm256_add_epi32(N3, N7);
        M7 = _mm256_sub_epi32(N3, N7);

        M0 = _mm256_abs_epi32(M0);
        M1 = _mm256_abs_epi32(M1);
        M2 = _mm256_abs_epi32(M2);
        M3 = _mm256_abs_epi32(M3);
        M4 = _mm256_abs_epi32(M4);
        M5 = _mm256_abs_epi32(M5);
        M6 = _mm256_abs_epi32(M6);
        M7 = _mm256_abs_epi32(M7);

        T0 = _mm256_unpacklo_epi16(T4, T6);
        T1 = _mm256_unpackhi_epi16(T4, T6);
        T2 = _mm256_unpacklo_epi16(T5, T7);
        T3 = _mm256_unpackhi_epi16(T5, T7);
        N0 = _mm256_madd_epi16(T0, C0);
        N1 = _mm256_madd_epi16(T0, C1);
        N2 = _mm256_madd_epi16(T1, C0);
        N3 = _mm256_madd_epi16(T1, C1);
        N4 = _mm256_madd_epi16(T2, C0);
        N5 = _mm256_madd_epi16(T2, C1);
        N6 = _mm256_madd_epi16(T3, C0);
        N7 = _mm256_madd_epi16(T3, C1);

        T0 = _mm256_add_epi32(N0, N4);
        T1 = _mm256_sub_epi32(N0, N4);
        T2 = _mm256_add_epi32(N2, N6);
        T3 = _mm256_sub_epi32(N2, N6);
        T4 = _mm256_add_epi32(N1, N5);
        T5 = _mm256_sub_epi32(N1, N5);
        T6 = _mm256_add_epi32(N3, N7);
        T7 = _mm256_sub_epi32(N3, N7);

        T0 = _mm256_abs_epi32(T0);
        T1 = _mm256_abs_epi32(T1);
        T2 = _mm256_abs_epi32(T2);
        T3 = _mm256_abs_epi32(T3);
        T4 = _mm256_abs_epi32(T4);
        T5 = _mm256_abs_epi32(T5);
        T6 = _mm256_abs_epi32(T6);
        T7 = _mm256_abs_epi32(T7);

        T0 = _mm256_add_epi32(T0, M0);
        T1 = _mm256_add_epi32(T1, M1);
        T2 = _mm256_add_epi32(T2, M2);
        T3 = _mm256_add_epi32(T3, M3);
        T4 = _mm256_add_epi32(T4, M4);
        T5 = _mm256_add_epi32(T5, M5);
        T6 = _mm256_add_epi32(T6, M6);
        T7 = _mm256_add_epi32(T7, M7);

        T0 = _mm256_add_epi32(T0, T1);
        T2 = _mm256_add_epi32(T2, T3);
        T4 = _mm256_add_epi32(T4, T5);
        T6 = _mm256_add_epi32(T6, T7);

        T0 = _mm256_add_epi32(T0, T2);
        T4 = _mm256_add_epi32(T4, T6);

        M0 = _mm256_add_epi32(T0, T4);

        M0 = _mm256_hadd_epi32(M0, M0);
        M0 = _mm256_hadd_epi32(M0, M0);
    }
    return (u32)((_mm256_extract_epi32(M0, 0) + _mm256_extract_epi32(M0, 4)) / com_tbl_sqrt[1] * 2);
}

u32 uavs3e_had_8x16_avx2(pel* p_org, int i_org, pel* p_pred, int i_pred)
{
    __m256i T01, T23, T45, T67, T89, T1011, T1213, T1415;
    __m256i M01, M23, M45, M67, M89, M1011, M1213, M1415;
    __m256i N01, N23, N45, N67, N89, N1011, N1213, N1415;
    __m256i sign = _mm256_set_epi16(1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1);

#define HOR_LINE256(i, j) { \
        __m256i T0, T1, T2, T3, T4, T5, T6; \
        T0 = _mm256_loadu2_m128i((__m128i*)(p_org + i_org), (__m128i*)p_org); p_org  += (i_org  << 1);\
        T1 = _mm256_loadu2_m128i((__m128i*)(p_pred + i_pred), (__m128i*)p_pred); p_pred += (i_pred << 1);\
        T4 = _mm256_loadu2_m128i((__m128i*)(p_org + i_org), (__m128i*)p_org); p_org += (i_org << 1); \
        T5 = _mm256_loadu2_m128i((__m128i*)(p_pred + i_pred), (__m128i*)p_pred); p_pred += (i_pred << 1); \
        T0 = _mm256_sub_epi16(T0, T1); \
        T4 = _mm256_sub_epi16(T4, T5); \
        T1 = _mm256_unpacklo_epi64(T0, T4); \
        T5 = _mm256_unpackhi_epi64(T0, T4); \
        T2 = _mm256_add_epi16(T1, T5); \
        T3 = _mm256_sub_epi16(T1, T5); \
        T0 = _mm256_unpacklo_epi32(T2, T3); \
        T4 = _mm256_unpackhi_epi32(T2, T3); \
        T1 = _mm256_unpackhi_epi32(T0, T0); \
        T2 = _mm256_unpacklo_epi32(T0, T0); \
        T5 = _mm256_unpackhi_epi32(T4, T4); \
        T6 = _mm256_unpacklo_epi32(T4, T4); \
        T1 = _mm256_sign_epi16(T1, sign); \
        T5 = _mm256_sign_epi16(T5, sign); \
        T0 = _mm256_add_epi16(T2, T1); \
        T4 = _mm256_add_epi16(T6, T5); \
        T1 = _mm256_hadd_epi16(T0, T4); \
        T2 = _mm256_hsub_epi16(T0, T4); \
        M##i = _mm256_unpacklo_epi16(T1, T2); \
        M##j = _mm256_unpackhi_epi16(T1, T2); \
    }

    HOR_LINE256(01, 23)
    HOR_LINE256(45, 67)
    HOR_LINE256(89, 1011)
    HOR_LINE256(1213, 1415)

#undef HOR_LINE256

    // vertical
    N01 = _mm256_add_epi16(M01, M89);
    N23 = _mm256_add_epi16(M23, M1011);
    N45 = _mm256_add_epi16(M45, M1213);
    N67 = _mm256_add_epi16(M67, M1415);
    N89 = _mm256_sub_epi16(M01, M89);
    N1011 = _mm256_sub_epi16(M23, M1011);
    N1213 = _mm256_sub_epi16(M45, M1213);
    N1415 = _mm256_sub_epi16(M67, M1415);

    T01 = _mm256_add_epi16(N01, N45);
    T23 = _mm256_add_epi16(N23, N67);
    T45 = _mm256_sub_epi16(N01, N45);
    T67 = _mm256_sub_epi16(N23, N67);
    T89 = _mm256_add_epi16(N89, N1213);
    T1011 = _mm256_add_epi16(N1011, N1415);
    T1213 = _mm256_sub_epi16(N89, N1213);
    T1415 = _mm256_sub_epi16(N1011, N1415);

    {
        __m256i C0 = _mm256_set1_epi16(1);
        __m256i C1 = _mm256_set_epi16(-1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1);
        __m256i N0, N1, N2, N3, N4, N5, N6, N7;
        __m256i T0, T1, T2, T3, T4, T5, T6, T7;

        T01 = _mm256_permute4x64_epi64(T01, 0xd8);
        T23 = _mm256_permute4x64_epi64(T23, 0xd8);
        T45 = _mm256_permute4x64_epi64(T45, 0xd8);
        T67 = _mm256_permute4x64_epi64(T67, 0xd8);

        N0 = _mm256_unpacklo_epi16(T01, T23);
        N1 = _mm256_unpackhi_epi16(T01, T23);
        N2 = _mm256_unpacklo_epi16(T45, T67);
        N3 = _mm256_unpackhi_epi16(T45, T67);

        T0 = _mm256_madd_epi16(N0, C0);
        T1 = _mm256_madd_epi16(N0, C1);
        T2 = _mm256_madd_epi16(N1, C0);
        T3 = _mm256_madd_epi16(N1, C1);
        T4 = _mm256_madd_epi16(N2, C0);
        T5 = _mm256_madd_epi16(N2, C1);
        T6 = _mm256_madd_epi16(N3, C0);
        T7 = _mm256_madd_epi16(N3, C1);

        N0 = _mm256_add_epi32(T0, T2);
        N1 = _mm256_sub_epi32(T0, T2);
        N2 = _mm256_add_epi32(T1, T3);
        N3 = _mm256_sub_epi32(T1, T3);
        N4 = _mm256_add_epi32(T4, T6);
        N5 = _mm256_sub_epi32(T4, T6);
        N6 = _mm256_add_epi32(T5, T7);
        N7 = _mm256_sub_epi32(T5, T7);

        N0 = _mm256_abs_epi32(N0);
        N1 = _mm256_abs_epi32(N1);
        N2 = _mm256_abs_epi32(N2);
        N3 = _mm256_abs_epi32(N3);
        N4 = _mm256_abs_epi32(N4);
        N5 = _mm256_abs_epi32(N5);
        N6 = _mm256_abs_epi32(N6);
        N7 = _mm256_abs_epi32(N7);

        N0 = _mm256_add_epi32(N0, N1);
        N1 = _mm256_add_epi32(N2, N3);
        N2 = _mm256_add_epi32(N4, N5);
        N3 = _mm256_add_epi32(N6, N7);

        T0 = _mm256_permute4x64_epi64(T89, 0xd8);
        T1 = _mm256_permute4x64_epi64(T1011, 0xd8);
        T2 = _mm256_permute4x64_epi64(T1213, 0xd8);
        T3 = _mm256_permute4x64_epi64(T1415, 0xd8);

        M01 = _mm256_unpacklo_epi16(T0, T1);
        M23 = _mm256_unpackhi_epi16(T0, T1);
        M45 = _mm256_unpacklo_epi16(T2, T3);
        M67 = _mm256_unpackhi_epi16(T2, T3);

        T0 = _mm256_madd_epi16(M01, C0);
        T1 = _mm256_madd_epi16(M01, C1);
        T2 = _mm256_madd_epi16(M23, C0);
        T3 = _mm256_madd_epi16(M23, C1);
        T4 = _mm256_madd_epi16(M45, C0);
        T5 = _mm256_madd_epi16(M45, C1);
        T6 = _mm256_madd_epi16(M67, C0);
        T7 = _mm256_madd_epi16(M67, C1);

        N4 = _mm256_add_epi32(T0, T2);
        N5 = _mm256_sub_epi32(T0, T2);
        N6 = _mm256_add_epi32(T1, T3);
        N7 = _mm256_sub_epi32(T1, T3);
        N4 = _mm256_abs_epi32(N4);
        N5 = _mm256_abs_epi32(N5);
        N6 = _mm256_abs_epi32(N6);
        N7 = _mm256_abs_epi32(N7);
        N0 = _mm256_add_epi32(N0, N4);
        N1 = _mm256_add_epi32(N1, N5);
        N2 = _mm256_add_epi32(N2, N6);
        N3 = _mm256_add_epi32(N3, N7);

        N4 = _mm256_add_epi32(T4, T6);
        N5 = _mm256_sub_epi32(T4, T6);
        N6 = _mm256_add_epi32(T5, T7);
        N7 = _mm256_sub_epi32(T5, T7);
        N4 = _mm256_abs_epi32(N4);
        N5 = _mm256_abs_epi32(N5);
        N6 = _mm256_abs_epi32(N6);
        N7 = _mm256_abs_epi32(N7);
        N0 = _mm256_add_epi32(N0, N4);
        N1 = _mm256_add_epi32(N1, N5);
        N2 = _mm256_add_epi32(N2, N6);
        N3 = _mm256_add_epi32(N3, N7);

        T01 = _mm256_add_epi32(N0, N1);
        T45 = _mm256_add_epi32(N2, N3);

        T01 = _mm256_add_epi32(T01, T45);

        T01 = _mm256_hadd_epi32(T01, T01);
        T01 = _mm256_hadd_epi32(T01, T01);
    }

    return (u32)((_mm256_extract_epi32(T01, 0) + _mm256_extract_epi32(T01, 4)) / com_tbl_sqrt[1] * 2.0);
}

#endif