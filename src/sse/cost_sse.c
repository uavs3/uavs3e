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

// --------------------------------------------------------------------------------------------------------------------
// SAD
// --------------------------------------------------------------------------------------------------------------------

#if (BIT_DEPTH == 8)

u32 uavs3e_get_sad_4_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m128i sum = _mm_setzero_si128();
    int i_orgx2 = i_org << 1;
    int i_orgx3 = i_orgx2 + i_org;
    int i_predx2 = i_pred << 1;
    int i_predx3 = i_predx2 + i_pred;

    height >>= 2;

    while (height--) {
        __m128i A = _mm_setr_epi32(*(int *)(p_org), *(int *)(p_org + i_org), *(int *)(p_org + i_orgx2), *(int *)(p_org + i_orgx3));
        __m128i B = _mm_setr_epi32(*(int *)(p_pred), *(int *)(p_pred + i_pred), *(int *)(p_pred + i_predx2), *(int *)(p_pred + i_predx3));
        __m128i C = _mm_sad_epu8(A, B);
        sum = _mm_add_epi64(sum, C);

        p_org  += i_org  << 2;
        p_pred += i_pred << 2;
    }

    return (u32)(_mm_extract_epi64(sum, 0) + _mm_extract_epi64(sum, 1));
}

u32 uavs3e_get_sad_8_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m128i sum = _mm_setzero_si128();
    __m128i o0, o1, p0, p1, s0, s1;
    int i_org2 = i_org << 1;
    int i_org3 = i_org2 + i_org;
    int i_pred2 = i_pred << 1;
    int i_pred3 = i_pred2 + i_pred;

    height >>= 2;

    while (height--) {
        o0 = _mm_set_epi64x(*(s64*)(p_org), *(s64*)(p_org + i_org));
        o1 = _mm_set_epi64x(*(s64*)(p_org + i_org2), *(s64*)(p_org + i_org3));
        p0 = _mm_set_epi64x(*(s64*)(p_pred), *(s64*)(p_pred + i_pred));
        p1 = _mm_set_epi64x(*(s64*)(p_pred + i_pred2), *(s64*)(p_pred + i_pred3));

        s0 = _mm_sad_epu8(o0, p0);
        s1 = _mm_sad_epu8(o1, p1);
        sum = _mm_add_epi64(sum, s0);
        sum = _mm_add_epi64(sum, s1);

        p_org += i_org << 2;
        p_pred += i_pred << 2;
    }

    return (u32)(_mm_extract_epi64(sum, 0) + _mm_extract_epi64(sum, 1));
}

#define CAL_SAD_W16(d) { \
        __m128i A = _mm_loadu_si128((__m128i*)(p_org  + d * 16)); \
        __m128i B = _mm_loadu_si128((__m128i*)(p_pred + d * 16)); \
        __m128i C = _mm_sad_epu8(A, B); \
        sum = _mm_add_epi32(sum, C); \
    }

u32 uavs3e_get_sad_16_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m128i sum = _mm_setzero_si128();

    while (height--) {
        CAL_SAD_W16(0);
        p_org  += i_org;
        p_pred += i_pred;
    }
    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    return _mm_extract_epi32(sum, 0);
}

u32 uavs3e_get_sad_32_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m128i sum = _mm_setzero_si128();

    while (height--) {
        CAL_SAD_W16(0);
        CAL_SAD_W16(1);
        p_org += i_org;
        p_pred += i_pred;
    }
    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    return _mm_extract_epi32(sum, 0);
}

u32 uavs3e_get_sad_64_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m128i sum = _mm_setzero_si128();

    while (height--) {
        CAL_SAD_W16(0);
        CAL_SAD_W16(1);
        CAL_SAD_W16(2);
        CAL_SAD_W16(3);
        p_org += i_org;
        p_pred += i_pred;
    }
    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    return _mm_extract_epi32(sum, 0);
}

u32 uavs3e_get_sad_128_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m128i sum = _mm_setzero_si128();

    while (height--) {
        CAL_SAD_W16(0);
        CAL_SAD_W16(1);
        CAL_SAD_W16(2);
        CAL_SAD_W16(3);
        CAL_SAD_W16(4);
        CAL_SAD_W16(5);
        CAL_SAD_W16(6);
        CAL_SAD_W16(7);
        p_org += i_org;
        p_pred += i_pred;
    }
    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    return _mm_extract_epi32(sum, 0);
}

void uavs3e_get_sad_x3_4_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height)
{
    __m128i sum0, sum1, sum2;
    __m128i o, p0, p1, p2, t0, t1, t2;
    int i_orgx2 = i_org << 1;
    int i_orgx3 = i_orgx2 + i_org;
    int i_predx2 = i_pred << 1;
    int i_predx3 = i_predx2 + i_pred;

    height >>= 2;
    sum0 = sum1 = sum2 = _mm_setzero_si128();

    while (height--) {
        o = _mm_setr_epi32(*(int *)(p_org), *(int *)(p_org + i_org), *(int *)(p_org + i_orgx2), *(int *)(p_org + i_orgx3));
        p0 = _mm_setr_epi32(*(int *)(p_pred0), *(int *)(p_pred0 + i_pred), *(int *)(p_pred0 + i_predx2), *(int *)(p_pred0 + i_predx3));
        p1 = _mm_setr_epi32(*(int *)(p_pred1), *(int *)(p_pred1 + i_pred), *(int *)(p_pred1 + i_predx2), *(int *)(p_pred1 + i_predx3));
        p2 = _mm_setr_epi32(*(int *)(p_pred2), *(int *)(p_pred2 + i_pred), *(int *)(p_pred2 + i_predx2), *(int *)(p_pred2 + i_predx3));
        t0 = _mm_sad_epu8(o, p0);
        t1 = _mm_sad_epu8(o, p1);
        t2 = _mm_sad_epu8(o, p2);
        sum0 = _mm_add_epi64(sum0, t0);
        sum1 = _mm_add_epi64(sum1, t1);
        sum2 = _mm_add_epi64(sum2, t2);

        p_org += i_org << 2;
        p_pred0 += i_pred << 2;
        p_pred1 += i_pred << 2;
        p_pred2 += i_pred << 2;
    }

    sad[0] = (u32)(_mm_extract_epi64(sum0, 0) + _mm_extract_epi64(sum0, 1));
    sad[1] = (u32)(_mm_extract_epi64(sum1, 0) + _mm_extract_epi64(sum1, 1));
    sad[2] = (u32)(_mm_extract_epi64(sum2, 0) + _mm_extract_epi64(sum2, 1));
}

void uavs3e_get_sad_x3_8_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height)
{
    __m128i sum0, sum1, sum2;
    __m128i o0, p0, p1, p2, t0, t1, t2;
    int i_org2 = i_org << 1;
    int i_pred2 = i_pred << 1;

    height >>= 1;
    sum0 = sum1 = sum2 = _mm_setzero_si128();

    while (height--) {
        o0 = _mm_set_epi64x(*(s64*)(p_org), *(s64*)(p_org + i_org));
        p0 = _mm_set_epi64x(*(s64*)(p_pred0), *(s64*)(p_pred0 + i_pred));
        p1 = _mm_set_epi64x(*(s64*)(p_pred1), *(s64*)(p_pred1 + i_pred));
        p2 = _mm_set_epi64x(*(s64*)(p_pred2), *(s64*)(p_pred2 + i_pred));

        t0 = _mm_sad_epu8(o0, p0);
        t1 = _mm_sad_epu8(o0, p1);
        t2 = _mm_sad_epu8(o0, p2);
        sum0 = _mm_add_epi64(sum0, t0);
        sum1 = _mm_add_epi64(sum1, t1);
        sum2 = _mm_add_epi64(sum2, t2);

        p_org += i_org2;
        p_pred0 += i_pred2;
        p_pred1 += i_pred2;
        p_pred2 += i_pred2;
    }

    sad[0] = (u32)(_mm_extract_epi64(sum0, 0) + _mm_extract_epi64(sum0, 1));
    sad[1] = (u32)(_mm_extract_epi64(sum1, 0) + _mm_extract_epi64(sum1, 1));
    sad[2] = (u32)(_mm_extract_epi64(sum2, 0) + _mm_extract_epi64(sum2, 1));
}

#define CAL_SAD_W16_X3(d) { \
        __m128i o = _mm_loadu_si128((__m128i*)(p_org  + d * 16)); \
        __m128i p0 = _mm_loadu_si128((__m128i*)(p_pred0 + d * 16)); \
        __m128i p1 = _mm_loadu_si128((__m128i*)(p_pred1 + d * 16)); \
        __m128i p2 = _mm_loadu_si128((__m128i*)(p_pred2 + d * 16)); \
        __m128i t0 = _mm_sad_epu8(o, p0); \
        __m128i t1 = _mm_sad_epu8(o, p1); \
        __m128i t2 = _mm_sad_epu8(o, p2); \
        sum0 = _mm_add_epi64(sum0, t0); \
        sum1 = _mm_add_epi64(sum1, t1); \
        sum2 = _mm_add_epi64(sum2, t2); \
    }

void uavs3e_get_sad_x3_16_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height)
{
    __m128i sum0, sum1, sum2;

    sum0 = sum1 = sum2 = _mm_setzero_si128();

    while (height--) {
        CAL_SAD_W16_X3(0);
        p_org += i_org;
        p_pred0 += i_pred;
        p_pred1 += i_pred;
        p_pred2 += i_pred;
    }
    sad[0] = (u32)(_mm_extract_epi64(sum0, 0) + _mm_extract_epi64(sum0, 1));
    sad[1] = (u32)(_mm_extract_epi64(sum1, 0) + _mm_extract_epi64(sum1, 1));
    sad[2] = (u32)(_mm_extract_epi64(sum2, 0) + _mm_extract_epi64(sum2, 1));
}

void uavs3e_get_sad_x3_32_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height)
{
    __m128i sum0, sum1, sum2;

    sum0 = sum1 = sum2 = _mm_setzero_si128();

    while (height--) {
        CAL_SAD_W16_X3(0);
        CAL_SAD_W16_X3(1);
        p_org += i_org;
        p_pred0 += i_pred;
        p_pred1 += i_pred;
        p_pred2 += i_pred;
    }
    sad[0] = (u32)(_mm_extract_epi64(sum0, 0) + _mm_extract_epi64(sum0, 1));
    sad[1] = (u32)(_mm_extract_epi64(sum1, 0) + _mm_extract_epi64(sum1, 1));
    sad[2] = (u32)(_mm_extract_epi64(sum2, 0) + _mm_extract_epi64(sum2, 1));
}

void uavs3e_get_sad_x3_64_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height)
{
    __m128i sum0, sum1, sum2;

    sum0 = sum1 = sum2 = _mm_setzero_si128();

    while (height--) {
        CAL_SAD_W16_X3(0);
        CAL_SAD_W16_X3(1);
        CAL_SAD_W16_X3(2);
        CAL_SAD_W16_X3(3);
        p_org += i_org;
        p_pred0 += i_pred;
        p_pred1 += i_pred;
        p_pred2 += i_pred;
    }
    sad[0] = (u32)(_mm_extract_epi64(sum0, 0) + _mm_extract_epi64(sum0, 1));
    sad[1] = (u32)(_mm_extract_epi64(sum1, 0) + _mm_extract_epi64(sum1, 1));
    sad[2] = (u32)(_mm_extract_epi64(sum2, 0) + _mm_extract_epi64(sum2, 1));
}

void uavs3e_get_sad_x3_128_sse(pel *p_org, int i_org, pel *p_pred0, pel *p_pred1, pel *p_pred2, int i_pred, u32 sad[3], int height)
{
    __m128i sum0, sum1, sum2;

    sum0 = sum1 = sum2 = _mm_setzero_si128();

    while (height--) {
        CAL_SAD_W16_X3(0);
        CAL_SAD_W16_X3(1);
        CAL_SAD_W16_X3(2);
        CAL_SAD_W16_X3(3);
        CAL_SAD_W16_X3(4);
        CAL_SAD_W16_X3(5);
        CAL_SAD_W16_X3(6);
        CAL_SAD_W16_X3(7);
        p_org += i_org;
        p_pred0 += i_pred;
        p_pred1 += i_pred;
        p_pred2 += i_pred;
    }
    sad[0] = (u32)(_mm_extract_epi64(sum0, 0) + _mm_extract_epi64(sum0, 1));
    sad[1] = (u32)(_mm_extract_epi64(sum1, 0) + _mm_extract_epi64(sum1, 1));
    sad[2] = (u32)(_mm_extract_epi64(sum2, 0) + _mm_extract_epi64(sum2, 1));
}

// --------------------------------------------------------------------------------------------------------------------
// SSD
// --------------------------------------------------------------------------------------------------------------------

u64 uavs3e_get_ssd_4_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m128i sum = _mm_setzero_si128();

    height >>= 1;

    while (height--) {
        __m128i A = _mm_setr_epi32(*(int *)(p_org), *(int *)(p_org  + i_org), 0, 0);
        __m128i B = _mm_setr_epi32(*(int *)(p_pred), *(int *)(p_pred + i_pred), 0, 0);
        A = _mm_cvtepu8_epi16(A);
        B = _mm_cvtepu8_epi16(B);

        __m128i C = _mm_sub_epi16(A, B);
        C = _mm_madd_epi16(C, C);
        sum = _mm_add_epi32(sum, C);
        p_org  += (i_org  << 1);
        p_pred += (i_pred << 1);
    }
    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    return _mm_extract_epi32(sum, 0);
}

u64 uavs3e_get_ssd_8_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m128i sum = _mm_setzero_si128();

    while (height--) {
        __m128i A = _mm_loadl_epi64((__m128i *)(p_org));
        __m128i B = _mm_loadl_epi64((__m128i *)(p_pred));
        A = _mm_cvtepu8_epi16(A);
        B = _mm_cvtepu8_epi16(B);

        __m128i C = _mm_sub_epi16(A, B);
        C = _mm_madd_epi16(C, C);
        sum = _mm_add_epi32(sum, C);
        p_org  += i_org;
        p_pred += i_pred;
    }
    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    return _mm_extract_epi32(sum, 0);
}

#define CAL_SSD_W16(d) { \
        __m128i A = _mm_loadu_si128((__m128i*)(p_org  + d * 16)); \
        __m128i B = _mm_loadu_si128((__m128i*)(p_pred + d * 16)); \
        __m128i C = _mm_sub_epi16(_mm_unpacklo_epi8(A, zero), _mm_unpacklo_epi8(B, zero)); \
        __m128i D = _mm_sub_epi16(_mm_unpackhi_epi8(A, zero), _mm_unpackhi_epi8(B, zero)); \
        C = _mm_madd_epi16(C, C);\
        D = _mm_madd_epi16(D, D);\
        sum = _mm_add_epi32(sum, C); \
        sum = _mm_add_epi32(sum, D); \
    }

u64 uavs3e_get_ssd_16_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m128i zero = _mm_setzero_si128();
    __m128i sum = _mm_setzero_si128();

    while (height--) {
        CAL_SSD_W16(0);
        p_org  += i_org;
        p_pred += i_pred;
    }
    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    return (u32)_mm_extract_epi32(sum, 0);
}

u64 uavs3e_get_ssd_32_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m128i zero = _mm_setzero_si128();
    __m128i sum = _mm_setzero_si128();

    while (height--) {
        CAL_SSD_W16(0);
        CAL_SSD_W16(1);
        p_org += i_org;
        p_pred += i_pred;
    }
    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    return (u32)_mm_extract_epi32(sum, 0);
}

u64 uavs3e_get_ssd_64_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m128i zero = _mm_setzero_si128();
    __m128i sum = _mm_setzero_si128();

    while (height--) {
        CAL_SSD_W16(0);
        CAL_SSD_W16(1);
        CAL_SSD_W16(2);
        CAL_SSD_W16(3);
        p_org += i_org;
        p_pred += i_pred;
    }
    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    return (u32)_mm_extract_epi32(sum, 0);
}

u64 uavs3e_get_ssd_128_sse(pel *p_org, int i_org, pel *p_pred, int i_pred, int height)
{
    __m128i zero = _mm_setzero_si128();
    __m128i sum = _mm_setzero_si128();

    while (height--) {
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
    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    return (u32)_mm_extract_epi32(sum, 0);
}

u32 uavs3e_had_4x4_sse(pel *org, int s_org, pel *cur, int s_cur)
{
    int satd = 0;
    __m128i sum;

    __m128i r0 = _mm_cvtepu8_epi16(_mm_setr_epi32(*(int *)(org), *(int *)(org + s_org), 0, 0));
    __m128i r1 = _mm_srli_si128(r0, 8);
    __m128i r2 = _mm_cvtepu8_epi16(_mm_setr_epi32(*(int *)(org + 2 * s_org), *(int *)(org + 3 * s_org), 0, 0));
    __m128i r3 = _mm_srli_si128(r2, 8);
    __m128i r4 = _mm_cvtepu8_epi16(_mm_setr_epi32(*(int *)(cur), *(int *)(cur + s_cur), 0, 0));
    __m128i r5 = _mm_srli_si128(r4, 8);
    __m128i r6 = _mm_cvtepu8_epi16(_mm_setr_epi32(*(int *)(cur + 2 * s_cur), *(int *)(cur + 3 * s_cur), 0, 0));
    __m128i r7 = _mm_srli_si128(r6, 8);

    r0 = _mm_sub_epi16(r0, r4);
    r1 = _mm_sub_epi16(r1, r5);
    r2 = _mm_sub_epi16(r2, r6);
    r3 = _mm_sub_epi16(r3, r7);

    // first stage
    r4 = r0;
    r5 = r1;
    r0 = _mm_add_epi16(r0, r3);
    r1 = _mm_add_epi16(r1, r2);
    r4 = _mm_sub_epi16(r4, r3);
    r5 = _mm_sub_epi16(r5, r2);
    r2 = r0;
    r3 = r4;
    r0 = _mm_add_epi16(r0, r1);
    r2 = _mm_sub_epi16(r2, r1);
    r3 = _mm_sub_epi16(r3, r5);
    r5 = _mm_add_epi16(r5, r4);
    // shuffle - flip matrix for vertical transform
    r0 = _mm_unpacklo_epi16(r0, r5);
    r2 = _mm_unpacklo_epi16(r2, r3);
    r3 = r0;
    r0 = _mm_unpacklo_epi32(r0, r2);
    r3 = _mm_unpackhi_epi32(r3, r2);
    r1 = r0;
    r2 = r3;
    r1 = _mm_srli_si128(r1, 8);
    r3 = _mm_srli_si128(r3, 8);
    // second stage
    r4 = r0;
    r5 = r1;
    r0 = _mm_add_epi16(r0, r3);
    r1 = _mm_add_epi16(r1, r2);
    r4 = _mm_sub_epi16(r4, r3);
    r5 = _mm_sub_epi16(r5, r2);
    r2 = r0;
    r3 = r4;
    r0 = _mm_add_epi16(r0, r1);
    r2 = _mm_sub_epi16(r2, r1);
    r3 = _mm_sub_epi16(r3, r5);
    r5 = _mm_add_epi16(r5, r4);
    // abs
    sum = _mm_abs_epi16(r0);
    sum = _mm_add_epi16(sum, _mm_abs_epi16(r2));
    sum = _mm_add_epi16(sum, _mm_abs_epi16(r3));
    sum = _mm_add_epi16(sum, _mm_abs_epi16(r5));

    sum = _mm_cvtepu16_epi32(sum);
    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    satd = _mm_cvtsi128_si32(sum);
    satd = ((satd + 1) >> 1);
    return satd;
}

u32 uavs3e_had_8x8_sse(pel *p_org, int i_org, pel *p_pred, int i_pred)
{
    __m128i T0, T1, T2, T3, T4, T5, T6, T7;
    __m128i M0, M1, M2, M3, M4, M5, M6, M7;
    const __m128i zero = _mm_setzero_si128();
    __m128i sign = _mm_set_epi16(1,1,-1,-1,1,1,-1,-1);

    // horizontal
#define HOR_LINE(i, j) { \
        T0 = _mm_loadl_epi64((__m128i const*)p_org ); \
        T1 = _mm_loadl_epi64((__m128i const*)p_pred); \
        T4 = _mm_loadl_epi64((__m128i const*)(p_org  + i_org )); \
        T5 = _mm_loadl_epi64((__m128i const*)(p_pred + i_pred)); \
        T0 = _mm_cvtepu8_epi16(T0); \
        T1 = _mm_cvtepu8_epi16(T1); \
        T4 = _mm_cvtepu8_epi16(T4); \
        T5 = _mm_cvtepu8_epi16(T5); \
        T0 = _mm_sub_epi16(T0, T1); \
        T4 = _mm_sub_epi16(T4, T5); \
        T1 = _mm_unpacklo_epi64(T0, T4); \
        T5 = _mm_unpackhi_epi64(T0, T4); \
        T2 = _mm_add_epi16(T1, T5); \
        T3 = _mm_sub_epi16(T1, T5); \
        T0 = _mm_unpacklo_epi32(T2, T3); \
        T4 = _mm_unpackhi_epi32(T2, T3); \
        T1 = _mm_unpackhi_epi32(T0, T0); \
        T2 = _mm_unpacklo_epi32(T0, T0); \
        T5 = _mm_unpackhi_epi32(T4, T4); \
        T6 = _mm_unpacklo_epi32(T4, T4); \
        T1 = _mm_sign_epi16(T1, sign); \
        T5 = _mm_sign_epi16(T5, sign); \
        T0 = _mm_add_epi16(T2, T1); \
        T4 = _mm_add_epi16(T6, T5); \
        T1 = _mm_hadd_epi16(T0, T4); \
        T2 = _mm_hsub_epi16(T0, T4); \
        M##i = _mm_unpacklo_epi16(T1, T2); \
        M##j = _mm_unpackhi_epi16(T1, T2); \
        p_pred += (i_pred << 1); \
        p_org  += (i_org  << 1); \
    }

    HOR_LINE(0, 1)
    HOR_LINE(2, 3)
    HOR_LINE(4, 5)
    HOR_LINE(6, 7)
 
#undef HOR_LINE

    // vertical
    T0 = _mm_add_epi16(M0, M4);
    T1 = _mm_add_epi16(M1, M5);
    T2 = _mm_add_epi16(M2, M6);
    T3 = _mm_add_epi16(M3, M7);
    T4 = _mm_sub_epi16(M0, M4);
    T5 = _mm_sub_epi16(M1, M5);
    T6 = _mm_sub_epi16(M2, M6);
    T7 = _mm_sub_epi16(M3, M7);

    M0 = _mm_add_epi16(T0, T2);
    M1 = _mm_add_epi16(T1, T3);
    M2 = _mm_sub_epi16(T0, T2);
    M3 = _mm_sub_epi16(T1, T3);
    M4 = _mm_add_epi16(T4, T6);
    M5 = _mm_add_epi16(T5, T7);
    M6 = _mm_sub_epi16(T4, T6);
    M7 = _mm_sub_epi16(T5, T7);

    T0 = _mm_abs_epi16(_mm_add_epi16(M0, M1));
    T1 = _mm_abs_epi16(_mm_sub_epi16(M0, M1));
    T2 = _mm_abs_epi16(_mm_add_epi16(M2, M3));
    T3 = _mm_abs_epi16(_mm_sub_epi16(M2, M3));
    T4 = _mm_abs_epi16(_mm_add_epi16(M4, M5));
    T5 = _mm_abs_epi16(_mm_sub_epi16(M4, M5));
    T6 = _mm_abs_epi16(_mm_add_epi16(M6, M7));
    T7 = _mm_abs_epi16(_mm_sub_epi16(M6, M7));

    T0 = _mm_add_epi16(T0, T1);
    T2 = _mm_add_epi16(T2, T3);
    T4 = _mm_add_epi16(T4, T5);
    T6 = _mm_add_epi16(T6, T7);

    T0 = _mm_add_epi16(T0, T2);
    T4 = _mm_add_epi16(T4, T6);

    M0 = _mm_cvtepi16_epi32(T0);
    M2 = _mm_cvtepi16_epi32(T4);
    M4 = _mm_unpackhi_epi16(T0, zero);
    M6 = _mm_unpackhi_epi16(T4, zero);

    M0 = _mm_add_epi32(M0, M2);
    M4 = _mm_add_epi32(M4, M6);

    M0 = _mm_add_epi32(M0, M4);

    M0 = _mm_hadd_epi32(M0, M0);
    M0 = _mm_hadd_epi32(M0, M0);

    return (_mm_cvtsi128_si32(M0) + 2) >> 2;
}

u32 uavs3e_had_16x8_sse(pel *p_org, int i_org, pel *p_pred, int i_pred)
{
    u32 uiSum = 0;
    __m128i T0_0, T1_0, T2_0, T3_0, T4_0, T5_0, T6_0, T7_0;
    __m128i T0_1, T1_1, T2_1, T3_1, T4_1, T5_1, T6_1, T7_1;
    __m128i M0_0, M1_0, M2_0, M3_0, M4_0, M5_0, M6_0, M7_0;
    __m128i M0_1, M1_1, M2_1, M3_1, M4_1, M5_1, M6_1, M7_1;
    const __m128i zero = _mm_setzero_si128();
    __m128i sum;
    __m128i sign = _mm_set_epi16(1,1,-1,-1,1,1,-1,-1);

#define HOR_LINE(i) { \
    __m128i T0, T1, T2, T3, T4, T5, T6; \
    T0 = _mm_loadu_si128((__m128i const*)p_org ); p_org  += i_org;  \
    T4 = _mm_loadu_si128((__m128i const*)p_pred); p_pred += i_pred; \
    T1 = _mm_sub_epi16(_mm_cvtepu8_epi16(T0), _mm_cvtepu8_epi16(T4)); \
    T5 = _mm_sub_epi16(_mm_cvtepu8_epi16(_mm_srli_si128(T0, 8)), _mm_cvtepu8_epi16(_mm_srli_si128(T4, 8))); \
    T0 = _mm_add_epi16(T1, T5); \
    T4 = _mm_sub_epi16(T1, T5); \
    T1 = _mm_unpacklo_epi64(T0, T4); \
    T5 = _mm_unpackhi_epi64(T0, T4); \
    T2 = _mm_add_epi16(T1, T5); \
    T3 = _mm_sub_epi16(T1, T5); \
    T0 = _mm_unpacklo_epi32(T2, T3); \
    T4 = _mm_unpackhi_epi32(T2, T3); \
    T1 = _mm_unpackhi_epi32(T0, T0); \
    T2 = _mm_unpacklo_epi32(T0, T0); \
    T5 = _mm_unpackhi_epi32(T4, T4); \
    T6 = _mm_unpacklo_epi32(T4, T4); \
    T1 = _mm_sign_epi16(T1, sign); \
    T5 = _mm_sign_epi16(T5, sign); \
    T0 = _mm_add_epi16(T2, T1); \
    T4 = _mm_add_epi16(T6, T5); \
    T1 = _mm_hadd_epi16(T0, T4); \
    T2 = _mm_hsub_epi16(T0, T4); \
    M##i##_0 = _mm_unpacklo_epi16(T1, T2); \
    M##i##_1 = _mm_unpackhi_epi16(T1, T2); \
}

    HOR_LINE(0);
    HOR_LINE(1);
    HOR_LINE(2);
    HOR_LINE(3);
    HOR_LINE(4);
    HOR_LINE(5);
    HOR_LINE(6);
    HOR_LINE(7);
 
#undef HOR_LINE

    // vertical
    T0_0 = _mm_add_epi16(M0_0, M4_0); T0_1 = _mm_add_epi16(M0_1, M4_1);
    T1_0 = _mm_add_epi16(M1_0, M5_0); T1_1 = _mm_add_epi16(M1_1, M5_1);
    T2_0 = _mm_add_epi16(M2_0, M6_0); T2_1 = _mm_add_epi16(M2_1, M6_1);
    T3_0 = _mm_add_epi16(M3_0, M7_0); T3_1 = _mm_add_epi16(M3_1, M7_1);
    T4_0 = _mm_sub_epi16(M0_0, M4_0); T4_1 = _mm_sub_epi16(M0_1, M4_1);
    T5_0 = _mm_sub_epi16(M1_0, M5_0); T5_1 = _mm_sub_epi16(M1_1, M5_1);
    T6_0 = _mm_sub_epi16(M2_0, M6_0); T6_1 = _mm_sub_epi16(M2_1, M6_1);
    T7_0 = _mm_sub_epi16(M3_0, M7_0); T7_1 = _mm_sub_epi16(M3_1, M7_1);
    
    M0_0 = _mm_add_epi16(T0_0, T2_0); M0_1 = _mm_add_epi16(T0_1, T2_1);
    M1_0 = _mm_add_epi16(T1_0, T3_0); M1_1 = _mm_add_epi16(T1_1, T3_1);
    M2_0 = _mm_sub_epi16(T0_0, T2_0); M2_1 = _mm_sub_epi16(T0_1, T2_1);
    M3_0 = _mm_sub_epi16(T1_0, T3_0); M3_1 = _mm_sub_epi16(T1_1, T3_1);
    M4_0 = _mm_add_epi16(T4_0, T6_0); M4_1 = _mm_add_epi16(T4_1, T6_1);
    M5_0 = _mm_add_epi16(T5_0, T7_0); M5_1 = _mm_add_epi16(T5_1, T7_1);
    M6_0 = _mm_sub_epi16(T4_0, T6_0); M6_1 = _mm_sub_epi16(T4_1, T6_1);
    M7_0 = _mm_sub_epi16(T5_0, T7_0); M7_1 = _mm_sub_epi16(T5_1, T7_1);
    
    T0_0 = _mm_abs_epi16(_mm_add_epi16(M0_0, M1_0)); T0_1 = _mm_abs_epi16(_mm_add_epi16(M0_1, M1_1));
    T1_0 = _mm_abs_epi16(_mm_sub_epi16(M0_0, M1_0)); T1_1 = _mm_abs_epi16(_mm_sub_epi16(M0_1, M1_1));
    T2_0 = _mm_abs_epi16(_mm_add_epi16(M2_0, M3_0)); T2_1 = _mm_abs_epi16(_mm_add_epi16(M2_1, M3_1));
    T3_0 = _mm_abs_epi16(_mm_sub_epi16(M2_0, M3_0)); T3_1 = _mm_abs_epi16(_mm_sub_epi16(M2_1, M3_1));
    T4_0 = _mm_abs_epi16(_mm_add_epi16(M4_0, M5_0)); T4_1 = _mm_abs_epi16(_mm_add_epi16(M4_1, M5_1));
    T5_0 = _mm_abs_epi16(_mm_sub_epi16(M4_0, M5_0)); T5_1 = _mm_abs_epi16(_mm_sub_epi16(M4_1, M5_1));
    T6_0 = _mm_abs_epi16(_mm_add_epi16(M6_0, M7_0)); T6_1 = _mm_abs_epi16(_mm_add_epi16(M6_1, M7_1));
    T7_0 = _mm_abs_epi16(_mm_sub_epi16(M6_0, M7_0)); T7_1 = _mm_abs_epi16(_mm_sub_epi16(M6_1, M7_1));
    
    T0_0 = _mm_add_epi16(T0_0, T1_0); T0_1 = _mm_add_epi16(T0_1, T1_1);
    T2_0 = _mm_add_epi16(T2_0, T3_0); T2_1 = _mm_add_epi16(T2_1, T3_1);
    T4_0 = _mm_add_epi16(T4_0, T5_0); T4_1 = _mm_add_epi16(T4_1, T5_1);
    T6_0 = _mm_add_epi16(T6_0, T7_0); T6_1 = _mm_add_epi16(T6_1, T7_1);
    
    T0_0 = _mm_add_epi16(T0_0, T2_0); T0_1 = _mm_add_epi16(T0_1, T2_1);
    T4_0 = _mm_add_epi16(T4_0, T6_0); T4_1 = _mm_add_epi16(T4_1, T6_1);
    
    M0_0 = _mm_cvtepi16_epi32(T0_0); M0_1 = _mm_cvtepi16_epi32(T0_1);
    M2_0 = _mm_cvtepi16_epi32(T4_0); M2_1 = _mm_cvtepi16_epi32(T4_1);
    M4_0 = _mm_unpackhi_epi16(T0_0, zero); M4_1 = _mm_unpackhi_epi16(T0_1, zero);
    M6_0 = _mm_unpackhi_epi16(T4_0, zero); M6_1 = _mm_unpackhi_epi16(T4_1, zero);
    
    M0_0 = _mm_add_epi32(M0_0, M2_0); M0_1 = _mm_add_epi32(M0_1, M2_1);
    M4_0 = _mm_add_epi32(M4_0, M6_0); M4_1 = _mm_add_epi32(M4_1, M6_1);
    
    sum = _mm_add_epi32(M0_0, M4_0);
    sum = _mm_add_epi32(sum, M0_1);
    sum = _mm_add_epi32(sum, M4_1);

    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    uiSum = _mm_cvtsi128_si32(sum);

    uiSum = (u32)(uiSum / com_tbl_sqrt[1] * 2);
    return uiSum;
}

u32 uavs3e_had_8x16_sse(pel *p_org, int i_org, pel *p_pred, int i_pred)
{
    __m128i T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15;
    __m128i M0, M1, M2, M3, M4, M5, M6, M7, M8, M9, M10, M11, M12, M13, M14, M15;
    const __m128i zero = _mm_setzero_si128();
    __m128i sum;
    __m128i sign = _mm_set_epi16(1,1,-1,-1,1,1,-1,-1);

    // horizontal
#define HOR_LINE(i, j) { \
        T0 = _mm_loadl_epi64((__m128i const*)p_org ); \
        T1 = _mm_loadl_epi64((__m128i const*)p_pred); \
        T4 = _mm_loadl_epi64((__m128i const*)(p_org  + i_org )); \
        T5 = _mm_loadl_epi64((__m128i const*)(p_pred + i_pred)); \
        T0 = _mm_cvtepu8_epi16(T0); \
        T1 = _mm_cvtepu8_epi16(T1); \
        T4 = _mm_cvtepu8_epi16(T4); \
        T5 = _mm_cvtepu8_epi16(T5); \
        T0 = _mm_sub_epi16(T0, T1); \
        T4 = _mm_sub_epi16(T4, T5); \
        T1 = _mm_unpacklo_epi64(T0, T4); \
        T5 = _mm_unpackhi_epi64(T0, T4); \
        T2 = _mm_add_epi16(T1, T5); \
        T3 = _mm_sub_epi16(T1, T5); \
        T0 = _mm_unpacklo_epi32(T2, T3); \
        T4 = _mm_unpackhi_epi32(T2, T3); \
        T1 = _mm_unpackhi_epi32(T0, T0); \
        T2 = _mm_unpacklo_epi32(T0, T0); \
        T5 = _mm_unpackhi_epi32(T4, T4); \
        T6 = _mm_unpacklo_epi32(T4, T4); \
        T1 = _mm_sign_epi16(T1, sign); \
        T5 = _mm_sign_epi16(T5, sign); \
        T0 = _mm_add_epi16(T2, T1); \
        T4 = _mm_add_epi16(T6, T5); \
        T1 = _mm_hadd_epi16(T0, T4); \
        T2 = _mm_hsub_epi16(T0, T4); \
        M##i = _mm_unpacklo_epi16(T1, T2); \
        M##j = _mm_unpackhi_epi16(T1, T2); \
        p_pred += (i_pred << 1); \
        p_org  += (i_org  << 1); \
    }

    HOR_LINE(0, 1)
    HOR_LINE(2, 3)
    HOR_LINE(4, 5)
    HOR_LINE(6, 7)
    HOR_LINE(8, 9)
    HOR_LINE(10, 11)
    HOR_LINE(12, 13)
    HOR_LINE(14, 15)

#undef HOR_LINE

    // vertical
    T0  = _mm_add_epi16(M0, M8 );
    T1  = _mm_add_epi16(M1, M9 );
    T2  = _mm_add_epi16(M2, M10);
    T3  = _mm_add_epi16(M3, M11);
    T4  = _mm_add_epi16(M4, M12);
    T5  = _mm_add_epi16(M5, M13);
    T6  = _mm_add_epi16(M6, M14);
    T7  = _mm_add_epi16(M7, M15);
    T8  = _mm_sub_epi16(M0, M8 );
    T9  = _mm_sub_epi16(M1, M9 );
    T10 = _mm_sub_epi16(M2, M10);
    T11 = _mm_sub_epi16(M3, M11);
    T12 = _mm_sub_epi16(M4, M12);
    T13 = _mm_sub_epi16(M5, M13);
    T14 = _mm_sub_epi16(M6, M14);
    T15 = _mm_sub_epi16(M7, M15);

    M0  = _mm_add_epi16(T0,  T4 );
    M1  = _mm_add_epi16(T1,  T5 );
    M2  = _mm_add_epi16(T2,  T6 );
    M3  = _mm_add_epi16(T3,  T7 );
    M4  = _mm_sub_epi16(T0,  T4 );
    M5  = _mm_sub_epi16(T1,  T5 );
    M6  = _mm_sub_epi16(T2,  T6 );
    M7  = _mm_sub_epi16(T3,  T7 );
    M8  = _mm_add_epi16(T8,  T12);
    M9  = _mm_add_epi16(T9,  T13);
    M10 = _mm_add_epi16(T10, T14);
    M11 = _mm_add_epi16(T11, T15);
    M12 = _mm_sub_epi16(T8,  T12);
    M13 = _mm_sub_epi16(T9,  T13);
    M14 = _mm_sub_epi16(T10, T14);
    M15 = _mm_sub_epi16(T11, T15);
    
    T0  = _mm_add_epi16(M0 , M2 );   
    T1  = _mm_add_epi16(M1 , M3 );   
    T2  = _mm_sub_epi16(M0 , M2 );   
    T3  = _mm_sub_epi16(M1 , M3 );   
    T4  = _mm_add_epi16(M4 , M6 );   
    T5  = _mm_add_epi16(M5 , M7 );   
    T6  = _mm_sub_epi16(M4 , M6 );   
    T7  = _mm_sub_epi16(M5 , M7 );   
    T8  = _mm_add_epi16(M8 , M10);   
    T9  = _mm_add_epi16(M9 , M11);   
    T10 = _mm_sub_epi16(M8 , M10);   
    T11 = _mm_sub_epi16(M9 , M11);   
    T12 = _mm_add_epi16(M12, M14);   
    T13 = _mm_add_epi16(M13, M15);   
    T14 = _mm_sub_epi16(M12, M14);   
    T15 = _mm_sub_epi16(M13, M15);   

    M0  = _mm_abs_epi16(_mm_add_epi16(T0 , T1 ));
    M1  = _mm_abs_epi16(_mm_sub_epi16(T0 , T1 ));
    M2  = _mm_abs_epi16(_mm_add_epi16(T2 , T3 ));
    M3  = _mm_abs_epi16(_mm_sub_epi16(T2 , T3 ));
    M4  = _mm_abs_epi16(_mm_add_epi16(T4 , T5 ));
    M5  = _mm_abs_epi16(_mm_sub_epi16(T4 , T5 ));
    M6  = _mm_abs_epi16(_mm_add_epi16(T6 , T7 ));
    M7  = _mm_abs_epi16(_mm_sub_epi16(T6 , T7 ));
    M8  = _mm_abs_epi16(_mm_add_epi16(T8 , T9 ));
    M9  = _mm_abs_epi16(_mm_sub_epi16(T8 , T9 ));
    M10 = _mm_abs_epi16(_mm_add_epi16(T10, T11));
    M11 = _mm_abs_epi16(_mm_sub_epi16(T10, T11));
    M12 = _mm_abs_epi16(_mm_add_epi16(T12, T13));
    M13 = _mm_abs_epi16(_mm_sub_epi16(T12, T13));
    M14 = _mm_abs_epi16(_mm_add_epi16(T14, T15));
    M15 = _mm_abs_epi16(_mm_sub_epi16(T14, T15));

    T0 = _mm_add_epi16(M0, M1);
    T1 = _mm_add_epi16(M2, M3);
    T2 = _mm_add_epi16(M4, M5);
    T3 = _mm_add_epi16(M6, M7);
    T4 = _mm_add_epi16(M8, M9);
    T5 = _mm_add_epi16(M10, M11);
    T6 = _mm_add_epi16(M12, M13);
    T7 = _mm_add_epi16(M14, M15);

    T0 = _mm_add_epi16(T0, T1);
    T2 = _mm_add_epi16(T2, T3);
    T4 = _mm_add_epi16(T4, T5);
    T6 = _mm_add_epi16(T6, T7);

    T0 = _mm_add_epi16(T0, T2);
    T4 = _mm_add_epi16(T4, T6);

    M0 = _mm_cvtepi16_epi32(T0);
    M2 = _mm_cvtepi16_epi32(T4);
    M4 = _mm_unpackhi_epi16(T0, zero);
    M6 = _mm_unpackhi_epi16(T4, zero);

    M0 = _mm_add_epi32(M0, M2);
    M4 = _mm_add_epi32(M4, M6);

    sum = _mm_add_epi32(M0, M4);

    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);

    return (u32)(_mm_cvtsi128_si32(sum) / com_tbl_sqrt[1] * 2.0);
}

u32 uavs3e_had_8x4_sse(pel *p_org, int i_org, pel *p_pred, int i_pred)
{
    __m128i T0, T1, T2, T3, T4, T5, T6;
    __m128i M0, M1, M2, M3;
    __m128i sign = _mm_set_epi16(1, 1, -1, -1, 1, 1, -1, -1);

    // horizontal
#define HOR_LINE(i, j) { \
        T0 = _mm_loadl_epi64((__m128i const*)p_org ); \
        T1 = _mm_loadl_epi64((__m128i const*)p_pred); \
        T4 = _mm_loadl_epi64((__m128i const*)(p_org  + i_org )); \
        T5 = _mm_loadl_epi64((__m128i const*)(p_pred + i_pred)); \
        T0 = _mm_cvtepu8_epi16(T0); \
        T1 = _mm_cvtepu8_epi16(T1); \
        T4 = _mm_cvtepu8_epi16(T4); \
        T5 = _mm_cvtepu8_epi16(T5); \
        T0 = _mm_sub_epi16(T0, T1); \
        T4 = _mm_sub_epi16(T4, T5); \
        T1 = _mm_unpacklo_epi64(T0, T4); \
        T5 = _mm_unpackhi_epi64(T0, T4); \
        T2 = _mm_add_epi16(T1, T5); \
        T3 = _mm_sub_epi16(T1, T5); \
        T0 = _mm_unpacklo_epi32(T2, T3); \
        T4 = _mm_unpackhi_epi32(T2, T3); \
        T1 = _mm_unpackhi_epi32(T0, T0); \
        T2 = _mm_unpacklo_epi32(T0, T0); \
        T5 = _mm_unpackhi_epi32(T4, T4); \
        T6 = _mm_unpacklo_epi32(T4, T4); \
        T1 = _mm_sign_epi16(T1, sign); \
        T5 = _mm_sign_epi16(T5, sign); \
        T0 = _mm_add_epi16(T2, T1); \
        T4 = _mm_add_epi16(T6, T5); \
        T1 = _mm_hadd_epi16(T0, T4); \
        T2 = _mm_hsub_epi16(T0, T4); \
        M##i = _mm_unpacklo_epi16(T1, T2); \
        M##j = _mm_unpackhi_epi16(T1, T2); \
        p_pred += (i_pred << 1); \
        p_org  += (i_org  << 1); \
    }

    HOR_LINE(0, 1)
    HOR_LINE(2, 3)

#undef HOR_LINE

    // vertical
    T0 = _mm_add_epi16(M0, M2);
    T1 = _mm_add_epi16(M1, M3);
    T2 = _mm_sub_epi16(M0, M2);
    T3 = _mm_sub_epi16(M1, M3);
  
    M0 = _mm_abs_epi16(_mm_add_epi16(T0, T1));
    M1 = _mm_abs_epi16(_mm_sub_epi16(T0, T1));
    M2 = _mm_abs_epi16(_mm_add_epi16(T2, T3));
    M3 = _mm_abs_epi16(_mm_sub_epi16(T2, T3));

    T0 = _mm_add_epi16(M0, M1);
    T2 = _mm_add_epi16(M2, M3);
    T0 = _mm_add_epi16(T0, T2);

    M0 = _mm_cvtepi16_epi32(T0);
    M1 = _mm_unpackhi_epi16(T0, _mm_setzero_si128());

    M0 = _mm_add_epi32(M0, M1);

    M0 = _mm_hadd_epi32(M0, M0);
    M0 = _mm_hadd_epi32(M0, M0);

    return (u32)(_mm_cvtsi128_si32(M0) / com_tbl_sqrt[0] * 2.0);
}

u32 uavs3e_had_4x8_sse(pel *org, int s_org, pel *cur, int s_cur)
{
    int k, i;
    __m128i m1[8], m2[8];
    __m128i n1[4][2];
    __m128i n2[4][2];
    __m128i sum;
    int satd = 0;

    for (k = 0; k < 8; k++) {
        __m128i r0 = _mm_cvtepu8_epi16(_mm_loadl_epi64((__m128i *)org));
        __m128i r1 = _mm_cvtepu8_epi16(_mm_loadl_epi64((__m128i *)cur));
        m2[k] = _mm_sub_epi16(r0, r1);
        org += s_org;
        cur += s_cur;
    }
    // vertical
    m1[0] = _mm_add_epi16(m2[0], m2[4]);
    m1[1] = _mm_add_epi16(m2[1], m2[5]);
    m1[2] = _mm_add_epi16(m2[2], m2[6]);
    m1[3] = _mm_add_epi16(m2[3], m2[7]);
    m1[4] = _mm_sub_epi16(m2[0], m2[4]);
    m1[5] = _mm_sub_epi16(m2[1], m2[5]);
    m1[6] = _mm_sub_epi16(m2[2], m2[6]);
    m1[7] = _mm_sub_epi16(m2[3], m2[7]);
    m2[0] = _mm_add_epi16(m1[0], m1[2]);
    m2[1] = _mm_add_epi16(m1[1], m1[3]);
    m2[2] = _mm_sub_epi16(m1[0], m1[2]);
    m2[3] = _mm_sub_epi16(m1[1], m1[3]);
    m2[4] = _mm_add_epi16(m1[4], m1[6]);
    m2[5] = _mm_add_epi16(m1[5], m1[7]);
    m2[6] = _mm_sub_epi16(m1[4], m1[6]);
    m2[7] = _mm_sub_epi16(m1[5], m1[7]);
    m1[0] = _mm_add_epi16(m2[0], m2[1]);
    m1[1] = _mm_sub_epi16(m2[0], m2[1]);
    m1[2] = _mm_add_epi16(m2[2], m2[3]);
    m1[3] = _mm_sub_epi16(m2[2], m2[3]);
    m1[4] = _mm_add_epi16(m2[4], m2[5]);
    m1[5] = _mm_sub_epi16(m2[4], m2[5]);
    m1[6] = _mm_add_epi16(m2[6], m2[7]);
    m1[7] = _mm_sub_epi16(m2[6], m2[7]);
    // horizontal
    // transpose
    m2[0] = _mm_unpacklo_epi16(m1[0], m1[1]);
    m2[1] = _mm_unpacklo_epi16(m1[2], m1[3]);
    m2[2] = _mm_unpacklo_epi16(m1[4], m1[5]);
    m2[3] = _mm_unpacklo_epi16(m1[6], m1[7]);
    m1[0] = _mm_unpacklo_epi32(m2[0], m2[1]);
    m1[1] = _mm_unpackhi_epi32(m2[0], m2[1]);
    m1[2] = _mm_unpacklo_epi32(m2[2], m2[3]);
    m1[3] = _mm_unpackhi_epi32(m2[2], m2[3]);
    m2[0] = _mm_unpacklo_epi64(m1[0], m1[2]);
    m2[1] = _mm_unpackhi_epi64(m1[0], m1[2]);
    m2[2] = _mm_unpacklo_epi64(m1[1], m1[3]);
    m2[3] = _mm_unpackhi_epi64(m1[1], m1[3]);
    for (i = 0; i < 4; i++) {
        n1[i][0] = _mm_cvtepi16_epi32(m2[i]);
        n1[i][1] = _mm_cvtepi16_epi32(_mm_shuffle_epi32(m2[i], 0xEE));
    }
    for (i = 0; i < 2; i++) {
        n2[0][i] = _mm_add_epi32(n1[0][i], n1[2][i]);
        n2[1][i] = _mm_add_epi32(n1[1][i], n1[3][i]);
        n2[2][i] = _mm_sub_epi32(n1[0][i], n1[2][i]);
        n2[3][i] = _mm_sub_epi32(n1[1][i], n1[3][i]);
        n1[0][i] = _mm_abs_epi32(_mm_add_epi32(n2[0][i], n2[1][i]));
        n1[1][i] = _mm_abs_epi32(_mm_sub_epi32(n2[0][i], n2[1][i]));
        n1[2][i] = _mm_abs_epi32(_mm_add_epi32(n2[2][i], n2[3][i]));
        n1[3][i] = _mm_abs_epi32(_mm_sub_epi32(n2[2][i], n2[3][i]));
    }
    for (i = 0; i < 4; i++) {
        m1[i] = _mm_add_epi32(n1[i][0], n1[i][1]);
    }
    m1[0] = _mm_add_epi32(m1[0], m1[1]);
    m1[2] = _mm_add_epi32(m1[2], m1[3]);
    sum = _mm_add_epi32(m1[0], m1[2]);
    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    satd = _mm_cvtsi128_si32(sum);
    satd = (int)(satd / com_tbl_sqrt[0] * 2);
    return satd;
}

#elif (BIT_DEPTH == 10)

u32 uavs3e_had_4x4_sse(pel *org, int s_org, pel *cur, int s_cur)
{
    int satd = 0;
    __m128i sum;
    __m128i r0 = (_mm_loadl_epi64((const __m128i *)&org[0]));
    __m128i r1 = (_mm_loadl_epi64((const __m128i *)&org[s_org]));
    __m128i r2 = (_mm_loadl_epi64((const __m128i *)&org[2 * s_org]));
    __m128i r3 = (_mm_loadl_epi64((const __m128i *)&org[3 * s_org]));
    __m128i r4 = (_mm_loadl_epi64((const __m128i *)&cur[0]));
    __m128i r5 = (_mm_loadl_epi64((const __m128i *)&cur[s_cur]));
    __m128i r6 = (_mm_loadl_epi64((const __m128i *)&cur[2 * s_cur]));
    __m128i r7 = (_mm_loadl_epi64((const __m128i *)&cur[3 * s_cur]));

    r0 = _mm_sub_epi16(r0, r4);
    r1 = _mm_sub_epi16(r1, r5);
    r2 = _mm_sub_epi16(r2, r6);
    r3 = _mm_sub_epi16(r3, r7);

    // first stage
    r4 = r0;
    r5 = r1;
    r0 = _mm_add_epi16(r0, r3);
    r1 = _mm_add_epi16(r1, r2);
    r4 = _mm_sub_epi16(r4, r3);
    r5 = _mm_sub_epi16(r5, r2);
    r2 = r0;
    r3 = r4;
    r0 = _mm_add_epi16(r0, r1);
    r2 = _mm_sub_epi16(r2, r1);
    r3 = _mm_sub_epi16(r3, r5);
    r5 = _mm_add_epi16(r5, r4);
    // shuffle - flip matrix for vertical transform
    r0 = _mm_unpacklo_epi16(r0, r5);
    r2 = _mm_unpacklo_epi16(r2, r3);
    r3 = r0;
    r0 = _mm_unpacklo_epi32(r0, r2);
    r3 = _mm_unpackhi_epi32(r3, r2);
    r1 = r0;
    r2 = r3;
    r1 = _mm_srli_si128(r1, 8);
    r3 = _mm_srli_si128(r3, 8);
    // second stage
    r4 = r0;
    r5 = r1;
    r0 = _mm_add_epi16(r0, r3);
    r1 = _mm_add_epi16(r1, r2);
    r4 = _mm_sub_epi16(r4, r3);
    r5 = _mm_sub_epi16(r5, r2);
    r2 = r0;
    r3 = r4;
    r0 = _mm_add_epi16(r0, r1);
    r2 = _mm_sub_epi16(r2, r1);
    r3 = _mm_sub_epi16(r3, r5);
    r5 = _mm_add_epi16(r5, r4);
    // abs
    sum = _mm_abs_epi16(r0);
    sum = _mm_add_epi16(sum, _mm_abs_epi16(r2));
    sum = _mm_add_epi16(sum, _mm_abs_epi16(r3));
    sum = _mm_add_epi16(sum, _mm_abs_epi16(r5));

    sum = _mm_cvtepu16_epi32(sum);
    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);
    satd = _mm_cvtsi128_si32(sum);
    satd = ((satd + 1) >> 1);
    return satd;
}

#endif