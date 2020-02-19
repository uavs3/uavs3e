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
    sum = _mm256_hadd_epi32(sum, sum);
    sum = _mm256_hadd_epi32(sum, sum);
    return ((u32)_mm256_extract_epi32(sum, 0)) + ((u32)_mm256_extract_epi32(sum, 4));
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
    sum = _mm256_hadd_epi32(sum, sum);
    sum = _mm256_hadd_epi32(sum, sum);
    return ((u32)_mm256_extract_epi32(sum, 0)) + ((u32)_mm256_extract_epi32(sum, 4));
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
    sum = _mm256_hadd_epi32(sum, sum);
    sum = _mm256_hadd_epi32(sum, sum);
    return ((u32)_mm256_extract_epi32(sum, 0)) + ((u32)_mm256_extract_epi32(sum, 4));
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
    sum = _mm256_hadd_epi32(sum, sum);
    sum = _mm256_hadd_epi32(sum, sum);
    return ((u32)_mm256_extract_epi32(sum, 0)) + ((u32)_mm256_extract_epi32(sum, 4));
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
