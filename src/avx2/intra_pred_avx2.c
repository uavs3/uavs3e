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

#if (BIT_DEPTH == 8)
void uavs3e_ipred_ver_avx2(pel *src, pel *dst, int i_dst, int width, int height)
{
    int y;
    switch (width) {
    case 4: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            CP32(dst, src);
            CP32(dst + i_dst, src);
            CP32(dst + i_dst2, src);
            CP32(dst + i_dst3, src);
            dst += i_dst4;
        }
        break;
    }
    case 8:
        for (y = 0; y < height; y += 2) {
            CP64(dst, src);
            CP64(dst + i_dst, src);
            dst += i_dst << 1;
        }
        break;
    case 12:
        for (y = 0; y < height; y += 2) {
            CP64(dst, src);
            CP64(dst + i_dst, src);
            CP32(dst + 8, src + 8);
            CP32(dst + 8 + i_dst, src + 8);
            dst += i_dst << 1;
        }
        break;

    case 16: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        __m128i T0;
        T0 = _mm_loadu_si128((__m128i *)src);
        for (y = 0; y < height; y += 4) {
            _mm_storeu_si128((__m128i *)(dst), T0);
            _mm_storeu_si128((__m128i *)(dst + i_dst), T0);
            _mm_storeu_si128((__m128i *)(dst + i_dst2), T0);
            _mm_storeu_si128((__m128i *)(dst + i_dst3), T0);
            dst += i_dst4;
        }
        break;
    }
    case 24: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        __m128i T0 = _mm_loadu_si128((__m128i *)src);
        for (y = 0; y < height; y++) {
            _mm_storeu_si128((__m128i *)(dst), T0); CP64(dst + 16, src + 16);
            _mm_storeu_si128((__m128i *)(dst + i_dst), T0); CP64(dst + i_dst + 16, src + 16);
            _mm_storeu_si128((__m128i *)(dst + i_dst2), T0); CP64(dst + i_dst2 + 16, src + 16);
            _mm_storeu_si128((__m128i *)(dst + i_dst3), T0); CP64(dst + i_dst3 + 16, src + 16);
            dst += i_dst;
        }
        break;
    }
    case 32: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        __m256i T0;
        T0 = _mm256_loadu_si256((__m256i *)(src));
        for (y = 0; y < height; y += 4) {
            _mm256_storeu_si256((__m256i *)(dst), T0);
            _mm256_storeu_si256((__m256i *)(dst + i_dst), T0);
            _mm256_storeu_si256((__m256i *)(dst + i_dst2), T0);
            _mm256_storeu_si256((__m256i *)(dst + i_dst3), T0);
            dst += i_dst4;
        }
        break;
    }
    case 48: {
        __m128i T1 = _mm_loadu_si128((__m128i *)(src + 0));
        __m128i T2 = _mm_loadu_si128((__m128i *)(src + 16));
        __m128i T3 = _mm_loadu_si128((__m128i *)(src + 32));

        for (y = 0; y < height; y++) {
            _mm_storeu_si128((__m128i *)(dst + 0), T1);
            _mm_storeu_si128((__m128i *)(dst + 16), T2);
            _mm_storeu_si128((__m128i *)(dst + 32), T3);
            dst += i_dst;
        }
        break;
    }
    case 64: {
        __m256i T0, T1;
        int i_dst2 = i_dst << 1;
        T0 = _mm256_loadu_si256((__m256i *)(src));
        T1 = _mm256_loadu_si256((__m256i *)(src + 32));
        for (y = 0; y < height; y += 2) {
            _mm256_storeu_si256((__m256i *)(dst), T0);
            _mm256_storeu_si256((__m256i *)(dst + 32), T1);
            _mm256_storeu_si256((__m256i *)(dst + i_dst), T0);
            _mm256_storeu_si256((__m256i *)(dst + i_dst + 32), T1);
            dst += i_dst2;
        }
        break;
    }
    default:
        com_assert(0);
        break;
    }
}

void uavs3e_ipred_hor_avx2(pel *src, pel *dst, int i_dst, int width, int height)
{
    int y;
    switch (width) {
    case 4: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            M32(dst) = 0x01010101 * src[-y];
            M32(dst + i_dst) = 0x01010101 * src[-y - 1];
            M32(dst + i_dst2) = 0x01010101 * src[-y - 2];
            M32(dst + i_dst3) = 0x01010101 * src[-y - 3];
            dst += i_dst4;
        }
        break;
    }
    case 8: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            M64(dst) = 0x0101010101010101 * src[-y];
            M64(dst + i_dst) = 0x0101010101010101 * src[-y - 1];
            M64(dst + i_dst2) = 0x0101010101010101 * src[-y - 2];
            M64(dst + i_dst3) = 0x0101010101010101 * src[-y - 3];
            dst += i_dst4;
        }
        break;
    }
    case 12: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            M64(dst) = 0x0101010101010101 * src[-y];
            M64(dst + i_dst) = 0x0101010101010101 * src[-y - 1];
            M64(dst + i_dst2) = 0x0101010101010101 * src[-y - 2];
            M64(dst + i_dst3) = 0x0101010101010101 * src[-y - 3];
            M32(dst + 8) = 0x01010101 * src[-y];
            M32(dst + 8 + i_dst) = 0x01010101 * src[-y - 1];
            M32(dst + 8 + i_dst2) = 0x01010101 * src[-y - 2];
            M32(dst + 8 + i_dst3) = 0x01010101 * src[-y - 3];

            dst += i_dst4;
        }
        break;
    }

    case 16: {
        __m128i T0, T1, T2, T3;
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            T0 = _mm_set1_epi8((char)src[-y]);
            T1 = _mm_set1_epi8((char)src[-y - 1]);
            T2 = _mm_set1_epi8((char)src[-y - 2]);
            T3 = _mm_set1_epi8((char)src[-y - 3]);
            _mm_storeu_si128((__m128i *)(dst), T0);
            _mm_storeu_si128((__m128i *)(dst + i_dst), T1);
            _mm_storeu_si128((__m128i *)(dst + i_dst2), T2);
            _mm_storeu_si128((__m128i *)(dst + i_dst3), T3);
            dst += i_dst4;
        }
        break;
    }
    case 24: {
        __m128i T0, T1, T2, T3;
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            T0 = _mm_set1_epi8((char)src[-y]);
            T1 = _mm_set1_epi8((char)src[-y - 1]);
            T2 = _mm_set1_epi8((char)src[-y - 2]);
            T3 = _mm_set1_epi8((char)src[-y - 3]);
            _mm_storeu_si128((__m128i *)(dst), T0);
            _mm_storeu_si128((__m128i *)(dst + i_dst), T1);
            _mm_storeu_si128((__m128i *)(dst + i_dst2), T2);
            _mm_storeu_si128((__m128i *)(dst + i_dst3), T3);
            M64(dst + 16) = _mm_extract_epi64(T0, 0);
            M64(dst + 16 + i_dst) = _mm_extract_epi64(T1, 0);
            M64(dst + 16 + i_dst2) = _mm_extract_epi64(T2, 0);
            M64(dst + 16 + i_dst3) = _mm_extract_epi64(T3, 0);
            dst += i_dst4;
        }
        break;
    }

    case 32: {
        __m256i T0, T1;
        int i_dst2 = i_dst << 1;
        for (y = 0; y < height; y += 2) {
            T0 = _mm256_set1_epi8((char)src[-y]);
            T1 = _mm256_set1_epi8((char)src[-y - 1]);
            _mm256_storeu_si256((__m256i *)(dst), T0);
            _mm256_storeu_si256((__m256i *)(dst + i_dst), T1);
            dst += i_dst2;
        }
        break;
    }
    case 48: {
        __m128i T0, T1;
        int i_dst2 = i_dst << 1;
        for (y = 0; y < height; y += 2) {
            T0 = _mm_set1_epi8((char)src[-y]);
            T1 = _mm_set1_epi8((char)src[-y - 1]);
            _mm_storeu_si128((__m128i *)(dst), T0);
            _mm_storeu_si128((__m128i *)(dst + 16), T0);
            _mm_storeu_si128((__m128i *)(dst + 32), T0);
            _mm_storeu_si128((__m128i *)(dst + i_dst), T1);
            _mm_storeu_si128((__m128i *)(dst + i_dst + 16), T1);
            _mm_storeu_si128((__m128i *)(dst + i_dst + 32), T1);
            dst += i_dst2;
        }
        break;
    }

    case 64: {
        __m256i T0, T1;
        int i_dst2 = i_dst << 1;
        for (y = 0; y < height; y += 2) {
            T0 = _mm256_set1_epi8((char)src[-y]);
            T1 = _mm256_set1_epi8((char)src[-y - 1]);
            _mm256_storeu_si256((__m256i *)(dst), T0);
            _mm256_storeu_si256((__m256i *)(dst + 32), T0);
            _mm256_storeu_si256((__m256i *)(dst + i_dst), T1);
            _mm256_storeu_si256((__m256i *)(dst + i_dst + 32), T1);
            dst += i_dst2;
        }
        break;
    }
    default:
        com_assert(0);
        break;
    }
}

void uavs3e_ipred_dc_avx2(pel *src, pel *dst, int i_dst, int width, int height, u16 avail_cu, int bit_depth)
{
    int   i, x, y;
    int   dc;
    pel  *p_src = src - 1;
    int left_avail = IS_AVAIL(avail_cu, AVAIL_LE);
    int above_avail = IS_AVAIL(avail_cu, AVAIL_UP);

    if (left_avail && above_avail) {
        int length = width + height + 1;
        __m128i sum = _mm_setzero_si128();
        __m128i val;

        p_src = src - height;

        for (i = 0; i < length - 7; i += 8) {
            val = _mm_cvtepu8_epi16(_mm_loadl_epi64((__m128i *)(p_src + i)));
            sum = _mm_add_epi16(sum, val);
        }
        if (i < length) {
            int left_pixels = length - i;
            __m128i mask = _mm_load_si128((__m128i *)(uavs3e_simd_mask[(left_pixels << 1) - 1]));
            val = _mm_cvtepu8_epi16(_mm_loadl_epi64((__m128i *)(p_src + i)));
            val = _mm_and_si128(val, mask);
            sum = _mm_add_epi16(sum, val);
        }
        sum = _mm_add_epi16(sum, _mm_srli_si128(sum, 8));
        sum = _mm_add_epi16(sum, _mm_srli_si128(sum, 4));
        sum = _mm_add_epi16(sum, _mm_srli_si128(sum, 2));

        dc = _mm_extract_epi16(sum, 0) + ((width + height) >> 1) - src[0];

        dc = (dc * (4096 / (width + height))) >> 12;

    } else if (left_avail) {
        dc = 0;
        for (y = 0; y < height; y++) {
            dc += p_src[-y];
        }
        dc += height / 2;
        dc /= height;
    } else {
        p_src = src + 1;
        dc = 0;
        if (above_avail) {
            for (x = 0; x < width; x++) {
                dc += p_src[x];
            }
            dc += width / 2;
            dc /= width;
        } else {
            dc = 1 << (bit_depth - 1);
        }
    }

    switch (width) {
    case 4: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        u32 v32 = 0x01010101 * dc;
        for (y = 0; y < height; y += 4) {
            M32(dst) = v32;
            M32(dst + i_dst) = v32;
            M32(dst + i_dst2) = v32;
            M32(dst + i_dst3) = v32;
            dst += i_dst4;
        }
        break;
    }
    case 8: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        u64 v64 = 0x0101010101010101 * dc;
        for (y = 0; y < height; y += 4) {
            M64(dst) = v64;
            M64(dst + i_dst) = v64;
            M64(dst + i_dst2) = v64;
            M64(dst + i_dst3) = v64;
            dst += i_dst4;
        }
        break;
    }
    case 16: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        __m128i T = _mm_set1_epi8((s8)dc);
        for (y = 0; y < height; y += 4) {
            _mm_storeu_si128((__m128i *)(dst), T);
            _mm_storeu_si128((__m128i *)(dst + i_dst), T);
            _mm_storeu_si128((__m128i *)(dst + i_dst2), T);
            _mm_storeu_si128((__m128i *)(dst + i_dst3), T);
            dst += i_dst4;
        }
        break;
    }
    case 32: {
        __m256i T = _mm256_set1_epi8((s8)dc);
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            _mm256_storeu_si256((__m256i *)(dst), T);
            _mm256_storeu_si256((__m256i *)(dst + i_dst), T);
            _mm256_storeu_si256((__m256i *)(dst + i_dst2), T);
            _mm256_storeu_si256((__m256i *)(dst + i_dst3), T);
            dst += i_dst4;
        }
        break;
    }
    case 64: {
        int i_dst2 = i_dst << 1;
        __m256i T = _mm256_set1_epi8((s8)dc);
        for (y = 0; y < height; y += 2) {
            _mm256_storeu_si256((__m256i *)(dst), T);
            _mm256_storeu_si256((__m256i *)(dst + 32), T);
            _mm256_storeu_si256((__m256i *)(dst + i_dst), T);
            _mm256_storeu_si256((__m256i *)(dst + i_dst + 32), T);
            dst += i_dst2;
        }
        break;
    }
    default:
        com_assert(0);
        break;
    }
}

void uavs3e_ipred_ang_x_avx2(pel *pSrc, pel *dst, int i_dst, int mode, int width, int height)
{
    int offset;
    __m128i mAddOffset = _mm_set1_epi16(64);

    if (width == 4) {
        __m256i mSwitch = _mm256_setr_epi8(0, 1, 2, 3, 1, 2, 3, 4, 2, 3, 4, 5, 3, 4, 5, 6, 0, 1, 2, 3, 1, 2, 3, 4, 2, 3, 4, 5, 3, 4, 5, 6);
        int j;

        for (j = 0; j < height; j += 2) {
            int offset2;
            pel *p0 = pSrc + getContextPixel(mode, 0, j + 1, &offset);
            pel *p1 = pSrc + getContextPixel(mode, 0, j + 2, &offset2);
            int c0 = 32 - offset;
            int c1 = 64 - offset;
            int c2 = 32 + offset;
            int c3 = offset;
            int c4 = 32 - offset2;
            int c5 = 64 - offset2;
            int c6 = 32 + offset2;
            int c7 = offset2;
            __m256i P = _mm256_set_m128i(_mm_loadl_epi64((__m128i*)p1), _mm_loadl_epi64((__m128i*)p0));
            __m256i C = _mm256_setr_epi8(c0, c1, c2, c3, c0, c1, c2, c3, c0, c1, c2, c3, c0, c1, c2, c3, c4, c5, c6, c7, c4, c5, c6, c7, c4, c5, c6, c7, c4, c5, c6, c7);
            __m256i T = _mm256_maddubs_epi16(_mm256_shuffle_epi8(P, mSwitch), C);
            __m128i m0, m1;

            m0 = _mm256_castsi256_si128(T);
            m1 = _mm256_extracti128_si256(T, 1);

            m0 = _mm_hadd_epi16(m0, m1);
            m0 = _mm_add_epi16(m0, mAddOffset);
            m0 = _mm_srai_epi16(m0, 7);
            m0 = _mm_packus_epi16(m0, m0);
            *(int*)dst = _mm_extract_epi32(m0, 0);
            dst += i_dst;
            *(int*)dst = _mm_extract_epi32(m0, 1);
            dst += i_dst;
        }
    }
    else if (width == 8) {
        __m256i off = _mm256_set1_epi16(64);
        __m256i mSwitch0 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
        __m256i mSwitch1 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
        int j;
        for (j = 0; j < height; j += 2) {
            int offset2;
            pel *p0 = pSrc + getContextPixel(mode, 0, j + 1, &offset);
            pel *p1 = pSrc + getContextPixel(mode, 0, j + 2, &offset2);
            int coef0 = ((64 - offset) << 8) | (32 - offset);
            int coef1 = (offset << 8) | (32 + offset);
            int coef2 = ((64 - offset2) << 8) | (32 - offset2);
            int coef3 = (offset2 << 8) | (32 + offset2);
            __m256i C0 = _mm256_set_epi16(coef2, coef2, coef2, coef2, coef2, coef2, coef2, coef2, coef0, coef0, coef0, coef0, coef0, coef0, coef0, coef0);
            __m256i C1 = _mm256_set_epi16(coef3, coef3, coef3, coef3, coef3, coef3, coef3, coef3, coef1, coef1, coef1, coef1, coef1, coef1, coef1, coef1);

            __m256i mSrc0;
            __m256i T0, T1;
            __m128i m0, m1;

            mSrc0 = _mm256_set_m128i(_mm_loadu_si128((__m128i*)(p1)), _mm_loadu_si128((__m128i*)(p0)));

            T0 = _mm256_shuffle_epi8(mSrc0, mSwitch0);
            T1 = _mm256_shuffle_epi8(mSrc0, mSwitch1);
            T0 = _mm256_maddubs_epi16(T0, C0);
            T1 = _mm256_maddubs_epi16(T1, C1);

            T0 = _mm256_add_epi16(T0, T1);
            T0 = _mm256_add_epi16(T0, off);
            T0 = _mm256_srai_epi16(T0, 7);

            m0 = _mm256_castsi256_si128(T0);
            m1 = _mm256_extracti128_si256(T0, 1);

            m0 = _mm_packus_epi16(m0, m1);

            _mm_storel_epi64((__m128i*)dst, m0);
            _mm_storel_epi64((__m128i*)(dst + i_dst), _mm_srli_si128(m0, 8));
            dst += i_dst << 1;
        }
    }
    else if (width == 12) {
        __m256i off = _mm256_set1_epi16(64);
        __m256i mSwitch0 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
        __m256i mSwitch1 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
        int j;
        for (j = 0; j < height; j += 2) {
            int offset2;
            pel *p0 = pSrc + getContextPixel(mode, 0, j + 1, &offset);
            pel *p1 = pSrc + getContextPixel(mode, 0, j + 2, &offset2);
            int coef0 = ((64 - offset) << 8) | (32 - offset);
            int coef1 = (offset << 8) | (32 + offset);
            int coef2 = ((64 - offset2) << 8) | (32 - offset2);
            int coef3 = (offset2 << 8) | (32 + offset2);
            __m256i C0 = _mm256_set_epi16(coef2, coef2, coef2, coef2, coef2, coef2, coef2, coef2, coef0, coef0, coef0, coef0, coef0, coef0, coef0, coef0);
            __m256i C1 = _mm256_set_epi16(coef3, coef3, coef3, coef3, coef3, coef3, coef3, coef3, coef1, coef1, coef1, coef1, coef1, coef1, coef1, coef1);

            __m256i mSrc0, mSrc1;
            __m256i T0, T1, T2, T3;
            __m128i m0, m1;

            mSrc0 = _mm256_set_m128i(_mm_loadu_si128((__m128i*)(p1)), _mm_loadu_si128((__m128i*)(p0)));
            mSrc1 = _mm256_set_m128i(_mm_loadl_epi64((__m128i*)(p1 + 8)), _mm_loadl_epi64((__m128i*)(p0 + 8)));

            T0 = _mm256_shuffle_epi8(mSrc0, mSwitch0);
            T1 = _mm256_shuffle_epi8(mSrc0, mSwitch1);
            T2 = _mm256_shuffle_epi8(mSrc1, mSwitch0);
            T3 = _mm256_shuffle_epi8(mSrc1, mSwitch1);
            T0 = _mm256_maddubs_epi16(T0, C0);
            T1 = _mm256_maddubs_epi16(T1, C1);
            T2 = _mm256_maddubs_epi16(T2, C0);
            T3 = _mm256_maddubs_epi16(T3, C1);

            T0 = _mm256_add_epi16(T0, T1);
            T2 = _mm256_add_epi16(T2, T3);
            T0 = _mm256_add_epi16(T0, off);
            T2 = _mm256_add_epi16(T2, off);
            T0 = _mm256_srai_epi16(T0, 7);
            T2 = _mm256_srai_epi16(T2, 7);
            T0 = _mm256_packus_epi16(T0, T2);
            T0 = _mm256_permute4x64_epi64(T0, 0xd8);

            m0 = _mm256_castsi256_si128(T0);
            m1 = _mm256_extracti128_si256(T0, 1);

            _mm_storel_epi64((__m128i*)dst, m0);
            *(int*)(dst + 8) = _mm_extract_epi32(m1, 0);
            m0 = _mm_srli_si128(m0, 8);
            dst += i_dst;
            _mm_storel_epi64((__m128i*)(dst), m0);
            *(int*)(dst + 8) = _mm_extract_epi32(m1, 2);
            dst += i_dst;
        }

    }
    else if (width == 24) {
        __m256i off = _mm256_set1_epi16(64);
        __m256i mSwitch0 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
        __m256i mSwitch1 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
        int j;
        for (j = 0; j < height; j++) {
            pel *p0 = pSrc + getContextPixel(mode, 0, j + 1, &offset);
            int coef0 = ((64 - offset) << 8) | (32 - offset);
            int coef1 = (offset << 8) | (32 + offset);
            __m256i C0 = _mm256_set1_epi16(coef0);
            __m256i C1 = _mm256_set1_epi16(coef1);
            __m256i mSrc0, mSrc1;
            __m256i T0, T1, T2, T3;

            mSrc0 = _mm256_set_m128i(_mm_loadu_si128((__m128i*)(p0 + 8)), _mm_loadu_si128((__m128i*)(p0)));
            mSrc1 = _mm256_set_m128i(_mm_setzero_si128(), _mm_loadu_si128((__m128i*)(p0 + 16)));

            T0 = _mm256_shuffle_epi8(mSrc0, mSwitch0);
            T1 = _mm256_shuffle_epi8(mSrc0, mSwitch1);
            T2 = _mm256_shuffle_epi8(mSrc1, mSwitch0);
            T3 = _mm256_shuffle_epi8(mSrc1, mSwitch1);
            T0 = _mm256_maddubs_epi16(T0, C0);
            T1 = _mm256_maddubs_epi16(T1, C1);
            T2 = _mm256_maddubs_epi16(T2, C0);
            T3 = _mm256_maddubs_epi16(T3, C1);

            T0 = _mm256_add_epi16(T0, T1);
            T2 = _mm256_add_epi16(T2, T3);
            T0 = _mm256_add_epi16(T0, off);
            T2 = _mm256_add_epi16(T2, off);
            T0 = _mm256_srai_epi16(T0, 7);
            T2 = _mm256_srai_epi16(T2, 7);
            T0 = _mm256_packus_epi16(T0, T2);
            T0 = _mm256_permute4x64_epi64(T0, 0xd8);

            _mm_storeu_si128((__m128i*)dst, _mm256_castsi256_si128(T0));
            _mm_storel_epi64((__m128i*)(dst + 16), _mm256_extracti128_si256(T0, 1));
            dst += i_dst;
        }
    }
    else { // x 16
        __m256i off = _mm256_set1_epi16(64);
        __m256i mSwitch0 = _mm256_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
        __m256i mSwitch1 = _mm256_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
        int j, col;
        for (j = 0; j < height; j++) {
            pel *p = pSrc + getContextPixel(mode, 0, j + 1, &offset);
            int coef0 = ((64 - offset) << 8) | (32 - offset);
            int coef1 = (offset << 8) | (32 + offset);
            __m256i C0 = _mm256_set1_epi16(coef0);
            __m256i C1 = _mm256_set1_epi16(coef1);
            __m128i m0, m1;

            for (col = 0; col < width; col += 16) {
                __m256i mSrc0 = _mm256_set_m128i(_mm_loadu_si128((__m128i*)(p + col + 8)), _mm_loadu_si128((__m128i*)(p + col)));
                __m256i T0, T1;

                T0 = _mm256_shuffle_epi8(mSrc0, mSwitch0);
                T1 = _mm256_shuffle_epi8(mSrc0, mSwitch1);
                T0 = _mm256_maddubs_epi16(T0, C0);
                T1 = _mm256_maddubs_epi16(T1, C1);
                T0 = _mm256_add_epi16(T0, T1);
                T0 = _mm256_add_epi16(T0, off);
                T0 = _mm256_srai_epi16(T0, 7);
                m0 = _mm256_castsi256_si128(T0);
                m1 = _mm256_extracti128_si256(T0, 1);
                m0 = _mm_packus_epi16(m0, m1);

                _mm_storeu_si128((__m128i*)(dst + col), m0);
            }
            dst += i_dst;
        }
    }
}

void uavs3e_ipred_ang_x_4_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height)
{
    ALIGNED_32(pel first_line[64 + 128 + 32]);
    int line_size = width + (height - 1) * 2;
    int real_size = COM_MIN(line_size, width * 2 - 1);
    int height2 = height * 2;
    int i;
    __m256i zero = _mm256_setzero_si256();
    __m256i offset = _mm256_set1_epi16(2);

    src += 3;

    for (i = 0; i < real_size - 16; i += 32, src += 32) {
        __m256i S0 = _mm256_loadu_si256((__m256i *)(src - 1));
        __m256i S2 = _mm256_loadu_si256((__m256i *)(src + 1));
        __m256i S1 = _mm256_loadu_si256((__m256i *)(src));

        __m256i L0 = _mm256_unpacklo_epi8(S0, zero);
        __m256i L1 = _mm256_unpacklo_epi8(S1, zero);
        __m256i L2 = _mm256_unpacklo_epi8(S2, zero);
        __m256i H0 = _mm256_unpackhi_epi8(S0, zero);
        __m256i H1 = _mm256_unpackhi_epi8(S1, zero);
        __m256i H2 = _mm256_unpackhi_epi8(S2, zero);

        __m256i sum1 = _mm256_add_epi16(L0, L1);
        __m256i sum2 = _mm256_add_epi16(L1, L2);
        __m256i sum3 = _mm256_add_epi16(H0, H1);
        __m256i sum4 = _mm256_add_epi16(H1, H2);

        sum1 = _mm256_add_epi16(sum1, sum2);
        sum3 = _mm256_add_epi16(sum3, sum4);

        sum1 = _mm256_add_epi16(sum1, offset);
        sum3 = _mm256_add_epi16(sum3, offset);

        sum1 = _mm256_srli_epi16(sum1, 2);
        sum3 = _mm256_srli_epi16(sum3, 2);

        sum1 = _mm256_packus_epi16(sum1, sum3);

        _mm256_storeu_si256((__m256i *)&first_line[i], sum1);
    }

    if (i < real_size) {
        __m128i z = _mm_setzero_si128();
        __m128i off = _mm_set1_epi16(2);
        __m128i S0 = _mm_loadu_si128((__m128i *)(src - 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src));

        __m128i L0 = _mm_unpacklo_epi8(S0, z);
        __m128i L1 = _mm_unpacklo_epi8(S1, z);
        __m128i L2 = _mm_unpacklo_epi8(S2, z);
        __m128i H0 = _mm_unpackhi_epi8(S0, z);
        __m128i H1 = _mm_unpackhi_epi8(S1, z);
        __m128i H2 = _mm_unpackhi_epi8(S2, z);

        __m128i sum1 = _mm_add_epi16(L0, L1);
        __m128i sum2 = _mm_add_epi16(L1, L2);
        __m128i sum3 = _mm_add_epi16(H0, H1);
        __m128i sum4 = _mm_add_epi16(H1, H2);

        sum1 = _mm_add_epi16(sum1, sum2);
        sum3 = _mm_add_epi16(sum3, sum4);

        sum1 = _mm_add_epi16(sum1, off);
        sum3 = _mm_add_epi16(sum3, off);

        sum1 = _mm_srli_epi16(sum1, 2);
        sum3 = _mm_srli_epi16(sum3, 2);

        sum1 = _mm_packus_epi16(sum1, sum3);

        _mm_storeu_si128((__m128i *)&first_line[i], sum1);
    }

    // padding
    for (i = real_size; i < line_size; i += 32) {
        __m256i pad = _mm256_set1_epi8((char)first_line[real_size - 1]);
        _mm256_storeu_si256((__m256i *)&first_line[i], pad);
    }

    switch (width) {
    case 4: {
        __m128i m0, m1;
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        for (i = 0; i < height2; i += 8) {
            m0 = _mm_loadu_si128((const __m128i*)(first_line + i));
            m1 = _mm_srli_si128(m0, 2);
            *((s32*)dst) = _mm_extract_epi32(m0, 0);
            *((s32*)(dst + i_dst)) = _mm_extract_epi32(m1, 0);
            *((s32*)(dst + i_dst2)) = _mm_extract_epi32(m0, 1);
            *((s32*)(dst + i_dst3)) = _mm_extract_epi32(m1, 1);

            dst += i_dst << 2;
        }
            break;
    }
    case 8: {
        __m128i m0, m1, m2, m3;
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        for (i = 0; i < height2; i += 8) {
            m0 = _mm_loadu_si128((const __m128i*)(first_line + i));
            m1 = _mm_srli_si128(m0, 2);
            m2 = _mm_srli_si128(m0, 4);
            m3 = _mm_srli_si128(m0, 6);
            _mm_storel_epi64((__m128i*)(dst), m0);
            _mm_storel_epi64((__m128i*)(dst + i_dst), m1);
            _mm_storel_epi64((__m128i*)(dst + i_dst2), m2);
            _mm_storel_epi64((__m128i*)(dst + i_dst3), m3);

            dst += i_dst << 2;
        }
        break;
    }
    case 12: {
        __m128i m0, m1;
        __m128i mask = _mm_set_epi32(0, -1, -1, -1);
        int i_dst2 = i_dst << 1;
        for (i = 0; i < height2; i += 4) {
            m0 = _mm_loadu_si128((const __m128i*)(first_line + i));
            m1 = _mm_srli_si128(m0, 2);
            _mm_maskmoveu_si128(m0, mask, (char*)(dst));
            _mm_maskmoveu_si128(m1, mask, (char*)(dst + i_dst));

            dst += i_dst2;
        }
        break;
    }
    case 16: {
        __m128i m0, m1, m2, m3;
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        pel* psrc = first_line;
        for (i = 0; i < height2; i += 8) {
            m0 = _mm_loadu_si128((const __m128i*)(psrc));
            m1 = _mm_loadu_si128((const __m128i*)(psrc + 2));
            m2 = _mm_loadu_si128((const __m128i*)(psrc + 4));
            m3 = _mm_loadu_si128((const __m128i*)(psrc + 6));
            _mm_storeu_si128((__m128i*)(dst), m0);
            _mm_storeu_si128((__m128i*)(dst + i_dst), m1);
            _mm_storeu_si128((__m128i*)(dst + i_dst2), m2);
            _mm_storeu_si128((__m128i*)(dst + i_dst3), m3);
            psrc += 8;
            dst += i_dst << 2;
        }
        break;
    }
    case 24: {
        __m128i m0, m1, m2, m3;
        int i_dst2 = i_dst << 1;
        pel* psrc = first_line;
        for (i = 0; i < height2; i += 4) {
            m0 = _mm_loadu_si128((const __m128i*)(psrc));
            m1 = _mm_loadu_si128((const __m128i*)(psrc + 16));
            m2 = _mm_loadu_si128((const __m128i*)(psrc + 2));
            m3 = _mm_loadu_si128((const __m128i*)(psrc + 18));
            _mm_storeu_si128((__m128i*)(dst), m0);
            _mm_storel_epi64((__m128i*)(dst + 16), m1);
            _mm_storeu_si128((__m128i*)(dst + i_dst), m2);
            _mm_storel_epi64((__m128i*)(dst + i_dst + 16), m3);
            psrc += 4;
            dst += i_dst2;
        }
        break;
    }
    case 32: {
        __m256i m0, m1, m2, m3;
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        pel* psrc = first_line;
        for (i = 0; i < height2; i += 8) {
            m0 = _mm256_loadu_si256((const __m256i*)(psrc));
            m1 = _mm256_loadu_si256((const __m256i*)(psrc + 2));
            m2 = _mm256_loadu_si256((const __m256i*)(psrc + 4));
            m3 = _mm256_loadu_si256((const __m256i*)(psrc + 6));
            _mm256_storeu_si256((__m256i*)(dst), m0);
            _mm256_storeu_si256((__m256i*)(dst + i_dst), m1);
            _mm256_storeu_si256((__m256i*)(dst + i_dst2), m2);
            _mm256_storeu_si256((__m256i*)(dst + i_dst3), m3);
            psrc += 8;
            dst += i_dst << 2;
        }
        break;
    }
    case 48: {
        __m256i m0, m1, m2, m3;
        __m128i s0, s1, s2, s3;
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        pel* psrc = first_line;
        for (i = 0; i < height2; i += 8) {
            m0 = _mm256_loadu_si256((const __m256i*)(psrc));
            m1 = _mm256_loadu_si256((const __m256i*)(psrc + 2));
            m2 = _mm256_loadu_si256((const __m256i*)(psrc + 4));
            m3 = _mm256_loadu_si256((const __m256i*)(psrc + 6));
            s0 = _mm_loadu_si128((const __m128i*)(psrc + 32));
            s1 = _mm_loadu_si128((const __m128i*)(psrc + 34));
            s2 = _mm_loadu_si128((const __m128i*)(psrc + 36));
            s3 = _mm_loadu_si128((const __m128i*)(psrc + 38));
            _mm256_storeu_si256((__m256i*)(dst), m0);
            _mm_storeu_si128((__m128i*)(dst + 32), s0);
            _mm256_storeu_si256((__m256i*)(dst + i_dst), m1);
            _mm_storeu_si128((__m128i*)(dst + 32 + i_dst), s1);
            _mm256_storeu_si256((__m256i*)(dst + i_dst2), m2);
            _mm_storeu_si128((__m128i*)(dst + 32 + i_dst2), s2);
            _mm256_storeu_si256((__m256i*)(dst + i_dst3), m3);
            _mm_storeu_si128((__m128i*)(dst + 32 + i_dst3), s3);
            psrc += 8;
            dst += i_dst << 2;
        }
        break;
    }
    case 64: {
        __m256i m0, m1, m2, m3, m4, m5, m6, m7;
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        pel* psrc = first_line;
        for (i = 0; i < height2; i += 8) {
            m0 = _mm256_loadu_si256((const __m256i*)(psrc));
            m1 = _mm256_loadu_si256((const __m256i*)(psrc + 2));
            m2 = _mm256_loadu_si256((const __m256i*)(psrc + 4));
            m3 = _mm256_loadu_si256((const __m256i*)(psrc + 6));
            m4 = _mm256_loadu_si256((const __m256i*)(psrc + 32));
            m5 = _mm256_loadu_si256((const __m256i*)(psrc + 34));
            m6 = _mm256_loadu_si256((const __m256i*)(psrc + 36));
            m7 = _mm256_loadu_si256((const __m256i*)(psrc + 38));
            _mm256_storeu_si256((__m256i*)(dst), m0);
            _mm256_storeu_si256((__m256i*)(dst + 32), m4);
            _mm256_storeu_si256((__m256i*)(dst + i_dst), m1);
            _mm256_storeu_si256((__m256i*)(dst + 32 + i_dst), m5);
            _mm256_storeu_si256((__m256i*)(dst + i_dst2), m2);
            _mm256_storeu_si256((__m256i*)(dst + 32 + i_dst2), m6);
            _mm256_storeu_si256((__m256i*)(dst + i_dst3), m3);
            _mm256_storeu_si256((__m256i*)(dst + 32 + i_dst3), m7);
            psrc += 8;
            dst += i_dst << 2;
        }
        break;
    }
    }
}

void uavs3e_ipred_ang_x_8_avx2(pel *src, pel *dst, int i_dst, int mode, int width, int height)
{
    ALIGNED_32(pel first_line[2 * (64 + 48 + 32)]);
    int line_size = width + height / 2 - 1;
    int real_size = COM_MIN(line_size, width * 2 + 1);
    int i;
    int aligned_line_size = ((line_size + 31) >> 4) << 4;
    pel *pfirst[2];

    __m256i zero = _mm256_setzero_si256();
    __m256i coeff = _mm256_set1_epi16(3);
    __m256i offset1 = _mm256_set1_epi16(4);
    __m256i offset2 = _mm256_set1_epi16(2);
    int i_dst2 = i_dst * 2;

    pfirst[0] = first_line;
    pfirst[1] = first_line + aligned_line_size;

    for (i = 0; i < real_size - 16; i += 32, src += 32) {
        __m256i p01, p02, p11, p12;
        __m256i S0 = _mm256_loadu_si256((__m256i *)(src));
        __m256i S3 = _mm256_loadu_si256((__m256i *)(src + 3));
        __m256i S1 = _mm256_loadu_si256((__m256i *)(src + 1));
        __m256i S2 = _mm256_loadu_si256((__m256i *)(src + 2));

        __m256i L0 = _mm256_unpacklo_epi8(S0, zero);
        __m256i L1 = _mm256_unpacklo_epi8(S1, zero);
        __m256i L2 = _mm256_unpacklo_epi8(S2, zero);
        __m256i L3 = _mm256_unpacklo_epi8(S3, zero);
        __m256i H0 = _mm256_unpackhi_epi8(S0, zero);
        __m256i H1 = _mm256_unpackhi_epi8(S1, zero);
        __m256i H2 = _mm256_unpackhi_epi8(S2, zero);
        __m256i H3 = _mm256_unpackhi_epi8(S3, zero);

        p01 = _mm256_add_epi16(L1, L2);
        p11 = _mm256_add_epi16(H1, H2);
        p01 = _mm256_mullo_epi16(p01, coeff);
        p11 = _mm256_mullo_epi16(p11, coeff);
        p02 = _mm256_add_epi16(L0, L3);
        p12 = _mm256_add_epi16(H0, H3);
        p02 = _mm256_add_epi16(p02, offset1);
        p12 = _mm256_add_epi16(p12, offset1);
        p01 = _mm256_add_epi16(p01, p02);
        p11 = _mm256_add_epi16(p11, p12);
        p01 = _mm256_srli_epi16(p01, 3);
        p11 = _mm256_srli_epi16(p11, 3);

        p01 = _mm256_packus_epi16(p01, p11);
        _mm256_storeu_si256((__m256i *)&pfirst[0][i], p01);

        p01 = _mm256_add_epi16(L1, L2);
        p02 = _mm256_add_epi16(L2, L3);
        p11 = _mm256_add_epi16(H1, H2);
        p12 = _mm256_add_epi16(H2, H3);

        p01 = _mm256_add_epi16(p01, p02);
        p11 = _mm256_add_epi16(p11, p12);

        p01 = _mm256_add_epi16(p01, offset2);
        p11 = _mm256_add_epi16(p11, offset2);

        p01 = _mm256_srli_epi16(p01, 2);
        p11 = _mm256_srli_epi16(p11, 2);

        p01 = _mm256_packus_epi16(p01, p11);
        _mm256_storeu_si256((__m256i *)&pfirst[1][i], p01);
    }

    if (i < real_size) {
        __m128i p01, p02, p11, p12;
        __m128i z = _mm_setzero_si128();
        __m128i coef = _mm_set1_epi16(3);
        __m128i off1 = _mm_set1_epi16(4);
        __m128i off2 = _mm_set1_epi16(2);
        __m128i S0 = _mm_loadu_si128((__m128i *)(src));
        __m128i S3 = _mm_loadu_si128((__m128i *)(src + 3));
        __m128i S1 = _mm_loadu_si128((__m128i *)(src + 1));
        __m128i S2 = _mm_loadu_si128((__m128i *)(src + 2));

        __m128i L0 = _mm_unpacklo_epi8(S0, z);
        __m128i L1 = _mm_unpacklo_epi8(S1, z);
        __m128i L2 = _mm_unpacklo_epi8(S2, z);
        __m128i L3 = _mm_unpacklo_epi8(S3, z);
        __m128i H0 = _mm_unpackhi_epi8(S0, z);
        __m128i H1 = _mm_unpackhi_epi8(S1, z);
        __m128i H2 = _mm_unpackhi_epi8(S2, z);
        __m128i H3 = _mm_unpackhi_epi8(S3, z);

        p01 = _mm_add_epi16(L1, L2);
        p11 = _mm_add_epi16(H1, H2);
        p01 = _mm_mullo_epi16(p01, coef);
        p11 = _mm_mullo_epi16(p11, coef);
        p02 = _mm_add_epi16(L0, L3);
        p12 = _mm_add_epi16(H0, H3);
        p02 = _mm_add_epi16(p02, off1);
        p12 = _mm_add_epi16(p12, off1);
        p01 = _mm_add_epi16(p01, p02);
        p11 = _mm_add_epi16(p11, p12);
        p01 = _mm_srli_epi16(p01, 3);
        p11 = _mm_srli_epi16(p11, 3);

        p01 = _mm_packus_epi16(p01, p11);
        _mm_storeu_si128((__m128i *)&pfirst[0][i], p01);

        p01 = _mm_add_epi16(L1, L2);
        p02 = _mm_add_epi16(L2, L3);
        p11 = _mm_add_epi16(H1, H2);
        p12 = _mm_add_epi16(H2, H3);

        p01 = _mm_add_epi16(p01, p02);
        p11 = _mm_add_epi16(p11, p12);

        p01 = _mm_add_epi16(p01, off2);
        p11 = _mm_add_epi16(p11, off2);

        p01 = _mm_srli_epi16(p01, 2);
        p11 = _mm_srli_epi16(p11, 2);

        p01 = _mm_packus_epi16(p01, p11);
        _mm_storeu_si128((__m128i *)&pfirst[1][i], p01);
    }

    // padding
    if (real_size < line_size) {
        __m128i pad1, pad2;
        pfirst[1][real_size - 1] = pfirst[1][real_size - 2];

        pad1 = _mm_set1_epi8((char)pfirst[0][real_size - 1]);
        pad2 = _mm_set1_epi8((char)pfirst[1][real_size - 1]);
        for (i = real_size; i < line_size; i += 16) {
            _mm_storeu_si128((__m128i *)&pfirst[0][i], pad1);
            _mm_storeu_si128((__m128i *)&pfirst[1][i], pad2);
        }
    }

    height /= 2;

    switch (width) {
    case 4: {
        int i_dst3 = i_dst + i_dst2;
        for (i = 0; i < height; i += 2) {
            *((s32*)dst) = *((s32*)(pfirst[0] + i));
            *((s32*)(dst + i_dst)) = *((s32*)(pfirst[1] + i));
            *((s32*)(dst + i_dst2)) = *((s32*)(pfirst[0] + i + 1));
            *((s32*)(dst + i_dst3)) = *((s32*)(pfirst[1] + i + 1));

            dst += i_dst << 2;
        }
        break;
    }
    case 8: {
        __m128i m0, m1, m2, m3;
        int i_dst3 = i_dst + i_dst2;
        for (i = 0; i < height; i += 2) {
            m0 = _mm_loadu_si128((const __m128i*)(pfirst[0] + i));
            m1 = _mm_loadu_si128((const __m128i*)(pfirst[1] + i));
            m2 = _mm_srli_si128(m0, 1);
            m3 = _mm_srli_si128(m1, 1);
            _mm_storel_epi64((__m128i*)(dst), m0);
            _mm_storel_epi64((__m128i*)(dst + i_dst), m1);
            _mm_storel_epi64((__m128i*)(dst + i_dst2), m2);
            _mm_storel_epi64((__m128i*)(dst + i_dst3), m3);

            dst += i_dst << 2;
        }
        break;
    }
    case 12: {
        __m128i m0, m1, m2, m3;
        __m128i mask = _mm_set_epi32(0, -1, -1, -1);
        int i_dst3 = i_dst + i_dst2;
        for (i = 0; i < height; i += 2) {
            m0 = _mm_loadu_si128((const __m128i*)(pfirst[0] + i));
            m1 = _mm_loadu_si128((const __m128i*)(pfirst[1] + i));
            m2 = _mm_srli_si128(m0, 1);
            m3 = _mm_srli_si128(m1, 1);
            _mm_maskmoveu_si128(m0, mask, (char*)(dst));
            _mm_maskmoveu_si128(m1, mask, (char*)(dst + i_dst));
            _mm_maskmoveu_si128(m2, mask, (char*)(dst + i_dst2));
            _mm_maskmoveu_si128(m3, mask, (char*)(dst + i_dst3));

            dst += i_dst << 2;
        }
        break;
    }
    case 16: {
        __m128i m0, m1, m2, m3;
        int i_dst3 = i_dst + i_dst2;
        for (i = 0; i < height; i += 2) {
            m0 = _mm_loadu_si128((const __m128i*)(pfirst[0] + i));
            m1 = _mm_loadu_si128((const __m128i*)(pfirst[1] + i));
            m2 = _mm_loadu_si128((const __m128i*)(pfirst[0] + i + 1));
            m3 = _mm_loadu_si128((const __m128i*)(pfirst[1] + i + 1));
            _mm_storeu_si128((__m128i*)(dst), m0);
            _mm_storeu_si128((__m128i*)(dst + i_dst), m1);
            _mm_storeu_si128((__m128i*)(dst + i_dst2), m2);
            _mm_storeu_si128((__m128i*)(dst + i_dst3), m3);

            dst += i_dst << 2;
        }
        break;
    }
    case 24: {
        __m128i m0, m1, m2, m3;
        for (i = 0; i < height; i++) {
            m0 = _mm_loadu_si128((const __m128i*)(pfirst[0] + i));
            m1 = _mm_loadu_si128((const __m128i*)(pfirst[1] + i));
            m2 = _mm_loadl_epi64((const __m128i*)(pfirst[0] + i + 16));
            m3 = _mm_loadl_epi64((const __m128i*)(pfirst[1] + i + 16));
            _mm_storeu_si128((__m128i*)(dst), m0);
            _mm_storel_epi64((__m128i*)(dst + 16), m2);
            _mm_storeu_si128((__m128i*)(dst + i_dst), m1);
            _mm_storel_epi64((__m128i*)(dst + i_dst + 16), m3);

            dst += i_dst2;
        }
        break;
    }
    case 32: {
        __m256i m0, m1, m2, m3;
        int i_dst3 = i_dst + i_dst2;
        for (i = 0; i < height; i += 2) {
            m0 = _mm256_loadu_si256((const __m256i*)(pfirst[0] + i));
            m1 = _mm256_loadu_si256((const __m256i*)(pfirst[1] + i));
            m2 = _mm256_loadu_si256((const __m256i*)(pfirst[0] + i + 1));
            m3 = _mm256_loadu_si256((const __m256i*)(pfirst[1] + i + 1));
            _mm256_storeu_si256((__m256i*)(dst), m0);
            _mm256_storeu_si256((__m256i*)(dst + i_dst), m1);
            _mm256_storeu_si256((__m256i*)(dst + i_dst2), m2);
            _mm256_storeu_si256((__m256i*)(dst + i_dst3), m3);

            dst += i_dst << 2;
        }
        break;
    }
    case 48: {
        __m256i m0, m1, m2, m3;
        __m128i s0, s1, s2, s3;
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        for (i = 0; i < height; i += 2) {
            m0 = _mm256_loadu_si256((const __m256i*)(pfirst[0] + i));
            m1 = _mm256_loadu_si256((const __m256i*)(pfirst[1] + i));
            m2 = _mm256_loadu_si256((const __m256i*)(pfirst[0] + i + 1));
            m3 = _mm256_loadu_si256((const __m256i*)(pfirst[1] + i + 1));
            s0 = _mm_loadu_si128((const __m128i*)(pfirst[0] + i + 32));
            s1 = _mm_loadu_si128((const __m128i*)(pfirst[1] + i + 32));
            s2 = _mm_loadu_si128((const __m128i*)(pfirst[0] + i + 33));
            s3 = _mm_loadu_si128((const __m128i*)(pfirst[1] + i + 33));
            _mm256_storeu_si256((__m256i*)(dst), m0);
            _mm_storeu_si128((__m128i*)(dst + 32), s0);
            _mm256_storeu_si256((__m256i*)(dst + i_dst), m1);
            _mm_storeu_si128((__m128i*)(dst + 32 + i_dst), s1);
            _mm256_storeu_si256((__m256i*)(dst + i_dst2), m2);
            _mm_storeu_si128((__m128i*)(dst + 32 + i_dst2), s2);
            _mm256_storeu_si256((__m256i*)(dst + i_dst3), m3);
            _mm_storeu_si128((__m128i*)(dst + 32 + i_dst3), s3);

            dst += i_dst << 2;
        }
        break;
    }
    case 64: {
        __m256i m0, m1, m2, m3, m4, m5, m6, m7;
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        for (i = 0; i < height; i += 2) {
            m0 = _mm256_loadu_si256((const __m256i*)(pfirst[0] + i));
            m1 = _mm256_loadu_si256((const __m256i*)(pfirst[1] + i));
            m2 = _mm256_loadu_si256((const __m256i*)(pfirst[0] + i + 1));
            m3 = _mm256_loadu_si256((const __m256i*)(pfirst[1] + i + 1));
            m4 = _mm256_loadu_si256((const __m256i*)(pfirst[0] + i + 32));
            m5 = _mm256_loadu_si256((const __m256i*)(pfirst[1] + i + 32));
            m6 = _mm256_loadu_si256((const __m256i*)(pfirst[0] + i + 33));
            m7 = _mm256_loadu_si256((const __m256i*)(pfirst[1] + i + 33));
            _mm256_storeu_si256((__m256i*)(dst), m0);
            _mm256_storeu_si256((__m256i*)(dst + 32), m4);
            _mm256_storeu_si256((__m256i*)(dst + i_dst), m1);
            _mm256_storeu_si256((__m256i*)(dst + 32 + i_dst), m5);
            _mm256_storeu_si256((__m256i*)(dst + i_dst2), m2);
            _mm256_storeu_si256((__m256i*)(dst + 32 + i_dst2), m6);
            _mm256_storeu_si256((__m256i*)(dst + i_dst3), m3);
            _mm256_storeu_si256((__m256i*)(dst + 32 + i_dst3), m7);

            dst += i_dst << 2;
        }
        break;
    }
    }
}


#elif(BIT_DEPTH == 10)
void uavs3e_ipred_ver_avx2(pel *src, pel *dst, int i_dst, int width, int height)
{
    int y;
    switch (width) {
    case 4: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            CP64(dst, src);
            CP64(dst + i_dst, src);
            CP64(dst + i_dst2, src);
            CP64(dst + i_dst3, src);
            dst += i_dst4;
        }
        break;
    }
    case 8: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        __m128i T0;
        T0 = _mm_loadu_si128((__m128i *)src);
        for (y = 0; y < height; y += 4) {
            _mm_storeu_si128((__m128i *)(dst), T0);
            _mm_storeu_si128((__m128i *)(dst + i_dst), T0);
            _mm_storeu_si128((__m128i *)(dst + i_dst2), T0);
            _mm_storeu_si128((__m128i *)(dst + i_dst3), T0);
            dst += i_dst4;
        }
        break;
    }
    case 12: {
        __m128i T0;
        T0 = _mm_loadu_si128((__m128i *)src);
        for (y = 0; y < height; y += 2) {
            _mm_storeu_si128((__m128i *)(dst), T0);
            _mm_storeu_si128((__m128i *)(dst + i_dst), T0);
            CP64(dst + 8, src + 8);
            CP64(dst + 8 + i_dst, src + 8);
            dst += i_dst << 1;
        }
        break;
    }
    case 16: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        __m256i T0;
        T0 = _mm256_loadu_si256((__m256i *)(src));
        for (y = 0; y < height; y += 4) {
            _mm256_storeu_si256((__m256i *)(dst), T0);
            _mm256_storeu_si256((__m256i *)(dst + i_dst), T0);
            _mm256_storeu_si256((__m256i *)(dst + i_dst2), T0);
            _mm256_storeu_si256((__m256i *)(dst + i_dst3), T0);
            dst += i_dst4;
        }
        break;
    }
    case 24: {
        __m256i T0;
        __m128i t0;
        int i_dst2 = i_dst << 1;
        T0 = _mm256_loadu_si256((__m256i *)(src));
        t0 = _mm_loadu_si128((__m128i *)(src + 16));
        for (y = 0; y < height; y += 2) {
            _mm256_store_si256((__m256i *)(dst), T0);
            _mm_store_si128((__m128i *)(dst + 16), t0);
            _mm256_store_si256((__m256i *)(dst + i_dst), T0);
            _mm_store_si128((__m128i *)(dst + i_dst + 16), t0);
            dst += i_dst2;
        }
        break;
    }
    case 32: {
        __m256i T0, T1;
        int i_dst2 = i_dst << 1;
        T0 = _mm256_loadu_si256((__m256i *)(src));
        T1 = _mm256_loadu_si256((__m256i *)(src + 16));
        for (y = 0; y < height; y += 2) {
            _mm256_store_si256((__m256i *)(dst), T0);
            _mm256_store_si256((__m256i *)(dst + 16), T1);
            _mm256_store_si256((__m256i *)(dst + i_dst), T0);
            _mm256_store_si256((__m256i *)(dst + i_dst + 16), T1);
            dst += i_dst2;
        }
        break;
    }
    case 48: {
        __m256i T0, T1, T2;
        int i_dst2 = i_dst << 1;
        T0 = _mm256_loadu_si256((__m256i *)(src));
        T1 = _mm256_loadu_si256((__m256i *)(src + 16));
        T2 = _mm256_loadu_si256((__m256i *)(src + 32));
        for (y = 0; y < height; y += 2) {
            _mm256_store_si256((__m256i *)(dst), T0);
            _mm256_store_si256((__m256i *)(dst + 16), T1);
            _mm256_store_si256((__m256i *)(dst + 32), T2);
            _mm256_store_si256((__m256i *)(dst + i_dst), T0);
            _mm256_store_si256((__m256i *)(dst + i_dst + 16), T1);
            _mm256_store_si256((__m256i *)(dst + i_dst + 32), T2);
            dst += i_dst2;
        }
        break;
    }
    case 64: {
        __m256i T0, T1, T2, T3;
        int i_dst2 = i_dst << 1;
        T0 = _mm256_loadu_si256((__m256i *)(src));
        T1 = _mm256_loadu_si256((__m256i *)(src + 16));
        T2 = _mm256_loadu_si256((__m256i *)(src + 32));
        T3 = _mm256_loadu_si256((__m256i *)(src + 48));
        for (y = 0; y < height; y += 2) {
            _mm256_store_si256((__m256i *)(dst), T0);
            _mm256_store_si256((__m256i *)(dst + 16), T1);
            _mm256_store_si256((__m256i *)(dst + 32), T2);
            _mm256_store_si256((__m256i *)(dst + 48), T3);
            _mm256_store_si256((__m256i *)(dst + i_dst), T0);
            _mm256_store_si256((__m256i *)(dst + i_dst + 16), T1);
            _mm256_store_si256((__m256i *)(dst + i_dst + 32), T2);
            _mm256_store_si256((__m256i *)(dst + i_dst + 48), T3);
            dst += i_dst2;
        }
        break;
    }
    default:
        com_assert(0);
        break;
    }
}

void uavs3e_ipred_hor_avx2(pel *src, pel *dst, int i_dst, int width, int height)
{
    int y;
    switch (width) {
    case 4: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            M64(dst) = 0x0001000100010001 * src[-y];
            M64(dst + i_dst) = 0x0001000100010001 * src[-y - 1];
            M64(dst + i_dst2) = 0x0001000100010001 * src[-y - 2];
            M64(dst + i_dst3) = 0x0001000100010001 * src[-y - 3];
            dst += i_dst4;
        }
        break;
    }
    case 8: {
        __m128i T0, T1, T2, T3;
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            T0 = _mm_set1_epi16(src[-y]);
            T1 = _mm_set1_epi16(src[-y - 1]);
            T2 = _mm_set1_epi16(src[-y - 2]);
            T3 = _mm_set1_epi16(src[-y - 3]);
            _mm_storeu_si128((__m128i *)(dst), T0);
            _mm_storeu_si128((__m128i *)(dst + i_dst), T1);
            _mm_storeu_si128((__m128i *)(dst + i_dst2), T2);
            _mm_storeu_si128((__m128i *)(dst + i_dst3), T3);
            dst += i_dst4;
        }
        break;
    }
    case 12: {
        __m128i T0, T1, T2, T3;
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            T0 = _mm_set1_epi16(src[-y]);
            T1 = _mm_set1_epi16(src[-y - 1]);
            T2 = _mm_set1_epi16(src[-y - 2]);
            T3 = _mm_set1_epi16(src[-y - 3]);
            _mm_storeu_si128((__m128i *)(dst), T0);
            _mm_storeu_si128((__m128i *)(dst + i_dst), T1);
            _mm_storeu_si128((__m128i *)(dst + i_dst2), T2);
            _mm_storeu_si128((__m128i *)(dst + i_dst3), T3);
            M64(dst + 8) = _mm_extract_epi64(T0, 0);
            M64(dst + 8 + i_dst) = _mm_extract_epi64(T1, 0);
            M64(dst + 8 + i_dst2) = _mm_extract_epi64(T2, 0);
            M64(dst + 8 + i_dst3) = _mm_extract_epi64(T3, 0);
            dst += i_dst4;
        }
        break;
    }
    case 16: {
        __m256i T0, T1;
        int i_dst2 = i_dst << 1;
        for (y = 0; y < height; y += 2) {
            T0 = _mm256_set1_epi16((short)src[-y]);
            T1 = _mm256_set1_epi16((short)src[-y - 1]);
            _mm256_storeu_si256((__m256i *)(dst), T0);
            _mm256_storeu_si256((__m256i *)(dst + i_dst), T1);
            dst += i_dst2;
        }
        break;
    }
    case 24: {
        __m128i T0, T1;
        int i_dst2 = i_dst << 1;
        for (y = 0; y < height; y += 2) {
            T0 = _mm_set1_epi16(src[-y]);
            T1 = _mm_set1_epi16(src[-y - 1]);
            _mm_store_si128((__m128i *)(dst), T0);
            _mm_store_si128((__m128i *)(dst + 8), T0);
            _mm_store_si128((__m128i *)(dst + 16), T0);
            _mm_store_si128((__m128i *)(dst + i_dst), T1);
            _mm_store_si128((__m128i *)(dst + i_dst + 8), T1);
            _mm_store_si128((__m128i *)(dst + i_dst + 16), T1);
            dst += i_dst2;
        }
        break;
    }

    case 32: {
        __m256i T0, T1;
        int i_dst2 = i_dst << 1;
        for (y = 0; y < height; y += 2) {
            T0 = _mm256_set1_epi16(src[-y]);
            T1 = _mm256_set1_epi16(src[-y - 1]);
            _mm256_store_si256((__m256i *)(dst), T0);
            _mm256_store_si256((__m256i *)(dst + 16), T0);
            _mm256_store_si256((__m256i *)(dst + i_dst), T1);
            _mm256_store_si256((__m256i *)(dst + i_dst + 16), T1);
            dst += i_dst2;
        }
        break;
    }
    case 48: {
        __m256i T0, T1;
        int i_dst2 = i_dst << 1;
        for (y = 0; y < height; y += 2) {
            T0 = _mm256_set1_epi16(src[-y]);
            T1 = _mm256_set1_epi16(src[-y - 1]);
            _mm256_store_si256((__m256i *)(dst), T0);
            _mm256_store_si256((__m256i *)(dst + 16), T0);
            _mm256_store_si256((__m256i *)(dst + 32), T0);
            _mm256_store_si256((__m256i *)(dst + i_dst), T1);
            _mm256_store_si256((__m256i *)(dst + i_dst + 16), T1);
            _mm256_store_si256((__m256i *)(dst + i_dst + 32), T1);
            dst += i_dst2;
        }
        break;
    }
    case 64: {
        __m256i T0, T1;
        int i_dst2 = i_dst << 1;
        for (y = 0; y < height; y += 2) {
            T0 = _mm256_set1_epi16(src[-y]);
            T1 = _mm256_set1_epi16(src[-y - 1]);
            _mm256_store_si256((__m256i *)(dst), T0);
            _mm256_store_si256((__m256i *)(dst + 16), T0);
            _mm256_store_si256((__m256i *)(dst + 32), T0);
            _mm256_store_si256((__m256i *)(dst + 48), T0);
            _mm256_store_si256((__m256i *)(dst + i_dst), T1);
            _mm256_store_si256((__m256i *)(dst + i_dst + 16), T1);
            _mm256_store_si256((__m256i *)(dst + i_dst + 32), T1);
            _mm256_store_si256((__m256i *)(dst + i_dst + 48), T1);
            dst += i_dst2;
        }
        break;
    }
    default:
        com_assert(0);
        break;
    }
}

void uavs3e_ipred_dc_avx2(pel *src, pel *dst, int i_dst, int width, int height, u16 avail_cu, int bit_depth)
{
    int   i, x, y;
    int   dc;
    pel  *p_src = src - 1;
    int left_avail = IS_AVAIL(avail_cu, AVAIL_LE);
    int above_avail = IS_AVAIL(avail_cu, AVAIL_UP);

    if (left_avail && above_avail) {
        int length = width + height + 1;
        __m128i sum = _mm_setzero_si128();
        __m128i val;

        p_src = src - height;

        for (i = 0; i < length - 7; i += 8) {
            val = _mm_loadu_si128((__m128i *)(p_src + i));
            sum = _mm_add_epi16(sum, val);
        }
        if (i < length) {
            int left_pixels = length - i;
            __m128i mask = _mm_load_si128((__m128i *)(uavs3e_simd_mask[left_pixels - 1]));
            val = _mm_loadu_si128((__m128i *)(p_src + i));
            val = _mm_and_si128(val, mask);
            sum = _mm_add_epi16(sum, val);
        }
        sum = _mm_add_epi16(sum, _mm_srli_si128(sum, 8));
        sum = _mm_add_epi16(sum, _mm_srli_si128(sum, 4));
        sum = _mm_add_epi16(sum, _mm_srli_si128(sum, 2));

        dc = _mm_extract_epi16(sum, 0) + ((width + height) >> 1) - src[0];

        dc = (dc * (4096 / (width + height))) >> 12;

    }
    else if (left_avail) {
        dc = 0;
        for (y = 0; y < height; y++) {
            dc += p_src[-y];
        }
        dc += height / 2;
        dc /= height;
    }
    else {
        p_src = src + 1;
        dc = 0;
        if (above_avail) {
            for (x = 0; x < width; x++) {
                dc += p_src[x];
            }
            dc += width / 2;
            dc /= width;
        }
        else {
            dc = 1 << (bit_depth - 1);
        }
    }

    switch (width) {
    case 4: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        u64 v64 = 0x0001000100010001 * dc;
        for (y = 0; y < height; y += 4) {
            M64(dst) = v64;
            M64(dst + i_dst) = v64;
            M64(dst + i_dst2) = v64;
            M64(dst + i_dst3) = v64;
            dst += i_dst4;
        }
        break;
    }
    case 8: {
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        __m128i T = _mm_set1_epi16((s16)dc);
        for (y = 0; y < height; y += 4) {
            _mm_storeu_si128((__m128i *)(dst), T);
            _mm_storeu_si128((__m128i *)(dst + i_dst), T);
            _mm_storeu_si128((__m128i *)(dst + i_dst2), T);
            _mm_storeu_si128((__m128i *)(dst + i_dst3), T);
            dst += i_dst4;
        }
        break;
    }
    case 16: {
        __m256i T = _mm256_set1_epi16((s16)dc);
        int i_dst2 = i_dst << 1;
        int i_dst3 = i_dst + i_dst2;
        int i_dst4 = i_dst << 2;
        for (y = 0; y < height; y += 4) {
            _mm256_storeu_si256((__m256i *)(dst), T);
            _mm256_storeu_si256((__m256i *)(dst + i_dst), T);
            _mm256_storeu_si256((__m256i *)(dst + i_dst2), T);
            _mm256_storeu_si256((__m256i *)(dst + i_dst3), T);
            dst += i_dst4;
        }
        break;
    }
    case 32: {
        int i_dst2 = i_dst << 1;
        __m256i T = _mm256_set1_epi16((s16)dc);
        for (y = 0; y < height; y += 2) {
            _mm256_store_si256((__m256i *)(dst), T);
            _mm256_store_si256((__m256i *)(dst + 16), T);
            _mm256_store_si256((__m256i *)(dst + i_dst), T);
            _mm256_store_si256((__m256i *)(dst + i_dst + 16), T);
            dst += i_dst2;
        }
        break;
    }
    case 64: {
        __m256i T = _mm256_set1_epi16((s16)dc);
        for (y = 0; y < height; y++) {
            _mm256_store_si256((__m256i *)(dst), T);
            _mm256_store_si256((__m256i *)(dst + 16), T);
            _mm256_store_si256((__m256i *)(dst + 32), T);
            _mm256_store_si256((__m256i *)(dst + 48), T);
            dst += i_dst;
        }
        break;
    }
    default:
        com_assert(0);
        break;
    }
}

#endif