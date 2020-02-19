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
            _mm_store_si128((__m128i *)(dst + 0), T1);
            _mm_store_si128((__m128i *)(dst + 16), T2);
            _mm_store_si128((__m128i *)(dst + 32), T3);
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
            _mm256_store_si256((__m256i *)(dst), T0);
            _mm256_store_si256((__m256i *)(dst + 32), T1);
            _mm256_store_si256((__m256i *)(dst + i_dst), T0);
            _mm256_store_si256((__m256i *)(dst + i_dst + 32), T1);
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
            _mm_store_si128((__m128i *)(dst), T0);
            _mm_store_si128((__m128i *)(dst + 16), T0);
            _mm_store_si128((__m128i *)(dst + 32), T0);
            _mm_store_si128((__m128i *)(dst + i_dst), T1);
            _mm_store_si128((__m128i *)(dst + i_dst + 16), T1);
            _mm_store_si128((__m128i *)(dst + i_dst + 32), T1);
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
            _mm256_store_si256((__m256i *)(dst), T0);
            _mm256_store_si256((__m256i *)(dst + 32), T0);
            _mm256_store_si256((__m256i *)(dst + i_dst), T1);
            _mm256_store_si256((__m256i *)(dst + i_dst + 32), T1);
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
            _mm256_store_si256((__m256i *)(dst), T);
            _mm256_store_si256((__m256i *)(dst + 32), T);
            _mm256_store_si256((__m256i *)(dst + i_dst), T);
            _mm256_store_si256((__m256i *)(dst + i_dst + 32), T);
            dst += i_dst2;
        }
        break;
    }
    default:
        com_assert(0);
        break;
    }
}
