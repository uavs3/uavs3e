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

void uavs3e_if_cpy_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
{
    int col;

    while (height--) {
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
        for (col = 0; col < width; col += 4) {
            CP32(dst + col, src + col);
        }
        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_cpy_w4_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
{
    while (height) {
        CP32(dst, src);
        CP32(dst + i_dst, src + i_src);
        height -= 2;
        src += i_src << 1;
        dst += i_dst << 1;
    }
}

void uavs3e_if_cpy_w8_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
{
    int i_src2 = i_src << 1;
    int i_dst2 = i_dst << 1;
    while (height) {
        CP64(dst, src);
        CP64(dst + i_dst, src + i_src);
        src += i_src2;
        dst += i_dst2;
        height -= 2;
    }
}

void uavs3e_if_cpy_w16_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
{
    int i_src2 = i_src << 1;
    int i_dst2 = i_dst << 1;
    int i_src3 = i_src2 + i_src;
    int i_dst3 = i_dst2 + i_dst;
    int i_src4 = i_src << 2;
    int i_dst4 = i_dst << 2;
    __m128i m0, m1, m2, m3;
    while (height) {
        m0 = _mm_loadu_si128((const __m128i *)(src));
        m1 = _mm_loadu_si128((const __m128i *)(src + i_src));
        m2 = _mm_loadu_si128((const __m128i *)(src + i_src2));
        m3 = _mm_loadu_si128((const __m128i *)(src + i_src3));
        _mm_storeu_si128((__m128i *)(dst), m0);
        _mm_storeu_si128((__m128i *)(dst + i_dst), m1);
        _mm_storeu_si128((__m128i *)(dst + i_dst2), m2);
        _mm_storeu_si128((__m128i *)(dst + i_dst3), m3);
        src += i_src4;
        dst += i_dst4;
        height -= 4;
    }
}

void uavs3e_if_cpy_w16x_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height)
{
    int col;

    while (height--) {
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
        for (col = 0; col < width; col += 16) {
            CP128(dst + col, src + col);
        }
        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_hor_chroma_w8x_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    __m128i mAddOffset = _mm_set1_epi16(32);
    __m128i mSwitch1   = _mm_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7,  8);
    __m128i mSwitch2   = _mm_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m128i mCoef1     = _mm_set1_epi16(*(s16 *)coeff);
    __m128i mCoef2     = _mm_set1_epi16(*(s16 *)(coeff + 2));

    src -= 1;

    while (height--) {
        for (int col = 0; col < width; col += 8) {
            __m128i mSrc = _mm_loadu_si128((__m128i*)(src + col));
            __m128i T0 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoef1);
            __m128i T1 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoef2);

            T0 = _mm_add_epi16(T0, T1);
            T0 = _mm_add_epi16(T0, mAddOffset);
            T0 = _mm_srai_epi16(T0, 6);
            T0 = _mm_packus_epi16(T0, T0);

            _mm_storel_epi64((__m128i*)&dst[col], T0);
        }
        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_hor_chroma_w8_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    __m128i mAddOffset = _mm_set1_epi16(32);
    __m128i mSwitch1 = _mm_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m128i mSwitch2 = _mm_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m128i mCoef1 = _mm_set1_epi16(*(s16 *)coeff);
    __m128i mCoef2 = _mm_set1_epi16(*(s16 *)(coeff + 2));

    src -= 1;
    height >>= 1;

    while (height--) {
        __m128i mSrc1 = _mm_loadu_si128((__m128i*)src); src += i_src;
        __m128i T0 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc1, mSwitch1), mCoef1);
        __m128i T1 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc1, mSwitch2), mCoef2);
        __m128i mSrc2 = _mm_loadu_si128((__m128i*)src); src += i_src;
        __m128i T2 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc2, mSwitch1), mCoef1);
        __m128i T3 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc2, mSwitch2), mCoef2);

        T0 = _mm_add_epi16(T0, T1);
        T2 = _mm_add_epi16(T2, T3);
        T0 = _mm_add_epi16(T0, mAddOffset);
        T2 = _mm_add_epi16(T2, mAddOffset);
        T0 = _mm_srai_epi16(T0, 6);
        T2 = _mm_srai_epi16(T2, 6);
        T0 = _mm_packus_epi16(T0, T2);

        _mm_storel_epi64((__m128i*)dst, T0);
        dst += i_dst;
        _mm_storel_epi64((__m128i*)dst, _mm_srli_si128(T0, 8));
        dst += i_dst;
    }
}

void uavs3e_if_hor_chroma_w4_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    __m128i mAddOffset = _mm_set1_epi16(32);
    __m128i mSwitch    = _mm_setr_epi8(0, 1, 2, 3, 1, 2, 3, 4, 2, 3, 4, 5, 3, 4, 5, 6);
    __m128i mCoef      = _mm_set1_epi32(*(int*)coeff);

    src -= 1;
    height >>= 2;

    while (height--) {
        __m128i mVal;
        __m128i T1 = _mm_maddubs_epi16(_mm_shuffle_epi8(_mm_loadu_si128((__m128i*)src), mSwitch), mCoef); src += i_src;
        __m128i T2 = _mm_maddubs_epi16(_mm_shuffle_epi8(_mm_loadu_si128((__m128i*)src), mSwitch), mCoef); src += i_src;
        __m128i T3 = _mm_maddubs_epi16(_mm_shuffle_epi8(_mm_loadu_si128((__m128i*)src), mSwitch), mCoef); src += i_src;
        __m128i T4 = _mm_maddubs_epi16(_mm_shuffle_epi8(_mm_loadu_si128((__m128i*)src), mSwitch), mCoef); src += i_src;

        T1 = _mm_hadd_epi16(T1, T2);
        T3 = _mm_hadd_epi16(T3, T4);
        T1 = _mm_add_epi16(T1, mAddOffset);
        T3 = _mm_add_epi16(T3, mAddOffset);
        T1 = _mm_srai_epi16(T1, 6);
        T3 = _mm_srai_epi16(T3, 6);

        mVal = _mm_packus_epi16(T1, T3);

        *(int*)dst = _mm_extract_epi32(mVal, 0); dst += i_dst;
        *(int*)dst = _mm_extract_epi32(mVal, 1); dst += i_dst;
        *(int*)dst = _mm_extract_epi32(mVal, 2); dst += i_dst;
        *(int*)dst = _mm_extract_epi32(mVal, 3); dst += i_dst;
    }
}

void uavs3e_if_hor_luma_w8x_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    int col;
    const int offset = 32;
    const int shift = 6;

    __m128i mAddOffset = _mm_set1_epi16(offset);

    __m128i mSwitch1 = _mm_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m128i mSwitch2 = _mm_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m128i mSwitch3 = _mm_setr_epi8(4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12);
    __m128i mSwitch4 = _mm_setr_epi8(6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14);

    __m128i mCoefy1_hor = _mm_set1_epi16(*(s16 *)coeff);
    __m128i mCoefy2_hor = _mm_set1_epi16(*(s16 *)(coeff + 2));
    __m128i mCoefy3_hor = _mm_set1_epi16(*(s16 *)(coeff + 4));
    __m128i mCoefy4_hor = _mm_set1_epi16(*(s16 *)(coeff + 6));

    src -= 3;
    while (height--) {

        __m128i T20, T40, T60, T80, s1, s2, sum, val;

        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
        for (col = 0; col < width; col += 8) {

            __m128i srcCoeff = _mm_loadu_si128((__m128i *)(src + col));

            T20 = _mm_maddubs_epi16(_mm_shuffle_epi8(srcCoeff, mSwitch1), mCoefy1_hor);
            T40 = _mm_maddubs_epi16(_mm_shuffle_epi8(srcCoeff, mSwitch2), mCoefy2_hor);
            T60 = _mm_maddubs_epi16(_mm_shuffle_epi8(srcCoeff, mSwitch3), mCoefy3_hor);
            T80 = _mm_maddubs_epi16(_mm_shuffle_epi8(srcCoeff, mSwitch4), mCoefy4_hor);

            s1 = _mm_add_epi16(T20, T40);
            s2 = _mm_add_epi16(T60, T80);
            sum = _mm_add_epi16(s1, s2);

            val = _mm_add_epi16(sum, mAddOffset);

            val = _mm_srai_epi16(val, shift);
            val = _mm_packus_epi16(val, val);

            _mm_storel_epi64((__m128i *)&dst[col], val);
        }

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_hor_luma_w4_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;

    __m128i mAddOffset = _mm_set1_epi16(offset);

    __m128i mSwitch1 = _mm_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 8, 9, 9, 10, 10, 11, 11, 12);
    __m128i mSwitch2 = _mm_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 10, 11, 11, 12, 12, 13, 13, 14);

    __m128i mCoefy1_hor = _mm_set1_epi16(*(s16 *)coeff);
    __m128i mCoefy2_hor = _mm_set1_epi16(*(s16 *)(coeff + 2));
    __m128i mCoefy3_hor = _mm_set1_epi16(*(s16 *)(coeff + 4));
    __m128i mCoefy4_hor = _mm_set1_epi16(*(s16 *)(coeff + 6));

    src -= 3;
    while (height) {
        __m128i T0, T1, T2, T3;
        __m128i m0, m1;
        __m128i src0, src1, src2, src3;

        uavs3e_prefetch(src + i_src * 2, _MM_HINT_NTA);
        src0 = _mm_loadu_si128((__m128i *)(src));
        src1 = _mm_loadu_si128((__m128i *)(src + i_src));
        src2 = _mm_srli_si128(src0, 4);
        src3 = _mm_srli_si128(src1, 4);

        m0 = _mm_unpacklo_epi64(src0, src1);
        m1 = _mm_unpacklo_epi64(src2, src3);

        T0 = _mm_shuffle_epi8(m0, mSwitch1);
        T1 = _mm_shuffle_epi8(m0, mSwitch2);
        T2 = _mm_shuffle_epi8(m1, mSwitch1);
        T3 = _mm_shuffle_epi8(m1, mSwitch2);

        T0 = _mm_maddubs_epi16(T0, mCoefy1_hor);
        T1 = _mm_maddubs_epi16(T1, mCoefy2_hor);
        T2 = _mm_maddubs_epi16(T2, mCoefy3_hor);
        T3 = _mm_maddubs_epi16(T3, mCoefy4_hor);

        T0 = _mm_add_epi16(T0, T1);
        T2 = _mm_add_epi16(T2, T3);

        T0 = _mm_add_epi16(T0, T2);

        T0 = _mm_add_epi16(T0, mAddOffset);

        T0 = _mm_srai_epi16(T0, shift);
        T0 = _mm_packus_epi16(T0, T0);

        height -= 2;
        src += i_src << 1;

        M32(dst) = _mm_extract_epi32(T0, 0);
        M32(dst + i_dst) = _mm_extract_epi32(T0, 1);

        dst += i_dst << 1;
    }
}

void uavs3e_if_hor_luma_w8_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;

    __m128i mAddOffset = _mm_set1_epi16(offset);

    __m128i mSwitch1 = _mm_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m128i mSwitch2 = _mm_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m128i mSwitch3 = _mm_setr_epi8(4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12);
    __m128i mSwitch4 = _mm_setr_epi8(6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14);

    __m128i mCoefy1_hor = _mm_set1_epi16(*(s16 *)coeff);
    __m128i mCoefy2_hor = _mm_set1_epi16(*(s16 *)(coeff + 2));
    __m128i mCoefy3_hor = _mm_set1_epi16(*(s16 *)(coeff + 4));
    __m128i mCoefy4_hor = _mm_set1_epi16(*(s16 *)(coeff + 6));

    src -= 3;
    while (height) {

        __m128i T0, T1, T2, T3;
        __m128i m0, m1, m2, m3;
        __m128i src0, src1;

        uavs3e_prefetch(src + i_src * 2, _MM_HINT_NTA);
        src0 = _mm_loadu_si128((__m128i *)(src));
        src1 = _mm_loadu_si128((__m128i *)(src + i_src));
        T0 = _mm_shuffle_epi8(src0, mSwitch1);
        T1 = _mm_shuffle_epi8(src0, mSwitch2);
        T2 = _mm_shuffle_epi8(src0, mSwitch3);
        T3 = _mm_shuffle_epi8(src0, mSwitch4);
        m0 = _mm_shuffle_epi8(src1, mSwitch1);
        m1 = _mm_shuffle_epi8(src1, mSwitch2);
        m2 = _mm_shuffle_epi8(src1, mSwitch3);
        m3 = _mm_shuffle_epi8(src1, mSwitch4);

        T0 = _mm_maddubs_epi16(T0, mCoefy1_hor);
        T1 = _mm_maddubs_epi16(T1, mCoefy2_hor);
        T2 = _mm_maddubs_epi16(T2, mCoefy3_hor);
        T3 = _mm_maddubs_epi16(T3, mCoefy4_hor);
        m0 = _mm_maddubs_epi16(m0, mCoefy1_hor);
        m1 = _mm_maddubs_epi16(m1, mCoefy2_hor);
        m2 = _mm_maddubs_epi16(m2, mCoefy3_hor);
        m3 = _mm_maddubs_epi16(m3, mCoefy4_hor);

        T0 = _mm_add_epi16(T0, T1);
        T2 = _mm_add_epi16(T2, T3);
        m0 = _mm_add_epi16(m0, m1);
        m2 = _mm_add_epi16(m2, m3);
        T0 = _mm_add_epi16(T0, T2);
        m0 = _mm_add_epi16(m0, m2);

        T0 = _mm_add_epi16(T0, mAddOffset);
        m0 = _mm_add_epi16(m0, mAddOffset);

        T0 = _mm_srai_epi16(T0, shift);
        m0 = _mm_srai_epi16(m0, shift);
        T0 = _mm_packus_epi16(T0, m0);

        height -= 2;

        _mm_storel_epi64((__m128i *)dst, T0);
        M64(dst + i_dst) = _mm_extract_epi64(T0, 1);

        src += i_src << 1;
        dst += i_dst << 1;
    }
}

void uavs3e_if_ver_chroma_w4_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m128i mAddOffset = _mm_set1_epi16(offset);
    __m128i coeff0 = _mm_set1_epi16(*(s16 *)coeff);
    __m128i coeff1 = _mm_set1_epi16(*(s16 *)(coeff + 2));
    __m128i mVal;
    __m128i m0, m1, m2, m3;

    src -= i_src;

    while (height > 0) {
        __m128i T0 = _mm_loadl_epi64((__m128i *)(src));
        __m128i T1 = _mm_loadl_epi64((__m128i *)(src + i_src));
        __m128i T2 = _mm_loadl_epi64((__m128i *)(src + 2 * i_src));
        __m128i T3 = _mm_loadl_epi64((__m128i *)(src + 3 * i_src));
        __m128i T4 = _mm_loadl_epi64((__m128i *)(src + 4 * i_src));
        uavs3e_prefetch(src + 5 * i_src, _MM_HINT_NTA);

        m0 = _mm_unpacklo_epi8(T0, T1);
        m1 = _mm_unpacklo_epi8(T2, T3);
        m2 = _mm_unpacklo_epi8(T1, T2);
        m3 = _mm_unpacklo_epi8(T3, T4);

        T0 = _mm_unpacklo_epi64(m0, m2);
        T1 = _mm_unpacklo_epi64(m1, m3);

        T0 = _mm_maddubs_epi16(T0, coeff0);
        T1 = _mm_maddubs_epi16(T1, coeff1);

        mVal = _mm_add_epi16(T0, T1);

        mVal = _mm_add_epi16(mVal, mAddOffset);
        mVal = _mm_srai_epi16(mVal, shift);
        mVal = _mm_packus_epi16(mVal, mVal);

        height -= 2;

        M32(dst) = _mm_extract_epi32(mVal, 0);
        M32(dst + i_dst) = _mm_extract_epi32(mVal, 1);

        src += i_src << 1;
        dst += i_dst << 1;
    }
}

void uavs3e_if_ver_chroma_w8_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m128i mAddOffset = _mm_set1_epi16(offset);
    __m128i coeff0 = _mm_set1_epi16(*(s32 *)coeff);
    __m128i coeff1 = _mm_set1_epi16(*(s32 *)(coeff + 2));
    __m128i mVal;

    src -= i_src;

    while (height--) {
        __m128i T00 = _mm_loadl_epi64((__m128i *)(src));
        __m128i T10 = _mm_loadl_epi64((__m128i *)(src + i_src));
        __m128i T20 = _mm_loadl_epi64((__m128i *)(src + 2 * i_src));
        __m128i T30 = _mm_loadl_epi64((__m128i *)(src + 3 * i_src));
        uavs3e_prefetch(src + 4 * i_src, _MM_HINT_NTA);

        T00 = _mm_unpacklo_epi8(T00, T10);
        T10 = _mm_unpacklo_epi8(T20, T30);

        T00 = _mm_maddubs_epi16(T00, coeff0);
        T10 = _mm_maddubs_epi16(T10, coeff1);

        mVal = _mm_add_epi16(T00, T10);

        mVal = _mm_add_epi16(mVal, mAddOffset);
        mVal = _mm_srai_epi16(mVal, shift);
        mVal = _mm_packus_epi16(mVal, mVal);

        _mm_storel_epi64((__m128i *)dst, mVal);

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_ver_chroma_w16_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m128i mAddOffset = _mm_set1_epi16(offset);
    __m128i coeff0 = _mm_set1_epi16(*(s32 *)coeff);
    __m128i coeff1 = _mm_set1_epi16(*(s32 *)(coeff + 2));
    __m128i mVal1, mVal2;

    src -= i_src;

    while (height--) {
        uavs3e_prefetch(src + 4 * i_src, _MM_HINT_NTA);
        __m128i T01 = _mm_loadu_si128((__m128i *)(src));
        __m128i T11 = _mm_loadu_si128((__m128i *)(src + i_src));
        __m128i T21 = _mm_loadu_si128((__m128i *)(src + 2 * i_src));
        __m128i T31 = _mm_loadu_si128((__m128i *)(src + 3 * i_src));

        __m128i T00 = _mm_unpacklo_epi8(T01, T11);
        __m128i T10 = _mm_unpacklo_epi8(T21, T31);
        __m128i T20 = _mm_unpackhi_epi8(T01, T11);
        __m128i T30 = _mm_unpackhi_epi8(T21, T31);

        T00 = _mm_maddubs_epi16(T00, coeff0);
        T10 = _mm_maddubs_epi16(T10, coeff1);
        T20 = _mm_maddubs_epi16(T20, coeff0);
        T30 = _mm_maddubs_epi16(T30, coeff1);

        mVal1 = _mm_add_epi16(T00, T10);
        mVal2 = _mm_add_epi16(T20, T30);

        mVal1 = _mm_add_epi16(mVal1, mAddOffset);
        mVal1 = _mm_srai_epi16(mVal1, shift);
        mVal2 = _mm_add_epi16(mVal2, mAddOffset);
        mVal2 = _mm_srai_epi16(mVal2, shift);
        mVal1 = _mm_packus_epi16(mVal1, mVal2);

        _mm_storeu_si128((__m128i *)dst, mVal1);

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_ver_chroma_w16x_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    int col;
    const int offset = 32;
    const int shift = 6;
    __m128i mAddOffset = _mm_set1_epi16(offset);
    pel const *p;
    __m128i coeff0 = _mm_set1_epi16(*(s16 *)coeff);
    __m128i coeff1 = _mm_set1_epi16(*(s16 *)(coeff + 2));
    __m128i mVal1, mVal2;

    src -= i_src;

    while (height--) {
        p = src;
        uavs3e_prefetch(src + 4 * i_src, _MM_HINT_NTA);
        for (col = 0; col < width; col += 16) {
            __m128i T01 = _mm_loadu_si128((__m128i *)(p));
            __m128i T11 = _mm_loadu_si128((__m128i *)(p + i_src));
            __m128i T21 = _mm_loadu_si128((__m128i *)(p + 2 * i_src));
            __m128i T31 = _mm_loadu_si128((__m128i *)(p + 3 * i_src));

            __m128i T00 = _mm_unpacklo_epi8(T01, T11);
            __m128i T10 = _mm_unpacklo_epi8(T21, T31);
            __m128i T20 = _mm_unpackhi_epi8(T01, T11);
            __m128i T30 = _mm_unpackhi_epi8(T21, T31);

            T00 = _mm_maddubs_epi16(T00, coeff0);
            T10 = _mm_maddubs_epi16(T10, coeff1);
            T20 = _mm_maddubs_epi16(T20, coeff0);
            T30 = _mm_maddubs_epi16(T30, coeff1);

            mVal1 = _mm_add_epi16(T00, T10);
            mVal2 = _mm_add_epi16(T20, T30);

            mVal1 = _mm_add_epi16(mVal1, mAddOffset);
            mVal1 = _mm_srai_epi16(mVal1, shift);
            mVal2 = _mm_add_epi16(mVal2, mAddOffset);
            mVal2 = _mm_srai_epi16(mVal2, shift);
            mVal1 = _mm_packus_epi16(mVal1, mVal2);

            _mm_store_si128((__m128i *)&dst[col], mVal1);

            p += 16;
        }

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_ver_luma_w4_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m128i mAddOffset = _mm_set1_epi16(offset);
    __m128i coeff0 = _mm_set1_epi16(*(s16 *)coeff);
    __m128i coeff1 = _mm_set1_epi16(*(s16 *)(coeff + 2));
    __m128i coeff2 = _mm_set1_epi16(*(s16 *)(coeff + 4));
    __m128i coeff3 = _mm_set1_epi16(*(s16 *)(coeff + 6));
    __m128i m0, m1, m2, m3, m4, m5, m6, m7;
    __m128i mVal;

    src -= 3 * i_src;

    while (height) {
        __m128i T00 = _mm_loadl_epi64((__m128i *)(src));
        __m128i T10 = _mm_loadl_epi64((__m128i *)(src + i_src));
        __m128i T20 = _mm_loadl_epi64((__m128i *)(src + 2 * i_src));
        __m128i T30 = _mm_loadl_epi64((__m128i *)(src + 3 * i_src));
        __m128i T40 = _mm_loadl_epi64((__m128i *)(src + 4 * i_src));
        __m128i T50 = _mm_loadl_epi64((__m128i *)(src + 5 * i_src));
        __m128i T60 = _mm_loadl_epi64((__m128i *)(src + 6 * i_src));
        __m128i T70 = _mm_loadl_epi64((__m128i *)(src + 7 * i_src));
        __m128i T80 = _mm_loadl_epi64((__m128i *)(src + 8 * i_src));
        uavs3e_prefetch(src + 9 * i_src, _MM_HINT_NTA);

        m0 = _mm_unpacklo_epi8(T00, T10);
        m1 = _mm_unpacklo_epi8(T20, T30);
        m2 = _mm_unpacklo_epi8(T40, T50);
        m3 = _mm_unpacklo_epi8(T60, T70);
        m4 = _mm_unpacklo_epi8(T10, T20);
        m5 = _mm_unpacklo_epi8(T30, T40);
        m6 = _mm_unpacklo_epi8(T50, T60);
        m7 = _mm_unpacklo_epi8(T70, T80);

        T00 = _mm_unpacklo_epi64(m0, m4);
        T10 = _mm_unpacklo_epi64(m1, m5);
        T20 = _mm_unpacklo_epi64(m2, m6);
        T30 = _mm_unpacklo_epi64(m3, m7);

        T00 = _mm_maddubs_epi16(T00, coeff0);
        T10 = _mm_maddubs_epi16(T10, coeff1);
        T20 = _mm_maddubs_epi16(T20, coeff2);
        T30 = _mm_maddubs_epi16(T30, coeff3);

        mVal = _mm_add_epi16(T00, T10);
        mVal = _mm_add_epi16(mVal, T20);
        mVal = _mm_add_epi16(mVal, T30);

        mVal = _mm_add_epi16(mVal, mAddOffset);
        mVal = _mm_srai_epi16(mVal, shift);
        mVal = _mm_packus_epi16(mVal, mVal);

        height -= 2;

        M32(dst) = _mm_extract_epi32(mVal, 0);
        M32(dst + i_dst) = _mm_extract_epi32(mVal, 1);

        src += i_src << 1;
        dst += i_dst << 1;
    }
}

void uavs3e_if_ver_luma_w8_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m128i mAddOffset = _mm_set1_epi16(offset);
    __m128i coeff0 = _mm_set1_epi16(*(s16 *)coeff);
    __m128i coeff1 = _mm_set1_epi16(*(s16 *)(coeff + 2));
    __m128i coeff2 = _mm_set1_epi16(*(s16 *)(coeff + 4));
    __m128i coeff3 = _mm_set1_epi16(*(s16 *)(coeff + 6));
    __m128i mVal;

    src -= 3 * i_src;

    while (height--) {
        __m128i T00 = _mm_loadl_epi64((__m128i *)(src));
        __m128i T10 = _mm_loadl_epi64((__m128i *)(src + i_src));
        __m128i T20 = _mm_loadl_epi64((__m128i *)(src + 2 * i_src));
        __m128i T30 = _mm_loadl_epi64((__m128i *)(src + 3 * i_src));
        __m128i T40 = _mm_loadl_epi64((__m128i *)(src + 4 * i_src));
        __m128i T50 = _mm_loadl_epi64((__m128i *)(src + 5 * i_src));
        __m128i T60 = _mm_loadl_epi64((__m128i *)(src + 6 * i_src));
        __m128i T70 = _mm_loadl_epi64((__m128i *)(src + 7 * i_src));
        uavs3e_prefetch(src + 8 * i_src, _MM_HINT_NTA);

        T00 = _mm_unpacklo_epi8(T00, T10);
        T10 = _mm_unpacklo_epi8(T20, T30);
        T20 = _mm_unpacklo_epi8(T40, T50);
        T30 = _mm_unpacklo_epi8(T60, T70);

        T00 = _mm_maddubs_epi16(T00, coeff0);
        T10 = _mm_maddubs_epi16(T10, coeff1);
        T20 = _mm_maddubs_epi16(T20, coeff2);
        T30 = _mm_maddubs_epi16(T30, coeff3);

        mVal = _mm_add_epi16(T00, T10);
        mVal = _mm_add_epi16(mVal, T20);
        mVal = _mm_add_epi16(mVal, T30);

        mVal = _mm_add_epi16(mVal, mAddOffset);
        mVal = _mm_srai_epi16(mVal, shift);
        mVal = _mm_packus_epi16(mVal, mVal);

        _mm_storel_epi64((__m128i *)dst, mVal);

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_ver_luma_w16_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    const int offset = 32;
    const int shift = 6;
    __m128i mAddOffset = _mm_set1_epi16(offset);

    src -= 3 * i_src;

    __m128i coeff0 = _mm_set1_epi16(*(s16 *)coeff);
    __m128i coeff1 = _mm_set1_epi16(*(s16 *)(coeff + 2));
    __m128i coeff2 = _mm_set1_epi16(*(s16 *)(coeff + 4));
    __m128i coeff3 = _mm_set1_epi16(*(s16 *)(coeff + 6));
    __m128i mVal1, mVal2;
    while (height--) {
        __m128i T01 = _mm_loadu_si128((__m128i *)(src));
        __m128i T11 = _mm_loadu_si128((__m128i *)(src + i_src));
        __m128i T21 = _mm_loadu_si128((__m128i *)(src + 2 * i_src));
        __m128i T31 = _mm_loadu_si128((__m128i *)(src + 3 * i_src));
        __m128i T41 = _mm_loadu_si128((__m128i *)(src + 4 * i_src));
        __m128i T51 = _mm_loadu_si128((__m128i *)(src + 5 * i_src));
        __m128i T61 = _mm_loadu_si128((__m128i *)(src + 6 * i_src));
        __m128i T71 = _mm_loadu_si128((__m128i *)(src + 7 * i_src));

        __m128i T00 = _mm_unpacklo_epi8(T01, T11);
        __m128i T10 = _mm_unpacklo_epi8(T21, T31);
        __m128i T20 = _mm_unpacklo_epi8(T41, T51);
        __m128i T30 = _mm_unpacklo_epi8(T61, T71);

        __m128i T40 = _mm_unpackhi_epi8(T01, T11);
        __m128i T50 = _mm_unpackhi_epi8(T21, T31);
        __m128i T60 = _mm_unpackhi_epi8(T41, T51);
        __m128i T70 = _mm_unpackhi_epi8(T61, T71);

        T00 = _mm_maddubs_epi16(T00, coeff0);
        T10 = _mm_maddubs_epi16(T10, coeff1);
        T20 = _mm_maddubs_epi16(T20, coeff2);
        T30 = _mm_maddubs_epi16(T30, coeff3);

        T40 = _mm_maddubs_epi16(T40, coeff0);
        T50 = _mm_maddubs_epi16(T50, coeff1);
        T60 = _mm_maddubs_epi16(T60, coeff2);
        T70 = _mm_maddubs_epi16(T70, coeff3);

        mVal1 = _mm_add_epi16(T00, T10);
        mVal1 = _mm_add_epi16(mVal1, T20);
        mVal1 = _mm_add_epi16(mVal1, T30);

        mVal2 = _mm_add_epi16(T40, T50);
        mVal2 = _mm_add_epi16(mVal2, T60);
        mVal2 = _mm_add_epi16(mVal2, T70);

        mVal1 = _mm_add_epi16(mVal1, mAddOffset);
        mVal1 = _mm_srai_epi16(mVal1, shift);

        mVal2 = _mm_add_epi16(mVal2, mAddOffset);
        mVal2 = _mm_srai_epi16(mVal2, shift);
        mVal1 = _mm_packus_epi16(mVal1, mVal2);

        _mm_storeu_si128((__m128i *)dst, mVal1);

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_ver_luma_w16x_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coeff, int max_val)
{
    int col;
    const int offset = 32;
    const int shift = 6;
    pel const *p;
    __m128i mAddOffset = _mm_set1_epi16(offset);
    __m128i coeff0 = _mm_set1_epi16(*(s16 *)coeff);
    __m128i coeff1 = _mm_set1_epi16(*(s16 *)(coeff + 2));
    __m128i coeff2 = _mm_set1_epi16(*(s16 *)(coeff + 4));
    __m128i coeff3 = _mm_set1_epi16(*(s16 *)(coeff + 6));
    __m128i mVal1, mVal2;

    src -= 3 * i_src;

    while (height--) {
        p = src;
        uavs3e_prefetch(src + 8 * i_src, _MM_HINT_NTA);
        for (col = 0; col < width; col += 16) {
            __m128i T01 = _mm_loadu_si128((__m128i *)(p));
            __m128i T11 = _mm_loadu_si128((__m128i *)(p + i_src));
            __m128i T21 = _mm_loadu_si128((__m128i *)(p + 2 * i_src));
            __m128i T31 = _mm_loadu_si128((__m128i *)(p + 3 * i_src));
            __m128i T41 = _mm_loadu_si128((__m128i *)(p + 4 * i_src));
            __m128i T51 = _mm_loadu_si128((__m128i *)(p + 5 * i_src));
            __m128i T61 = _mm_loadu_si128((__m128i *)(p + 6 * i_src));
            __m128i T71 = _mm_loadu_si128((__m128i *)(p + 7 * i_src));

            __m128i T00 = _mm_unpacklo_epi8(T01, T11);
            __m128i T10 = _mm_unpacklo_epi8(T21, T31);
            __m128i T20 = _mm_unpacklo_epi8(T41, T51);
            __m128i T30 = _mm_unpacklo_epi8(T61, T71);

            __m128i T40 = _mm_unpackhi_epi8(T01, T11);
            __m128i T50 = _mm_unpackhi_epi8(T21, T31);
            __m128i T60 = _mm_unpackhi_epi8(T41, T51);
            __m128i T70 = _mm_unpackhi_epi8(T61, T71);

            T00 = _mm_maddubs_epi16(T00, coeff0);
            T10 = _mm_maddubs_epi16(T10, coeff1);
            T20 = _mm_maddubs_epi16(T20, coeff2);
            T30 = _mm_maddubs_epi16(T30, coeff3);

            T40 = _mm_maddubs_epi16(T40, coeff0);
            T50 = _mm_maddubs_epi16(T50, coeff1);
            T60 = _mm_maddubs_epi16(T60, coeff2);
            T70 = _mm_maddubs_epi16(T70, coeff3);

            mVal1 = _mm_add_epi16(T00, T10);
            mVal1 = _mm_add_epi16(mVal1, T20);
            mVal1 = _mm_add_epi16(mVal1, T30);

            mVal2 = _mm_add_epi16(T40, T50);
            mVal2 = _mm_add_epi16(mVal2, T60);
            mVal2 = _mm_add_epi16(mVal2, T70);

            mVal1 = _mm_add_epi16(mVal1, mAddOffset);
            mVal1 = _mm_srai_epi16(mVal1, shift);

            mVal2 = _mm_add_epi16(mVal2, mAddOffset);
            mVal2 = _mm_srai_epi16(mVal2, shift);
            mVal1 = _mm_packus_epi16(mVal1, mVal2);

            _mm_store_si128((__m128i *)&dst[col], mVal1);

            p += 16;
        }

        src += i_src;
        dst += i_dst;
    }
}

void uavs3e_if_hor_ver_chroma_w4_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    ALIGNED_16(s16 tmp_res[4 * (32 + 4)]);
    s16 *tmp = tmp_res;
    int rows = height + 4;

    // HOR
    __m128i mSwitch = _mm_setr_epi8(0, 1, 2, 3, 1, 2, 3, 4, 2, 3, 4, 5, 3, 4, 5, 6);
    __m128i mCoef = _mm_set1_epi32(*(int*)coef_x);

    src -= i_src + 1;
    rows >>= 2;

    while (rows--) {
        __m128i T1 = _mm_maddubs_epi16(_mm_shuffle_epi8(_mm_loadl_epi64((__m128i*)src), mSwitch), mCoef); src += i_src;
        __m128i T2 = _mm_maddubs_epi16(_mm_shuffle_epi8(_mm_loadl_epi64((__m128i*)src), mSwitch), mCoef); src += i_src;
        __m128i T3 = _mm_maddubs_epi16(_mm_shuffle_epi8(_mm_loadl_epi64((__m128i*)src), mSwitch), mCoef); src += i_src;
        __m128i T4 = _mm_maddubs_epi16(_mm_shuffle_epi8(_mm_loadl_epi64((__m128i*)src), mSwitch), mCoef); src += i_src;
        T1 = _mm_hadd_epi16(T1, T2);
        T3 = _mm_hadd_epi16(T3, T4);
        _mm_store_si128((__m128i*)(tmp), T1); tmp += 8;
        _mm_store_si128((__m128i*)(tmp), T3); tmp += 8;
    }

    // VER
    __m128i mVal1, mVal2, mVal;
    __m128i mAddOffset = _mm_set1_epi32(1 << 11);
    __m128i mCoef0 = _mm_set1_epi16(*(s16 *)coef_y);
    __m128i mCoef1 = _mm_set1_epi16(*(s16 *)(coef_y + 2));
    mCoef0 = _mm_cvtepi8_epi16(mCoef0);
    mCoef1 = _mm_cvtepi8_epi16(mCoef1);

    tmp = tmp_res;
    height >>= 1;

    while (height--) {
        __m128i T00 = _mm_load_si128((__m128i *)(tmp));
        __m128i T10 = _mm_loadu_si128((__m128i *)(tmp + 4));
        __m128i T20 = _mm_load_si128((__m128i *)(tmp + 8));
        __m128i T30 = _mm_loadu_si128((__m128i *)(tmp + 12));

        tmp += 8;
        
        __m128i M0 = _mm_unpacklo_epi16(T00, T10);
        __m128i M1 = _mm_unpacklo_epi16(T20, T30);
        __m128i M2 = _mm_unpackhi_epi16(T00, T10);
        __m128i M3 = _mm_unpackhi_epi16(T20, T30);

        M0 = _mm_madd_epi16(M0, mCoef0);
        M1 = _mm_madd_epi16(M1, mCoef1);
        M2 = _mm_madd_epi16(M2, mCoef0);
        M3 = _mm_madd_epi16(M3, mCoef1);

        mVal1 = _mm_add_epi32(M0, M1);
        mVal2 = _mm_add_epi32(M2, M3);

        mVal1 = _mm_add_epi32(mVal1, mAddOffset);
        mVal2 = _mm_add_epi32(mVal2, mAddOffset);
        mVal1 = _mm_srai_epi32(mVal1, 12);
        mVal2 = _mm_srai_epi32(mVal2, 12);
        mVal = _mm_packs_epi32(mVal1, mVal2);
        mVal = _mm_packus_epi16(mVal, mVal);

        *(int*)dst = _mm_extract_epi32(mVal, 0); dst += i_dst;
        *(int*)dst = _mm_extract_epi32(mVal, 1); dst += i_dst;
    }
}

void uavs3e_if_hor_ver_chroma_w8_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    ALIGNED_16(s16 tmp_res[8 * (64 + 3)]);
    s16 *tmp = tmp_res;
    int rows = height + 3;

    // HOR
    __m128i mSwitch1 = _mm_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m128i mSwitch2 = _mm_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m128i mCoef0 = _mm_set1_epi16(*(s16 *)coef_x);
    __m128i mCoef1 = _mm_set1_epi16(*(s16 *)(coef_x + 2));

    src = src - 1 * i_src - 1;

    while (rows--) {
        __m128i mSrc = _mm_loadu_si128((__m128i*)src);
        __m128i T0 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoef0);
        __m128i T1 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoef1);

        T0 = _mm_add_epi16(T0, T1);
        _mm_store_si128((__m128i*)tmp, T0);
        src += i_src;
        tmp += 8;
    }

    // VER
    __m128i mVal1, mVal2, mVal;
    __m128i mAddOffset = _mm_set1_epi32(1 << 11);
    mCoef0 = _mm_set1_epi16(*(s16 *)coef_y);
    mCoef1 = _mm_set1_epi16(*(s16 *)(coef_y + 2));
    mCoef0 = _mm_cvtepi8_epi16(mCoef0);
    mCoef1 = _mm_cvtepi8_epi16(mCoef1);

    tmp = tmp_res;
    height >>= 1;

    while (height--) {
        __m128i T00 = _mm_load_si128((__m128i *)(tmp));
        __m128i T10 = _mm_load_si128((__m128i *)(tmp + 8));
        __m128i T20 = _mm_load_si128((__m128i *)(tmp + 2 * 8));
        __m128i T30 = _mm_load_si128((__m128i *)(tmp + 3 * 8));
        __m128i T40 = _mm_load_si128((__m128i *)(tmp + 4 * 8));

        __m128i M0 = _mm_unpacklo_epi16(T00, T10);
        __m128i M1 = _mm_unpacklo_epi16(T20, T30);
        __m128i M2 = _mm_unpackhi_epi16(T00, T10);
        __m128i M3 = _mm_unpackhi_epi16(T20, T30);

        M0 = _mm_madd_epi16(M0, mCoef0);
        M1 = _mm_madd_epi16(M1, mCoef1);
        M2 = _mm_madd_epi16(M2, mCoef0);
        M3 = _mm_madd_epi16(M3, mCoef1);

        mVal1 = _mm_add_epi32(M0, M1);
        mVal2 = _mm_add_epi32(M2, M3);

        mVal1 = _mm_add_epi32(mVal1, mAddOffset);
        mVal2 = _mm_add_epi32(mVal2, mAddOffset);
        mVal1 = _mm_srai_epi32(mVal1, 12);
        mVal2 = _mm_srai_epi32(mVal2, 12);
        mVal = _mm_packs_epi32(mVal1, mVal2);
        mVal = _mm_packus_epi16(mVal, mVal);

        _mm_storel_epi64((__m128i *)dst, mVal);

        M0 = _mm_unpacklo_epi16(T10, T20);
        M1 = _mm_unpacklo_epi16(T30, T40);
        M2 = _mm_unpackhi_epi16(T10, T20);
        M3 = _mm_unpackhi_epi16(T30, T40);

        M0 = _mm_madd_epi16(M0, mCoef0);
        M1 = _mm_madd_epi16(M1, mCoef1);
        M2 = _mm_madd_epi16(M2, mCoef0);
        M3 = _mm_madd_epi16(M3, mCoef1);

        mVal1 = _mm_add_epi32(M0, M1);
        mVal2 = _mm_add_epi32(M2, M3);

        mVal1 = _mm_add_epi32(mVal1, mAddOffset);
        mVal2 = _mm_add_epi32(mVal2, mAddOffset);
        mVal1 = _mm_srai_epi32(mVal1, 12);
        mVal2 = _mm_srai_epi32(mVal2, 12);
        mVal = _mm_packs_epi32(mVal1, mVal2);
        mVal = _mm_packus_epi16(mVal, mVal);

        _mm_storel_epi64((__m128i *)&dst[i_dst], mVal);

        tmp += 2 * 8;
        dst += 2 * i_dst;
    }
}

void uavs3e_if_hor_ver_chroma_w8x_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    ALIGNED_16(s16 tmp_res[64 * (64 + 3)]);
    s16 *tmp = tmp_res;
    int i_tmp = width;
    int rows = height + 3;

    // HOR
    __m128i mSwitch1 = _mm_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m128i mSwitch2 = _mm_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m128i mCoef0 = _mm_set1_epi16(*(s16 *)coef_x);
    __m128i mCoef1 = _mm_set1_epi16(*(s16 *)(coef_x + 2));

    src = src - 1 * i_src - 1;

    while (rows--) {
        for (int i = 0; i < width; i += 8) {
            __m128i mSrc = _mm_loadu_si128((__m128i*)&src[i]);
            __m128i T0 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoef0);
            __m128i T1 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoef1);

            T0 = _mm_add_epi16(T0, T1);
            _mm_store_si128((__m128i*)&tmp[i], T0);
        }
        src += i_src;
        tmp += i_tmp;

    }

    // VER
    __m128i mVal1, mVal2, mVal;
    __m128i mAddOffset = _mm_set1_epi32(1 << 11);
    mCoef0 = _mm_set1_epi16(*(s16 *)coef_y);
    mCoef1 = _mm_set1_epi16(*(s16 *)(coef_y + 2));
    mCoef0 = _mm_cvtepi8_epi16(mCoef0);
    mCoef1 = _mm_cvtepi8_epi16(mCoef1);

    tmp = tmp_res;
    height >>= 1;

    while (height--) {
        for (int i = 0; i < width; i += 8) {
            __m128i T00 = _mm_load_si128((__m128i *)(tmp + i));
            __m128i T10 = _mm_load_si128((__m128i *)(tmp + i + 1 * i_tmp));
            __m128i T20 = _mm_load_si128((__m128i *)(tmp + i + 2 * i_tmp));
            __m128i T30 = _mm_load_si128((__m128i *)(tmp + i + 3 * i_tmp));
            __m128i T40 = _mm_load_si128((__m128i *)(tmp + i + 4 * i_tmp));

            __m128i M0 = _mm_unpacklo_epi16(T00, T10);
            __m128i M1 = _mm_unpacklo_epi16(T20, T30);
            __m128i M2 = _mm_unpackhi_epi16(T00, T10);
            __m128i M3 = _mm_unpackhi_epi16(T20, T30);

            M0 = _mm_madd_epi16(M0, mCoef0);
            M1 = _mm_madd_epi16(M1, mCoef1);
            M2 = _mm_madd_epi16(M2, mCoef0);
            M3 = _mm_madd_epi16(M3, mCoef1);

            mVal1 = _mm_add_epi32(M0, M1);
            mVal2 = _mm_add_epi32(M2, M3);

            mVal1 = _mm_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm_srai_epi32(mVal1, 12);
            mVal2 = _mm_srai_epi32(mVal2, 12);
            mVal = _mm_packs_epi32(mVal1, mVal2);
            mVal = _mm_packus_epi16(mVal, mVal);

            _mm_storel_epi64((__m128i *)&dst[i], mVal);

            M0 = _mm_unpacklo_epi16(T10, T20);
            M1 = _mm_unpacklo_epi16(T30, T40);
            M2 = _mm_unpackhi_epi16(T10, T20);
            M3 = _mm_unpackhi_epi16(T30, T40);

            M0 = _mm_madd_epi16(M0, mCoef0);
            M1 = _mm_madd_epi16(M1, mCoef1);
            M2 = _mm_madd_epi16(M2, mCoef0);
            M3 = _mm_madd_epi16(M3, mCoef1);

            mVal1 = _mm_add_epi32(M0, M1);
            mVal2 = _mm_add_epi32(M2, M3);

            mVal1 = _mm_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm_srai_epi32(mVal1, 12);
            mVal2 = _mm_srai_epi32(mVal2, 12);
            mVal = _mm_packs_epi32(mVal1, mVal2);
            mVal = _mm_packus_epi16(mVal, mVal);

            _mm_storel_epi64((__m128i *)&dst[i + i_dst], mVal);
        }
        tmp += 2 * i_tmp;
        dst += 2 * i_dst;
    }
}

void uavs3e_if_hor_ver_luma_w4_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    int row;
    int shift;

    ALIGNED_16(s16 tmp_res[(32 + 7) * 4]);
    s16 *tmp = tmp_res;
    const int i_tmp = 4;

    __m128i mAddOffset;
    __m128i mSwitch12 = _mm_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 2, 3, 3, 4, 4, 5, 5, 6);
    __m128i mSwitch34 = _mm_setr_epi8(4, 5, 5, 6, 6, 7, 7, 8, 6, 7, 7, 8, 8, 9, 9, 10);

    __m128i mCoefy1_hor = _mm_set1_epi16(*(s16 *)coef_x);
    __m128i mCoefy2_hor = _mm_set1_epi16(*(s16 *)(coef_x + 2));
    __m128i mCoefy3_hor = _mm_set1_epi16(*(s16 *)(coef_x + 4));
    __m128i mCoefy4_hor = _mm_set1_epi16(*(s16 *)(coef_x + 6));
    __m128i mCoefy1_ver = _mm_set1_epi16(*(s16 *)coef_y);
    __m128i mCoefy2_ver = _mm_set1_epi16(*(s16 *)(coef_y + 2));
    __m128i mCoefy3_ver = _mm_set1_epi16(*(s16 *)(coef_y + 4));
    __m128i mCoefy4_ver = _mm_set1_epi16(*(s16 *)(coef_y + 6));
    __m128i mVal1, mVal;

    // HOR
    src = src - 3 * i_src - 3;
    row = height + 6; // height + 7 - 1

    {
        // odd number of rows, process 1st row
        __m128i mT0, mT1, mT2, mT3, s1, s2, mVal;
        __m128i mSrc = _mm_loadu_si128((__m128i *)(src));

        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);

        mT0 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch12), mCoefy1_hor);
        mT1 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, _mm_srli_si128(mSwitch12, 8)), mCoefy2_hor);
        mT2 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch34), mCoefy3_hor);
        mT3 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, _mm_srli_si128(mSwitch34, 8)), mCoefy4_hor);

        s1 = _mm_add_epi16(mT0, mT1);
        s2 = _mm_add_epi16(mT2, mT3);
        mVal = _mm_add_epi16(s1, s2);

        _mm_storel_epi64((__m128i *)tmp, mVal);

        src += i_src;
        tmp += i_tmp;
    }
    while (row) {
        __m128i mT0, mT1, mT2, mT3, s1, s2, swSrc1, swSrc2;
        __m128i mSrc1 = _mm_loadu_si128((__m128i *)(src));
        __m128i mSrc2 = _mm_loadu_si128((__m128i *)(src + i_src));
        row -= 2;
        src += i_src << 1;
        uavs3e_prefetch(src, _MM_HINT_NTA);
        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);

        swSrc1 = _mm_shuffle_epi8(mSrc1, mSwitch12);
        swSrc2 = _mm_shuffle_epi8(mSrc2, mSwitch12);

        mT0 = _mm_maddubs_epi16(_mm_unpacklo_epi64(swSrc1, swSrc2), mCoefy1_hor);
        mT1 = _mm_maddubs_epi16(_mm_unpackhi_epi64(swSrc1, swSrc2), mCoefy2_hor);

        swSrc1 = _mm_shuffle_epi8(mSrc1, mSwitch34);
        swSrc2 = _mm_shuffle_epi8(mSrc2, mSwitch34);

        mT2 = _mm_maddubs_epi16(_mm_unpacklo_epi64(swSrc1, swSrc2), mCoefy3_hor);
        mT3 = _mm_maddubs_epi16(_mm_unpackhi_epi64(swSrc1, swSrc2), mCoefy4_hor);

        s1 = _mm_add_epi16(mT0, mT1);
        s2 = _mm_add_epi16(mT2, mT3);

        _mm_store_si128((__m128i *)tmp, _mm_add_epi16(s1, s2));

        tmp += i_tmp << 1;
    }

    // VER
    shift = 12;
    mAddOffset = _mm_set1_epi32(1 << (shift - 1));
    tmp = tmp_res;

    mCoefy1_ver = _mm_cvtepi8_epi16(mCoefy1_ver);
    mCoefy2_ver = _mm_cvtepi8_epi16(mCoefy2_ver);
    mCoefy3_ver = _mm_cvtepi8_epi16(mCoefy3_ver);
    mCoefy4_ver = _mm_cvtepi8_epi16(mCoefy4_ver);

    while (height) {
        __m128i T00  = _mm_loadu_si128((__m128i *)(tmp));
        __m128i T10  = _mm_srli_si128(T00, 8);
        __m128i T20  = _mm_loadu_si128((__m128i *)(tmp + 2 * i_tmp));
        __m128i T30  = _mm_srli_si128(T20, 8);
        __m128i T40  = _mm_loadu_si128((__m128i *)(tmp + 4 * i_tmp));
        __m128i T50  = _mm_srli_si128(T40, 8);
        __m128i T60  = _mm_loadu_si128((__m128i *)(tmp + 6 * i_tmp));
        __m128i T70  = _mm_srli_si128(T60, 8);
        __m128i T80  = _mm_loadu_si128((__m128i *)(tmp + 8 * i_tmp));
        __m128i T90  = _mm_srli_si128(T80, 8);
        __m128i T100 = _mm_loadu_si128((__m128i *)(tmp + 10 * i_tmp));

        __m128i T0 = _mm_unpacklo_epi16(T00, T10);
        __m128i T1 = _mm_unpacklo_epi16(T20, T30);
        __m128i T2 = _mm_unpacklo_epi16(T40, T50);
        __m128i T3 = _mm_unpacklo_epi16(T60, T70);

        height -= 4;

        T0 = _mm_madd_epi16(T0, mCoefy1_ver);
        T1 = _mm_madd_epi16(T1, mCoefy2_ver);
        T2 = _mm_madd_epi16(T2, mCoefy3_ver);
        T3 = _mm_madd_epi16(T3, mCoefy4_ver);

        mVal1 = _mm_add_epi32(T0, T1);
        mVal1 = _mm_add_epi32(mVal1, T2);
        mVal1 = _mm_add_epi32(mVal1, T3);

        mVal1 = _mm_add_epi32(mVal1, mAddOffset);
        mVal1 = _mm_srai_epi32(mVal1, shift);
        mVal = _mm_packs_epi32(mVal1, mVal1);
        mVal = _mm_packus_epi16(mVal, mVal);

        M32(dst) = _mm_extract_epi32(mVal, 0);

        T0 = _mm_unpacklo_epi16(T10, T20);
        T1 = _mm_unpacklo_epi16(T30, T40);
        T2 = _mm_unpacklo_epi16(T50, T60);
        T3 = _mm_unpacklo_epi16(T70, T80);

        T0 = _mm_madd_epi16(T0, mCoefy1_ver);
        T1 = _mm_madd_epi16(T1, mCoefy2_ver);
        T2 = _mm_madd_epi16(T2, mCoefy3_ver);
        T3 = _mm_madd_epi16(T3, mCoefy4_ver);

        mVal1 = _mm_add_epi32(T0, T1);
        mVal1 = _mm_add_epi32(mVal1, T2);
        mVal1 = _mm_add_epi32(mVal1, T3);

        mVal1 = _mm_add_epi32(mVal1, mAddOffset);
        mVal1 = _mm_srai_epi32(mVal1, shift);
        mVal = _mm_packs_epi32(mVal1, mVal1);
        mVal = _mm_packus_epi16(mVal, mVal);

        M32(dst + i_dst) = _mm_extract_epi32(mVal, 0);

        T0 = _mm_unpacklo_epi16(T20, T30);
        T1 = _mm_unpacklo_epi16(T40, T50);
        T2 = _mm_unpacklo_epi16(T60, T70);
        T3 = _mm_unpacklo_epi16(T80, T90);

        T0 = _mm_madd_epi16(T0, mCoefy1_ver);
        T1 = _mm_madd_epi16(T1, mCoefy2_ver);
        T2 = _mm_madd_epi16(T2, mCoefy3_ver);
        T3 = _mm_madd_epi16(T3, mCoefy4_ver);

        mVal1 = _mm_add_epi32(T0, T1);
        mVal1 = _mm_add_epi32(mVal1, T2);
        mVal1 = _mm_add_epi32(mVal1, T3);

        mVal1 = _mm_add_epi32(mVal1, mAddOffset);
        mVal1 = _mm_srai_epi32(mVal1, shift);
        mVal = _mm_packs_epi32(mVal1, mVal1);
        mVal = _mm_packus_epi16(mVal, mVal);

        M32(dst + 2 * i_dst) = _mm_extract_epi32(mVal, 0);

        T0 = _mm_unpacklo_epi16(T30, T40);
        T1 = _mm_unpacklo_epi16(T50, T60);
        T2 = _mm_unpacklo_epi16(T70, T80);
        T3 = _mm_unpacklo_epi16(T90, T100);

        T0 = _mm_madd_epi16(T0, mCoefy1_ver);
        T1 = _mm_madd_epi16(T1, mCoefy2_ver);
        T2 = _mm_madd_epi16(T2, mCoefy3_ver);
        T3 = _mm_madd_epi16(T3, mCoefy4_ver);

        mVal1 = _mm_add_epi32(T0, T1);
        mVal1 = _mm_add_epi32(mVal1, T2);
        mVal1 = _mm_add_epi32(mVal1, T3);

        mVal1 = _mm_add_epi32(mVal1, mAddOffset);
        mVal1 = _mm_srai_epi32(mVal1, shift);
        mVal = _mm_packs_epi32(mVal1, mVal1);
        mVal = _mm_packus_epi16(mVal, mVal);

        M32(dst + 3 * i_dst) = _mm_extract_epi32(mVal, 0);

        tmp += 4 * i_tmp;
        dst += 4 * i_dst;
    }
}

void uavs3e_if_hor_ver_luma_w8_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    int row;
    int shift;

    ALIGNED_16(s16 tmp_res[(64 + 7) * 8]);
    s16 *tmp = tmp_res;
    const int i_tmp = 8;

    __m128i mAddOffset;

    __m128i mSwitch1 = _mm_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m128i mSwitch2 = _mm_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m128i mSwitch3 = _mm_setr_epi8(4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12);
    __m128i mSwitch4 = _mm_setr_epi8(6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14);

    __m128i mCoefy1_hor = _mm_set1_epi16(*(s16 *)coef_x);
    __m128i mCoefy2_hor = _mm_set1_epi16(*(s16 *)(coef_x + 2));
    __m128i mCoefy3_hor = _mm_set1_epi16(*(s16 *)(coef_x + 4));
    __m128i mCoefy4_hor = _mm_set1_epi16(*(s16 *)(coef_x + 6));
    __m128i mCoefy1_ver = _mm_set1_epi16(*(s16 *)coef_y);
    __m128i mCoefy2_ver = _mm_set1_epi16(*(s16 *)(coef_y + 2));
    __m128i mCoefy3_ver = _mm_set1_epi16(*(s16 *)(coef_y + 4));
    __m128i mCoefy4_ver = _mm_set1_epi16(*(s16 *)(coef_y + 6));
    __m128i mVal1, mVal2, mVal;

    // HOR
    src = src - 3 * i_src - 3;
    row = height + 7;

    while (row--) {
        __m128i mT0, mT1, mT2, mT3, s1, s2, mVal;
        __m128i mSrc = _mm_loadu_si128((__m128i *)(src));

        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);

        mT0 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoefy1_hor);
        mT1 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoefy2_hor);
        mT2 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch3), mCoefy3_hor);
        mT3 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch4), mCoefy4_hor);

        s1 = _mm_add_epi16(mT0, mT1);
        s2 = _mm_add_epi16(mT2, mT3);
        mVal = _mm_add_epi16(s1, s2);

        _mm_store_si128((__m128i *)tmp, mVal);

        src += i_src;
        tmp += i_tmp;
    }

    // VER
    shift = 12;
    mAddOffset = _mm_set1_epi32(1 << (shift - 1));
    tmp = tmp_res;

    mCoefy1_ver = _mm_cvtepi8_epi16(mCoefy1_ver);
    mCoefy2_ver = _mm_cvtepi8_epi16(mCoefy2_ver);
    mCoefy3_ver = _mm_cvtepi8_epi16(mCoefy3_ver);
    mCoefy4_ver = _mm_cvtepi8_epi16(mCoefy4_ver);

    while (height) {
        __m128i T00 = _mm_load_si128((__m128i *)(tmp));
        __m128i T10 = _mm_load_si128((__m128i *)(tmp + i_tmp));
        __m128i T20 = _mm_load_si128((__m128i *)(tmp + 2 * i_tmp));
        __m128i T30 = _mm_load_si128((__m128i *)(tmp + 3 * i_tmp));
        __m128i T40 = _mm_load_si128((__m128i *)(tmp + 4 * i_tmp));
        __m128i T50 = _mm_load_si128((__m128i *)(tmp + 5 * i_tmp));
        __m128i T60 = _mm_load_si128((__m128i *)(tmp + 6 * i_tmp));
        __m128i T70 = _mm_load_si128((__m128i *)(tmp + 7 * i_tmp));
        __m128i T80 = _mm_load_si128((__m128i *)(tmp + 8 * i_tmp));
        __m128i T90 = _mm_load_si128((__m128i *)(tmp + 9 * i_tmp));
        __m128i T100 = _mm_load_si128((__m128i *)(tmp + 10 * i_tmp));

        __m128i T0 = _mm_unpacklo_epi16(T00, T10);
        __m128i T1 = _mm_unpacklo_epi16(T20, T30);
        __m128i T2 = _mm_unpacklo_epi16(T40, T50);
        __m128i T3 = _mm_unpacklo_epi16(T60, T70);
        __m128i T4 = _mm_unpackhi_epi16(T00, T10);
        __m128i T5 = _mm_unpackhi_epi16(T20, T30);
        __m128i T6 = _mm_unpackhi_epi16(T40, T50);
        __m128i T7 = _mm_unpackhi_epi16(T60, T70);

        T0 = _mm_madd_epi16(T0, mCoefy1_ver);
        T1 = _mm_madd_epi16(T1, mCoefy2_ver);
        T2 = _mm_madd_epi16(T2, mCoefy3_ver);
        T3 = _mm_madd_epi16(T3, mCoefy4_ver);
        T4 = _mm_madd_epi16(T4, mCoefy1_ver);
        T5 = _mm_madd_epi16(T5, mCoefy2_ver);
        T6 = _mm_madd_epi16(T6, mCoefy3_ver);
        T7 = _mm_madd_epi16(T7, mCoefy4_ver);

        mVal1 = _mm_add_epi32(T0, T1);
        mVal1 = _mm_add_epi32(mVal1, T2);
        mVal1 = _mm_add_epi32(mVal1, T3);

        mVal2 = _mm_add_epi32(T4, T5);
        mVal2 = _mm_add_epi32(mVal2, T6);
        mVal2 = _mm_add_epi32(mVal2, T7);

        mVal1 = _mm_add_epi32(mVal1, mAddOffset);
        mVal2 = _mm_add_epi32(mVal2, mAddOffset);
        mVal1 = _mm_srai_epi32(mVal1, shift);
        mVal2 = _mm_srai_epi32(mVal2, shift);
        mVal = _mm_packs_epi32(mVal1, mVal2);
        mVal = _mm_packus_epi16(mVal, mVal);

        _mm_storel_epi64((__m128i *)dst, mVal);

        T0 = _mm_unpacklo_epi16(T10, T20);
        T1 = _mm_unpacklo_epi16(T30, T40);
        T2 = _mm_unpacklo_epi16(T50, T60);
        T3 = _mm_unpacklo_epi16(T70, T80);
        T4 = _mm_unpackhi_epi16(T10, T20);
        T5 = _mm_unpackhi_epi16(T30, T40);
        T6 = _mm_unpackhi_epi16(T50, T60);
        T7 = _mm_unpackhi_epi16(T70, T80);

        T0 = _mm_madd_epi16(T0, mCoefy1_ver);
        T1 = _mm_madd_epi16(T1, mCoefy2_ver);
        T2 = _mm_madd_epi16(T2, mCoefy3_ver);
        T3 = _mm_madd_epi16(T3, mCoefy4_ver);
        T4 = _mm_madd_epi16(T4, mCoefy1_ver);
        T5 = _mm_madd_epi16(T5, mCoefy2_ver);
        T6 = _mm_madd_epi16(T6, mCoefy3_ver);
        T7 = _mm_madd_epi16(T7, mCoefy4_ver);

        mVal1 = _mm_add_epi32(T0, T1);
        mVal1 = _mm_add_epi32(mVal1, T2);
        mVal1 = _mm_add_epi32(mVal1, T3);

        mVal2 = _mm_add_epi32(T4, T5);
        mVal2 = _mm_add_epi32(mVal2, T6);
        mVal2 = _mm_add_epi32(mVal2, T7);

        mVal1 = _mm_add_epi32(mVal1, mAddOffset);
        mVal2 = _mm_add_epi32(mVal2, mAddOffset);
        mVal1 = _mm_srai_epi32(mVal1, shift);
        mVal2 = _mm_srai_epi32(mVal2, shift);
        mVal = _mm_packs_epi32(mVal1, mVal2);
        mVal = _mm_packus_epi16(mVal, mVal);

        _mm_storel_epi64((__m128i *)&dst[i_dst], mVal);

        T0 = _mm_unpacklo_epi16(T20, T30);
        T1 = _mm_unpacklo_epi16(T40, T50);
        T2 = _mm_unpacklo_epi16(T60, T70);
        T3 = _mm_unpacklo_epi16(T80, T90);
        T4 = _mm_unpackhi_epi16(T20, T30);
        T5 = _mm_unpackhi_epi16(T40, T50);
        T6 = _mm_unpackhi_epi16(T60, T70);
        T7 = _mm_unpackhi_epi16(T80, T90);

        T0 = _mm_madd_epi16(T0, mCoefy1_ver);
        T1 = _mm_madd_epi16(T1, mCoefy2_ver);
        T2 = _mm_madd_epi16(T2, mCoefy3_ver);
        T3 = _mm_madd_epi16(T3, mCoefy4_ver);
        T4 = _mm_madd_epi16(T4, mCoefy1_ver);
        T5 = _mm_madd_epi16(T5, mCoefy2_ver);
        T6 = _mm_madd_epi16(T6, mCoefy3_ver);
        T7 = _mm_madd_epi16(T7, mCoefy4_ver);

        mVal1 = _mm_add_epi32(T0, T1);
        mVal1 = _mm_add_epi32(mVal1, T2);
        mVal1 = _mm_add_epi32(mVal1, T3);

        mVal2 = _mm_add_epi32(T4, T5);
        mVal2 = _mm_add_epi32(mVal2, T6);
        mVal2 = _mm_add_epi32(mVal2, T7);

        mVal1 = _mm_add_epi32(mVal1, mAddOffset);
        mVal2 = _mm_add_epi32(mVal2, mAddOffset);
        mVal1 = _mm_srai_epi32(mVal1, shift);
        mVal2 = _mm_srai_epi32(mVal2, shift);
        mVal = _mm_packs_epi32(mVal1, mVal2);
        mVal = _mm_packus_epi16(mVal, mVal);

        _mm_storel_epi64((__m128i *)&dst[2 * i_dst], mVal);

        T0 = _mm_unpacklo_epi16(T30, T40);
        T1 = _mm_unpacklo_epi16(T50, T60);
        T2 = _mm_unpacklo_epi16(T70, T80);
        T3 = _mm_unpacklo_epi16(T90, T100);
        T4 = _mm_unpackhi_epi16(T30, T40);
        T5 = _mm_unpackhi_epi16(T50, T60);
        T6 = _mm_unpackhi_epi16(T70, T80);
        T7 = _mm_unpackhi_epi16(T90, T100);

        T0 = _mm_madd_epi16(T0, mCoefy1_ver);
        T1 = _mm_madd_epi16(T1, mCoefy2_ver);
        T2 = _mm_madd_epi16(T2, mCoefy3_ver);
        T3 = _mm_madd_epi16(T3, mCoefy4_ver);
        T4 = _mm_madd_epi16(T4, mCoefy1_ver);
        T5 = _mm_madd_epi16(T5, mCoefy2_ver);
        T6 = _mm_madd_epi16(T6, mCoefy3_ver);
        T7 = _mm_madd_epi16(T7, mCoefy4_ver);

        mVal1 = _mm_add_epi32(T0, T1);
        mVal1 = _mm_add_epi32(mVal1, T2);
        mVal1 = _mm_add_epi32(mVal1, T3);

        mVal2 = _mm_add_epi32(T4, T5);
        mVal2 = _mm_add_epi32(mVal2, T6);
        mVal2 = _mm_add_epi32(mVal2, T7);

        mVal1 = _mm_add_epi32(mVal1, mAddOffset);
        mVal2 = _mm_add_epi32(mVal2, mAddOffset);
        mVal1 = _mm_srai_epi32(mVal1, shift);
        mVal2 = _mm_srai_epi32(mVal2, shift);
        mVal = _mm_packs_epi32(mVal1, mVal2);
        mVal = _mm_packus_epi16(mVal, mVal);

        _mm_storel_epi64((__m128i *)&dst[3 * i_dst], mVal);

        tmp += 4 * i_tmp;
        dst += 4 * i_dst;
        height -= 4;
    }
}

void uavs3e_if_hor_ver_luma_w8x_sse(const pel *src, int i_src, pel *dst, int i_dst, int width, int height, const s8 *coef_x, const s8 *coef_y, int max_val)
{
    int row, col;
    int shift;
    s16 const *p;

    ALIGNED_16(s16 tmp_res[(128 + 7) * 128]);
    s16 *tmp = tmp_res;
    const int i_tmp = width;

    __m128i mAddOffset;

    __m128i mSwitch1 = _mm_setr_epi8(0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8);
    __m128i mSwitch2 = _mm_setr_epi8(2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10);
    __m128i mSwitch3 = _mm_setr_epi8(4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12);
    __m128i mSwitch4 = _mm_setr_epi8(6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14);

    __m128i mCoefy1_hor = _mm_set1_epi16(*(s16 *)coef_x);
    __m128i mCoefy2_hor = _mm_set1_epi16(*(s16 *)(coef_x + 2));
    __m128i mCoefy3_hor = _mm_set1_epi16(*(s16 *)(coef_x + 4));
    __m128i mCoefy4_hor = _mm_set1_epi16(*(s16 *)(coef_x + 6));
    __m128i mCoefy1_ver = _mm_set1_epi16(*(s16 *)coef_y);
    __m128i mCoefy2_ver = _mm_set1_epi16(*(s16 *)(coef_y + 2));
    __m128i mCoefy3_ver = _mm_set1_epi16(*(s16 *)(coef_y + 4));
    __m128i mCoefy4_ver = _mm_set1_epi16(*(s16 *)(coef_y + 6));
    __m128i mVal1, mVal2, mVal;

    // HOR
    src = src - 3 * i_src - 3;
    row = height + 7;

    while (row--) {
        __m128i mT0, mT1, mT2, mT3, s1, s2, mVal;

        uavs3e_prefetch(src + i_src, _MM_HINT_NTA);
        for (col = 0; col < width; col += 8) {

            __m128i mSrc = _mm_loadu_si128((__m128i *)(src + col));

            mT0 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch1), mCoefy1_hor);
            mT1 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch2), mCoefy2_hor);
            mT2 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch3), mCoefy3_hor);
            mT3 = _mm_maddubs_epi16(_mm_shuffle_epi8(mSrc, mSwitch4), mCoefy4_hor);

            s1 = _mm_add_epi16(mT0, mT1);
            s2 = _mm_add_epi16(mT2, mT3);
            mVal = _mm_add_epi16(s1, s2);

            _mm_store_si128((__m128i *)&tmp[col], mVal);
        }

        src += i_src;
        tmp += i_tmp;
    }

    // VER
    shift = 12;
    mAddOffset = _mm_set1_epi32(1 << (shift - 1));
    tmp = tmp_res;

    mCoefy1_ver = _mm_cvtepi8_epi16(mCoefy1_ver);
    mCoefy2_ver = _mm_cvtepi8_epi16(mCoefy2_ver);
    mCoefy3_ver = _mm_cvtepi8_epi16(mCoefy3_ver);
    mCoefy4_ver = _mm_cvtepi8_epi16(mCoefy4_ver);

    while (height--) {
        p = tmp;
        for (col = 0; col < width; col += 8) {
            __m128i T00 = _mm_load_si128((__m128i *)(p));
            __m128i T10 = _mm_load_si128((__m128i *)(p + i_tmp));
            __m128i T20 = _mm_load_si128((__m128i *)(p + 2 * i_tmp));
            __m128i T30 = _mm_load_si128((__m128i *)(p + 3 * i_tmp));
            __m128i T40 = _mm_load_si128((__m128i *)(p + 4 * i_tmp));
            __m128i T50 = _mm_load_si128((__m128i *)(p + 5 * i_tmp));
            __m128i T60 = _mm_load_si128((__m128i *)(p + 6 * i_tmp));
            __m128i T70 = _mm_load_si128((__m128i *)(p + 7 * i_tmp));

            __m128i T0 = _mm_unpacklo_epi16(T00, T10);
            __m128i T1 = _mm_unpacklo_epi16(T20, T30);
            __m128i T2 = _mm_unpacklo_epi16(T40, T50);
            __m128i T3 = _mm_unpacklo_epi16(T60, T70);
            __m128i T4 = _mm_unpackhi_epi16(T00, T10);
            __m128i T5 = _mm_unpackhi_epi16(T20, T30);
            __m128i T6 = _mm_unpackhi_epi16(T40, T50);
            __m128i T7 = _mm_unpackhi_epi16(T60, T70);

            T0 = _mm_madd_epi16(T0, mCoefy1_ver);
            T1 = _mm_madd_epi16(T1, mCoefy2_ver);
            T2 = _mm_madd_epi16(T2, mCoefy3_ver);
            T3 = _mm_madd_epi16(T3, mCoefy4_ver);
            T4 = _mm_madd_epi16(T4, mCoefy1_ver);
            T5 = _mm_madd_epi16(T5, mCoefy2_ver);
            T6 = _mm_madd_epi16(T6, mCoefy3_ver);
            T7 = _mm_madd_epi16(T7, mCoefy4_ver);

            mVal1 = _mm_add_epi32(T0, T1);
            mVal1 = _mm_add_epi32(mVal1, T2);
            mVal1 = _mm_add_epi32(mVal1, T3);

            mVal2 = _mm_add_epi32(T4, T5);
            mVal2 = _mm_add_epi32(mVal2, T6);
            mVal2 = _mm_add_epi32(mVal2, T7);

            mVal1 = _mm_add_epi32(mVal1, mAddOffset);
            mVal2 = _mm_add_epi32(mVal2, mAddOffset);
            mVal1 = _mm_srai_epi32(mVal1, shift);
            mVal2 = _mm_srai_epi32(mVal2, shift);
            mVal = _mm_packs_epi32(mVal1, mVal2);
            mVal = _mm_packus_epi16(mVal, mVal);

            _mm_storel_epi64((__m128i *)&dst[col], mVal);

            p += 8;
        }
        tmp += i_tmp;
        dst += i_dst;
    }
}


